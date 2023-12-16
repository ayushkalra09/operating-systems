#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "settings.h"
#include "tuple.h"
#include <sys/shm.h>
#include "shared_mem.h"
#include <semaphore.h>
// #define SHM_KEY 1234
// #define SHM_SIZE sizeof(sm_data) //assume this for this
// Define the message structure
struct message
{
  long mtype;
  long sender_id;
  tuple data;
};

int main()
{
  int client_id;
  int server_queue;
  key_t server_key;
  int sequence_number = 1;
  struct message msg;

  // Create a key for the message queue
  server_key = ftok("load_balancer.c", SERVER_KEY);

  // Create a message queue
  server_queue = msgget(server_key, 0666);

  if (server_queue == -1)
  {
    perror("msgget");
    exit(1);
  }

  // Get the client ID from the user
  printf("Enter Client-ID: ");
  scanf("%d", &client_id);

  while (1)
  {
    int choice;
    printf("Client %d Menu:\n", client_id);
    printf("1. Add a new graph to the database\n");
    printf("2. Modify an existing graph of the database\n");
    printf("3. Perform DFS on an existing graph of the database\n");
    printf("4. Perform BFS on an existing graph of the database\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    // Prepare the message to send to the server
    // memset(msg.mtext, 0, sizeof(msg.mtext));
    msg.mtype = LOAD_BALANCER_PORT;
    msg.sender_id = client_id;
    int **mat;
    int n;
    switch (choice)
    {
    case 1:
      msg.data.operation_number = 1;
      printf("Enter graph file name\n");
      scanf("%s", msg.data.graph_file_name);
      // int n;
      printf("Enter number of nodes\n");
      scanf(" %i", &n);
      mat = (int **)malloc(n * sizeof(int **));
      printf("Enter the elements of the adj matrix\n");
      for (int i = 0; i < n; ++i)
      {
        mat[i] = (int *)malloc(n * sizeof(int *));
        for (int j = 0; j < n; ++j)
          scanf("%i", &mat[i][j]);
      }

      write_to_sm(mat, n);
      printf("Written to shared memory\n");
      msg.data.seq_number = get_seq_no();
      inc_seq_no();
      // Send the message to the server
      if (msgsnd(server_queue, &msg, sizeof(msg), 0) == -1)
      {
        perror("msgsnd");
        exit(1);
      }
      break;
    case 2:
      msg.data.operation_number = 2;
      printf("Enter graph file name\n");
      scanf("%s", msg.data.graph_file_name);
      // int n;
      printf("Enter number of nodes\n");
      scanf(" %i", &n);
      mat = (int **)malloc(n * sizeof(int **));
      printf("Enter the elements of the adj matrix\n");
      for (int i = 0; i < n; ++i)
      {
        mat[i] = (int *)malloc(n * sizeof(int *));
        for (int j = 0; j < n; ++j)
          scanf("%i", &mat[i][j]);
      }

      write_to_sm(mat, n);
      printf("Written to shared memory\n");
      msg.data.seq_number = get_seq_no();
      inc_seq_no();
      // Send the message to the server
      if (msgsnd(server_queue, &msg, sizeof(msg), 0) == -1)
      {
        perror("msgsnd");
        exit(1);
      }
      break;
    case 3:
      msg.data.operation_number = 3;
      printf("Enter graph file name\n");
      scanf("%s", msg.data.graph_file_name);
      msg.data.seq_number = get_seq_no();
      inc_seq_no();
      // Send the message to the server
      if (msgsnd(server_queue, &msg, sizeof(msg), 0) == -1)
      {
        perror("msgsnd");
        exit(1);
      }

      if (msgrcv(server_queue, &msg, sizeof(msg), client_id, 0) == -1)
      {
        perror("msgrcv");
        exit(1);
      }

      printf("Received data:\n%s", msg.data.graph_file_name);
      break;
    case 4:
      msg.data.operation_number = 4;
      printf("Enter graph file name\n");
      scanf("%s", msg.data.graph_file_name);
      msg.data.seq_number = get_seq_no();
      inc_seq_no();
      // Send the message to the server
      if (msgsnd(server_queue, &msg, sizeof(msg), 0) == -1)
      {
        perror("msgsnd");
        exit(1);
      }

      if (msgrcv(server_queue, &msg, sizeof(msg), client_id, 0) == -1)
      {
        perror("msgrcv");
        exit(1);
      }

      printf("Received data:\n%s", msg.data.graph_file_name);
      break;
    case 5:
      exit(0);
    case 6:
      msgctl(server_queue, IPC_RMID, NULL);
      printf("Client %d exiting.\n", client_id);
      exit(0);
    default:
      printf("Invalid choice. Try again.\n");
      continue;
    }

    printf("Message sent!\n");
    // Receive and display the server's reply
    // if (msgrcv(server_queue, &msg, sizeof(msg.sender
  }

  return 0;
}