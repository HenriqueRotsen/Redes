#ifndef MY_COMMON_H
#define MY_COMMON_H

#pragma once

/*************************************************/
/* Used for the server and client                */
/*************************************************/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h> // struct addrinfo | getaddrinfo()

int client_sockaddr_parse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
int server_sockaddr_parse(const char *IP_PROTO, const char *portstr, struct sockaddr_storage *storage);
void logexit(const char *msg);

/*************************************************/
/* Used in the game                              */
/*************************************************/


#endif