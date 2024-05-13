#pragma once
#include <unordered_map>
#include <winsqlite\winsqlite3.h>
#include <vector>
#include <mutex>
#include "ServerBase.h"
#include "ClientInfo.h"
#include "Room.h"
#include "DBManager.h"

class ShootingGameServer : public ServerBase
{
public:
	ShootingGameServer();
	virtual ~ShootingGameServer() override;
	bool Initialize();
	int JoinRoom(ClientInfo* pClientInfo, int roomId);
	bool LeaveRoom(ClientInfo* pClientInfo);

protected:
	virtual void DispatchThread() override;

	virtual void OnConnect(ClientInfo* pClientInfo) override;
	virtual void OnReceive(ClientInfo* pClientInfo, const char* packet, int packetSize) override;
	virtual void OnClose(ClientInfo* pClientInfo) override;

private:
	
	std::unordered_map<ClientInfo*, int> mClientRoomMap;
	std::vector<Room> mRoomArr;

private:
	std::mutex mMutex;
	std::condition_variable mCv;
	constexpr static UINT32 MAX_ROOM_COUNT = 10;
};

