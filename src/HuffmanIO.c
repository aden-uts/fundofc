#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HuffmanCoding.h"
#include "HuffmanUtils.h"
#include "Globals.h"

#define CACHE_SIZE 2
#define SIGNATURE_LEN 4

struct huffman_code_compressed_t {
	int code[2];
	char item;
	short len;
};

struct huffman_header_t {
    unsigned char signature[4];
    int symbol_size;
    short n_symbols;
	short key_offset;
    short table_offset;
	short data_offset;
	long int data_end;
};

struct huffman_key_t {
	short len;
    unsigned char item;
};

void print_bits(int num);
void set_bit( int A[],  int k );
int test_bit(int A[], int k);
int compress_huffman_code(struct huffman_code_t huffman_code);
void print_n_bits(int num[], int n, int offset, int marker);
void clear_int_array(int a[], int len);
void decompress_file(FILE* fp, char output_file_name[], int verbose);
void print_compressed_data(FILE* fp, int data_offset, int data_end);
int compare_bits_at_pos(int a[], int b[], int i, int j);
int load_header(struct huffman_header_t* header_p, FILE* fp, int verbose);
void load_keys(struct huffman_key_t keys[], FILE* fp, int offset, int n_symbols, int verbose);
void get_key_offsets(int offsets[], struct huffman_key_t keys[], int n_elements, int verbose);
int bits_to_bytes(int n_bits);
int compare_n_bits(int a[], int b[], int len, int offset_a, int offset_b);
int load_to_buffer(int buffer[], FILE* fp, int size_bytes, int count, long int offset_bytes );

struct huffman_header_t make_header(int symbol_size, int n_symbols, int table_size, int data_size) {
    struct huffman_header_t header;
    unsigned char signature[SIGNATURE_LEN] = {'H', 'U', 'F', 'F'};
    memcpy(header.signature, signature, SIGNATURE_LEN);
    header.symbol_size = symbol_size;
    header.n_symbols = n_symbols;
	header.key_offset = sizeof(header);
    header.table_offset = sizeof(header) + n_symbols*sizeof(struct huffman_key_t);
	header.data_offset = header.table_offset + table_size;
	header.data_end = data_size + header.data_offset;
    return header;
}

void print_header(struct huffman_header_t header) {
    printf("Signature:\t%s\n", header.signature);
    printf("Symbol Size: \t%d bytes\n", header.symbol_size);
    printf("N Symbols: \t%d\n", header.n_symbols);
	printf("Key offset: \t%d\n", header.key_offset);
    printf("Table Offset: \t%d\n", header.table_offset);
	printf("Data offset: \t%d\n", header.data_offset);
	printf("Data end: \t%ld\n", header.data_end);
}

