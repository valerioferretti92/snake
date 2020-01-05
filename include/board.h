#ifndef BOARD_H
#define BOARD_H

#include "point.h"
#include "printer.h"
#include "score_entry.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

class Board {
private:
  unsigned int matchId;
  unsigned int rows;
  unsigned int snakePaneWidth;
  unsigned int scorePaneWidth;
  unsigned int scoreBoardInitRowIndex;
  unsigned int scoreBoardInitColumnIndex;
  unsigned int gridSize;
  Printer printer;

  vector<string> looseMessage = {
    "                            ",
    "    **  **  ****  **  **    ",
    "     ****  **  ** **  **    ",
    "      **   **  ** **  **    ",
    "      **   **  ** **  **    ",
    "      **    ****   ****     ",
    "                            ",
    " **     ****    **** ****** ",
    " **    **  **  **      **   ",
    " **    **  **   **     **   ",
    " **    **  **    **    **   ",
    "  ****  ****  ****     **   ",
    "                            "};
  const unsigned int looseMessageColumns = 28;

  vector<string> winMessage = {
    "                         ",
    " **   **  *****  **   ** ",
    "  ** **  **   ** **   ** ",
    "   ***   **   ** **   ** ",
    "   ***   **   ** **   ** ",
    "   ***    *****   *****  ",
    "                         ",
    " **   **  *****  **   ** ",
    " **   ** **   ** ***  ** ",
    " ** * ** **   ** ** * ** ",
    " *** *** **   ** **  *** ",
    " **   **  *****  **   ** ",
    "                         "};
  const unsigned int winMessageColumns = 26;

  void printSnakePaneEdgeCh(unsigned int i, unsigned int j);
  void printScorePaneEdgeVerticalCh(unsigned int i, unsigned int j);
  void printScorePaneEdgeHorizontalCh(unsigned int i, unsigned int j);
  void printScoreString(unsigned int i, unsigned int j1, unsigned int j2, string formatString, unsigned int data);
  string getMinimumHeightError(unsigned int rows, unsigned int minHeight);
  string getMinimumWidthError(unsigned int columns, unsigned int minWidth);

public:
  Board(unsigned int _rows, unsigned int _columns);
  void initialize();
  void updateScore(map<unsigned int, score_entry>& scoreData);
  void printMessage(bool hasWon);
  Point getLowestPoint();
  Point getHighestPoint();
};

#endif
