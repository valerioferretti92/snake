#ifndef POINT_H
#define POINT_H

class Point {
public:
  unsigned int r_index;
  unsigned int c_index;

  Point(unsigned int _r_index = 1, unsigned int _c_index = 1);
  Point getNext(Point lowest, Point highest);

  friend bool operator==(const Point& lhs, const Point& rhs){
    return lhs.r_index == rhs.r_index && lhs.c_index == rhs.c_index;
  }

  friend bool operator!=(const Point& lhs, const Point& rhs){
    return lhs.r_index != rhs.r_index && lhs.c_index != rhs.c_index;
  }

  friend bool operator<(const Point& lhs, const Point& rhs){
    if(lhs.r_index < rhs.r_index) return true;
    else if(lhs.r_index == rhs.r_index && lhs.c_index < rhs.c_index) return true;
    else return false;
  }
};

#endif
