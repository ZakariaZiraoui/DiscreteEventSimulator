
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "Node.h"
#include "Event.h"
#include "Message.h"

#define BUSY 1
#define IDLE 0

#define N 3
#define M 3

#define PArrival 0
#define PDecideRoute 1
#define PStartTransmit 2
#define PEndTransmit 3

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



void Init ( void );
void Arrival ( int x ,int y );
void DecideRoute ( int x,int y,int Input);
void StartTransmit(int nodex,int nodey,int input, int output)
void EndTransmit (int nodex,int nodey,int input, int output);
void ReportStatistics ( void );


EventQueue *EL;
MessageQueue *Q;
Message CurrentClient;
Node Mat[3][3];

float lambda;

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
         case PArrival:      Arrival(event.Location.x,event.Location.y);      break;
         case PDecideRoute:  DecideRoute(event.Location.x,event.Location.y,event.Input); break;
         case PStartTransmit:    PDeparture();    break;
         case PEndTransmit:    PDeparture();    break;
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


void Init( void ) {

    Tnow  = 0.0;
    lambda=4000;
    MessageID = 0;
    ServerStatus=IDLE;
    printf("main");
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {

            Mat[i][j].PEQ=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].PEQ); //DisplayQueue(Q) ;
            Mat[i][j].XQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].XQ1);
            Mat[i][j].XQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].XQ2);
            Mat[i][j].YQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].YQ1);
            Mat[i][j].YQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].YQ2);

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

    float x=((float) rand() / (RAND_MAX))*(N-1);
    float y=((float) rand() / (RAND_MAX))*(M-1);
        //int x = rand() % N; int y = rand() % M;
    Location loc;
    loc.x=round(x); loc.y=round(y);
return loc;
}

void FreeQueues( void ) {
 while(!isQueueEmpty(Q)) DeQueue(Q);
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            while(!isQueueEmpty(Mat[i][j].PEQ)) DeQueue(Mat[i][j].PEQ); free(Mat[i][j].PEQ);
            while(!isQueueEmpty(Mat[i][j].XQ1)) DeQueue(Mat[i][j].XQ1); free(Mat[i][j].XQ1);
            while(!isQueueEmpty(Mat[i][j].XQ2)) DeQueue(Mat[i][j].XQ2); free(Mat[i][j].XQ2);
            while(!isQueueEmpty(Mat[i][j].YQ1)) DeQueue(Mat[i][j].YQ1); free(Mat[i][j].YQ1);
            while(!isQueueEmpty(Mat[i][j].YQ2)) DeQueue(Mat[i][j].YQ2); free(Mat[i][j].YQ2);
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
    while (loc.x==x&&loc.y==y)loc=RandDest();
    msg->Destination=&loc;
    msg->next = NULL;

    printf("\nP[%d] Arrival at %0.2f s",msg->ID,Tnow);
    Enqueue(Mat[x,y]->PEQ,msg);

    if(isHeadOfQueue(Mat[x,y]->PEQ,msg)) {
            Location loc;
            loc.x=x; loc.y=y;
       AddEvent(EL,DecideRoute,Tnow,loc,PE,NONE);
    }

    //if(ServerStatus == IDLE)    AddEvent (EL,StartService,Tnow);
    //AddEvent(EL,Arrival,Tnow+ exponentialDistribution( meanInterarrival ));

    float r = ((float) rand() / (RAND_MAX));
    AddEvent(EL,Arrival,Tnow-(lambda*log(1-r)),loc,PE,NONE);

}


