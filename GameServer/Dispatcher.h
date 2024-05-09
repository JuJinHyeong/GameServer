#pragma once
#include <vector>
#include <thread>
#include <WinSock2.h>
#include "ThreadSafeQueue.h"
#include "Message.h"
#include "IOCompletionPort.h"

class Dispatcher
{
public:
	Dispatcher(IOCompletionPort& iocp, ThreadSafeQueue<Message>& messageQueue);
	~Dispatcher();

	bool CreateThread();
	void DestoryThread();

private:
	void DispatchThread();

private:
	std::vector<std::thread> mDispatchThreadArr;
	bool mbIsDispatchRun = true;

private:
	ThreadSafeQueue<Message>& mMessageQueue;
	IOCompletionPort& mIOCP;
	static constexpr UINT32 MAX_DISPATCH_THREAD_COUNT = 4;
};
