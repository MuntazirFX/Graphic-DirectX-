# Graphic-DirectX-

Portable Joymania / SCIT 2002 asset toolkit: DirectX `.x`, DDS header, custom level `.dat`, `elements.txt`.

https://github.com/MuntazirFX/Graphic-DirectX-

## Done

- Text + binary `.x`
- Frames, skin pack, animation keys
- DDS header probe
- **Level `.dat`**: `uint32 count` + 60-byte records (`name[32]`, xyz, rot, variant)
- **elements.txt** catalog → PLATTFORM / ENEMY / DECO / BONUS / EXIT …
- `xdump`, `datdump`, `gdx_tests`, GitHub Actions CI

## Level .dat

```bash
cmake -S . -B build && cmake --build build
./build/datdump path/to/000.dat samples/elements.txt
```

Use original files from XPK (`levels\\000.dat`). Do not invent new level layouts unless testing.
