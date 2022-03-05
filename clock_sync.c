#include "clock_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init(uint8_t id)
{
    printf("init process: %d  \n", id);
    message_t message = {init_message, 0};
    sendToAllOtherParticipants(id, 3, message);
}

void start(uint8_t id)
{
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    printf("%d %d %d", sender, receiver, message);
    if (message.message_type == init_message)
    {
        // send()
        printf("\ngot a init message");
    }
    else if (message.message_type == echo_message)
    {
    }
    else
    {
        printf("wrong message_type");
    }
}

void sendToAllOtherParticipants(uint8_t sender, uint8_t participants, message_t message)
{
    for (int i = 0; i < participants + 1; i++)
    {
        if (i != sender)
        {
            printf("process %d: init send to: %d \n", sender, i);
            send(sender, i, message);
        }
    }
}