#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Message.h"

typedef struct Location {
   int  x,y;
}Location;

typedef struct Node {
   struct MessageQueue *XQ1,*XQ2,*YQ1,*YQ2,*PEQ;
   int X1Req[7],X2Req[7],Y1Req[7],Y2Req[7],PEReq[7];
   int X1State,X2State,Y1State,Y2State,PEState;
   struct Node *next;
} Node;

typedef struct NodeEx {
   struct MessageQueue *XQ1,*XQ2,*YQ1,*YQ2,*PEQ,*E1Q,*E2Q;
   int X1Req[7],X2Req[7],Y1Req[7],Y2Req[7],PEReq[7],E1Req[7],E2Req[7];
   bool HasExpressX1,HasExpressX2,HasExpressY1,HasExpressY2;
   int X1State,X2State,Y1State,Y2State,PEState,E1State,E2State;
   struct Node *next;
} NodeEx;


#endif // NODE_H_INCLUDED
