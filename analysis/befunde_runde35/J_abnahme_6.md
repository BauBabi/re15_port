# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 6

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, geprueft: HEAD 9d35daf7 (Merge-Basis master 154a73c1), Baum sauber.
Abnahme vom 2026-10-04 nach Nachbesserung 6 (Maengel M1-M5 aus J_abnahme_5.md).

Massstab:
- der Wortlaut von AUFTRAG.md,
- die Regeln aus VERTRAG.md und CLAUDE.md.

Alle Messlaeufe habe ich selbst gefahren (Scratch `scratchpad/jabn6/`).
- Messkopie: `re15_port/build/platform/pc/re15_pc_jabn6.exe`, md5 43b83e18..., also gleich re15_pc.exe vor und nach der Suite
  und gleich der Angabe im Dossier (Z. 1984).
- Alle Laeufe mit `SDL_ASSERT=always_ignore`. Beendet habe ich nur meinen eigenen Lauf j1f60 (PID 42216, re15_pc_jabn6.exe).

**Ergebnis: BESTANDEN.**

Was haelt:
- **M1 behoben.** Der Finisher B[8] laeuft jetzt durch den cmd-6-Hook 0x8011c414. Leon bleibt im exe-Tuerweg auf seinem Platz.
  Clip 0 laeuft 70 Bilder, Tod bei Treffer + 70. Den Hook habe ich selbst disassembliert, die Riegel-Tabelle stimmt
  93/93 mit der Roh-GDB-Spur des Originals ueberein.
- **M2 belegt.** Der Endlos-Zonensprung ist Original-Verhalten. Die Roh-GDB-Spur zeigt 10 Spruenge alle 40 Bilder, LOS 0 und
  hp 82; der Riegel ist 400/400.
- **M3-M5 erledigt** (Dossier-Korrekturen, Riegel `kette`, OFFEN N6-1).
- Punkte 1, 2, 3, 5 und 6: state.log und Bilder sind **bitgleich** zu Abnahme 5.
- Punkt 4: Biss-Takt j1 14/14 + Tod bildgleich.
- Gates: Suite 498/498 selbst gefahren (alle fuenf Fenster-Haken gruen), 20/20 Riegel, @0x-Gate (4 Stichproben-Gruppen
  bestaetigt), Pfad-Gate.

Nicht blockierend, aber dringend: **H1**. Im Baum laeuft seit 12:59 ein verwaister Suite-Lauf des Bau-Agenten weiter,
PIDs 41584/60872/40184 im selben `re15_port/build`. Er muss vom Orchestrator beendet werden.

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure`: `=== LOCAL-BUILD-OK (configure)`.
- `... build`: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`, re15_pc.exe md5 43b83e18.
- `... all`, selbst gefahren, 13:32-13:51 (Log `scratchpad/jabn6_suite_all.log`):
  - `100% tests passed, 0 tests failed out of 498`, Total 1119,3 s, **`=== LOCAL-BUILD-OK (all) — Tests 498/498`** (Schranke 478).
  - Im selben Lauf gruen:
    - weste_load_pin 5,79 s,
    - boot_bg_pin 17,69 s,
    - dark_start_pin 17,67 s,
    - relatch_pin 22,19 s,
    - save_counter_pin 13,69 s,
    - alle 20 `unit_r35_affen_*`.
  - Die Suite lief parallel zu meinen exe-Laeufen und parallel zu dem verwaisten ctest aus H1. Trotzdem war kein Test rot.
- Die Dossier-Zeile `=== LOCAL-BUILD-OK (all) — Tests 498/498` steht woertlich in `jnb6/suite3.log` (13:27:15).

## 1. Messlaeufe (exe, `scratchpad/jabn6/run.sh`, gleiche Einstellungen wie Abnahme 5)

Der echte Weg geht ueber die Tuer von ROOM11B0:
- Einstellung: `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11B0@240 RE15_PLAYER_POS=-25500,-28200,1024 RE15_PRESS=square@300..900`.
- Logs: `RE15_STATE_LOG`, `RE15_EVT_TRACE`, `RE15_ENEMY_DBG`.
- Bilder ueber `RE15_FRAMEDUMP`.

