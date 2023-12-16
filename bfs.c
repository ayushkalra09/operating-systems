#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_NODES 100
typedef struct Node {
    int data;
    struct Node* next;
} Node;
typedef struct Graph {
    int numNodes;
    Node* adjacencyList[MAX_NODES];
} Graph;
typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;
typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;
Graph* createGraph(int numNodes) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numNodes = numNodes;
    for (int i = 0; i < numNodes; i++) {
        graph->adjacencyList[i] = NULL;
    }
    return graph;
}
void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = dest;
    newNode->next = graph->adjacencyList[src];
    graph->adjacencyList[src] = newNode;

    newNode = (Node*)malloc(sizeof(Node));
    newNode->data = src;
    newNode->next = graph->adjacencyList[dest];
    graph->adjacencyList[dest] = newNode;
}


QueueNode* createQueueNode(int data) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}
void enqueue(Queue* queue, int data) {
    QueueNode* newNode = createQueueNode(data);
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}


int dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return -1; // qeue is empty
    }
    QueueNode* temp = queue->front;
    int data = temp->data;
    queue->front = temp->next;
    free(temp);
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    return data;
}
void* bfs(void* args) {
    Graph* graph = (Graph*)args;
    Queue* queue = createQueue();
    int* visited = (int*)malloc(graph->numNodes * sizeof(int));

    for (int i = 0; i < graph->numNodes; i++) {
        visited[i] = 0;
    }

    int startNode = 1; // Start BFS from node 0
    printf("BFS starting from node %d:\n", startNode);

    visited[startNode] = 1;
    enqueue(queue, startNode);

    while (queue->front != NULL) {
        int currentNode = dequeue(queue);
        printf("%d ", currentNode);

        Node* temp = graph->adjacencyList[currentNode];
        while (temp != NULL) {
            int adjNode = temp->data;
            if (!visited[adjNode]) {
                visited[adjNode] = 1;
                enqueue(queue, adjNode);
            }
            temp = temp->next;
        }
    }

    free(visited);
    free(queue);
    pthread_exit(NULL);
}

int main() {
    int adj_mat[5][5] = {
  {0, 1, 0, 0, 0},
  {1, 0, 1, 1, 0},
  {0, 1, 0, 0, 0},
  {0, 1, 0, 0, 1},
  {0, 0, 0, 1, 0}
};
    int numNodes = 5; // Change it

    Graph* graph = createGraph(numNodes);
    for(int i=0;i<5;i++){
        for(int j=0;j<i;j++){
            if(adj_mat[i][j]==1){
                addEdge(graph, i+1, j+1);
            }
        }
    }

    pthread_t thread;
    pthread_create(&thread, NULL, bfs, (void*)graph);
    pthread_join(thread, NULL);

    return 0;
}
