#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

// Estrutura para um nó do Heap Mínimo (representa um vértice e seu peso atual)
typedef struct NoHeap {
    int v;          // Índice do vértice
    double chave;   // Peso (custo) associado
} NoHeap;

// Estrutura do Heap Mínimo
typedef struct {
    int tamanho;      // Número atual de elementos no Heap
    int capacidade;   // Capacidade máxima
    int *pos;         // Mapa: pos[v] indica onde o vértice 'v' está no array 'array'
    NoHeap **array;   // Array de ponteiros para os nós do Heap
} MinHeap;

// Cria um novo nó para o Heap
NoHeap* criarNoHeap(int v, double chave);

// Cria um novo Heap Mínimo
MinHeap* criarMinHeap(int capacidade);

// Extrai o nó com o menor valor (Raiz do Heap)
NoHeap* extrairMinimo(MinHeap* minHeap);

// Atualiza o valor de um vértice (diminui seu peso) e reorganiza o Heap
void diminuirChave(MinHeap* minHeap, int v, double chave);

// Verifica se o vértice 'v' está no Heap
bool estaNoMinHeap(MinHeap* minHeap, int v);

// Verifica se o Heap está vazio
bool heapVazia(MinHeap* minHeap);

// Função para liberação a memória ocupada pelo Heap
void liberarMinHeap(MinHeap* minHeap);

#endif