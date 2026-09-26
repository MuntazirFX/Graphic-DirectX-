#include "gdx/gdx.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

static std::vector<uint8_t> makeTinyXpk() {
    const char* fname = "levels\\000.dat";
    const uint32_t nameLen = static_cast<uint32_t>(std::strlen(fname) + 1);
    const char payload[] = {'D','A','T','!'};
    const uint32_t count = 1;
    const uint32_t nameOff = 0;
    const uint32_t total = 4;
    const uint32_t sz = 4;
    const uint32_t tm = 1036948800;
    const uint32_t hdr =
        4 + 4 + 4 + nameLen + 4 + 4 + 4 + 4;
    std::vector<uint8_t> out(hdr + 4, 0);
    size_t p = 0;
    auto w32 = [&](uint32_t v) {
        std::memcpy(out.data() + p, &v, 4);
        p += 4;
    };
    w32(count);
    w32(nameOff);
    w32(nameLen);
    std::memcpy(out.data() + p, fname, nameLen);
    p += nameLen;
    w32(total);
    w32(sz);
    w32(tm);
    w32(hdr);
    std::memcpy(out.data() + p, payload, 4);
    return out;
}

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    CHECK(doc.firstMesh() && doc.firstMesh()->positions.size() == 8);

    std::vector<uint8_t> raw(4 + 60, 0);
    uint32_t n = 1;
    std::memcpy(raw.data(), &n, 4);
    const char* name = "PRESENT A";
    std::memcpy(raw.data() + 4, name, std::strlen(name));
    float xyz[3] = {1.f, 2.f, 3.f};
    std::memcpy(raw.data() + 4 + 32, xyz, sizeof(xyz));
    gdx::DatLevel lvl;
    CHECK(gdx::parseDatLevel(raw.data(), raw.size(), lvl));
    CHECK(lvl.entities[0].name == "PRESENT A");

    auto xpk = makeTinyXpk();
    gdx::XpkArchive a;
    CHECK(gdx::parseXpk(xpk.data(), xpk.size(), a));
    CHECK(a.count == 1);
    CHECK(a.find("levels/000.dat") != nullptr);
    auto got = a.extract("LEVELS\\000.DAT");
    CHECK(got.size() == 4);
    CHECK(got[0] == 'D' && got[3] == '!');

    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
