# Runde 30 — Thema C: Android, R1 als Umschalter

Stand: master 8d83a025 (v0.8.15). Phase ERMITTLUNG, kein Bau. Nichts an `engine/`,
`platform/`, `include/`, `shared_assets/` geaendert.
Eigener Bau: `re15_port/build_r30_android-r1-toggle/` (re15_pc.exe vom 2026-09-27 23:47).
Ausgaben: `build/r30_android-r1-toggle/`.

> **Einordnung.** Das ist eine PORT-KOMFORTFUNKTION auf Nutzerwunsch, kein Original-Verhalten.
> Sie braucht selbst kein `@0x`. Belegt wird aber, **welches Pad-Bit das Original wie liest**
> (Pegel, nicht Flanke) — nur deshalb darf der Umschalter das Bit einfach stehen lassen.

---

## 1. Auftrag

> Beim Android Port will ich eine minimale Änderung. Dort möchte ich, das R1 nicht gedrückt
> gehalten werden muss, um die Waffe zu heben, sondern das man einmal kurz R1 andrückt, dann
> bleibt die Kampfpose vorbereitet, und drückt man erneut R1 geht die Kampfpose wieder zurück.

Grund laut Nutzer: R1 halten + Viereck druecken ist auf dem Touchscreen ein Fingerknoten.

---

## 2. Messung im Port

| Nr | Was | Ergebnis | Datei |
|---|---|---|---|
| M1 | Overlay am PC (`RE15_TOUCH_OVERLAY=1 RE15_TOUCH_SELFTEST=1`), echtes Fenster per gdigrab | Overlay 960x720, u=60, 10 Knoepfe; Selbsttest `ok=21 fail=0`; R1-Finger liefert `bits=0800`, Loslassen `bits=0000` | [Bild](../../build/r30_android-r1-toggle/m1_overlay_titel_gdigrab.png), [Log](../../build/r30_android-r1-toggle/m1_overlay_selftest_debug.log) |
| M2 | Sonde `probe_r30_android_r1_toggle` (echter `re15_game_step`, ROOM1140, KI pausiert), 3 Modi x 9 Faelle | siehe Tabelle unten; Abschluss `RIEGEL-GRUEN` | [m2_sonde.txt](../../build/r30_android-r1-toggle/m2_sonde.txt) |
| M3 | Knopf-Geometrie mit der Arithmetik aus `tp_layout` | 960x720: R1 Mitte (858,58); 2400x1080 (Emulator): R1 Mitte **(2247,87)**, Viereck **(2000,801)**, START (1362,1008) | [m3_layout.txt](../../build/r30_android-r1-toggle/m3_layout.txt) |
| M4 | Echtlauf re15_pc.exe, Titel -> Spiel -> ROOM1030, R1 per Skript GEHALTEN (`M4`, `M2`), `RE15_WAFFEN_LOG` | F531-650 R1=1: Ziehen 14 Bilder, dann bereit; F651 R1=0: Senken 9 geloggte Bilder, F660 Phase 0. Zweites Halten: Heben 9 geloggte Bilder | [Log](../../build/r30_android-r1-toggle/m4_echtlauf_waffen.log), [Bild F600](../../build/r30_android-r1-toggle/m4_echtlauf_F600_ziel_overlay.png) |
| M5 | Zensus der R1-Leser im ORIGINAL (EXE + DEBUG.BIN + Overlays) | 19 Treffer, siehe 3.4 | [m5](../../build/r30_android-r1-toggle/m5_r1_leser_zensus.txt) |
| M6 | Zensus aller `Sce_key_ck` in den 240 RDTs | 1579 von 1579 Regionen sauber gelaufen; 255 Abfragen, Masken nur 0x0001/2/4/8/0x0040; **0 mit R1-Bit** | [m6](../../build/r30_android-r1-toggle/m6_scd_keyck_zensus.txt) |
| M7 | Echtlauf: Spieler stirbt WAEHREND des Zielens (`RE15_KILL_AT=600`), Kontrolllauf ohne Zielen | zielend: steht bei F630 und F710 aufrecht in der Messerpose; Kontrolle: F630 sackt, F710 liegt | [Vergleich](../../build/r30_android-r1-toggle/m7_tod_vergleich_oben-zielend_unten-kontrolle_F630_F710.png) |

### M2 im Einzelnen (Bilder = 30-Hz-Spielschritte)

Modi: **HALTEN** = Bit folgt dem Finger (heute). **RASTE** = Vorschlag (Umschalter + Phasen-Riegel).
**NAIV** = Umschalter ohne Phasen-Riegel (Gegenprobe).

