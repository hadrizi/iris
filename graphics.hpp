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

#define PIXEL_COL(r, g, b, a) ((uint32_t)(((uint8_t)r << 24) | ((uint8_t)g << 16) | ((uint8_t)b << 8) | (uint8_t)a))

// Helper color consts
constexpr pixel_t red    = 0xFF000000;
constexpr pixel_t green  = 0x00FF0000;
constexpr pixel_t blue   = 0x0000FF00;
constexpr pixel_t white  = 0xFFFFFFFF;
constexpr pixel_t black  = 0x00000000;

struct Canvas {
    size_t width, height;
    std::vector<pixel_t> pixels;

    Canvas()
        : width(0), height(0), pixels(0) {};
    Canvas(size_t width_, size_t height_)
        : width(width_), height(height_), pixels(width_ * height_, 0) {};
        
    void put_pixel(pixel_t col, size_t idx) {
        if (idx >= pixels.size()) return;
        pixels[idx] = col;
    }

    void put_pixel(pixel_t col, int x, int y) {
        if (!_is_in_bounds(x, y)) return;
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

    void triangle(pixel_t col, int x0, int y0, int x1, int y1, int x2, int y2, bool fill = false) {
        line(col, x0, y0, x1, y1);
        line(col, x1, y1, x2, y2);
        line(col, x2, y2, x0, y0);

        if (!fill) return;

        int bbminx = std::min(std::min(x0, x1), x2);
        int bbminy = std::min(std::min(y0, y1), y2);
        int bbmaxx = std::max(std::max(x0, x1), x2);
        int bbmaxy = std::max(std::max(y0, y1), y2);

        #ifdef IRIS_CANVAS_DEBUG // draw rectangle bounding box
            rectangle(red, bbminx, bbminy, bbmaxx, bbmaxy);
        #endif
        
        double triangle_area = _triangle_area(x0, y0, x1, y1, x2, y2);
        for (int xp = bbminx; xp <= bbmaxx; xp++) {
            for (int yp = bbminy; yp <= bbmaxy; yp++) {
                // x:48 y:39
                double subtriangle_area0 = _triangle_area(xp, yp, x1, y1, x2, y2);
                double subtriangle_area1 = _triangle_area(xp, yp, x2, y2, x0, y0);
                double subtriangle_area2 = _triangle_area(xp, yp, x0, y0, x1, y1);

                if (
                    subtriangle_area0 / triangle_area >= 0 &&
                    subtriangle_area1 / triangle_area >= 0 &&
                    subtriangle_area2 / triangle_area >= 0
                ) {
                    put_pixel(col, xp, yp);
                }
            }
        }

    }

    void rectangle(pixel_t col, int x0, int y0, int x1, int y1, bool fill = false) {
        line(col, x0, y0, x0, y1);
        line(col, x0, y1, x1, y1);
        line(col, x1, y1, x1, y0);
        line(col, x1, y0, x0, y0);

        if (!fill) return;

        for (int x = x0; x <= x1; x++) {
            for (int y = y0; y <= y1; y++) {
                put_pixel(col, x, y);
            }
        }

    }

    void flip_horizontally() {
        // it's a hack for sure but it's cheap and it works
        std::reverse(pixels.begin(), pixels.end());
    }

private:
    bool _is_in_bounds(int x, int y) {
        return 0 <= x && x < (int)width && 0 <= y && y < (int)height;
    }

    double _triangle_area(int x0, int y0, int x1, int y1, int x2, int y2) { 
        return ((y1-y0)*(x1+x0) + (y2-y1)*(x2+x1) + (y0-y2)*(x0+x2)) / 2.;
    }
}; // Canvas

void output_canvas_to_image(const Canvas& canvas, const char* filename) {
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