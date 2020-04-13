#pragma once
#include<winsock2.h>
#include <ws2tcpip.h>
#include<cstdio>
#include<cstdlib>

//Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

int mainTCPClient(int argc, char** argv)
{
	for (size_t i = 0; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}
	WSADATA wsaData;

	int iResult{};
	
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
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

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
		printf("sin_addr: %s\n", inet_ntoa(((sockaddr_in *)ptr)->sin_addr));
		printf("sin_por: %d\n", ntohs(((sockaddr_in*)ptr)->sin_port));
		
		printf("\n");
	}
	freeaddrinfo(result);

	//-----------------------------------------------//
	//Clean up and exit
	printf("Exiting.\n");
	WSACleanup();
	return 0;
}