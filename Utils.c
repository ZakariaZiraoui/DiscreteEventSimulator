#include "Utils.h"
#include <string.h>

void replacechar(char *s,char c1,char c2){
   int i=0;
   for(i=0;s[i];i++)
	  if(s[i]==c1)   s[i]=c2;
}