void DecideRoute ( int nodex ,int nodey,int input ) {

    int Destx,Desty;

    if(input==PE){
        Destx=GetQueueHead(Mat[nodex][nodey].PEQ).Destination->x;
        Desty=GetQueueHead(Mat[nodex][nodey].PEQ).Destination->y;
    }
    if(input==X1){
        Destx=GetQueueHead(Mat[nodex][nodey].XQ1).Destination->x;
        Desty=GetQueueHead(Mat[nodex][nodey].XQ1).Destination->y;
    }
    if(input==X2){
        Destx=GetQueueHead(Mat[nodex][nodey].XQ2).Destination->x;
        Desty=GetQueueHead(Mat[nodex][nodey].XQ2).Destination->y;
    }
    if(input==Y1){
        Destx=GetQueueHead(Mat[nodex][nodey].YQ1).Destination->x;
        Desty=GetQueueHead(Mat[nodex][nodey].YQ1).Destination->y;
    }
    if(input==Y2){
        Destx=GetQueueHead(Mat[nodex][nodey].YQ2).Destination->x;
        Desty=GetQueueHead(Mat[nodex][nodey].XQ2).Destination->y;
    }
    Location loc;
    loc.x=nodex; loc.y=nodey;

    if((Destx==nodex)&&(Desty==nodey)){
        if(Mat[nodex][nodey].PEState==IDLE)
            AddEvent(EL,PStartTransmit,Tnow,loc,input,PE);
        else
             Mat[nodex][nodey].PEReq[input]=1;
    }else
        if(Destx>nodex){
            if(Mat[nodex][nodey].X1State==IDLE)
                 AddEvent(EL,PStartTransmit,Tnow,loc,input,X1);
            else
                Mat[nodex][nodey].X1Req[input]=1;
        }else
            if(Destx<nodex){
                if(Mat[nodex][nodey].X2State==IDLE)
                     AddEvent(EL,PStartTransmit,Tnow,loc,input,X2);
                else
                    Mat[nodex][nodey].X2Req[input]=1;
            }else
                if(Desty>nodey){
                    if(Mat[nodex][nodey].Y1State==IDLE)
                         AddEvent(EL,PStartTransmit,Tnow,loc,input,Y1);
                    else
                        Mat[nodex][nodey].Y1Req[input]=1;
                }else
                     if(Desty<nodey){
                        if(Mat[nodex][nodey].Y2State==IDLE)
                             AddEvent(EL,PStartTransmit,Tnow,loc,input,Y2);
                        else
                            Mat[nodex][nodey].Y2Req[input]=1;
                     }
     printf("\nP[%d,%d] DecideRoute at %0.2f s",nodex,nodey,Tnow);

    /*ServerStatus = BUSY;
    CurrentClient = DeQueue(Q);
    CurrentClient.TStartService=Tnow;

    AddEvent (EL,Departure,Tnow+CurrentClient.TServiceTime);*/
}


void StartTransmit(int nodex,int nodey,int input, int output){

    Location loc;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:
             Mat[nodex][nodey].PEState=BUSY;
             Mat[nodex][nodey].PEReq[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
          break;

        case X1:
             Mat[nodex][nodey].X1State=BUSY;
             Mat[nodex][nodey].X1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
          break;

        case X2:
             Mat[nodex][nodey].X2State=BUSY;
             Mat[nodex][nodey].X2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
          break;

        case Y1:
             Mat[nodex][nodey].Y1State=BUSY;
             Mat[nodex][nodey].Y1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
          break;

        case Y2:
             Mat[nodex][nodey].Y2State=BUSY;
             Mat[nodex][nodey].Y2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
          break;

        default: break;
    }
}

void EndTransmit(int nodex,int nodey,int nodez, int input, int output){

    if(output==PE){

     ODRing[nodex][nodey][nodez].StatusPE=idle;



 if(input==x){

//printf("crash output pe input x \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);

//printf("message destination x %d message destination y %d\n",m.Destination_X,m.Destination_Y);

 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);
 }




mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }

  if(input==xprev){

//printf("crash output pe input x \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);

//printf("message destination x %d message destination y %d\n",m.Destination_X,m.Destination_Y);

 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);
 }




mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }


 if(input==y){

//printf("crash output pe input y \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].YQueue);


 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,y,-1);
 }



mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }


  if(input==yprev){

//printf("crash output pe input y \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].YPrevQueue);


 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YPrevQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,yprev,-1);
 }



mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }



  if(input==z){

//printf("crash output pe input y \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].ZQueue);


 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].ZQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,z,-1);
 }



mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }

 if(input==zprev){

//printf("crash output pe input y \n");

  m=Msg_Pop(&ODRing[nodex][nodey][nodez].ZPrevQueue);


 if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].ZPrevQueue)==1){


    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,zprev,-1);
 }



mresponsetime=mresponsetime+(tnow-m.Arrival_Time);
//printf("response time of message %d = %f\n\n",m.ID, tnow-m.Arrival_Time);


 }


total++;

int bool1=0;

   if((ODRing[nodex][nodey][nodez].PERequests[x]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,PE); ODRing[nodex][nodey][nodez].PERequests[x]=0; bool1=1;

   }

     if((ODRing[nodex][nodey][nodez].PERequests[xprev]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,PE); ODRing[nodex][nodey][nodez].PERequests[xprev]=0; bool1=1;

   }


   if((ODRing[nodex][nodey][nodez].PERequests[y]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,y,PE); ODRing[nodex][nodey][nodez].PERequests[y]=0; bool1=1; }

  if((ODRing[nodex][nodey][nodez].PERequests[yprev]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,yprev,PE); ODRing[nodex][nodey][nodez].PERequests[yprev]=0; bool1=1; }

     if((ODRing[nodex][nodey][nodez].PERequests[z]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,z,PE); ODRing[nodex][nodey][nodez].PERequests[z]=0; bool1=1; }


     if((ODRing[nodex][nodey][nodez].PERequests[zprev]==1)&&(bool1==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,zprev,PE); ODRing[nodex][nodey][nodez].PERequests[zprev]=0; bool1=1; }

