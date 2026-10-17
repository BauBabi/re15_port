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
