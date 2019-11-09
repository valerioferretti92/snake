#ifndef SNAKE_H
#define SNAKE_H

#define FRAMES_KEY 0
#define ROWS_KEY 1
#define COLUMNS_KEY 2
#define APPLES_KEY 3
#define SCORE_KEY 4

#include "score_entry.h"
#include "printer.h"
#include "point.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <list>
#include <map>
#include <set>

using namespace std;

class Snake {
private:
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
  map<unsigned int, score_entry> scoreData = {
    {FRAMES_KEY, getScoreEntry("FRAMES: ", 0)},
    {ROWS_KEY, getScoreEntry("ROWS: ", 0)},
    {COLUMNS_KEY, getScoreEntry("COLUMNS: ", 0)},
    {APPLES_KEY, getScoreEntry("APPLES: ", 0)},
    {SCORE_KEY, getScoreEntry("SCORE: ", 0)},
  };

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

  void delay();
  void win();
  void loose();
};

#endif
