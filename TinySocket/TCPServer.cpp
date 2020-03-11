#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll

int main() {

    int re = 0;
    //��ʼ�� DLL
    WSADATA wsaData;
    re = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (re != 0)
    {
        std::cout << "1, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //�����׽���
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock == INVALID_SOCKET)
    {
        std::cout << "2: " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    int iMode = 0; //1:��������0������
    re = ioctlsocket(servSock, FIONBIO, (u_long FAR*) & iMode); //���������ã��ɹ� ����0�� ʧ�ܷ���SOCKET_ERROR���󣬿�ͨ��WSAGetLastError()��ȡ��Ӧ�������
    if (re != 0)
    {
        std::cout << "3, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //���׽���
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
    sockAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
    sockAddr.sin_port = htons(1234);  //�˿�
    re = bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)); ////�󶨳ɹ�������0��ʧ�ܷ���-1
    if (re != 0)
    {
        std::cout << "4, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //�������״̬
    re = listen(servSock, 10); //�ڶ�����������������Ӹ���
    if (re != 0)
    {
        std::cout << "5, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //���տͻ�������
    sockaddr_in clntAddr;
    int nSize = sizeof(SOCKADDR);

#if 1
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    if (clntSock == INVALID_SOCKET)
    {
        std::cout << "6, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }
#else
    while (true) //����ͻ������Ӳ���
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize); //���ؿͻ��˵��׽��֣���������ϢclntAddr

        if (clntSock != INVALID_SOCKET)
        {
            //����ͻ��˵���Ϣ
            printf("IP: %s, Port: %d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
        }
        std::cout << "------------------------------" << std::endl;
    }
#endif // 0

    //��ͻ��˷�������
    const char* str = "Hello World!";
    re = send(clntSock, str, strlen(str) + sizeof(char), NULL);
    std::cout << "7, re = " << re << ", " << WSAGetLastError() << std::endl;


#if 0
    //���շ��������ص�����
    char szBuffer[1024] = { 0 };
    re = recv(clntSock, szBuffer, MAXBYTE, NULL);
    std::cout << "8, re = " << re << ", " << WSAGetLastError() << std::endl;
    std::cout << szBuffer << std::endl;
#else
    char szBuffer[1024] = { 0 };
    while (true)
    {
        memset(szBuffer, 0, 1024);
        re = recv(clntSock, szBuffer, MAXBYTE, NULL);
        if (re == 0 || re == SOCKET_ERROR) //�ж϶Է��Ƿ�ر�docket
        {
            std::cout << re << " �ͻ��������Ѿ��Ͽ�\n";
            system("pause");
        }
        printf(szBuffer);
        std::cout << std::endl;
    }
#endif // 0

    //�ر��׽���
    closesocket(clntSock);
    closesocket(servSock);

    //��ֹ DLL ��ʹ��
    WSACleanup();
    system("pause");
    return 0;
}