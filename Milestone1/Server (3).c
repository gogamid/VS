#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define MESG_SIZE 80
#define SA struct sockaddr
#define CON_PERMA 1
#define CON_FIRST 0
#define MAX_CLIENTS 50

int discon_ctr = 0;
int consocket = 0;
pthread_t threads[MAX_CLIENTS];
const int Distance = 'a'-'A';

typedef struct {
    int thread_ID;
    int sockfd;
} thread;

// Function designed for chat between client and server.
void changeCase(char* _str) {
    while (*_str != 0) {
        if(*_str >= 'a'  && *_str <= 'z') {
            *_str -= Distance;
        } else if(*_str >= 'A'  && *_str <= 'Z') {
            *_str += Distance;
        }
        _str++;
    }
}

void* serverFunction(void* _param) {
    int thread_ID = ((thread*)_param)->thread_ID;
    int sockfd = ((thread*)_param)->sockfd;
    char buffer[MESG_SIZE];

    // infinite loop for chat
    while (1) {
        // clear buffer
        bzero(buffer, MESG_SIZE);

        if(read(sockfd, buffer, sizeof(buffer)) <= 0) {

            fprintf(stderr, "Client exit with clt+c\n");
            discon_ctr = discon_ctr + 1;
            break;
        }
        // print message on serverside
        printf("CLIENT %d Message: %s", thread_ID, buffer);
        // exchange upper-case letters by lower-case letter and vice versa.
        changeCase(buffer);

        // and send that buffer to client
        write(sockfd, buffer, sizeof(buffer));

        // if msg contains "QUIT" then server exit and chat ended.
        if (strncmp("QUIT", buffer, 4) == 0) {
            printf("Exiting\n");
            discon_ctr = discon_ctr + 1;
            break;
        }
    }

    close(sockfd);
    pthread_join(threads[thread_ID], NULL);
}

int setupConnection(int _port, int con_func) {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, client;

    // socket initialisation
    if((consocket == 0 && con_func == CON_FIRST) || con_func == CON_PERMA) {
        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            fprintf(stderr, "ERROR: Cannot create socket --- exit\n");
            exit(3);
        } else {
            printf("Socket created.\n");
        }
        bzero(&servaddr, sizeof(servaddr));

        // set up socket
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(_port);

        // Binding socket to any IP
        if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            fprintf(stderr, "ERROR: Cannot bind socket --- exit\n");
            exit(4);
        } else {
            printf("Socket bound.\n");
        }
    } else {
        sockfd = consocket;
    }

    if(consocket == 0) {
        consocket = sockfd;
    }

    // Server listens
    if ((listen(sockfd, 5)) != 0) {
        fprintf(stderr, "Cannot listen --- exit\n");
        exit(5);
    } else {
        printf("listening...\n");
    }
    len = sizeof(client);

    // Accept data packet from client
    connfd = accept(sockfd, (SA*)&client, &len);
    if (connfd < 0) {
        fprintf(stderr, "Server accept failed --- exit\n");
        exit(6);
    } else {
        printf("accept client %d.\n", _port);
    }

    return connfd;
}

int firstTouch(int _port) {
    int sockfd = setupConnection(_port, CON_FIRST);
    char buffer[MESG_SIZE];
    bzero(buffer, MESG_SIZE);
    read(sockfd, buffer, MESG_SIZE);
    printf("received: %s\n", buffer);
    ((int*)buffer)[0] = _port + 1;
    write(sockfd, buffer, MESG_SIZE);
    return _port + 1;
}

int main(int argc, char** argv) {
    int port = 4567;

    if (argc > 2) {
        fprintf(stderr, "SERVER usage: %s [port] --- exit\n", argv[0]);
        return 1;
    }
    if(argc == 2) {
        char* tmp = argv[1];
        while(tmp[0] != 0) {
            if(!isdigit((int)tmp[0])) {
                fprintf(stderr, "ERROR: %s is no valid port --- exit\n", argv[1]);
                return 2;
            }
            tmp++;
        }
        port = atoi(argv[1]);
    }


    int thread_ID = 0;

    thread* param = malloc(sizeof(thread));

    while(1) {
        //port
        port = firstTouch(port);

       if (thread_ID >= MAX_CLIENTS && discon_ctr == 0) {
           printf("Max Connections already reached = %d\n", MAX_CLIENTS);
          continue;
       } else if (thread_ID >= MAX_CLIENTS && discon_ctr > 0) {
            discon_ctr = discon_ctr - 1;
       }

        // define parameters
        param->thread_ID = thread_ID;
        param->sockfd = setupConnection(port, CON_PERMA);
        //create a thread
        pthread_create(&threads[thread_ID], NULL, serverFunction, param);

        thread_ID ++;

    }

    return 0;
}
