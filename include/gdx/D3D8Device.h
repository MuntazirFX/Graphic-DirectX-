#pragma once

#include "gdx/D3D8.h"
#include <cstdint>
#include <string>
#include <vector>

namespace gdx {

struct Color8 { uint8_t r = 0, g = 0, b = 0, a = 255; };

class Device8 {
public:
    int w = 0, h = 0;
    Mat4 world, view, proj;
    std::vector<Color8> color;
    std::vector<float> depth;

    void reset(int width, int height, Color8 clear = {12, 14, 22, 255});
    void setWorld(const Mat4& m) { world = m; }
    void setView(const Mat4& m) { view = m; }
    void setProj(const Mat4& m) { proj = m; }
    void draw(const DrawItem& item);
    bool savePPM(const std::string& path) const;
    int litPixels() const;
};

} // namespace gdx
