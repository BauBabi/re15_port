---
name: re15-savestate-ghidra
description: Extrahiert byte-true Spielzustand aus einem DuckStation-RE1.5-Savestate, indem Ghidra (statischer Mechanismus — welche FUN_ welches DAT_ liest/schreibt) mit den dynamischen RAM-Werten des Savestates kombiniert wird. Verwenden, um Globals zu lesen/benennen, eine unbekannte RAM-Adresse per Differenz-Diff zu finden, die Stage eines Savestates zu bestimmen, oder den Framebuffer zu dumpen — für byte-true RE und Port-Vergleich.
---

# RE1.5 Savestate-Extraktion mit Ghidra

Der Savestate liefert den **dynamischen Wert** in einem konkreten Raum/Moment; Ghidra liefert den **statischen Mechanismus** (welche Funktion welches Global liest/schreibt und wie). Zusammen → byte-true belegte Erkenntnis. Savestates erzeugt der Schwester-Skill **re15-room-capture**. Siehe Memory `reai-v2-duckstation-dynamic-re` + `reai-v2-ghidra-pipeline`.

## 1. Savestate dekodieren (`scripts/re15_ss.py`)

DuckStation `.sav` = `DUCCS`-Header + Name + zstd-Frame (`0x28b52ffd`). Dekomprimiert ~4 MB. Die **RAM-Base** wird per Signatur gefunden: die ersten 48 Code-Bytes von `info/Re1.5/PSX.EXE` (ab Datei-Offset `0x800`, lädt @`0x80010000`) werden im Blob gesucht → `base = pos − 0x10000`. Jedes Global: `blob[base + (addr − 0x80000000)]` (LE). VRAM @ `base + 0x200000` (1024×512, 16bpp 1555).

```bash
python scripts/re15_ss.py <savestate.sav> [out.png]   # Globals + optional Framebuffer-PNG
```

`Ram`-Klasse: `.u8/.s8/.u16/.s16/.u32/.s32(addr)`, `.bytes(addr,n)`; `vram_png(ram, path)`.

## 2. Bekannte Globals labeln (`scripts/re15_savestate_inspect.py`)

Dumpt benannten Zustand mit zitierter Ghidra-Evidenz pro Feld:

```bash
python scripts/re15_savestate_inspect.py <savestate.sav> [out.png]
```

Verifizierter Output (Demo-Savestates in `stage_saves/mzd_demo/`):

```
stage           : STAGE 1 (boot / briefing-area)   |   STAGE 2
overlay fp      : 0x96290818                        |   0x1c0394d0
camera cut      : 1 (DAT_800b532e)                  |   2
player HP       : 100 (DAT_800acaee, FUN_80012d60)
player state    : 1 (DAT_800aca58: 1=idle 2=hit 3=death)
display flags   : 0x0 (DAT_800aca3c: 0x100=BG-skip 0x10=fade-dir)
fade level      : 0x0 (DAT_800b5568: 0xF0=black)
```

Globals-Beleg: HP `DAT_800acaee` = player+0x9a (FUN_80012d60 Damage); State `DAT_800aca58` (FUN_80031c44); Display-Flags `DAT_800aca3c` (FUN_80014230-Gate, Bit 0x100 = BG-Renderer-Skip); Fade `DAT_800b5568` (FUN_80021a0c).

## 3. Stage bestimmen (Overlay-Fingerprint)

Jede Stage lädt ihr Overlay @`0x80100000` und es bleibt über **alle Räume der Stage resident** (Ghidra/CLAUDE.md). CRC32 der ersten 32 KB = stabiler Per-Stage-Fingerprint. Beleg: zwei verschiedene Räume **derselben** Stage → identischer fp (Stage-2-Tunnel und der resume-Locker-Room: beide `0x1c0394d0`); verschiedene Stages → 97 % Byte-Differenz.

> **Nicht** gegen `STAGE{n}.BIN` matchen — die Disc-Overlays sind komprimiert/container-verpackt, der RAM-Abzug matcht die Datei-Bytes nicht (~13 % = Zufall). Der Self-Fingerprint ist der zuverlässige Weg. Fingerprint-Tabelle in `re15_savestate_inspect.py` (`STAGE_FP`) bei neuen Stage-Captures erweitern.

