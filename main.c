
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "Node.h"
#include "Event.h"
#include "Message.h"

#define BUSY 1
#define IDLE 0

#define N 2
#define M 2

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

//WorkLoad variables
float ServiceTime;
float lambda;

// Statistical counters
float MeanResponseTime, MeanWatingTime,Throughput;
int ClientServed,MessageID;
int TR=10;

void Init ( void );
void Arrival ( int x ,int y );
void DecideRoute ( int x,int y,int Input);
void StartTransmit(int nodex,int nodey,int input, int output);
void EndTransmit (int nodex,int nodey,int input, int output);
void ReportStatistics ( void );

void FreeQueues( void );
Location RandDest();


EventQueue *EL;
Message CurrentMsg;
Node Mat[N][M];


int main ( ) {

    srand(time(NULL));
    EL = malloc(sizeof(EventQueue));
    InitEventList(EL);

    Init();
    DisplayEventList(EL);

    while ( ClientServed <100 ) {//  EL->count 00000
      Event event = GetEvent(EL);
      DisplayEventInfo(event);

      //DisplayEventList(EL);
      Tnow=event.Time;
      switch ( event.Type ) {
         case PArrival:      Arrival(event.Location.x,event.Location.y);      break;
         case PDecideRoute:  DecideRoute(event.Location.x,event.Location.y,event.Input); break;
         case PStartTransmit:    StartTransmit(event.Location.x,event.Location.y,event.Input,event.Output); break;
         case PEndTransmit:    EndTransmit(event.Location.x,event.Location.y,event.Input,event.Output); break;
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
    int x;
     do {
        printf("\n\nPress q to exit the Program: ");
        x = getchar();
    } while (x != EOF && x != 'q');
   return 0;
}


void Init( void ) {

    Tnow  = 0.0;
    lambda=4000;
    MessageID = 0;
    printf("Init");
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            Mat[i][j].PEQ=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].PEQ); //DisplayQueue(Q) ;
            Mat[i][j].XQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].XQ1);
            Mat[i][j].XQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].XQ2);
            Mat[i][j].YQ1=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].YQ1);
            Mat[i][j].YQ2=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].YQ2);
            for(int k=0;k<10;k++) {
                Mat[i][j].PEReq[k]=IDLE;
                Mat[i][j].X1Req[k]=IDLE;
                Mat[i][j].X2Req[k]=IDLE;
                Mat[i][j].Y1Req[k]=IDLE;
                Mat[i][j].Y2Req[k]=IDLE;
            }
             Mat[i][j].PEState=IDLE;
             Mat[i][j].X1State=IDLE;
             Mat[i][j].X2State=IDLE;
             Mat[i][j].Y1State=IDLE;
             Mat[i][j].Y2State=IDLE;
          }
    }

    Location loc;
    //loc.x=0; loc.y=0;
    //AddEvent(EL,Arrival,Tnow,loc,NONE,NONE); //First primary event


    //Statistic Vars
    MeanResponseTime=0.0;
    MeanWatingTime=0.0;
    Throughput=0.0;
    ClientServed=0;

    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
                loc.x=i; loc.y=j;
            AddEvent(EL,PArrival,Tnow,loc,PE,NONE);
          }
      }
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
 //while(!isQueueEmpty(Q)) DeQueue(Q);
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
    msg->TResponse = 0;
    msg->TWait = 0;
    Location loc=RandDest();
    while (loc.x==x&&loc.y==y)loc=RandDest();
    msg->DestX=loc.x;
    msg->DestY=loc.y;
    msg->next = NULL;

    printf("\n Msg[%d] Arrives in [%d][%d]->[%d][%d] PEQ",msg->ID,x,y,loc.x,loc.y);
    Enqueue(Mat[x][y].PEQ,msg);

    if(isHeadOfQueue(Mat[x][y].PEQ,msg)) {
            Location loc;
            loc.x=x; loc.y=y;
       AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
    }

    float r = ((float) rand() / (RAND_MAX));
    AddEvent(EL,PArrival,Tnow-(lambda*log(1-r)),loc,PE,NONE);

}


void DecideRoute ( int nodex ,int nodey,int input ) {
    char in[3];
    Message msg;
    int Destx,Desty,MsgID;

    if(input==PE){
        msg=GetQueueHead(Mat[nodex][nodey].PEQ);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "PE", 3);
    }
    if(input==X1){
        msg=GetQueueHead(Mat[nodex][nodey].XQ1);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "X1", 3);
    }
    if(input==X2){
        msg=GetQueueHead(Mat[nodex][nodey].XQ2);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "X2", 3);
    }
    if(input==Y1){
        msg=GetQueueHead(Mat[nodex][nodey].YQ1);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "Y1", 3);
    }
    if(input==Y2){
        msg=GetQueueHead(Mat[nodex][nodey].YQ2);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "Y2", 3);
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
        printf("\n Msg[%d] is in [%d][%d]%s -> [%d][%d] ",MsgID,nodex,nodey,in,Destx,Desty);
        printf("\n End DecideRoute( %0.2f s) [%d][%d] -> [%d][%d] ",Tnow,nodex,nodey,Destx,Desty);
}


