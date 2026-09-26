#pragma once

#include "gdx/XTypes.h"
#include <cmath>
#include <cstdint>
#include <vector>

namespace gdx {

// Fixed-function bits Joymania used (D3D8 FVF POS+NORMAL+UV).
struct GpuVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct DrawItem {
    std::string name;
    std::string texture;
    Mat4 world;
    float scale = 1;
    std::vector<GpuVertex> v;
    std::vector<uint32_t> i;
};

inline Mat4 mul(const Mat4& a, const Mat4& b) {
    Mat4 r;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col) {
            float s = 0;
            for (int k = 0; k < 4; ++k)
                s += a.m[row * 4 + k] * b.m[k * 4 + col];
            r.m[row * 4 + col] = s;
        }
    return r;
}

inline Mat4 translation(float x, float y, float z) {
    Mat4 m;
    m.m[12] = x;
    m.m[13] = y;
    m.m[14] = z;
    return m;
}

inline Mat4 scaling(float s) {
    Mat4 m;
    m.m[0] = m.m[5] = m.m[10] = s;
    return m;
}

inline Mat4 perspectiveD3D(float fovY, float aspect, float zn, float zf) {
    Mat4 m{};
    const float ys = 1.f / std::tan(fovY * 0.5f);
    const float xs = ys / aspect;
    m.m[0] = xs;
    m.m[5] = ys;
    m.m[10] = zf / (zf - zn);
    m.m[11] = 1.f;
    m.m[14] = -zn * zf / (zf - zn);
    m.m[15] = 0.f;
    return m;
}

inline void meshToDraw(const Mesh& mesh, DrawItem& out) {
    out.name = mesh.name;
    out.texture = mesh.materials.empty() ? std::string() : mesh.materials[0].texture;
    const size_t n = mesh.positions.size();
    out.v.resize(n);
    for (size_t i = 0; i < n; ++i) {
        GpuVertex g{};
        g.x = mesh.positions[i].x;
        g.y = mesh.positions[i].y;
        g.z = mesh.positions[i].z;
        if (i < mesh.normals.size()) {
            g.nx = mesh.normals[i].x;
            g.ny = mesh.normals[i].y;
            g.nz = mesh.normals[i].z;
        } else {
            g.ny = 1;
        }
        if (i < mesh.uvs.size()) {
            g.u = mesh.uvs[i].x;
            g.v = mesh.uvs[i].y;
        }
        out.v[i] = g;
    }
    out.i = mesh.indices;
}

} // namespace gdx
