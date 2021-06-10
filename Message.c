#include <stdlib.h>
#include "Message.h"


void InitQueue(MessageQueue *Q){
    Q->count = 0;
    Q->head = NULL;
    Q->rear = NULL;
}

void Enqueue(MessageQueue *Q, Message *clt){
    if(!isQueueEmpty(Q)){
        Q->rear->next = clt;
        Q->rear = clt;
    }
    else{
        Q->head = Q->rear = clt;
    }
    Q->count++;
}

bool isHeadOfQueue(MessageQueue *Q, Message *msg){
    if(!isQueueEmpty(Q)){
        if(Q->head->ID==msg->ID) return true;
    }
    return false;
}

Message GetQueueHead(MessageQueue *Q) {
    if (Q->head == NULL) return;
    Message *tmp;
    tmp = Q->head;
    return *tmp;
}

Message DeQueue(MessageQueue *Q){
    if (Q->head == NULL) return;
    Message *tmp,clt;
    clt.ID = Q->head->ID;
    clt.TArrival = Q->head->TArrival;
    clt.TServiceTime = Q->head->TServiceTime;
    clt.TStartService = Q->head->TStartService;
    tmp = Q->head;
    Q->head = Q->head->next;
    Q->count--;
    if (Q->head == NULL) Q->rear = NULL;
    free(tmp);
    return(clt);
}

void DisplayQueue(MessageQueue *Q) {
   Message *ptr = Q->head;
   printf("\nQueue [ ");
   while(ptr != NULL) {
      printf("(%d,%.2f,%.2f,%.2f) ",ptr->ID,ptr->TArrival,ptr->TStartService,ptr->TServiceTime);
      ptr = ptr->next;
   }
   printf(" ]");
}

bool isQueueEmpty(MessageQueue *Q) {
   return (Q->head == NULL);
}

int Queuelength(MessageQueue *Q) {
    int length = 0;
    Message *current;
   for(current = Q->head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}