## 4. Unbekanntes Global FINDEN (`scripts/re15_diff.py`)

Mehrere Savestates aus bekannten Kontexten laden, Bytes per Constraints filtern → Kandidaten-Adressen. Constraints: `--eq A,B` / `--neq A,B` (Label-Paare), `--alldiffer`, `--min/--max`, `--width 1|2|4`, `--region LO HI`.

```bash
# Stage-Diskriminator: Adresse, wo res==s2 (gleiche Stage) und s1!=s2:
python scripts/re15_diff.py s1=stage1.sav s2=stage2.sav res=resume.sav \
       --eq res,s2 --neq s1,s2 --max 8
# Per-Raum-Diskriminator (drei verschiedene Räume): alle Werte verschieden:
python scripts/re15_diff.py a=r1.sav b=r2.sav c=r3.sav --alldiffer --width 2
```

**Achtung Fallstrick:** die `--eq/--neq`-Beziehung muss zur echten Kontext-Zugehörigkeit passen — der resume-Savestate lag (per Overlay-fp, §3) in DERSELBEN Stage wie der Triangle-Jump, nicht in der Boot-Stage; ein Diff mit falscher Annahme (`--eq res,s1`) findet 0 Treffer. Immer per Overlay-fp gegenchecken, welche Saves wirklich gleich/verschieden sind. Die Treffer sind **Kandidaten** → in §5 mit Ghidra bestätigen.

## 5. Gefundenes Global mit Ghidra BENENNEN (`scripts/re15_ghidra.py`)

Decompiliert jede Funktion, die die Adresse referenziert → zeigt den Mechanismus byte-genau. Der Wrapper baut `code.bin` (strippt den 0x800-Header), cached das Ghidra-Projekt (erster Lauf import+analyze ~25–40 s, danach `-process`, wenige Sekunden) und gibt das decompilierte C aus.

```bash
python scripts/re15_ghidra.py 0x800b532e 0x800b5330        # decompiliert + druckt
python scripts/re15_ghidra.py 0x800acaee --out hp.c        # in Datei schreiben
python scripts/re15_ghidra.py 0x800b532e --reanalyze       # frischen Import erzwingen
```

Darunter ruft er `analyzeHeadless … -loader BinaryLoader -loader-baseAddr 0x80010000 -postScript decomp_globals.py <out> <addrs>` (Jython-Postskript `scripts/decomp_globals.py`). analyzeHeadless: `…/Downloads/ghidra_11.4.2_PUBLIC_20250826/…/support/analyzeHeadless.bat`, Java 17.

**Belegtes Beispiel aus dieser Session:** Ein Diff flaggte `0x800b532c/0x800b532e` (Werte 1 in Stage-1-Raum, 2 in Stage-2-Raum) als vermeintliche Stage-ID. Die Ghidra-Decompilation widerlegte das byte-genau: `DAT_800b5330 = DAT_800b532e << 3` und `DAT_800b532e` indiziert die Kamera-Cut-Tabellen @`0x8008f82c` → es ist der **aktive Kamera-Cut-Index**, nicht die Stage (die Werte 1/2 waren zufällig der Start-Cut). Genau dafür ist Schritt 5 da: nicht den Diff-Wert glauben, den Mechanismus belegen.

## 6. Enemy-AI-State dumpen (`scripts/re15_enemy_state.py`)

Dumpt die **live Gegner-KI** aus dem Savestate: das Enemy-Array `DAT_800acc2c` (stride `0x1f4`) + den Player-Block, und labelt jeden Gegner per der byte-true RE'ten FSM-Map (`RE15_FUN_CATALOG.md` Sektion „Enemy AI" + Memory `reai-v2-foundation-combat`). Spart die manuelle Forensik, die sonst pro Save anfällt.

```bash
python scripts/re15_enemy_state.py <savestate.sav>
```

Verifizierter Output (Live-Combat-Save `stage_saves/mzd_stage1_combat_death.sav`, 7 Zombies + toter Spieler):

```
player HP    : -1   |  player state : 7 (death/grabbed)  |  active count : 7
slot type  +0x4 +0x5 +0x9(&f) act  dist   flags hitbox    label
0    0x10  1    6    0x00(0)  0x01 357    0x11  400/1440  active: approach (+0x5=6)
...                                              450/1530  idle           (type 0x47, HASH_1)
```

