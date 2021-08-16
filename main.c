#include <time.h>
#include "Mesh.h"
#include "ExpressCube.h"

// Simulation clock
float Tnow,Tend;

int Lambda,Batch;

// Statistical counters
float MeanResponseTime, MeanWaitingTime,Throughput;
float OverallMeanResponseTime,OverallMeanWaitingTime,OverallThroughput;
int ClientServed,MessageID;

void Reporting (void);
void ReportStatistics ( void );
void ReportBatchMean(void);
void ReportOverallStatistics(void);
void SummarizeResults (void) ;

EventQueue *EL;
Message CurrentMsg;

FILE *report,*summary;
char destreport[52],timeStr[21];

int main ( ){
    int cpt;
    int LTab [17]= {200,32000,10500,9200,1000,400,350,
                    300,250,200,180,160,150,100,90,80,70,50};

    if(Summary)SummarizeResults ();
    for(cpt=0; cpt<1; cpt++){//17
        Lambda =LTab[cpt];

        Reporting ();

        srand(time(NULL));
        EL = malloc(sizeof(EventQueue));
        InitEventList(EL);

        if (BatchMeansMethod){
         Tend=0;
         OverallMeanResponseTime=0.0;
         OverallMeanWaitingTime=0.0;
         OverallThroughput=0.0;
            for(Batch=1; Batch<=Batches; Batch++) {
               MeanResponseTime=0.0;     MeanWaitingTime=0.0;
               Throughput=0.0;           ClientServed=0;
               switch(Topo){
                case MESH:    Mesh();        break;
                case EXPRESS: ExpressCube(); break;
                default :
                    printf("\nTopology Not Defined !"); exit(0); break;
                }
                ReportBatchMean();
            }
         ReportOverallStatistics();
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
    if(Summary)fclose(summary);
   return 0;
}

void ReportStatistics ( void ) {
    printf("\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    printf("\nInjection Rate : %d",Lambda);
    printf("\nNumber of Clients Served : %d",ClientServed);
    printf("\nThe Mean Response Time : %0.2f cycles",MeanResponseTime/ClientServed);
    printf("\nThe Mean Waiting  Time : %0.2f cycles",MeanWaitingTime/ClientServed);
    printf("\nThroughput : %0.3f ",ClientServed/Tnow);

    fprintf(report,"\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    fprintf(report,"\nInjection Rate : %d",Lambda);
    fprintf(report,"\nNumber of Clients Served : %d",ClientServed);
    fprintf(report,"\nThe Mean Response Time : %0.2f cycles",MeanResponseTime/ClientServed);
    fprintf(report,"\nThe Mean Waiting  Time : %0.2f cycles",MeanWaitingTime/ClientServed);
    fprintf(report,"\nThroughput : %0.3f ",ClientServed/Tnow);

    if(Summary)
        fprintf(summary,"%d\t%0.2f\t%0.2f\t%0.3f\n",Lambda,MeanResponseTime/ClientServed,MeanWaitingTime/ClientServed,ClientServed/Tnow);
}

void ReportOverallStatistics ( void ) {
    printf("\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    printf("\nInjection Rate : %d",Lambda);
    printf("\nBatch Size : %d",ClientServed);
    printf("\nNumber of Batches : %d",Batches);
    printf("\nNumber of Clients Served : %d",ClientServed*Batches);
    printf("\nOverall Response Time : %0.2f cycles",OverallMeanResponseTime/Batches);
    printf("\nOverall Waiting  Time : %0.2f cycles",OverallMeanWaitingTime/Batches);
    printf("\nOverall Throughput : %0.3f ",OverallThroughput);

    fprintf(report,"\n\nEnd Of The Simulation at T= %0.2f cycles",Tnow);
    fprintf(report,"\nInjection Rate : %d",Lambda);
    fprintf(report,"\nBatch Size : %d",ClientServed);
    fprintf(report,"\nNumber of Batches : %d",Batches);
    fprintf(report,"\nNumber of Clients Served : %d",ClientServed*Batches);
    fprintf(report,"\nOverall Response Time : %0.2f cycles",OverallMeanResponseTime/Batches);
    fprintf(report,"\nOverall Waiting  Time : %0.2f cycles",OverallMeanWaitingTime/Batches);
    fprintf(report,"\nOverall Throughput : %0.3f ",OverallThroughput);

    if(Summary)
        fprintf(summary,"%d\t%0.2f\t%0.2f\t%0.3f\n",Lambda,OverallMeanResponseTime/Batches,OverallMeanWaitingTime/Batches,OverallThroughput);
}

void ReportBatchMean(){

    MeanResponseTime=MeanResponseTime/ClientServed;
    MeanWaitingTime=MeanWaitingTime/ClientServed;
    Throughput=ClientServed/(Tnow-Tend);
    Tend=Tnow;

    OverallMeanResponseTime+=MeanResponseTime;
    OverallMeanWaitingTime +=MeanWaitingTime;
    OverallThroughput+=Throughput;

    printf("\nBatch N°%2d : Mean Response Time : %0.2f cycles\tMean Waiting Time : %2.2f cycles\tThroughput : %0.3f",Batch,MeanResponseTime,MeanWaitingTime,Throughput);
    fprintf(report,"\nBatch N°%2d : Mean Response Time : %0.2f cycles\tMean Waiting Time : %2.2f cycles\tThroughput : %0.3f",Batch,MeanResponseTime,MeanWaitingTime,Throughput);
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

void SummarizeResults (){

    strcpy (destreport, "reports/Simulation Result ");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    //strncpy(timeStr,asctime(tm),19);
    strftime (timeStr,21,"%Y-%m-%d %H:%M:%S",tm);
    replacechar(timeStr,':','-');
    strncat (destreport,timeStr,21);
    strcat (destreport," .log");

    summary = fopen(destreport, "w");

    if(summary==NULL)  {
        printf("\nCould not Summarize, please check the path !");
        exit(0);
    }
}

