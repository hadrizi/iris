#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <random>
#include <format>

// #define IRIS_CANVAS_DEBUG

#include "graphics/graphics.hpp"
#include "math/math.hpp"
#include "mesh/mesh.hpp"
#include "window/window.hpp"

// Game configs; should be moved to some kind of Game object in the future
constexpr size_t width  = 800;
constexpr size_t height = 800;
constexpr size_t far_plane = 255;
constexpr double camera = 255.;

void draw_mesh(iris::Mesh& mesh, iris::Canvas& canvas, iris::Canvas& depth_canvas, double angle) {
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

        canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true);
        depth_canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true, true);
        
        // iris::snapshot_canvas(canvas);
        // iris::snapshot_canvas(depth_canvas, "depth_");
    }
}

int main() {
    iris::Mesh mesh;
    mesh.load_from_obj("assets/cube.obj");
    mesh.transform_to_ndc();

    iris::Canvas canvas(width, height);
    iris::Canvas depth_canvas(width, height);
    
    canvas.fill(iris::black);
    depth_canvas.fill(iris::black);
    
    iris::IrisWindow window(width, height);
    
    double angle_param = 0;
    double max_angle_param = 72;
    while(!window.closed) {
        depth_canvas.fill(iris::black);
        canvas.fill(iris::black);
        draw_mesh(mesh, canvas, depth_canvas, angle_param * (M_PI / 36));

        canvas.flip_horizontally();
        depth_canvas.flip_horizontally();
        
        window.handle_native_event();
        window.draw_canvas(depth_canvas, 0, 0);
        
        angle_param++;
        if (angle_param > max_angle_param) angle_param = 0;

        canvas.flip_horizontally();
        depth_canvas.flip_horizontally();
    }
}