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
    char inp = 'n';
    while (inp == 'n' || inp == 'N')
    {
        printf("Want to terminate the application? Press Y (Yes) or N (No)\n");
        scanf(" %c", &inp);
        if (inp == 'n' || inp == 'N')
            continue;
        if (inp == 'y' || inp == 'Y')
        {
            tuple data = {get_seq_no(), 6, ""};
            msg = (struct message){LOAD_BALANCER_PORT, -1, data};
            if (msgsnd(server_queue, &msg, sizeof(msg), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }
        }
        else
        {
            printf("Only enter Y/N\n");
            inp = 'n';
        }
    }
    return 0;
}
