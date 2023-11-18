#include "common.h"

int client_sockaddr_parse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage){

    if(addrstr == NULL || portstr == NULL){
        return -1;
    }

    uint16_t port = (uint16_t) atoi(portstr);

    if(port == 0){
        return -1;
    }

    port = htons(port); //host to network (big  endian)

    struct in_addr inaddr4; //32 bit IP Address
    if(inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; //128 bit IP Address
    if(inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6 -> sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }
    return -1;
}

int server_sockaddr_parse(const char *IP_PROTO, const char *portstr, struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t) atoi(portstr);
    
    if(port == 0) return -1;

    port = htons(port);

    memset(storage, 0, sizeof(*storage));

    if(0 == strcmp(IP_PROTO, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY; //Qualquer add disponivel no computador
        addr4->sin_port = port;
        return 0;
    }


    if(0 == strcmp(IP_PROTO, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any; //Qualquer add disponivel no computador
        addr6->sin6_port = port;
        return 0;
    }

    return -1;
}

void logexit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
