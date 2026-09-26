#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gdx {

struct Vec2 { float x = 0, y = 0; };
struct Vec3 { float x = 0, y = 0, z = 0; };
struct Vec4 { float x = 0, y = 0, z = 0, w = 1; };

struct Mat4 {
    float m[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
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
    std::string texture;
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
    std::vector<uint32_t> indices;
    std::vector<int> materialOfFace;
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
    int meshIndex = -1;
};

enum class AnimKeyType { Rotation = 0, Scale = 1, Position = 2, Matrix = 4 };

struct AnimKey {
    AnimKeyType type = AnimKeyType::Matrix;
    int time = 0;
    Vec4 quat{};     // rotation
    Vec3 vec{};      // pos / scale
    Mat4 matrix{};
};

struct AnimTrack {
    std::string frameName;
    std::vector<AnimKey> keys;
};

struct AnimationSet {
    std::string name;
    std::vector<AnimTrack> tracks;
};

struct Document {
    bool binary = false;
    std::string header;
    std::vector<Mesh> meshes;
    std::vector<Frame> frames;
    std::vector<Material> looseMaterials;
    std::vector<AnimationSet> animations;

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
