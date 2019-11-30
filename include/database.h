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
      "completion REAL NOT NULL,"
      "sleepTime INTEGER NOT NULL,"
      "msPerApple INTEGER NOT NULL,"
      "gameOver INTEGER NOT NULL DEFAULT 0,"
      "hasWon INTEGER NOT NULL DEFAULT 0,"
      "duration INTEGER NOT NULL,"
      "timestamp TEXT NOT NULL);";

  string insertMatchData =
    "INSERT INTO Match VALUES("
      ":match_id,"
      ":rows,"
      ":columns,"
      ":length,"
      ":completion,"
      ":sleep_time,"
      ":ms_per_apple,"
      ":game_over,"
      ":has_won,"
      ":duration,"
      ":timestamp);";

  string getDbFolderPath();
  string getDbFilePath();

public:
  Database();
  ~Database();
  void initialize();
  void insert(match_data matchData);
};

#endif
