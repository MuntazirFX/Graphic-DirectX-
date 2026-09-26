#include "gdx/Host.h"
#include <iostream>

int main(int argc, char** argv) {
    gdx::HostConfig cfg;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-game" && i + 1 < argc) cfg.gameDir = argv[++i];
        else if (a == "-pak" && i + 1 < argc) cfg.archive = argv[++i];
        else if (a == "-map" && i + 1 < argc) cfg.levelDat = argv[++i];
        else if (a == "-model" && i + 1 < argc) cfg.meshX = argv[++i];
        else if (a == "-o" && i + 1 < argc) cfg.ppmOut = argv[++i];
    }
    gdx::Host host;
    if (!host.boot(cfg)) {
        std::cerr << host.log;
        return 2;
    }
    if (!host.frame()) {
        std::cerr << host.log << "ref_soft: empty frame\n";
        return 3;
    }
    host.ref.savePPM(cfg.ppmOut);
    std::cout << host.log;
    std::cout << "ref_soft: lit=" << host.ref.litPixels() << " " << cfg.ppmOut << "\n";
    return 0;
}
