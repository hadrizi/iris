#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

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

std::vector<iris::Vector2> load_obj(const char* filename) {
    std::ifstream f(filename);
    std::string line;

    std::vector<double> x_array;
    std::vector<double> y_array;

    double l = 1000., r = 0.; // min and max x values
    double b = 1000., t = 0.; // min and max y values

    std::vector<iris::Vector2> out;

    while (std::getline(f, line)) {
        if (!line.starts_with("v ")) continue;

        std::istringstream iss(line.substr(2));
        double x, y;

        iss >> x >> y;

        x_array.push_back(x);
        y_array.push_back(y);
        
        if (x < l) l = x;
        if (x > r) r = x;

        if (y < b) b = y;
        if (y > t) t = y;
    }

    for (size_t i = 0; i < x_array.size(); i++) {
        double normalized_x = ((2 * x_array[i]) / (r - l)) - ((r + l) / (r - l));
        double normalized_y = ((2 * y_array[i]) / (t - b)) - ((t + b) / (t - b));

        out.push_back(iris::Vector2(normalized_x, normalized_y));
    }

    return out;
}

int main() {
    iris::Canvas canvas(width, height, black);

    std::vector<iris::Vector2> vertices = load_obj("assets/teapot.obj");
    for (const iris::Vector2& v: vertices) {
        size_t x = width  / 2 * (v.x + 1.);
        size_t y = height / 2 * (v.y + 1.);

        canvas.put_pixel(white, x, y);
    }

    canvas.flip_horizontally();
    iris::output_canvas_to_image(canvas, "out.ppm");
}