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

    // Inicializa o heap
    minHeap->tamanho = V;
    double pesoTotal = 0.0;

    printf("\n--- Algoritmo de Prim (Resultados) ---\n");

    // Configura a primeira raiz (Vértice 0)
    chave[0] = 0.0;
    diminuirChave(minHeap, 0, chave[0]);

    while (!heapVazia(minHeap)) {
        // Extrai o vértice mais "barato" da fronteira
        NoHeap* minNo = extrairMinimo(minHeap);
        int u = minNo->v;
        
        // Se o vértice ainda está desconectado (chave infinita), iniciar novo componente
        if (minNo->chave == DBL_MAX) {
            // Este vértice não foi alcançado - iniciar novo componente
            chave[u] = 0.0;
            pai[u] = -1;
            // Não adiciona ao custo (é uma nova raiz)
        } else if (pai[u] != -1) {
            // Se não for raiz (tem pai), adiciona o custo da aresta
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

// DFS iterativa usando pilha explícita (evita stack overflow em grafos grandes)
static void dfsIterativa(Grafo* grafo, int inicio, int* visitado) {
    // Alocar pilha para DFS iterativa
    int* pilha = (int*) malloc(grafo->numVertices * sizeof(int));
    if (!pilha) return;
    
    int topo = 0;
    pilha[topo++] = inicio;
    
    while (topo > 0) {
        int v = pilha[--topo];
        
        if (visitado[v]) continue;
        visitado[v] = 1;
        
        // Adiciona todos os vizinhos não visitados na pilha
        NoAdjacencia* adj = grafo->listaAdj[v];
        while (adj != NULL) {
            if (!visitado[adj->destino]) {
                pilha[topo++] = adj->destino;
            }
            adj = adj->proximo;
        }
    }
    
    free(pilha);
}

// Conta o número de componentes conexos no grafo
int contarComponentesConexos(Grafo* grafo) {
    if (!grafo || !grafo->listaAdj) return 0;
    
    int V = grafo->numVertices;
    int* visitado = (int*) calloc(V, sizeof(int));
    if (!visitado) return -1;
    
    int componentes = 0;
    
    for (int v = 0; v < V; v++) {
        if (!visitado[v]) {
            dfsIterativa(grafo, v, visitado);
            componentes++;
        }
    }
    
    free(visitado);
    return componentes;
}