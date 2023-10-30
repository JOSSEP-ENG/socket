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

	if(connect(client_socket,(struct sockaddr*)&server_address, sizeof(server_address))== -1) {
		printf("connecting failed...\n");
		close(client_socket);
		return 0;
	}

	std::cout << "Successed to connected to the Server!!" << std::endl;
	
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
	
	std::thread thread_send(message_send);
	std::thread thread_recv(message_recv);

	thread_send.join();
	thread_recv.join();

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


