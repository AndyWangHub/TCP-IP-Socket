#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

int main() {
    int re = 0;
    //初始化DLL
    WSADATA wsaData;
    re = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (re != 0)
    {
        std::cout << "1, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "2: " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    int iMode = 0; //1:非阻塞，0：阻塞
    re = ioctlsocket(sock, FIONBIO, (u_long FAR*) & iMode); //非阻塞设置，成功 返回0， 失败返回SOCKET_ERROR错误，可通过WSAGetLastError()获取相应错误代码
    if (re != 0)
    {
        std::cout << "3, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }
    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
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

    //接收服务器传回的数据
    char szBuffer[MAXBYTE] = { 0 };
    re = recv(sock, szBuffer, MAXBYTE, NULL);
    if (re == 0 || re == SOCKET_ERROR) //判断对方是否关闭docket
    {
        std::cout << re << " 服务端链接已经断开\n";
        system("pause");
    }
    //输出接收到的数据
    printf("Message form server: %s\n", szBuffer);

    const char* str = "I am client!";
    re = send(sock, str, strlen(str) + sizeof(char), NULL);

    //关闭套接字
    closesocket(sock);
    //终止使用 DLL
    WSACleanup();

    system("pause");
    return 0;
}