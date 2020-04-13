/*
Server
Initialize Winsock.
Create a socket.
Bind the socket.
Listen on the socket for a client.
Accept a connection from a client.
Receive and send data.
Disconnect.

Client
Initialize Winsock.
Create a socket.
Connect to the server.
Send and receive data.
Disconnect.
*/
#include <iostream>
using namespace std;

#include"UDPServer.h"
#include"TCPClient.h"

int main(int argc, char** argv)
{
	//mainUDPServer(argc, argv);
	mainTCPClient(argc, argv);

	system("pause");
	return 0;
}