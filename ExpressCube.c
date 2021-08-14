#include "ExpressCube.h"


void ExpressCube (){

    StartingMsgExpress();
    InitEx();
    //DisplayEventList(EL);
    while ( ClientServed <Max ) {
      Event event = GetEvent(EL);
      //DisplayEventInfo(event,report);
      Tnow=event.Time;
      switch ( event.Type ) {
         case PArrival:         ArrivalEx(event.Location.x,event.Location.y);      break;
         case PDecideRoute:     DecideRouteEx(event.Location.x,event.Location.y,event.Input); break;
         case PStartTransmit:   StartTransmitEx(event.Location.x,event.Location.y,event.Input,event.Output); break;
         case PEndTransmit:     EndTransmitEx(event.Location.x,event.Location.y,event.Input,event.Output); break;
         default : break;
      }
   }
   FreeQueuesEx();
}

void InitEx( void ) {
    Tnow  = 0.0;
    MessageID = 0;
    int i,j,NHopBtwnEdges = N-1;

    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            MatEx[i][j].PEQ=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].PEQ);
            MatEx[i][j].XQ1=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].XQ1);
            MatEx[i][j].XQ2=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].XQ2);
            MatEx[i][j].YQ1=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].YQ1);
            MatEx[i][j].YQ2=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].YQ2);
            MatEx[i][j].E1Q=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].E1Q);
            MatEx[i][j].E2Q=malloc(sizeof(MessageQueue));    InitQueue(MatEx[i][j].E2Q);
            for(int k=0;k<7;k++) {
                MatEx[i][j].PEReq[k]=IDLE;
                MatEx[i][j].X1Req[k]=IDLE;                MatEx[i][j].X2Req[k]=IDLE;
                MatEx[i][j].Y1Req[k]=IDLE;                MatEx[i][j].Y2Req[k]=IDLE;
                MatEx[i][j].E1Req[k]=IDLE;                MatEx[i][j].E2Req[k]=IDLE;
            }
             MatEx[i][j].PEState=IDLE;
             MatEx[i][j].X1State=IDLE;             MatEx[i][j].X2State=IDLE;
             MatEx[i][j].Y1State=IDLE;             MatEx[i][j].Y2State=IDLE;
             MatEx[i][j].E1State=IDLE;             MatEx[i][j].E2State=IDLE;
             MatEx[i][j].HasExpressX1=false;       MatEx[i][j].HasExpressX2=false;
             MatEx[i][j].HasExpressY1=false;       MatEx[i][j].HasExpressY2=false;
          }
    }

    for(j=0;j<M;j++) { //Putting Express Links in Lines
        for(i=0; i<N; i++){
           if(i % 2 == 0 && j % 2 == 0 && j+Hops<=NHopBtwnEdges ){
                    MatEx[i][j].HasExpressX1=true;
                    MatEx[i][j+Hops].HasExpressX2=true;
            }
            if(i % 2 == 1 && j % 2 == 1 && j+Hops<=NHopBtwnEdges){
                    MatEx[i][j].HasExpressX1=true;
                    MatEx[i][j+Hops].HasExpressX2=true;
            }
          }
    }

    for(i=0; i<N; i++) { //Putting Express Links in Columns
        for(j=0;j<M;j++)   {
           if(i % 2 == 1 && j % 2 == 0 && i+Hops<=NHopBtwnEdges ){
                    MatEx[i][j].HasExpressY1=true;
                    MatEx[i+Hops][j].HasExpressY2=true;
            }
            if(i % 2 == 0 && j % 2 == 1 && i+Hops<=NHopBtwnEdges ){
                    MatEx[i][j].HasExpressY1=true;
                    MatEx[i+Hops][j].HasExpressY2=true;
            }
          }
    }
    for(i=0; i<N; i++){//Displaying The Topology in The Console
        printf("\n\n");
        for(j=0;j<M;j++) {
            if(MatEx[i][j].HasExpressY2) printf(" Y2"); else printf("   ");
            if(MatEx[i][j].HasExpressX1&&MatEx[i][j].HasExpressX2) printf(" X2[%d][%d]X1",i,j);
                else if (MatEx[i][j].HasExpressX1)                 printf("   [%d][%d]X1",i,j);
                    else if (MatEx[i][j].HasExpressX2)             printf(" X2[%d][%d]  ",i,j);
                        else                                       printf("   [%d][%d]  ",i,j);
            if(MatEx[i][j].HasExpressY1) printf(" Y1"); else printf("   ");
        }
    }
    printf("\n");

    //Initialization of Statistic Vars
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

