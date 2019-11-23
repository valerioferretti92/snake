#ifndef SNAKE_H
#define SNAKE_H

#include "score_entry.h"
#include "match_data.h"
#include "printer.h"
#include "point.h"
#include <thread>
#include <atomic>
#include <queue>
#include <list>
#include <map>
#include <set>

using namespace std;

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

  void updateScore(bool hasEaten);
  void printTailChar(unsigned int i, unsigned int j);
  void printHeadChar(unsigned int i, unsigned int j);
  void printAppleChar(unsigned int i, unsigned int j);

public:
  Snake(Point _lowest, Point _highest);

  void getUserInput();
  void initialize();
  Point computeNewHead();
  Point computeNewApple();
  void update(Point nhead);

  bool getIsFeasible(Point nhead); //isFeasible
  bool getIsGameOver(); //isGameOver
  bool getHasWon(); //hasWon
  map<unsigned int, score_entry>& getScore();
  match_data getMatchData();

  void delay();
  void win();
  void loose();

private:
  void initializeScoreMap();
};

#endif
