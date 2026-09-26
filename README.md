# Graphic-DirectX- / gdxhost

Xash3D-**style** split for Joymania data. Not a GoldSrc fork. Not Xash code.

| Xash3D | Here |
|---|---|
| `filesystem` PAK/WAD | `Xpk` (`xmas.xpk`, `bb.xpk`) |
| `model` MDL | `.x` parser |
| `world` BSP | `.dat` + elements.txt |
| `ref_soft` | `Device8` software raster |
| `client.dll` | `GameSession` |
| `xash` launcher | `gdxhost` |

```bash
cmake -S . -B build && cmake --build build
./build/gdxhost -model samples/cube.x -o cube.ppm
./build/gdxhost -pak xmas.xpk -map levels\\000.dat -model gfx\\something.x
```
