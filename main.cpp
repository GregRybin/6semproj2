#include <iostream>
#include <vector>

struct Point {
  double x, y, z;
  int id;
  Point(double x = 0, double y = 0, double z = 0, int id = -1) 
        : x(x), y(y), z(z), id(id) {}
}

struct Cube {
  double x1, x2, y1, y2, z1, z2;
  int ix, iy, iz;
  std::vector<int> point_ids;
    
  Cube(int x, int y, int z, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax): ix(x), iy(y), iz(z), x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax), z_min(zmin), z_max(zmax) {}

  bool containsPoint(const Point& p) const {
    return (p.x >= x_min && p.x < x_max) && (p.y >= y_min && p.y < y_max) &&(p.z >= z_min && p.z < z_max);
  }
}