void compress_input_file(FILE *fp, char output_file_name[], int verbose) {
	
	/* init vars
		- i for many for loops
		- c for reading chars from file
		- element count for number of items needing encoding
	 */

	if (verbose >= __V1) { printf("Beginning compression\n"); }

	int i;
	char chars[MAX_SYMBOLS];
	int char_counts[MAX_SYMBOLS];
	int element_count = 0;


	/* Reading chars from file */

	/* Loop over all chars in file to generate char/freq table
		as well as element counts */
	if (fseek(fp, 0, SEEK_SET) != 0) {
		perror("fseek");
		exit(1);
	}

	if (verbose >= __V3) { printf("Counting chars in input ... "); }
	char c[1]; 
	while (fread(c, sizeof(c), 1, fp) == 1) {
		/* Bool flag to check if already found */
		int found = 0;
		/* Cast c to char for safety :) */
		char c_i = c[0];


		/* Loop through to see if char has already been seen
			and increment the count of char occurences if yes */
		for (i = 0; i <= element_count; i++) {
			if (c_i == chars[i]) {
				char_counts[i] += 1;
				found = 1;
				break;
			}
		}
		/* If char hasn't been found add it to the list */
		if (found == 0) {
			chars[element_count] = c_i;
			char_counts[element_count] = 1;
			element_count++;
		}
	}
	if (verbose >= __V3) { printf("Success\n"); }
	if (verbose >= __V4) {
		printf("Distinct characters in input: %d\n", element_count);
		printf("|-----------------------------|\n");
		printf("| Char           | Frequency  |\n");
    	printf("|----------------|------------|\n");
    	for (i = 0; i < element_count; i++) {
			printf("| 0x%-12x | %-10d |\n", chars[i], char_counts[i]);
		}
		printf("|-----------------------------|\n");
	}

	/* Genereate Huffman codes */
	/* Codes are stored as char array initially, but compressed later on */
	struct huffman_code_t huffman_codes[element_count];
	get_huffman_codes(huffman_codes, chars, char_counts, element_count);
	
	/* Calculate the required number of bits to compress the file */
	long unsigned int required_data_bits = 0;
	for (i = 0; i < element_count; i++) {
		required_data_bits += huffman_codes[i].len * char_counts[i];
	} 


	/* Codes need to be sorted by length for compression in code table */
	sort_codes(huffman_codes, element_count);



	/* Compress char array codes from huffman_codes to bit arrays */
	struct huffman_code_compressed_t huffman_codes_compressed[element_count];

	for (i = 0; i < element_count; i++) {
		huffman_codes_compressed[i].item = huffman_codes[i].item;
		huffman_codes_compressed[i].len = huffman_codes[i].len;
		huffman_codes_compressed[i].code[0] = compress_huffman_code(huffman_codes[i]);
	}

	/* Calculate total length of codes */

	unsigned int code_length_total = 0;
	for (i = 0; i < element_count; i++) {
		code_length_total += huffman_codes[i].len;
	}

	int required_bits = (code_length_total / (sizeof(int) * 8)) + 1;	
	
	/* Fit code bit arrays into one larger bit array (code table) */

	int code_table_bit_array[required_bits];

	for (i = 0; i < required_bits; i++) {
		code_table_bit_array[i] = 0;
	}

	int upto = 0;		
	for (i = 0; i < element_count; i++) {
		int j;
		for (j = 0; j < huffman_codes[i].len; j++) {
			if (huffman_codes[i].code[j] == 1) {
				set_bit(code_table_bit_array, j + upto);
			}
		}
		upto += huffman_codes[i].len;
	}

	struct huffman_key_t huffman_keys[element_count];
	for (i = 0; i < element_count; i++) {
		huffman_keys[i].item = huffman_codes[i].item;
		huffman_keys[i].len = huffman_codes[i].len;
	}

	/* ######### OUTPUTTING FULL FILE ############ */
	int table_size = sizeof(code_table_bit_array[0]) * required_bits;
	struct huffman_header_t output_header = make_header(1, element_count, table_size, required_data_bits);
	fseek(fp, 0L, SEEK_END);

	// FILE * output_compress_file = fopen("outputfile", "wb");

	FILE * output_fp = fopen(output_file_name, "wb");
	if (!output_fp) {
		perror("fopen");
		exit(1);
	}
	if (fwrite(&output_header, sizeof(struct huffman_header_t), 1, output_fp) != 1) {
		perror("fwrite");
		exit(1);
	}
	if (fwrite(huffman_keys, sizeof(struct huffman_key_t), element_count, output_fp) != element_count) {
		perror("fwrite");
		exit(1);
	}
	if (fwrite(code_table_bit_array, sizeof(code_table_bit_array[0]) , required_bits, output_fp) != required_bits) {
		perror("fwrite");
		exit(1);
	}
	if (fseek(fp, 0, SEEK_SET) != 0) {
		perror("fseek");
		exit(1);
	}
	int cache[CACHE_SIZE];
	clear_int_array(cache, CACHE_SIZE);
	int cache_i = 0;
	long unsigned int count = 0;
	unsigned char ch[1];
	while (fread(ch, sizeof(ch), 1, fp) == 1) {
		count ++;
		
		unsigned char c_i = (unsigned char) ch[0];
		int i;
		for (i = 0; i < element_count; i++) {

			if (c_i == huffman_codes[i].item) {
				int j;
				for (j = 0; j < huffman_codes[i].len; j++) {
					if (cache_i == CACHE_SIZE * 32) {
						if (output_fp != NULL) {
							fwrite(cache, sizeof(int), CACHE_SIZE, output_fp);
						}
						clear_int_array(cache, CACHE_SIZE);

						cache_i = 0;
					}
					if (huffman_codes[i].code[j] == 1) {
						set_bit(cache, cache_i);
					}
					cache_i++;
				}
			}
		}
	}

	if (output_fp != NULL) {
		int success = fwrite(cache, sizeof(int) , CACHE_SIZE, output_fp);
		printf("Success: %d\n", success);
	}
	fclose(output_fp);

}

