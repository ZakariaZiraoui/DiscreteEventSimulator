#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUSY 1
#define IDLE 0

#define Arrival 0
#define StartService 1
#define Departure 2

// Simulation clock
float Tnow;

// State variables
int ServerStatus;

//WorkLoad variables
float ArrivalTime,ServiceTime;

// Statistical counters
float MeanResponseTime, MeanWatingTime;
int ClientServed,ClientID;

struct Client {
   int  ID;
   float TArrival,TStartService,TServiceTime;
   struct Client *next;
};
typedef struct Client Client;

struct Event {
   int  Type;
   float Time;
   struct Event *next;
};
typedef struct Event Event;

struct EventList{
     int count;
     Event *rear;
     Event *head;
};
typedef struct EventList EventList;

struct Queue{
     int count;
     Client *rear;
     Client *head;
};
typedef struct Queue Queue;

void Init ( void );
void PArrival ( void );
void PStartService ( void );
void PDeparture ( void );
void Statistics ( void );

void InitEventList(EventList *EL);
Event GetEvent(EventList *EL);
void AddEvent(EventList *EL, int type,float time);
void DisplayEventList(EventList *EL);
bool isEmpty(EventList *EL);
int length(EventList *EL);

void InitQueue (Queue *Q);
Client DeQueue(Queue *Q);
void Enqueue(Queue *Q, Client *clt);
void DisplayQueue(Queue *Q);
bool isQueueEmpty(Queue *Q);
int Queuelength(Queue *Q);

EventList *EL;
Queue *Q;
Client CurrentClient;

int main ( ) {

    EL = malloc(sizeof(EventList));
    InitEventList(EL);

    Q = malloc(sizeof(Queue));
    InitQueue(Q);
    DisplayQueue(Q);

    Init();
    DisplayEventList(EL);

    while ( ClientServed <=100 ) {//  EL->count 00000
      Event event = GetEvent(EL);
      //printf("\n\nGetting Event From The EvenList (%d %.2f ):",event.Type,event.Time);
      //DisplayEventList(EL);
      //DisplayQueue(Q);
      Tnow=event.Time;
      switch ( event.Type ) {
         case Arrival:      PArrival();      break;
         case StartService: PStartService(); break;
         case Departure:    PDeparture();    break;
         default : break;
      }
   }

    Statistics();
    while(!isEmpty(EL)) GetEvent(EL);// To Free All The Allocated Memory
    while(!isQueueEmpty(Q)) DeQueue(Q);
    DisplayEventList(EL);
    DisplayQueue(Q);
    free(EL);
    free(Q);
   return 0;
}

void InitEventList(EventList *EL){
    EL->count = 0;
    EL->head = NULL;
    EL->rear = NULL;
}

void AddEvent(EventList *EL, int type,float time){
    Event *tmp,*current;
    tmp = malloc(sizeof(Event));
    tmp->Time = time;
    tmp->Type = type;
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

Event GetEvent(EventList *EL){
    Event *tmp,ev;
    ev.Time = EL->head->Time;
    ev.Type = EL->head->Type;
    tmp = EL->head;
    EL->head = EL->head->next;
    EL->count--;
    free(tmp);
    return(ev);
}

void DisplayEventList(EventList *EL) {
   Event *ptr = EL->head;
   printf("\nEvent List [ ");
   while(ptr != NULL) {
      printf("(%d,%.2f) ",ptr->Type,ptr->Time);
      ptr = ptr->next;
   }
   printf(" ]");
}

bool isEmpty(EventList *EL) {
   return (EL->head == NULL);
}

int length(EventList *EL) {
    int length = 0;
    Event *current;
   for(current = EL->head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}

void InitQueue(Queue *Q){
    Q->count = 0;
    Q->head = NULL;
    Q->rear = NULL;
}

void Enqueue(Queue *Q, Client *clt){
    if(!isQueueEmpty(Q)){
        Q->rear->next = clt;
        Q->rear = clt;
    }
    else{
        Q->head = Q->rear = clt;
    }
    Q->count++;
}

Client DeQueue(Queue *Q){
    if (Q->head == NULL) return;
    Client *tmp,clt;
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

void DisplayQueue(Queue *Q) {
   Client *ptr = Q->head;
   printf("\nQueue [ ");
   while(ptr != NULL) {
      printf("(%d,%.2f,%.2f,%.2f) ",ptr->ID,ptr->TArrival,ptr->TStartService,ptr->TServiceTime);
      ptr = ptr->next;
   }
   printf(" ]");
}

bool isQueueEmpty(Queue *Q) {
   return (Q->head == NULL);
}

int Queuelength(Queue *Q) {
    int length = 0;
    Client *current;
   for(current = Q->head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}

void Init( void ) {

    ClientID = 0;
    ServerStatus=IDLE;
    Tnow  = 0.0;

    ArrivalTime=4;
    ServiceTime=5;

    AddEvent(EL,Arrival,Tnow); //First primary event

    //Statistic Vars
    MeanResponseTime=0.0;
    MeanWatingTime=0.0;
    ClientServed=0;
}

void PArrival ( void ) {
    printf("\nP[%d] Arrival at %0.2f s",ClientID+1,Tnow);
    Client *clt;
    ClientID++;
    clt = malloc(sizeof(Client));
    clt->ID = ClientID;
    clt->TArrival = Tnow;
    clt->TServiceTime = ServiceTime;
    clt->TStartService = 0;
    clt->next = NULL;

    Enqueue(Q,clt);
    if(ServerStatus == IDLE)    AddEvent (EL,StartService,Tnow);
    //AddEvent(EL,Arrival,Tnow+ exponentialDistribution( meanInterarrival ));

    AddEvent(EL,Arrival,Tnow+ ArrivalTime);
}

void PStartService ( void ) {
    ServerStatus = BUSY;
    CurrentClient = DeQueue(Q);
    CurrentClient.TStartService=Tnow;
    printf("\nP[%d] StartService at %0.2f s",CurrentClient.ID,Tnow);
    AddEvent (EL,Departure,Tnow+CurrentClient.TServiceTime);
}

void PDeparture ( void ) {
    printf("\nP[%d] Departure  at %0.2f s",CurrentClient.ID,Tnow);
    ServerStatus=IDLE;
    MeanResponseTime= MeanResponseTime+Tnow-CurrentClient.TArrival;
    MeanWatingTime = MeanWatingTime + CurrentClient.TStartService - CurrentClient.TArrival;
    ClientServed++;
    //free(CurrentClient); Already in  DeQueue

    if(!isQueueEmpty(Q)) AddEvent(EL,StartService,Tnow);
}

void Statistics ( void ) {
   printf("\n\nEnd Of The Simulation at T= %0.2f s",Tnow);
   printf("\nArrival Time : %0.2f s   Service Time : %0.2f s",ArrivalTime,ServiceTime);
   printf("\nNumber of Clients Served : %d",ClientServed);
   printf("\nThe Mean Response Time : %0.2f s",MeanResponseTime/ClientServed);
   printf("\nThe Mean Waiting  Time : %0.2f s",MeanWatingTime/ClientServed);
   printf("\nThroughput : %0.2f ",ClientServed/Tnow);
}



