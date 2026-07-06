# DuckStation-Savestate-Container (MZD-Disc) — Format-Map für einen Repacker

Zweck: die Struktur, um einen Savestate zu **patchen + neu zu packen** (z.B. Player-Position
neben einen Gegner teleportieren → Wave-2-Attack-Dynamik provozieren OHNE Navigation). Gemappt
2026-07-06 aus `stage_saves/mzd_stage1_maggot.sav` (1 574 913 B).

## Layout (little-endian)

```
[0x000]  "DUCCS\0\0\0"                          Magic
[0x008]  Disc-Titel-String (nullterminiert, gepolstert)   "Biohazard 1.5 (MZD Mod) …"
[0x088]  "HASH-957757946319438E\0…"             Disc-Hash (gepolstert)
[0x0b8]  u32 = 256                              (VRAM-Params? 0x100)
[0x0bc]  u32 = 192  (=0xc0)                     Offset zur Sektions-Tabelle (zeigt auf 0xc0)
--- Sektions-Tabelle @0xc0 ---
[0x0c0]  u32  frame0 COMPRESSED size   = 86999  ← beim Repack UPDATEN
[0x0c4]  u32  frame0 OFFSET            = 300    (= 0x12c, Start von Frame 0)
[0x0c8]  u32  = 2                              (Kompressions-Typ? zstd)
[0x0cc]  u32  frame1 COMPRESSED size   = 1487614
[0x0d0]  u32  frame1 DECOMP size       = 3819109
[0x0d4]  u32  frame1 OFFSET            = 87299  ← beim Repack UPDATEN (= 300 + neue frame0-csize)
[0x0d8]  Cue-Pfad-String  "C:\Users\…\Biohazard 1.5 (MZD Mod)…cue"
[0x12c]  zstd-Frame 0  (86999 B → 4 015 717 B)  ← enthält die PSX-Main-RAM @0x80000000
[…]      zstd-Frame 1  @87299 (1 487 614 B → 3 819 109 B)
```

- **Die decompressed-size je Frame steht im zstd-Frame-Header selbst** (zstd content-size),
  NICHT nochmal für frame0 im Container → beim In-Place-Patch (gleiche dsize) muss sie nicht
  angefasst werden.
- `re15_ss.py`s `Ram` liest Frame 0 (`decompress_sav` findet das erste zstd-Magic @300) → die
  Main-RAM (Player/Gegner-State) liegt in **Frame 0**.

## Repack-Rezept (ungetestet — Validität via DuckStation-Load bestätigen!)

1. `blob0 = zstd_decompress(data[300:87299])`  (4 015 717 B).
2. Patch `blob0` an `find_ram_base(blob0) + (addr − 0x80000000)`.
3. `f0 = zstd_compress(blob0)`  → neue csize `n = len(f0)`.
4. Header patchen: `[0xc0] = n`, `[0xd4] = 300 + n`.
5. Reassemble: `header[:300] + f0 + data[87299:]`  (Frame 1 + Trailer unverändert anhängen).
6. **VALIDIEREN:** (a) `re15_ss.Ram(out).s16(0x800acaee)` == erwarteter Wert (Frame-0-Integrität);
   (b) in DuckStation quickladen — akzeptiert es den Save? Wenn **Checksum/undokumentiertes
   Feld** existiert, schlägt (b) fehl → dann ist der Repack-Weg tot, zurück zur Navigation.

## Offene Risiken (warum ungetestet zurückgestellt)

- Möglicher **globaler Checksum** oder ein Feld, das die Gesamt-Dateigröße/Frame-1-Integrität
  bindet — nur ein realer DuckStation-Load beweist die Akzeptanz.
- Der Player-Positions-Global (für den Teleport-Patch) ist noch nicht lokalisiert (bekannt:
  HP @0x800acaee, State @0x800aca58, Display-Flags @0x800aca3c).

## Anwendung: Maggot-Leap-Launch-Provoke (der ursprüngliche Zweck)

Player-Global neben den Maggot (ROOM11C0-Save, Maggot @~(-1220,-21568)) patchen → advancen →
`re15_enemy_state.py` liest das Maggot-`+0x5` (5=Bite / 15=Leap-Airborne / 6=Heavy) live →
pinnt die Launch-Entscheidung, die statisch nur als Skelett bekannt ist (siehe RE15_MAGGOT_AI.md
„LEAP-Skelett"). Gleiches Werkzeug schaltet Spider/Zombie-Girl-Wave-2-Dynamik frei.
