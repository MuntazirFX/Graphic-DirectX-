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
    CHECK(doc.firstMesh() && doc.firstMesh()->positions.size() == 8);

    gdx::ElementCatalog cat;
    cat.parse(
        "ELEMENT \"PRESENT A\"\nTYPE BONUS\n"
        "ELEMENT \"Plattform EXIT\"\nTYPE EXIT\n"
        "ELEMENT \"SNOW A\"\nTYPE PLATTFORM\n");

    gdx::DatLevel lvl;
    lvl.valid = true;
    lvl.count = 3;
    lvl.entities = {
        {"SNOW A", 0, 0, 0, 0, 0, 0, 0},
        {"PRESENT A", 1, 0, 0, 0, 0, 0, 0},
        {"Plattform EXIT", 2, 0, 0, 0, 0, 0, 0},
    };
    gdx::GameSession s;
    gdx::GameConfig cfg;
    cfg.defaultTimeSec = 235;
    gdx::buildSession(lvl, cat, s, cfg);
    CHECK(s.totalPresents == 1);
    CHECK(s.spawn() && s.spawn()->ent.name == "SNOW A");
    CHECK(gdx::tryCollect(s, 1));
    CHECK(s.collectedPresents == 1);
    CHECK(s.lives == 4); // extra life at 100%
    CHECK(gdx::tryExit(s));
    CHECK(gdx::levelCompletePoints(s) == 10 + 235 * 2);

    if (fails) {
        std::fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    std::printf("ok\n");
    return 0;
}
