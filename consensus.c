#include "clock_sync.h"
#include <stdio.h>
#include <stdint.h>

#define n 4

void round_action_consensus(Node *node)
{
    int round_action;

    if (node->round <= 3)
    {
        round_action = node->round - 1;
    }
    else
    {
        round_action = (node->round - 1) % 3;
    }

    // printf("%d\n", round_action);

    switch (round_action)
    {
    case 0:
        init_send(node);
        break;
    case 1:
        safety_send(node);
        break;
    case 2:
        decision(node);
        break;
    }

    printf("P=%d R=%d V1=%d V2=%d V3=%d V4=%d\n", node->id, node->round, node->consensusValues[0], node->consensusValues[1], node->consensusValues[2], node->consensusValues[3]);
}

// ROUND 1
void init_send(Node *node)
{
    uint16_t readValue = read_input(node->id);

    node->consensusValues[node->id] = readValue;
    sendMessage(node->id, -1, 2, readValue);

    // printf("ID: %d  INPUT:%d\n", node->id, readValue);
}

// ROUND 2
void safety_send(Node *node)
{
    for (int i = 0; i < n; i++)
    {
        if (node->consensusValues[i] != -1)
        {
            // printf("%d\n", node->consensusValues[i]);
            sendMessage(node->id, -1, 2, node->consensusValues[i]);
        }
    }
}

// ROUND 3
void decision(Node *node)
{
    uint16_t minValue = node->consensusValues[node->id];

    for (int i = 0; i < n; i++)
    {
        // printf("HERE\n");
        if (node->consensusValues[i] < minValue && node->consensusValues[i] >= 0)
        {

            minValue = node->consensusValues[i];
        }
    }

    // set_result(node->id, minValue, node->consensusValues[node->id]);
}