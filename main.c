#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "grafo.h"
#include "algoritmos.h"

// Detectar plataforma
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#endif

/* 
   Medição de Memória (Multiplataforma)
*/
long getPeakRSS() {
#ifdef _WIN32
    // Windows
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return (long)(pmc.PeakWorkingSetSize / 1024); // Converter para kB
    }
    return -1;
#else
    // Linux
    FILE* file = fopen("/proc/self/status", "r");
    if (!file) return -1;

    char line[256];
    long peak = -1;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "VmHWM:", 6) == 0) {
            sscanf(line, "VmHWM: %ld kB", &peak);
            break;
        }
    }

    fclose(file);
    return peak;  // Em kB
#endif
}

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

    Grafo* grafo = criarGrafo(arqNos, arqArestas);
    
    if (!grafo) {
        printf("Erro ao criar o grafo. Verifique os arquivos.\n");
        return 1;
    }

    printf("Grafo carregado com sucesso!\n");
    printf("Vertices: %d | Arestas: %d\n", grafo->numVertices, grafo->numArestas);

    // Verificar conectividade do grafo (apenas para grafos pequenos/médios)
    int componentes = -1;
    if (grafo->numVertices < 100000) {
        componentes = contarComponentesConexos(grafo);
        printf("Componentes Conexos: %d\n", componentes);
        
        if (componentes > 1) {
            printf("AVISO: Grafo desconexo detectado! Sera calculada a Floresta Geradora Minima.\n");
        }
    } else {
        printf("Componentes Conexos: (nao verificado - grafo muito grande)\n");
    }

    /* 
       KRUSKAL — Tempo e Memória
    */
    printf("\nIniciando Kruskal...\n");

    long memAntesK = getPeakRSS();
    clock_t inicioK = clock();

    double custoKruskal = executarKruskal(grafo);

    clock_t fimK = clock();
    long memDepoisK = getPeakRSS();

    double tempoKruskal = (double)(fimK - inicioK) / CLOCKS_PER_SEC;
    long memKruskal = memDepoisK - memAntesK;

    printf("--> Custo Total (Kruskal): %.2f\n", custoKruskal);
    printf("--> Tempo (Kruskal): %.6f segundos\n", tempoKruskal);
    printf("--> Memória Usada (Kruskal): %ld kB\n", memKruskal);

    /* 
       PRIM — Tempo e Memória
    */
    printf("\nIniciando Prim...\n");

    long memAntesP = getPeakRSS();
    clock_t inicioP = clock();

    double custoPrim = executarPrim(grafo);

    clock_t fimP = clock();
    long memDepoisP = getPeakRSS();

    double tempoPrim = (double)(fimP - inicioP) / CLOCKS_PER_SEC;
    long memPrim = memDepoisP - memAntesP;

    printf("--> Custo Total (Prim): %.2f\n", custoPrim);
    printf("--> Tempo (Prim): %.6f segundos\n", tempoPrim);
    printf("--> Memória Usada (Prim): %ld kB\n", memPrim);

    /* 
        Validação básica
    */
    printf("\n--- Validacao ---\n");
    double diferenca = custoKruskal - custoPrim;
    if (diferenca < 0) diferenca = -diferenca; // abs para double
    
    if (diferenca < 0.001) {
        printf("SUCESSO: Ambos os algoritmos encontraram o mesmo custo minimo.\n");
        if (componentes > 1) {
            printf("(Floresta Geradora Minima com %d componentes)\n", componentes);
        }
    } else {
        printf("ALERTA: Os custos diferem! Verifique a implementacao.\n");
        printf("Diferenca: %.2f\n", diferenca);
    }

    liberarGrafo(grafo);
    return 0;
}
