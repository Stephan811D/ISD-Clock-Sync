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

int node0_echoReceived[4] = {0, 0, 0, 0};
int node1_echoReceived[4] = {0, 0, 0, 0};
int node2_echoReceived[4] = {0, 0, 0, 0};
int node3_echoReceived[4] = {0, 0, 0, 0};

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
    printf("Receive: Sender: %d Receiver: %d Type: %d Value: %d \n", sender, receiver, message.message_type, message.value);
    printf("0: %d 1: %d 2: %d 3: %d\n", node0_localK, node1_localK, node2_localK, node3_localK);

    if (message.message_type == init_message)
    {
        // uint64_t test = message.value;
        message_t echoMessage = {echo_message, message.value};
        // echoMessage.value = echoMessage.value + 2;

        switch (receiver)
        {
        case 0:
            node0_initReceived[sender] = 1;
            if (checkSecondInitReceived(node0_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node0_initReceived[i] = 0;
                }
            }
            break;
        case 1:
            node1_initReceived[sender] = 1;
            if (checkSecondInitReceived(node1_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node1_initReceived[i] = 0;
                }
            }
            break;
        case 2:
            node2_initReceived[sender] = 1;
            if (checkSecondInitReceived(node2_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node2_initReceived[i] = 0;
                }
            }
            break;
        case 3:
            node3_initReceived[sender] = 1;
            if (checkSecondInitReceived(node3_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node3_initReceived[i] = 0;
                }
            }
            break;
        }
    }
    else if (message.message_type == echo_message)
    {
        switch (receiver)
        {
        case 0:
            node0_echoReceived[sender] = 1;
            if (checkEchoReceived(node0_echoReceived, message, node0_localK))
            {
                printf("echo: receiver: %d\n", receiver);
                node0_localK = node0_localK + 1;
                message_t initMessage = {init_message, node0_localK};
                sendToAllOtherParticipants(receiver, 4, initMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node0_echoReceived[i] = 0;
                }
            }
            break;
        case 1:
            node1_echoReceived[sender] = 1;
            if (checkEchoReceived(node1_echoReceived, message, node1_localK))
            {
                printf("echo: receiver: %d\n", receiver);
                node1_localK = node1_localK + 1;
                message_t initMessage = {init_message, node1_localK};
                sendToAllOtherParticipants(receiver, 4, initMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node1_echoReceived[i] = 0;
                }
            }
            break;
        case 2:
            node2_echoReceived[sender] = 1;
            if (checkEchoReceived(node2_echoReceived, message, node2_localK))
            {
                printf("echo: receiver: %d\n", receiver);
                node2_localK = node2_localK + 1;
                message_t initMessage = {init_message, node2_localK};
                sendToAllOtherParticipants(receiver, 4, initMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node2_echoReceived[i] = 0;
                }
            }
            break;
        case 3:
            node3_echoReceived[sender] = 1;
            if (checkEchoReceived(node3_echoReceived, message, node3_localK))
            {
                printf("echo: receiver: %d\n", receiver);
                node3_localK = node3_localK + 1;
                message_t initMessage = {init_message, node3_localK};
                sendToAllOtherParticipants(receiver, 4, initMessage, true);
                for (int i = 0; i < 4; i++)
                {
                    node3_echoReceived[i] = 0;
                }
            }
            break;
        }
    }
    else
    {
        printf("wrong message_type");
    }
}

void sendToAllOtherParticipants(uint8_t sender, uint8_t participants, message_t message, bool loopback)
{
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

int checkEchoReceived(int node[], message_t message, int node_localK)
{
    int buffer = 0;
    for (int i = 0; i < 4; i++)
    {
        if (message.value == node_localK || message.value == node_localK + 1)
        {
            buffer = buffer + node[i];
        }
        // printf("FUNCTION: %d %d %d %d %d\n", node[0], node[1], node[2], node[3], buffer);
    }

    if (buffer == 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}