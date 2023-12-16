#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "settings.h"
#include "tuple.h"
#include "dfs.h"
#include "graph.h"
#include "bfs.h"
// Define the message structure
struct message
{
  long mtype;
  long sender_id;
  tuple data;
};

void *handle_dfs(void *data)
{
  struct message *r = (struct message *)data;
  tuple req = r->data;
  pthread_t tid;
  struct graph_ret gr = load_graph(req.graph_file_name);
  int stnode = 0;
  printf("enter start node");
  scanf("%d", &stnode);
  thread_data res = {gr.adj_mat, gr.n, stnode, -1};
  pthread_create(&tid, NULL, dfs, &res);
  pthread_join(tid, NULL);
  // printf("DFS = %s\n", res.res);
  strcpy(req.graph_file_name, res.res);
  struct message ret_msg = {r->mtype, r->mtype, req};
  if (msgsnd(r->sender_id, &ret_msg, sizeof(ret_msg), 0) == -1)
  {
    perror("msgsnd");
  }
  pthread_exit(NULL);
}

// Function to handle client requests
void handle_client_request_dfs(int client_id, int client_queue, tuple req)
{
  pthread_t tid;
  struct message data = {client_id, client_queue, req};
  pthread_create(&tid, NULL, handle_dfs, &data);
  // pthread_exit(NULL)
}
void *handle_bfs(void *data)
{
  struct message *r = (struct message *)data;
  tuple req = r->data;
  pthread_t tid;
  struct graph_ret gr = load_graph(req.graph_file_name);
  int stnode = 0;
  printf("enter start node");
  scanf("%d", &stnode);
  thread_data res = {gr.adj_mat, gr.n, stnode, -1};
  pthread_create(&tid, NULL, bfs, &res);
  pthread_join(tid, NULL);
  // printf("DFS = %s\n", res.res);
  strcpy(req.graph_file_name, res.res);
  struct message ret_msg = {r->mtype, r->mtype, req};
  if (msgsnd(r->sender_id, &ret_msg, sizeof(ret_msg), 0) == -1)
  {
    perror("msgsnd");
  }
  pthread_exit(NULL);
}
void handle_client_request_bfs(int client_id, int client_queue, tuple req)
{
  pthread_t tid;
  struct message data = {client_id, client_queue, req};
  pthread_create(&tid, NULL, handle_bfs, &data);
  // pthread_exit(NULL)
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
    if (msgrcv(server_queue, &msg, sizeof(msg), SERVER_2_PORT, 0) == -1)
    {
      perror("msgrcv");
      exit(1);
    }
    printf("Got tuple (%i, %i, %s) %ld\n", msg.data.operation_number, msg.data.seq_number, msg.data.graph_file_name, msg.mtype);

    if (msg.data.operation_number == 6)
    {
      printf("Exiting Server 2\n.");
      exit(0);
    }
    // Fork a new child server
    // child_pid = fork();

    // if (child_pid == -1)
    // {
    //   perror("fork");
    //   exit(1);
    // }

    // if (child_pid == 0)
    // {
    // In the child process, handle the client's request
    if (msg.data.operation_number == 3)
      handle_client_request_dfs(msg.sender_id, server_queue, msg.data);

    if (msg.data.operation_number == 4)
      handle_client_request_bfs(msg.sender_id, server_queue, msg.data);
    // }
    // else
    // {
    //   // In the parent process, continue listening for more client requests
    //   continue;
    // }
  }

  // Cleanup and exit the main server (this part will not be reached)
  msgctl(server_queue, IPC_RMID, NULL);
  return 0;
}
