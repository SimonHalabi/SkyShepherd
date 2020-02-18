// TCP-Server.cpp : Defines the entry point for the console application.
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
#include <fstream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 256


int __cdecl main(void)
{
	std::cout << "****************\n*    SERVER    *\n****************\n\n";


	char str[INET_ADDRSTRLEN];

	//Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		std::cout << "WSAStartup failed with error: " << iResult << std::endl;
		return 1;
	}


	//Create a SOCKET for listening for incoming connections request
	SOCKET ListenSocket, ClientSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}


	//The sockaddr_in structure specifies the address family,
	//IP address, and port for the socket that is being bound
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	InetPton(AF_INET, "127.0.0.1", &addrServer.sin_addr.s_addr);
	//InetPton(AF_INET, "192.168.0.20", &addrServer.sin_addr.s_addr);
	addrServer.sin_port = htons(6666);
	memset(&(addrServer.sin_zero), '\0', 8);

	//Bind socket
	if (bind(ListenSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
		std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}


	//Listen for incomin connection requests on the created socket
	if (listen(ListenSocket, 5) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "Accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ListenSocket);


	//Variables for recieve
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	// Receive until the peer shuts down the connection
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		//printf("Bytes received: %d\n", iResult);


		//Create string
		std::string filename;
		//std::cout << filename << std::endl;
		for (int i = 0; i < iResult; i++) filename += recvbuf[i];
		//std::cout << "Opening file: " << filename << std::endl;

		//Open file and count bytes
		std::fstream file;
		file.open(filename, std::ios::in | std::ios::binary);


		file.seekg(0, std::ios::end);
		int fileSize = file.tellg();
		file.close();
		std::cout << "File size: " << fileSize << std::endl;


		//Convert to char array
		std::string temp = std::to_string(fileSize);
		//std::cout << "Filesize in string: " << temp << std::endl;
		char tempc[DEFAULT_BUFLEN];
		int i = 0;
		while (temp[i] != '\0') {
			tempc[i] = temp[i];
			i++;
		}
		tempc[i] = '\0';
		//std::cout << "Filesize in char array: " << tempc << std::endl;
		const char* sendbuf = tempc;







		// Send file size to client
		iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iSendResult == SOCKET_ERROR) {
			std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		//std::cout << "Bytes sent: " << iSendResult << std::endl;
	}
	else if (iResult == 0)
		std::cout << "Connection closing...\n" << std::endl;
	else {
		std::cout << "Recieve failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Connection closing...\n" << std::endl;

	//Shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	while (true);
	//std::string exit;
	//std::cin >> exit;
	return 0;
}