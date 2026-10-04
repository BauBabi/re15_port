# Runde 35 Spur C "zgirl": Unabhaengige Abnahme 1 (2026-10-04)

Baum `.claude/worktrees/r35_zgirl`, Zweig `r35/zgirl`, HEAD `5b385514`, Merge-Basis `154a73c1`.
Gegenstand: Nachbesserung 1 nach Abnahme 0 (`C_abnahme_0.md`: M1 blockierend, M2, M3).

Gebaut: `local_build.sh configure` und `local_build.sh build` -> `ninja: no work to do`,
`=== LOCAL-BUILD-OK (build)`. Die exe und die Testprogramme entsprechen also HEAD.
Code-Diff gegen die Merge-Basis (`git diff 154a73c1`, gelesen):
* `aot_common.c`: U1 unveraendert (`g_scd_pending_scenario = (int)d->target_cut;` unbedingt), Kommentar
  neu geschrieben (M3).
* `enemy_ai_boss_g5.c`: neue Funktion `re15_g5_boss_spawn()` setzt `s_g5.aktiv = 0`. Damit laeuft der
  vorhandene Konstruktor in `re15_g5_boss_tick` (Bedingung `s_g5_slot != slot || !g->aktiv`) im naechsten
  Tick erneut.
* `enemy_ai_common.c` `re15_enemy_spawn_root`: Im G5-Zweig (0x36 in 0x5090/5091) wird jetzt
  `re15_g5_boss_spawn(slot)` aufgerufen, +5 Zeilen.
* `scd_room_setup.c` `scd_room_reenter`: `re15_enemy_spawn_count_reset()` kommt vor den Init-Lauf
  (`scd_vm_tick`), +5 Zeilen mit Kommentar "Runde 35 Spur C".
* Neue Tests und Werkzeuge: T9 in `test_r35_zgirl.c`, `test_r35_zgirl_5090.cmake`, `reentry_zensus.py`.

`git diff master --stat` zeigt 172 Dateien. Grund: master ist inzwischen bei `87cc8575` (Integration
A+K+B+E+L). Das sind Aenderungen anderer Spuren, nicht dieses Zweigs.
`git merge-tree --write-tree master HEAD` laeuft ohne Konflikt durch.

Messwerkzeug: `scratchpad/abn1c_lauf.sh`. Es nutzt eine eigene exe-Kopie `re15_pc_abn1c_<marke>.exe`,
die nach dem Lauf geloescht wird; fremde Prozesse werden nie beendet. Laufordner:
`scratchpad/c_abn1/<marke>/` mit env.txt, debug.log, state.log und Framedumps.
Scratchpad = `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`.
Die Bilder liegen nur dort; laut Auftrag wird nur dieser Bericht committet.
Bilder entstehen per RE15_FRAMEDUMP mit `SDL_RENDER_DRIVER=opengl`, also beschleunigt und ohne
SOFTWARE_RENDER.

**Ergebnis: BESTANDEN.** Punkt 1 ist erfuellt. M1 ist am echten Weg behoben und gegengeprueft, M2 und M3
sind erledigt, alle Gates halten.

---

## Punkt 1: "Was sollen Zombie Maedchen sein? Wenn es das gibt, muss es natuerlich mit portiert werden"

### 1a Was es ist
Abschnitt "Fuer den Nutzer" im Dossier: Typ 0x13 = Modell EM013, eine weibliche Zombie-Variante. Sie kommt
nur in ROOM4050/4051 vor (Labor-Schlaftrakt), hinter Tuer 6 (Cut 9) und Tuer 7 (Cut 14).
In Abnahme 0 mit `em_zensus.py` nachgefahren: genau die Records `@0x01eb4` und `@0x01f5c`. Die Bilder
unten zeigen eine Frau mit braunem Haar und Kleid. Die Frage ist beantwortet.

### 1b Portierung: echter Weg am neuen Stand (Leon laeuft zur Tuer, Aktionstaste)

