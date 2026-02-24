#ifndef IRIS_WINDOW_H_
#define IRIS_WINDOW_H_

#include <string>

#include <stddef.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../graphics/graphics.hpp"

namespace iris {

struct IrisWindow {
    size_t width, height;
    std::string name;
    uint8_t fps;

    bool closed = false;

    IrisWindow();
    IrisWindow(size_t width_, size_t height_, std::string name="Iris window", uint8_t fps_=30);

    ~IrisWindow();
    
    // platform agnostic interface

    XEvent handle_native_event();
    void draw_canvas(iris::Canvas& canvas, int offset_x, int offset_y);
    void clear();

    // ===========================
private:
    // X11

    Display*          x_display;
    Window            x_root_window;
    Window            x_window;
    Atom              x_wm_delete_window;
    XSizeHints*       x_size_hints;
    GC                x_gc;
    XImage*           x_window_image;

    void _x_init();
    void _x_destroy();
    XEvent _x_handle_event();
    void _x_on_delete();
    void _x_draw_canvas(iris::Canvas& canvas, int offset_x, int offset_y);
    void _x_clear();

    // ===============================
}; // Window

} // iris

#endif // IRIS_WINDOW_H_