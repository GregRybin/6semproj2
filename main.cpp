#include <iostream>

struct Point {
  double x, y, z;
  int id;
  Point(double x = 0, double y = 0, double z = 0, int id = -1) 
        : x(x), y(y), z(z), id(id) {}
}
