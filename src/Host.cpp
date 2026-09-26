#include "gdx/Host.h"
#include "gdx/EngineName.h"
#include <sstream>

namespace gdx {

void Host::print(const std::string& line) {
    log += line;
    log += '\n';
}

bool Host::boot(const HostConfig& cfg) {
    log.clear();
    print(kEngineTag);
    print(std::string("credits: ") + kProgrammers);

    if (!cfg.archive.empty()) {
        if (!parseXpkFile(cfg.archive, xpk)) {
            print("fs: failed " + cfg.archive + " " + xpk.error);
            return false;
        }
        title = titleInfo(detectTitle(xpk, cfg.archive));
        print(std::string("fs: mounted ") + cfg.archive + " files=" + std::to_string(xpk.count));
        print(std::string("game: ") + title.fullName);
        auto el = xpk.extract("data\\elements.txt");
        if (el.empty()) el = xpk.extract("elements.txt");
        if (!el.empty()) {
            catalog.parse(std::string(el.begin(), el.end()));
            print("game: elements " + std::to_string(catalog.size()));
        }
    } else {
        title = titleInfo(TitleId::Scit2002);
        print("fs: no archive (loose files)");
    }

    if (!cfg.levelDat.empty()) {
        std::vector<uint8_t> bytes;
        if (xpk.valid) bytes = xpk.extract(cfg.levelDat);
        if (bytes.empty()) {
            if (!parseDatLevelFile(cfg.levelDat, level))
                print("world: " + level.error);
        } else {
            parseDatLevel(bytes.data(), bytes.size(), level);
        }
        if (level.valid) {
            buildSession(level, catalog, session);
            print("world: entities " + std::to_string(level.count) +
                  " presents " + std::to_string(session.totalPresents));
        }
    }

    std::string err;
    bool gotMesh = false;
    if (!cfg.meshX.empty()) {
        if (xpk.valid) {
            auto xb = xpk.extract(cfg.meshX);
            if (!xb.empty())
                gotMesh = parseBytes(xb.data(), xb.size(), meshDoc, err);
        }
        if (!gotMesh)
            gotMesh = parseFile(cfg.meshX, meshDoc, err);
    }
    if (!gotMesh)
        gotMesh = parseFile("samples/cube.x", meshDoc, err);
    if (!gotMesh) {
        print("model: " + err);
        return false;
    }
    print("model: meshes " + std::to_string(meshDoc.meshes.size()));

    ref.reset(cfg.width, cfg.height);
    ref.setView(translation(0, 0, 4));
    print("ref_soft: " + std::to_string(cfg.width) + "x" + std::to_string(cfg.height));
    return true;
}

bool Host::frame() {
    if (!meshDoc.firstMesh()) return false;
    DrawItem item;
    meshToDraw(*meshDoc.firstMesh(), item);
    ref.draw(item);
    return ref.litPixels() > 0;
}

} // namespace gdx
