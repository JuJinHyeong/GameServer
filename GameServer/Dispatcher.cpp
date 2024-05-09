#include "Dispatcher.h"
#include <iostream>

Dispatcher::Dispatcher(IOCompletionPort& iocp, ThreadSafeQueue<Message>& messageQueue)
	:
	mIOCP(iocp),
	mMessageQueue(messageQueue)
{
}

Dispatcher::~Dispatcher()
{
}

bool Dispatcher::CreateThread()
{
	for (UINT32 i = 0; i < MAX_DISPATCH_THREAD_COUNT; ++i) {
		mDispatchThreadArr.emplace_back([this]() { DispatchThread(); });
	}
	std::cout << "Create Dispatch Thread Success\n";
	return true;
}

void Dispatcher::DestoryThread()
{
	mbIsDispatchRun = false;
	for (std::thread& th : mDispatchThreadArr) {
		if (th.joinable()) {
			th.join();
		}
	}
}

void Dispatcher::DispatchThread()
{
	while (mbIsDispatchRun) {
		Message message = mMessageQueue.Pop();

		std::cout << "Receive opcode: " << message.opcode << ", value: " << message.value << "\n";

		mIOCP.BindSend(message.pClientInfo, message.value, (int)strlen(message.value));
	}
}
