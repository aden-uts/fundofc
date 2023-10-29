#ifndef HUFFMANIO_H
#define HUFFMANIO_H
#include <stdio.h>
#include <stdlib.h>

void compress_input_file(FILE *fp, char output_file_name[]);
void decompress_file(FILE *fp, char output_file_name[], int verbose);

#endif