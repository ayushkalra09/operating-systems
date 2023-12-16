#ifndef DFS_H
#define DFS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct {
  int **adj_mat;
  int n;
  int i;
  int parent;
  char res[256];
} thread_data;

void *dfs(void *arg) {
  thread_data *tdata = (thread_data *)arg;
  memset(tdata->res, 0, 256 * sizeof(char));
  if (tdata->i >= tdata->n) {
    pthread_exit(NULL);
  }

  pthread_t tid[tdata->n];
  thread_data res[tdata->n];
  int is_leaf = 1;
  for(int j = 0; j < tdata->n; ++j) {
    if (tdata->adj_mat[tdata->i][j] == 0 || j == tdata->parent || j == tdata->i)
      continue;
    
    is_leaf = 0;
    res[j].adj_mat = tdata->adj_mat;
    res[j].i = j;
    res[j].parent = tdata->i;
    res[j].n = tdata->n;
    pthread_create(&tid[j], NULL, dfs, &res[j]);
  }
  
  if (is_leaf == 1) {
    sprintf(tdata->res, "%i ", tdata->i + 1);
    pthread_exit(NULL);
  }

  for (int j = 0; j < tdata->n; ++j) {
    if (tdata->adj_mat[tdata->i][j] == 0)
      continue;
    
    pthread_join(tid[j], NULL);
    if (strlen(res[j].res) > 0)
      strcat(tdata->res, res[j].res);
  }

  pthread_exit(NULL);
}

#endif