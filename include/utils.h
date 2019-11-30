#ifndef UTILS_H
#define UTILS_H

#include "match_data.h"
#include <string>

using namespace std;

string getTimestamp();
string generateIdentifier();
string getPlaytimeAsString(unsigned int duration);
string getCompletionAsString(double completion);
void printMatchSummary(match_data matchData);

#endif
