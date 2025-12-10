#include "union_find.h"

int encontrar(Subconjunto subconjuntos[], int i) {
    // Se i não é pai de si mesmo, busca recursivamente e atualiza o pai (compressão de caminho)
    if (subconjuntos[i].pai != i) {
        subconjuntos[i].pai = encontrar(subconjuntos, subconjuntos[i].pai);
    }
    return subconjuntos[i].pai;
}

void unir(Subconjunto subconjuntos[], int x, int y) {
    int raizX = encontrar(subconjuntos, x);
    int raizY = encontrar(subconjuntos, y);

    // Elemento com menor rank é anexado sob a raiz do elemento com maior rank
    if (subconjuntos[raizX].altura < subconjuntos[raizY].altura) {
        subconjuntos[raizX].pai = raizY;

    } else if (subconjuntos[raizX].altura > subconjuntos[raizY].altura) {
        subconjuntos[raizY].pai = raizX;
    } 
    // Se os ranks são iguais, escolhe um como raiz e incrementa seu rank
    else {
        subconjuntos[raizY].pai = raizX;
        subconjuntos[raizX].altura++;
    }
}