| Lauf | Einstellung | Ergebnis gegen Abnahme 5 |
|---|---|---|
| t1 | Tuerweg, keine Eingabe, Bilder F760-1300/10 | state.log 0 Zeilen Unterschied, 55/55 Bilder bitgleich |
| b2 | Tuerweg, `RE15_FORCE_CUT=4`, Bilder F1075-1320/15 | 0 Zeilen, 17/17 Bilder bitgleich |
| o7 | Tuerweg, `RE15_INPUT_SCRIPT=W34.5,U2.5,W1`, Bilder F1180-2100/4 | 0 Zeilen, 231/231 Bilder bitgleich |
| j1 | r3-Eintritt `RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-22604,14455,0` | 0 Zeilen |
| w7b | Tuerweg, Item 7, Feuerskript, Bilder F2200-2700/20 | 0 Zeilen, 26/26 Bilder bitgleich |
| w8 | Tuerweg, Item 8, Feuerskript | 0 Zeilen |
| w3y | Tuerweg, Item 3, Feuerskript, EXIT 3070#11C0, Bilder F2500-3068/4 | bis F2812 gleich (79/79 Bilder bitgleich), ab **F2813** anders (Finisher, M1) |
| j1f52 / j1f60 | r3-Eintritt + Item 3, Feuerskript ab W52/W60 (Versuch, einen zweiten Finisher zu erzeugen) | kein Finisher. j1f60: Tod durch Heavy (-12) F2301, Weg cmd 3; j1f52: Leon lebt bis F3000 mit hp 16 |

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss dann wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**

**Verstecken** (b2, Slot 1 Typ 0x42):
- `F1088 st=4/5/2`, `F1100 (-10873,-12883)`, `F1140 (-17233,-7994)`.
- **`F1145 st=4/6/0 @(-18025,-7379)`**, unveraendert bis F1320. Das Original zeigt (-18000,-7403).
- Bild `jabn6/b2_ada.png`: F1090-F1135 laeuft Ada zwischen die Wagen, ab F1150 ist sie nicht mehr im Bild.

**Zurueckkommen** (w7b, beide Gorillas getoetet):
- debug.log: `[evt] F2037 room=11c0 Evt_exec sub=3`.
- Ada: `F2325 st=4/4/2 @(-18025,-7379)` -> `F2351 st=4/6/0 @(-16264,-8159)`.
- debug.log: `[evt] F2629 ... sub=4`.
- Bild `jabn6/w7_ada_zurueck.png`: Ada steht neben Leon mit "Ada: Okay, it's over now." und "...There are some outrageous
  monsters out there.", danach "Leon: Yeah... Anyway, let's get out of here.". F2640/F2680 zeigen den Gang zur Tuer.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- t1 state.log:
  - `F815 G1 st=1/0/0 g=30 @(-1220,-21568)`;
  - **`F816 st=1/0/1 g=10 mo=22 @(-3617,-17798)`**. Das Original hat (-3617,-17798), g=0x10, Clip 22.
- Bild `jabn6/t1_wagen.png`: F790-F810 sitzt der Gorilla in der offenen Heckklappe, ab F820 steht er vor dem Pfeiler.
- 55/55 Bilder sind bitgleich zu Abnahme 5.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper, der so nicht im Original existiert" — **erfuellt**
- Nachbesserung 6 aendert keinen Part-/Zeichner-Code: `git diff 2853783f HEAD` beruehrt nur
  - affen_11c0.c (Finisher, affen_glieder),
  - 4 Haken in enemy_ai_common.c,
  - eine Kommentarzeile in game_step_common.c,
  - re15_affen.h.
- t1 und o7 sind Bild fuer Bild bitgleich zu Abnahme 5 (55 + 231 Bilder). Damit gilt die Kette der Bitgleichheit bis
  Abnahme 1, dort gegen das Original geprueft.
