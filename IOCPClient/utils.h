//utils declaration


#pragma once

#include "stdafx.h"
#include <string>


bool parseHost(const char* host, std::string* arr); // parse specified host to a array of name and port
bool getAddrByName(const char* host, sockaddr* sa); // get host address
bool bindSocket(SOCKET s); // bind specified socket to local address
bool ConnectEx(SOCKET s, const sockaddr* name, int namelen, LPOVERLAPPED lpOverlapped); // connect using overlapped