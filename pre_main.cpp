#include <iostream>

struct Point {
    double x, y;
}

struct Line {
    Point p, v;
    
    Line(const Point& p, const Point& v) : p(p), v(v) {}
};

struct Segment {
    Point p1, p2;
    
    Segment(const Point& p1, const Point& p2) : p1(p1), p2(p2) {}
};

Line perpendicularBisector(const Point& a, const Point& b) {
    Point mid = Point((a.x + b.x) / 2, (a.y + b.y) / 2);
    Point dir = Point(a.y - b.y, b.x - a.x); 
    return Line(mid, dir);
}
