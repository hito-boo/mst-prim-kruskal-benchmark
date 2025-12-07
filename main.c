#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>

// --- ESTRUTURAS BÁSICAS ---

typedef struct {
    int id;
    double x, y;
} NodeData;

typedef struct Edge {
    int u, v;
    double weight;
} Edge;

typedef struct AdjListNode {
    int dest;
    double weight;
    struct AdjListNode* next;
} AdjListNode;

typedef struct {
    int V, E;
    AdjListNode** array;
    Edge* edgeList;
    NodeData* nodes;
} Graph;

// --- UNION-FIND PARA DETECTAR COMPONENTES CONEXAS ---

typedef struct {
    int parent;
    int rank;
} Subset;

int find(Subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

void Union(Subset subsets[], int x, int y) {
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);

    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;
    else {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

// --- FUNÇÕES AUXILIARES ---

double calc_dist(NodeData n1, NodeData n2) {
    return sqrt(pow(n1.x - n2.x, 2) + pow(n1.y - n2.y, 2));
}

Graph* createGraph(int V, int E_count) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    
    graph->V = V;
    graph->E = E_count;
    graph->array = (AdjListNode**)malloc(V * sizeof(AdjListNode*));
    if (!graph->array) {
        free(graph);
        return NULL;
    }
    for (int i = 0; i < V; ++i) graph->array[i] = NULL;
    
    graph->edgeList = (Edge*)malloc(E_count * sizeof(Edge));
    if (!graph->edgeList) {
        free(graph->array);
        free(graph);
        return NULL;
    }
    
    graph->nodes = (NodeData*)malloc(V * sizeof(NodeData));
    if (!graph->nodes) {
        free(graph->edgeList);
        free(graph->array);
        free(graph);
        return NULL;
    }
    return graph;
}

void freeGraph(Graph* graph) {
    if (!graph) return;

    if (graph->array) {
        for (int i = 0; i < graph->V; ++i) {
            AdjListNode* crawl = graph->array[i];
            while (crawl) {
                AdjListNode* temp = crawl;
                crawl = crawl->next;
                free(temp);
            }
        }
        free(graph->array);
    }

    if (graph->edgeList) free(graph->edgeList);
    if (graph->nodes) free(graph->nodes);
    free(graph);
}

void addEdge(Graph* graph, int edgeIndex, int src, int dest) {
    double weight = calc_dist(graph->nodes[src], graph->nodes[dest]);

    graph->edgeList[edgeIndex].u = src;
    graph->edgeList[edgeIndex].v = dest;
    graph->edgeList[edgeIndex].weight = weight;

    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (!newNode) return;
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = graph->array[src];
    graph->array[src] = newNode;

    newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (!newNode) return;
    newNode->dest = src;
    newNode->weight = weight;
    newNode->next = graph->array[dest];
    graph->array[dest] = newNode;
}

// --- DETECÇÃO DE COMPONENTES CONEXAS ---

int countConnectedComponents(Graph* graph, int** componentMap) {
    Subset* subsets = (Subset*)malloc(graph->V * sizeof(Subset));
    if (!subsets) return -1;
    
    for (int i = 0; i < graph->V; i++) {
        subsets[i].parent = i;
        subsets[i].rank = 0;
    }
    
    // Une vértices conectados por arestas
    for (int i = 0; i < graph->E; i++) {
        int x = find(subsets, graph->edgeList[i].u);
        int y = find(subsets, graph->edgeList[i].v);
        if (x != y) {
            Union(subsets, x, y);
        }
    }
    
    // Mapeia cada vértice para seu componente
    *componentMap = (int*)malloc(graph->V * sizeof(int));
    if (!*componentMap) {
        free(subsets);
        return -1;
    }
    
    int numComponents = 0;
    int* rootToComponent = (int*)malloc(graph->V * sizeof(int));
    if (!rootToComponent) {
        free(*componentMap);
        free(subsets);
        return -1;
    }
    
    for (int i = 0; i < graph->V; i++) {
        rootToComponent[i] = -1;
    }
    
    for (int i = 0; i < graph->V; i++) {
        int root = find(subsets, i);
        if (rootToComponent[root] == -1) {
            rootToComponent[root] = numComponents++;
        }
        (*componentMap)[i] = rootToComponent[root];
    }
    
    free(rootToComponent);
    free(subsets);
    return numComponents;
}

// --- MIN-HEAP ---

typedef struct {
    int v;
    double key;
} MinHeapNode;

typedef struct {
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;

MinHeapNode* newMinHeapNode(int v, double key) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    if (!node) return NULL;
    node->v = v;
    node->key = key;
    return node;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!minHeap) return NULL;
    
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    if (!minHeap->pos) {
        free(minHeap);
        return NULL;
    }
    
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    if (!minHeap->array) {
        free(minHeap->pos);
        free(minHeap);
        return NULL;
    }
    
    return minHeap;
}

