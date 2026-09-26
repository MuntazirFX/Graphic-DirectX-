# Graphic-DirectX-

Portable DirectX `.x` + DDS header toolkit for the Santa Claus in Trouble 2002 iOS remake.
No Windows SDK. No D3DX.

https://github.com/MuntazirFX/Graphic-DirectX-

## Features

- Text and binary `.x` (0302 / 0303, float32 / float64)
- Mesh, normals, UVs, materials, texture names
- Frame hierarchy + bind matrices
- SkinWeights → packed 4-bone weights (`XSkin.h`)
- AnimationSet / AnimationKey (text; binary best-effort)
- DDS header probe (`DDS.h`)
- D3D → Metal axis option
- `xdump` CLI + `gdx_tests`
- GitHub Actions CI

This is **not** a Direct3D 9/11/12 runtime. It loads Joymania-era `.x` / DDS assets so a Metal engine can draw them.

## Build

```bash
cmake -S . -B build && cmake --build build
./build/xdump samples/cube.x
./build/gdx_tests
```
