#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdx {

struct XpkEntry {
    std::string name;
    uint32_t offset = 0;
    uint32_t size = 0;
    uint32_t time = 0;
};

struct XpkArchive {
    bool valid = false;
    std::string error;
    uint32_t count = 0;
    uint32_t dataStart = 0;
    uint32_t totalDataSize = 0;
    std::vector<XpkEntry> files;
    std::unordered_map<std::string, size_t> byKey;
    std::vector<uint8_t> bytes; // full archive kept for extract

    const XpkEntry* find(const std::string& name) const;
    std::vector<uint8_t> extract(const std::string& name) const;
};

std::string xpkNormalize(const std::string& name);
bool parseXpk(const void* data, size_t size, XpkArchive& out);
bool parseXpkFile(const std::string& path, XpkArchive& out);

} // namespace gdx
