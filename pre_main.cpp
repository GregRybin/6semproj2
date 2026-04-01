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
    
    Point clipToBounds(const Point& p, const Point& dir, bool& success) {
        success = true;
        double t_min = -std::numeric_limits<double>::infinity();
        double t_max = std::numeric_limits<double>::infinity();
        
        if (dir.x > 0) {
            t_min = std::max(t_min, (BOUNDS_MIN - p.x) / dir.x);
            t_max = std::min(t_max, (BOUNDS_MAX - p.x) / dir.x);
        } else if (dir.x < 0) {
            t_min = std::max(t_min, (BOUNDS_MAX - p.x) / dir.x);
            t_max = std::min(t_max, (BOUNDS_MIN - p.x) / dir.x);
        } else {
            if (p.x < BOUNDS_MIN || p.x > BOUNDS_MAX) success = false;
        }
        
        if (dir.y > 0) {
            t_min = std::max(t_min, (BOUNDS_MIN - p.y) / dir.y);
            t_max = std::min(t_max, (BOUNDS_MAX - p.y) / dir.y);
        } else if (dir.y < 0) {
            t_min = std::max(t_min, (BOUNDS_MAX - p.y) / dir.y);
            t_max = std::min(t_max, (BOUNDS_MIN - p.y) / dir.y);
        } else {
            if (p.y < BOUNDS_MIN || p.y > BOUNDS_MAX) success = false;
        }
        
        if (t_min >= t_max || !std::isfinite(t_min) || !std::isfinite(t_max)) {
            success = false;
            return Point(0, 0);
        }
        
        return p + dir * t_min;
    }
    
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
        
        clipEdges();
        removeDuplicateEdges();
    }
    
    void clipEdges() {
        std::vector<VoronoiEdge> clippedEdges;
        
        for (const auto& edge : edges) {
            Point start = edge.start;
            Point end = edge.end;
            Point dir = end - start;
            
            bool startIn = (start.x >= BOUNDS_MIN && start.x <= BOUNDS_MAX &&
                           start.y >= BOUNDS_MIN && start.y <= BOUNDS_MAX);
            bool endIn = (end.x >= BOUNDS_MIN && end.x <= BOUNDS_MAX &&
                         end.y >= BOUNDS_MIN && end.y <= BOUNDS_MAX);
            
            if (startIn && endIn) {
                clippedEdges.push_back(edge);
            } else if (startIn || endIn) {
                bool success;
                Point newPoint;
                
                if (startIn) {
                    newPoint = clipToBounds(start, dir, success);
                    if (success) {
                        clippedEdges.emplace_back(start, newPoint, edge.site1, edge.site2);
                    }
                } else if (endIn) {
                    newPoint = clipToBounds(end, dir * (-1), success);
                    if (success) {
                        clippedEdges.emplace_back(newPoint, end, edge.site1, edge.site2);
                    }
                }
            }
        }
        
        edges = clippedEdges;
    }
    
    void removeDuplicateEdges() {
        std::vector<VoronoiEdge> uniqueEdges;
        
        for (int i = 0; i < edges.size(); i++) {
            bool duplicate = false;
            for (int j = i + 1; j < edges.size(); j++) {
                if ((edges[i].site1 == edges[j].site1 && edges[i].site2 == edges[j].site2) ||
                    (edges[i].site1 == edges[j].site2 && edges[i].site2 == edges[j].site1)) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                uniqueEdges.push_back(edges[i]);
            }
        }
        
        edges = uniqueEdges;
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
    
    void printSVG(const std::string& filename) const {
        FILE* f = fopen(filename.c_str(), "w");
        if (!f) return;
        
        fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"600\" height=\"600\">\n");
        fprintf(f, "<rect width=\"600\" height=\"600\" fill=\"white\" stroke=\"black\"/>\n");
        
        auto transform = [](double x, double y) -> std::pair<double, double> {
            return {x * 60, 600 - y * 60};
        };
        
        fprintf(f, "<g stroke=\"blue\" stroke-width=\"1.5\">\n");
        for (const auto& e : edges) {
            auto [x1, y1] = transform(e.start.x, e.start.y);
            auto [x2, y2] = transform(e.end.x, e.end.y);
            fprintf(f, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"blue\"/>\n",
                   x1, y1, x2, y2);
        }
        fprintf(f, "</g>\n");
        
        fprintf(f, "<g fill=\"red\" stroke=\"black\" stroke-width=\"1\">\n");
        char labels[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        for (int i = 0; i < sites.size(); i++) {
            auto [x, y] = transform(sites[i].x, sites[i].y);
            fprintf(f, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"4\" fill=\"red\"/>\n", x, y);
            fprintf(f, "<text x=\"%.2f\" y=\"%.2f\" font-size=\"12\" fill=\"black\">%c</text>\n",
                   x + 5, y - 5, labels[i]);
        }
        fprintf(f, "</g>\n");
        
        fprintf(f, "</svg>\n");
        fclose(f);
        
        std::cout << "SVG saved to " << filename << std::endl;
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
    voronoi.printSVG("voronoi_diagram.svg");
    
    return 0;
}
