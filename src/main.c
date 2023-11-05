/*******************************************************************************
Author: Aden Northcote, Emery Strasser, Jonathon Tjoe and Jordan Stevens
Group number: 16 
Date: 05/11/2023
Purpose: This program compresses any file using Huffman Encoding with high entropy.
Instructions:
1. cd fundofc-main
2. make
3. for help type "./huff -h"
4. to compress "./huff -c asd.bmp test.huff"
   if inside a file "./huff -c tests/asd.bmp test.huff"
5. to decompress text file "./huff -d test.huff text.txt" 
   for Image "./huff -d test.huff text.bmp"
6. with verbose "./huff -c -v 1 tests/asd.bmp test.huff"
                "./huff -c -v 2 tests/asd.bmp test.huff"
*******************************************************************************/


/*******************************************************************************
Include header files and function prototypes
*******************************************************************************/
#include "HuffmanCoding.h"
#include "HuffmanIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Globals.h"

#define MAX_FILENAME 128

#define HELP_ARG_WIDTH 25
#define HELP_DESCRIPTION_WIDTH 40

struct cli_args_t {
  unsigned char compress_flag;
  unsigned char decompress_flag;
  unsigned char verbose_flag_value;
  unsigned short found_flags;
  char input_file_name[MAX_FILENAME];
  char output_file_name[MAX_FILENAME];
};

void print_help();
int file_exists(char file_name[]);
struct cli_args_t parse_cli_args(int argc, char *argv[]);


/*******************************************************************************
main
*******************************************************************************/
int main(int argc, char **argv) {

  struct cli_args_t cli_args = parse_cli_args(argc, argv);
  unsigned short verbose_lvl = cli_args.verbose_flag_value;

  if (verbose_lvl >= __V1) {
    printf("Input file: %s\n", cli_args.input_file_name);
  }
  if (verbose_lvl >= __V1) {
    printf("Output file: %s\n", cli_args.output_file_name);
  }

  if (verbose_lvl >= __V4) {
    printf("Testing memory: %db ... ", MAX_SIZE);
  }
  char *buf = (char *)malloc(MAX_SIZE);
  if (!buf) {
    fprintf(stderr, "Couldn't allocate memory\n");
    exit(1);
  }
  if (verbose_lvl >= __V4) {
    printf("Success\n");
  }
  if (verbose_lvl >= __V4) {
    printf("Checking for input file ... ");
  }

  FILE *input_fp;
  if (!file_exists(cli_args.input_file_name)) {
    fprintf(stderr, "Input file %s not found. Exiting\n",
            cli_args.input_file_name);
    exit(1);
  } else {
    input_fp = fopen(cli_args.input_file_name, "rb");
    if (!input_fp) {
      perror("fopen");
      free(buf);
      exit(1);
    }
  }

  if (verbose_lvl >= __V4) {
    printf("Success.\n");
  }
  if (verbose_lvl >= __V4) {
    printf("Checking output file does not exist ... ");
  }

  if (file_exists(cli_args.output_file_name)) {
    fprintf(stderr, "Output file %s already exists. Exiting\n",
            cli_args.output_file_name);
    exit(1);
  }

  if (verbose_lvl >= __V4) {
    printf("Success.\n");
  }

  if (cli_args.compress_flag == 1) {
    compress_input_file(input_fp, cli_args.output_file_name, verbose_lvl);
  } else if (cli_args.decompress_flag == 1) {
    decompress_file(input_fp, cli_args.output_file_name, verbose_lvl);
  }

  return 1;
}


