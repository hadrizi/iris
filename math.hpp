#ifndef IRIS_MATH_H_
#define IRIS_MATH_H_

#include <iostream>
#include <cmath>

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

template <typename T>
struct Vector3 {
    T x, y, z;

    Vector3(): x((T)0), y((T)0), z((T)0) {};
    Vector3(T x_, T y_, T z_): x(x_), y(y_), z(z_) {};
}; // Vector3

template <typename T>
std::ostream &operator<<(std::ostream &os, Vector3<T> const &m) { 
    return os << "x:" << m.x << " y:" << m.y << " z:" << m.z;
}

typedef Vector2<int>    Vector2i;
typedef Vector2<double> Vector2f;
typedef Vector3<int>    Vector3i;
typedef Vector3<double> Vector3f;

// TODO: should be a part of the future Scene object prolly
Vector3i project_vert_orthogonally(Vector3f vert, size_t canvas_width, size_t canvas_height, size_t far_plane) {
    return Vector3i(
        canvas_width  / 2 * (vert.x + 1.),
        canvas_height / 2 * (vert.y + 1.),
        far_plane     / 2 * (vert.z + 1.)
    );
}

Vector3f rotate_vert_y(Vector3f vert, double angle) {
    return Vector3f(
         cos(angle) * vert.x + sin(angle) * vert.z,
         vert.y,
        -sin(angle) * vert.x + cos(angle) * vert.z
    );
}

Vector3f project_vert_perspective(Vector3f vert, double camera) {
    return Vector3f(
        vert.x / (1 - vert.z / camera),
        vert.y / (1 - vert.z / camera),
        vert.z / (1 - vert.z / camera)
    );
}

} // iris

#endif // IRIS_MATH_H_