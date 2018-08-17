// IOCPClient class implement


#include "stdafx.h"
#include "IocpClient.h"

//member initialization
HANDLE IocpClient::m_hIocp = NULL;
Handler IocpClient::m_pfnHandler = NULL;
Initiator IocpClient::m_pfnInitiator = NULL;


IocpClient::IocpClient() // constructor
{
}


IocpClient::~IocpClient() // destructor
{
}


bool IocpClient::globalInit() // init global envirenment
{
	if (!m_pfnHandler || !m_pfnInitiator)
		return false;

	//winsock initialization
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;

	//get processor num
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int threadNum = sysInfo.dwNumberOfProcessors * 2;

	//create iocp
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadNum);
	if (!m_hIocp)
		return false;

	loop(threadNum);

	return true;
}


bool IocpClient::start(int asynNum, int flag) // start a client
{
	if (!localInit(asynNum))
		return false;

	for (int i = 0; i < asynNum; i++)
	{
		m_pfnInitiator(flag);
	}

	return true;
}


PerHandleContext* IocpClient::allocateHandleContext(int flag) // handle context allocator
{
	PerHandleContext *pHandleContext = new PerHandleContext();
	if (pHandleContext)
	{
		//create socket
		pHandleContext->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (pHandleContext->socket != INVALID_SOCKET)
		{
			pHandleContext->flag = flag;
			pHandleContext->pIOContext = new PerIOContext();
			if (pHandleContext->pIOContext)
			{
				//initialize overlapped
				pHandleContext->pIOContext->overlapped.Internal = 0;
				pHandleContext->pIOContext->overlapped.InternalHigh = 0;
				pHandleContext->pIOContext->overlapped.Offset = 0;
				pHandleContext->pIOContext->overlapped.OffsetHigh = 0;
				pHandleContext->pIOContext->overlapped.hEvent = NULL;

				pHandleContext->pIOContext->ioEvent = IO_CONNECT;
				pHandleContext->pIOContext->bufWrite.buf = pHandleContext->pIOContext->fixedWriteBuf;
				pHandleContext->pIOContext->bufWrite.len = sizeof(pHandleContext->pIOContext->fixedWriteBuf); // delayed determine
				pHandleContext->pIOContext->bufRead.buf = pHandleContext->pIOContext->fixedReadBuf;
				pHandleContext->pIOContext->bufRead.len = sizeof(pHandleContext->pIOContext->fixedReadBuf);

				ZeroMemory(pHandleContext->pIOContext->bufRead.buf, pHandleContext->pIOContext->bufRead.len);
			}
		}
		else
		{
			delete pHandleContext;
		}
	}

	return pHandleContext;
}


void IocpClient::releaseHandleContext(PerHandleContext* pHandleContext) // handle context deallocator
{
	if (pHandleContext)
	{
		if (pHandleContext->pIOContext)
		{
			delete pHandleContext->pIOContext;
		}

		delete pHandleContext;
	}
}


void IocpClient::setHandler(Handler handler) // set loop handler
{
	m_pfnHandler = handler;
}


void IocpClient::setInitiator(Initiator initiator) // set asynchronous operation initiator
{
	m_pfnInitiator = initiator;
}


bool IocpClient::localInit(int asynNum) // init a client instance
{
	return true;
}


void IocpClient::loop(int threadNum) // start worker threads for poll completion state
{
	for (int i = 0; i < threadNum; i++)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_pfnHandler, (LPVOID)m_hIocp, 0, NULL);
	}
}