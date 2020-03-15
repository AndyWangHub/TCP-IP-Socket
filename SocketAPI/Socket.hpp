#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

namespace Socket
{
	constexpr auto MAX_BUFFER = 2046;
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
		SOCKET m_socket;
		BOOL m_binded;

		WSADATA m_wsaData;
	};

	UDP::UDP()
	{
		this->m_socket = INVALID_SOCKET;
		this->m_binded = create();
	}

	UDP::~UDP()
	{
		if (m_socket != INVALID_SOCKET)
			close();
	}
	void UDP::close()
	{
		closesocket(m_socket);
		WSACleanup();
		m_socket = INVALID_SOCKET;
	}
	BOOL UDP::bind(int prot)
	{
		if (!m_binded) return false;

		sockaddr_in servAddr;
		memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
		servAddr.sin_family = PF_INET;  //使用IPv4地址
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
		servAddr.sin_port = htons(1234);  //端口

		return  m_binded = !::bind(m_socket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
	}
	INT UDP::send(std::string ip, int prot, std::string& data)
	{
		sockaddr_in clntAddr;  //客户端地址信息
		int nSize = sizeof(SOCKADDR);
		clntAddr.sin_family = PF_INET;
		clntAddr.sin_addr.s_addr = inet_addr(ip.c_str()); 
		clntAddr.sin_port = htons(prot);  //端口

		return sendto(m_socket, data.c_str(), data.length(), 0, (SOCKADDR*)&clntAddr, nSize);
	}
	INT UDP::send(const char* ip, int prot, const char* data, int len)
	{
		sockaddr_in clntAddr;  //客户端地址信息
		int nSize = sizeof(SOCKADDR);
		clntAddr.sin_family = PF_INET;
		clntAddr.sin_addr.s_addr = inet_addr(ip);
		clntAddr.sin_port = htons(prot);  //端口

		return sendto(m_socket, data, len, 0, (SOCKADDR*)&clntAddr, nSize);
	}
	BOOL UDP::receive(Datagram& pack)
	{
		sockaddr_in clntAddr;
		int nSize = sizeof(SOCKADDR);
		char buffer[MAX_BUFFER] = { 0 };  //缓冲区
		//memset(buffer, 0, MAX_BUFFER + 1);
		int re = recvfrom(m_socket, buffer, MAX_BUFFER, 0, (SOCKADDR*)&clntAddr, &nSize);
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
			m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socket == INVALID_SOCKET)
			return false;
		else
			if (ioctlsocket(m_socket, FIONBIO, (u_long FAR*) & iMode) == -1)
				return false;
		return true;
	}


	class TCPClient
	{
	public:
		TCPClient();
		~TCPClient();

		void close(); //释放套接口
		BOOL connect(std::string ip, int prot); //连接服务器
		INT send(std::string& data); //发送数据
		INT send(const char* buf, int len); //发送数据
		INT receive(std::string& data); //接受数据
	private:
		BOOL create(); //创建套接口
	private:
		SOCKET m_socket;
		BOOL m_binded;

		WSADATA m_wsaData;
	};

	TCPClient::TCPClient()
	{
		m_socket = INVALID_SOCKET;
		m_binded = create();
	}

	TCPClient::~TCPClient()
	{
		if (m_socket != INVALID_SOCKET) close();
	}
	inline void TCPClient::close()
	{
		//关闭套接字
		closesocket(m_socket);
		//终止使用 DLL
		WSACleanup();
		m_socket = INVALID_SOCKET;
	}
	inline BOOL TCPClient::connect(std::string ip, int prot)
	{
		//向服务器发起请求
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
		sockAddr.sin_family = PF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
		sockAddr.sin_port = htons(prot);
		if(::connect(m_socket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) !=0)
			return false;
		return true;
	}
	inline INT TCPClient::send(std::string& data)
	{
		return ::send(m_socket, data.c_str(), data.length(), NULL);
	}
	inline INT TCPClient::send(const char* buf, int len)
	{
		return ::send(m_socket, buf, len, NULL);
	}
	inline INT TCPClient::receive(std::string& data)
	{
		int re = 0;
		char buffer[MAX_BUFFER] = { 0 };
		re = recv(m_socket, buffer, MAX_BUFFER, NULL);
		data.assign(buffer, re);
		return re;
	}
	inline BOOL TCPClient::create()
	{
		bool re = true;

		if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) re = false;

		m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //创建套接字
		if (m_socket == INVALID_SOCKET) re = false;

		if (ioctlsocket(m_socket, FIONBIO, (u_long FAR*) & iMode) != 0) re = false;

		return re;
	}
}

#endif // !_SOCKET_HPP_
