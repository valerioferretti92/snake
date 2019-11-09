#ifndef SCORE_ENTRY_H
#define SCORE_ENTRY_H

#include <string>

using namespace std;

struct score_entry{
  string text;
  unsigned int value;
};

score_entry getScoreEntry(string text, unsigned int value);

#endif