void decompress_file(FILE* fp, char output_file_name[], int verbose) {

	if (verbose >= __V2) { printf("Decompressing file...\n"); }

	struct huffman_header_t* header = (struct huffman_header_t *) malloc(sizeof(struct huffman_header_t));
	load_header(header, fp, verbose);

	FILE* output_fp;
	printf("%s", output_file_name);
	output_fp = fopen(output_file_name, "wb");
	if (!output_fp) {
		perror("fopen");
		exit(1);
	}
	fseek(output_fp, 0, SEEK_SET);
	
	int compressed_bit_count = header->data_end - header->data_offset;
	if (verbose >= __V2) { printf("%d bytes to decompress (%d bits)\n", bits_to_bytes(compressed_bit_count), compressed_bit_count); }

	int n_symbols, min_code_len, max_code_len = 0;
	n_symbols = header->n_symbols;
	if (verbose >= __V3) { printf("%d symbols found...\n", n_symbols); }

	struct huffman_key_t keys[n_symbols];
	load_keys(keys, fp, header->key_offset, n_symbols, verbose);
	
	int key_offsets[n_symbols];
	get_key_offsets(key_offsets, keys, n_symbols, verbose);
	
	min_code_len = keys[0].len;
	max_code_len = keys[n_symbols - 1].len;

	if (verbose >= __V3) { printf("Min code length: %d\nMax code length: %d\n", min_code_len, max_code_len); }
	if (verbose >= __V2) { printf("Starting data decompression...\n"); }

	printf("Compressed data:\n");
	print_compressed_data(fp, header->data_offset, header->data_end);

	int max_buffer_size = max_code_len / sizeof(int) / 8 + 2;
	int max_buffer_size_bits = max_buffer_size * sizeof(int) * 8;
	if (bits_to_bytes(compressed_bit_count)  < max_buffer_size) { max_buffer_size = bits_to_bytes(compressed_bit_count); }

	if (verbose >= __V3) { printf("Setting buffer size to %d bits\n", max_buffer_size_bits); }
	
	/* Setup input buffer */
	int input_buffer[max_buffer_size];
	clear_int_array(input_buffer, max_buffer_size);

	int current_input_buffer_size_bits = max_buffer_size_bits;
	if (compressed_bit_count < max_buffer_size_bits) {
		current_input_buffer_size_bits = compressed_bit_count;
	}

	if (verbose >= __V3) {
		printf("Loading to buffer...\n");
	}

	int input_byte_upto = 0;
	load_to_buffer(input_buffer, fp, sizeof(input_buffer[0]), max_buffer_size, header->data_offset);
	input_byte_upto += header->data_offset + (max_buffer_size * sizeof(input_buffer[0]));
	printf("header data offset: %d\n", header->data_offset);
	printf("input byte upto: %d\n", input_byte_upto);
	print_n_bits(input_buffer, current_input_buffer_size_bits, 0, 0);

	int unbuffered_bits = compressed_bit_count - current_input_buffer_size_bits;
	printf("Unb: %d\n", unbuffered_bits);
	int input_buffer_offset_bits = 0;

	while (current_input_buffer_size_bits > 0) {
		
		int input_buffer_window_len = min_code_len;

		/* Setup key buffer */
		long int key_byte_upto = 0;
		int key_buffer[max_buffer_size];
		clear_int_array(key_buffer, max_buffer_size);
		int key_buffer_offset_bits = 0;
		load_to_buffer(key_buffer, fp, sizeof(key_buffer[0]), max_buffer_size, header->table_offset);
		key_byte_upto = header->table_offset + sizeof(key_buffer[0]) * max_buffer_size;
		int current_key_buffer_size_bits = sizeof(key_buffer[0]) * max_buffer_size * 8;

		int search_key_i;
		int searched_key_bits = 0;
		int decompressed_input_bits = 0;
		
		for(search_key_i = 0; search_key_i < n_symbols; search_key_i++) {
			input_buffer_window_len = keys[search_key_i].len;

			int i;
			for (i = 0; i < searched_key_bits; i++) {

				key_buffer_offset_bits++;
				current_key_buffer_size_bits--;

				if (current_key_buffer_size_bits == max_buffer_size_bits - sizeof(key_buffer[0]) * 8) {
					// printf("Adding\n");
					int key_buffer_chunk_i;
					for (key_buffer_chunk_i = 0; key_buffer_chunk_i < max_buffer_size - 1; key_buffer_chunk_i++) {
						key_buffer[key_buffer_chunk_i] = key_buffer[key_buffer_chunk_i + 1];
					}
					load_to_buffer(key_buffer + max_buffer_size - 1, fp, sizeof(key_buffer[0]), 1, key_byte_upto);
					// printf("Added: ");
					// print_n_bits(key_buffer, sizeof(key_buffer[0]) * 8, sizeof(key_buffer[0] * 8), 0);
					key_byte_upto += sizeof(key_buffer[0]);
					current_key_buffer_size_bits += sizeof(key_buffer[0]) * 8;
					// print_n_bits(key_buffer, current_key_buffer_size_bits, key_buffer_offset_bits, 0);
					key_buffer_offset_bits -= sizeof(key_buffer[0]) * 8;
					// print_n_bits(key_buffer, current_key_buffer_size_bits, key_buffer_offset_bits, 0);
					// printf("\n");
				}
			}

			// printf("Searched %d key bits\n", searched_key_bits);
			// printf("kb:  ");
			// print_n_bits(key_buffer, current_key_buffer_size_bits, key_buffer_offset_bits,0 );
			// printf("buf: ");
			// print_n_bits(input_buffer, current_input_buffer_size_bits, input_buffer_offset_bits, 0);

			// print_n_bits(input_buffer, input_buffer_window_len, input_buffer_offset_bits, 0);
			// print_n_bits(key_buffer, input_buffer_window_len, key_buffer_offset_bits, 0);
			if (compare_n_bits(input_buffer, key_buffer, input_buffer_window_len, input_buffer_offset_bits, key_buffer_offset_bits) == 1) {
				decompressed_input_bits = input_buffer_window_len;
				putc(keys[search_key_i].item, output_fp);
				break;
			}
			// printf("\n");
			searched_key_bits = input_buffer_window_len;
		}

		int j;
		for (j = 0; j < decompressed_input_bits; j++) {
			input_buffer_offset_bits++;
			current_input_buffer_size_bits--;

			if (current_input_buffer_size_bits == max_buffer_size_bits - (sizeof(input_buffer[0]) * 8)
				&& unbuffered_bits > 0) {
				// printf("Buffer: ");
				// print_n_bits(input_buffer, current_input_buffer_size_bits, input_buffer_offset_bits, 0);
				int input_buffer_chunk_i;
				for (input_buffer_chunk_i = 0; input_buffer_chunk_i < max_buffer_size - 1; input_buffer_chunk_i++) {
					input_buffer[input_buffer_chunk_i] = input_buffer[input_buffer_chunk_i + 1];
				}

				int load_success;
				load_success = load_to_buffer(input_buffer + max_buffer_size - 1, fp, sizeof(input_buffer[0]), 1, input_byte_upto);
				// printf("Load count: %d\n", load_success);
				if (load_success != 1) {
					printf("Failed to load file to buffer, exiting\n");
					if (verbose >= __V2) {
						printf("fread returned %d\n", load_success);
					}
					if (verbose >= __V3) {
						printf("Buffer: ");
						print_n_bits(input_buffer, sizeof(input_buffer[0]) * max_buffer_size * 8, 0 ,0);
						printf("Next byte: %d\n", input_byte_upto);
						printf("Bits remaining: %d\n", unbuffered_bits);
					}
					exit(1);
				}
				printf("Added: ");
				print_n_bits(input_buffer, 32, 32, 0);
				input_buffer_offset_bits -= sizeof(input_buffer[0]) * 8;
				if (unbuffered_bits < (sizeof(input_buffer[0]) * 8)) {
					current_input_buffer_size_bits += unbuffered_bits;
					unbuffered_bits = 0;
				} else {
					current_input_buffer_size_bits += sizeof(input_buffer[0]) * 8;
					unbuffered_bits -= sizeof(input_buffer[0]) * 8;
				}

				input_byte_upto += sizeof(input_buffer[0]);
				// print_n_bits(input_buffer, current_input_buffer_size_bits, input_buffer_offset_bits, 0);
				// printf("Remaining bits: %d\n", unbuffered_bits);
				// printf("input_byte_upto: %d\n", input_byte_upto);

			}
		}
	}
}

