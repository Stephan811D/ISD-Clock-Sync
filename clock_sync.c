#include "clock_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int k = 0;

int node0_initReceived[4] = {0, 0, 0, 0};
int node1_initReceived[4] = {0, 0, 0, 0};
int node2_initReceived[4] = {0, 0, 0, 0};
int node3_initReceived[4] = {0, 0, 0, 0};

int node0_localK = 0;
int node1_localK = 0;
int node2_localK = 0;
int node3_localK = 0;

int participantsAmount = 3;

void init(uint8_t id)
{
    printf("init process: %d  \n", id);
    message_t message = {init_message, 0};

    sendToAllOtherParticipants(id, 4, message, true);
}

void start(uint8_t id)
{
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    // printf("value %d\n", message.value);
    // printf("type %d\n", message.message_type);

    printf("Receive: Sender: %d Receiver: %d Type: %d Value: %d \n", sender, receiver, message.message_type, message.value);

    if (message.message_type == init_message)
    {
        // uint64_t test = message.value;
        message_t message = {echo_message, message.value};

        switch (receiver)
        {
        case 0:
            node0_initReceived[sender] = 1;
            if (checkSecondInitReceived(node0_initReceived))
            {

                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                send(0, 1, message);

                // sendToAllOtherParticipants(receiver, 4, message, true);
            }
            break;
        case 1:
            node1_initReceived[sender] = 1;
            if (checkSecondInitReceived(node1_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, message, true);
            }
            break;
        case 2:
            node2_initReceived[sender] = 1;
            if (checkSecondInitReceived(node2_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, message, true);
            }
            break;
        case 3:
            node3_initReceived[sender] = 1;
            if (checkSecondInitReceived(node3_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, message, true);
            }
            break;
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
    printf("HIER: %d", message.value);

    for (int i = 0; i < participants; i++)
    {
        if (i != sender || loopback)
        {
            printf("node %d: type: %d value: %d send to: %d \n", sender, message.message_type, message.value, i);
            send(sender, i, message);
        }
    }
}

int checkSecondInitReceived(int node[])
{
    int buffer = 0;
    for (int i = 0; i < 4; i++)
    {
        buffer = buffer + node[i];
        // printf("FUNCTION: %d %d %d %d %d\n", node[0], node[1], node[2], node[3], buffer);
    }

    if (buffer == 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}