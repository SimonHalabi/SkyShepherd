#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")

constexpr auto DEFAULT_BUFLEN = 256;

int __cdecl main(int argc, char** argv)
{

	std::cout << "****************\n*    CLIENT    *\n****************\n\n";
	//Initialize Winsock
	WSADATA wsaData;
	auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		std::cout << "WSAStartup Failed with error: " << iResult << std::endl;
		return 1;
	}

	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	InetPton(AF_INET, "127.0.0.1", &addrServer.sin_addr.s_addr);
	//InetPton(AF_INET, "192.168.0.20", &addrServer.sin_addr.s_addr);
	addrServer.sin_port = htons(6666);
	memset(&(addrServer.sin_zero), '\0', 8);

	// Connect to server.
	std::cout << "Connecting..." << std::endl;
	iResult = connect(ConnectSocket, (SOCKADDR*)&addrServer, sizeof(addrServer));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		std::cout << "Unable to connect to server: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	//variables
	char filename[DEFAULT_BUFLEN] = { 0 };
	std::cout << "Name of file: ";
	std::cin.getline(filename, DEFAULT_BUFLEN, '\n');
	//std::cout << filename;

	std::fstream InFile;
	InFile.open(filename, std::ios::in | std::ios::binary);
	while (!InFile.is_open())
	{
		std::cout << "File not found" << std::endl;
		std::cout << "Name of file: ";
		std::cin.getline(filename, DEFAULT_BUFLEN, '\n');
		InFile.open(filename, std::ios::in | std::ios::binary);
	}
	std::cout << '\n';
	InFile.close();

	const char* sendbuf = filename;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	//std::cout << "Dlugosc tab:" << (int)strlen(sendbuf) << std::endl;

	// Send file name
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);



	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			std::string PrintString;
			for (int i = 0; i < iResult; i++)
				PrintString += recvbuf[i];
			std::cout << "Message Received: " + PrintString;
		}
		else if (iResult == 0)
			std::cout << "Connection closed\n" << std::endl;
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);


	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	while (true);
	return 0;
}