void get_key_offsets(int offsets[], struct huffman_key_t keys[], int n_elements, int verbose) {
	
	if (verbose >= __V2) { printf("Calcuating offsets...\n"); }

	int i;
	int offset = 0;
	for (i = 0; i < n_elements; i++) {
		offsets[i] = offset;
		offset += keys[i].len;
	}

	if (verbose >= __V2) { printf("Success\n"); }
	if (verbose >= __V3) { 
		int i;
		for (i = 0; i < n_elements; i++) {
			printf("%X\t%d\n", keys[i].item, offsets[i]);
		}
	}
}

int bits_to_bytes(int n_bits) { return n_bits / 8 + 1; }

int load_to_buffer(int buffer[], FILE* fp, int size_bytes, int count, long int offset_bytes ) {
	fseek(fp, offset_bytes, SEEK_SET);
	int read_count = fread(buffer, size_bytes, count, fp);
	if (read_count == 0) {
		perror("Error: ");
	}
	return read_count;
}

void print_keys(struct huffman_key_t keys[], int n) {
	printf("Item\tCode Length\n");
	int i;
	for (i = 0; i < n; i++) {
		printf("%X\t%d\n", keys[i].item, keys[i].len);
	}
}

void load_keys(struct huffman_key_t keys[], FILE* fp, int offset, int n_symbols, int verbose) {
	if (verbose >= __V2) { printf("Loading keys...\n"); }
	fseek(fp, offset, SEEK_SET);
	int n = fread(keys, sizeof(struct huffman_key_t), n_symbols, fp);
	if (n != n_symbols) {
		printf("Invalid keys, exiting.\n");
		exit(1);
	} else if (verbose >= __V2) {
		printf("Loaded keys...\n");
	}

	if (verbose >= __V3) { print_keys(keys, n_symbols); }

}

