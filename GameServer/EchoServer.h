#pragma once
#include "ServerBase.h"

class EchoServer : public ServerBase
{
protected:
	virtual void OnConnect(ClientInfo* pClientInfo) override;
	virtual void OnReceive(ClientInfo* pClientInfo, const char* buffer, int length) override;
	virtual void OnClose(ClientInfo* pClientInfo) override;
};

