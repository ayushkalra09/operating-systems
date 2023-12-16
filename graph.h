#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>

struct graph_ret {
  int n;
  int **adj_mat;
};

struct graph_ret load_graph(char *file_name) {
  FILE *graph_file = fopen(file_name, "r");
  if (graph_file) {
    int n;
    fscanf(graph_file, "%i", &n);
    int **adj_mat = (int **)malloc(n * sizeof(int **));
    for(int i = 0; i < n; ++i) {
      adj_mat[i] = (int *)malloc(n * sizeof(int *));
      for (int j = 0; j < n; ++j)
        fscanf(graph_file, "%i", &adj_mat[i][j]);
    }
    fclose(graph_file);
    return (struct graph_ret){n, adj_mat};
  }
  return (struct graph_ret){0, NULL};
}

int write_graph(char *file_name, int **adj_mat, int n) {
  FILE *graph_file = fopen(file_name, "w");
  if (graph_file) {
    fprintf(graph_file, "%i\n", n);
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        fprintf(graph_file, "%i ", adj_mat[i][j]);
      }
      fprintf(graph_file, "\n");
    }
    fclose(graph_file);
    return 0;
  }
  return -1;
}

#endif