int load_header(struct huffman_header_t* header_p, FILE* fp, int verbose) {

	unsigned char valid_signature[4] = {'H', 'U', 'F', 'F'};

	if (verbose >= __V4)  { printf("Loading header...\n"); }

	if (fseek(fp, 0, SEEK_SET) != 0) {
		perror("fseek");
		exit(1);
	};
	unsigned char signature [4];
	if (fread(signature, sizeof(unsigned char), SIGNATURE_LEN, fp) != SIGNATURE_LEN) {
		perror("fread");
		exit(1);
	}
	int signature_test = memcmp(signature, valid_signature, SIGNATURE_LEN * sizeof(unsigned char));
	if (signature_test != 0) {
		fprintf(stderr, "Invalid input file, exiting\n");
		exit(1);
	}

	fseek(fp, 0, SEEK_SET);
	fread(header_p, sizeof(struct huffman_header_t), 1, fp);

	if (verbose >= __V2) { printf("Loaded header...\n"); }
	if (verbose >= __V3) { print_header(*header_p); }
	return 1;
}

int compare_n_bits(int a[], int b[], int len, int offset_a, int offset_b) {
	int i;
	for (i = 0; i < len; i++) {
		if (compare_bits_at_pos(a, b, offset_a + i, offset_b + i) == 0) {
			return 0;
		}
	}
	return 1;
}


