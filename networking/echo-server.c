#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr

static void usage_print();

static int socket_create();
static struct sockaddr_in server_address_create();

// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
	char buff[MAX]; 
	int n; 
	// infinite loop for chat 
	for (;;) { 
		memset(buff, 0, MAX);

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff)); 
		// print buffer which contains the client contents 
		printf("From client: %s\t To client : ", buff); 
		memset(buff, 0, MAX);
		n = 0; 
		// copy server message in the buffer 
		while ((buff[n++] = getchar()) != '\n') 
			; 

		// and send that buffer to client 
		write(sockfd, buff, sizeof(buff)); 

		// if msg contains "Exit" then server exit and chat ended. 
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} 
	} 
} 

// Driver function 
int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "error: please, provider exactly 2 arguments (see usage below)")
		usage_print();
		return EXIT_FAILURE;
	}
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr = {0}, cli = {0}; 

	// socket create and verification 
	int socket_fd = socket_create();

	const char* host = argv[1];
	const char* port = argv[2];
	struct sockaddr_in server_address = server_address_create();

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

	// Function for chatting between client and server 
	func(connfd); 

	// After chatting close the socket 
	close(sockfd); 
}

static void usage_print() {
	printf("usage: echo-server <IP> <PORT>\n\n");
	printf("IP:\tThis is the binding address that the server will use to listen for connections\n");
	printf("PORT:\tThis is the \n");
}

static int socket_create() {
	int domain = AF_INET;
	int type = SOCK_STREAM;
	int protocol = 0;

	int socket_fd = socket(domain, type, protocol); 
	if (socket_fd == -1) { 
		fprintf(stderr, "error: failed to create socket\n");
		exit(EXIT_FAILURE);
	} 

	puts("Socket successfully created...");
	return socket_fd;
}

static struct sockaddr_in server_address_create() {
	struct sockaddr_in address = {0};

	// assign IP, PORT 
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	return address;
}
