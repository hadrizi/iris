#ifndef IRIS_MATH_H_
#define IRIS_MATH_H_

#include <iostream>

namespace iris {

struct Vector2 {
    double x, y;

    Vector2(double x_, double y_): x(x_), y(y_) {}
}; // Vector2

std::ostream &operator<<(std::ostream &os, Vector2 const &m) { 
    return os << "x:" << m.x << " y:" << m.y;
}

} // iris

#endif // IRIS_MATH_H_