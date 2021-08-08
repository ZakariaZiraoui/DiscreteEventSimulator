#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
   int  ID;
   float TArrival,TResponse,TWait;
   int DestX,DestY;
   struct Message *next;
} Message;

typedef struct MessageQueue{
     int count;
     Message *rear;
     Message *head;
} MessageQueue;

void InitQueue (MessageQueue *Q);
Message DeQueue(MessageQueue *Q);
bool isHeadOfQueue(MessageQueue *Q, Message *msg);
Message GetQueueHead(MessageQueue *Q);
void Enqueue(MessageQueue *Q, Message *clt);
void DisplayQueue(MessageQueue *Q,int input,int x ,int y,FILE *report);
bool isQueueEmpty(MessageQueue *Q);
int Queuelength(MessageQueue *Q);

#endif // MESSAGE_H_INCLUDED
