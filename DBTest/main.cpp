#include <iostream>
#include <winsqlite/winsqlite3.h>

int main() {
    sqlite3* db;
    char* errMsg = nullptr;
    int rc;

    // �����ͺ��̽� ���� ����
    rc = sqlite3_open("example.db", &db);
    if (rc) {
        std::cerr << "�����ͺ��̽��� �� �� �����ϴ�: " << sqlite3_errmsg(db);
        return 1;
    }
    else {
        std::cout << "�����ͺ��̽��� ���������� ���Ƚ��ϴ�.\n";
    }

    // SQL ���� ����
    const char* sql = "CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, name TEXT NOT NULL);";
    rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL ����: " << errMsg;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "���̺� ���� �Ϸ�.\n";
    }

    // �����ͺ��̽� �ݱ�
    sqlite3_close(db);
    return 0;
}