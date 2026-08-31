#ifndef IRIS_MATH_H_
#define IRIS_MATH_H_

#include <iostream>
#include <cmath>

namespace iris {

template <typename T>
struct Vector2 {
    T x, y;

    Vector2();
    Vector2(T x_, T y_);
}; // Vector2

template <typename T>
std::ostream& operator<<(std::ostream &os, Vector2<T> const &m);

template <typename T>
struct Vector3 {
    T x, y, z;

    Vector3();
    Vector3(T x_, T y_, T z_);
}; // Vector3

template <typename T>
std::ostream& operator<<(std::ostream &os, Vector3<T> const &m);

typedef Vector2<int>    Vector2i;
typedef Vector2<double> Vector2f;
typedef Vector3<int>    Vector3i;
typedef Vector3<double> Vector3f;

// TODO: should be a part of the future Scene object prolly
Vector3i screen(Vector3f vert, size_t canvas_width, size_t canvas_height, size_t far_plane);
Vector2i screen2(Vector2f vert, size_t canvas_width, size_t canvas_height);
Vector2f project(Vector3f vert);
Vector3f perspective(Vector3f vert, double camera);

Vector3f rotate_vert_y(Vector3f vert, double angle);
Vector3f translate_vert_z(Vector3f vert, double dz);
Vector3f translate_vert_y(Vector3f vert, double dy);

} // iris

#endif // IRIS_MATH_H_