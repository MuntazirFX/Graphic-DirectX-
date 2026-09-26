#pragma once

#include "gdx/XTypes.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace gdx {

struct PackedVertexSkin {
    uint16_t bones[4] = {0, 0, 0, 0};
    float    weights[4] = {1, 0, 0, 0};
};

inline int findBone(const Document& doc, const std::string& name) {
    for (int i = 0; i < static_cast<int>(doc.frames.size()); ++i)
        if (doc.frames[i].name == name) return i;
    return -1;
}

inline std::vector<PackedVertexSkin> packSkin(const Document& doc, const Mesh& mesh) {
    std::vector<PackedVertexSkin> out(mesh.positions.size());
    for (const auto& inf : mesh.skins) {
        int bone = findBone(doc, inf.boneName);
        if (bone < 0) bone = 0;
        const size_t n = std::min(inf.vertices.size(), inf.weights.size());
        for (size_t k = 0; k < n; ++k) {
            int vi = inf.vertices[k];
            if (vi < 0 || vi >= static_cast<int>(out.size())) continue;
            auto& p = out[static_cast<size_t>(vi)];
            int slot = -1;
            float minw = p.weights[0];
            int minis = 0;
            for (int s = 0; s < 4; ++s) {
                if (p.weights[s] <= 0.0001f && p.bones[s] == 0 && s > 0) { slot = s; break; }
                if (s == 0 && p.weights[0] == 1.f && p.bones[0] == 0 && p.weights[1] == 0.f) {
                    slot = 0;
                    break;
                }
                if (p.weights[s] < minw) {
                    minw = p.weights[s];
                    minis = s;
                }
            }
            if (slot < 0) {
                if (inf.weights[k] > minw) slot = minis;
                else continue;
            }
            p.bones[slot] = static_cast<uint16_t>(bone);
            p.weights[slot] = inf.weights[k];
        }
    }
    for (auto& p : out) {
        float s = p.weights[0] + p.weights[1] + p.weights[2] + p.weights[3];
        if (s > 0.0001f) {
            p.weights[0] /= s; p.weights[1] /= s; p.weights[2] /= s; p.weights[3] /= s;
        } else {
            p.weights[0] = 1;
        }
    }
    return out;
}

} // namespace gdx
