#ifndef DATABASE_H
#define DATABASE_H

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
      "matchId INTEGER PRIMARY KEY,"
      "rows INTEGER NOT NULL,"
      "columns INTEGER NOT NULL,"
      "appleX INTEGER NOT NULL,"
      "appleY INTEGER NOT NULL,"
      "cDirection TEXT NOT NULL,"
      "sleepTime INTEGER NOT NULL,"
      "gameOver INTEGER NOT NULL,"
      "hasWon INTEGER NOT NULL DEFAULT 0);";

  string createSnakeSqlc =
    "CREATE TABLE IF NOT EXISTS Snake ("
      "x INTEGER NOT NULL,"
      "y INTEGER NOT NULL,"
      "matchId INTEGER NOT NULL,"
      "sequenceNumber INTEGER NOT NULL,"
      "PRIMARY KEY (x, y, matchId),"
      "FOREIGN KEY(matchId) REFERENCES Match(matchId) ON DELETE CASCADE);";

  string foreignKeysOnSqlc = "PRAGMA foreign_keys = ON";

  string getDbFolderPath();
  string getDbFilePath();

public:
  Database();
  ~Database();
  void initialize();
};

#endif
