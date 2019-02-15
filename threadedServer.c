#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> //threading, obviously

/*
	Basic server creation:
	1) Create socket
	2) Bind socket
	3) Listen for connections
	4) Accept connections
	5) Send and recieve data
*/

#define MAX_THREADS 3

void *connectionHandler(void *socketDesc)
{
	//Get socket descriptor
	int socket = *(int *)socketDesc;
	int readSize;
	char *message, clientMessage[1000];

	//Send messages to client(s?)
	message = "Hello, I am your host.\nI will echo your message.\n";
	write(socket, message, strlen(message));

	//Receive data
	while((readSize = read(socket, clientMessage, sizeof(clientMessage))) > 0)
	{
		puts(clientMessage);
		write(socket, clientMessage, sizeof(clientMessage));
		
		memset(&clientMessage, 0, sizeof(clientMessage));
	}

	if(readSize <= 0)
	{
		printf("Client has disconnected, or connection has been lost.\n");
		fflush(stdout);
	}

	//Free socket pointer
	free(socketDesc);

	return 0;
}

int main(int argc, char *argv[])
{
	int socketDesc, clientSocket, c, readSize, port, *newSocket, i;
	struct sockaddr_in server, client;
	char clientMessage[2000];
	
	if(argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		return 1;
	}

	//Create the socket
	socketDesc = socket(AF_INET, SOCK_STREAM, 0);
	
	printf("Creating socket...\n");

	if(socketDesc == -1) printf("Could not create socket.");

	printf("Socket created.\n");


	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	printf("Binding socket...\n");

	//Bind socket
	if(bind(socketDesc, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		perror("Failed to bind the socket.");
		return 1;
	}

	printf("Socket bound.\n");

	//Listen for incoming connections, maximum of 3 (Argument, maybe?)
	listen(socketDesc, 3);	
	
	//Accept incoming connection(s)
	printf("Waiting for a connection...\n");
	c = sizeof(struct sockaddr_in);

	pthread_t threadID;	

	while((clientSocket = accept(socketDesc, (struct sockaddr*)&client, (socklen_t*)&c)))
	{		
		printf("Connection to client %d established.\n", i);
		
		newSocket = malloc(1);
		*newSocket = clientSocket;

		if((pthread_create(&threadID, NULL, connectionHandler, (void *)newSocket)) < 0)
		{
			printf("Could not initialize thread %d.", i);
			return 1;
		}
		
		printf("Thread assigned to client %d.\n", i);
	}
	pthread_join(threadID, NULL);

	if(clientSocket < 0)
	{
		perror("Connection failed.");
		return 1;
	}

	return 0;
}

















