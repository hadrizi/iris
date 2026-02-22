#include "graphics.hpp"

iris::Canvas::Canvas()
    : width(0), height(0), pixels(0), depth_buffer(0) {};

iris::Canvas::Canvas(size_t width_, size_t height_)
    : width(width_), height(height_), pixels(width_ * height_, 0), depth_buffer(width_ * height_, 0) {};
    
void iris::Canvas::put_pixel(pixel_t col, size_t idx) {
    if (idx >= pixels.size()) return;
    pixels[idx] = col;
}

void iris::Canvas::put_pixel(pixel_t col, int x, int y) {
    if (!_is_in_bounds(x, y)) return;
    pixels[width * y + x] = col;
}

iris::pixel_t iris::Canvas::get_pixel(size_t idx) {
    if (idx >= pixels.size()) return 0;
    return pixels[idx];
}

iris::pixel_t iris::Canvas::get_pixel(int x, int y) {
    if (!_is_in_bounds(x, y)) return 0;
    return pixels[width * y + x];
}

void iris::Canvas::fill(pixel_t col) {
    std::fill(pixels.begin(), pixels.end(), col);
    std::fill(depth_buffer.begin(), depth_buffer.end(), 0);
}

void iris::Canvas::line(pixel_t col, int x0, int y0, int x1, int y1) {
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

void iris::Canvas::triangle(
    pixel_t col,
    int x0, int y0, int z0,
    int x1, int y1, int z1,
    int x2, int y2, int z2,
    bool fill,
    bool use_depth_color
) {
    if (!fill) {
        line(col, x0, y0, x1, y1);
        line(col, x1, y1, x2, y2);
        line(col, x2, y2, x0, y0);
    }

    int bbminx = std::min(std::min(x0, x1), x2);
    int bbminy = std::min(std::min(y0, y1), y2);
    int bbmaxx = std::max(std::max(x0, x1), x2);
    int bbmaxy = std::max(std::max(y0, y1), y2);

    #ifdef IRIS_CANVAS_DEBUG // draw rectangle bounding box
        rectangle(red, bbminx, bbminy, bbmaxx, bbmaxy);
    #endif
    
    // barycentric coordinates are used to check if point is inside a triangle
    double triangle_area = _triangle_area(x0, y0, x1, y1, x2, y2);
    for (int xp = bbminx; xp <= bbmaxx; xp++) {
        for (int yp = bbminy; yp <= bbmaxy; yp++) {
            double t0 = _triangle_area(xp, yp, x1, y1, x2, y2) / triangle_area;
            double t1 = _triangle_area(xp, yp, x2, y2, x0, y0) / triangle_area;
            double t2 = _triangle_area(xp, yp, x0, y0, x1, y1) / triangle_area;
            if (t0 < 0 || t1 < 0 || t2 < 0) continue;
            if (!_is_in_bounds(xp, yp)) continue;

            uint8_t zp = static_cast<uint8_t>(z0 * t0 + z1 * t1 + z2 * t2);
            if (zp <= depth_buffer[width * yp + xp]) continue;
            
            col = use_depth_color ? PIXEL_COL(zp, zp, zp, 255) : col;
            
            depth_buffer[width * yp + xp] = zp;
            put_pixel(col, xp, yp);
        }
    }

}

void iris::Canvas::rectangle(pixel_t col, int x0, int y0, int x1, int y1, bool fill) {
    if (!fill) {        
        line(col, x0, y0, x0, y1);
        line(col, x0, y1, x1, y1);
        line(col, x1, y1, x1, y0);
        line(col, x1, y0, x0, y0);
    };

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            put_pixel(col, x, y);
        }
    }

}

void iris::Canvas::draw_text(const char* text, pixel_t col, int x, int y, size_t size) {
    char c = ' ';
    size_t char_idx = 0;
    while (c != '\0') {
        c = text[char_idx];

        glyph_t glyph_map = DEFAULT_FONT[c];
        int offseted_x0, offseted_y0, offseted_x1, offseted_y1;

        for (size_t i = 0; i < DEFAULT_FONT_HEIGHT; ++i) {
            for (size_t j = 0; j < DEFAULT_FONT_WIDTH; ++j) {
                offseted_x0 = x + (j * size) + (char_idx * size * DEFAULT_FONT_WIDTH);
                offseted_y0 = y + (i * size);
                offseted_x1 = offseted_x0 + (size - 1);
                offseted_y1 = offseted_y0 + (size - 1);
                
                if (glyph_map[i][j] == 0) continue;

                rectangle(
                    col,
                    offseted_x0, offseted_y0,
                    offseted_x1, offseted_y1,
                    true
                );
            }
        }
        
        char_idx++;
    }
        
}

void iris::Canvas::flip_horizontally() {
    // it's a hack for sure but it's cheap and it works
    std::reverse(pixels.begin(), pixels.end());
}

bool iris::Canvas::_is_in_bounds(int x, int y) {
    return 0 <= x && x < (int)width && 0 <= y && y < (int)height;
}

double iris::Canvas::_triangle_area(int x0, int y0, int x1, int y1, int x2, int y2) { 
    return ((y1-y0)*(x1+x0) + (y2-y1)*(x2+x1) + (y0-y2)*(x0+x2)) / 2.;
}

void iris::output_canvas_to_image(const Canvas& canvas, const char* filename) {
    std::ofstream f;
    f.open(filename, std::ios::binary);
    f << "P6\n" << canvas.width << " " << canvas.height << "\n255\n";

    for (const pixel_t& p: canvas.pixels) {
        f << (uint8_t)((p >> 24) & 0xFF) << (uint8_t)((p >> 16) & 0xFF) << (uint8_t)((p >> 8) & 0xFF);
    }

    f.close();
}

void iris::snapshot_canvas(Canvas& canvas, const char* prefix) {
    static size_t snapshot_counter = 1;

    canvas.flip_horizontally();
    output_canvas_to_image(canvas, std::format("out/{}out{}.ppm", prefix, snapshot_counter++).c_str());
    canvas.flip_horizontally();
}