#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Node.h"
#include "Event.h"
#include "Message.h"
#include "Utils.h"
#include "Mesh.h"



void Mesh (){

    Init();
    //DisplayEventList(EL);
    while ( ClientServed <10 ) {
      Event event = GetEvent(EL);
      //DisplayEventInfo(event,report);
      Tnow=event.Time;
      switch ( event.Type ) {
         case PArrival:      Arrival(event.Location.x,event.Location.y);      break;
         case PDecideRoute:  DecideRoute(event.Location.x,event.Location.y,event.Input); break;
         case PStartTransmit:    StartTransmit(event.Location.x,event.Location.y,event.Input,event.Output); break;
         case PEndTransmit:    EndTransmit(event.Location.x,event.Location.y,event.Input,event.Output); break;
         default : break;
      }
   }
   FreeQueues();

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



