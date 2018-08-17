// type declaration

#pragma once

#define MAX_BUFFER_SIZE 8192

// io event
typedef enum{
	IO_CONNECT,
	IO_READ,
	IO_WRITE,
}IO_EVENT;


//io context
typedef struct{
	WSAOVERLAPPED overlapped;
	IO_EVENT ioEvent;
	WSABUF bufWrite;
	WSABUF bufRead;
	char fixedWriteBuf[MAX_BUFFER_SIZE];
	char fixedReadBuf[MAX_BUFFER_SIZE];
}PerIOContext, *PPerIOContext;


//handle context
struct PerHandleContext
{
	SOCKET socket;
	PPerIOContext pIOContext;
	int flag;
	int extra;
};


//function pointer
typedef DWORD(*Handler) (LPVOID);
typedef bool(*Initiator) (int);
