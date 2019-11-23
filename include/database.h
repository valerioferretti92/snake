#ifndef DATABASE_H
#define DATABASE_H

#include "match_data.h"
#include "sqlite3.h"
#include <string>

using namespace std;

class Database {
private:
  sqlite3* dbConnection;
  string dbFileName = "sqlite_snake.db";
  string dbFolderName = ".snake";

  string createMatchSqlc =
    "CREATE TABLE IF NOT EXISTS Match ("
      "matchId TEXT PRIMARY KEY,"
      "rows INTEGER NOT NULL,"
      "columns INTEGER NOT NULL,"
      "length INTEGER NOT NULL,"
      "sleepTime INTEGER NOT NULL,"
      "gameOver INTEGER NOT NULL DEFAULT 0,"
      "hasWon INTEGER NOT NULL DEFAULT 0,"
      "timestamp TEXT NOT NULL);";

  string getDbFolderPath();
  string getDbFilePath();

public:
  Database();
  ~Database();
  void initialize();
  void insert(match_data matchData);
};

#endif
