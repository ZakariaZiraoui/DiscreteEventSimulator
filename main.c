
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "Node.h"
#include "Event.h"
#include "Message.h"
#include "Utils.h"

#define BUSY 1
#define IDLE 0

#define N 8
#define M 8

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
float lambda=80;

//Simulation Parameters
int TR=32;

// Statistical counters
float MeanResponseTime, MeanWatingTime,Throughput;
int ClientServed,MessageID;


void Init ( void );
void Arrival ( int x ,int y );
void DecideRoute ( int x,int y,int Input);
void StartTransmit(int nodex,int nodey,int input, int output);
void EndTransmit (int nodex,int nodey,int input, int output);
void ReportStatistics ( void );

void FreeQueues( void );
Location RandDest();
void StartingMsg();


EventQueue *EL;
Message CurrentMsg;
Node Mat[N][M];

FILE *report;
char destreport[55],timeStr[24];


int main ( ){

    strcpy (destreport, "reports/Simulation Report ");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    strncpy(timeStr,asctime(tm),24);
    replacechar(timeStr,':','-');
    strncat (destreport,timeStr,24);
    strcat (destreport," .log");

    report = fopen(destreport, "w");

    if(report==NULL)  {
        printf("\nCould not report");
        return 0;
    }

    srand(time(NULL));
    EL = malloc(sizeof(EventQueue));
    InitEventList(EL);

    Init();
    //DisplayEventList(EL);

    while ( ClientServed <2500 ) {//  EL->count 00000
      Event event = GetEvent(EL);
      //DisplayEventInfo(event,report);
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
    //DisplayEventList(EL);

    free(EL);
    FreeQueues();
    fclose(report);
    int x;
     do {
        printf("\n\nPress q to exit the Program: ");
        x = getchar();
    } while (x != EOF && x != 'q');

   return 0;
}


void Init( void ) {

    Tnow  = 0.0;
    MessageID = 0;
    StartingMsg();
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            Mat[i][j].PEQ=malloc(sizeof(MessageQueue));    InitQueue(Mat[i][j].PEQ);
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



    //Statistic Vars
    MeanResponseTime=0.0;
    MeanWatingTime=0.0;
    Throughput=0.0;
    ClientServed=0;

    Location loc;
    for(i=0; i<N; i++) { //First primary event
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

    printf("\n (%7.2f ns) Msg[%d] Arrives in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    fprintf(report,"\n (%7.2f ns) Msg[%d] Arrives in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    Enqueue(Mat[x][y].PEQ,msg);

    if(isHeadOfQueue(Mat[x][y].PEQ,msg)) {
            Location loc;
            loc.x=x; loc.y=y;
       AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
    }else
    {
        printf(        "\n (%7.2f ns) Msg[%d] Not Head Of Queue in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
        fprintf(report,"\n (%7.2f ns) Msg[%d] Not Head Of Queue in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    }

    float r = ((float) rand() / (RAND_MAX));
    AddEvent(EL,PArrival,Tnow-(lambda*log(1-r)),loc,PE,NONE);

}


void DecideRoute ( int nodex ,int nodey,int input ) {
    char in[3],out[3];
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
        if(Mat[nodex][nodey].PEState==IDLE){
            AddEvent(EL,PStartTransmit,Tnow,loc,input,PE); strncpy(out, "PE", 3);
        }else
             Mat[nodex][nodey].PEReq[input]=1;
    }else
        if(Destx>nodex){
            if(Mat[nodex][nodey].X1State==IDLE){
                 AddEvent(EL,PStartTransmit,Tnow,loc,input,X1);strncpy(out, "X1", 3);
            }else
                Mat[nodex][nodey].X1Req[input]=1;
        }else
            if(Destx<nodex){
                if(Mat[nodex][nodey].X2State==IDLE){
                     AddEvent(EL,PStartTransmit,Tnow,loc,input,X2); strncpy(out, "X2", 3);
                }else
                    Mat[nodex][nodey].X2Req[input]=1;
            }else
                if(Desty>nodey){
                    if(Mat[nodex][nodey].Y1State==IDLE){
                         AddEvent(EL,PStartTransmit,Tnow,loc,input,Y1); strncpy(out, "Y1", 3);
                   } else
                        Mat[nodex][nodey].Y1Req[input]=1;
                }else
                     if(Desty<nodey){
                        if(Mat[nodex][nodey].Y2State==IDLE){
                             AddEvent(EL,PStartTransmit,Tnow,loc,input,Y2); strncpy(out, "Y2", 3);
                       } else
                            Mat[nodex][nodey].Y2Req[input]=1;
                     }
        printf("\n (%7.2f ns) Msg[%d] Decides in %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,MsgID,in,nodex,nodey,Destx,Desty,out);
        fprintf(report,"\n (%7.2f ns) Msg[%d] Decides in %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,MsgID,in,nodex,nodey,Destx,Desty,out);

        //printf("\n Msg[%d] is in [%d][%d]%s -> [%d][%d] ",MsgID,nodex,nodey,in,Destx,Desty);
        //fprintf(report,"\n Msg[%d] is in [%d][%d]%s -> [%d][%d] ",MsgID,nodex,nodey,in,Destx,Desty);
        //printf("\n End DecideRoute( %0.2f s) [%d][%d] -> [%d][%d] ",Tnow,nodex,nodey,Destx,Desty);
        //fprintf(report,"\n End DecideRoute( %0.2f s) [%d][%d] -> [%d][%d] ",Tnow,nodex,nodey,Destx,Desty);
}


void StartTransmit(int nodex,int nodey,int input,int output){

    char in[3],out[3];
    Location loc,nextDest;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:
             Mat[nodex][nodey].PEState=BUSY;
             Mat[nodex][nodey].PEReq[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "PE", 3);nextDest.x=nodex; nextDest.y=nodey;
          break;

        case X1:
             Mat[nodex][nodey].X1State=BUSY;
             Mat[nodex][nodey].X1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X1", 3);nextDest.x=nodex+1; nextDest.y=nodey;
          break;

        case X2:
             Mat[nodex][nodey].X2State=BUSY;
             Mat[nodex][nodey].X2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X2", 3);nextDest.x=nodex-1; nextDest.y=nodey;
          break;

        case Y1:
             Mat[nodex][nodey].Y1State=BUSY;
             Mat[nodex][nodey].Y1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y1", 3);nextDest.x=nodex; nextDest.y=nodey+1;
          break;

        case Y2:
             Mat[nodex][nodey].Y2State=BUSY;
             Mat[nodex][nodey].Y2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y2", 3);nextDest.x=nodex; nextDest.y=nodey-1;
          break;

        default: break;
    }
    switch(input){
        case 0: strncpy(in, "X1", 3);break;        case 1: strncpy(in, "X2", 3);break;
        case 2: strncpy(in, "Y1", 3);break;        case 3: strncpy(in, "Y2", 3);break;
        case 4: strncpy(in, "PE", 3);break;        default: strncpy(in, "NN",3);break;
    }
    printf("\n (%7.2f ns) Start Transmit from  %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,in,nodex,nodey,nextDest.x,nextDest.y,out);
    fprintf(report,"\n (%7.2f ns) Start Transmit from  %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,in,nodex,nodey,nextDest.x,nextDest.y,out);
}

void EndTransmit(int nodex,int nodey,int input, int output){
    //25 Cases in Total, 9 Cases won't be happened so you will find the 16 remain cases handled below

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
                    printf("\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));

                break;
                case X2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].XQ2);
                    printf("\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                    break;
                case Y1:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ1);
                    printf("\n (%7.2f ns) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                break;
                case Y2:
                    CurrentMsg=DeQueue(Mat[nodex][nodey].YQ2);
                    printf("\n (%7.2f ns) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    printf("\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f ns   Mean Response Time : %0.2f ns",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1));
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(Mat[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].XQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
             }
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(Mat[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                     if(!isQueueEmpty(Mat[nodex][nodey].XQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
             }
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(Mat[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(Mat[nodex][nodey].YQ1)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                break;
             }
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     else DisplayQueue(Mat[nodex][nodey-1].YQ2,report);
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
                     printf("\n (%7.2f ns) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n (%7.2f ns) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(Mat[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     if(!isQueueEmpty(Mat[nodex][nodey].YQ2)) //TODO Verify Condition
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                break;
             }
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

     //printf("\n End EndTransmit( %0.2f s) [%d][%d] ",Tnow,nodex,nodey);
     //fprintf(report,"\n End EndTransmit( %0.2f s) [%d][%d] ",Tnow,nodex,nodey);

}


void ReportStatistics ( void ) {
    printf("\n\nEnd Of The Simulation at T= %0.2f ns",Tnow);
    printf("\nInjection Rate : %0.0f",lambda);
    printf("\nNumber of Clients Served : %d",ClientServed);
    printf("\nThe Mean Response Time : %0.2f ns",MeanResponseTime/ClientServed);
    //printf("\nThe Mean Waiting  Time : %0.2f ns",MeanWatingTime/ClientServed);
    printf("\nThroughput : %0.3f ",ClientServed/Tnow);

    fprintf(report,"\n\nEnd Of The Simulation at T= %0.2f ns",Tnow);
    fprintf(report,"\nInjection Rate : %0.0f",lambda);
    fprintf(report,"\nNumber of Clients Served : %d",ClientServed);
    fprintf(report,"\nThe Mean Response Time : %0.2f ns",MeanResponseTime/ClientServed);
    //fprintf(report,"\nThe Mean Waiting  Time : %0.2f ns",MeanWatingTime/ClientServed);
    fprintf(report,"\nThroughput : %0.3f ",ClientServed/Tnow);
}


void StartingMsg(){

	replacechar(timeStr,'-',':');
	printf("Simulation Starting Time %.24s\n",timeStr);
	printf("Welcome to Network-On-Chip 2D Mesh Simulator \n");
	printf("This simulator was designed based on The Discrete Event Simulation.\n");
	printf("It's a final year project to obtain the Master Degree\n");
	printf("Student developer : Ziraoui Zakaria \n");
	printf("Supervisor: Prof. Mohamed Ould-Khaoua\n");
	printf("\nNetwork Simulation Parameters :\n");
    printf("\nTopology : %d x %d Mesh ",N,M);
    printf("\nNumber of Nodes  : %d ",N*M);
    printf("\nRouting : Dimension-ordered (DOR)");
    printf("\nTraffic : Uniform Random");
    printf("\nInjection Rate : %0.0f",lambda);
    printf("\nTransmission Time : %d ns",TR);
	printf("\n\nPlease wait, Simulation has been started.\n");

	fprintf(report,"Simulation Starting Time %.24s\n",timeStr);
	fprintf(report,"Welcome to Network-On-Chip 2D Mesh Simulator \n");
	fprintf(report,"This simulator was designed based on The Discrete Event Simulation.\n");
	fprintf(report,"It's a final year project to obtain the Master Degree\n");
	fprintf(report,"Student developer : Ziraoui Zakaria \n");
	fprintf(report,"Supervisor: Prof. Mohamed Ould-Khaoua\n");
	fprintf(report,"\nNetwork Simulation Parameters :\n");
    fprintf(report,"\nTopology : %d x %d Mesh ",N,M);
    fprintf(report,"\nNumber of Nodes  : %d ",N*M);
    fprintf(report,"\nRouting : Dimension-ordered (DOR)");
    fprintf(report,"\nTraffic : Uniform Random");
    fprintf(report,"\nInjection Rate : %0.0f",lambda);
    fprintf(report,"\nTransmission Time : %d ns",TR);
	fprintf(report,"\n\nPlease wait, Simulation has been started.\n");

}


