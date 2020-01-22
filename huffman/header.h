#ifndef HEADER_H
#define HEADER_H

#define uchar unsigned char
#define ushort unsigned short
#define lli long long int

typedef struct node NODE;
typedef struct element ELEMENT;
typedef struct hash_table HASH;
typedef struct priority_queue PRIORITY_QUEUE;

struct node {
    lli priority;
    uchar caracter;
    NODE* next;
    NODE* left;
    NODE* right;
};

struct priority_queue {
    lli size;
    NODE* head;
};

struct element {
    int size;
    uchar code;
};

struct hash_table {
    ELEMENT* array[256];
};

#endif
