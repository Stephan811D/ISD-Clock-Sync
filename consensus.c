#include "clock_sync.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define n 4
#define lateStart 0    //round no for late start
uint8_t consensCrash = 0;         //switch for crash of node 0 --> 1 = node crashes; 0 = normal mode


void round_action(Node *node) //round action which is now called in consensus.c
{
    int round_action = 0;
    if (node->round <= 3)   //if round is smaller 3, modulo is not working
    {
        round_action = node->round - 1;
    }
    else
    {
        round_action = (node->round - 1) % 3; 
    }

    switch (round_action)       //switch through the consens finding steps
    {
        case 0:
            for (int i = 0; i < n; i++)
            {
                node->consensValues[i] = -1;  //reset the received consesus values per process
            }
            start_consens(node);
            break;
        case 1:
            broadcast(node);
            break;
        case 2:
            decision(node);
            break;
    }
}

void start_consens(Node *node)
{
    uint16_t readValue = getValue(node->id, node->round);

    node->consensValues[node->id] = readValue;
    if (node->id == 0 && consensCrash == 1)        //testing the crash of node 0
    {
        sendMessage(node->id, 1, 2, readValue);
        consensCrash = 2;
    }
    else if (node->id != 0 || ((node->round >= lateStart) && lateStart !=0))    //starting node 0 late 
    {
        sendMessage(node->id, -1, 2, readValue);
    }
}

void broadcast(Node *node)
{
    if (node->id != 0 || ((node->round >= lateStart) && lateStart !=0) || (consensCrash==0 && lateStart==0))
    {
        for (int i = 0; i < n; i++)
        {
            if (node->consensValues[i] != -1)
            {
                sendMessage(node->id, -1, 2, node->consensValues[i]); //broadcasts all received messages incluing the own value
            }
        }
    }
}

void decision(Node *node)
{
    if (node->id != 0 || ((node->round >= lateStart) && lateStart !=0) || (consensCrash==0 && lateStart==0))
    {
        uint16_t minValue = node->consensValues[node->id];

        for (int i = 0; i < n; i++)
        {
            if (node->consensValues[i] < minValue && node->consensValues[i] >= 0)
            {
                minValue = node->consensValues[i];    //create minimum of the received values
            }
        }
        //printf("round: %d ", node->round);
        set_result(node->id, minValue, node->consensValues[node->id]);
    }
}