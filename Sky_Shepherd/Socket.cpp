#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#define _WINSOCK_DEPRECIATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

constexpr auto PORT = 80;

const char szHost[] = "google.com";

SOCKET create_socket()
{
	const auto sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		ExitProcess(EXIT_FAILURE);
	return sock;
}

HOSTENT get_server_info()
{
	HOSTENT* host = gethostbyname(szHost);
	if (host == nullptr)
		ExitProcess(EXIT_FAILURE);
	return *host;
}

void connect_to_server(SOCKET sock, SOCKADDR_IN sin)
{
	if (connect(sock, (const sockaddr*)&sin, sizeof(sin)) != 0)
		ExitProcess(EXIT_FAILURE);
}

void send_message(const char szMsg [],SOCKET sock)
{
	if (!send(sock, szMsg, strlen(szMsg), 0))
		ExitProcess(EXIT_FAILURE);
}

//clean up code ReSharper compliant
int main2()
{
	//init winsock
	WSAData wsaData;
	const auto DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
		ExitProcess(EXIT_FAILURE);

	//define server info
	SOCKADDR_IN sin;
	ZeroMemory(&sin, sizeof(sin));
	sin.sin_port = htons(PORT);
	sin.sin_family = AF_INET;
	memcpy(&sin.sin_addr.S_un.S_addr, get_server_info().h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));
	
	SOCKET sock = create_socket();
	connect_to_server(sock,sin);
	send_message("HEAD / HTTP/1.0\r\n\r\n", sock);

	char szBuffer[4096] = "";
	char szTemp[4096] = "";
	while (recv(sock, szTemp, 4096, 0))
		strcat(szBuffer, szTemp);
	std::cout << szBuffer << std::endl;

	closesocket(sock);
	WSACleanup();
	system("pause");
	ExitProcess(EXIT_SUCCESS);
}