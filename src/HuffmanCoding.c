#include <stdio.h>
#include <stdlib.h>

#include "HuffmanCoding.h"

struct min_huffman_node_t {
  char item;
  unsigned freq;
  struct min_huffman_node_t *left, *right;
};

struct min_heap_t {
  unsigned size;
  unsigned capacity;
  struct min_huffman_node_t **array;
};

struct min_huffman_node_t *newNode(char item, unsigned freq) {
  struct min_huffman_node_t *temp = (struct min_huffman_node_t *)malloc(sizeof(struct min_huffman_node_t));

  temp->left = temp->right = NULL;
  temp->item = item;
  temp->freq = freq;

  return temp;
}

struct min_heap_t *createMinH(unsigned capacity) {
  struct min_heap_t *min_heap_t = (struct min_heap_t *)malloc(sizeof(struct min_heap_t));

  min_heap_t->size = 0;

  min_heap_t->capacity = capacity;

  min_heap_t->array = (struct min_huffman_node_t **)malloc(min_heap_t->capacity * sizeof(struct min_huffman_node_t *));
  return min_heap_t;
}

void swap_node(struct min_huffman_node_t **a, struct min_huffman_node_t **b) {
  struct min_huffman_node_t *t = *a;
  *a = *b;
  *b = t;
}


void min_heapify(struct min_heap_t *min_heap_t, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < min_heap_t->size && min_heap_t->array[left]->freq < min_heap_t->array[smallest]->freq)
    smallest = left;

  if (right < min_heap_t->size && min_heap_t->array[right]->freq < min_heap_t->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swap_node(&min_heap_t->array[smallest], &min_heap_t->array[idx]);
    min_heapify(min_heap_t, smallest);
  }
}


int check_size_one(struct min_heap_t *min_heap_t) {
  return (min_heap_t->size == 1);
}


struct min_huffman_node_t *extractMin(struct min_heap_t *min_heap_t) {
  struct min_huffman_node_t *temp = min_heap_t->array[0];
  min_heap_t->array[0] = min_heap_t->array[min_heap_t->size - 1];

  --min_heap_t->size;
  min_heapify(min_heap_t, 0);

  return temp;
}


void insertmin_heap_t(struct min_heap_t *min_heap_t, struct min_huffman_node_t *min_heap_tNode) {
  ++min_heap_t->size;
  int i = min_heap_t->size - 1;

  while (i && min_heap_tNode->freq < min_heap_t->array[(i - 1) / 2]->freq) {
    min_heap_t->array[i] = min_heap_t->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  min_heap_t->array[i] = min_heap_tNode;
}

void buildmin_heap_t(struct min_heap_t *min_heap_t) {
  int n = min_heap_t->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i)
    min_heapify(min_heap_t, i);
}

int is_leaf(struct min_huffman_node_t *root) {
  return !(root->left) && !(root->right);
}

struct min_heap_t *createAndBuildmin_heap_t(char item[], int freq[], int size) {
  struct min_heap_t *min_heap_t = createMinH(size);
  int i;
  for (i = 0; i < size; ++i)
    min_heap_t->array[i] = newNode(item[i], freq[i]);

  min_heap_t->size = size;
  buildmin_heap_t(min_heap_t);

  return min_heap_t;
}

struct min_huffman_node_t *buildHuffmanTree(char item[], int freq[], int size) {
  struct min_huffman_node_t *left, *right, *top;
  struct min_heap_t *min_heap_t = createAndBuildmin_heap_t(item, freq, size);

  while (!check_size_one(min_heap_t)) {
    left = extractMin(min_heap_t);
    right = extractMin(min_heap_t);

    top = newNode('$', left->freq + right->freq);

    top->left = left;
    top->right = right;

    insertmin_heap_t(min_heap_t, top);
  }
  return extractMin(min_heap_t);
}

void print_h_codes(struct min_huffman_node_t *root, int arr[], int top) {
  if (root->left) {
    arr[top] = 0;
    print_h_codes(root->left, arr, top + 1);
  }
  if (root->right) {
    arr[top] = 1;
    print_h_codes(root->right, arr, top + 1);
  }
  if (is_leaf(root)) {
    printf("  %c\t| ", (unsigned char) root->item);
    print_array(arr, top);
  }
}

void huffman_codes(char item[], int freq[], int size) {
  struct min_huffman_node_t *root = buildHuffmanTree(item, freq, size);
  
  int arr[MAX_TREE_HT], top = 0;

  print_h_codes(root, arr, top);
  if (root->left) {
    printf("YES\n");
  }

}


void print_array(int arr[], int n) {
  int i;
  for (i = 0; i < n; ++i)
    printf("%d", arr[i]);

  printf("\n");
}

