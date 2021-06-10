#include <stdlib.h>
#include "Event.h"


void InitEventList(EventQueue *EL){
    EL->count = 0;
    EL->head = NULL;
    EL->rear = NULL;
}

void AddEvent(EventQueue *EL, int type,float time,Location location,int Input,int Output){
    Event *tmp,*current;
    tmp = malloc(sizeof(Event));
    tmp->Time = time;
    tmp->Type = type;
    tmp->Location=location;
    tmp->Input=Input;
    tmp->Output=Output;
    tmp->next = NULL;
    /* Special case for the head end */
    if (EL->head == NULL
        || EL->head->Time >= time) {
        tmp->next = EL->head;
        EL->head = tmp;
    }
    else {
        /* Locate the Event before the point of insertion */
        current = EL->head;
        while (current->next != NULL
               && current->next->Time <= tmp->Time) {
            current = current->next;
        }
        tmp->next = current->next;
        current->next = tmp;
    }
    EL->count++;
}

Event GetEvent(EventQueue *EL){
    Event *tmp,ev;
    ev.Time = EL->head->Time;
    ev.Type = EL->head->Type;
    tmp = EL->head;
    EL->head = EL->head->next;
    EL->count--;
    free(tmp);
    return(ev);
}

void DisplayEventList(EventQueue *EL) {
   Event *ptr = EL->head;
   printf("\nEvent List [ ");
   while(ptr != NULL) {
      printf("(%d,%.2f) ",ptr->Type,ptr->Time);
      ptr = ptr->next;
   }
   printf(" ]");
}

bool isEmpty(EventQueue *EL) {
   return (EL->head == NULL);
}

int length(EventQueue *EL) {
    int length = 0;
    Event *current;
   for(current = EL->head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}
