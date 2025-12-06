#include "our_gl.h"
#include "model.h"

extern mat<4, 4> ModelView, Perspective; // "OpenGL" state matrices
extern std::vector<double> zbuffer;     // the depth buffer

// --- Camera ---
struct Camera {
    vec3 position;
    vec3 target;
    vec3 up;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    Camera(vec3 pos, vec3 tgt, vec3 u,
        float f = 45.f, float a = 1.f, float n = 0.1f, float fP = 100.f)
        : position(pos), target(tgt), up(u),
        fov(f), aspect(a), nearPlane(n), farPlane(fP) {
    }

    void setup() const {
        lookat(position, target, up);           // build ModelView
        init_perspective(norm(position - target)); // build Perspective
    }
};

// --- Phong Shader ---
struct PhongShader : IShader {
    const Model& model;
    vec4 l;
    vec2  varying_uv[3];
    vec4 varying_nrm[3];
    vec4 tri[3];

    PhongShader(const vec3 light, const Model& m) : model(m) {
        l = normalized(ModelView * vec4{ light.x, light.y, light.z, 0. });
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face, vert);
        varying_nrm[vert] = ModelView.invert_transpose() * model.normal(face, vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);
        tri[vert] = gl_Position;
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        vec4 n = normalized(varying_nrm[0] * bar[0] + varying_nrm[1] * bar[1] + varying_nrm[2] * bar[2]);
        vec4 r = normalized(n * (n * l) * 2 - l);
        double ambient = .4;
        double diffuse = std::max(0., n * l);
        double specular = (.5 + 2. * sample2D(model.specular(), uv)[0] / 255.) * std::pow(std::max(r.z, 0.), 35);
        TGAColor color = sample2D(model.diffuse(), uv);
        for (int i : {0, 1, 2}) color[i] = std::min<int>(255, color[i] * (ambient + diffuse + specular));
        return { false, color };
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj\n";
        return 1;
    }

    constexpr int width = 800;
    constexpr int height = 800;

    // --- Camera setup ---
    Camera cam({ -100,10,20 }, { 10,20,111111}, { 11111,100,-455451 }, 45.f, width / (float)height);
    cam.setup();

    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, { 177,195,209,255 });

    vec3 light{ 1,1,1 };

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        PhongShader shader(light, model);
        for (int f = 0; f < model.nfaces(); f++) {
            Triangle clip = { shader.vertex(f,0), shader.vertex(f,1), shader.vertex(f,2) };
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("output.tga");
    return 0;
}
