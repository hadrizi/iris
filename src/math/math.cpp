#include "math.hpp"

template <typename T> iris::Vector2<T>::Vector2(): x((T)0), y((T)0) {};
template <typename T> iris::Vector2<T>::Vector2(T x_, T y_): x(x_), y(y_) {};

template <typename T>
std::ostream& iris::operator<<(std::ostream &os, Vector2<T> const &m) { 
    return os << "x:" << m.x << " y:" << m.y;
}

template <typename T> iris::Vector3<T>::Vector3(): x((T)0), y((T)0), z((T)0) {};
template <typename T> iris::Vector3<T>::Vector3(T x_, T y_, T z_): x(x_), y(y_), z(z_) {};

template <typename T>
std::ostream& iris::operator<<(std::ostream &os, Vector3<T> const &m) { 
    return os << "x:" << m.x << " y:" << m.y << " z:" << m.z;
}

// TODO: should be a part of the future Scene object prolly
iris::Vector3i iris::screen(Vector3f vert, size_t canvas_width, size_t canvas_height, size_t far_plane) {
    return Vector3i(
        (vert.x + 1.) / 2 * canvas_width,
        (1. - vert.y) / 2 * canvas_height,
        (vert.z + 1.) / 2 * far_plane
    );
}

iris::Vector2i iris::screen2(Vector2f vert, size_t canvas_width, size_t canvas_height) {
    return Vector2i(
        (vert.x + 1.) / 2 * canvas_width,
        (1. - vert.y) / 2 * canvas_height
    );
}

iris::Vector2f iris::project(Vector3f vert) {
    return Vector2f(
        vert.x / vert.z,
        vert.y / vert.z
    );
}

iris::Vector3f iris::rotate_vert_y(Vector3f vert, double angle) {
    double s = sin(angle);
    double c = cos(angle);
    return Vector3f(
        c * vert.x + s * vert.z,
        vert.y,
        s * vert.x - c * vert.z
    );
}

iris::Vector3f iris::translate_vert_z(Vector3f vert, double dz) {
    return Vector3f(
        vert.x,
        vert.y,
        vert.z + dz
    );
}

iris::Vector3f iris::translate_vert_y(Vector3f vert, double dy) {
    return Vector3f(
        vert.x,
        vert.y + dy,
        vert.z
    );
}


iris::Vector3f iris::perspective(Vector3f vert, double camera) {
    return Vector3f(
        vert.x / (1 - vert.z / camera),
        vert.y / (1 - vert.z / camera),
        vert.z / (1 - vert.z / camera)
    );
}

// template bullshit

template struct iris::Vector2<int>;
template struct iris::Vector2<double>;
template struct iris::Vector3<int>;
template struct iris::Vector3<double>;

template std::ostream& iris::operator<< <int>(std::ostream&, iris::Vector2<int> const&);
template std::ostream& iris::operator<< <double>(std::ostream&, iris::Vector2<double> const&);
template std::ostream& iris::operator<< <int>(std::ostream&, iris::Vector3<int> const&);
template std::ostream& iris::operator<< <double>(std::ostream&, iris::Vector3<double> const&);

// ===================