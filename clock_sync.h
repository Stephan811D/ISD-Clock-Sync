#pragma once

#include <stdint.h>
#include <stdbool.h>

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
    uint64_t localK;
    uint16_t round;
    int64_t initValueReceived[4];
    int64_t echoValueReceived[4];
    bool echoSent;
    uint64_t lastEcho;

    // consensus
    int16_t consensusValues[4];
} Node;

#ifdef __cplusplus
extern "C"
{
#endif
    void send(uint8_t sender, uint8_t receiver, message_t message);
    void init(uint8_t id);
    void start(uint8_t id);
    void receive(uint8_t sender, uint8_t receiver, message_t message);
    void round_action(uint8_t p, uint64_t round, uint64_t clock);
    void sendMessage(uint8_t sender, int8_t receiver, uint8_t type, uint64_t value);
    int acceptInitK(int64_t initValuesArray[], uint64_t value);
    int acceptEchoK(int64_t echoValuesArray[], uint64_t value);
    int progress(int64_t echoValuesArray[], uint64_t value);
    int catchUp(int64_t echoValuesArray[], uint64_t value);

    // consensus
    void set_result(uint8_t id, uint16_t result, uint16_t value);
    uint16_t read_input(uint8_t id);

    void round_action_consensus(Node *node);
    void init_send(Node *node);
    void safety_send();
    void decision();
#ifdef __cplusplus
}
#endif
