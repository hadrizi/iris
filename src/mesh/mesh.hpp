#ifndef IRIS_MESH_H_
#define IRIS_MESH_H_

#include <vector>
#include <tuple>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "../math/math.hpp"

namespace iris {

typedef std::tuple<size_t, size_t, size_t> face_t;

struct Mesh {
    std::vector<iris::Vector3f> vertices;
    std::vector<face_t> faces;

    Mesh();

    void load_from_obj(const char* filename);

    void transform_to_ndc();
    
    const iris::Vector3f& get_face_vert(face_t face, size_t idx) const;
private:
    // mesh bounding box
    double l = 1000., r = 0.;
    double b = 1000., t = 0.;
    double c = 1000., f = 0.;

    void _sort_faces();
}; // Mesh

} // iris

#endif // IRIS_MESH_H_