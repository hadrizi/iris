#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>

#include "graphics.hpp"
#include "math.hpp"
#include "mesh.hpp"

constexpr size_t width  = 800;
constexpr size_t height = 800;

// Helper color consts
constexpr uint32_t red    = 0xFF000000;
constexpr uint32_t green  = 0x00FF0000;
constexpr uint32_t blue   = 0x0000FF00;
constexpr uint32_t white  = 0xFFFFFFFF;
constexpr uint32_t black  = 0x00000000;

int main() {
    iris::Canvas canvas(width, height, black);

    iris::Mesh mesh;
    mesh.load_from_obj("assets/teapot.obj");
    mesh.transform_to_ndc();
    
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
        
        canvas.triangle(white, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
    }

    canvas.flip_horizontally();
    iris::output_canvas_to_image(canvas, "out.ppm");
}