#include "gdx/gdx.h"
#include <cstdio>
#include <string>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    CHECK(doc.firstMesh());
    gdx::DrawItem d;
    gdx::meshToDraw(*doc.firstMesh(), d);
    CHECK(d.v.size() == 8);
    CHECK(d.i.size() == 36);
    CHECK(d.texture == "cube.dds");
    auto p = gdx::perspectiveD3D(1.0f, 1.333f, 0.1f, 100.f);
    CHECK(p.m[0] != 0);
    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
