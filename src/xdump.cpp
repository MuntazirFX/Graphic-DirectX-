#include "gdx/XParser.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: xdump file.x\n";
        return 1;
    }
    gdx::Document doc;
    std::string err;
    if (!gdx::parseFile(argv[1], doc, err, gdx::ConvertOptions::none())) {
        std::cerr << "error: " << err << "\n";
        return 2;
    }
    std::cout << "header: " << doc.header << "\n";
    std::cout << "frames: " << doc.frames.size() << "\n";
    std::cout << "meshes: " << doc.meshes.size() << "\n";
    for (const auto& m : doc.meshes) {
        std::cout << "  mesh '" << m.name << "' verts=" << m.positions.size()
                  << " tris=" << m.indices.size() / 3
                  << " uvs=" << m.uvs.size()
                  << " nrm=" << m.normals.size()
                  << " skins=" << m.skins.size()
                  << " mats=" << m.materials.size() << "\n";
        for (const auto& s : m.skins)
            std::cout << "    bone " << s.boneName << " n=" << s.vertices.size() << "\n";
        for (const auto& mat : m.materials)
            std::cout << "    mat " << mat.name << " tex=" << mat.texture << "\n";
    }
    return 0;
}
