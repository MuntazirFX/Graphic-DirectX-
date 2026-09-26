# Graphic-DirectX-

Portable **DirectX `.x`** loader (no D3DX, no Windows SDK).
For Santa Claus in Trouble 2002 iOS remake: parse original mesh / frame / skin files from XPK.

Repo: https://github.com/MuntazirFX/Graphic-DirectX-

## Status

- Text `.x` (`xof 0302txt` / `0303txt`)
- Binary `.x` (`xof 0302bin` / `0303bin`, 32- or 64-bit floats)
- `Mesh`, `MeshNormals`, `MeshTextureCoords`, `MeshMaterialList`, `Material`, `TextureFilename`
- `Frame` + `FrameTransformMatrix`
- `XSkinMeshHeader` + `SkinWeights`
- n-gon fan triangulation
- Optional D3D → Metal (`ConvertOptions::metalFromD3D()`)

Animation keys still skipped.

## Build

```bash
cmake -S . -B build
cmake --build build
./build/xdump samples/cube.x
./build/xdump path/to/extracted.x
```

`xdump` prints verts / tris / bones / textures. Use that on a real SCIT model next.
