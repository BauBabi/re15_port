# Runde 36 — Spur B "kleidung": UNABHAENGIGE ABNAHME 1 (2026-10-10)

Baum `.claude/worktrees/r36_kleidung`, Zweig `r36/kleidung`, HEAD c8033896 (Basis master 78a4a748).
Pruefer: Abnahme 1 (nach Nachbesserung 1; glaubt nichts, was nicht selbst am gebauten Stand gemessen wurde).
Bericht fortlaufend geschrieben (Sitzungslimit). STAND: abgeschlossen.

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Z.32)

> Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.

## 1. Diff gelesen, Bau

* `git log master..HEAD`: 22 Eintraege (d0b7d321 .. c8033896, darin 2 Merges von master und der
  Abnahme-0-Commit a096a1a5); Nachbesserung 1 = 42d144d0, 9bcda7fd, 8e2fd61d, ddba0b6a, 5dde2bc3,
  4d502344, c8033896.
* `git diff master --name-only`: 20 Dateien — analysis/befunde_runde36/{B_abnahme_0.md, B_kleidung.md,
  2 png}; engine/src/{enemy_ai_common.c, enemy_ai_re2_crow.c, enemy_ai_re2_dog.c,
  enemy_ai_re2_zombie.c, kleidung_r36.c (neu), re15_damage.c}; include/{re15_damage.h,
  re15_enemy_ai.h, re15_kleidung_r36.h (neu)}; platform/pc/main.c;
  tests/integration/test_r36_kleidung_{bild,hundeweg,spielweg}.cmake; tests/unit/probes/r36_kleidung.cmake;
  tests/unit/test_r36_kleidung.c + _teile.inc.
* Code-Diff vollstaendig gelesen (engine/include/platform/tests). Nachbesserung 1 im Code: nur
  `re15_kleidung_hundebiss(hund, spieler, schaden)` (n = schaden/10 Stempel, je eigener
  `re15_engine_rand8()&1`, je `re15_wound_add(panel,0x32)`) und `enum { RE2D_BISS_SCHADEN = 20 }` in
  enemy_ai_re2_dog.c re2d_contact (speist Schaden UND Stempelzahl). Die uebrigen Code-Teile sind seit
  Abnahme 0 (HEAD 866af8e3) unveraendert (`git diff 866af8e3 HEAD -- re15_port/engine re15_port/include
  re15_port/platform` = nur kleidung_r36.c / re15_kleidung_r36.h / enemy_ai_re2_dog.c).
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-10 20:10:11 (juenger als jede Quelle des
  Diffs, letzte 20:09:24; Arbeitsbaum ohne Aenderungen) -> gebauter Stand = HEAD c8033896.

## 2. RE-Gate (selbst disassembliert, `re15_disasm.py` / `re2_disasm.py` im Baum)

Neue Konstanten der Nachbesserung 1, jede an der richtigen Binaerdatei gelesen:

