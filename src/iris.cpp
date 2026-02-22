#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <random>
#include <format>
#include <chrono>
#include <thread>

// #define IRIS_CANVAS_DEBUG

#include "graphics/graphics.hpp"
#include "math/math.hpp"
#include "mesh/mesh.hpp"
#include "window/window.hpp"

const char* model_name = "assets/fixed_teapot.obj";

// game configs; should be moved to some kind of Game object in the future
constexpr size_t width  = 800;
constexpr size_t height = 800;
constexpr size_t far_plane = 255;
constexpr double camera = 255.;
constexpr int font_size = 3;

void draw_mesh(iris::Mesh& mesh, iris::Canvas& canvas, double angle) {
    for (const auto& face: mesh.faces) {
        iris::Vector3i v0 = iris::project_vert_orthogonally(
            iris::project_vert_perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 0), angle), camera),
            canvas.width, canvas.height, far_plane
        );
        iris::Vector3i v1 = iris::project_vert_orthogonally(
            iris::project_vert_perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 1), angle), camera),
            canvas.width, canvas.height, far_plane
        );
        iris::Vector3i v2 = iris::project_vert_orthogonally(
            iris::project_vert_perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 2), angle), camera),
            canvas.width, canvas.height, far_plane
        );

        // std::random_device dev;
        // std::mt19937 rng(dev());
        // std::uniform_int_distribution<std::mt19937::result_type> rnd(0, 255);

        iris::pixel_t face_col = PIXEL_COL(255, 255, 255, 255);

        // canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true);
        canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true, true);
    }
}

void test_mesh_rendering() {
    iris::Mesh mesh;
    mesh.load_from_obj(model_name);
    mesh.transform_to_ndc();

    iris::IrisWindow window(width, height, "mesh test", 60);
    iris::Canvas canvas(window.width, window.height);    
    
    double angle_param = 0;
    double rotation_speed = M_PI / 6;
    
    const auto frame_time = std::chrono::nanoseconds(1'000'000'000LL / window.fps);
    auto next_frame = std::chrono::steady_clock::now();

    int frame_counter = 0;
    auto fps_timer = std::chrono::steady_clock::now();
    int current_fps = window.fps;

    auto previous_time = std::chrono::steady_clock::now();
    
    while(!window.closed) {
        auto current_time = std::chrono::steady_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - previous_time).count();
        previous_time = current_time;

        // events
        window.handle_native_event();
        
        // logic update
        angle_param += rotation_speed * delta_time;
        
        // rendering
        canvas.fill(iris::black);
        draw_mesh(mesh, canvas, angle_param);

        canvas.draw_text(
            std::format("fps: {}", current_fps).c_str(),
            iris::white,
            10, 10 + 0 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 0),
            font_size
        );
        canvas.draw_text(
            std::format("model: {}", model_name).c_str(), 
            iris::white,
            10, 10 + 1 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 1),
            font_size
        );
        canvas.draw_text(
            std::format("delta: {}", delta_time).c_str(),
            iris::white,
            10, 10 + 2 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 2),
            font_size
        );
        
        window.draw_canvas(canvas, 0, 0);

        // fps counting
        frame_counter++;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - fps_timer);
        if (elapsed.count() >= 1)
        {
            current_fps = frame_counter;
            frame_counter = 0;
            fps_timer = now;
        }

        // frame cap
        next_frame += frame_time;
        if (now < next_frame) {
            std::this_thread::sleep_until(next_frame);
        } else {
            next_frame = now;
        }
    }
}

void test_text_rendering() {
    iris::IrisWindow window(width, height);
    iris::Canvas canvas(window.width, window.height);

    const auto frame_time = std::chrono::nanoseconds(1'000'000'000LL / window.fps);
    auto next_frame = std::chrono::steady_clock::now();
    
    int counter = 0;
    while(!window.closed) {
        canvas.fill(iris::black);
        
        window.handle_native_event();
        
        // canvas.rectangle(iris::white, 20, 20, 60, 60, true);
        canvas.draw_text(std::format("fps:{}", counter % 60).c_str(), iris::white, 10, 10, 2);
        counter++;
        window.draw_canvas(canvas, 0, 0);
        
        next_frame += frame_time;
        auto now = std::chrono::steady_clock::now();

        if (now < next_frame) {
            std::this_thread::sleep_until(next_frame);
        } else {
            next_frame = now;
        }
    }

}

int main() {
    test_mesh_rendering();
    return 0;
}