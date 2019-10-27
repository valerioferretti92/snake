#include <curses.h>
#include <sqlite3.h>
#include <math.h>
#include <algorithm>
#include <set>
#include <list>
#include <queue>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <iostream>

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

#define APPLE_INIT_X 3
#define APPLE_INIT_Y 20

#define INIT_SNAKE_SIZE 10
#define SNAKE_INIT_X 3
#define SNAKE_INIT_Y 3
#define SNAKE_INIT_DIR RIGHT
#define SNAKE_MAX_SLEEPTIME_MS 100
#define SNAKE_MIN_SLEEPTIME_MS 50

#define SNAKE_PANE_MIN_WIDTH 30
#define SCORE_PANE_WIDTH 30
#define SCORE_LINES 5
#define SCORE_LINE_HEIGHT 4
#define BOARD_MIN_HEIGHT SCORE_LINES * SCORE_LINE_HEIGHT + 1

#define SNAKE_EDGE 1
#define SNAKE_TAIL 2
#define SNAKE_HEAD 3
#define APPLE 4
#define SCORE_EDGE 5
#define SCORE_STRING 6
#define WIN 7
#define LOOSE 8

#define FRAMES_KEY 0
#define ROWS_KEY 1
#define COLUMNS_KEY 2
#define APPLES_KEY 3
#define SCORE_KEY 4

using namespace std;

struct score_entry{
  string text;
  unsigned int value;
};

score_entry getScoreEntry(string _text, unsigned int _value){
  score_entry sentry;
  sentry.text = _text;
  sentry.value = _value;
  return sentry;
}

class Point {
public:
  unsigned int r_index;
  unsigned int c_index;

  Point(unsigned int _r_index, unsigned int _c_index);
  Point getNext(Point lowest, Point highest);
};

Point::Point(unsigned int _r_index = 1, unsigned int _c_index = 1):
  r_index(_r_index), c_index(_c_index){ }

Point Point::getNext(Point lowest, Point highest){
  if(r_index < lowest.r_index || r_index > highest.r_index) return Point(0,0);
  if(c_index < lowest.c_index || c_index > highest.c_index) return Point(0,0);
  if(r_index == highest.r_index && c_index == highest.c_index) return Point(0,0);

  if(c_index < highest.c_index) return Point(r_index, c_index + 1);
  else return Point(r_index + 1, lowest.c_index);
}

bool operator==(const Point& lhs, const Point& rhs){
  return lhs.r_index == rhs.r_index && lhs.c_index == rhs.c_index;
}

bool operator!=(const Point& lhs, const Point& rhs){
  return lhs.r_index != rhs.r_index && lhs.c_index != rhs.c_index;
}

bool operator<(const Point& lhs, const Point& rhs){
  if(lhs.r_index < rhs.r_index) return true;
  else if(lhs.r_index == rhs.r_index && lhs.c_index < rhs.c_index) return true;
  else return false;
}

//LINES and COLS to be persisted. All others are derived from them.
class Board {
private:
  unsigned int matchId;
  unsigned int rows;
  unsigned int snakePaneWidth;
  unsigned int scorePaneWidth;
  unsigned int scoreBoardInitRowIndex;
  unsigned int scoreBoardInitColumnIndex;
  unsigned int gridSize;

  vector<string> looseMessage = {
    "                            ","    **  **  ****  **  **    ","     ****  **  ** **  **    ",
    "      **   **  ** **  **    ","      **   **  ** **  **    ","      **    ****   ****     ",
    "                            "," **     ****    **** ****** "," **    **  **  **      **   ",
    " **    **  **   **     **   "," **    **  **    **    **   ","  ****  ****  ****     **   ",
    "                            "};
  const unsigned int looseMessageColumns = 28;

  vector<string> winMessage = {
    "                         "," **   **  *****  **   ** ","  ** **  **   ** **   ** ",
    "   ***   **   ** **   ** ","   ***   **   ** **   ** ","   ***    *****   *****  ",
    "                         "," **   **  *****  **   ** "," **   ** **   ** ***  ** ",
    " ** * ** **   ** ** * ** "," *** *** **   ** **  *** "," **   **  *****  **   ** ",
    "                         "};
  const unsigned int winMessageColumns = 26;

  void printSnakePaneEdgeCh(unsigned int i, unsigned int j);
  void printScorePaneEdgeCh(unsigned int i, unsigned int j, char character);
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
      printScorePaneEdgeCh(scoreBoardInitRowIndex + i, scoreBoardInitColumnIndex, SCORE_PANE_CH_V);
      printScorePaneEdgeCh(scoreBoardInitRowIndex + i, snakePaneWidth + scorePaneWidth - 2, SCORE_PANE_CH_V);
    } else for(int j = 1; j < scorePaneWidth - 1; j++){
      printScorePaneEdgeCh(scoreBoardInitRowIndex + i, snakePaneWidth + j, SCORE_PANE_CH_H);
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

  if(hasWon) attron(COLOR_PAIR(WIN)); else attron(COLOR_PAIR(LOOSE));
  attron(A_BOLD);
  for(int i = 0; i < messageRows; i++)
    mvprintw(initRowIndex++, initColumnIndex, hasWon ? winMessage.at(i).c_str() : looseMessage.at(i).c_str());
  if(hasWon) attroff(COLOR_PAIR(WIN)); else attroff(COLOR_PAIR(LOOSE));
  attroff(A_BOLD);
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
  attron(A_BOLD);
  attron(COLOR_PAIR(SNAKE_EDGE));
  mvaddch(i, j, EDGE_CH);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(SNAKE_EDGE));
}