Felder + FSM-Map (alle byte-true belegt): **+0x4** Main-State → `PTR_FUN_801217a0[+0x4]` (0=init 1=active-AI(FUN_8011d9f4) 2=hurt 3=death 4=idle); **+0x9** Sub-State (`(&0xf)`→`PTR_FUN_801217b4`; 0=sub0=FUN_8011da48, aktive Humanoid-AI); **+0x5** Anim-Phase (FUN_8011da48→`DAT_801217b8[+0x5]`; Attack-Commit `0x701` setzt +0x5=7); **+0x0** low = Model-Instanz-Action (`@0x80071d40`; **0x16-0x19 = Lunge-Sequenz = Hitbox-Fenster**); **Hitbox** = `*(ent+0x78)` radius_min@+6/height@+8/radius_max@+10 (Typ 0x47 = 450/1530, Typ 0x10/0x11/0x16 = 400/1440). Die eigentliche Hitbox `FUN_80017fa4` feuert über den Action-Driver `FUN_80019e20`, wenn die Lunge-Anim die aktiven Frames erreicht.

## 7. DORMANZ-Check: ein Gate-Bit über ALLE Saves sweepen (`scripts/re15_flag_sweep.py`)

**Bevor du einen Code-Pfad portierst, der auf einem Flag/Bit gegated ist, prüfe ob das Bit im echten Spiel je gesetzt wird.** Ein statisches `if (DAT & bit)` im Disassembly heißt NICHT, dass der Pfad live läuft — das Bit könnte im Prototyp nie gesetzt werden (dormant).

```bash
python scripts/re15_flag_sweep.py 0x800aca3c 0x1     # Bit 0x1 über alle stage_saves/*.sav
python scripts/re15_flag_sweep.py 0x800acaee          # einen Wert (ohne Maske) sweepen
python scripts/re15_flag_sweep.py <addr> <mask> --width 1|2|4 --glob "stage_saves/*.sav"
```

Dumpt das Global pro Save + den Overlay-Fingerprint (gruppiert nach Stage, §3) + ein **VERDICT**: ist das Bit in 0 Saves gesetzt → DORMANT (der gegatete Pfad läuft nicht; den echten In-Game-Mechanismus ermitteln, nicht das statische Gate annehmen); sonst → live (+ in welchen Stages).

**Beleg (Phase 8.7, 2026-06-29 — der Befund, der das Attack-Modell korrigierte):** `DAT_800aca3c & 1` (das STAGE1-Zombie-Lunge-Arm-Gate, das `FUN_8010ab2c` liest) ist über ALLE 9 Saves — inkl. dem Live-ROOM1140-Combat mit 7 aktiven Zombies — **nie** gesetzt. → Der Lunge-Arm ist im Prototyp **dormant**; die Zombies greifen per **GRAB** an (FUN_80102548, −10/−5 HP), nicht per Lunge. Der Sweep hat eine ganze (sonst plausibel byte-true portierte) Annahme widerlegt. Siehe Memory `disasm-verify-decompiles` + `reai-v2-foundation-combat`.

→ Kombiniere mit §6 (`re15_enemy_state.py`, der Live-Gegner-State) für die Combat-RE: **erst den Live-Combat-Save dumpen** (welche State/+0x5/+0x9/Hitbox erreichen die Gegner WIRKLICH?) + Gate-Bits sweepen, DANN den bestätigten Pfad byte-true portieren. So wurden 8.7–8.9 (Wake-up/Grab/Turn) gegen die Realität verankert + zwei Modell-Annahmen korrigiert.

## Disziplin

„Wert ändert sich plausibel zwischen zwei Savestates" ist **kein** Beweis für die Bedeutung einer Adresse. Erst wenn Ghidra zeigt, welche Funktion das Global wie verwendet, ist es byte-true. Diff findet Kandidaten — Ghidra bestätigt/benennt sie. Und: ein statisches Gate (`if (DAT & bit)`) heißt nicht, dass der Pfad live ist — §7 sweepen, sonst portierst du dormanten Code als „den In-Game-Mechanismus".
