#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HuffmanCoding.h"

typedef unsigned char byte;

/* struct huffman_file_t {
    struct huffman_header_t;
    struct huffman_table_t;
}; */

struct huffman_header_t {
    unsigned char signature[4];
    int symbol_size;
    int n_symbols;
    int data_offset;
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
    header.data_offset = sizeof(header);
    return header;
}

/*struct huffman_table_t make_table(unsigned char value, unsigned char prefix[]) {

}*/

void print_header(struct huffman_header_t header) {
    printf("Signature:\t%s\n", header.signature);
    printf("Symbol Size: \t%d bytes\n", header.symbol_size);
    printf("N Symbols: \t%d\n", header.n_symbols);
    printf("Data Offset: \t%d\n", header.data_offset);
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
		printf("(%X): %d\n", (unsigned char) chars[i], char_counts[i]);
	}

    

	HuffmanCodes(chars, char_counts, element_count);

    struct huffman_header_t header = make_header(sizeof(chars[0]), element_count);
    print_header(header);
}