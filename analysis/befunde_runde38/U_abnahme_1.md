# Runde 38 Spur U "wasserzombie" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, geprueft HEAD `76b6fa12`
(letzter Code-Commit `2e04fd53`; merge-base mit master `cb782bda`). Vorgaenger: `U_abnahme_0.md` (a4809c35, 2 Maengel).
Der Teilbericht `U_abnahme_1_teil_haenger.md` (abgebrochener Vorlauf dieser Abnahme) ist NICHT Grundlage dieses Urteils —
alles hier ist neu gemessen. Fortlaufend geschrieben (Sitzungslimit).

## 0. Stand der Pruefung

- [x] AUFTRAG/VERTRAG/Dossier (inkl. Abschnitt 8) und Abnahme 0 gelesen
- [x] Code-Diff gelesen (`git diff cb782bda HEAD`, 13 Dateien; `git diff master` zeigt zusaetzlich Orchestrator-Dateien
      nur, weil master seit der Basis weitergelaufen ist) und `git diff 371f4dc8 HEAD` (Nachbesserung 1 allein)
- [x] Bau: `local_build.sh configure` OK (3 s), `local_build.sh build` -> `ninja: no work to do`; Arbeitsbaum sauber;
      exe 06:53:01 neuer als alle Quellen (wasserzombie_r38.c 06:51:24); exe enthaelt `zwischenbild` und
      `RE15_WASSERZOMBIE_LOG`; md5 re15_pc.exe `2a5d6e340081ecbae35df3a9e45f7f55`
- [x] Riegel der Spur: `ctest -R "^(unit_r38_wasserzombie|integration_r38_wasserzombie)$" --timeout 900` ->
      `100% tests passed, 0 tests failed out of 2` (unit 0.16 s, integration 135.22 s, Laeufe a-e); Lauf e wz.log
      woertlich: `F400 R2000 slot=3 t=16 zwischenbild st=1/9/0 mo=22 af=49 ...` / `F401 ... abbruch st=1/9/0 mo=22 af=50`
- [ ] Gezielte Gruppen der beruehrten Module
- [ ] Nutzer-Punkt an der exe (Leon/Elza, beide KI, Bild, echter Tuerweg)
- [ ] Mangel 1 nachgemessen
- [ ] Mangel 2 geprueft
- [ ] Nebenwirkungen der Nachbesserung (Code-Pfade)
- [ ] RE-Gate (Stichproben selbst disassembliert)
- [ ] Vertrag
- [ ] Urteil

## 1. Nutzer-Punkt (Wortlaut)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## 2. Messung an der exe

Messkopie `re15_port/build/platform/pc/re15_pc_u_abn1b.exe` (md5 `2a5d6e34...` = re15_pc.exe; eigener Name, kein
taskkill; jeder Lauf mit `timeout -k 5`). Arbeitsordner (nicht eingecheckt): Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/U38abn1b/`
(Skript `tools/lauf.sh`, Kontaktbogen `tools/sheet.py`, je Lauf ein Unterordner mit debug.log/wz.log/gy.log/fd/*.ppm).
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
RE15_WASSERZOMBIE_LOG=wz.log RE15_GEGNER_Y_LOG=gy.log RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=-18500,-6400,2048,0`,
Wegplan `RE15_WEGPLAN` = `RAUM 2070 / WARTE 20 / BLICK 2048 / WARTE 10 / AKTION / RAUM 2000 / WARTE 10 /
GEHE -20600 -10600 200 / BLICK 2048 / WARTE 1500 / ENDE` = ECHTER Tuerweg ROOM2070 -> ROOM2000 (Leon:
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`; Elza: 2071 -> 2001 ueber das Elza-Bit des Debug-Sprungs,
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=3:69`).

### M1 — ECHTER WEG Tuer 2070 -> 2000, Leon, Standard-KI (RE2), Bild F130-F410

