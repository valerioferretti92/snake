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
  mkdir(getDbFolderPath().c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP |  S_IROTH | S_IXOTH);

  //initializing sqlite3 library and opening db connection
  sqlite3_initialize();
  sqlite_rcode = sqlite3_open_v2(getDbFilePath().c_str(), &dbConnection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if(sqlite_rcode != SQLITE_OK) throw "Could not open db!";
}

Database::~Database(){
  int sqlite_rcode, attempt = 0;
  string closingAttempt = "Failed to close db! Trying again in 1 second...";
  string forcedClosing = "Forcibly closing db!";
  string msg;

  //closing down db connection (3 attempts)
  while(attempt <= 3){
    sqlite_rcode = sqlite3_close(dbConnection);
    if(sqlite_rcode == SQLITE_OK) break;
    msg = attempt == 3 ? forcedClosing : closingAttempt;
    cout << msg << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));
    attempt++;
  }

  //deallocating resources assigned to sqlite3
  sqlite3_shutdown();
}

string Database::getDbFolderPath(){
  struct passwd* pw;
  const char* homedir;

  pw = getpwuid(getuid());
  homedir = pw->pw_dir;
  return string(homedir).append("/").append(dbFolderName).append("/");
}

string Database::getDbFilePath(){
  return getDbFolderPath().append(dbFileName);
}