void Board::printScorePaneEdgeCh(unsigned int i, unsigned int j, char character){
  attron(COLOR_PAIR(SCORE_EDGE));
  mvaddch(i, j, character);
  attroff(COLOR_PAIR(SCORE_EDGE));
}

void Board::printScoreString(unsigned int i, unsigned int j1, unsigned int j2, string text, unsigned int data){
  unsigned int initColumn;
  ostringstream ss;
  string scoreMessage;
  ss << text << data;
  scoreMessage = ss.str();
  initColumn = (j2 - j1 - scoreMessage.size()) / 2 + j1;

  attron(A_BOLD);
  attron(COLOR_PAIR(SCORE_STRING));
  mvprintw(i, initColumn, scoreMessage.c_str(), data);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(SCORE_STRING));
}

string Board::getMinimumWidthError(unsigned int actualRows, unsigned int minimumRows){
  return "Terminal's windows width is " + to_string(actualRows) + ", " + to_string(minimumRows) + " is the lowest acceptable value.";
}

string Board::getMinimumHeightError(unsigned int actualColumns, unsigned int minimumColumns){
  return "Terminal's windows height is " + to_string(actualColumns) + ", " + to_string(minimumColumns) + " is the lowest acceptable value.";
}

class Snake {
private:
  Point lowest;
  Point highest;
  Point apple;
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

Snake::Snake(Point _lowest, Point _highest): lowest(_lowest), highest(_highest)
{
  //data initialization
  gameOver.store(false);
  hasWon.store(false);
  scoreData[ROWS_KEY].value = highest.r_index + 1;
  scoreData[COLUMNS_KEY].value = highest.c_index + 1;
  apple.r_index = APPLE_INIT_X;
  apple.c_index = APPLE_INIT_Y;
  sleepTimeMs = SNAKE_MAX_SLEEPTIME_MS;
  srand(time(NULL));

  //snake initial position and length.
  //snake's head is going to be the first element in the list
  for(int i = 0; i < INIT_SNAKE_SIZE; i++){
    Point sn_point;
    sn_point.r_index = SNAKE_INIT_X;
    sn_point.c_index = SNAKE_INIT_Y + i;
    snake.push_front(sn_point);
  }

  //initialize appleCandidates
  Point point = lowest;
  list<Point>::iterator it;
  while(point != Point(0,0)){
    it = std::find(snake.begin(), snake.end(), point);
    if(it == snake.end()) appleCandidates.insert(point);
    point = point.getNext(lowest, highest);
  }
}

void Snake::initialize(){
  //print snake
  list<Point>::iterator it;
  for(it = snake.begin(); it != snake.end(); ++it){
    if(distance(snake.begin(), it) == 0) printHeadChar(it->r_index, it->c_index);
    else printTailChar(it->r_index, it->c_index);
  }

  //print apple char
  printAppleChar(apple.r_index, apple.c_index);
}

void Snake::getUserInput(){
  static char direction = SNAKE_INIT_DIR;

  do{
    char nextDir = getch();
    if(nextDir != UP && nextDir != DOWN && nextDir != LEFT && nextDir != RIGHT && nextDir != QUIT) continue;
    if(nextDir == direction) continue;
    if(direction == UP && nextDir == DOWN) continue;
    if(direction == RIGHT && nextDir == LEFT) continue;
    if(direction == DOWN && nextDir == UP) continue;
    if(direction == LEFT && nextDir == RIGHT) continue;
    if(nextDir == QUIT) break;
    nextDirs.push(nextDir);
    direction = nextDir;
  }while(1);

  gameOver.store(true);
}

Point Snake::computeNewHead(){
  static char nextDir = SNAKE_INIT_DIR;
  if(nextDirs.size() != 0){
    nextDir = nextDirs.front();
    nextDirs.pop();
  }

  //calculate new snake positions
  Point nhead = snake.front();
  if(nextDir == UP) nhead.r_index--;
  else if(nextDir == DOWN) nhead.r_index++;
  else if(nextDir == LEFT) nhead.c_index--;
  else if(nextDir == RIGHT) nhead.c_index++;
  return nhead;
}

Point Snake::computeNewApple(){
  set<Point>::iterator it = appleCandidates.begin();
  advance(it, rand() % appleCandidates.size());
  return *it;
}

void Snake::update(Point nhead){
  Point ohead = snake.front();
  Point tail = snake.back();

  snake.push_front(nhead);
  appleCandidates.erase(nhead);
  printTailChar(ohead.r_index, ohead.c_index);
  printHeadChar(nhead.r_index, nhead.c_index);

  if(nhead == apple){
    apple = computeNewApple();
    printAppleChar(apple.r_index, apple.c_index);
    sleepTimeMs = sleepTimeMs - 1;
    updateScore(true);
  } else  {
    snake.pop_back();
    appleCandidates.insert(tail);
    mvaddch(tail.r_index, tail.c_index, SNAKE_BLANK_CH);
    updateScore(false);
  }
}

bool Snake::getIsFeasible(Point nhead){
  list<Point>::iterator it;
  it = std::find(snake.begin(), snake.end(), nhead);
  if(it != snake.end()) return false;
  if(nhead.r_index < lowest.r_index || nhead.r_index > highest.r_index) return false;
  if(nhead.c_index < lowest.c_index || nhead.c_index > highest.c_index) return false;
  return true;
}

bool Snake::getIsGameOver(){
  return gameOver.load();
}

bool Snake::getHasWon(){
  return snake.size() == (scoreData[ROWS_KEY].value * scoreData[COLUMNS_KEY].value);
}

map<unsigned int, score_entry>& Snake::getScore(){
  return scoreData;
}

void Snake::win(){
  gameOver.store(true);
  hasWon.store(true);
}

void Snake::loose(){
  gameOver.store(true);
  hasWon.store(false);
}

void Snake::delay(){
  this_thread::sleep_for(chrono::milliseconds(sleepTimeMs));
}

void Snake::updateScore(bool hasEaten){
  int gridSize = 0;

  scoreData[FRAMES_KEY].value++;
  if(hasEaten){
    scoreData[APPLES_KEY].value++;
    gridSize = scoreData[ROWS_KEY].value * scoreData[COLUMNS_KEY].value;
    scoreData[SCORE_KEY].value = floor(((float)snake.size() / (float)gridSize) * 10000);
  }
}

void Snake::printTailChar(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(SNAKE_TAIL));
  mvaddch(i, j, SNAKE_TAIL_CH);
  attroff(COLOR_PAIR(SNAKE_TAIL));
}

