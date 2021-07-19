#ifndef EXPRESSCUBE_H_INCLUDED
#define EXPRESSCUBE_H_INCLUDED

#define PArrival 0
#define PDecideRoute 1
#define PStartTransmit 2
#define PEndTransmit 3

#define N 8
#define M 8
#define Hops 2

#define BUSY 1
#define IDLE 0

#define NONE -1
#define X1 0
#define X2 1
#define Y1 2
#define Y2 3
#define PE 4
#define E1 5
#define E2 6
#include "Node.h"
#include "Event.h"

NodeEx MatEx[N][M];

extern float Tnow;
extern int ClientServed,MessageID;
extern float lambda;
extern int TR,TRE;
extern EventQueue *EL;
extern float MeanResponseTime, MeanWatingTime,Throughput;
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
