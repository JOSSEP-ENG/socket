#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {

	int server_socket;
	pid_t child_process;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket==-1) {
		std::cerr<<"socket() error"<<std::endl;
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	memset(&server_address,	0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));	

	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))==-1) {
		std::cerr << "bind() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}

	if(listen(server_socket, SOMAXCONN)==-1) {
		std::cerr << "listen() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}
	
	while(1) {
	
		struct sockaddr_in client_address;
		socklen_t client_address_len = sizeof(client_address);
		
		int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
		if(client_socket == -1) {
			std::cerr << "accept() error" << std::endl;
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		pid_t child_pid = fork();
		if(child_pid == -1) {
			std::cerr << "fork() error" << std::endl;
			close(client_socket);
			close(server_socket);
			return EXIT_FAILURE;
		}

		// 자식 프로세스
		if(child_pid == 0) {
			close(server_socket);
			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &client_address.sin_addr, addr, sizeof(addr));
			std::cout << "[TCP Server] Client connected. " << addr << ":" 
			<< ntohs(client_address.sin_port) << std::endl;

			while(1) {
				char buffer[1024];
				int bytesRead = recv(client_socket, buffer, 1024, 0);
				if(bytesRead == -1) {
					std::cerr << "recv() error" << std::endl;
					close(client_socket);
					return EXIT_FAILURE;
				}
				else if(bytesRead==0) {
					break;
				}
				buffer[bytesRead] = '\0';
				std::cout << "[TCP/" << addr << "]" << buffer << std::endl;
			}
			close(client_socket);
			std::cout<<"[TCP Server] Close connected : IP address = " << addr << std::endl;
			return EXIT_SUCCESS;
		}
		// 부모 프로세스
		else {
			std::cout << "child_pid : " << child_pid << std::endl;
		}
	}
	close(server_socket);
	return EXIT_SUCCESS;
}
