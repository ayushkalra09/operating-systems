#ifndef TUPLE_H
#define TUPLE_H

typedef struct {
  int seq_number;
  int operation_number;
  char graph_file_name[256];
} tuple;

#endif