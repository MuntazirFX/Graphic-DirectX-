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

    CHECK(gdx::detectTitleFromPath("game/xmas.xpk") == gdx::TitleId::Scit2002);
    CHECK(gdx::detectTitleFromPath("bb.xpk") == gdx::TitleId::Rosso);
    CHECK(gdx::detectTitleFromPath("SantaClaus2/xmas.xpk") == gdx::TitleId::ScitAgain);
    CHECK(gdx::detectTitleFromPath("data.pak") == gdx::TitleId::ScitHd);
    CHECK(gdx::titleInfo(gdx::TitleId::Rosso).xpkFamily);
    CHECK(!gdx::titleInfo(gdx::TitleId::ScitHd).xpkFamily);

    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