Befehl: `tools/lauf.sh <exe> m1_leon_re2 "410#2000" leon 300 "RE15_FRAMEDUMP=130-410/1:fd/f"` -> `exit=0 dauer=31s`.
debug.log: `[pl] Spieler-Familie PL00`, `[debug-menu] AUTO-JUMP -> ROOM2070 (Frame 250)`,
`[aot] DOOR FIRE slot=0 rect=(-19450,-6400,hw=500,hh=1500) target_cut=13 spawn=(-20650,-1800,-6200)`,
`[room] PC loaded room2000.rdt (359612 bytes)`, `[flow] EXIT_AT: Bild 410 in Raum 2000 erreicht -> exit`.
wz.log (woertlich):
```
F144 R2000 slot=4 t=16 re2-eintritt st=1/10/0 mo=27 af=18 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F144 R2000 slot=4 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F150 R2000 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=0 kontakt=00
F220 R2000 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10290) b0 f1ba=-1800 rot=0 kontakt=00
F308 R2000 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F308 R2000 slot=4 t=16 re2-uebergabe st=1/0/1 mo=0 af=3 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F340 R2000 slot=3 t=16 re2-eintritt st=1/10/0 mo=0 af=63 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F340 R2000 slot=3 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F351 R2000 slot=3 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F400 R2000 slot=3 t=16 zwischenbild st=1/9/0 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F401 R2000 slot=3 t=16 abbruch st=1/9/0 mo=22 af=50 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
```
Lesart: Kontakt 0x09 = Richtung 0 / u1 = 1 (Kanalwand x = -21650 von Band 0 aus); Drehen 87 -> 0 in 0x10-Schritten
(6 Bilder), Clip 0x16 von F150 (Bild 0) bis F308 (Bild 158) = 159 Bilder; +0x1ba -1800 genau bei Clip-Bild 70 (0x46);
Ende: y -1800, Band 1, Vorschub +1140 in x (-22068 -> -20928 = 0x474 entlang Blick 0).
Bild (`m1_climb_crop.png`, F140-F310, Ausschnitt x30-190/y70-220, 2x; `m1_full_overview.png` ganz): F140 Zombie im
Kanal an der Wand, F150-F200 Haende/Oberkoerper auf der Stegkante, Beine haengen im Kanal, F220 Knie kommt hoch
(Schatten liegt ab hier auf dem Steg), F240-F260 kniet auf der Kante, F280-F300 richtet sich neben Leon auf, F310
steht auf dem Steg und greift Leon. **Das ist ein Hochziehen an den Rand.**

### M5 — Mangel 1 nachgemessen (derselbe Lauf M1, Bilder F396-F405)

gy.log Slot 3 (woertlich, Slot-4-Teil gekuerzt):
```
F399 R2000 PL(-20526,-1800,-10338 b1) [3 t=16 st=1/10/3/0 g=00 mo=22 af=48 @(-22068,0,-9970) b0 f1ba=0 ...]
F400 R2000 PL(-20718,-1800,-10232 b1) [3 t=16 st=1/9/0/0 g=00 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 ...]
F401 R2000 PL(-20909,-1800,-10127 b1) [3 t=16 st=1/9/1/0 g=00 mo=13 af=23 @(-22468,0,-9970) b0 f1ba=0 ...]
```
- wz.log: `F400 ... zwischenbild st=1/9/0 mo=22 af=49` und `F401 ... abbruch st=1/9/0 mo=22 af=50` (Abnahme 0, alter
  Stand: `abbruch ... af=9`). Die Uhr laeuft im Zwischenbild auf der RE1.5-Bank weiter (49 -> 50, Clip 0x16 = 159).
- Gegenprobe Ausfuehrung: die ersten 720 Zeilen meines gy.log (bis F406) sind **byte-gleich** mit dem gy.log der
  Abnahme 0 auf dem alten Stand a4809c35 (`scratchpad/m_domino/gy.log`, gleicher Weg, `cmp` ohne Ausgabe, md5 beide
  `68c1de6c05f1744770b2af4976c0eb9b`) -> die Nachbesserung aendert keine Ausfuehrung, nur Bank/Uhr/Schatten im Zwischenbild.
- Bild (`m5_zwischenbild_crop.png`, F396-F405, Ausschnitt x20-180/y80-230, 2x): F396-F399 haengt der Kletterer mit dem
  Oberkoerper auf der Stegkante unter dem greifenden Zombie; **F400 haengt er unveraendert an der Kante** (alter Stand,
  `scratchpad/m_domino/crop_domino2.png`: F400 bis auf ein Bein unten links verschwunden); F401-F405 taumelt er im
  RE2-Taumel-Clip 13 rueckwaerts durch den Kanal (eigener Abbruch-Tick, EXEC[9] P0 + Schub 400 — siehe Hinweise).
