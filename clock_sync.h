#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    init_message,
    echo_message
} message_type_t;

typedef struct message_struct
{
    message_type_t message_type;
    uint64_t value;
} message_t;

typedef struct node_struct
{
    int8_t id;
    uint32_t localK;
    uint16_t round;
    int initValueReceived[4];
    int echoValueReceived[4];
    bool echoSent;
    int16_t lastEcho;
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
    void sendMessage(uint8_t sender, int8_t receiver, uint8_t type, uint32_t value);
    int acceptInitK(int initValuesArray[], int value);
    int acceptEchoK(int echoValuesArray[], int value);
    int progress(int echoValuesArray[], int value);
    int catchUp(int echoValuesArray[], int value);
#ifdef __cplusplus
}
#endif
