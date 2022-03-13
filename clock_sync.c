#include "clock_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define f 1
#define n 4
#define ROUND_LENGTH 100

Node nodes[n];

void init(uint8_t id)
{
    nodes[id].id = id;
    nodes[id].localK = 0;
    nodes[id].round = 0;
    nodes[id].echoSent = false;
    nodes[id].lastEcho = 0;
    for (int i = 0; i < n; i++)
    {
        nodes[id].initValueReceived[i] = -1;
        nodes[id].echoValueReceived[i] = -1;
    }
    printf("init process: %d  \n", id);
}

void start(uint8_t id)
{
    // sendMessage(id, -1, 0, 0);
    sendMessage(nodes[id].id, -1, 0, 0);
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    // printf("Receive: Sender: %d Receiver: %d Type: %d Value: %d \n", sender, receiver, message.message_type, message.value);
    // printf("K0=%d K1=%d K2=%d K3=%d\n", nodes[0].localK, nodes[1].localK, nodes[2].localK, nodes[3].localK);

    // uint32_t k = nodes[receiver].localK;
    if (message.message_type == init_message)
    {
        nodes[receiver].initValueReceived[sender] = message.value;

        if (message.value == 0)
        {
            if (nodes[receiver].echoSent)
            {
                sendMessage(receiver, sender, 1, nodes[receiver].lastEcho);
            }
            else
            {
                sendMessage(receiver, sender, 0, 0);
            }
        }
        if (acceptInitK(nodes[receiver].initValueReceived, nodes[receiver].localK))
        {
            sendMessage(receiver, -1, 1, nodes[receiver].localK);
        }
    }
    else if (message.message_type == echo_message)
    {
        nodes[receiver].echoValueReceived[sender] = message.value;
        if (acceptEchoK(nodes[receiver].echoValueReceived, nodes[receiver].localK))
        {
            sendMessage(receiver, -1, 1, nodes[receiver].localK);
        }
        if (progress(nodes[receiver].echoValueReceived, nodes[receiver].localK))
        {
            nodes[receiver].localK = nodes[receiver].localK + 1;
            sendMessage(receiver, -1, 0, nodes[receiver].localK);
            if (nodes[receiver].localK % ROUND_LENGTH == 0)
            {
                nodes[receiver].round = nodes[receiver].localK / ROUND_LENGTH;
                // nodes[receiver].round++;
                round_action(receiver, nodes[receiver].round, nodes[receiver].localK);
            }
        }
        if (catchUp(nodes[receiver].echoValueReceived, nodes[receiver].localK))
        {
            nodes[receiver].localK = message.value;
            sendMessage(receiver, -1, 1, nodes[receiver].localK);
            if (nodes[receiver].localK % ROUND_LENGTH == 0)
            {
                nodes[receiver].round = nodes[receiver].localK / ROUND_LENGTH;
                // nodes[receiver].round++;
                round_action(receiver, nodes[receiver].round, nodes[receiver].localK);
            }
        }
    }
    else
    {
        printf("wrong message_type");
    }
}

void sendMessage(uint8_t sender, int8_t receiver, uint8_t type, uint32_t value)
{
    message_t message;
    message.value = value;

    if (type == 0)
    {
        message.message_type = init_message;
    }
    else
    {
        message.message_type = echo_message;

        nodes[sender].echoSent = true;
        nodes[sender].lastEcho = value;
    }

    if (receiver == -1)
    {
        for (int i = 0; i < 4; i++)
        {
            send(sender, i, message);
            // printf("node %d: type: %d value: %d send to: %d \n", sender, message.message_type, message.value, i);
        }
    }
    else
    {
        send(sender, receiver, message);
    }
}

int acceptInitK(int initValuesArray[], int value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == initValuesArray[i])
        {
            counter++;
        }
        if (counter == f + 1)
        {
            return 1;
        }
    }
    return 0;
}

int acceptEchoK(int echoValuesArray[], int value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echoValuesArray[i] || value + 1 == echoValuesArray[i])
        {
            counter++;
        }
        if (counter == f + 1)
        {
            return 1;
        }
    }
    return 0;
}

int progress(int echoValuesArray[], int value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echoValuesArray[i] || value + 1 == echoValuesArray[i])
        {
            counter++;
        }
        if (counter == n - f)
        {
            return 1;
        }
    }
    return 0;
}

int catchUp(int echoValuesArray[], int value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (echoValuesArray[i] > value)
        {
            if (value == echoValuesArray[i] || value + 1 == echoValuesArray[i])
            {
                counter++;
            }
            if (counter == f + 1)
            {
                return 1;
            }
        }
    }
    return 0;
}