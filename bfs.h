#ifndef BFS_H
#define BFS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
  int **adj_mat;
  int n;
  int i;
  int parent;
  char res[256];
} thread_data1;

typedef struct QueueNode
{
  int data;
  struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
  QueueNode *front;
  QueueNode *rear;
} Queue;

Queue *createQueue()
{
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->front = queue->rear = NULL;
  return queue;
}

void enqueue(Queue *queue, int data)
{
  QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
  newNode->data = data;
  newNode->next = NULL;

  if (queue->rear == NULL)
  {
    queue->front = queue->rear = newNode;
    return;
  }

  queue->rear->next = newNode;
  queue->rear = newNode;
}

int dequeue(Queue *queue)
{
  if (queue->front == NULL)
    return -1;

  QueueNode *temp = queue->front;
  int data = temp->data;
  queue->front = temp->next;

  if (queue->front == NULL)
    queue->rear = NULL;

  free(temp);
  return data;
}

void *bfs(void *arg)
{
  thread_data1 *tdata = (thread_data1 *)arg;
  memset(tdata->res, 0, 256 * sizeof(char));

  if (tdata->i >= tdata->n)
  {
    pthread_exit(NULL);
  }

  pthread_t tid[tdata->n];
  thread_data1 res[tdata->n];
  int thread_count = 0;

  Queue *queue = createQueue();

  enqueue(queue, tdata->i);
  sprintf(tdata->res + strlen(tdata->res), "%i ", tdata->i + 1);
  while (queue->front != NULL)
  {
    int current = dequeue(queue);

    for (int j = 0; j < tdata->n; ++j)
    {
      if (tdata->adj_mat[current][j] == 1 && j != tdata->parent)
      {
        res[thread_count].adj_mat = tdata->adj_mat;
        res[thread_count].i = j;
        res[thread_count].parent = current;
        res[thread_count].n = tdata->n;
        pthread_create(&tid[thread_count], NULL, bfs, &res[thread_count]);
        thread_count++;
      }
    }
  }

  for (int k = 0; k < thread_count; ++k)
  {
    pthread_join(tid[k], NULL);
    if (strlen(res[k].res) > 0)
      strcat(tdata->res, res[k].res);
  }


  pthread_exit(NULL);
}

#endif
