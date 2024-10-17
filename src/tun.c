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

void tun_read(){
   int bytes, tunfd;
   char buffer[BUFFER_SIZE];

   tunfd = tun_alloc("tun0", IFF_TUN | IFF_NO_PI);

   while(1){
      bytes = read(tunfd, buffer, BUFFER_SIZE);
      ASSERT(bytes);
   }
}
