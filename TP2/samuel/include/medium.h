#ifndef MEDIUM_H
#define MEDIUM_H

#pragma once

#include "common.h"

#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048
#define MAX_NUMBER_CLIENT 10
#define DEBUG 0


struct BlogOperation {
    int client_id;
    int operation_type;
    int server_response;
    char topic[TOPIC_SIZE];
    char content[CONTENT_SIZE];
};

void receiveBlogOperation(int sockfd, struct BlogOperation *msg);
void sendBlogOperation(int sockfd, struct BlogOperation *msg);
void print_BlogOperation(struct BlogOperation *msg);

void receiveBlogOperation(int sockfd, struct BlogOperation *msg) {
    // if (recv(sockfd, &msg->client_id, sizeof(int), 0) < 0) logexit("Error to send the client id\n");
    // if (recv(sockfd, &msg->operation_type, sizeof(int), 0) < 0) logexit("Error to send operation type\n");
    // if (recv(sockfd, &msg->server_response, sizeof(int), 0) < 0) logexit("Error to send server_response\n");

    // if(DEBUG) printf("Receive int values \n");

    // int total = 0;
    // memset(msg->topic, 0, TOPIC_SIZE);
    // while (1) {
    //    int count = recv(sockfd, &msg->topic + total, TOPIC_SIZE - total, 0);
    //    if (count < 0) logexit("Error to send topic\n");
    //    if (count == 0) break;
    //    total += count;
    // }

    // if(DEBUG) printf("Received Topic\n");

    // total = 0;
    // memset(msg->content, 0, CONTENT_SIZE);
    // while (1) {
    //     int count = recv(sockfd, &msg->content + total, CONTENT_SIZE - total, 0);
    //     if (count < 0) logexit("Error to send content\n");
    //     total += count;
    //     if (count == 0 || total == CONTENT_SIZE) break;
    // }

    // if(DEBUG) printf("Received Contend\n");
    if (recv(sockfd, msg, sizeof(struct BlogOperation), 0) < 0) logexit("Error to send the struct BlogOperation\n");
}

void sendBlogOperation(int sockfd, struct BlogOperation *msg) {
    // if (send(sockfd, &msg->client_id, sizeof(int), 0) < 0) logexit("Error to send the client id\n");
    // if (send(sockfd, &msg->operation_type, sizeof(int), 0) < 0) logexit("Error to send operation type\n");
    // if (send(sockfd, &msg->server_response, sizeof(int), 0) < 0) logexit("Error to send server_response\n");
    // if (send(sockfd, &msg->topic, TOPIC_SIZE, 0) < 0) logexit("Error to send topic\n");
    // if (send(sockfd, &msg->content, CONTENT_SIZE, 0) < 0) logexit("Error to send content\n");
    if (send(sockfd, msg, sizeof(struct BlogOperation), 0) < 0) logexit("Error to send struct\n");
}

void print_BlogOperation(struct BlogOperation *msg) {
    printf("**********************************************\n");
    printf("client id:%d\n", msg->client_id);
    printf("Operation Type:%d and server response: %d\n", msg->operation_type, msg->server_response);
    printf("Topic: %s\n", msg->topic);
    printf("Content: %s\n", msg->content);
    printf("**********************************************\n");
}
#endif