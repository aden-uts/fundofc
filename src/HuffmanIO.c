#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<unistd.h> /* sleep for testing */

#include "HuffmanCoding.h"
#include "HuffmanUtils.h"

#define CACHE_SIZE 2
#define SIGNATURE_LEN 4
#define V 1
#define VV 2
#define VVV 3

void print_bits(int num);
void set_bit( int A[],  int k );
int test_bit(int A[], int k);

typedef unsigned char byte;

/* struct huffman_file_t {
    struct huffman_header_t;
    struct huffman_table_t;
}; */

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
	int data_end;
};

struct huffman_key_t {
	short len;
    unsigned char item;
};

struct huffman_table_t {
    int n;
};

struct huffman_table_entry_t {
    unsigned char value[1];
    unsigned char prefix[];
};

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

/*struct huffman_table_t make_table(unsigned char value, unsigned char prefix[]) {

}*/

void print_header(struct huffman_header_t header) {
    printf("Signature:\t%s\n", header.signature);
    printf("Symbol Size: \t%d bytes\n", header.symbol_size);
    printf("N Symbols: \t%d\n", header.n_symbols);
	printf("Key offset: \t%d\n", header.key_offset);
    printf("Table Offset: \t%d\n", header.table_offset);
	printf("Data offset: \t%d\n", header.data_offset);
	printf("Data end: \t%d\n", header.data_end);
}


