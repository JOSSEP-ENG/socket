#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE	512

/*매개변수*/
// int socket : 데이터를 받아올 소켓의 파일 디스크립터
// char *p : 받아온 데이터를 저장할 메모리 주소
/*반환값*/
// 성공 시 저장한 바이트 수 만큼 반환.
// 실패 시 -1 반환.
// 데이터가 없을 경우 '0'을 반환.
int recv_ahead(int socket, char* p) {
	// nbytes : 현재까지 받아온 데이터의 바이트 수를 저장.
	// buf : 데이터를 저장하는 버퍼
	// ptr : 현재 읽고 있는 데이터 위치
	static int nbytes = 0;
	static char buf[1024];
	static char *ptr;
	
	// nbytes가 '0' 이거나 '-1' 인 경우 새로운 데이터를 받음.
	if(nbytes==0 || nbytes==-1) {
		// 소켓에서 데이터를 받아 buf에 저장.
		// 성공 시 받아온 데이터 바이트 수 반환.
		nbytes = recv(socket,buf,sizeof(buf),0);
		// 실패 시 -1
		if(nbytes == -1) {
			return -1;
		}
		// receive할 데이터가 없을 시
		else if(nbytes==0) {
			return 0;
		}
		// ptr을 buf의 시작으로 초기화
		ptr = buf;
	}
	// ptr이 가리키는 위치의 데이터를 p에 저장.
	// ptr 다음칸으로 이동.
	// nbytes '1' 감소.
	--nbytes;
	*p = *ptr++;

	return 1;
}

// 해당 함수는 한 줄의 데이터를 받아옴.
// '\n'을 만나거나 최대 길이(maxlen)에 도달할 때까지 데이터를 받아옴.
/*매개변수*/
// int socket : 데이터를 받아올 소켓의 파일 디스크립터.
// char *buf : 받아온 데이터를 저장할 버퍼.
// int maxlen : 받아올 데이터의 최대 길이.
/*반환값*/
// 성공 시 받아온 데이터의 길이 반환
// 데이터가 없을 경우 '0' 반환
// 실패 시 '-1' 반환
int recvline(int socket, char *buf, int maxlen) {
	int n, nbytes;
	char c, *ptr=buf;

	for(n=1; n<maxlen; n++) {
		nbytes = recv_ahead(socket, &c);
		if(nbytes == 1) {
			*ptr++ = c;
			if(c == '\n') {
				break;
			}
		}
		else if(nbytes == 0) {
			*ptr = 0;
			return n-1;
		}
		else {
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

int main(int argc, char *argv[]) {
	int server_socket;
	// 소켓 생성 (IPv4, 순서 데이터 스트림, 옵션 0)
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	// 소캣 생성 실패 시
	if(server_socket == -1) {
		std::cerr << "socket() error";
		return EXIT_FAILURE;
	}
	
	// 구조체 변수 server_address 선언.
	struct sockaddr_in server_address;
	// 구조체변수 server_address '0'으로 초기화.
	memset(&server_address, 0, sizeof(server_address));
	// 구조체변수 server_address 멤버변수 값 대입.
	// IPv4 type, any client, 포트번호
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));
	
	// 소켓 바인딩 실패 시
	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))==-1) {
		std::cerr << "bind() error";
		close(server_socket);
		return EXIT_FAILURE;
	}
	
	// SOMAXCONN : 한번에 요청 가능한 최대 접속 승인 수를 의미
	if(listen(server_socket, SOMAXCONN)==-1) {
		std::cerr << "listen() error";
		close(server_socket);
		return EXIT_FAILURE;
	}
	
	// 클라이언트 소켓 변수 생성.
	int client_socket;
	// 클라이언트 주소 구조체 변수 생성.
	struct sockaddr_in client_address;
	// 클라아인트 주소 구조체 변수의 크기
	socklen_t client_address_len = sizeof(client_address);

	while(1) {
		// 클라이언트측에서 서버에 연결을 시도.
		// 서버측에서 accept를 통해서 클라이언트측과 네트워크 연결.
		client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);

		// 서버측에서 accept 실패 시
		if(client_socket == -1) {
			std::cerr << "accept() error";
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		// binary->string으로 변환 후 IP주소 문자열을 담을 배열.
		char addr[INET_ADDRSTRLEN];
		// const char* inet_ntop(int af, const void* src, char* dst, socklen_t size);
		// IP address를 binary -> string 형태로 변환시켜주는 함수.
		inet_ntop(AF_INET, &client_address.sin_addr, addr, sizeof(addr));
		// 클라이언트 IP 주소, 포트번호 출력
		std::cout << "[TCP server] Client connected : IP address = " << addr << ", port number = "
			<< ntohs(client_address.sin_port) << std::endl;
		
		while(1) {
			// 클라이언트측에서 보낸 문자열 데이터를 담을 buffer 배열.
			char  buffer[BUFSIZE+1];
			int bytesRead = recvline(client_socket, buffer, BUFSIZE+1);
			// receive 실패 시
			if(bytesRead==-1){
				std::cerr << "recv() error";
				close(client_socket);
				close(server_socket);
				return EXIT_FAILURE;
			// receive할 데이터가 없을 시 반복문 종료
			} else if(bytesRead==0) {
				break;
			}
			buffer[bytesRead] = '\0';
			std::cout << "[TCP/" << addr << "]" << buffer << std::endl;
		}

		close(client_socket);
		std::cout << "[TCP server] Close connected : IP address = " << addr << std::endl;
	}

	close(server_socket);
	return EXIT_SUCCESS;
}


