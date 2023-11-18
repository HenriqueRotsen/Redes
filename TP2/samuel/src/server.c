#include "server.h"

int main(int argc, char *argv[]) {

    init_user_ID();

    signal(SIGINT, interrupt_handler);
    pthread_mutex_init(&mutex, NULL);

    if(server_sockaddr_parse(argv[1], argv[2], &storage) == 1) {
        perror("server sockaddr parse");
        exit(-1);
    }


    /*************************************************/
    /* Create a stream socket                        */
    /*************************************************/
    serverfd = socket(storage.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (serverfd < 0)
    {
      perror("socket");
      exit(-1);
    }
    
    /*************************************************/
    /* Bind to the local address                     */
    /*************************************************/
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(serverfd, addr, sizeof(storage)) != 0){
        printf("bind() failed\n");
        exit(-1);
    }


    /*************************************************/
    /* accept upto 5 pending requests                */
    /* and make the server socket a passive socket   */
    /*************************************************/
    if (listen(serverfd, 5) == -1) {
            printf("Listen Failed\n");
            exit(1);
    }

    
    for (;;) {
        /* accept is a blocking call and the call would stop further invocation of
        code till a client connection is accepted */

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) (&cstorage);
        client_length = sizeof(struct sockaddr_storage);

        clientfd = accept(serverfd, (struct sockaddr *)&caddr, &client_length);

        if (clientfd < 0) {
            printf("Unable to connect to Client\n");
            exit(1);
        }

        struct client_data *cdata = malloc(sizeof(struct client_data));
        if (!cdata) {
            printf("malloc");
            exit(1);
        }
        cdata->csock = clientfd;
        memcpy(&(cdata->storage), &storage, sizeof(storage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata); 
  
        
    }

    close(serverfd);
    return 0;
}

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *) data;
    struct sockaddr *cadddr = (struct sockaddr *)(&cdata->storage);

    // TODO APLICATION RUN HERE
    /* read and write data from/to client socket */

    struct BlogOperation client_start_msg;
    receiveBlogOperation(cdata->csock, &client_start_msg);
    
    // New Connection -> Code 1
    if (client_start_msg.operation_type == 1 && client_start_msg.client_id == 0){
        new_connection(cdata->csock);
    }

    for (;;) {
        if (DEBUG) print_server_status();
        
        struct BlogOperation client_msg;
        receiveBlogOperation(cdata->csock, &client_msg);

        if(DEBUG) print_BlogOperation(&client_msg);
        
        process_client_msg(&client_msg);
        if (client_msg.operation_type == 5) break;
    }

    close(cdata->csock);

    // Tenho que dar um free na memoria

    pthread_exit(EXIT_SUCCESS);
}


void interrupt_handler (int signum) {
    close(serverfd);

    printf("socket connection closed\n");
    exit(0);
}

void process_client_msg(struct BlogOperation *client_msg) {    
    switch (client_msg->operation_type)
    {
    // Novo post em um tópico 2
    case 2:
        /* code */
        publish(client_msg);
        break;
    // Listagem de tópicos 3
    case 3:
        list_topics(client_msg->client_id);
        break;
    // Inscrição em um tópico 4
    case 4:
        subscribe_to_topic(client_msg);
        break;
    // Desconectar do servidor 5
    case 5:
        client_disconnect(client_msg->client_id);
        /* code */
        break;
    // Desinscrição de um tópico 6
    case 6:
        unsubscribe_from_topic(client_msg);
        break;
    
    default:
        break;
        // logexit("No Operation Found\n");
    }
}

// Inicia com valores padrão o vetor de clientes
void init_user_ID() {
    for (int i = 0; i < MAX_NUMBER_CLIENT; i++) {
        clients[i].available = 1;
        clients[i].sock = 0; 
    }
}

// Conecta um novo cliente
void new_connection(int socket) {
    pthread_mutex_lock(&mutex);
    int i;
    for (i = 0; i < MAX_NUMBER_CLIENT; i++) {
        if (clients[i].available == 1) break; 
    }

    clients[i].available = 0;
    clients[i].sock = socket;
    pthread_mutex_unlock(&mutex);

    struct BlogOperation server_msg;
    server_msg.client_id = i + 1;
    server_msg.operation_type = 1;
    server_msg.server_response = 1;
    strcpy(server_msg.topic, "");
    strcpy(server_msg.content, "");
    
    printf("client %02d connected\n", server_msg.client_id);

    sendBlogOperation(clients[i].sock, &server_msg);
}

void publish(struct BlogOperation *client_msg) {
    if (DEBUG) printf("Debug Message: Enter the function publish\n");

    struct topic *ptr = find_topic(client_msg->topic);
    if(ptr == NULL) logexit("Empty Point in publish funcion\n");

    printf("new post added in %s by %02d\n", client_msg->topic, client_msg->client_id);

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_NUMBER_CLIENT; i++) {
        if(ptr->subscribe[i] == 1) {
                struct BlogOperation server_msg;
                server_msg.client_id = client_msg->client_id;
                server_msg.operation_type = 2;
                server_msg.server_response = 1;
                strcpy(server_msg.topic, client_msg->topic);
                strcpy(server_msg.content, client_msg->content);

                if (clients[i].available == 1) logexit("Error: User no long connected\n");
                sendBlogOperation(clients[i].sock, &server_msg);
        }
    }
    pthread_mutex_unlock(&mutex);

}

struct topic * find_topic(char *msg) {
    if (DEBUG) printf("Debug Message: Enter the function find topic searching for %s\n", msg);

