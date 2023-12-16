#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/shm.h>
#include "graph.h"

#define SHM_KEY 1245
#define SEQ_KEY 1255
// #define SHM_SIZE sizeof(sm_data)

void write_to_sm(int **adj_mat, int n)
{
    int shmid = shmget(SHM_KEY, (n * n + 1) * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    shared_memory[i++] = n;
    for (int j = 0; j < n; ++j)
    {
        for (int k = 0; k < n; ++k)
            shared_memory[i++] = adj_mat[j][k];
    }

    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void init_seq_no()
{
    int shmid = shmget(SEQ_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    *shared_memory = 1;

    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void inc_seq_no()
{
    int shmid = shmget(SEQ_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    *shared_memory += 1;

    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

int get_seq_no()
{
    int shmid = shmget(SEQ_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    return *shared_memory;
}

struct graph_ret read_from_sm()
{
    int shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int n = *shared_memory;
    shmid = shmget(SHM_KEY, (n * n + 1) * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    int **adj_mat = (int **)malloc(n * sizeof(int **));
    int i = 1;
    for (int j = 0; j < n; ++j)
    {
        adj_mat[j] = (int *)malloc(n * sizeof(int *));
        for (int k = 0; k < n; ++k)
        {
            adj_mat[j][k] = shared_memory[i++];
        }
    }

    // Detach the shared memory segment
    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt");
        return (struct graph_ret){0, NULL};
    }

    // printf("Data read from shared memory: %d\n", data);
    return (struct graph_ret){n, adj_mat};
    // Remove the shared memory segment (optional)
    // if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    //     perror("shmctl");
    // }
}

#endif