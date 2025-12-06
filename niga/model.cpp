
#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>

Model::Model(const char* filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            float x, y, z;
            iss >> x >> y >> z;
            verts.push_back(Vec3f(x, y, z));
        }
        else if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            int f;
            std::vector<int> face;
            while (iss >> f) {
                face.push_back(f - 1);
            }
            faces.push_back(face);
        }
    }
}

int Model::nverts() { return verts.size(); }
int Model::nfaces() { return faces.size(); }

Vec3f Model::vert(int i) { return verts[i]; }

std::vector<int> Model::face(int idx) { return faces[idx]; }

Model::~Model() {}
