#include "HuffmanCoding.h"

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
	parse_file(f);
}

void parse_file(FILE *fp) {
	int c;
	char chars[MAX_SYMBOLS];
	int char_counts[MAX_SYMBOLS];
	int element_count = 0;
	while ((c = fgetc(fp)) != EOF) {
		// printf("%c ", c);
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
}