#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grafo.h"
#include "algoritmos.h"

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);
    
    // Verificação de Argumentos
    if (argc < 3) {
        printf("Uso: %s <arquivo_nos.csv> <arquivo_arestas.csv>\n", argv[0]);
        return 1;
    }

    const char* arqNos = argv[1];
    const char* arqArestas = argv[2];

    printf("Carregando grafo...\n");
    printf("Nos: %s\n", arqNos);
    printf("Arestas: %s\n", arqArestas);

    // Criação do Grafo (Isolada no módulo)
    Grafo* grafo = criarGrafo(arqNos, arqArestas);
    
    if (!grafo) {
        printf("Erro ao criar o grafo. Verifique os arquivos.\n");
        return 1;
    }

    printf("Grafo carregado com sucesso!\n");
    printf("Vertices: %d | Arestas: %d\n", grafo->numVertices, grafo->numArestas);

    // Execução do Algoritmo de Kruskal
    printf("\nIniciando Kruskal...\n");
    
    clock_t inicioK = clock();
    double custoKruskal = executarKruskal(grafo);
    clock_t fimK = clock();
    
    double tempoKruskal = (double)(fimK - inicioK) / CLOCKS_PER_SEC;
    
    printf("--> Custo Total (Kruskal): %.2f\n", custoKruskal);
    printf("--> Tempo (Kruskal): %.6f segundos\n", tempoKruskal);

    // Execução do Algoritmo de Prim
    printf("\nIniciando Prim...\n");

    clock_t inicioP = clock();
    double custoPrim = executarPrim(grafo);
    clock_t fimP = clock();

    double tempoPrim = (double)(fimP - inicioP) / CLOCKS_PER_SEC;

    printf("--> Custo Total (Prim): %.2f\n", custoPrim);
    printf("--> Tempo (Prim): %.6f segundos\n", tempoPrim);

    // Validação básica
    printf("\n--- Validacao ---\n");
    if (abs(custoKruskal - custoPrim) < 0.001) { // Tolerância pequena
        printf("SUCESSO: Ambos os algoritmos encontraram o mesmo custo minimo.\n");
    } else {
        printf("ALERTA: Os custos diferem! Verifique a implementacao.\n");
        printf("Diferenca: %.2f\n", custoKruskal - custoPrim);
    }

    // Limpeza de Memória
    liberarGrafo(grafo);

    return 0;
}