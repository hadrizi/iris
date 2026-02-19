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

void draw_mesh(iris::Canvas& canvas) {
    iris::Mesh mesh;
    mesh.load_from_obj("assets/fixed_teapot.obj");
    mesh.transform_to_ndc();
    
    // size_t c = 1;
    for (const auto& face: mesh.faces) {
        iris::Vector2i v0 = iris::project_vert_orthogonally(
            mesh.get_face_vert(face, 0),
            canvas.width, canvas.height
        );
        iris::Vector2i v1 = iris::project_vert_orthogonally(
            mesh.get_face_vert(face, 1),
            canvas.width, canvas.height
        );
        iris::Vector2i v2 = iris::project_vert_orthogonally(
            mesh.get_face_vert(face, 2),
            canvas.width, canvas.height
        );

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> rnd(0, 255);

        double max_z = std::max({
            mesh.get_face_vert(face, 0).z,
            mesh.get_face_vert(face, 1).z,
            mesh.get_face_vert(face, 2).z
        });

        uint8_t mapped_col = 0 + ((max_z + 1) / 2) * (255 - 0);
        iris::pixel_t face_col = PIXEL_COL(mapped_col, mapped_col, mapped_col, 255);
        
        canvas.triangle(face_col, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, true);
        
        // canvas.flip_horizontally();
        // iris::output_canvas_to_image(canvas, std::format("out/out{}.ppm", c).c_str());
        // c++;
        // canvas.flip_horizontally();
    }

    canvas.flip_horizontally();
}

int main() {
    iris::Canvas canvas(width, height);
    canvas.fill(iris::black);

    // canvas.triangle(iris::white, 60, 10, 10, 50, 75, 70, true);
    draw_mesh(canvas);

    iris::output_canvas_to_image(canvas, "out/out.ppm");
}