# Runde 35 Spur M (cut11c0_fenster) — Abnahme 0 (unabhaengig), 2026-10-04

Gegenstand: Zweig `r35/cut11c0_fenster` @1e958236 (Basis 154a73c1), 12 Commits e88b2a32..1e958236.
Massstab: AUFTRAG.md Z. 93 (Wortlaut), VERTRAG.md, Dossier `M_cut11c0_fenster.md`.
Alles unten habe ich selbst gemessen; Behauptungen des Bau-Agenten sind nur dort uebernommen, wo es ausdruecklich
dabei steht.

**Ergebnis: NICHT bestanden.** Der Nutzer-Punkt ist erfuellt (echter Tuerweg 1130 -> 1120, ganze Kette gemessen).
Suite, Pfad-Gate und Tests halten. Durchgefallen ist das @0x-Gate an genau einer Stelle (Mangel 1): zwei
verhaltensrelevante Konstanten stehen ohne Beleg im Code. Ihre Werte habe ich selbst geprueft, sie sind richtig.
Die Korrektur ist eine Kommentarzeile.

---

## 0. Bau und Suite (selbst gefahren)

| Schritt | Befehl | Ergebnis |
|---|---|---|
| Configure | `cd <baum> && bash re15_port/tools/local_build.sh configure` | Toolchain OK, cc1 EXIT=0 |
| Build | `bash re15_port/tools/local_build.sh build` | `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`: die exe entspricht HEAD; die letzten 3 Commits aendern nur das Dossier |
| Suite | `bash re15_port/tools/local_build.sh test` (eigener Lauf, 1145,6 s) | **`=== LOCAL-BUILD-OK (test) — Tests 483/483`**, 0 Failed. Kein Fenster-Haken war rot, deshalb musste ich keinen nachfahren |
| Spur-Tests | `ctest --test-dir re15_port/build -R "r35_fenster" -V` | 5/5 gruen (unit_r35_fenster_glas/_kraehe/_ereignis/_knall, integration_r35_fenster 25,97 s) |

Im Dossier steht zusaetzlich die woertliche Zeile `=== LOCAL-BUILD-OK (all) — Tests 483/483` (N = 483 >= 478).

## 1. Punkt des Nutzers (Wortlaut AUFTRAG.md Z. 93)

> "Wenn Leon dann in ROOM 1120 Cut1 Richtung dem Fenster hinten zulaeuft, sollen die Scheiben zerbrechen - so wie
> in Resident Evil 2 - und eine Kraehe "rein fliegen". Die Glas Zersplitter Effekt musst du aus Resident Evil 2
> extrahieren, sowie der Knall Sound. Es waere super, wenn du dann auch im Background das Hintere Fenster etwas
> "beschaedigen" koenntest."

### 1.1 Messaufbau: ECHTER Weg durch die Tuer, nicht der Integrations-Spielstand

Der Integrationshaken der Spur startet per Speicherstand direkt in ROOM1120 Cut 1, mit RE15_SOFTWARE_RENDER.
Deshalb habe ich selbst den Weg durch die Tuer gefahren, mit Standard-Renderer und Ton an. Die exe-Kopie lief unter
eigenem Namen: `re15_port/build/platform/pc/re15_pc_abn_m0.exe`, Lauf-Skript `scratchpad/m0/lauf.sh`.

```
RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_SET_FLAG=9:73,3:94                # Story-Stand nach der Irons-Todesszene (Spur L); (3,94) oeffnet die Tuer 1130->1120
RE15_DEBUG_JUMP=1130@240  RE15_PLAYER_POS=-3050,-2150,2048     # vor Tuer-Slot 1 von ROOM1130 (Ziel ROOM1120 Cut 3)
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
RE15_INPUT_SCRIPT=A0.1,W5,XU1.2,L0.3667,XU0.4,R0.3667,XU0.97,L0.3667,XU0.5,U3,W2
RE15_STATE_LOG / RE15_FENSTER_LOG / RE15_FRAMEDUMP=600-760/4:bild_ / RE15_SE_DEBUG=1 / RE15_AUDIO_CAP_SYNC
```
Gegenprobe ohne (3,94): `[tuer1120] ROOM1130 Slot 1 -> Text-Platz msg 6 (Flag (3,94) = 0)`. Die Tuer ist
gesperrt, wie es die Story verlangt.

