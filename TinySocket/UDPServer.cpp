#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

#define BUF_SIZE 100

int main() {
    int re = 0;
    WSADATA wsaData;
    re = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (re != 0)
    {
        return false;
    }

    //创建套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("%d ,%d\n", 2, WSAGetLastError());
        system("pause");
        return false;
    }

    int iMode = 0; //1:非阻塞，0：阻塞
    re = ioctlsocket(sock, FIONBIO, (u_long FAR*) & iMode);

    //绑定套接字
    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_port = htons(1234);  //端口
    bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));

    //接收客户端请求
    sockaddr_in clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    char buffer[BUF_SIZE] = {0};  //缓冲区
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &nSize);
        sendto(sock, buffer, strLen, 0, (SOCKADDR*)&clntAddr, nSize);
        printf("IP: %s, Port: %d, %s\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port), buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}