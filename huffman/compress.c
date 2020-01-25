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
    FILE* file = fopen("file.txt", "rb");
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
    if (current == NULL) return;

    printf("caracter: %c freq: %lld\n", current->caracter, current->priority);
    print_tree(current->left);
    print_tree(current->right);
}

void get_pre_order_tree(NODE* tree, FILE* file) {
    if (tree != NULL) {
        if (tree->left == NULL && tree->right == NULL) {
            if (tree->caracter == '*' || tree->caracter == '\\') {
                fputc('\\', file);
            }
        }
        fputc(tree->caracter, file);
        get_pre_order_tree(tree->left, file);
        get_pre_order_tree(tree->right, file);
    }
}

// CALCULAR O TAMANHO DA ÁRVORE

ushort get_size_tree(NODE* tree) {
    if (tree->left == NULL && tree->right == NULL) {
        if (tree->caracter == '*' || tree->caracter == '\\') {
            return 2;
        } else {
            return 1;
        }
    }
    return 1 + get_size_tree(tree->left) + get_size_tree(tree->right);
}

// FUNCOES DE HASH

HASH* create_hash() {
    HASH* new_hash = (HASH*) malloc(sizeof(HASH));
    for (int i = 0; i < 256; i++) {
        new_hash->array[i] = NULL;
    }
    return new_hash;
}

void print_hash(HASH* hash) {
    for (int i = 0; i < 256; i++) {
        if (hash->array[i] != NULL) {
            ushort byte = hash->array[i]->code;
            int size = hash->array[i]->size;

            printf("caracter: %c size: %d code: %d\n", i, size, byte);
        
            // for (int j = 7; j >= 0; j--) {
            //     printf("%d", is_bit_i_set(byte, j));
            // }
            //printf("\n");
        }
    }
}

// NOVA CODIFICACAO

bool is_bit_i_set(ushort byte, int i) {
    ushort temp = 1 << i;
    return temp & byte;
}

void new_codification(HASH* hash, NODE* tree, int size, ushort byte) {
    if (tree->left == NULL && tree->right == NULL) { // cheguei numa folha.

        int index = tree->caracter; // pego a representação inteira do caracter(posicao na hash).
        ELEMENT* element = (ELEMENT*) malloc(sizeof(ELEMENT)); // no vazio.

        element->size = size;
        element->code = byte;
        hash->array[index] = element;
        return;
    }

    byte <<= 1;
    new_codification(hash, tree->left, size + 1, byte);

    byte++; // poe 1 no primeiro bit do byte.
    new_codification(hash, tree->right, size + 1, byte);
}

// CALCULAR LIXO

uchar get_trash(HASH* hash, lli* frequence) {
    lli sum = 0; 

    for (int i = 0; i < 256; i++) {
        if (hash->array[i] != NULL) {
            sum += hash->array[i]->size * frequence[i];
        }
    }
    uchar trash = 0; // tamanho do lixo
    lli bits = 0; // quantia de bits usados
    lli bytes = sum / 8; // quantia de bytes alocados

    if (sum % 8 != 0) {
        bytes++;
        bits = bytes * 8; // quantidade de bits alocados(NAO É O TOTAL DE BITS USADOS TA!)
        trash = bits - sum;
    }
}

// COMPACTAR ARQUIVO TESTE

void compact_file(FILE* arq_compact, HASH* hash) {
    FILE* read_file = fopen("file.txt", "rb");
    uchar caracter;
    int i = 8;
    int size = 0;
    int qsb = 0;
    int to_complete = 0;
    uchar byte = 0;
    ushort code = 0;

    while (fscanf(read_file, "%c", &caracter) != EOF) {
        code = hash->array[caracter]->code;
        size = hash->array[caracter]->size;

        byte |= code;
        qsb = i - size;

        if (qsb > 0) {
            byte <<= qsb;
            i -= size;
        } 
        else if (qsb < 0) {
            fprintf(arq_compact,"%c", byte);

            to_complete = abs(qsb);
            i = 8;
            byte = 0;
            
            byte |= code;
            qsb = 8 - to_complete;
            byte <<= qsb;
            i -= to_complete;
        }
    }
    fclose(read_file);
}


// PRINTAR O CABEÇALHO NO ARQUIVO COMPACTADO

int main() {
    PRIORITY_QUEUE* queue = create_priority_queue();
    lli frequence[256]; // ignoramos o indice zero.

    create_freq_array(frequence);
    fill_freq_array(frequence);
   
    fill_priority_queue(frequence, queue);

    NODE* tree = create_huff_tree(queue);
    //print_tree(tree);

    HASH* hash = create_hash();
    new_codification(hash, tree, 0, 0); 
    
    print_hash(hash);

    uchar trash = get_trash(hash, frequence); // trash so ocupa 3 bits ou seja so alocamos 1 byte para guardar o lixo.
    ushort size_tree = get_size_tree(tree);
    
    //printf("%d\n", trash);
    
    ushort header = 0;
    header |= trash; 
    header <<= 13; 
    header = header | size_tree;
    
    // printf("trash %u\n", trash);
    // printf("header %u\n", header);
    
    FILE* file = fopen("compacted.txt.huff", "wb");
    uchar byte_1 = header >> 8;
    uchar byte_2 = header; // pega so o primeiro byte da header(que tem 2 bytes)

    // printf("byte 1: %d\n", byte_1);
    // printf("byte 2: %d\n", byte_2);

    fputc(byte_1, file);
    fputc(byte_2, file);

    get_pre_order_tree(tree, file);

    compact_file(file, hash);
    fclose(file);

    return 0;
}