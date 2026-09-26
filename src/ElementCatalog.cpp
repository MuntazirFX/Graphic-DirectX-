#include "gdx/ElementCatalog.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace gdx {
namespace {

std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
    return s;
}

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::string toMesh(const std::string& file) {
    std::string out = file;
    auto p = out.rfind('.');
    if (p != std::string::npos) {
        std::string ext = lower(out.substr(p));
        if (ext == ".ani") out.replace(p, std::string::npos, ".x");
    }
    return out;
}

} // namespace

size_t ElementCatalog::parse(const std::string& text) {
    defs_.clear();
    byName_.clear();
    byLower_.clear();
    ElementDef cur;
    bool open = false;
    std::istringstream in(text);
    std::string line;
    auto flush = [&]() {
        if (!open || cur.name.empty()) return;
        cur.meshFile = toMesh(cur.file);
        if (lower(cur.file).size() >= 4 && lower(cur.file).substr(lower(cur.file).size() - 4) == ".ani")
            cur.aniFile = cur.file;
        byName_[cur.name] = defs_.size();
        byLower_[lower(trim(cur.name))] = defs_.size();
        defs_.push_back(cur);
        cur = {};
        open = false;
    };
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ls(line);
        std::string key;
        ls >> key;
        for (char& c : key) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        std::string rest;
        std::getline(ls, rest);
        rest = trim(rest);
        if (key == "ELEMENT") {
            flush();
            cur.name = trim(rest);
            open = true;
        } else if (key == "FILE") cur.file = trim(rest);
        else if (key == "TYPE") cur.type = trim(rest);
        else if (key == "EFFECT") cur.effect = trim(rest);
        else if (key == "RADIUS") cur.radius = std::strtof(rest.c_str(), nullptr);
        else if (key == "SCALING") cur.scaling = std::strtof(rest.c_str(), nullptr);
        else if (key == "SPEED") cur.speed = std::strtof(rest.c_str(), nullptr);
        else if (key == "JUMPHEIGHT") cur.jumpHeight = std::strtof(rest.c_str(), nullptr);
        else if (key == "VERTICALOFFSET") cur.verticalOffset = std::strtof(rest.c_str(), nullptr);
        else if (key == "FRICTION") cur.friction = std::strtof(rest.c_str(), nullptr);
        else if (key == "ROTATION") cur.rotation = std::strtof(rest.c_str(), nullptr);
        else if (key == "WALKANIM") cur.walkAnim = std::atoi(rest.c_str());
    }
    flush();
    return defs_.size();
}

const ElementDef* ElementCatalog::find(const std::string& name) const {
    auto it = byName_.find(name);
    if (it != byName_.end()) return &defs_[it->second];
    auto it2 = byLower_.find(lower(trim(name)));
    if (it2 != byLower_.end()) return &defs_[it2->second];
    return nullptr;
}

ElementCatalog& ElementCatalog::shared() {
    static ElementCatalog c;
    return c;
}

} // namespace gdx
