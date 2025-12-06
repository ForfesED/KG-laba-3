
#pragma once
#include <cstdint>
#include <vector>
#include <fstream>

struct TGAColor {
    uint8_t bgra[4];
    TGAColor() { bgra[0]=bgra[1]=bgra[2]=bgra[3]=0; }
    TGAColor(uint8_t r,uint8_t g,uint8_t b,uint8_t a=255){
        bgra[2]=r; bgra[1]=g; bgra[0]=b; bgra[3]=a;
    }
};

class TGAImage {
public:
    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };
    TGAImage(int w,int h,int bpp);
    ~TGAImage();
    bool write_tga_file(const char* filename);
    void set(int x,int y,const TGAColor& c);
    void flip_vertically();
private:
    int width, height, bytespp;
    std::vector<uint8_t> data;
};
