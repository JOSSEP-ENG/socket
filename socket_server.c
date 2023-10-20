#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

void error_handling(char *);

int main(int argc, char* argv[]) {
	
	// 서버, 클라이언트 소켓 디스크립터 변수 생성.
	int server_socket;
	int client_socket;
	// 서버, 클라이언트 IP, PORT 저장 구조체 변수 생성.
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	socklen_t client_address_size;
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		return 1;
	}
	
	// create server socket
	// (IPv4, 순서 데이터 스트림, 디폴트 설정) 소켓 생성.
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// 서버 소켓 생성 실패 시 에러 처리.
	if(server_socket == -1) {
		error_handling("socket() error");
	}

	// binding server socket
	// IPv4 설정, IPv4 주소 입력(빅엔디안 방식), 포트번호 입력(문자열->정수로 변경하고 빅엔디안 방식으로 저장)
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));
	
	//  
	if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))==-1) {
		close(server_socket);
		error_handling("bind() error");
	}
	
	// listen
	if(listen(server_socket,5)==-1) {
		close(server_socket);
		error_handling("listen() error");
	}
	
	// accept
	client_address_size = sizeof(client_address);
	client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);

	if(client_socket == -1) {
		close(server_socket);
		error_handling("accept() error");
	}

	char message[] = "Hello World";
	send(client_socket,message, sizeof(message),0);

	char buffer[1024];
	int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
	if(bytesRead == -1) {
		close(client_socket);
		close(server_socket);
		error_handling("recv() error");
	} 
	else {
		buffer[bytesRead]='\0';
		printf("Received response from client : %s\n",buffer);
	}

	close(client_socket);
	close(server_socket);
	return 0;
}

void error_handling(char* message) {
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

