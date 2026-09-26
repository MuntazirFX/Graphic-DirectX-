#include "gdx/gdx.h"
#include "gdx/Host.h"
#include <cstdio>

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); ++fails; } } while (0)

int main() {
    gdx::Host host;
    gdx::HostConfig cfg;
    cfg.meshX = "samples/cube.x";
    CHECK(host.boot(cfg));
    CHECK(host.frame());
    CHECK(host.ref.litPixels() > 20);
    if (fails) return 1;
    std::printf("ok\n");
    return 0;
}
