#include "point.h"

using namespace std;

Point::Point(unsigned int _r_index = 1, unsigned int _c_index = 1):
  r_index(_r_index), c_index(_c_index){ }

Point Point::getNext(Point lowest, Point highest){
  if(r_index < lowest.r_index || r_index > highest.r_index) return Point(0,0);
  if(c_index < lowest.c_index || c_index > highest.c_index) return Point(0,0);
  if(r_index == highest.r_index && c_index == highest.c_index) return Point(0,0);

  if(c_index < highest.c_index) return Point(r_index, c_index + 1);
  else return Point(r_index + 1, lowest.c_index);
}
