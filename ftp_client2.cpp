#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include <Windows.h>
#include <fstream>

#define SERVER_PORT 9090
#define BUF_SIZE 512

int main(int argc, char* argv[]) {

	char* server_ip = (char*)"192.168.1.77";

	// 명령행 인수가 있으면 IP주소로 사용.
	if (argc > 1) server_ip = argv[1];

	// 윈속 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return EXIT_FAILURE;
	}

	// 클라이언트 소켓 생성 WSASocket()함수도 있음.
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
	char buffer[BUF_SIZE];
	int bytesRead;

	// 서버로부터 메시지 수신
	bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead == SOCKET_ERROR) {
		std::cerr << "Error receiving message from server" << std::endl;
	}
	else {
		if (bytesRead < BUF_SIZE) {
			buffer[bytesRead] = '\0';
		}
		std::cout << buffer << std::endl;
	}
	
	// 서버에 다운로드 받을 파일 명 송신
	char* fileName = new char[1024];
	memset(fileName, 0, sizeof(fileName));
	std::cin.getline(fileName, 1024);

	int sendBytes = send(clientSocket, fileName, (int)strlen(fileName), 0);
	if (sendBytes == SOCKET_ERROR) {
		std::cerr << "send() error" << std::endl;
	}

	// 응답확인
	int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (!std::strcmp(buffer, "404")) {
		std::cout << "Not Found Exception" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}
	
	std::streampos fileSize;
	std::cout << "sizeof(fileSize) : " << sizeof(fileSize) << std::endl;

	// 16bytes로 맞추니 되네.
	bytes = recv(clientSocket, reinterpret_cast<char*>(&fileSize), 16, 0);
	std::cout << "sizeof(fileSize) : " << sizeof(fileSize) << std::endl;
	std::cout << "sizeof(bytes) : " << sizeof(bytes) << std::endl;
	std::cout << "bytes : " << bytes << std::endl << std::endl;
	
	
	//char fileLen[20];
	//memset(fileLen, 0, 20);
	//if ((bytesRead = recv(clientSocket, fileLen, 20, 0)) <= 0) {
	//	std::cerr << "Error receiving data" << std::endl;
	//	return EXIT_FAILURE;
	//}

	//fileLen[bytesRead] = '\0';
	std::cout << "file length : " << bytesRead << std::endl;

	// 파일 오픈
	std::ofstream file(fileName, std::ios_base::out | std::ios_base::binary);
	if (!file.is_open()) {
		std::cerr << "File open error" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	// 파일 쓰기
	bytesRead = 0;
	memset(&buffer, 0, BUF_SIZE);
	std::streampos bytesWritten = 0;

	while (bytesWritten < fileSize) {
	//while (1){
		memset(&buffer, 0, BUF_SIZE);
		if ((bytesRead = recv(clientSocket, buffer, BUF_SIZE, 0)) <= 0) {
			//std::cerr << "Error receiving data" << std::endl;
			break;
		}
		file.write(buffer, bytesRead);
		bytesWritten += bytesRead;
		std::cout << bytesRead << ":" << bytesWritten << "/" << fileSize << std::endl;
		//std::cout << bytesRead << ":" << bytesWritten << std::endl;
	}

	//while((bytesRead = recv(clientSocket, buffer, BUF_SIZE, 0)) > 0) {
	//	file.write(buffer, bytesRead);
	//}

	// 리소스 정리
	file.close();
	closesocket(clientSocket);
	WSACleanup();

	return EXIT_SUCCESS;
}

