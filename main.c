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
#include "Mesh.h"

// Simulation clock
float Tnow;

// State variables

//WorkLoad variables
float lambda=200;

//Simulation Parameters
int TR=32;

// Statistical counters
float MeanResponseTime, MeanWatingTime,Throughput;
int ClientServed,MessageID;

void Reporting ();
void ReportStatistics ( void );

EventQueue *EL;
Message CurrentMsg;

FILE *report;
char destreport[55],timeStr[24];


int main ( ){

    Reporting ();

    srand(time(NULL));
    EL = malloc(sizeof(EventQueue));
    InitEventList(EL);

    Mesh();

    ReportStatistics();

    while(!isEmpty(EL)) GetEvent(EL);// To Free All The Allocated Memory
    //DisplayEventList(EL);


    free(EL);
    fclose(report);
    /*int x;
     do {
        printf("\n\nPress q to exit the Program: ");
        x = getchar();
    } while (x != EOF && x != 'q');*/

   return 0;
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

void Reporting (){

    strcpy (destreport, "reports/Simulation Report ");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    strncpy(timeStr,asctime(tm),24);
    replacechar(timeStr,':','-');
    strncat (destreport,timeStr,24);
    strcat (destreport," .log");

    report = fopen(destreport, "w");

    if(report==NULL)  {
        printf("\nCould not report, please check the path !");
        exit(0);
    }
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