Weg im Log (Laeufe w3/w4/w5, alle drei bildgleich):
```
[aot] DOOR FIRE slot=1 rect=(-3050,-2150,hw=500,hh=1000) target_cut=3 spawn=(-7600,0,-2900)
[tuer] Sequenz ... 301 Bilder ...
[room] PC loaded room1120.rdt (86900 bytes)
[fenster] ROOM1120 scharf: Slot 4 Band x 3500..6650 z 4300..6400 Ereignis 24
[fenster] Kraehe Slot 4 versteckt bei (5400,-2500,12100)
state.log: F1 (-7600,-2900) cam=3 -> F521 cam=2 -> Suedgang -> F595 (4508,-13) rot -1056 (Norden) -> F625 (4346,3707) cam=1
[fenster] Ausloeser: Spieler (4319,4373) im Band, Ereignis 24 0          # F634, cam=1, Blick Norden (rot -1056)
[fenster] Splitter Bank 0x10 Platz 95 Lage x 4700 z 10200   ... 13 Zeilen, Plaetze 95..83
[fenster] Knall 1: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+5 0
[fenster] Knall 2: Raumbank-Satz 0x21 (RE2 Se_on 0x02210001) T+10 0
[fenster] Zeitlinie fertig T+23 Splitter 13 Knalle 2 0
```

### 1.2 Teilaspekte, jeweils gemessen

| Teil | Messung | Urteil |
|---|---|---|
| **Ausloeser "in Cut 1 Richtung Fenster hinten zulaeuft"** | Ausgeloest bei (4319,4373), cam=1, Blick Norden zum Fenster. Ich habe das Fenster (Rueckprojektion: Scheiben x 4221..5808, y -3049..-1953, z 11200) selbst in alle 5 Kameras der RDT projiziert (`tools/maske/geom.build_view`). Nur Cut 1 zeigt es, bei Bildpunkt x 182..209 / y 83..101. Cut 0 liegt bei x 358..735 neben dem Bild, Cut 2/4 bei x < 0, Cut 3 hinter der Kamera. Laut SCA-Sperrzellen sperrt der Block (-10500,1650,14000,4750) den Bereich x < 3500. Der Weg nach Norden fuehrt deshalb zwingend durch das Band x 3500..6650. | erfuellt |
| **Scheiben zerbrechen wie RE2, Glas-Effekt aus RE2 extrahiert** | `cmp`: GLAS1090.ESP == `info/re2leon/.../room1090/effect.esp`, GLAS1090_10..14.TIM == esp10..14.tim. `glas1090_extrakt.py --pruefen`: alle Dateien liegen in ROOM1090.RDT (esp11 @0x2E1C8 ... esp14 @0x32688). Die 13 Splitter-Saetze im Code habe ich mit den sub15-Bytes @0x0054..0x01D4 verglichen (27 Saetze, 13 davon mit x in [-2800,-2200], alle 13 feldgleich, Gier `00 0c`). In den Framedumps (`scratchpad/m0/w4/zoom.png`) ist das Fenster bei F632 heil. Bei F636 steht die Kraehe vor der Scheibe. Bei F640/644 sitzen die RE2-Splitter an der Scheibe und die Scheiben sind gezackt offen. Bei F652..F672 fliegen die Splitter Richtung Kamera und landen vor Leon am Boden. Die Splitterformen entsprechen den TIM-Formen (`scratchpad/m0/tims.png`). | erfuellt |
| **Knall aus RE2 extrahiert** | `cmp`: GLAS1090.EDT/.VH/.VB == snd0.edt/.vh/.vb; `--pruefen`: entspricht ROOM1090.RDT @0x075FC/@0x076BC/@0x084DC. RE15_SE_DEBUG im echten Lauf: zwei Mal 4 Lagen `[se] Stimme: se=33 layer=0..3 vag=4 note=67 ... vol=127`, keine GATE-Verwerfung. RE15_AUDIO_CAP_SYNC (1470 Stereo-Bilder je Takt): der Effektivwert springt bei Takt 1381 von ~1000 auf 8136..10403 (Spitze 32459). Letzter Takt 1440 = F700, also ist Takt 1381 = F641 = T+5. Bei Takt 1386/1387 (T+10) setzt der Knall neu an: kurzer Einbruch auf 8128, dann wieder 10065. Danach klingt er bis Takt 1403 auf 3789 ab. | erfuellt |
| **eine Kraehe "rein fliegen"** | state.log Slot 4: F634 State 4/2/1 bei (5400,12100), also hinter der Rueckwand z 11200 und unsichtbar. F637 ss2=2, Clip 4, (5377,11512). F640 (5346,10705). F643 State 1 Sub 13 bei (5326,10214), damit nach 7 Bildern durch die Scheibe im Raum. Danach Anflug auf Leon, F679 Angriff (Leon hp 100 -> 95). Im Bild ist die schwarze Kraehe ab F636 vor dem Fenster, bei F760 greift sie Leon an. | erfuellt |
| **hinteres Fenster im Hintergrund beschaedigt** | Ab F640 und bei F760 dunkle, gezackte Loecher in beiden Scheiben, helle Bruchkanten, Scherben-Tupfen am Wandfuss. **Wiedereintritt** (Lauf w6, gleicher Tuerweg, `RE15_SET_FLAG=9:73,3:94,9:79`): `[fenster] ROOM1120 aus: (9,73)=1 (9,79)=1`. Kein Ausloeser, keine Splitter, keine Knalle. Das Fenster ist bei F620/F660/F700 beschaedigt (`scratchpad/m0/w6/zoom.png`). Die Integration B deckt zusaetzlich den Weg ueber den Speicherstand ab (CONTINUE, Loch 11 / Kante 2156). Die Schadens-Kunst ist als PORT-WAHL gekennzeichnet; ein Original dafuer gibt es nicht. | erfuellt |

