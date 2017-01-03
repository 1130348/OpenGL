#include "httpRequest.h"
#include <string>
#include <iostream>

httpRequest::httpRequest()
{
	this->initialize("GET", "127.0.0.1", "/");
	//this->fullRequest = "You need to add host URI and/or some headers";   
}

void httpRequest::initialize(std::string methodName, std::string hostName, std::string uriValue)
{
	this->method = methodName;
	this->httpVer = "HTTP/1.1";
	this->host = hostName;
	this->uri = " " + uriValue + " ";
}

std::string httpRequest::parseHeaders()
{
	for (int i = 0; i < this->headers.size(); i++)
	{
		this->parsedHeaders += this->headers.at(i);
	}

	return this->parsedHeaders;
}

void httpRequest::setMethod(std::string methodName)
{
	this->method = methodName;
}

void httpRequest::setHost(std::string hostName)
{
	this->fullHost = "HOST: " + hostName;
	this->host = hostName;
}

void httpRequest::setUri(std::string uriValue)
{
	this->uri = " " + uriValue + " ";
}

void httpRequest::addHeader(std::string httpHeader)
{
	this->headers.push_back(httpHeader + "\r\n");
}


void httpRequest::buildRequest()
{
	this->fullRequest = this->method + this->uri + this->httpVer + "\r\n";
	this->fullRequest = this->fullRequest + this->fullHost + "\r\n";
	this->fullRequest = this->fullRequest + this->parseHeaders() + "\r\n";
	this->fullRequest = this->fullRequest + "\r\n";
}


std::string httpRequest::getHost()
{
	return this->host;
}


std::string httpRequest::toString()
{
	return this->fullRequest;
}

int httpRequest::getRequestLength()
{
	return this->fullRequest.length();
}