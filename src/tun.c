#include "ss.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>


static struct global GLOBAL;

int tun_alloc(char *dev, int flags){
   int tunfd;
   struct ifreq ifr;

   ASSERT((tunfd = open("/dev/net/tun", O_RDWR)));
   memset(&ifr, 0, sizeof(ifr));

   ifr.ifr_flags = flags;
   if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
   ASSERT(ioctl(tunfd, TUNSETIFF, (void*)&ifr));

   return tunfd;
}

int tun_write(int tunfd, char* buffer, int len){
   int bytes;

   while (bytes < len){
      bytes += write(tunfd, buffer, len);
      ASSERT(bytes);
   }

   return bytes;
}

int tun_read(int tunfd, char* buffer, int len){
   int bytes;
   
   bytes = read(tunfd, buffer, len);
   ASSERT(bytes);
   return bytes;
}


int tun_run(char* ip, int is_server){
   fd_set read_set;
   char buffer[BUFFER_SIZE];
   struct sockaddr_in serv_addr;
   int tunfd, sockfd, maxfd, bytes;
   
   GLOBAL.server_running = 1;

   tunfd = tun_alloc("tun0", IFF_TUN | IFF_NO_PI);
   if (is_server){
      sockfd = init_server(ip);
   } else {
      sockfd = init_client(ip, &serv_addr); 
   }

   while(GLOBAL.server_running){
      FD_ZERO(&read_set);
      FD_SET(tunfd, &read_set);
      FD_SET(sockfd, &read_set);

      maxfd = max(tunfd, sockfd);
      select(maxfd, &read_set, NULL, NULL, NULL);
      if (FD_ISSET(tunfd, &read_set)){
         //FIXME
         bytes = tun_read(tunfd, buffer, BUFFER_SIZE);
         parse_packet(buffer, bytes);
      } 
      if (FD_ISSET(sockfd, &read_set)){
         bytes = recv_packet(sockfd, buffer, BUFFER_SIZE);
         tun_write(tunfd, buffer, bytes);
      }
   }

   return 0;
}
