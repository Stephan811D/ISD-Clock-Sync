#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    init_message,
    echo_message,
    consens_message
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
    uint64_t k;
    uint16_t round;
    int64_t initValues[4];
    int64_t echoValues[4];
    bool echoSent;
    uint64_t lastEcho;
    int16_t consensValues[4];
} Node;

#ifdef __cplusplus
extern "C"
{
#endif
    void send(uint8_t sender, uint8_t receiver, message_t message);
    void init(uint8_t id);
    void start(uint8_t id);
    void receive(uint8_t sender, uint8_t receiver, message_t message);
    void round_action_old(uint8_t p, uint64_t round, uint64_t clock);
    void sendMessage(uint8_t sender, int8_t receiver, uint8_t type, uint64_t value);
    int initK(int64_t initValuesArray[], uint64_t value);
    int echoK(int64_t echoValuesArray[], uint64_t value);
    int progress(int64_t echoValuesArray[], uint64_t value);
    int catchUp(int64_t echoValuesArray[], uint64_t value);

    // consens
    void set_result(uint8_t id, uint16_t result, uint16_t value);
    uint16_t getValue(uint8_t id, uint16_t round);
    void round_action(Node *node);
    void start_consens(Node *node);
    void broadcast(Node *node);
    void decision(Node *node);
#ifdef __cplusplus
}
#endif
