#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    Point operator-(const Point& other) const { return Point(x - other.x, y - other.y); }
    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    Point operator*(double t) const { return Point(x * t, y * t); }
    double cross(const Point& other) const { return x * other.y - y * other.x; }
    double dot(const Point& other) const { return x * other.x + y * other.y; }
    double len() const { return std::sqrt(x * x + y * y); }
};

struct Line {
    Point p, v;
    Line(const Point& p, const Point& v) : p(p), v(v) {}
    static Point intersect(const Line& a, const Line& b) {
        double t = (b.p - a.p).cross(b.v) / a.v.cross(b.v);
        return a.p + a.v * t;
    }
};

struct VoronoiEdge {
    Point start, end;
    int site1, site2;
    VoronoiEdge(const Point& s, const Point& e, int s1, int s2) : start(s), end(e), site1(s1), site2(s2) {}
};

Line perpendicularBisector(const Point& a, const Point& b) {
    Point mid = Point((a.x + b.x) / 2, (a.y + b.y) / 2);
    Point dir = Point(a.y - b.y, b.x - a.x);
    return Line(mid, dir);
}

bool isLeft(const Point& p, const Line& line) {
    return (p - line.p).cross(line.v) > 0;
}

double angle(const Point& p) {
    return std::atan2(p.y, p.x);
}

class VoronoiDiagram {
private:
    std::vector<Point> sites;
    std::vector<VoronoiEdge> edges;
    const double BOUNDS_MIN = 0;
    const double BOUNDS_MAX = 10;
    
public:
    VoronoiDiagram(const std::vector<Point>& points) : sites(points) {}
    
    void compute() {
        edges.clear();
        
        for (int i = 0; i < sites.size(); i++) {
            std::vector<Line> bisectors;
            std::vector<int> neighbors;
            
            for (int j = 0; j < sites.size(); j++) {
                if (i == j) continue;
                bisectors.push_back(perpendicularBisector(sites[i], sites[j]));
                neighbors.push_back(j);
            }
            
            std::vector<Point> vertices;
            std::vector<int> vertexFromNeighbor;
            
            for (int j = 0; j < bisectors.size(); j++) {
                for (int k = j + 1; k < bisectors.size(); k++) {
                    try {
                        Point vertex = Line::intersect(bisectors[j], bisectors[k]);
                        
                        bool valid = true;
                        for (int l = 0; l < bisectors.size(); l++) {
                            if (l != j && l != k) {
                                if (!isLeft(vertex, bisectors[l])) {
                                    valid = false;
                                    break;
                                }
                            }
                        }
                        
                        if (valid) {
                            vertices.push_back(vertex);
                            vertexFromNeighbor.push_back(j);
                        }
                    } catch (...) {}
                }
            }
            
            if (vertices.size() >= 3) {
                Point center = sites[i];
                std::sort(vertices.begin(), vertices.end(), 
                    [&center](const Point& a, const Point& b) {
                        return angle(a - center) < angle(b - center);
                    });
                
                for (int j = 0; j < vertices.size(); j++) {
                    int next = (j + 1) % vertices.size();
                    edges.emplace_back(vertices[j], vertices[next], i, neighbors[vertexFromNeighbor[j]]);
                }
            }
        }
    }
    
    void print() const {
        std::cout << "Voronoi Diagram Edges:\n";
        std::cout << "======================\n\n";
        
        for (int i = 0; i < edges.size(); i++) {
            const auto& e = edges[i];
            std::cout << "Edge " << i + 1 << ":\n";
            std::cout << "  Between sites " << char('A' + e.site1) << " and " << char('A' + e.site2) << "\n";
            std::cout << "  From (" << e.start.x << ", " << e.start.y << ") ";
            std::cout << "to (" << e.end.x << ", " << e.end.y << ")\n\n";
        }
    }
};

int main() {
    std::vector<Point> points = {
        Point(1.2, 8.5),
        Point(3.7, 2.1),
        Point(5.9, 9.3),
        Point(7.4, 4.6),
        Point(2.8, 5.0),
        Point(9.1, 1.8),
        Point(4.3, 7.2),
        Point(0.9, 3.4)
    };
    
    VoronoiDiagram voronoi(points);
    voronoi.compute();
    voronoi.print();
    
    return 0;
}
