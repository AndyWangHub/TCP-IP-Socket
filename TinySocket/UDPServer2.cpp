#include<iostream>
#include<cstdlib>
#include<vector>
#include<array>
using namespace std;

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

constexpr auto MAXBUFF = 1024U;

void show_addr(PSOCKADDR_IN paddr)
{
	std::cout << "sin_family: " << paddr->sin_family << endl;
	std::cout << "sin_addr: " << inet_ntoa(paddr->sin_addr) << endl;
	std::cout << "sin_port: " << ntohs(paddr->sin_port) << endl;
	std::cout << "------------------------------------" << std::endl;
}

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup falied" << endl;
	}

	//SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //Windows socket1 ������ʽ
	SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED); //Windows socket2 ������ʽ
	if (sock == INVALID_SOCKET)
	{
		cout << "socket falied" << endl;
	}

	unsigned long iMode = 0; //1:��������0������
	if (ioctlsocket(sock, FIONBIO, (u_long FAR*) & iMode) != 0)
	{
		cout << "ioctlsocket falied" << endl;
	}

	SOCKADDR_IN serv_addr_in;
	memset(&serv_addr_in, 0, sizeof(SOCKADDR_IN));
	serv_addr_in.sin_family = AF_INET;
	serv_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1") // ϵͳ�Զ�����IP 
	serv_addr_in.sin_port = 0; //htons(1234); //0��ʾϵͳ����˿� 
	if (bind(sock, (PSOCKADDR)(&serv_addr_in), sizeof(SOCKADDR)) != 0)
	{
		cout << "bind falied" << endl;
	}
	show_addr(&serv_addr_in);

	int socket_size = sizeof(SOCKADDR);
	getsockname(sock, (PSOCKADDR)(&serv_addr_in), &socket_size); //��ȡ����ĵ�ַ��Ϣ
	show_addr(&serv_addr_in);
	
	SOCKADDR_IN clie_addr_in;
	int nSize = sizeof(SOCKADDR), recvLen, sendLen;
	std::array<char, MAXBUFF> buff{};
	//char buff[MAXBUFF] = {0};
/*	while (true)
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
*/
	std::vector<char> buf;
	buf.reserve(MAXBUFF);
	LPWSABUF lpBuffers;
	lpBuffers->buf = buf.data();
	lpBuffers->len = buf.capacity();

	DWORD far lpNumberOfBytesRecvd;
	DWORD far lpFlags;
	SOCKADDR lpFrom;
	int far lpFromlen;
	OVERLAPPED lpOverlapped;
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
	WSARecvFrom(sock, lpBuffers, 1, &lpNumberOfBytesRecvd, &lpFlags, &lpFrom, &lpFromlen, NULL, NULL);

	system("pause");
	return 0;
}