| Fall | HALTEN | RASTE | NAIV |
|---|---|---|---|
| A Heben / 30 Bilder stehen / Viereck / Senken (Browning) | 10 / 30 / Magazin 15->14 / 10 | **10 / 30 / 15->14 / 10** | 10 / 30 / 15->14 / 10 |
| B Inventar: oeffnet nach | 9 | 9 | 9 |
| B R1-Bit im Pad-Wort waehrend des Menues | 32 von 32 | **0 von 32** | 32 von 32 |
| B ein R1-Tipp im Tab-Schirm | substate 0->2 (FILE) | **0->2 (FILE)** | 0->0 (**Tipp verschluckt**) |
| C Kiste, Tipp 1 / Tipp 2 (Soll je +5) | 5 / 10 | **5 / 10** | 0 / **20** (Dauerlauf) |
| D Messer gehoben -> Inventar -> Browning -> zu | Phase 2 bleibt, `melee_latch=1` mit Browning | Senken 10 Bilder, neuer Tipp: bereit nach 10, `melee_latch=0`, 15->14 | wie HALTEN |
| E Raumblende + cmd-Reset | danach wieder bereit nach 10 | Raste 0, Phase 0 | wie HALTEN |
| F Cutscene 40 Bilder | Zielphase aktiv 40/40 | Zielphase aktiv 40/40, danach Senken 10 | 40/40, bleibt Phase 2 |
| G Text-Freeze 40 Bilder | aktiv 40/40 | aktiv 40/40, danach Senken 10 | 40/40, bleibt Phase 2 |
| I Tod | aktiv 40/40 | aktiv 40/40, Raste 0 | aktiv 40/40, Raste 1 |
| H Treffer (cmd 2, Clip 8) | wieder bereit nach 32 | **Raste bleibt 1, bereit nach 32** | 32 |

Lesart:
* Fall A: ein dauerhaft gesetztes R1-Bit genuegt. Heben, Halten, Feuern und Senken sind in
  allen drei Modi bildgleich (10/30/1/10).
* Faelle B und C sind der Grund fuer den Phasen-Riegel: ohne ihn verschluckt das Inventar jeden
  zweiten Tipp, und die Kiste blaettert nach dem zweiten Tipp von selbst weiter
  (`bx_repeat`, `re15_itembox.c:346-355`).
* Fall D zeigt einen Port-Zustand, der mit gehaltenem R1 schon heute erreichbar ist: nach dem
  Waffenwechsel im Inventar steht die Zielphase weiter auf 2 und der Klassen-Latch auf Messer.
  Mit dem Riegel faellt die Raste beim Oeffnen, das naechste Heben ist frisch.

---

## 3. Original-Mechanismus

Alle Zeilen selbst disassembliert aus `info/Re1.5/PSX.EXE` (t_addr 0x80010000) bzw.
`info/Re1.5/PSX/BIN/*.BIN`, Werkzeug `re15_disasm.py`; DEBUG.BIN mit Ladeadresse 0x800c0000
ueber `analysis/befunde_runde30/android-r1-toggle/dis_modul.py`.

### 3.1 R1 im Pad-Wort

```
Preset-Tabelle 0 @0x80073dbc, Datei-Offset 0x645bc:
  00 10 00 20 00 40 00 80 00 10 00 40 80 00 80 00
  08 00 40 00 08 00 04 00 00 80 00 20 80 00 40 00
  Eintrag [8]  = 0x0008 (PADR1) -> virtuelles Bit 8  = 0x0100
  Eintrag [10] = 0x0008 (PADR1) -> virtuelles Bit 10 = 0x0400
FUN_80030444 baut das virtuelle HELD-Wort:
  800304b8  lw v0,0(a3)          Tabellenzeiger
  800304c4  lhu v0,0(v1)         Rohmaske des Eintrags
  800304cc  and v0,v0,t0         gegen das rohe Pad
  800304d4  sllv v0,t1,a0        1 << i
  800304e4  sw v0,16(a2)         -> 0x800ac768
  80030564  sh a0,0x800ac760     rohes HELD (Halbwort)
  800305a0  sh a0,0x800ac762     rohe FLANKE (Halbwort)
```

### 3.2 Zielen ist ein PEGEL

```
Zieleintritt (DECIDE Substate 0, Funktion @0x80031f38):
  80031f40  lui s0,0x800b
  80031f44  addiu s0,s0,-14488   s0 = 0x800ac768 (virtuelles HELD)
  80031ff4  lw v0,0(s0)
  80031ffc  andi v0,v0,0x100     R1
  80032000  beq v0,zero,0x80032024
  8003200c  lbu v0,0x800aca5d    angelegte Waffe
  80032014  beq v0,zero,0x80032024
  80032018  ori v0,zero,0x701
  80032020  sw v0,0x800aca58     cmd 1, Aktion 7 = Zielen
Halten / Senken (Schusswaffe):
  800331e4  lw v1,0x800ac768
  800331ec  andi v0,v1,0x100
  800331f0  bne v0,zero,0x8003320c   R1 steht -> bleiben
  800331f8  ori v0,zero,0x3
  80033200  sh v0,0x800aca5a         R1 weg -> Senken
Halten / Senken (Nahkampf):
  80035128  lw v1,0x800ac768
  80035130  andi v0,v1,0x100
  8003513c  ori v0,zero,0x3
  80035144  sh v0,0x800aca5a
Feuern:
  80033300  lw v0,0x800ac768
  80033308  andi v0,v0,0x40      Viereck, ebenfalls Pegel
```