**Urteil Punkt 1: erfuellt.**

Beobachtungen ohne Mangel:
* Das Band hat kein Blickrichtungs-Tor. Das entspricht RE2: sat 0x41 hat keinen Richtungstest; den AUTO-Pass habe ich
  in RE1.5 nachgesehen (`andi v0,v0,0x10` / `bne v0,s6` @0x80042ca0-a4). Auf dem Story-Weg nach (9,73) (1150 -> 1130
  -> Tuer 2 -> Cut 1 nach Norden -> Tuer 0 Richtung 1060/1040) wird das Band zum ersten Mal nach Norden gekreuzt.
  Das Ereignis kommt nur einmal ((9,79)).
* Der Knall laeuft ohne Raumlage. RE2 `0x8005ba28` legt die Se_on-Lage (rec+6/8/10) in den Stimmsatz 0x800d4f18
  (+20/+24/+28, @0x8005bc1c-48). Der Port spielt alle 4 Lagen mit vol 127 in der Mitte, wie die schon vorhandenen
  RE2-Raumbaenke (PANEL/Tuer/Aufzug). Die Spitze von 32459/32767 im Mitschnitt liegt knapp unter dem Clipping.
* Zielhilfe auf die versteckte Kraehe (OFFEN laut Dossier). Mein Teilversuch (Lauf w8: Browning, 8 Schuesse
  nach Norden aus (4400,2375) vor dem Ausloesen): Kraehen 1 und 3 sterben (F767/F815), die versteckte Kraehe 4 bleibt
  ueber den ganzen Lauf bei hp 10. Den Fall "nur noch die versteckte Kraehe lebt" deckt das nicht ab. Er bleibt mit
  dem Messweg aus dem Dossier OFFEN.

## 2. RE-Gate

### 2.1 Stichproben, selbst disassembliert (re2_disasm.py / re15_disasm.py, richtige Binaerdatei)

