#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SHM_SIZE sizeof(int)

int main() {
    // Get the shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    int *shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Read an integer from the shared memory
    int data = *shared_memory;

    // Detach the shared memory segment
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    printf("Data read from shared memory: %d\n", data);

    // Remove the shared memory segment (optional)
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
