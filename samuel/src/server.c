#include "server.h"

int main(int argc, char *argv[]) {

    arg_parsing(argc, argv);
    load_input(filename);
    print_board(get_review_board().board);

    signal(SIGINT, interrupt_handler);

    // Create a reliable, stream socket using TCP
    /*************************************************/
    /* Create an AF_INET6 stream socket              */
    /*************************************************/
    serverfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (serverfd < 0)
    {
      perror("socket");
      exit(-1);
    }

    /*************************************************/
   /* Initialize the socket address structure       */
   /*************************************************/
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    
    // What this does?
    // Atriubui um endereço de IP para o socket e a porta
    inet_pton(AF_INET6, "192.168.0.112",  &server_addr.sin6_addr);
    // memcpy(&server_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    server_addr.sin6_port = htons(SERVER_PORT);
    

    // Bind to the local address
    if (bind(serverfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("bind() failed\n");
        exit(1);
    }
    
    /* accept upto 5 pending requests
    * and make the server socket a passive socket
    */
    if (listen(serverfd, 5) == -1) {
            printf("Listen Failed\n");
            exit(1);
    }

    /* accept is a blocking call and the call would stop further invocation of
    code till a client connection is accepted */
    client_length = sizeof(client_addr);
    
    for (;;) {
        clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &client_length);
        printf("client connected\n");

        if (clientfd < 0) {
            printf("Unable to connect to Client FD\n");
            exit(1);
        }

        /* read and write data from/to client socket */
        for (;;) {
            struct action client_msg;
            /* Use the client socket FD to read data from the client */
            len = recv(clientfd, &client_msg, sizeof(struct action), 0);
            if (len != sizeof(struct action)){
                if (DEBUG) {
                    printf("Error: Receive a different number os bytes\n");
                    printf("Error: Bytes receive: %d  |  Bytes expected: 76\n", len);
                    break;
                }
            }

            if(client_msg.type == 7) break;
            
            struct action server_msg = receive_client_message(client_msg);

            /*************************************************/
            /* Send data buffer to the worker job            */
            /*************************************************/
            len = send(clientfd, &server_msg, sizeof(struct action), 0);
            if (len != sizeof(struct action))
            {
                perror("send");
                close(clientfd);
                exit(-1);
            }
        }
        printf("client disconnected\n");    
        close(clientfd);
    }


    // struct action server_msg;
    // struct action client_msg;
    
    // // Simula o inicio de jogo
    // client_msg.type = 0;

    // for(;;){
    //     server_msg = receive_client_message(client_msg);
    //     // printf("server_msg: %d\n", server_msg.type);
    //     client_msg = simula_client(server_msg);
    //     // printf("client_msg: %d\n", client_msg.type);

    //     if(client_msg.type == 7) {
    //         break;
    //     }
    //     else if(client_msg.type < 0) {
    //         printf("Erro menssage\n");
    //     }
    // }
    close(serverfd);

    // print_board(game_board.board);
    return 0;
}

void arg_parsing(int argc, char *argv[]) {
    int opt;

    if (argc < 5) {
        printf("Number of Arguments: %d\n", argc);       
        exit(1);
    }

    if (strcmp("v4", argv[1]) == 0) IP_version = 0;
    else if (strcmp("v6", argv[1]) == 0) IP_version = 1;
    else {
        printf("Invalid Ip version\n");
        exit(1);
    }

    sscanf(argv[2], "%d", &SERVER_PORT);

    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch (opt) {
        case 'i': 
            // printf("%s\n", optarg);
            strcpy(filename, optarg); 
            break;
        
        default:
            fprintf(stderr, "Usage: %s [-i] [files...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void load_input(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: file cannot be open");
        exit(1);
    }

    struct Board board;

    for(size_t i = 0; i < BOARD_N_ROWS; ++i)
    {
        for(size_t j = 0; j < BOARD_N_ROWS; ++j) {
            int number;
            fscanf(file, "%d,", &number);
            board.board[i][j] = number;
        }
    }

    fclose(file);
    load_game(board);
}

struct action receive_client_message(struct action act) {
    if (DEBUG) {
        printf("--------------------------------------------\n");
        printf("Server\n");
        printf("Client Message Type: %d\n", act.type);
        printf("--------------------------------------------\n");
    }

   
    switch (act.type)
    {   
    case 0:
        return server_start_game();
    case 1:
        return server_reveal_cell(act.coordinates);
    case 2:
        return server_flag_cell(act.coordinates);
    // Server -> Cliente
    // State -> Enviada pelo servidor ao cliente sempre que 
    // houver mudança no estado do board
    // case 3:
    //     server_start_game();
    //     break;
    case 4:
        return server_remove_flag(act.coordinates);
    case 5:
        return server_reset_game();
    // Server -> Cliente
    // Win -> Enviada pelo servidor ao cliente quando o jogo for ganho
    // case 6:
    //     server_start_game();
    //     break;
    // case 7:
    //     close_conection();
    //     break;
    // case 8:
    //     server_game_over();
    //     break;
    default:
        printf("Action not found\n");
        exit(1);
        break;
    }
}

struct action server_remove_flag(int coord[2]) {
    struct action server_answer;
    server_answer.type = remove_flag(coord);
    copy_matrix(get_game_board().board, server_answer.board);

    return server_answer;
}

struct action server_flag_cell(int coord[2]) {
    struct action server_answer;
    server_answer.type = set_flag(coord);
    copy_matrix(get_game_board().board, server_answer.board);

    return server_answer;
}

struct action server_reveal_cell(int coord[2]) {
    if(DEBUG) printf("x: %d, y: %d\n", coord[0], coord[1]);
    
    struct action server_answer;
    server_answer.type = reveal_move(coord);

    // If Game Over
    if (server_answer.type == 8) copy_matrix(get_review_board().board, server_answer.board);
    else if (server_answer.type == 6) copy_matrix(get_review_board().board, server_answer.board);
    else copy_matrix(get_game_board().board, server_answer.board);

    if(DEBUG) print_board(board_answer.board);   

    // send_to_client_board(server_answer);
    return server_answer;
}
 

struct action server_start_game() {
    start_game();
    struct action server_answer;
    server_answer.type = 3;
    copy_matrix(game_board.board, server_answer.board);
    return server_answer;

}

struct action server_reset_game() {
    reset_game();
    printf("starting new game\n");
    struct action server_answer;
    server_answer.type = 3;
    copy_matrix(get_game_board().board, server_answer.board);
    return server_answer;
}

void interrupt_handler (int signum) {
    close(serverfd);

    printf("socket connection closed\n");
    exit(0);
}