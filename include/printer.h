#ifndef PRINTER_H
#define PRINTER_H

#include <vector>
#include <string>

using namespace std;

class Printer {
public:
  void initialize();
  void close();
  void flush();
  unsigned int getLines();
  unsigned int getCols();
  void printWinMessage(unsigned int i, unsigned int j, vector<string> winMessage);
  void printLooseMessage(unsigned int i, unsigned int j, vector<string> looseMessage);
  void printSnakePaneEdgeCh(unsigned int i, unsigned int j);
  void printScorePaneEdgeVerticalCh(unsigned int i, unsigned int j);
  void printScorePaneEdgeHorizontalCh(unsigned int i, unsigned int j);
  void printScoreString(unsigned int i, unsigned int j, string scoreMessage, unsigned int data);
  void printTailChar(unsigned int i, unsigned int j);
  void deleteTailChar(unsigned int i, unsigned int j);
  void printHeadChar(unsigned int i, unsigned int j);
  void printAppleChar(unsigned int i, unsigned int j);
};

#endif
