#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "algoritmos.h"
#include "heap.h"
#include "union_find.h"

// Função auxiliar privada utilizada como comparadora para o qsort (necessário para o Kruskal)
static int compararArestas(const void* a, const void* b) {
    Aresta* a1 = (Aresta*)a;
    Aresta* a2 = (Aresta*)b;
    if (a1->peso < a2->peso) return -1;
    if (a1->peso > a2->peso) return 1;
    return 0;
}

// Algoritmo de Kruskal
double executarKruskal(Grafo* grafo) {
    if (!grafo || !grafo->listaArestas) return 0.0;

    double pesoTotal = 0.0;
    int arestasNaArvore = 0;
    int V = grafo->numVertices;

    // Ordenar todas as arestas por peso
    qsort(grafo->listaArestas, grafo->numArestas, sizeof(Aresta), compararArestas);

    // Alocar memória para o Union-Find
    Subconjunto* subconjuntos = (Subconjunto*) malloc(V * sizeof(Subconjunto));
    
    // Inicializa: cada vértice é seu próprio pai (conjuntos disjuntos)
    for (int v = 0; v < V; ++v) {
        subconjuntos[v].pai = v;
        subconjuntos[v].altura = 0;
    }

    printf("\n--- Algoritmo de Kruskal (Resultados) ---\n");

    // Iterar pelas arestas ordenadas
    for (int i = 0; i < grafo->numArestas; i++) {
        // Otimização: Se há V-1 arestas, a árvore está pronta
        if (arestasNaArvore >= V - 1) break;

        Aresta proximaAresta = grafo->listaArestas[i];

        int x = encontrar(subconjuntos, proximaAresta.origem);
        int y = encontrar(subconjuntos, proximaAresta.destino);

        // Se x != y, não formam ciclo (estão em componentes conexos diferentes)
        if (x != y) {
            // printf("Aresta: %d - %d \tPeso: %.2f\n", proximaAresta.origem, proximaAresta.destino, proximaAresta.peso);
            
            pesoTotal += proximaAresta.peso;
            unir(subconjuntos, x, y);
            arestasNaArvore++;
        }
    }

    free(subconjuntos);
    return pesoTotal;
}

// Algoritmo de Prim
double executarPrim(Grafo* grafo) {
    if (!grafo || !grafo->listaAdj) return 0.0;

    int V = grafo->numVertices;
    int* pai = (int*) malloc(V * sizeof(int));              // Armazena a árvore final
    double* chave = (double*) malloc(V * sizeof(double));   // Custo mínimo para conectar o vértice
    
    // Cria o Heap Mínimo
    MinHeap* minHeap = criarMinHeap(V);
    if (!minHeap) { // Segurança extra
        free(pai); free(chave); return 0.0;
    }

    // Inicializa todos com custo infinito
    for (int v = 0; v < V; v++) {
        pai[v] = -1;
        chave[v] = DBL_MAX;
        minHeap->array[v] = criarNoHeap(v, chave[v]);
        minHeap->pos[v] = v;
    }

    // Configura a Raiz (Vértice 0)
    chave[0] = 0.0;
    diminuirChave(minHeap, 0, chave[0]);
    minHeap->tamanho = V; 

    double pesoTotal = 0.0;

    printf("\n--- Algoritmo de Prim (Resultados) ---\n");

    while (!heapVazia(minHeap)) {
        // Extrai o vértice mais "barato" da fronteira
        NoHeap* minNo = extrairMinimo(minHeap);
        int u = minNo->v; 

        // Se não for a raiz inicial (que não tem pai), imprime a conexão
        if (pai[u] != -1) {
            // printf("Aresta: %d - %d \tPeso: %.2f\n", pai[u], u, minNo->chave);
            pesoTotal += minNo->chave;
        }

        // Percorre vizinhos na Lista de Adjacência
        NoAdjacencia* vizinho = grafo->listaAdj[u];
        while (vizinho != NULL) {
            int v = vizinho->destino;

            // Se v ainda está no Heap e achamos um caminho mais curto até ele
            if (estaNoMinHeap(minHeap, v) && vizinho->peso < chave[v]) {
                chave[v] = vizinho->peso;
                pai[v] = u;
                diminuirChave(minHeap, v, chave[v]);
            }
            vizinho = vizinho->proximo;
        }
        
        free(minNo); // Libera o nó extraído do Heap
    }

    liberarMinHeap(minHeap);
    free(pai);
    free(chave);

    return pesoTotal;
}