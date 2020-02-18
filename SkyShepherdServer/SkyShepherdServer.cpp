#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include "Sheep.h"
#include <Windows.h>
#include <algorithm>
#include <array>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

constexpr auto DEFAULT_BUFLEN = 256;

int time_to_seconds(std::string time)
{
	auto Hours = 0;
	auto Minutes = 0;
	auto Seconds = 0;
	for (auto i = 0; i < 3; i++)
	{
		std::string TimeToken = "Time Token";
		std::size_t Pos = 0;
		std::string Delimeter = ":";
		if ((Pos = time.find(Delimeter)) != std::string::npos)
		{
			TimeToken = time.substr(0, Pos);
			if (i == 0)
				Hours = std::stoi(TimeToken);
			time.erase(0, Pos + Delimeter.length());
			Minutes = std::stoi(TimeToken.substr(0, Pos));
		}
		else
		{
			Seconds = std::stoi(time);
		}
	}
	return Hours * 3600 + Minutes * 60 + Seconds;
}

bool compare(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.first < b.first;
}

int __cdecl main(void)
{
	std::cout << "****************\n*    SERVER    *\n****************\n\n";
	char str[INET_ADDRSTRLEN];
	//Initialize Winsock
	WSADATA wsaData;
	auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
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


	//Listen for incoming connection requests on the created socket
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
	auto recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		//printf("Bytes received: %d\n", iResult);
		//Create string
		std::string filename;
		//std::cout << filename << std::endl;
		for (int i = 0; i < iResult; i++) filename += recvbuf[i];
		//std::cout << "Opening file: " << filename << std::endl;

		//Open file
		std::fstream InFile;
		InFile.open(filename, std::ios::in | std::ios::binary);
		
		auto NumRows = -2;
		auto NumColumns = 1;
		std::string Line;
		while (!InFile.eof())
		{
			//read row
			std::getline(InFile, Line, '\n');
			if (NumRows == -1)
			{
				std::string Token = "token";
				std::size_t Pos = 0;
				std::string Delimeter = ",";
				while ((Pos = Line.find(Delimeter)) != std::string::npos)
				{
					Token = Line.substr(0, Pos);
					Line.erase(0, Pos + Delimeter.length());
					NumColumns++;
				}
			}
			NumRows++;
		}
		//Close "BBOB.csv"
		InFile.close();
		auto* BBOB = new Sheep[NumRows];
		//InFile.open(R"(C:\Users\Simon\Desktop\study\EWE\SkySheperd\BBOB.csv)");
		InFile.open(filename, std::ios::in | std::ios::binary);
		std::getline(InFile, Line, '\n'); //removes first row
		for (auto i = 0; i < NumRows; i++)
		{
			std::getline(InFile, Line, '\n');
			for (auto j = 0; j < NumColumns; j++)
			{
				std::string Token = "token";
				std::size_t Pos = 0;
				std::string Delimeter = ",";
				if ((Pos = Line.find(Delimeter)) != std::string::npos)
				{
					Token = Line.substr(0, Pos);
					Line.erase(0, Pos + Delimeter.length());
					switch (j)
					{
					case 2:
						BBOB[i].EventTime = Token;
						break;
					case 3:
						BBOB[i].Longitude = std::stod(Token);
						break;
					case 4:
						BBOB[i].Latitude = std::stod(Token);
						break;
					case 5:
						BBOB[i].Altitude = std::stoi(Token);
						break;
					case 6:
						BBOB[i].HeartRate = std::stod(Token);
						break;
					case 7:
						BBOB[i].BreathingRate = std::stod(Token);
						break;
					case 8:
						BBOB[i].Activity = std::stod(Token);
						break;
					case 9:
						BBOB[i].PeakAcceleration = std::stod(Token);
						break;
					case 10:
						BBOB[i].Original = Token;
						break;
					default:
						break;
					}
				}
				else
				{
					BBOB[i].Original = Line;
				}
			}
		}
		auto* p = new std::pair<int, int>[NumRows];
		for (auto i = 0; i < NumRows; i++)
		{
			p[i] = std::make_pair(time_to_seconds(BBOB[i].EventTime), i + 1);
		}
		std::sort(p, p + NumRows, compare);
		auto* CorrectOrder = new Sheep[NumRows];
		for (auto i = 0; i < NumRows; i++)
		{
			CorrectOrder[i] = BBOB[p[i].second - 1];
		}
		delete[] p;
		for (auto i = 0; i < NumRows; i++)
		{
			BBOB[i] = CorrectOrder[i];
		}
		delete[] CorrectOrder;
		InFile.close();

		auto Delay = 0;
		for (auto i = 0; i < NumRows; i++)
		{
			std::string TransmitString = "";
			if (i == 0)
				Delay = 0;
			else
			{
				Delay = time_to_seconds(BBOB[i].EventTime) - time_to_seconds(BBOB[i - 1].EventTime);
			}
			//sleep for the delay (convert milliseconds to seconds)
			Sleep(Delay * 1000);
			//print data
			TransmitString = "Time: " + BBOB[i].EventTime + " ";
			TransmitString += "Longitude: " + std::to_string(BBOB[i].Longitude) + " ";
			TransmitString += "Latitude: " + std::to_string(BBOB[i].Latitude) + " ";
			TransmitString += "Altitude: " + std::to_string(BBOB[i].Altitude) + '\n';
			//std::cout << TransmitString;
			
			// Send string to client
			iSendResult = send(ClientSocket, TransmitString.c_str(), (int)strlen(TransmitString.c_str()), 0);
			if (iSendResult == SOCKET_ERROR) {
				std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			else
				std::cout << "Sending Message: " + TransmitString;
		}
		delete[] BBOB;
	

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
	return 0;
}