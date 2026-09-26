# Graphic-DirectX-

Joymania family toolkit — same XPK / .x / .dat pipeline for:

| Id | Game | Archive | Exe |
|---|---|---|---|
| scit | Santa Claus in Trouble (2002) | `xmas.xpk` | SantaClausInTrouble.exe |
| scit-again | ... again! (2004) | `xmas.xpk` | SantaClaus2.exe |
| rosso | Rosso Rabbit in Trouble (2003) | `bb.xpk` | RossoRabbitInTrouble.exe |
| scit-hd | HD (2020) | `data.pak` (not XPK) | SantaClausInTrouble.exe |

Detect: `gdx::detectTitle(archive, path)`.
HD pak format is separate (jeysym scit-hd-pak-tool) — flagged, not parsed as XPK.