| Behauptung | Datei / Adresse | Befund |
|---|---|---|
| Se_on -> 0x8005ba28, Bank = a0>>24, Satz = (a0>>16)&0xff | RE2 PSX.EXE @0x80056530 `jal 0x8005ba28`; @0x8005ba30 `srl t1,a0,24`; @0x8005ba7c-80 `srl v0,a0,16 / andi s7,v0,0xff`; EDT-Zeiger `lw a1,-17544(at)` = 0x800dbb78+Bank*4 @0x8005ba8c | stimmt |
| ESP-Op-Tabelle 0x8009D868: 5/16/39/84 | [5]=0x8001df90 [16]=0x8001f128 [39]=0x800206fc [84]=0x80025348 | stimmt |
| Op 16: +0x14 := Boden, Op A := +0x0B, Op B := 5 | @0x8001f15c `jal 0x8004fba0` (a1 2, a2 8192, a3 0); @0x8001f170 `lbu a0,11` / @0x8001f174 `sw v0,20(v1)` / @0x8001f178 `sb a0,0`; @0x8001f184 `addiu v0,zero,5` / @0x8001f18c `sb v0,1` | stimmt, Port op_16 identisch |
| Op 39: Op A 84, Op B 0, Spawn 0x14000000 \| +0x3A | @0x80020700 `addiu v0,zero,84`, @0x8002070c `lui a0,0x1400`, @0x80020724 `sb zero,1`, @0x80020734 Einheitsmatrix 0x8009db44, @0x80020740 `jal 0x8001cbe8` | stimmt |
| Op 84: Op A/B 0, Status 0, zweites Glitzern | @0x80025360/64 `sb zero,1/0`, @0x80025378 `sh zero,24`, @0x80025384 `jal 0x8001cbe8` | stimmt |
| Op 5 Bodenzweig | @0x8001e070 `slt v1,a0,v1` -> Op[+2]; @0x8001e098 Kontakt 0x800dcbc8; @0x8001e0bc `beq a0,v0` -> Op[+3] | stimmt mit Port op_5 |
| Sce_espr_on -> FUN_8001bf10 | @0x800565cc `lhu s0,2(s1)`, @0x800565e8 `lhu a1,6(s1)`, SVECTOR rec+8/10/12, @0x80056610 `lh a1,14(s1)`, @0x80056614 `jal 0x8001bf10` | stimmt |
| Sleep/Sleeping = n Bilder spaeter | @0x80053a20 Sleep gibt 1 zurueck (weiter); Sleeping @0x80053a50 Zaehler-1; bei 0: PC+=3 @0x80053a6c, Rueckgabe 2 @0x80053a88. Damit laufen die Folgezeilen im n-ten Folgebild | stimmt (T+2/5/10/23 gegen sub15-Bytes `09 0a 02 00` @0x2E, `0a 03 00` @0x205, `0a 05 00` @0x215, `0a 0d 00` @0x233) |
| Member 0x17 -> +0x1D4 | Tabelle 0x80011228[23] = 0x80055d8c, Delay-Slot `sh a2,468(a0)` @0x80055d90 | stimmt |
| Spawn-Wort +0x10E = rec+4 | @0x8005734c `lhu v0,4(v1)`, @0x80057354 `sh v0,270(s0)` | stimmt |
| Kraehe INIT State-4-Zweig | EMOVL21_S0.BIN @0x80100430 `andi v1,v1,0x4000`, @0x80100444/48 `+4 := 4`, @0x80100454 `andi 0xbfff`, @0x80100460 `ori 0x1` -> +0x22A, @0x8010046c/74 `andi 0xf` -> +5, @0x80100488 `jalr` Tabelle 0x80104908[4] = 0x801034dc. INIT-Rest @0x80100490 (Test +0x10E&0x40) greift bei 0x4002 nicht, deshalb ist die Reihenfolge im Port egal | stimmt |
| Kraehe Sub 2 0x801037f8 | Tabelle 0x80104a64[2]; P0 @0x80103834/54 `lui a0,0x8 / ori 0x8` (0x80008), faellt ohne Sprung in P1 @0x80103878; P1 `andi 0x4` @0x80103880, Anim 0x00070004 @0x80103888-94, `300` @0x80103898, `6` @0x801038a0, `sh zero,468/326`, `and 0xfff7ffff` @0x801038c4; P2 `jal 0x80015350` @0x801038dc, `-10` @0x80103900, Zaehler `bne v1,zero` @0x8010390c (Alt-Wert), `-9`-Maske @0x80103924, `jal 0x80104078(e,1,4)` @0x8010392c. Das ergibt 7 Bewegungsbilder 300..240 | stimmt |
| Wandpass-Tor word0&8 | RE2 PSX.EXE @0x80035694 `andi v0,v1,0x8` / @0x80035698 `bne v0,zero,0x800356ec` | stimmt |
| RE1.5 sce 3 = Ereignis-Platz | RE1.5 PSX.EXE Tabelle 0x8007469c[3] = 0x800430f0; @0x800430fc `lhu a0,0(v0)` (p0), @0x80043100 `lbu a1,3(v0)` (Ereignis), @0x80043104 `jal 0x8003ee3c` | stimmt |
| RE1.5 ROOM1120 hat kein Fenster-Ereignis | `scd_dump_room.py ROOM1120.RDT`: main00 = 3 Tueren + Aot 1/3 + 3 Kraehen `44 0n 21`, sub00 = `01 00`, sub01 = Flag-Buchhaltung (3,54..56). Kein Sce_espr_on, kein Se_on | stimmt, Beta -> Retail ist berechtigt |

