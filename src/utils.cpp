#include "utils.h"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;

string generateIdentifier(){
  unsigned long long int millsecondsfromEpoch;

  millsecondsfromEpoch = chrono::duration_cast<chrono::milliseconds>(
    chrono::system_clock::now().time_since_epoch()).count();
  return to_string(millsecondsfromEpoch);
}

string getTimestamp() {
  const auto now = std::chrono::system_clock::now();
  const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);

  std::stringstream timestamp;
  timestamp << std::put_time(std::localtime(&nowAsTimeT), "%a %b %d %Y %T");
  return timestamp.str();
}

void printMatchSummary(match_data matchData){
  cout << "Area: " << matchData.rows * matchData.columns << endl;
  cout << "Snake Length: " << matchData.length << endl;
  cout << "Sleep Time: " << matchData.sleepTime << endl;
  cout << "Victory: " << matchData.hasWon << endl;
}

void printTotalPlayTime(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2){
  auto s = chrono::duration_cast<chrono::seconds>(t2 - t1).count();
  auto ms = chrono::duration_cast<chrono::milliseconds>((t2 - t1) % chrono::seconds(1)).count();
  cout << "Play Time: " << s << "s " << ms << "ms " << endl;
}