- **Mangel 1 ist behoben.**

### M1-vorher — derselbe Tuerweg mit der v0.8.25-exe (erklaert der Fix den Nutzerbefund?)

`release/pkg-win/re15_port_v0.8.25/re15_pc.exe` (Hauptbaum, nur gelesen) als Kopie `scratchpad/U38abn1b/v0825/
re15_pc_v0825_u1b.exe` (md5 `7aa892fc...` beide), Assets des Pakets (`RE15_CD_ROOT=<paket>/shared_assets/PSX
RE15_RE2_ASSET_ROOT=<paket>/shared_assets/RE2`), identischer Wegplan, `RE15_EXIT_AT=700#2000` -> `exit=0 dauer=41s`.
Tuer feuert identisch (`[aot] DOOR FIRE slot=0 ... spawn=(-20650,-1800,-6200)`, `PC loaded room2000.rdt`).
gy.log ausgezaehlt (`tools/inv.py` + Zaehler): 1400 Typ-0x16-Eintraege, **0 auf Band 1**, 673 an der Kanalwand
x = -22068; z.B. `F500 ... [3 t=16 st=1/14/2/0 ... @(-22068,0,-10044) b0 ...] [4 t=16 st=1/14/1/0 ... @(-22068,0,-10894) b0 ...]`.
Das ist der Nutzerbefund (laufen zum Rand, kommen nie hoch). Mit dem Spurstand ziehen sich auf demselben Weg beide hoch
(M1: F308 Slot 4 oben; Slot 3 wird nur durch den RE2-Partner-Domino unterbrochen). Der Fix erklaert den Befund: das
RE2-Gehirn hat kein Hochziehen (Abnahme 0 Nr. 8: kein +-1800 in EMZ0/EMOVL10), der neue Dispatch fuehrt die RE1.5-Maschine ein.

### M2 — ECHTER WEG 2070 -> 2000, Leon, `RE15_AI_FLAVOR=re15`, Bild

`tools/lauf.sh <exe> m2_leon_re15 "620#2000" leon 300 RE15_AI_FLAVOR=re15 "RE15_FRAMEDUMP=140-620/10:fd/f"` ->
`exit=0 dauer=57s`. wz.log (woertlich):
```
F146 R2000 slot=4 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=4002 kontakt=09
F152 R2000 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=0 kontakt=00
F222 R2000 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-9931) b0 f1ba=-1800 rot=0 kontakt=00
F310 R2000 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-9931) b1 f1ba=-1800 rot=0 kontakt=00
F426 R2000 slot=3 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-10474) b0 f1ba=0 rot=63 kontakt=09
F430 R2000 slot=3 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10474) b0 f1ba=0 rot=0 kontakt=00
F500 R2000 slot=3 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10474) b0 f1ba=-1800 rot=0 kontakt=00
F588 R2000 slot=3 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10474) b1 f1ba=-1800 rot=0 kontakt=00
```
gy.log: 0 Gegner-Eintraege unter dem Wasserboden (y > 0), 0 auf Band 2+ (Vorbefund der RE1.5-KI: Sturz bis y = +1800,
danach Band 2). F589/F620 steht Slot 3 auf (-20928,-1800,-10474) b1. Bild (`m2_re15_crop.png`): F150-F300 Haende auf
der Kante, Oberkoerper drueber, Knie, Aufrichten neben Leon; F480 zweiter Zombie mit dem Oberkoerper auf der Kante.
Leon wehrt sich im Wegplan nicht und stirbt (F540 Weissblitz, F590 "YOU DIED") — erwartbar, nicht Spur U.

### M3 — ECHTER WEG 2071 -> 2001, Elza, Standard-KI, Bild inkl. Zwischenbild

