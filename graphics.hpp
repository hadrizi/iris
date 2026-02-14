#ifndef IRIS_GRAPHICS_H_
#define IRIS_GRAPHICS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

namespace iris {

typedef uint32_t pixel_t;

void put_pixel(
    std::vector<pixel_t>& pixels,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a,
    size_t idx
) {
    pixel_t pixel_val = r;
    pixel_val = pixel_val | (g <<  8);
    pixel_val = pixel_val | (b << 16);
    pixel_val = pixel_val | (a << 24);

    pixels[idx] = pixel_val;
}

uint8_t get_r(pixel_t pixel) {
    return (uint8_t)(pixel & 0xFF);
}

uint8_t get_g(pixel_t pixel) {
    return (uint8_t)((pixel >> 8) & 0xFF);
}

uint8_t get_b(pixel_t pixel) {
    return (uint8_t)((pixel >> 16) & 0xFF);
}

uint8_t get_a(pixel_t pixel) {
    return (uint8_t)((pixel >> 24) & 0xFF);
}

void output_pixels_to_image(
    std::vector<pixel_t>& pixels,
    size_t width, size_t height,
    const char* filename
) {
    std::ofstream f;
    f.open(filename);
    f << "P6\n" << width << " " << height << "\n255\n";

    for (size_t idx = 0; idx < width * height; idx++) {
        f << get_r(pixels[idx]) << get_g(pixels[idx]) << get_b(pixels[idx]);
    }

    f.close();
}

} // iris

#endif // IRIS_GRAPHICS_H_