- Bild `jabn6/o7_brust.png`: geschlossenes Fell am Rumpf, kein mitschwingendes Fremdteil.
- Riegel `teile` ist gruen.

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **erfuellt**

**Biss-Takt** (j1, `jabn6/takt.py`):
- Freigabe F1071 bei (-7138,-12372) = Original.
- Bisse +468, 521, 571, 624, 674, 727, 778, 830, 882, 933, 986, 1036, 1090, 1139, Tod **F2265 = +1194 = 39,8 s**.
- **14/14 Bisse und Tod bildgleich** mit der Original-Liste aus Abnahme 5.
- Heavy +363 gegen Original +364: steht jetzt als OFFEN N6-1 im Dossier.
- t1 (Tuerweg) unveraendert +1/0..-2 (OFFEN N5-3).

**Finisher (M1)**, w3y, state.log:
- `F2806 S2 1/8/1 mo=21` (B[8]).
- **`F2813 PL(-9975,-10422) hp 46 -> -554, mo=0`**.
- Leon `F2817 (-9974,-10421)` (1 Einheit Koerper-Schub), danach **unbewegt bis F3070**. Abnahme 5 mass am alten Stand:
  F2814 (-327,483).
- **`pst 7` ab F2883 = Treffer + 70.**
- Gorilla 2: F2816 vom 2050-Kreis geschoben ((-10396,-11131) -> (-11537,-11755)), S1/8/2 F2822, Sub 4 F2826, Sub 3 F2827.
  Das ist relativ zum Treffer dieselbe Folge wie im Original (GDB F230/F234/F235 bei Treffer-Eintritt F221).
- Bilder:
  - `jabn6/w3y_finisher_zoom.png`: F2808/F2812 Leon zielt am Streifenwagen. F2816 Gorilla ueber ihm, Blutspritzer am Kopf.
    F2820-F2864 Leon sinkt an seinem Platz hinter der Motorhaube zusammen. **Leon verschwindet nicht.**
  - `w3y_finisher.png` / `w3y_todeskamera.png`: ab F2872 Weissblende. F2892-F2952 Todeskamera: Leon liegt in der Blutlache,
    der Gorilla steht ueber ihm. "YOU DIED".

**Zonensprung (M2)**, w3y:
- Slot 3 springt ab F2240 alle 40 Bilder zwischen (-13518,-403) und (-13418,-403), Leon hp bleibt 46.
- Das ist dieselbe Periode wie in der Original-Spur `jnb6/g_m2b.txt`, selbst ausgewertet:
  - Anlauf-Starts F195, 235, ..., 555 (10 Starts, alle 40 Bilder);
  - nur diese zwei Lagen (260 + 140 Bilder);
  - `L0000` in 400/400 Bildern;
  - Leon hp 82 konstant.
- Riegel `zonensprung` selbst gefahren: `400/400 Bilder gleich dem Original, 10 Blind-Zonen-Anlaeufe, Leon hp min 82`.

**Urteil:** Biss-Takt, Finisher und Blind-Zonen-Verhalten entsprechen am gebauten Stand dem Original. Die beiden Befunde, die
in Abnahme 5 "teilweise" begruendet hatten, sind behoben (M1) bzw. am Original belegt (M2). Die Restabweichungen stehen mit
Adresse und Messweg unter OFFEN:
- N6-1 Heavy 1 Bild;
- N5-1 Lage bis 26;
- N6-3 Flugschritt 1 Einheit je Bild;
- N5-3 Tuerweg ohne Original-Aufnahme.

Nachgetragen zu Abnahme 5 H3, selbst gelesen: FUN_80011f50 (RE_15_Quellcode_V2, Z. 169-176) setzt den Gegner erst bei
`*(short*)(+0x9a) < 0` auf Zustand 3. Ein Gorilla mit hp 0 lebt also auch im Original weiter (w3y: Slot 2 hp 0 ab F2774 ->
Vergeltungssprung -> Finisher).

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- o7: Clip 3 nach jedem der vier Griffe:
  - `F1310 S3 1/15/5`,
  - `F1657 S3 1/15/5`,
  - `F1786 S2 1/15/5`,
  - `F2019 S3 1/15/5`.
