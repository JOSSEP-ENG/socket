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

	// �Լ� ���� : struct hostent* gethostbyname(const char* name)
	// hostname�� ����Ͽ� �ش� �����ο� ���� ������
	// hostent ����ü Ÿ���� hostinfo�� ����.
	const char* hostname = "www.yes24.com";	
	struct hostent* hostinfo = gethostbyname(hostname);

	// hostinfo ����ü������ �ش� �����ο� ���� ������ ���������� ������ �ƴٸ�,
	// hostinfo ����ü ������ ������ �ش� ������ ������ �� �ִ� �޸� �ּҰ��� ����.
	// ������ ������ �� ���� ������ NULL �� ����.
	if (hostinfo != NULL) {
		if (hostinfo->h_addrtype == AF_INET) {
			// h_addr_list���� ������ ���ӿ� �ش�Ǵ� IP�ּҵ��� �������� �迭�� ����.
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

	// ���ڿ� IP�� ���̳ʸ� �������� ��ȯ.
	// 0, ���� : ����, errno ����
	// 1 : IPv4 ��ȯ ����
	// 2 : IPv6 ��ȯ ����
	if (inet_pton(AF_INET, ipAddress, (struct in_addr*)&ipv4Addr) == 1) {
		printf("%#x\n", ipv4Addr.s_addr);
	}

	// IP ���̳ʸ� ������ ���ڿ��� ��ȯ.
	char ipv4str[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &ipv4Addr, ipv4str, sizeof(ipv4str)) != NULL) {
		std::cout << ipv4str << std::endl;
	}
	*/
	
	return EXIT_SUCCESS;
}