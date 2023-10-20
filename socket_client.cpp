#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>

int main() {

	// Windows 소켓 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Error WSAStartup" << std::endl;
		return EXIT_FAILURE;
	}

	// 클라이언트 소켓 생성
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		std::cerr << "Error creating client socket" << std::endl;
		WSACleanup();
		return EXIT_FAILURE;
	}

	// 서버에 연결
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9090);
	server_address.sin_addr.s_addr = inet_addr("192.168.1.30");

	if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
		std::cerr << "Error connecting to server" << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return EXIT_FAILURE;
	}
	
	std::cout << "Connected to server" << std::endl;

	char buffer[1024];
	int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
	if (bytesRead == SOCKET_ERROR) {
		std::cerr << "Error receiving message from server" << std::endl;
	}
	else {
		buffer[bytesRead] ='\0';
		std::cout << "Received message from server : " << buffer << std::endl;
	}

	const char* message = "Hello from Client!";
	int bytesSent = send(client_socket, message, (int)strlen(message), 0);
	if (bytesSent == SOCKET_ERROR) {
		std::cerr << "Error sending data to server" << std::endl;
	}

	closesocket(client_socket);
	WSACleanup();

	return EXIT_SUCCESS;
}