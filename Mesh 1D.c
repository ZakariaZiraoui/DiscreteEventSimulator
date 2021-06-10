#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define BUSY 1
#define IDLE 0

#define N 3
#define M 3

#define Arrival 0
#define DecideRoute 1
#define StartTransmit 2
#define EndTransmit 3

#define NONE -1
#define X1 0
#define X2 1
#define Y1 2
#define Y2 3
#define PE 4

// Simulation clock
float Tnow;

// State variables
int ServerStatus;

//WorkLoad variables
float ArrivalTime,ServiceTime;

// Statistical counters
float MeanResponseTime, MeanWatingTime;
int ClientServed,MessageID;

struct Location {
   int  x,y;
};
typedef struct Location Location;

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

struct Message {
   int  ID;
   float TArrival,TStartService,TServiceTime;
   Location Destination;
   struct Message *next;
};
typedef struct Message Message;

struct MessageQueue{
     int count;
     Message *rear;
     Message *head;
};
typedef struct MessageQueue MessageQueue;

struct Node {
   MessageQueue *XQ1,*XQ2,*YQ1,*YQ2,*PEQ;
   int XT1[10],XT2[10]?YT1[10],YT2[10],PET[10];
   float Time;
   struct Node *next;
};
typedef struct Node Node;

void Init ( void );
void Arrival ( void );
void PStartService ( void );
void PDeparture ( void );
void DecideRoute ( void );
void StartTransmit ( void );
void EndTransmit ( void );
void ReportStatistics ( void );

void InitEventList(EventQueue *EL);
Event GetEvent(EventQueue *EL);
void AddEvent(EventQueue *EL, int type,float time);
void DisplayEventList(EventQueue *EL);
bool isEmpty(EventQueue *EL);
int length(EventQueue *EL);

void InitQueue (MessageQueue *Q);
Message DeQueue(MessageQueue *Q);
void Enqueue(MessageQueue *Q, Message *clt);
void DisplayQueue(MessageQueue *Q);
bool isQueueEmpty(MessageQueue *Q);
int Queuelength(MessageQueue *Q);

EventQueue *EL;
MessageQueue *Q;
Message CurrentClient;
Node Mat[3][3];

int main ( ) {

    srand(time(NULL));
    EL = malloc(sizeof(EventQueue));
    InitEventList(EL);

    Init();
    DisplayEventList(EL);

    while ( ClientServed <=100 ) {//  EL->count 00000
      Event event = GetEvent(EL);
      //printf("\n\nGetting Event From The EvenList (%d %.2f ):",event.Type,event.Time);
      //DisplayEventList(EL);
      //DisplayQueue(Q);
      Tnow=event.Time;
      switch ( event.Type ) {
         case Arrival:      Arrival(event.Location.x,event.Location.y);      break;
         case DecideRoute: DecideRoute(event.Location.x,event.Location.y,) break;
         case StartTransmit:    PDeparture();    break;
         case EndTransmit:    PDeparture();    break;
         default : break;
      }
   }

    ReportStatistics();
    while(!isEmpty(EL)) GetEvent(EL);// To Free All The Allocated Memory
    //while(!isQueueEmpty(Q)) DeQueue(Q);
    //DisplayQueue(Q); free(Q);
    DisplayEventList(EL);

    free(EL);
    FreeQueues();
   return 0;
}

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

void Init( void ) {

    Tnow  = 0.0;
    MessageID = 0;
    ServerStatus=IDLE;

    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {

            Mat[i,j].PEQ=malloc(sizeof(MessageQueue));    InitQueue(Mat[i,j].PEQ); //DisplayQueue(Q) ;
            Mat[i,j].XQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i,j].XQ1);
            Mat[i,j].XQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i,j].XQ2);
            Mat[i,j].YQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i,j].YQ1);
            Mat[i,j].YQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i,j].YQ2);

             //scanf("%d", &disp[i][j]);
          }
    }

    Location loc;
    loc.x=0; loc.y=0;
    AddEvent(EL,Arrival,Tnow,loc,NONE,NONE); //First primary event

    //Statistic Vars
    MeanResponseTime=0.0;
    MeanWatingTime=0.0;
    ClientServed=0;
}

Location RandDest(){
    int x = rand() % N;
    int y = rand() % M;
    Location loc;
    loc.x=x; loc.y=y;
return loc;
}

void FreeQueues( void ) {
 while(!isQueueEmpty(Q)) DeQueue(Q);
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            while(!isQueueEmpty(Mat[i,j].PEQ)) DeQueue(Mat[i,j].PEQ); free(PEQ);
            while(!isQueueEmpty(Mat[i,j].XQ1)) DeQueue(Mat[i,j].XQ1); free(XQ1);
            while(!isQueueEmpty(Mat[i,j].XQ2)) DeQueue(Mat[i,j].XQ2); free(XQ2);
            while(!isQueueEmpty(Mat[i,j].YQ1)) DeQueue(Mat[i,j].YQ1); free(YQ1);
            while(!isQueueEmpty(Mat[i,j].YQ2)) DeQueue(Mat[i,j].YQ2); free(YQ2);
          }
    }
}

void Arrival ( int x,int y ) {

    Message *msg;
    MessageID++;
    msg = malloc(sizeof(Message));
    msg->ID = MessageID;
    msg->TArrival = Tnow;
    msg->TServiceTime = ServiceTime;
    msg->TStartService = 0;
    Location loc=RandDest();
    msg->Destination=loc;
    msg->next = NULL;

    printf("\nP[%d] Arrival at %0.2f s",msg.ID,Tnow);
    Enqueue(Mat[x,y].PEQ,msg);

    if(isHeadOfQueue(Mat[x,y].PEQ,msg)) {
            Location loc;
            loc.x=x; loc.y=y;
       AddEvent(EL,DecideRoute,Tnow,loc,PE,NONE);
       }

    //if(ServerStatus == IDLE)    AddEvent (EL,StartService,Tnow);
    //AddEvent(EL,Arrival,Tnow+ exponentialDistribution( meanInterarrival ));

    AddEvent(EL,Arrival,Tnow+ ArrivalTime,NONE,NONE);
}


void DecideRoute ( int x ,int y,int input ) {
    ServerStatus = BUSY;
    CurrentClient = DeQueue(Q);
    CurrentClient.TStartService=Tnow;
    printf("\nP[%d] StartService at %0.2f s",CurrentClient.ID,Tnow);
    AddEvent (EL,Departure,Tnow+CurrentClient.TServiceTime);
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

void ReportStatistics ( void ) {
   printf("\n\nEnd Of The Simulation at T= %0.2f s",Tnow);
   printf("\nArrival Time : %0.2f s   Service Time : %0.2f s",ArrivalTime,ServiceTime);
   printf("\nNumber of Clients Served : %d",ClientServed);
   printf("\nThe Mean Response Time : %0.2f s",MeanResponseTime/ClientServed);
   printf("\nThe Mean Waiting  Time : %0.2f s",MeanWatingTime/ClientServed);
   printf("\nThroughput : %0.2f ",ClientServed/Tnow);
}



