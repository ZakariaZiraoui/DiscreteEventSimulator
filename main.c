#include <time.h>
#include "Mesh.h"
#include "ExpressCube.h"

// Simulation clock
float Tnow;
int Lambda;

// Statistical counters
float MeanResponseTime, MeanWatingTime,Throughput;
int ClientServed,MessageID;

void Reporting ();
void ReportStatistics ( void );

EventQueue *EL;
Message CurrentMsg;

FILE *report;
char destreport[52],timeStr[21];

int main ( ){
    int LTab [17]= {32000,10500,9200,1000,400,350,
                    300,250,200,180,160,150,100,90,80,70,50};

    int cpt;
    for(cpt=0; cpt<17; cpt++){
    Lambda =LTab[cpt];

    Reporting ();

    srand(time(NULL));
    EL = malloc(sizeof(EventQueue));
    InitEventList(EL);

    if (BatchMeansMethod){
     int counter;
        for(counter=1; counter<=10; counter++) {
           switch(Topo){
            case MESH:    Mesh();        break;
            case EXPRESS: ExpressCube(); break;
            default :
                printf("\nTopology Not Defined !"); exit(0); break;
            }
            ReportStatistics();
        }
    }else {
       switch(Topo){
            case MESH:    Mesh();        break;
            case EXPRESS: ExpressCube(); break;
            default :
                printf("\nTopology Not Defined !"); exit(0); break;
            }
            ReportStatistics();
    }

    while(!isEmpty(EL)) GetEvent(EL);// To Free All The Allocated Memory
    //DisplayEventList(EL);

    free(EL);
    fclose(report);
    /*int x;
     do {
        printf("\n\nPress q to exit the Program: ");
        x = getchar();
    } while (x != EOF && x != 'q');*/


    }
   return 0;
}

void ReportStatistics ( void ) {
    printf("\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    printf("\nInjection Rate : %d",Lambda);
    printf("\nNumber of Clients Served : %d",ClientServed);
    printf("\nThe Mean Response Time : %0.2f cycles",MeanResponseTime/ClientServed);
    printf("\nThe Mean Waiting  Time : %0.2f cycles",MeanWatingTime/ClientServed);
    printf("\nThroughput : %0.3f ",ClientServed/Tnow);

    fprintf(report,"\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    fprintf(report,"\nInjection Rate : %d",Lambda);
    fprintf(report,"\nNumber of Clients Served : %d",ClientServed);
    fprintf(report,"\nThe Mean Response Time : %0.2f cycles",MeanResponseTime/ClientServed);
    fprintf(report,"\nThe Mean Waiting  Time : %0.2f cycles",MeanWatingTime/ClientServed);
    fprintf(report,"\nThroughput : %0.3f ",ClientServed/Tnow);
}

void Reporting (){

    strcpy (destreport, "reports/Simulation Report ");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    //strncpy(timeStr,asctime(tm),19);
    strftime (timeStr,21,"%Y-%m-%d %H:%M:%S",tm);
    replacechar(timeStr,':','-');
    strncat (destreport,timeStr,21);
    strcat (destreport," .log");

    report = fopen(destreport, "w");

    if(report==NULL)  {
        printf("\nCould not report, please check the path !");
        exit(0);
    }
}


