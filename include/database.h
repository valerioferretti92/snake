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

  string getDbFolderPath();
  string getDbFilePath();
public:
  Database();
  ~Database();
};

#endif
