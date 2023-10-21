#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#define MAX_TREE_HT 50
#define MAX_SIZE (16 * 1024 * 1024)
#define MAX_SYMBOLS 1024

void printArray(int arr[], int n);
void HuffmanCodes(char item[], int freq[], int size);

#endif