#include "DBManager.h"
#include <iostream>

sqlite3* DBManager::mDB = nullptr;
bool DBManager::bIsInit = false;

bool DBManager::Initialize()
{
	int result = sqlite3_open("server.db", &mDB);
	if (result) {
		std::cerr << "cannot open database: " << sqlite3_errmsg(mDB) << std::endl;
		return false;
	}
	else {
		std::cout << "database opened successfully\n";
	}

	result = Exec("CREATE TABLE IF NOT EXISTS users \
			(id INTEGER PRIMARY KEY AUTOINCREMENT, \
			user_id TEXT NOT NULL UNIQUE, \
			pw TEXT NOT NULL);");
	if (!result) {
		return false;
	}

	return true;
}

bool DBManager::Exec(const std::string& sql)
{
	char* errMsg = nullptr;
	int result = sqlite3_exec(mDB, sql.c_str(), nullptr, 0, &errMsg);
	if (result != SQLITE_OK) {
		std::cerr << "Sql Error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	else {
		std::cout << "sql executed successfully\n";
	}
	return true;
}

std::string DBManager::Encrypt(const std::string& pw)
{
	return pw + "123";
}

bool DBManager::Close()
{
	int result = sqlite3_close(mDB);
	if (result) {
		std::cerr << "sql close failed\n";
		return false;
	}
	else {
		std::cout << "sql closed successfully\n";
	}
	return true;
}

bool DBManager::CreateUser(const std::string& user_id, const std::string& pw)
{
	std::string sql = "INSERT INTO users (user_id, pw) VALUES ('" + user_id + "', '" + Encrypt(pw) + "');";
	const char* tail = nullptr;
	return false;
}

bool DBManager::FindUsers(const std::string& user_id, std::vector<User>& users)
{
	users.clear();
	sqlite3_stmt* stmt;
	std::string sql = "SELECT id, user_id, pw FROM users WHERE user_id=" + user_id;
	std::cout << sql << std::endl;
	const char* tail = nullptr;
	int result = sqlite3_prepare_v3(mDB, sql.c_str(), -1, 0, &stmt, &tail);
	if (result != SQLITE_OK) {
		std::cerr << "SQL 준비 오류: " << sqlite3_errmsg(mDB) << '\n';
		sqlite3_close(mDB);
		return false;
	}

	while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
		User user;
		user.id = sqlite3_column_int(stmt, 0);
		user.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		user.pw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		users.push_back(user);
	}

	if (result != SQLITE_DONE) {
		std::cerr << "SQL 실행 오류: " << sqlite3_errmsg(mDB) << '\n';
		return false;
	}
	return true;
}

bool DBManager::FindUsers(const std::string& user_id, User* users, const unsigned int size)
{
	sqlite3_stmt* stmt;
	std::string sql = "SELECT id, user_id, pw FROM users WHERE user_id=" + user_id;
	const char* tail = nullptr;
	int result = sqlite3_prepare_v3(mDB, sql.c_str(), -1, 0, &stmt, &tail);
	if (result != SQLITE_OK) {
		std::cerr << "SQL 준비 오류: " << sqlite3_errmsg(mDB) << '\n';
		sqlite3_close(mDB);
		return false;
	}

	unsigned int index = 0;
	while ((result = sqlite3_step(stmt)) == SQLITE_ROW && index < size) {
		User user;
		user.id = sqlite3_column_int(stmt, 0);
		user.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		user.pw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		users[index++] = user;
	}

	if (result != SQLITE_DONE) {
		std::cerr << "SQL 실행 오류: " << sqlite3_errmsg(mDB) << '\n';
		return false;
	}
	return true;
}


