#include "gdx/gdx.h"
#include <iostream>

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "samples/cube.x";
    gdx::Document doc;
    std::string err;
    if (!gdx::parseFile(path, doc, err) || !doc.firstMesh()) {
        std::cerr << err << "\n";
        return 2;
    }
    gdx::DrawItem item;
    gdx::meshToDraw(*doc.firstMesh(), item);
    gdx::Device8 dev;
    dev.reset(320, 240);
    dev.setView(gdx::translation(0, 0, 4));
    dev.draw(item);
    const char* out = argc > 2 ? argv[2] : "cube.ppm";
    dev.savePPM(out);
    std::cout << "lit=" << dev.litPixels() << " wrote " << out << "\n";
    return dev.litPixels() > 10 ? 0 : 3;
}
