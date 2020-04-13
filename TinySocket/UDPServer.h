#pragma once
#include<winsock2.h>
#include <ws2tcpip.h>
#include<cstdio>
#include<cstdlib>

//Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


int mainUDPServer(int argc, char** argv)
{
	int iResult = 0;
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 0; 

	WSADATA wsaData{};

	SOCKET serv_socket{};
	SOCKADDR_IN serv_addr_in{};

	SOCKADDR_IN addr_in{};
	int addr_in_size = sizeof(addr_in);

	SOCKADDR_IN clie_addr_in{};
	int clie_addr_in_size = sizeof(clie_addr_in);

	unsigned short serv_port = 27015;

	int recv_buf_len = 1024;
	char recv_buf[1024]{ 0 };

	int send_buf_len = 1024;
	int send_buf_size = 0;
	char send_buf[1024]{ 0 };


	//-----------------------------------------------//
	//Initalize Winsocket
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("WSAStartup failed with error %d\n", iResult);
		return 1;
	}
	printf("Windows Sockets is version %d.%d\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	//-----------------------------------------------//
	//create a receiver socket to receiver datagrams
	serv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serv_socket == INVALID_SOCKET)
	{
		printf("socket failed with error %d\n", WSAGetLastError());
		return 1;
	}
	//-----------------------------------------------//
	//Bind the socket to any address and the specified port.
	serv_addr_in.sin_family = AF_INET; //IPV4 address
	serv_addr_in.sin_port = htons(serv_port); //Automatically assign ports when the parameter is zero
	serv_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1")

	iResult = bind(serv_socket, (PSOCKADDR)&serv_addr_in, sizeof(serv_addr_in));
	if (iResult != 0)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//-----------------------------------------------//
	//Get socket local address information
	iResult = getsockname(serv_socket, (PSOCKADDR)&addr_in, &addr_in_size);
	if (iResult != 0)
	{
		printf("getsockname failed with error %d\n", WSAGetLastError());
		closesocket(serv_socket);
		WSACleanup();
		return 1;
	}
	printf("Local ip: %s\n", inet_ntoa(addr_in.sin_addr));
	printf("Local port: %d\n", ntohs(addr_in.sin_port));
	//-----------------------------------------------//
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	iResult = ioctlsocket(serv_socket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
		closesocket(serv_socket);
		WSACleanup();
		return 1;
	}
	//-----------------------------------------------//
	//Call the recvfrom function to receive datagrams on the bound socket.
	printf("Receiving datagrams ...\n");
	iResult = recvfrom(serv_socket,
		recv_buf, recv_buf_len, 0, (PSOCKADDR)&clie_addr_in, &clie_addr_in_size);

	if (iResult == SOCKET_ERROR)
	{
		printf("recvfrom failed with error %d\n", WSAGetLastError());

		if (WSAGetLastError() == WSAEMSGSIZE)
			printf("The datagram is larger than the length of the buffer\n");
		else if (WSAGetLastError() == WSAEWOULDBLOCK)
			printf("The socket is marked as nonblocking and the recvfrom operation would block\n");

		closesocket(serv_socket);
		WSACleanup();
		return 1;
	}

	printf("The datagram received by recvfrom is(string) %s\n", recv_buf);
	//-----------------------------------------------//
	//Send a datagram to the receiver
	printf("Sending a datagram to the receiver ......\n");
	send_buf_size = iResult;
	memcpy(send_buf, recv_buf,send_buf_size);

	iResult = sendto(serv_socket,
		send_buf, send_buf_size, 0, (PSOCKADDR)&clie_addr_in, clie_addr_in_size);
	if (iResult == SOCKET_ERROR) 
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(serv_socket);
		WSACleanup();
		return 1;
	}
	//-----------------------------------------------//
	//Close the socket when finished receiving datagrams
	printf("Finished receiving. Closing socket.\n");
	iResult = closesocket(serv_socket);
	if (iResult == SOCKET_ERROR)
	{
		printf("closesocket failed with error %d\n", WSAGetLastError());
		return 1;
	}
	//-----------------------------------------------//
	//Clean up and exit
	printf("Exiting.\n");
	WSACleanup();
	return 0;
}
