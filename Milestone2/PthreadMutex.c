#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_CLIENTS  5
#define MAX_MSG_SIZE 128

typedef struct {
	int clientID;  // ID von der verschicken soll
	char mesg[MAX_MSG_SIZE];  // Nachricht die verschickt werden
} MessageForClient; 

typedef struct {   
	int ID;
	MessageForClient* mfc;
} ThreadData;
#define SA struct sockaddr

MessageForClient Message;  // Globale Nachricht zum austausch für alle
pthread_t clients[MAX_CLIENTS];  // Alle Thread/Clients
pthread_mutex_t mut;  
int clientID[]={0,0,0,0,0}; // if busy 1 

void clientFunction(int sockfd,char* buffer1) {
	char buffer[MAX_MSG_SIZE];
	int n;
	// while (1) {
		// bzero(buffer, sizeof(buffer));
		// printf("Enter string: ");
		// n = 0;
		// while ((buffer[n++] = getchar()) != '\n')
		// 	;
		write(sockfd, buffer1, sizeof(buffer1));
		bzero(buffer1, sizeof(buffer1));
		read(sockfd, buffer, sizeof(buffer));
		printf("From Server : %s", buffer);
		// if ((strncmp(buffer, "QUIT", 4)) == 0) {
		// 	printf("Client exit.\n");
		// 	break;
		// }
	// }
}

int setupConnection(char* _serverAddress, int _port) {
	int sockfd;
	struct sockaddr_in servaddr;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "Error: Cannot create socket --- exit\n");
		return -3;
	} else {
		printf("Socket successfully created..\n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// Set up socket
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(_serverAddress);
	servaddr.sin_port = htons(_port);

	// Connect to server
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		fprintf(stderr, "Error: Cannot connect server --- exit\n");
		return -4;
	} else {
		printf("connected to the server..\n");
	}

	return sockfd;
}


void permanentConnection(char* _serverAddress, int _port, char* buffer) {
	int sockfd = setupConnection(_serverAddress, _port);
	int retries = 5;
	while(sockfd < 0 && retries > 0) {
		sleep(1);
		sockfd = setupConnection(_serverAddress, _port);
		retries--;
	}
	if(retries == 0 && sockfd < 0) {
		fprintf(stderr, "Error: Cannot reconnect server --- exit\n");
		exit(-1*sockfd);
	}
	clientFunction(sockfd, buffer);
	// close(sockfd);
} 

int setMessageForClient(int _id, char* _str) {
	if(_id >= MAX_CLIENTS) {
		fprintf(stderr, "Error: no such client\n");
		return -1;
	}
	pthread_mutex_lock(&mut);
	if(Message.clientID != -1) {
		pthread_mutex_unlock(&mut);
		return 0;
	}
	Message.clientID = _id;
	strncpy(Message.mesg, _str, MAX_MSG_SIZE);
	pthread_mutex_unlock(&mut);
	return 1;
}

void* ThreadFunc(void* _data) {
	ThreadData data = *((ThreadData*)_data);
	int myID = data.ID;
	MessageForClient* mfc = data.mfc;
	char buffer[MAX_MSG_SIZE];

	while(1) {
		pthread_mutex_lock(&mut);
		if(mfc->clientID == myID) {
			strncpy(buffer, mfc->mesg, MAX_MSG_SIZE);
			printf("%d: got message: %s\n", myID, buffer);
			mfc->clientID = -1;
			//socket client
			char* serverAddress = "127.0.0.1";
			int port = 4567;
			permanentConnection(serverAddress, port, buffer);

			pthread_mutex_unlock(&mut);
			if(strncmp(buffer, "quit", 4) == 0)
				break;
		} else {
			pthread_mutex_unlock(&mut);
			usleep(100);
		}
	}
	return NULL;
}

//TODO take creation of 5 CLient Threads
int main() {
	ThreadData td[MAX_CLIENTS];
	for(int i = 1; i < MAX_CLIENTS; i++) {
		td[i].ID = i;
		td[i].mfc = &Message;
	}
	pthread_mutex_init(&mut, NULL);

	for(int i = 0; i < 5; i++) {
		pthread_create(&clients[i], NULL, ThreadFunc, &(td[i]));
	}

	char buffer[MAX_MSG_SIZE];
	int target;
	Message.clientID = -1;
	while(1) {
		while(Message.clientID != -1) {
			usleep(100);
		}
		printf("Ziel: ");
		scanf("%d", &target);
		printf("Nachricht: ");
		scanf("%s", buffer);

		while(!setMessageForClient(target, buffer))
			usleep(100);
	}

	return 0;
}
