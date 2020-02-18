// TCP-Client.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 256

int __cdecl main(int argc, char** argv)
{

	std::cout << "****************\n*    CLIENT    *\n****************\n\n";

	//Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
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
			std::string size;
			for (int i = 0; i < iResult; i++) size += recvbuf[i];
			if (size == "-1") std::cout << "No file named \"" << filename << "\"" << std::endl;
			else std::cout << "The size of file \"" << filename << "\" is: " << size << std::endl;
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