// take statistics



        }



    else if(output==x){


 ODRing[nodex][nodey][nodez].StatusX=idle;

 if (input==PE){

// printf("crash output x input pe \n");

     m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);



Msg_Put(&ODRing[nodex+1][nodey][nodez].XQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex+1][nodey][nodez].XQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex+1,nodey,nodez,x,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);

}


 }

 else if(input==x){

// printf("crash output x input x \n");

 m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);



Msg_Put(&ODRing[nodex+1][nodey][nodez].XQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex+1][nodey][nodez].XQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex+1,nodey,nodez,x,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);


 }



 int bool2=0;

if(ODRing[nodex][nodey][nodez].XRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,x); bool2=1; ODRing[nodex][nodey][nodez].XRequests[PE]=0;

}

   if((ODRing[nodex][nodey][nodez].XRequests[x]==1)&&(bool2==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,x); bool2=1; ODRing[nodex][nodey][nodez].XRequests[x]=0;

   }




}


else if(output==xprev){


 ODRing[nodex][nodey][nodez].StatusXPrev=idle;

 if (input==PE){

// printf("crash output x input pe \n");

     m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);



Msg_Put(&ODRing[nodex-1][nodey][nodez].XPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex-1][nodey][nodez].XPrevQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex-1,nodey,nodez,xprev,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);

}


 }

 else if(input==xprev){

// printf("crash output x input x \n");

 m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);



Msg_Put(&ODRing[nodex-1][nodey][nodez].XPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex-1][nodey][nodez].XPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex-1,nodey,nodez,xprev,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);


 }



 int bool2=0;

if(ODRing[nodex][nodey][nodez].XPrevRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,xprev); bool2=1; ODRing[nodex][nodey][nodez].XPrevRequests[PE]=0;

}

   if((ODRing[nodex][nodey][nodez].XPrevRequests[xprev]==1)&&(bool2==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,xprev); bool2=1; ODRing[nodex][nodey][nodez].XPrevRequests[xprev]=0;

   }




}

else if(output==y){



 ODRing[nodex][nodey][nodez].StatusY=idle;

 if (input==PE){

// printf("crash output y input pe \n");

    m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);



Msg_Put(&ODRing[nodex][nodey+1][nodez].YQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey+1][nodez].YQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex,nodey+1,nodez,y,-1);




if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);


     }

 }

 else if(input==x){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);


Msg_Put(&ODRing[nodex][nodey+1][nodez].YQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey+1][nodez].YQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey+1,nodez,y,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);


 }


  else if(input==xprev){


// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);

Msg_Put(&ODRing[nodex][nodey+1][nodez].YQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey+1][nodez].YQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey+1,nodez,y,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);


 }



  else if(input==y){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YQueue);


Msg_Put(&ODRing[nodex][nodey+1][nodez].YQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey+1][nodez].YQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey+1,nodez,y,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,y,-1);


 }


 int bool3=0;

if(ODRing[nodex][nodey][nodez].YRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,y); bool3=1; ODRing[nodex][nodey][nodez].YRequests[PE]=0;

}


   if((ODRing[nodex][nodey][nodez].YRequests[x]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,y); bool3=1; ODRing[nodex][nodey][nodez].YRequests[x]=0;

   }


    if((ODRing[nodex][nodey][nodez].YRequests[xprev]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,y); bool3=1; ODRing[nodex][nodey][nodez].YRequests[xprev]=0;

   }


    if((ODRing[nodex][nodey][nodez].YRequests[y]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,y,y); ODRing[nodex][nodey][nodez].YRequests[y]=0;

   }


}


else if(output==yprev){



 ODRing[nodex][nodey][nodez].StatusYPrev=idle;

 if (input==PE){

// printf("crash output y input pe \n");

    m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);



Msg_Put(&ODRing[nodex][nodey-1][nodez].YPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey-1][nodez].YPrevQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex,nodey-1,nodez,yprev,-1);




if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);


     }

 }

 else if(input==x){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);


Msg_Put(&ODRing[nodex][nodey-1][nodez].YPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey-1][nodez].YPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey-1,nodez,yprev,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);


 }


  else if(input==xprev){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);


Msg_Put(&ODRing[nodex][nodey-1][nodez].YPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey-1][nodez].YPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey-1,nodez,yprev,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);


 }



  else if(input==yprev){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YPrevQueue);


Msg_Put(&ODRing[nodex][nodey-1][nodez].YPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey-1][nodez].YPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey-1,nodez,yprev,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,yprev,-1);


 }


 int bool3=0;

