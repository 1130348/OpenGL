#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "httpClient.h"
#include "httpRequest.h"
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma GCC diagnostic ignored "-Wwrite-strings"

//Mudar Porto confomre o Porto da API
#define DEFAULT_PORT "18688"

httpClient::httpClient()
{
	int wsResult = 0;

	wsResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (wsResult != 0) {
		std::cout << "WSAStartup failed with error: " << wsResult << std::endl;
	}

	ZeroMemory(&this->hints, sizeof(this->hints));

	this->hints.ai_family = AF_UNSPEC;
	this->hints.ai_socktype = SOCK_STREAM;
	this->hints.ai_protocol = IPPROTO_TCP;
}

void httpClient::connectTo(std::string ip)
{
	int wsResult = 0;

	wsResult = getaddrinfo(ip.c_str(), DEFAULT_PORT, &this->hints, &result);
	if (wsResult != 0) {
		std::cout << "getaddrinfo failed with error: " << wsResult << std::endl;
		WSACleanup();
	}

	for (this->ptr = this->result; this->ptr != NULL; this->ptr = this->ptr->ai_next) {
		// Create a SOCKET 
		this->connectSocket = socket(this->ptr->ai_family, this->ptr->ai_socktype,
			this->ptr->ai_protocol);
		if (this->connectSocket == INVALID_SOCKET) {
			std::cout << "Socket failed with error: " << WSAGetLastError();
			WSACleanup();
		}

		// Connect to server.
		wsResult = connect(this->connectSocket, this->ptr->ai_addr, (int)this->ptr->ai_addrlen);
		if (wsResult == SOCKET_ERROR) {
			closesocket(this->connectSocket);
			this->connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (this->connectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		exit(1);
	}
}


std::string httpClient::receive(httpRequest request)
{
	int iResult = 0, requestLength = 0, iSent = 0;
	requestLength = request.getRequestLength();
	char buffer[10054];

	do {
		iResult = send(this->connectSocket, request.toString().c_str(), requestLength - iSent, 0);
		iSent = iSent + iResult;
	} while (iSent < requestLength);

	iResult = recv(this->connectSocket, buffer, 10054, 0);
	if (iResult > 0)
	{
		//std::cout<<buffer<<std::endl;
	}
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	closesocket(this->connectSocket);
	WSACleanup();

	std::string s = buffer;

	return s;
}

void httpClient::sendRequest(httpRequest request)
{
	this->connectTo(request.getHost());
	this->resp = this->receive(request);
}

std::string httpClient::getResponse()
{
	return this->resp;
}