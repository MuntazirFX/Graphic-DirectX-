#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace gdx {

struct Vec2 { float x = 0, y = 0; };
struct Vec3 { float x = 0, y = 0, z = 0; };
struct Vec4 { float x = 0, y = 0, z = 0, w = 0; };

struct Mat4 {
    // row-major, matching .x FrameTransformMatrix order
    float m[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
};

struct ColorRGBA {
    float r = 1, g = 1, b = 1, a = 1;
};

struct Material {
    std::string name;
    ColorRGBA face;
    float power = 0;
    ColorRGBA specular;
    ColorRGBA emissive;
    std::string texture; // TextureFilename, often .dds
};

struct SkinInfluence {
    std::string boneName;
    std::vector<int> vertices;
    std::vector<float> weights;
    Mat4 offset;
};

struct Mesh {
    std::string name;
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;
    std::vector<uint32_t> indices; // triangles
    std::vector<int> materialOfFace; // per original face, before tri fan expand matches tri count after load
    std::vector<int> materialOfTri;
    std::vector<Material> materials;
    std::vector<SkinInfluence> skins;
    int maxWeightsPerVertex = 0;
    int numBones = 0;
    int faceCountRaw = 0;
};

struct Frame {
    std::string name;
    Mat4 local;
    int parent = -1;
    int meshIndex = -1; // mesh attached to this frame, if any
};

struct Document {
    bool binary = false;
    std::string header;
    std::vector<Mesh> meshes;
    std::vector<Frame> frames;
    std::vector<Material> looseMaterials;

    const Mesh* firstMesh() const {
        return meshes.empty() ? nullptr : &meshes[0];
    }
};

struct ConvertOptions {
    bool flipZ = false;
    bool flipWinding = false;

    static ConvertOptions none() { return {}; }
    static ConvertOptions metalFromD3D() {
        ConvertOptions o;
        o.flipZ = true;
        o.flipWinding = true;
        return o;
    }
};

} // namespace gdx
