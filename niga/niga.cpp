#include <vector>
#include <cmath>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;


void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}


void triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage& image, TGAColor color, int* zbuffer) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > (t1.y - t0.y) || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = total_height == 0 ? 0 : (float)i / total_height;
        float beta = segment_height == 0 ? 0 : (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

        Vec3f A = Vec3f(t0) + (Vec3f(t2) - Vec3f(t0)) * alpha;
        Vec3f B = second_half ? Vec3f(t1) + (Vec3f(t2) - Vec3f(t1)) * beta
            : Vec3f(t0) + (Vec3f(t1) - Vec3f(t0)) * beta;

        if (A.x > B.x) std::swap(A, B);

        for (int j = (int)A.x; j <= (int)B.x; j++) {
            float phi = B.x == A.x ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
            Vec3f P = A + (B - A) * phi;
            int idx = (int)P.x + (int)P.y * width;
            if (zbuffer[idx] < (int)P.z) {
                zbuffer[idx] = (int)P.z;
                image.set((int)P.x, (int)P.y, color);
            }
        }
    }
}


int main(int argc, char** argv) {
    if (argc == 2) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    int* zbuffer = new int[width * height];
    for (int i = 0; i < width * height; i++) zbuffer[i] = -std::numeric_limits<int>::max();

   
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3i screen_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3i(
                (int)((v.x + 1.) * width / 2.),
                (int)((v.y + 1.) * height / 2.),
                (int)(v.z * 255) 
            );
        }
        triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, white, zbuffer);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    delete[] zbuffer;
    delete model;

    return 0;
}
