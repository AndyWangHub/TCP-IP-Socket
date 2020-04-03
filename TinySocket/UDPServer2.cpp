#include<iostream>
#include<cstdlib>
#include<array>
using namespace std;

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

constexpr auto MAXBUFF = 1024;

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup falied" << endl;
	}

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		cout << "socket falied" << endl;
	}

	unsigned long iMode = 0; //1:·Ç×èÈû£¬0£º×èÈû
	if (ioctlsocket(sock, FIONBIO, (u_long FAR*) & iMode) != 0)
	{
		cout << "ioctlsocket falied" << endl;
	}

	SOCKADDR_IN serv_addr_in;
	memset(&serv_addr_in, 0, sizeof(SOCKADDR_IN));
	serv_addr_in.sin_family = AF_INET;
	serv_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1")
	serv_addr_in.sin_port = htons(1234);
	if (bind(sock, (PSOCKADDR)(&serv_addr_in), sizeof(SOCKADDR)) != 0)
	{
		cout << "bind falied" << endl;
	}
	
	SOCKADDR_IN clie_addr_in;
	int nSize = sizeof(SOCKADDR), recvLen, sendLen;
	std::array<char, MAXBUFF> buff{};
	//char buff[MAXBUFF] = {0};
	while (true)
	{
		recvLen = recvfrom(sock,buff.data(),MAXBUFF,0, (PSOCKADDR)(&clie_addr_in),&nSize);
		std::cout << "nSize: " << nSize << endl;
		std::cout << "sin_family: " << clie_addr_in.sin_family << endl;
		std::cout << "sin_addr: " << inet_ntoa(clie_addr_in.sin_addr) << endl;
		std::cout << "sin_port: " << ntohs(clie_addr_in.sin_port) << endl;
		std::cout << "recvLen: " << recvLen << endl;
		std::cout << "buff.size: " << buff.size() << endl;

		printf("ASCII: %s\n", buff);
		printf("HEX: ");
		for (int i = 0; i < recvLen; i++)
		{
			short sh = buff[i];
			sh = sh & 0XFF;
			printf("%0X ", sh);
		}
		printf("\n");

		sendLen = sendto(sock,buff.data(),recvLen,0, (PSOCKADDR)(&clie_addr_in), nSize);
		//std::cout << "sendLen: " << sendLen << endl;

	}


	system("pause");
	return 0;
}