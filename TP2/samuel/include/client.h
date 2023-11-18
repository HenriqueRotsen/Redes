#ifndef MY_CLIENT_H
#define MY_CLIENT_H

#include "medium.h"
#include "common.h"
#include "pthread.h"

// Input Variable
char server_addr[100]; // server address
char server_port[10]; // Porta Usada pelo Servidor

// Client Varibles
int len, rtnVal;
int serverfd; // socket file descriptor
int clientfd;
char send_buf[80];
char recv_buf[80];
struct sockaddr_storage storage;
socklen_t client_length; /* length of client structure received on accept */


// Medium Variable
int my_ID;

void arg_parsing(int argc, char *argv[]);
void interrupt_handler (int signum);
void * recv_thread(void *data);
void start_connection(int sockfd);
void get_user_input(struct BlogOperation *msg);
int get_operation_input(struct BlogOperation *msg);
int get_content_input(struct BlogOperation *msg);
void process_server_message(struct BlogOperation *msg);
void init_cliet_msg(struct BlogOperation *msg);

#endif