### 2.2 Wortsuche im Diff
`git diff 154a73c1...HEAD -- re15_port | grep '^+'` nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
approx: **0 Treffer**. Einziger neuer getenv ist `RE15_FENSTER_LOG` (Messschiene, kein Verhalten).

### 2.3 Erklaert der Fix den Befund?
Die Vorbedingung, dass RE1.5 hier kein Fenster-Ereignis hat, habe ich selbst gemessen (s. o.). Die Kette
Ausloeser -> (9,79) -> Zeitlinie -> Splitter/Knall/Kraehe -> Schaden steht im Log des echten Laufs. Die Kraehe ist
vor dem Ausloesen unsichtbar (Bild F632) und fliegt danach durch die Scheibe herein (state.log).

### 2.4 Gate-Luecke -> Mangel 1
`re15_port/include/re15_fenster1120.h:53-54`
```
#define RE15_FENSTER_P0            0x00FF
#define RE15_FENSTER_P1            ((uint16_t)((RE15_FENSTER_EREIGNIS << 8) | 0x18))
```
Weder Code noch Commit-Message belegen p0 = 0x00FF und das untere p1-Byte 0x18. Die Kommentarzeilen darueber
belegen nur die sce-3-Typtabelle und Byte 3. Verhaltensrelevant ist p0, denn der Handler gibt es als a0 an 0x8003ee3c
weiter. Dort waehlt a0 >= 0xa einen freien Ereignis-Thread-Platz (`sltiu v0,a2,0xa` @0x8003ee54, ab @0x8003ee70).
Selbst geprueft: Die Werte sind die Original-Satzform ROOM1050.RDT @0x0C22 `2c 07 03 31 ... ff 00 18 02`
(p0 @0x0C30 = `ff 00`, p1 @0x0C32 = `18 02`). Dieselbe Form belegt re15_adaruf.h:81-84. Kleinigkeit, kein eigener
Mangel: `re2_fx.c` re2fx_register_raum `if (n >= 8) break; /* sltiu v0,t0,0x8 */` zitiert die Instruktion ohne
eigene Adresse; die Funktion selbst ist mit @0x8001bccc-0x8001bd20 belegt.

## 3. Vertrag

