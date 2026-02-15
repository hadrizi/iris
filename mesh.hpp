#ifndef IRIS_MESH_H_
#define IRIS_MESH_H_

#include <vector>
#include <tuple>
#include <fstream>
#include <string>
#include <sstream>

#include "math.hpp"

namespace iris {

struct Mesh {
    std::vector<iris::Vector3f> vertices;
    std::vector<std::tuple<size_t, size_t, size_t>> faces;

    void load_from_obj(const char* filename) {
        vertices.clear();
        faces.clear();
        
        std::ifstream f(filename);
        std::string line;

        while (std::getline(f, line)) {
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
            }

            if (line.starts_with("f ")) {
                size_t v0, v1, v2;
                iss >> v0 >> v1 >> v2;

                faces.push_back({v0, v1, v2});
            }
            
        }
    }

    void normalize() {
        for (iris::Vector3f& v: vertices) {
            v.x = (l < -1) || (r > 1) ? ((2 * v.x) / (r - l)) - ((r + l) / (r - l)) : v.x;
            v.y = (b < -1) || (t > 1) ? ((2 * v.y) / (t - b)) - ((t + b) / (t - b)) : v.y;
        }
    }
private:
    // mesh bounding box
    double l = 1000., r = 0.;
    double b = 1000., t = 0.;
}; // Mesh

} // iris

#endif // IRIS_MESH_H_