#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

int main() {
    int re = 0;
    //��ʼ��DLL
    WSADATA wsaData;
    re = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (re != 0)
    {
        std::cout << "1, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //�����׽���
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "2: " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    int iMode = 0; //1:��������0������
    re = ioctlsocket(sock, FIONBIO, (u_long FAR*) & iMode); //���������ã��ɹ� ����0�� ʧ�ܷ���SOCKET_ERROR���󣬿�ͨ��WSAGetLastError()��ȡ��Ӧ�������
    if (re != 0)
    {
        std::cout << "3, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }
    //���������������
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    re = connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    if (re != 0)
    {
        std::cout << "4, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //���շ��������ص�����
    char szBuffer[MAXBYTE] = { 0 };
    re = recv(sock, szBuffer, MAXBYTE, NULL);
    if (re == 0 || re == SOCKET_ERROR) //�ж϶Է��Ƿ�ر�docket
    {
        std::cout << re << " ����������Ѿ��Ͽ�\n";
        system("pause");
    }
    //������յ�������
    printf("Message form server: %s\n", szBuffer);

    const char* str = "I am client!";
    re = send(sock, str, strlen(str) + sizeof(char), NULL);

    //�ر��׽���
    closesocket(sock);
    //��ֹʹ�� DLL
    WSACleanup();

    system("pause");
    return 0;
}