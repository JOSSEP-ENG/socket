#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <map>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

void addClient(int, const std::string&);
void removeClient(int);
void closeSocket();
void thread_chat(int);
void error_handling(const std::string&);

std::map<int,std::string> clientInfo;

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


	std::string infoMessage = "=======Enter your user name=======";
	char userName[128];
	int bytes_received = 0;

	while(1) {	
		struct sockaddr_in client_address;
		socklen_t client_len = sizeof(client_address);
		int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
		if(client_socket == -1) {
			error_handling("accept() error");
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		if(send(client_socket, infoMessage.c_str(), infoMessage.length(), 0) == -1) {
			error_handling("send() error");
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		bytes_received = recv(client_socket, userName, sizeof(userName), 0);
		if(bytes_received == -1) {
			error_handling("recv() error");
			close(server_socket);
			return EXIT_FAILURE;
		}
		else {
			userName[bytes_received] = '\0';
		}
			
		char addr[INET_ADDRSTRLEN];
		memset(addr,0,sizeof(addr));
		inet_ntop(AF_INET, &client_address.sin_addr, addr, sizeof(addr));
		std::cout << addr << " @ Connected to Server." << std::endl;

		addClient(client_socket, userName);
		std::thread thread_socket(thread_chat, client_socket);
		thread_socket.detach();
	}
	
	closeSocket();
	close(server_socket);
	return EXIT_SUCCESS;
}

void thread_chat(int socket) {
	char buffer[512];
	int bytes_received = 0;
	std::string loginMessage = "[" + clientInfo[socket] + "] has logged on.";
	for(const auto& pair : clientInfo) {
		send(pair.first, loginMessage.c_str(), loginMessage.length(), 0);
	}

	while(1) {
		bytes_received = recv(socket, buffer, sizeof(buffer), 0);
		if(bytes_received == -1) {
			error_handling("recv() error");
			close(socket);
			return;
		}
		else {
			buffer[bytes_received] = '\0';
		}

		for(const auto& pair : clientInfo) {
			if(send(pair.first, buffer, strlen(buffer), 0) == -1) {
				error_handling("send() error");
				close(socket);
				return;
			}
		}
	}
}

void addClient(int socket, const std::string& userName) {
	clientInfo[socket] = userName;
}

void removeClient(int socket) {
	clientInfo.erase(socket);
	close(socket);
}

void closeSocket() {
	for(auto& pair : clientInfo){
		int socket = pair.first;
		removeClient(socket);
	}
}

void error_handling(const std::string& message) {
	std::cerr << "Error: " << message << std::endl;
}

