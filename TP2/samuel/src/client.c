#include "client.h"

int main(int argc, char *argv[]) {
    arg_parsing(argc, argv);

    signal (SIGINT, interrupt_handler);

    client_sockaddr_parse(server_addr, server_port, &storage);

    /*************************************************/
    /* Create a stream socket                        */
    /*************************************************/
    clientfd = socket(storage.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (clientfd < 0)
    {
      perror("socket");
      exit(-1);
    }

    /********************************************************************/
    /* Use the connect() function to establish a connection to the      */
    /* server.                                                          */
    /********************************************************************/
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (connect(clientfd, addr, sizeof(storage)))
    {
        /*****************************************************************/
        /* Note: the res is a linked list of addresses found for server. */
        /* If the connect() fails to the first one, subsequent addresses */
        /* (if any) in the list can be tried if required.               */
        /*****************************************************************/
        printf("connect() failed");
        exit(1);
    }

    start_connection(clientfd);
    struct BlogOperation server_msg;
    if(DEBUG) printf("Waiting for ID\n");
    receiveBlogOperation(clientfd, &server_msg);
    my_ID = server_msg.client_id;
    if(DEBUG) printf("My ID: %d\n", my_ID);

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);

    for (;;) {
        struct BlogOperation client_msg;
        get_user_input(&client_msg);
        sendBlogOperation(clientfd, &client_msg);
        if (client_msg.operation_type == 5) break;
    }

    close(clientfd); 
    close(serverfd);

    return 0;
}

void * recv_thread(void *data) {
    for (;;) {
        struct BlogOperation server_msg;
        receiveBlogOperation(clientfd, &server_msg);
        process_server_message(&server_msg);
    }
}

void arg_parsing(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Number of Arguments passed: %d\n", argc);       
        printf("Number of arguments needed: 3\n");
        exit(1);
    }
    
    memset(server_addr, 0, sizeof(server_addr));
    sscanf(argv[1], "%s", server_addr);
    sscanf(argv[2], "%s", server_port);
}

void interrupt_handler (int signum) {
    close(serverfd);

    printf("socket connection closed\n");
    exit(0);
};

void start_connection(int sockfd) {
    struct BlogOperation client_msg;
    client_msg.client_id = 0;
    client_msg.operation_type = 1;
    client_msg.server_response = 0;
    strcpy(client_msg.topic, "");
    strcpy(client_msg.content, "");
    sendBlogOperation(sockfd, &client_msg);
}

void init_cliet_msg(struct BlogOperation *msg) {
    msg->client_id = my_ID;
    msg->operation_type = 0;
    msg->server_response = 0;
    strcpy(msg->topic, "");
    strcpy(msg->content, "");
}

void get_user_input(struct BlogOperation *msg) {
    int validInput = 0;

    do
    {
        init_cliet_msg(msg);
        validInput = get_operation_input(msg);
        if (validInput == 1 && msg->operation_type == 2) {
            validInput = get_content_input(msg);
        }
    } while (!validInput);  
}

int get_operation_input(struct BlogOperation *msg) {
    char line[CONTENT_SIZE];
    memset(line, 0, CONTENT_SIZE);
    fgets(line, CONTENT_SIZE, stdin);

    char operation[TOPIC_SIZE];
    char secondWord[TOPIC_SIZE];
    char thirdWord[TOPIC_SIZE];
    int nWords = sscanf(line, "%s%s%s", operation, secondWord, thirdWord);

    switch (nWords)
    {
    case 1:
        if(strcmp(operation, "exit") == 0) {
            msg->operation_type = 5;
            return 1;
        }
        printf("Invalid Operation\n");
        return 0;
    case 2:
        if(strcmp(operation, "list") == 0) {
            // the string is empty
            if (strcmp(secondWord, "topics") == 0) {
                msg->operation_type = 3;
                return 1;
            }
            printf("To list the topics use: list topics\n");
            return 0;
        }
        else if(strcmp(operation, "subscribe") == 0) {
            // the string is empty
            if (strcmp(secondWord, "") == 0) {
                printf("Empty Topic\n");
                return 0;
            }
            msg->operation_type = 4;
            strcpy(msg->topic, secondWord);
            return 1;

        }
        else if(strcmp(operation, "unsubscribe") == 0) {
            // the string is empty
            if (strcmp(secondWord, "") == 0) {
                printf("Empty Topic\n");
                return 0;
            }
            msg->operation_type = 6;
            strcpy(msg->topic, secondWord);
            return 1;
        }
        else {
            printf("Invalid Operation\n");
            return 0;
        }
        break;
    // Post a phrase
    case 3:
        if(strcmp(operation, "publish") != 0) return 0;
        if(strcmp(secondWord, "in") != 0) return 0;
        if(strcmp(thirdWord, "") == 0) return 0;
        
        msg->operation_type = 2;
        strcpy(msg->topic, thirdWord);
        return 1;

    default:
        printf("Invalid number of Words\n");
        return 0;
    }

    return 0;
}

int get_content_input(struct BlogOperation *msg) {
    char line[CONTENT_SIZE];
    memset(line, 0, CONTENT_SIZE);
    fgets(line, CONTENT_SIZE, stdin);
    if (strcmp(line, "") == 0) return 0;
    strcpy(msg->content, line);
    return 1;
}

void process_server_message(struct BlogOperation *msg) {
    switch (msg->operation_type)
    {
    case 2: 
        printf("new post added in %s by %02d\n", msg->topic, msg->client_id);
        printf("%s", msg->content);
        break;
    case 3:
        printf("%s", msg->content);
        break;
    case 4:
        printf("%s", msg->content);
        break;
    default:
        break;
    }
}