#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "grafo.h"

// Função auxiliar para contar linhas
static int contarLinhas(const char* nomeArquivo) {
    FILE* fp = fopen(nomeArquivo, "r");
    if (!fp) return 0;
    int linhas = 0;
    int ch;
    int ultimoFoiNovaLinha = 1;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            linhas++;
            ultimoFoiNovaLinha = 1;
        } else {
            ultimoFoiNovaLinha = 0;
        }
    }
    if (!ultimoFoiNovaLinha && linhas > 0) linhas++;
    fclose(fp);
    return linhas;
}

// Função auxiliar para adicionar nó na lista de adjacência (insere no início)
static void adicionarNaListaAdj(Grafo* grafo, int origem, int destino, double peso) {
    NoAdjacencia* novoNo = (NoAdjacencia*) malloc(sizeof(NoAdjacencia));
    novoNo->destino = destino;
    novoNo->peso = peso;
    
    // Aponta para o antigo cabeça da lista
    novoNo->proximo = grafo->listaAdj[origem];
    
    // Atualiza o cabeça da lista
    grafo->listaAdj[origem] = novoNo;
}

Grafo* criarGrafo(const char* nomeArquivoNos, const char* nomeArquivoArestas) {
    // Alocar a estrutura principal
    Grafo* grafo = (Grafo*) malloc(sizeof(Grafo));
    if (!grafo) return NULL;
    char bufferHeader[1024];

    // Carregar quantidade de vértices
    int totalLinhasNos = contarLinhas(nomeArquivoNos);
    grafo->numVertices = (totalLinhasNos > 0) ? totalLinhasNos - 1 : 0;
    if (grafo->numVertices <= 0) {
        free(grafo);
        return NULL;
    }

    grafo->nos = (Ponto*) malloc(grafo->numVertices * sizeof(Ponto));
    
    // Aloca vetor de listas de adjacência (AdjListNode** array)
    grafo->listaAdj = (NoAdjacencia**) malloc(grafo->numVertices * sizeof(NoAdjacencia*));
    
    // Inicializa listas com NULL
    for (int i = 0; i < grafo->numVertices; i++) {
        grafo->listaAdj[i] = NULL;
    }

    // Lê o arquivo de nós
    FILE* arqNos = fopen(nomeArquivoNos, "r");
    if (arqNos) {
        fgets(bufferHeader, sizeof(bufferHeader), arqNos);
        for (int i = 0; i < grafo->numVertices; i++) {
            if (fscanf(arqNos, "%d,%lf,%lf", &grafo->nos[i].id, &grafo->nos[i].x, &grafo->nos[i].y) != 3) {
                printf("Aviso: Erro de leitura na linha %d do arquivo de nos.\n", i + 2);
            }
        }
        fclose(arqNos);
    } else {
        liberarGrafo(grafo);
        return NULL;
    }

    // Carregar arestas
    int totalLinhasArestas = contarLinhas(nomeArquivoArestas);
    grafo->numArestas = (totalLinhasArestas > 0) ? totalLinhasArestas - 1 : 0;
    
    if (grafo->numArestas > 0) {
        // Aloca vetor linear para o Kruskal
        grafo->listaArestas = (Aresta*) malloc(grafo->numArestas * sizeof(Aresta));
        
        FILE* arqArestas = fopen(nomeArquivoArestas, "r");
        if (arqArestas) {
            fgets(bufferHeader, sizeof(bufferHeader), arqArestas);
            for (int i = 0; i < grafo->numArestas; i++) {
                int u, v;
                // Lê apenas os índices
                if (fscanf(arqArestas, "%d,%d", &u, &v) == 2) {
                    u--; 
                    v--;
                    double peso = INFINITY;
                    
                    // Preenche vetor linear (Kruskal)
                    grafo->listaArestas[i].origem = u;
                    grafo->listaArestas[i].destino = v;
                    
                    // Calcula distância Euclidiana
                    if (u >= 0 && u < grafo->numVertices && v >= 0 && v < grafo->numVertices) {
                        peso = calcularDistancia(grafo->nos[u], grafo->nos[v]);
                    } else {
                        printf("Aviso: Aresta %d-%d refere-se a vertices inexistentes!\n", u, v);
                    }
                    grafo->listaArestas[i].peso = peso;

                    // Preenche Listas de Adjacência (Prim)
                    if (peso != INFINITY) {
                        adicionarNaListaAdj(grafo, u, v, peso);
                        adicionarNaListaAdj(grafo, v, u, peso);
                    }
                }
            }
            fclose(arqArestas);
        }
    } else {
        grafo->listaArestas = NULL;
    }

    return grafo;
}

void liberarGrafo(Grafo* grafo) {
    if (!grafo) return;
    if (grafo->nos) free(grafo->nos);
    if (grafo->listaArestas) free(grafo->listaArestas);
    
    // Libera as listas encadeadas de adjacência
    if (grafo->listaAdj) {
        for (int i = 0; i < grafo->numVertices; i++) {
            NoAdjacencia* atual = grafo->listaAdj[i];
            while (atual) {
                NoAdjacencia* temp = atual;
                atual = atual->proximo;
                free(temp);
            }
        }
        free(grafo->listaAdj);
    }
    free(grafo);
}

double calcularDistancia(Ponto p1, Ponto p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}