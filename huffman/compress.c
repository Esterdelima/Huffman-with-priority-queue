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
    FILE* file = fopen("meg.mp4", "rb");
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

// executar um caso pequeno para entender a funcao, ela eh facil, so entender com calma :D

void compact_file(FILE* arq_compact, HASH* hash, uchar trash_size) {
    FILE* read_file = fopen("meg.mp4", "rb"); // abro e leio o arquivo.
    int size;
    int quant_byte; // verifica se o byte ja esta completo com 8 bits.
    ushort code; // salva a codificaçao do caracter lido (salvo na hash).
    uchar caracter; // pega a leitura do caracter.
    uchar compress_byte; // byte com a compressao.
    
    quant_byte = 0, compress_byte = 0, size = 0; // inicializo tudo com zero.

    while (fscanf(read_file, "%c", &caracter) != EOF) { // lemos o arquivo ate o EOF.

        code = hash->array[caracter]->code; // pega a nova codificaçao (na hash) do caracter lido.
        size = hash->array[caracter]->size - 1; // pega o tamanho dessa codificaçao.

        for (size; size >= 0; size--) { 
            // codificação com mais de um byte
            if (is_bit_i_set(code, size)) {
                compress_byte += 1; // poe 1 no bit mais a direita.
            }
            
            quant_byte += 1; // soma 1 a quantia de bits.
            
            if (quant_byte == 8) { // renovar o byte.

                fprintf(arq_compact, "%c", compress_byte); // printa o byte no arquivo.
                quant_byte = 0; // zera a quantia de bits.
                compress_byte = 0; // zera o byte (0000 0000).

            } 
            compress_byte <<= 1; // shifta para frente o bit mais a dieita para nao perde-lo.
        }
    }
    compress_byte >>= 1; // ajusta o ultimo byte, pois ele contem o ultimo bit errado. (Executar um pequeno caso na mao para ver).
    compress_byte <<= trash_size; //  shiftei o tamanho do lixo apos o ultimo bit do ultimo byte do arquivo codificado.

    fprintf(arq_compact, "%c", compress_byte); // printa o ultimo byte ajustado no arquivo.
    fclose(read_file);
}

// PRINTAR O CABEÇALHO NO ARQUIVO COMPACTADO

// PARTE DA DESCOMPACTCAO

NODE* construct_tree(uchar *str, int *i) {
   if (str[*i] != '*') { // no folha
        if (str[*i] == '\\') *i += 1;
        NODE* leaf = create_node(0, str[*i], NULL, NULL);
        return leaf;
   }
   else {
        NODE* tree = create_node(0, '*', NULL, NULL);
        *i += 1;
        tree->left = construct_tree(str, i);
        *i += 1;
        tree->right = construct_tree(str, i);
        return tree;
   }
}

void descompact() {
    FILE* file = fopen("ester.mp4.huff", "rb"); // arquivo de escrita compactada
    uchar byte_1, byte_2;
    
    fscanf(file, "%c", &byte_1); // pego o primeiro byte do arquivo compactado (que contem os 3 bits de lixo).
    fscanf(file, "%c", &byte_2); // pego o segundo byte do arquivo compactado (com parte do tamanho da arvore).
    
    // PEGA O LIXO E O TAMANHO DA ARVORE
    int i = 0;
    ushort size_tree = 0; // zera os 16 bits com o tamanho da arvore.
    ushort trash = byte_1 >> 5; // elimina os 5 bits do tamanho da arvore.
    size_tree = byte_1 << 11; // anda 8 bits pra passar pro byte + a esquerda, +3 pra sumir c os 3 bits de lixo.
    size_tree >>= 3; // os 3 bits de lixo voltam zerados.
    size_tree |= byte_2; // recebe o restante do tamanho da arvore no 2 byte.

    uchar str[size_tree];

    for (i = 0; i < size_tree; i++) { // le a arvore em pre ordem no arquivo compacatado.
        fscanf(file, "%c", &str[i]);
    }
    
    i = 0;
    NODE* tree = construct_tree(str, &i);

    lli cont_bytes = 0;
    uchar byte = 0;

    while (fscanf(file, "%c", &byte) != EOF) {
        cont_bytes++;
    }

    fseek(file, 2 + size_tree, SEEK_SET); // função de percorrer o arquivo a partir de um ponto especifico.

    int limit = 0;
    NODE* current = tree;
    FILE* descompacted = fopen("descompacted.mp4", "wb");

    for (cont_bytes; cont_bytes > 0; cont_bytes--) { // conta os bytes percorridos.
        fscanf(file, "%c", &byte);
        
        if (cont_bytes == 1) {
            limit = trash; // ultimo byte com o lixo (so verificamos esse byte ate o seu lixo).
        }

        for (i = 7; i >= limit; i--) {
            if (is_bit_i_set(byte, i)) {
                current = current->right; // 1-> direita (bit setado com 1)
            } else {
                current = current->left; // 0 -> esquerda (bit setado com 0)
            }
    
            if (current->left == NULL && current->right == NULL) { // folha, hora de printar o caracter no novo arquivo :D.
                fprintf(descompacted, "%c", current->caracter);
                current = tree; // ponteiro pro inicio da arvore.
            }
        }
    }
    fclose(file);
    fclose(descompacted);
}

int main() {
    PRIORITY_QUEUE* queue = create_priority_queue();
    lli frequence[256]; // ignoramos o indice zero.

    create_freq_array(frequence);
    fill_freq_array(frequence);
   
    fill_priority_queue(frequence, queue);

    NODE* tree = create_huff_tree(queue);

    HASH* hash = create_hash();
    new_codification(hash, tree, 0, 0); 
    
    //print_hash(hash);
    //print_tree(tree);

    uchar trash = get_trash(hash, frequence); // trash so ocupa 3 bits ou seja so alocamos 1 byte para guardar o lixo.
    ushort size_tree = get_size_tree(tree);
    
    //printf("%d\n", trash);
    
    ushort header = 0;
    header |= trash; 
    header <<= 13; 
    header = header | size_tree;
    
    // printf("trash %u\n", trash);
    // printf("header %u\n", header);
    
    FILE* file = fopen("ester.mp4.huff", "wb"); // arquivo de escrita compactada
    uchar byte_1 = header >> 8;
    uchar byte_2 = header; // pega so o primeiro byte da header(que tem 2 bytes)

    // printf("byte 1: %d\n", byte_1);
    // printf("byte 2: %d\n", byte_2);

    fputc(byte_1, file);
    fputc(byte_2, file);

    get_pre_order_tree(tree, file);

    compact_file(file, hash, trash);
    fclose(file);

    descompact();

    return 0;
}