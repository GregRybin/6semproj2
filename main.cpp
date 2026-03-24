#include <iostream>
#include <vector>

struct Point {
  double x, y, z;
  int id;
  Point(double x = 0, double y = 0, double z = 0, int id = -1) 
        : x(x), y(y), z(z), id(id) {}
};

struct Cube {
  double x_min, x_max, y_min, y_max, z_min, z_max;
  int ix, iy, iz;
  std::vector<int> point_ids;
    
  Cube(int x, int y, int z, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax): ix(x), iy(y), iz(z), x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax), z_min(zmin), z_max(zmax) {}

  bool containsPoint(const Point& p) const {
    return (p.x >= x_min && p.x < x_max) && (p.y >= y_min && p.y < y_max) &&(p.z >= z_min && p.z < z_max);
  }
};

std::vector<Cube*> getNeighborCubes(std::vector<std::vector<std::vector<Cube>>>& grid, 
                                     int ix, int iy, int iz, 
                                     int divisions_x, int divisions_y, int divisions_z) {
    std::vector<Cube*> neighbors;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                if (dx == 0 && dy == 0 && dz == 0) continue;
                int nx = ix + dx;
                int ny = iy + dy;
                int nz = iz + dz;
                if (nx >= 0 && nx < divisions_x && 
                    ny >= 0 && ny < divisions_y && 
                    nz >= 0 && nz < divisions_z) {
                    neighbors.push_back(&grid[nx][ny][nz]);
                }
            }
        }
    }
    
    return neighbors;
};

std::vector<std::vector<std::vector<Cube>>> createGrid(
    double x_min, double x_max,
    double y_min, double y_max,
    double z_min, double z_max,
    int div_x, int div_y, int div_z) {
    double cube_size_x = (x_max - x_min) / div_x;
    double cube_size_y = (y_max - y_min) / div_y;
    double cube_size_z = (z_max - z_min) / div_z;
    std::vector<std::vector<std::vector<Cube>>> grid;
    grid.resize(div_x);
    for (int i = 0; i < div_x; ++i) {
        grid[i].resize(div_y);
        double current_x_min = x_min + i * cube_size_x;
        double current_x_max = current_x_min + cube_size_x;
        for (int j = 0; j < div_y; ++j) {
            grid[i][j].resize(div_z);
            double current_y_min = y_min + j * cube_size_y;
            double current_y_max = current_y_min + cube_size_y;
            for (int k = 0; k < div_z; ++k) {
                double current_z_min = z_min + k * cube_size_z;
                double current_z_max = current_z_min + cube_size_z;
                grid[i][j][k] = Cube(i, j, k, 
                                     current_x_min, current_x_max,
                                     current_y_min, current_y_max,
                                     current_z_min, current_z_max);
            }
        }
    }
    
    return grid;
}

int main() {
  return 0;
}