Keiner dieser Leser nimmt das Flankenwort 0x800ac76c. Ein stehendes Bit ist fuer die
Spieler-FSM von einem gehaltenen Knopf nicht zu unterscheiden.

### 3.3 Wer beendet das Zielen ausser R1

```
Treffer/Tod (Resolver):
  80012ee8  bgez v1,0x80012f00   HP >= 0 -> kein Tod
  80012ef0  ori v0,zero,0x3
  80012ef4  sb v0,4(s1)          cmd 3
  80012ef8  sb zero,5(s1)
  80012efc  sb zero,6(s1)
Cutscene (Plc_motion, Handler @0x80041b90):
  80041ba4  ori v1,zero,0x4
  80041ba8  sb a1,148(v0)        +0x94 Motion
  80041bb0  sb v1,4(v0)          cmd 4
Dispatcher FUN_80031c44:
  80031c78  bltz a0,0x80031da8   Pause-Bit 0x80000000 -> alles uebersprungen
  80031c8c  lbu v1,0x800aca58    cmd
  80031ca4  addiu at,at,16272    Tabelle 0x80073f90
  80031cb4  jalr v0
  Tabelle: [1]=0x80031de8 [2]=0x80035af0 [3]=0x800366bc [4]=0x80030660 [5]=0x80036834
Pad-Freeze:
  800304f4  lw v0,g_pauseflags
  800304f8  lui v1,0x100
  80030514  andi v0,v0,0xf000
  8003051c  sw v0,0x800ac768     HELD auf 0xf000 gekappt -> R1 (0x100) faellt weg
Inventar-Ende (Transitions-FSM FUN_8001c958, Tabelle @0x8001069c):
  8001cb40  jal 0x80029a98       Status-Task starten (a1 = 0x8004603c)
  8001cb48  jal 0x80029ac8       Task 0 parkt
  8001cb70  j 0x8001cbac
  8001cbac  ori v0,zero,0x3
  8001cbb4  sb v0,0x800b5359     Zustand 3
  8001cbdc  sb zero,0x800aca58   cmd := 0
```

### 3.4 R1-Leser im Original (M5)

| Adresse | Wort | Maske | Bedeutung |
|---|---|---|---|
| 0x80031ffc, 0x80032418, 0x80032708, 0x80032a68, 0x80032c9c | virt. HELD | 0x0100 | Zieleintritt aus den fuenf DECIDE-Handlern |
| 0x800331ec, 0x800335bc, 0x80033600, 0x80033fa8 | virt. HELD | 0x0100 | Schusswaffe: Halten, Rueckstoss-Abbruch, nach dem Nachladen |
| 0x800342f8, 0x8003462c, 0x80034648 | virt. HELD | 0x0100 | Dauerfeuer |
| 0x80035130 | virt. HELD | 0x0100 | Nahkampf Halten |
| 0x80049814 | rohe FLANKE | 0x0008 | Status-Schirm: Sprung auf FILE |
| 0x800c6df4 (DEBUG.BIN) | rohe FLANKE | 0x0008 | Datei-Schirm: verlassen |
| 0x8004c470 | rohe FLANKE | 0x0008 | Zaehler +1 im Bereich 0x8004c4xx — Zweck NICHT BELEGT |
| 0x8004cff8, 0x8004d344 | rohes HELD | 0x0008 | Bereich 0x8004cxxx/0x8004dxxx — Zweck NICHT BELEGT |
| 0x80020c98 | rohes HELD | 0x090c | alle vier Bits zugleich (`bne v0,v1`) — Zweck NICHT BELEGT |

Abdeckung des Zensus: Registerverfolgung je Funktion; Leser, die das Wort ueber Stack oder
Argument weiterreichen, sieht er nicht. Virtuelles Bit 10 (0x0400) hat in diesem Zensus
keinen einzigen Leser.

### 3.5 Titel — Berichtigung der Aufgabenstellung

```
TITLE.BIN @0x80102c0c  lhu v0,0x800ac762      rohe FLANKE
          @0x80102c14  andi v0,v0,0x8f0
TITLE.BIN @0x80101260  lhu v0,0(a1)           a1 = 0x800ac762
          @0x80101268  andi v0,v0,0x8f0
```

