#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gdx {

// Joymania SCIT 2002 level file (levels\\000.dat …)
//   uint32 count
//   Record[count] 60 bytes:
//     char  name[32]
//     float x,y,z
//     float rx,ry,rz
//     int32 variant
struct DatEntity {
    std::string name;
    float x = 0, y = 0, z = 0;
    float rx = 0, ry = 0, rz = 0;
    int32_t variant = 0;
};

struct DatLevel {
    uint32_t count = 0;
    std::vector<DatEntity> entities;
    bool valid = false;
    std::string error;
};

bool parseDatLevel(const void* data, size_t size, DatLevel& out);
bool parseDatLevelFile(const std::string& path, DatLevel& out);

} // namespace gdx
