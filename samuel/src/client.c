#include "client.h"

int main(int argc, char *argv[]) {
    arg_parsing(argc, argv);

    signal (SIGINT, interrupt_handler);

    // Initialize the variable hints
    memset(&hints, 0x00, sizeof(hints));
    // hints.ai_flags = AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    /********************************************************************/
    /* Check if we were provided the address of the server using        */
    /* inet_pton() to convert the text form of the address to binary    */
    /* form. If it is numeric then we want to prevent getaddrinfo()     */
    /* from doing any name resolution.                                  */
    /********************************************************************/
    rtnVal = inet_pton(AF_INET, input_addr, &server_addr);
    if (rtnVal == 1)    /* valid IPv4 text address? */
    {
        hints.ai_family = AF_INET;
        // hints.ai_flags |= AI_NUMERICHOST;
    }
    else
    {
        rtnVal = inet_pton(AF_INET6, input_addr, &server_addr);
        if (rtnVal == 1) /* valid IPv6 text address? */
        {

        hints.ai_family = AF_INET6;
        // hints.ai_flags |= AI_NUMERICHOST;
        }
    }
    /********************************************************************/
    /* Get the address information for the server using getaddrinfo().  */
    /* To open a socket client-server, we need some important           */
    /* information about the server that we want to connect to such as  */
    /* the domain address, the address family it uses, etc.             */
    /* Reference: https://linuxhint.com/c-getaddrinfo-function-usage/   */
    /********************************************************************/
    rtnVal = getaddrinfo(input_addr, SERVER_PORT, &hints, &res);
    if (rtnVal != 0)
    {
        printf("Host not found --> %s\n", gai_strerror(rtnVal));
        if (rtnVal == EAI_SYSTEM)
            printf("getaddrinfo() failed");
        exit(-1);
    }

    if (DEBUG) printf("Usa IPv6: %d\n", hints.ai_socktype);

    // Create a reliable, stream socket using TCP
    /*************************************************/
    /* Create an AF_INET6 stream socket              */
    /*************************************************/
    clientfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (clientfd < 0)
    {
      perror("socket");
      exit(-1);
    }

    /********************************************************************/
    /* Use the connect() function to establish a connection to the      */
    /* server.                                                          */
    /********************************************************************/
    rtnVal = connect(clientfd, res->ai_addr, res->ai_addrlen);
    if (rtnVal < 0)
    {
        /*****************************************************************/
        /* Note: the res is a linked list of addresses found for server. */
        /* If the connect() fails to the first one, subsequent addresses */
        /* (if any) in the list can be tried if required.               */
        /*****************************************************************/
        printf("connect() failed");
        exit(1);
    }

    /*************************************************/
    /* Initialize the socket address structure       */
    /*************************************************/
    
    
    // memset(&server_addr, 0, sizeof(server_addr));
    // server_addr.sin6_family = AF_INET6;
    
    // Program init
    struct action server_msg;
    server_msg.type = -1;
    struct action client_msg;
    
    for (;;) {
        client_msg = process_game_action(server_msg);

        /*************************************************/
        /* Send data buffer to the worker job            */
        /*************************************************/
        len = send(clientfd, &client_msg, sizeof(struct action), 0);
        if (len != sizeof(struct action))
        {
            perror("send");
            close(clientfd);
            exit(-1);
        }
        if(DEBUG) printf("%d bytes sent\n", len);

        /* Use the client socket FD to read data from the client */
        len = recv(clientfd, &server_msg, sizeof(struct action), 0);
        if (len != sizeof(struct action)){
            if(DEBUG) {
                printf("Error: Receive a different number os bytes\n");
                printf("Error: Bytes receive: %d  |  Bytes expected: 76\n", len);
            }
            break;
        }

        if (client_msg.type == 7) break;

        if (DEBUG) {
            printf("Server Reply type: %d \n", server_msg.type);
            print_matrix(server_msg.board);
        }
    }

    close(clientfd); 
    close(serverfd);

    return 0;
}

void arg_parsing(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Number of Arguments passed: %d\n", argc);       
        printf("Number of arguments needed: 3\n");
        exit(1);
    }
    
    memset(input_addr, 0, sizeof(input_addr));
    sscanf(argv[1], "%s", input_addr);
    sscanf(argv[2], "%s", SERVER_PORT);
}

