#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MSG_SIZE 128
#define SA struct sockaddr
char* serverAddress;
const int Distance = 'a'-'A';

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

void serverFunction(int sockfd) {
	char buffer[MAX_MSG_SIZE];

	// infinite loop for chat
	while (1) {
		// clear buffer
		bzero(buffer, MAX_MSG_SIZE);
		// read the message from client and copy it in buffer
		read(sockfd, buffer, sizeof(buffer));
		// exchange upper-case letters by lower-case letter and vice versa.
		changeCase(buffer);

		// and send that buffer to client
		write(sockfd, buffer, sizeof(buffer));

		// if msg contains "QUIT" then server exit and chat ended.
		if (strncmp("QUIT", buffer, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

int setupConnection(int _port) {
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, client;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "Error: Cannot create socket --- exit\n");
		exit(3);
	} else {
		printf("Socket created.\n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// set up socket
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(serverAddress);
	servaddr.sin_port = htons(_port);

	// Binding socket to any IP
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		fprintf(stderr, "Error: Cannot bind socket --- exit\n");
		exit(4);
	} else {
		printf("Socket bound.\n");
	}

	// Server listens
	if ((listen(sockfd, 5)) != 0) {
		fprintf(stderr, "Error: Cannot listen --- exit\n");
		exit(5);
	} else { 
		printf("Server listening.\n");
	}
	len = sizeof(client);

	// Accept data packet from client
	connfd = accept(sockfd, (SA*)&client, &len);
	if (connfd < 0) {
		fprintf(stderr, "Error: Server accept failed --- exit\n");
		exit(6);
	} else {
		printf("Server accept client.\n");
	}

	return connfd;
}

void permantConnection(int _port) {
	int sockfd = setupConnection(_port); 
	serverFunction(sockfd);
	close(sockfd);
}
//TODO threads mit 5 p2plients
int main(int argc, char** argv) {
  if (argc > 6 || argc == 1) {
    fprintf(stderr, "usage: %s ip1...ip5 --- exit\n", argv[0]);
    return 1;
  }

  for (size_t i = 1; i < 6; i++) {
    printf("%s", argv[i]);
  }

  int port = 4567;
  serverAddress = argv[1];

  permantConnection(port);

}