void ArrivalEx ( int x,int y ) {
    float rp,r;
    Message *msg;
    MessageID++;
    msg = malloc(sizeof(Message));
    msg->ID = MessageID;
    msg->TArrival = Tnow;
    msg->TResponse = 0;
    msg->TWait = 0;
    msg->TQueing = Tnow;
    Location loc;
    switch(Traffic){
        case UNIFORM:
            loc=RandDest();
            while (loc.x==x&&loc.y==y)loc=RandDest();
        break;
        case HOTSPOT:
                rp=((float) rand() / (RAND_MAX));
                if((rp>P)||((x==HotX)&&(y==HotY))){
                    loc=RandDest();
                    while ((loc.x==x&&loc.y==y)||(loc.x==HotX&&loc.y==HotY))loc=RandDest();
                }
                else{ loc.x=HotX;   loc.y=HotY;  }
         break;
        default :
            printf("\nTraffic Type Not Defined !"); exit(0); break;
    }
    msg->DestX=loc.x;
    msg->DestY=loc.y;
    msg->next = NULL;

    printf("\n ( %7.2f ) Msg[%d] Arrives in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    fprintf(report,"\n ( %7.2f ) Msg[%d] Arrives in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    Enqueue(MatEx[x][y].PEQ,msg);

    if(isHeadOfQueue(MatEx[x][y].PEQ,msg)) {
            Location loc;
            loc.x=x; loc.y=y;
       AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
    }else
    {
        printf(        "\n ( %7.2f ) Msg[%d] Not Head Of Queue in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
        fprintf(report,"\n ( %7.2f ) Msg[%d] Not Head Of Queue in PEQ[%d][%d] -> [%d][%d] ",Tnow,msg->ID,x,y,loc.x,loc.y);
    }

    r = ((float) rand() / (RAND_MAX));
    AddEvent(EL,PArrival,Tnow-(Lambda*log(1-r)),loc,PE,NONE);

}

void DecideRouteEx ( int nodex ,int nodey,int input ) {
    char in[3],out[3];
    Message msg;
    int Destx,Desty,MsgID;

    if(input==PE){
        msg=GetQueueHead(MatEx[nodex][nodey].PEQ,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "PE",3);
    }
    if(input==X1){
        msg=GetQueueHead(MatEx[nodex][nodey].XQ1,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "X1",3);
    }
    if(input==X2){
        msg=GetQueueHead(MatEx[nodex][nodey].XQ2,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "X2",3);
    }
    if(input==Y1){
        msg=GetQueueHead(MatEx[nodex][nodey].YQ1,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "Y1",3);
    }
    if(input==Y2){
        msg=GetQueueHead(MatEx[nodex][nodey].YQ2,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "Y2",3);
    }
    if(input==E1){
        msg=GetQueueHead(MatEx[nodex][nodey].E1Q,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "E1",3);
    }
    if(input==E2){
        msg=GetQueueHead(MatEx[nodex][nodey].E2Q,Tnow);
        Destx=msg.DestX;        Desty=msg.DestY;
        MsgID=msg.ID;           strncpy(in, "E2",3);
    }
    Location loc;
    loc.x=nodex; loc.y=nodey;


    if((Destx==nodex)&&(Desty==nodey)){
        if(MatEx[nodex][nodey].PEState==IDLE){
            AddEvent(EL,PStartTransmit,Tnow,loc,input,PE); strncpy(out, "PE", 3);
        }else
             MatEx[nodex][nodey].PEReq[input]=1;
    }else
        if(Destx>nodex){
            if((Destx-nodex>=Hops)&&(MatEx[nodex][nodey].HasExpressX1)) {
                if(MatEx[nodex][nodey].E1State==IDLE){
                     AddEvent(EL,PStartTransmit,Tnow,loc,input,E1);strncpy(out, "E1", 3);
                }else
                    MatEx[nodex][nodey].E1Req[input]=1;
            }else
                if(MatEx[nodex][nodey].X1State==IDLE){
                         AddEvent(EL,PStartTransmit,Tnow,loc,input,X1);strncpy(out, "X1", 3);
                }else
                    MatEx[nodex][nodey].X1Req[input]=1;
        }else
            if(Destx<nodex){
              if((nodex-Destx>=Hops)&&(MatEx[nodex][nodey].HasExpressX2)) {
                if(MatEx[nodex][nodey].E2State==IDLE){
                     AddEvent(EL,PStartTransmit,Tnow,loc,input,E2);strncpy(out, "E2", 3);
                }else
                    MatEx[nodex][nodey].E2Req[input]=1;
            }else
                if(MatEx[nodex][nodey].X2State==IDLE){
                     AddEvent(EL,PStartTransmit,Tnow,loc,input,X2); strncpy(out, "X2", 3);
                }else
                    MatEx[nodex][nodey].X2Req[input]=1;
            }else
                if(Desty>nodey){
                 if((Desty-nodey>=Hops)&&(MatEx[nodex][nodey].HasExpressY1)) {
                    if(MatEx[nodex][nodey].E1State==IDLE){
                         AddEvent(EL,PStartTransmit,Tnow,loc,input,E1);strncpy(out, "E1", 3);
                    }else
                        MatEx[nodex][nodey].E1Req[input]=1;
                }else
                    if(MatEx[nodex][nodey].Y1State==IDLE){
                         AddEvent(EL,PStartTransmit,Tnow,loc,input,Y1); strncpy(out, "Y1", 3);
                   } else
                        MatEx[nodex][nodey].Y1Req[input]=1;
                }else
                     if(Desty<nodey){
                      if((nodey-Desty>=Hops)&&(MatEx[nodex][nodey].HasExpressY2)) {
                        if(MatEx[nodex][nodey].E2State==IDLE){
                             AddEvent(EL,PStartTransmit,Tnow,loc,input,E2);strncpy(out, "E2", 3);
                        }else
                            MatEx[nodex][nodey].E2Req[input]=1;
                    }else
                        if(MatEx[nodex][nodey].Y2State==IDLE){
                             AddEvent(EL,PStartTransmit,Tnow,loc,input,Y2); strncpy(out, "Y2", 3);
                       } else
                            MatEx[nodex][nodey].Y2Req[input]=1;
                     }

        printf("\n ( %7.2f ) Msg[%d] Decides in %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,MsgID,in,nodex,nodey,Destx,Desty,out);
        fprintf(report,"\n ( %7.2f ) Msg[%d] Decides in %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,MsgID,in,nodex,nodey,Destx,Desty,out);

        //printf("\n Msg[%d] is in [%d][%d]%s -> [%d][%d] ",MsgID,nodex,nodey,in,Destx,Desty);
        //fprintf(report,"\n Msg[%d] is in [%d][%d]%s -> [%d][%d] ",MsgID,nodex,nodey,in,Destx,Desty);
        //printf("\n End DecideRoute( %0.2f s) [%d][%d] -> [%d][%d] ",Tnow,nodex,nodey,Destx,Desty);
        //fprintf(report,"\n End DecideRoute( %0.2f s) [%d][%d] -> [%d][%d] ",Tnow,nodex,nodey,Destx,Desty);
}

void StartTransmitEx(int nodex,int nodey,int input,int output){

    char in[3],out[3];
    Location loc,nextDest;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:
             MatEx[nodex][nodey].PEState=BUSY;
             MatEx[nodex][nodey].PEReq[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "PE", 3);nextDest.x=nodex; nextDest.y=nodey;
          break;

        case X1:
             MatEx[nodex][nodey].X1State=BUSY;
             MatEx[nodex][nodey].X1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X1", 3);nextDest.x=nodex+1; nextDest.y=nodey;
          break;

        case X2:
             MatEx[nodex][nodey].X2State=BUSY;
             MatEx[nodex][nodey].X2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "X2", 3);nextDest.x=nodex-1; nextDest.y=nodey;
          break;

        case Y1:
             MatEx[nodex][nodey].Y1State=BUSY;
             MatEx[nodex][nodey].Y1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y1", 3);nextDest.x=nodex; nextDest.y=nodey+1;
          break;

        case Y2:
             MatEx[nodex][nodey].Y2State=BUSY;
             MatEx[nodex][nodey].Y2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TR,loc,input,output);
             strncpy(out, "Y2", 3);nextDest.x=nodex; nextDest.y=nodey-1;
          break;

        case E1:
             MatEx[nodex][nodey].E1State=BUSY;
             MatEx[nodex][nodey].E1Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TRE,loc,input,output);
             strncpy(out, "E1", 3);
             if(MatEx[nodex][nodey].HasExpressX1){ nextDest.x=nodex+Hops; nextDest.y=nodey;}
             if(MatEx[nodex][nodey].HasExpressY1){ nextDest.x=nodex; nextDest.y=nodey+Hops;}
          break;

        case E2:
             MatEx[nodex][nodey].E2State=BUSY;
             MatEx[nodex][nodey].E2Req[input]=0;
             AddEvent(EL,PEndTransmit,Tnow+TRE,loc,input,output);
             strncpy(out, "E2", 3);
             if(MatEx[nodex][nodey].HasExpressX2){ nextDest.x=nodex-Hops; nextDest.y=nodey;}
             if(MatEx[nodex][nodey].HasExpressY2){ nextDest.x=nodex; nextDest.y=nodey-Hops;}
          break;

        default: break;
    }
    switch(input){
        case 0: strncpy(in, "X1", 3);break;        case 1: strncpy(in, "X2", 3);break;
        case 2: strncpy(in, "Y1", 3);break;        case 3: strncpy(in, "Y2", 3);break;
        case 4: strncpy(in, "PE", 3);break;        case 5: strncpy(in, "E1", 3);break;
        case 6: strncpy(in, "E2", 3);break;        default: strncpy(in, "NN",3);break;
    }
    printf("\n ( %7.2f ) Start Transmit from  %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,in,nodex,nodey,nextDest.x,nextDest.y,out);
    fprintf(report,"\n ( %7.2f ) Start Transmit from  %sQ[%d][%d] -> [%d][%d]%sQ ",Tnow,in,nodex,nodey,nextDest.x,nextDest.y,out);
}

void EndTransmitEx(int nodex,int nodey,int input, int output){
    //49 Cases in Total, 15 Cases won't happened so you will find the 34 remain cases handled below

    Message *msg;
    int boolean=0;
    Location loc,loc1;
    loc.x=nodex; loc.y=nodey;

    switch (output) {
        case PE:{
             MatEx[nodex][nodey].PEState=IDLE;
             switch(input){
                case X1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                break;
                case X2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    break;
                case Y1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ1);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].YQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                break;
                case Y2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ2);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].YQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                break;
                case E1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E1Q);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].E1Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E1,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));

                break;
                case E2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E2Q);
                    printf("\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]PEQ ",Tnow,CurrentMsg.ID,nodex,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                    if(!isQueueEmpty(MatEx[nodex][nodey].E2Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E2,NONE);
                    MeanResponseTime+=(Tnow-CurrentMsg.TArrival);
                    MeanWatingTime+=CurrentMsg.TWait;
                    printf("\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));
                    fprintf(report,"\n               Msg[%d] Response Time : %0.2f cycles   Mean Response Time : %0.2f cycles\n               Msg[%d] Waiting  Time : %0.2f cycles   Mean Waiting  Time : %0.2f cycles",
                            CurrentMsg.ID,Tnow-CurrentMsg.TArrival,MeanResponseTime/(ClientServed+1),CurrentMsg.ID,CurrentMsg.TWait,MeanWatingTime/(ClientServed+1));

                break;
             }
             ClientServed++;
             boolean=0;
             if( MatEx[nodex][nodey].PEReq[E1]==1){
                AddEvent(EL,PStartTransmit,Tnow,loc,E1,PE); MatEx[nodex][nodey].PEReq[E1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].PEReq[E2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E2,PE); MatEx[nodex][nodey].PEReq[E2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].PEReq[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,PE); MatEx[nodex][nodey].PEReq[X1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].PEReq[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,PE); MatEx[nodex][nodey].PEReq[X2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].PEReq[Y1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y1,PE); MatEx[nodex][nodey].PEReq[Y1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].PEReq[Y2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y2,PE); MatEx[nodex][nodey].PEReq[Y2]=0; boolean=1;
             }

        break;
        }
        case X1:{
             MatEx[nodex][nodey].X1State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex+1; loc1.y=nodey;
                     Enqueue(MatEx[nodex+1][nodey].XQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex+1; loc1.y=nodey;
                     Enqueue(MatEx[nodex+1][nodey].XQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
                case E1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E1Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex+1; loc1.y=nodey;
                     Enqueue(MatEx[nodex+1][nodey].XQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]X1Q",Tnow,CurrentMsg.ID,nodex+1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex+1][nodey].XQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].E1Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E1,NONE);
                break;
             }
             boolean=0;
             if((MatEx[nodex][nodey].X1Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,X1); MatEx[nodex][nodey].X1Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].X1Req[E1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E1,X1); MatEx[nodex][nodey].X1Req[E1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].X1Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,X1); MatEx[nodex][nodey].X1Req[X1]=0; boolean=1;
             }
        break;
    }
        case X2:{
             MatEx[nodex][nodey].X2State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex-1; loc1.y=nodey;
                     Enqueue(MatEx[nodex-1][nodey].XQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex-1; loc1.y=nodey;
                     Enqueue(MatEx[nodex-1][nodey].XQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
                case E2:
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].E2Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex-1; loc1.y=nodey;
                     Enqueue(MatEx[nodex-1][nodey].XQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]X2Q",Tnow,CurrentMsg.ID,nodex-1,nodey,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex-1][nodey].XQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,X2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E2Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E2,NONE);
                break;
             }
             boolean=0;
             if((MatEx[nodex][nodey].X2Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,X2); MatEx[nodex][nodey].X2Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].X2Req[E2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E2,X2); MatEx[nodex][nodey].X2Req[E2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].X2Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,X2); MatEx[nodex][nodey].X2Req[X2]=0; boolean=1;
             }

        break;
    }
        case Y1:{
             MatEx[nodex][nodey].Y1State=IDLE;
             switch(input){
                case PE:
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;
                case X1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;
                case X2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                    msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;
                case Y1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].YQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                break;
                case E1:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E1Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E1Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E1,NONE);
                break;
                case E2:
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E2Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey+1;
                     Enqueue(MatEx[nodex][nodey+1].YQ1,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]Y1Q",Tnow,CurrentMsg.ID,nodex,nodey+1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey+1].YQ1,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E2Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E2,NONE);
                break;
             }
             boolean=0;
             if((MatEx[nodex][nodey].Y1Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,Y1); MatEx[nodex][nodey].Y1Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y1Req[E1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E1,Y1); MatEx[nodex][nodey].Y1Req[E1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y1Req[E2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E2,Y1); MatEx[nodex][nodey].Y1Req[E2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y1Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,Y1); MatEx[nodex][nodey].Y1Req[X1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y1Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,Y1); MatEx[nodex][nodey].Y1Req[X2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y1Req[Y1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y1,Y1); MatEx[nodex][nodey].Y1Req[Y1]=0; boolean=1;
             }

        break;
    }
        case Y2:{
             MatEx[nodex][nodey].Y2State=IDLE;
             switch(input){
                case PE:{
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;}
                case X1:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;}
                case X2:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     //else DisplayQueue(MatEx[nodex][nodey-1].YQ2,output,loc1.x,loc1.y,report);
                    if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;}
                case Y2:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].YQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                break;}
                case E1:{//Through X Axis Only
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E1Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E1Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E1,NONE);
                break;}
                case E2:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E2Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     loc1.x=nodex; loc1.y=nodey-1;
                     Enqueue(MatEx[nodex][nodey-1].YQ2,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]Y2Q",Tnow,CurrentMsg.ID,nodex,nodey-1,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[nodex][nodey-1].YQ2,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,Y2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E2Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E2,NONE);
                break;}
             }
             boolean=0;
             if((MatEx[nodex][nodey].Y2Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,Y2); MatEx[nodex][nodey].Y2Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y2Req[E1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E1,Y2); MatEx[nodex][nodey].Y2Req[E1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y2Req[E2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E2,Y2); MatEx[nodex][nodey].Y2Req[E2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y2Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,Y2); MatEx[nodex][nodey].Y2Req[X1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y2Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,Y2); MatEx[nodex][nodey].Y2Req[X2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].Y2Req[Y2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y2,Y2); MatEx[nodex][nodey].Y2Req[Y2]=0; boolean=1;
             }
        break;
    }
        case E1:{
             MatEx[nodex][nodey].E1State=IDLE;
             switch(input){
                case PE:{
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX1){ loc1.x=nodex+Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY1){ loc1.x=nodex; loc1.y=nodey+Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E1Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E1Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;}
                case X1:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX1){ loc1.x=nodex+Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY1){ loc1.x=nodex; loc1.y=nodey+Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E1Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E1Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;}
                case X2:{//Through Y Axis Only
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     //if(MatEx[nodex][nodey].HasExpressX1) { loc1.x=nodex+Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY1) { loc1.x=nodex; loc1.y=nodey+Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E1Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E1Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;}
                case Y1:{//Through Y Axis Only
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     //if(MatEx[nodex][nodey].HasExpressX1){ loc1.x=nodex+Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY1) {loc1.x=nodex; loc1.y=nodey+Hops; }
                     Enqueue(MatEx[loc1.x][loc1.y].E1Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E1Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].YQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y1,NONE);
                break;}
                case E1: {
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E1Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX1){ loc1.x=nodex+Hops; loc1.y=nodey; }
                     if(MatEx[nodex][nodey].HasExpressY1) { loc1.x=nodex; loc1.y=nodey+Hops; }
                     Enqueue(MatEx[loc1.x][loc1.y].E1Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E1Q[%d][%d] -> [%d][%d]E1Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E1Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E1,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E1Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E1,NONE);
                break;}
             }
             boolean=0;
             if((MatEx[nodex][nodey].E1Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,E1); MatEx[nodex][nodey].E1Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E1Req[E1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E1,E1); MatEx[nodex][nodey].E1Req[E1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E1Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,E1); MatEx[nodex][nodey].E1Req[X1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E1Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,E1); MatEx[nodex][nodey].E1Req[X2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E1Req[Y1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y1,E1); MatEx[nodex][nodey].E1Req[Y1]=0; boolean=1;
             }
        break;
        }
        case E2:{
             MatEx[nodex][nodey].E2State=IDLE;
             switch(input){
                case PE:{
                     CurrentMsg=DeQueue(MatEx[nodex][nodey].PEQ);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX2)  {loc1.x=nodex-Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY2)  {loc1.x=nodex; loc1.y=nodey-Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E2Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans PEQ[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E2Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].PEQ))
                         AddEvent(EL,PDecideRoute,Tnow,loc,PE,NONE);
                break;}
                case X1:{ //Through Y Axis Only
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ1);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     //if(MatEx[nodex][nodey].HasExpressX2){ loc1.x=nodex-Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY2)  {loc1.x=nodex; loc1.y=nodey-Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E2Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X1Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E2Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ1))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X1,NONE);
                break;}
                case X2:{
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].XQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX2) {loc1.x=nodex-Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY2) {loc1.x=nodex; loc1.y=nodey-Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E2Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans X2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E2Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].XQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,X2,NONE);
                break;}
                case Y2:{//Through Y Axis Only
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].YQ2);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     //if(MatEx[nodex][nodey].HasExpressX2) { loc1.x=nodex-Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY2)  {loc1.x=nodex; loc1.y=nodey-Hops;}
                     Enqueue(MatEx[loc1.x][loc1.y].E2Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans Y2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E2Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].YQ2))
                         AddEvent(EL,PDecideRoute,Tnow,loc,Y2,NONE);
                break;}
                case E2: {
                    CurrentMsg=DeQueue(MatEx[nodex][nodey].E2Q);
                     msg = malloc(sizeof(Message));
                     msg->ID = CurrentMsg.ID;
                     msg->TArrival = CurrentMsg.TArrival;
                     msg->TResponse = CurrentMsg.TResponse;
                     msg->TWait = CurrentMsg.TWait;
                     msg->TQueing = Tnow;
                     msg->DestX=CurrentMsg.DestX;
                     msg->DestY=CurrentMsg.DestY;
                     msg->next = NULL;
                     if(MatEx[nodex][nodey].HasExpressX2) { loc1.x=nodex-Hops; loc1.y=nodey;}
                     if(MatEx[nodex][nodey].HasExpressY2) {loc1.x=nodex; loc1.y=nodey-Hops; }
                     Enqueue(MatEx[loc1.x][loc1.y].E2Q,msg);
                     printf("\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     fprintf(report,"\n ( %7.2f ) Msg[%d] Ends Trans E2Q[%d][%d] -> [%d][%d]E2Q",Tnow,CurrentMsg.ID,loc1.x,loc1.y,CurrentMsg.DestX,CurrentMsg.DestY);
                     if(isHeadOfQueue(MatEx[loc1.x][loc1.y].E2Q,msg))
                         AddEvent(EL,PDecideRoute,Tnow,loc1,E2,NONE);
                     if(!isQueueEmpty(MatEx[nodex][nodey].E2Q))
                         AddEvent(EL,PDecideRoute,Tnow,loc,E2,NONE);
                break;}
             }
             boolean=0;
             if((MatEx[nodex][nodey].E2Req[PE]==1)){
                AddEvent(EL,PStartTransmit,Tnow,loc,PE,E2); MatEx[nodex][nodey].E2Req[PE]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E2Req[E2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,E2,E2); MatEx[nodex][nodey].E2Req[E2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E2Req[X1]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X1,E2); MatEx[nodex][nodey].E2Req[X1]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E2Req[X2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,X2,E2); MatEx[nodex][nodey].E2Req[X2]=0; boolean=1;
             }
             if((MatEx[nodex][nodey].E2Req[Y2]==1)&&(boolean==0)){
                AddEvent(EL,PStartTransmit,Tnow,loc,Y2,E2); MatEx[nodex][nodey].E2Req[Y2]=0; boolean=1;
             }
        break;
        }
        default: break;
    }

     //printf("\n End EndTransmit( %0.2f s) [%d][%d] ",Tnow,nodex,nodey);
     //fprintf(report,"\n End EndTransmit( %0.2f s) [%d][%d] ",Tnow,nodex,nodey);
}

