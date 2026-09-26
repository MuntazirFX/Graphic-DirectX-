#include "gdx/Title.h"
#include <algorithm>
#include <cctype>

namespace gdx {
namespace {

std::string low(std::string s) {
    for (char& c : s) {
        if (c == '\\' || c == '/') c = '/';
        else c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

bool hasName(const XpkArchive& a, const char* needle) {
    const std::string n = low(needle);
    for (const auto& f : a.files) {
        if (low(f.name).find(n) != std::string::npos) return true;
    }
    return false;
}

} // namespace

TitleInfo titleInfo(TitleId id) {
    switch (id) {
    case TitleId::Scit2002:
        return {id, "scit", "Santa Claus in Trouble (2002)", "xmas.xpk",
                "SantaClausInTrouble.exe", true};
    case TitleId::ScitAgain:
        return {id, "scit-again", "Santa Claus in Trouble ... again! (2004)", "xmas.xpk",
                "SantaClaus2.exe", true};
    case TitleId::Rosso:
        return {id, "rosso", "Rosso Rabbit in Trouble (2003)", "bb.xpk",
                "RossoRabbitInTrouble.exe", true};
    case TitleId::ScitHd:
        return {id, "scit-hd", "Santa Claus in Trouble HD (2020)", "data.pak",
                "SantaClausInTrouble.exe", false};
    default:
        return {};
    }
}

TitleId detectTitleFromPath(const std::string& path) {
    const std::string p = low(path);
    if (p.find("data.pak") != std::string::npos || p.find("scit-hd") != std::string::npos ||
        p.find("scithd") != std::string::npos)
        return TitleId::ScitHd;
    if (p.find("bb.xpk") != std::string::npos || p.find("rosso") != std::string::npos)
        return TitleId::Rosso;
    if (p.find("again") != std::string::npos || p.find("santaclaus2") != std::string::npos)
        return TitleId::ScitAgain;
    if (p.find("xmas.xpk") != std::string::npos || p.find("scit") != std::string::npos)
        return TitleId::Scit2002;
    return TitleId::Unknown;
}

TitleId detectTitle(const XpkArchive& arc, const std::string& path) {
    // Content beats filename: same xmas.xpk name is used by SCIT and Again.
    if (hasName(arc, "rosso") || hasName(arc, "carrot") || hasName(arc, "rabbit"))
        return TitleId::Rosso;
    if (hasName(arc, "again") || hasName(arc, "sc2") || hasName(arc, "santa2"))
        return TitleId::ScitAgain;
    // Again still uses winter/present assets; prefer path if given.
    TitleId fromPath = detectTitleFromPath(path);
    if (fromPath == TitleId::ScitAgain || fromPath == TitleId::Rosso || fromPath == TitleId::ScitHd)
        return fromPath;
    if (hasName(arc, "present") || hasName(arc, "winter") || hasName(arc, "santa"))
        return TitleId::Scit2002;
    if (fromPath != TitleId::Unknown) return fromPath;
    if (arc.valid && !arc.files.empty()) return TitleId::Scit2002;
    return TitleId::Unknown;
}

} // namespace gdx
