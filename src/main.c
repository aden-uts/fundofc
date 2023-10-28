#include "HuffmanCoding.h"
#include "HuffmanIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Globals.h"

#define MAX_FILENAME 256

#define HELP_ARG_WIDTH 25 
#define HELP_DESCRIPTION_WIDTH 40

void print_help();
int file_exists(char file_name[]);

int main(int argc, char **argv)
{
	if(argc < 2) {
		printf("Usage: %s FILENAME \n", argv[0]);
		return 1;
	}

	unsigned short compress_flag = 0, decompress_flag = 0, verbose_flag_value = 0, found_flags = 0;

	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			exit(0);
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
			if (i + 1 < argc) {
				if (strcmp(argv[i + 1], "1") == 0) {
					verbose_flag_value = __V1;
				} else if (strcmp(argv[i + 1], "2") == 0) {
					verbose_flag_value = __V2;
				} else if (strcmp(argv[i + 1], "3") == 0) {
					verbose_flag_value = __V3;
				} else if (strcmp(argv[i + 1], "DEBUG") == 0) {
					verbose_flag_value = __V4;
				} else {
					fprintf(stderr, "Invalid value for -v, --verbose\n");
					fprintf(stderr, "Valid values are 1, 2, 3, DEBUG\n");
				}
				/* increment i to avoid reading the verbose value again */
				i++;
				found_flags += 2;
			} else {
				fprintf(stderr, "Invalid use of -v, --verbose\n");
				exit(1);
			}
		} else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--compress") == 0) {
			compress_flag = 1;
			found_flags++;
		} else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decompress") == 0) {
			decompress_flag = 1;
			found_flags++;
		}
	}

	unsigned short verbose_lvl = verbose_flag_value;

	if (verbose_lvl >= __V2) { printf("Verbosity: %d\n", verbose_lvl); }
	if (verbose_lvl >= __V4) { printf("Found flags: %d\n", found_flags); }
	if (verbose_lvl >= __V4) { printf("Arg count: %d\n", argc); }


	char input_file_name[MAX_FILENAME];
	char output_file_name[MAX_FILENAME];

	unsigned short output_set = 0;
	int filename_count = argc - 1 - found_flags;
	if (verbose_lvl >= __V4) { printf("Filenames found: %d\n", filename_count); }
	if (filename_count < 1) {
		fprintf(stderr, "FILE not provided. Exiting\n");
		exit(1);
	} else if (filename_count > 2) {
		fprintf(stderr, "More than 2 filenames provided. Only FILE and [OUTPUT] are allowed. Exiting\n");
		exit(1);
	} else if (filename_count == 1) {
		strcpy(input_file_name, argv[argc - 1]);
		if (decompress_flag == 1) {
			strcpy(output_file_name, input_file_name);
			strcat(output_file_name, ".huff");
			output_set = 1;
		}
	} else if (filename_count == 1) {
	} else {
		strcpy(input_file_name, argv[argc - 2]);
		strcpy(output_file_name, argv[argc - 1]);
		output_set = 1;
	}

	if (verbose_lvl >= __V1) { printf("Input file: %s\n", input_file_name); }
	if (verbose_lvl >= __V1 && output_set != 0) { printf("Output file: %s\n", output_file_name); }

	if (verbose_lvl >= __V4) { printf("Testing memory: %db ... ", MAX_SIZE); }
	char *buf = (char *)malloc(MAX_SIZE);
	if(!buf) {
		fprintf(stderr, "Couldn't allocate memory\n");
		exit(1);
	}
	if (verbose_lvl >= __V4) { printf("Success\n"); }
	if (verbose_lvl >= __V4) { printf("Checking for input file ... "); }
	

	FILE *input_fp;
	if (!file_exists(input_file_name)) {
		fprintf(stderr, "Input file %s not found. Exiting\n", input_file_name);
		exit(1);
	} else {
		input_fp = fopen(input_file_name, "rb");
		if(!input_fp) {
			perror("fopen");
			free(buf);
			exit(1);
		}
	}

	if (verbose_lvl >= __V4) { printf("Success.\n"); }
	if (verbose_lvl >= __V4) { printf("Checking output file does not exist ... "); }

	FILE *output_fp;
	if (output_set == 1) {
		if (file_exists(output_file_name)) {
			fprintf(stderr, "Output file %s already exists. Exiting\n", output_file_name);
			exit(1);
		} else {
			output_fp = fopen(output_file_name, "wb");
			if(!output_fp) {
				perror("fopen");
				free(buf);
				exit(1);
			}
		}
	}
	if (verbose_lvl >= __V4) { printf("Success.\n"); }

	printf("%u %u\n", compress_flag, decompress_flag);

	if (compress_flag == 1 || (compress_flag == 0 && decompress_flag == 0) ) { 
		compress_input_file(input_fp);
	} else if (decompress_flag == 1) {
		decompress_file(input_fp, output_fp, verbose_lvl);
	}

	return 1;
}

void print_help() {
	printf("Usage: huff [OPTIONS] FILE\n");
	printf("Compress or decompress FILE via the Huff algorithm.\n");
	printf("If no options are passed, the FILE will be compressed.\n");
	printf("\n");
	printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-h, --help", HELP_DESCRIPTION_WIDTH, "Display this help and exit." );
    printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-v, --verbose V", HELP_DESCRIPTION_WIDTH, "Set output verbosity. Valid values are 1, 2, 3, DEBUG." );
}

int file_exists(char file_name[]) {
	FILE* fp;
	if ((fp = fopen(file_name, "rb"))) {
		fclose(fp);
		return 1;
	} else {
		return 0;
	}
}