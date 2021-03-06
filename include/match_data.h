#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <string>

using namespace std;

struct match_data{
  string matchId;
  unsigned int rows;
  unsigned int columns;
  long unsigned int length;
  double completion;
  unsigned int sleepTime;
  unsigned int msPerApple;
  bool gameOver;
  bool hasWon;
  unsigned int duration;
  string timestamp;
};

#endif
