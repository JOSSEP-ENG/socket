#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	int client_socket;
	struct sockaddr_in client_address;
	struct sockaddr_in server_address;
	char* port = argv[2];
	//create client socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		printf("create socket failed...\n");
	}

	// connect
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server_address.sin_addr.s_addr);

	if(connect(client_socket,(struct sockaddr*)&server_address, sizeof(server_address))== -1) {
		printf("connecting failed...\n");
		close(client_socket);
		return 0;
	}

	printf("Connected to server\n");
	
	// read
	char buffer[1024];
	int bytesRead = recv(client_socket, buffer, sizeof(buffer),0);
	if(bytesRead == -1) {
		printf("Error receiving message from server\n");
	}
	else {
		printf("Received response from sever : %s\n",buffer);
	}

	// write
	const char* message = "Hello from Client!\n";
	int bytesSent = send(client_socket, message, (int)strlen(message),0);
	if(bytesSent == -1){
		printf("Error sending data to server\n");
	}
	
	// read	
	bytesRead = recv(client_socket, buffer, sizeof(buffer),0);
	if(bytesRead == -1) {
		printf("Error receiving message from server\n");
	}
	else {
		printf("echo : %s\n", buffer);
	}

	// close
	close(client_socket);

	return 0;
}
