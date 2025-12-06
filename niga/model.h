
#pragma once
#include <vector>
#include <string>
#include "geometry.h"

class Model {
public:
    Model(const char* filename);
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
    ~Model();
private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<int>> faces;
};