void StartTransmit(int nodex,int nodey,int input, int output){

     //printf("\nP[%d,%d] StartTransmit [%d]->[%d] at %0.2f s",nodex,nodey,input,output,Tnow);
    char in[3],out[3];
    Location loc;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:
             Mat[nodex][nodey].PEState=BUSY;
             Mat[nodex][nodey].PEReq[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "PE", 3);
          break;

        case X1:
             Mat[nodex][nodey].X1State=BUSY;
             Mat[nodex][nodey].X1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X1", 3);
          break;

        case X2:
             Mat[nodex][nodey].X2State=BUSY;
             Mat[nodex][nodey].X2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X2", 3);
          break;

        case Y1:
             Mat[nodex][nodey].Y1State=BUSY;
             Mat[nodex][nodey].Y1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y1", 3);
          break;

        case Y2:
             Mat[nodex][nodey].Y2State=BUSY;
             Mat[nodex][nodey].Y2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y2", 3);
          break;

        default: break;
    }
   // printf("\n End StartTransmit( %0.2f s) [%d][%d] -> Output %s ",Tnow,nodex,nodey,out);
}

void EndTransmit(int nodex,int nodey,int input, int output){
   // printf("\nP[%d,%d] EndTransmit [%d]->[%d] at %0.2f s",nodex,nodey,input,output,Tnow);
    Message *msg;
    int boolean=0;
    Location loc,loc1;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:
             Mat[nodex][nodey].PEState=IDLE;
             switch(input){
                case X1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ1);
                    printf("\n Msg[%d] is in [%d][%d] X1->PE",CurrentMsg.ID,nodex,nodey);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n Response Time Msg[%d] : %0.2f ns",CurrentMsg.ID,Tnow-CurrentMsg.TArrival);
                    printf("\n Mean Response Time After Msg[%d] : %0.2f ns",CurrentMsg.ID,MeanResponseTime/(ClientServed+1));
                break;
                case X2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ2);
                    printf("\n Msg[%d] is in [%d][%d] X2->PE",CurrentMsg.ID,nodex,nodey);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n Response Time Msg[%d] : %0.2f ns",CurrentMsg.ID,Tnow-CurrentMsg.TArrival);
                    printf("\n Mean Response Time After Msg[%d] : %0.2f ns",CurrentMsg.ID,MeanResponseTime);
                break;
                case Y1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ1);
                    printf("\n Msg[%d] is in [%d][%d] Y1->PE",CurrentMsg.ID,nodex,nodey);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n Response Time Msg[%d] : %0.2f ns",CurrentMsg.ID,Tnow-CurrentMsg.TArrival);
                    printf("\n Mean Response Time After Msg[%d] : %0.2f ns",CurrentMsg.ID,MeanResponseTime);
                break;
                case Y2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ2);
                    printf("\n Msg[%d] is in [%d][%d] Y2->PE",CurrentMsg.ID,nodex,nodey);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n Response Time Msg[%d] : %0.2f ns",CurrentMsg.ID,Tnow-CurrentMsg.TArrival);
                    printf("\n Mean Response Time After Msg[%d] : %0.2f ns",CurrentMsg.ID,MeanResponseTime);
                break;
             }
             ClientServed++;
             boolean=0;
             if((Mat[nodex][nodey].PEReq[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,PE); Mat[nodex][nodey].PEReq[X1]=0; boolean=1;
             }
             if((Mat[nodex][nodey].PEReq[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,PE); Mat[nodex][nodey].PEReq[X2]=0; boolean=1;
             }
             if((Mat[nodex][nodey].PEReq[Y1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y1,PE); Mat[nodex][nodey].PEReq[Y1]=0; boolean=1;
             }
             if((Mat[nodex][nodey].PEReq[Y2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y2,PE); Mat[nodex][nodey].PEReq[Y2]=0; boolean=1;
             }
        break;

        case X1:
             Mat[nodex][nodey].X1State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(Mat[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex+1; loc1.y=nodey;
                     Enqueue(Mat[nodex+1][nodey].XQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] PE->X1",CurrentMsg.ID,nodex+1,nodey);
                     if(isHeadOfQueue(Mat[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].PEQ)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex+1; loc1.y=nodey;
                     Enqueue(Mat[nodex+1][nodey].XQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] X1->X1",CurrentMsg.ID,nodex+1,nodey);
                     if(isHeadOfQueue(Mat[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
             }
             //ClientServed++;
             boolean=0;
             if((Mat[nodex][nodey].X1Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,X1); Mat[nodex][nodey].X1Req[PE]=0; boolean=1;
             }
             if((Mat[nodex][nodey].X1Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,X1); Mat[nodex][nodey].X1Req[X1]=0; boolean=1;
             }
        break;

        case X2:
             Mat[nodex][nodey].X2State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(Mat[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex-1; loc1.y=nodey;
                     Enqueue(Mat[nodex-1][nodey].XQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] PE->X2",CurrentMsg.ID,nodex-1,nodey);
                     if(isHeadOfQueue(Mat[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].PEQ)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex-1; loc1.y=nodey;
                     Enqueue(Mat[nodex-1][nodey].XQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] X2->X2",CurrentMsg.ID,nodex-1,nodey);
                     if(isHeadOfQueue(Mat[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
             }
             //ClientServed++;
             boolean=0;
             if((Mat[nodex][nodey].X2Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,X2); Mat[nodex][nodey].X2Req[PE]=0; boolean=1;
             }
             if((Mat[nodex][nodey].X2Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,X2); Mat[nodex][nodey].X2Req[X2]=0; boolean=1;
             }
        break;

        case Y1:
             Mat[nodex][nodey].Y1State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(Mat[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(Mat[nodex][nodey+1].YQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] PE->Y1",CurrentMsg.ID,nodex,nodey+1);
                     if(isHeadOfQueue(Mat[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].PEQ)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(Mat[nodex][nodey+1].YQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] X1->Y1",CurrentMsg.ID,nodex,nodey+1);
                     if(isHeadOfQueue(Mat[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
                case X2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ2);
                    msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(Mat[nodex][nodey+1].YQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] X2->Y1",CurrentMsg.ID,nodex,nodey+1);
                     if(isHeadOfQueue(Mat[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
                case Y1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(Mat[nodex][nodey+1].YQ1,msg);
                     printf("\n Msg[%d] is in [%d][%d] Y1->Y1",CurrentMsg.ID,nodex,nodey+1);
                     if(isHeadOfQueue(Mat[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                break;
             }
             //ClientServed++;
             boolean=0;
             if((Mat[nodex][nodey].Y1Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,Y1); Mat[nodex][nodey].Y1Req[PE]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y1Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,Y1); Mat[nodex][nodey].Y1Req[X1]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y1Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,Y1); Mat[nodex][nodey].Y1Req[X2]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y1Req[Y1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y1,Y1); Mat[nodex][nodey].Y1Req[Y1]=0; boolean=1;
             }
        break;

        case Y2:
             Mat[nodex][nodey].Y2State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(Mat[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(Mat[nodex][nodey-1].YQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] PE->Y2",CurrentMsg.ID,nodex,nodey-1);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].PEQ)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ1);
                    msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(Mat[nodex][nodey-1].YQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] X1->Y2",CurrentMsg.ID,nodex,nodey-1);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
                case X2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(Mat[nodex][nodey-1].YQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] X2->Y2",CurrentMsg.ID,nodex,nodey-1);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     else DisplayQueue(Mat[nodex][nodey-1].YQ2);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
                case Y2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(Mat[nodex][nodey-1].YQ2,msg);
                     printf("\n Msg[%d] is in [%d][%d] Y2->Y2",CurrentMsg.ID,nodex,nodey-1);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                break;
             }
             //ClientServed++;
             boolean=0;
             if((Mat[nodex][nodey].Y2Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,Y2); Mat[nodex][nodey].Y2Req[PE]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y2Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,Y2); Mat[nodex][nodey].Y2Req[X1]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y2Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,Y2); Mat[nodex][nodey].Y2Req[X2]=0; boolean=1;
             }
             if((Mat[nodex][nodey].Y2Req[Y2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y2,Y2); Mat[nodex][nodey].Y2Req[Y2]=0; boolean=1;
             }
        break;


        default: break;
    }

     printf("\n End EndTransmit( %0.2f s) [%d][%d] ",Tnow,nodex,nodey);

}


void ReportStatistics ( void ) {
    printf("\n\nEnd Of The Simulation at T= %0.2f ns",Tnow);
    printf("\nInjection Rate : %f",lambda);
    printf("\nNumber of Clients Served : %d",ClientServed);
    printf("\nThe Mean Response Time : %0.2f ns",MeanResponseTime/ClientServed);
    printf("\nThe Mean Waiting  Time : %0.2f ns",MeanWatingTime/ClientServed);
    printf("\nThroughput : %0.2f ",ClientServed/Tnow);
}



