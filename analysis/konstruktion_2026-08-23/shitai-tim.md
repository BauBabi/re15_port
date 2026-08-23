# SHITAI.TIM-Upload (0x80039c2c)

All RE questions are resolved. Compiling the dossier.

---

# SHITAI (Leichen-Prop-System) — RE-Dossier-Rohdaten

## 1) MECHANISMUS (vollständig, disasm-verifiziert)

### 1.1 Raumlade-Kette (FUN_800396fc, aus Raumlader FUN_8001d600)
- `@0x80039724` `sb zero → 0x800afbb7` — Leichen-Instanz-Zähler wird bei JEDEM Raumladen genullt.
- `@0x8003974c` `ori v0,zero,0xa05` + `@0x80039754` `sh v0 → DAT_800aca4c` — VRAM-Cursor-Reset: Textur-Slot=5, CLUT-Zeile=10 (Halbwort-Store setzt 0x800aca4c=5 und 0x800aca4d=10).
- `@0x800399a8` `jal 0x80039c2c` — unconditional SHITAI-Load (zwischen `jal 0x8003ea7c` Raum-TIMs @0x800399a0 und `jal 0x80044210` BGM @0x800399b0).

### 1.2 FUN_80039c2c — lädt SHITAI.**TM2** (das MODELL), NICHT das TIM
Raw-Disasm:
```
80039c30 ori a0,zero,0x29        ; Datei-ID 0x29
80039c54 sw a1 → DAT_8008f6a4    ; Puffer-Zeiger (Heap-Cursor DAT_800ac77c)
80039c58 jal FUN_80013b60        ; blockierender CD-Load, Debug-Label "SHITAI" @0x80010c24
80039c64 ori a0,zero,0x2         ; mode 2
80039c68 ori a2,zero,0xf         ; tpage-Slot 0xf
80039c74 jal FUN_80022150
80039c78 ori a3,zero,0x8         ; CLUT-Zeile 8
80039c7c-88 DAT_800ac77c += buf[0]   ; Heap-Advance = Resident-Size 0x3c4
```
Datei-Tabelle `@0x8006f43c` (Stride 8: u32 size, u16+u8 Sektor): Eintrag **0x29 = 1396 B = EMD/SHITAI.TM2** ✓, Eintrag **0x28 = 16928 B = EMD/SHITAI.TIM** (per Größen-Match auf den CD-Baum belegt; 0x20=DATA/ST_00.TIM 36640, 0x21=DATA/TEX.TIM 165408 "TEX TIM").

### 1.3 SHITAI.TM2-Format (MD1-Familie, Basis buf+0xc)
- `+0x00`=0x3c4 Resident-Size, `+0x04`=0 Reloc-Flag, `+0x08`=2 Part-Count.
- Part-Header (7 Worte `{v_off,v_cnt,n_off,n_cnt,face_off,face_cnt,uv_off}`): Part0 (Tris) @+0xc = `{0x38,29,0x120,29,0x208,20,0x3b8}`, Part1 (Quads) @+0x28 = `{0x38,29,0x120,29,0x2f8,12,0x4a8}` — geteilte Verts/Normalen.
- Relozierer FUN_8002288c (einmalig, Gate `+0x04==0→1`): alle 4 Offsets pro Part += buf+0xc (Savestate-verifiziert: buf=0x80124950, +0x0c→0x80124994 = buf+0xc+0x38 ✓).
- Face-Records = MD1 `{n,v}`-u16-Paare (alle Indizes <29 verifiziert, Datei-Offset face_off+0xc, z. B. Tri0 @0x214: n0=0,v0=9,n1=1,v1=0xa,n2=2,v2=2). UV-Records = MD1 12/16-B `{u,v,clut}{u,v,page}{u,v}`: **alle 32 Records baked clut=0x7800, page=0x0080**; max u=126, max v=64.
- ⛔ **VERTEX-ARRAY (Datei 0x44..0x12B, 29×8 B) ist auf der Disc 0xFF-GEWISCHT** — jeder Vertex = (−1,−1,−1). `info/Re1.5` und `shared_assets` byte-identisch (sha256-Präfix d1c5404b8aa2ac66). Normalen (plausible Q12-Einheitsvektoren @0x12C), Faces und UVs sind intakt. Das Mesh ist degeneriert → rasterisiert 0 Pixel.

