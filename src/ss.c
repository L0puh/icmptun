#include "ss.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>



int init_client(char* ip, struct sockaddr_in *serv_addr){
   int sockfd, bytes;
   
   bzero(serv_addr, sizeof(*serv_addr));
   
   serv_addr->sin_family = AF_INET;
   serv_addr->sin_addr.s_addr = inet_addr(ip);

   ASSERT((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)));

   return sockfd;
}

int send_in_chunks(int sockfd, char* data, struct sockaddr_in serv_addr){
   char* chunk;
   int chunk_size, offset, size;

   offset = 0;
   size = strlen(data);
   while (offset < size && data != NULL){
      chunk_size = (size - offset) < DATA_SIZE ? (size-offset): DATA_SIZE;
      chunk = malloc(chunk_size); 
      strncpy(chunk, data+offset, chunk_size);
     
      offset += chunk_size;
      ASSERT(send_packet(sockfd, chunk, chunk_size, serv_addr));
      free(chunk);
   }
   return size;
}

int parse_packet(char* buffer, size_t size){

   struct ip *ip;
   struct icmp *icmp;

   int hdr_len;

   ip = (struct ip*) buffer;
   if (ip->ip_p != IPPROTO_ICMP){
      printf("failed, not an ICMP\n");
      return -1;
   }
   if (icmp->icmp_type == ICMP_ECHO) { return 0; }

   printf("RECV %zu bytes: ", size);
   hdr_len = ip->ip_hl << 2;
   icmp = (struct icmp*) (buffer + hdr_len);
   size -= hdr_len;
   printf("size: %zu, ", size);
   if (size < 8) {
      printf("corrupted icmp packet\n");
      return -1;
   }
   printf("data:\n%s\n", (char*)icmp->icmp_data);
   return 0;
}

int send_packet(int sockfd, char* data, size_t data_size, struct sockaddr_in serv_addr){
   int size, bytes;
   struct icmp* icmp;
   char buffer[BUFFER_SIZE]; 

   bzero(buffer, sizeof(buffer));
   icmp = (struct icmp*) buffer;
   icmp->icmp_type = ICMP_ECHO;
   icmp->icmp_code = 0;
   icmp->icmp_id = getpid();
   icmp->icmp_seq = 1;
  
   memcpy(icmp->icmp_data, data, data_size);
   
   size = sizeof(struct icmp) + data_size; 
   icmp->icmp_cksum = 0; 
   icmp->icmp_cksum = checksum((u_short*) icmp, size);

   ASSERT((bytes = sendto(sockfd, buffer, size, 0, 
         (struct sockaddr*)&serv_addr, sizeof(serv_addr))));

   printf("SENT %d bytes\ndata:\n%s\n", bytes, icmp->icmp_data);
   
   return bytes;
}

int recv_packet(int sockfd, char* buffer, int len){
   int bytes;
   socklen_t len_addr;
   struct sockaddr_in cli_addr;

   cli_addr.sin_family = AF_INET;
   cli_addr.sin_addr.s_addr = INADDR_ANY;

   len_addr = sizeof(cli_addr);
   ASSERT((bytes = recvfrom(sockfd, buffer, len, 0, (struct sockaddr*)&cli_addr, &len_addr)));
   return bytes;
}

int init_server(char* ip){
   int sockfd;
   
   ASSERT((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)));
   setuid(getuid());
  
   return sockfd;
}

unsigned short checksum(void *data, int len) {
    uint sum = 0;
    ushort result, *buf = data;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
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

char* read_file(const char* filename){
   int len;
   FILE *file;
   char* buffer;
   
   file = fopen(filename, "r");
   if (file == NULL){
      printf("[-] ERROR: file is unavailable: %s\n", filename);
      exit(-1);
   }

   fseek(file, 0, SEEK_END);
   len = ftell(file);
   printf("%d\n", len);
   fseek(file, 0, SEEK_SET);
   buffer = malloc(len);

   if (buffer == NULL){
      printf("[-] ERROR: allocation failed: %s\n", filename);
      exit(-1);
   }
   fread(buffer, 1, len, file);
   fclose(file);
  
   return buffer;
}


int max(int a, int b){
   return a > b ? a: b;
}
