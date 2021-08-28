#include "Utils.h"


Location RandDest(){

    float x=((float) rand() / (RAND_MAX))*(N-1);
    float y=((float) rand() / (RAND_MAX))*(M-1);
    //int x = rand() % N; int y = rand() % M;
    Location loc;
    loc.x=round(x); loc.y=round(y);
return loc;
}

void replacechar(char *s,char c1,char c2){
   int i=10;
   for(i=10;s[i];i++)
	  if(s[i]==c1)   s[i]=c2;
}