### 1.4 FUN_80022150(2, buf, 0xf, 8) — Prim-Patch + Top-RAM-Kopie
- Pro Prim: `w1_hi16 (tsb) += 0xf` → 0x0080+0xf=**0x008f** = tpage x-Basis 15 → **VRAM (960,0), 8bpp**; `w0 += 8*0x400000` → cba 0x7800+0x200=**0x7a00** = **CLUT (0,488)** (Dekompilat LAB_80022234/Zeile 37/45; identische Konvention beim Spieler: Cursor:=Slot 0x17/Zeile 1 @0x8003155c-94, dann FUN_80022150(2,model,0x17,1) @0x800315b8-cc — Patch-Konstanten == Upload-Slot/CLUT-Zeile).
- Prim-Arrays werden per memcpy (FUN_800104b0) an den **FIXEN Anker 0x8018fff0−size** kopiert (`@0x8002227c lui 0x8018; ori 0xfff0`), hier 0x8018fe40 (20×12+12×16=0x1b0), uv_ptrs umgeschrieben. Der Anker ist GETEILT: jeder spätere FUN_80022150-Aufruf (jeder Gegner-EMD-Load @0x8002263c) überschreibt ihn. Savestate mzd_stage1_briefing: SHITAI-UVs nirgends mehr im RAM (beide Kopien überschrieben), Header+Verts+Normalen+Faces am Puffer intakt.

### 1.5 VRAM-Slot-System FUN_8004ee78 (Cursor DAT_800aca4c/4d)
`x = Slot*64 (−1024 ab Slot 16), y = 0/256; CLUT-y = 480+Zeile; Cursor += Breite/64, Zeile += CLUT-h` (Dekompilat Zeilen 12-30). Boot: TEX.TIM Slot 0x1b→(704,256), eigene CLUT-Rect (256,480,32,24) @0x800212d0-d8; ST_00.TIM Cursor 0x41a→Slot 0x1a/(640,256), CLUT 484 @0x800212f0-f8.

### 1.6 ⛔ SHITAI.TIM (ID 0x28) wird von NICHTS geladen — Beleg-Kette
1. Kein `ori a0,zero,0x28` in der EXE; alle 25 `jal FUN_80013b60`-Sites enumeriert (ghidra1_V2:78951 XREF[25]) — keine lädt 0x28.
2. Keine ID-Tabelle enthält 0x28: Boot-Kette @0x80073bdc (IDs 6,0,1,2,3,4,5,0x2b×4), DEBUG @0x80073ad8 (0x2c-0x33), TITLE @0x80102690/4 (0x2c,0x44,…), Spieler @0x80073f70 (0x3c-0x43), Tür @0x80071d2c (0x25…), BGM/VAB-Tabellen; Brute-Scan des EXE-Datenbereichs 0x80070000-0x80075000 auf u16==0x28: **0 Treffer**.
3. Overlays: alle `jal FUN_80013b60` in STAGE1-6/DEBUG/TITLE gescannt — nur Tabellen-IDs, keine 0x28.
4. SHITAI.TIM-Pixel/CLUT existieren in KEINER anderen CD-Datei (game-weite Byte-Suche über info/Re1.5; Treffer nur EMD/SHITAI.TIM selbst + das gepatchte Disc-Image, das dieselbe Datei enthält).
5. Laufzeit-Beweis: Savestates (mzd_stage1_briefing, room1140_entry, mzd_title) — VRAM(960,0..127) = Raum-Texturen, **CLUT-Zeile 488 x=0..255 komplett 0x0000**.
- SHITAI.TIM-Header: 8bpp-CLUT-TIM, CLUT 256×1 @(0,480) (Datei +0x08..0x13), Pixel 64 hw × 128 Zeilen, gespeicherte Org (0,0) (Datei @0x214) — die intendierte Position (960,0)/(0,488) existiert NUR über das Cursor-Paar (0xf,8) im Patch-Aufruf. Raumladen reserviert weder Slot 0xf noch CLUT-Zeilen 8/9 (Reset auf 5/10 @0x8003974c) → der Slot wird in texturreichen Räumen von Raum-Texturen überschrieben.

