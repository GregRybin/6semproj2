#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
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

Line perpendicularBisector(const Point& a, const Point& b) {
    Point mid = Point((a.x + b.x) / 2, (a.y + b.y) / 2);
    Point dir = Point(a.y - b.y, b.x - a.x);
    return Line(mid, dir);
}

double angle(const Point& p) {
    return std::atan2(p.y, p.x);
}

bool isLeft(const Point& p, const Line& line) {
    return (p - line.p).cross(line.v) > 0;
}

class VoronoiDiagram {

}

int main() {
    std::vector<Point> points = {
        Point(1.2, 8.5),  // A
        Point(3.7, 2.1),  // B
        Point(5.9, 9.3),  // C
        Point(7.4, 4.6),  // D
        Point(2.8, 5.0),  // E
        Point(9.1, 1.8),  // F
        Point(4.3, 7.2),  // G
        Point(0.9, 3.4)   // H
    };
    
    std::cout << "Input points:\n";
    char labels[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    for (int i = 0; i < points.size(); i++) {
        std::cout << labels[i] << ": (" << points[i].x << ", " << points[i].y << ")\n";
    }
    std::cout << "\n";
    return 0;
}
