#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "compress.h"

// FUNCOES FILA DE PRIORIDADE

PRIORITY_QUEUE* create_priority_queue() {
    PRIORITY_QUEUE* queue = (PRIORITY_QUEUE*) malloc(sizeof(PRIORITY_QUEUE));
    queue->head = NULL;
    queue->size = 0;

    return queue;
};

void fill_priority_queue(lli* frequence, PRIORITY_QUEUE* queue) {
    for (int i = 0; i < 256; i++) {
        if (frequence[i] != 0) {
            NODE* new_node = create_node(frequence[i], i, NULL, NULL);
            enqueue(new_node, queue);
        }
    }
}

void enqueue(NODE* new_node, PRIORITY_QUEUE* queue) {
    if (queue->head == NULL || new_node->priority <= queue->head->priority) {
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        NODE* current = queue->head;

        while (current->next != NULL && current->next->priority < new_node->priority) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
    queue->size += 1;
}

NODE* dequeue(PRIORITY_QUEUE* queue) {
    if (queue->head == NULL) {
        printf("QUEUE UNDERFLOW\n");
        return NULL;
    }
    queue->size -= 1;
    NODE* first_node = queue->head;
    queue->head = queue->head->next;
    first_node->next = NULL;
    
    return first_node;
}

void print_queue(NODE* current) {
    while (current != NULL) {
        printf("%c %lld\n", current->caracter, current->priority);
        current = current->next;
    }
    printf("\n");
}

// FUNCOES NODE 

NODE* create_node(lli priority, uchar caracter, NODE* left, NODE* right) {

    NODE* new_node = (NODE*) malloc(sizeof(NODE));
    new_node->priority = priority;
    new_node->caracter = caracter; // representacao do caracter na ASCII.
    new_node->left = left;
    new_node->right = right;
    new_node->next = NULL;

    return new_node;
}

// FUNCOES ARRAY DE FREQUENCIA

void create_freq_array(lli* frequence) {
    for (int i = 0; i < 256; i++) {
        frequence[i] = 0;
    }
}

void fill_freq_array(lli* frequence) {
    FILE* file = fopen("/home/alunoic/Downloads/WhatsApp Video 2020-01-21 at 17.19.35 (1).mp4", "rb");
    uchar caracter;

    while (fscanf(file, "%c", &caracter) != EOF) {
        frequence[caracter] += 1;
    }
    fclose(file);
}

// FUNCOES ARVORE DE HUFFMAN

NODE* create_huff_tree(PRIORITY_QUEUE* queue) {
    while (queue->size > 1) {
        NODE* left_node = dequeue(queue);
        NODE* right_node = dequeue(queue);
        
        lli sum = left_node->priority + right_node->priority;
        NODE* tree_node = create_node(sum, '*', left_node, right_node);

        enqueue(tree_node, queue);
    }
    //print_queue(queue->head);
    return queue->head;
}

void print_tree(NODE* current) {
    if (current == NULL) {
        return;
    }
    printf("caracter: %c freq: %lld\n", current->caracter, current->priority);
    print_tree(current->left);
    print_tree(current->right);
}

// FUNCOES DE HASH

HASH* create_hash() {
    HASH* new_hash = (HASH*) malloc(sizeof(HASH));
    for (int i = 0; i < 256; i++) {
        new_hash->array[i] = NULL;
    }
    return new_hash;
}

// NOVA CODIFICACAO

void new_codification(HASH* hash, NODE* tree, int size, ushort byte) {
    if (tree->left == NULL && tree->right == NULL) { // cheguei numa folha.
        //printf("%u\n", byte);
        printf("CARACTER EM BINÁRIO: %c %d\n", tree->caracter, transform_binary(byte, -1));
        return;
    }
    byte <<= 1;
    new_codification(hash, tree->left, size + 1, byte);
    byte++; // poe 1 no primeiro bit do byte.
    new_codification(hash, tree->right, size + 1, byte);
}

int transform_binary(int decimal_number, int res) {
    if (!decimal_number) {
        return 0;
    }
    res = decimal_number % 2;
    return transform_binary(decimal_number / 2, res) * 10 + res;
}

int main() {
    PRIORITY_QUEUE* queue = create_priority_queue();
    lli frequence[256]; // ignoramos o indice zero.

    create_freq_array(frequence);
    fill_freq_array(frequence);
    // for (int i = 0; i < 256; i++) {
    //     printf("caracter: %c freq: %lld\n", i, frequence[i]);
    // }

    fill_priority_queue(frequence, queue);

    NODE* tree = create_huff_tree(queue);
    //print_tree(tree);

    HASH* hash = create_hash();
    new_codification(hash, tree, 0, 0); 

    return 0;
}