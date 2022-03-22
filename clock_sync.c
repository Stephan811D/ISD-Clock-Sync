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
    nodes[id].k = 0;
    nodes[id].round = 0;
    nodes[id].echoSent = false;
    nodes[id].lastEcho = 0;
    for (int i = 0; i < n; i++)
    {
        nodes[id].initValues[i] = -1;
        nodes[id].echoValues[i] = -1;
        nodes[id].consensValues[i] = -1;
    }
    printf("init process: %d\n", id);
}

void start(uint8_t id)
{
    sendMessage(nodes[id].id, -1, 0, 0);
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    uint64_t *k;
    k = &nodes[receiver].k; //using pointer for readability

    if (message.message_type == init_message)
    {
        nodes[receiver].initValues[sender] = message.value;

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
        if (acceptInitK(nodes[receiver].initValues, *k))
        {
            sendMessage(receiver, -1, 1, *k);
        }
    }
    else if (message.message_type == echo_message)
    {
        nodes[receiver].echoValues[sender] = message.value;

        if (acceptEchoK(nodes[receiver].echoValues, *k))
        {
            sendMessage(receiver, -1, 1, *k);
        }
        if (progress(nodes[receiver].echoValues, *k))
        {
            *k = *k + 1;
            sendMessage(receiver, -1, 0, *k);
        }
        if (catchUp(nodes[receiver].echoValues, *k))
        {
            *k = message.value;
            sendMessage(receiver, -1, 1, *k);
        }

    }
    else if (message.message_type == consens_message)
    {
        int valueReceived = 0;

        for (int i = 0; i < n; i++)
        {
            if (nodes[receiver].consensValues[i] == -1)
            {
                for (int j = 0; j < n; j++)
                {
                    if (nodes[receiver].consensValues[j] == message.value)
                    {
                        valueReceived = 1;
                    }
                }
                if (valueReceived == 0)
                {
                    nodes[receiver].consensValues[i] = message.value;
                }
            }
        }
    }
    else
    {
        printf("wrong message_type\n");
    }
    if (*k % ROUND_LENGTH == 0)
    {
        if(*k / ROUND_LENGTH > nodes[receiver].round)
        {
            round_action(&nodes[receiver]);nodes[receiver].round = *k / ROUND_LENGTH;
            
            //round_action_old(receiver, nodes[receiver].round,*k);
        }
    }
}

void sendMessage(uint8_t sender, int8_t receiver, uint8_t type, uint64_t value)
{
    message_t message;
    message.value = value;

    if (type == 0)
    {
        message.message_type = init_message;
    }
    else if (type == 1)
    {
        message.message_type = echo_message;

        nodes[sender].echoSent = true;
        nodes[sender].lastEcho = value;
    }
    else if (type == 2)
    {
        message.message_type = consens_message;
    }

    if (receiver == -1)
    {
        for (int i = 0; i < n; i++)
        {
            send(sender, i, message);
            // printf("node %d: type: %d value: %d send to: %d \n", sender, message.message_type, message.value, i);
        }
    }
    else if (receiver >= 0)
    {
        send(sender, receiver, message);
    }
}

int acceptInitK(int64_t initValuesArray[], uint64_t value)
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

int acceptEchoK(int64_t echoValuesArray[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echoValuesArray[i] || value == echoValuesArray[i] + 1)
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

int progress(int64_t echoValuesArray[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echoValuesArray[i] || value == echoValuesArray[i] + 1)
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

int catchUp(int64_t echoValuesArray[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (echoValuesArray[i] > value)
        {
            for (int j = 0; j < n; j++)
            { // check received values, if two values are same or within 1 difference
                if (i != j)
                {
                    if (abs(echoValuesArray[i] - echoValuesArray[j]) <= 1 || (echoValuesArray[i] - echoValuesArray[j]) == 0)
                    {
                        counter++;
                    }
                }
            }
        }
        if (counter == f + 1)
        {
            return 1;
        }
    }
    return 0;
}