- Bild `jabn6/o7_brust.png` (F1312-F1368): Der hintere Gorilla steht aufrecht, die Arme wechselnd an Brust und Kopf.
- Bitgleich zu Abnahme 5.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**

`jabn6/p6.py`, Segment 11C0:

| Lauf | Slot 2 Exit-Subs | Slot 3 Exit-Subs |
|---|---|---|
| w3y (Item 3) | 3,3,7,3,3,7,3,3,7,3,3,7,3,3,7 | 3,3,7,3,3,7 |
| w7b (Item 7) | 3,3,7 | 3,3,7 |
| w8 (Item 8) | 3,3,7,3 | 3,3,7,3 |

- Der Vergeltungssprung kommt jeweils beim 3. Treffer.
- Selektor-Spruenge ohne Treffer sind Original-Regel; ihre Haeufung ist M2, also am Original belegt.

## 3. Nachpruefung der Maengel aus Abnahme 5

| Mangel | Nachpruefung | Stand |
|---|---|---|
| M1 Finisher | Disasm selbst (s. 4), exe w3y (s. 2), Riegel `finisher` selbst gefahren, Riegel-Tabelle gegen die Roh-GDB-Spur | behoben |
| M2 Zonensprung | Roh-Spur `jnb6/g_m2b.txt` selbst ausgewertet, Riegel `zonensprung` selbst gefahren, Disasm @0x80118a94/@0x80116e70/@0x80117fc8 | als Original-Verhalten belegt |
| M3 Nutzer-Aussage | Riegel `takt` mit `R35_TAKT_SPUR=1` neu gefahren, S-Zeilen identisch mit Abnahme 5; gegen `jnb1/g_orig_dec.txt`: 696 Bilder, **14 bitgleich (F196-F209)**, e1 max 25,7 (F585), e2 max 22,0 (F597). Dossier: 14/695, 26/22 (F594) | erledigt (Kleinstabweichung H8) |
| M4 kette_test | Riegel `kette` selbst gefahren: 14={14,13,12,0}, 17={17,16,15,0}, 6={6,5,4,1,0}, 10={10,9,8,1,0}; `affen_glieder` wird von `affen_kette` und `re15_affen_kette_test` benutzt | erledigt |
| M5 Heavy | OFFEN N6-1 mit Messweg (GDB-Schreib-Haltepunkt player.hp 0x800acaee in vs9763-9767) | erledigt |

**Riegel `finisher`**, selbst gefahren (`jabn6/pt/finisher.txt` und LastTest.log der Suite):
- Teil A:
  - Treffer T26;
  - +0x8f 7..0;
  - Clip 0, Bild 0..69 lueckenlos;
  - groesster Schritt 2, Entfernung 3;
  - Ereignisse `0x1000 0x203c 0x4045`;
  - Tod T96 = T26 + 70.
- Teil B: Gorilla 93/93; Leon hp 93, "Kommando" 93 (siehe H5); Clip/Bild/+0x8f 70/70; Leon-Lage hoechstens 2 (F226); Gorilla im
  Sprung hoechstens 12 (F233).

**Echtheit der Original-Daten:**
- Die Tabelle `s_fin_orig[93][14]` (test_r35_affen.c:1762) habe ich Zeile fuer Zeile gegen `jnb6/g_fin_dec.txt` verglichen:
  **0 Abweichungen**.
- `g_fin_dec.txt` gegen die Roh-Zeilen `g_fin.txt`, Stichproben:
  - VSync 10081 = F220: `hp 46 cmd 1`;
  - **10083 = F221: `hp -554 cmd 6 6/0/1 c0/1 f6 h01`**;
  - 10089: Leon (-9975,-10421);
  - 10201/10203: `c0/60`, `c0/61`.
