#ifndef COMPRESS_H
#define COMPRESS_H
#include "header.h"

// FUNCOES DE FILA

PRIORITY_QUEUE* create_priority_queue();

void fill_priority_queue(lli* frequence, PRIORITY_QUEUE* queue);

void enqueue(NODE* new_node, PRIORITY_QUEUE* queue);

void print_queue(NODE* current);

// FUNCOES NODE

NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right);

// FUNCOES ARRAY DE FREQUENCIA

void create_freq_array(lli* frequence);

void fill_freq_array(lli* frequence);

// FUNCOES ARVORE DE HUFFMAN

NODE* create_huff_tree(PRIORITY_QUEUE* queue);

void print_tree(NODE* current);

#endif