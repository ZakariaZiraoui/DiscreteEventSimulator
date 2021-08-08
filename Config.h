#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

//Simulator Parameters
#define Topo 0  // 0 For Mesh 1 For ExpressCube
#define N 8     //Number of Nodes in X Dimension
#define M 8     //Number of Nodes in Y Dimension
#define Hops 2  //Number of Hops Only For ExpressCube
#define TR 32   //Transmission Time Normal Links
#define TRE 32  //Transmission Time Express Links

//WorkLoad variables
#define Lambda 80 //Injection Rate

//Simulation Parameters
#define Max 100 //Max Number of Clients Reached
#define BatchMeansMethod 0  //Use The Batch Means Method


//Implementation Configuration
//DO NOT TOUCH

#define MESH 0
#define EXPRESS 1

#define PArrival 0
#define PDecideRoute 1
#define PStartTransmit 2
#define PEndTransmit 3

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

#endif // CONFIG_H_INCLUDED
