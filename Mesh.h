#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "Config.h"
#include "Utils.h"
#include "Event.h"

Node Mat[N][M];

extern float Tnow;
extern int ClientServed,MessageID;
extern EventQueue *EL;
extern float MeanResponseTime, MeanWatingTime,Throughput;
extern Message CurrentMsg;
extern char timeStr[21];
extern FILE *report;

void Mesh ( void );
void StartingMsgMesh();
void Init ( void );
void Arrival ( int x ,int y );
void DecideRoute ( int x,int y,int Input);
void StartTransmit(int nodex,int nodey,int input, int output);
void EndTransmit (int nodex,int nodey,int input, int output);
void FreeQueues( void );


#endif // MESH_H_INCLUDED
