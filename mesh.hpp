#ifndef IRIS_MESH_H_
#define IRIS_MESH_H_

#include <vector>
#include <tuple>
#include <fstream>
#include <string>
#include <sstream>

#include "math.hpp"

namespace iris {

typedef std::tuple<size_t, size_t, size_t> face_t;

struct Mesh {
    std::vector<iris::Vector3f> vertices;
    std::vector<face_t> faces;

    Mesh(): vertices(0), faces(0) {}

    void load_from_obj(const char* filename) {
        vertices.clear();
        faces.clear();
        
        std::ifstream obj_f(filename);
        std::string line;

        while (std::getline(obj_f, line)) {
            if (line.size() < 2) continue;
            std::istringstream iss(line.substr(2));
            
            if (line.starts_with("v ")) {
                double x, y, z;
        
                iss >> x >> y >> z;
        
                vertices.push_back(iris::Vector3f(x, y, z));
                
                // calculate bounding box
                if (x < l) l = x;
                if (x > r) r = x;
        
                if (y < b) b = y;
                if (y > t) t = y;

                if (z > c) c = z;
                if (z < f) f = z;
            }

            if (line.starts_with("f ")) {
                size_t v0, v1, v2;
                iss >> v0 >> v1 >> v2;

                faces.push_back({v0, v1, v2});
            }    
        }
    }

    void transform_to_ndc() {
        for (iris::Vector3f& v: vertices) {
            v.x = (l < -1) || (r > 1) ? ((2 * v.x) / (r - l)) - ((r + l) / (r - l)) : v.x;
            v.y = (b < -1) || (t > 1) ? ((2 * v.y) / (t - b)) - ((t + b) / (t - b)) : v.y;
            v.z = (c < -1) || (f > 1) ? ((2 * v.z) / (f - c)) - ((f + c) / (f - c)) : v.z;
        }
    }

    const iris::Vector3f& get_face_vert(face_t face, size_t idx) const {
        switch (idx) {
            case 0: return vertices[std::get<0>(face) - 1];
            case 1: return vertices[std::get<1>(face) - 1];
            case 2: return vertices[std::get<2>(face) - 1];
            default: throw std::out_of_range("Invalid face index");
        }
    }
private:
    // mesh bounding box
    double l = 1000., r = 0.;
    double b = 1000., t = 0.;
    double c = 1000., f = 0.;

    void _sort_faces() {
        std::vector<std::pair<double, face_t>> temp;

        for (const auto& f : faces) {
            double max_z = std::max({
                vertices[std::get<0>(f) - 1].z,
                vertices[std::get<1>(f) - 1].z,
                vertices[std::get<2>(f) - 1].z
            });
            temp.push_back({max_z, f});
        }

        std::sort(temp.begin(), temp.end(),
                [](const auto& a, const auto& b) {
                    return a.first < b.first;
                });

        faces.clear();
        for (const auto& [_, f] : temp)
            faces.push_back(f);

    }
}; // Mesh

} // iris

#endif // IRIS_MESH_H_