#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Node.h"

struct Event {
   int  Type;
   float Time;
   Location Location;
   int Input,Output;
   struct Event *next;
};
typedef struct Event Event;

struct EventQueue{
     int count;
     Event *rear;
     Event *head;
};
typedef struct EventQueue EventQueue;

void InitEventList(EventQueue *EL);
Event GetEvent(EventQueue *EL);
void AddEvent(EventQueue *EL, int type,float time,Location location,int Input,int Output);
void DisplayEventList(EventQueue *EL,FILE *report);
bool isEmpty(EventQueue *EL);
int length(EventQueue *EL);
void DisplayEventInfo(Event event,FILE *report);

#endif // EVENT_H_INCLUDED