**Lauf `walk6_re2`** (Spiel-Default RE2-KI). Umgebung: `RE15_DEBUG_JUMP=4050@gp
RE15_PLAYER_POS=-9300,-24100,3072,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=90
RE15_INPUT_SCRIPT=U1.2,A0.1,W0.5,A0.1 RE15_FRAMEDUMP=60-600/30:f_ RE15_EXIT_AT=600#4050`
```
debug.log [aot] DOOR FIRE slot=6 rect=(-9300,-22700,hw=1000,hh=500) target_cut=9 spawn=(-9350,0,-2600)
          [spawn-diag] Sce_em_set type=0x13 behavior=0x00 slot=0 pos=(-9900,0,1150) dir=512
          [enemy] RE2 EM013 loaded: 17 meshes, 15 bones, 31 clips -> slot 11
          [enemy] Hybrid EM13: RE1.5-Geometrie (15 Meshes) unter RE2-Rig (15 Bones, 31 Clips), 0 Kanten ohne Zuordnung
state.log F126 st=1 d=3788 @(-9900,1150) hp=98   (Anlauf)
          F286 ss1=3 gr=1 (Griff)   Spieler-HP 100 -> 80 (F307) -> 60 (F345)
          F389 ss1=5 (weggestossen/liegt)   F548 ss1=1 wieder Anlauf   475 Bilder mit t=13
```
Bild `c_abn1/walk6_re2/kontakt_walk6.png`: Cut 9 ist der Schlafraum mit Etagenbett und Umbrella-Schild.
F150/F240 Anlauf, F300/F330 Griff, F420 liegt sie am Boden, F570 steht sie wieder auf. F120 zeigt den Gang
vor der Tuer (Bodenaufschrift "FEMALE AREA").

**Lauf `walk7_re15`** (zweite Stelle, `RE15_AI_FLAVOR=re15`, `RE15_PLAYER_POS=-16000,-24750,2048,0`, sonst wie oben)
```
[aot] DOOR FIRE slot=7 rect=(-17450,-24750,hw=500,hh=1000) target_cut=14 spawn=(1550,0,-1150)
[spawn-diag] Sce_em_set type=0x13 behavior=0x00 slot=0 pos=(1600,0,4700) dir=1024
[enemy] EM13 loaded: 15 meshes, 15 bones, 42 clips -> slot 11
F126 hp=80 (INIT 50..81)  F135 ss1=2 Anlauf  F329 ss1=3 Griff
Spieler-HP 100 -> 90 (F343) -> 85 -> 80 -> 75   F444 ss1=5 Fressen   F480 Spieler-HP -1
```
Bild `c_abn1/walk7_re15/kontakt_walk7.png`: Anlauf mit vorgestreckten Armen, Griff, Leon am Boden,
Game-Over-Kamera mit dem fressenden Maedchen.

**Lauf `kill6_re2`** (Tod und Kill-Flag): `RE15_GIVE=8:7,22:30 RE15_EQUIP=8`, Skript mit Zielen/Feuern,
`RE15_FIRE_AOT=6@900#4050` fuer den zweiten Eintritt.
```
F183 st=3 hp=-1 (Schrotschuss)  F228 st=7 (Leiche)
[fire-aot] slot=6 at F900 -> DOOR FIRE slot=6 -> [spawn-diag] Sce_em_set type=0x13 (Diag-Zeile steht vor dem Kill-Flag-Gate)
F901 kein t=13 mehr bis F1100 -> Gate @0x80042120-38 unterdrueckt den Spawn
```

**Wirkt die Nachbesserung auf das Maedchen?** Nein, gemessen. Ich habe `walk6_re2` mit
`EXE_SRC=re15_pc_zgirl.exe` gefahren, das ist der Bau vor N1 (05:07). Ergebnis: Lauf
`walk6_re2_alt` hat dieselben Zeilen. Gegen den Abnahme-0-Lauf `c_mess/walk6_re2` (Stand 0ab1e6f5)
verglichen: 600 von 600 Bildern sind identisch. Ein zweiter Lauf `walk6_re2_b` ist ebenfalls identisch,
der Lauf ist also deterministisch.

**ctest** `ctest --test-dir re15_port/build -R zgirl -V` (PowerShell, Log `scratchpad/abn1_ctest_zgirl.log`):
12/12 gruen. Das sind unit_zgirl_ai, unit_r35_zgirl_{zensus, killflag, tuer, ki_re15, ki_re2, tod, messer,
selbsttueren, wiedereintritt}, integration_r35_zgirl (253.98 s) und integration_r35_zgirl_5090 (144.07 s,
`900 Bilder, Boss-HP am Ende 600, x max 1994`).

