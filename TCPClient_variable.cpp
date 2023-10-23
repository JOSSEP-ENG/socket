#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string.h>

#define SERVER_PORT	9090
#define BUF_SIZE		50

int main(int argc, char* argv[]) {

	char* server_ip = (char*)"192.168.1.29";

	// ����� �μ��� ������ IP�ּҷ� ���.
	if (argc > 1) server_ip = argv[1];

	// ���� �ʱ�ȭ
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return EXIT_FAILURE;
	}

	// Ŭ���̾�Ʈ ���� ���� WSASocket() �Լ��� ����.
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error creating client socket" << std::endl;
		WSACleanup();
		return EXIT_FAILURE;
	}

	// ���� ���� ����
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(server_ip);

	// ������ ����
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		std::cerr << "Error connecting to server" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	std::cout << "Connected to server" << std::endl;

	// Ŭ���̾�Ʈ���� ������ ���� ������
	char message[BUF_SIZE];
	const char* sendData[] = {
		"Do you enjoy socket programming?",
		"Hello Server",
		"This is Client",
		"is it hard? But what can I do? We have to do it",
		"You can do it"
	};

	// ������ ������ ����
	int sendBytes = 0;
	int len = 0;
	for (int i = 0; i < 5; i++) {
		len = strlen(sendData[i]);
		strncpy(message, sendData[i], len);
		message[len++] = '\n';

		sendBytes = send(clientSocket, message, len, 0);
		if (sendBytes == SOCKET_ERROR) {
			std::cerr << "send() error" << std::endl;
			break;
		}
		std::cout << "[TCP Client] " << sendBytes << "bytes send success" << std::endl;
	}
	closesocket(clientSocket);
	WSACleanup();

	return EXIT_SUCCESS;
}