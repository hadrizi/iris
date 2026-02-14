#ifndef IRIS_GRAPHICS_H_
#define IRIS_GRAPHICS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>

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

    void line(pixel_t col, int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;

        if (dx == 0 && dy == 0) {
            put_pixel(col, x1, y1);
            return;
        }

        if (abs(dx) > abs(dy)) {
            if (x0 > x1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            for (int x = x0; x <= x1; x++) {
                int y = dy * (x - x0) / dx + y0;
                put_pixel(col, x, y);
            }
        } else {
            if (y0 > y1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            for (int y = y0; y <= y1; y++) {
                int x = dx * (y - y0) / dy + x0;
                put_pixel(col, x, y);
            }
        }
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