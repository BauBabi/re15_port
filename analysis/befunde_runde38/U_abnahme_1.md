# Runde 38 Spur U "wasserzombie" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, geprueft Stand `34771078`
(letzter Code-Commit `2e04fd53`; die Commits danach sind nur Abnahme-Berichte; merge-base mit master `cb782bda`).
Vorgaenger: `U_abnahme_0.md` (Stand a4809c35, 2 Maengel). Die Teilberichte `U_abnahme_1_teil_haenger.md` und
`U_abnahme_1_teil_limit.md` (abgebrochene Vorlaeufe dieser Abnahme, UNGEPRUEFT) sind NICHT Grundlage dieses Urteils:
jede Aussage hier ist in dieser Sitzung neu gemessen bzw. neu disassembliert. Fortlaufend geschrieben (Sitzungslimit).

## 0. Stand der Pruefung

- [x] AUFTRAG/VERTRAG/Dossier (inkl. Abschnitt 8) und Abnahme 0 gelesen
- [x] Code-Diff gelesen: `git diff cb782bda HEAD -- re15_port` (10 Code-/Test-Dateien) und `git diff 371f4dc8 HEAD`
      (Nachbesserung 1 allein). `git diff master` zeigt zusaetzlich Orchestrator-Dateien (AUFTRAG/VERTRAG/UEBERGABE,
      orchestrator/*) nur, weil master seit der Basis weitergelaufen ist — nicht Teil der Spur.
- [x] Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`,
      `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`;
      Arbeitsbaum sauber; exe 2026-10-17 06:53:01 neuer als alle Quellen (wasserzombie_r38.c 06:51:24,
      enemy_ai_common.c 06:36:42, main.c 2026-10-16 22:59); exe enthaelt `zwischenbild` und `RE15_WASSERZOMBIE_LOG`;
      md5 re15_pc.exe `2a5d6e340081ecbae35df3a9e45f7f55`
- [x] Riegel der Spur: 2/2 gruen (Abschnitt 5)
- [ ] Gezielte Gruppen der beruehrten Module
- [ ] Nutzer-Punkt an der exe (Leon/Elza, beide KI, echter Tuerweg, Bild)
- [x] Mangel 1 der Abnahme 0 nachgemessen (M5): behoben
- [ ] Mangel 2 der Abnahme 0 geprueft
- [ ] RE-Gate (Stichproben selbst disassembliert)
- [ ] Vertrag
- [ ] Urteil

## 1. Nutzer-Punkt (Wortlaut)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## 2. Messung an der exe

Messkopie `re15_port/build/platform/pc/re15_pc_u_abn1c.exe` (md5 `2a5d6e340081ecbae35df3a9e45f7f55` = re15_pc.exe; eigener
Name, kein taskkill; jeder Lauf mit `timeout -k 5`). Arbeitsordner (nicht eingecheckt): Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/U38abn1c/`
(Skript `tools/lauf.sh`, Kontaktbogen `tools/sheet.py`, je Lauf ein Unterordner mit debug.log/wz.log/gy.log/fd/*.ppm).
Gemeinsame Umgebung von `tools/lauf.sh`: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
RE15_WASSERZOMBIE_LOG=wz.log RE15_GEGNER_Y_LOG=gy.log RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=-18500,-6400,2048,0`,
Wegplan `RE15_WEGPLAN` = `RAUM 2070 / WARTE 20 / BLICK 2048 / WARTE 10 / AKTION / RAUM 2000 / WARTE 10 /
GEHE -20600 -10600 200 / BLICK 2048 / WARTE 1500 / ENDE` = ECHTER Tuerweg ROOM2070 -> ROOM2000 (Leon:
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`; Elza: 2071 -> 2001 ueber das Elza-Bit des Debug-Sprungs,
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=3:69`).

### M1 — ECHTER WEG Tuer 2070 -> 2000, Leon, Standard-KI (RE2), Bild F130-F410

Befehl: `tools/lauf.sh <exe> m1_leon_re2 "410#2000" leon 300 "RE15_FRAMEDUMP=130-410/1:fd/f"` -> `exit=0 dauer=31s`,
281 Bilder. debug.log: `[pl] Spieler-Familie PL00 (character=0, Elza-Bit=0)`, `[debug-menu] AUTO-JUMP -> ROOM2070
(Frame 250)`, `[room] PC loaded room2070.rdt`, `[aot] DOOR FIRE slot=0 rect=(-19450,-6400,hw=500,hh=1500) target_cut=13
spawn=(-20650,-1800,-6200)`, `[room] PC loaded room2000.rdt (359612 bytes)`, `[flow] EXIT_AT: Bild 410 in Raum 2000
erreicht -> exit`. wz.log (vollstaendig, woertlich):
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
Lesart: Kontakt 0x09 = Richtung 0 / Bit 0 = u1 1 (Kanalwand von Band 0 aus); Drehen 87 -> 0 in 0x10-Schritten (F144 ->
F150), Clip 0x16 von F150 (Bild 0) bis F308 (Bild 158) = 159 Bilder; +0x1ba -1800 genau bei Clip-Bild 70 (0x46);
Ende y -1800, Band 1, Vorschub 1140 in x (-22068 -> -20928 = 0x474 entlang Blick 0).
Bild (selbst angesehen): `m1_overview.png` (F140/180/220/260/300/310, ganz) und `m1_climb_crop.png` (F140-F310 in
10er-Schritten, Ausschnitt x50-190/y60-170, 2x): F140 Zombie im Kanal vor der Stegkante neben Leon; F150-F200 Haende und
Oberkoerper auf der Stegkante, Beine haengen im Kanal; F220-F240 er stemmt sich hoch, ein Knie kommt auf die Kante;
F250-F270 kniet auf der Kante; F280-F300 richtet sich neben Leon auf; F310 steht auf dem Steg und greift Leon.
**Das ist ein Hochziehen an den Rand.** Der zweite Zombie steht dabei im Kanal (setzt ab F340 selbst an).

### M5 — Mangel 1 der Abnahme 0 nachgemessen (derselbe Lauf M1, Bilder F396-F405)

- wz.log: `F400 ... slot=3 ... zwischenbild st=1/9/0 mo=22 af=49` / `F401 ... abbruch st=1/9/0 mo=22 af=50` — der
  Advancer taktet im Folgebild auf der RE1.5-Bank weiter (49 -> 50, Clip 0x16 = 159 Bilder). Abnahme 0 (alter Stand
  a4809c35, Scratchpad `m_domino/wz.log`): `F401 ... abbruch st=1/9/0 mo=22 af=9`.
- `diff m_domino/wz.log U38abn1c/m1_leon_re2/wz.log`: einziger Unterschied ist genau diese Stelle
  (`< F401 ... abbruch ... af=9` gegen `> F400 ... zwischenbild ... af=49` / `> F401 ... abbruch ... af=50`).
- gy.log (Zustand/Clip/Bild/Lage/Band/+0x1ba aller Gegner je Bild): die ersten 720 Zeilen (bis F406) meines Laufs sind
  **byte-gleich** mit dem gy.log der Abnahme 0 auf dem alten Stand (`cmp` ohne Ausgabe; md5 beide
  `68c1de6c05f1744770b2af4976c0eb9b`) -> die Nachbesserung aendert keine Ausfuehrung, nur Bank/Uhr/Schatten im
  Zwischenbild. Slot 3 woertlich: `F399 ... [3 t=16 st=1/10/3/0 g=00 mo=22 af=48 @(-22068,0,-9970) b0 f1ba=0 ...]`,
  `F400 ... [3 t=16 st=1/9/0/0 g=00 mo=22 af=49 ...]`, `F401 ... [3 t=16 st=1/9/1/0 g=00 mo=13 af=23 @(-22468,0,-9970) ...]`.
- Bild (selbst angesehen): `m5_full.png` (F398-F401 ganz, 2x) und `m5_zwischenbild_crop.png` (F396-F405, Ausschnitt
  x20-180/y80-230, 3x): F396-F399 haengt der zweite Kletterer mit dem Oberkoerper auf der Stegkante, Beine im Kanal;
  **F400 haengt er unveraendert weiter an der Kante** (Abnahme 0: bis auf ein Bein verschwunden,
  `m_domino/crop_domino2.png`); F401-F405 taumelt er im RE2-Taumel-Clip 13 rueckwaerts durch den Kanal (eigener
  Abbruch-Tick: EXEC[9] P0 + Schub 400, siehe Hinweise).
- **Mangel 1 ist behoben.**

### M2 — ECHTER WEG 2070 -> 2000, Leon, `RE15_AI_FLAVOR=re15` (RE1.5-KI), Bild

`tools/lauf.sh <exe> m2_leon_re15 "620#2000" leon 300 RE15_AI_FLAVOR=re15 "RE15_FRAMEDUMP=140-620/10:fd/f"` ->
`exit=0 dauer=38s`; `[pl] Spieler-Familie PL00`, `[aot] DOOR FIRE slot=0 ...`, `[room] PC loaded room2000.rdt`.
(`RE15_AI_FLAVOR=re15` -> RE15_AI_FLAVOR_RE15, enemy_ai_re2_zombie.c:112-123; kein `re2-eintritt` im wz.log = RE1.5-Zweig.)
wz.log (vollstaendig, woertlich):
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
gy.log (`tools/inv.py`): 1366 Gegner-Eintraege, **0 unter dem Wasserboden (y > 0), 0 auf Band >= 2** (Vorbefund der
RE1.5-KI laut Dossier 1: Sturz bis y = +1800, danach Band 2). Bild (`m2_re15_crop.png`, F150-F310 und F430-F590 je 20,
Ausschnitt x30-200/y40-200, 2x): F150-F250 Haende/Oberkoerper auf der Kante, Beine im Kanal; F270-F290 Knien und
Aufrichten; F310 steht er auf dem Steg und greift Leon; F430-F510 der zweite mit dem Oberkoerper auf der Kante.
Leon wehrt sich im Wegplan nicht und stirbt (F530 Weissblitz, F550-F590 Todesszene/"YOU DIED") — erwartbar, nicht Spur U.

### M3 — ECHTER WEG 2071 -> 2001, Elza, Standard-KI (RE2), Bild inkl. Zwischenbild

`tools/lauf.sh <exe> m3_elza_re2 "410#2001" elza 300 "RE15_FRAMEDUMP=130-410/1:fd/f"` -> `exit=0 dauer=47s`.
debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[debug-menu] AUTO-JUMP -> ROOM2070 (Frame 250)`,
`[room] PC loaded room2071.rdt`, `[aot] DOOR FIRE slot=0 ...`, `[room] PC loaded room2001.rdt (360660 bytes)`.
wz.log (vollstaendig, woertlich):
```
F144 R2001 slot=2 t=16 re2-eintritt st=1/10/0 mo=27 af=18 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F144 R2001 slot=2 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F150 R2001 slot=2 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=0 kontakt=00
F220 R2001 slot=2 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10290) b0 f1ba=-1800 rot=0 kontakt=00
F308 R2001 slot=2 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F308 R2001 slot=2 t=16 re2-uebergabe st=1/0/1 mo=0 af=3 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F340 R2001 slot=1 t=16 re2-eintritt st=1/10/0 mo=0 af=63 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F340 R2001 slot=1 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F351 R2001 slot=1 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F400 R2001 slot=1 t=16 zwischenbild st=1/9/0 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F401 R2001 slot=1 t=16 abbruch st=1/9/0 mo=22 af=50 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
```
gy.log: 946 Eintraege, 0 unter dem Wasserboden, 0 auf Band >= 2. Bild (`m3_elza_crop.png`, F150-F310 je 20 und
F397-F402 einzeln, Ausschnitt x30-200/y50-220, 2x): F150-F290 Hochziehen neben Elza (Haende, Oberkoerper, Knie,
Aufrichten), F310 steht er auf dem Steg und greift Elza; F397-F400 haengt der zweite Kletterer an der Kante — **auch
F400** —, F401/F402 Taumel im Kanal. (Die 2001-Wasserzombies spawnen nur mit Flag (3,0x45): ROOM2001 sub00
`06 00 2e 00` @0x1826 / `21 03 45 01` @0x182A, Abnahme 0 M3 — deshalb `RE15_SET_FLAG=3:69`.)

### M8 — ECHTER WEG 2071 -> 2001, Elza, `RE15_AI_FLAVOR=re15`, Bild

`tools/lauf.sh <exe> m8_elza_re15 "620#2001" elza 300 RE15_AI_FLAVOR=re15 "RE15_FRAMEDUMP=150-610/20:fd/f"` ->
`exit=0 dauer=53s`; `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `PC loaded room2071.rdt`, `DOOR FIRE slot=0 ...`,
`PC loaded room2001.rdt`. wz.log (vollstaendig, woertlich):
```
F146 R2001 slot=2 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=4002 kontakt=09
F152 R2001 slot=2 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=0 kontakt=00
F222 R2001 slot=2 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-9931) b0 f1ba=-1800 rot=0 kontakt=00
F310 R2001 slot=2 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-9931) b1 f1ba=-1800 rot=0 kontakt=00
F424 R2001 slot=1 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-10516) b0 f1ba=0 rot=93 kontakt=09
F430 R2001 slot=1 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10516) b0 f1ba=0 rot=0 kontakt=00
F500 R2001 slot=1 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10516) b0 f1ba=-1800 rot=0 kontakt=00
F588 R2001 slot=1 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10516) b1 f1ba=-1800 rot=0 kontakt=00
```
gy.log: 1366 Eintraege, 0 unter dem Wasserboden, 0 auf Band >= 2. Bild (`m8_elza_re15_crop.png`, F150-F310 und
F430-F590 je 20): Haende auf der Kante, Oberkoerper drueber, Knien, Aufrichten neben Elza (F310 steht er auf dem Steg);
F430-F510 der zweite an der Kante; ab F530 stirbt Elza (wehrt sich nicht). Damit sind alle vier Kombinationen
(Leon/Elza x RE2-/RE1.5-KI) auf dem echten Tuerweg gemessen (M1, M2, M3, M8) — in allen vier ziehen sich beide
Wasserzombies hoch (bzw. der zweite in M1/M3 setzt nach dem RE2-Domino-Abbruch spaeter erneut an, siehe M7).

### M1-vorher — derselbe Tuerweg mit der v0.8.25-exe (erklaert der Fix den Nutzerbefund?)

`release/pkg-win/re15_port_v0.8.25/re15_pc.exe` (Hauptbaum, nur gelesen) als Kopie `U38abn1c/v0825/re15_pc_v0825_u1c.exe`
(md5 `7aa892fce28a1d12b6f589cfc9fa9893` beide), Assets des Pakets (`RE15_CD_ROOT=<paket>/shared_assets/PSX
RE15_RE2_ASSET_ROOT=<paket>/shared_assets/RE2`, debug.log `[asset] cd-root: .../re15_port_v0.8.25/shared_assets/PSX`),
identischer Wegplan, `RE15_EXIT_AT=700#2000 RE15_FRAMEDUMP=150-700/50:fd/f` -> `exit=0 dauer=41s`. Tuer feuert identisch
(`[aot] DOOR FIRE slot=0 rect=(-19450,-6400,hw=500,hh=1500) target_cut=13 spawn=(-20650,-1800,-6200)`,
`[room] PC loaded room2000.rdt (359612 bytes)`). gy.log ausgezaehlt (`tools/band.py`): **1400 Typ-0x16-Eintraege,
0 auf Band 1, 673 an der Kanalwand x = -22068**, Zustaende nur 1/1 (759), 1/14 (342), 1/8 (253), 1/12, 1/0, 0/0 —
z.B. `F600 ... [3 t=16 st=1/14/2/0 ... @(-22067,0,-10184) b0 ...] [4 t=16 st=1/14/2/0 ... @(-22068,0,-10986) b0 ...]`.
Bild (`m1v_v0825_sheet.png`, F150-F650 je 100): beide Zombies stehen die ganze Zeit unten im Kanal an der Wand unter
Leon. **Das ist der Nutzerbefund** (laufen zum Rand, kommen nie hoch). Mit dem Spurstand auf demselben Weg (M7, bis
F1100): 441 Eintraege auf Band 1, erster `F308 slot 4`; Zustand 1/10 (Hochziehen) 567 Eintraege. Der Fix erklaert den
Befund: das RE2-Gehirn hat kein Hochziehen (RE2-Overlays ohne +-1800, Abnahme 0 Nr. 8), der neue Dispatch fuehrt bei
u1=1-Kante vorn die RE1.5-Maschine ANIMATE[0x0A] ein (Abschnitt 4).

### M7 — Gegenprobe "aendert die Nachbesserung sonst etwas?" bis F1100 (Leon, Standard-KI, Tuerweg)

`tools/lauf.sh <exe> m7_f1100 "1100#2000" leon 400` -> `exit=0 dauer=53s`. Referenz: Abnahme-0-Lauf desselben Wegs auf
dem alten Stand (`scratchpad/m_tuer2000/`, gy.log 2026-10-17 05:41 = vor dem Bau der Nachbesserungs-exe 06:53,
`RE15_EXIT_AT=1100#2000`). gy.log (1414 Zeilen, Zustand/Clip/Bild/Lage/Band/+0x1ba aller Gegner je Bild): **byte-gleich**
(`cmp` ohne Ausgabe, md5 beide `4188d6f12f90fd029ad5f82863ed7cd6`). `diff` der wz.log: einziger Unterschied
```
< F401 R2000 slot=3 t=16 abbruch st=1/9/0 mo=22 af=9 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
---
> F400 R2000 slot=3 t=16 zwischenbild st=1/9/0 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
> F401 R2000 slot=3 t=16 abbruch st=1/9/0 mo=22 af=50 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
```
Beide Zombies kommen danach hoch (woertlich): `F724 ... slot=4 ... re2-eintritt`, `F744 ... klettern`,
`F902 R2000 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-20926,-1800,-9605) b1 f1ba=-1800`, `F791 ... slot=3 ...
re2-eintritt`, `F956 R2000 slot=3 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10407) b1 f1ba=-1800`.
gy.log: 2200 Typ-0x16-Eintraege, 0 unter dem Wasserboden, 0 auf Band >= 2. Die Behauptung des Bau-Agenten (gy.log bis
F1000 byte-gleich, wz.log nur Abbruch-Zeile) haelt, hier sogar bis F1100.

### M4 — ROOM2020 (Leon) ueber den Raumlader (Debug-Sprung), Standard-KI, Bild; Tuerweg 2050 -> 2020 versucht

Tuerweg-Versuch (echter Weg in den zweiten Wasserraum): die Engine stellt in ROOM2050 die Tuer nach 2020 als Slot 1 auf
(`tools/engine_tueren.txt` Z. 127 `2050 2020 -5150 4450 11350 -3150 0`; Wegplan `AOTS` woertlich: `AOT slot 1 typ 1 ev 0
flags 0x31 band 0 mitte (-5150,4450) halb (1000,500)`). Lauf `m4t_2050` (`RE15_DEBUG_JUMP=2050@250
RE15_PLAYER_POS=-5150,4450,3072,0`, Wegplan `GEHE -5150 4450 120` + `BLICK 3072/1024/0/2048` je mit `AKTION`): der
Spieler steht bei (-5124,4718) (also im Rechteck), die Tuer feuert in keiner Blickrichtung (kein `DOOR FIRE`, Lauf endet
per timeout 200 s). Zweiter Versuch `m4t_2050b` (Start an der 2040-Ankunft, zu Fuss): der Spieler klemmt bei
(-8018,-5650). Wie in Abnahme 0 (dort ebenso nicht ausloesbar) ist das der Lade-/Tuerweg von ROOM2050, nicht Spur U;
ROOM2020/2021 decken deshalb der LADE-Weg CONTINUE (Riegel `integration_r38_wasserzombie` a/b Leon, c Elza 2021,
Abschnitt 5) und der Raumlader ab. Die 2020-Spawns stehen in main00 ohne If/Ck-Klammer (Abnahme 0 M3) und erscheinen
damit auf jedem Weg in den Raum.

Raumlader: `RE15_DEBUG_JUMP=2020@250 RE15_PLAYER_POS=-9500,2600,1024,1 RE15_FORCE_CUT=6 RE15_FRAMEDUMP=60-300/10:fd/f
RE15_EXIT_AT=300#2020` -> `exit=0 dauer=19s` (`[parity] JUMP-Spawn ersetzt durch RE15_PLAYER_POS (-9500,2600) rot=1024
band=1`, `[room] PC loaded room2020.rdt (322276 bytes)`). wz.log (vollstaendig, woertlich):
```
F80 R2020 slot=4 t=16 re2-eintritt st=1/10/0 mo=0 af=31 @(-10756,0,1082) b0 f1ba=0 rot=3527 kontakt=c9
F80 R2020 slot=4 t=16 drehen st=1/10/1 mo=1 af=0 @(-10756,0,1082) b0 f1ba=0 rot=3527 kontakt=c9
F109 R2020 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-10756,0,1082) b0 f1ba=0 rot=3072 kontakt=c0
F179 R2020 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-10756,0,1082) b0 f1ba=-1800 rot=3072 kontakt=c0
F267 R2020 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-10756,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
F267 R2020 slot=4 t=16 re2-uebergabe st=1/0/1 mo=0 af=1 @(-10756,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
```
Kontakt 0xc9 = Richtung 0xC / u1 1 (Suedwand des Kanals); Drehen 3527 -> 3072, Vorschub +1140 in z (1082 -> 2222).
gy.log: 1200 Eintraege, 0 unter dem Wasserboden, 0 auf Band >= 2. Bild (`m4_2020_sheet.png`, Kanalblick per
FORCE_CUT 6 nur zur Sicht, F80-F290 je 30): F80/F110 Arme auf der Stegkante, F140/F170 Oberkoerper ueber der Kante,
F200/F230 die Beine kommen hoch, F260/F290 steht er auf dem Steg (rechts im Bild).

### M6 — Ton des Kletter-Clips in der Standard-KI (RE2-Zweig)

Lauf wie M1, aber MIT Ton: `tools/lauf_ton.sh <exe> m6_ton` (`RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=cap.raw
RE15_FRAMEDUMP=270-290/1:fd/f RE15_EXIT_AT=300#2000`, kein RE15_NOAUDIO) -> `exit=0 dauer=26s`, cap.raw 4 627 560 Byte.
wz.log: `F150 ... klettern`, `F220 ... boden-1800` (Klettern ab F150 wie M1).
Frame-Woerter von Clip 0x16 (eigenes Mini-Programm `se_bits/se_bits_c.exe`, gegen `build/engine/libre15_engine.a`
gelinkt, CDEMD0.EMS Typ 0x16): `Typ 0x16: 43 Clips, Clip 0x16 = 159 Bilder`, `Clip 0x16 Bild 102: Frame-Wort 0x0080023b
-> SE-Bits 0x2`, `Clip 0x16 Bild 130: Frame-Wort 0x00800249 -> SE-Bits 0x2` (= SE 1; Dekodierung FUN_8001b38c selbst
disassembliert: `lw v0,360(v0)` +0x168 @0x8001b3a4, `srl s0,v0,22` @0x8001b3b4, je Bit `jal 0x800453d0` mit a0 = Bitnummer
@0x8001b3cc-d0; die Zombie-Wurzel ruft sie `jal 0x8001b38c` @0x801004c8). debug.log: `[se] Stimme: se=1 layer=0 vag=2
note=61 ... -> SE-Stimme 4 (SPU 20)` genau einmal zwischen `[walk] F240` und `[walk] F270` (= F252, Clip-Bild 102) und
einmal zwischen `[framedump] ... f000279.ppm` und `f000280.ppm` (= F280, Clip-Bild 130). Die Klettertoene des
RE1.5-Clips laufen also auch im RE2-Zweig (`wz_frame_se`), je SE-Bild einmal.

### M4b — ROOM2020 laenger (alle Wasser-Zombies des Raums), Standard-KI

Wie M4 ohne Bild, `RE15_EXIT_AT=1000#2020`: der stillstehende Leon stirbt bei ~F903 (danach Titel-/Neustart-Ablauf in
R1240/R1170, Lauf per timeout 300 s beendet — die Zaehlung unten ist auf die R2020-Bilder beschraenkt, `gy_2020.log`).
wz.log (Auszug, woertlich):
```
F80 R2020 slot=4 t=16 re2-eintritt st=1/10/0 mo=0 af=31 @(-10756,0,1082) b0 f1ba=0 rot=3527 kontakt=c9
F267 R2020 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-10756,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
F347 R2020 slot=3 t=16 re2-eintritt st=1/10/0 mo=0 af=53 @(-8650,0,1082) b0 f1ba=0 rot=3007 kontakt=c9
F415 R2020 slot=2 t=16 re2-eintritt st=1/10/0 mo=2 af=5 @(-9553,0,1082) b0 f1ba=0 rot=2947 kontakt=c9
F510 R2020 slot=3 t=16 oben st=1/0/0 mo=22 af=158 @(-8650,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
F581 R2020 slot=2 t=16 oben st=1/0/0 mo=22 af=158 @(-9553,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
```
Beide Band-0-Wasserzombies (Aktor 4 und 3) ziehen sich an der Suedwand hoch; Aktor 2 (Spawn auf dem Nordsteg
(-11250,-1800,-3100) Band 1, gy.log F1) faellt ueber die Absturzkante in den Kanal (F300 `[2 ... @(-9433,0,-992) b0]`)
und zieht sich danach ebenfalls hoch. R2020: 3612 Gegner-Eintraege, **0 unter dem Wasserboden, 0 auf Band >= 2**.

### Zensus — welche Stage-2-Zombies stehen im Wasser? (eigener Lauf)

Skript `U38abn1c/re/zensus_em.py` (Sce_em_set 0x44 ueber den SCD-Walker von `re15_port/tools/aot_sce_census.py`, alle
`shared_assets/PSX/STAGE2/ROOM*.RDT`): `Sce_em_set gesamt STAGE2: 150 | Zombie-Typen 0x10-0x1F: 12` — alle Typ 0x16, nur
in ROOM2000/2001 (sub00 @0x017cc/@0x017e0 bzw. @0x0182e/@0x01842, Band pc4 = 0) und ROOM2020/2021 (main00 @0x008f6/
@0x0090a Band 1, @0x0091e/@0x00932 Band 0). Damit sind alle Wasserzombies der Stage 2 von M1-M4b abgedeckt (2000/2001
echter Tuerweg Leon/Elza, 2020 Raumlader, 2020/2021 CONTINUE im Riegel a/b/c).

## 3. Mangel 2 der Abnahme 0 (Code-Hygiene) geprueft

- Suche nach `re15_enemy_ai_live_edge_fall`, `EDGE-FALL`, `edge_fall` in re15_port/engine/src, include, tests, platform:
  einziger Treffer `include/re15_wasserzombie_r38.h:30` (Abschnitt "WAS DER PORT VORHER TAT" — Beschreibung, kein Code).
  Die tote Funktion ist entfernt (Diff `enemy_ai_common.c` -43 Zeilen @@ -3368).
- Kommentar am Aufruf (`enemy_ai_common.c` in `re15_enemy_ai_live_active`): "Runde 38 Spur U: HOCHZIEHEN an der u1=1-Kante
  (@0x8011f890[0xa]=FUN_801033c8 / STAGE2 0x8010325c): +0x38 -= 1800 und +0x82 += 1 = ein Band HOCH, +0x1ba
  @0x801033f8, Vorschub @0x80103474" — stimmt mit meiner Disassembly (Abschnitt 4 Nr. 2).
- `gcc -std=c11 -Wall -Wextra -fsyntax-only -DRE15_PLATFORM_PC=1 -Iinclude` (msys64-gcc) auf wasserzombie_r38.c,
  enemy_ai_re2_zombie.c, schuss_r36.c, enemy_ai_common.c, tests/unit/test_r38_wasserzombie.c,
  tests/unit/probe_r38_wasserzombie_karte.c: **keine Meldung aus `re15_wasserzombie_r38.h` und keine aus einer
  Spur-U-Zeile**. Uebrig: `-Wcomment` aus `include/re15_emd.h:37` / `include/re15_collision.h:85/90` (fremde Koepfe) und in
  enemy_ai_common.c Z. 1518 (`-Wunused-variable` px0/pz0), 6113 (`-Wmisleading-indentation`), 11346 (`-Wsign-compare`) —
  `git blame`: 4b64f5c56 (2026-08-25), 773b20af7 (2026-07-06), 95d99f781 (2026-07-26); derselbe Lauf auf
  `cb782bda:enemy_ai_common.c` meldet dieselben drei (Z. 1517/6152/11385) -> vorbestehend.
- **Mangel 2 ist behoben.**

## 4. RE-Gate — selbst disassembliert

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` (Hauptbaum, nur lesend).
Binaerdateien: `info/Re1.5/PSX.EXE`, `info/Re1.5/PSX/BIN/STAGE2.BIN` (roh @0x80100000), `info/re2leon/PSX.EXE`,
`info/re2leon/COMMON/BIN/EMOVL10_S0.BIN`.

Kern-Mechanik (Ausgangsstand der Spur):
1. **Ausloeser** `dis 0x80101eec 24 --bin STAGE2.BIN`: `80101f00 lbu a0,144(a1)` (+0x90) / `80101f08 andi v0,a0,0x3` /
   `80101f0c beq v0,zero,0x80101f48` / `80101f10 andi v0,a0,0xf0` / `80101f14 lh v1,106(a1)` / `80101f18 sll v0,v0,4` /
   `80101f1c subu` / `80101f20 addiu v0,v0,512` / `80101f24 andi 0xfff` / `80101f28 slti v0,v0,1024` /
   `80101f30 andi v0,a0,0x1` / `80101f34 addiu v0,v0,9` / `80101f38 sll v0,v0,8` / `80101f3c ori v0,v0,0x1` /
   `80101f44 sw v0,4(a1)`. Erreichbar: `table 0x80117944` [2] und `table 0x80117a64` [2] = 0x80101eec. =
   `re15_wasserzombie_kante_vorn` (u1&3, Kegel +0x200/<0x400, Bit 0 -> 0xA01).
2. **Hochziehen** `table 0x80117994 12 --bin STAGE2.BIN`: [10] = **0x8010325c**. `dis 0x8010325c 136`: Phase 0
   `801032b0 sb v1,6(a0)` / `801032c0 sb v1,148(v0)` (v1 = 1 aus dem Delay-Slot `80103288 ori v1,zero,0x1`) /
   `801032d0 sb zero,149` / `801032dc ori v0,zero,0x7` / `801032e0 sb v0,143`; Phase 1 `801032ec addu a2,zero,zero` /
   `801032f8 jal 0x8001f314` / `801032fc ori a3,zero,0x200`, `80103310 ori a1,zero,0x10` / `80103318 jal 0x8001aa68` /
   `8010331c sll a0,a0,4`, `80103334 addu v1,v0,v1` / `80103340 sh v1,106(a0)` (Delay-Slot, immer), d == 0 ->
   `80103354 sb v0(=2),6`; Phase 2 `80103368 ori v0,zero,0x16` / `8010336c sb v0,148`, `80103388-8c` +0x8f = 7,
   `80103398 ori v0,zero,0x474` / `8010339c sh v0,140`, `801033b4-bc` +0x6a = (+0x90&0xf0)<<4; Phase 3
   `801033cc lbu v1,149` / `801033d0 ori v0,zero,0x46` / `801033d4 bne`, `801033e0 sh 0x474,176`,
   `801033f0 lhu v0,442` / `801033f8 addiu v0,v0,-1800` / `801033fc sh v0,442`, `80103414 jal 0x8001f314` /
   `80103418 ori a3,zero,0x200`, `8010341c beq v0,zero` / Delay `80103420 addu a0,zero,zero`, `80103430-3c` +0x82 += 1,
   `8010344c-58` +0x38 -= 1800, `80103464 ori v0,zero,0x1` / `80103468 sw v0,4`, `80103474 jal 0x800245d8` / Delay
   `80103478 sh zero,176`. Zeile fuer Zeile = `re15_wasserzombie_hochziehen` und alle `RE15_WZ_*`-Konstanten im Kopf
   (SUB 0x0A, CLIP_DREH 1, CLIP_KLETTER 0x16, FRAC 7, RATE 0x200, DREHSCHRITT 0x10, TEMPO 0x474, BODENBILD 0x46, BAND 1800).
3. **Frame-Wort-SE** `dis 0x8001b38c 30` (PSX.EXE): `8001b3a4 lw v0,360(v0)` (+0x168), `8001b3b4 srl s0,v0,22`, je Bit
   `8001b3cc jal 0x800453d0` mit `8001b3d0 andi a0,s1,0xff`; Aufruf aus der Zombie-Wurzel `801004c8 jal 0x8001b38c`
   (STAGE2). = `wz_frame_se` (gemessen M6).
4. **Schatten** `dis 0x801004e0 12 --bin STAGE2.BIN`: `801004fc lh a1,442(v0)` / `80100500 jal 0x8001b064` /
   `80100504 addiu a0,a0,176`. = `re15_wasserzombie_schatten_y` (+0x1ba).

Neue Belege der Nachbesserung 1 (die Stellen, auf die der Bau-Agent den Fix stuetzt):
5. **anim_set FUN_8001f314** `dis 0x8001f314 42`: `8001f324 lbu v0,148(t0)` (+0x94), `8001f330 addu a2,a1,v0` /
   `8001f334 lw v0,0(a2)` (Clipzeile aus der UEBERGEBENEN Bank a1), `8001f35c lbu v0,149(t0)` (+0x95),
   `8001f36c sw a2,360(t0)` (+0x168), `8001f38c jal 0x8001f3bc` (sonst `8001f3a4 jal 0x8001f8b4`). **Stimmt.**
6. **FUN_8001f3bc** `dis 0x8001f3bc 30` + `dis 0x8001f560 12` + `dis 0x8001f5e0 20`: `8001f40c lw s1,392(v1)`
   (v1 = g_entity(cur) -> +0x188 Teileblock), `8001f42c sw v0,44(s1)` / `8001f430 sh v0,56(s1)`, je Teil
   `8001f570 addiu a0,s1,120` / `8001f574 jal 0x80068098` und `8001f5f0 jal 0x80068098` (a1 = s1+24 im Delay-Slot
   `8001f5f4`), danach `8001f610 lbu v0,149(v1)` / `8001f618 addiu v0,v0,1` / `8001f61c sb v0,149(v1)`. 0x80068098 beginnt
   mit `lh t7,0(a0)` / `bgez` / `andi t9,t7,0xfff` (RotMatrix-Form). **Stimmt**: die Pose wird im eigenen anim_set-Aufruf
   in den Teileblock gerechnet.
7. **Eigener Lade-Scan** (Skript `U38abn1c/re/scan9495.py`, alle lb/lbu/lh/lhu/lw mit Offset 148/149, Basis != sp, Text
   von PSX.EXE): **31 Treffer** (= Zahl des Bau-Agenten). Byte-Leser (Clip/Bild des Aktors) nur in FUN_8001ae38
   (0x8001ae48/5c), anim_set/FUN_8001f3bc/FUN_8001f8b4 (0x8001f324-0x8001fb4c) und 0x800526d4-0x80052cbc. Die Halbwort-/
   Wort-Leser haben andere Basen (selbst nachgesehen: `8001b5fc lhu a0,148(s0)` neben `lhu a1,98(s0)`; `80024ed8 lhu
   v1,148(fp)` / `80025190 lh v1,148(fp)` in der Nackenfeder FUN_80024e40 (Katalog: Teil +0x94 = Nickwinkel);
   `800378c8 lhu v1,148(s0)` direkt nach `jal 0x80068098`; `80060a7c lw v0,148(s0)` / `lw v1,152(s0)` = Wortpaar).
   Kein Zeichner wertet +0x94/+0x95 des Aktors neu aus. **Stimmt.**
8. **RE2-Partner-Domino** `dis 0x80102E70 20 --bin EMOVL10_S0.BIN`: `80102e90 addiu v0,zero,2305` (0x901),
   `80102e94 jal 0x80015910`, `80102e98 sw v0,4(s0)` (Delay-Slot), `80102ea4 jal 0x8005bd6c` (a0 = 4 aus
   `80102e9c`), `80102ea8 sb v0,363(s0)` (+0x16B). RE2 PSX.EXE `dis 0x80015910 10`: `lh v0,118(a1)` / `lh v1,118(a0)` /
   `subu` / `addiu 1024` / `andi 0xfff` / `jr ra` / `slti v0,v0,2048` — reine Seitenpruefung ohne Store. Der Domino
   schreibt weder Clip noch Bild noch Teileblock des Partners. **Stimmt.**
9. **EXEC[9] P0** `dis 0x80103EF0 36 --bin EMOVL10_S0.BIN`: `80103f30 sw v0,332(s1)` (Clipwort +0x14C, Delay-Slot),
   Index `80103f14-20` = 2*(+0x16B) + (+0x16A) in die Stapeltabelle s0 = sp+16 (`80103ebc addiu s0,sp,16`), gefuellt per
   lwl/lwr/swl/swr ab `0x80100000+216` (`80103e70 addiu a1,a1,216`); `bytes 0x801000D8 8` = `03 03 04 0d ...` =
   {3,3,4,0x0D}; `80103f60 addiu v0,zero,400` / `80103f64 sh v0,324(s1)` (Schub +0x144). **Stimmt** (Riegel F prueft
   den Clip in {3,4,0x0D}).
10. **Sce_em_set** `dis 0x800421c0 22` (PSX.EXE): `800421e0 sw zero,4(s0)` (Zustandswort 0), `800421f8-8004220c`
    v0 = -(pc[2]*1800) -> `80042210 sh v0,442(s0)`. **Stimmt** (Grund fuer `e->state != 0` in `wz_marke_bank`).

Diff-Suche (`git diff cb782bda HEAD -- re15_port`, Plus-Zeilen) nach den Rate-Woertern deferred, tunable, interim,
for now, faithful, plausib, TODO, FIXME, approx, geschaetzt, vermut, ungefaehr, heuristi: **0 Treffer**. Einziger neuer
`getenv`: `RE15_WASSERZOMBIE_LOG` (Messschiene, oeffnet nur eine Logdatei, kein Verhalten, kein Abschluss-Schalter).
Die Nachbesserung bringt **keine neue Konstante** (`wz_marke_bank`: `state != 0` = Sce_em_set @0x800421e0, Nr. 10;
`zwischen` = nur Messschiene). Gekennzeichnet: PORT-WAHL (Rig-Wechsel ohne Ueberblendung; +0x1ba-Ruecksetzen beim
RE2-Abbruch), PORT-EMULATION (LOOP-Bit 0x04 in Phase 0/1, Rueckwaerts-Bit 0x80 aus, a2 = 0 @0x801032ec), PORT-ABBILDUNG
(Eintritt aus dem RE2-Gang 1/1, weil das RE2-Gehirn kein Hochziehen hat — Nutzer-Wortlaut "wie im Original Resident
Evil 1.5").

**Erklaert der Fix den Befund?** Ja: Mangel 1 war, dass Bank/Uhr/Schatten am fremd geschriebenen Zustandswort hingen
(`wz_marke_gilt` = Zustand 1 UND Sub 0x0A); jetzt haengen sie an der Marke, die nur der eigene Tick setzt/loescht
(`wz_marke_bank`). Gemessen: F400 Kletterpose statt "bis auf ein Bein leer", Abbruch af 49 -> 50 statt 49 -> 9 (M5),
Ausfuehrung sonst byte-gleich (M5/M7). Die RE-Begruendung (die Pose wird im eigenen Tick gerechnet, der fremde Schreiber
aendert sie nicht) ist durch Nr. 5-9 belegt. **RE-Gate haelt.**
