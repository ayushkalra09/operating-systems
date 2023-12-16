#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "settings.h"
#include "tuple.h"
#include "shared_mem.h"
#include <pthread.h>

// Define the message structure
struct message
{
  long mtype;
  long sender_id;
  tuple data;
};

void *handle_operation_1(void *data)
{
  tuple *tdata = (tuple *)data;
  struct graph_ret sdata = read_from_sm();
  printf("Number of nodes : %i\n", sdata.n);
  int ret = write_graph(tdata->graph_file_name, sdata.adj_mat, sdata.n);
  if (ret == 0)
    printf("File %s written successfully!\n", tdata->graph_file_name);
  else
    printf("Error in writing file %s\n", tdata->graph_file_name);
  pthread_exit(NULL);
}

void *handle_operation_2(void *data)
{
  tuple *tdata = (tuple *)data;
  struct graph_ret sdata = read_from_sm();
  int ret = write_graph(tdata->graph_file_name, sdata.adj_mat, sdata.n);
  if (ret == 0)
    printf("File %s updated successfully!\n", tdata->graph_file_name);
  else
    printf("Error in updating file %s\n", tdata->graph_file_name);
  pthread_exit(NULL);
}

// Function to handle client requests
void handle_client_request(int client_id, int client_queue, tuple req)
{
  if (req.operation_number == 1)
  {
    pthread_t tid;
    pthread_create(&tid, NULL, handle_operation_1, &req);
    // pthread_join(tid, NULL);
  }
  else
  {
    pthread_t tid;
    pthread_create(&tid, NULL, handle_operation_2, &req);
  }
}

int main()
{
  int server_queue;
  key_t server_key;

  // Create a key for the message queue
  server_key = ftok("load_balancer.c", SERVER_KEY);

  // Create a message queue
  server_queue = msgget(server_key, IPC_CREAT | 0666);

  if (server_queue == -1)
  {
    perror("msgget");
    exit(1);
  }

  // Main server loop
  while (1)
  {
    // int client_id;
    pid_t child_pid;

    // Receive the client's request for a new child server
    struct message msg;
    printf("waiting for msg\n");
    if (msgrcv(server_queue, &msg, sizeof(msg.sender_id) + sizeof(msg.data), PRIMARY_SERVER_PORT, 0) == -1)
    {
      perror("msgrcv");
      exit(1);
    }
    printf("Got tuple (%i, %i, %s) %ld\n", msg.data.operation_number, msg.data.seq_number, msg.data.graph_file_name, msg.mtype);
    if (msg.data.operation_number == 6)
    {
      printf("Exiting Primary Server\n.");
      exit(0);
    }
    handle_client_request(msg.mtype, server_queue, msg.data);
  }

  // Cleanup and exit the main server (this part will not be reached)
  msgctl(server_queue, IPC_RMID, NULL);
  return 0;
}
