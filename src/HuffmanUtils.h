#ifndef HUFFMANUTILS_H
#define HUFFMANUTILS_H

#define MAX_TREE_HT 10240

struct huffman_code_t {
  unsigned char code[MAX_TREE_HT];
  unsigned char item;
  short len;
};

#endif