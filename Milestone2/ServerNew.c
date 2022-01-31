#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define IP_ADD_SIZE 15
#define MAX_CLIENTS 5
#define MAX_MSG_SIZE 128


//send quit in all clients
void abortAllConnections() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    printf("Closing socket of a client %d and peer connection\n", i);
  }
}

//sets up connection, without sending message bind, thread erstellt
void connectClientToPeer(int _clientID, char* _serverAddress, int _port) {
  printf("Connecting Client %d to %s with port %d\n", _clientID, _serverAddress,
         _port);
}

//send quit only in one client
void abortClientPeerConnection(int _clientID) {
  printf("Closing socket of a client %d and peer connection\n", _clientID);
}

//sends message to one client, this client sends it to p2pclient
void sendTextToClient(int _clientID, char* message) {
  printf("Send Message %s to client %d\n", message, _clientID);
}

//TODO ports für p2plients
//TODO  socketprogrammierung implementieren
//Threads für Client copy vom PthreadMutex.c vom Prof
int main() {
  int port = 4567;
  char cmd;
  char serverAddress[IP_ADD_SIZE];
  char buffer[MAX_MSG_SIZE];
  int clientID = 0;
  int c = 0;
  int running = 1;

  while (running) {
    printf("Zahl 1-5 oder C,D,Q: ");

    scanf(" %c", &cmd);
    switch (cmd) {
      case 'C':
        printf("IP of P2P Client: ");
        scanf("%s", serverAddress);
        connectClientToPeer(clientID, serverAddress, port);  // setupConnection
        break;
      case 'D':
        printf("Zahl 1-5: ");
        scanf("%d", &clientID);
        abortClientPeerConnection(clientID);
        break;

      case 'Q':
        abortAllConnections();
        running = 0;
        break;
      case '0':
        int clientID = 0;
        printf("Message eingeben: ");
        scanf("%s", buffer);
        sendTextToClient(clientID, buffer);
        break;
      case '1':
        int clientID = 1;
        printf("Message eingeben: ");
        scanf("%s", buffer);
        sendTextToClient(clientID, buffer);
        break;
      case '2':
        int clientID = 2;
        printf("Message eingeben: ");
        scanf("%s", buffer);
        sendTextToClient(clientID, buffer);
        break;
      case '3':
        int clientID = 3;
        printf("Message eingeben: ");
        scanf("%s", buffer);
        sendTextToClient(clientID, buffer);
        break;
      case '4':
        int clientID = 4;
        printf("Message eingeben: ");
        scanf("%s", buffer);
        sendTextToClient(clientID, buffer);
        break;
    }
  }
}