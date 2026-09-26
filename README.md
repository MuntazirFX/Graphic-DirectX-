# Graphic-DirectX-

Portable **DirectX `.x`** loader (no D3DX, no Windows SDK).
Meant for the Santa Claus in Trouble 2002 iOS remake: parse original mesh / frame / skin files extracted from XPK.

## Status

- Text `.x` (`xof 0302txt` / `0303txt`)
- `Mesh`, `MeshNormals`, `MeshTextureCoords`, `MeshMaterialList`, `Material`, `TextureFilename`
- `Frame` + `FrameTransformMatrix` hierarchy
- `XSkinMeshHeader` + `SkinWeights`
- n-gon faces fan-triangulated
- Optional D3D → Metal axis convert (`z = -z`, flip winding)
- Binary `.x` detected, not parsed yet

## Build

```bash
cmake -S . -B build
cmake --build build
./build/xdump path/to/model.x
```

## Use from Santa-iOS-Engine

Add `include/` to header search path and compile `src/XParser.cpp`.

```cpp
#include "gdx/XParser.h"

gdx::Document doc;
std::string err;
if (!gdx::parseFile("santa.x", doc, err, gdx::ConvertOptions::metalFromD3D())) {
    // err
}
const gdx::Mesh* m = doc.firstMesh();
```

Do not author new `.x` files. Load the originals.