### 1.7 Leichen-Positions-Tabelle 0x800b11cc (Stride 8, Index = Kill-Flag-ID p7)
**Schreiber = 0x80039a74** (Ghidra hat die Funktion NICHT abgegrenzt — Dekompilat fehlt; raw belegt):
```
80039a7c v0 = 0x800b11cc
80039a84 v1 = entity[0x1c6]  ; Flag-/Corpse-Index (Latch)
80039a94 sh entity[+0x8] → rec+0        ; Typ
80039aa0/b4/c8 (pos+0x80)>>8 → rec+5/6/7 ; x/y/z aus entity+0x34/38/3C, signed byte
80039adc-ae4 (yaw@+0x6a + 8)>>4 → rec+4  ; yaw 0..255
80039ae8 sw zero,0(entity)               ; Entity-Wort +0 := 0
80039aec-b08 0x800aca4e--                ; Spawn-Zähler DEKREMENT
80039af8-b10 0x800b0ff2--                ; Lebend-Zähler (work_vars[0x11]) DEKREMENT
80039b14 jal 0x8004267c                  ; Warteschlangen-Nachrücker
```
Aufrufer: die Overlay-Corpse-States (State 7, Sub 0) — STAGE1 `@0x801095e0` (FUN_80109554, direkt nach `+0 |= 2` @0x801095d4, vor Blutlachen-Farbe 0xffff38 und Kill-Flag-SET `FUN_8004ef90(stage<3 ? 0x800b1038 : 0x800b1058, entity[0x1c6])`), STAGE2 `@0x80109474`, STAGE3 `@0x801096cc`, STAGE4 `@0x80109594`, STAGE5 `@0x80109714`, STAGE6: keiner.
**0x8004267c = Live-Nachrücker**: scannt Entity-Array 0x800acc2c (Stride 0x1F4) nach Wort+0 == 0x8000 (vom Gleichzeitig-Limit geparkter Record @0x80042230-38) und AKTIVIERT ihn (+0=1 bzw. 0x2001, Zähler++ @0x800426b4-d0) → in ROOM1030/1031/1040/1041 rückt beim Töten sofort der nächste Zombie nach.

