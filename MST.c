#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CITIES 100
#define MAX_EDGES 500

typedef struct EdgeNode
{
    char dest[50];
    int weight;
    struct EdgeNode* next;
} EdgeNode;

typedef struct
{
    char name[50];
    EdgeNode* edges;
} City;

typedef struct
{
    char src[50];
    char dest[50];
    int weight;
} Edge;

City graph[MAX_CITIES];
Edge edgeList[MAX_EDGES];
int cityCount = 0;
int edgeCount = 0;
int parent[MAX_CITIES];

//min-heap for prim
typedef struct
{
    int cityx;
    int weight;
    int fromx;
} HeapNode;

typedef struct
{
    HeapNode nodes[MAX_EDGES];
    int size;
} MinHeap;

void minHeapSetup(MinHeap* h)
{
    h->size = 0;
}

void minHeapSwap(HeapNode* a, HeapNode* b)
{
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapPush(MinHeap* h, HeapNode node)
{
   int i = h->size;
   h->size++;
    h->nodes[i] = node;
    while (i != 0)
    {
        int parent = (i - 1) / 2;
        if (h->nodes[parent].weight <= h->nodes[i].weight)
        {
            break;
        }
        minHeapSwap(&h->nodes[parent], &h->nodes[i]);
        i = parent;
    }
}

HeapNode minHeapPop(MinHeap* h)
{
    HeapNode root = h->nodes[0];
    h->size--;
    h->nodes[0] = h->nodes[h->size];
    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < h->size && h->nodes[left].weight < h->nodes[smallest].weight)
        {
            smallest = left;
        }
        if (right < h->size && h->nodes[right].weight < h->nodes[smallest].weight)
        {
            smallest = right;
        }
        if (smallest == i)
        {
            break;
        }
        minHeapSwap(&h->nodes[i], &h->nodes[smallest]);
        i = smallest;
    }
    return root;
}

int minHeapEmpty(MinHeap* h)
{
    return h->size == 0;
}

//graph management
void clearGraph()
{
    for (int i = 0; i < MAX_CITIES; i++)
    {
        EdgeNode* edge = graph[i].edges;
        while (edge!=NULL)
        {
            EdgeNode* temp = edge;
            edge = edge->next;
            free(temp);
        }
        graph[i].edges = NULL;
        strcpy(graph[i].name, "");
    }
    cityCount = edgeCount = 0;
}

int findCityIndex(const char* name)
{
    for (int i = 0; i < cityCount; i++)
    {
        if (strcmp(graph[i].name, name) == 0) {return i;}
    }
    return -1;
}

void addCity(const char* name)
{
    if (findCityIndex(name) == -1 && cityCount < MAX_CITIES)
    {
        strcpy(graph[cityCount++].name, name);
    }
}

void addEdge(const char* src, const char* dest, int weight)
{
    if (edgeCount >= MAX_EDGES) {return;}
    int srcx = findCityIndex(src);
    int destx = findCityIndex(dest);
    if (srcx == -1 || destx == -1) {return;}

    strcpy(edgeList[edgeCount].src, src);
    strcpy(edgeList[edgeCount].dest, dest);
edgeList[edgeCount].weight = weight;
edgeCount++;
    EdgeNode* newEdge = malloc(sizeof(EdgeNode));
    strcpy(newEdge->dest, dest);
    newEdge->weight = weight;
    newEdge->next = graph[srcx].edges;
    graph[srcx].edges = newEdge;

    EdgeNode* reverseEdge = malloc(sizeof(EdgeNode));
    strcpy(reverseEdge->dest, src);
    reverseEdge->weight = weight;
    reverseEdge->next = graph[destx].edges;
    graph[destx].edges = reverseEdge;
}

void loadCities()
{
    clearGraph();
    FILE* file = fopen("cities.txt", "r");
    if (file == NULL)
    {
        printf("Error with opening file...!\n");
        return;
    }
    char line[300];
    while (fgets(line, sizeof(line), file))
    {
        char src[50];
        char dest[50];
        int distance;
        char* token = strtok(line, "#");
        if (!token){ continue;}
        strncpy(src, token, sizeof(src)-1);
        token = strtok(NULL, "#");
        if (!token) {continue;}
        strncpy(dest, token, sizeof(dest)-1);
        token = strtok(NULL, "#\n");
        if (!token) {continue;}
        distance = atoi(token);
        addCity(src);
        addCity(dest);
        addEdge(src, dest, distance);
    }
    fclose(file);

    printf("Successfully loaded %d cities and %d edges\n", cityCount, edgeCount);
}

