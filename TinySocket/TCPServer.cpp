#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif // !_WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

int main(){

    int re = 0;
    //初始化 DLL
    WSADATA wsaData;
    re = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(re != 0)
	{
        std::cout << "1, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
		return false;
	}

    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(servSock == INVALID_SOCKET)
	{
        std::cout << "2: " << WSAGetLastError() << std::endl;
        system("pause");
		return false;
	}

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    sockAddr.sin_port = htons(1234);  //端口
	
	int iMode = 0; //1:非阻塞，0：阻塞
    re = ioctlsocket(servSock, FIONBIO, (u_long FAR*) & iMode); //非阻塞设置，成功 返回0， 失败返回SOCKET_ERROR错误，可通过WSAGetLastError()获取相应错误代码
    if (re != 0)
    {
        std::cout << "3, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    re = bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)); ////绑定成功，返回0，失败返回-1
    if (re != 0)
    {
        std::cout << "4, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }

    //进入监听状态
    re = listen(servSock, 10); //第二个参数，是最多链接个数
    if (re != 0)
    {
        std::cout << "5, re = " << re << ", " << WSAGetLastError() << std::endl;
        system("pause");
        return false;
    }
 
    //接收客户端请求
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
    while (true) //多个客户端链接测试
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize); //返回客户端的套接字，基本的信息clntAddr

        if (clntSock != INVALID_SOCKET)
        {
            //输出客户端的信息
            printf("IP: %s, Port: %d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
        }
        std::cout << "------------------------------" << std::endl;
    }
#endif // 0

    //向客户端发送数据
    const char *str = "Hello World!";
    re = send(clntSock, str, strlen(str)+sizeof(char), NULL);
    std::cout << "7, re = " << re << ", " << WSAGetLastError() << std::endl;


#if 0
    //接收服务器传回的数据
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
        if (re == 0 || re == SOCKET_ERROR) //判断对方是否关闭docket
        {
            std::cout << re << " 客户端链接已经断开\n";
            system("pause");
        }
        printf(szBuffer);
        std::cout << std::endl;
    }
#endif // 0

    //关闭套接字
    closesocket(clntSock);
    closesocket(servSock);

    //终止 DLL 的使用
    WSACleanup();
    system("pause");
    return 0;
}