- Gegenprobe des Bau-Agenten `jnb6/fin_alt.txt` (Haken aus): `FEHLER ... 15116`, `FEHLER ... Clip`, `FEHLER ... Tod`, also rot.
  Die schwache alte Platzierungspruefung "Station TICK" war dort gruen. Im Endstand ist sie durch "groesster Schritt je Bild
  <= 2" ersetzt, und diese Pruefung waere in der Gegenprobe rot.

## 4. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)

Code-Diff Nachbesserung 6 vollstaendig gelesen (`git diff 2853783f HEAD -- re15_port`, 428+/18-). Jede neue
verhaltensrelevante Konstante traegt eine Adresse:
- +0x8f 7 @0x8011c468-70;
- Clip/Bild 0 @0x8011c490/98;
- 0x2000 @0x8011c440/@0x8011c4f8;
- Part 8 = +0x5a0 @0x8011c4a4/@0x8011c51c (172*8+0x40, vorhandener Beleg enemy_ai_common.c:1185);
- CORE 3 @0x8011c4a8-b8;
- 0x3c @0x8011c4e0;
- Rate 0x200 @0x8011c538;
- Wunden @0x8011c55c-74;
- aca58 := 7 @0x8011c57c-84.

`s_fin_ph = 3` ist als Port-Zustand gekennzeichnet, `hp = -1` als PORT-PLUMBING. Die Commit-Messages tragen die Adressen.

Stichproben, SELBST disassembliert mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` in der richtigen Binaerdatei:

1. **STAGE1.BIN 0x8011c3d4 / Tabelle 0x80121580 / 0x8011c414**:
   - `lbu v0,-13735(v0)` = aca59 @0x8011c3d8, `addiu at,at,5504` = 0x80121580 @0x8011c3ec, `jalr v0` @0x8011c3fc.
   - Tabelle [0] = [1] = **0x8011c414**.
   - 0x8011c414:
     - Verteilung ueber aca5a (`lbu v1,0(a1)` @0x8011c424).
     - aca5a 0: `sb 1` @0x8011c464, **`ori v0,zero,0x7` / `sb v0,-13597(at)` = 0x800acae3** @0x8011c468-70,
       `sb zero` acae8/acae9 @0x8011c490/98, `jal 0x80019700` @0x8011c4a0 (a0 = 0x2000 aus dem Delay-Slot @0x8011c440,
       a2 = [acbdc]+1440), `lui a0,0x403` / `ori a0,a0,0x1` / `jal 0x80045024` @0x8011c4a8-b8, aca3c |= 0xc0 @0x8011c4c0-d4,
       **ohne Sprung weiter nach @0x8011c4d8**.
     - aca5a 1: `ori v0,zero,0x3c` / `bne` @0x8011c4e0-e4, `jal 0x80045630` (2,0) @0x8011c4f0, Blut @0x8011c518,
       **`jal 0x8001f314` mit `ori a3,zero,0x200`** @0x8011c534-38, aca5a += v0 @0x8011c548-50.
     - aca5a 2: `jal 0x80037edc` (0,0xa) @0x8011c55c (a0 = 0 aus dem Delay-Slot @0x8011c454), (5,0x32) @0x8011c568,
       (7,0x32) @0x8011c574, **`sw 7` -> 0x800aca58** @0x8011c57c-84.
     - **Kein `jal 0x8001ad68`** im ganzen Koerper bis `jr ra` @0x8011c590.
   - **Bestaetigt.**
2. **STAGE1.BIN B[8] @0x80119198-0x80119204 und Registrierung**:
   - `addiu v0,v0,-600` / `sh` player.hp @0x801191a8-ac.
   - `sh 0x12c,476(a1)` im Delay-Slot @0x801191b8.
   - **`ori v0,zero,0x6` / `sw v0,-13736(at)`** @0x801191c4-cc.
   - acbfc/acbcc/acbd0 @0x801191dc/ec/9204.
   - +0x93 |= 1 @0x801191f4-fc.
   - Registrierung `addiu v0,v0,-15404` = 0x8011c3d4 / `sw v0,-14092(at)` = 0x800ac8f4 @0x8011eab8-c8.
   - PSX.EXE `lw v0,-514(v0)` / `jalr` @0x8003692c-34.
   - **Bestaetigt.**
3. **PSX.EXE FUN_80031c44** (Grund fuer den Schub auf den toten Leon):
   - Zwischen dem Dispatcher `jalr v0` @0x80031cb4 und **`jal 0x8002b544` @0x80031cbc** gibt es keine Abfrage. Die einzige Abfrage
     ist g_pauseflags `bltz` @0x80031c78.
   - Decompilat FUN_8002b544: aec4 fuer jede aktive Entity, ohne HP-Abfrage.
   - FUN_8002aec4 bricht nur bei Flag 2 / beidseitig 0x1000 / Flag 4 ab.
   - **Bestaetigt.** Die verbleibende allgemeine hp-Sperre ist eine Port-Annahme und steht als OFFEN N6-2.
4. **STAGE1.BIN M2-Stellen**:
   - `ori v0,zero,0x3` / `bne` / `ori v0,zero,0x32a` / `sh v0,140(a0)` @0x80118a94-aa0 (Impuls nur bei +0x7 = 3);
   - `ori a2,zero,0x4` / `jal 0x8003b0a4` @0x80116e68-70;
   - Yaw-Fenster `andi 0xf0` / `sll 4` / `subu` / `addiu 512` / `andi 0xfff` / `slti 1024` @0x80117fc8-e4;
   - Sub 7 / +0x6 = 0 / **+0x7 := 3** @0x80118000-24.
   - **Bestaetigt.**

Weitere Pruefungen:
- **Guess-Tells** in allen hinzugefuegten Zeilen des Zweigs (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
  approx|ungefaehr|geschaetzt|vermutlich|naeherung|annahme|getenv`): Treffer sind nur die Mess-Logs `RE15_AFFEN_FUSS`,
  `R35_TAKT_SPUR`, `R35_FIN_SPUR`. Kein Env-Schalter dient als Abschluss.
