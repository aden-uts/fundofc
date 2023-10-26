#ifndef HUFFMANUTILS_H
#define HUFFMANUTILS_H

#define MAX_TREE_HT 1024

struct huffman_code_t {
  unsigned char code[MAX_TREE_HT];
  char item;
  short len;
};

#endif