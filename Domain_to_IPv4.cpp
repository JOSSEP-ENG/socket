#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iostream>

int main() {	

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed\n");
		return EXIT_FAILURE;
	}

	// 함수 원형 : struct hostent* gethostbyname(const char* name)
	// hostname을 사용하여 해당 도메인에 대한 정보를
	// hostent 구조체 타입인 hostinfo에 대입.
	const char* hostname = "www.yes24.com";	
	struct hostent* hostinfo = gethostbyname(hostname);

	// hostinfo 구조체변수에 해당 도메인에 대한 정보가 정상적으로 대입이 됐다면,
	// hostinfo 구조체 포인터 변수에 해당 도메인 정보가 들어가 있는 메모리 주소값이 대입.
	// 도메인 정보가 들어가 있지 않으면 NULL 값 대입.
	if (hostinfo != NULL) {
		if (hostinfo->h_addrtype == AF_INET) {
			// h_addr_list에는 도메인 네임에 해당되는 IP주소들이 여러개의 배열로 저장.
			char** ip = hostinfo->h_addr_list;
			while (*ip != NULL) {
				struct in_addr addr;
				memcpy(&addr, *ip, sizeof(struct in_addr));
				// char* inet_ntoa(struct in_addr addr)
				std::cout << inet_ntoa(addr) << std::endl;
				ip++;
			}
		}
	}

	const char* ipAddress = "61.111.13.51";
	struct in_addr hostAddress;
	
	if (inet_pton(AF_INET, ipAddress, &hostAddress) == 1) {
		struct hostent* host = gethostbyaddr((const char*)&hostAddress, sizeof(struct in_addr), AF_INET);

		if (host != NULL) {
			printf("Official host name : %s\n", host->h_name);
			printf("Host aliases : \n");
			for (char** alias = host->h_aliases; *alias != NULL; alias++) {
				printf("\t%s\n", *alias);
			}
		}
		else {
			fprintf(stderr, "gethostbyaddr failed : %d\n", WSAGetLastError());
		}
	}
	else {
		fprintf(stderr, "inet_pton failed\n");
	}

	WSACleanup();

	/*	
	const char* ipAddress = "192.168.1.225";
	struct in_addr ipv4Addr;

	// 문자열 IP를 바이너리 형식으로 변환.
	// 0, 음수 : 오류, errno 변수
	// 1 : IPv4 변환 성공
	// 2 : IPv6 변환 성공
	if (inet_pton(AF_INET, ipAddress, (struct in_addr*)&ipv4Addr) == 1) {
		printf("%#x\n", ipv4Addr.s_addr);
	}

	// IP 바이너리 형식을 문자열로 변환.
	char ipv4str[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &ipv4Addr, ipv4str, sizeof(ipv4str)) != NULL) {
		std::cout << ipv4str << std::endl;
	}
	*/
	
	return EXIT_SUCCESS;
}