void freeMinHeap(MinHeap* minHeap) {
    if (!minHeap) return;
    
    // Libera todos os nós que ainda estão no heap
    if (minHeap->array) {
        for (int i = 0; i < minHeap->size; ++i) {
            if (minHeap->array[i]) {
                free(minHeap->array[i]);
            }
        }
        free(minHeap->array);
    }
    
    if (minHeap->pos) free(minHeap->pos);
    free(minHeap);
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->key < minHeap->array[smallest]->key)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->key < minHeap->array[smallest]->key)
        smallest = right;

    if (smallest != idx) {
        minHeap->pos[minHeap->array[smallest]->v] = idx;
        minHeap->pos[minHeap->array[idx]->v] = smallest;
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap)) return NULL;

    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    
    minHeap->array[0] = lastNode;
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(MinHeap* minHeap, int v, double key) {
    int i = minHeap->pos[v];
    minHeap->array[i]->key = key;

    while (i && minHeap->array[i]->key < minHeap->array[(i - 1) / 2]->key) {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap* minHeap, int v) {
    return minHeap->pos[v] < minHeap->size;
}

// --- PRIM EM UM COMPONENTE ---

double runPrimOnComponent(Graph* graph, int* vertices, int vertexCount, int startVertex) {
    if (vertexCount == 0) return 0.0;
    if (vertexCount == 1) return 0.0;
    
    double* key = (double*)malloc(graph->V * sizeof(double));
    if (!key) return -1;
    
    int* parent = (int*)malloc(graph->V * sizeof(int));
    if (!parent) {
        free(key);
        return -1;
    }
    
    int* inComponent = (int*)calloc(graph->V, sizeof(int));
    if (!inComponent) {
        free(parent);
        free(key);
        return -1;
    }
    
    // Marca vértices do componente
    for (int i = 0; i < vertexCount; i++) {
        inComponent[vertices[i]] = 1;
    }
    
    MinHeap* minHeap = createMinHeap(graph->V);
    if (!minHeap) {
        free(inComponent);
        free(parent);
        free(key);
        return -1;
    }

    for (int v = 0; v < graph->V; ++v) {
        parent[v] = -1;
        key[v] = DBL_MAX;
        minHeap->array[v] = newMinHeapNode(v, key[v]);
        if (!minHeap->array[v]) {
            // Libera nós já alocados
            for (int j = 0; j < v; j++) {
                if (minHeap->array[j]) free(minHeap->array[j]);
            }
            freeMinHeap(minHeap);
            free(inComponent);
            free(parent);
            free(key);
            return -1;
        }
        minHeap->pos[v] = v;
    }

    key[startVertex] = 0;
    decreaseKey(minHeap, startVertex, key[startVertex]);
    minHeap->size = graph->V;

    int processedCount = 0;
    while (!isEmpty(minHeap) && processedCount < vertexCount) {
        MinHeapNode* minNode = extractMin(minHeap);
        int u = minNode->v;
        free(minNode);
        
        if (!inComponent[u]) continue;
        processedCount++;

        AdjListNode* pCrawl = graph->array[u];
        while (pCrawl != NULL) {
            int v = pCrawl->dest;
            if (inComponent[v] && isInMinHeap(minHeap, v) && pCrawl->weight < key[v]) {
                key[v] = pCrawl->weight;
                parent[v] = u;
                decreaseKey(minHeap, v, key[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    double finalCost = 0;
    for (int i = 0; i < vertexCount; i++) {
        int v = vertices[i];
        if (v != startVertex && parent[v] != -1) {
            finalCost += key[v];
        }
    }

    free(inComponent);
    free(key);
    free(parent);
    freeMinHeap(minHeap);
    return finalCost;
}

// --- PRIM COMPLETO ---

double runPrim(Graph* graph, int* isConnected) {
    int* componentMap;
    int numComponents = countConnectedComponents(graph, &componentMap);
    
    if (numComponents < 0) {
        *isConnected = 0;
        return -1;
    }
    
    *isConnected = (numComponents == 1);
    
    if (numComponents > 1) {
        fprintf(stderr, "AVISO: Grafo desconexo com %d componentes\n", numComponents);
    }
    
    // Agrupa vértices por componente
    int** componentVertices = (int**)malloc(numComponents * sizeof(int*));
    int* componentSizes = (int*)calloc(numComponents, sizeof(int));
    
    if (!componentVertices || !componentSizes) {
        free(componentMap);
        if (componentVertices) free(componentVertices);
        if (componentSizes) free(componentSizes);
        *isConnected = 0;
        return -1;
    }
    
    // Conta vértices por componente
    for (int i = 0; i < graph->V; i++) {
        componentSizes[componentMap[i]]++;
    }
    
    // Aloca arrays para cada componente
    for (int c = 0; c < numComponents; c++) {
        componentVertices[c] = (int*)malloc(componentSizes[c] * sizeof(int));
        if (!componentVertices[c]) {
            for (int j = 0; j < c; j++) free(componentVertices[j]);
            free(componentVertices);
            free(componentSizes);
            free(componentMap);
            *isConnected = 0;
            return -1;
        }
        componentSizes[c] = 0; // Reset para usar como índice
    }
    
    // Preenche arrays de componentes
    for (int i = 0; i < graph->V; i++) {
        int comp = componentMap[i];
        componentVertices[comp][componentSizes[comp]++] = i;
    }
    
    // Executa Prim em cada componente
    double totalCost = 0;
    for (int c = 0; c < numComponents; c++) {
        int startVertex = componentVertices[c][0];
        double cost = runPrimOnComponent(graph, componentVertices[c], componentSizes[c], startVertex);
        if (cost < 0) {
            for (int j = 0; j < numComponents; j++) free(componentVertices[j]);
            free(componentVertices);
            free(componentSizes);
            free(componentMap);
            *isConnected = 0;
            return -1;
        }
        totalCost += cost;
    }
    
    // Libera memória
    for (int c = 0; c < numComponents; c++) {
        free(componentVertices[c]);
    }
    free(componentVertices);
    free(componentSizes);
    free(componentMap);
    
    return totalCost;
}

// --- KRUSKAL ---

int compareEdges(const void* a, const void* b) {
    Edge* a1 = (Edge*)a;
    Edge* b1 = (Edge*)b;
    if (a1->weight < b1->weight) return -1;
    if (a1->weight > b1->weight) return 1;
    return 0;
}

double runKruskal(Graph* graph, int* isConnected) {
    int V = graph->V;
    Edge* result = (Edge*)malloc(V * sizeof(Edge));
    if (!result) {
        *isConnected = 0;
        return -1;
    }
    
    int e = 0;
    int i = 0;
    double finalCost = 0;

    qsort(graph->edgeList, graph->E, sizeof(Edge), compareEdges);

    Subset* subsets = (Subset*)malloc(V * sizeof(Subset));
    if (!subsets) {
        free(result);
        *isConnected = 0;
        return -1;
    }
    
    for (int v = 0; v < V; ++v) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    while (e < V - 1 && i < graph->E) {
        Edge next_edge = graph->edgeList[i++];

        int x = find(subsets, next_edge.u);
        int y = find(subsets, next_edge.v);

        if (x != y) {
            result[e++] = next_edge;
            Union(subsets, x, y);
            finalCost += next_edge.weight;
        }
    }
    
    // Verifica se o grafo é conexo
    int numComponents = 0;
    for (int v = 0; v < V; v++) {
        if (subsets[v].parent == v) {
            numComponents++;
        }
    }
    
    *isConnected = (numComponents == 1);
    
    if (numComponents > 1) {
        fprintf(stderr, "AVISO: Grafo desconexo com %d componentes\n", numComponents);
    }

    free(subsets);
    free(result);
    return finalCost;
}

// --- MAIN ---

int countLines(char* filename) {
    FILE* fp = fopen(filename, "r");
    if(!fp) return 0;
    int lines = 0;
    char ch;
    int lastWasNewline = 1;
    while((ch = fgetc(fp)) != EOF) {
        if(ch == '\n') {
            lines++;
            lastWasNewline = 1;
        } else {
            lastWasNewline = 0;
        }
    }
    if(!lastWasNewline && lines > 0) lines++;
    fclose(fp);
    return lines;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_nos> <arquivo_arestas>\n", argv[0]);
        return 1;
    }

    char* nodeFile = argv[1];
    char* edgeFile = argv[2];

    int V = countLines(nodeFile) - 1;
    int E_max = countLines(edgeFile) - 1;

    Graph* graph = createGraph(V, E_max);
    if (!graph) {
        fprintf(stderr, "ERRO: Falha ao criar grafo\n");
        return 1;
    }

    // Ler NÓS
    FILE* fp = fopen(nodeFile, "r");
    if (!fp) {
        fprintf(stderr, "ERRO: Não foi possível abrir %s\n", nodeFile);
        freeGraph(graph);
        return 1;
    }
    
    char buffer[1024];
    if (!fgets(buffer, 1024, fp)) {
        fprintf(stderr, "ERRO: Arquivo de nós vazio\n");
        fclose(fp);
        freeGraph(graph);
        return 1;
    }
    
    int id;
    double x, y;
    
    while(fgets(buffer, 1024, fp)) {
        if (sscanf(buffer, "%d,%lf,%lf", &id, &x, &y) == 3) {
            int idx = id - 1;
            if (idx >= 0 && idx < V) {
                graph->nodes[idx].id = id;
                graph->nodes[idx].x = x;
                graph->nodes[idx].y = y;
            }
        }
    }
    fclose(fp);

    // Ler ARESTAS
    fp = fopen(edgeFile, "r");
    if (!fp) {
        fprintf(stderr, "ERRO: Não foi possível abrir %s\n", edgeFile);
        freeGraph(graph);
        return 1;
    }
    
    if (!fgets(buffer, 1024, fp)) {
        fprintf(stderr, "ERRO: Arquivo de arestas vazio\n");
        fclose(fp);
        freeGraph(graph);
        return 1;
    }
    
    int u, v, edgeCount = 0;
    
    while(fgets(buffer, 1024, fp) && edgeCount < E_max) {
        if (sscanf(buffer, "%d,%d", &u, &v) == 2) {
            addEdge(graph, edgeCount++, u-1, v-1);
        }
    }
    graph->E = edgeCount;
    fclose(fp);

    // Executar algoritmos
    clock_t start, end;
    int isConnectedPrim = 1, isConnectedKruskal = 1;
    
    // PRIM
    start = clock();
    double costPrim = runPrim(graph, &isConnectedPrim);
    end = clock();
    double timePrim = ((double) (end - start)) / CLOCKS_PER_SEC;

    // KRUSKAL
    start = clock();
    double costKruskal = runKruskal(graph, &isConnectedKruskal);
    end = clock();
    double timeKruskal = ((double) (end - start)) / CLOCKS_PER_SEC;

    // VALIDAÇÃO DOS RESULTADOS
    int validationPassed = 1;
    
    if (isConnectedPrim != isConnectedKruskal) {
        fprintf(stderr, "ERRO: Inconsistência na detecção de conectividade!\n");
        fprintf(stderr, "  Prim detectou: %s\n", isConnectedPrim ? "conexo" : "desconexo");
        fprintf(stderr, "  Kruskal detectou: %s\n", isConnectedKruskal ? "conexo" : "desconexo");
        validationPassed = 0;
    }
    
    // Tolera pequenas diferenças de ponto flutuante
    double costDiff = fabs(costPrim - costKruskal);
    double tolerance = 0.01; // 1 centésimo de diferença
    
    if (costDiff > tolerance) {
        fprintf(stderr, "ERRO: Custos diferentes entre Prim e Kruskal!\n");
        fprintf(stderr, "  Prim: %.4f\n", costPrim);
        fprintf(stderr, "  Kruskal: %.4f\n", costKruskal);
        fprintf(stderr, "  Diferença: %.4f\n", costDiff);
        validationPassed = 0;
    }
    
    if (costPrim < 0 || costKruskal < 0) {
        fprintf(stderr, "ERRO: Falha na execução dos algoritmos\n");
        validationPassed = 0;
    }

    // Saída: V,E,CostPrim,TimePrim,CostKruskal,TimeKruskal,IsConnected,ValidationPassed
    printf("%d,%d,%.4f,%.6f,%.4f,%.6f,%d,%d\n", 
           V, graph->E, costPrim, timePrim, costKruskal, timeKruskal,
           isConnectedPrim, validationPassed);

    freeGraph(graph);
    return validationPassed ? 0 : 1;
}