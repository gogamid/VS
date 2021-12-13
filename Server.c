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

const int Distance = 'a' - 'A';
char client_message[2000];
char buffer[1024];
int c = 1;

// Function designed for chat between client and server.
void changeCase(char *_str)
{
	while (*_str != 0)
	{
		if (*_str >= 'a' && *_str <= 'z')
		{
			*_str -= Distance;
		}
		else if (*_str >= 'A' && *_str <= 'Z')
		{
			*_str += Distance;
		}
		_str++;
	}
}

void serverFunction(int sockfd)
{
	char buffer[MESG_SIZE];

	// infinite loop for chat
	while (1)
	{
		// clear buffer
		bzero(buffer, MESG_SIZE);
		// read the message from client and copy it in buffer
		read(sockfd, buffer, sizeof(buffer));
		// exchange upper-case letters by lower-case letter and vice versa.
		changeCase(buffer);

		// and send that buffer to client
		write(sockfd, buffer, sizeof(buffer));

		// if msg contains "QUIT" then server exit and chat ended.
		if (strncmp("QUIT", buffer, 4) == 0)
		{
			printf("Server Exit...\n");
			break;
		}
	}
}

int firstTouch(int _port, int _newSocket)
{
	int p = p + c++;
	int sockfd = _newSocket;
	char buffer[MESG_SIZE];
	bzero(buffer, MESG_SIZE);
	read(sockfd, buffer, MESG_SIZE);
	printf("received: %s\n", buffer);
	((int *)buffer)[0] = p;
	write(sockfd, buffer, MESG_SIZE);
	close(sockfd);
	return p;
}

int setupConnection(int _port)
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, client;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		fprintf(stderr, "Error: Cannot create socket --- exit\n");
		exit(3);
	}
	else
	{
		printf("Socket created.\n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// set up socket
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(_port);

	// Binding socket to any IP
	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
	{
		fprintf(stderr, "Error: Cannot bind socket --- exit\n");
		exit(4);
	}
	else
	{
		printf("Socket bound.\n");
	}

	// Server listens
	if ((listen(sockfd, 5)) != 0)
	{
		fprintf(stderr, "Error: Cannot listen --- exit\n");
		exit(5);
	}
	else
	{
		printf("Server listening.\n");
	}
	len = sizeof(client);

	// Accept data packet from client
	connfd = accept(sockfd, (SA *)&client, &len);
	if (connfd < 0)
	{
		fprintf(stderr, "Error: Server accept failed --- exit\n");
		exit(6);
	}
	else
	{
		printf("Server accept client.\n");
	}

	return connfd;
}

void permantConnection(int _port)
{

	int permSockID = setupConnection(_port);
	serverFunction(permSockID);
	close(permSockID);
}

void *socketThread(void *arg)
{
	int *arr = (int *)arg;
	int port = arr[0];
	int newSocket = arr[1];
	port = firstTouch(port, newSocket);
	permantConnection(port);

	close(newSocket);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int port = 4567;
	if (argc > 2)
	{
		fprintf(stderr, "usage: %s [port] --- exit\n", argv[0]);
		return 1;
	}
	if (argc == 2)
	{
		char *tmp = argv[1];
		while (tmp[0] != 0)
		{
			if (!isdigit((int)tmp[0]))
			{
				fprintf(stderr, "Error: %s is no valid port --- exit\n", argv[1]);
				return 2;
			}
			tmp++;
		}
		port = atoi(argv[1]);
	}

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, client;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		fprintf(stderr, "Error: Cannot create socket --- exit\n");
		exit(3);
	}
	else
	{
		printf("Socket created.\n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// set up socket
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// Binding socket to any IP
	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
	{
		fprintf(stderr, "Error: Cannot bind socket --- exit\n");
		exit(4);
	}
	else
	{
		printf("Socket bound.\n");
	}

	// Listen on the socket, with 40 max connection requests queued
	if (listen(sockfd, 50) == 0)
		printf("Listening\n");
	else
		printf("Error\n");
	pthread_t tid[60];
	int i = 0;
	while (1)
	{
		len = sizeof(client);

		// Accept data packet from client
		connfd = accept(sockfd, (SA *)&client, &len);
		if (connfd < 0)
		{
			fprintf(stderr, "Error: Server accept failed --- exit\n");
			exit(6);
		}
		else
		{
			printf("Server accept client.\n");
		}

		int arr[] = {port, connfd};

		if (pthread_create(&tid[i++], NULL, socketThread, (void *)arr) != 0)
			printf("Failed to create thread\n");

		if (i >= 50)
		{
			i = 0;
			while (i < 50)
			{
				pthread_join(tid[i++], NULL);
			}
			i = 0;
		}
	}
	return 0;
}
