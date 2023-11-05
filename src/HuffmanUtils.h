/*******************************************************************************
Author: Aden Northcote, Emery Strasser, Jonathon Ngo and Jordan Stevens
Date: 05/11/2023
Purpose: This program compresses any file using Huffman Encoding with high entropy.
*******************************************************************************/


/*******************************************************************************
Include header files and function prototypes
*******************************************************************************/
#ifndef HUFFMANUTILS_H
#define HUFFMANUTILS_H

#define MAX_TREE_HT 10240

struct huffman_code_t {
  unsigned char code[MAX_TREE_HT];
  unsigned char item;
  short len;
};

#endif