#include "ss.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct global GLOBAL;

unsigned short checksum(packet_t *data, size_t len){
   int sum;
   unsigned short *ptr = (unsigned short*)data, res;

   sum = res = 0;
   while (len > 1){
      sum += *ptr++;
      len-=2;
   }
   if (len == 1){
      *(unsigned char*) (&res) = *(unsigned char*)ptr;
      sum += res;
   }
   sum = (sum >> 16) + (sum & 0xffff);
   sum += (sum>>16);
   res = ~sum;
   return res;
}

int init_client(char* ip){
   
   int sockfd, bytes;
   packet_t pckt;
   struct sockaddr_in serv_addr;

   bzero(&serv_addr, sizeof(serv_addr));

   serv_addr.sin_family = AF_INET;
   ASSERT(inet_aton(ip, (struct in_addr*)&serv_addr.sin_addr.s_addr));
   ASSERT((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)));
   
   strcpy(pckt.data, "hello world\0");
   pckt.icmp.icmp_type = ICMP_ECHO;
   pckt.icmp.icmp_code = 0;
   pckt.icmp.icmp_id = 1;
   pckt.icmp.icmp_seq = 0;
   pckt.icmp.icmp_cksum = 0;
   pckt.icmp.icmp_cksum = checksum(&pckt, sizeof(pckt));

   ASSERT((bytes = sendto(sockfd, &pckt, sizeof(pckt), 0, 
                   (struct sockaddr*)&serv_addr, sizeof(serv_addr))));

   printf("SENT %d bytes: %s\n", bytes, pckt.data);
   return 0;
}

void exit_on_error(char* file, const char* func, int line){
   if (errno != 0){
      if (errno == EINTR) return;
      char* msg = malloc(1024);
      sprintf(msg, "[-] ERROR in %s (%s:%d):", file, func, line);
      perror(msg);
      free(msg);
      exit(-1);
   }
}

int server_loop(int sockfd){
   int bytes;
   packet_t pckt;
   struct sockaddr_in cli_addr;
   socklen_t len_addr;

   len_addr = sizeof(cli_addr);
   while(GLOBAL.server_running){
      ASSERT((bytes = recvfrom(sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&cli_addr, &len_addr)));
      printf("RECV %d bytes: ", bytes); //FIXME
      printf("%s\n", pckt.data); 
   }
   return 0;
}

int init_server(char* ip){
   int sockfd;
   
   ASSERT((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)));
   
   setuid(getuid());
   GLOBAL.server_running = 1;
  
   // TODO: add virtual interfaces?

   server_loop(sockfd);
   
   return 0;
}