0x8f0 in PsyQ-Ordnung = START (0x0800) + Dreieck/Kreis/Kreuz/Viereck (0x00f0). R1 ist dort
0x0008 und gehoert NICHT dazu. "confirm 0x8f0 = D-Pad/R1" trifft nicht zu. Der Port liest
entsprechend Face-Tasten + START (`main.c:2981-2983`, `:2194-2195`).

---

## 4. Ursache

Kein Defekt, sondern fehlende Funktion:

1. `touch_overlay_pc.c` kennt nur den Fingerzustand. `TP_R1` (`:26`) haengt am Rechteck
   `:159`, `tp_bits_for_point` (`:208-218`) liefert das Bit, solange der Finger aufliegt.
   Einzige Latch-Logik ist der Ein-Tick-Latch fuer Blitz-Tipps (`seen`/`up_pending`, `:39-44`)
   und die F9-Flanke (`s_marke`). Einen Umschalter gibt es nirgends, auch nicht fuer Rennen.
2. `input_pc.c:373` ODERt die Overlay-Bits ungefiltert ins Pad-Wort; `:409` schreibt
   `g_engine.pad_current`.
3. `main.c:5914-5915` reicht das Wort ueber `pc_pad_config` an `gctx` weiter.
4. `player_common.c:940` liest `r1_held` als Pegel, `:941-975` senkt bei `!r1_held`,
   `game_step_common.c:1639` verlangt denselben Pegel fuer den Schuss.

Aktiv ist das Overlay per Kompilierschalter plus Env (`touch_overlay_pc.c:83-91`):
Android immer an (`__ANDROID__`, abschaltbar nur mit `RE15_TOUCH_OVERLAY=0`, das sich dort
nicht setzen laesst), Desktop nur mit `RE15_TOUCH_OVERLAY=1` (Maus = ein Finger).
Ohne Overlay gibt `re15_touch_pc_pad_bits` bei `:269` sofort 0 zurueck.

---

## 5. Umsetzungsplan

Grundsatz: der Umschalter sitzt IM OVERLAY. Tastatur (`input_pc.c:345`) und Gamepad
(`:227`) laufen nicht durch ihn und bleiben Halte-Tasten — auch ein Bluetooth-Pad am
Android-Geraet.

### Schritt 1 — reine Logik

Neue Datei `re15_port/platform/pc/src/touch_r1_toggle_pc.h`, Inhalt = der Vorschlag
`analysis/befunde_runde30/android-r1-toggle/touch_r1_toggle.h` (header-only, kein SDL):

```c
typedef struct { unsigned char latched, prev_down; } re15_r1_toggle_t;
static inline int re15_r1_toggle_step(re15_r1_toggle_t *t, int finger_r1, int phase_live)
{
    int down = finger_r1 ? 1 : 0;
    int edge = down && !t->prev_down;
    t->prev_down = (unsigned char)down;
    if (!phase_live) { t->latched = 0; return down; }
    if (edge) t->latched = (unsigned char)!t->latched;
    return t->latched;
}
```

### Schritt 2 — Overlay (`touch_overlay_pc.c` / `.h`)

* Zustand: `static re15_r1_toggle_t s_r1t; static int s_r1_live, s_r1_inject;`
* Neue Aufrufe:
  `void re15_touch_pc_r1_phase(int live)` — EIN-TICK-Freigabe, wird vom naechsten
  `pad_bits` verbraucht; `int re15_touch_pc_r1_latched(void)`;
  `void re15_touch_pc_r1_inject(int down)` — Messhaken fuer das Eingabeskript.
* In `re15_touch_pc_pad_bits` nach `tp_bits_locked` (`:273`):
  Finger = `(bits & TP_R1) || s_r1_inject`; `re15_r1_toggle_step`; R1-Bit ersetzen;
  `s_r1_live = 0; s_r1_inject = 0`. Der fruehe Ausstieg `:269` bleibt davor.
* `re15_touch_pc_event` (`:362-366`): in allen drei `tp_release_all`-Zweigen zusaetzlich
  `re15_r1_toggle_reset`. Ebenso in `re15_touch_pc_init` und am Ende von `tp_selftest`.
* Nur Hauptthread: `pad_bits` und `draw` laufen dort, der Finger-Watch fasst die Raste nicht
  an. Kein zusaetzlicher Mutex.

### Schritt 3 — Phasen-Riegel in der Engine

Neue Funktion `int re15_player_pad_live(void)` (neue Datei `engine/src/pad_phase_common.c`,
Deklaration in `include/re15_player.h`). 0, sobald eine Zeile zutrifft:

