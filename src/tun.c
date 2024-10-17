#include "ss.h"

#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>


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
