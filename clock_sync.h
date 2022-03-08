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

#ifdef __cplusplus
extern "C"
{
#endif
    void send(uint8_t sender, uint8_t receiver, message_t message);
    void init(uint8_t id);
    void start(uint8_t id);
    void receive(uint8_t sender, uint8_t receiver, message_t message);
    void round_action(uint8_t p, uint64_t round, uint64_t clock);
    void sendToAllOtherParticipants(uint8_t sender, uint8_t participants, message_t message, bool loopback);
    int checkSecondInitReceived(int node[]);
#ifdef __cplusplus
}
#endif
