#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>

// Representa um vértice com coordenadas
typedef struct Ponto {
    int id;
    double x;
    double y;
} Ponto;

// Representa uma conexão entre dois vértices
typedef struct Aresta {
    int origem;     // Vértice origem
    int destino;    // Vértice destino
    double peso;    // Distância calculada
} Aresta;

// Nó de uma lista de adjacência
typedef struct NoAdjacencia {
    int destino;
    double peso;
    struct NoAdjacencia* proximo;
} NoAdjacencia;

// Estrutura que encapsula todo o grafo
typedef struct {
    int numVertices;
    int numArestas;
    
    // Para o Prim
    NoAdjacencia** listaAdj;

    // Para o Kruskal
    Aresta* listaArestas;
    
    Ponto* nos;
} Grafo;

// Cria o grafo, aloca as listas e preenche os campos a partir da leitura dos arquivos
Grafo* criarGrafo(const char* nomeArquivoNos, const char* nomeArquivoArestas);

// Libera toda a memória alocada pelo grafo
void liberarGrafo(Grafo* grafo);

// Calcular distância entre dois pontos
double calcularDistancia(Ponto p1, Ponto p2);

#endif