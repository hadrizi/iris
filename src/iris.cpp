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

// game configs; should be moved to some kind of Game object in the future
constexpr size_t width  = 1000;
constexpr size_t height = 1000;
constexpr size_t far_plane = 255;
constexpr double camera = 255.;
constexpr int font_size = 3;

std::string mesh_filenames[4] = {
    "assets/african_head.obj",
    "assets/cube.obj",
    "assets/smaller_monkey.obj",
    "assets/fixed_teapot.obj"
};

struct MeshQueue {
    std::vector<iris::Mesh> meshes;

    MeshQueue(const std::string mesh_filenames[], size_t mesh_filenames_amount) {
        for (size_t i = 0; i < mesh_filenames_amount; ++i) {
            iris::Mesh mesh;
            mesh.load_from_obj(mesh_filenames[i].c_str());
            mesh.transform_to_ndc();
            
            meshes.push_back(mesh);
        }
    }

    const iris::Mesh& get_current() { return meshes[current_mesh_idx]; }
    
    void next() {
        current_mesh_idx++;
        if ((size_t)current_mesh_idx >= meshes.size()) current_mesh_idx = 0;
    }
    
    void prev() {
        current_mesh_idx--;
        if (current_mesh_idx < 0) current_mesh_idx = meshes.size() - 1;
    }

private:
    int current_mesh_idx = 0;
};

struct GameState {
    MeshQueue mesh_queue;
    iris::Canvas canvas;
    double mesh_angle = 0;
};

// move to Renderer object
void draw_mesh(const iris::Mesh& mesh, iris::Canvas& canvas, double angle) {
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

        iris::pixel_t face_col = PIXEL_COL(255, 255, 255, 255);

        canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true, true);
    }
}

int main() {
    iris::IrisWindow window(width, height, "mesh test", 60);
    
    GameState s = {
        .mesh_queue = MeshQueue(mesh_filenames, 4),
        .canvas = iris::Canvas(window.width, window.height),
        .mesh_angle = 0,
    };

    window.update([&s](double delta_time, XEvent event, iris::IrisWindow* window) -> void {
        // angle update
        double rotation_speed = M_PI / 6;
        s.mesh_angle += rotation_speed * delta_time;
        
        // rendering
        s.canvas.fill(iris::black);
        draw_mesh(s.mesh_queue.get_current(), s.canvas, s.mesh_angle);

        // gui
        // todo add gui objects(labels, containers, etc.)
        s.canvas.draw_text(
            std::format("fps: {}", window->current_fps).c_str(),
            iris::white,
            10, 10 + 0 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 0),
            font_size
        );
        s.canvas.draw_text(
            std::format("model: {}", s.mesh_queue.get_current().name).c_str(), 
            iris::white,
            10, 10 + 1 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 1),
            font_size
        );
        s.canvas.draw_text(
            std::format("delta: {}", delta_time).c_str(),
            iris::white,
            10, 10 + 2 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 2),
            font_size
        );
        
        window->draw_canvas(s.canvas, 0, 0);

        // handle events
        if (event.type == KeyPress) {
            switch (XLookupKeysym(&event.xkey, 0)) {
                case 'e':
                    s.mesh_queue.next();
                    break;
                case 'q':
                    s.mesh_queue.prev();
                    break;
                case XK_Escape:
                    window->closed = true;
                    break;
                default:
                    break;
            }
        }
    });
}