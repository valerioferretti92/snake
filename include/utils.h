#ifndef UTILS_H
#define UTILS_H

#include "match_data.h"
#include <string>
#include <chrono>

using namespace std;

string getTimestamp();
string generateIdentifier();
void printMatchSummary(match_data matchData);
void printTotalPlayTime(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2);

#endif
