#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {

	int server_socket;
	pid_t child_process;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket==-1) {
		std::cerr<<"socket() error"<<std::endl;
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	memset(&server_address,	0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));	

	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))==-1) {
		std::cerr << "bind() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}

	if(listen(server_socket, SOMAXCONN)==-1) {
		std::cerr << "listen() error" << std::endl;
		close(server_socket);
		return EXIT_FAILURE;
	}
	
	while(1) {
		struct sockaddr_in client_address;
		socklen_t client_address_len = sizeof(client_address);
		
		int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
		if(client_socket == -1) {
			std::cerr << "accept() error" << std::endl;
			close(server_socket);
			return EXIT_FAILURE;
		}
		
		pid_t child_pid = fork();
		if(child_pid == -1) {
			std::cerr << "fork() error" << std::endl;
			close(client_socket);
			close(server_socket);
			return EXIT_FAILURE;
		}

		// 자식 프로세스
		if(child_pid == 0) {
			close(server_socket);
			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &client_address.sin_addr, addr, sizeof(addr));
			std::cout << "[TCP Server] Client connected. " << addr << ":" 
			<< ntohs(client_address.sin_port) << std::endl;
			
			// 파일의 크기를 저장할 변수
			size_t fsize;
			size_t nsize = 0;
			// 클라이언트에서 보낸 파일이름을 저장할 변수
			char fileName[50];
			char path[50] = "/home/ubuntu/project/multi-process/";
			// 클라이언트에서 보낸 "파일명" 저장.
			int fileNameRead = recv(client_socket, fileName, 50, 0);
			if(fileNameRead == -1) {
				std::cerr << "recv() error" << std::endl;
				close(client_socket);
				return EXIT_FAILURE;
			}else {
				// path+"파일명"
				fileName[strlen(fileName)+1] = '\0';	
				strcat(path, fileName);
				std::cout<< "path = " << path << std::endl;
			}
			// 해당 path에 파일이 존재하는지 검사.(binary 읽기)
			FILE* fp = fopen(path, "rb");
			// 해당 파일 존재 여부 검사하는 변수.
			// 파일 존재 시 '0', 존재하지 않을 시 '1'
			// 초기값으로 '0'과 '1'이 아닌 값으로 셋팅.
			char isNull = 's';
			// 해당 path에 파일이 존재하지 않으면 
			if(fp == NULL) {
				isNull = '1';
				std::cout << "file no exist" << std::endl;
				if(send(client_socket, &isNull, 1,0)==-1) {
					std::cout << "send() error" << std::endl;
					close(client_socket);
					close(server_socket);
					return EXIT_FAILURE;
				}
			}
			// 해당 파일이 존재한다면
			else {
				isNull = '0';	
				std::cout << "file exist!" << std::endl;
				if(send(client_socket, &isNull, 1,0)==-1) {
					std::cout << "send() error" << std::endl;
					close(client_socket);
					close(server_socket);
					return EXIT_FAILURE;
				}
				char buffer[256];
				// file pointer의 위치를 SEEK_END 기준으로 '0'만큼 이동.
				fseek(fp, 0, SEEK_END);
				// 현재 file pointer의 위치를 읽어옴.
				fsize = ftell(fp);
				// file pointer 위치를 다시 처음으로 이동.
				fseek(fp,0,SEEK_SET);
				// fsize를 문자열로 변환.
				// 클라이언트로 fsize send.
				char s_fsize[20];
				sprintf(s_fsize,"%ld",fsize);
				send(client_socket, s_fsize, strlen(s_fsize), 0);
				std::cout << strlen(s_fsize) << std::endl;

				while(!feof(fp)) {
					// fread(binary file read)
					int fpsize = fread(buffer, 1, 256, fp);
					//nsize += fpsize;
					send(client_socket, buffer, fpsize, 0);
				}
				std::cout << "File Transfer complete." << fsize << "bytes" << std::endl;
				fclose(fp);
			}
			close(client_socket);
			std::cout<<"[TCP Server] Close connected : IP address = " << addr << std::endl;
			return EXIT_SUCCESS;
		}
		// 부모 프로세스
		else {
			std::cout << "child_pid : " << child_pid << std::endl;
		}
	}
	close(server_socket);
	return EXIT_SUCCESS;
}
