#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE  1
#define FALSE 0

#define CLIENTS_TURN 0
#define SERVERS_TURN 1

// Variable zum beenden der Threads
int running = TRUE;
// Buffer fuer den String
char str[128];
// Rundenvariable zum speichern welcher Thread was machen soll
int turn = CLIENTS_TURN;

// Funktion fuer den Client, um Eingaben zu machen.
void* client(void* arg) {
	while(running == TRUE) {
		// Aktive warten bis der Client dran ist (nichtschoen aber OK)
		while(turn != CLIENTS_TURN)
			sleep(1);
		printf("Client: Mach eingabe: ");
		scanf("%s", str);
		if(strcmp(str, "quit") == 0) {
			// beenden des Threads
			running = FALSE;
		}
		// Sagen das der Server dran ist
		turn = SERVERS_TURN;
	}
	return NULL;
}

// Funktion fuer den Server, um Ausgaben zu machen.
void* server(void* arg) {
	while(running == TRUE) {
		// Aktive warten bis der Server dran ist (nichtschoen aber OK)
		while(turn != SERVERS_TURN)
			sleep(1);
		printf("Server: %s\n", str);
		// Sagen das der Client dran ist
		turn = CLIENTS_TURN;
	}
	return NULL;
}

int main(int argc, char** argv) {
	pthread_t cl;
	pthread_t se;

	// Erzeugen der Threads
	pthread_create(&cl, NULL, client, NULL);
	pthread_create(&se, NULL, server, NULL);
	
	// Warten bis alle Threads beendet sind
	pthread_join(cl, NULL);
	pthread_join(se, NULL);

	return 0;
}