| Bedingung | Warum |
|---|---|
| `re15_menu_is_open() \|\| re15_menu_gameplay_frozen()` | R1-Flanken im Menue: `menu_common.c:187`, `:1572`, `re15_itembox.c:449` |
| `re15_item_modal_active() \|\| re15_discard_active()` | Freeze-Returns `game_step_common.c:1019`, `:1030` |
| `g_re15_pauseflags & (RE15_PAUSE_PLAYER \| RE15_PAUSE_PAD)` | Text, Tuer-/Raumblende; Original @0x80031c78, @0x80030514 |
| `re15_room_transition_active() \|\| g_room_change.pending` | `room_common.c:153`, `:189` |
| `g_scd.player_mode == 2 \|\| g_scd.letterbox_countdown != 0` | Cutscene, dieselbe Bedingung wie `game_step_common.c:1178` |
| `re15_player_is_dead() \|\| re15_death_presentation_active()` | Tod |

Bewusst NICHT im Riegel: Treffer, Knockdown, Griff, Treppe, Klettern. Dort liest der
Dispatcher das Pad zwar nicht, aber die Raste soll stehen bleiben — Fall H misst, dass die
Waffe nach dem Treffer von selbst wieder hochkommt (32 Bilder, wie beim Halten).

### Schritt 4 — `main.c`

Unmittelbar vor `re15_input_tick()` in der Spielschleife (`main.c:4317`):

```c
re15_touch_pc_r1_phase(re15_player_pad_live() && !re15_debug_menu_open() &&
                       pc_pad_config(RE15_PAD_BIT_R1) == RE15_PAD_BIT_R1);
```

Die letzte Bedingung haelt den Umschalter aus den Belegungen heraus, in denen R1 nicht
reines Zielen ist (TYPE C: `k_pad_remap[2][1] = 0x0800`, `main.c:2229`).
Die Front-End-Schleifen (`main.c:1583, 1600, 1732, 2158, 2602, 2849, 2855, 2961, 3017`)
rufen die Freigabe nicht auf — dort ist R1 damit automatisch Halte-Taste und die Raste faellt.

### Schritt 5 — Eingabeskript (`input_pc.c`)

Neuer Skriptbuchstabe `P` = "Finger auf dem Overlay-R1". `script_parse_once` vor die
Overlay-Zeile `:373` ziehen; im Tick mit `P` `re15_touch_pc_r1_inject(1)` rufen; nach der
Skript-Ersetzung `:404-407` das R1-Bit des Overlays wieder einODERn. Ohne `P` und ohne Finger
ist dieses Bit 0, bestehende Skriptlaeufe aendern sich nicht.

### Schritt 6 — Anzeige

Bestehender Stil (`touch_overlay_pc.c:538-541`): `base_idle` 255,255,255,42 / `base_down`
…,120 / `line_idle` …,130 / `line_down` …,230; Schrift 170 bzw. 240.
Im Zweig `K_RECT` (`:600-612`) fuer den Knopf mit `bit == TP_R1`:

* `down = (held & TP_R1) || s_r1t.latched` -> Fuellung `base_down`, Rahmen `line_down`,
  Schrift 240. Der Knopf sieht gerastet so aus wie gedrueckt, weil das Bit gesetzt IST.
* Zusaetzlich bei `latched`: ein zweiter Rahmen `tp_rect_outline` in `line_down`, um `2*t`
  nach innen gerueckt. Keine neue Farbe, keine neue Zeichenfunktion.

### Schritt 7 — Riegel

`tests/unit/probes/r30_android-r1-toggle.cmake`: Include-Pfad auf
`${CMAKE_SOURCE_DIR}/platform/pc/src`, in der Sonde `#include "touch_r1_toggle_pc.h"` und
die lokale `phase_live()` durch `re15_player_pad_live()` ersetzen, dann
`add_test(NAME unit_r30_android_r1_toggle COMMAND probe_r30_android_r1_toggle)`, TIMEOUT 60.
`tp_selftest` um die Raste erweitern (Freigabe vor jedem `pad_bits`): Tipp -> 0800, Finger
weg -> 0800, zweiter Tipp -> 0000, Raste + Viereck-Finger -> 8800, ohne Freigabe -> 0000.

### Schritt 8 — Abnahme

| Pruefung | Soll |
|---|---|
| `ctest -R unit_r30_android_r1_toggle` | `RIEGEL-GRUEN`, Fall A 10/10/1 in HALTEN und RASTE |
| `RE15_TOUCH_OVERLAY=1 RE15_TOUCH_SELFTEST=1` | `SELFTEST RESULT ok=<21+neu> fail=0` |
| Echtlauf wie M4, aber `RE15_INPUT_SCRIPT="W2,S1,W40,P0.1,W4,P0.1,W3"` + `RE15_WAFFEN_LOG` | `pad=0800` vom ersten bis zum zweiten `P`, Phasenfolge und Bildzahlen wie M4 |
| `RE15_FRAMEDUMP` in einem gerasteten Bild | R1 mit Doppelrahmen, gegen M4-Bild (R1 im Ruhestil) |
| Android: `release/build_android.sh --version <v>`, Emulator 2400x1080 | `adb shell input tap 2247 87` rastet, `… tap 2000 801` feuert, zweites `tap 2247 87` senkt; `adb logcat -s re15` |

