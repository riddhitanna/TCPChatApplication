#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int clientCount = 0;		// keeps count of the total number of clients
int clients[1024];			//to store the address of the clients
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{
	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;

};	// stores the details of a perticular client

struct client Client[1024];		
pthread_t thread[1024];


//OO MODE

void * doNetworking(void * ClientDetail){
	char countchar[100];
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
	printf("Client %d connected.\n",index + 1);
	int sendto_int; // saves which client the message is to be sent to
	while(1){
		char data[1024];
        char sendto[1024];
		int count=0;
		for(int i=0;i<clientCount;i++){
			if(clients[i] == clientSocket){
				count=i;
		    }
		}
        int read2 = recv(clientSocket, sendto, 1024,0);
        int read = recv(clientSocket,data,1024,0);
		if(sendto=="Q"){
			close(clientSocket);
			break;
		}
        sendto_int=atoi(sendto);
		printf("\n Message from client %d: %s",count+1, data);
        printf("\n The client wants to send this message to client %d", sendto_int);
		sprintf(countchar, "%d", count+1);
		send(Client[sendto_int-1].sockID, &countchar, 1024, 0);	
		send(Client[sendto_int-1].sockID, &data, 1024, 0);	
        printf("\n Message sent to client %d", sendto_int);
	}
	return NULL;
}


// BC MODE
void * doNetworking2(void * ClientDetail){
	char countchar[100];
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
	printf("Client %d connected.\n",index + 1);
	int sendto_int; // saves which client the message is to be sent to
	while(1){
		char data[1024];
        char sendto[1024];
		int count=0;
		for(int i=0;i<clientCount;i++){
			if(clients[i] == clientSocket){
				count=i;
		    }
		}
        int read2 = recv(clientSocket, sendto, 1024,0);
        int read = recv(clientSocket,data,1024,0);
		if(sendto=="Q"){
			close(clientSocket);
			break;
		}
        sendto_int=atoi(sendto);
		printf("\n Message from client %d: %s",count+1, data);
        printf("\n This message will be sent to all clients since we are in the BC mode.");
		sprintf(countchar, "%d", count+1);
		for(int i=0; i<clientCount; i++){
			if(clients[i]!=clientSocket){
				send(Client[i].sockID, &countchar, 1024, 0);	
			    send(Client[i].sockID, &data, 1024, 0);	
        	    printf("\n Message sent to client %d", i+1);
			}
		}
	}
	return NULL;
}




int main(){
	int mode;
	printf("\n Enter the mode: 1. OO 2. BC \n");
	scanf("%d", &mode);
	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);		// create server socket 
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;	//binding
	if(listen(serverSocket,1024) == -1) return 0;	//listening
	printf("Server started listenting on port 9000 ...\n");

	//OO MODE
	if (mode==1){
		while(1){
		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;
        clients[clientCount]= Client[clientCount].sockID;
		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);
		clientCount ++;
		
	}
	}

	// BC MODE
	else if(mode==2){
		while(1){
		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;
        clients[clientCount]= Client[clientCount].sockID;
		pthread_create(&thread[clientCount], NULL, doNetworking2, (void *) &Client[clientCount]);
		clientCount ++;
	}
	}
	

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);
	return 0;
}