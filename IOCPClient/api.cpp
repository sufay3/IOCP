//api implement

#include "stdafx.h"
#include "api.h"
#include "iocpclient.h"
#include "iocpclientimp.h"


//reference function pointer
extern GetDestHost pfnGetDestHost;
extern RequestBuilder pfnRequestBuilder;
extern OnResponse pfnOnResponse;


bool WINAPI initIocp() //initialize global envirenment
{
	IocpClient::setInitiator(initiator);
	IocpClient::setHandler(handler);

	return IocpClient::globalInit();
}


void WINAPI startIocp(int asynNum, int flag) //start iocp
{
	IocpClient client;
	client.start(asynNum, flag);
}


void WINAPI setGetDestHostFn(GetDestHost pfnGDH) // set getDestHost callback
{
	pfnGetDestHost = pfnGDH;
}


void WINAPI setRequestBuilder(RequestBuilder pfnRB) // set requestBuilder callback
{
	pfnRequestBuilder = pfnRB;
}


void WINAPI setOnResponse(OnResponse pfnOR) // set onResponse callback
{
	pfnOnResponse = pfnOR;
}

