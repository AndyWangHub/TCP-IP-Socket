#include <iostream>
#include "Socket.hpp"
#include <thread>

void threadFun(Socket::UDP* udp);

int main()
{
	std::string ip = "127.0.0.1";
	int prot = 2046;
	std::string str = "Server";
	Socket::Datagram pack;

	Socket::UDP udp;
	udp.bind(1024);

	std::thread t(threadFun, &udp);

	while (true)
	{
		std::cin >> str;
		udp.send(ip, prot, str);
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
			std::cout << "Date: " << pack.data << std::endl;
		}
		std::cout << "-----------------------" << std::endl;
	}
}
