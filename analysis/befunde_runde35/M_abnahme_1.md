# Runde 35 Spur M (cut11c0_fenster) — Abnahme 1 (unabhaengig), 2026-10-04

Gegenstand: Zweig `r35/cut11c0_fenster` @7424b475 (Basis 154a73c1). Seit Abnahme 0 (16a396f7) kamen 6 Commits
hinzu, die Nachbesserung 1: 72026be6, c6263a47, a8fa3f36, 1401c58c, 2676c6cb und 7424b475.
Massstab: AUFTRAG.md Z. 93 (Wortlaut), VERTRAG.md, Dossier `M_cut11c0_fenster.md` und Abnahme 0 (`M_abnahme_0.md`, Mangel 1).
Alles unten habe ich selbst am gebauten Stand gemessen. Behauptungen des Bau-Agenten habe ich nur uebernommen, wo es
ausdruecklich dabei steht.

**Ergebnis: BESTANDEN.** Der Nutzer-Punkt ist erfuellt. Gemessen habe ich ueber den echten Tuerweg 1130 -> 1120
mit Standard-Renderer. Das Ergebnis: Kette im Log, Bildfolge, Ton per deterministischer Aufnahme, Gegenprobe mit
geschlossenem Tor, Wiedereintritt. Mangel 1 aus Abnahme 0 ist behoben. Jede neue Belegstelle habe ich selbst
disassembliert oder in den Dateibytes nachgesehen, den spielweiten Zensus 287/287 habe ich selbst nachgerechnet.
Suite (`=== LOCAL-BUILD-OK (test) — Tests 484/484`, eigener Lauf), Pfad-Gate und Tests (6/6 `r35_fenster`) halten.

---

## 0. Bau und Suite (selbst gefahren)

| Schritt | Befehl | Ergebnis |
|---|---|---|
| Configure | `cd <baum> && bash re15_port/tools/local_build.sh configure` | Toolchain OK (cc1 EXIT=0, Mini-Compile OK), `=== LOCAL-BUILD-OK (configure)` |
| Build | `bash re15_port/tools/local_build.sh build` | `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. Die exe entspricht HEAD (md5 `d2916d517f698c22f239a2f8887e9ec8`) |
| Suite | `bash re15_port/tools/local_build.sh test` (eigener Lauf, 1159,06 s) | **`100% tests passed, 0 tests failed out of 484`** / **`=== LOCAL-BUILD-OK (test) — Tests 484/484`**. Kein Fenster-Haken war rot (boot_bg/dark_start/relatch/save_counter/weste_load gruen), deshalb musste ich keinen nachfahren |
| Spur-Tests | `ctest --test-dir re15_port/build -R "r35_fenster" -V` (einzeln nach der Suite) | 6/6 gruen. glas: `Landung Bild 27 (erwartet 27) ... Op5 26 Op16 1 Op39 1 Op84 1 unbekannt 0`; kraehe: `Durchflug 7 Bilder, z 12100 -> 10217 (1883), Zustand 1/4`; knall: `Satz 0x21: 4 Lagen: Ton 7..10 VAG 4 (vol 127 mitte 80/80/73/80)`; satzform: `ROOM1050.RDT @0x0c22: ... p0 0x00ff p1 0x0218`, `ROOM1020.RDT @0x1e18: ... sat 0x41 p0 0x00ff p1 0x0318`, `Ereignis-Faeden: 10 dann 11`; integration (26,96 s): A Bild 180 Loch 73 / Kante 2156 (vorher 125/195), B Loch 11 / Kante 2156, `r35_fenster: OK` |
| Spur-Tests (Muster der Aufgabe) | `ctest --test-dir re15_port/build -R "r35_cut11c0_fenster"` | `No tests were found!!!`. Die Tests heissen `*_r35_fenster*` (s. §5), das ist kein Mangel |

Im Dossier steht woertlich `=== LOCAL-BUILD-OK (all) — Tests 484/484` (Z. 348, N = 484 >= 478).

## 1. Punkt des Nutzers (Wortlaut AUFTRAG.md Z. 93)

> "Wenn Leon dann in ROOM 1120 Cut1 Richtung dem Fenster hinten zulaeuft, sollen die Scheiben zerbrechen - so wie
> in Resident Evil 2 - und eine Kraehe "rein fliegen". Die Glas Zersplitter Effekt musst du aus Resident Evil 2
> extrahieren, sowie der Knall Sound. Es waere super, wenn du dann auch im Background das Hintere Fenster etwas
> "beschaedigen" koenntest."

### 1.1 Messaufbau: ECHTER Weg durch die Tuer

Fuer die Messlaeufe nutzte ich eine eigene exe-Kopie `re15_port/build/platform/pc/re15_pc_abn_m1.exe` (md5 wie oben) und
das Lauf-Skript `scratchpad/M_abn1/lauf.sh`. Den Integrations-Spielstand habe ich nicht verwendet, ebenso wenig
RE15_SOFTWARE_RENDER.
```
RE15_NO_INTRO=1 RE15_SET_FLAG=9:73,3:94      # Stand nach der Irons-Todesszene (Spur L, in master RE15_IT_BIT_GESEHEN 73);
                                             # (3,94) oeffnet die Tuer 1130->1120