/*******************************************************************************
print_help
This function prints the initial menu with all instructions on how to use this
program.
inputs:
- none
outputs:
- none
*******************************************************************************/
void print_help() {
  printf("Usage: huff [OPTIONS] [-c|-d] FILE [OUTPUT]\n");
  printf("Compress or decompress FILE via the Huff algorithm.\n");
  printf("If no options are passed, the FILE will be compressed. If no ");
  printf("OUTPUT is provided, the input filename will be used with .huf");
  printf("f added.\n");
  printf("\n");
  printf("Options:\n");
  printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-h, --help", HELP_DESCRIPTION_WIDTH,
         "Display this help and exit.");
  printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-v, --verbose V",
         HELP_DESCRIPTION_WIDTH,
         "Set output verbosity. Valid values are 1, 2, 3, DEBUG.");
  printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-c, --compress",
         HELP_DESCRIPTION_WIDTH, "Compress FILE.");
  printf("  %-*s%-*s\n", HELP_ARG_WIDTH, "-d, --decompress",
         HELP_DESCRIPTION_WIDTH, "decompress FILE.");
}


/*******************************************************************************
file_exists
This function checks whether the users given file exists
inputs:
- filename
outputs:
- 1 if exists
- 0 if not found
*******************************************************************************/
int file_exists(char file_name[]) {
  FILE *fp;
  if ((fp = fopen(file_name, "rb"))) {
    fclose(fp);
    return 1;
  } else {
    return 0;
  }
}


/*******************************************************************************
parse_cli_args
This function processes the flags given to the initial menu to determine what
 operations the user wishes to complete
inputs:
- the argument char
- array of arguments
outputs:
- none
*******************************************************************************/
struct cli_args_t parse_cli_args(int argc, char *argv[]) {
  if (argc < 2) {
    print_help();
    exit(1);
  }

  unsigned short compress_flag = 0, decompress_flag = 0, verbose_flag_value = 0,
                 found_flags = 0;

  int i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_help();
      exit(0);
    } else if (strcmp(argv[i], "-v") == 0 ||
               strcmp(argv[i], "--verbose") == 0) {
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
    } else if (strcmp(argv[i], "-c") == 0 ||
               strcmp(argv[i], "--compress") == 0) {
      compress_flag = 1;
      found_flags++;
    } else if (strcmp(argv[i], "-d") == 0 ||
               strcmp(argv[i], "--decompress") == 0) {
      decompress_flag = 1;
      found_flags++;
    }
  }
  if (compress_flag == 1 && decompress_flag == 1) {
    fprintf(stderr, "Cannot run with both compress/decompress set. Exiting\n");
    exit(1);
  }
  struct cli_args_t cli_args = {.compress_flag = compress_flag,
                                .decompress_flag = decompress_flag,
                                .found_flags = found_flags,
                                .verbose_flag_value = verbose_flag_value};

  if (verbose_flag_value >= __V2) {
    printf("Verbosity: %d\n", verbose_flag_value);
  }
  if (verbose_flag_value >= __V4) {
    printf("Found flags: %d\n", found_flags);
  }
  if (verbose_flag_value >= __V4) {
    printf("Arg count: %d\n", argc);
  }

  if (compress_flag == 0 && decompress_flag == 0) {
    if (verbose_flag_value >= __V4) {
      printf("Both compress/decompress flags unset. Defaulting to compress.\n");
    }
    compress_flag = 1;
  }

  char input_file_name[MAX_FILENAME];
  char output_file_name[MAX_FILENAME];

  int filename_count = argc - 1 - found_flags;
  if (verbose_flag_value >= __V4) {
    printf("Filenames found: %d\n", filename_count);
  }
  if (filename_count < 1) {
    fprintf(stderr, "FILE not provided. Exiting\n");
    exit(1);
  } else if (filename_count > 2) {
    fprintf(stderr, "More than 2 filenames provided. Only FILE and [OUTPUT] "
                    "are allowed. Exiting\n");
    exit(1);
  } else if (filename_count == 1) {
    strcpy(input_file_name, argv[argc - 1]);
    /* Set default output filename if not set in args */
    strcpy(output_file_name, input_file_name);
    if (compress_flag == 1) {
      strcat(output_file_name, ".huff");
    } else if (decompress_flag == 1) {
      strcat(output_file_name, ".huffout");
    }
  } else {
    strcpy(input_file_name, argv[argc - 2]);
    strcpy(output_file_name, argv[argc - 1]);
  }
  strcpy(cli_args.input_file_name, input_file_name);
  strcpy(cli_args.output_file_name, output_file_name);

  return cli_args;
}
