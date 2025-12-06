
#include "tgaimage.h"

TGAImage::TGAImage(int w,int h,int bpp)
    : width(w), height(h), bytespp(bpp), data(w*h*bpp) {}

TGAImage::~TGAImage() {}

void TGAImage::set(int x,int y,const TGAColor& c){
    if(x<0||y<0||x>=width||y>=height) return;
    for(int i=0;i<bytespp;i++)
        data[(x+y*width)*bytespp+i] = c.bgra[i];
}

void TGAImage::flip_vertically(){
    int stride = width * bytespp;
    std::vector<uint8_t> line(stride);
    for(int i=0;i<height/2;i++){
        memcpy(&line[0], &data[i*stride], stride);
        memcpy(&data[i*stride], &data[(height-1-i)*stride], stride);
        memcpy(&data[(height-1-i)*stride], &line[0], stride);
    }
}

bool TGAImage::write_tga_file(const char* filename){
    std::ofstream out(filename, std::ios::binary);
    if(!out.is_open()) return false;

    uint8_t header[18] = {0};
    header[2] = (bytespp == GRAYSCALE ? 3 : 2);
    header[12] = width & 0xFF;
    header[13] = (width >> 8) & 0xFF;
    header[14] = height & 0xFF;
    header[15] = (height >> 8) & 0xFF;
    header[16] = bytespp * 8;

    out.write((char*)header, 18);
    out.write((char*)data.data(), data.size());

    uint8_t footer[26] = {0};
    out.write((char*)footer, 26);
    return true;
}
