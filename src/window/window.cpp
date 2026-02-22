#include "window.hpp"

iris::IrisWindow::IrisWindow():
    width(0), height(0), name("") { _x_init(); };

iris::IrisWindow::IrisWindow(size_t width_, size_t height_, std::string name_):
    width(width_), height(height_), name(name_) { _x_init(); };

iris::IrisWindow::~IrisWindow() { _x_destroy(); }

void iris::IrisWindow::handle_native_event() {
    _x_handle_event();
}

void iris::IrisWindow::draw_canvas(Canvas& canvas, int offset_x, int offset_y) {
    _x_draw_canvas(canvas, offset_x, offset_y);
}

void iris::IrisWindow::clear() {
    _x_clear();
}

// X11

void iris::IrisWindow::_x_init() {
    x_display = XOpenDisplay("");
    // TODO: error check for display

    x_root_window = DefaultRootWindow(x_display);
    // TODO: error check for x_root_window

    x_window = XCreateSimpleWindow(
        x_display, x_root_window, 0, 0, width, height, 0, 0, BlackPixel(x_display, 0));
    // TODO: error check for x_window

    x_gc = XCreateGC(x_display, x_window, 0, NULL);
    // TODO: error check for x_gc

    XGetWindowAttributes(x_display, x_window, &x_wa);

    XStoreName(x_display, x_window, name.c_str());
    
    x_size_hints = XAllocSizeHints();
    x_size_hints->flags = PMinSize | PMaxSize;
    x_size_hints->min_width = x_size_hints->max_width = width;
    x_size_hints->min_height = x_size_hints->max_height = height;
    XSetWMNormalHints(x_display, x_window, x_size_hints);

    XMapWindow(x_display, x_window);
    XSelectInput(x_display, x_window, StructureNotifyMask);

    x_wm_delete_window = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x_display, x_window, &x_wm_delete_window, 1);
}

void iris::IrisWindow::_x_destroy() {
    XFree(x_size_hints);
    XCloseDisplay(x_display);
}

void iris::IrisWindow::_x_on_delete() {
    XDestroyWindow(x_display, x_window);
    closed = true;
}

void iris::IrisWindow::_x_handle_event() {
    XEvent x_event;
    XNextEvent(x_display, &x_event);
        
    switch (x_event.type) {
    case ClientMessage:
        if((Atom)x_event.xclient.data.l[0] == x_wm_delete_window) {
            _x_on_delete();
        }
        break;
    case ConfigureNotify:
        // WSLg hack to disable resizing
        // see https://github.com/microsoft/wslg/issues/1223
        if ((size_t)x_event.xconfigure.width != width || (size_t)x_event.xconfigure.height != height) {
            XResizeWindow(x_display, x_window, width, height);
            XFlush(x_display);
        }
        break;
    default:
        break;
    }
}

void iris::IrisWindow::_x_draw_canvas(Canvas& canvas, int offset_x, int offset_y) {
    if (canvas.pixels.size() <= 0) return;

    XImage* window_image = XCreateImage(
        x_display,
        x_wa.visual,
        x_wa.depth,
        ZPixmap,
        0,
        (char*) &canvas.pixels[0],
        canvas.width,
        canvas.height,
        32,
        canvas.width * sizeof(canvas.pixels[0])
    );
    
    XPutImage(
        x_display, x_window, x_gc, window_image, offset_x, offset_y, 0, 0, canvas.width, canvas.height);

    XFree(window_image);
}

void iris::IrisWindow::_x_clear() {
    XClearWindow(x_display, x_window);
}

// =======================
