#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "graphics.hpp"

struct Vector2 {
    int x, y;

    Vector2(int x_, int y_): x(x_), y(y_) {}
};

std::vector<Vector2> load_obj(const char* filename) {
    std::ifstream f(filename);
    std::string line;

    std::vector<double> x_array;
    std::vector<double> y_array;

    double l, r = 0; // min and max x values
    double b, t = 0; // min and max y values

    std::vector<Vector2> out;

    while (std::getline(f, line)) {
        if (!line.starts_with("v ")) continue;

        std::istringstream iss(line.substr(2));
        double x, y;

        iss >> x;
        iss >> y;

        x_array.push_back(x);
        y_array.push_back(y);
        
        if (x < l) l = x;
        if (x > r) r = x;

        if (y < b) b = y;
        if (y > t) t = y;
    }

    for (size_t i = 0; i < x_array.size(); i++) {
        double normalized_x = (x_array[i] - l) / (r - l);
        double normalized_y = (y_array[i] - b) / (t - b);

        out.push_back(Vector2(normalized_x, normalized_y));
    }

    return out;
}

int main(int argc, char** argv) {
    size_t width  = 3;
    size_t height = 2;
    std::vector<iris::pixel_t> pixels(width * height);
    
    iris::put_pixel(pixels, 255, 0,   0,   0, 0);
    iris::put_pixel(pixels, 0,   255, 0,   0, 1);
    iris::put_pixel(pixels, 0,   0,   255, 0, 2);
    iris::put_pixel(pixels, 255, 255, 255, 0, 3);
    iris::put_pixel(pixels, 0,   255, 255, 0, 4);
    iris::put_pixel(pixels, 0,   0,   0,   0, 5);

    iris::output_pixels_to_image(pixels, width, height, "out.ppm");
}