void FreeQueuesEx( void ) {
    int i, j;
    for(i=0; i<N; i++) {
          for(j=0;j<M;j++) {
            while(!isQueueEmpty(MatEx[i][j].PEQ)) DeQueue(MatEx[i][j].PEQ); free(MatEx[i][j].PEQ);
            while(!isQueueEmpty(MatEx[i][j].XQ1)) DeQueue(MatEx[i][j].XQ1); free(MatEx[i][j].XQ1);
            while(!isQueueEmpty(MatEx[i][j].XQ2)) DeQueue(MatEx[i][j].XQ2); free(MatEx[i][j].XQ2);
            while(!isQueueEmpty(MatEx[i][j].YQ1)) DeQueue(MatEx[i][j].YQ1); free(MatEx[i][j].YQ1);
            while(!isQueueEmpty(MatEx[i][j].YQ2)) DeQueue(MatEx[i][j].YQ2); free(MatEx[i][j].YQ2);
            while(!isQueueEmpty(MatEx[i][j].E1Q)) DeQueue(MatEx[i][j].E1Q); free(MatEx[i][j].E1Q);
            while(!isQueueEmpty(MatEx[i][j].E2Q)) DeQueue(MatEx[i][j].E2Q); free(MatEx[i][j].E2Q);
          }
    }
}