void clear_int_array(int a[], int len) {
	int i;
	for (i = 0; i < len; i++) {
		a[i] = 0;
	}
}

int compress_huffman_code(struct huffman_code_t huffman_code) {
	struct huffman_code_compressed_t* huffman_compressed = (struct huffman_code_compressed_t *) malloc(sizeof(struct huffman_code_compressed_t));
	int i;
	huffman_compressed->code[0] = 0;
	for (i = 0; i < huffman_code.len; i++) {
		if (huffman_code.code[i] == 1) {
			set_bit(huffman_compressed->code, i);
		}
	}
	return huffman_compressed->code[0];
}

void  set_bit( int A[],  int k )
{
    A[k/32] |= 1 << (k%32);  /* Set the bit at the k-th position in A[i] */
}

void clear_bit( int A[],  int k )                
{
    A[k/32] &= ~(1 << (k%32));
}

void encode_file(struct huffman_code_t huffman_codes[], FILE* fp) {

}

int test_bit( int A[],  int k )
{
    return ( (A[k/32] & (1 << (k%32) )) != 0 ) ;     
}

int compare_bits(int a[], int b[], int i) {
    return ((a[i/32] & (1 << (i%32))) != 0) == ((b[i/32] & (1 << (i%32))) != 0);
}

int compare_bits_at_pos(int a[], int b[], int i, int j) {
    return ((a[i/32] & (1 << (i%32))) != 0) == ((b[j/32] & (1 << (j%32))) != 0);

}
void print_bits(int num) {
    int num_bits = sizeof(int) * 8; 
	int i;
	for (i = 0; i < num_bits; i++) {
		int bit = (num >> i) & 1;
		printf("%d", bit);
	}
}

void print_n_bits(int num[], int n, int offset, int marker) {
	if (marker == 0) {
		marker = -1;
	}
	int i;
	int size = sizeof(num[0]) * 8;
	for (i = offset; i < n+offset; i++) {
		if (i == marker) {
			printf("|");
		}
		printf("%d", ( (num[i/size] & (1 << (i%size) )) != 0 ));  
        
	}
    
    
    printf("\n");
}

void print_compressed_data(FILE* fp, int data_offset, int data_end) {
	int buffer[1];
	int bits_printed = 0;
	int marker = 0;
	clear_int_array(buffer, 1);
	fseek(fp, data_offset, SEEK_SET);
	while(fread(buffer, sizeof(buffer[0]), 1, fp) == 1) {
		int bits_to_print = sizeof(buffer[0]) * 8;
		int bits_remaining = (data_end - data_offset) - bits_printed;
		if ( bits_remaining < bits_to_print) { marker = bits_remaining; }
		print_n_bits(buffer, bits_to_print, 0, marker);
		bits_printed += bits_to_print;
	}
}