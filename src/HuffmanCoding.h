#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#include "HuffmanUtils.h"

#define MAX_SIZE (16 * 1024 * 1024)
#define MAX_SYMBOLS 1024

void print_array(int arr[], int n);
void huffman_codes(char item[], int freq[], int size);
void get_huffman_codes(struct huffman_code_t huffman_codes[], char item[],
                       int freq[], int size);
void sort_codes(struct huffman_code_t huffman_codes[], int size);

#endif