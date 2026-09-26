#include "gdx/XParser.h"
#include "gdx/XBinary.h"

#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>

namespace gdx {
namespace {

struct Tok {
    const char* p = nullptr;
    const char* end = nullptr;

    void skip() {
        for (;;) {
            while (p < end && std::isspace(static_cast<unsigned char>(*p))) ++p;
            if (p + 1 < end && p[0] == '/' && p[1] == '/') {
                while (p < end && *p != '\n') ++p;
                continue;
            }
            if (p < end && *p == '#') {
                while (p < end && *p != '\n') ++p;
                continue;
            }
            break;
        }
    }

    bool done() {
        skip();
        return p >= end;
    }

    bool consume(char c) {
        skip();
        if (p < end && *p == c) {
            ++p;
            return true;
        }
        return false;
    }

    bool matchIdent(const char* s) {
        skip();
        const char* q = p;
        while (*s && q < end && *q == *s) {
            ++q;
            ++s;
        }
        if (*s) return false;
        if (q < end && (std::isalnum(static_cast<unsigned char>(*q)) || *q == '_'))
            return false;
        p = q;
        return true;
    }

    std::string ident() {
        skip();
        const char* s = p;
        if (p < end && (std::isalpha(static_cast<unsigned char>(*p)) || *p == '_')) {
            ++p;
            while (p < end && (std::isalnum(static_cast<unsigned char>(*p)) || *p == '_' || *p == '-' || *p == '.'))
                ++p;
        }
        return std::string(s, p);
    }

    std::string quoted() {
        skip();
        if (p >= end || *p != '"') return {};
        ++p;
        const char* s = p;
        while (p < end && *p != '"') ++p;
        std::string out(s, p);
        if (p < end && *p == '"') ++p;
        return out;
    }

    bool number(float& v) {
        skip();
        char* next = nullptr;
        v = std::strtof(p, &next);
        if (next == p) return false;
        p = next;
        consume(';');
        consume(',');
        return true;
    }

    bool integer(int& v) {
        float f;
        if (!number(f)) return false;
        v = static_cast<int>(f);
        return true;
    }

