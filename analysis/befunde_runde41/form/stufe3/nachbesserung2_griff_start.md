# Nachbesserung 2, Zusatzpunkt Koordinator: START waehrend des Riesenform-Griffs

Befund (GR-Abnahme 2, Nebenbefund 1, `.claude/worktrees/r41_granate/analysis/befunde_runde41/GR_abnahme_2.md` Abschnitt 7.1, Log
`GR_belege/abnahme2/abn2_logauszug.txt` b5/b6): START im Griff der Riesenform (Sub 15, Clip 27) -> Statusschirm (mit Waffenwechsel),
danach "Form und Leon > 600 Bilder eingefroren, Schuesse wirkungslos"; ohne START endet der Griff normal.

## 0. Stand

| Schritt | Stand |
|---|---|
| 1 Messen/Reproduzieren | erledigt (Abschnitt 1): echtes Fenster und Engine-Harness |
| 2 Original | erledigt (Abschnitt 2): RE1.5 und RE2 verwerfen START im Griff ganz |
| 3 Behebung | erledigt (Abschnitt 3): engine/src/menu_common.c re15_menu_start_poll |
| 4 Riegel | erledigt (Abschnitt 4): unit_n2_griff_start, Gegenprobe rot ohne Behebung |
| 5 Echtes Fenster | erledigt (Abschnitt 5): stufe3/nachbesserung2/griff_start/echt_vorher_nachher.png |

## 1. Messung

**Echtes Fenster** (eigene exe-Kopie aus `re15_port/build_n2s`, gdigrab `hwnd=`, Renderer des Nutzers; Lauf
`re15_port/build_n2s/spiel/griff_start_vorher`, Eingaben wie Nachbesserung 1 `l5090_angriffe`: `RE15_DEBUG_JUMP=5090@250`,
`RE15_PLAYER_POS=500,-9175,2048,0`, `RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W60`, dazu `RE15_PAD_AT=760:S,900:S`). Zustandsprotokoll um
`hk=<aca58>/<aca5a> io=<Erlaubnis> hr=<+0x93> ms=<Menue-Stufe>` ergaenzt (platform/pc/main.c, nur Messung):

| Bild | Spieler | Haken | +0x93 | Erlaubnis | Menue | Form |
|---|---|---|---|---|---|---|
| F727 | Griff, mo 13 | 6/1 | 01 | - | 0 | Clip 27 Bild 8 |
| F731-F759 | mo 14 (Haken [0] Phase 3) | 6/3 | 01 | 0 | 0 | Bild 12-40 |
| F760 | START-Flanke | 6/4 | 01 | 0 | 0 | Bild 41 |
| F761 | Haken los (`loesen`) | 0/255 | 00 | - | 0 | Bild 42 |
| F762-F906 | | 0 | 00 | 1 | 2 (Schirm offen) | steht auf Bild 42 (Spiel angehalten) |
| F900 | START schliesst | | | | -> 4, 5 | |
| F914 ff. | idle 200/210/214, Treffer F1170 (Rueckstoss), Tod F1287 | | | | 0 | Bild 43 ..., F980 Gehen, Biss |

Lesung: Der Port oeffnet den Schirm nicht IM Griff, aber er MERKT START vor (klebende Anforderung) und oeffnet ihn im ersten freien Bild
nach dem Loslassen. Nach dem Schliessen laufen Form und Leon weiter (Bildleiste `scratchpad n2s/folge.png`: Schirm, danach Form hinter Leon).
Lauf b5 der GR-Abnahme passt Bild fuer Bild dazu (START F1699/F1700 bei Form-Bild 50, Haken los bei Form-Bild 94, ab da steht alles):
das "Einfrieren" ist der nachtraeglich aufgegangene Schirm, in dem das Eingabeskript weiter `MA` (R1 + Viereck) drueckte - das
navigiert im Schirm und legte eine andere Waffe an (mg 4 -> 239), schliesst ihn aber nie. Engine-Harness (`test_n2_griff_start`,
echter `re15_game_step`, Bahnsteig-Bank wie `pc_enemy_load_bahnsteig`): gleiche Folge in Gangmitte (Haken 12 Bilder) und an der
Nordwand (Ringkampf, Haken 64 Bilder), mit und ohne Waffenwechsel im Schirm; nach dem Schliessen kein Einfrieren.

## 2. Original

**RE1.5 PSX.EXE, Spiel-Schwanz** (info/Re1.5/PSX.EXE; Version_40%/PSX.EXE bytegleich ueber 0x8001cd04-0x8001cdf0):

