#include "database.h"
#include "printer.h"
#include "point.h"
#include "board.h"
#include "snake.h"
#include <thread>
#include <iostream>

using namespace std;

/*********************************** MAIN *************************************/

int main(int argc, char** argv){
  //initializing screen
  Printer printer;
  printer.initialize();

  try{
    //initializing data structures
    Database dbConnection;
    dbConnection.initialize();
    Board board(printer.getLines(), printer.getCols());
    board.initialize();
    Snake snake(board.getLowestPoint(), board.getHighestPoint());
    snake.initialize();

    //playing
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
    thread_uinput.join();
    printer.close();

  }catch(string msg){
    printer.close();
    cout << msg << endl;
    exit(-1);
  };
}