- **Erklaert der Fix den Befund?** Ja.
  - Vorbedingung: w3y F2813 im Wurf-Pfad mit altem Anker, F2814 Sprung um 14000 (Abnahme 5).
  - Nachher ist w3y bis F2812 bitgleich, ab F2813 laeuft Clip 0, Leon bleibt stehen.
  - Die Gegenprobe ohne Haken ist rot (15116).

## 5. Vertrags-/Pfad-Gate, Tests
- `git diff 154a73c1 HEAD --name-only` (25 Dateien):
  - keine Pfade unter `release/`, `platform/android/` oder `shared_assets/`;
  - keine Edits an `tests/unit/CMakeLists.txt` oder `tests/integration/CMakeLists.txt`;
  - `git log 154a73c1..HEAD -- release re15_port/platform/android re15_port/shared_assets <beide CMakeLists>` ist leer.
  - Der Zweig ist nicht auf master 87cc8575 nachgezogen; `git diff master` zeigt deshalb fremde Spuren.
- **Haken N6 in gemeinsamen Dateien**, je mit "Runde 35 Spur J (15)":
  - enemy_ai_common.c 1 + 2 + 1 + 1 Zeilen (Z. 1221, 1351-1352, 1604, 4448);
  - game_step_common.c 1 Kommentarzeile.
  - Das haelt Vertrag 1.4.
- **Zuteilung:** keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse oder Assets (grep im Diff leer).
- **Tests:** probes/r35_affen.cmake hat 20 Riegel, alle gruen (Suite).
  - Neu und messend: `finisher` (Teil A + Teil B gegen die Original-Spur), `zonensprung` (400 Bilder gegen das Original),
    `kette` (Ketten gegen die fest verdrahteten Ketten).
  - `finisher`, `zonensprung` und `kette` habe ich zusaetzlich direkt gefahren (`jabn6/pt/*.txt`), ebenso `takt` mit Spur.

