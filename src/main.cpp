#include "match_data.h"
#include "database.h"
#include "printer.h"
#include "point.h"
#include "board.h"
#include "snake.h"
#include "utils.h"
#include "ncurses.h"
#include <signal.h>
#include <thread>
#include <iostream>

using namespace std;

void resizeHandler(int sig){
  endwin();
  cout << "I prefer to suicide rather then to resize!" << endl;
  exit(-1);
}

int main(int argc, char** argv){
  //Registering handler for resize signal
  signal(SIGWINCH, resizeHandler);

  //initializing screen
  Printer printer;
  printer.initialize();

  try{
    //initializing data structures
    Database database;
    database.initialize();
    Board board(printer.getLines(), printer.getCols());
    board.initialize();
    Snake snake(board.getLowestPoint(), board.getHighestPoint());
    snake.initialize();

    //playing
    auto t1 = std::chrono::system_clock::now();
    thread thread_uinput(&Snake::getUserInput, &snake);
    while(!snake.getIsGameOver()){
      Point nhead = snake.computeNewHead();
      snake.delay();
      if(snake.getHasWon()) snake.win();
      else if(!snake.getIsFeasible(nhead)) snake.loose();
      else {
        snake.update(nhead);
        board.updateScore(snake.getScore());
      }
      if(snake.getIsGameOver()) board.printMessage(snake.getHasWon());
      printer.flush();
    }
    auto t2 = std::chrono::system_clock::now();
    thread_uinput.join();
    printer.close();

    //saving match data into sqlite db
    match_data matchData = snake.getMatchData();
    database.insert(matchData);

    //printing out match info as summary
    printMatchSummary(matchData);
    printTotalPlayTime(t1, t2);

  }catch(string msg){
    printer.close();
    cout << msg << endl;
    exit(-1);
  };
}
