#include <stdio.h>
#include <stdlib.h>
#include "dfs.h"
#include "graph.h"

// int adj_mat[5][5] = {
//   {0, 1, 0, 0, 0},
//   {1, 0, 1, 1, 0},
//   {0, 1, 0, 0, 0},
//   {0, 1, 0, 0, 1},
//   {0, 0, 0, 1, 0}
// };

int main() {
  pthread_t tid;
  thread_data tdata;
  // int **arr = (int **)malloc(5 * sizeof(int *));
  // for (int i = 0; i < 5; ++i) {
  //   arr[i] = (int *)malloc(5 * sizeof(int));
  //   for(int j = 0; j < 5; ++j)
  //     arr[i][j] = adj_mat[i][j];
  // }
  
  struct graph_ret data = load_graph("g1.txt");
  tdata.adj_mat = data.adj_mat;
  tdata.i = 0;
  tdata.n = data.n;
  tdata.parent = -1;
  pthread_create(&tid, NULL, dfs, &tdata);
  pthread_join(tid, NULL);
  printf("%s\n", tdata.res);
  return 0;
}