if(ODRing[nodex][nodey][nodez].YPrevRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,yprev); bool3=1; ODRing[nodex][nodey][nodez].YPrevRequests[PE]=0;

}


   if((ODRing[nodex][nodey][nodez].YPrevRequests[x]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,yprev); bool3=1; ODRing[nodex][nodey][nodez].YPrevRequests[x]=0;

   }


    if((ODRing[nodex][nodey][nodez].YPrevRequests[xprev]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,yprev); bool3=1; ODRing[nodex][nodey][nodez].YPrevRequests[xprev]=0;

   }


    if((ODRing[nodex][nodey][nodez].YPrevRequests[yprev]==1)&&(bool3==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,yprev,yprev); ODRing[nodex][nodey][nodez].YPrevRequests[yprev]=0;

   }


}

else if(output==z){

 ODRing[nodex][nodey][nodez].StatusZ=idle;

 if (input==PE){

// printf("crash output y input pe \n");

    m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);


Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);





if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);


     }

 }

 else if(input==x){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);


Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);


 }


  else if(input==xprev){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);


Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);


 }



else if(input==y){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YQueue);

Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);

if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,y,-1);


 }


   else if(input==yprev){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YPrevQueue);

Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);

if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,yprev,-1);


 }



  else if(input==z){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].ZQueue);

Msg_Put(&ODRing[nodex][nodey][nodez+1].ZQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez+1].ZQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez+1,z,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].ZQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,z,-1);


 }




int bool4=0;

if(ODRing[nodex][nodey][nodez].ZRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[PE]=0;

}

   if((ODRing[nodex][nodey][nodez].ZRequests[x]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[x]=0;

   }

     if((ODRing[nodex][nodey][nodez].ZRequests[xprev]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[xprev]=0;

   }

   if((ODRing[nodex][nodey][nodez].ZRequests[y]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,y,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[y]=0;

   }

     if((ODRing[nodex][nodey][nodez].ZRequests[yprev]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,yprev,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[yprev]=0;

   }

if((ODRing[nodex][nodey][nodez].ZRequests[z]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,z,z); bool4=1; ODRing[nodex][nodey][nodez].ZRequests[z]=0;

   }




}


else if(output==zprev){

 ODRing[nodex][nodey][nodez].StatusZPrev=idle;

 if (input==PE){

// printf("crash output y input pe \n");

    m=Msg_Pop(&ODRing[nodex][nodey][nodez].PEQueue);


Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)

    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);





if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].PEQueue)==1){
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,PE,-1);


     }

 }

 else if(input==x){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XQueue);


Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,x,-1);


 }


  else if(input==xprev){

// printf("crash output y input x \n");
m=Msg_Pop(&ODRing[nodex][nodey][nodez].XPrevQueue);


Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);



if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].XPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,xprev,-1);


 }



else if(input==y){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YQueue);

Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);

if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,y,-1);


 }


   else if(input==yprev){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].YPrevQueue);

Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);

if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].YPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,yprev,-1);


 }



  else if(input==zprev){


// printf("crash output y input y \n");
 m=Msg_Pop(&ODRing[nodex][nodey][nodez].ZPrevQueue);

Msg_Put(&ODRing[nodex][nodey][nodez-1].ZPrevQueue,m.ID,m.Destination_X,m.Destination_Y,m.Destination_Z,m.Arrival_Time,m.Response_Time,m.Waiting_Time);

if(m.ID==Msg_getHead(&ODRing[nodex][nodey][nodez-1].ZPrevQueue).ID)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez-1,zprev,-1);


if(Msg_EmptyQueue(&ODRing[nodex][nodey][nodez].ZPrevQueue)==1)
    E_Put(&EQ,pdecideroute,tnow,nodex,nodey,nodez,zprev,-1);


 }




int bool4=0;

if(ODRing[nodex][nodey][nodez].ZPrevRequests[PE]==1){

E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,PE,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[PE]=0;

}

   if((ODRing[nodex][nodey][nodez].ZPrevRequests[x]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,x,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[x]=0;

   }

     if((ODRing[nodex][nodey][nodez].ZPrevRequests[xprev]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,xprev,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[xprev]=0;

   }

   if((ODRing[nodex][nodey][nodez].ZPrevRequests[y]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,y,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[y]=0;

   }

     if((ODRing[nodex][nodey][nodez].ZPrevRequests[yprev]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,yprev,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[yprev]=0;

   }

if((ODRing[nodex][nodey][nodez].ZPrevRequests[zprev]==1)&&(bool4==0)){
     E_Put(&EQ,pstarttransmit,tnow,nodex,nodey,nodez,zprev,zprev); bool4=1; ODRing[nodex][nodey][nodez].ZPrevRequests[zprev]=0;

   }


}

free(&m);

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



