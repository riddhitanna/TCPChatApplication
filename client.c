
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<signal.h>

#define PORT 9000
int clientSocket, ret;
void* sighandler(){
	char c[1]="Q";
	send(clientSocket, &c, strlen(c), 0);
	close(clientSocket);
	printf("Disconnected from server\n");
	exit(1);	
}


int main(){
	signal(SIGINT, sighandler);
	struct sockaddr_in serverAddr;
	char buffer[1024], recvfrom[1024];  //buffer stores the message and recvfrom stores which client the message is received from
    char sendto[1];		// stores the client to which the incoming message is to be sent to 
	int recvfrom_int;
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);		// create a client socket 
	if(clientSocket < 0){
		printf("Error in creating the client socket \n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));		// connect to server 
	if(ret < 0){
		printf("Error in connecting to the server..\n");
		exit(1);
	}
	printf("Connected to Server!\n");

	while(1){
        printf("\n Enter the client number to which you wish to send this message to: ");
        scanf("%s", sendto);
        send(clientSocket, &sendto, strlen(sendto), 0);
		printf("Message from Client: \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, strlen(buffer), 0);
		if(strcmp(buffer, "Q") == 0){
			close(clientSocket);
			printf("Disconnected from server\n");
			exit(1);
		}
		if(recv(clientSocket, recvfrom, 1024, 0) < 0){
			printf("Error in receiving data.\n");
		}
		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("Error in receiving data.\n");
		}
		else{
			printf("Message from client %s: \t%s\n", recvfrom, buffer);
		}
	}
	return 0;
}