#include "ss.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(char* argv);

int main(int argc, char* argv[]) {
   int opt, is_server;
   char *filename, *ip;
   
   if (argc == 1) print_usage(argv[0]);
   while ((opt = getopt(argc, argv, "s:c:f:")) != -1){
      switch (opt) {
         case 's':
            ip = optarg;
            is_server = 1;
            break;
         case 'c':
            ip = optarg;
            is_server = 0;
            break;
         case 'f':
            filename = optarg;
            break;
         default:
            print_usage(argv[0]);
            return 0;
      }

   }
   if (ip && is_server){
      run_server(ip);
   } else if (ip && filename && !is_server){
      run_client(filename, ip);
   } else {
      print_usage(argv[0]);
   }

   return 0; 
}


void print_usage(char* argv){
   printf("usage:\n\
         -s [ip to run server on]               - set up server point\n\
         -c [ip of the server] -f [filename]    - set up client point\n\
         example: %s -s 127.0.0.1\n", argv);
   exit(0);
}
