#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock_sync.h"

#define n 4

typedef struct message_queue_struct
{
    uint8_t sender;
    uint8_t receiver;
    uint64_t delivery_time;
    message_t message;
    struct message_queue_struct *next;
} message_queue_t;

message_queue_t *messages = NULL;
uint64_t now = 0;
uint64_t round_len = 100;

void insert_message(message_queue_t *new)
{
    message_queue_t *cur = messages, *last = NULL;
    while (cur != NULL && cur->delivery_time < new->delivery_time)
    {
        last = cur;
        cur = cur->next;
    }
    new->next = cur;
    if (last == NULL)
        messages = new;
    else
        last->next = new;
}

void send(uint8_t sender, uint8_t receiver, message_t message)
{
    message_queue_t *new = (message_queue_t *)malloc(sizeof(message_queue_t));
    memset(new, 0x00, sizeof(message_queue_t));
    new->sender = sender;
    new->receiver = receiver;
    new->message = message;
    new->delivery_time = now + rand() % ((9 * round_len) / 10) + 1;
    new->next = NULL;

    // printf("%d \n", rand() % n);
    // printf("%d \n", receiver);

    // if (!(receiver == rand() % n))
    //{
    insert_message(new);
    //}
}

void round_action(uint8_t p, uint64_t round, uint64_t clock)
{
    printf("%d: %lld %lld %lld\n", p, round, clock, now);
}

int main()
{
    for (uint8_t i = 0; i < 4; i++)
        init(i);

    for (uint8_t i = 0; i < 4; i++)
        start(i);

    do
    {
        while (messages != NULL && messages->delivery_time == now)
        {
            message_queue_t *message = messages;
            messages = messages->next;

            // if (!(message->receiver == rand() % n))
            {
                receive(message->sender, message->receiver, message->message);
            }

            free(message);
        }
        now++;
        // printf("%llu\n", now);
    } while (messages != NULL && now < 10000 * round_len);
}

// uint64_t