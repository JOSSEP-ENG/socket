#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <codecvt>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

void error_handling(const std::string&);
void message_send();
void message_recv();

int client_socket;
char userName[128];

int main(int argc, char* argv[]) {
	
	if(argc !=2 ) {
		std::cerr << "Usage: " << argv[0] << "<port>" << std::endl;
	}	

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0) {	
		error_handling("socket() error");
		return EXIT_FAILURE;
	}
	
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		error_handling("bind() error");
		close(server_socket);
		return EXIT_FAILURE;
	}

	if(listen(server_socket, SOMAXCONN) == -1) {
		error_handling("listen() error");
		close(server_socket);
		return EXIT_FAILURE;
	}
	
	struct sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	
	std::string infoMessage = "Enter your user name.";
	const char* buffer = infoMessage.c_str();
	int infoMessageSize = strlen(buffer);
	std::string loginMessage;

	//while(1) {
		
	client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
	if(client_socket == -1) {
		error_handling("accept() error");
		close(server_socket);
		return EXIT_FAILURE;
	}
		
	int cnt = 0;
	while(1) {
		if(send(client_socket, buffer, infoMessageSize,0)==-1) {
			error_handling("send() error");
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		int bytes_received = recv(client_socket, userName, sizeof(userName), 0);
		if(bytes_received == -1) {
			cnt++;
			error_handling("recv() error");
			if(cnt >=3) {
				close(server_socket);
				return EXIT_FAILURE;
			}
		}
		else {
			userName[bytes_received] = '\0';
			break;
		}
	}
		
	char addr[INET_ADDRSTRLEN];
	memset(addr,0,sizeof(addr));
	inet_ntop(AF_INET, &client_address.sin_addr, addr, sizeof(addr));
	
	std::ostringstream oss;
	oss << "=====[" << addr << "@" << userName << "]has logged on====="<<std::endl;
	loginMessage = oss.str();
	std::cout << loginMessage << std::endl;	
	send(client_socket, loginMessage.c_str(),strlen(loginMessage.c_str()),0);
		
	//}

	std::thread thread_send(message_send);
	std::thread thread_recv(message_recv);

	thread_send.join();
	thread_recv.join();
		
	close(server_socket);
	return EXIT_SUCCESS;
}

void error_handling(const std::string& message) {
	std::cerr << "Error: " << message << std::endl;
}

void message_send() {
	std::string sendMessage;
	while(1) {
		std::getline(std::cin, sendMessage);
		sendMessage = std::string("[Server]: ")+sendMessage;
		send(client_socket,sendMessage.c_str(), sendMessage.length(), 0);
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


