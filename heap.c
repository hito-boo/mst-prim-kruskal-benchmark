#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

// Função auxiliar para realizar a troca dos nós do Heap
static void trocarNosHeap(NoHeap** a, NoHeap** b) {
    NoHeap* t = *a;
    *a = *b;
    *b = t;
}

// Garante a propriedade do Heap Mínimo (o pai é menor que os filhos) descendo na árvore
static void minHeapify(MinHeap* minHeap, int idx) {
    int menor = idx;
    int esquerda = 2 * idx + 1;
    int direita = 2 * idx + 2;

    if (esquerda < minHeap->tamanho && minHeap->array[esquerda]->chave < minHeap->array[menor]->chave)
        menor = esquerda;

    if (direita < minHeap->tamanho && minHeap->array[direita]->chave < minHeap->array[menor]->chave)
        menor = direita;

    if (menor != idx) {
        // Atualizar o mapa de posições antes de trocar
        NoHeap* menorNo = minHeap->array[menor];
        NoHeap* idxNo = minHeap->array[idx];

        // Troca as posições no mapa
        minHeap->pos[menorNo->v] = idx;
        minHeap->pos[idxNo->v] = menor;

        // Troca os nós no array
        trocarNosHeap(&minHeap->array[menor], &minHeap->array[idx]);
        minHeapify(minHeap, menor);
    }
}

NoHeap* criarNoHeap(int v, double chave) {
    NoHeap* no = (NoHeap*) malloc(sizeof(NoHeap));
    if (no) {
        no->v = v;
        no->chave = chave;
    }
    return no;
}

MinHeap* criarMinHeap(int capacidade) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    if (!minHeap) return NULL;

    minHeap->pos = (int*) malloc(capacidade * sizeof(int));
    minHeap->tamanho = 0;
    minHeap->capacidade = capacidade;
    minHeap->array = (NoHeap**) calloc(capacidade, sizeof(NoHeap*));

    if (!minHeap->pos || !minHeap->array) {
        if (minHeap->pos) free(minHeap->pos);
        if (minHeap->array) free(minHeap->array);
        free(minHeap);
        return NULL;
    }
    
    return minHeap;
}

NoHeap* extrairMinimo(MinHeap* minHeap) {
    if (heapVazia(minHeap))
        return NULL;

    NoHeap* raiz = minHeap->array[0];
    NoHeap* ultimoNo = minHeap->array[minHeap->tamanho - 1];

    minHeap->array[0] = ultimoNo;

    // Atualiza posições
    minHeap->pos[raiz->v] = minHeap->tamanho - 1;
    minHeap->pos[ultimoNo->v] = 0;

    minHeap->tamanho--;
    minHeapify(minHeap, 0);

    return raiz;
}

void diminuirChave(MinHeap* minHeap, int v, double chave) {
    // Pega o índice do vértice v no Heap
    int i = minHeap->pos[v];

    minHeap->array[i]->chave = chave;

    // Sobe na árvore enquanto o nó for menor que o pai
    while (i && minHeap->array[i]->chave < minHeap->array[(i - 1) / 2]->chave) {
        
        // Atualiza posições antes de trocar
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;

        trocarNosHeap(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

bool estaNoMinHeap(MinHeap* minHeap, int v) {
    if (minHeap->pos[v] < minHeap->tamanho)
        return true;
    return false;
}

bool heapVazia(MinHeap* minHeap) {
    return minHeap->tamanho == 0;
}

void liberarMinHeap(MinHeap* minHeap) {
    if (!minHeap) return;
    if (minHeap->array) {
        for (int i = 0; i < minHeap->tamanho; ++i) {
            if (minHeap->array[i]) {
               free(minHeap->array[i]);
            }
        }
        free(minHeap->array);
    }
    
    if (minHeap->pos) free(minHeap->pos);
    free(minHeap);
}