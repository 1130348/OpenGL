
#ifndef httpClient_H
#define httpClient_H

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "httpRequest.h"

class httpClient
{
	WSADATA wsaData;
	SOCKET connectSocket;
	std::string ip;
	std::string porto;
	std::string resp;
	struct addrinfo *result, *ptr, hints;

	void connectTo(std::string ip);
	std::string receive(httpRequest request);

public:
	httpClient();
	void httpClient::setPorto(std::string);
	void sendRequest(httpRequest request);

	std::string getResponse();
};

#endif