void StartingMsgExpress(){

	replacechar(timeStr,'-',':');
	printf("Simulation Starting Time %.24s\n",timeStr);
	printf("Welcome to Network-On-Chip ExpressCube Simulator \n");
	printf("This simulator was designed based on The Discrete Event Simulation.\n");
	printf("It's a final year project to obtain the Master Degree\n");
	printf("Student developer : Ziraoui Zakaria \n");
	printf("Supervisor: Prof. Mohamed Ould-Khaoua\n");
	printf("\nNetwork Simulation Parameters :\n");
    printf("\nTopology : %d x %d Express Cube with %d Hops",N,M,Hops);
    printf("\nNumber of Nodes  : %d ",N*M);
    printf("\nRouting : Dimension-ordered (DOR)");
    if (Traffic==UNIFORM) printf("\nTraffic : Uniform Random");
    else                  printf("\nTraffic : Hotspot [%d][%d]",HotX,HotY);
    printf("\nMessage length : 32 phits");
    printf("\nInjection Rate : %d",Lambda);
    printf("\nTransmission Time Normal  Links : %d Cycles",TR);
    printf("\nTransmission Time Express Links : %d Cycles",TRE);
	printf("\n\nPlease wait, Simulation has been started.\n");

	fprintf(report,"Simulation Starting Time %.24s\n",timeStr);
	fprintf(report,"Welcome to Network-On-Chip ExpressCube Simulator \n");
	fprintf(report,"This simulator was designed based on The Discrete Event Simulation.\n");
	fprintf(report,"It's a final year project to obtain the Master Degree\n");
	fprintf(report,"Student developer : Ziraoui Zakaria \n");
	fprintf(report,"Supervisor: Prof. Mohamed Ould-Khaoua\n");
	fprintf(report,"\nNetwork Simulation Parameters :\n");
    fprintf(report,"\nTopology : %d x %d Express Cube with %d Hops",N,M,Hops);
    fprintf(report,"\nNumber of Nodes  : %d ",N*M);
    fprintf(report,"\nRouting : Dimension-ordered (DOR)");
    if (Traffic==UNIFORM) fprintf(report,"\nTraffic : Uniform Random");
    else                  fprintf(report,"\nTraffic : Hotspot [%d][%d]",HotX,HotY);
    fprintf(report,"\nMessage length : 32 phits");
    fprintf(report,"\nInjection Rate : %d",Lambda);
    fprintf(report,"\nTransmission Time Normal  Links : %d Cycles",TR);
    fprintf(report,"\nTransmission Time Express Links : %d Cycles",TRE);
	fprintf(report,"\n\nPlease wait, Simulation has been started.\n");
}



