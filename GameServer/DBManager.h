#pragma once
#include <winsqlite/winsqlite3.h>
#include <string>
#include <vector>

struct User {
	int id = 0;
	std::string userId;
	std::string pw;
};

class DBManager
{
public:
	// TODO: connection pool?
	static bool Initialize();
	static bool Close();
	static bool CreateUser(const std::string& user_id, const std::string& pw);
	static bool FindUsers(const std::string& user_id, std::vector<User>& users);
	static bool FindUsers(const std::string& user_id, User* users, const unsigned int size);

private:
	static bool Exec(const std::string& sql);
	// TODO: move to encryption class
	static std::string Encrypt(const std::string& pw);
	
private:
	static sqlite3* mDB;
	static bool bIsInit;
};
