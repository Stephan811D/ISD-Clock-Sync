#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


#include "clock_sync.h"

#define n 4
bool lateStart=0, crash=0, messageDrop=0;


typedef struct message_queue_struct
{
    uint8_t sender;
    uint8_t receiver;
    uint64_t delivery_time;
    message_t message;
    struct message_queue_struct *next;
} message_queue_t;

message_queue_t *messages = NULL;
uint64_t now = 0, i = 0;
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

    // Test cases
    if(messageDrop)//drop random messages
    {
        int randomnumber = rand() % 10; //generate random number between 1 and 10
        if ((1 != randomnumber))
        {
            insert_message(new); //if TRUE (probability 1:10) --> drop message
        }
    }
    else if (lateStart) //start process late
    {
        if (! (message.value<4 && (receiver==3 || sender==3) )) //start process3 after k >3
        {
            insert_message(new);
        }    
    }
    else if (crash)//process crash
    {
        if (! (message.value>150 && (receiver==1 || sender==1) )) //stop process1 after k >150
        {
            insert_message(new);
        }
    }
    else
    {
        insert_message(new);
    }    
}

void round_action_old(uint8_t p, uint64_t round, uint64_t clock)
{
    printf("%d: %lld %lld %lld\n", p, round, clock, now);
}

uint16_t getValue(uint8_t id, uint16_t rd)
{
    int temp = 0;
    switch (id)
    {
    case 0:
        temp = rd;
        break;
    case 1:
        temp = rd+2;
        break;
    case 2:
        temp = rd+4;
        break;
    case 3:
        temp = rd+6;
        break;
    default:
        break;
    }

    return temp;
}

void set_result(uint8_t id, uint16_t result, uint16_t value)
{
    printf("Id %d: Result: %d Value: %d\n", id, result, value);
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
            i++;
            message_queue_t *message = messages;
            messages = messages->next;
            receive(message->sender, message->receiver, message->message);

            free(message);
        }
        now++;
        // printf("%llu\n", now);
    } while (messages != NULL && now < 10000 * round_len);
    // printf("messages: %d now:%d \n",messages,now);
}