`tools/lauf.sh <exe> m3_elza_re2 "410#2001" elza 300 "RE15_FRAMEDUMP=130-410/1:fd/f"` -> `exit=0 dauer=46s`.
debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `AUTO-JUMP -> ROOM2070` (Elza-Bit) ->
`PC loaded room2071.rdt`, `DOOR FIRE slot=0 ...`, `PC loaded room2001.rdt (360660 bytes)`. wz.log (woertlich, Auszug):
```
F144 R2001 slot=2 t=16 re2-eintritt st=1/10/0 mo=27 af=18 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F150 R2001 slot=2 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=0 kontakt=00
F220 R2001 slot=2 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10290) b0 f1ba=-1800 rot=0 kontakt=00
F308 R2001 slot=2 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F400 R2001 slot=1 t=16 zwischenbild st=1/9/0 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F401 R2001 slot=1 t=16 abbruch st=1/9/0 mo=22 af=50 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
```
gy.log: 0 unter dem Wasserboden, 0 auf Band 2+. Bild (`m3_elza_crop.png`): F150-F310 Hochziehen neben Elza (Haende,
Knie, Knien, Aufrichten, Griff); F397-F400 haengt der zweite Kletterer an der Kante — **auch F400** (Abnahme 0 alter
Stand: bis auf ein Bein leer); F401/F402 Taumel im Kanal. (Die 2001-Wasserzombies spawnen nur mit Flag (3,0x45),
Original-Skript ROOM2001 sub00 @0x1826/@0x182A — Abnahme 0 M3; deshalb `RE15_SET_FLAG=3:69`.)

### M4 — ROOM2020 ueber den Raumlader (Debug-Sprung), Leon, Standard-KI, Bild

Ein Tuerweg nach ROOM2020 war nicht fahrbar: Door_aot_set-Zensus (eigenes Skript `re/doors.py` ueber den
SCD-Walker von `tools/aot_sce_census.py`) -> nach 2020 fuehren nur ROOM2030 @0x01b2a (Slot 2) und ROOM2050 @0x01c6a
(Slot 1). In ROOM2030 ist Slot 2 zur Laufzeit nicht installiert (Wegplan `AOTS`: nur Slots 0/1/3 vom Typ 1); 2050 -> 2020
feuerte schon in Abnahme 0 nicht (ausserhalb der Spur). Deshalb Raumlader (derselbe scd_room_reenter wie eine Tuer):
`RE15_DEBUG_JUMP=2020@250 RE15_PLAYER_POS=-9500,2600,1024,1 RE15_FORCE_CUT=6 RE15_FRAMEDUMP=60-300/10:fd/f
RE15_EXIT_AT=300#2020` -> `exit=0 dauer=19s`. wz.log (woertlich):
```
F80 R2020 slot=4 t=16 re2-eintritt st=1/10/0 mo=0 af=31 @(-10756,0,1082) b0 f1ba=0 rot=3527 kontakt=c9
F109 R2020 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-10756,0,1082) b0 f1ba=0 rot=3072 kontakt=c0
F179 R2020 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-10756,0,1082) b0 f1ba=-1800 rot=3072 kontakt=c0
F267 R2020 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-10756,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
```
Kontakt 0xc9 = Richtung 0xC / u1 = 1 (Suedwand z = 1500); Vorschub +1140 in z. gy.log: 0 unter dem Wasserboden, 0 auf
Band 2+. Bild (`m4_2020_sheet.png`, Kanalblick per FORCE_CUT 6, nur zur Sicht): F80-F170 Arme/Oberkoerper auf der
Stegkante, F200-F230 Beine kommen hoch. ROOM2021 (Elza) deckt der Riegel `integration_r38_wasserzombie` Lauf c ab (gruen).

## 3. Mangel 2 (Code-Hygiene) geprueft

- `grep -rn "re15_enemy_ai_live_edge_fall|EDGE-FALL|edge_fall" engine/src include tests platform`: einziger Treffer
  `include/re15_wasserzombie_r38.h:30` (Abschnitt "WAS DER PORT VORHER TAT") — die tote Funktion ist entfernt.
- Kommentar am Aufruf `enemy_ai_common.c:4732-4734`: "HOCHZIEHEN an der u1=1-Kante (@0x8011f890[0xa]=FUN_801033c8 /
  STAGE2 0x8010325c): +0x38 -= 1800 und +0x82 += 1 = ein Band HOCH ..." — stimmt mit der Disassembly (Abschnitt 4).
- `gcc -std=c11 -Wall -Wextra -fsyntax-only -DRE15_PLATFORM_PC=1 -Iinclude` auf wasserzombie_r38.c,
  enemy_ai_re2_zombie.c, schuss_r36.c, enemy_ai_common.c, test_r38_wasserzombie.c, probe_r38_wasserzombie_karte.c: keine
  Meldung aus `re15_wasserzombie_r38.h` oder einer Spur-U-Zeile. Uebrig in enemy_ai_common.c Z. 1518 (`-Wunused-variable`
  px0/pz0), 6113 (`-Wmisleading-indentation`), 11346 (`-Wsign-compare`) — `git blame` 2026-08-25/07-06/07-26, und
  derselbe Lauf auf `cb782bda:enemy_ai_common.c` meldet dieselben drei (Z. 1517/6152/11385) -> vorbestehend.