RE15_DEBUG_JUMP=1130@240 RE15_PLAYER_POS=-3050,-2150,2048          # vor Tuer-Slot 1 von ROOM1130
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
RE15_INPUT_SCRIPT=A0.1,W5,XU1.2,L0.3667,XU0.4,R0.3667,XU0.97,L0.3667,XU0.5,U3,W2
RE15_STATE_LOG / RE15_FENSTER_LOG / RE15_FRAMEDUMP=560-800/4:bild_ / RE15_EXIT_AT=820
```
Lauf r1 (`scratchpad/M_abn1/r1_tuerweg/`), debug.log:
```
[glas1120] GLAS1090.ESP 3092 B -> re2fx_register_raum rc=0, Texturen ok (Slot 55)
[setflag] flag(9,73/0x49) = 1   [setflag] flag(3,94/0x5e) = 1
[debug-menu] JUMP -> 113 3F CORRIDOR    (ROOM1130) spawn=(-3050,0,-2150) cut=0
[aot] DOOR FIRE slot=1 rect=(-3050,-2150,hw=500,hh=1000) target_cut=3 spawn=(-7600,0,-2900)
[tuer] Sequenz fertig: 301 Bilder + 1 Warten ...
[room] PC loaded room1120.rdt (86900 bytes)
[enemy] RE2 EM021 loaded: 13 meshes, 13 bones, 12 clips -> slot 11
```
fenster.log:
```
[fenster] ROOM1120 scharf: Slot 4 Band x 3500..6650 z 4300..6400 Ereignis 24
[fenster] Kraehe Slot 4 versteckt bei (5400,-2500,12100)
[fenster] Ausloeser: Spieler (4319,4373) im Band, Ereignis 24 0
[fenster] Splitter Bank 0x10 Platz 95 Lage x 4700 z 10200   ... 13 Zeilen, Plaetze 95..83, Banken 0x10..0x13
[fenster] Knall 1: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+5 0
[fenster] Knall 2: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+10 0
[fenster] Zeitlinie fertig T+23 Splitter 13 Knalle 2 0
```
state.log zeigt den Spieler bei F600 (4463,982) cam=2, dann geht er nach Norden (rot -1056). F630 (4331,4077) cam=1,
F634 (4319,4373) cam=1. Das ist das Ausloese-Bild: Leon steht im Band und laeuft auf die Rueckwand mit dem Fenster
(z 11200) zu.

### 1.2 Teilaspekte, jeweils gemessen

| Teil | Messung | Urteil |
|---|---|---|
| **Ausloeser "in Cut 1 Richtung Fenster hinten zulaeuft"** | Ausgeloest in F634, cam=1, rot -1056 (Norden), z steigt je Bild um 74. Ohne (9,73) habe ich als Gegenprobe denselben Tuerweg gefahren (Lauf r3, `RE15_SET_FLAG=3:94`): `[fenster] ROOM1120 aus: (9,73)=0 (9,79)=0`. Der Spieler kreuzt das Band in F634/F660 (z 4373/6297), es gibt keinen Ausloeser und keine vierte Kraehe im state.log. Das Fenster bleibt heil, Loch 125 / Kante 195 konstant in F620/F660/F700 | erfuellt |
| **Scheiben zerbrechen wie RE2, Glas-Effekt aus RE2 extrahiert** | `cmp`: GLAS1090.ESP == `info/re2leon/PL0/RDT/room1090/effect.esp`, GLAS1090_10..14.TIM == esp10..14.tim. `glas1090_extrakt.py --pruefen` findet alle Dateien in ROOM1090.RDT (u. a. esp12 @0x2F208, esp13 @0x31248, esp14 @0x32688). Die 13 Splitter-Saetze in `fenster_1120.c` habe ich gegen die 27 `3a`-Saetze in sub15.scd @0x0054..0x01F4 gerechnet: 13 haben x in [-2800,-2200], alle 13 sind feldgleich (Bank/Sub/Skala/x/y/z), die Gier ist bei allen 27 0x0C00. Bildfolge (`scratchpad/M_abn1/r1_zoom.png`): F620..F632 Fenster heil, Leon laeuft darauf zu. F636 die Kraehe hinter der Scheibe. F640 Splitter an den Scheiben. F644..F672 fliegen die Splitter zur Kamera, fallen vor Leon auf den Boden und verschwinden. Die Splitter-Formen und -Farben sind die der RE2-TIMs: Bruchstueck-Umrisse, CLUT hellgrau-blau 0xef59.., mit TPage-OR 0x20 additiv gemischt (`scratchpad/M_abn1/tims.png`) | erfuellt |
| **Knall aus RE2 extrahiert** | `cmp`: GLAS1090.EDT/.VH/.VB == snd0.edt/.vh/.vb; `--pruefen` findet sie in ROOM1090.RDT @0x075FC/@0x076BC/@0x084DC. Mein Rechner hat kein Audiogeraet (`[audio] SDL_OpenAudioDevice failed: WASAPI ...` in r1), deshalb habe ich den Ton mit `RE15_AUDIO_CAP_SYNC` aufgenommen (Lauf r2, gleicher Tuerweg, EXIT_AT 720). debug.log zeigt zweimal 4 Lagen `[se] Stimme: se=33 layer=0..3 vag=4 note=67 ... vol=127` (Mitte 80/80/73/80). Den Effektivwert habe ich je Takt (1470 Stereo-Bilder) berechnet: Takt 1380 = 1000, Takt 1381 = 8136, Spitze 32439/32459. Bei Takt 1386 faellt er auf 8128 und steigt bei 1387 wieder auf 10065, das ist der zweite Einsatz 5 Bilder spaeter. Danach klingt er bis Takt 1421 auf ~1300 ab. Takt 1460 = F720, also ist Takt 1381 = F641 und liegt damit auf T+5..7 nach dem Ausloeser F634 | erfuellt |
| **eine Kraehe "rein fliegen"** | state.log Slot 4 (Typ 0x21): Von F600 bis F634 State 4/2/1 bei (5400,12100), das ist hinter der Rueckwand z 11200. F635 ss2=2, Clip 4. In den 7 Bewegungsbildern F636..F642 sinkt z: 11801, 11512, 11233, 10964, 10705, 10456, 10217 (zusammen 1883). Die Wandebene z 11200 kreuzt die Kraehe zwischen F638 und F639. F642 State 1 Sub 4, F643 Sub 13 (Anflug). F680 greift sie an, Leon hat hp 100 -> 95, bei F760 75. Im Bild ist die schwarze Kraehe ab F636 an der Scheibe und greift ab F680 Leon an | erfuellt |
| **hinteres Fenster im Hintergrund beschaedigt** | Pixelsummen an den Loch-/Kantenpunkten des Integrationshakens, gemessen in meinen Framedumps: vorher (F620/F632) Loch 125 / Kante 195, nachher F700 11/1615, F760 11/2156, F800 27/2156. Im Bild sind beide Scheiben gezackt offen und dunkel, die Bruchkanten hell, darunter Scherben-Tupfen am Wandfuss. **Wiedereintritt** (Lauf r4, gleicher Tuerweg, `RE15_SET_FLAG=9:73,3:94,9:79`): `[fenster] ROOM1120 aus: (9,73)=1 (9,79)=1`, keine vierte Kraehe, keine Splitter. Loch 11 / Kante 2156 schon ab F620 (`scratchpad/M_abn1/r3_r4_zoom.png`). Die Kunst ist als PORT-WAHL gekennzeichnet. `tools/r35_fenster/schaden_bauen.py` erzeugt `gen/fenster1120_schaden.inc` reproduzierbar: Kopie mit umgebogenem OUT, `cmp` gleich, 309 Operationen | erfuellt |

**Urteil Punkt 1: erfuellt.**

## 2. RE-Gate

### 2.1 Mangel 1 aus Abnahme 0: Behebung nachgemessen

`git diff 16a396f7 HEAD -- re15_port` enthaelt nur Kommentare in `re15_fenster1120.h:49-65` und `re2_fx.c:1117` sowie
den neuen Riegel (`test_r35_cut11c0_fenster.c` +59, `probes/r35_cut11c0_fenster.cmake` +2/-1). Die Werte 0x00FF und
0x18 sind unveraendert. Weil ninja nichts neu baut, ist die gemessene exe dieselbe wie vor der Nachbesserung.
Jede Belegstelle habe ich selbst nachgesehen:

| Beleg im Code / Commit 7424b475 | Meine Messung | Befund |
|---|---|---|
| ROOM1050.RDT @0x0C22 `2c 07 03 31 ... ff 00 18 02 00 00` (p0 @0x0C30, p1 @0x0C32) | `xxd -s 0x0C22 -l 20`: `2c07 0331 0000 a041 0add 2003 2003 ff00 / 1802 0000`. `cmp` gegen info/Re1.5/PSX/STAGE1/ROOM1050.RDT ergibt gleich | stimmt |
| ROOM1020.RDT @0x1E18 `2c 06 03 41 ... ff 00 18 03 00 00` | `xxd`: `2c06 0341 0000 3abc 2496 b80b 3043 ff00 / 1803 0000` | stimmt |
| RE2 room1090 sub03 @0x0022 `ff 00 18 0f` | `xxd sub03.scd`: @0x0014 `2c 06 05 41 00 00 b9 df 2d c3 34 08 7c 15 ff 00 18 0f 00 00` | stimmt |
| Zensus: 287 sce-3-Saetze in 124 Raeumen, alle p0 0x00FF / p1-Unterbyte 0x18, 100 % Abdeckung, 0 Stopps | Selbst gerechnet (`scratchpad/M_abn1/census_sce3.py`, importiert `re15_port/tools/aot_sce_census.py`, ROOT = Baum): `files=240`, `coverage: walked 214985 of 214985 code bytes = 100.00%`, `total stops: 0`, `sce3 287 rooms 124`, Formen `(0x2c,0x31) 153 / (0x46,0x31) 67 / (0x2c,0x41) 54 / (0x2c,0xb1) 3 / (0x46,0x41) 9 / (0x2c,0xc1) 1`, **abweichend p0/p1lo: 0** | stimmt |
| `lhu a0,0(v0)` @0x800430fc, `lbu a1,3(v0)` @0x80043100, `jal 0x8003ee3c` @0x80043104; Tabelle @0x8007469c[3] = 0x800430f0 | re15_disasm.py PSX.EXE: genau diese drei Instruktionen. `read 0x8007469c 6`: [3] = 2147758320 = 0x800430F0 | stimmt |
| 0x8003ee3c: `sltiu v0,a2,0xa` @0x8003ee54; a0 >= 10: Platz 2 @0x8003ee70, sonst Suche 3..9 @0x8003ee74-a0 | @0x8003ee54 `sltiu v0,a2,0xa`, @0x8003ee58 `bne v0,zero,0x8003eef8` (a0 < 10 = fester Platz). @0x8003ee64 `lbu v0,11821(v0)` = 0x800b2e2d = 0x800b2b4d + 2*0x170, @0x8003ee6c `beq v0,zero` mit Delay-Slot `ori a2,zero,0x2` @0x8003ee70. @0x8003ee74 `ori a0,zero,0x9`, Schleife @0x8003ee7c-a0 ueber 0x800b2b4d + Platz*0x170; der Delay-Slot `addiu a2,a2,1` liefert den gefundenen Platz, Platz 9 ist der Rueckfall. Ich habe also nachgerechnet: p0 >= 10 waehlt den ersten freien Faden 2..9 | stimmt |
| Start 0x8003edec @0x8003ef54 | @0x8003ef50 `addiu v0,v0,11084` (0x800b2b4c), @0x8003ef54 `jal 0x8003edec` | stimmt |
| Aot_reset schreibt +2 nur (`sh v0,2(v1)` @0x8004079c) | LAB_80040738: @0x80040790 `sh v0,0(v1)`, @0x8004079c `sh v0,2(v1)`, @0x800407a8 `sh v0,4(v1)`, v1 = Eintrag+12 bzw. +20 (@0x80040774-84) | stimmt |
| Aot_set legt nur den Zeiger ab (@0x80040584 / @0x800406d0) | @0x80040580 `addiu v0,v0,2` / @0x80040584 `sw v0,0(v1)`; @0x800406cc `addiu v0,a2,2` / @0x800406d0 `sw v0,0(s0)` | stimmt |
| re2_fx.c `sltiu v0,t0,0x8` @0x8001bd24 / `bne` @0x8001bd28 (RE2) | re2_disasm.py RE2 PSX.EXE: @0x8001bd24 `sltiu v0,t0,0x8`, @0x8001bd28 `bne v0,zero,0x8001bcc8`. Dazu gegengeprueft: Raum-Registry-Basis 8. Ich habe nach `jal 0x8001bca0` gesucht, es gibt 2 Aufrufer: @0x8001bb8c mit Delay `addu a3,zero,zero` (Kern, Basis 0) und @0x8001bc6c mit Delay `addiu a3,zero,8` (Raum, Basis 8) | stimmt |

Die Commit-Message 7424b475 nennt alle Adressen und Byte-Offsets aus der Tabelle. Der Riegel
`unit_r35_fenster_satzform` liest die beiden RDT-Saetze aus `shared_assets/PSX` und vergleicht sie mit
RE15_FENSTER_P0/P1/SAT. Er misst also gegen die ausgelieferten Bytes und nicht gegen eine Kopie der Konstante.
Die Mechanik-Haelfte (Faeden 10, dann 11) prueft das vorhandene scd_event_fire. Das reicht als Beleg dafuer, dass der
Port fuer p0 >= 10 den ersten freien Faden nimmt.

### 2.2 Weitere Stichproben, selbst disassembliert

| Behauptung | Datei / Adresse | Befund |
|---|---|---|
| Kraehe State 4 Sub 2 0x801037f8 (RE2 EMOVL21_S0.BIN) | P0 @0x80103830 `beq v1,zero` / Delay `lui a0,0x8`, @0x80103854 `ori a0,a0,0x8` (word0 \|= 0x80008), @0x8010386c `ori v1,v1,0x4000`. Ohne Sprung geht es weiter in P1 @0x80103878 (`andi v0,v0,0x4` @0x80103880). P1: Anim 0x00070004 @0x80103888-94, `300` @0x80103898, `6` @0x801038a0, `sh zero,468/326` @0x801038bc/c0, Maske 0xfff7ffff @0x80103890/b8/c4 (loescht nur 0x80000, Bit 8 bleibt). P2: `jal 0x80015350` @0x801038dc, `-10` @0x80103900, `bne v1,zero` auf den Alt-Wert @0x8010390c, `-9` @0x80103924, `jal 0x80104078(e,1,4)` @0x8010392c. Das ergibt 7 Bewegungsbilder, passend zu meinem state.log (F636..F642) | stimmt, Port `re2c_perch2` identisch |
| Kill-Flag 0xFF schaltet das Gate aus (@0x80042128) | RE1.5 @0x80042120 `lbu a1,7(a1)`, @0x80042124 `ori v0,zero,0xff`, @0x80042128 `beq a1,v0,0x8004215c` (ueberspringt `jal 0x8004efe4`) | stimmt |
| Spawn-Satzform wie ROOM1120 main00 @0x00D0C | `xxd`: `4400 2100 0000 00a7 8214 0000 521c 0000 000c 0000` | stimmt (Form; Lage/Slot/Kill-Flag sind Abbildung bzw. PORT-WAHL, im Header so gekennzeichnet) |
| Zeitlinie / Rumble / Knall-Saetze aus sub15 | `sub15.scd` (588 B): @0x0000 `22 04 31 01`, @0x0004 `46 06 00 ..`, @0x0012 `34 17 04 00`, @0x002E `09 0a 02 00`, @0x003A `8a 00 03 00 03 00`, @0x0040 `8a 00 03 00 07 00`, @0x0046 `8b fa 08 00 00 00`, @0x0204 `09 0a 03 00`, @0x0208/@0x0218 `36 02 21 01 00 00 9c f0 00 00 50 c9`, @0x0214 `09 0a 05 00`, @0x0232 `09 0a 0d 00`, @0x024A `01 00` | stimmt mit T+2/5/10/23 und den Rumble-Argumenten |

### 2.3 Wortsuche und Env-Schalter
`git diff master...HEAD -- re15_port | grep '^+'` habe ich nach deferred/tunable/interim/for now/faithful/plausib/
TODO/FIXME/approx/vorerst/vorlaeufig/Platzhalter durchsucht: **0 Treffer**. Neue getenv: nur `RE15_FENSTER_LOG`
(Messschiene, kein Verhalten). Flag-Schreibzugriffe ausserhalb der Tests: nur das Port-Programm `22 09 4f 01` = (9,79).

### 2.4 Erklaert der Fix den Befund?
Der Befund aus Abnahme 0 war eine Belegluecke, kein Verhaltensfehler. Die Nachbesserung schliesst genau diese Luecke:
Kommentar, Commit-Message und ein Riegel gegen die Dateibytes. Werte und exe bleiben unveraendert. Dass sich nichts
geaendert hat, bestaetigen ninja (`no work to do`) und meine Laeufe r1..r4, die Abnahme 0 bildgleich wiederholen
(Ausloeser (4319,4373), Splitter-Plaetze 95..83, Kraehenbahn).

## 3. Vertrag

| Pruefung | Befund |
|---|---|
| Bank-9-Bits | geschrieben nur (9,79), gelesen (9,73), wie VERTRAG §1.1 |
| Nachrichten-IDs | keine belegt |
| Ereignis / AOT-Slot | Ereignis 24 (VERTRAG §1.3); Slot 4 mit Zensus im Dossier §1 |
| Gegner-Slot / Texturplatz | Gegner-Slot 3 (Aktor 4) in ROOM1120. Rohsuche nach Kraehen-Saetzen `44 nn 21`: nur 10C0/1120 (0..2) und 1170 (5/6), keine fremde Belegung. Texturplatz 55: in master 87cc8575 laut render_pc.c:209 frei. Eine Suche in allen r35/*-Zweigen nach `TIM_SLOT 53..55` ergibt keinen weiteren Nutzer |
| Pfade | `git diff master...HEAD --name-only`: 30 Dateien. Treffer auf `release/`, `platform/android/`, `shared_assets/PSX/`, `tests/unit/CMakeLists.txt`, `tests/integration/CMakeLists.txt`: **0**. Neue Assets nur unter `shared_assets/RE2/GLAS1090.*`, fuer das Paket-/Android-Gate im Dossier genannt |
| Gemeinsame Dateien | enemy_ai_common.c +4/-1, game_step_common.c +2, scd_room_setup.c +2, scd_vm.c +2, main.c +13 (5 Haken), jeweils mit dem Kommentar `Runde 35 Spur M`. re2_fx.c +125 / enemy_ai_re2_crow.c +116 sind Fachmodule (ESP-Ops und Kraehen-State 4 brauchen die statischen Helfer dort) |
| Tests | 5 Unit-Riegel + 1 exe-Haken (eigener exe-Name `re15_pc_r35m_haken_*`, Muster r34n_b_cursor), alle messend: Landebild gegen unabhaengige Rechnung, Kraehenbahn 1883, EDT-Satz 0x21, Tor/Raum/VM/AOT/Zeitlinie, Satzform gegen die RDT-Bytes, exe-Log plus Pixel vor/nach plus Wiedereintritt. Ergebnis siehe §0 |

## 4. Maengel

**Keine.** Mangel 1 aus Abnahme 0 (@0x-Gate RE15_FENSTER_P0 / p1-Unterbyte 0x18) ist behoben, Nachweis in §2.1.
Die Kleinigkeit aus Abnahme 0 §2.4 (`sltiu` ohne eigene Adresse in re2_fx.c) ist ebenfalls erledigt: @0x8001bd24 /
@0x8001bd28 habe ich selbst nachgesehen.

## 5. Hinweise (keine Maengel dieser Spur)

* **Kein Richtungstor.** Lauf r5 (`RE15_DEBUG_JUMP=1120@240`, `RE15_PLAYER_POS=5000,8500,1024`, nach Sueden ins
  Band): Auch beim Weglaufen vom Fenster loest das Ereignis aus (`Ausloeser: Spieler (5003,6312)`). Das ist die belegte
  RE2-Form: sat 0x41 ohne Richtungstest. Den AUTO-Pass habe ich selbst nachgesehen: RE1.5 @0x80042c98 `lbu v0,1(s0)`,
  @0x80042ca0 `andi v0,v0,0x10`, @0x80042ca4 `bne v0,s6`, also nur der Pass-Typ. Auf dem Story-Weg
  1150 -> 1130 -> Tuer 2 -> Cut 2/1 wird das Band zuerst nach Norden gekreuzt. ROOM1170 hat laut Tuer-Saetzen
  Ausgaenge u. a. nach 10B0/1130/1140. Ob man ueber diesen Umweg 1120 auslassen und spaeter von Tuer 0 kommend nach
  Sueden ausloesen kann, habe ich nicht bis zum Ende verfolgt. Ein Richtungstor waere eine Port-Erfindung ohne Beleg. (Lauf r5 hatte zusaetzlich ein
  Sprung-Artefakt: cam blieb 0, und die Geschwindigkeit betrug 200/Bild. Fuer die Frage "loest es aus" spielt das keine
  Rolle.)
* Das Aufgabenmuster `ctest -R "r35_cut11c0_fenster"` trifft 0 Tests, die Tests heissen `*_r35_fenster*`
  (steht auch im Dossier).
* Der Probe-Merge gegen master 87cc8575 (`git merge-tree --write-tree --name-only master HEAD`) meldet weiterhin
  Konflikte in `re2_fx.c`, `scd_room_setup.c` und `scd_vm.c`. Wie in Abnahme 0 fuegen die Konflikte nur hinzu,
  beide Seiten bleiben erhalten.
* (9,73) setzt Spur L. In master ist das schon so: `re15_irons_tod.h:57 #define RE15_IT_BIT_GESEHEN 73`. Allein auf
  diesem Zweig gemessen habe ich mit RE15_SET_FLAG.
* Auf Rechnern ohne Audiogeraet ist der Knall stumm (kein SDL-Geraet, `g_audio.initialized` 0). Gemessen habe ich
  ihn deshalb ueber RE15_AUDIO_CAP_SYNC, also durch dieselbe Mix-Kette.
* OFFEN laut Dossier, unveraendert: RE2-Kraehe Sub 0/1, Zielhilfe auf die versteckte Kraehe, Rumble-Weitergabe,
  PSX-Zeichner.

## 6. Messartefakte (Scratchpad, nicht eingecheckt)
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/M_abn1/`:
r1_tuerweg (Log, state.log, Framedumps F560..F800), r2_ton (cap.raw, SE-Debug), r3_tor_zu (Gegenprobe ohne (9,73)),
r4_wiedereintritt ((9,79)=1), r5_sueden (Richtung), `r1_zoom.png`, `r1_voll.png`, `r3_r4_zoom.png`, `tims.png`,
`census_sce3.py` + `census.json`, `schaden_neu.inc`, `ctest_r35_fenster.log`; Suite-Protokoll `scratchpad/m1_test.log`.
Die exe-Kopie `re15_pc_abn_m1.exe` habe ich nach den Laeufen wieder geloescht.