    int topic_exist = 0;
    struct topic *ptr = list_head;
    while(ptr != NULL) {
        if (strcmp(ptr->topic_name, msg) == 0) {
            topic_exist = 1;
            break;
        }
        ptr = ptr->next;
    }

    if(topic_exist) return ptr;
    else {
        insert_topic(msg); 
        return list_tail;
    }
}

void subscribe_to_topic(struct BlogOperation *msg) {
    
    if(DEBUG) printf("Debug Message: Enter the function subscribe topic\n");
    
    struct topic *ptr = find_topic(msg->topic);
    if(ptr == NULL) logexit("Empty Point in subscribe funcion\n");
    
    int client_id = msg->client_id;

    if (ptr->subscribe[client_id - 1] == 1) {
        if(DEBUG) printf("Debug Message: Client already subscribed to a topic\n");
        
        struct BlogOperation server_msg;
        server_msg.client_id = client_id;
        server_msg.operation_type = 4;
        server_msg.server_response = 1;
        strcpy(server_msg.topic, "");
        strcpy(server_msg.content, "error: already subscribed\n");

        sendBlogOperation(clients[client_id - 1].sock, &server_msg);
    } else {
        if(DEBUG) printf("Debug Message: Subscribing client to topic\n");
        printf("client %02d subscribed to %s\n", msg->client_id, msg->topic);
        ptr->subscribe[client_id - 1] = 1;
    }

}

void unsubscribe_from_topic(struct BlogOperation *msg) {
    pthread_mutex_lock(&mutex);
    if(DEBUG) printf("Debug Message: Enter the function unsubscribe from topic\n");
    
    struct topic *ptr = find_topic(msg->topic);
    if(ptr == NULL) logexit("Empty Point in unsubscribe funcion\n");

    int client_id = msg->client_id;
    if (ptr->subscribe[client_id - 1] == 1) {
        ptr->subscribe[client_id - 1] = 0;
    }
    else {
        if(DEBUG) printf("Debug Message: Client isn't subscribed to the topic %s\n", msg->topic);
    }

    printf("client %02d unsubscribed to %s\n", msg->client_id, msg->topic);
    pthread_mutex_unlock(&mutex);
}

void list_topics(int client_id) {
    if(DEBUG) printf("Debug Message: Enter the function list topic\n");

    struct BlogOperation server_msg;
    server_msg.client_id = client_id;
    server_msg.operation_type = 3;
    server_msg.server_response = 1;
    strcpy(server_msg.topic, "");
    strcpy(server_msg.content, "");

    int length = 0;
    if (list_head != NULL) {
        struct topic *ptr = list_head;
        while(ptr != NULL) {
            // concatenates str1 and str2
            // the resultant string is stored in str1.
            if (length == 0) length += sprintf(server_msg.content + length, "%s", ptr->topic_name);
            else length += sprintf(server_msg.content + length, ";%s", ptr->topic_name);
            ptr = ptr->next;
        }
        strcat(server_msg.content, "\n");
    }
    else {
        sprintf(server_msg.content, "no topics available\n");
    }

    sendBlogOperation(clients[client_id - 1].sock, &server_msg);
}   

void client_disconnect(int client_id) {
    pthread_mutex_lock(&mutex);
    int index = client_id - 1;
    clients[index].available = 1;
    clients[index].sock = 0;


    struct topic *ptr = list_head;
    while(ptr != NULL) {
        ptr->subscribe[index] = 0;
        ptr = ptr->next;
    }
    

    printf("client %02d disconnected\n", client_id);
    pthread_mutex_unlock(&mutex);
}

void insert_topic(char *msg) {
    pthread_mutex_lock(&mutex);
    if(DEBUG) printf("Debug Message: Enter the function insert topic\n");

    struct topic *new_topic = malloc(sizeof(struct topic));
    if (new_topic == NULL) {
        logexit("Error when allocating memory to a new topic");
    }
    new_topic->prev = NULL;
    new_topic->next = NULL;
    strcpy(new_topic->topic_name, msg);

    if(DEBUG) printf("Debug Message: Initialize a new topic named %s\n", new_topic->topic_name);
    
    for(int i = 0; i < MAX_NUMBER_CLIENT; i++) {

    }
    
    if (list_head == NULL) {
        list_head = new_topic;
        list_tail = new_topic;
    }
    else {
        list_tail->next = new_topic;
        list_tail = new_topic;
    }
    pthread_mutex_unlock(&mutex);
}

void print_server_status() {
    printf("**************    Server Status   **************\n");
    printf("Clients Connected: ");
    for (int i = 0; i < MAX_NUMBER_CLIENT; i++) {
        printf("%d:", i+1);
        if (clients[i].available == 1) printf("X ");
        else printf("O ");
    }
    printf("\n");

    printf("Topics \n");
    
    if (list_head == NULL) printf("Empty Topic List\n");
    else {
        struct topic *ptr = list_head;
        while(ptr != NULL) {
            printf("topic name: %s\n", ptr->topic_name);
            printf("subscribed client: ");
            
            for (int i = 0; i < MAX_NUMBER_CLIENT; i++) {
                printf("%d:", i+1);
                if (ptr->subscribe[i] == 1) printf("O ");
                else printf("X ");
            }

            printf("\n");

            ptr = ptr->next;
        }
    }
    printf("************    END Server Status   ************\n");
}

void init_server_msg(struct BlogOperation *msg) {
    msg->server_response = 1;
    strcpy(msg->topic, "");
    strcpy(msg->content, "");
}