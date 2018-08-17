//utils


#include "stdafx.h"
#include "utils.h"



bool parseHost(const char* host, std::string* arr) // parse specified host to a array of name and port
{
	std::string shost = host;
	int sepLoc = shost.find(":");

	if (sepLoc < 0)
		return false;

	arr[0] = shost.substr(0, sepLoc);
	arr[1] = shost.substr(sepLoc + 1);

	return arr[0] != "" && arr[1] != "";
}


bool getAddrByName(const char* host, sockaddr* sa) // get host address
{
	std::string arr[2];
	if (!parseHost(host, arr))
		return false;

	addrinfo *ai = NULL;
	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(arr[0].c_str(), arr[1].c_str(), &hints, &ai) != 0)
		return false;

	*sa = *ai->ai_addr;
	freeaddrinfo(ai);

	return true;
}


bool bindSocket(SOCKET s) // bind specified socket to local address
{
	sockaddr_in sa;

	ZeroMemory(&sa, sizeof(sa)); //local address and port auto-allocated by system
	sa.sin_family = AF_INET;

	return  bind(s, (sockaddr*)&sa, sizeof(sa)) == 0;
}


bool ConnectEx(SOCKET s, const sockaddr* name, int namelen, LPOVERLAPPED lpOverlapped) // connect using overlapped
{
	LPFN_CONNECTEX pfnConnectEx = NULL;
	GUID guidConnectEx = WSAID_CONNECTEX;
	DWORD byteNum;
	int ret;

	// get ConnectEx function pointer
	if (WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &pfnConnectEx, sizeof(pfnConnectEx), &byteNum, NULL, NULL) != 0)
		return false;

	ret = pfnConnectEx(s, name, namelen, NULL, 0, NULL, lpOverlapped);
	if (ret == 0 && WSAGetLastError() != WSA_IO_PENDING)
		return false;

	return true;
}