#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE                        (16 * 1024 * 1024)

#define HEADER_SIZE_B                   14
#define HEADER_OFFSET                   0x00
#define HEADER_SIGNATURE                0x00
#define HEADER_FILESIZE                 0x02
#define HEADER_RESERVED                 0x06
#define HEADER_DATA                     0x0A

#define INFO_HEADER_SIZE_B              40
#define INFO_HEADER_OFFSET              0x0E
#define INFO_HEADER_SIZE                0x0E
#define INFO_HEADER_WIDTH               0x12
#define INFO_HEADER_HEIGHT              0x16
#define INFO_HEADER_PLANES              0x1A
#define INFO_HEADER_BPS                 0x1C
#define INFO_HEADER_COMPRESSION         0x1E
#define INFO_HEADER_IMAGE_SIZE          0x22
#define INFO_HEADER_XPPM                0x26
#define INFO_HEADER_YPPM                0x2A
#define INFO_HEADER_COLORS_USED         0x2E
#define INFO_HEADER_IMPORTANT_COLORS    0x32

typedef unsigned char byte;

union int_data {
  unsigned char bytes[4];
  unsigned int value;
};

union short_data {
  unsigned char bytes[2];
  unsigned short value;
};

struct  bmp_header_t {
    byte signature[2];
    union int_data filesize;
    unsigned char reserved[4];
    union int_data offset;
} typedef bmp_header;

union bmp_header_data {
  byte data[HEADER_SIZE_B];
  bmp_header header;
};

struct bmp_info_header_t {
    union int_data size;
    union int_data width;
    union int_data height;
    union short_data planes;
    union short_data bits_per_pixel;
    union int_data compression;
    union int_data image_size;
    union int_data x_pixels_per_m;
    union int_data y_pixels_per_m;
    union int_data colours_used;
    union int_data important_colours;
} typedef bmp_info_header;

struct bmp_file_t {
    bmp_header header;
    bmp_info_header info_header;
    unsigned char data[786486];
} typedef bmp_file;


unsigned int byteArrayToInt(unsigned char arr[]);
// bmp_header parse_header(const char* buf);
bmp_header parse_header(FILE* fp);
bmp_info_header parse_info_header(FILE *fp);
bmp_file parse_bmp(FILE* fp);

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: %s <png file>\n", argv[0]);
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
	// int size = fread(buf, 1, MAX_SIZE, f);


    bmp_file bmp;


    printf("%d sizeof\n", sizeof(bmp.header));
    printf("%d sizeof\n", sizeof(union int_data));
    unsigned char* test[14];

    // union bmp_header_data* header;

    // union bmp_header_data header;
    // header = (union bmp_header_data *) malloc(sizeof(union bmp_header_data));

    // // size_t readcount = fread(&header, sizeof(header), 1, f);
    // int i;
    // for (i = 0; i < HEADER_SIZE && (header->data[i] = getc(f)) != EOF; i++) {
    //     printf("%x ", header->data[i]);
    // }
    // ;
    // printf("%d", readcount);
    // if (readcount != 1) {
    //     fprintf(stderr, "Error reading file.\n");
    // }
    // printf("test: %s", test);
    // int i = 0;
    // for (int i = 0; i < 14; i++) {
    //     printf("%X ", test[i]);
    // }
    // printf("\n");
    // for (i = 0; i < HEADER_SIZE; i++) {
    //     printf("%d %X\n", i, header->data[i]);
    // }
    // printf("%X %X %X %X \n", header->header.signature[0], header->header.signature[1], header->header.signature[2], header->header.signature[3]);
    // printf("%X %X %X %X \n", header->header.filesize.bytes[0], header->header.filesize.bytes[1], header->header.filesize.bytes[2], header->header.filesize.bytes[3]);

    // print_header(header->header);

    // bmp.header = parse_header(buf);
    // bmp.header = parse_header(f);
    bmp = parse_bmp(f);
    print_header(bmp.header);
    // bmp.info_header = parse_info_header(f);
    print_info_header(bmp.info_header);

    return 0;
}

// bmp_header parse_header(const char* buf) {
//     bmp_header header;

//     memcpy(header.signature, buf, 2);
//     header.signature[2] = '\0';

//     memcpy(header.filesize.bytes, buf + 0x02, 4);
//     memcpy(header.reserved, buf + 0x06, 4);
//     memcpy(header.data.bytes, buf + 0x0A, 4);

//     return header;
// }

