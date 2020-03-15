#include <iostream>
#include "Socket.hpp"
#include <thread>

#if 0
void threadFun(Socket::UDP* udp);

int main()
{
	std::string ip = "127.0.0.1";
	int prot = 2046;
	std::string str = "Server";
	Socket::Datagram pack;

	Socket::UDP udp;
	udp.bind(1234);

	std::thread t(threadFun, &udp);

	while (true)
	{
		std::cin >> str;
		//udp.send(ip, prot, str);
		udp.send("127.0.0.1", 2046, str.c_str(), str.size());
	}
	
	t.join();
	udp.close();

	system("pause");
	return 0;
}

void threadFun(Socket::UDP* udp)
{
	Socket::Datagram pack;
	while (true)
	{
		if (udp->receive(pack))
		{
			std::cout << "IP: " << pack.address.ip << "  Prot: " << pack.address.port << std::endl;
			std::cout << "Date[" <<pack.data.length() << "]: " << pack.data << std::endl;
		}
		std::cout << "-----------------------" << std::endl;
	}
}



#endif // 0

#if 1

void threadFun(Socket::TCPClient* tcpClient);

int main()
{
	Socket::TCPClient tcpClient;
	if (!tcpClient.connect("127.0.0.1", 10246))
	{
		std::cout << "Á¬½ÓÊ§°Ü" << std::endl;
		system("pause");
	}

	std::thread t(threadFun, &tcpClient);

	std::string str = "123456789";
	while (true)
	{
		std::cin >> str;
		tcpClient.send(str);
	}

	system("pause");
	return 0;
}

void threadFun(Socket::TCPClient* tcpClient)
{
	std::string data;
	while (true)
	{
		tcpClient->receive(data);
		std::cout << "data[" << data.length() << "]: " << data << std::endl;
	}
}
#endif // 1
