#include "clock_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int node0_initReceived[4] = {0, 0, 0, 0};
int node1_initReceived[4] = {0, 0, 0, 0};
int node2_initReceived[4] = {0, 0, 0, 0};
int node3_initReceived[4] = {0, 0, 0, 0};

int node0_echoReceived[4] = {0, 0, 0, 0};
int node1_echoReceived[4] = {0, 0, 0, 0};
int node2_echoReceived[4] = {0, 0, 0, 0};
int node3_echoReceived[4] = {0, 0, 0, 0};

int node0_echoValueReceived[4] = {0, 0, 0, 0};
int node1_echoValueReceived[4] = {0, 0, 0, 0};
int node2_echoValueReceived[4] = {0, 0, 0, 0};
int node3_echoValueReceived[4] = {0, 0, 0, 0};

bool node0_echoSent;
bool node1_echoSent;
bool node2_echoSent;
bool node3_echoSent;

int node0_localK = 0;
int node1_localK = 0;
int node2_localK = 0;
int node3_localK = 0;

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
    printf("K0=%d K1=%d K2=%d K3=%d\n", node0_localK, node1_localK, node2_localK, node3_localK);

    if (message.message_type == init_message)
    {
        // uint64_t test = message.value;
        message_t echoMessage = {echo_message, message.value};
        // echoMessage.value = echoMessage.value + 2;

        switch (receiver)
        {
        case 0:
            node0_initReceived[sender] = 1;
            // Zeile 5-10 Pseudocode
            if (message.value == 0 && node0_echoSent)
            {
                // re-send last (echo) to p
            }
            else
            {
                // resend init 0 to p
            }
            // Zeile 12-14 Pseudocode
            if (checkSecondInitReceived(node0_initReceived))
            {
                printf("receiver %d got 2nd init sending echo to everyone\n", receiver);
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
                node0_echoSent = true;
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
                node1_echoSent = true;
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
                node2_echoSent = true;
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
                node3_echoSent = true;
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
            node0_echoValueReceived[sender] = message.value;
            // Zeile 16-18 Pseudocode
            if (checkEchoReceived(node0_echoReceived, message, node0_localK, 2, true, node0_echoValueReceived))
            {
                printf("echo 2: receiver: %d\n", receiver);
                message_t echoMessage = {echo_message, message.value};
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
            }
            // Zeile 19-23 Pseudocode
            else if (checkEchoReceived(node0_echoReceived, message, node0_localK, 3, false, node0_echoValueReceived))
            {
                printf("echo 3: receiver: %d\n", receiver);
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
            node1_echoValueReceived[sender] = message.value;
            if (checkEchoReceived(node1_echoReceived, message, node1_localK, 2, true, node1_echoValueReceived))
            {
                printf("echo 2: receiver: %d\n", receiver);
                message_t echoMessage = {echo_message, message.value};
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
            }
            else if (checkEchoReceived(node1_echoReceived, message, node1_localK, 3, true, node1_echoValueReceived))
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
            node2_echoValueReceived[sender] = message.value;
            if (checkEchoReceived(node2_echoReceived, message, node2_localK, 2, true, node2_echoValueReceived))
            {
                printf("echo 2: receiver: %d\n", receiver);
                message_t echoMessage = {echo_message, message.value};
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
            }
            else if (checkEchoReceived(node2_echoReceived, message, node2_localK, 3, true, node2_echoValueReceived))
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
            node3_echoValueReceived[sender] = message.value;
            if (checkEchoReceived(node3_echoReceived, message, node3_localK, 2, true, node3_echoValueReceived))
            {
                printf("echo 2: receiver: %d\n", receiver);
                message_t echoMessage = {echo_message, message.value};
                sendToAllOtherParticipants(receiver, 4, echoMessage, true);
            }
            else if (checkEchoReceived(node3_echoReceived, message, node3_localK, 3, true, node3_echoValueReceived))
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

int checkEchoReceived(int node[], message_t message, int node_localK, int atLeast, bool receiverBigger, int node_echo[])
{
    int buffer = 0;
    for (int i = 0; i < 4; i++)
    {
        if ((message.value == node_localK || message.value == node_localK + 1) && !receiverBigger)
        {
            buffer = buffer + node[i];
        }
        else if ((message.value > node_localK) && receiverBigger)
        {
        }
        // printf("FUNCTION: %d %d %d %d %d\n", node[0], node[1], node[2], node[3], buffer);
    }

    if (buffer == atLeast)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
