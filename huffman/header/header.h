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
    NODE* left; // NODE_DESCOM
    NODE* right; // NODE_DESCOM
};

// NO DE FILA E DE ARVORE

struct node {
    lli priority;
    void* caracter; // uchar
    NODE* next; // NODE
    NODE* left; // NODE
    NODE* right; // NODE
};

// FILA DE PRIORIDADE

struct priority_queue {
    lli size;
    NODE* head; // NODE
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
