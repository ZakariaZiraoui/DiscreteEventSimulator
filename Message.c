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

void DisplayQueue(MessageQueue *Q,int in,int x ,int y ,FILE *report) {
   Message *ptr = Q->head;
   char input[3];
    switch(in){
        case 0: strncpy(input, "X1", 3);break;
        case 1: strncpy(input, "X2", 3);break;
        case 2: strncpy(input, "Y1", 3);break;
        case 3: strncpy(input, "Y2", 3);break;
        case 4: strncpy(input, "PE", 3);break;
        case 5: strncpy(input, "E1", 3);break;
        case 6: strncpy(input, "E2", 3);break;
        case -1: strncpy(input, "NN", 3);break;
    }

   printf("\n %sQ[%d][%d] [ ",input,x,y);
   fprintf(report,"\n %sQ[%d][%d] [ ",input,x,y);
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