| # | Zitat im Diff | Binaerdatei | Gelesen | Urteil |
|---|---|---|---|---|
| 1 | RE1.5-Biss 10 HP `addiu a0,v0,-10` @0x8010f2c8 | STAGE1.BIN | `8010f2bc lhu v0,-13586(v0)` (player.hp) / `8010f2c8 addiu a0,v0,-10` / `8010f2d0 sh a0,-13586(at)`; Gates `8010f2dc bne (+0x1e4) -> 8010f468`, `8010f2e4 bltz -> 8010f468` | bestaetigt |
| 2 | EIN Stempel je Biss `jal 0x80037edc`/`ori a1,zero,0x32` @0x8010f360/64, rng @0x8010f334, Bytes @0x80121014 | STAGE1.BIN | `8010f310 jal 0x8001a780`, `8010f31c addiu v0,v0,2`, `8010f324 sb v0,-13735(at)` (aca59), `8010f334 jal 0x8001af20`, `8010f34c andi v0,v0,0x1`, `8010f33c/40 a0 = 0x80121010`, `8010f35c lbu a0,0(v1)`, `8010f360 jal 0x80037edc` / `8010f364 ori a1,zero,0x32`, `8010f368 j 0x8010f7f0` (genau ein Aufruf, keine Schleife); `bytes 0x80121010` = `e8 08 11 80 05 07 04 06` | bestaetigt |
| 3 | Lande-Biss @0x8010f444 ohne Stempel | STAGE1.BIN | `8010f438 lhu player.hp` / `8010f444 addiu a0,v0,-10` / `8010f44c sh`; `8010f460 bgez -> 8010f4b8`; Byte-Scan `jal 0x80037edc` (0x0C00DFB7) ueber STAGE1.BIN: 22 Stellen, zwischen 0x8010f368 und 0x80111b64 KEINE | bestaetigt |
| 4 | Blickrichtung a780 | PSX.EXE | `8001a788 lh v0,106(a0)` / `8001a78c lh v1,106(cur)` / `subu` / `addiu 1024` / `andi 0xfff` / `slti v0,v0,2048` — Port `((spieler.rot_y - hund.rot_y + 0x400) & 0xfff) < 0x800` (a0 = s0-147 = Spieler, cur = Hund) | bestaetigt |
| 5 | RE2-Biss 20 HP `addiu a0,zero,20` @0x80104EB8 | EMD0G_MOD0.BIN (RE2) | `80104eb0 bne s2,zero,80104ecc` / `80104eb8 addiu a0,zero,20` / `80104ebc jal 0x800401d4` / `80104ec0 addu a1,zero,zero`; Boden-Biss `80104f7c sh v0,6(s0)`, `80104f90 bltz`, `80104fa4 jal 0x801051bc` | bestaetigt |
| 6 | OFFEN-Notiz Claim @0x801051f8/0x80105200, Pruefung @0x80104e5c-68 | EMD0G_MOD0.BIN | `801051f8 ori v1,v1,0x80` / `801051fc jr ra` / `80105200 sb v1,467(a0)`; `80104e5c lbu v0,467(s1)` / `80104e64 andi 0x80` / `80104e68 bne -> 80104fcc` | bestaetigt (Nebenbefund korrekt belegt) |

Weitere Pruefung: PSX.EXE enthaelt KEINEN `jal 0x80037edc` und kein Datenwort 0x80037edc (Byte-Scan) —
der Spieler-HIT-Zweig der EXE (aca58 := 2) stempelt also nicht zusaetzlich; das RE1.5-Verhaeltnis ist
damit vollstaendig: 10 HP <-> 1 x 0x32 je Knockdown-Biss.

Massstab-Ableitung `n = schaden/10`: keine freie Zahl, sondern Quotient zweier belegter Original-
Konstanten (10 @0x8010f2c8, 20 @0x80104EB8); im Header als PORT-MAPPING gekennzeichnet (RE2 kennt kein
Wundsystem). Die Teile aus Abnahme 0 (Phase 4 @0x8010a4e8, Helper 0x801201b8, FUN_80037edc, Kraehe
0x801161e8, LUT @0x80074208, Hook-A-Typen) sind unveraendert und dort bereits gegengelesen.

