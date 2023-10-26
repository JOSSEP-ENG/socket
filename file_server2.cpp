#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <filesystem>

/**
* 파일 전송 클라이언트 소켓 처리
*/
void clientHandle(int client_socket){

	std::cout << "========== connected client socket ==========" << std::endl;
	// 클라이언트에게 파일명 요청
	char message[] = "File Name : ";
	send(client_socket, message, sizeof(message), 0);

	// 클라이언트가 전송한 파일 명 수신
	char buffer[1024];
	int bytesRead = recv(client_socket, buffer, 1024, 0);
	if(bytesRead==-1){
		std::cerr << "recv() error";
		close(client_socket);
		return;
	} else if(bytesRead==0){
		std::cerr << "no filename";
		close(client_socket);
		return;
	}
	
	// 클라이언트가 요청한 파일 오픈
	buffer[bytesRead] = '\0';
	std::ifstream readFile(buffer, std::ios_base::in | std::ios_base::binary);

	// 파일 오픈 실패 시 오류 메시지 전송 후 소켓 종료
	if(!readFile.is_open()){
		std::cerr << "File Not Found" << std::endl;
		char result[] = "404";
		send(client_socket, result, std::strlen(message), 0);
		close(client_socket);
		return;
	}

	// 파일 오픈 성공 시 성공 메시지 전송 후 파일 전송
	char result[] = "200";
	send(client_socket, message, std::strlen(message), 0);

	// 파일의 마지막으로 파일포인터 이동 후, 위치를 반환하는 방법으로 파일 사이즈 취득.
	readFile.seekg(0, std::ios::end);
	std::streampos fileSize = readFile.tellg();
	readFile.seekg(0, std::ios::beg);                    // 다시 파일 맨 처음으로 포인터 이동
	send(client_socket, &fileSize, sizeof(fileSize), 0); // 파일 사이즈 클라이언트에게 전송

	// 파일 내용 클라이언트에게 전송
	while(!readFile.eof()){
		memset(&buffer, 0, sizeof(buffer)); // 필요한지 확인 필요.
		readFile.read(buffer, sizeof(buffer));
		send(client_socket, buffer, readFile.gcount(), 0);
	}

	// 리소스 정리
	readFile.close();
	if(recv(client_socket, buffer, 1024,0)<=0){
			std::cout << "========== closed client socket ==========" << std::endl;
			close(client_socket);
	}
	//close(client_socket);
	//	std::cout << "========== closed client socket ==========" << std::endl;
}


int main(int argc, char * argv[]) {

	// 서버 소켓 생성
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket==-1){
		std::cout << "socket() error" << std::endl;
		return EXIT_FAILURE;
	}

	// 서버에 주소,포트번호등 바인딩
	struct sockaddr_in server_address;
	memset(&server_address ,0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))==-1){
		std::cerr << "bind() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}

	// 서버를 listen상태로 변경(연결 대기)
	if(listen(server_socket, SOMAXCONN)==-1){
		std::cerr << "listen() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}

	while(1){

		// 클라이언트의 요청을 queue에서 가져와서 연결
		struct sockaddr_in client_address;
		socklen_t client_address_len = sizeof(client_address);

		int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
		if(client_socket == -1){
			std::cerr << "accept() error " << std::endl;
			close(server_socket);
			return EXIT_FAILURE;
		}

		// 연결이 성공한 클라이언트 소캣을 복사하여 자식프로세스 생성
		pid_t child_pid = fork();
		if (child_pid == -1){
			std::cerr << "fork() error" << std::endl;
			close(client_socket);
			close(server_socket);
			return EXIT_FAILURE;
		}

		// 자식 프로세스 처리
		if(child_pid==0) {
			// 서버소켓은 여기에서는 필요없으므로 닫아주고
			close(server_socket);
			// 데이터 송수신 처리 호출
			clientHandle(client_socket);
			return EXIT_SUCCESS;
		// 부모 프로세스 처리
		} else {
			// 클라이언트 소캣은 자식 프로세스에 복사되었으므로 여기서는 닫아줌
			close(client_socket);
			//std::cout << "child_pid : " << child_pid << std::endl;
		}
	}

	// 서버 소켓은 계속 실행되다가 Ctrl+C등 입력 시 종료
	// 사실 Ctrl+C로 종료했을 때, 여기 오지 않음.
	// 리소스 정리를 확실하게 하려면 시그널(SIGINT) 핸들링 필요
	close(server_socket);
	return EXIT_SUCCESS;
}
