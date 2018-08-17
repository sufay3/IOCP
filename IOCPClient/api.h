//api declaration

#pragma once

#include "stdafx.h"
#include "iocpclientimp.h"


bool WINAPI initIocp(); //initialize global envirenment
void WINAPI startIocp(int asynNum, int flag = 0); //start iocp

void WINAPI setGetDestHostFn(GetDestHost); // set getDestHost callback
void WINAPI setRequestBuilder(RequestBuilder); // set requestBuilder callback
void WINAPI setOnResponse(OnResponse); // set onResponse callback