**Urteil Punkt 1: erfuellt.** Die Frage ist beantwortet. Der Gegner erscheint an beiden Original-Stellen,
wenn Leon die echte Tuer mit der Aktionstaste nimmt. Er ist sichtbar und texturiert, laeuft an, packt,
macht Schaden, stirbt und bleibt tot. Das gilt fuer beide KI-Varianten.

---

## Nachpruefung der Maengel aus Abnahme 0

### M1 (war blockierend): Endkampf ROOM5090. Behoben, am echten Weg gemessen.
**Lauf `kette5090` / `kette5090_bild`**: der ganze Weg in einem exe-Lauf.
1. ROOM6030 -> ROOM5090 Cut 4 im Nordwagen. Das ist die einzige Tuer nach ROOM5090. Gefeuert per
   `RE15_FIRE_AOT=3@30#6030`, weil es eine Kreuz-Raum-Tuer ist.
2. Danach ausschliesslich die Aktionstaste, die Selbst-Tueren werden zu Fuss erreicht. Leon laeuft durch
   den Nordwagen (Rand z=4900 frei laut SCA) zu Tuer 0. Im Mittelwagen biegt er 48 Einheiten links ab, um
   an SCA-Kasten 16 vorbeizukommen (x 6900..11175, z -9250..-5675), biegt zurueck und erreicht Tuer 2.
   Danach geht es geradeaus in den Suedwagen.

Umgebung: `RE15_DEBUG_JUMP=6030@gp RE15_FIRE_AOT=3@30#6030 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=W1,U14,A0.1,W1,UL0.0333,U9.23,UR0.0333,U3,A0.1,W1,U12
RE15_EXIT_AT=2000#5090`
```
[aot] DOOR FIRE slot=3 rect=(-27200,-23250,hw=450,hh=1000) target_cut=4 spawn=(27200,0,4900)
[spawn-diag] Sce_em_set type=0x36 ... (1. Spawn)                     F101 hp=600
[input-script] Tick 450 -> F550 Tasten 0x8000  (Aktionstaste)
[aot] DOOR FIRE slot=0 rect=(-550,5125,hw=450,hh=1975) target_cut=9 spawn=(24700,0,-9250)
[spawn-diag] Sce_em_set type=0x36 ... (2. Spawn)                     F551 hp=600
[input-script] Tick 852 -> F952 Tasten 0x8000  (Aktionstaste)
[aot] DOOR FIRE slot=2 rect=(-550,-9175,hw=450,hh=1975) target_cut=14 spawn=(24800,0,-23550)
[spawn-diag] Sce_em_set type=0x36 ... (3. Spawn)                     F953 hp=600
F1133 cam=15 g=13 (Kampfstart) mo=1 x=-9000 hp=600 -> F1300 x=-4200 -> F1404 mo=3 x=-1986
      -> F1604 mo=4 -> F1785 mo=2 x=1960 -> F2000 hp=600   (Boss-HP nach F100: min 600, max 600)
```
Bild `c_abn1/kette5090_bild/kontakt_kette5090.png`:
* F500: Nordwagen vor Tuer 0.
* F600/F1000: Wagenuebergaenge.
* F1200: Kampfbeginn, der G5 hinter Leon im Feuer.
* F1400: der G5 an Leon.
* F1600 bis F2000: der G5 im Gang des Suedwagens, wie vorgesehen.

**A/B mit gleicher Eingabe** gegen die Basis-exe ohne U1 (`scratchpad/base_build`, Lauf `kette5090_basis`).
Dort feuern die Tueren, aber es gibt keinen zweiten oder dritten Spawn.
* Boss-Eintrag (Zustand, Clip, Lage, HP) Bild fuer Bild ab dem Kampfstart: F1133..F2000, **868 von 868
  identisch**.
* Leon weicht in 285 Bildern ab (F550..F1715), hoechstens um 50 Einheiten. Ursache ist die Eintrittspose
  nach dem Wiedereintritt (F550 `mo=210` statt `mo=200`, `re15_player_room_entry_pose`). Das ist die
  gewollte Folge von U1, keine Wirkung von N1.
