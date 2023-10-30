#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <string>
#include <WinSock2.h>

SOCKET client_socket;
std::string userName;

void message_send();
void message_recv();

int main() {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Error WSAStartup" << std::endl;
		return EXIT_FAILURE;
	}

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		std::cerr << "Error creating client socket" << std::endl;
		WSACleanup();
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("192.168.1.19");
	server_address.sin_port = htons(9090);

	if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
		std::cerr << "Error connecting to server" << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return EXIT_FAILURE;
	}
	std::cout << "Successed to connected to the Server" << std::endl;

	char infoMessage[128];
	int bytes_received = recv(client_socket, infoMessage, sizeof(infoMessage), 0);
	if (bytes_received == SOCKET_ERROR) {
		std::cerr << "Error receiving message from server" << std::endl;
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

	closesocket(client_socket);
	return EXIT_SUCCESS;
}

void message_send() {
	std::string sendMessage;
	while (1) {		
		std::getline(std::cin, sendMessage);
		sendMessage = "[" + userName + "]: " + sendMessage;
		send(client_socket, sendMessage.c_str(), sendMessage.length(), 0);
	}
}

void message_recv() {
	char recv_buffer[1024];
	int recvBytes = 0;
	while (1) {
		recvBytes = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
		if (recvBytes > 0) {
			recv_buffer[recvBytes] = '\0';
			std::cout << recv_buffer << std::endl;
		}
	}
}