- **Mangel 2 ist behoben.**

## 4. RE-Gate — selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`)

Binaerdateien: `info/Re1.5/PSX.EXE`, `info/Re1.5/PSX/BIN/STAGE2.BIN` (roh @0x80100000), `info/re2leon/PSX.EXE`,
`info/re2leon/COMMON/BIN/EMOVL10_S0.BIN`.

Stichproben zu den NEUEN Belegen der Nachbesserung:
1. **anim_set FUN_8001f314 (RE1.5 PSX.EXE)** — `dis 0x8001f314 40`: `8001f324 lbu v0,148(t0)` (+0x94),
   `8001f330 addu a2,a1,v0` / `8001f334 lw v0,0(a2)` (Clipzeile aus der UEBERGEBENEN Bank a1), `8001f35c lbu v0,149(t0)`
   (+0x95), `8001f36c sw a2,360(t0)` (+0x168), `8001f38c jal 0x8001f3bc` (sonst `8001f3a4 jal 0x8001f8b4`). Stimmt.
2. **FUN_8001f3bc** — `8001f40c lw s1,392(v1)` (v1 = g_entity(cur) -> +0x188 Teileblock), `8001f42c sw v0,44(s1)` /
   `8001f430 sh v0,56(s1)`, `8001f570 addiu a0,s1,120` / `8001f574 jal 0x80068098`, `8001f5f0 jal 0x80068098` (a1 = s1+24
   im Delay-Slot), `8001f610-1c lbu/addiu 1/sb 149` (+0x95 += 1). 0x80068098 = Drehmatrix (RE15_FUN_CATALOG.md Z. 62,
   `8002b4d4 jal 0x80068098`). Stimmt: die Pose wird im eigenen anim_set-Aufruf in den Teileblock gerechnet.
3. **Eigener Lade-Scan** Offset 148/149, Basis != sp, ueber PSX.EXE (`re/scan9495.py`): **31 Treffer** (= Zahl im Dossier).
   Byte-Leser nur in FUN_8001ae38 (0x8001ae48/5c), anim_set/FUN_8001f3bc/FUN_8001f8b4 (0x8001f324-0x8001fb4c) und
   0x800526d4-0x80052cbc; die Halbwort-/Wort-Leser haben eine andere Basis als der Aktor (Stichprobe `8001b5f4 lw v0,
   g_entity(cur)` neben `8001b5fc lhu a0,148(s0)` — s0 ist nicht der Aktor; `800375d4-e0` dasselbe Muster;
   `8001ed50 lw a1,148(s2)` / `lw a2,152(s2)` = Wortpaar eines anderen Objekts). Kein Zeichner, der +0x94/+0x95 des
   Aktors neu auswertet.
4. **RE2-Partner-Domino (EMOVL10_S0.BIN)** — `dis 0x80102E84 12`: `80102e90 addiu v0,zero,2305` (0x901),
   `80102e94 jal 0x80015910`, `80102e98 sw v0,4(s0)` (Delay-Slot), `80102ea4 jal 0x8005bd6c` (a0 = 4, SE),
   `80102ea8 sb v0,363(s0)` (+0x16B). RE2 PSX.EXE `dis 0x80015910`: nur `lh 118(a1)` / `lh 118(a0)` / `subu` /
   `addiu 1024` / `andi 0xfff` / `jr ra` / `slti 2048` — reine Seitenpruefung. Kein Clip/Bild/Teileblock des Partners.
   Stimmt; der Port-Zwilling enemy_ai_re2_zombie.c:2169 schreibt ebenso nur das Wort (+ `re2z_gaitrow`, + SE 4).
5. **EXEC[9] P0 (EMOVL10_S0.BIN)** — `dis 0x80103EF0 36`: `80103f30 sw v0,332(s1)` (Clipwort, Delay-Slot), Tabelle
   `bytes 0x801000D8` = `03 03 04 0d` ({3,3,4,0x0D}), `80103f60 addiu v0,zero,400` / `80103f64 sh v0,324(s1)` (Schub
   +0x144 = 400). Stimmt (Riegel F prueft den Clip in {3,4,0x0D}).
