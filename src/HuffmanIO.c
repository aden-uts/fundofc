#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HuffmanCoding.h"
#include "HuffmanUtils.h"

#define CACHE_SIZE 2

typedef unsigned char byte;

/* struct huffman_file_t {
    struct huffman_header_t;
    struct huffman_table_t;
}; */

struct huffman_code_compressed_t {
	int code[0];
	char item;
	short len;
};

struct huffman_header_t {
    unsigned char signature[4];
    int symbol_size;
    int n_symbols;
    int table_offset;
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

struct huffman_header_t make_header(int symbol_size, int n_symbols) {
    struct huffman_header_t header;
    unsigned char signature[4] = {'T', 'E', 'S', 'T'};
    memcpy(header.signature, signature, 4);
    header.symbol_size = symbol_size;
    header.n_symbols = n_symbols;
    header.table_offset = sizeof(header);
    return header;
}

/*struct huffman_table_t make_table(unsigned char value, unsigned char prefix[]) {

}*/

void print_header(struct huffman_header_t header) {
    printf("Signature:\t%s\n", header.signature);
    printf("Symbol Size: \t%d bytes\n", header.symbol_size);
    printf("N Symbols: \t%d\n", header.n_symbols);
    printf("Table Offset: \t%d\n", header.table_offset);
}


void parse_file(FILE *fp) {
	int c;
	char chars[MAX_SYMBOLS];
	int char_counts[MAX_SYMBOLS];
	int element_count = 0;
	while ((c = fgetc(fp)) != EOF) {
		int i;
		int found = 0;
		char c_i = (char) c;
		for (i = 0; i <= element_count; i++) {
			if (c_i == chars[i]) {
				char_counts[i] += 1;
				found = 1;
				break;
			}
		}
		if (found == 0) {
			chars[element_count] = c_i;
			char_counts[element_count] = 1;
			element_count++;
		}
	}

	int i;
	for (i = 0; i < element_count; i++) {
		printf("(%c): %d\n", (unsigned char) chars[i], char_counts[i]);
	}


	struct huffman_code_t huffman_codes[element_count];

	get_huffman_codes(huffman_codes, chars, char_counts, element_count);

	FILE * output_fp = fopen("output", "w");

    if (output_fp != NULL) {
    	fwrite(huffman_codes, sizeof(struct huffman_code_t) , element_count, output_fp);
		fclose(output_fp);
	}

	struct huffman_code_t huffman_codes_test[element_count];

	printf("Testing load...\n");
	FILE * input_fp = fopen("output", "r");

	
	
	fread(huffman_codes_test, sizeof(struct huffman_code_t), element_count, input_fp);
	for (i = 0; i < element_count; i++) {
		printf("%c ", huffman_codes_test[i].item);
		int j;
		for (j = 0; j < huffman_codes_test[i].len; j++) {
			printf("%d", huffman_codes_test[i].code[j]);
		}
		printf("\n");
	}


    struct huffman_header_t header = make_header(sizeof(chars[0]), element_count);
    print_header(header);

	printf("%d \n", sizeof(struct huffman_code_t));

	struct huffman_code_compressed_t huffman_test;

	printf("Testing compression...\n");
	compress_huffman_code(huffman_codes[40]);
	
	struct huffman_code_compressed_t huffman_codes_compressed[element_count];

	for (i = 0; i < element_count; i++) {
		huffman_codes_compressed[i].item = huffman_codes[i].item;
		huffman_codes_compressed[i].len = huffman_codes[i].len;
		huffman_codes_compressed[i].code[0] = compress_huffman_code(huffman_codes[i]);
	}

	FILE * output_compressed_fp = fopen("outputcompressed", "w");

    if (output_compressed_fp != NULL) {
    	fwrite(huffman_codes, sizeof(struct huffman_code_compressed_t) , element_count, output_compressed_fp);
		fclose(output_compressed_fp);
	}

	int sum = 0;
	for (i = 0; i < element_count; i++) {
		sum += huffman_codes[i].len;
	}
	printf("%d\n", sum);
	printf("%d\n", sum / 8 + 1);


	int required_bits = sum / 32 + 1;	
	int test_compress[required_bits];

	for (i = 0; i < required_bits; i++) {
		test_compress[i] = 0;
	}

	int upto = 0;		
	for (i = 0; i < element_count; i++) {
		int j;
		for (j = 0; j < huffman_codes[i].len; j++) {
			if (huffman_codes[i].code[j] == 1) {
				set_bit(test_compress, j + upto);
			}
		}
		upto += huffman_codes[i].len;
	}
	printf("################\n");
	for (i = 0; i < required_bits; i++) {
		print_bits(test_compress[i]);
	}

	FILE * output_more_compressed_fp = fopen("outputmorecompressed", "w");

	struct huffman_key_t huffman_keys[element_count];
	for (i = 0; i < element_count; i++) {
		huffman_keys[i].item = huffman_codes[i].item;
		huffman_keys[i].len = huffman_codes[i].len;
	}

    if (output_more_compressed_fp != NULL) {
		fwrite(huffman_keys, sizeof(struct huffman_key_t), element_count, output_more_compressed_fp);
    	fwrite(test_compress, sizeof(test_compress[0]) , required_bits, output_more_compressed_fp); 
		fclose(output_more_compressed_fp);
	}

	printf("%d\n", sizeof(short));
	printf("%d\n", sizeof(struct huffman_code_compressed_t));
	printf("%d\n", element_count);
	printf("%d\n", sizeof(struct huffman_key_t));
	printf("%d\n", sizeof(huffman_keys));
	printf("%d\n", sizeof(test_compress));

	printf("Testing load...\n");

	FILE * input2_fp = fopen("outputmorecompressed", "r");
	struct huffman_key_t huffman_keys_load[element_count];

	fread(huffman_keys_load, sizeof(struct huffman_key_t), element_count, input2_fp);
	for (i = 0; i < element_count; i++) {
		printf("%c ", huffman_keys_load[i].item);
		printf("%d ", huffman_keys_load[i].len);
		printf("\n");
	}

	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	printf("%d\n", sz);

	int codes[sz];

	FILE * output_compress_file = fopen("outputfile", "w");

	fseek(fp, 0L, SEEK_SET);
	while ((c = fgetc(fp)) != EOF) {
		int i;
		int found = 0;
		char c_i = (char) c;
		int cache[CACHE_SIZE];
		for (i = 0; i < CACHE_SIZE; i++) {
			cache[i] = 0;
		}
		int cache_i = 0;
		for (i = 0; i <= element_count; i++) {
			if (c_i == huffman_codes_test[i].item) {
				int j;
				for (j = 0; j < huffman_codes_test[i].len; j++) {
					printf("%d", huffman_codes_test[i].code[j]);
				}
				printf("\n");
				for (j = 0; j < huffman_codes_test[i].len; j++) {
					printf("About to test cache size\n");
					if (cache_i == 32) {
						printf("here\n");
						printf("%d %d\n", cache_i, sizeof(cache));
						if (output_compress_file != NULL) {
							fwrite(cache, sizeof(cache) , 1, output_compress_file);
							fclose(output_compress_file);
						}
						cache[0] = 0;
						cache_i = 0;
					}
					printf("About to check code\n");
					if (huffman_codes_test[i].code[j] == 1) {
						printf("Setting bit %d\n", cache_i);
						set_bit(cache, cache_i);
					} else {
						printf("Skipping bit %d\n", cache_i);
					}
					cache_i++;
				}
			}
		}
		
	}

}

int compress_huffman_code(struct huffman_code_t huffman_code) {
	struct huffman_code_compressed_t* huffman_compressed = (struct huffman_code_compressed_t *) malloc(sizeof(struct huffman_code_compressed_t));
	int i;
	huffman_compressed->code[0] = 0;
	for (i = 0; i < huffman_code.len; i++) {
		printf("%d", huffman_code.code[i]);
	}
	for (i = 0; i < 32; i++) {
		if (test_bit(huffman_compressed->code, i)) {
			printf("1");
		} else {
			printf("0");
		}
	}
	printf("\n");
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

void print_bits(int num) {
	printf("Num is: %d\n", num);
    int num_bits = sizeof(int) * 8; 
	int i;
    for(i = num_bits - 1; i >= 0; i--) {
        int bit = (num >> i) & 1; 
        
        printf("%d", bit);
        
        if(i % 8 == 0) {
            printf(" "); 
        }
    }
    
    printf("\n");
}