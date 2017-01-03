
#ifndef httpRequest_H
#define httpRequest_H

#include <string>
#include <vector>

class httpRequest
{
	std::string method;
	std::string httpVer;
	std::string host;
	std::string fullHost;
	std::string uri;
	std::vector<std::string> headers;
	std::string parsedHeaders;
	std::string fullPath;
	std::string fullRequest;

	void initialize(std::string methodName, std::string hostName, std::string uriValue);
	std::string parseHeaders();

public:
	httpRequest();
	void setMethod(std::string methodName);
	void addHeader(std::string httpHeader);
	void setHost(std::string hostName);
	void setUri(std::string uriValue);
	void buildRequest();

	int getRequestLength();
	std::string getHost();

	std::string toString();
};


#endif