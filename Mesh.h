#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#define PArrival 0
#define PDecideRoute 1
#define PStartTransmit 2
#define PEndTransmit 3

#define N 8
#define M 8

#define BUSY 1
#define IDLE 0

#define NONE -1
#define X1 0
#define X2 1
#define Y1 2
#define Y2 3
#define PE 4
#include "Node.h"
#include "Event.h"

Node Mat[N][M];

extern float Tnow;
extern int ClientServed,MessageID;
extern float lambda;
extern int TR;
extern EventQueue *EL;
extern float MeanResponseTime, MeanWatingTime,Throughput;
extern Message CurrentMsg;
extern FILE *report;

void Mesh ( void );
void Init ( void );
void Arrival ( int x ,int y );
void DecideRoute ( int x,int y,int Input);
void StartTransmit(int nodex,int nodey,int input, int output);
void EndTransmit (int nodex,int nodey,int input, int output);
void FreeQueues( void );



#endif // MESH_H_INCLUDED
