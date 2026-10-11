# Runde 36 — Spur J "szenenleon": UNABHAENGIGE ABNAHME 1 (nach Nachbesserung 1)

Pruefer: Abnahme-Agent 1 (glaubt nur, was er selbst am gebauten Stand gemessen hat).
Baum: `.claude/worktrees/r36_szenenleon`, Zweig `r36/szenenleon`, gemessen auf c532feca (Merge-Basis mit master
836023f5; master steht inzwischen auf 565da62b = nur synchro/*.wav der Spur L — der Zwei-Punkt-Diff `git diff master`
zeigt diese 66 wav deshalb als "geloescht", der Drei-Punkt-Diff `master...HEAD` beruehrt synchro/ NICHT).
Massstab: AUFTRAG.md (Wortlaut + Nachtrag 14:00 + Nachtrag "abends" Spur J Punkt 2), VERTRAG.md, CLAUDE.md (RE-Gate),
Maengel M1-M4 der Abnahme 0.

STAND: ABGESCHLOSSEN 2026-10-11.

## Ergebnis: BESTANDEN

Alle vier Punkte am gebauten Stand (c532feca) ueber den ECHTEN Weg gemessen: 1 erfuellt, 2 erfuellt (inkl. Nachtrag
"erst schliessen, dann abschliessen"), 3 erfuellt, 4 erfuellt. Maengel M1-M4 der Abnahme 0 behoben. RE-Gate haelt (11
Stichproben), Vertrag haelt, gezielte Tests 159/159 gruen. Keine offenen Maengel; Hinweise H1-H8 in §7.

## 0. Bau

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
- `git status --short re15_port` leer (Quellen = HEAD c532feca). re15_pc.exe 2026-10-11 01:07:52, juengste Quelle
  szenenleon_r36.c 01:07:31 (`find ... -newer re15_pc.exe` leer); test_r36_szenenleon.exe 01:08:08.
- Messkopie `re15_port/build/platform/pc/re15_pc_r36j_abn1.exe` (lauf.sh im Notizblock
  `%TEMP%/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/J_abn1/`): Titel-Autovorlauf
  (RE15_TITLE_SHOT_AF=2), RE15_WINDOW_SCALE=1, SDL_AUDIODRIVER=dummy, **SDL_RENDER_DRIVER=direct3d11 (beschleunigt, KEIN
  Software-Renderer)**, Arbeitsordner = Zielordner (debug.log dort). Eigene Werkzeuge im Notizblock: `tools/bgvgl.py`
  (BSS-Bildvergleich), `tools/tuerbild.py` (Tuerbereich je Framedump gegen BSS zu/offen), `tools/zoom.py` (Ausschnitte).
- Referenzbilder selbst dekodiert: `build/tests/unit/probe_bg_dump.exe shared_assets/PSX <J_abn1/bg> 1` (428 Bilder, 0
  Fehlschlaege). `bgvgl.py` (Schwelle 30): **Bild 2 vs 8: 1135 px, bbox x 127..169 y 86..145; Bild 0 vs 7: 678 px, x
  206..230 y 36..96**; 3 vs 9: 49 px; 4 vs 10: 36 px. Augenschein (`bg_tuer_2_8_0_7.png`, `bg_tuer_0_7.png`): 2/0 = Tuer
  GESCHLOSSEN, 8/7 = Tuerblatt aufgeschwungen (OFFEN).

## 1. Punkt 1 — ROOM11B0: Animation + gemeinsames Wegschieben des Containers mit Marvin

Wortlaut: "In der Cutscene in ROOM 11B0 mit Leon, Ada und Marvin klappt das mit der Animation und dem gemeinsamen
Wegschieben des Containers mit Marvin noch nicht." (Teil-1-Code seit Abnahme 0 unveraendert; neu gemessen.)

ECHTER Weg Tuer ROOM11C0 -> ROOM11B0: Lauf `a_11b0_tuer` (Befehl 2.1) fuer state.log/debug.log, Lauf `p1_schub` (gleiche
Schalter, `RE15_FRAMEDUMP=1600-1800/4`, `RE15_EXIT_AT=1800#11b0`) fuer die Bilder.
- debug.log (beide Laeufe): `[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13 (Schub-Sub @0x80052864)` +
  `[scd] Plc_dest(slot=1 mode=0x13 dest=(0,0)) -> state4/sub19` (Marvin) -> `[szl-r36] ROOM11B0 Spieler: Schub-Sub zu Ende
  -> Sub 6 (Event-Reach)`.
- state.log (Lauf a, eigene Auswertung; Leon = PL, Marvin = Aktor 1 t=40):
  ```
  F1641 Leon (-1651,-21535) mo=17 | Marvin ss1=19 ss2=3 mo=17 af=1 @(852,-21516)   <- beide Sub 0x13, Clip 0x11
  F1650 Leon (-1651,-21535) mo=18 | Marvin ss1=19 ss2=5 mo=18 af=0 @(852,-21516)   <- beide Clip 0x12 (Schub)
  F1680 Leon (-1651,-20897) mo=18 | Marvin ss1=19 ss2=5 mo=18 af=5 @(857,-20897)   <- gleichauf in z
  F1716 Leon (-1639,-19999) mo=18 | Marvin ss1=19 ss2=5 mo=18     @(873,-19999)
  F1728 Leon (-1639,-19999) mo=17 | Marvin ss1=19 ss2=6 mo=17                      <- 0x11 rueckwaerts
  F1740 Leon mo=1 (Sub 6)         | Marvin ss1=6                                    <- beide Sub 6
  ```
  Leon +1536 in z, Marvin +1517, in jedem Bild dieselbe z-Lage (Schub-Takt synchron).
- Bilder `p1_schub_sheet.png` (F1632/1648/1668/1692/1716/1744): beide neben dem Container -> beide mit dem Ruecken zur
  Kamera, Haende am Container in Schiebehaltung (Marvin an der rechten Kante) -> der Container wandert nach hinten -> ab
  F1716 liegt der Gully-Deckel frei -> beide stehen danach neben dem verschobenen Container.
- RE-Stichprobe (selbst, info/Re1.5/PSX.EXE): Tabellen `0x80073e30[19]` -> 0x80052864 und `0x80076ca0[19]` -> 0x80052864;
  @0x80052a74..@0x80052aa0: +0x6=5 (`sb v0,6(v1)`), +0x94=0x12 (`ori v0,zero,0x12` / `sb v0,148(v1)`), +0x95=0.

**Urteil Punkt 1: erfuellt.**

## 2. Punkt 2 — Erweiterung: Marvin schliesst die Werkstatt-Tuer, dann schliesst er ab

Wortlaut (Nachtrag 14:00 + Nachtrag "abends" massgeblich): "Hold on" -> Marvin rennt zur Innenseite der Werkstatt-Tuer ->
(Nachtrag abends) ZUERST die Tuer SCHLIESSEN inkl. Animation + Tuer-zu-Ton -> DANN Zuschliess-Animation + Zuschliess-Ton
(RE2 Robert Kendo) -> Tuer bleibt geschlossen, gleiche Meldung wie bei Elza -> zurueck zu Leon/Ada -> "I just want to make
sure we don't have any unwanted followers." -> spaeter beim Anklicken Elzas Text. Parkplatz-Tuer NICHT.
Abnahme-Vorgabe: Ereignisfolge im Log mit Bildnummern (Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton) +
Framedump-Serie.

### 2.1 Szene ueber den ECHTEN Weg (Tuer ROOM11C0 -> ROOM11B0), Lauf `a_11b0_tuer`
`lauf.sh a_11b0_tuer 700 RE15_DEBUG_JUMP=11c0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3,4:0x40 RE15_FIRE_AOT=0@60#11c0
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=<ziel>/state.log RE15_FRAMEDUMP=2280-2520/1:<ziel>/f_ RE15_EXIT_AT=30#1260`
-> exit=0, 241 Bilder. debug.log (woertlich, gekuerzt um [walk]/[framedump]):
```
[fire-aot] slot=0 at F60 (Raum 11C0)          [se] Stimme: se=1 layer=0 vag=2 note=61 ... (13641 Hz)   (Tuerdurchgang)
[room] PC loaded room11b0.rdt (232492 bytes)
[scd F1587] Cut_chg(6)
[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13 (Schub-Sub @0x80052864)
[scd] Plc_dest(slot=1 mode=0x13 dest=(0,0)) -> state4/sub19 (POSE/EVENT, kein Walk)
[szl-r36] ROOM11B0 Spieler: Schub-Sub zu Ende -> Sub 6 (Event-Reach)
[szl-r36] ROOM11B0 sub06 @0x017EA Aot_on 5 -> erst die Erweiterung (Ereignis 22: Marvin schliesst die Werkstatt-Tuer ab)
[msg] room=11b0 id=20
[scd F2232] Cut_chg(10)     ...     [scd F2315] Cut_chg(8)
[scd] Plc_dest(slot=1 mode=0x09 dest=(-22903,-16230)) -> state4/sub9
[scd] Plc_motion(entity=0, motion=20, flags=0x00) -> slot=1
[szl-r36] ROOM11B0 F2376 +0x005C Schliessen: Plc_motion(0,20) Marvin greift zur offenen Tuer
[scd F2391] Cut_chg(2)
[se] Se_on bank=14 id=1 -> RE2-Tuerbank (Runde 35 Spur K)
[se] Stimme: se=1 layer=0 vag=2 note=61 fine=109 center=83 vol=110 pitch=0x4f3 (13641 Hz) -> SE-Stimme 7 (SPU 23)
[szl-r36] ROOM11B0 F2391 +0x0064 Tuer zu: Cut_replace 0<->7 + 2<->8 (sub00 @0x01184/@0x01188 zurueck), Cut_chg 2
[szl-r36] ROOM11B0 F2391 +0x006E Tuer-zu-Ton: Se_on Port-Bank 0x0E Satz 1 (RE2 DOOR1A Door_exit)
[scd F2401] Plc_flg(subop=0, mask=0x0080) -> slot=1 anim_flags=0x0080          (Clip 20 rueckwaerts)
[szl-r36] ROOM11B0 F2426 +0x008A Abschliessen: Plc_motion(0,15) (RE2 Kendo Clip 0)
[se] Se_on bank=0x3A id=10 -> RE2-Schlossbank SCHLOSS1010 (Runde 36 Spur J)
[se] Stimme: se=10 layer=0 vag=3 note=66 fine=0 center=90 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme -15 (SPU 1)
[szl-r36] ROOM11B0 F2446 +0x0092 Abschliess-Ton: Se_on Port-Bank 0x3A Satz 0x0A (SCHLOSS1010)
[scd F2456] Plc_flg(subop=0, mask=0x0080)                                      (Clip 15 rueckwaerts)
[scd F2551] Cut_chg(4)   [scd F2636] Cut_chg(6)   ...   [msg] room=11b0 id=21
[aot] DOOR FIRE slot=5 ...  -> [room] PC loaded room1260.rdt
```
**Ereignisfolge mit Bildnummern: Schliessen F2376 < Tuer-zu-Ton F2391 < Abschliessen F2426 < Abschliess-Ton F2446** (Abstand
Schliessen->Tuer zu 15 Bilder = Sleep 15, Tuer-zu-Ton->Abschliessen 35 = Sleep 10 + 25, Abschliessen->Klick 20 = Sleep 20).
Der Tuer-zu-Ton ist dieselbe Stimmzeile wie beim Tuerdurchgang 11C0->11B0 (vag=2 note=61, 13641 Hz).

Tuerbild je Bild (`tools/tuerbild.py bg 2 8 a_11b0_tuer state.log`, Tuerbereich = Pixel mit |Bild2-Bild8|>30):
- F2354..F2391: OFFEN (71..80 % der sichtbaren Tuerpixel naeher an Bild 8; d_offen 12..15, d_zu 47..50).
  (F2316..F2353 laeuft Marvin durch den Tuerbereich und verdeckt ihn — dort keine Aussage.)
- **F2392..F2520: ZU in allen 129 Bildern** (92..97 % naeher an Bild 2, d_zu 6..13, d_offen ~55).
- Das Tuerbild wechselt also EIN Bild nach dem Bild, in dem Cut_chg(2) und der Tuer-zu-Ton laufen (Darstellung folgt
  der VM um ein Bild, wie bei jedem Cut_chg).
- Augenschein (`a_schliessen_1.png` F2370..F2391, `a_schliessen_2.png` F2392..F2426, Ausschnitt x 95..195, y 70..170, 2x):
  F2370 Marvin steht vor der OFFENEN Tuer (Blatt aufgeschwungen hinter ihm), F2378..F2391 linker Arm geht zur Tueroeffnung
  vor, F2392 Tuer geschlossen gemalt, Hand am Blatt, F2395..F2426 Arm zurueck; danach Kendos Griff.
- Grenze (vom Bau-Agenten als OFFEN 5 genannt, hier bestaetigt): das Tuerblatt existiert nur im Hintergrund, es
  SPRINGT von offen auf zu (ein Bild), ein Zuschwingen gibt es in den Daten nicht (kein Tuer-Objekt: RDT @0x4C/@0x54 = 0
  selbst gelesen; Obj_model_set nur Container/2 weitere). Der Nachtrag verlangt "sichtbar, wie die Tuer zugeht, soweit die
  Tuer im Bild/als Objekt existiert" — das ist erfuellt.
- Marvins Gesten je Bild (`tools/handbahn.py` auf `tools/r34n_d/gesten_arme.py`, ROOM11B0 rec 1): Clip 20 linke Hand
  0,34,110,...,665,**667 (Bild 15)**,660,...,489 (Bild 24) — vorwaerts bis zum Maximum, dann gehalten (Bild 24 noch 73 %),
  rueckwaerts zurueck auf 0; Clip 15 (Kendo) linke Hand 0..**900 (Bild 19)**. state.log Marvin: F2376 mo=20 af=1 ...
  F2390 af=15 (VM-Bild des Tausches F2391 = af 16), F2401..F2425 Clip 20 rueckwaerts, F2426 mo=15 af=1 ... F2445 af=19
  gehalten bis F2455, F2456.. Clip 15 rueckwaerts. Kein Sprung, keine doppelte Geste.

### 2.2 Danach, ECHTER Weg: Szene -> Gully -> ROOM1260 -> Leiter zurueck -> zu Fuss zur Werkstatt-Tuer (Lauf `r_rueckweg`)
(9,84) setzt hier die Szene selbst (KEIN RE15_SET_FLAG fuer Bit 84):
`lauf.sh r_rueckweg 480 RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3 RE15_FIRE_AOT=0@60#1260
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=2900
RE15_INPUT_SCRIPT=R0.3667,U1.0667,L0.2,U2.2333,R0.2,U4.6667,R0.2333,U4.4333,L0.2333,U0.6667,W0.3333,A0.2,W3
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_TUERSE_LOG=... RE15_STATE_LOG=... RE15_FRAMEDUMP=2900-3420/2:<ziel>/f_
RE15_EXIT_AT=3420#11b0` (Weg vorher an der Gully-Ankunft geeicht: Drehung 96/Bild, Gehen 75/Bild, Lauf `k_weg1`;
Wegpunkte = Marvins Weg der Erweiterung).
- debug.log: Szene wie 2.1 (F2380 Schliessen < F2395 Tuer-zu-Ton < F2430 Abschliessen < F2450 Abschliess-Ton) -> `[aot] DOOR
  FIRE slot=5` -> `[room] PC loaded room1260.rdt` -> `[fire-aot] slot=0 at F60 (Raum 1260)` -> `[aot] DOOR FIRE slot=0 ...
  target_cut=6 spawn=(-195,0,-21821)` (ROOM1260 Door_aot_set Slot 0 @0x0112A -> Raum 0x1B) -> `[room] PC loaded
  room11b0.rdt` -> **`[szl-r36] ROOM11B0 Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf nach sub00 ...`**.
- state.log Wiedereintritt (eigene Auswertung): Kamerafolge **6 (F1) -> 5 (F2916) -> 4 (F3062) -> 3 (F3182) -> 2 (F3210,
  Leon (-17743,-22218))**, **0 von 3420 Bildern mit Cut 7/8**; F3339 `msg(a=1 ... id=22)` bei Leon (-20942,-16152);
  tuerse.log `F3339 raum=11B1 nachricht=12 art=M weg=AOT satz=0(ZU_A) nr=1`.
- Tuerbild: alle 106 Framedumps mit Cut 2 (F3210..F3420) ZU (d_zu 6..12 ab F3212, d_offen ~55). Kontaktbogen
  `r_weg_sheet.png`: Gully -> Werkstatt mit Streifenwagen auf der Buehne -> Werkstatt-Tuer geschlossen gemalt ->
  "The door is lo[cked.]".
- Gegenprobe Original (Zonen-Netz selbst aus der RVD RDT @0x258 gelesen): getauscht (Tuer offen) faehrt dieselbe Folge
  3 -> 8 (Datensatz @0x0334 `from=3 to=2`, cam_to 2<->8 getauscht); ausgeliefert 3 -> 2. Die Messung oben zeigt 2.

### 2.3 Danach: Werkstatt-Tuer von beiden Seiten (Sprung mit den Flags nach der Szene)
`lauf.sh b_ost 300 RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=4:0xf3,3:0x43,3:0x82,3:0x83,4:0xcd,9:0x54
RE15_PLAYER_POS=-20500,-16300,2048 RE15_TUERSE_LOG=<ziel>/tuerse.log RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT=U1.5,W0.5,A0.2,W3 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=...
RE15_FRAMEDUMP=0-420/1:<ziel>/f_ RE15_EXIT_AT=430#11b0`; West (`c_west`) gleich mit `RE15_PLAYER_POS=-23300,-16300,0`.
- debug.log (beide): `[szl-r36] ROOM11B0 Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf nach sub00 ...`,
  im selben Init-Lauf `[scd F250] Cut_chg(7)` (sub00 @0x0117E) und `[scd F250] Cut_chg(0)` (Ereignis 23).
- Ost: Leon bleibt bei x -20922 stehen (Zelle (3,0x0C) solid); F360 state.log `msg(a=1 fsm=1 id=22)`, tuerse.log
  `F360 raum=11B1 nachricht=12 art=M weg=AOT satz=0(ZU_A) nr=1` (= Elzas Tuer-zu-Ton), debug.log `[se] Stimme: se=0 ...
  (15256 Hz)`. West: dasselbe (`F360 raum=11B1 nachricht=12 ...`).
- Tuerbild: Ost 421 Framedumps F0..F420 alle ZU (ab F6 d_zu 3.4 / d_offen 55), state.log cam=2 in jedem Bild (F0 = 0 aus
  dem Init); West 421 Framedumps alle ZU (ab F6 d_zu 0.0 / d_offen 47), cam=0 in jedem Bild. Kein einziges Bild 8/7.
- Bild `bc_tuer_text_F370.png` (Notizblock): links Ost Cut 2, rechts West Cut 0 — Tuer geschlossen gemalt, Leon davor,
  Zeile "The door is lo[cked.]" im Aufbau.
- Text: msg 22 = ROOM11B1 msg 12 @0x179F Byte fuer Byte (23 B, selbst verglichen), Text-Platz 8 = ROOM11B1 sub00
  @0x0112A bis auf das Nachrichten-Byte (12 -> 22), selbst verglichen.

### 2.4 Lade-Weg (CONTINUE, Boot-Weg in main.c)
Karte mit `probe_r36_szenenleon_karte.exe re15_card.mcr ost cut=2` (Ausgabe `Karte re15_card.mcr: Raum 0x11B0, Leon Ost
(-20500,-16300), Flag(9,84)=1, Cut 2`), exe mit `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_FRAMEDUMP=0-200/2 RE15_EXIT_AT=200#11b0` (direct3d11): debug.log `[save] CONTINUE: resumed in room 11b0` ->
`[szl-r36] ... Ereignis 23 im Init-Lauf ...` -> `[scd F0] Cut_chg(7)` / `[scd F0] Cut_chg(0)`; state.log 201 Bilder cam=2;
101 Framedumps alle ZU (d_zu 2.9..3.4). Hinweis: ROOM11B0 hat keine Schreibmaschine (kein "Save"-Text in der RDT,
selbst gesucht) — ein Spielstand IN 11B0 entsteht im echten Spiel nicht; der Lauf prueft nur die Robustheit des
Boot-Wegs (Haken main.c).

### 2.5 Urteil Punkt 2
Gegen den Wortlaut (14:00 + Nachtrag abends), je Teil gemessen:
| Teil | Beleg | erfuellt |
|---|---|---|
| "Hold on" | msg 20 "Marvin: Hold on." (2.1) | ja |
| rennt zur Innenseite der Werkstatt-Tuer | Plc_dest Modus 5, ~200 Einheiten/Bild (Leon geht 75), Ankunft (-20860,-16230) ostseitig, Drehung nach Westen (2.1) | ja |
| ZUERST schliessen inkl. Animation (Nachtrag) | Clip 20 F2376, Tuerbild 8 -> 2 ab F2392 (Pixel), Hand am Blatt (2.1) | ja (Tuer springt, s. Grenze) |
| Tuer-zu-Ton | RE2-Tuerbank DOOR1A Satz 1 (Door_exit) F2391 (2.1) | ja |
| DANN Zuschliess-Animation + -Ton (Kendo) | Clip 15 == RE2 1010 Clip 0 F2426, SCHLOSS1010 Satz 0x0A F2446 (2.1, 5.1) | ja |
| Reihenfolge Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton | F2376 < F2391 < F2426 < F2446 (2.1); F2380 < F2395 < F2430 < F2450 (2.2) | ja |
| Tuer bleibt geschlossen | Bild ZU in jedem Framedump nach F2392 (2.1), beim Wiederkommen ueber den echten Weg (2.2), Ost/West (2.3), CONTINUE (2.4); Zelle solid, Leon kommt nicht durch | ja |
| gleiche Meldung wie bei Elza | msg 22 == 11B1 msg 12 Byte fuer Byte, Text-Platz == 11B1 @0x0112A, Tuer-zu-Ton wie Elza (tuerse.log raum=11B1 nachricht=12) (2.2/2.3) | ja |
| zurueck zu Leon/Ada + "I just want to make sure we don't have any unwanted followers." | Rueckweg Cut 4/6, msg 21 zweizeilig (2.1) | ja |
| Parkplatz-Tuer NICHT | kein Opcode/Haken auf 11B0 Slot 1 (Programm + Ereignis 23 gelesen) | ja |

**Urteil Punkt 2: erfuellt.** (Grenze, kein Mangel: das Tuerblatt existiert nur als Hintergrundbild — es springt in einem
Bild von offen auf zu, ein Zuschwingen geben die Daten nicht her; der Nachtrag verlangt das Zugehen ausdruecklich nur "soweit
die Tuer im Bild/als Objekt existiert".)

## 3. Punkt 3 — ROOM10F0: Kamera bleibt auf Leon, kein Ploppen, nur Tuer-zu-Ton zum Schluss

Wortlaut: "In der neuen Cutscene in ROOM 10F0 verfolgt die Kamera wie Marvin und Ada den Raum verlassen. Sie 'ploppen'
dann einfach weg. ... Bleibe lieber mit der Kamera wenn Marvin und Ada den Raum verlassen auf Leon fixiert. und mache an
der Stelle nur einen 'Tuer schliessen' Sound zum schluss." (Code seit Abnahme 0 unveraendert; trotzdem neu gemessen.)

ECHTER Weg Tuer ROOM10D0 -> ROOM10F0, Lauf `e_10f0`: `lauf.sh e_10f0 480 RE15_DEBUG_JUMP=10d0@250 RE15_SET_FLAG=3:0x32
RE15_FIRE_AOT=0@90#10d0 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=... RE15_FRAMEDUMP=2560-2820/1:<ziel>/f_
RE15_EXIT_AT=2830#10f0` -> exit=0, 261 Bilder.
- debug.log: `[fire-aot] slot=0 at F90 (Raum 10D0)` -> `[room] PC loaded room10f0.rdt` -> `[cut10f0] ROOM10F0: Szene
  gestartet` -> Kamera-Anweisungen F6 2, F66 0, F396 2, F772 0, **F911 Cut_chg(2) = die letzte**; danach bis F2830 kein
  Cut_chg (`[cut10f0] Szene zu Ende`). state.log: cam=2 in jedem Bild ab F911 bis F2830.
- state.log Spruenge > 2000 (eigene Auswertung): Marvin (Aktor 2, t=40) F2670 (8328,-278) -> (-30000,-30000); Ada
  (Aktor 1, t=42) F2687 (8315,-246) -> (-30000,-30000).
- Pixel-Differenz aufeinanderfolgender Framedumps (Schwelle 60): bis F2664->F2665 wandert die Aenderung an den unteren
  Bildrand (bbox y bis 215), **F2665 -> F2700 in JEDEM Bildpaar 0 Pixel** — beide Park-Bilder (F2670, F2687) aendern im
  Bild nichts. Kontaktbogen `p3_abgang_sheet.png` (F2600..F2700): beide gehen auf die Kamera zu und verlassen das Bild
  unten, Leon bleibt hinten an den Pulten im Bild.
- Ton: zwei `Se_on bank=14 id=1 -> RE2-Tuerbank` im ganzen Lauf — der erste beim Hereinkommen von Ada/Marvin (Szene
  Runde 35, vor Zeile 23), **der zweite im Bild F2687** (zwischen den Framedump-Zeilen f_002686 und f_002687 = im Bild,
  in dem Ada geparkt wird), danach keiner. = ein Tuer-zu-Ton zum Schluss.

**Urteil Punkt 3: erfuellt.**

## 4. Punkt 4 — Irons tot: kein Kopf-Tracking mehr hinter Leon

Wortlaut: "Sobald Irons gegen Ende von Stage1 tot ist, soll er auch nicht mehr den Kopf hinter Leon hinterher bewegen."
(Code seit Abnahme 0 unveraendert; neu gemessen.)

### 4.1 ECHTER Weg: Tuer ROOM1130 -> ROOM1150, Todesszene (Lauf `f_1150_tod`)
`lauf.sh f_1150_tod 600 RE15_DEBUG_JUMP=1130@250 RE15_SET_FLAG=9:0x47,3:0x5e RE15_FIRE_AOT=2@90#1130 RE15_IT_LOG=1
RE15_NECK_TRACE=<ziel>/neck.log RE15_STATE_LOG=... RE15_EXIT_AT=5#1040` -> EXIT in ROOM1040 (Montage durchlaufen).
- debug.log: `[fire-aot] slot=2 at F90 (Raum 1130)` -> `[room] PC loaded room1150.rdt` -> `[irons-tod] ROOM1150 Zustand 1:
  Szene startet` -> `[irons-tod] ... Signal (5,12): Irons' Arm faellt (Clip 2 ab Bild 72)` -> unmittelbar
  `[szl-r36] ROOM1150 Irons tot: Kopf-FSM +0x1b8 = 0x12 (kein Blickziel mehr)` -> Tuerknall -> Montage 1130 -> 1040.
- neck.log Irons (slot=1, eigene Auswertung): F6..F893 `fl=00` (Tracking; zuletzt acc=(-335,-233) tgt=(-333,-159));
  **F894 `fl=12`**, Akku gleitet in 15 Bildern auf (0,19) und steht **F909..F1332 = 424 Bilder konstant (0,19)**.

### 4.2 Gegenprobe Standpunkt A/B, tot gegen lebend (Bild 150, Cut 7 = Nahaufnahme der Liege)
`lauf.sh g_<fall> 200 RE15_DEBUG_JUMP=1150@250 RE15_SET_FLAG=<flags> RE15_PLAYER_POS=<A|B> RE15_FORCE_CUT=7
RE15_NECK_TRACE=... RE15_FRAMEDUMP=150:<ziel>/f_000150.ppm RE15_EXIT_AT=160#1150`; tot = 9:0x49,9:0x47,3:0x5e, lebend =
3:0x5e; A = (-17500,-25500), B = (-21000,-22500).
| Fall | Irons (state.log F150) | neck.log F150 |
|---|---|---|
| tot A | st=4 ss1=0 ss2=2 mo=2 af=89 | fl=12 acc=(0,19) |
| tot B | st=4 ss1=0 ss2=2 mo=2 af=89 | fl=12 acc=(0,19) |
| lebend A | st=4 ss2=1 mo=3 af=5 | fl=00 acc=(-79,-18) |
| lebend B | st=4 ss2=1 mo=3 af=5 | fl=00 acc=(-714,-58) |
- Pixel (Schwelle 30): **tot A gegen tot B: 0 px**; lebend A gegen lebend B: 165 px, bbox x 247..273 y 52..78 (= der Kopf).
  Bild `p4_irons_AB.png` (lebend A, lebend B, tot A, tot B): lebend dreht der Kopf mit Leons Standpunkt, tot nicht. Die
  Messung erkennt Tracking — beim Toten ist keins da.
- Form im Code: +0x1b8 = 0x12, `ori v0,zero,0x12` @0x80042614 / `sb v0,440(s0)` @0x80042618 (Abnahme 0 selbst
  disassembliert; Code unveraendert).

**Urteil Punkt 4: erfuellt.**

## 5. Gates

### 5.1 RE-Gate (Stichproben SELBST disassembliert/gelesen; `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` bzw.
`re2_disasm.py`, richtige Binaerdatei je Zeile)
| zitiert (Nachbesserung 1) | selbst gelesen | Urteil |
|---|---|---|
| Cut_replace = TAUSCH, LAB_80040414 @0x8004044c..@0x800404a8 (RE1.5 PSX.EXE) | @0x80040438 `lbu a2,1(v1)` (a), @0x80040440 `lbu a1,2(v1)` (b); @0x80040458 `bne v0,a2` -> @0x80040464 `sb a1,0(v1)`, @0x80040468 `bne v0,a1` -> @0x80040470 `sb a2,0(v1)`; dasselbe fuer +1 @0x8004047c..@0x80040494; @0x80040498 `addiu v1,v1,20`; Ende bei 0xff @0x800404a4 | stimmt: a<->b, selbstinvers |
| "Cut_replace wechselt die Ansicht nicht" (@0x800404ac..c8 Anker-Fixup) | @0x800404b0 `lw v0,0x800ac794`, @0x800404b8 `lbu v0,2(v0)`, @0x800404c0 `bne v0,a2`, @0x800404c8 `jal 0x800142f4` mit a0=a1; Sprungziel FUN_800142f4: @0x80014300 `sb a0,0x800afbb5` (gezeigter Cut), `jal 0x80014324`, @0x80014310 `sw v0,0x800ac794` (Anker neu) — traegt der Anker nach dem Tausch a, zeigte er vorher b: es wird der schon gezeigte Cut b zurueckgeschrieben | stimmt (deshalb `29 02` Cut_chg im Programm noetig) |
| Cut_chg LAB_800402a0: @0x800402d4 Auto aus, @0x800402fc work_vars[0x0A] = Cut, @0x80040300 Cut zeigen | @0x800402d4 `ori v0,v0,0x100`, @0x800402fc `sh a0,4068(at)` (0x800b0fe4), @0x80040300 `jal 0x800142f4` (a0 = pc[1] @0x800402c4) | stimmt |
| Init-Lauf FUN_8003ef6c @0x8003efd4/@0x8003f018, Schleife @0x8003f17c..@0x8003f188 | @0x8003efc0 `ori a0,zero,0x1` + @0x8003efd4 `jal 0x8003ee3c` / @0x8003efd8 `addu a1,zero,zero` (Slot 1, sub 0); @0x8003f018 `jal 0x8003f0a0`; @0x8003f17c `addiu s2,s2,1`, @0x8003f180 `sltiu v0,s2,0xa`, @0x8003f188 `addiu s1,s1,368` | stimmt |
| RE2 Door_exit Satz 1: @0x800141f0 `jal 0x8005ba28` / @0x800141f4 `lui a0,0x1` (info/re2leon/PSX.EXE) | genau so; Sprungziel FUN_8005ba28: @0x8005ba30 `srl t1,a0,24` (Bank 0), @0x8005ba7c `srl v0,a0,16` / `andi s7,v0,0xff` (Satz 1) | stimmt |
| RE2-Tuertabelle @0x8009A658 `38 53 fc 92 0b 00 00 00 f5 f4 00 00` = DOOR1A | Bytes selbst gelesen, identisch; Schritt 12 B ab DOOR13 @0x8009A604 (0x13+7 = 0x1A); gen/szl11b0_tuerton.inc == shared_assets/RE2/DOOR/DOOR1A.DO2[0..0x5338) (sha1 b5312b10..., selbst nachgerechnet) | stimmt |
| ROOM11B0 sub00 @0x0116C..@0x01193, ROOM11B1 sub00 @0x010FA/@0x0110E/@0x01112/@0x0112A, ROOM11C0 @0x018E4, ROOM1050 @0x00CA2 | Datei-Bytes selbst gelesen: `06 00 36 00 21 04 06 01 06 00 0c 00 23 00 0a 00 00 00 29 07 3c 01 08 00 4b 00 07 00 4b 02 08 00 37 03 0c 00 39 03 0c 40`; 11B1 `21 03 83 01 ...`, `4b 00 07 00`, `4b 02 08 00`, `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00`; 11C0 `... 3f 00 13 00 43 00 80 00`; 1050 @0x00CA2 `04 ff 18 03` (und @0x00C8A `21 03 6e 01` = Ck, wie vom Bau-Agenten als Hinweis gemeldet) | stimmt |
| Kamera "gleich" RDT @0x00A0==@0x0160, @0x0060==@0x0140, @0x00E0==@0x01A0 | Kamera-Geometrie (Zoom, Auge, Ziel) byte-gleich; die 32-Byte-Saetze unterscheiden sich NUR im Wort +0x1C (Zeiger auf die Masken-/Sprite-Daten, z.B. 2032 vs 2188) | stimmt fuer die Kamera; Formulierung "==" ungenau (Hinweis H1) |
| Clip 20: 25 Bilder, linke Hand max Bild 15 (+362/-515/+219) | `gesten_arme.py ROOM11B0.RDT 1 20`: "clip 20 (25 Bilder) ... links Hand max 667 bei b15 delta (362,-515,219)" | stimmt |
| RE2 Kendo Clip 0 == ROOM11B0 rec1 Clip 15 | RBJ selbst aus RE2 ROOM1010.RDT @0x47DC (22632 B) geschnitten, `rbj_vgl.py`: "RE2 clip 0 == 11B0 rec1 clip 15 (20 Bilder ...)"; nebenbei: RE2 clip 3 == rec1 clip 20 (Marvins Schliess-Geste ist ebenfalls eine Kendo-Geste des Gun Shops) | stimmt |
| Punkt 1/4 (unveraendert): 0x80073e30[19]/0x80076ca0[19] -> 0x80052864; @0x80052a74..a90; @0x80042614/18 | selbst: `[19] 0x80073e7c -> 0x80052864`, `[19] 0x80076cec -> 0x80052864`; `sb v0,6(v1)` (=5), `ori v0,zero,0x12` / `sb v0,148(v1)`; @0x80042614 `ori v0,zero,0x12`, @0x80042618 `sb v0,440(s0)` | stimmt |

- Rate-Woerter im Code-Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen; deferred|tunable|interim|for now|
  faithful|plausib|TODO|FIXME|vermut|approx|geschaetzt|ungefaehr|sieht richtig): **0 Treffer**. `getenv` im Engine-/
  Plattform-Diff: nur `RE15_SE_DEBUG` (audio_pc.c, Log). Kein Verhaltensschalter.
- Neue Konstanten der Nachbesserung: Cuts 2/8/0/7 (BSS + sub00 @0x01184/@0x01188), Satz 1 (RE2 @0x800141f4), Port-Bank
  0x0E (= RE15_CUT10F0_SE_BANK), Sleep 15/10/25 (Clip 20: Maximum Bild 15, Laenge 25), Rueckweg Cut 4 (BSS 4 vs 10 = 36 px
  Licht durch die offene Tuer, selbst gemessen); Clip 20 und DOOR1A als **PORT-WAHL** gekennzeichnet und begruendet
  (Header `RE15_SZL_CLIP_SCHLIESS`, tuerton_bauen.py). Commit c532feca traegt die Adressen.
- Erklaert der Fix den Befund (M1)? Ja: ohne Rueck-Tausch fuehrt das Zonen-Netz nach sub00 @0x01184/@0x01188 in die
  OFFEN-Bilder (RVD selbst gelesen: z.B. Datensatz @0x0334 `from=3 to=2` wird getauscht zu `3 -> 8`); mit Rueck-Tausch
  ist das Netz wieder das ausgelieferte (geschlossene). Gemessen: Echtweg-Folge 6-5-4-3-**2** (2.2).
- RE-Gate: **haelt.**

### 5.2 Vertrag (VERTRAG.md)
- Bank 9: nur Bit 84 geschrieben (`22 09 54 01` im Programm); gelesen (9,84) und (9,73) (erlaubt). Bit 85 unbenutzt.
- Nachrichten ROOM11B0 20/21/22 (J: 20..27); Ereignisse 22/23 nur in ROOM11B0 (`re15_szl_ereignis` prueft den Raum).
- Port-Baenke: 0x3A (SCHLOSS1010, neu) und 0x0E (RE2-Tuerbank, aus cut_10f0.c — J ist Eigentuemer von cut_10f0.c).
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, keine
  tests/*/CMakeLists.txt; neue Assets nur `shared_assets/RE2/SCHLOSS1010.EDT/.VH/.VB` (seit Abnahme 0 unveraendert,
  Paket-Gate im Dossier genannt); Tuer-zu-Ton eingebacken (`gen/szl11b0_tuerton.inc`). Neue Logik in `szenenleon_r36.c` +
  `re15_szenenleon_r36.h`. Tests: `tests/unit/probes/r36_szenenleon.cmake` (GLOB), `test_r36_szenenleon.c`,
  `tests/integration/test_r36_szenenleon.cmake`, Messwerkzeug `probe_r36_szenenleon_karte.c` (kein add_test).
  Integrations-Arbeitsordner `r36_szenenleon_wd` != Targetname.
- Gemeinsame Dateien: Nachbesserung 1 fuegt nur 1 Zeile in scd_room_setup.c (nach `scd_thread_start(1, ...)`) und 2 in
  main.c (Boot-Weg, NACH der Pre-Intro-Erkennung main.c:4790-4793) hinzu. Die Pre-Intro-Erkennung beim Raumwechsel
  (main.c:8110-8117) wertet Ereignis-Slots nur fuer 0x1170 — der belegte Slot 10 in 11B0 stoert sie nicht.
- Hinweis zum Zwei-Punkt-Diff: `git diff master --stat` zeigt 66 synchro/*.wav als geloescht; das ist master 565da62b
  (Spur L, nach der Merge-Basis 836023f5) — der Zweig selbst beruehrt synchro/ nicht (`git diff master...HEAD` ohne
  synchro/).
- Probe-Merge (`git merge-tree --write-tree r36/integration r36/szenenleon`, nur lesend, integration 99b397e7): 4
  Konflikte, alle "beide Seiten fuegen an derselben Stelle hinzu" -> beide behalten: enemy_ai_common.c Include (Spur B),
  scd_room_setup.c Include + Installer-Zeilen (Spur A/L), scd_vm.c Include (Spur L), audio_pc.c neuer Block hinter
  `re15_audio_re2_glas_se` (Spur L `re15_audio_se_play_bank` vs. J SCHLOSS1010).
- Zusammenspiel mit Spur L in ROOM1151 (statisch, integration 99b397e7 `elza_r36.c:253-266`): L legt Irons tot (state 4,
  Sub 0, Phase 2, Clip 2, Bild 89) mit `neck_flags = 0x80|0x12`; Js `irons_kopf_tick` greift auf dieselbe Pose und setzt
  0x12 (Bit 0x80 faellt weg). `re15_npc_neck_spawn_init` (game_step_common.c:937) laeuft nur bei `neck_bone == 0` und im
  Spielschritt VOR `re15_szl_tick` — im ersten Bild schuetzt Ls 0x80 die Flags, danach ist bone 8 und die Init laeuft nie
  wieder. Ergebnis 0x12, kein Tracking. Im Integrationsbaum nachzumessen (Hinweis H3).

### 5.3 Nebenwirkungen (selbst gemessen)
- **ROOM11B1 (Elza)**: `lauf.sh h_11b1 200 RE15_GOTO_ROOM=11b1 RE15_SET_FLAG=4:0xf3 RE15_FRAMEDUMP=100-300/50
  RE15_EXIT_AT=300#11b1` -> 300 Bilder cam=0, Tuerbild ZU (d_zu 0.0 gegen BSS 0), 0 Zeilen `[szl-r36]` (Spur-J-Code
  bleibt dort stumm). Riegel `unit_r36_szenenleon_tuer`: 11B1 Platz 8 = msg 12, kein Port-Programm (PASS).
- **ROOM11B0 ohne (9,84)** (vor der Szene, Original): Riegel `tuer` Gegenprobe "Zonen getauscht, gezeigt Bild 8" (PASS);
  `re15_szl_init_lauf` kehrt ohne Bit 84 sofort zurueck (Code gelesen).
- **Parkplatz-Tuer 11B0 -> 11C0**: weder Erweiterung noch Ereignis 23 enthalten ein Door_aot_set/Aot_set auf Slot 1
  (Programm-Listing `gen/szl11b0_szene.inc` gelesen).
- **Tuerbank**: die Erweiterung laedt DOOR1A in die RE2-Tuerbank (Port-Bank 0x0E); danach verlaesst die Szene den Raum
  ueber den Gully ohne Tuersequenz, jede spaetere Tuer laedt ihr eigenes Archiv; ROOM10F0 (cut_10f0.c) laedt seinen
  Tonteil beim Raumaufbau selbst — im 10F0-Lauf (Punkt 3) spielte der Abgangs-Ton mit dem 10F0-Satz (vag=1 note=61
  16397 Hz), nicht mit DOOR1A.

### 5.4 Gezielte Tests (selbst gefahren, KEINE volle Suite — Auftrag)
`ctest --test-dir re15_port/build -R "<regex>" --timeout 900 --output-on-failure`, Regex = die 157 des Bau-Agenten (Dossier
N1.5: Spur + Module scd_vm/aot/enemy_ai/anim_select/game_step/lock_se/audio/cut_10f0/irons_tod + Lade-/Boot-Riegel) plus
`unit_r34n_f_leiche_laden|unit_map_speichern_laden` (Lade-Weg) = 159 Tests (Regex-Datei `J_abn1/regex.txt`), woertlich:
```
100% tests passed, 0 tests failed out of 159
Total Test time (real) = 1373.79 sec
```
darunter integration_r36_szenenleon 161.78 s (A..E), integration_r35_cut10f0 245.47 s, integration_r35_cut1150 512.71 s,
integration_elza_vollstart 100.16 s, integration_r30_granate_laden 91.76 s, integration_r30_sicherung_laden 39.83 s,
integration_weste_load_pin 6.55 s, integration_boot_bg_pin 3.24 s. Kein Fenster-Haken musste nachgefahren werden.
- Unit-Teile zusaetzlich einzeln (Ausgaben im Notizblock `J_abn1/unit_*.txt`), je `PASS (0 Fehler)`:
  `schub` "Leon 0x11=1 0x12=1 rueck=1 Ende B1552 z -21535 -> -19999 | Marvin 0x11=1 0x12=1 Ende B1553 | Container z -20918 ->
  -19549 Flags waehrend 0x0201 Ende 0x0081 | snd0 0 -> 8"; `erweiterung` "Schliessen (Clip 20) B2191 < Tuer-zu-Ton (0x0E/1)
  B2206, Bild 2 gezeigt B2206 < Abschliessen (Clip 15) B2241 < Abschliess-Ton (0x3A/0x0A) B2261"; `tuer` "(9,84)=1, Eintritt
  Cut 8: nach dem Init-Lauf Zelle u0 0xFF fl 0x03, Zonen 1, Cut 2 (work_vars[0x0A] 2)", Ost/West "Bilder mit 7/8: 0";
  `abgang10f0` "Zeile 23 B2466 | Cut danach 2, Wechsel 0 | Marvin geparkt B2651 (vorher im Bild 0) | Ada geparkt B2676
  (vorher im Bild 0) | Tuerton B2676 (1x)"; `irons` "lebend A -615 / B -491 ... tot ... A (0,-153) / B (0,-153)".
- integration_r36_szenenleon (Arbeitsordner `build/tests/integration/r36_szenenleon_wd/lauf_8e996dc8_*`): A2 debug.log
  F2378 Schliessen < F2393 Tuer-zu-Ton < F2428 Abschliessen < F2448 Abschliess-Ton; state.log B 731 / C 371 / D 151 Zeilen,
  davon mit cam 8/7: 0 / 0 / 0.
- Riegel je Punkt vorhanden und messend (Assertions gelesen): Punkt 1 `schub` + integration A; Punkt 2 `programm`
  (Bytefolge gegen die RDT-Datei), `erweiterung` (Bildnummern, Zonen-Zustand gegen die ausgelieferte RVD, Kamerafolge
  6-10-8-2-4-6), `tuer` (Init-Lauf, Ost/West, Gegenprobe ohne Bit 84, 11B1), integration A/A2/B/C/D/E (E = Gegenprobe:
  ohne Bit 84 sieht die Messung Cut 8); Punkt 3 `abgang10f0`; Punkt 4 `irons` (lebend-Gegenprobe).

## 6. Maengel der Abnahme 0 — Stand

| Mangel (Abnahme 0) | Stand | Beleg hier |
|---|---|---|
| M1 Tuer bleibt im Bild offen | behoben | 2.1 (ab F2392 ZU), 2.2 (Echtweg-Rueckkehr, Folge 6-5-4-3-2, 0 Bilder 7/8), 2.3 (Ost/West je 421 Bilder ZU), 2.4 (CONTINUE) |
| M2 Nachtrag: erst schliessen, dann abschliessen | behoben | 2.1 Bildnummern F2376 < F2391 < F2426 < F2446 + Framedump-Serie; 2.2 zweiter Lauf F2380 < F2395 < F2430 < F2450 |
| M3 Falschaussage im Dossier | behoben | J_szenenleon.md §2.3 (Z.288), OFFEN 1 (Z.318), "Fuer den Nutzer" (Z.345) durchgestrichen + berichtigt; im Code keine Spur der Behauptung (git grep) |
| M4 kein Riegel fuer das Tuerbild | behoben | 5.4 (Assertions gelesen, Tests gruen, Gegenprobe E vorhanden) |

## 7. Ergebnis

| Punkt | Urteil | Kern |
|---|---|---|
| 1 Container-Schieben 11B0 | erfuellt | Echtweg 11C0->11B0: Leon + Marvin Sub 0x13 synchron (0x11/0x12/0x11-rueck), +1536/+1517, Container wandert, Gully frei |
| 2 Erweiterung Werkstatt-Tuer | erfuellt | Hold on -> Lauf -> Schliessen (Clip 20, Bild 8->2, DOOR1A-Tuer-zu-Ton) F2376<F2391 -> Abschliessen (Kendo Clip 15, SCHLOSS1010) F2426<F2446 -> Tuer zu (jedes Bild, Echtweg-Rueckkehr, Ost/West, CONTINUE) mit Elzas Text+Ton -> Rueckweg -> "I just want to make sure..." |
| 3 10F0-Abgang | erfuellt | Echtweg 10D0->10F0: Cut 2 ab F911 bis Ende, Parken F2670/F2687 bei 0 geaenderten Pixeln, ein Tuer-zu-Ton F2687 |
| 4 Irons-Kopf nach dem Tod | erfuellt | Echtweg 1130->1150: fl 0x00 -> 0x12 F894, Akku 424 Bilder konstant; tot A/B 0 px, lebend A/B 165 px am Kopf |

Gates: RE-Gate haelt (11 Stichproben selbst disassembliert/gelesen, 0 Rate-Woerter, Fix erklaert den Befund), Vertrag
haelt, gezielte Tests 159/159 gruen. **Keine offenen Maengel.**

Hinweise (kein Mangel, fuer Orchestrator/Nutzer):
- H1 Dossier N1.1/Commit c532feca schreiben "Kamera RDT @0x00A0 == @0x0160" (und 0x0060/0x0140, 0x00E0/0x01A0): die
  Kamera-Geometrie ist byte-gleich, die 32-Byte-Saetze unterscheiden sich aber im Wort +0x1C (Masken-/Sprite-Zeiger, z.B.
  2032 vs 2188). Aussage "gleiche Kamera, anderes Bild" stimmt; "==" ist ungenau.
- H2 Probe-Merge in r36/integration (99b397e7): 4 Konflikte, alle trivial (beide Seiten behalten) — 5.2.
- H3 ROOM1151: J (`irons_kopf_tick`) und L (`re15_elza_r36_install`, neck 0x80|0x12) wirken auf denselben toten Irons;
  statisch vertraeglich (Endstand 0x12, kein Tracking) — im Integrationsbaum messen (neck.log ROOM1151, zwei Standpunkte).
- H4 Grenzen, im Dossier als OFFEN mit Adresse + Messweg: Tuerblatt springt (nur Hintergrundbild, OFFEN 5), Kendos
  Nachwippen ohne Plc_cnt/Plc_stop (OFFEN 2, PORT-WAHL Sleep 35), Schloss-Ton nicht positional (OFFEN 3), Elza-Zeitleiste
  (OFFEN 4, Spur P).
- H5 Paket-Gate (nur Orchestrator): `shared_assets/RE2/SCHLOSS1010.EDT/.VH/.VB` muessen ins Paket (selbst geprueft: byte-
  gleich RE2 ROOM1010.RDT @0xA044/@0xA104/@0xAF24); der Tuer-zu-Ton ist eingebacken (`gen/szl11b0_tuerton.inc`).
- H6 integration_r36_szenenleon D/E (CONTINUE) prueft einen Spielstand IN ROOM11B0, den es im Spiel nicht gibt (keine
  Schreibmaschine in 11B0) — Robustheit des Boot-Wegs, kein Spielweg.
- H7 Zwei-Punkt-Diff gegen master zeigt 66 synchro/*.wav als geloescht (master 565da62b nach der Merge-Basis); beim
  Zusammenfuehren keine Rolle (Drei-Punkt-Diff beruehrt synchro/ nicht), aber bei einem `git diff master`-Review nicht
  verwechseln.
- H8 Bestaetigt (Hinweis des Bau-Agenten, nicht Spur J): ROOM1050 @0x00C8A ist `21 03 6e 01` (Ck), das Evt_exec steht
  @0x00CA2 `04 ff 18 03` — der Kommentar der Spur K in scd_room_setup.c zitiert die falsche Stelle.

Belegbilder (Notizblock `J_abn1/`, nicht im Repo): `bg_tuer_2_8_0_7.png`, `bg_tuer_0_7.png`, `a_schliessen_1.png`,
`a_schliessen_2.png`, `a_frames_2316_2360.png`, `bc_tuer_text_F370.png`, `r_weg_sheet.png`, `p1_schub_sheet.png`,
`p3_abgang_sheet.png`, `p4_irons_AB.png`; Mess-Ausgaben `a_tuerbild.txt`, `b_tuerbild.txt`, `c_tuerbild.txt`,
`d_tuerbild.txt`, `r_tuerbild.txt`, `ctest_gezielt.log`.
