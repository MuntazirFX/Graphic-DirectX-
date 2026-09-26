#include "gdx/gdx.h"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: datdump level.dat [elements.txt]\n";
        return 1;
    }
    if (argc >= 3) {
        std::ifstream in(argv[2]);
        std::ostringstream ss;
        ss << in.rdbuf();
        gdx::ElementCatalog::shared().parse(ss.str());
    }
    gdx::DatLevel lvl;
    if (!gdx::parseDatLevelFile(argv[1], lvl)) {
        std::cerr << "error: " << lvl.error << "\n";
        return 2;
    }
    std::cout << "entities: " << lvl.count << "\n";
    for (const auto& e : lvl.entities) {
        const gdx::ElementDef* d = gdx::ElementCatalog::shared().find(e.name);
        std::cout << "  " << e.name << " pos=" << e.x << "," << e.y << "," << e.z
                  << " type=" << (d ? d->type : "UNKNOWN") << "\n";
    }
    return 0;
}