Android-Bau: `build_android.sh` verwirft `app/.cxx` vor jedem Lauf, die neue
`engine/src/pad_phase_common.c` wird also vom GLOB erfasst. `platform/android/README.md`
(Tabelle "R1 = Zielen (halten)") anpassen.

---

## 6. Risiken und Nebenbefunde

**R1 — Raste faellt beim Inventar.** Gewaehlt, weil der Port beim Schliessen des Inventars
die Zielphase nicht zuruecksetzt (N1) und mit stehender Raste der Messer-Latch an der
Browning klebt (Fall D). Alternative "Raste ueberlebt das Inventar" setzt N1 voraus.

**R2 — Belegung TYPE C.** Zielen liegt dort auf R2 (`k_pad_remap[2][8] = 0x0200`), das
Overlay hat weder L2 noch R2. Zielen ist mit TYPE C auf dem Touchscreen heute schon
unerreichbar.

**R3 — Aktion bei stehender Raste.** Viereck feuert, Tueren und Untersuchen sind gesperrt
(`game_step_common.c:1954`). Das ist Original-Verhalten beim Zielen; mit der Raste haelt es
an, bis der Nutzer R1 erneut antippt. Deshalb die Anzeige in Schritt 6.

**Nebenbefunde am Port, unabhaengig vom Umschalter, mit gehaltenem R1 schon heute
erreichbar — eigener Auftrag:**

| Nr | Befund | Messung | Original |
|---|---|---|---|
| N1 | Inventar schliessen setzt das Kommandoregister nicht zurueck; Zielphase und Klassen-Latch ueberleben | M2 Fall D HALTEN: Phase 2, `melee_latch=1`, Waffe 3 | `sb zero,0x800aca58` @0x8001cbdc |
| N2 | Tod waehrend des Zielens: die Zielpose ueberdeckt die Todesanimation | M7; M2 Fall I: Zielphase aktiv 40/40. `re15_player_death_cmd3` ruft `re15_player_aim_interrupt` nicht, der Render-Override `main.c:7616` hat kein Todes-Gate | cmd 3 @0x80012ef0-efc ersetzt cmd 1 |
| N3 | Cutscene waehrend des Zielens: Zielphase bleibt eingefroren | M2 Fall F: 40/40 | cmd 4 @0x80041bb0 bzw. Pad-Kappung @0x80030514 |

N2 und N3 werden mit der Raste haeufiger sichtbar, weil die Pose laenger steht.

**Eigener Fehler im Messlauf.** M1 habe ich mit `taskkill /IM re15_pc.exe /F` beendet. Das
trifft JEDEN Prozess dieses Namens, also auch Laeufe anderer Agenten, falls zu dem Zeitpunkt
(2026-09-27 ca. 23:48) welche liefen. Alle spaeteren Laeufe enden ueber `timeout` auf dem
eigenen Prozess.

---

## 7. Nicht belegt

* Ende-zu-Ende mit echtem Finger oder Maus: nicht gefahren. Belegt sind die beiden Haelften
  getrennt (M1 Finger -> Bit, M2/M4 Bit -> Spiel). Eine Maus-Automatik habe ich auf dem
  geteilten Desktop nicht eingesetzt.
* Android-Geraet und Emulator: in dieser Phase nicht gestartet. Die Emulator-Koordinaten in
  M3 stammen aus der Layout-Arithmetik, nicht aus einem Tipp.
* Zweck der Original-Leser 0x8004c470, 0x8004cff8, 0x8004d344, 0x80020c98 und des
  virtuellen Bits 10.
* N3: welche der beiden Original-Ursachen (cmd 4 oder Pad-Kappung) in einer bestimmten
  Szene greift.
* Die Sonde fuettert fuer BEIDE Waffen die Cliplaengen der Browning-Bank (PL00W03). Die
  Messer-Ziehdauer in Fall D (32 Bilder) ist deshalb ein Sonden-Wert; der Echtlauf M4 misst
  mit der echten Messerbank 14 geloggte Bilder. Die Aussagen der Faelle haengen nicht daran.
* Fall E ist eine Nachstellung (Pause-Wort + die beiden Aufrufe aus `room_common.c:288/308`),
  kein echter Tuerdurchgang.
* Verhalten bei Rotation / Systemleiste auf Android (`SDL_WINDOWEVENT_SIZE_CHANGED`): der
  Plan loescht dort die Raste; ob das Ereignis auf dem Geraet im Spiel ueberhaupt auftritt,
  ist nicht gemessen.

---

## 8. UMSETZUNG (Bau Runde 30, Zweig r30/android-r1-toggle)

Arbeitsbaum `.claude/worktrees/r30_android`, Basis master d98e9639. Nachweise (unversioniert)
unter `build/r30_android-r1-toggle/bau/` des Arbeitsbaums.

