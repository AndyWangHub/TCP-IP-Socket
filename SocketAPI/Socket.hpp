#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

namespace Socket
{
	constexpr auto MAX_BUFFER = 1024;
	constexpr auto iMode = 0; //1:非阻塞，0：阻塞

	typedef struct
	{
		std::string ip;
		int port;
	}Address;

	typedef struct
	{
		Address address;
		std::string data;
	}Datagram;

	class UDP
	{
	public:
		UDP();
		~UDP();

		void close(); //释放套接口
		BOOL bind(int prot); //绑定端口
		INT send(std::string ip, int prot, std::string& data); //发送数据
		INT send(const char* ip, int prot, const char* data, int len); //发送数据
		BOOL receive(Datagram& pack); //接受数据
	private:
		BOOL create(); //创建套接口
	private:
		SOCKET m_socket_server;
		BOOL m_binded;

		WSADATA m_wsaData;
	};

	UDP::UDP()
	{
		this->m_socket_server = INVALID_SOCKET;
		this->m_binded = create();
	}

	UDP::~UDP()
	{
		if (m_socket_server != INVALID_SOCKET)
			close();
	}
	void UDP::close()
	{
		closesocket(m_socket_server);
		WSACleanup();
		this->m_socket_server = INVALID_SOCKET;
	}
	BOOL UDP::bind(int prot)
	{
		if (!m_binded) return false;

		sockaddr_in servAddr;
		memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
		servAddr.sin_family = PF_INET;  //使用IPv4地址
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
		servAddr.sin_port = htons(1234);  //端口

		return  m_binded = !::bind(m_socket_server, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
	}
	INT UDP::send(std::string ip, int prot, std::string& data)
	{
		sockaddr_in clntAddr;  //客户端地址信息
		int nSize = sizeof(SOCKADDR);
		clntAddr.sin_family = PF_INET;
		clntAddr.sin_addr.s_addr = inet_addr(ip.c_str()); 
		clntAddr.sin_port = htons(prot);  //端口

		return sendto(m_socket_server, data.c_str(), data.length(), 0, (SOCKADDR*)&clntAddr, nSize);
	}
	INT UDP::send(const char* ip, int prot, const char* data, int len)
	{
		sockaddr_in clntAddr;  //客户端地址信息
		int nSize = sizeof(SOCKADDR);
		clntAddr.sin_family = PF_INET;
		clntAddr.sin_addr.s_addr = inet_addr(ip);
		clntAddr.sin_port = htons(prot);  //端口

		return sendto(m_socket_server, data, len, 0, (SOCKADDR*)&clntAddr, nSize);
	}
	BOOL UDP::receive(Datagram& pack)
	{
		sockaddr_in clntAddr;
		int nSize = sizeof(SOCKADDR);
		char buffer[MAX_BUFFER] = { 0 };  //缓冲区
		//memset(buffer, 0, MAX_BUFFER + 1);
		int re = recvfrom(m_socket_server, buffer, MAX_BUFFER, 0, (SOCKADDR*)&clntAddr, &nSize);
		if (re == -1) return false;

		pack.data.assign(buffer, re);
		pack.address.ip = inet_ntoa(clntAddr.sin_addr);
		pack.address.port = ntohs(clntAddr.sin_port);

		return true;
	}
	inline BOOL UDP::create()
	{
		if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
			return false;
			//创建套接字
			m_socket_server = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socket_server == INVALID_SOCKET)
			return false;
		else
			if (ioctlsocket(m_socket_server, FIONBIO, (u_long FAR*) & iMode) == -1)
				return false;
		return true;
	}
}

#endif // !_SOCKET_HPP_
