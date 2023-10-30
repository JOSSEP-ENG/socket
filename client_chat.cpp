#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int client_socket;
std::string userName;

void message_send();
void message_recv();

int main(int argc, char* argv[]){
	//struct sockaddr_in client_address;
	//char* port = argv[2];
	//create client socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		printf("create socket failed...\n");
	}

	// connect
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("192.168.1.19");
	server_address.sin_port = htons(9090);
	//inet_pton(AF_INET, argv[1], &server_address.sin_addr.s_addr);

	if(connect(client_socket,(struct sockaddr*)&server_address, sizeof(server_address))== -1) {
		printf("connecting failed...\n");
		close(client_socket);
		return 0;
	}

	printf("Connected to server\n");
	
	char infoMessage[128];
	int bytes_received = recv(client_socket, infoMessage, sizeof(infoMessage), 0);
	if(bytes_received == -1) {
		std::cerr << "Error receiving message from server" << std::endl;
		close(client_socket);
		return EXIT_FAILURE;
	}
	else {
		infoMessage[bytes_received] = '\0';
		std::cout << infoMessage << std::endl;
	}
	
	std::cin >> userName;
	send(client_socket, userName.c_str(), userName.length(), 0);

	char loginMessage[128];
	bytes_received = recv(client_socket, loginMessage, sizeof(loginMessage), 0);
	if(bytes_received == -1) {
		std::cerr << "Error receiving message from server" << std::endl;
		close(client_socket);
		return EXIT_FAILURE;
	}
	else {
		loginMessage[bytes_received] = '\0';
		std::cout << loginMessage << std::endl; 
	}

	std::thread thread_send(message_send);
	std::thread thread_recv(message_recv);

	thread_send.join();
	thread_recv.join();
	/*
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
	*/

	// close
	close(client_socket);

	return EXIT_SUCCESS;
}

void message_send() {
	std::string sendMessage;
	while(1) {
		std::getline(std::cin,sendMessage);
		sendMessage = "["+userName+"]: "+sendMessage;
		send(client_socket, sendMessage.c_str(), sendMessage.length(), 0);
	}
}

void message_recv() {
	char recv_buffer[1024];
	int recvBytes = 0;
	while(1) {
		recvBytes = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
		if(recvBytes > 0) {
			recv_buffer[recvBytes] = '\0';
			std::cout<<recv_buffer<<std::endl;
		}
	}
}


