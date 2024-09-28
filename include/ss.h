#ifndef SS_H
#define SS_H

#define ASSERT(res) { if (res < 0) exit_on_error(__FILE__, __func__, __LINE__); }
#define BUFFER_SIZE 1024

#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

struct global {
   int server_running;
};

typedef struct {
   struct icmp icmp;
   char data[BUFFER_SIZE];
} packet_t;

int init_client(char* ip);
int init_server(char* ip);

void exit_on_error(char* file, const char* func, int line);

#endif
