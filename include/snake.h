#ifndef SNAKE_H
#define SNAKE_H

#include "score_entry.h"
#include "match_data.h"
#include "printer.h"
#include "point.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <list>
#include <map>
#include <set>

using namespace std;

enum MatchStatus {WON, LOST, ONGOING};

class Snake {
private:
  string matchId;
  Point lowest;
  Point highest;
  Point apple;
  Printer printer;
  set<Point> appleCandidates;
  list<Point> snake;
  queue<char> nextDirs;
  unsigned int sleepTimeMs;
  atomic<bool> gameOver;
  atomic<bool> hasWon;
  map<unsigned int, score_entry> scoreData;
  atomic<bool> paused;
  mutex mmutex;
  condition_variable mcvariable;
  list<chrono::system_clock::time_point> pauseStartTimes;
  list<chrono::system_clock::time_point> pauseStopTimes;

  void update(Point nhead);
  Point computeNewHead();
  Point computeNewApple();
  MatchStatus computeMatchStatus(Point nhead);
  void delay();
  void win();
  void loose();
  void updateScore(bool hasEaten);
  void initializeScoreMap();
  unsigned int getPlayTime(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2);
  void printTailChar(unsigned int i, unsigned int j);
  void printHeadChar(unsigned int i, unsigned int j);
  void printAppleChar(unsigned int i, unsigned int j);

public:
  Snake(Point _lowest, Point _highest);

  void initialize();
  void getUserInput();
  map<unsigned int, score_entry>& move();
  bool getIsGameOver();
  bool getHasWon();
  match_data getMatchData(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2);
};

#endif
