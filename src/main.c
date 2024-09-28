#include "ss.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(char* argv);

int main(int argc, char* argv[]) {
   int opt;
   
   if (argc == 1) print_usage(argv[0]);
   while ((opt = getopt(argc, argv, "s:c:")) != -1){
      switch (opt) {
         case 's':
            init_server(optarg);
            break;
         case 'c':
            init_client(optarg);
            break;
         default:
            print_usage(argv[0]);
      }
   }

   return 0; 
}


void print_usage(char* argv){
   printf("usage:\n\
         -s [ip to run server on] - set up server point\n\
         -c [ip of the server] - set up client point\n\
         example: %s -s 127.0.0.1\n", argv);
   exit(0);
}