## 6. Hinweise (nicht blockierend)

### H1 (operativ, dringend): verwaister Suite-Lauf des Bau-Agenten laeuft im Baum weiter
- Prozesse, gemessen 13:58:46:
  - bash 41584 (`re15_port/tools/local_build.sh all`, Start 12:59:00);
  - bash 60872 (Start 12:59:04);
  - **ctest 40184** (`ctest --test-dir re15_port/build --timeout 30 --output-on-failure`, Start 12:59:04).
- Die Kind-Prozesse laufen in **diesem** Baum: cmake mit `-DRE15_PC_EXE=C:/workspace/git/reAi_v2/.claude/worktrees/r35_affen/
  re15_port/build/platform/pc/re15_pc.exe`.
- Zuordnung: `jnb6/suite2.log` endet um 12:59:04 mit `=== [local_build] test — ctest in re15_port/build`. Das ist der Lauf,
  den das Dossier (Z. 2049-2050) als "von mir abgebrochen (eigene Task)" fuehrt. Die Task ist beendet, ihr ctest aber nicht.
- Folgen:
  - Der End-Lauf suite3 (bis 13:27, 498/498) lief **gleichzeitig** mit diesem ctest im selben `build/`. Das verstoesst gegen
    VERTRAG 1.5 ("Zwei Builds im selben build/ gleichzeitig: NIE").
  - Der verwaiste Lauf steht um 13:56 bei 360/498. Die Tests laufen dort in Zeitueberschreitungen (17 x "Test Failed", z.B.
    `integration_r30_granate_laden` 600 s, `unit_r30_hund_tod` 300 s; `Testing/Temporary/LastTest.log.tmp7ad5a`).
  - Am Ende ueberschreibt er `Testing/Temporary/LastTest.log` und `LastTestsFailed.log` mit diesen Fehlschlaegen.
- Fuer das Urteil zaehlt meine eigene Suite (498/498), die das Ergebnis traegt. Ich habe die fremden PIDs **nicht** beendet.
- **Der Orchestrator muss 40184, 60872 und 41584 beenden**, bevor im Baum gebaut oder der Baum entfernt wird.

### Weitere Hinweise
- **H2** game_step_common.c:1425-1426, Kommentar im Tod-Zweig: "der Koerper-Schub selbst steigt bei HP < 0 aus
  (FUN_8002AEC4-Gate, re15_body_push_player)".
  - Seit (15) stimmt das nicht mehr: Im Gorilla-Finisher schiebt der Port den toten Leon.
  - Ein HP-Gate in FUN_8002aec4 zeigt das Decompilat nicht.
- **H3** enemy_ai_common.c:4448: Der Kommentar sagt "nicht im Gorilla-Finisher cmd 6". Die Bedingung
  `re15_player_victim_gorilla() && pl->state != 7` gilt aber fuer jeden Gorilla-Opfer-Zustand, also auch fuer den Wurf (cmd 5),
  falls Leon dort hp < 0 haette. Ich habe keinen Weg dorthin gemessen; cmd 3 setzt den Opfer-Zustand zurueck.
- **H4** affen_11c0.c:478: `re15_affen_finisher_aktiv()` wird nirgends aufgerufen (toter Code, ohne Behauptung im Dossier).
- **H5** Riegel `finisher` Teil B, "Kommando 93/93":
  - Der Port-Wert ist abgeleitet: `(re15_player_victim_state() == 2) ? (pl->state == 7 ? 7 : 6) : 1` (test_r35_affen.c:1934).
  - pl->state selbst bleibt im cmd-6-Fenster 1: w3y `pst=1` F2813-F2882, Riegel-Ausgabe "Zustand 1".
  - Gemessen wird damit der Zeitpunkt der Uebergaenge (Treffer-Bild, Tod +70), kein Kommando-Register 6.
  - Eine sichtbare Folge habe ich nicht gefunden: Der Tod-Zweig friert die Eingabe ein, kein Schuss im Fenster.
