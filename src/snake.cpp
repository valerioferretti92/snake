#include "snake.h"
#include "utils.h"
#include <math.h>
#include <iomanip>
#include <ncurses.h>
#include <algorithm>

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define QUIT 'q'

#define FRAMES_KEY 0
#define ROWS_KEY 1
#define COLUMNS_KEY 2
#define APPLES_KEY 3
#define SCORE_KEY 4

#define APPLE_INIT_X 3
#define APPLE_INIT_Y 20

#define INIT_SNAKE_SIZE 10
#define SNAKE_INIT_X 3
#define SNAKE_INIT_Y 3
#define SNAKE_INIT_DIR RIGHT
#define SNAKE_MAX_SLEEPTIME_MS 100
#define SNAKE_MIN_SLEEPTIME_MS 50

using namespace std;

Snake::Snake(Point _lowest, Point _highest): lowest(_lowest), highest(_highest)
{
  //data initialization
  matchId = generateIdentifier();
  initializeScoreMap();
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
  } else {
    snake.pop_back();
    appleCandidates.insert(tail);
    printer.deleteTailChar(tail.r_index, tail.c_index);
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

match_data Snake::getMatchData(){
  return {
    matchId,
    highest.r_index + 1,
    highest.c_index + 1,
    snake.size(),
    sleepTimeMs,
    gameOver.load(),
    hasWon.load(),
    getTimestamp()
  };
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
  printer.printTailChar(i, j);
}

void Snake::printHeadChar(unsigned int i, unsigned int j){
  printer.printHeadChar(i, j);
}

void Snake::printAppleChar(unsigned int i, unsigned int j){
  printer.printAppleChar(i, j);
}

void Snake::initializeScoreMap(){
  scoreData[FRAMES_KEY] = {"FRAMES: ", 0};
  scoreData[ROWS_KEY] = {"ROWS: ", 0};
  scoreData[COLUMNS_KEY] = {"COLUMNS: ", 0};
  scoreData[APPLES_KEY] = {"APPLES: ", 0};
  scoreData[SCORE_KEY] = {"SCORE: ", 0};
}
