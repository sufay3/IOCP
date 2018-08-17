//iocp client implement


#include "stdafx.h"
#include "iocpclientimp.h"
#include "iocpclient.h"
#include "utils.h"


#pragma comment(lib,"ws2_32.lib") // link ws2_32.lib


//function pointer initialization
GetDestHost pfnGetDestHost = NULL;
RequestBuilder pfnRequestBuilder = NULL;
OnResponse pfnOnResponse = NULL;


bool initiator(int flag) //asynchronous operation initiator
{
	bool ret;
	PerHandleContext *pHandleContext = IocpClient::allocateHandleContext(flag); // allocate handle context

	if (pHandleContext)
	{
		if (CreateIoCompletionPort((HANDLE)pHandleContext->socket, IocpClient::m_hIocp, (ULONG_PTR)pHandleContext, 0)) // bind iocp handle
		{
			if (!bindSocket(pHandleContext->socket))
				return false;

			sockaddr sa;
			if (getAddrByName("userapi.plu.cn:80", &sa)) //get host address
			{
				ret = ConnectEx(pHandleContext->socket, &sa, sizeof(sa), &pHandleContext->pIOContext->overlapped); // connect
				if (!ret)
					return false;

				return true;
			}
		}
	}

	return false;
}


DWORD handler(LPVOID handlerContext) // used to supervise completion queue
{
	HANDLE hIocp = handlerContext;

	LPWSAOVERLAPPED lpOverlapped = NULL;
	PerHandleContext *pHandleContext = NULL;
	PPerIOContext pIOContext = NULL;
	DWORD dwByteNum;
	IO_EVENT ioEvent;

	int ret = 0;
	DWORD dwRecvFlag = 0;
	int handleFlag = 0;


	while (true)
	{
		ret = GetQueuedCompletionStatus(hIocp, &dwByteNum, (PULONG_PTR)&pHandleContext, (LPOVERLAPPED*)&lpOverlapped, INFINITE);

		//fail
		if (!pHandleContext)
			return 0;

		//get completed io event
		pIOContext = (PPerIOContext)lpOverlapped;
		ioEvent = pIOContext->ioEvent;

		//handle flag
		handleFlag = pHandleContext->flag;

		//io fail
		if (!ret || (ret && ioEvent != IO_CONNECT && dwByteNum == 0))
		{
			cleanup(pHandleContext);
			initiator(handleFlag);
			continue;
		}

		//handle io event
		switch (ioEvent)
		{
		case IO_CONNECT: // connected
			pIOContext->ioEvent = IO_WRITE; // post a write operation after connected

			requestBuilder(pIOContext->bufWrite.buf, handleFlag, &pHandleContext->extra); //(char*)allocMemory(request, strlen(request) + 1); //  // make a buffer
			pIOContext->bufWrite.len = strlen(pIOContext->bufWrite.buf); // buffer length

			//post write operation
			ret = WSASend(pHandleContext->socket, &(pIOContext->bufWrite), 1, &dwByteNum, 0, &(pIOContext->overlapped), NULL);
			if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				cleanup(pHandleContext);
				initiator(handleFlag);
				continue;
			}

			break;

		case IO_WRITE: // send finished
			pIOContext->ioEvent = IO_READ; // post a read operation after sending

			//post read operation
			dwRecvFlag = 0;
			ret = WSARecv(pHandleContext->socket, &(pIOContext->bufRead), 1, &dwByteNum, &dwRecvFlag, &(pIOContext->overlapped), NULL);
			if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				cleanup(pHandleContext);
				initiator(handleFlag);
				continue;
			}

			break;

		default: // receive operation finished
			onResponse(pHandleContext->pIOContext->bufRead.buf, handleFlag, pHandleContext->extra); //handle response

			//cleanup and re-initiator
			cleanup(pHandleContext);
			initiator(handleFlag);

		}//switch
	}//while
}


void cleanup(PerHandleContext* pHandleContext) // cleanup
{
	if (pHandleContext)
	{
		closesocket(pHandleContext->socket);
		IocpClient::releaseHandleContext(pHandleContext);
	}
}


const char* getDestHost() // get destination host
{
	return pfnGetDestHost();
}


void requestBuilder(char* reqBuf, int flag, int* extra) // build request
{
	pfnRequestBuilder(reqBuf, flag, extra);
}


void onResponse(const char* response, int flag, int extra) // handler for response
{
	pfnOnResponse(response, flag, extra);
}
