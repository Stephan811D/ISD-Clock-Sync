#include "clock_sync.h"

#define n 4
bool crash = false;

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

    switch (round_action)
    {
    case 0:
        for (int i = 0; i < n; i++)
        {
            node->consensusValues[i] = -1;
        }
        init_send(node);
        break;
    case 1:
        safety_send(node);
        break;
    case 2:
        decision(node);
        break;
    }
}

// ROUND 1
void init_send(Node *node)
{
    uint16_t readValue = read_input(node->id);

    node->consensusValues[node->id] = readValue;
    if (node->id == 0 && crash == 0)
    {
        sendMessage(node->id, 1, 2, readValue);
        crash = 1;
    }
    else if (node->id != 0 || node->round >= 12)
    {
        sendMessage(node->id, -1, 2, readValue);
    }
}

// ROUND 2
void safety_send(Node *node)
{
    if (node->id != 0 || node->round >= 12)
    {
        for (int i = 0; i < n; i++)
        {
            if (node->consensusValues[i] != -1)
            {
                sendMessage(node->id, -1, 2, node->consensusValues[i]);
            }
        }
    }
}

// ROUND 3
void decision(Node *node)
{
    if (node->id != 0 || node->round >= 12)
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
        set_result(node->id, minValue, node->consensusValues[node->id]);
    }
}