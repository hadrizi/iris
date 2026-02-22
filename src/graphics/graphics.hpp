#ifndef IRIS_GRAPHICS_H_
#define IRIS_GRAPHICS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <format>

#include "font.hpp"

namespace iris {

typedef uint32_t pixel_t;

#define PIXEL_COL(r, g, b, a) ((uint32_t)(((uint8_t)r << 24) | ((uint8_t)g << 16) | ((uint8_t)b << 8) | (uint8_t)a))
#define PIXEL_RED(p)   ((uint8_t)((uint32_t)p >> 24))
#define PIXEL_GREEN(p) ((uint8_t)((uint32_t)p >> 16))
#define PIXEL_BLUE(p)  ((uint8_t)((uint32_t)p >> 8))
#define PIXEL_ALPHA(p) ((uint8_t)((uint32_t)p))

// Helper color consts

constexpr pixel_t red    = 0xFF000000;
constexpr pixel_t green  = 0x00FF0000;
constexpr pixel_t blue   = 0x0000FF00;
constexpr pixel_t white  = 0xFFFFFFFF;
constexpr pixel_t black  = 0x00000000;

// ==================

struct Canvas {
    size_t width, height;
    std::vector<pixel_t> pixels;
    std::vector<uint8_t> depth_buffer;

    Canvas();
    Canvas(size_t width_, size_t height_);
        
    // simple operations
    
    void put_pixel(pixel_t col, size_t idx);
    void put_pixel(pixel_t col, int x, int y);
    pixel_t get_pixel(size_t idx);
    pixel_t get_pixel(int x, int y);
    void fill(pixel_t col);
    void line(pixel_t col, int x0, int y0, int x1, int y1);
    void flip_horizontally();

    // ==================

    // figures
    
    void triangle(
        pixel_t col,
        int x0, int y0, int z0,
        int x1, int y1, int z1,
        int x2, int y2, int z2,
        bool fill=false,
        bool use_depth_color=false
    );
    void rectangle(pixel_t col, int x0, int y0, int x1, int y1, bool fill=false);
    
    // ==================

    // text

    void draw_text(const char* text, pixel_t col, int x, int y, size_t size=1);

    // ==================

private:
    // some helper stuff
    
    bool _is_in_bounds(int x, int y);
    double _triangle_area(int x0, int y0, int x1, int y1, int x2, int y2);

    // ==================

}; // Canvas

void output_canvas_to_image(const Canvas& canvas, const char* filename);

void snapshot_canvas(Canvas& canvas, const char* prefix="");


} // iris

#endif // IRIS_GRAPHICS_H_