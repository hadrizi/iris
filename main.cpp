#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <random>
#include <format>

// #define IRIS_CANVAS_DEBUG

#include "graphics.hpp"
#include "math.hpp"
#include "mesh.hpp"

// Game configs; should be moved to some kind of Game object in the future
constexpr size_t width  = 800;
constexpr size_t height = 800;
constexpr size_t far_plane = 255;
constexpr double camera = 255.;

void draw_mesh(iris::Canvas& canvas, iris::Canvas& depth_canvas, double angle) {
    iris::Mesh mesh;
    mesh.load_from_obj("assets/fixed_teapot.obj");
    mesh.transform_to_ndc();
    
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

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> rnd(0, 255);

        iris::pixel_t face_col = PIXEL_COL(rnd(rng), rnd(rng), rnd(rng), 255);

        canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true);
        depth_canvas.triangle(face_col, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, true, true);
        
        // iris::snapshot_canvas(canvas);
        // iris::snapshot_canvas(depth_canvas, "depth_");
    }
}

int main() {
    iris::Canvas canvas(width, height);
    iris::Canvas depth_canvas(width, height);
    
    canvas.fill(iris::black);
    depth_canvas.fill(iris::black);

    double angle_param = 36;
    for (int i = 0; i < angle_param * 2; ++i) {
        draw_mesh(canvas, depth_canvas, (M_PI / angle_param) * i);
        iris::snapshot_canvas(depth_canvas);
    
        depth_canvas.fill(iris::black);
    }

    // iris::output_canvas_to_image(depth_canvas, "out/out.ppm");
    // iris::output_canvas_to_image(depth_canvas, "out/depth_out.ppm");
}