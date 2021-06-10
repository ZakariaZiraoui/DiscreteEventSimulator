#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Message.h"


typedef struct Location {
   int  x,y;
}Location;


typedef struct Node {
   struct MessageQueue *XQ1,*XQ2,*YQ1,*YQ2,*PEQ;
   int X1Req[10],X2Req[10],Y1Req[10],Y2Req[10],PEReq[10];
   int X1State,X2State,Y1State,Y2State,PEState;
   struct Node *next;
} Node;


#endif // NODE_H_INCLUDED
