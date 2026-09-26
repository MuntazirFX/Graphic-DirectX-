#include "gdx/gdx.h"
#include <cstdio>
#include <string>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    gdx::DrawItem item;
    gdx::meshToDraw(*doc.firstMesh(), item);
    gdx::Device8 dev;
    dev.reset(160, 120);
    dev.setView(gdx::translation(0.f, 0.f, 4.f));
    dev.draw(item);
    CHECK(dev.litPixels() > 20);
    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok lit=%d\n", dev.litPixels());
    return 0;
}