bmp_header parse_header(FILE *fp) {

    bmp_header* header = (bmp_header*) malloc(sizeof(bmp_header));

    fseek(fp, HEADER_OFFSET, SEEK_SET);

    int count;
    int seek_status;

    count = fread(header->signature, sizeof(header->signature), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid file signature\n");
        exit(6);
    }
    count = fread(header->filesize.bytes, sizeof(header->filesize), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid filesize \n");
        exit(6);
    }
    count = fread(header->reserved, sizeof(header->reserved), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid reserved \n");
        exit(6);
    }
    count = fread(header->offset.bytes, sizeof(header->offset), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid reserved \n");
        exit(6);
    }

    return *header;
}

bmp_info_header parse_info_header(FILE *fp) {
    bmp_info_header* info_header = (bmp_info_header*) malloc(sizeof(bmp_info_header));

    fseek(fp, INFO_HEADER_OFFSET, SEEK_SET);

    int count;
    int seek_status;

    count = fread(info_header->size.bytes, sizeof(info_header->size), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->width.bytes, sizeof(info_header->width), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->height.bytes, sizeof(info_header->height), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->planes.bytes, sizeof(info_header->planes), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->bits_per_pixel.bytes, sizeof(info_header->bits_per_pixel), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->compression.bytes, sizeof(info_header->compression), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->image_size.bytes, sizeof(info_header->image_size), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->x_pixels_per_m.bytes, sizeof(info_header->x_pixels_per_m), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->y_pixels_per_m.bytes, sizeof(info_header->y_pixels_per_m), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }

    count = fread(info_header->colours_used.bytes, sizeof(info_header->colours_used), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }
    count = fread(info_header->important_colours.bytes, sizeof(info_header->important_colours), 1, fp);
    if (count != 1) {
        fprintf(stderr, "Invalid size \n");
        exit(6);
    }
    
    // unsigned char size[4];
    // memcpy(size, buf, 4);
    // header.size = byteArrayToInt(size);

    // unsigned char width[4];
    // memcpy(width, buf + 4, 4);
    // header.width = byteArrayToInt(width);

    // unsigned char height[4];
    // memcpy(height, buf + 8, 4);
    // header.height = byteArrayToInt(height);

    // unsigned char bpp[2];
    // memcpy(bpp, buf + 12, 2);
    // printf("%X %X\n", bpp[0], bpp[1]);
    // printf("!!!!!!!!!!!!");
    // header.bits_per_pixel = (int));

    return *info_header;
}

void parse_bmp_data(FILE* fp, const unsigned int data_offset) {
    fseek(fp, data_offset, SEEK_SET);
    unsigned char data[1000];
    int i;
    printf("Pixel data\n");
    for (i = 0; i < 100 && (data[i] = getc(fp)) != EOF; i++) {
        printf("%03d ", data[i]);
        if (i % (4*3) == 0) {
            printf("\n");
            
        }
    }
    printf("\n");
    printf("\033[0;m");
}

bmp_file parse_bmp(FILE* fp) {
    bmp_file* bmp = (bmp_file*) malloc(sizeof(bmp_file));
    bmp->header = parse_header(fp);
    bmp->info_header = parse_info_header(fp);
    // bmp->data = parse_bmp_data(fp, bmp->header.offset.value);
    parse_bmp_data(fp, bmp->header.offset.value);

    return *bmp;
}

void print_header(bmp_header header) {
    printf("Signature:\t%.2s\n", header.signature);
    printf("Filesize: \t%d bytes\n", header.filesize.value);
    printf("Reserved: \t%s\n", header.reserved);
    printf("Data Offset: \t%d\n", header.offset.value);
}

void print_info_header(bmp_info_header header) {
    printf("Size:\t%d\n", header.size.value);
    printf("Width:\t%d\n", header.width.value);
    printf("Height:\t%d\n", header.height.value);
    printf("Planes:\t%u\n", header.planes.value);
    printf("Bits Per Pixel:\t%d\n", header.bits_per_pixel);
    printf("Compression:\t%u\n", header.compression.value);
    printf("Image size:\t%u\n", header.image_size.value);
    printf("x pixels per metre:\t%u\n", header.x_pixels_per_m.value);
    printf("y pixels per metre:\t%u\n", header.y_pixels_per_m.value);
    printf("colours used:\t%u\n", header.colours_used.value);
    printf("important colours:\t%u\n", header.important_colours.value);

}

unsigned int byteArrayToInt(unsigned char arr[]) {
    unsigned int result = 0;
    
    result |= arr[0];  // Least significant byte
    result |= arr[1] << 8;
    result |= arr[2] << 16;
    result |= arr[3] << 24;  // Most significant byte
    
    return result;
}