struct action process_game_action(struct action act) {
    struct action client_action;
    if (DEBUG) {
        // printf("********************************************************\n");
        // printf("Cliente\n");
        // printf("Action Type: %d\n", act.type);
    }

    // Program init
    if (act.type == -1) {
        // print_board(act.board);
        client_action = get_client_input_and_validate(act.board);
    }
    else if (act.type == 3) {
        print_board(act.board);
        client_action = get_client_input_and_validate(act.board);
    }
    else if (act.type == 6) {
        printf("YOU WIN!\n");
        print_board(act.board);
        client_action = get_client_input_and_validate(act.board);
    }
    else if (act.type == 8) {
        printf("GAME OVER!\n");
        print_board(act.board);
        client_action = get_client_input_and_validate(act.board);
    }
    else {
        if (DEBUG) {
            printf("server answer: %d", act.type);
        }
        client_action = get_client_input_and_validate(act.board);
    }

    if(DEBUG) printf("********************************************************\n");
    return client_action;
}

int get_command_code(char *command) {
    if (strcmp(command, "start") == 0) 
    {
        return 0;
    } 
    else if (strcmp(command, "reveal") == 0)
    {
        return 1;
    }
    else if (strcmp(command, "flag") == 0)
    {
        return 2;
    }
    else if (strcmp(command, "remove_flag") == 0)
    {
        return 4;
    }
    else if (strcmp(command, "reset") == 0)
    {
        return 5;
    }
    else if (strcmp(command, "exit") == 0)
    {
        return 7;
    }
    /* more else if clauses */
    else /* default: */
    {
        // printf("Comando Invalido\n");
        return -1;
    }
}

struct action get_client_input_and_validate(int currBoard[BOARD_N_ROWS][BOARD_N_COLS]) {
    struct action client_action;
    char input_str[1000];
    
    do {
        // Le a entrada
        fgets(input_str, sizeof(input_str), stdin);

        // Faz o parsing da entrada do jogador
        client_action = parse_action(input_str);
    } while (!isActionValid(client_action, currBoard));

    return client_action;
}

int isActionValid(struct action act, int currBoard[BOARD_N_ROWS][BOARD_N_COLS]) {
    switch (act.type)
    {
    //Start Command
    case 0:
        // printf("\n")
        return 1;
    // Reveal Command
    case 1:
        if (act.coordinates[0] >= BOARD_N_ROWS || act.coordinates[1] >= BOARD_N_COLS) {
            printf("error: invalid cell\n");
            return 0;
        }
        // cell already revealed
        if (currBoard[act.coordinates[0]][act.coordinates[1]] >= 0 ) {
            printf("error: cell already revealed\n");
            return 0;
        }

        return 1;
    // Flag Command
    case 2:
        if (act.coordinates[0] >= BOARD_N_ROWS || act.coordinates[1] >= BOARD_N_COLS) {
            printf("error: invalid cell\n");
            return 0;
        }
        // cell already has a flag
        if (currBoard[act.coordinates[0]][act.coordinates[1]] == -3 ) {
            printf("error: cell already has a flag\n");
            return 0;   
        }
        // cannot insert flag in revealed cell
        if (currBoard[act.coordinates[0]][act.coordinates[1]] >= 0 ) {
            printf("error: cannot insert flag in revealed cell\n");
            return 0;   
        }
        return 1;
    // State -> Server Send This
    case 3:
        // printf("\n")
        return 1;
    // Remove Flag Command
    case 4:
        if (act.coordinates[0] >= BOARD_N_ROWS || act.coordinates[1] >= BOARD_N_COLS) {
            printf("error: invalid cell\n");
            return 0;
        }
        // cannot remove flag in revealed cell
        // But this error isn't important
        if (currBoard[act.coordinates[0]][act.coordinates[1]] >= 0 ) {
            return 0;
        }
        return 1;
    // Reset Command
    case 5:
        // printf("\n")
        return 1;

    case 7:
        return 1;

    default:
        printf("error: command not found\n");
        return 0;
    }
}

struct action parse_action(char *input) {
    char command[50];
    int coord_x, coord_y;
    sscanf(input, "%s %d,%d", command, &coord_x, &coord_y);

    struct action client_action;
    client_action.type = get_command_code(command);
    client_action.coordinates[0] = coord_x;
    client_action.coordinates[1] = coord_y;

    return client_action;
}

void interrupt_handler (int signum) {
    close(serverfd);

    printf("socket connection closed\n");
    exit(0);
};