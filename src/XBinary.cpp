#include "gdx/XBinary.h"
#include "gdx/XParser.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace gdx {
namespace {

enum : uint16_t {
    TOK_NAME = 1,
    TOK_STRING = 2,
    TOK_INTEGER = 3,
    TOK_GUID = 5,
    TOK_INTEGER_LIST = 6,
    TOK_FLOAT_LIST = 7,
    TOK_OBRACE = 10,
    TOK_CBRACE = 11,
    TOK_COMMA = 19,
    TOK_SEMICOLON = 20,
    TOK_TEMPLATE = 31
};

struct Bin {
    const uint8_t* p = nullptr;
    const uint8_t* end = nullptr;
    bool f64 = false;

    bool need(size_t n) const { return p + n <= end; }

    uint16_t u16() {
        if (!need(2)) return 0;
        uint16_t v;
        std::memcpy(&v, p, 2);
        p += 2;
        return v;
    }
    uint32_t u32() {
        if (!need(4)) return 0;
        uint32_t v;
        std::memcpy(&v, p, 4);
        p += 4;
        return v;
    }
    float f32() {
        if (f64) {
            if (!need(8)) return 0;
            double d;
            std::memcpy(&d, p, 8);
            p += 8;
            return static_cast<float>(d);
        }
        if (!need(4)) return 0;
        float v;
        std::memcpy(&v, p, 4);
        p += 4;
        return v;
    }

    bool peekTok(uint16_t& t) const {
        if (!need(2)) return false;
        std::memcpy(&t, p, 2);
        return true;
    }
};

struct NumBuf {
    std::vector<float> f;
    std::vector<int> i;
    size_t fi = 0, ii = 0;

