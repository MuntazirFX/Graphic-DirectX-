#pragma once

#include "gdx/gdx.h"
#include <string>

namespace gdx {

struct HostConfig {
    std::string gameDir = ".";
    std::string archive;     // xmas.xpk / bb.xpk
    std::string levelDat;    // levels\\000.dat inside archive or on disk
    std::string meshX;       // fallback mesh
    int width = 320;
    int height = 240;
    std::string ppmOut = "frame.ppm";
};

struct Host {
    TitleInfo title;
    XpkArchive xpk;
    ElementCatalog catalog;
    DatLevel level;
    GameSession session;
    Document meshDoc;
    Device8 ref;
    std::string log;

    void print(const std::string& line);
    bool boot(const HostConfig& cfg);
    bool frame();
};

} // namespace gdx
