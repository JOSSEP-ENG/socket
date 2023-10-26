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
	char message[BUF_SIZE] = "test2.txt";

	// 서버에 데이터 전송
	int sendBytes = 0;
	sendBytes = send(clientSocket, message, (int)strlen(message), 0);
	if (sendBytes == SOCKET_ERROR) {
		std::cerr << "send() error" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "[TCP Client] " << "fileName " << sendBytes << "bytes send success" << std::endl;

	// 클라이언트에서 요청한 파일이 서버에 없을 경우
	// 서버에서 fopen 실패 시 isNull = '1' send
	// 클라이언트 receive
	char isNull;
	recv(clientSocket, &isNull, 1, 0);
	if (isNull == '1') {
		std::cerr << "file no exist from server." << std::endl;
	}
	else {
		// 서버에서 가져온 데이터 저장.
		// 1. 서버에서 보낸 파일 받기.
		// 2. buffer에 데이터 저장.
		// 3. 파일 생성.
		// 4. 파일에 buffer 내용 write
		// 5. 파일 닫기.
		char recvData[256];
		int receivedBytes = 0;
		int bytesRead;
		char s_fsize[20];		
		FILE* fp = NULL;
	
		fp = fopen(message, "wb");
		
		// 서버에서 보낸 File size를 받음.
		// File size의 글자수에 주의..!
		// File size string과 서버에서 보낸 Data의 메모리가 근접해있어서
		// File size string->int 변환할 때, Data의 String의 일부가 잘리는 현상 발생할 수 있음.
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