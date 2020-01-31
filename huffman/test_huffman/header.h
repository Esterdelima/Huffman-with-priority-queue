#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define uchar unsigned char
#define ushort unsigned short
#define lli long long int

typedef struct node NODE;
typedef struct node_tree_descompress NODE_DESCOM;
typedef struct element ELEMENT;
typedef struct hash_table HASH;
typedef struct hash_node HASH_NODE;
typedef struct priority_queue PRIORITY_QUEUE;

// NO DA ARVORE DA DESCOMPACTACAO

struct node_tree_descompress {
    void* caracter; // uchar
    NODE_DESCOM* left; 
    NODE_DESCOM* right; 
};

// NO DE FILA E DE ARVORE

struct node {
    lli priority;
    void* caracter; // uchar
    NODE* next; 
    NODE* left; 
    NODE* right; 
};

// FILA DE PRIORIDADE

struct priority_queue {
    lli size;
    NODE* head; 
};

// NO DE HASH

struct element {
    int size; 
    void* code; // ushort
};

// HASH

struct hash_table {
    void* array[256]; // ELEMENT
};

#endif
