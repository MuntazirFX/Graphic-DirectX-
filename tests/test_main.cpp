#include "gdx/gdx.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    CHECK(doc.firstMesh() != nullptr);
    CHECK(doc.firstMesh()->positions.size() == 8);
    CHECK(doc.firstMesh()->indices.size() == 36);

    std::vector<uint8_t> raw(4 + 60, 0);
    uint32_t n = 1;
    std::memcpy(raw.data(), &n, 4);
    const char* name = "PRESENT A";
    std::memcpy(raw.data() + 4, name, std::strlen(name));
    float xyz[6] = {1.f, 2.f, 3.f, 0, 90.f, 0};
    std::memcpy(raw.data() + 4 + 32, xyz, sizeof(xyz));
    int32_t var = 7;
    std::memcpy(raw.data() + 4 + 56, &var, 4);
    gdx::DatLevel lvl;
    CHECK(gdx::parseDatLevel(raw.data(), raw.size(), lvl));
    CHECK(lvl.count == 1);
    CHECK(lvl.entities[0].name == "PRESENT A");
    CHECK(lvl.entities[0].x == 1.f && lvl.entities[0].y == 2.f && lvl.entities[0].z == 3.f);
    CHECK(lvl.entities[0].variant == 7);

    gdx::ElementCatalog cat;
    const char* txt =
        "ELEMENT   \"PRESENT A\"\n"
        "FILE      \"gfx\\\\present.x\"\n"
        "TYPE      BONUS\n";
    CHECK(cat.parse(txt) == 1);
    CHECK(cat.find("PRESENT A") != nullptr);
    CHECK(cat.find("PRESENT A")->type == "BONUS");

    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
