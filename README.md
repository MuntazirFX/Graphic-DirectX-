# Graphic-DirectX-

Joymania / Santa Claus in Trouble 2002 asset toolkit.

https://github.com/MuntazirFX/Graphic-DirectX-

## Done

- DirectX `.x` text + binary
- Level `.dat` (60-byte records)
- `elements.txt` catalog
- **XPK** (`xmas.xpk`): count, name table, sizes, timestamps, absolute data offsets
- DDS header, skin pack
- `xdump` `datdump` `xpkdump` + CI

SCIT 2002 `xmas.xpk` stores file bytes raw (no per-file MSZip). Name "MSZip" is the family label; this reader matches the on-disk layout verified in `Santa-ios` / XPKTool.

```bash
./build/xpkdump xmas.xpk
./build/xpkdump xmas.xpk levels\\000.dat
```