| Pruefung | Befund |
|---|---|
| Bank-9-Bits | geschrieben nur (9,79) (Programm `22 09 4f 01`), gelesen (9,73). Entspricht VERTRAG §1.1 |
| Nachrichten-IDs | keine belegt (1120: 9..15 nicht gebraucht) |
| Ereignis / AOT-Slot | Ereignis 24 (VERTRAG §1.3); Slot 4 mit Zensus im Dossier §1 (main00 0..3, Kamerazonen 48..63) |
| Texturplatz | 55. In master ist 53..55 frei (render_pc.c:209, RE15_TIM_SLOT_MAX 56), RE2FX_TIM_SLOT ist 52 |
| Pfade | `git diff 154a73c1...HEAD --name-only`: 29 Dateien. Treffer auf `release/`, `platform/android/`, `shared_assets/PSX/`, `tests/unit/CMakeLists.txt` oder `tests/integration/CMakeLists.txt`: **0**. Neue Assets unter `shared_assets/RE2/GLAS1090.*` stehen im Dossier fuer das Paket-/Android-Gate |
| Gemeinsame Dateien | enemy_ai_common.c +4/-1, game_step_common.c +2, scd_room_setup.c +2, scd_vm.c +2, main.c +13 (5 Haken mit je 1-4 Zeilen), audio_pc.c +46 (eigener Block), re2_fx.h +10, re2_fx.c +125 (4 case-Zeilen, esp_at-Weiche, 1 Zeile register_core, Block am Ende). Alle Haken haben den Kommentar `Runde 35 Spur M` |
| Tests | 4 Unit-Riegel + 1 exe-Haken, jeder misst. glas prueft das Landebild 27 gegen eine unabhaengige Rechnung aus den Schrittbytes. kraehe prueft 7 Bilder / 1883 Einheiten / ACTIVE 4. knall prueft EDT @0x84 `00 00 7c 60` -> Ton 7..10, VAG 4. ereignis prueft Tor, Raum, AOT, VM-Spawn, T+2/5/10/23 und den Wiedereintritt. integration prueft Log und Pixel vor/nach sowie den Wiedereintritt |

## 4. Maengel

1. **@0x-Gate: RE15_FENSTER_P0 = 0x00FF und das untere Byte 0x18 von RE15_FENSTER_P1 haben keinen Beleg**
   (`re15_port/include/re15_fenster1120.h:53-54`, und auch nicht in der Commit-Message 55554779/1e958236).
   Pruefen: `git show HEAD:re15_port/include/re15_fenster1120.h | sed -n 46,54p`. Dort steht kein @0x/Byte-Offset
   fuer 0x00FF/0x18. Abhilfe: Satzform ROOM1050 sub00 @0x00C22 (`ff 00 18 02`, p0 @0x0C30) und die Semantik von p0
   (0x8003ee3c: a0 >= 0xa -> freier Thread-Platz, @0x8003ee54) in den Kommentar und die Commit-Message schreiben.
   Die Werte selbst sind richtig, Verhalten aendert sich nicht.

## 5. Hinweise fuer den Orchestrator (keine Maengel dieser Spur)

* `ctest -R "r35_cut11c0_fenster"` findet 0 Tests. Die Tests heissen `unit_r35_fenster_*` / `integration_r35_fenster`;
  mit `-R r35_fenster` laufen alle 5.
* Ein Probe-Merge gegen das aktuelle master 87cc8575 (`git merge-tree --write-tree master HEAD`) meldet Konflikte in
  `re2_fx.c` (Spur B hat Ops 7/15/17/22/23/24/47/59/70 an denselben Stellen ergaenzt: Vorwaertsdeklarationen,
  case-Liste, Block am Dateiende), in `scd_room_setup.c` und in `scd_vm.c` (Nachbarzeilen der Haken). Alle drei
  Konflikte fuegen nur hinzu, beide Seiten bleiben erhalten. Die Op-Nummern ueberschneiden sich nicht.
* (9,73) setzt erst Spur L (in master: `re15_irons_tod.h` RE15_IT_BIT_GESEHEN 73). Allein auf diesem Zweig ist das
  Ereignis im Spiel deshalb nicht erreichbar. Gemessen habe ich mit RE15_SET_FLAG.
* Die PSX-Seite (Raum-ESP, Texturen, Knall, Schaden) ist nur auf PC verdrahtet. Das steht im Dossier unter OFFEN.

## 6. Messartefakte (Scratchpad, nicht eingecheckt)
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/m0/`
w3 (Weg + Log), w4 (`sheet.png`, `zoom.png`, Framedumps F600..F760), w5 (SE-Debug + `cap.raw`), w6 (Wiedereintritt,
`zoom.png`), w7/w8 (Zielhilfe-Versuch), `tims.png` (RE2-Glas-TIMs).
