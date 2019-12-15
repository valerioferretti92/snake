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
#define PAUSEPLAY 'r'

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
  paused.store(false);
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

//function that implements one snake move
map<unsigned int, score_entry>& Snake::move(){
  unique_lock<mutex> ulock(mmutex);
  mcvariable.wait(ulock, [this](){ return !paused.load(); });
  ulock.unlock();

  Point nhead = computeNewHead();
  delay();
  switch(computeMatchStatus(nhead)){
    case WON: win(); break;
    case LOST: loose(); break;
    case ONGOING: update(nhead); break;
  }
  return scoreData;
}

//function that read input from user to control the snake
void Snake::getUserInput(){
  static char direction = SNAKE_INIT_DIR;

  while(1){
    char userInput = getch();
    //whitelist of allowed char
    list<char> wlist = {UP, DOWN, LEFT, RIGHT, QUIT, PAUSEPLAY};
    list<char>::iterator it = find(wlist.begin(), wlist.end(), userInput);
    if(it == wlist.end()) continue;
    //skipping not allowed combination
    if(userInput == direction) continue;
    if(direction == UP && userInput == DOWN) continue;
    if(direction == RIGHT && userInput == LEFT) continue;
    if(direction == DOWN && userInput == UP) continue;
    if(direction == LEFT && userInput == RIGHT) continue;
    //termination char if game was not paused
    if(userInput == QUIT && !paused.load()) break;
    //termination char if game was paused
    if(userInput == QUIT && paused.load()){
      pauseStopTimes.push_front(std::chrono::system_clock::now());
      paused.store(false);
      mcvariable.notify_all();
      break;
    }
    //pause char
    if(userInput == PAUSEPLAY && !paused.load()){
      pauseStartTimes.push_front(std::chrono::system_clock::now());
      paused.store(true);
      mcvariable.notify_all();
      continue;
    }
    //play char
    if(userInput == PAUSEPLAY && paused.load()){
      pauseStopTimes.push_front(std::chrono::system_clock::now());
      paused.store(false);
      mcvariable.notify_all();
      continue;
    }
    //storing next move
    nextDirs.push(userInput);
    direction = userInput;
  }

  gameOver.store(true);
}

//true if the game has been lost or won, false otherwise
bool Snake::getIsGameOver(){
  return gameOver.load();
}

//true if the game has been won, false otherwise
bool Snake::getHasWon(){
  return hasWon.load();
}

//return a snapshot of snake data
match_data Snake::getMatchData(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2){
  double area;
  double completion;
  unsigned int durationMs;
  unsigned int msPerApple;

  //completion percentage
  area = (highest.r_index + 1) * (double) (highest.c_index + 1);
  completion = snake.size() / (double) area;
  //duration
  durationMs = getPlayTime(t1, t2);
  //avergae ms per apple
  if(snake.size() == 10) msPerApple = 0;
  else msPerApple = durationMs / (snake.size() - 10);
  return {
    matchId,
    highest.r_index + 1,
    highest.c_index + 1,
    snake.size(),
    completion,
    sleepTimeMs,
    msPerApple,
    gameOver.load(),
    hasWon.load(),
    durationMs,
    getTimestamp()
  };
}

/****************************** PRIVATE METHODS ******************************/

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
    sleepTimeMs = sleepTimeMs > 40 ? sleepTimeMs - 1 : sleepTimeMs;
    updateScore(true);
  } else {
    snake.pop_back();
    appleCandidates.insert(tail);
    printer.deleteTailChar(tail.r_index, tail.c_index);
    updateScore(false);
  }
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

MatchStatus Snake::computeMatchStatus(Point nhead){
  //won
  if(snake.size() == (scoreData[ROWS_KEY].value * scoreData[COLUMNS_KEY].value)){
    return WON;
  }
  //lost
  list<Point>::iterator it;
  it = std::find(snake.begin(), snake.end(), nhead);
  if(it != snake.end()) return LOST;
  if(nhead.r_index < lowest.r_index || nhead.r_index > highest.r_index){
    return LOST;
  }
  if(nhead.c_index < lowest.c_index || nhead.c_index > highest.c_index){
    return LOST;
  }
  //ongoing
  return ONGOING;
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

void Snake::initializeScoreMap(){
  scoreData[FRAMES_KEY] = {"FRAMES: ", 0};
  scoreData[ROWS_KEY] = {"ROWS: ", 0};
  scoreData[COLUMNS_KEY] = {"COLUMNS: ", 0};
  scoreData[APPLES_KEY] = {"APPLES: ", 0};
  scoreData[SCORE_KEY] = {"SCORE: ", 0};
}

unsigned int Snake::getPlayTime(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2){
  string errorMessage = "Playtime calculation failed!";
  unsigned int totalPauseDurationMs = 0;
  unsigned int totalDurationMs;
  unsigned int pauseNumber = 0;

  //computing total pause duration in ms
  if(pauseStopTimes.size() != pauseStartTimes.size()) throw errorMessage;
  pauseNumber = pauseStopTimes.size();
  for(unsigned int i = 0; i < pauseNumber; i++){
    chrono::system_clock::time_point t1 = pauseStartTimes.front();
    chrono::system_clock::time_point t2 = pauseStopTimes.front();
    unsigned int pauseDurationMs = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
    totalPauseDurationMs = totalPauseDurationMs + pauseDurationMs;
    pauseStartTimes.pop_front();
    pauseStopTimes.pop_front();
  }
  //computing total duration in ms
  totalDurationMs = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
  //returning total play time
  return totalDurationMs - totalPauseDurationMs;
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