    void reset() {
        f.clear();
        i.clear();
        fi = ii = 0;
    }
    bool hasI() const { return ii < i.size(); }
    bool hasF() const { return fi < f.size(); }
    int nextI() { return ii < i.size() ? i[ii++] : 0; }
    float nextF() { return fi < f.size() ? f[fi++] : 0.f; }
};

bool readTokenName(Bin& b, std::string& name) {
    uint32_t n = b.u32();
    if (!b.need(n)) return false;
    name.assign(reinterpret_cast<const char*>(b.p), n);
    b.p += n;
    while (!name.empty() && name.back() == '\0') name.pop_back();
    return true;
}

bool readTokenString(Bin& b, std::string& s) {
    uint32_t n = b.u32();
    if (!b.need(n)) return false;
    s.assign(reinterpret_cast<const char*>(b.p), n);
    b.p += n;
    while (!s.empty() && s.back() == '\0') s.pop_back();
    uint16_t term;
    if (b.peekTok(term) && (term == TOK_COMMA || term == TOK_SEMICOLON))
        b.u16();
    return true;
}

bool slurpList(Bin& b, uint16_t tok, NumBuf& nb) {
    if (tok == TOK_INTEGER_LIST) {
        uint32_t n = b.u32();
        if (!b.need(n * 4u)) return false;
        nb.i.reserve(nb.i.size() + n);
        for (uint32_t k = 0; k < n; ++k) nb.i.push_back(static_cast<int>(b.u32()));
        return true;
    }
    if (tok == TOK_FLOAT_LIST) {
        uint32_t n = b.u32();
        const size_t es = b.f64 ? 8u : 4u;
        if (!b.need(n * es)) return false;
        nb.f.reserve(nb.f.size() + n);
        for (uint32_t k = 0; k < n; ++k) nb.f.push_back(b.f32());
        return true;
    }
    if (tok == TOK_INTEGER) {
        nb.i.push_back(static_cast<int>(b.u32()));
        return true;
    }
    return false;
}

void skipBlock(Bin& b) {
    int depth = 1;
    while (b.p < b.end && depth > 0) {
        uint16_t t = b.u16();
        if (t == TOK_OBRACE) ++depth;
        else if (t == TOK_CBRACE) --depth;
        else if (t == TOK_NAME) {
            std::string dummy;
            readTokenName(b, dummy);
        } else if (t == TOK_STRING) {
            std::string dummy;
            readTokenString(b, dummy);
        } else if (t == TOK_INTEGER) {
            b.u32();
        } else if (t == TOK_GUID) {
            if (b.need(16)) b.p += 16;
        } else if (t == TOK_INTEGER_LIST) {
            uint32_t n = b.u32();
            if (b.need(n * 4u)) b.p += n * 4u;
        } else if (t == TOK_FLOAT_LIST) {
            uint32_t n = b.u32();
            size_t es = b.f64 ? 8u : 4u;
            if (b.need(n * es)) b.p += n * es;
        }
    }
}

void fillNums(Bin& b, NumBuf& nb) {
    uint16_t t;
    while (b.peekTok(t)) {
        if (t == TOK_CBRACE || t == TOK_NAME || t == TOK_STRING || t == TOK_OBRACE)
            break;
        if (t == TOK_COMMA || t == TOK_SEMICOLON) {
            b.u16();
            continue;
        }
        b.u16();
        if (!slurpList(b, t, nb)) break;
    }
}

bool parseMaterialBin(Bin& b, Material& mat) {
    NumBuf nb;
    fillNums(b, nb);
    mat.face.r = nb.nextF(); mat.face.g = nb.nextF(); mat.face.b = nb.nextF(); mat.face.a = nb.nextF();
    if (nb.hasI() && !nb.hasF()) {
        // some files pack colors as ints; ignore
    }
    mat.power = nb.nextF();
    mat.specular.r = nb.nextF(); mat.specular.g = nb.nextF(); mat.specular.b = nb.nextF();
    mat.emissive.r = nb.nextF(); mat.emissive.g = nb.nextF(); mat.emissive.b = nb.nextF();

    uint16_t t;
    while (b.peekTok(t) && t != TOK_CBRACE) {
        if (t == TOK_NAME) {
            b.u16();
            std::string id;
            readTokenName(b, id);
            std::string inst;
            uint16_t t2;
            if (b.peekTok(t2) && t2 == TOK_NAME) {
                b.u16();
                readTokenName(b, inst);
            }
            if (b.peekTok(t2) && t2 == TOK_OBRACE) {
                b.u16();
                if (id == "TextureFilename") {
                    uint16_t t3;
                    if (b.peekTok(t3) && t3 == TOK_STRING) {
                        b.u16();
                        readTokenString(b, mat.texture);
                    }
                    skipBlock(b); // from inside? we already consumed OBRACE
                    // skipBlock assumes we're after OBRACE with depth 1 starting... we consumed OBRACE so skipBlock is correct if we set depth 1 from current
                    // wait skipBlock reads until matching close from depth 1, and we already ate OBRACE so call skipBlock.
                } else {
                    skipBlock(b);
                }
            }
        } else {
            b.u16();
            if (t == TOK_STRING) {
                std::string dummy;
                readTokenString(b, dummy);
            }
        }
    }
    return true;
}

bool parseMeshBin(Bin& b, Mesh& mesh) {
    NumBuf nb;
    fillNums(b, nb);

    int nverts = nb.nextI();
    if (nverts < 0) nverts = 0;
    mesh.positions.resize(static_cast<size_t>(nverts));
    for (int i = 0; i < nverts; ++i) {
        mesh.positions[i].x = nb.nextF();
        mesh.positions[i].y = nb.nextF();
        mesh.positions[i].z = nb.nextF();
    }
    int nfaces = nb.nextI();
    if (nfaces < 0) nfaces = 0;
    mesh.faceCountRaw = nfaces;
    for (int f = 0; f < nfaces; ++f) {
        int nidx = nb.nextI();
        std::vector<uint32_t> poly;
        for (int k = 0; k < nidx; ++k)
            poly.push_back(static_cast<uint32_t>(nb.nextI()));
        for (size_t k = 2; k < poly.size(); ++k) {
            mesh.indices.push_back(poly[0]);
            mesh.indices.push_back(poly[k - 1]);
            mesh.indices.push_back(poly[k]);
            mesh.materialOfTri.push_back(0);
        }
    }

    uint16_t t;
    while (b.peekTok(t) && t != TOK_CBRACE) {
        if (t != TOK_NAME) {
            b.u16();
            continue;
        }
        b.u16();
        std::string id;
        readTokenName(b, id);
        std::string inst;
        if (b.peekTok(t) && t == TOK_NAME) {
            b.u16();
            readTokenName(b, inst);
        }
        if (!(b.peekTok(t) && t == TOK_OBRACE)) continue;
        b.u16();

        if (id == "MeshNormals") {
            NumBuf n2;
            fillNums(b, n2);
            int nn = n2.nextI();
            mesh.normals.resize(static_cast<size_t>(std::max(nn, 0)));
            for (int i = 0; i < nn; ++i) {
                mesh.normals[i].x = n2.nextF();
                mesh.normals[i].y = n2.nextF();
                mesh.normals[i].z = n2.nextF();
            }
            skipBlock(b);
        } else if (id == "MeshTextureCoords") {
            NumBuf n2;
            fillNums(b, n2);
            int nu = n2.nextI();
            mesh.uvs.resize(static_cast<size_t>(std::max(nu, 0)));
            for (int i = 0; i < nu; ++i) {
                mesh.uvs[i].x = n2.nextF();
                mesh.uvs[i].y = n2.nextF();
            }
            skipBlock(b);
        } else if (id == "MeshMaterialList") {
            NumBuf n2;
            fillNums(b, n2);
            int nmat = n2.nextI();
            int nfm = n2.nextI();
            mesh.materialOfFace.resize(static_cast<size_t>(std::max(nfm, 0)));
            for (int i = 0; i < nfm; ++i) mesh.materialOfFace[i] = n2.nextI();
            (void)nmat;
            while (b.peekTok(t) && t != TOK_CBRACE) {
                if (t == TOK_NAME) {
                    b.u16();
                    std::string mid;
                    readTokenName(b, mid);
                    std::string minst;
                    if (b.peekTok(t) && t == TOK_NAME) {
                        b.u16();
                        readTokenName(b, minst);
                    }
                    if (b.peekTok(t) && t == TOK_OBRACE) {
                        b.u16();
                        if (mid == "Material") {
                            Material mat;
                            mat.name = minst;
                            parseMaterialBin(b, mat);
                            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
                            mesh.materials.push_back(mat);
                        } else {
                            skipBlock(b);
                        }
                    }
                } else if (t == TOK_OBRACE) {
                    b.u16();
                    skipBlock(b);
                } else {
                    b.u16();
                }
            }
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
        } else if (id == "XSkinMeshHeader") {
            NumBuf n2;
            fillNums(b, n2);
            mesh.maxWeightsPerVertex = n2.nextI();
            n2.nextI();
            mesh.numBones = n2.nextI();
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
        } else if (id == "SkinWeights") {
            SkinInfluence inf;
            if (b.peekTok(t) && t == TOK_STRING) {
                b.u16();
                readTokenString(b, inf.boneName);
            }
            NumBuf n2;
            fillNums(b, n2);
            int n = n2.nextI();
            inf.vertices.resize(static_cast<size_t>(std::max(n, 0)));
            inf.weights.resize(inf.vertices.size());
            for (int i = 0; i < n; ++i) inf.vertices[i] = n2.nextI();
            for (int i = 0; i < n; ++i) inf.weights[i] = n2.nextF();
            for (int i = 0; i < 16; ++i) inf.offset.m[i] = n2.nextF();
            mesh.skins.push_back(std::move(inf));
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
        } else {
            skipBlock(b);
        }
    }
    return true;
}

bool parseFrameBin(Bin& b, Document& doc, int parent, const std::string& name) {
    Frame fr;
    fr.name = name;
    fr.parent = parent;
    int self = static_cast<int>(doc.frames.size());
    doc.frames.push_back(fr);

    uint16_t t;
    while (b.peekTok(t) && t != TOK_CBRACE) {
        if (t != TOK_NAME) {
            b.u16();
            continue;
        }
        b.u16();
        std::string id;
        readTokenName(b, id);
        std::string inst;
        if (b.peekTok(t) && t == TOK_NAME) {
            b.u16();
            readTokenName(b, inst);
        }
        if (!(b.peekTok(t) && t == TOK_OBRACE)) continue;
        b.u16();

        if (id == "FrameTransformMatrix") {
            NumBuf n2;
            fillNums(b, n2);
            for (int i = 0; i < 16; ++i) doc.frames[self].local.m[i] = n2.nextF();
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
        } else if (id == "Frame") {
            parseFrameBin(b, doc, self, inst);
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
        } else if (id == "Mesh") {
            Mesh mesh;
            mesh.name = inst;
            parseMeshBin(b, mesh);
            if (b.peekTok(t) && t == TOK_CBRACE) b.u16();
            doc.frames[self].meshIndex = static_cast<int>(doc.meshes.size());
            doc.meshes.push_back(std::move(mesh));
        } else {
            skipBlock(b);
        }
    }
    return true;
}

} // namespace