void Snake::printHeadChar(unsigned int i, unsigned int j){
  attron(A_BOLD);
  attron(COLOR_PAIR(SNAKE_HEAD));
  mvaddch(i, j, SNAKE_HEAD_CH);
  attroff(COLOR_PAIR(SNAKE_HEAD));
  attroff(A_BOLD);
}

void Snake::printAppleChar(unsigned int i, unsigned int j){
  attron(COLOR_PAIR(APPLE));
  mvaddch(i, j, APPLE_CH);
  attroff(COLOR_PAIR(APPLE));
}

class Database {
private:
  string dbFilename;
  sqlite3* dbConnection;
public:
  Database(string dbFilename);
  ~Database();
};

Database::Database(string dbFilename) : dbFilename(dbFilename){
  int sqlite_rcode;

  sqlite3_initialize();
  sqlite_rcode = sqlite3_open_v2(dbFilename.c_str(), &dbConnection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if(sqlite_rcode != SQLITE_OK) throw "Could not open db!";
}

Database::~Database(){
  int sqlite_rcode, attempt = 0;
  string closingAttempt = "Failed to close db! Trying again in 1 second...";
  string forcedClosing = "Forcibly closing db!";
  string msg;

  while(attempt <= 3){
    sqlite_rcode = sqlite3_close(dbConnection);
    if(sqlite_rcode == SQLITE_OK) break;
    msg = attempt == 3 ? forcedClosing : closingAttempt;
    cout << msg << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));
    attempt++;
  }
  sqlite3_shutdown();
}

/************************ INIT / SHUTDOWN FUNCTIONS ***************************/

void ncurses_initialize(){
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

/*********************************** MAIN *************************************/

int main(int argc, char** argv){
  Point nhead;

  //initializing screen
  ncurses_initialize();

  try{

    //initializing data structures
    Database dbConnection("../db/sqlite_snake.db");
    Board board(LINES, COLS);
    board.initialize();
    Snake snake(board.getLowestPoint(), board.getHighestPoint());
    snake.initialize();

    //playing
    thread thread_uinput(&Snake::getUserInput, &snake);
    while(!snake.getIsGameOver()){
      nhead = snake.computeNewHead();
      snake.delay();
      if(snake.getHasWon()) snake.win();
      else if(!snake.getIsFeasible(nhead)) snake.loose();
      else {
        snake.update(nhead);
        board.updateScore(snake.getScore());
      }
      if(snake.getIsGameOver()) board.printMessage(snake.getHasWon());
      refresh();
    }
    thread_uinput.join();
    endwin();

  }catch(string msg){
    endwin();
    cout << msg << endl;
    exit(-1);
  };
}
