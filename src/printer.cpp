#include "printer.h"
#include "ncurses.h"

#define SNAKE_EDGE 1
#define SNAKE_TAIL 2
#define SNAKE_HEAD 3
#define APPLE 4
#define SCORE_EDGE 5
#define SCORE_STRING 6
#define WIN 7
#define LOOSE 8

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define QUIT 'q'
#define EDGE_CH '#'
#define APPLE_CH '@'
#define SNAKE_HEAD_CH 'O'
#define SNAKE_TAIL_CH 'X'
#define SNAKE_BLANK_CH ' '
#define SCORE_PANE_CH_H '-'
#define SCORE_PANE_CH_V '|'

using namespace std;

void Printer::initialize(){
  //determines the terminal type, including its size and features, and sets up the curses environment
  initscr();
  //disables line buffering and sets curses to take one character at a time
  cbreak();
  //tells curses not to echo the input back to the screen
  noecho();
  //hide cursor
  curs_set(0);
  //colors set up
  start_color();
  init_pair(SNAKE_EDGE, COLOR_RED, COLOR_BLACK);
  init_pair(SNAKE_TAIL, COLOR_CYAN, COLOR_BLACK);
  init_pair(SNAKE_HEAD, COLOR_BLUE, COLOR_BLACK);
  init_pair(APPLE, COLOR_GREEN, COLOR_BLACK);
  init_pair(SCORE_EDGE, COLOR_WHITE, COLOR_BLACK);
  init_pair(SCORE_STRING, COLOR_WHITE, COLOR_BLACK);
  init_pair(WIN, COLOR_GREEN, COLOR_BLACK);
  init_pair(LOOSE, COLOR_YELLOW, COLOR_BLACK);
  assume_default_colors(COLOR_WHITE, COLOR_BLACK);
  //clear the screen
  clear();
}

void Printer::close(){
  endwin();
}

void Printer::flush(){
  refresh();
}

unsigned int Printer::getLines(){
  return LINES;
}

unsigned int Printer::getCols(){
  return COLS;
}

void Printer::printWinMessage(unsigned int i, unsigned int j, vector<string> winMessage){
  attron(COLOR_PAIR(WIN));
  attron(A_BOLD);
  for(int strIndex = 0; strIndex < winMessage.size(); strIndex++)
    mvprintw(i++, j, winMessage.at(strIndex).c_str());
  attroff(A_BOLD);
  attroff(COLOR_PAIR(WIN));
}

void Printer::printLooseMessage(unsigned int i, unsigned int j, vector<string> looseMessage){
  attron(COLOR_PAIR(LOOSE));
  attron(A_BOLD);
  for(int strIndex = 0; strIndex < looseMessage.size(); strIndex++)
    mvprintw(i++, j, looseMessage.at(strIndex).c_str());
  attroff(A_BOLD);
  attroff(COLOR_PAIR(LOOSE));
}

void Printer::printSnakePaneEdgeCh(unsigned int i, unsigned int j){
  attron(A_BOLD);
  attron(COLOR_PAIR(SNAKE_EDGE));
  mvaddch(i, j, EDGE_CH);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(SNAKE_EDGE));
}

void Printer::printScorePaneEdgeHorizontalCh(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(SCORE_EDGE));
  mvaddch(i, j, SCORE_PANE_CH_H);
  attroff(COLOR_PAIR(SCORE_EDGE));
}

void Printer::printScorePaneEdgeVerticalCh(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(SCORE_EDGE));
  mvaddch(i, j, SCORE_PANE_CH_V);
  attroff(COLOR_PAIR(SCORE_EDGE));
}

void Printer::printScoreString(unsigned int i, unsigned int j, string scoreMessage, unsigned int data){
  attron(A_BOLD);
  attron(COLOR_PAIR(SCORE_STRING));
  mvprintw(i, j, scoreMessage.c_str(), data);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(SCORE_STRING));
}

void Printer::printTailChar(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(SNAKE_TAIL));
  mvaddch(i, j, SNAKE_TAIL_CH);
  attroff(COLOR_PAIR(SNAKE_TAIL));
}

void Printer::deleteTailChar(unsigned int i, unsigned int j){
  mvaddch(i, j, SNAKE_BLANK_CH);
}

void Printer::printHeadChar(unsigned int i, unsigned int j){
  attron(A_BOLD);
  attron(COLOR_PAIR(SNAKE_HEAD));
  mvaddch(i, j, SNAKE_HEAD_CH);
  attroff(COLOR_PAIR(SNAKE_HEAD));
  attroff(A_BOLD);
}

void Printer::printAppleChar(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(APPLE));
  mvaddch(i, j, APPLE_CH);
  attroff(COLOR_PAIR(APPLE));
}