    void skipBlock() {
        skip();
        if (!consume('{')) return;
        int depth = 1;
        while (p < end && depth) {
            if (*p == '{') ++depth;
            else if (*p == '}') --depth;
            ++p;
        }
    }
};

void applyConvert(Document& doc, const ConvertOptions& opt) {
    if (!opt.flipZ && !opt.flipWinding) return;
    auto flipV = [&](Vec3& v) {
        if (opt.flipZ) v.z = -v.z;
    };
    auto flipM = [&](Mat4& m) {
        if (!opt.flipZ) return;
        m.m[14] = -m.m[14];
        m.m[11] = -m.m[11];
    };
    for (auto& mesh : doc.meshes) {
        for (auto& p : mesh.positions) flipV(p);
        for (auto& n : mesh.normals) flipV(n);
        if (opt.flipWinding) {
            for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
                std::swap(mesh.indices[i + 1], mesh.indices[i + 2]);
        }
        for (auto& s : mesh.skins) flipM(s.offset);
    }
    for (auto& f : doc.frames) flipM(f.local);
}

bool parseMaterialBody(Tok& t, Material& mat) {
    t.number(mat.face.r);
    t.number(mat.face.g);
    t.number(mat.face.b);
    t.number(mat.face.a);
    t.number(mat.power);
    t.number(mat.specular.r);
    t.number(mat.specular.g);
    t.number(mat.specular.b);
    t.number(mat.emissive.r);
    t.number(mat.emissive.g);
    t.number(mat.emissive.b);
    while (!t.done() && *t.p != '}') {
        if (t.matchIdent("TextureFilename")) {
            t.consume('{');
            mat.texture = t.quoted();
            t.consume(';');
            t.consume('}');
        } else if (t.consume('{')) {
            int depth = 1;
            while (t.p < t.end && depth) {
                if (*t.p == '{') ++depth;
                else if (*t.p == '}') --depth;
                ++t.p;
            }
        } else {
            ++t.p;
        }
    }
    return true;
}

bool parseMeshBody(Tok& t, Mesh& mesh) {
    int nverts = 0;
    if (!t.integer(nverts) || nverts < 0) return false;
    mesh.positions.resize(static_cast<size_t>(nverts));
    for (int i = 0; i < nverts; ++i) {
        t.number(mesh.positions[i].x);
        t.number(mesh.positions[i].y);
        t.number(mesh.positions[i].z);
    }
    int nfaces = 0;
    if (!t.integer(nfaces) || nfaces < 0) return false;
    mesh.faceCountRaw = nfaces;
    mesh.indices.clear();
    mesh.materialOfFace.assign(static_cast<size_t>(nfaces), 0);
    for (int f = 0; f < nfaces; ++f) {
        int nidx = 0;
        t.integer(nidx);
        std::vector<uint32_t> poly;
        poly.reserve(static_cast<size_t>(nidx));
        for (int k = 0; k < nidx; ++k) {
            int id = 0;
            t.integer(id);
            poly.push_back(static_cast<uint32_t>(id));
        }
        for (size_t k = 2; k < poly.size(); ++k) {
            mesh.indices.push_back(poly[0]);
            mesh.indices.push_back(poly[k - 1]);
            mesh.indices.push_back(poly[k]);
            mesh.materialOfTri.push_back(0);
        }
    }
    while (!t.done() && *t.p != '}') {
        t.skip();
        if (t.p >= t.end || *t.p == '}') break;
        if (t.matchIdent("MeshNormals")) {
            t.consume('{');
            int nn = 0;
            t.integer(nn);
            mesh.normals.resize(static_cast<size_t>(std::max(nn, 0)));
            for (int i = 0; i < nn; ++i) {
                t.number(mesh.normals[i].x);
                t.number(mesh.normals[i].y);
                t.number(mesh.normals[i].z);
            }
            int nf = 0;
            t.integer(nf);
            for (int i = 0; i < nf; ++i) {
                int nidx = 0;
                t.integer(nidx);
                for (int k = 0; k < nidx; ++k) {
                    int dummy = 0;
                    t.integer(dummy);
                }
            }
            t.consume('}');
            continue;
        }
        if (t.matchIdent("MeshTextureCoords")) {
            t.consume('{');
            int nu = 0;
            t.integer(nu);
            mesh.uvs.resize(static_cast<size_t>(std::max(nu, 0)));
            for (int i = 0; i < nu; ++i) {
                t.number(mesh.uvs[i].x);
                t.number(mesh.uvs[i].y);
            }
            t.consume('}');
            continue;
        }
        if (t.matchIdent("MeshMaterialList")) {
            t.consume('{');
            int nmat = 0, nfaceMat = 0;
            t.integer(nmat);
            t.integer(nfaceMat);
            std::vector<int> faceMat(static_cast<size_t>(std::max(nfaceMat, 0)), 0);
            for (int i = 0; i < nfaceMat; ++i) t.integer(faceMat[i]);
            mesh.materialOfFace = faceMat;
            if (!mesh.materialOfTri.empty() && !faceMat.empty()) {
                for (size_t i = 0; i < mesh.materialOfTri.size(); ++i)
                    mesh.materialOfTri[i] = faceMat[std::min(i, faceMat.size() - 1)];
            }
            while (!t.done() && *t.p != '}') {
                if (t.matchIdent("Material")) {
                    Material mat;
                    mat.name = t.ident();
                    t.consume('{');
                    parseMaterialBody(t, mat);
                    t.consume('}');
                    mesh.materials.push_back(mat);
                } else if (t.consume('{')) {
                    std::string ref = t.ident();
                    Material mat;
                    mat.name = ref;
                    mesh.materials.push_back(mat);
                    t.consume('}');
                } else {
                    t.skip();
                    if (t.p < t.end && *t.p != '}') ++t.p;
                    else break;
                }
            }
            t.consume('}');
            continue;
        }
        if (t.matchIdent("XSkinMeshHeader")) {
            t.consume('{');
            t.integer(mesh.maxWeightsPerVertex);
            int maxFace = 0;
            t.integer(maxFace);
            t.integer(mesh.numBones);
            t.consume('}');
            continue;
        }
        if (t.matchIdent("SkinWeights")) {
            t.consume('{');
            SkinInfluence inf;
            inf.boneName = t.quoted();
            int n = 0;
            t.integer(n);
            inf.vertices.resize(static_cast<size_t>(std::max(n, 0)));
            inf.weights.resize(inf.vertices.size());
            for (int i = 0; i < n; ++i) t.integer(inf.vertices[i]);
            for (int i = 0; i < n; ++i) t.number(inf.weights[i]);
            for (int i = 0; i < 16; ++i) t.number(inf.offset.m[i]);
            mesh.skins.push_back(std::move(inf));
            t.consume('}');
            continue;
        }
        std::string unk = t.ident();
        (void)unk;
        if (t.consume('{')) {
            int depth = 1;
            while (t.p < t.end && depth) {
                if (*t.p == '{') ++depth;
                else if (*t.p == '}') --depth;
                ++t.p;
            }
        } else if (!t.done()) {
            ++t.p;
        }
    }
    return true;
}

bool parseFrame(Tok& t, Document& doc, int parent) {
    Frame fr;
    fr.name = t.ident();
    fr.parent = parent;
    t.consume('{');
    int self = static_cast<int>(doc.frames.size());
    doc.frames.push_back(fr);
    while (!t.done() && *t.p != '}') {
        if (t.matchIdent("FrameTransformMatrix")) {
            t.consume('{');
            for (int i = 0; i < 16; ++i) t.number(doc.frames[self].local.m[i]);
            t.consume('}');
            continue;
        }
        if (t.matchIdent("Frame")) {
            parseFrame(t, doc, self);
            continue;
        }
        if (t.matchIdent("Mesh")) {
            Mesh mesh;
            mesh.name = t.ident();
            t.consume('{');
            parseMeshBody(t, mesh);
            t.consume('}');
            doc.frames[self].meshIndex = static_cast<int>(doc.meshes.size());
            doc.meshes.push_back(std::move(mesh));
            continue;
        }
        std::string unk = t.ident();
        (void)unk;
        if (t.consume('{')) {
            int depth = 1;
            while (t.p < t.end && depth) {
                if (*t.p == '{') ++depth;
                else if (*t.p == '}') --depth;
                ++t.p;
            }
        } else if (!t.done() && *t.p != '}') {
            ++t.p;
        }
    }
    t.consume('}');
    return true;
}

} // namespace

bool parseText(const std::string& text, Document& out, std::string& error, ConvertOptions opt) {
    out = {};
    Tok t;
    t.p = text.data();
    t.end = text.data() + text.size();
    t.skip();
    if (text.size() >= 16 && text.compare(0, 3, "xof") == 0)
        out.header = text.substr(0, 16);
    if (t.matchIdent("xof")) {
        while (t.p < t.end && *t.p != '\n') ++t.p;
    }
    while (!t.done()) {
        if (t.matchIdent("template")) {
            t.ident();
            t.skipBlock();
            continue;
        }
        if (t.matchIdent("Frame")) {
            parseFrame(t, out, -1);
            continue;
        }
        if (t.matchIdent("Mesh")) {
            Mesh mesh;
            mesh.name = t.ident();
            t.consume('{');
            if (!parseMeshBody(t, mesh)) {
                error = "Mesh parse failed";
                return false;
            }
            t.consume('}');
            out.meshes.push_back(std::move(mesh));
            continue;
        }
        if (t.matchIdent("Material")) {
            Material mat;
            mat.name = t.ident();
            t.consume('{');
            parseMaterialBody(t, mat);
            t.consume('}');
            out.looseMaterials.push_back(mat);
            continue;
        }
        std::string id = t.ident();
        if (id.empty()) {
            if (!t.done()) ++t.p;
            continue;
        }
        t.skipBlock();
    }
    applyConvert(out, opt);
    return true;
}

bool parseBytes(const void* data, size_t size, Document& out, std::string& error, ConvertOptions opt) {
    if (!data || size < 12) {
        error = "too small";
        return false;
    }
    const char* c = static_cast<const char*>(data);
    if (std::string(c, 4) != "xof ") {
        error = "not an .x file";
        return false;
    }
    const bool bin = size >= 16 && std::string(c + 8, 3) == "bin";
    if (bin) return parseBinary(data, size, out, error, opt);
    return parseText(std::string(c, c + size), out, error, opt);
}

bool parseFile(const std::string& path, Document& out, std::string& error, ConvertOptions opt) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    const std::string bytes = ss.str();
    return parseBytes(bytes.data(), bytes.size(), out, error, opt);
}

} // namespace gdx