### 1.8 Corpse-Spawn beim Wiederbetreten — Em_set-Dead-Branch + FUN_80039b2c
`@0x80042120-4c`: Flag gesetzt → `lbu a0,entity[0x1c6]` (**der Latch, nicht pc[7]!**) → `jal FUN_80039b2c` (analysis/zombie_10d0_reentry.md:93 etikettiert das fälschlich als „Cleanup, KEIN Spawn" — es ist der Leichen-Prop-Spawner). FUN_80039b2c (raw):
- Instanz = **0x800af33c** + Zähler×0x6c (Zähler 0x800afbb7; Kapazität exakt 20 bis zum Zählerbyte, kein Bounds-Check; ROOM1030/1040 haben je 20 Persist-Records).
- Tabelle: rec+5/6/7 `lb<<8` → inst+0x28/2c/30 (+Shorts +0x34/36/38) @0x80039b6c-a8; rec+4 `<<4` → yaw inst+0x5e @0x80039bd0-d4.
- inst+0x4 = SHITAI_buf+0xc (Part0), inst+0xc = buf+0x28 (Part1) @0x80039bbc-c8; inst+0x64 = 0x808080 @0x80039bcc; inst+0x68 = **0x80072d4c = Identitäts-MATRIX** (4096-Diagonale, gedumpt).
- FUN_80025940/FUN_80025a98: bauen double-buffered **POLY_GT3 (0x28 B)/GT4 (0x34 B)**-Pakete aus den Part-UV-Arrays in den Raum-Heap (inst+0x8/+0x10); Semi-Trans-Bits aus Inst-Flags `&0xc<<19`; uv_ptr wird durch Paket-Ptr mit Tag 0xFF000000 ersetzt → weitere Leichen memcpy'en die Pakete (build-once pro Raum). Zähler++ @0x80039c14.
- Renderer **FUN_80039ca0** (per Frame aus dem Render-Pass `@0x8001d1b8`; RE15_FUN_CATALOG.md Zeile 138 etikettiert ihn als „NPC render" — er rendert AUSSCHLIESSLICH diese Instanzen, DAT_800afbb7-XREFs sind nur b2c/c2c/ca0/Reset): RotMatrix(inst+0x5c) → FUN_80022da0 mit Identität → Per-Actor-Licht FUN_80053fc0(inst+0x50) → RVD-Sichtbarkeit FUN_80014368 → FUN_800254a0/FUN_800256b0 (opak) bzw. FUN_80022f0c/FUN_80023098 (Flags&0x10). FUN_800254a0 liest Face-Records {n,v}×3, GTE rtpt/ncct, OT-Einsortierung otz>>4.

### 1.9 NETTO-BEFUND: Feature im Preview DREIFACH totgelegt
Der Dead-Branch LÄUFT (Census unten), die Tabelle WIRD bei Kills geschrieben — aber die gespawnte Leiche ist unsichtbar: (1) alle 29 Vertices 0xFF → Null-Flächen-Polys, (2) Textur-Slot (960,0) wird nie beladen, (3) CLUT (0,488) = Nullen. Dasselbe Muster wie das Save-System („Save is not available in this preview"). Sichtbares Original-Verhalten beim Wiederbetreten = Gegner weg, KEINE Leiche. Übrigens: `s_SHITAI_OKIBA @0x8007027c` („Leichen-Lager") ist nur ein Debug-Raumname, kein Code-Bezug.

### 1.10 Census (Scratch-Skript mit dem byte-true SCD-Walker aus tools/aot_sce_census.py, 240 RDTs)
844 Sce_em_set gesamt, **609 persist-fähig (pc[7]≠0xFF) in 107 Räumen**; max p7 = 0xFC → Tabellen-Spanne bis 0x800b19B4. Leon/Elza-Varianten (xxx0/xxx1) teilen p7-Werte. Auszug (Raum: Anzahl/Typen/p7): 1010:4/t10, 1020:8/t10,11, **1030+1031: je 20/t16 p7=00-13 (Queue-Räume!)**, **1040+1041: je 20/t16 p7=14-27**, 1070:10, 1090:4, 10C0:3/t21, 10D0:2/t10 p7=cd, 10E0, 1100, 1120, 1140:10/t10,11,16 p7=d3-d7, 1141:4/t27, 1170:7/t21, 1180, 1190:6/t20, 11C0:4/t27, 11C1:5/t20 p7=d3-d7, 11D0:11/t20, 11E0, 11F0, 1200, 1210:10/t1a, 1220:10/t16, 1230; STAGE2: 2000, 2020, 2030, 2050, 2060, 2070, 2090, 20A0 (t16/t25); STAGE3: 3000, 3010, 3040, 3060, 30D0, 30E0; STAGE4: 4000, 4001, 4040, 4050, 4080, 40A0, 40B0 (t29/t2b/t13/t18); STAGE5: 5000, 5040, 5060, 5070, 50A0, 50B0, 5100, 5110, 5120, 5130, 5140. Caveat: **0x800b11de (= Record 2, Byte +2) wird von 0x80030cd8/0x80030f2c als fremder Flag-Bank-Zeiger benutzt** (`FUN_8004ef90(0x800acc10+0x45CE, flag@0x800acc17)`) — BSS-Überlappung, weiterer Abandonment-Beleg (p7=2-Kills würden diese Bits clobbern).

## 2) PORT-STAND
- `grep -ri SHITAI re15_port` = 0. Kein TM2-Parser, kein Leichen-Prop, keine Tabelle, kein 0x80039a74-Äquivalent.
- Em_set: `re15_port/engine/src/scd_vm.c` `op_sce_em_set` (~3183): Kill-Flag-Gate byte-true (Latch `a->em_flag_id` Zeile 3248; Zonen 7/8 = Bänke 0x800b1038/58, Zeile 3212); Suppress = Record VERWORFEN (Zeile ~3239-42) — kein Corpse-Spawn, keine 0x8000-Parkierung.
- Todes-Seite: Kill-Flag-Set vorhanden (`enemy_ai_common.c:6899/6932/9086`); Corpse-Settle `re15_enemy_corpse_settle` (`enemy_ai_common.c:2111`); **Lebend-Zähler `g_scd.work_vars[0x11]` wird im Port NIE dekrementiert** (nur Reset `scd_room_setup.c:168`/`scd_vm.c:423`, ++ @3241) und **kein Live-Nachrücker** → reale Divergenz in ROOM1030/1031/1040/1041 (Original: Nachrücker-Spawn beim Töten; Port: Deckel für immer). Zudem widerspricht `@0x80039aec-b08` (Spawn-Zähler−−) dem Port-Kommentar „NEVER decremented" (`enemy_ai_common.c:2536`).
- TIM-Slots: abstrakte Slots 0..44 (`render_pc.c:166 RE15_TIM_SLOT_MAX 45`), Upload `re15_render_pc_upload_tim_slot` (`platform/pc/main.c:133`), Prop-Muster `pc_load_room_prop_set` (`main.c:~806`): `re15_md1_parse` + Slot `RE15_TIM_SLOT_PROP(op)` (`main.c:131`). MD1-Parser `md1_common.c:41` — TM2 mappt 1:1 auf `re15_md1_mesh_t` (Face-/UV-/Vertex-Strides identisch; nur Header anders: Basis +0xc, 7-Wort-Part-Header, Tri/Quad getrennt).

## 3) IMPLEMENTIERUNGS-PLAN (byte-true)
1. **Corpse-Commit-Buchhaltung** (Ort: die drei Kill-Flag-Set-Stellen `enemy_ai_common.c:6899/6932/9086` bzw. Corpse-State-Entry): (a) Tabelle `re15_corpse_table[253]` `{u16 type; u8 yaw; s8 x,y,z}` in `game_state.c` (raumübergreifend persistent wie g_game.flags, NICHT im Save-File — Original ist BSS), Werte `(pos+0x80)>>8` @0x80039aa0-d0, `(yaw+8)>>4` @0x80039adc; (b) `g_scd.work_vars[0x11]--` @0x80039af8-b10; (c) Spawn-Zähler−− @0x80039aec-b08 + Kommentar 2536 korrigieren.
2. **Live-Nachrücker (0x8004267c)**: `op_sce_em_set` muss über-Cap-Records PARKEN (Actor reserviert, Zustand „0x8000") statt verwerfen (@0x80042230-38); beim Corpse-Commit ersten geparkten Record aktivieren (+Zähler++ @0x800426b4-d0). Verifikation: `re15-room-probe` ROOM1030 — 6 spawnen, einen töten → siebter aktiviert; Gegenprobe DuckStation ROOM1030.
3. **Em_set-Dead-Branch**: statt bloßem Suppress einen datenhaltigen, NICHT gezeichneten Corpse-Prop aus `re15_corpse_table[latch]` anlegen (Zähler-Analogon zu 0x800afbb7) — byte-true zur Auslieferung, denn das gerenderte Original-Mesh ist wegen des 0xFF-Wipes unsichtbar (Null-Flächen-Polys). KEINE erfundene Geometrie; eine „Restauration" ist aus Shipped-Daten UNMÖGLICH (Vertices existieren nicht auf der Disc). Falls der Nutzer sichtbare Leichen will → separate Entscheidung (RE2-Geometrie-Spende wäre Mod, nicht byte-true).
4. **Tests**: Unit-Test pinnt den TM2-FF-Wipe (Parse SHITAI.TM2, assert Vertex-Region 0x44..0x12B == 0xFF und dass nichts gezeichnet wird); ROOM1030-Nachrücker-Probe als ctest.
5. SHITAI.TIM/TM2-Parser + Slot-45-Upload NUR falls Punkt 3 je sichtbar werden soll (TM2→`re15_md1_t`: mesh0.tri_* aus Part0, quad_* aus Part1, gemeinsame Verts/Normalen, Basis +0xc; UVs sind TIM-lokal, max u=126/v=64 — direkter Slot-Bind wie Props).

## 4) OFFEN (ehrlich)
1. **Kein Hardware-/Emulator-Beweis des Re-Entry-Frames**: Es existiert kein Savestate „Zombie getötet → Raum wiederbetreten". Statisch ist die Unsichtbarkeit dreifach belegt (0xFF-Vertices, fehlender TIM-Upload, Null-CLUT); der letzte Beweis wäre ein DuckStation-Lauf ROOM10D0 (kill → re-enter, p7=0xCD) per `re15-room-capture` + Framebuffer-Dump. Nächster RE-Schritt, falls verlangt.
2. **FUN_80022f0c/FUN_80023098** (Semi-Trans-Zeichenvariante, Inst-Flags&0x10) nicht disassembliert; FUN_80039b2c initialisiert inst+0x0 NICHT (Stale-BSS) — ob Flags&0x10 je gesetzt sein kann, ungeklärt (folgenlos, da unsichtbar).
3. **Stale-Latch-Quirk**: `@0x80042140` liest den Entity-Latch `+0x1c6` statt pc[7] — nach Slot-Wiederverwendung über Räume hinweg kann der Index vom FALSCHEN Record stammen. Für den Port empfohlen: pc[7] verwenden + Quirk dokumentieren (sichtbare Wirkung im Original = null).
4. **Scan-Negative**: „Nichts lädt ID 0x28" ist über alle statischen Wege (Call-Sites, ID-Tabellen, Daten-Scan, Overlays, Pixel-Suche) + Laufzeit-VRAM belegt; ein exotischer selbstmodifizierender Pfad ist nicht formal ausschließbar.
5. Werkzeug-Notiz: `re15_disasm.py scan` ist ein Funktions-Summarizer, KEIN Xref-Finder (Negativ-Ergebnisse damit sind wertlos); eigener Xref-Scanner liegt im Scratchpad (`xref_scan.py`, `emset_census.py`, `shitai_check*.py`). Katalog-Korrekturen fällig: 0x80039ca0 („NPC render" → Corpse-Renderer), analysis/zombie_10d0_reentry.md:93 („Cleanup" → Corpse-Prop-Spawn), 0x80039a74 als neue Funktion (Ghidra-Lücke).
