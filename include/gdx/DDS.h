#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace gdx {

struct DDSInfo {
    bool valid = false;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipCount = 1;
    uint32_t fourCC = 0; // DXT1/DXT3/DXT5 or 0
    uint32_t rgbBitCount = 0;
    size_t dataOffset = 0;
};

inline DDSInfo parseDDSHeader(const void* data, size_t size) {
    DDSInfo i;
    if (!data || size < 128) return i;
    const auto* b = static_cast<const uint8_t*>(data);
    if (std::memcmp(b, "DDS ", 4) != 0) return i;
    uint32_t hsize;
    std::memcpy(&hsize, b + 4, 4);
    std::memcpy(&i.height, b + 12, 4);
    std::memcpy(&i.width, b + 16, 4);
    uint32_t flags;
    std::memcpy(&flags, b + 8, 4);
    std::memcpy(&i.mipCount, b + 28, 4);
    if (i.mipCount == 0) i.mipCount = 1;
    std::memcpy(&i.fourCC, b + 84, 4);
    std::memcpy(&i.rgbBitCount, b + 88, 4);
    i.dataOffset = 4 + hsize;
    if (i.dataOffset < 128) i.dataOffset = 128;
    i.valid = i.width > 0 && i.height > 0;
    return i;
}

} // namespace gdx
