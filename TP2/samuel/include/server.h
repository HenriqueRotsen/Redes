#ifndef MY_SERVER_H
#define MY_SERVER_H

#include <pthread.h>

#include "common.h"
#include "medium.h"

// Input Variable
int IP_version; // 0 -> IPv4, 1 -> IPv6 
int SERVER_PORT; // Porta Usada pelo Servidor

// Client Varibles
int clientfd;
socklen_t client_length; /* length of client structure received on accept */

// Server Variables
int serverfd; // socket file descriptor
struct sockaddr_storage storage;
char send_buf[80];
char recv_buf[80];

struct client_data
{
    int csock;
    struct sockaddr_storage storage;
};

pthread_mutex_t mutex;

// Medium Variables

struct client_info {
    int available;
    int sock;
};

struct client_info clients[MAX_NUMBER_CLIENT];


struct topic
{
    struct topic *prev;
    struct topic *next;
    char topic_name[TOPIC_SIZE];
    int subscribe[MAX_NUMBER_CLIENT];
};

struct topic *list_head = NULL;
struct topic *list_tail = NULL;


void arg_parsing(int argc, char *argv[]);
void * client_thread(void *data);
int close_conection();
void new_connection(int sock);
void process_client_msg(struct BlogOperation *client_msg);
void init_user_ID();
struct topic * find_topic(char *msg) ;
void insert_topic(char *msg);
void publish(struct BlogOperation *client_msg);
void subscribe_to_topic(struct BlogOperation *msg);
void unsubscribe_from_topic(struct BlogOperation *msg);
void list_topics(int client_id);
void client_disconnect(int client_id);
void delete_topic();
void print_server_status();

// Signal Handler
void interrupt_handler (int signum);

#endif
