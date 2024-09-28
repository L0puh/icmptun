#include "ss.h"

#include <stdio.h>

int init_client(char* ip){
   printf("IP OF THE SERVER: %s\n", ip);
   return 0;
}

int init_server(char* ip){
   printf("SERVER IS RUNNING ON: %s\n", ip);
   return 0;
}