6. **Sce_em_set (RE1.5 PSX.EXE)** — `dis 0x800421c0 22`: `800421e0 sw zero,4(s0)` (Zustandswort 0), `800421f8-80042210`
   v0 = -(pc[2]*1800) -> `sh v0,442(s0)` (+0x1ba). Stimmt (Grund fuer `e->state != 0` in `wz_marke_bank`).

Kern-Mechanik erneut selbst geprueft:
7. `table 0x80117994 --bin STAGE2.BIN`: [10] = **0x8010325c**. `dis 0x8010325c 136`: Phase 0 `801032b0 sb v1,6` /
   `801032c0 sb v1,148` (v1 = 1 aus `80103288`) / `801032d0 sb zero,149` / `801032dc-e0 ori 7 / sb 143`; Phase 1
   `801032f8 jal 0x8001f314` (a2 = 0 `801032ec`, a3 = 0x200 `801032fc`), `80103310 ori a1,zero,0x10` / `80103318 jal
   0x8001aa68` (a0 = (+0x90&0xf0)<<4), `80103340 sh v1,106` (Delay-Slot, immer), d == 0 -> `80103354 sb 2,6`; Phase 2
   `80103368 ori 0x16` / `8010336c sb 148`, `80103398 ori 0x474` / `8010339c sh 140`, `801033bc sh v0,106`; Phase 3
   `801033d0 ori 0x46`, `801033e0 sh 0x474,176`, `801033f0-fc lhu 442 / addiu -1800 / sh 442`, `80103414 jal 0x8001f314`
   (a3 = 0x200 `80103418`), `8010341c beq v0,zero` / Delay `80103420 addu a0,zero,zero`, `80103430-3c` +0x82 += 1,
   `8010344c-58` +0x38 -= 1800, `80103464-68` +0x4 = 1, `80103474 jal 0x800245d8` / Delay `80103478 sh zero,176`.
   Zeile fuer Zeile = `re15_wasserzombie_hochziehen` und alle `RE15_WZ_*`-Konstanten im Kopf.
8. Ausloeser `dis 0x80101f00 18 --bin STAGE2.BIN`: `lbu a0,144(a1)` / `andi 0x3` / `beq` / `andi 0xf0` / `lh v1,106` /
   `sll 4` / `subu` / `addiu 512` / `andi 0xfff` / `slti 1024` / `andi 0x1` / `addiu 9` / `sll 8` / `ori 1` /
   `j 0x80102078` / `sw v0,4(a1)` = `re15_wasserzombie_kante_vorn`.

Diff-Suche (`git diff cb782bda HEAD -- re15_port`, 1305 Plus-Zeilen) nach `deferred|tunable|interim|for now|faithful|
plausib|TODO|FIXME|approx|geschaetzt|vermut|ungefaehr|heuristi|sieht .. aus`: **0 Treffer**. Einziger neuer `getenv`:
`RE15_WASSERZOMBIE_LOG` (Messschiene, oeffnet nur eine Logdatei; kein Verhalten, kein Abschluss-Schalter). Die
Nachbesserung bringt keine neue Konstante (`wz_marke_bank`: `state != 0` = Sce_em_set @0x800421e0, Nr. 6; `zwischen` =
nur Messschiene). Gekennzeichnet: PORT-WAHL (Rig-Wechsel ohne Ueberblendung; +0x1ba-Ruecksetzen beim RE2-Abbruch),
PORT-EMULATION (LOOP-Bit 0x04 in Phase 0/1, Rueckwaerts-Bit 0x80 aus, a2 = 0 @0x801032ec), PORT-ABBILDUNG (Eintritt aus
dem RE2-Gang 1/1, weil das RE2-Gehirn kein Hochziehen hat — Nutzer-Wortlaut "wie im Original Resident Evil 1.5").
**RE-Gate haelt.** Der Fix erklaert den gemessenen Befund (Mangel 1): Bank/Uhr hingen am fremd geschriebenen Wort; jetzt an
der Marke des eigenen Ticks -> F400 Kletterpose, af 49 -> 50 (M5), Ausfuehrung byte-gleich zum alten Stand.

