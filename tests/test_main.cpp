#include "gdx/gdx.h"
#include <cstdio>
#include <string>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    CHECK(doc.firstMesh() != nullptr);
    CHECK(doc.firstMesh()->positions.size() == 8);
    CHECK(doc.firstMesh()->indices.size() == 36);
    CHECK(!doc.firstMesh()->materials.empty());
    CHECK(doc.firstMesh()->materials[0].texture == "cube.dds");

    const char fakeDds[128] = {'D','D','S',' '};
    // height/width at 12/16 remain 0 — invalid, just don't crash
    auto dds = gdx::parseDDSHeader(fakeDds, sizeof(fakeDds));
    CHECK(!dds.valid);

    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
