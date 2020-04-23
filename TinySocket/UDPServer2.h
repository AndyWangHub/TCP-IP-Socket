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
	int err = 0;
	int retval = 0;
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 0;


	WSADATA wsaData{};
	WSABUF DataBuf{};
	WSAOVERLAPPED Overlapped;

	SOCKET servSocket = INVALID_SOCKET;
	struct sockaddr_in RecvAddr;
	struct sockaddr_in SenderAddr;

	int SenderAddrSize = sizeof(SenderAddr);
	
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	char RecvBuf[1024] = { 0 };
	int BufLen = 1024;
	DWORD BytesRecv = 0;
	DWORD Flags = 0;

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

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
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
	// Make sure the Overlapped struct is zeroed out
	SecureZeroMemory((PVOID)&Overlapped, sizeof(WSAOVERLAPPED));
	// Create an event handle and setup the overlapped structure.
	Overlapped.hEvent = WSACreateEvent();
	if (Overlapped.hEvent == NULL) {
		printf("WSACreateEvent failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//-----------------------------------------------//
	//Create a receiver socket to receiver datagrams
	servSocket = WSASocket(AF_UNSPEC,
		SOCK_DGRAM,
		IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (servSocket == INVALID_SOCKET) {
		/* Could not open a socket */
		printf("WSASocket failed with error: %ld\n", WSAGetLastError());
		WSACloseEvent(Overlapped.hEvent);
		WSACleanup();
		return 1;
	}
	//-----------------------------------------------
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Bind the socket to any address and the specified port.
	iResult = bind(servSocket, result->ai_addr, result->ai_addrlen);
	if (iResult != 0) {
		/* Bind to the socket failed */
		printf("bind failed with error: %ld\n", WSAGetLastError());
		WSACloseEvent(Overlapped.hEvent);
		closesocket(servSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	//-----------------------------------------------
 // Call the recvfrom function to receive datagrams
 // on the bound socket.
	DataBuf.len = BufLen;
	DataBuf.buf = RecvBuf;
	printf("Listening for incoming datagrams on port=%s\n", DEFAULT_PORT);
	iResult = WSARecvFrom(servSocket,
		&DataBuf,
		1,
		&BytesRecv,
		&Flags,
		(SOCKADDR*)&SenderAddr,
		&SenderAddrSize, &Overlapped, NULL);
	printf("-------------- %d\n", iResult);
	if (iResult != 0) 
	{
		err = WSAGetLastError();
		if (err != WSA_IO_PENDING) {
			printf("WSARecvFrom failed with error: %ld\n", err);
			WSACloseEvent(Overlapped.hEvent);
			closesocket(servSocket);
			WSACleanup();
			return 1;
		}
		else {
			iResult = WSAWaitForMultipleEvents(1, &Overlapped.hEvent, TRUE, INFINITE, TRUE);
			if (iResult == WSA_WAIT_FAILED) {
				printf("WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
				retval = 1;
			}

			iResult = WSAGetOverlappedResult(servSocket, &Overlapped, &BytesRecv,
				FALSE, &Flags);
			if (iResult == FALSE) {
				printf("WSArecvFrom failed with error: %d\n", WSAGetLastError());
				retval = 1;
			}
			else
			{
				printf("Number of received bytes = %d\n", BytesRecv);
				printf("data[BytesRecv: %d, len: %d]: %s\n", BytesRecv, DataBuf.len, DataBuf.buf);
			}

			printf("Finished receiving. Closing socket.\n");
		}

	}

	//-----------------------------------------------//
	// When the application is finished receiving, close the socket.
	WSACloseEvent(Overlapped.hEvent);
	//closesocket(serv_socket);
	closesocket(servSocket);
	//Clean up and exit
	WSACleanup();
	printf("Exiting.\n");

	return 0;
}
