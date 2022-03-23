#include "clock_sync.h"

/* CONSENSUS - TESTCASES */

typedef enum
{
    random,
    fixed_value,
    process_id
} input_values_t;

/* NODE CRASH */

/*
    which node should crash at start and how long
*/
uint8_t node_to_crash_at_start = 0;
uint8_t node_start_rounds_to_crash = 20;

/*
    which node should crash and when
*/
uint8_t node_to_crash = 0;
uint8_t node_round_to_crash = 150;

/* INPUT VALUE  */

/*
    random: return random values;
    fixed_value: return fixed values;
    process_id: returns process id;
*/
input_values_t input_value = process_id;

/*
    max value for random input_value
*/
uint8_t random_input_values_max = 50;

Node *node;

void round_action_consensus(Node *current_node)
{
    node = current_node;

    uint8_t round_action;

    if (node->round <= 3)
    {
        round_action = node->round - 1;
    }
    else
    {
        round_action = (node->round - 1) % 3;
    }

    if (start_crash_condition() && crash_condition())
    {
        switch (round_action)
        {
        case 0:
            reset_consensus_values();
            send_consensus_init();
            break;
        case 1:
            send_new_consensus_values();
            break;
        case 2:
            consensus_decision();
            break;
        }
    }
}

void send_consensus_init()
{
    uint16_t readValue = read_input(node->id);

    node->consensus_values[node->id] = readValue;

    send_message(node->id, -1, 2, readValue);
}

void send_new_consensus_values()
{
    for (int i = 0; i < n; i++)
    {
        if (node->consensus_values[i] != -1)
        {
            send_message(node->id, -1, 2, node->consensus_values[i]);
        }
    }
}

void consensus_decision()
{
    uint16_t minValue = node->consensus_values[node->id];

    for (int i = 0; i < n; i++)
    {
        if (node->consensus_values[i] < minValue && node->consensus_values[i] >= 0)
        {
            minValue = node->consensus_values[i];
        }
    }
    set_result(node->id, minValue, node->consensus_values[node->id]);
}

void reset_consensus_values()
{
    for (int i = 0; i < n; i++)
    {
        node->consensus_values[i] = -1;
    }
}

bool start_crash_condition()
{
    if (node->id == node_to_crash_at_start && node->round <= node_start_rounds_to_crash)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool crash_condition()
{
    if (node->id == node_to_crash && node->round >= node_round_to_crash)
    {
        return false;
    }
    else
    {
        return true;
    }
}

uint16_t read_input(uint8_t id)
{
    switch (input_value)
    {
    case random:
        return rand() % random_input_values_max;
        break;
    case fixed_value:
        switch (id)
        {
        case 0:
            return 10;
            break;
        case 1:
            return 12;
            break;
        case 2:
            return 14;
            break;
        case 3:
            return 16;
            break;
        default:
            return 18;
            break;
        }
        break;
    case process_id:
        return id;
        break;
    }
}

void set_result(uint8_t id, uint16_t result, uint16_t value)
{
    printf("%d: Result: %d Value: %d\n", id, result, value);
}