void primMST(const char* startCity)
{
    if (cityCount == 0)
    {
        printf("No cities loaded ...!\n");
        return;
    }

    int visited[MAX_CITIES] = {0};
    int minCost = 0;
    int edgesAdded = 0;
    int startx = findCityIndex(startCity);
    if (startx == -1)
    {
        printf("Error:City '%s' not found...!\n", startCity);
        return;
    }

    clock_t start = clock();
    MinHeap heap;
    minHeapSetup(&heap);
    visited[startx] = 1;

    EdgeNode* edge = graph[startx].edges;
while (edge != NULL)
    {
        int destx = findCityIndex(edge->dest);
            if (!visited[destx])        {
            HeapNode node = {destx, edge->weight, startx};
            minHeapPush(&heap, node);
        }
        edge = edge->next;
    }

    printf("\n MST Edges (Prim's using Min-Heap):\n");

    while (edgesAdded < cityCount - 1 && !minHeapEmpty(&heap))
    {
        HeapNode curr = minHeapPop(&heap);
        if (visited[curr.cityx])
        {
            continue;
        }
        visited[curr.cityx] = 1;
        minCost += curr.weight;
        edgesAdded++;
        printf("%s -- %s (%d km)\n", graph[curr.fromx].name, graph[curr.cityx].name, curr.weight);

        edge = graph[curr.cityx].edges;
        while (edge != NULL)
        {
            int destx = findCityIndex(edge->dest);
            if (!visited[destx])
            {
                HeapNode node = {destx, edge->weight, curr.cityx};
                minHeapPush(&heap, node);
            }
            edge = edge->next;
        }
    }

    printf("Total Cost: %d km\n", minCost);
    printf("Execution Time: %.4f seconds\n\n", (double)(clock() - start) / CLOCKS_PER_SEC);
}

int find(int i)
{
    if (parent[i] != i) {parent[i] = find(parent[i]);}
    return parent[i];
}

void join(int i, int j)
{
    parent[find(i)] = find(j);
}

void bubbleSortEdges() {
    for (int i = 0; i < edgeCount - 1; i++) {
        for (int j = 0; j < edgeCount - i - 1; j++) {
            if (edgeList[j].weight > edgeList[j + 1].weight) {
                Edge temp = edgeList[j];
                edgeList[j] = edgeList[j + 1];
                edgeList[j + 1] = temp;
            }
        }
    }
}


void kruskalMST()
{
    if (cityCount == 0)
    {
        printf("Error: No cities loaded\n");
        return;
    }

    clock_t start = clock();
    bubbleSortEdges();
    for (int i = 0; i < cityCount; i++) parent[i] = i;
    int minCost = 0;
    int edgesAdded = 0;
    printf("\n MST Edges Kruskal's:\n");

    for (int i = 0; i < edgeCount && edgesAdded < cityCount - 1; i++)
    {
        int x = findCityIndex(edgeList[i].src);
        int y = findCityIndex(edgeList[i].dest);
        if (x != -1 && y != -1 && find(x) != find(y))
        {
            printf("%s -- %s (%d km)\n", edgeList[i].src, edgeList[i].dest, edgeList[i].weight);
            minCost += edgeList[i].weight;
            join(x, y);
            edgesAdded++;
        }
    }

    if (edgesAdded < cityCount - 1)
    {
        printf("Warning: Graph is disconnected.\n");
    }

    printf("Total Cost: %d km\n", minCost);
    printf("Execution Time: %.4f seconds\n\n", (double)(clock() - start) / CLOCKS_PER_SEC);
}
int main()
{
    for (int i = 0; i < MAX_CITIES; i++)
    {
        parent[i] = i;
    }
    int choice;
    char startCity[50];

    do
    {
        printf("\nMenu:\n");
        printf("1. Load cities\n");
        printf("2. Run Prim's Algorithm\n");
        printf("3. Run Kruskal's Algorithm\n");
        printf("4. Compare Algorithms\n");
        printf("5. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            loadCities();
            break;
        case 2:
            if (cityCount == 0)
            {
                printf("Error: Load cities first\n");
                break;
            }
            printf("Enter starting city: ");
            scanf("%49s", startCity);
            primMST(startCity);
            break;
        case 3:
            if (cityCount == 0)
            {
                printf("Error: Load cities first\n");
                break;
            }
            kruskalMST();
            break;
        case 4:
            if (cityCount == 0)
            {
                printf("Error: Load cities first\n");
                break;
            }
            printf("Enter starting city for Prim's Algorithm: ");
            scanf("%49s", startCity);
            printf("\n--- Prim's Algorithm ---\n");
            primMST(startCity);
            printf("\n--- Kruskal's Algorithm ---\n");
            kruskalMST();
            break;
        case 5:
            break;
        default:
            printf("Invalid choice\n");
        }
    }
    while (choice != 5);

    clearGraph();
    return 0;
}
