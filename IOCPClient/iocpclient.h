//IocpClient class declaration

#pragma once


#include "base.h"


class IocpClient
{
public:
	//members
	static HANDLE m_hIocp; // global iocp handle

	//methods
	IocpClient(); //constructor
	~IocpClient(); //destructor

	static void setHandler(Handler handler); // set loop handler
	static void setInitiator(Initiator initiator); // set asynchronous operation initiator

	static bool globalInit(); // init global envirentment
	bool start(int asynNum, int flag = 0); // start a client

	static PerHandleContext* allocateHandleContext(int flag = 0); // context allocator
	static void releaseHandleContext(PerHandleContext* pHandleContext); // context deallocator

protected:
	//members
	static Handler m_pfnHandler; // handler inner loop
	static Initiator m_pfnInitiator; // initiator

	//methods
	bool localInit(int asynNum); // init a client
	static void loop(int threadNum); // start worker threads for poll completion state
};

