#include "board.h"
#include <sstream>
#include <chrono>

#define SNAKE_PANE_MIN_WIDTH 30
#define SCORE_PANE_WIDTH 30
#define SCORE_LINES 5
#define SCORE_LINE_HEIGHT 4
#define BOARD_MIN_HEIGHT SCORE_LINES * SCORE_LINE_HEIGHT + 1

using namespace std;

//initializes the board with number of editable rows and columns
Board::Board(unsigned int _rows, unsigned int _columns){
  if(_rows < BOARD_MIN_HEIGHT)
    throw getMinimumHeightError(_rows, BOARD_MIN_HEIGHT);
  if(_columns < SNAKE_PANE_MIN_WIDTH + SCORE_PANE_WIDTH)
    throw getMinimumWidthError(_columns, SNAKE_PANE_MIN_WIDTH + SCORE_PANE_WIDTH);

  rows = _rows;
  snakePaneWidth = _columns - SCORE_PANE_WIDTH;
  if(rows % 2 == 1 && snakePaneWidth % 2 == 1) snakePaneWidth--;
  scorePaneWidth = _columns - snakePaneWidth;
  scoreBoardInitRowIndex = (rows - BOARD_MIN_HEIGHT) / 2 - 1;
  scoreBoardInitColumnIndex = snakePaneWidth + 1;
  gridSize = (snakePaneWidth - 2) * (rows - 2);
  using namespace chrono;
  matchId = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

//draw the board's edges
void Board::initialize(){
  //snake pane
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < snakePaneWidth; j++){
      if(i == 0 || i == rows - 1 || j == 0 || j == snakePaneWidth - 1)
        printSnakePaneEdgeCh(i, j);
    }
  }
  //score pane
  for(int i = 0; i < BOARD_MIN_HEIGHT; i++){
    if(i % 4 != 0){
      printScorePaneEdgeVerticalCh(scoreBoardInitRowIndex + i, scoreBoardInitColumnIndex);
      printScorePaneEdgeVerticalCh(scoreBoardInitRowIndex + i, snakePaneWidth + scorePaneWidth - 2);
    } else for(int j = 1; j < scorePaneWidth - 1; j++){
      printScorePaneEdgeHorizontalCh(scoreBoardInitRowIndex + i, snakePaneWidth + j);
    }
  }
}

void Board::updateScore(map<unsigned int, score_entry>& scoreData){
  int r, c1, c2;

  c1 = scoreBoardInitColumnIndex;
  c2 = scoreBoardInitColumnIndex + scorePaneWidth - 1;
  for(int i = 0; i < SCORE_LINES; i++){
    r = scoreBoardInitRowIndex + SCORE_LINE_HEIGHT / 2 + i * SCORE_LINE_HEIGHT;
    printScoreString(r, c1, c2, scoreData[i].text, scoreData[i].value);
  }
}

void Board::printMessage(bool hasWon){
  int initRowIndex, initColumnIndex, messageRows, messageColumns;

  messageRows = hasWon ? winMessage.size() : looseMessage.size();
  messageColumns = hasWon ? winMessageColumns : looseMessageColumns;
  initRowIndex = (rows - 2 - messageRows) / 2 + 1;
  initColumnIndex = (snakePaneWidth - 2 - messageColumns) / 2 + 1;

  if(hasWon) printer.printWinMessage(initRowIndex, initColumnIndex, winMessage);
  else printer.printLooseMessage(initRowIndex, initColumnIndex, looseMessage);
}

//returns the point whose sum of x and y indexes is the lowest possible on the gaming grid
//not (0,0) to take into account board's edges
Point Board::getLowestPoint(){
  return Point(1,1);
}

//returns the point whose sum of x and y indexes is the highest possible on the gaming grid
//takes into account board's edges and the fact we want to return indexes and not sizes
Point Board::getHighestPoint(){
  return Point(rows - 2, snakePaneWidth - 2);
}

void Board::printSnakePaneEdgeCh(unsigned int i, unsigned int j){
  printer.printSnakePaneEdgeCh(i, j);
}

void Board::printScorePaneEdgeVerticalCh(unsigned int i, unsigned int j){
  printer.printScorePaneEdgeVerticalCh(i, j);
}

void Board::printScorePaneEdgeHorizontalCh(unsigned int i, unsigned int j){
  printer.printScorePaneEdgeHorizontalCh(i, j);
}

void Board::printScoreString(unsigned int i, unsigned int j1, unsigned int j2, string text, unsigned int data){
  unsigned int initColumn;
  ostringstream ss;
  string scoreMessage;
  ss << text << data;
  scoreMessage = ss.str();
  initColumn = (j2 - j1 - scoreMessage.size()) / 2 + j1;

  printer.printScoreString(i, initColumn, scoreMessage, data);
}

string Board::getMinimumWidthError(unsigned int actualRows, unsigned int minimumRows){
  return "Terminal's windows width is " + to_string(actualRows) + ", " + to_string(minimumRows) + " is the lowest acceptable value.";
}

string Board::getMinimumHeightError(unsigned int actualColumns, unsigned int minimumColumns){
  return "Terminal's windows height is " + to_string(actualColumns) + ", " + to_string(minimumColumns) + " is the lowest acceptable value.";
}
