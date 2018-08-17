// iocp client declaration

#pragma once

#include "stdafx.h"
#include "base.h"


// function pointer declaration
typedef const char* (WINAPI *GetDestHost) ();
typedef void (WINAPI *RequestBuilder) (char*, int, int*);
typedef void (WINAPI *OnResponse) (const char*, int, int);


bool initiator(int flag); //asynchronous operation initiator
DWORD handler(LPVOID handlerContext); // used to supervise completion queue
void cleanup(PerHandleContext* pHandleContext); // cleanup

const char* getDestHost(); // get destination host
void requestBuilder(char* request, int flag = 0, int* extra = NULL); // build request
void onResponse(const char* response, int flag = 0, int extra = 0); // handler for response