```
8001ccf4  lui   a1,0x800b
8001ccf8  addiu a1,a1,-13764        a1 = 0x800aca3c
8001ccfc  lw    a0,0(a1)            a0 = aca3c
8001cd04  andi  v0,a0,0x40
8001cd08  bne   v0,zero,0x8001cdec  aca3c & 0x40 -> GANZER START-Block uebersprungen (auch die Vormerkung)
8001cd14  lw    v0,g_pauseflags / and 0x01000000 / bne -> 0x8001cdec
8001cd68  lhu   v0,0x800ac762 / andi 0x800 / beq -> 0x8001cd94      rohe START-Flanke
8001cd80  lw    v0,0x800aca3c / ori 0x8000 / sw                     Vormerkung (klebend)
8001cd98  lw    v1,0x800aca3c / andi 0x8000 / beq -> 0x8001cdec
8001cdb0  lbu   v0,0x800acae7 (Spieler+0x93) / bne -> 0x8001cdec     erst bei +0x93 == 0 oeffnen
8001cdbc  ori   v0,v1,0x40 / sw 0x800aca3c ; sb 1,0x800b5359 ; g_pauseflags |= 0xff000000
```

**RE1.5 STAGE5.BIN, Birkin-Spieler-Haken 0x8011afd8** (Port birkin_opfer.c): jede der drei Maschinen setzt beim Packen
`aca3c |= 0x40` und +0x93 |= 1 und loescht beides beim Loslassen:

| Haken | setzen | loeschen |
|---|---|---|
| [0] von vorn (cmd 6 der Form) | `ori v0,v0,0x40` @0x8011b0b4, `sw v0,0x800aca3c` @0x8011b0c0; `ori v1,v1,1` / `sb +0x93` @0x8011b0b8/c8 | `andi v1,v1,0xfe` / `sb +0x93` @0x8011b3d4/dc; `addiu v1,zero,-65` / `and` / `sw aca3c` @0x8011b3e0-ec |
| [1] von hinten | `ori v0,v0,0x40` @0x8011b4a0, `sw` @0x8011b4ac; `sb +0x93` @0x8011b4b4 | `sb +0x93` @0x8011b6b4; `addiu v1,zero,-65` @0x8011b6b8, `sw` @0x8011b6c4 |
| [2] Biss | `ori v0,v0,0x40` @0x8011b770, `sw` @0x8011b77c; `sb +0x93` @0x8011b784 | `sb +0x93` @0x8011b958; `addiu v1,zero,-65` @0x8011b95c, `sw` @0x8011b968 |

Damit ist im Original waehrend des ganzen Griffs `aca3c & 0x40` gesetzt, der START-Block @0x8001cd04-08 wird uebersprungen: START im
Griff wird **verworfen**, nicht vorgemerkt; es geht auch nach dem Loslassen kein Schirm auf.

**RE2 (ghidra_re2_Leon.txt), Spiel-Schleife**: `lui s3,0x800d` / `addiu s3,s3,-0x3e18` @0x800257cc-d0 -> s3 = 0x800cc1e8 (gegengeprueft:
`lw v1,0x39f4(s3)` = 0x800cfbdc = Pausenflags, @0x800264ec). Vor der START-Flanke:

```
80026438  lbu  v0,0x3be3(s3)        = 0x800cfdcb = Spieler+0x93 (G4-Overlay prueft dasselbe Byte @0x80102420-2c)
80026440  bne  v0,zero,0x80026540   -> ganzer START-Block uebersprungen
80026460  lhu  v0,0x800ce306 / andi 0x100 ...  START-Flanke -> 0x800cfbd8 |= 0x8000
800264dc  andi v0,v1,0x8000 / beq -> 0x80026540 ; sonst oeffnen (cfbd8 |= 0x40, 0x800df348 = 1, Pausenflags |= 0xff000000)
```

RE2 verwirft START also ebenfalls, solange Spieler+0x93 steht (Griff, Treffer). Beide Spiele: kein Schirm im Griff, keine Vormerkung.

**Port vorher:** `re15_menu_start_poll` (engine/src/menu_common.c) modelliert @0x8001cd04-08 nur als "Menue schon offen" (`s_alive ||
s_stage != 0`); birkin_opfer.c fuehrte `aca3c |= 0x40` als "ohne Port-Gegenstueck, nicht gebaut". Das Tor @0x8001cdb0 (+0x93) bildet
der Port ueber eigene Praedikate nach (`s_inv_open_allowed`: Griff/Treffer/Tod/Szene/Treppe), die Vormerkung laeuft aber durch.

