#ifndef SS_H
#define SS_H

#define ASSERT(res) { if (res < 0) exit_on_error(__FILE__, __func__, __LINE__); }

#define BUFFER_SIZE 4028
#define DATA_SIZE   2014

#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

struct global {
   int server_running;
};

int init_server(char* ip);
int init_client(char* ip, struct sockaddr_in* serv_addr);


int parse_packet(char* buffer, size_t size);
int send_in_chunks(int sockfd, char* data, struct sockaddr_in serv_addr);
int send_packet(int sockfd, char* data, size_t data_size, struct sockaddr_in serv_addr);
int recv_packet(int sockfd, char* buffer, int len);

void exit_on_error(char* file, const char* func, int line);
unsigned short checksum(void *data, int len);

char* read_file(const char* filename);

void run_client(char* filename, char* ip);
void run_server(char* ip);

int max(int a, int b);

#endif
