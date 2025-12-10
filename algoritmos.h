#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "grafo.h"

// Executa o algoritmo de Kruskal
// Imprime as arestas da árvore selecionada e retorna o custo total
double executarKruskal(Grafo* grafo);

// Executa o algoritmo de Prim
// Imprime as arestas da árvore selecionada e retorna o custo total
double executarPrim(Grafo* grafo);

#endif