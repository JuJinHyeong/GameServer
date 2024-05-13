#include "ShootingGameServer.h"
#include <iostream>
#include <string>

ShootingGameServer::ShootingGameServer()
	:
	ServerBase()
{
	mRoomArr.resize(MAX_ROOM_COUNT);
}

ShootingGameServer::~ShootingGameServer()
{
	ServerBase::~ServerBase();
	DBManager::Close();
}

bool ShootingGameServer::Initialize()
{
	bool result = ServerBase::Initialize();
	if (!result) {
		return false;
	}
	result = DBManager::Initialize();
	if (!result) {
		return false;
	}
	return true;
}

void ShootingGameServer::DispatchThread()
{
	while (mbIsDispatchRun) {
		Message msg = mMessageQueue.Pop();
		if (msg.opcode == "LOGIN") {
			// TODO: aysnc call
			bool result = false;
			int end = 0;
			while (msg.data[++end] != ':');
			std::string userId = msg.data.substr(0, end);
			std::string pw = msg.data.substr(end + 1);
			std::vector<User> users;
			DBManager::FindUsers(userId, users);
			if (users.size() == 1) {
				User& user = users[0];
				if (user.pw == pw) {
					result = true;
				}
			}
			BindSend(msg.pClientInfo, result ? "1" : "0", 1);
		}
		else if (msg.opcode == "SIGNUP") {
			int end = 0;
			while (msg.data[++end] != ':');
			std::string userId = msg.data.substr(0, end);
			std::string pw = msg.data.substr(end + 1);
			bool result = DBManager::CreateUser(userId, pw);
			BindSend(msg.pClientInfo, result ? "1" : "0", 2);
		}
		else if (msg.opcode == "JOIN") {
			int roomId = JoinRoom(msg.pClientInfo, msg.data.empty() ? -1 : std::stoi(msg.data));
			std::string response = std::to_string(roomId);
			BindSend(msg.pClientInfo, response.c_str(), (int)response.size());
		}
		else if (msg.opcode == "MOVE") {

		}
		else if (msg.opcode == "SHOOT") {

		}
		else {

		}
	}
}

void ShootingGameServer::OnConnect(ClientInfo* pClientInfo)
{
	std::cout << "Connected Client IP: " << pClientInfo->IP << std::endl;
}

void ShootingGameServer::OnReceive(ClientInfo* pClientInfo, const char* packet, int packetSize)
{
	mMessageQueue.Push({ pClientInfo, packet, packetSize });
}

void ShootingGameServer::OnClose(ClientInfo* pClientInfo)
{
	std::cout << "Disconnected Client IP: " << pClientInfo->IP << std::endl;
}

int ShootingGameServer::JoinRoom(ClientInfo* pClientInfo, int roomId)
{
	if (roomId < 0) {
		// create new room
		int playerIndex = -1;
		std::lock_guard<std::mutex> lock(mMutex);
		for (int i = 0; i < mRoomArr.size(); i++) {
			if (mRoomArr[i].IsEmpty()) {
				roomId = i;
				playerIndex = mRoomArr[i].AddPlayer({ pClientInfo });
				break;
			}
		}
	}
	else {
		// join existed room
		int playerIndex = -1;
		std::lock_guard<std::mutex> lock(mMutex);
		playerIndex = mRoomArr[roomId].AddPlayer({ pClientInfo });
	}

	mClientRoomMap[pClientInfo] = roomId;

	return roomId;
}

bool ShootingGameServer::LeaveRoom(ClientInfo* pClientInfo)
{
	bool ret = false;
	int roomId = mClientRoomMap[pClientInfo];
	{
		std::lock_guard<std::mutex> lock(mMutex);
		ret = mRoomArr[roomId].RemovePlayer({ pClientInfo });
	}
	return ret;
}

