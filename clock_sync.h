#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define f 1
#define n 4
#define ROUND_LENGTH 100

typedef enum
{
    init_message,
    echo_message,
    consensus_message
} message_type_t;

typedef struct message_struct
{
    message_type_t message_type;
    uint64_t value;
} message_t;

typedef struct node_struct
{
    // clock sync
    int8_t id;
    uint64_t local_k;
    uint16_t round;
    int64_t init_value_received[4];
    int64_t echo_value_received[4];
    bool echo_sent;
    uint64_t last_echo;

    // consensus
    int16_t consensus_values[4];
} Node;

#ifdef __cplusplus
extern "C"
{
#endif
    /* CLOCK SYNC */

    void send(uint8_t sender, uint8_t receiver, message_t message);
    void init(uint8_t id);
    void start(uint8_t id);
    void receive(uint8_t sender, uint8_t receiver, message_t message);
    void round_action(uint8_t p, uint64_t round, uint64_t clock);
    void send_message(uint8_t sender, int8_t receiver, uint8_t type, uint64_t value);
    int accept_init_k(int64_t initValuesArray[], uint64_t value);
    int accept_echo_k(int64_t echo_values_array[], uint64_t value);
    int progress(int64_t echo_values_array[], uint64_t value);
    int catch_up(int64_t echo_values_array[], uint64_t value);

    /* CONSENSUS */

    void round_action_consensus(Node *current_node);
    void send_consensus_init();
    void send_new_consensus_values();
    void consensus_decision();
    void reset_consensus_values();
    bool start_crash_condition();
    bool crash_condition();
    uint16_t read_input(uint8_t id);
    void set_result(uint8_t id, uint16_t result, uint16_t value);
#ifdef __cplusplus
}
#endif
