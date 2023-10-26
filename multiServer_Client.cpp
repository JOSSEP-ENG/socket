#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string.h>
#include <Windows.h>

#define SERVER_PORT		9090
#define BUF_SIZE		50

int main(int argc, char* argv[]) {

	char* server_ip = (char*)"192.168.1.50";

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
	char message[BUF_SIZE] = "test2.txt";

	// ������ ������ ����
	int sendBytes = 0;
	sendBytes = send(clientSocket, message, (int)strlen(message), 0);
	if (sendBytes == SOCKET_ERROR) {
		std::cerr << "send() error" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "[TCP Client] " << "fileName " << sendBytes << "bytes send success" << std::endl;

	// Ŭ���̾�Ʈ���� ��û�� ������ ������ ���� ���
	// �������� fopen ���� �� isNull = '1' send
	// Ŭ���̾�Ʈ receive
	char isNull;
	recv(clientSocket, &isNull, 1, 0);
	if (isNull == '1') {
		std::cerr << "file no exist from server." << std::endl;
	}
	else {
		// �������� ������ ������ ����.
		// 1. �������� ���� ���� �ޱ�.
		// 2. buffer�� ������ ����.
		// 3. ���� ����.
		// 4. ���Ͽ� buffer ���� write
		// 5. ���� �ݱ�.
		char recvData[256];
		int receivedBytes = 0;
		int bytesRead;
		char s_fsize[20];		
		FILE* fp = NULL;
	
		fp = fopen(message, "wb");
		
		// �������� ���� File size�� ����.
		// File size�� ���ڼ��� ����..!
		// File size string�� �������� ���� Data�� �޸𸮰� �������־
		// File size string->int ��ȯ�� ��, Data�� String�� �Ϻΰ� �߸��� ���� �߻��� �� ����.
		recv(clientSocket, s_fsize, 4, 0);
		int i_fsize = atoi(s_fsize);
		std::cout << "fileSize(int): " << i_fsize << std::endl;

		while ((bytesRead = recv(clientSocket, recvData, sizeof(recvData), 0))>0) {						
			std::cout << bytesRead << std::endl;
			fwrite(recvData, sizeof(char), bytesRead, fp);
			receivedBytes += bytesRead;			
			if (i_fsize == receivedBytes) {				
				break;
			}					
		}
		std::cout << "file gets complete! " << receivedBytes << "bytes" << std::endl;
		fclose(fp);
	}	
	closesocket(clientSocket);
	WSACleanup();

	return EXIT_SUCCESS;
}