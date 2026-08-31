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

std::string mesh_filenames[3] = {
    "assets/cube.obj",
    // "assets/african_head.obj",
    // "assets/diablo.obj",
    "assets/smaller_monkey.obj",
    "assets/fixed_teapot.obj"
};

struct MeshQueue {
    std::vector<iris::Mesh> meshes;
    int current_mesh_idx = 0;

    MeshQueue(const std::string mesh_filenames[], size_t mesh_filenames_amount) {
        for (size_t i = 0; i < mesh_filenames_amount; ++i) {
            iris::Mesh mesh;
            mesh.load_from_obj(mesh_filenames[i].c_str());
            // mesh.transform_to_ndc();
            
            meshes.push_back(mesh);
        }
    }

    const iris::Mesh& get_current() { return meshes[current_mesh_idx]; }
    
    void push(iris::Mesh& mesh) {
        meshes.push_back(mesh);
    }

    void next() {
        current_mesh_idx++;
        if ((size_t)current_mesh_idx >= meshes.size()) current_mesh_idx = 0;
    }
    
    void prev() {
        current_mesh_idx--;
        if (current_mesh_idx < 0) current_mesh_idx = meshes.size() - 1;
    }
};

struct Colorama {
    std::vector<iris::pixel_t> colors;
    size_t current_idx = 0;

    Colorama() {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> rnd(0, 255);

        // 4 colors theorem
        // learned it from persona 5 :)
        colors.push_back(PIXEL_COL(rnd(rng), rnd(rng), rnd(rng), 255));
        colors.push_back(PIXEL_COL(rnd(rng), rnd(rng), rnd(rng), 255));
        colors.push_back(PIXEL_COL(rnd(rng), rnd(rng), rnd(rng), 255));
        colors.push_back(PIXEL_COL(rnd(rng), rnd(rng), rnd(rng), 255));
    }

    iris::pixel_t get() {
        iris::pixel_t c = colors[current_idx];
        current_idx = current_idx + 1 >= colors.size() ? 0 : current_idx + 1;

        return c;
    }
};

struct GameState {
    MeshQueue mesh_queue;
    iris::Mesh cube_mesh;
    iris::Canvas canvas;
    Colorama colorama;
    double mesh_angle = 0;
    double dz = 1.5;
    double dy = 0;
    bool debug = false;
};

// move to Renderer object
void draw_mesh(const iris::Mesh& mesh, iris::Canvas& canvas, double angle) {
    for (const auto& face: mesh.faces) {
        iris::Vector3i v0 = iris::screen(
            iris::perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 0), angle), camera),
            canvas.width, canvas.height, far_plane
        );
        iris::Vector3i v1 = iris::screen(
            iris::perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 1), angle), camera),
            canvas.width, canvas.height, far_plane
        );
        iris::Vector3i v2 = iris::screen(
            iris::perspective(iris::rotate_vert_y(mesh.get_face_vert(face, 2), angle), camera),
            canvas.width, canvas.height, far_plane
        );

        iris::pixel_t face_col = PIXEL_COL(255, 255, 255, 255);

        canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true);
        canvas.rectangle(iris::red, v0.x - 5, v0.y - 5, v0.x + 5, v0.y + 5, true);
        canvas.rectangle(iris::red, v1.x - 5, v1.y - 5, v1.x + 5, v1.y + 5, true);
        canvas.rectangle(iris::red, v2.x - 5, v2.y - 5, v2.x + 5, v2.y + 5, true);
    }
}

void draw_mesh2(
    const iris::Mesh& mesh,
    iris::Canvas& canvas,
    Colorama& colorama,
    double angle,
    double dz, double dy,
    bool draw_debug = false
) {    
    for (const auto& face: mesh.faces) {
        iris::Vector3f v0 = iris::translate_vert_y(
            (iris::translate_vert_z(
                iris::rotate_vert_y(mesh.get_face_vert(face, 0), angle), dz)), dy);
        iris::Vector3f v1 = iris::translate_vert_y(
            (iris::translate_vert_z(
                iris::rotate_vert_y(mesh.get_face_vert(face, 1), angle), dz)), dy);
        iris::Vector3f v2 = iris::translate_vert_y(
            (iris::translate_vert_z(
                iris::rotate_vert_y(mesh.get_face_vert(face, 2), angle), dz)), dy);

        iris::Vector2i p0 = iris::screen2(
            iris::project(v0),
            canvas.width, canvas.height
        );
        iris::Vector2i p1 = iris::screen2(
            iris::project(v1),
            canvas.width, canvas.height
        );
        iris::Vector2i p2 = iris::screen2(
            iris::project(v2),
            canvas.width, canvas.height
        );

        canvas.triangle(
            draw_debug ? iris::green : colorama.get(),
            p0.x, p0.y, v0.z,
            p1.x, p1.y, v1.z,
            p2.x, p2.y, v2.z,
            !draw_debug
        );
    }

    colorama.current_idx = 0;

    if (!draw_debug) return;

    int i = 1;
    for (const auto& vert: mesh.vertices) {
        iris::Vector3f v = iris::translate_vert_y(
            (iris::translate_vert_z(
                iris::rotate_vert_y(vert, angle), dz)), dy);


        iris::Vector2i p = iris::screen2(
            iris::project(v),
            canvas.width, canvas.height
        );

        canvas.rectangle(iris::red, p.x - 2, p.y - 2, p.x + 2, p.y + 2, true);
        canvas.draw_text(
            std::format("{}", i).c_str(),
            iris::white,
            p.x - 2,
            p.y - 4 - iris::DEFAULT_FONT_HEIGHT * 2,
            2
        );

        i++;
    }

}

