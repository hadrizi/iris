#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>

#include "graphics.hpp"
#include "math.hpp"

constexpr size_t width  = 800;
constexpr size_t height = 800;

// Helper color consts
constexpr uint32_t red    = 0xFF000000;
constexpr uint32_t green  = 0x00FF0000;
constexpr uint32_t blue   = 0x0000FF00;
constexpr uint32_t white  = 0xFFFFFFFF;
constexpr uint32_t black  = 0x00000000;

typedef std::tuple<
    std::vector<iris::Vector2f>,
    std::vector<std::tuple<size_t, size_t, size_t>>
>
obj_model_t;

obj_model_t load_obj(const char* filename) {
    std::ifstream f(filename);
    std::string line;

    std::vector<double> x_array;
    std::vector<double> y_array;

    double l = 1000., r = 0.; // min and max x values
    double b = 1000., t = 0.; // min and max y values

    std::vector<iris::Vector2f>                     vertices_out;
    std::vector<std::tuple<size_t, size_t, size_t>> faces_out;

    while (std::getline(f, line)) {
        if (line.size() < 2) continue;
        std::istringstream iss(line.substr(2));
        
        if (line.starts_with("v ")) {
            double x, y;
    
            iss >> x >> y;
    
            x_array.push_back(x);
            y_array.push_back(y);
            
            if (x < l) l = x;
            if (x > r) r = x;
    
            if (y < b) b = y;
            if (y > t) t = y;
        }

        if (line.starts_with("f ")) {
            size_t v0, v1, v2;
            iss >> v0 >> v1 >> v2;

            faces_out.push_back({v0, v1, v2});
        }
        
    }
    
    for (size_t i = 0; i < x_array.size(); i++) {
        double normalized_x = ((2 * x_array[i]) / (r - l)) - ((r + l) / (r - l));
        double normalized_y = ((2 * y_array[i]) / (t - b)) - ((t + b) / (t - b));

        vertices_out.push_back(iris::Vector2f(normalized_x, normalized_y));
    }

    return {vertices_out, faces_out};
}

int main() {
    iris::Canvas canvas(width, height, black);

    const auto [vertices, faces] = load_obj("assets/teapot.obj");
    
    for (const std::tuple<size_t, size_t, size_t>& face: faces) {
        iris::Vector2i v0 = iris::project_vert(
            vertices[std::get<0>(face) - 1],
            canvas.width, canvas.height
        );
        iris::Vector2i v1 = iris::project_vert(
            vertices[std::get<1>(face) - 1],
            canvas.width, canvas.height
        );
        iris::Vector2i v2 = iris::project_vert(
            vertices[std::get<2>(face) - 1],
            canvas.width, canvas.height
        );
        
        canvas.line(white, v0.x, v0.y, v1.x, v1.y);
        canvas.line(white, v1.x, v1.y, v2.x, v2.y);
        canvas.line(white, v2.x, v2.y, v0.x, v0.y);
    }

    canvas.flip_horizontally();
    iris::output_canvas_to_image(canvas, "out.ppm");
}