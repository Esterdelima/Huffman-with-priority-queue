#ifndef DESCOMPRESS_H
#define DESCOMPRESS_H
#include "/home/ester/Documentos/p2/Huffman-with-priority-queue/huffman/header/header.h"

// FUNCAO DE CRIAR UM NO

NODE_DESCOM* create_node(uchar caracter, NODE_DESCOM* left, NODE_DESCOM* right);

// FUNCAO QUE VERIFICA SE EH UMA FOLHA

bool is_leaf(NODE_DESCOM* current);

// FUNCAO DE MONTAR A ARVORE

NODE_DESCOM* construct_tree(uchar *str, int *i);

// FUNCAO DE DESCOMPRESSAO

void descompact();

// SETAR OS BITS

bool is_bit_i_set(ushort byte, int i);

#endif