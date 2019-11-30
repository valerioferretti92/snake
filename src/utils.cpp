#include "utils.h"
#include <iostream>
#include <sstream>
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

string getPlaytimeAsString(unsigned int duration){
  stringstream ss;

  ss << duration / 1000 << "s " << duration % 1000 << "ms ";
  return ss.str();
}

string getCompletionAsString(double completion){
  stringstream ss;

  ss << setprecision(3) << completion * 100 << "%";
  return ss.str();
}

void printMatchSummary(match_data matchData){
  cout << "Area: " << matchData.rows * matchData.columns << endl;
  cout << "Snake Length: " << matchData.length << endl;
  cout << "Completion: " << getCompletionAsString(matchData.completion) << endl;
  cout << "Sleep Time: " << matchData.sleepTime << endl;
  cout << "Ms per Apple: " << matchData.msPerApple << endl;
  cout << "Victory: " << matchData.hasWon << endl;
  cout << "Play time: " << getPlaytimeAsString(matchData.duration) << endl;
}
