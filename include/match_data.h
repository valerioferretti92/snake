#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <string>

using namespace std;

struct match_data{
  string matchId;
  unsigned int rows;
  unsigned int columns;
  long unsigned int length;
  unsigned int sleepTime;
  bool gameOver;
  bool hasWon;
  string timestamp;
};

#endif