* Ein Wiederholungslauf `kette5090` gegen `kette5090_bild` hat 0 von 2000 Bildern verschieden.

**Gegenprobe** mit derselben Eingabe und dem Bau vor N1 (`re15_pc_zgirl.exe`, nur U1; Lauf `kette5090_u1alt`):
```
2. und 3. Sce_em_set type=0x36 wie oben, aber Boss hp=0 ab F551
F1133 cam=15 mo=6 x=-9000 hp=-1 -> mo 7 / 8 / 10 bis F2000, x bleibt -9000   (Tod ohne Treffer)
```
Damit erklaert der Fix den Befund. Vorbedingung im Protokoll: zweiter Spawn im selben Slot bei aktivem
Modul, dann HP 0. Mit `re15_g5_boss_spawn` laeuft der Konstruktor, und das Verhalten ist dasselbe wie im
Lauf ohne Neuspawn.

### M2: Zensus und Pins. Erledigt.
* (a) Abschnitt M2a im Dossier. `reentry_zensus.py` habe ich selbst gefahren, die Typen der 14 Raeume
  stimmen mit dem Dossier ueberein:
  * 20A0: 0x25
  * 30E0: 0x10/0x11 + NPC
  * 4000: 0x29 + NPC
  * 4050: 0x13/0x18
  * 40A0: 0x18
  * 5090: 0x30 + 0x4d
  * 6030: NPC
  * kein 0x21, kein Obj_model_set

  Stichprobe am exe mit `RE15_FIRE_AOT` an der Selbst-Tuer, je 700 Bilder:
  * ROOM20A0 Slot 1: Spinnen neu gespawnt, HP 111/110 aus dem INIT.
  * ROOM4000 Slot 7: Kakerlake 0x29, HP 87.
  * ROOM40A0 Slot 10: zwei 0x18, die greifen an (Spieler-HP 60).
  * ROOM30E0 Slot 2: laeuft durch.

  Kein Absturz, `rc=0` bei allen vier (Laeufe `stich_<raum>`).
* (b) Zum Pin T9 `unit_r35_zgirl_wiedereintritt` (Ausgabe selbst gelesen):
  * ROOM5090: Erstbetritt HP 600 mit 2 Spawns, Tuer 2 -> Szenario 14, Spawns dieses Eintritts 2, nach dem
    Neuspawn HP 600, Kampfstart ohne Routine 3.
  * ROOM4050 Tuer 6 sechsmal: Zaehler je 1.
  * T9 laeuft ohne Wurzelbewegung (keine EM36-Bank geladen) und prueft deshalb nur HP und Tod. Dass der
    Boss anlaeuft, sichert `integration_r35_zgirl_5090` (x max 1994, gefordert >= -7500).
  * Beide Pins unterscheiden die Faelle. Mit dem Bau vor N1 faellt die HP-Pruefung, siehe die Gegenprobe
    oben: hp 0, danach -1.

### M3: Kommentar. Erledigt.
`aot_common.c` sagt nicht mehr "BEWUSST NICHT verallgemeinert". Der neue Text nennt die Messung und die
zwei nachgezogenen Zustaende.

---

## Gates

### G1 Suite: erfuellt
Das Dossier enthaelt woertlich `=== LOCAL-BUILD-OK (all) — Tests 489/489` (>= 478).
Beleg im Baum ist `re15_port/build/local_build_ctest.log` (07:39:43): `100% tests passed, 0 tests failed
out of 489`, 489x `Passed`, kein Failed, Timeout oder Not Run. Die letzte Code- oder Test-Aenderung ist
`e0ef6c7e` von 07:10:39, die exe stammt von 07:11:58. Das Log ist also neuer als der Code. Mein Bau
meldete `no work to do`. Die 12 zgirl-Tests habe ich selbst gefahren (gruen). Kein Fenster-Haken war rot,
deshalb musste keiner nachgefahren werden.

### G2 @0x-Gate: erfuellt. Stichproben selbst disassembliert, richtige Binaerdatei, Sprungziele verfolgt.
1. **PSX.EXE Sce_em_set:**
   * `800421c8 lbu v0,2(s2)` / `800421d0 sb v0,130(s0)` (s0 = neues Entity, +0x82)
   * `800421e0 sw zero,4(s0)`: +0x4..+0x7 = 0. Stimmt.