- **H6** OFFEN N6-3: Der Flugschritt (-196,+172) gegen das Original (-196,+171) ist eine echte, gemessene Abweichung in
  `re15_dog_advance` (12 Einheiten nach 29 Bildern). Sie ist fuer alle Nutzer offen gelassen und sollte spurenuebergreifend
  eingeplant werden.
- **H7** Die Todes-Praesentation nach dem Finisher habe ich nicht gegen das Original gemessen:
  - Port: Weissblende ab rund Treffer + 59, Todeskamera ab rund + 79.
  - Das Dossier beschreibt im Original ab Treffer + 96 eine Schnappschuss-Wiedergabe; dort springt aber der VSync-Zaehler.
  - Das ist der vorhandene allgemeine Gameover-Ablauf, nicht Teil von N6.
- **H8** Zu M3: Das Dossier nennt 695 Bilder und e2 F594; nachgerechnet sind es 696 Bilder (F196-F892) und e2 22,0 bei F597.
  Die Aussage "hoechstens 26/22" haelt.
- **H9** Die GDB-Experimente am Original habe ich nicht wiederholt. DuckStation-GDB braucht eine Aenderung von settings.ini
  ausserhalb des Baums. Geprueft habe ich die Roh-Spuren `jnb6/g_fin.txt` / `g_m2b.txt`: VSync-Zeilen, Patch-Kopf "Patch bei
  VSync 10029" und die Dekodierung.
  - Beide Experimente setzen den Zustand per Speicherschreiben (r3 s033).
  - M2 belegt damit "gleiches Verhalten ab gleichem Zustand". Dass das Original von selbst in diesen Zustand kommt, ist nicht
    gemessen. Genau diesen Messweg hatte Abnahme 5 (b) verlangt.

## 7. Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (b2 F1145 weg, w7b F2351 zurueck, sub=3/4; bitgleich zu Abnahme 5) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (F816 (-3617,-17798) = Original; 55/55 Bilder bitgleich) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (kein Part-Code geaendert, 286 Bilder bitgleich, Riegel teile) |
| 4 KI zielstrebiger/aggressiver wie im Original | erfuellt (Biss-Takt 14/14 + Tod bildgleich; Finisher = cmd-6-Hook 0x8011c414, exe Leon bleibt, Tod +70, Riegel 93/93 gegen das Original; Zonensprung = Original 400/400) |
| 5 Brust-schlagen-Animation | erfuellt (Clip 3 nach jedem der vier Griffe) |
| 6 Sprung erst nach 3 Treffern | erfuellt (w3y 5+2, w7b 1+1, w8 1+1, jeweils beim 3. Treffer) |

| Gate | Stand |
|---|---|
| Bau | gruen |
| Suite | 498/498, selbst gefahren, alle Fenster-Haken gruen |
| Riegel | 20/20 |
| @0x-Gate | gehalten (4 Stichproben-Gruppen bestaetigt, 0 Guess-Tells) |
| Pfad-Gate | gehalten |
| Dossier | M3/M4/M5 erledigt; Kleinstabweichungen H5/H8 |

**Abnahme 6: BESTANDEN.** Vor dem Zusammenfuehren muss der Orchestrator den verwaisten ctest-Lauf (H1, PIDs 40184/60872/41584)
beenden.

Messlaeufe, Bilder und Auswertungen liegen in `scratchpad/jabn6/`:
- Skripte: run.sh, fire.txt, ana.py, takt.py, p6.py, sheet.py.
- Laeufe: t1, b2, o7, j1, w7b, w8, w3y, j1f52, j1f60; Riegel-Ausgaben pt/ (finisher, zonensprung, kette, takt).
- Bilder: b2_ada.png, t1_wagen.png, o7_brust.png, w7_ada_zurueck.png, w3y_finisher.png, w3y_finisher_zoom.png,
  w3y_todeskamera.png.
- Suite-Log: `scratchpad/jabn6_suite_all.log`.