void parse_file(FILE *fp) {
	
	/* init vars
		- i for many for loops
		- c for reading chars from file
		- element count for number of items needing encoding
	 */

	int i, c;
	char chars[MAX_SYMBOLS];
	int char_counts[MAX_SYMBOLS];
	int element_count = 0;


	/* Reading chars from file */

	/* Loop over all chars in file */
	while ((c = fgetc(fp)) != EOF) {
		/* Bool flag to check if already found */
		int found = 0;
		/* Cast c to char for safety :) */
		char c_i = (char) c;

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
	struct huffman_code_t huffman_codes[element_count];
	get_huffman_codes(huffman_codes, chars, char_counts, element_count);
	printf("Element count %d\n", element_count);
	
	int required_data_bits = 0;
	for (i = 0; i < element_count; i++) {
		required_data_bits += huffman_codes[i].len * char_counts[i];
	} 
	printf("!!!!!!!!!Required data bits: %d\n", required_data_bits);
	sort_codes(huffman_codes, element_count);


	/* ######### THIS SHOULD BE MOVED #########  */

	FILE * output_fp = fopen("output", "w");

	printf("Opened file...\n");
    if (output_fp != NULL) {
    	fwrite(huffman_codes, sizeof(struct huffman_code_t) , element_count, output_fp);
		fclose(output_fp);
	}
	printf("Wrote to file...\n");

	struct huffman_code_t huffman_codes_large[element_count];

	printf("Testing load...\n");
	FILE * input_fp = fopen("output", "r");

	
	printf("Testing read...\n");
	fread(huffman_codes_large, sizeof(struct huffman_code_t), element_count, input_fp);
	for (i = 0; i < element_count; i++) {
		printf("%c ", huffman_codes_large[i].item);
		int j;
		for (j = 0; j < huffman_codes_large[i].len; j++) {
			printf("%d", huffman_codes_large[i].code[j]);
		}
		printf("\n");
	}



	printf("%d \n", sizeof(struct huffman_code_t));

	struct huffman_code_compressed_t huffman_test;

	printf("Testing compression...\n");
	compress_huffman_code(huffman_codes[0]);
	printf("Compression done...\n");
	
	struct huffman_code_compressed_t huffman_codes_compressed[element_count];

	printf("Starting loop...\n");
	for (i = 0; i < element_count; i++) {
		huffman_codes_compressed[i].item = huffman_codes[i].item;
		huffman_codes_compressed[i].len = huffman_codes[i].len;
		huffman_codes_compressed[i].code[0] = compress_huffman_code(huffman_codes[i]);
	}

	FILE * output_compressed_fp = fopen("outputcompressed", "w");

    if (output_compressed_fp != NULL) {
    	fwrite(huffman_codes_compressed, sizeof(struct huffman_code_compressed_t) , element_count, output_compressed_fp);
		fclose(output_compressed_fp);
	}

	int sum = 0;
	for (i = 0; i < element_count; i++) {
		sum += huffman_codes[i].len;
	}
	printf("SUM: %d\n", sum);
	printf("SUM: %d\n", sum / 8 + 1);


	int required_bits = sum / 32 + 1;	
	int test_compress[required_bits];
	printf("Required bits: %d\n", required_bits);

	for (i = 0; i < required_bits; i++) {
		test_compress[i] = 0;
	}

	printf("Start compressing table...\n");

	int upto = 0;		
	for (i = 0; i < element_count; i++) {
		int j;
		for (j = 0; j < huffman_codes[i].len; j++) {
			if (huffman_codes[i].code[j] == 1) {
				set_bit(test_compress, j + upto);
			}
		}
		upto += huffman_codes[i].len;
		printf("%d\n", upto);
	}
/*	printf("################\n");\*/
	for (i = 0; i < required_bits; i++) {
		printf("here\n");
		print_bits(test_compress[i]);
	}
	printf("Done...\n");

	printf("Test compress:");


	FILE * output_more_compressed_fp = fopen("outputmorecompressed", "w");

	printf("Setting keys...\n");
	struct huffman_key_t huffman_keys[element_count];
	for (i = 0; i < element_count; i++) {
		huffman_keys[i].item = huffman_codes[i].item;
		huffman_keys[i].len = huffman_codes[i].len;
	}
	printf("Set keys done...\n");

	printf("Writing file...\n");
    if (output_more_compressed_fp != NULL) {
		fwrite(huffman_keys, sizeof(struct huffman_key_t), element_count, output_more_compressed_fp);
    	fwrite(test_compress, sizeof(test_compress[0]) , required_bits, output_more_compressed_fp); 
		fclose(output_more_compressed_fp);
	}
	printf("Done writing file!...\n");
	printf("here!");
	printf("here!");
	FILE * input2_fp = fopen("outputmorecompressed", "r");
	struct huffman_key_t huffman_keys_load[element_count];

	fread(huffman_keys_load, sizeof(struct huffman_key_t), element_count, input2_fp);
	for (i = 0; i < element_count; i++) {
/*		printf("%c ", huffman_keys_load[i].item);\*/
/*		printf("%d ", huffman_keys_load[i].len);\*/
/*		printf("\n");\*/
	}


	/* ######### OUTPUTTING FULL FILE ############ */
	int table_size = sizeof(test_compress[0]) * required_bits;
	printf("Required bits %d", required_bits);
	struct huffman_header_t output_header = make_header(1, element_count, table_size, required_data_bits);
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
/*	printf("%d\n", sz);\*/

	int codes[sz];	

	FILE * output_compress_file = fopen("outputfile", "w");

	fwrite(&output_header, sizeof(struct huffman_header_t), 1, output_compress_file);
	fwrite(huffman_keys, sizeof(struct huffman_key_t), element_count, output_compress_file);
	fwrite(test_compress, sizeof(test_compress[0]) , required_bits, output_compress_file);


	fseek(fp, 0L, SEEK_SET);
	int found = 0;
	int cache[CACHE_SIZE];
	for (i = 0; i < CACHE_SIZE; i++) {
		cache[i] = 0;
	}
	int cache_i = 0;
	while ((c = fgetc(fp)) != EOF) {
		
		char c_i = (char) c;
		int i;
		for (i = 0; i < element_count; i++) {

			if (c_i == huffman_codes_large[i].item) {
				int j;
				for (j = 0; j < huffman_codes_large[i].len; j++) {
/*					printf("About to test cache size\n");\*/
					if (cache_i == CACHE_SIZE * 32) {
/*						printf("here\n");\*/
/*						printf("%d %d\n", cache_i, sizeof(cache));\*/

						if (output_compress_file != NULL) {
							fwrite(cache, sizeof(int) , CACHE_SIZE, output_compress_file);
						}
						cache[0] = 0;
						cache_i = 0;
					}
/*					printf("About to check code\n");\*/
					if (huffman_codes_large[i].code[j] == 1) {
/*						printf("Setting bit %d\n", cache_i);\*/
						set_bit(cache, cache_i);
					} else {
/*						printf("Skipping bit %d\n", cache_i);\*/
					}
					cache_i++;
				}
			}
		}
		
	}
	if (output_compress_file != NULL) {
		printf("Final cache write...\n");
		for (i = 0; i < CACHE_SIZE; i++) {
			print_bits(cache[i]);
		}
		int success = fwrite(cache, sizeof(int) , CACHE_SIZE, output_compress_file);
		printf("Success: %d\n", success);
	}
	fclose(output_compress_file);



	/* ######### LOADING FULL FILE ############ */
	FILE * input_output_file = fopen("outputfile", "r");

	decompress_file(input_output_file, VVV);
	

	// fread(huffman_keys_input, sizeof(struct huffman_key_t), n_symbols, input_compress_file);
	// for (i = 0; i < n_symbols; i++) {
	// 	printf("%c\t%d\n", huffman_keys_input[i].item, huffman_keys_input[i].len);
	// }

	// min_size = huffman_keys_input[0].len;
	// max_size = huffman_keys_input[n_symbols - 1].len;
	
	// printf("Navigating to data offset at %db\n", header_input->data_offset);
	// int max_stretch = max_size / 32 + 2;
	// printf("Max stretch: %d\n", max_stretch);

	// int input_buf[max_stretch];
	// clear_int_array(input_buf, max_stretch);
	// printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
	// print_n_bits(input_buf, 64, 0);
	// printf("!!!!!!!!!!!!!!!!!!!!!\n");
	// int input_buf_size = 0;
	// int input_buf_i = 0;

	// fseek(input_compress_file, header_input->data_offset, SEEK_SET);

	// int decrypted = 0;
	// int match_count = 0;
	// while (!decrypted) {
	// 	printf("Still not decrypted...\n");
	// 	/* Set input buffer from file contents */
	// 	while (input_buf_size < max_stretch) {
	// 		input_buf_size += fread(input_buf, sizeof(int), max_stretch, input_compress_file);
	// 	}
	// 	/*printf("")
	// 	print_bits(input_buf[0]);*/
		
	// 	/* Buffer is now full,
	// 		start reading and looking for known patterns */
		
	// 	/* start reading minimum bits from input
	// 		and check them against letters with the same number */
	// 	int current_check_length = min_size;

	// 	/* Loop over keys */
	// 	int key_i;
	// 	int key_i_offset = 0;

	// 	/* for testing */
	// 	int found_code = 0;
	// 	while (!found_code) {
	// 		for (key_i = 0; key_i < n_symbols; key_i++) {
	// 			sleep(1);
	// 			printf("Trying key_i: %d\n", key_i);
	// 			/* check if current key has length that we're looking for */
	// 			if (huffman_keys_input[key_i].len == current_check_length) {
	// 				printf("Checking %c (matching length)\n",huffman_keys_input[key_i].item);
	// 				/* navigate to key offset in table */
	// 				fseek(input_compress_file, header_input->table_offset, SEEK_SET);
	// 				/* Load code from table into a buffer */
	// 				int table_buf[max_stretch];
	// 				clear_int_array(table_buf, max_stretch);
	// 				fread(table_buf, sizeof(int), max_stretch, input_compress_file);

	// 				/* Compare bits between input and table */
	// 				int bit_i;
	// 				found_code = 1;
	// 				for (bit_i = 0; bit_i < current_check_length ; bit_i++) {
	// 					printf("bit_i: %d\n", bit_i);
	// 					printf("checking key bit %d\n",  (key_i_offset % 32) + bit_i);
	// 					if (compare_bits_at_pos(input_buf, table_buf, input_buf_i + bit_i, (key_i_offset % 32) + bit_i)) {
	// 						/* input_buf_i += current_check_length; */
	// 						match_count++;
	// 						printf("%d matched\n", match_count);
	// 					} else {
	// 						printf("doesn't match bit\n");
	// 						found_code = 0;
	// 					}
	// 					printf("Input buf: offset %d\n", input_buf_i);
	// 					print_n_bits(input_buf, max_stretch * 32, 0);
	// 					print_bits(input_buf[0]);
	// 					print_bits(input_buf[1]);
	// 					printf("\n");
	// 					for (i = 0; i < input_buf_i; i++) {
	// 						printf(" ");
	// 					}
	// 					print_n_bits(input_buf, current_check_length, input_buf_i);
	// 					printf("Table buf: offset %d\n", key_i_offset);
	// 					print_bits(table_buf[0]);
	// 					print_bits(table_buf[1]);
	// 					printf("\n");
	// 					for (i = 0; i < key_i_offset; i++) {
	// 						printf(" ");
	// 					}
	// 					print_n_bits(table_buf, current_check_length, key_i_offset);
	// 					printf("%d\n", compare_bits(input_buf, table_buf, current_check_length));
	// 					printf("Current check length: %d\n", current_check_length);
	// 					printf("\n");
	// 				}
	// 				printf("bit check loop over\n");
	// 				printf("Found code: %d\n", found_code);
	// 				if (found_code == 1) {
	// 					printf("Writing...\n");
	// 					input_buf_i += current_check_length;
	// 					putc(huffman_keys_input[key_i].item, input_output_file);

	// 					key_i_offset += huffman_keys_input[i].len;

	// 					printf("breaking");
	// 					break;
	// 				}
	// 			} else {
	// 				printf("lengths do not match\n");
	// 			}
	// 			if (ftell(input_compress_file) + input_buf_i >= header_input->data_end - 1) {
	// 				printf("DONEDONEDONE\n");
	// 				exit(1);
	// 			} else {
	// 				printf("ftell: %d\n", ftell(input_compress_file) + input_buf_i);
	// 				printf("Data done: %d\n", header_input->data_end);
	// 			}
	// 		}
	// 	current_check_length += 1;
	// 	}


		/* run loop once for testing */
		/* decrypted = 1; */
	}




	


void decompress_file(FILE* fp, int verbose) {
	if (verbose >= VV) { printf("Decompressing file...\n"); }
	struct huffman_header_t* header = (struct huffman_header_t *) malloc(sizeof(struct huffman_header_t));
	load_header(header, fp, verbose);
	if (verbose >= VV) { printf("Loaded header...\n"); }
	if (verbose >= VVV) { print_header(*header); }
	
	int compressed_bit_count = header->data_end - header->data_offset;
	if (verbose >= VV) { printf("%d bytes to decompress (%d bits)\n", bits_to_bytes(compressed_bit_count), compressed_bit_count); }


	int n_symbols, min_code_len, max_code_len = 0;
	n_symbols = header->n_symbols;
	if (verbose >= VVV) { printf("%d symbols found...\n", n_symbols); }

	struct huffman_key_t keys[n_symbols];
	load_keys(keys, fp, header->key_offset, n_symbols, verbose);
	if (verbose >= VVV) { print_keys(keys, n_symbols); }

	min_code_len = keys[0].len;
	max_code_len = keys[n_symbols - 1].len;

	if (verbose >= VVV) { printf("Min code length: %d\nMax code length: %d\n", min_code_len, max_code_len); }

	if (verbose >= VV) { printf("Starting data decompression...\n"); }


	int max_buffer_size = max_code_len / sizeof(int) / 8 + 2;
	int max_buffer_size_bits = max_buffer_size * sizeof(int) * 8;
	if (bits_to_bytes(compressed_bit_count)  < max_buffer_size) { max_buffer_size = bits_to_bytes(compressed_bit_count); }
	if (verbose >= VVV) { printf("Setting buffer size to %d bits\n", max_buffer_size_bits); }

	int input_buffer[2];
	clear_int_array(input_buffer, 2);

	int current_buffer_size_bits = max_buffer_size_bits;
	if (compressed_bit_count < max_buffer_size_bits) {
		current_buffer_size_bits = compressed_bit_count;
	}


	if (verbose >= VVV) {
		printf("Loading to buffer...\n");
	}
	int input_byte_upto = 0;
	load_to_buffer(input_buffer, fp, sizeof(input_buffer[0]), max_buffer_size, header->data_offset);
	input_byte_upto += header->data_offset + max_buffer_size;
	printf("header data offset: %d\n", header->data_offset);
	printf("input byte upto: %d\n", input_byte_upto);
	print_n_bits(input_buffer, current_buffer_size_bits, 0);

	int unbuffered_bits = compressed_bit_count - current_buffer_size_bits;
	printf("Unb: %d\n", unbuffered_bits);
	int input_buffer_offset_bits = 0;

	while (current_buffer_size_bits > 0) {
		

		input_buffer_offset_bits++;
		current_buffer_size_bits--;

		if (current_buffer_size_bits == max_buffer_size_bits - (sizeof(input_buffer[0]) * 8)
			&& unbuffered_bits > 0) {
			print_n_bits(input_buffer, current_buffer_size_bits, input_buffer_offset_bits);
			input_buffer[0] = input_buffer[1];
			int load_success;
			printf("Sizeof: %d\n", sizeof(input_buffer[0]));
			printf("ibupto: %d\n", input_byte_upto);
			load_success = load_to_buffer(input_buffer + max_buffer_size - 1, fp, sizeof(input_buffer[0]), 1, input_byte_upto);
			printf("Load count: %d\n", load_success);
			if (load_success != 1) {
				printf("Failed to load file to buffer, exiting\n");
				exit(1);
			}
			input_buffer_offset_bits -= sizeof(input_buffer[0]) * 8;
			current_buffer_size_bits += sizeof(input_buffer[0]) * 8;
			unbuffered_bits -= sizeof(input_buffer[0]) * 8;
			input_byte_upto += sizeof(input_buffer[0]);
			print_n_bits(input_buffer, current_buffer_size_bits, input_buffer_offset_bits);
			printf("input_byte_upto: %d\n", input_byte_upto);
		}


	}



	// fseek(input_compress_file, header_input->key_offset, SEEK_SET);
	// struct huffman_key_t huffman_keys_input[n_symbols];

}

int bits_to_bytes(int n_bits) { return n_bits / 8 + 1; }

int load_to_buffer(int buffer[], FILE* fp, int size_bytes, int count, int offset_bytes ) {
	fseek(fp, offset_bytes, SEEK_SET);
	int read_count = fread(buffer, size_bytes, count, fp);
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
	if (verbose >= VV) { printf("Loading keys...\n"); }
	fseek(fp, offset, SEEK_SET);
	int n = fread(keys, sizeof(struct huffman_key_t), n_symbols, fp);
	if (n != n_symbols) {
		printf("Invalid keys, exiting.\n");
		exit(1);
	} else if (verbose >= VV) {
		printf("Loaded keys...\n");
	}

}

void load_header(struct huffman_header_t* header_p, FILE* fp, int verbose) {

	unsigned char valid_signature[4] = {'H', 'U', 'F', 'F'};

	if (verbose >= VVV)  { printf("Loading header...\n"); }

	fseek(fp, 0, SEEK_SET);
	unsigned char signature [4];
	fread(signature, sizeof(unsigned char), SIGNATURE_LEN, fp);
	int signature_test = memcmp(signature, valid_signature, SIGNATURE_LEN * sizeof(unsigned char));
	if (signature_test != 0) {
		printf("Invalid file, exiting\n");
		exit(1);
	}

	fseek(fp, 0, SEEK_SET);
	fread(header_p, sizeof(struct huffman_header_t), 1, fp);
}

int compare_n_bits(int a[], int b[], int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("calling compare\n");
		if (compare_bits(a, b, i) == 0) {
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
/*		printf("%d", huffman_code.code[i]);\*/
	}
	for (i = 0; i < 32; i++) {
		if (test_bit(huffman_compressed->code, i)) {
/*			printf("1");\*/
		} else {
/*			printf("0");\*/
		}
	}
/*	printf("\n");\*/
	for (i = 0; i < huffman_code.len; i++) {
		if (huffman_code.code[i] == 1) {
			set_bit(huffman_compressed->code, i);
		}
		
		/* set_bit(huffman_compressed->code, i); */
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
    /*for(i = num_bits - 1; i >= 0; i--) {
        int bit = (num >> i) & 1; 
        
        printf("%d", bit);
        
        if(i % 8 == 0) {
            printf(" "); 
        }
    } */
    
}

void print_n_bits(int num[], int n, int offset) {
	int i;
	int size = sizeof(num[0]) * 8;
	for (i = offset; i < n+offset; i++) {
		printf("%d", ( (num[i/size] & (1 << (i%size) )) != 0 ));  
        
	}
    
    
    printf("\n");
}