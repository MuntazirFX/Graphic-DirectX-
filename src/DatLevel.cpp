#include "gdx/DatLevel.h"
#include <cstring>
#include <fstream>
#include <sstream>

namespace gdx {

bool parseDatLevel(const void* data, size_t size, DatLevel& out) {
    out = {};
    if (!data || size < 4) {
        out.error = "too small";
        return false;
    }
    const auto* b = static_cast<const uint8_t*>(data);
    uint32_t count = 0;
    std::memcpy(&count, b, 4);
    constexpr size_t kRec = 60;
    const size_t expected = 4 + static_cast<size_t>(count) * kRec;
    if (count == 0 || count > 100000 || expected != size) {
        out.error = "size mismatch count=" + std::to_string(count);
        out.count = count;
        return false;
    }
    out.count = count;
    out.entities.resize(count);
    for (uint32_t i = 0; i < count; ++i) {
        const uint8_t* rec = b + 4 + i * kRec;
        DatEntity& e = out.entities[i];
        size_t n = 0;
        while (n < 32 && rec[n] != 0) ++n;
        e.name.assign(reinterpret_cast<const char*>(rec), n);
        std::memcpy(&e.x, rec + 32, 4);
        std::memcpy(&e.y, rec + 36, 4);
        std::memcpy(&e.z, rec + 40, 4);
        std::memcpy(&e.rx, rec + 44, 4);
        std::memcpy(&e.ry, rec + 48, 4);
        std::memcpy(&e.rz, rec + 52, 4);
        std::memcpy(&e.variant, rec + 56, 4);
    }
    out.valid = true;
    return true;
}

bool parseDatLevelFile(const std::string& path, DatLevel& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        out = {};
        out.error = "cannot open " + path;
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    const auto s = ss.str();
    return parseDatLevel(s.data(), s.size(), out);
}

} // namespace gdx
