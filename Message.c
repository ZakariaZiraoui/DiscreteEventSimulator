#include <stdlib.h>
#include <stdio.h>
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
    if (Q->head == NULL) return ;
    Message *tmp;
    tmp = Q->head;
    return *tmp;
}

Message DeQueue(MessageQueue *Q){
    if (Q->head == NULL) return;
    Message *tmp,clt;
    clt.ID = Q->head->ID;
    clt.TArrival = Q->head->TArrival;
    clt.TResponse = Q->head->TResponse;
    clt.TWait = Q->head->TWait;
    clt.DestX = Q->head->DestX;
    clt.DestY = Q->head->DestY;
    tmp = Q->head;
    Q->head = Q->head->next;
    Q->count--;
    if (Q->head == NULL) Q->rear = NULL;
    free(tmp);
    return(clt);
}

void DisplayQueue(MessageQueue *Q,FILE *report) {
   Message *ptr = Q->head;
   printf("\nQueue [ ");
   fprintf(report,"\nQueue [ ");
   while(ptr != NULL) {
      printf("(Msg[%d],TAr:%.2f,Dest:[%d][%d])",ptr->ID,ptr->TArrival,ptr->DestX,ptr->DestY);
      fprintf(report,"(Msg[%d],TAr:%.2f,Dest:[%d][%d])",ptr->ID,ptr->TArrival,ptr->DestX,ptr->DestY);
      ptr = ptr->next;
   }
   printf(" ]");
   fprintf(report," ]");
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
