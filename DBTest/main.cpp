#include <iostream>
#include <winsqlite/winsqlite3.h>

int main() {
    sqlite3* db;
    char* errMsg = nullptr;
    int rc;

    // 데이터베이스 파일 열기
    rc = sqlite3_open("example.db", &db);
    if (rc) {
        std::cerr << "데이터베이스를 열 수 없습니다: " << sqlite3_errmsg(db);
        return 1;
    }
    else {
        std::cout << "데이터베이스가 성공적으로 열렸습니다.\n";
    }

    // SQL 쿼리 실행
    const char* sql = "CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, name TEXT NOT NULL);";
    rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL 에러: " << errMsg;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "테이블 생성 완료.\n";
    }

    // 데이터베이스 닫기
    sqlite3_close(db);
    return 0;
}