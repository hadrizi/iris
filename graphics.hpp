#ifndef IRIS_GRAPHICS_H_
#define IRIS_GRAPHICS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace iris {

typedef uint32_t pixel_t;

struct Canvas {
    size_t width, height;
    std::vector<pixel_t> pixels;

    Canvas()
        : width(0), height(0) {};
    Canvas(size_t width_, size_t height_)
        : width(width_), height(height_), pixels(width_ * height_) {};
    Canvas(size_t width_, size_t height_, pixel_t col)
        : width(width_), height(height_), pixels(width_ * height_, col) {};
        
    void put_pixel(pixel_t col, size_t idx) {
        pixels[idx] = col;
    }

    void put_pixel(pixel_t col, size_t x, size_t y) {
        pixels[width * y + x] = col;
    }

    void fill(pixel_t col) {
        std::fill(pixels.begin(), pixels.end(), col);
    }

    void flip_horizontally() {
        std::reverse(pixels.begin(), pixels.end());
    }
}; // Canvas

void output_canvas_to_image(Canvas canvas, const char* filename) {
    std::ofstream f;
    f.open(filename, std::ios::binary);
    f << "P6\n" << canvas.width << " " << canvas.height << "\n255\n";

    for (const pixel_t& p: canvas.pixels) {
        f << (uint8_t)((p >> 24) & 0xFF) << (uint8_t)((p >> 16) & 0xFF) << (uint8_t)((p >> 8) & 0xFF);
    }

    f.close();
}

} // iris

#endif // IRIS_GRAPHICS_H_