2. **RE2 EM36:** ausgeschnitten aus `shared_assets/RE2/CDEMD0.EMS @0x6C0000`, 23476 Byte,
   md5 `b57f8315dc8ae5583df13b4699395f6d` = Dossier. Mit `re2_disasm.py` und `RE_OVERLAY_DIR` gelesen:
   * `80100164 lbu v0,4(s3)` / `8010016c sll v0,v0,2` / `80100178 lw v0,21964(at)` (0x801055cc)
   * Tabelle `@0x801055cc` = {0x801003cc, 0x80100784, 0x801025bc, 0x80102bbc, 0x80103834, 0, 0, 0x80103878}
   * Konstruktor:
     * `801003d8 addiu v0,zero,1` / `801003e8 sw v0,4(s0)` (Bytes `04 00 02 ae`)
     * `801003fc addiu v0,zero,600` / `80100400 sh v0,342(s0)` (Bytes `58 02 02 24 56 01 02 a6`)
     * `80100414 addiu v0,zero,400` (easy)

   Stimmt Byte fuer Byte.
3. **PSX.EXE Spawn-Zaehler:**
   * `8003f014 sb zero,-13746(at)` (= 0x800aca4e) liegt in FUN_8003ef6c. Die Funktion beginnt bei
     `8003ef6c addiu sp,sp,-24`, und bis 0x8003f014 gibt es kein `jr ra`.
   * Aufgerufen von `80039a00 jal 0x8003ef6c` im Raumlader FUN_800396fc.
   * Den Raumlader ruft `8001d988 jal 0x800396fc` unbedingt. Davor steht `8001d968 beq v1,v0,0x8001d988`,
     es wird nur die Stage verglichen.
   * Leser:
     * STAGE1 `801022c4 lbu v0,-13746(v0)` / `801022cc sltiu v0,v0,0x5` und `80105ea4` / `80105eac`
       (dasselbe Muster).
     * STAGE4 dieselben bei `80102278` / `80105e58` (-0x4c).

   Stimmt.
4. **Kraehen-Schwarm (Dossier OFFEN):** `8003ed84 sh zero,-13744(at)` (0x800aca50) in FUN_8003ecec, die
   von `8003ef84 jal 0x8003ecec` in derselben Raum-Init gerufen wird. Der Eintrag unter OFFEN ist also
   richtig belegt (siehe H1).

Der Diff (`+`-Zeilen in re15_port) enthaelt keine Treffer fuer deferred, tunable, interim, for now,
faithful, plausibel, TODO, approx oder getenv. Jede neue Verhaltenszeile traegt ihre Adresse:
* `@0x800421e0` / `@0x801003CC` / `@0x801003fc` in enemy_ai_boss_g5.c und enemy_ai_common.c
* `@0x8003f014` / `@0x801022c4` / `@0x80105ea4` in scd_room_setup.c

Die Commit-Message von `5b385514` nennt dieselben Adressen und Bytes.

### G3 Fix erklaert den Befund: erfuellt
Siehe M1: Gegenprobe (vor N1: hp 0, dann -1, Todesclips), Fix (HP 600, Boss laeuft an) und Basis ohne U1
(Boss Bild fuer Bild gleich). Die Vorbedingung des Fixes (Modul aktiv, gleicher Slot, `op_sce_em_set`
setzt `a->hp = 0`, scd_vm.c:3858) steht im Code. Die Wirkung im Protokoll ist hp=0 ab dem zweiten Spawn.

### G4 Pfad- und Vertrags-Gate: erfuellt
* `git diff 154a73c1 --name-only` enthaelt kein release/, kein platform/android/, kein shared_assets/PSX/
  und keine Edits an tests/unit/CMakeLists.txt oder tests/integration/CMakeLists.txt.
* Gemeinsame Dateien nur mit kleinen Haken:
  * aot_common.c: 1 Codezeile plus Kommentar
  * enemy_ai_common.c: +5
  * scd_room_setup.c: +5, mit Kommentar "Runde 35 Spur C"
