#include "gdx/Xpk.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <sstream>

namespace gdx {

std::string xpkNormalize(const std::string& name) {
    std::string k = name;
    for (char& c : k) {
        if (c == '/') c = '\\';
        else c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return k;
}

const XpkEntry* XpkArchive::find(const std::string& name) const {
    auto it = byKey.find(xpkNormalize(name));
    if (it == byKey.end()) return nullptr;
    return &files[it->second];
}

std::vector<uint8_t> XpkArchive::extract(const std::string& name) const {
    const XpkEntry* e = find(name);
    if (!e || e->offset + e->size > bytes.size()) return {};
    return std::vector<uint8_t>(bytes.begin() + e->offset, bytes.begin() + e->offset + e->size);
}

bool parseXpk(const void* data, size_t size, XpkArchive& out) {
    out = {};
    if (!data || size < 8) {
        out.error = "too small";
        return false;
    }
    const auto* b = static_cast<const uint8_t*>(data);
    auto rd32 = [&](size_t off, uint32_t& v) -> bool {
        if (off + 4 > size) return false;
        std::memcpy(&v, b + off, 4);
        return true;
    };

    size_t p = 0;
    uint32_t count = 0;
    if (!rd32(p, count)) return false;
    p += 4;
    if (count == 0 || count > 100000) {
        out.error = "bad count";
        return false;
    }
    if (p + count * 4u > size) {
        out.error = "name offsets truncated";
        return false;
    }
    std::vector<uint32_t> nameOff(count);
    for (uint32_t i = 0; i < count; ++i) {
        rd32(p, nameOff[i]);
        p += 4;
    }
    uint32_t namesLen = 0;
    if (!rd32(p, namesLen)) {
        out.error = "namesLen";
        return false;
    }
    p += 4;
    if (namesLen == 0 || p + namesLen > size) {
        out.error = "names blob";
        return false;
    }
    const char* names = reinterpret_cast<const char*>(b + p);
    p += namesLen;

    uint32_t total = 0;
    if (!rd32(p, total)) {
        out.error = "totalDataSize";
        return false;
    }
    p += 4;
    if (p + count * 4u * 3u > size) {
        out.error = "tables truncated";
        return false;
    }
    std::vector<uint32_t> sizes(count), times(count), dOff(count);
    for (uint32_t i = 0; i < count; ++i) {
        rd32(p, sizes[i]);
        p += 4;
    }
    for (uint32_t i = 0; i < count; ++i) {
        rd32(p, times[i]);
        p += 4;
    }
    for (uint32_t i = 0; i < count; ++i) {
        rd32(p, dOff[i]);
        p += 4;
    }

    out.dataStart = static_cast<uint32_t>(p);
    out.totalDataSize = total;
    out.count = count;
    out.bytes.assign(b, b + size);
    out.files.resize(count);

    uint32_t run = out.dataStart;
    for (uint32_t i = 0; i < count; ++i) {
        XpkEntry e;
        if (nameOff[i] < namesLen) {
            const char* s = names + nameOff[i];
            size_t maxn = namesLen - nameOff[i];
            size_t n = 0;
            while (n < maxn && s[n]) ++n;
            e.name.assign(s, n);
        }
        e.size = sizes[i];
        e.time = times[i];
        e.offset = (dOff[i] >= out.dataStart && dOff[i] + e.size <= size) ? dOff[i] : run;
        run += e.size;
        out.files[i] = e;
        if (!e.name.empty()) out.byKey[xpkNormalize(e.name)] = i;
    }
    out.valid = true;
    return true;
}

bool parseXpkFile(const std::string& path, XpkArchive& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        out = {};
        out.error = "cannot open " + path;
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    const auto s = ss.str();
    return parseXpk(s.data(), s.size(), out);
}

} // namespace gdx
