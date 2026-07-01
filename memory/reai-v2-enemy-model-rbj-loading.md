---
name: reai-v2-enemy-model-rbj-loading
description: "Warum RE1.5-Gegner/Irons als statische Leon-Modelle rendern — Asset-Lade-Root-Causes (CDEMD0.EMS + RBJ-im-RDT)"
metadata:
  type: project
---

Symptom: RE1.5-Gegner (ROOM1140-Zombies, Irons in ROOM1150) rendern als **statische Leon-Modelle** —
weil zwei ASSET-Ladewege fehlschlugen und `re15_actor_anim_select` dann auf `def_mesh` (Leon PL00)
zurückfällt. Beide 2026-07-01 gefixt (Commits fd1ea376 + a664c768), verifiziert 33/33 ctest + headless.

**1. `pc_read_shared` fand die CD-Baum-Assets NICHT.** Es suchte nur den extrahierten `assets_shared`-
Baum, aber `EMD/CDEMD0.EMS` (die Gegner-Modell-Archiv-Datei — der Disc hat KEINE per-Typ `EM<NN>.EMD`)
und `DATA/CORE00.ESP` (globale Effekt-Bank) liegen NUR in `shared_assets/PSX`. → Gegner-Load scheiterte
still → Leon-Fallback. **Fix:** CD-Root-Fallback in `pc_read_shared` (env `RE15_CD_ROOT` / Compile-Default
`RE15_CD_ROOT_DEFAULT = shared_assets/PSX`, gleiche Konvention wie render_pc.c), zusätzlich `<cdroot>/../`
für die abgeleiteten `extracted_fx/`. Danach: `[enemy] EM16/EM10/EM11 loaded: 15 meshes/15 bones/43 clips`.
`re15_ems.c` splittet den Typ-Blob byte-true aus CDEMD0.EMS (`s_ems_order`-Tabelle, Typ→Blob-Index).

**2. RE1.5 hat KEINE standalone RBJ-Dateien** — die per-Raum-CINEMATIC-Animation (RBJ = Clip-Tabelle +
Keyframe-Pool, z.B. Irons-Kneel/Prone in 1150) steckt IM RDT jedes Raums am **Header-Offset 0x5C**
(`animationStart`, die letzte Section-Adresse; genau was `RDTExtractor.writeAnimation` slict). Der Port
suchte `RBJ/ROOM####.RBJ` — die existiert nur für 1170 (extrahiert). → Irons' em45-Rebind (enemy_common.c
`{0x1150,0x45,1}` = RBJ RECORD 1) feuerte nie → statischer Leon. **Fix:** RBJ in `re15_rdt_parse` parsen
(`rdt.animation`/`animation_size`; Länge selbst-beschreibend = `dirOff + recordCount*8`, das Directory
sitzt am Container-ENDE — byte-identisch zur extrahierten ROOM1170.RBJ), und in main.c an BEIDEN Cinematic-
Overlay-Stellen (Boot + Cross-Room) aus dem residenten RDT fallbacken, wenn keine standalone Datei da ist.
`RE15_RBJ` env bleibt Debug-Override; borrowed Alias in den RDT-Puffer (NIE freen). Danach: ROOM1150
"cinematic bank ... 67276 bytes, from RDT@0x5C" → 15 clips/411 kf + `[enemy] EM45 loaded`. ROOM1140 hat
`animationStart=0` (keine Cinematics) = korrekt; seine Zombies animieren aus den EMS-Bänken.

**Render-Pfad (korrekt, kein Bug):** `re15_actor_anim_select` (anim_select_common.c) routet einen geladenen
Gegner-Bank (`re15_enemy_find(type)`) → eigenes mesh/skel/anim + `pc_tex_slot`; NPC-`anim_frame` wird in
`player_common.c` advanced (liegende 0x0C/0E/12/13 auf Frame 0 eingefroren = prone). Diagnose-Tool:
`RE15_ENEMY_DIAG=1` → eine Zeile pro Actor beim ersten Render (Modell-Quelle/motion/anim_frame).

Build auf proot-Debian (aarch64): gcc 14.2 + cmake 3.31 + ninja + SDL2 2.32.4 — vollständig, `build_linux`
baut re15_pc (SDL2 statisch via FetchContent), 33/33 ctest. Siehe [[reai-v2-build-recipe]].
