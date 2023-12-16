#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h> // Include semaphore library
#include "settings.h"
#include "tuple.h"
#include "shared_mem.h"

// Define the message structure
struct message
{
    long mtype;
    long sender_id;
    tuple data;
};

// Define a semaphore for file access synchronization
sem_t fileSemaphore;

int main()
{
    int server_queue;
    key_t server_key;
    init_seq_no();

    // Create a semaphore for file access
    sem_init(&fileSemaphore, 1, 1); // Initial value of 1 for mutual exclusion
    int writer = 0;
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
        pid_t child_pid;

        // Receive the client's request for a new child server
        struct message msg;
        printf("waiting for msg\n");
        if (msgrcv(server_queue, &msg, sizeof(msg), LOAD_BALANCER_PORT, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
            if (msg.data.operation_number == 1 || msg.data.operation_number == 2)
            {
                sem_wait(&fileSemaphore);
            }
        }
        printf("Got tuple (%i, %i, %s) %ld\n", msg.data.operation_number, msg.data.seq_number, msg.data.graph_file_name, msg.mtype);

        // Acquire the semaphore before processing the request

        if (msg.data.operation_number == 1 || msg.data.operation_number == 2)
        {
            msg.mtype = PRIMARY_SERVER_PORT;
            writer++;
            sem_post(&fileSemaphore);
        }
        else if (msg.data.operation_number == 6)
        {
            msg.mtype = PRIMARY_SERVER_PORT;
            if (msgsnd(server_queue, &msg, sizeof(msg.sender_id) + sizeof(tuple), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }

            msg.mtype = SERVER_1_PORT;
            if (msgsnd(server_queue, &msg, sizeof(msg.sender_id) + sizeof(tuple), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }

            msg.mtype = SERVER_2_PORT;
            if (msgsnd(server_queue, &msg, sizeof(msg.sender_id) + sizeof(tuple), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }

            sleep(5);

            // Release the semaphore before terminating
            sem_post(&fileSemaphore);

            msgctl(server_queue, IPC_RMID, NULL);
            printf("Load Balancer exiting.\n");
            exit(0);
        }
        else if (msg.data.seq_number % 2 == 1)
        {
            msg.mtype = SERVER_1_PORT;
        }
        else
        {
            msg.mtype = SERVER_2_PORT;
        }

        // Release the semaphore after processing the request

        if (msgsnd(server_queue, &msg, sizeof(msg.sender_id) + sizeof(tuple), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
    }

    // Cleanup and exit the main server (this part will not be reached)
    sem_destroy(&fileSemaphore); // Destroy the semaphore
    msgctl(server_queue, IPC_RMID, NULL);
    return 0;
}
