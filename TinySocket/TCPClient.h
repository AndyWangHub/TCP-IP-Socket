#pragma once
#include<winsock2.h>
#include<ws2tcpip.h>
#include<ip2string.h>
#include<cstdio>
#include<cstdlib>

//Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

//Link with Ntdll.dll和Ntdll.lib
#pragma comment(lib,"Ntdll.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int mainTCPClient(int argc, char** argv)
{

	WSADATA wsaData;
	SOCKET clieSocket = INVALID_SOCKET;

	int iResult{};

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	const char* sendbuf = "This is a test";
	char recvbuf[DEFAULT_BUFLEN]{ 0 };
	int recvbuflen = DEFAULT_BUFLEN;
	
	// Validate the parameters
	if (argc != 2) 
	{
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	//-----------------------------------------------//
	//Initalize Winsocket
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("WSAStartup failed with error %d\n", iResult);
		return 1;
	}
	printf("Windows Sockets is version %d.%d\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	//-----------------------------------------------//
	// Resolve the server address and port
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

//	SOCKADDR_IN addr_in;
//	addr_in.
	// 本地链接 IPv6 地址. . . . . . . . : fe80::a0b5:c067:eab2:b499%18
	//IPv4 地址 . . . . . . . . . . . . : 192.168.1.12
	char hostname[255] = { 0 }; //主机名   
	iResult = gethostname(hostname, sizeof(hostname));
	if (iResult == SOCKET_ERROR)
	{
		printf("gethostname failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	printf("hostname = %s\n", hostname);
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		printf("family: %d\n", ptr->ai_family);
		printf("socktype: %d\n", ptr->ai_socktype);
		printf("protocol: %d\n", ptr->ai_protocol);
		printf("addrlen: %d\n", ptr->ai_addrlen);
		if (ptr->ai_family == AF_INET)
		{
			sockaddr_in* paddr_in = (sockaddr_in*)(ptr->ai_addr);
			printf("sin_addr: %s\n", inet_ntoa(paddr_in->sin_addr));
			printf("sin_por: %d\n", ntohs(paddr_in->sin_port));
			printf("sin_family: %d\n", (paddr_in->sin_family));
		}
		else if (ptr->ai_family == AF_INET6)
		{
			const sockaddr_in6* paddr_in6 = (sockaddr_in6*)ptr->ai_addr;
			char ip6[100] = { 0 };
			RtlIpv6AddressToStringA(&(paddr_in6->sin6_addr), ip6);
			printf("sin_addr: %s\n", ip6);
			printf("sin_por: %d\n", ntohs(paddr_in6->sin6_port));
			printf("sin_family: %d\n", (paddr_in6->sin6_family));

			char bu6[10] = { 'a','%',9};
			printf("%s\n", bu6);
		}
		printf("\n");

		//-----------------------------------------------//
		 // Create a SOCKET for connecting to server
		clieSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (clieSocket == INVALID_SOCKET) 
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		//-----------------------------------------------//
		// Connect to server
		//iResult = connect(clieSocket, ptr->ai_addr, ptr->ai_addrlen);
		iResult = connect(clieSocket, ptr->ai_addr, ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(clieSocket);
			clieSocket = INVALID_SOCKET;
			continue;
		}
		break;

	}
	freeaddrinfo(result);

	if (clieSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//-----------------------------------------------//
	// Send an initial buffer
	iResult = send(clieSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(clieSocket);
		WSACleanup();
		return 1;
	}
	printf("Bytes Sent: %ld\n", iResult);
	//-----------------------------------------------//
	 // Receive until the peer closes the connection
	do {

		iResult = recv(clieSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	//-----------------------------------------------//
	//Clean up and exit
	printf("Exiting.\n");
	closesocket(clieSocket);
	WSACleanup();

	return 0;
}