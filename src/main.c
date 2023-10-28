#include "HuffmanCoding.h"
#include "HuffmanIO.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: %s FILENAME \n", argv[0]);
		return 1;
	}
	char *buf = (char *)malloc(MAX_SIZE);
	if(!buf) {
		fprintf(stderr, "Couldn't allocate memory\n");
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	if(!f) {
		perror("fopen");
		free(buf);
		return 1;
	}
	compress_input_file(f);

	return 1;
}