bool parseBinary(const void* data, size_t size, Document& out, std::string& error, ConvertOptions opt) {
    out = {};
    out.binary = true;
    if (!data || size < 16) {
        error = "binary .x too small";
        return false;
    }
    const auto* c = static_cast<const uint8_t*>(data);
    out.header.assign(reinterpret_cast<const char*>(c), 16);

    Bin b;
    b.p = c + 16;
    b.end = c + size;
    b.f64 = std::memcmp(c + 12, "0064", 4) == 0;

    while (b.p + 2 <= b.end) {
        uint16_t t = b.u16();
        if (t == TOK_TEMPLATE) {
            // name + guid + block
            uint16_t t2;
            if (b.peekTok(t2) && t2 == TOK_NAME) {
                b.u16();
                std::string n;
                readTokenName(b, n);
            }
            if (b.peekTok(t2) && t2 == TOK_OBRACE) {
                b.u16();
                skipBlock(b);
            } else {
                // guid then brace
                if (b.peekTok(t2) && t2 == TOK_GUID) {
                    b.u16();
                    if (b.need(16)) b.p += 16;
                }
                if (b.peekTok(t2) && t2 == TOK_OBRACE) {
                    b.u16();
                    skipBlock(b);
                }
            }
            continue;
        }
        if (t != TOK_NAME) continue;
        std::string id;
        if (!readTokenName(b, id)) break;
        std::string inst;
        uint16_t t2;
        if (b.peekTok(t2) && t2 == TOK_NAME) {
            b.u16();
            readTokenName(b, inst);
        }
        if (!(b.peekTok(t2) && t2 == TOK_OBRACE)) continue;
        b.u16();

        if (id == "Frame") {
            parseFrameBin(b, out, -1, inst);
            if (b.peekTok(t2) && t2 == TOK_CBRACE) b.u16();
        } else if (id == "Mesh") {
            Mesh mesh;
            mesh.name = inst;
            parseMeshBin(b, mesh);
            if (b.peekTok(t2) && t2 == TOK_CBRACE) b.u16();
            out.meshes.push_back(std::move(mesh));
        } else if (id == "Material") {
            Material mat;
            mat.name = inst;
            parseMaterialBin(b, mat);
            if (b.peekTok(t2) && t2 == TOK_CBRACE) b.u16();
            out.looseMaterials.push_back(mat);
        } else {
            skipBlock(b);
        }
    }

    // reuse text-path convert via parseText's apply? duplicate small call by going through parseBytes after
    // apply here by calling parseText convert: we don't have applyConvert exported.
    // Flip locally:
    if (opt.flipZ || opt.flipWinding) {
        for (auto& mesh : out.meshes) {
            if (opt.flipZ) {
                for (auto& v : mesh.positions) v.z = -v.z;
                for (auto& n : mesh.normals) n.z = -n.z;
            }
            if (opt.flipWinding) {
                for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
                    std::swap(mesh.indices[i + 1], mesh.indices[i + 2]);
            }
        }
    }
    (void)error;
    return !out.meshes.empty() || !out.frames.empty();
}

} // namespace gdx
