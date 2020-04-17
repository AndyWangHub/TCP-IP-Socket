#pragma once
#include<winsock2.h>
#include <ws2tcpip.h>
#include<ip2string.h>
#include<cstdio>

//Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
//Link with Ntdll.dllºÍNtdll.lib
#pragma comment(lib,"Ntdll.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int mainUDPServer2(int argc,char * argv[])
{
	int iResult = 0;
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 0;

	WSADATA wsaData{};

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

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
	//Get hostname
	char hostname[255] = { 0 };  
	iResult = gethostname(hostname, sizeof(hostname));
	if (iResult == SOCKET_ERROR)
	{
		printf("gethostname failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	printf("hostname = %s\n", hostname);

	// Resolve the server address and port
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	iResult = getaddrinfo(hostname, NULL, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	//-----------------------------------------------
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		printf("family: %d\n", ptr->ai_family);
		//printf("socktype: %d\n", ptr->ai_socktype);
		//printf("protocol: %d\n", ptr->ai_protocol);
		//printf("addrlen: %d\n", ptr->ai_addrlen);
		//printf("canonname: %s\n", ptr->ai_canonname);
		if (ptr->ai_family == AF_INET)
		{
			sockaddr_in* paddr_in = (sockaddr_in*)(ptr->ai_addr);
			//printf("sin_addr: %s\n", inet_ntoa(paddr_in->sin_addr));
			char ip4[255] = { 0 };
			RtlIpv4AddressToStringA(&(paddr_in->sin_addr), ip4);
			printf("ipv4_addr: %s\n", ip4);
			printf("ipv4_por: %d\n", ntohs(paddr_in->sin_port));
			//printf("sin_family: %d\n", (paddr_in->sin_family));
		}
		else if (ptr->ai_family == AF_INET6)
		{
			const sockaddr_in6* paddr_in6 = (sockaddr_in6*)ptr->ai_addr;
			char ip6[255] = { 0 };
			RtlIpv6AddressToStringA(&(paddr_in6->sin6_addr), ip6);
			printf("ipv6_addr: %s\n", ip6);
			printf("ipv6__por: %d\n", ntohs(paddr_in6->sin6_port));
			//printf("sin_family: %d\n", (paddr_in6->sin6_family));
		}
		printf("--------------------------------------------\n");
	}
	freeaddrinfo(result);

	//-----------------------------------------------//
	//Clean up and exit
	printf("Exiting.\n");
	//closesocket(serv_socket);
	WSACleanup();

	return 0;
}
