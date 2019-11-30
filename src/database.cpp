#include "database.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <chrono>
#include <thread>

Database::Database(){
  int sqlite_rcode, directory_rcode;

  //creating db folder in user's home directory
  directory_rcode = mkdir(getDbFolderPath().c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP |  S_IROTH | S_IXOTH);
  if(directory_rcode == -1 && errno != EEXIST){
    throw string("Could not create db folder!");
  }

  //initializing sqlite3 library and opening db connection
  sqlite3_initialize();
  sqlite_rcode = sqlite3_open_v2(
    getDbFilePath().c_str(), &dbConnection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if(sqlite_rcode != SQLITE_OK) throw string("Could not open db!");
}

Database::~Database(){
  int sqlite_rcode, attempt = 0;
  string closingAttempt = "Failed to close db! Trying again in 1 second...";
  string forcedClosing = "Forcibly closing db!";
  string msg;

  //closing down db connection (3 attempts) and deallocating resources assigned to sqlite3
  while(attempt <= 3){
    sqlite_rcode = sqlite3_close(dbConnection);
    if(sqlite_rcode == SQLITE_OK) break;
    msg = attempt == 3 ? forcedClosing : closingAttempt;
    cout << msg << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));
    attempt++;
  }
  sqlite3_shutdown();
}

void Database::initialize(){
  char* error_message = NULL;
  int sqlite_rcode;

  sqlite_rcode = sqlite3_exec(dbConnection, createMatchSqlc.c_str(), NULL, NULL, &error_message);
  if(sqlite_rcode != SQLITE_OK) throw string(error_message);
}

void Database::insert(match_data matchData){
  sqlite3_stmt* insertStmt;
  int rcode, index;

  //Preparing statement
  rcode = sqlite3_prepare_v2(dbConnection, insertMatchData.c_str(), -1, &insertStmt, NULL);
  if(rcode != SQLITE_OK) throw string("Insert statement error at PREPARE stage!");

  //Binding statement parameters
  index = sqlite3_bind_parameter_index(insertStmt, ":match_id");
  sqlite3_bind_text(insertStmt, index, matchData.matchId.c_str(), -1, SQLITE_STATIC);
  index = sqlite3_bind_parameter_index(insertStmt, ":rows");
  sqlite3_bind_int(insertStmt, index, matchData.rows);
  index = sqlite3_bind_parameter_index(insertStmt, ":columns");
  sqlite3_bind_int(insertStmt, index, matchData.columns);
  index = sqlite3_bind_parameter_index(insertStmt, ":length");
  sqlite3_bind_int(insertStmt, index, matchData.length);
  index = sqlite3_bind_parameter_index(insertStmt, ":completion");
  sqlite3_bind_double(insertStmt, index, matchData.completion);
  index = sqlite3_bind_parameter_index(insertStmt, ":sleep_time");
  sqlite3_bind_int(insertStmt, index, matchData.sleepTime);
  index = sqlite3_bind_parameter_index(insertStmt, ":ms_per_apple");
  sqlite3_bind_int(insertStmt, index, matchData.msPerApple);
  index = sqlite3_bind_parameter_index(insertStmt, ":game_over");
  sqlite3_bind_int(insertStmt, index, matchData.gameOver ? 1 : 0);
  index = sqlite3_bind_parameter_index(insertStmt, ":has_won");
  sqlite3_bind_int(insertStmt, index, matchData.hasWon ? 1 : 0);
  index = sqlite3_bind_parameter_index(insertStmt, ":duration");
  sqlite3_bind_int(insertStmt, index, matchData.duration);
  index = sqlite3_bind_parameter_index(insertStmt, ":timestamp");
  sqlite3_bind_text(insertStmt, index, matchData.timestamp.c_str(), -1, SQLITE_STATIC);

  //Executing sqlite insert statement
  rcode = sqlite3_step(insertStmt);
  if((rcode != SQLITE_DONE) && (rcode != SQLITE_ROW))
    throw string(sqlite3_expanded_sql(insertStmt));

  //Finalizing statement
  sqlite3_finalize(insertStmt);
}

string Database::getDbFolderPath(){
  struct passwd* pw = getpwuid(getuid());
  return string(pw->pw_dir).append("/").append(dbFolderName).append("/");
}

string Database::getDbFilePath(){
  return getDbFolderPath().append(dbFileName);
}
