#ifndef IRIS_MATH_H_
#define IRIS_MATH_H_

#include <iostream>

namespace iris {

template <typename T>
struct Vector2 {
    T x, y;

    Vector2(): x((T)0), y((T)0) {};
    Vector2(T x_, T y_): x(x_), y(y_) {};
}; // Vector2

template <typename T>
std::ostream &operator<<(std::ostream &os, Vector2<T> const &m) { 
    return os << "x:" << m.x << " y:" << m.y;
}

typedef Vector2<int>    Vector2i;
typedef Vector2<double> Vector2f;

Vector2i project_vert(Vector2f vert, size_t canvas_width, size_t canvas_height) {
    return Vector2i(
        canvas_width  / 2 * (vert.x + 1.),
        canvas_height / 2 * (vert.y + 1.)
    );
}

} // iris

#endif // IRIS_MATH_H_