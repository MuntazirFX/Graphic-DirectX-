#pragma once

#include "gdx/Xpk.h"
#include <string>

namespace gdx {

enum class TitleId {
    Unknown = 0,
    Scit2002,     // Santa Claus in Trouble (xmas.xpk)
    ScitAgain,    // Santa Claus in Trouble ... again! (xmas.xpk)
    Rosso,        // Rosso Rabbit in Trouble (bb.xpk)
    ScitHd        // Santa Claus in Trouble HD (data.pak, not XPK)
};

struct TitleInfo {
    TitleId id = TitleId::Unknown;
    const char* shortName = "unknown";
    const char* fullName = "Unknown";
    const char* archiveName = "";
    const char* exeName = "";
    bool xpkFamily = false;
};

TitleInfo titleInfo(TitleId id);
TitleId detectTitleFromPath(const std::string& path);
TitleId detectTitle(const XpkArchive& arc, const std::string& path = {});

} // namespace gdx