Rate-Woerter im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vermutl|
geschaetzt|ungefaehr`): 0 Treffer. getenv im Diff: `RE15_KLEIDUNG_LOG` (Messhaken, nur stderr),
`RE15_ASSET_DIR` / `R36B_TRACE` (nur im Test) — kein Env-Schalter als Abschluss.


## 3. Spur-Tests + Testgruppen der beruehrten Module (selbst gefahren)

* `ctest --test-dir re15_port/build -R "unit_r36_kleidung|dog|hund|crow|kraehe|zombie|re2z|wound|damage|weste|victim|grab|re2_hit|savedata|savepoint|writher|1210|crawl|1140|1190|elza_zweig|r35_affen|r35_raeume|r26_mg_blut" -E "integration_" -j 4 --timeout 600 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 118` (Gruppen je Modul des Diffs: RE1.5-/RE2-Hund,
  RE2-Kraehe, RE1.5-/RE2-Zombie + Kriecher + 1140, Arme 1210/EM01A (writher), Schaden/Wunden/Weste/
  Spielstand, Affen, Elza-Zweig).
* `ctest --test-dir re15_port/build -R "integration_r36_kleidung" -j 1 --timeout 900 --output-on-failure -V`
  -> `100% tests passed, 0 tests failed out of 3` (148,9 s):
  * integration_r36_kleidung_bild 66,7 s — LA/LA2/LB/LC/EA/EB (Leon Raum 1130 char 0, Elza Raum 1131 char 4).
  * integration_r36_kleidung_spielweg 32,4 s — `[kleidung] F97 abwurf typ=0x11 dir=0 hp=70` /
    `F163 abwurf typ=0x10 dir=1 hp=50` / `F229 abwurf typ=0x11 dir=0 hp=20 gen=2 ... 1/0`;
    Spielstand `Raum 1140 ... hp 20 cut 1 wunden 0/30 0/0 0/0 0/0 0/50 0/100 0/0 1/0`;
    `bild: 131 Pixel verschieden, Kasten (176,103)-(190,125), ausserhalb [150,240)x[80,210): 0`.
  * integration_r36_kleidung_hundeweg 49,8 s — `F582 dir=2+2 hp=80`, `F585 dir=3+2 hp=60 gen=2 ... 1/0`,
    `F587 dir=0+1 hp=40`; `erstes Decal bei hp 60 (vor Danger)`; Spielstand `Raum 1190 ... hp 40 cut 0
    wunden 0/0 0/0 0/0 0/0 1/0 0/50 0/50 0/50`; `bild: 27 Pixel verschieden, Kasten (198,97)-(207,109)`.
* Direkt (Ausgabe gelesen): `test_r36_kleidung hund` -> Bisse F30/F78/F111/F116, je hp -20, je genau
  2 x 0x32 auf einem Paar (F30 Panel 4 0/0->0/100; F116 Panel 5 +50 und Panel 7 0/100 -> 1/0);
  `griff re2` -> Abwurf #3 F492 p4=L1 p7=L1 gen 3; `griff re15` -> Abwurf #3 F416 p4=L1 p7=L1
  (bildgleich zu Abnahme 0); `kraehe` -> Baender OK, Griffe F348/F1278 je +130.

## 4. Messung an der exe (eigene Messkopien, eigener Arbeitsordner im Scratchpad)

Messkopien neben der exe: `re15_pc_abn1_nach.exe` (md5 c869ebc6… = re15_pc.exe des gebauten Stands)
und `re15_pc_abn1_vor.exe` (md5 810990b9… = `re15_pc_r36b_nb1_vorher.exe` = Abnahme-0-Messkopie
`re15_pc_abn0b_hund.exe`, Stand 866af8e3). Maschine weiterhin in RDP -> `SDL_ASSERT=always_ignore`.

### 4.1 Hundeweg ROOM1190 (der dokumentierte Weg des Nutzers; Befehl der Abnahme 0)

```
SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_KLEIDUNG_LOG=1 RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450
RE15_DEBUG_DOGWAKE=500 RE15_STATE_LOG=state.log RE15_FRAMEDUMP=560-900/10:h_ RE15_EXIT_AT=900 <exe>  -> RC=0, 39 s
```
Nachher (`re15_pc_abn1_nach.exe`), debug.log:
```
[kleidung] F582 hundebiss typ=0x20 dir=2+2 hp=80 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/0,0/0,0/0
[kleidung] F585 hundebiss typ=0x20 dir=3+2 hp=60 gen=2 panels=0/0,0/0,0/0,0/0,1/0,0/0,0/50,0/0
[wound] sync: applied=1 ok=1 gen=2 slotgen=2
[kleidung] F587 hundebiss typ=0x20 dir=0+1 hp=40 gen=2 panels=0/0,0/0,0/0,0/0,1/0,0/50,0/50,0/50
[kleidung] F673 hundebiss typ=0x20 dir=0+1 hp=20 gen=2 panels=0/0,0/0,0/0,0/0,1/0,0/100,0/50,0/100
[kleidung] F708 hundebiss typ=0x20 dir=0+0 hp=0 gen=3 panels=0/0,0/0,0/0,0/0,1/0,1/50,0/50,0/100
[wound] sync: applied=1 ok=1 gen=3 slotgen=4
```
Vorher (`re15_pc_abn1_vor.exe`): dieselben Bild-Nummern F582/F585/F587/F673/F708 mit `dir=2/2/1/0/0`,
je ein Stempel, `gen=1` bis zum Tod, keine `[wound] sync`-Zeile — Mangel M1 der Abnahme 0 reproduziert.
Erstes Decal nachher: **F585, hp 60 (Caution)**, Panel 4 = 100 + 50 = 150 >= 0x78 -> Stufe 1.

Nebenwirkung Zufallsstrom: `state.log` beider Laeufe (1141 Zeilen, je Bild Spieler x/z/rot/hp/Zustand +
jeder Gegner mit Zustand/Unterzustand/Distanz/HP, FX-Zahl) ist **bytegleich** (`diff` leer). Der
zusaetzliche `re15_engine_rand8()`-Zug je Biss aendert auf diesem Weg kein Gegner-/Spielerverhalten.

Bild (Kamera "Firing Range", beschleunigter Renderer, Framedump-Serie, Pixel-Differenz nachher/vorher):
Bild 560..700 = **0 Pixel** (Panel 4 liegt auf der Westen-RUECKSEITE, Leon steht im Kampf seitlich zur
Kamera); erst ab Bild 710 (nach dem Todesbiss F708, Panel 5) 41 / 4 / 3 ... Pixel um (171,89)-(179,100).
Im laufenden Kampf dieser Kamera ist das Decal also nicht zu sehen — sichtbar wird es, sobald Leon der
Kamera den Ruecken zeigt (4.5).

### 4.2 Massstab-Gegenprobe: derselbe Hundeweg unter dem RE1.5-Gehirn

Befehl wie 4.1 + `RE15_AI_FLAVOR=re15`, `RE15_EXIT_AT=1300` (Messkopie nachher). HP je Bild aus
`state.log`: `F667 90 | F722 80 | F780 70 | F850 60 | F890 50 | F959 40 | F1008 30 | F1057 20 | F1229 -1`
(10-HP-Bisse wie im Original @0x8010f2c8); debug.log `[walk] F840 ...` / `[wound] sync: applied=1 ok=1
gen=2 slotgen=2` / `[walk] F870 ...` -> erstes Decal mit dem Biss F850 bei **hp 60** — dieselbe HP-Stufe
wie unter dem RE2-Gehirn nachher (F585, hp 60). Der Massstab "1 Stempel je 10 HP" bildet das
RE1.5-Verhaeltnis also auf diesem Weg genau ab.

### 4.3 Zombie-Weg (eigener Lauf, NICHT der Riegel-Weg: andere Position, anderes Hammern)

Spielstand per Kartenwerkzeug `test_r36_kleidung karte re15_card.mcr 1140 0 -1300 0 -20100 1024 100 - 0`
(Riegel: -1400/-19600 rot 0 cut 1), dann
```
SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_KLEIDUNG_LOG=1 RE15_STATE_LOG=state.log RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=30 RE15_INPUT_SCRIPT=B9 RE15_SAVE_TEST_AGAIN=420 RE15_SAVE_TEST_EXIT_AFTER=1
RE15_FRAMEDUMP=0-420/6:z_ "RE15_EXIT_AT=700#1140" re15_pc_abn1_nach.exe   -> RC=0
[save] CONTINUE: resumed in room 1140 (hp=100)
[kleidung] F97 abwurf typ=0x10 dir=1 hp=80 gen=1 panels=0/10,0/0,0/0,0/0,0/50,0/0,0/0,0/50
[kleidung] F163 abwurf typ=0x10 dir=0 hp=60 gen=1 panels=0/20,0/0,0/0,0/0,0/50,0/50,0/0,0/100
[kleidung] F257 abwurf typ=0x11 dir=1 hp=30 gen=2 panels=0/30,0/0,0/0,0/0,0/100,0/50,0/0,1/0
[wound] sync: applied=1 ok=1 gen=2 slotgen=2
[save] saved (room 1140) slot n=2 -> next=3; card=none (slot -1 qty 0)
```
HP-Verlauf (state.log): `F79 80 | F145 60 | F238 30 | F398 10`. Die Abwurf-Stempel entsprechen exakt den
Helpern (hinten 0x8010a208 (0,10)(4,50)(7,50), vorn 0x8010a1cc (0,10)(5,50)(7,50)); der dritte
ueberlebte Griff hebt Panel 7 auf 150 -> Stufe 1 bei **hp 30 (Caution)**, danach geht Leon auf hp 10
(Danger) — mit Decal. Vom Spiel geschriebener Spielstand (`nullen` auf einer KOPIE): `Raum 1140 char 0
pos (-1094,0,-19882) rot 2433 hp 10 cut 1 wunden 0/30 0/0 0/0 0/0 0/100 0/50 0/0 1/0`.

Bild: CONTINUE aus diesem Spielstand bzw. aus der Kopie mit genullter Wund-Tabelle, `RE15_INPUT_SCRIPT=R3,W3`
ab Bild 30, Framedump 20..160/4, beschleunigter Renderer: Differenz in JEDEM Bild ausschliesslich im
Figurenkasten x185..215 / y107..138 (Bild 20: 127 px, Bild 64: 139, Bild 112: 159, Bild 132..160: 57;
Rueckenansicht Bild 44..52, 80, 88: 0 px). Sichtpruefung der 5x-Vergroesserung (Bild 20 und 112, ohne/mit):
**roter Blut-/Rissfleck auf der Weste oberhalb von "R.P.D." (Leons rechte Brust/Schulter)**, Leon in der
Danger-Haltung (scratchpad `abn1/z1_zoom.png`, `abn1/z1_full20.png`).

### 4.4 Raumwechsel: das Decal bleibt

Derselbe Spielstand (mit / genullt), CONTINUE + `RE15_DEBUG_JUMP=1130@40` (Laden von ROOM1130 ueber den
Debug-Menue-Weg), `R4,W3`, Framedump 10..150/4 in ROOM1130 (`JUMP -> 113 3F CORRIDOR (ROOM1130)
spawn=(-1300,0,-13950)`): Differenz in 23 von 36 Bildern, alle im Kasten x172..191 / y87..104 (Bild 66:
55 px), 0 px in den Rueckenbildern; Sichtpruefung Bild 66 (6x): roter Fleck oben auf der Weste.
Die Wunde ueberlebt den Raumwechsel (Slot-0-Reupload). Einschraenkung: Raumwechsel per Debug-Sprung,
nicht per Tuer (der Lade-Pfad des Raums ist derselbe, die Tuer-Animation fehlt).

### 4.5 Hundeweg-Bild und Elza (Bilder der selbst gefahrenen Riegel-Laeufe, Sichtpruefung)

* Hundeweg (integration_r36_kleidung_hundeweg, Lauf hund_69f05852, Bild 42 Rueckenansicht): 12x-Ausschnitt
  (190..213, 92..115) ohne/mit: **dunkelroter Riss-/Blutstreifen rechts neben den Ruecken-Buchstaben** —
  klein (27 Pixel), aber eindeutig auf der Weste; keine Abweichung ausserhalb der Figur.
* Elza (integration_r36_kleidung_bild, Lauf 993f4427, EA/EB = alle Panels Stufe 2): Blutflecken/Risse auf
  dem weissen Shirt, Aermel und der roten Hose, keine blauen Saeume neben den UV-Inseln (LUT-Zeile 4).

### 4.6 Ursache dynamisch gegengeprueft (Fix erklaert den Befund)

Messkopie `re15_pc_abn1_master.exe` (= `re15_pc_abn0b_master.exe` der Abnahme 0, master-Bau ohne
`RE15_KLEIDUNG_LOG`-String), derselbe Kartenwerkzeug-Spielstand wie 4.3, `B9` ab Bild 30, Ende Bild 420:

| Gehirn | HP-Verlauf (state.log) | `[wound] sync` |
|---|---|---|
| master, RE2 (Standard) | F79 80, F145 60, F238 30, F398 10 | **keine** -> Leon auf Danger ohne Decal = Nutzer-Befund |
| master, `RE15_AI_FLAVOR=re15` | F75 90, F95 85, F142 75, F146 65, F253 55, F273 50 | `applied=1 ok=1 gen=2` (Decal) |
| HEAD c8033896, RE2 (4.3) | F79 80, F145 60, F238 30, F398 10 (identisch zu master) | `gen=2` beim 3. Abwurf (hp 30) |

`git show e11962ec` (2026-08-22, "RE2-KI ist jetzt der Standard"): `-static re15_ai_flavor_t s_flavor =
RE15_AI_FLAVOR_RE15;` -> `+... = RE15_AI_FLAVOR_RE2;`; e11962ec ist Vorfahre von v0.8.21 (9362dd7e).
Damit ist belegt: die Stempel hingen nur am RE1.5-Gehirn, seit e11962ec laeuft RE2 als Standard ->
keine Decals mehr; der Fix stempelt unter RE2 auf demselben Weg, ohne die Gegner-/HP-Kurve zu aendern.

### 4.7 Elza mit beschleunigtem Renderer (die Riegel bild/spielweg nutzen RE15_SOFTWARE_RENDER=1)

Kartenwerkzeug `karte re15_card.mcr 1131 4 -1024 0 -3336 856 20 <wunden> 2`, CONTINUE, Framedump Bild 150,
OHNE Software-Renderer: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; ohne/alle:2 = 474 px in
(194,110)-(223,181); ohne/`4:1,5:1` = 78 px in (207,127)-(222,154). Sichtpruefung 4x: Blut/Risse auf dem
weissen Shirt, am Arm und auf der Hose; Panels 4/5 = Fleck oben rechts auf dem Shirt; keine Saeume.

### 4.8 Hook-A-Typen in ALLEN Stages (Dossier §7 nannte die Zwillinge "nicht gegengelesen")

Byte-Scan STAGE1..5.BIN nach `lui at,0x800b` + `sw rX,(0xC758+4*Typ)(at)` (Hook-Tabelle 0x800ac758),
Wert von rX aus dem vorausgehenden `lui`/`addiu`:
```
STAGE1 0x10,0x11,0x12,0x16,0x18,0x1c-0x1f -> 0x8010a28c; 0x13 -> 0x8010c16c; 0x20/0x21/0x27 eigene
STAGE2 0x12,0x16,0x18,0x1c-0x1f -> 0x8010a120; 0x10,0x11,0x13 -> 0x8010c000; 0x22/0x23/0x25 eigene
STAGE3 0x10,0x11,0x12,0x16,0x18,0x1c-0x1f -> 0x8010a378; 0x13 -> 0x8010c258; 0x20/0x29 eigene
STAGE4 0x10,0x11,0x12,0x16,0x18,0x1c-0x1f -> 0x8010a240; 0x13 -> 0x8010c120; 0x29/0x2b eigene
STAGE5 0x10,0x11,0x12,0x16,0x18,0x1c-0x1f -> 0x8010a3c0; 0x13 -> 0x8010c2a0; 0x29/0x2b eigene
```
Die Zombie-Typmenge ist in allen fuenf Stages dieselbe {0x10-0x13, 0x16, 0x18, 0x1c-0x1f} = die Liste in
`re15_kleidung_generischer_griff` (+ 0x1A PORT-WAHL). Der Typ-Filter wirft also in keiner Stage einen
registrierten Greifer weg (keine Regression unter dem RE1.5-Gehirn, das vorher typ-unabhaengig stempelte).

### 4.9 Elza-Hundeweg in IHREM Raum ROOM1191 (beide Szenarien)

Kartenwerkzeug `karte re15_card.mcr 1191 4 3050 0 -11650 0 100 - 0`, CONTINUE, `RE15_DEBUG_SUB=13@60
RE15_DEBUG_DOGWAKE=110 RE15_KLEIDUNG_LOG=1 "RE15_EXIT_AT=600#1191"` (Dossier N1.5 nahm ROOM1190 mit char 4):
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[kleidung] F192 hundebiss typ=0x20 dir=2+2 hp=80 gen=1 ...
[kleidung] F195 hundebiss typ=0x20 dir=3+2 hp=60 gen=2 panels=0/0,0/0,0/0,0/0,1/0,0/0,0/50,0/0
[wound] sync: applied=1 ok=1 gen=2 slotgen=2
[kleidung] F197 ... hp=40 / F283 ... hp=20 / F318 ... hp=0 gen=3
```
Erstes Decal bei hp 60 auch fuer Elza in ROOM1191.

