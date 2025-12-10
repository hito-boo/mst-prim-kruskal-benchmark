#ifndef UNION_FIND_H
#define UNION_FIND_H

// Estrutura para representar um subconjunto no Union-Find
typedef struct Subconjunto {
    int pai;
    int altura;
} Subconjunto;

// Encontra o representante (raiz) do conjunto do elemento 'i' (com compressão de caminho)
int encontrar(Subconjunto subconjuntos[], int i);

// Une os conjuntos de 'x' e 'y' usando a união por rank (menor altura)
void unir(Subconjunto subconjuntos[], int x, int y);

#endif