#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#define MAX_TREE_HT 50
#define MAX_SIZE (16 * 1024 * 1024)
#define MAX_SYMBOLS 1024

void print_array(int arr[], int n);
void huffman_codes(char item[], int freq[], int size);

#endif