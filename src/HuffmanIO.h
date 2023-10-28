#ifndef HUFFMANIO_H
#define HUFFMANIO_H
#include <stdio.h>
#include <stdlib.h>

void compress_input_file(FILE *fp);
void decompress_file(FILE *fp, FILE* fp_out, int verbose);

#endif