## 3. Behebung

`engine/src/menu_common.c` `re15_menu_start_poll`: neben `s_alive || s_stage != 0` (Menue offen = aca3c & 0x40 vom Menue) springt der Poll
jetzt auch dann ueber den ganzen START-Block, wenn der Birkin-Spieler-Haken laeuft (`re15_bk_opfer_cmd()` = aca58 5/6 von der Packung bis
`loesen` = genau die Spanne, in der der Haken aca3c & 0x40 haelt, @0x8011b0b4-c0 .. @0x8011b3e0-ec). Wirkung wie @0x8001cd04-08: START im
Griff wird verworfen, nichts vorgemerkt. Keine neue Konstante. Der Kopf von `engine/src/birkin_opfer.c` nennt die Nachbildung (vorher
"ohne Port-Gegenstueck"). Messhaken (nur Messung): `platform/pc/main.c` Zustandsprotokoll `hk=<aca58>/<aca5a> io=<Erlaubnis> hr=<+0x93>
ms=<Menue-Stufe>`.

Nicht geaendert: das Tor @0x8001cdb0 (+0x93) bildet der Port weiter ueber `s_inv_open_allowed` nach; die Menue-Schliessfolge (Stufe 3
`sb zero,0x800aca58` @0x8001cbdc) ist unveraendert und war nicht die Ursache (Kontrolle unten).

## 4. Riegel `unit_n2_griff_start` (tests/unit/test_n2_griff_start.c, echter re15_game_step, Bahnsteig-Bank wie pc_enemy_load_bahnsteig)

| Fall | Ergebnis (Bau re15_port/build_n2s) |
|---|---|
| START im Griff, 5090 Gangmitte (Haken [0] 30 Bilder, 4 START-Flanken) | kein Schirm, weder im Griff noch danach; Form verlaesst Clip 27, tickt weiter; Spieler frei, laeuft |
| START im Griff, 5090/5091 Nordwand (Ringkampf, 75 Bilder, 11 Flanken) | dito |
| Gegenprobe ohne START, Gangmitte / Nordwand | Haken los nach 34 / 86 Bildern, Ende wie oben |
| Kontrolle START nach dem Loslassen + Waffenwechsel, 5090 Nordwand / 5091 Gangmitte | Schirm geht normal auf (21 Bilder nach dem Loslassen), Wechsel Messer -> Pistole uebernommen, nach dem Schliessen laufen Form und Spieler weiter |

Gegenprobe des Riegels: mit ausgeschalteter Zeile (`if (0 && re15_bk_opfer_cmd())`) 3 Fehler "START im Griff vorgemerkt, Schirm ging
17 Bilder nach dem Loslassen auf"; mit der Zeile gruen.

Gezielte Suite im eigenen Baum (`ctest --test-dir re15_port/build_n2s -R "birkin|griff|inv_|inventar|menu|r30_android|kleinkram|hinweis|opfer|n2_griff|r39_status|r38_griff" -j 6`, 942 s): **101 von 101 gruen**, darunter unit_n2_griff_start, unit_birkin_opfer,
unit_r41_birkin_wand_griff/_g5/_rein, unit_birkin_szenen, integration_r41_birkin_pistole und die Inventar-/Kleinkram-Tests.

## 5. Echtes Fenster nachher

Lauf `re15_port/build_n2s/spiel/griff_start_nachher` (gleiche Eingaben, `RE15_PAD_AT=740:S,750:S,760:S`, alle im Griff hk=6/3..6/4):
ms=0 in jedem Bild, kein Schirm; Haken los F761, Form beendet Clip 27 bei F820 (Gehen) und greift weiter an. Blatt
`stufe3/nachbesserung2/griff_start/echt_vorher_nachher.png` (oben vorher: Schirm ab F762 nach dem Loslassen; unten nachher: kein Schirm).

## 6. Offen

- Die anderen Spieler-Haken (Zombie-Griff cmd 5 @0x8010a28c, Hund, Gorilla, Affe) setzen im Original moeglicherweise ebenfalls aca3c |= 0x40;
  nicht geprueft (ausserhalb des Auftrags). Der Port sperrt dort weiter nur das Oeffnen (s_inv_open_allowed), die Vormerkung laeuft.
- acc0c (zweites Sperrbit des Birkin-Hakens) bleibt ungebaut.