* enemy_ai_boss_g5.c bekommt eine neue Funktion mit 19 Zeilen (Modul-Datei, nicht in der Liste der
  gemeinsamen Dateien).
* Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots oder Ereignisse belegt.
* Tests stehen in `tests/unit/probes/r35_zgirl.cmake`, `test_r35_zgirl.c` und `test_r35_zgirl*.cmake`.
* Der Diff an VERTRAG.md (+9) ist der Orchestrator-Hinweis aus `e7e7131c`, unveraendert seit Abnahme 0.

### G5 Tests: erfuellt
Jede Behauptung hat einen messenden Pin:
* Maedchen: T1..T7 und integration_r35_zgirl
* alle Selbst-Tueren: T8
* Endkampf und Spawn-Zaehler nach Selbst-Tuer: T9 und integration_r35_zgirl_5090

12/12 gruen.

### G6 Regressionsfreiheit: erfuellt (in Abnahme 0 nicht erfuellt)
Am echten Weg ueber beide Selbst-Tueren ist der Endkampf mit der Basis gleich (868/868 Boss-Bilder).

---

## Maengel
Keine.

## Hinweise (keine Maengel dieser Abnahme)
* **H1 Kraehen-Schwarm 0x800aca50.** Das Original nullt ihn in derselben Raum-Init (`8003ed84`, ueber
  `8003ef84 jal 0x8003ecec`). Der Port tut das nur beim Raumwechsel. Das Dossier fuehrt es unter OFFEN mit
  Adresse und naechstem Messweg (1170-Intro mit und ohne Reset vergleichen). Auswirkung: In den 46 neu
  einsteigenden Raeumen gibt es keinen Typ 0x21 (Zensus selbst gefahren). Der betroffene Stage-1-
  Wiedereintritt ROOM1170 lief schon vor Runde 35 so. Es ist also keine Regression dieses Zweigs.
* **H2 Zusammenfuehrung mit Spur H (O1).** Nach dem Merge gehoeren der Aufruf
  `re15_schwerkraft_8001bd60` in `re15_zgirl_ai_tick` und der Typ 0x13 in `re15_schwerkraft_seed`.
  In ROOM4050 hat beides keine Wirkung (alle 410 SCA-Zellen 0x0300, Abnahme 0 G2 Nr. 7).
* **H3 Zusammenfuehrung mit master.** merge-tree ist konfliktfrei. master traegt inzwischen 517 Tests,
  nach dem Merge gilt die neue Schranke. `test_r35_granate.cmake` (master, Birkin-Lauf) armiert ueber
  `RE15_DEBUG_SUB=4` ohne Selbst-Tuer und ist von U1/N1 nicht betroffen. Spur I (entladen) aendert
  ebenfalls Raumlade-Pfade. Der Zaehler-Reset sitzt in `scd_room_reenter`, das alle drei Ladewege
  gemeinsam nutzen.
* **H4 Kleinigkeit im Dossier.** "Fuer den Nutzer (5 Zeilen)" hat jetzt 6 Zeilen.

## Zusammenfassung
| Pruefung | Urteil |
|---|---|
| Punkt 1 (Wortlaut) | erfuellt: Typ 0x13 erklaert; Spawn an Tuer 6 und Tuer 7 ueber die Aktionstaste; KI RE2/RE1.5; Griff, Schaden, Tod, Kill-Flag; N1 laesst das Maedchen bitgleich |
| M1 aus Abnahme 0 | behoben: Kette ROOM6030 -> Tuer 0 -> Tuer 2 -> Kampf, Boss-HP 600 durchgehend, Boss laeuft an; Gegenprobe vor N1 stirbt |
| M2 aus Abnahme 0 | erledigt: Zensus nachgefahren, T9 und exe-Pin gruen und unterscheidend |
| M3 aus Abnahme 0 | erledigt |
| G1 Suite | erfuellt (489/489 im Log nach dem Code, zgirl 12/12 selbst gefahren) |
| G2 @0x | erfuellt (4 Stichproben inkl. RE2 EM36 Byte fuer Byte) |
| G3 Fix erklaert Befund | erfuellt |
| G4 Pfad/Vertrag | erfuellt |
| G5 Tests | erfuellt |
| G6 Regression | erfuellt |

bestanden = true.
