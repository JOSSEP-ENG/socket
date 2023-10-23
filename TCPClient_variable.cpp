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

	// 명령행 인수가 있으면 IP주소로 사용.
	if (argc > 1) server_ip = argv[1];

	// 윈속 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return EXIT_FAILURE;
	}

	// 클라이언트 소켓 생성 WSASocket() 함수도 있음.
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error creating client socket" << std::endl;
		WSACleanup();
		return EXIT_FAILURE;
	}

	// 서버 정보 설정
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(server_ip);

	// 서버에 연결
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		std::cerr << "Error connecting to server" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	std::cout << "Connected to server" << std::endl;

	// 클라이언트에서 서버로 보낼 데이터
	char message[BUF_SIZE];
	const char* sendData[] = {
		"Do you enjoy socket programming?",
		"Hello Server",
		"This is Client",
		"is it hard? But what can I do? We have to do it",
		"You can do it"
	};

	// 서버에 데이터 전송
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