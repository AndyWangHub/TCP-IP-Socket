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

int MulticastClient()
{

	int iRet = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	int iFlag = 1;	// 0-同一台主机 1-夸主机
	iRet = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&iFlag, sizeof(iFlag));
	if (iRet != 0) {
		printf("setsockopt fail:%d", WSAGetLastError());
		return -1;
	}

	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr("234.2.2.2");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(65000);

	char strSend[1024] = { 0 };
	static int iIdx = 0;
	while (1)
	{
		sprintf_s(strSend, "udp send group data:%d", iIdx++);
		iRet = sendto(sock, strSend, strlen(strSend) + 1, 0, (sockaddr*)&addr, sizeof(sockaddr));
		if (iRet <= 0) {
			printf("send fail:%d", WSAGetLastError());
		}
		else {
			printf("send data:%s\n", strSend);
		}
		Sleep(500);
	}

	closesocket(sock);
	WSACleanup();

	return 0;

}
