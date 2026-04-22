#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> //math
#include <map>
#include <iomanip> //

struct Point3D {
    double x, y, z; //point coords
    
    Point3D(double x_ = 0, double y_ = 0, double z_ = 0) : x(x_), y(y_), z(z_) {}
    
    double squaredDistanceTo(const Point3D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return dx*dx + dy*dy + dz*dz; // в целом нет нужды извлекать корень, потому что всё равно будем сравнивать только расстояния между собой
    }
};

struct Domain {
    Point3D center;
    int index;
    int idx_x, idx_y, idx_z;  // axis index
};

class VoronoiOnGrid {
private:
    std::vector<Domain> domains;
    std::vector<Point3D> sites;  // центры кристаллизации
    std::map<int, std::vector<int>> voronoi_cells;  // словарь индексов ближайших ячеек по индексу центра кристаллизации
    
public:
    void generateGrid(const Point3D& box_min, const Point3D& box_max, 
                      int nx, int ny, int nz) {
        domains.clear();
        
        double step_x = (box_max.x - box_min.x) / nx;
        double step_y = (box_max.y - box_min.y) / ny;
        double step_z = (box_max.z - box_min.z) / nz;
        
        double half_step_x = step_x / 2.0;
        double half_step_y = step_y / 2.0;
        double half_step_z = step_z / 2.0;
        
        int index = 0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    double center_x = box_min.x + i * step_x + half_step_x;
                    double center_y = box_min.y + j * step_y + half_step_y;
                    double center_z = box_min.z + k * step_z + half_step_z;
                    
                    Domain d;
                    d.center = Point3D(center_x, center_y, center_z);
                    d.index = index;
                    d.idx_x = i;
                    d.idx_y = j;
                    d.idx_z = k;
                    domains.push_back(d);
                    index++;
                }
            }
        }
        
    }
    
    void addSites(const std::vector<Point3D>& sites_list) {
        sites = sites_list;
    }
    
    void computeVoronoiDiagram() {
        voronoi_cells.clear();
        
        for (const auto& domain : domains) {
            int closest_site = 0;
            double min_dist = domain.center.squaredDistanceTo(sites[0]);
            
            for (size_t i = 1; i < sites.size(); i++) {
                double dist = domain.center.squaredDistanceTo(sites[i]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_site = i;
                }
            }
            
            voronoi_cells[closest_site].push_back(domain.index);
        }
    }
    
    void saveToCSV(const std::string& filename) {
        std::ofstream outfile(filename); //
        //header
        outfile << "domain_index,center_x,center_y,center_z,grid_x,grid_y,grid_z,closest_site_index,site_x,site_y,site_z" << std::endl;
        std::map<int, int> domain_to_site;
        for (const auto& cell : voronoi_cells) {
            for (int domain_idx : cell.second) {
                domain_to_site[domain_idx] = cell.first;
            }
        }
        
        // ->csv
        outfile << std::fixed << std::setprecision(6); //_. _ _ _ _ _ _
        for (const auto& domain : domains) {
            int site_idx = domain_to_site[domain.index];
            const Point3D& site = sites[site_idx];
            
            outfile << domain.index << ","
                    << domain.center.x << ","
                    << domain.center.y << ","
                    << domain.center.z << ","
                    << domain.idx_x << ","
                    << domain.idx_y << ","
                    << domain.idx_z << ","
                    << site_idx << ","
                    << site.x << ","
                    << site.y << ","
                    << site.z << std::endl;
        }
        outfile.close();
    }
};

int main() {
    // box
    Point3D box_min(0.0, 0.0, 0.0);
    Point3D box_max(10.0, 10.0, 10.0);
    
    // divisions (default?)
    int nx = 20; 
    int ny = 20;  
    int nz = 20; 
    
    // random?
    std::vector<Point3D> sites = {        //   z  y
        Point3D(1.0, 1.0, 1.0),//1        //   x  
        Point3D(1.0, 1.0, 9.0),//2        //       4----------8
        Point3D(1.0, 9.0, 1.0),//3        //     /          / |
        Point3D(1.0, 9.0, 9.0),//4        //   /          /   |
        Point3D(9.0, 1.0, 1.0),//5        //  2---------6     | <<-- это внутри box'а
        Point3D(9.0, 1.0, 9.0),//6        //  |    3    |     7
        Point3D(9.0, 9.0, 1.0),//7        //  |         |    /
        Point3D(9.0, 9.0, 9.0),//8        //  |         | /
                                          //  1---------5
    };
    
    VoronoiOnGrid voronoi;
    voronoi.generateGrid(box_min, box_max, nx, ny, nz);
    voronoi.addSites(sites);
    voronoi.computeVoronoiDiagram();
    voronoi.saveToCSV("voronoi_results.csv");
    
    return 0;
}
