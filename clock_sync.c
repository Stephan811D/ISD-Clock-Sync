#include "clock_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int k[4] = 0;
int initReceived[4][4] = 0;
int participantsAmount = 3;
int currentNode = 0;

void init(uint8_t id)
{
    printf("init process: %d  \n", id);
    message_t message = {init_message, 0};
    sendToAllOtherParticipants(id, participantsAmount, message, true);
}

void start(uint8_t id)
{
    currentNode = id;
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    bool gotInitMessage = false;
    printf("message_value: %d\n", message.message_type);

    // printf("%d %d %d", sender, receiver, message);
    if (message.message_type == init_message)
    {
        if (sender != receiver)
        {
            switch (sender)
            {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            }
        }

        if (!gotInitMessage)
        {
            message_t message = {echo_message, k};
            // sendToAllOtherParticipants(sender, participantsAmount, message, true);
            // printf("\ngot a init message: sender: %d receiver: %d\n", sender, receiver);
            gotInitMessage = true;
        }
    }
    else if (message.message_type == echo_message)
    {
        if (message.value == k || message.value == k + 1)
        {
            printf("\n GOT ECHO \n");
            message_t message = {echo_message, k};
            // sendToAllOtherParticipants(sender, participantsAmount, message, true);
        }
    }
    else
    {
        printf("wrong message_type");
    }
}

void sendToAllOtherParticipants(uint8_t sender, uint8_t participants, message_t message, bool loopback)
{
    for (int i = 0; i < participants + 1; i++)
    {
        if (i != sender || loopback)
        {
            printf("process %d: %d send to: %d \n", sender, message.message_type, i);
            send(sender, i, message);
        }
    }
}