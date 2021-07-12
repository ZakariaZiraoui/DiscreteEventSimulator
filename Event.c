#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    ev.Location=EL->head->Location;
    ev.Input=EL->head->Input;
    ev.Output=EL->head->Output;
    tmp = EL->head;
    EL->head = EL->head->next;
    EL->count--;
    free(tmp);
    return(ev);
}

void DisplayEventList(EventQueue *EL,FILE *report) {
   Event *ptr = EL->head;
   printf("\nEvent List [ ");
   fprintf(report,"\nEvent List [ ");
   while(ptr != NULL) {
      printf("(%d,%.2f)[%d][%d] ",ptr->Type,ptr->Time,
                            ptr->Location.x,ptr->Location.y);
      fprintf(report,"(%d,%.2f)[%d][%d] ",ptr->Type,ptr->Time,
                            ptr->Location.x,ptr->Location.y);
      ptr = ptr->next;
   }
   printf(" ]");
   fprintf(report," ]");
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

void DisplayEventInfo(Event event,FILE *report){
    char input[3],output[3];
    switch(event.Input){
        case 0: strncpy(input, "X1", 3);break;
        case 1: strncpy(input, "X2", 3);break;
        case 2: strncpy(input, "Y1", 3);break;
        case 3: strncpy(input, "Y2", 3);break;
        case 4: strncpy(input, "PE", 3);break;
        case -1: strncpy(input, "NN", 3);break;
    }
    switch(event.Output){
        case 0: strncpy(output, "X1", 3);break;
        case 1: strncpy(output, "X2", 3);break;
        case 2: strncpy(output, "Y1", 3);break;
        case 3: strncpy(output, "Y2", 3);break;
        case 4: strncpy(output, "PE", 3);break;
        case -1: strncpy(output, "NN", 3);break;
    }
    switch(event.Type){
        case 0:
            printf("\nEvent Arrival (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
            fprintf(report,"\nEvent Arrival (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
        break;
        case 1:
            printf("\nEvent DecideRoute (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
            fprintf(report,"\nEvent DecideRoute (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
        break;
        case 2:
            printf("\nEvent StartTransmit (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
            fprintf(report,"\nEvent StartTransmit (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
        break;
        case 3:
            printf("\nEvent EndTransmit (%.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
            fprintf(report,"\nEvent EndTransmit ( %.2f ns) [%d][%d] %s -> %s :",
                event.Time,event.Location.x,event.Location.y,input,output);
        break;


    }



}
