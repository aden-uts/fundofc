/*******************************************************************************
Author: Aden Northcote, Emery Strasser, Jonathon Ngo and Jordan Stevens
Date: 05/11/2023
Purpose: This program compresses any file using Huffman Encoding with high entropy.
*******************************************************************************/


/*******************************************************************************
Include header files and function prototypes
*******************************************************************************/
#ifndef HUFFMANIO_H
#define HUFFMANIO_H
#include <stdio.h>
#include <stdlib.h>

void compress_input_file(FILE *fp, char output_file_name[], int verbose);
void decompress_file(FILE *fp, char output_file_name[], int verbose);

#endif