#include "gdx/gdx.h"
#include <cstdio>
#include <cstring>
#include <string>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Document doc;
    std::string err;
    CHECK(gdx::parseFile("samples/cube.x", doc, err));
    CHECK(std::strstr(gdx::kProgrammers, "Vlcek") != nullptr);
    CHECK(std::strstr(gdx::kProgrammers, "Ohlmann") != nullptr);
    CHECK(gdx::detectTitleFromPath("bb.xpk") == gdx::TitleId::Rosso);
    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
