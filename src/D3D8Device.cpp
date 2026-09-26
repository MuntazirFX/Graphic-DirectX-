#include "gdx/D3D8Device.h"
#include <algorithm>
#include <cmath>
#include <fstream>

namespace gdx {
namespace {

struct V {
    float x, y, z, w;
    float nx, ny, nz;
    float u, v;
};

V xform(const Mat4& m, float x, float y, float z) {
    V o{};
    o.x = m.m[0] * x + m.m[4] * y + m.m[8] * z + m.m[12];
    o.y = m.m[1] * x + m.m[5] * y + m.m[9] * z + m.m[13];
    o.z = m.m[2] * x + m.m[6] * y + m.m[10] * z + m.m[14];
    o.w = m.m[3] * x + m.m[7] * y + m.m[11] * z + m.m[15];
    return o;
}

void barycentric(float x0, float y0, float x1, float y1, float x2, float y2,
                 float px, float py, float& a, float& b, float& c) {
    const float den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
    if (std::fabs(den) < 1e-8f) {
        a = b = c = -1;
        return;
    }
    a = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) / den;
    b = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / den;
    c = 1.f - a - b;
}

} // namespace

void Device8::reset(int width, int height, Color8 clear) {
    w = width;
    h = height;
    color.assign(static_cast<size_t>(w * h), clear);
    depth.assign(static_cast<size_t>(w * h), 1.f);
    world = {};
    view = {};
    proj = perspectiveD3D(0.9f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.f);
}

void Device8::draw(const DrawItem& item) {
    if (w <= 0 || item.i.size() < 3) return;
    const Mat4 wvp = mul(mul(item.world.m[0] == 1 && item.world.m[12] == 0 ? world : item.world, view), proj);
    const Mat4 M = mul(mul(world, view), proj);
    (void)wvp;
    auto project = [&](const GpuVertex& gv) {
        V e = xform(world, gv.x, gv.y, gv.z);
        V c = xform(view, e.x, e.y, e.z);
        V cl = xform(proj, c.x, c.y, c.z);
        if (cl.w == 0) cl.w = 1;
        const float inv = 1.f / cl.w;
        V s = cl;
        s.x = (cl.x * inv * 0.5f + 0.5f) * static_cast<float>(w);
        s.y = (1.f - (cl.y * inv * 0.5f + 0.5f)) * static_cast<float>(h);
        s.z = cl.z * inv;
        s.w = cl.w;
        s.nx = gv.nx;
        s.ny = gv.ny;
        s.nz = gv.nz;
        s.u = gv.u;
        s.v = gv.v;
        return s;
    };
    (void)M;
    for (size_t t = 0; t + 2 < item.i.size(); t += 3) {
        const uint32_t ia = item.i[t], ib = item.i[t + 1], ic = item.i[t + 2];
        if (ia >= item.v.size() || ib >= item.v.size() || ic >= item.v.size()) continue;
        V A = project(item.v[ia]);
        V B = project(item.v[ib]);
        V C = project(item.v[ic]);
        const float minx = std::max(0.f, std::floor(std::min({A.x, B.x, C.x})));
        const float maxx = std::min(static_cast<float>(w - 1), std::ceil(std::max({A.x, B.x, C.x})));
        const float miny = std::max(0.f, std::floor(std::min({A.y, B.y, C.y})));
        const float maxy = std::min(static_cast<float>(h - 1), std::ceil(std::max({A.y, B.y, C.y})));
        for (int y = static_cast<int>(miny); y <= static_cast<int>(maxy); ++y) {
            for (int x = static_cast<int>(minx); x <= static_cast<int>(maxx); ++x) {
                float ba, bb, bc;
                barycentric(A.x, A.y, B.x, B.y, C.x, C.y,
                            static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f,
                            ba, bb, bc);
                if (ba < 0 || bb < 0 || bc < 0) continue;
                const float z = ba * A.z + bb * B.z + bc * C.z;
                const int idx = y * w + x;
                if (z < 0.f || z > 1.f || z >= depth[static_cast<size_t>(idx)]) continue;
                depth[static_cast<size_t>(idx)] = z;
                const float nx = ba * A.nx + bb * B.nx + bc * C.nx;
                const float ny = ba * A.ny + bb * B.ny + bc * C.ny;
                const float nz = ba * A.nz + bb * B.nz + bc * C.nz;
                float ndl = nx * 0.35f + ny * 0.8f + nz * 0.45f;
                ndl = std::max(0.2f, std::min(1.f, ndl));
                color[static_cast<size_t>(idx)] = {
                    static_cast<uint8_t>(40 + ndl * 200),
                    static_cast<uint8_t>(50 + ndl * 180),
                    static_cast<uint8_t>(70 + ndl * 150),
                    255};
            }
        }
    }
}

bool Device8::savePPM(const std::string& path) const {
    std::ofstream o(path, std::ios::binary);
    if (!o) return false;
    o << "P6\n" << w << " " << h << "\n255\n";
    for (const auto& p : color) {
        o.put(static_cast<char>(p.r));
        o.put(static_cast<char>(p.g));
        o.put(static_cast<char>(p.b));
    }
    return true;
}

int Device8::litPixels() const {
    int n = 0;
    for (const auto& p : color)
        if (p.r > 20 || p.g > 20 || p.b > 30) ++n;
    return n;
}

} // namespace gdx