### 8.1 Vorab nachgeprueft

* Tragende Adressen selbst disassembliert (`re15_disasm.py`, info/Re1.5/PSX.EXE): Pegel-Leser
  `andi 0x100` @0x80031ffc (s0 = 0x800ac768 @0x80031f40/44) / @0x800331ec, `andi 0x40`
  @0x80033308; Pause-Gates `bltz a0` @0x80031c78, `andi 0xf000` @0x80030514; Menue-Pfad
  @0x8001cb40-cb70 -> Zustand 3 -> `sb zero,0x800aca58` @0x8001cbdc (Rumpf @0x8001cbb8-cc28
  ohne Verzweigung); Task-Suspend der Status-Task FUN_80029bf8(0) @0x800460c4 (`ori 0x40`
  @0x80029c10); Plc_motion `sb v1(=4),4(v0)` @0x80041bb0, Plc_dest `sb v0(=4),4(a1)`
  @0x80041c14, Plc_ret `sb 1,4` @0x80041f90. Alle wie im Dossier, keine Abweichung.
* Ausgangszustand: `probe_r30_android_r1_toggle` im Arbeitsbaum liefert byte-gleich
  `m2_sonde.txt` (Fall A 10/10/1, C 5 und 10, H 32) — `bau/b0_sonde_ausgangszustand.txt`.

### 8.2 Gebaut (je Plan-Schritt ein Commit)

| Schritt | Datei(en) | Commit |
|---|---|---|
| 1 Logik | `platform/pc/src/touch_r1_toggle_pc.h` (= Vorschlag, header-only) | 5edc2010 |
| 3 Riegel | `engine/src/pad_phase_common.c` `re15_player_pad_live()`, Deklaration `include/re15_player.h` | 5edc2010 |
| 2, 4, 5, 6 | `touch_overlay_pc.c/.h`, `platform/pc/main.c`, `input_pc.c` | 3f124f62 |
| 7 Riegel scharf | `tests/unit/probe_r30_android_r1_toggle.c`, `probes/r30_android-r1-toggle.cmake` (`unit_r30_android_r1_toggle`) | 9b8cbbdf |
| README | `platform/android/README.md` (R1 als Umschalter, TYPE C als Grenze) | 659420fc |
| N1 | `menu_common.c` `menu_stage3_cmd_zero`, `game_step_common.c` `re15_player_cmd_zero`, `re15_enemy_ai.h`; Riegel `unit_r30_android_n1_inventar_cmd` | a8d37947 |
| N3 | `scd_vm.c` (op_plc_motion, op_plc_dest); Riegel `unit_r30_android_n3_plc_aim` | d8e4f1ed |

### 8.3 Gemessen

| Pruefung | Soll | Ist |
|---|---|---|
| `unit_r30_android_r1_toggle` | RIEGEL-GRUEN, Fall A 10/10/1 HALTEN und RASTE, C 5/10, H 32 | GRUEN; stdout bis auf die Titelzeile byte-gleich `m2_sonde.txt` (`bau/b1_*`); nach N1 aendert sich nur Fall D (Phase 0 direkt nach dem Schliessen, `bau/b2_*`) |
| Selbsttest `RE15_TOUCH_OVERLAY=1 RE15_TOUCH_SELFTEST=1` | ok=21+neu, fail=0 | **ok=32 fail=0** (11 Raste-Faelle, `bau/b3_selftest_debug.log`) |
| Echtlauf P (Browning, ROOM1030) | pad=0800 vom ersten bis zum zweiten P, Phasenfolge wie gehalten, Viereck genau ein Schuss | F431..F556: **126 von 126** Bildern mit R1-Bit; Heben ph1 F431-439 (9 geloggte Bilder), bereit F440; Viereck F464-466 -> **1** Rueckstoss-Einsatz (F465), Magazin 15 -> 14 (Inventarbild F800); zweiter P F557: Senken ph4 F557-565 (9), Phase 0 F566. Gehaltenes R1 (Skript M, ohne Overlay): Heben 9 / Senken 9 / 1 Schuss — gleiche Folge (`bau/b4_*`, `b5_ab_b1_neu_waffen.log`) |
| Raste faellt beim Inventar (Echtlauf) | nach dem Schliessen kein R1-Bit | F850 (erstes Spielbild nach dem Menue): pad=0000, ph=0 |
| Raste faellt beim Tod (Echtlauf, `RE15_KILL_AT=1100`) | R1 im Ruhestil nach dem Tod | innere Rahmenkante F1099/F1100 Helligkeit 243 (gerastet), F1101-1103 65 (Ruhe) (`bau/b7_*`) |
| Framedump gerastet | Doppelrahmen gegen Ruhestil | F700/F1000 innere Kante 243, F600/F900 65, F800 (Inventar) 111 ohne Innenrahmen (`bau/b6_r1_knopf_vergleich_*.png`) |
| Bestand ohne Overlay (master-Exe gegen neu, Skript M) | unveraendert, ausser N1 | b1 (Zielen/Feuern/Senken, 801 Zeilen): **identisch**. b2 (Inventar bei gehaltenem R1, 858 Zeilen): 9 Zeilen F628-636 verschieden = N1 (frisches Heben statt stehender Zielphase), Rest identisch |
| N1-Riegel | nach dem Schliessen Phase 0, Wort 1, Eintritts-Pose | vorher ROT (7): Phase 2, Wort 0, keine Pose, melee_latch 1 an der Browning; nachher GRUEN: Phase 0, Wort 1, Pose scharf, R1 gehalten -> bereit nach 10, melee_latch 0, 15 -> 14 |
| N3-Riegel | Plc_motion/Plc_dest beenden die Zielphase | vorher ROT (4): Phase 2, aktiv 40/40; nachher GRUEN: Phase 0, aktiv 0/40, nach Plc_ret bereit nach 10 |
| Suite | gruen | 363/363 (360 + 3 neue Riegel) |

