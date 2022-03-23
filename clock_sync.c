#include "clock_sync.h"

typedef enum
{
    clock_sync,
    consensus
} test_mode_t;

/* TEST MODE */

/*
    clock_sync: output = clock sync algorithm;
    consensus: output = consenus algorithm;
*/

test_mode_t test_mode = consensus;

/* DEBUG */
bool debug_mode = false;

Node nodes[n];

void init(uint8_t id)
{
    nodes[id].id = id;
    nodes[id].local_k = 0;
    nodes[id].round = 0;
    nodes[id].echo_sent = false;
    nodes[id].last_echo = 0;
    for (int i = 0; i < n; i++)
    {
        nodes[id].init_value_received[i] = -1;
        nodes[id].echo_value_received[i] = -1;
        nodes[id].consensus_values[i] = -1;
    }
    if (debug_mode)
    {
        printf("init process: %d  \n", id);
    }
}

void start(uint8_t id)
{
    send_message(nodes[id].id, -1, 0, 0);
}

void receive(uint8_t sender, uint8_t receiver, message_t message)
{
    uint64_t *k;
    k = &nodes[receiver].local_k;

    if (message.message_type == init_message)
    {
        nodes[receiver].init_value_received[sender] = message.value;

        if (message.value == 0)
        {
            if (nodes[receiver].echo_sent)
            {
                send_message(receiver, sender, 1, nodes[receiver].last_echo);
            }
            else
            {
                send_message(receiver, sender, 0, 0);
            }
        }
        if (accept_init_k(nodes[receiver].init_value_received, *k))
        {
            send_message(receiver, -1, 1, *k);
        }
    }
    else if (message.message_type == echo_message)
    {
        nodes[receiver].echo_value_received[sender] = message.value;

        if (accept_echo_k(nodes[receiver].echo_value_received, *k))
        {
            send_message(receiver, -1, 1, *k);
        }
        if (progress(nodes[receiver].echo_value_received, *k))
        {
            *k = *k + 1;
            send_message(receiver, -1, 0, *k);
            if (*k % ROUND_LENGTH == 0)
            {
                nodes[receiver].round = *k / ROUND_LENGTH;
                if (test_mode == clock_sync)
                {
                    round_action(receiver, nodes[receiver].round, *k);
                }
                else if (test_mode == consensus)
                {
                    round_action_consensus(&nodes[receiver]);
                }
            }
        }
        if (catch_up(nodes[receiver].echo_value_received, *k))
        {
            *k = message.value;
            send_message(receiver, -1, 1, *k);
            if (*k % ROUND_LENGTH == 0)
            {
                nodes[receiver].round = *k / ROUND_LENGTH;
                if (test_mode == clock_sync)
                {
                    round_action(receiver, nodes[receiver].round, *k);
                }
                else if (test_mode == consensus)
                {
                    round_action_consensus(&nodes[receiver]);
                }
            }
        }
    }
    else if (message.message_type == consensus_message)
    {
        int valueReceived = 0;

        for (int i = 0; i < n; i++)
        {
            if (nodes[receiver].consensus_values[i] == -1)
            {
                for (int j = 0; j < n; j++)
                {
                    if (nodes[receiver].consensus_values[j] == message.value)
                    {
                        valueReceived = 1;
                    }
                }
                if (valueReceived == 0)
                {
                    nodes[receiver].consensus_values[i] = message.value;
                }
            }
        }
    }
}

void send_message(uint8_t sender, int8_t receiver, uint8_t type, uint64_t value)
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

        nodes[sender].echo_sent = true;
        nodes[sender].last_echo = value;
    }
    else if (type == 2)
    {
        message.message_type = consensus_message;
    }

    if (receiver == -1)
    {
        for (int i = 0; i < n; i++)
        {
            send(sender, i, message);
            if (debug_mode)
            {
                printf("node %d: type: %d value: %d send to: %d \n", sender, message.message_type, message.value, i);
            }
        }
    }
    else if (receiver >= 0)
    {
        send(sender, receiver, message);
        if (debug_mode)
        {
            printf("node %d: type: %d value: %d send to: %d \n", sender, message.message_type, message.value, receiver);
        }
    }
}

int accept_init_k(int64_t initValuesArray[], uint64_t value)
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

int accept_echo_k(int64_t echo_values_array[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echo_values_array[i] || value == echo_values_array[i] + 1)
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

int progress(int64_t echo_values_array[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (value == echo_values_array[i] || value == echo_values_array[i] + 1)
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

int catch_up(int64_t echo_values_array[], uint64_t value)
{
    int counter = 0;

    for (int i = 0; i < n; i++)
    {
        if (echo_values_array[i] > value)
        {
            for (int j = 0; j < n; j++)
            {
                if (i != j)
                {
                    if (abs(echo_values_array[i] - echo_values_array[j]) <= 1 || (echo_values_array[i] - echo_values_array[j]) == 0)
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