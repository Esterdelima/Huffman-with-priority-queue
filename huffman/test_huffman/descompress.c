#include "header.h"
#include "descompress.h"

// FUNCAO DE CRIAR UM NO

NODE_DESCOM* create_node(uchar caracter, NODE_DESCOM* left, NODE_DESCOM* right) {
    NODE_DESCOM* new_node = (NODE_DESCOM*) malloc(sizeof(NODE_DESCOM));
    // cast pro caracter
    uchar* aux = (uchar*) malloc(sizeof(uchar));
    *aux = caracter;
    new_node -> caracter = aux; // representacao do caracter na ASCII.

    new_node -> left = left;
    new_node -> right = right;

    return new_node;
}

// FUNCAO QUE VERIFICA SE EH UMA FOLHA

bool is_leaf(NODE_DESCOM* current) {
    if (current -> left == NULL && current -> right == NULL) return true;
    else return false;
}

// FUNCAO DE MONTAR A ARVORE

NODE_DESCOM* construct_tree(uchar *str, int *i) {
   if (str[*i] != '*') { // no folha
        if (str[*i] == '\\') *i += 1;
        NODE_DESCOM* leaf = create_node(str[*i], NULL, NULL);
        return leaf;
   }
   else {
        NODE_DESCOM* tree = create_node('*', NULL, NULL);
        *i += 1;
        tree->left = construct_tree(str, i);
        *i += 1;
        tree->right = construct_tree(str, i);
        return tree;
   }
}

// SETAR OS BITS

bool is_bit_i_set(ushort byte, int i) {
    ushort temp = 1 << i;
    return temp & byte;
}

// FUNCAO DE DESCOMPRESSAO

void descompact() {
    FILE* file = fopen("bandeira.jpg.huff", "rb"); // arquivo de escrita compactada
    uchar byte_1, byte_2;
    
    fscanf(file, "%c", &byte_1); // pego o primeiro byte do arquivo compactado (que contem os 3 bits de lixo).
    fscanf(file, "%c", &byte_2); // pego o segundo byte do arquivo compactado (com parte do tamanho da arvore).
    
    // PEGAR O LIXO E O TAMANHO DA ARVORE
    
    ushort size_tree = 0;       // zera os 16 bits com o tamanho da arvore.
    ushort trash = byte_1 >> 5; // elimina os 5 bits do tamanho da arvore.
    size_tree = byte_1 << 11;   // anda 8 bits pra passar pro byte + a esquerda, +3 pra sumir c os 3 bits de lixo.
    size_tree >>= 3;            // os 3 bits de lixo voltam zerados.
    size_tree |= byte_2;        // recebe o restante do tamanho da arvore no 2 byte.

    uchar str[size_tree];       // salva a arvore em pre ordem

    int i;
    for (i = 0; i < size_tree; i++) { // le a arvore em pre ordem no arquivo compacatado.
        fscanf(file, "%c", &str[i]);
    }

    // CONSTRUIR A ARVORE
    
    i = 0; 
    NODE_DESCOM* tree = construct_tree(str, &i);

    // CONTAR OS BYTES DO ARQUIVO

    lli cont_bytes = 0;
    uchar byte = 0;

    while (fscanf(file, "%c", &byte) != EOF) {
        cont_bytes++;
    }

    // DESCOMPACTAR

    fseek(file, 2 + size_tree, SEEK_SET); // função de percorrer o arquivo a partir de um ponto especifico.

    int limit = 0;
    NODE_DESCOM* current = tree; 
    FILE* descompacted = fopen("descompacted.jpg", "wb");

    for (cont_bytes; cont_bytes > 0; cont_bytes--) { // conta os bytes percorridos.
        fscanf(file, "%c", &byte);
        
        if (cont_bytes == 1) { // ultimo byte com o lixo (so verificamos esse byte ate o seu lixo).
            limit = trash; 
        }

        for (i = 7; i >= limit; i--) {
            if (is_bit_i_set(byte, i)) {
                current = current -> right; // 1-> direita (bit setado com 1)
            } 
            else {
                current = current -> left; // 0 -> esquerda (bit setado com 0)
            }
            if (is_leaf(current)) { // folha, hora de printar o caracter no novo arquivo :D.
                fprintf(descompacted, "%c", *((uchar*) ((NODE_DESCOM*) (current -> caracter))) );
                current = tree; // ponteiro pro inicio da arvore.
            }
        }
    }
    fclose(file);
    fclose(descompacted);
}


int main() {

    descompact();
    return 0;
}