### 4.10 Gegenprobe des neuen Riegels mit der Vorher-exe

`cmake -DRE15_PC_EXE=.../re15_pc_abn1_vor.exe -DRE15_KARTE_TOOL=.../test_r36_kleidung.exe -DWORKDIR=<scratch>
-P re15_port/tests/integration/test_r36_kleidung_hundeweg.cmake` -> rc=1:
`r36_kleidung_hundeweg[bisse]: Biss ohne genau zwei Stempel (Massstab 0x32 je 10 HP): [kleidung] F582
hundebiss typ=0x20 dir=2 hp=80 gen=1 ...` — der Riegel trennt vorher/nachher (Restkopie
`re15_pc_r36b_hund_806d5cdd.exe` danach entfernt).

## 5. Vertrag

* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine im Diff (`flag_set|msg_install|scd_event_fire` im
  Code-Diff: nur `re15_game_flag_set(3, 152, 1)` im TEST (bestehendes Raum-Flag "Hunde frei", Muster
  probe_r27_hund_biss.c)). Spur B hat laut VERTRAG.md keine Zuteilung und nimmt keine.
* Pfade: keine Datei unter release/, platform/android/, shared_assets/PSX/, keine tests/*/CMakeLists.txt.
  Tests ueber die eigene `tests/unit/probes/r36_kleidung.cmake` (GLOB), Messkopien mit eigenem Namen.
* Gemeinsame Dateien nur kleine Haken (numstat): enemy_ai_common.c +5/-13 (Include, Accessor, zwei
  Inline-Bloecke -> je 1 Aufruf, 1 Zeile EM01A), main.c +8/-10 (LUT-Tabelle -> Aufruf), re15_damage.c
  +6/-3, enemy_ai_re2_{zombie,dog,crow}.c je Include + 1-3 Zeilen, je 1 Deklaration in 2 Headern.
* Riegel je Teil vorhanden und messend (8 Spur-Tests, alle selbst gruen gefahren, s. 3).

## 6. Urteil je Nutzer-Punkt

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt." | **erfuellt** | Unter dem Standardgehirn RE2 an der exe gemessen: Zombie-Weg (eigener Lauf 4.3) Decal beim 3. ueberlebten Griff bei hp 30 (Caution), danach hp 10 (Danger) mit Decal, Bild: roter Blut-/Rissfleck auf der Weste (Sichtpruefung, 127..159 px nur im Figurenkasten); Riegel-Weg hp 20 (131 px). Hundeweg ROOM1190 (dokumentierter Weg des Nutzers) Decal beim 2. Biss bei hp 60 (Caution) — vorher nie (Vorher-exe: 5 Bisse bis hp 0, gen 1); RE1.5-Gehirn auf demselben Weg ebenfalls hp 60; Bild 42 Westen-Rueckseite 27 px. Decal ueberlebt Speichern/CONTINUE und Raumwechsel (4.4). Ursache (e11962ec, RE2 Standard; Stempel nur im RE1.5-Zombie-Code) dynamisch mit dem master-Bau gegengeprueft (4.6). Elza (beide Szenarien) gleich (4.7, 4.9). |

## 7. Gates

| Gate | Ergebnis |
|---|---|
| Bau (`local_build.sh configure` + `build`) | OK, gebauter Stand = HEAD c8033896 |
| Spur-Tests + Modul-Gruppen | 118/118 + 3/3 gruen (selbst gefahren) |
| RE-Gate (3+ Adressen selbst disassembliert, richtige Binaerdatei) | OK — 0x8010f2c8 / 0x8010f360-64 / 0x8010f444 / 0x80121014 (STAGE1.BIN), 0x8001a780 (PSX.EXE), 0x80104EB8 + 0x801051f8 (EMD0G_MOD0.BIN); jede neue Konstante mit @0x im Code und in der Commit-Message |
| Rate-Woerter / Env-Schalter als Abschluss | keine |
| Fix erklaert den Befund | ja (M1: halber Massstab 1 Stempel/20 HP; Ursprung: e11962ec, dynamisch belegt) |
| Vertrag (Bits/IDs/Ereignisse/Pfade/Haken) | eingehalten |
| Nebenwirkungen | Hunde-/Spielerverhalten bytegleich (state.log 1141 Zeilen identisch), Zombie-HP-Kurve master = HEAD, RE1.5-Gehirn-Stempel unveraendert (griff_re15 F416), Elza-Decals jetzt auf Elzas Panels, Typ-Filter verliert in keiner Stage einen Greifer (4.8) |

## 8. Maengel

Keine blockierenden Maengel.

Hinweise (nicht blockierend, nachpruefbar):
1. Die Riegel `integration_r36_kleidung_bild` und `_spielweg` vergleichen mit `RE15_SOFTWARE_RENDER=1`
   (test_r36_kleidung_bild.cmake:59, test_r36_kleidung_spielweg.cmake:68/116); nur `_hundeweg` nimmt den
   beschleunigten Renderer. Die eigenen Laeufe 4.3/4.4/4.7 (beschleunigt) bestaetigen dieselben Decals —
   eine Regression nur im beschleunigten Wund-Blit bei Vorderansicht/Elza wuerde aber nur der Hundeweg-
   Riegel (Rueckenansicht) fangen.
2. Dossier §6 nennt `probe_save_injured` und `probe_r17_weste_karte` unter "Bestehende Riegel unveraendert
   gruen" — beides sind Werkzeuge ohne add_test (tests/unit/CMakeLists.txt:854-857, probes/r17_ada-und-weste.cmake:4-9),
   keine ctest-Riegel.
3. Sichtbare Nebenwirkung, die §8 "Fuer den Nutzer" nicht nennt: unter dem RE2-Gehirn spawnt jeder
   ueberlebte Griff jetzt zusaetzlich das RE1.5-Release-Blut (FUN_80019700 a0=0x1500 @0x8010a574, Part 8
   bzw. Part 3) — belegt (Phase 4 @0x8010a4e8, Dossier §4), aber dem Nutzer nicht angekuendigt.
4. Im Kampfbild von ROOM1190 (Kamera "Firing Range", Leon seitlich) ist das erste Decal nicht zu sehen
   (Panel 4 = Westen-Rueckseite, Bild 560..700: 0 px) — erwartet nach Original-Mapping (Biss von hinten ->
   04/06), sichtbar sobald Leon der Kamera den Ruecken zeigt; der Nutzer sollte das wissen.
5. Raumwechsel nur per Debug-Sprung gemessen (4.4), nicht per Tuer; der Lade-Pfad ist derselbe.
6. OFFEN ausserhalb Spur B (Adressen selbst bestaetigt): RE2-Hund-Claim PL+0x1D3|=0x80 (@0x801051f8/
   0x80105200, Pruefung @0x80104e5c-68) fehlt im Port -> drei Bisse in fuenf Bildern (F582/F585/F587).
   Ebenso offen (vorbestehend, nie portiert): STAGE3-5 (0,0x32)-Stellen, Birkin-Stempel STAGE5, RE2-Kraehe
   ohne Dive/Strike-Zwilling. Die RE2-Kraehe ist nur per Unit-Riegel (ROOM10C0 live) gemessen, nicht an der exe.
7. Keine v0.8.21-Windows-exe lokal (release/ enthaelt nur APKs) — die Gegenprobe lief mit dem master-Bau
   der Abnahme 0 (4.6); e11962ec ist Vorfahre von v0.8.21.

## 9. Ergebnis

**bestanden** — der Nutzer-Punkt ist am gebauten Stand erfuellt, alle Gates halten. Mangel M1 der Abnahme 0
(Hundeweg ohne Decal) ist behoben und erklaert; der in Abnahme 0 offene Zombie-Weg ist selbst gemessen.
Messdaten/Bilder: Scratchpad `abn1/` (hund_nach, hund_vor, hund_re15, z1, z1_mit/ohne, z2_mit/ohne,
m_re2/m_re15, elza_EA/EB/EC, elza_hund, gegenprobe_wd).