### 8.4 Abweichungen vom Plan

1. **Selbsttest** mit 11 statt 5 Raste-Faellen (zusaetzlich: Finger weg nach dem zweiten Tipp,
   Freigabe kehrt zurueck ohne Wiederbeleben, Blitz-Tipp und Folgetick wie `adb input tap`,
   Halten ohne Freigabe).
2. **main.c bindet `touch_overlay_pc.h` nicht ein**, sondern deklariert
   `re15_touch_pc_r1_phase` lokal: der Header zieht `SDL.h` und damit die
   SDL_main-Umbenennung von `main()` nach sich, die main.c ausserhalb von Android bewusst
   vermeidet.
3. **Skriptpuffer 32 Bit**, P = Bit 0x10000 (kein Pad-Bit frei, das kein Buchstabe belegt).
4. **Echtlauf-Skript** abweichend von `W2,S1,W40,P0.1,W4,P0.1,W3`: der Abstand Eingabe-Tick ->
   Spielbild haengt vom Front-End-Verlauf ab (Lauf mit `S1` im Vorlauf: START landete in
   Bild 67; ohne: Tick-Index 524 = Bild 431). Gefahren:
   `W17.47,P0.1,W1,A0.1,W3,P0.1,W3,P0.1,W2,S0.1,W4,S0.1,W4,P0.1,W2` mit
   `RE15_GIVE=3:15 RE15_EQUIP=3` (Browning, damit Schuss und Magazin sichtbar sind),
   `RE15_DEBUG_JUMP=1030@100`, sauberes Ende ueber `RE15_KILL_AT=1100` +
   `RE15_BOOT_EXIT_AT=2` (eigener Prozess, kein taskkill).
5. **N1 ohne Raum-Teil:** `re15_player_cmd_reset` ist geteilt in `re15_player_cmd_zero`
   (Kommandowort-Statics) + `re15_prop_push_reset`. Am Inventar-Ende gibt es keinen neuen Raum;
   die Fortsetzung loescht in aca3c nur 0x40|0x8000 (`and` @0x8001cb68/@0x8001cb6c), die
   Objekt-Zaehler obj[+0x8C] und aca3c & 0x2000 bleiben stehen.
6. **N3 auch fuer Plc_dest:** derselbe cmd-4-Store steht dort @0x80041c14; der Plan nannte nur
   Plc_motion @0x80041bb0. Der Port ruft `re15_player_aim_interrupt` genau dort, wo er fuer den
   Spieler `state = 4` schreibt.

### 8.5 Nicht gemessen

* Android-Bau und Emulator (`adb shell input tap 2247 87` / `2000 801`): nicht gefahren — laeuft
  beim Paketieren. Der Code hat keinen `__ANDROID__`-Sonderfall; `pad_phase_common.c` liegt in
  `engine/src/` und wird vom GLOB erfasst (`build_android.sh` verwirft `app/.cxx`).
* Ende-zu-Ende mit echter Maus/echtem Finger: nicht gefahren. Belegt: synthetischer Finger ->
  Latch -> Umschalter -> Bit (Selbsttest) und Skript-P -> Umschalter -> Spiel (Echtlauf).
* N3 im Echtlauf (eine Szene, die waehrend des Zielens startet): nur im Riegel mit eigenem
  Skript-Schnipsel gemessen; die ausgelieferten Szenen der Suite (u.a. ROOM1170-Intro) laufen
  gruen.
* Rotation/Systemleiste auf Android (`SDL_WINDOWEVENT_SIZE_CHANGED` loescht die Raste): nicht
  auf dem Geraet gemessen.
* N2 (Tod beim Zielen, Zielpose ueberdeckt die Todesanimation) ist nicht Teil dieser Spur
  (Spur hund-tod).