int main() {
    iris::IrisWindow window(width, height, "mesh test", 60);
    
    iris::Mesh cube_mesh;
    cube_mesh.name = "test_cube";
    cube_mesh.vertices.push_back(iris::Vector3f( .25,  .25,  .25));
    cube_mesh.vertices.push_back(iris::Vector3f(-.25,  .25,  .25));
    cube_mesh.vertices.push_back(iris::Vector3f( .25, -.25,  .25));
    cube_mesh.vertices.push_back(iris::Vector3f(-.25, -.25,  .25));

    cube_mesh.vertices.push_back(iris::Vector3f( .25,  .25, -.25));
    cube_mesh.vertices.push_back(iris::Vector3f(-.25,  .25, -.25));
    cube_mesh.vertices.push_back(iris::Vector3f( .25, -.25, -.25));
    cube_mesh.vertices.push_back(iris::Vector3f(-.25, -.25, -.25));

    cube_mesh.faces.push_back(iris::face_t{1, 2, 3});
    cube_mesh.faces.push_back(iris::face_t{3, 4, 2});

    cube_mesh.faces.push_back(iris::face_t{5, 6, 7});
    cube_mesh.faces.push_back(iris::face_t{7, 8, 6});
    
    cube_mesh.faces.push_back(iris::face_t{2, 6, 8});
    cube_mesh.faces.push_back(iris::face_t{8, 4, 2});
    
    cube_mesh.faces.push_back(iris::face_t{1, 5, 7});
    cube_mesh.faces.push_back(iris::face_t{7, 3, 1});
    
    cube_mesh.faces.push_back(iris::face_t{1, 5, 6});
    cube_mesh.faces.push_back(iris::face_t{6, 2, 1});

    cube_mesh.faces.push_back(iris::face_t{3, 7, 8});
    cube_mesh.faces.push_back(iris::face_t{8, 4, 3});

    MeshQueue mesh_queue = MeshQueue(mesh_filenames, 3);
    mesh_queue.push(cube_mesh);

    mesh_queue.current_mesh_idx = mesh_queue.meshes.size() - 1;

    GameState s = {
        .mesh_queue = mesh_queue,
        .cube_mesh = cube_mesh,
        .canvas = iris::Canvas(window.width, window.height),
        .colorama = Colorama(),
    };

    window.update([&s](double delta_time, XEvent event, iris::IrisWindow* window) -> void {
        // logic update
        double rotation_speed = M_PI * delta_time;
        // s.dz += 1*delta_time;
        // s.mesh_angle += rotation_speed;
        
        // rendering
        s.canvas.fill(iris::black);
        draw_mesh2(
            s.mesh_queue.get_current(),
            s.canvas,
            s.colorama,
            s.mesh_angle,
            s.dz, s.dy,
            s.debug
        );

        // gui
        // todo add gui objects(labels, containers, etc.)
        s.canvas.draw_text(
            std::format("fps: {}", window->current_fps).c_str(),
            iris::white,
            10, 10 + 0 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 0),
            font_size
        );
        s.canvas.draw_text(
            std::format("name: {}", s.mesh_queue.get_current().name).c_str(), 
            iris::white,
            10, 10 + 1 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 1),
            font_size
        );
        s.canvas.draw_text(
            std::format("faces: {}  verts: {}", s.mesh_queue.get_current().faces.size(), s.mesh_queue.get_current().vertices.size()).c_str(), 
            iris::white,
            10, 10 + 2 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 2),
            font_size
        );
        s.canvas.draw_text(
            std::format("delta: {}", delta_time).c_str(),
            iris::white,
            10, 10 + 3 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 3),
            font_size
        );
        s.canvas.draw_text(
            std::format("depth: {}", s.dz).c_str(),
            iris::white,
            10, 10 + 4 * (font_size * iris::DEFAULT_FONT_HEIGHT) + (10 * 4),
            font_size
        );
        
        window->draw_canvas(s.canvas, 0, 0);

        // handle events
        if (event.type == KeyRelease) {
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
                case XK_Right:
                    s.mesh_angle += rotation_speed;
                    break;
                case XK_Left:
                    s.mesh_angle -= rotation_speed;
                    break;
                case XK_Up:
                    s.dz += 1 * delta_time;
                    break;
                case XK_Down:
                    s.dz -= 1 * delta_time;
                    break;
                case 'w':
                    s.dy += 1 * delta_time;
                    break;
                case 's':
                    s.dy -= 1 * delta_time;
                    break;
                case 'b':
                    s.debug = !s.debug;
                    break;
                default:
                    break;
            }
        }
    });
}