#ifndef HEADER_H
#define HEADER_H

#define uchar unsigned char
#define lli long long int

typedef struct node NODE;
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

#endif
