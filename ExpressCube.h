#ifndef EXPRESSCUBE_H_INCLUDED
#define EXPRESSCUBE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "Utils.h"
#include "Config.h"
#include "Event.h"

NodeEx MatEx[N][M];

extern float Tnow,rp;
extern int ClientServed,MessageID,Lambda,Batch;
extern EventQueue *EL;
extern float MeanResponseTime, MeanWaitingTime,Throughput;
extern Message CurrentMsg;
extern char timeStr[21];
extern FILE *report;

void ExpressCube ( void );
void StartingMsgExpress();
void InitEx ( void );
void ArrivalEx ( int x ,int y );
void DecideRouteEx ( int x,int y,int Input);
void StartTransmitEx(int nodex,int nodey,int input, int output);
void EndTransmitEx (int nodex,int nodey,int input, int output);
void FreeQueuesEx ( void );


#endif // EXPRESSCUBE_H_INCLUDED
