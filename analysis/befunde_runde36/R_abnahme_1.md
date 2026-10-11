# Runde 36 — Spur R "dachkiste" — Abnahme 1 (unabhaengig, 2026-10-11, Nachbesserung 1)

Baum `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste` @ 01248da5 (Basis 99398dc7; Abnahme 0 = 03a64b28 auf
f487909d). Gebaut mit `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)` (ninja: no work to do; die exe
16:25:06 ist unveraendert, sha256 `13ffe6a5cce82c1d…` = Stand der Abnahme 0 — die Nachbesserung aenderte keinen
Engine-Code; `probe_r36_dachkiste.exe` 17:59:16 ist juenger als `probe_r36_dachkiste.c` 17:52:04). Messkopie
`re15_pc_r36abn1.exe` neben der exe, Arbeitsordner im Scratchpad (`abn1/<lauf>/`), Startskript = Kopie von
`R_belege/lauf_nb1.sh` (Grundumgebung unveraendert). Der Bericht wird fortlaufend geschrieben.

Wortlaut (Orchestrator): (1) an der exe messen, ob Leon/Elza die Dach-Kiste obj 1 in ROOM1090/1091 bis zum Fall-Ausloeser
schieben koennen (echter Weg, jede Richtung); (2) am Original messen; (3) blockt das Original -> Port blockt an derselben
Stelle mit demselben Mechanismus (Beleg, Pin), sonst byte-true + Grenze benennen + Pin; (4) **dieselbe Pruefung** fuer
ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste? Abnahme 0 hatte Punkt 4 mit den
Maengeln M1 (Zensus 2070 unvollstaendig), M2 (Ersatzweg 2030 -> 2060 unbelegt), M3 (kein Pin fuer Punkt 4) als
"teilweise" gewertet; die Nachbesserung 1 (c92f4d53, 01248da5) behauptet, alle drei behoben zu haben.

## Code-Diff (git diff 99398dc7 -- re15_port, wirklich gelesen)

Gegenueber Abnahme 0 unveraendert: `engine/src/aot_common.c` (3-Zeilen-Haken, `gesperrt` halten + Aufruf, gleiche
Verzweigung — kein Verhalten), NEU `engine/src/dachkiste_r36.c` + `include/re15_dachkiste_r36.h` (Messschrieb
`RE15_PUSH_LOG`, ohne Env fruehes return). Geaendert in der Nachbesserung: NUR Tests — `tests/unit/probe_r36_dachkiste.c`
(Pfadausgabe ueber Vorgaenger-Zustaende, ROOM3050/3051 `soll = 1` + Rechteck-Pruefung der ersten Ausloeser-Lage gegen
r*[46] +-450, ROOM2050/2051 `soll = -1` mit Begruendung im Kommentar), `tests/integration/test_r36_dachkiste.cmake`
(drei neue Laeufe fall3050/weg2040/treppe2040), `tests/unit/probes/r36_dachkiste.cmake` (Kommentare). Einziger
Env-Schalter: `getenv("RE15_PUSH_LOG")` (Messschrieb, kein Verhalten). Guess-Tells (deferred/tunable/interim/for now/
faithful/plausibel/TODO/sieht richtig) im re15_port-Diff: KEINE Treffer.

## Gates

### Suite (gezielt, kein voller Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur|unit_re2_room1090_ab|unit_plc_back_yaw_1090|probe_1090_|unit_map_1090|unit_r35_cut10f0|unit_r36_szenenleon_abgang10f0" --timeout 900`:
```
100% tests passed, 0 tests failed out of 31   (Total Test time (real) = 18.04 sec; unit_r36_dachkiste_zensus 8.98 s)
```
`ctest --test-dir re15_port/build -R integration_r36_dachkiste --timeout 1200 --output-on-failure` (eigener Lauf,
Laufordner `re15_port/build/r36_dachkiste_wd/lauf_e3d996d9/`):
```
1/1 Test #572: integration_r36_dachkiste ........   Passed  569.48 sec
100% tests passed, 0 tests failed out of 1
```
Inhalt der sechs Laeufe (selbst gelesen): `leon` DOOR FIRE slot=1 -> room1090, 3x `obj=1 SCHUB-GESPERRT` + 3x
`TREFFER-ZELLE r3[4]: x -7256..4624 z -18560..-16403`, 0 SCHUB-OK, 0 AUSLOESER, Ende `p1 (-5960,-9000,-15482) b5`;
`elza` PL04, room10f1 -> room1091, dieselben Zahlen; `west` 2x GESPERRT an r3[2] x -9200..-6878, Record; `fall3050` 425x
`room=3050 p=1 obj=1 SCHUB-OK`, 1x GESPERRT (`r3[31]: x -27000..-25000`, Westwand), `F1383 p=1 AUSLOESER wort=0x4002
band=4 pos=(-24058,-7200,-18488)`, `zust 0->1 … 1->1 n=6`, Ende `p1 (-24058,0,-15561) b0 r(-1029,0,0)`; `weg2040`
`[aot] DOOR FIRE slot=5 rect=(5500,-19200,hw=500,hh=1000) target_cut=12 spawn=(-19750,-9000,-19850)` -> `loaded
room2040.rdt`; `treppe2040` `[stair] START dir=DOWN cur_band=5 target_band=3`, climb.txt `T420 room=2040 …
pos=(-1359,-5400,-13782) … band=3`.

### RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, info/Re1.5/PSX.EXE)
Die Kette FUN_8002bd44 / FUN_8003b558 / FUN_8002d1e8 / FUN_8002cabc war in Abnahme 0 disassembliert und stimmt. Neu zitierte
Stellen der Nachbesserung:
* Objekt-Pass `dis 0x8002c0fc 24`: `8002c0fc lbu v1,8(s2)` / `8002c104 ori v0,zero,0x4` / `8002c108 bne` (Typ 4),
  `8002c110 lbu v0,4(s2)` / `8002c118 bne` (faellt schon), `8002c120 addiu a0,s2,52` (&obj+0x34), `8002c124 lbu a2,130(s2)`
  (obj+0x82 Band), `8002c128 jal 0x8003b7f0` mit Delay-Slot `8002c12c ori a1,zero,0x1c2` (= 450), `8002c130 andi v0,v0,0x2`,
  `8002c134 beq`, `8002c13c jal 0x8002d3d8`, `8002c144-14c sb 1,4(s2) / sb zero,5(s2)`: stimmt. (Die Dossier-Zitate
  "+-450 @0x8002c128" meinen den Aufruf; die Konstante steht im Delay-Slot @0x8002c12c — Praezisierung, kein Defekt.)
* FUN_8003b7f0 `dis 0x8003b7f0 80`: `8003b890 sra t0,v0,16` (t0 = a1 = Radius), `8003b894 sll t1,t0,1` (2r),
  `8003b8a0 lhu v0,0(a1)` (Zelle+10 Typwort) / `8003b8a8-ac sll 16 / sra 28` (Bandnibble) / `8003b8b0 bne a2,v0`,
  `8003b8b8 lh a0,-6(a1)` (x0) / `8003b8c4 subu a0,a0,t0` (x0 - r) / `8003b8c8 subu v1,v1,a0` / `8003b8cc addu v0,v0,t1`
  (w + 2r) / `8003b8d0 sltu v1,v1,v0`; dito z @0x8003b8dc-f8; Rueckgabe `8003b900 lhu v0,0(a1)` = Typwort: a1 = 0x1c2 ist
  der um 450 erweiterte Rechtecktest auf die OBJEKTLAGE — stimmt mit "Absprungzelle +-450".
* Band-Wildcard `dis 0x80042c9c 20`: `80042cac lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cb8 bne -> 0x80042cd4`
  (Wildcard ueberspringt den Bandvergleich), sonst `80042cc0 lbu v1,130(s1)` / `80042cc4 lbu v0,2(s0)` / `80042ccc bne`:
  stimmt (ROOM2030 slot 8 Band-Byte 0x80).
* Treppen-count `dis 0x800435a0 12`: `800435b8 lbu v0,14(a2)` / `800435c0 sb v0,158(v1)` (Entity+0x9e): AOT-Eintrag+14 =
  Record-Byte 16 (nach Opcode+Slot) = `02` im Record @0x01B8E — "count 2 (Byte 16)" stimmt.

### Vertrag
* Bank-9-Bits/Nachrichten-IDs/Ereignisse: keine. Gelesen/gesetzt nur (9,71) im Riegel (Messhaken, vorhandenes Flag).
* Pfade (git diff 99398dc7 --name-only): analysis/befunde_runde36/R_* (+ R_belege/*), engine/src/aot_common.c,
  engine/src/dachkiste_r36.c, include/re15_dachkiste_r36.h, tests/unit/probe_r36_dachkiste.c,
  tests/unit/probes/r36_dachkiste.cmake, tests/integration/test_r36_dachkiste.cmake. Keine release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists: eingehalten. Messkopie eigener Name, kein taskkill, RE15_WINDOW_SCALE=1.

(Fortsetzung: Punkt-Messungen, Urteile, Maengel — s. unten.)

## Punkte 1-3 (ROOM1090/1091) — Stand gegenueber Abnahme 0

Code und exe sind seit Abnahme 0 unveraendert (sha256 der exe identisch, kein Engine-Diff in der Nachbesserung). Die
Messungen der Abnahme 0 (9 exe-Laeufe Leon/Elza, echter Weg und Sprung, vier Richtungen, Gehen/Laufen/schraeg: 0
angenommene Schuebe, 0 Ausloeser, Kiste auf dem Record; eigene Wiederholung am Original DuckStation-GDB Plan NORD:
`TREFFER-ZELLE a0=80123984` = r3[4], `SCHUB-GESPERRT … s4/s5=(-5960,-15482)`, 0 Fall-Ausloeser) gelten unveraendert.
In dieser Abnahme erneut gemessen: der Riegel `integration_r36_dachkiste` (eigener Lauf `lauf_e3d996d9`, s. Gates):
`leon`/`elza` echter Weg 10F0/10F1 -> Tritt -> Tuer slot 1 -> 1090/1091 -> drei Schuebe, je 3x `SCHUB-GESPERRT …
ziel=(-5960,-155xx)` + `TREFFER-ZELLE r3[4]`, 0 SCHUB-OK, 0 AUSLOESER, Ende `p1 (-5960,-9000,-15482) b5`; `west` 2x
GESPERRT an r3[2]. Mechanismus = FUN_8003b558(obj,2) @0x8002bfb0 trifft die Bruestungszelle (Band @0x8003b634-644,
u0 & 2 @0x8003b64c-660, Rechteck r = hx @0x8003b738-794), Sperre @0x8002c0b0, Ruecksetzung @0x8002c0b8-d4 — im Port
dieselbe Kette (`re15_prop_push_tick` / `re15_collision_box_blocked`), kein Verhaltens-Fix noetig, keiner gemacht.
**Punkte 1, 2, 3: erfuellt (unveraendert).**

## Punkt 4 — eigene Pruefung der Nachbesserung 1 (M1/M2/M3)

### M1 — Tuer-Zensus (berichtigt): selbst nachgelesen, stimmt
* `python -I re15_port/tools/scd_dump_room.py …/STAGE2/ROOM2070.RDT`: main00 @0x00ABA `Ifel_ck 06 00 3c 00`, @0x00AD6
  `Door_aot_set slot=4 sce=0 floor=0 … -> 11A0`, @0x00AF6 `Else_ck 07 00 26 00`, @0x00AFA `slot=4 sce=2 … -> 11A0`,
  @0x00B1C `Door_aot_set 3b 05 02 31 00 00 88 13 18 b1 e8 03 d0 07 da b2 d8 dc 76 b2 00 00 01 04 0c 05 …` = slot 5 floor 0
  -> ROOM2040 cut 12, nfloor 5, Spawn (-19750,-9000,-19850), UNBEDINGT (nach dem Else-Block, vor `Switch 13 0a 8e 01`
  @0x00B3C); ROOM2071 identisch. `stage2_tueren_2040.txt` fuehrt jetzt alle 14 Records 2070/2071 (selbst gezaehlt).
* SCA ROOM2070 Band 0 (eigener Dump `sca_dump.py ROOM2070.RDT 0`, alle 5 Regionen identisch, je 12 Zellen):
  r0[0..3] = geschlossener Ring x -20450..6500 z -9200..-3600 (Mittelgang, Innenraum z -8200..-4600), r0[4..7] = Ring
  z 3650..9100 (Suedgang), r0[8..11] = Ring x -20400..6450 z -22000..-16450 (Nordgang, Innenraum z -21000..-17450).
  Tuer slot 5 rect x 5000..6000 z -20200..-18200 liegt in der Ostwand des Nordgangs, slot 4 (-19900..-18900,
  -20250..-18250) in dessen Westwand; slot 0/1 im Mittelgang, slot 2/3 im Suedgang. "Drei getrennt ummauerte Gaenge"
  stimmt; der in Abnahme 0 vorgeschlagene Laufweg 2000 slot 3 -> 2070 -> slot 5 ist damit tatsaechlich keiner (die
  Ankunft (-18650,0,-6450) liegt im Mittelgang). Die exe-Laeufe `nb1_2000_2070m` / `nb1_2070n` des Bau-Agenten und mein
  Riegel-Lauf `weg2040` (DOOR FIRE slot=5 aus der 11A0-Ankunft) bestaetigen das.
* Stichprobe des Zensus-Werkzeugs gegen scd_dump_room.py: ROOM2020 slot 1 @0x00866 (sce 0, `Ck(3,219)=0`) / @0x0088A
  (sce 2, sonst), ROOM11A0 slot 0 @0x00F5A (`NOT(Ck(3,151)=0)`), ROOM2050 slot 0/1/2 @0x01C4A/@0x01C6A/@0x01C8A: stimmen.
* **ABER (neuer Befund, s. Maengel N1):** der im Dossier (4.3, "Fuer den Nutzer") als zweiter Zugang genannte Weg
  "ROOM11A0 slot 0 @0x00F5A ((3,151)=1) -> 2070 Nordgang -> slot 5 -> 2040" setzt (3,151)=1 voraus. Eigener Scan ALLER
  Set-Opcodes in STAGE1/2/3 (`scd_dump_room.py` je RDT, grep `22 03 97 01`): (3,151) wird NUR in ROOM2070/2071 sub02
  @0x00CE4 gesetzt; sub02 haengt an `Aot_set slot=7 sce=3` @0x00AC2 (Ereignis, dasselbe Rechteck wie Tuer slot 4, also IM
  Nordgang). Der Nordgang selbst hat genau zwei Tueren: slot 4 (-> 11A0, erst Ereignis, dann Tuer) und slot 5 (-> 2040
  Galerie); betreten wird er zum ersten Mal NUR aus ROOM2040 slot 4 @0x0131C (floor 5, Galerie). Der "zweite Zugang"
  nach 2040 ueber 11A0 ist also zirkulaer: er steht erst offen, wenn der Spieler schon in der Galerie von 2040 war. Fuer
  die Softlock-Frage (Kiste 2050 faellt, BEVOR 2040 erreicht wurde) taugt er nicht.

### M2 — Ersatzweg nach ROOM2040 ohne die Kiste: selbst an der exe gefahren, Glied fuer Glied
Der Bau-Agent nennt (4.3) den Weg 2050 slot 1 -> 2020 (Ereignis sub03 (3,219)) -> 2030 Band 9 -> slot 3 -> 2060 Band 0 ->
Tritt obj 2 -> slot 1 -> 2040 Halle und raeumt selbst ein, dass 2020, die Band-9-Etage von 2030 und der Tritt aus der
Startlage nicht gefahren sind. Eigene Laeufe (Messkopie `re15_pc_r36abn1.exe`, Grundumgebung des Riegels, je
`RE15_DEBUG_JUMP=<raum>@250` + `RE15_PLAYER_POS=<Tuer-Ankunft>`):
* **R2 ROOM2030 (Band 9, Ankunft aus 2020 slot 1: (-450,-16200,-9700) Blick 3072)** — Skript `W2,L0.37,W0.3,U3.5,W0.5,
  R0.37,W0.3,U1.0,W0.5,L0.37,W0.5,A0.1,W3,A0.1,W3`: debug.log `[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900)
  target_cut=0 spawn=(7600,0,3950)` -> `[room] PC loaded room2060.rdt`. **Glied 2030 Band 9 -> slot 3 @0x01B4C -> 2060:
  gemessen OK** (Streifen z -10350..-9000 nach Westen bis an r0[65] x -7700, Korridor x -7700..-6350 nach Sueden, Tuer).
* **R1 ROOM2020 (Band 1, Ankunft aus 2050 slot 1: (11350,-1800,-3150) Blick 3072)** — Skript `W2,L0.37,W0.3,U5.5,…`
  (Suedstreifen nach Westen bis in die Luecke x -2200..-400 zwischen den Band-1-Zellen r0[13]/r0[14], dann nach +z,
  dann nach Westen zur Tuer slot 1 x -15200..-13200 z 2800..3800): der Spieler laeuft mit 74/Bild nach Westen und
  bleibt bei **x 218** stehen (climb.txt T482..T496 `pos=(218,-1800,-33xx) rot=2016`, Pad UP gehalten), kommt also NICHT
  in die Luecke; kein `DOOR FIRE`, `EXIT_AT: Bild 1000 in Raum 2020`. Haut-Arithmetik: 218 = -250 + 450 + 18 = die
  Ostkante der Band-1-Zelle r0[15] @0x004F8 (x -2350..-250 z -3850..3300, typ 1, **u0 0x00**, u1 0x00, fl 0x13) — im
  Port blockt diese Zelle den Spieler, obwohl `collision_constrain_impl` (re15_collision.c:727 `(mask & e->u0) == 0 ->
  continue`) sie mit u0 = 0 ueberspringen muesste (Diagnose-Lauf R1b mit `RE15_COLL_TRACE=1` aus der Luecke heraus: s. u.).
  Geometrie Band 1 ROOM2020 (eigener Dump): Suedstreifen z -3850..-2550 (r0[9] Suedwand, r0[11] Zwischenwand x -400..
  13100), Nordstreifen z 1500..3300 (r0[10], r0[12]), dazwischen die Absprungzellen r0[13] (x -21000..-2200) und r0[14]
  (x -400..13100) mit u0 0x01 (Spielersperre), die Luecke x -2200..-400 traegt nur r0[15] (u0 0). Ob das ORIGINAL den
  Spieler dort passieren laesst (u0 = 0 -> keine Spielerwand nach FUN_8003b0a4-Maske 1), ist nicht gemessen; der Port
  laesst ihn NICHT passieren. **Glied 2050 -> 2020 -> 2030 ist an der exe NICHT gangbar** — der genannte Ersatzweg
  scheitert im Port bereits am ersten Raum.
* **R3 ROOM2060 (Band 0, Ankunft aus 2030 slot 3: (7600,0,3950) Blick 2048)** — Tritt obj 2 aus der Startlage
  (-3125,0,2075) in die Kerbe slot 2 @0x00E92 (x -550..1500 z -8300..-6250) schieben, klettern, Tuer slot 1 @0x00DBA
  (floor 1): erster Lauf an der Drehgeometrie gescheitert (Drehung = 11 Bilder x 96 = 1056 je 0.37 s, nicht 1024; Gehen
  75/Bild ohne Auslauf; der Spieler driftete in r0[8]); Wiederholung s. unten (R3b).
* **R1b ROOM2020 (Diagnose, Spieler per RE15_PLAYER_POS=(-1300,-3150) Blick 3072 Band 1 DIREKT in die Luecke gesetzt,
  Skript `W2,U3,…`):** beim Laden steht der Spieler sofort bei `pos=(-1300,-1800,-4318)` (climb.txt T271) — er wurde
  um 1168 nach -z herausgeschoben: -4318 = -3850 - 450 - 18 = Suedkante von r0[15] minus Radius minus Haut. Der Port
  behandelt r0[15] (u0 0x00) also als Spielerwand (Herausschieben beim Laden UND Klemme beim Gehen, s. R1). Welche
  Routine das tut, ist hier nicht abschliessend geklaert (RE15_COLL_TRACE schreibt auf stderr, die GUI-exe liefert
  keins); nach `collision_constrain_impl` (re15_collision.c:727, Maske 1 & u0) duerfte die Zelle nicht sperren —
  Hinweis fuer die Nachbesserung: entweder sperrt das Original dort ebenfalls (dann ist der Nordstreifen von 2020 nur
  aus 2030 erreichbar und der "Ersatzweg" existiert auch im Original nicht), oder der Port hat hier einen eigenen
  Defekt. In beiden Faellen ist der Satz des Dossiers "ROOM2040 bleibt erreichbar ueber ROOM2020 -> ROOM2030 ->
  ROOM2060" an der exe NICHT gedeckt.

### M3 — Pins: vorhanden, 3050 diskriminierend; 2050 "begruendete Diagnose" beruht auf einem Modell-Fehlpositiv
* `unit_r36_dachkiste_zensus` (eigener Lauf: PASS, 8.98 s): 1090/1091 `soll = 0` (12 Zellen, 0 Ausloeser, Kistenlage
  x [-5978,-5959) z [-15503,-15481)); 3050/3051 `soll = 1` + Rechteck-Pin der ersten Ausloeser-Lage (-23875,-18500)
  in r*[46] +-450 (x -25450..-22500, z -18500..-16350 — selbst nachgerechnet aus r0[46] @0x00AD4 x -25000..-22950
  z -18050..-16800): gefunden, Pfad W -> O -> N wie im Dossier. 2050/2051 `soll = -1` mit Begruendung.
* `integration_r36_dachkiste` `fall3050` (eigener Lauf): 425 SCHUB-OK, Sperre nur an der Westwand r3[31], AUSLOESER
  `wort=0x4002 band=4 pos=(-24058,-7200,-18488)` (-18488 = Zellkante -18050 - 450 + Schritt, wie behauptet), Fall
  `zust 0->1 … n=6`, LANDUNG y=0 Band 0, Endlage `(-24058,0,-15561) b0 r(-1029,0,0)`: **der vorgesehene Fall der
  3050-Kiste laeuft im Port und ist gepinnt.** Die Records (eigener Dump): obj 1 @0x01F1A Band 4 unter `Ck (3,93)=0`
  @0x01F02, Fall-Lage @0x01F40 Band 0 (-24000,0,-22200), Latch sub01 @0x02184-2196 (`Ck (3,93)=0 / Work_set 3,1 /
  Member_cmp 15 == 7 / Set (3,93)=1`), `Item_aot_set slot 6 @0x01EC6` floor 1 Item 0x07 x6 (Super Redhawk) x -24800..
  -23000 z -23700..-22900, Tueren slot 0 @0x01E86 floor 0 / slot 1 @0x01EA6 floor 4: nichts haengt an der oberen Lage.
  ROOM3050 am ORIGINAL (Schritt 2 der "selben Pruefung") ist NICHT gemessen (kein Spielstand; re15-room-capture
  koennte einen erzeugen) — die Datenlage (Item nur auf der gefallenen Kiste, Latch ueber die Kerbe) macht den Fall
  zum vorgesehenen Weg; byte-true offen bleibt nur, ob das Original die Kiste auf demselben Pfad passieren laesst.
* `zensus_diag.txt` (R_belege, Stand b131cbd8 13:25) ist VERALTET: zeigt fuer 3050/3051 "Spieler-Startlage gesperrt ->
  keine Suche" mit dem alten Start (-3526,2083) und fuer 1090 "9" Zellen; die Sonde (eigener Lauf `probe_r36_dachkiste
  diag`) liefert heute 12 Zellen bzw. 13373/8 mit dem Start (-3596,1523) = 30E0 slot 6 @0x00ABA (selbst nachgelesen).
  Das Dossier (4.1) zitiert die Datei als Beleg der Tabelle — Beleg und Tabelle stimmen nicht ueberein (kleiner Mangel).

### Kann die 2050-Kiste obj 6 im Port ueberhaupt fallen? (Schritt 1 der "selben Pruefung" — vom Bau-Agenten nicht gemessen)
Eigene exe-Laeufe (Band 3, `RE15_DEBUG_JUMP=2050@250`, Messkopie):
* **R4b** Spieler (-200,-9348) Blick 2048 (oestlich der Kiste, Schub nach Westen): 17x `p=0 obj=6 SCHUB-OK` von
  lage x -1600 bis -1690, dann `F86 SCHUB-GESPERRT … ziel=(-1728,-9348)` + `TREFFER-ZELLE r0[38]: x -9550..-2600 z
  -14000..-8100` — nach Westen nur 90 Einheiten (halboffenes Rechteck: -1690 - (-2600-900) = 8760 < 6950+1800 = 8750
  falsch -> frei; -1728 -> 8722 < 8750 -> gesperrt). Endlage `p0 (-1690,-5400,-9348) b3`.
* **R4c/R4d** Spieler (-1600,-10698) Blick 3072 (noerdlich, Schub nach Sueden): 98x SCHUB-OK, Kiste von z -9348 auf
  **-7446** (1902 Einheiten in 114 Bildern: 12 Bilder Anlauf, danach ~18.6/Bild). Der anschliessende Umweg auf die
  Westseite (Tasche x <= -2950 zwischen r0[45] z -8100..-7100 und r0[32] z -4900..-4400, Schubstellung (-3040,-6600):
  Reichweitenpunkt (-2477,-6600) gegen Kistenmitte (-1600,-7446) = |dx| 877, |dz| 846 <= 900 @0x8002d224) kam im
  Skript nicht an: nach jedem Schub-Ende und nach jedem Gehen laeuft eine 20-Bild-Animation `mo=8` mit ~70-90/Bild
  Wurzelbewegung in Richtung der ALTEN Geschwindigkeit (R4c: 1376 Einheiten rueckwaerts nach dem Schub; R4d: 1676
  Einheiten nach +x waehrend der Spieler schon +z blickte) und Drehungen werden in `mo=17` geschluckt — geskriptete
  Umwege mit mehreren Drehungen landen deshalb nicht an den berechneten Punkten (R4d: Spieler gegen r0[43] bei
  (4632,-9218)). Die Frage "faellt die 2050-Kiste im Port?" ist damit von mir NICHT entschieden; gemessen ist nur: nach
  Westen 90, nach Sueden bis -7446 moeglich, nach Osten/Norden entlang der Spalte x -1600 moeglich (Zensus), und der
  einzige Weg aus der Spalte ist der West-Schub aus der Tasche (rechnerisch in Reichweite). Die Sonde des Bau-Agenten
  findet den Ausloeser nur mit ihren Toleranzen (halb/432/Kistenschrumpfung: der dort gefundene West-Schub bei
  Kistenmitte z -8850 verlangt eine Spielerstellung x <= -2950 bei z >= -7650 (r0[38]) UND ausserhalb z -8550..-6650
  (r0[45]) — mit exakten Regeln unmoeglich; mit Kistenmitte >= -7532 dagegen moeglich). Die Frage bleibt an der exe
  UND am Original offen; die Nachbesserung muss sie messen (Skript-Hinweise: nach jedem Schub W1.5, Gehstrecken an
  Waenden enden lassen, Drehung = 11 Bilder x 96 = 1056 je 0.37 s / 960 je 0.33 s, Gehen 75/Bild, Schub 18-21/Bild).

### Stichproben der exe-Laeufe des Bau-Agenten (R_belege/nb1_*, selbst gelesen)
`nb1_2030treppe/stair_dbg.log`: `[STAIRREG] slot=8 type=12 chain=128 ... count=2`, `[stair] START dir=UP cur_band=3
target_band=5`, climb.txt `T376 ... pos=(-6155,-9000,-7693) ... band=5` -- Treppe 2030 hebt Band 3 -> 5 (count 2 @0x01B8E
Byte 16, `lbu v0,14(a2)` @0x800435b8), die Tueren slot 1/2/3 liegen auf floor 9: der alte Weg ueber 2030 Band 3 ist
zu Recht gestrichen. `nb1_2000_2070m/debug.log`: `DOOR FIRE slot=3 ... spawn=(-18650,0,-6450)`, `loaded room2070.rdt`,
Spieler danach (-7112,0,-7732) Band 0 = Mittelgang. `nb1_2070n`, `nb1_2040treppe`, `nb1_3050fall_d`: durch meinen
eigenen Riegel-Lauf (weg2040/treppe2040/fall3050) bestaetigt.

## Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 -- an der exe messen, ob Leon/Elza die Dach-Kiste bis zum Fall-Ausloeser schieben koennen (echter Weg, jede Richtung) | **erfuellt** | unveraendert seit Abnahme 0 (exe identisch): Riegel `leon`/`elza` echter Weg + `west` in meinem Lauf `lauf_e3d996d9` -- 0 SCHUB-OK, 0 AUSLOESER, Sperrzellen r3[4]/r3[2], Kiste auf dem Record; dazu die 9 Laeufe der Abnahme 0 |
| 2 -- am Original messen: bis zur Absprungzelle oder vorher geblockt? | **erfuellt** | orig_lauf5/6 (Auslieferungs-EXE, Haltepunkte an den disassemblierten Stellen) + eigene Wiederholung NORD in Abnahme 0: erster Schubschritt trifft 0x80123984 = r3[4], Ruecksetzung, 0 Fall-Ausloeser |
| 3 -- Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x, Pin) | **erfuellt** | Kette @0x8002bf14-c0e8 / FUN_8003b558 @0x8003b634-794 (Abnahme 0 disassembliert), gleiche Zelle, gleiche Ziel-Lage, gleiche Ruecksetzung; Pins `unit_r36_dachkiste_zensus` + `integration_r36_dachkiste` (leon/elza/west) gruen, diskriminierend |
| 4 -- dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt ein Fortschritt an der Kiste? | **teilweise** | 3050/3051: Fall = vorgesehener Weg (Records selbst nachgelesen), an der exe gefahren und gepinnt (`fall3050`, Sonde `soll = 1`) -- Schritt 2 (Original) nicht gemessen, Datenlage eindeutig. 2050/2051: M1 (Zensus) berichtigt und von mir bestaetigt; ABER die Antwort "ROOM2040 bleibt ohne die Kiste erreichbar" ist nicht gedeckt: der genannte zweite Zugang ueber 11A0 ist zirkulaer ((3,151) nur in 2070 sub02, Nordgang nur aus der 2040-Galerie betretbar -- N1), der genannte Ersatzweg 2050 -> 2020 -> 2030 -> 2060 -> 2040 scheitert im Port im ersten Raum (2020 Band 1: Spieler bei x 218 bzw. beim Laden auf z -4318 von r0[15] gesperrt -- N2); ob die Kiste im Port/Original fallen kann, ist nicht gemessen (die Sonde liefert fuer 2050 ein Toleranz-Fehlpositiv -- N3). Kleiner: Beleg-Datei `zensus_diag.txt` veraltet (N4), 3050 am Original offen (N5) |

## Maengel (nummeriert, nachpruefbar)

1. **N1 -- "Zweiter Zugang" nach ROOM2040 ueber ROOM11A0 ist zirkulaer (Dossier 4.3 und "Fuer den Nutzer").**
   ROOM11A0 slot 0 @0x00F5A (-> 2070 Nordgang) steht unter `NOT(Ck(3,151)=0)`; (3,151) wird in STAGE1/2/3 ausschliesslich
   in ROOM2070/2071 sub02 @0x00CE4 gesetzt (Scan aller `Set`-Opcodes je RDT), sub02 haengt am Ereignis `Aot_set slot=7
   sce=3` @0x00AC2 im Nordgang; der Nordgang (SCA r0[8..11], geschlossener Ring) hat nur die Tueren slot 4 (-> 11A0) und
   slot 5 (-> 2040) und wird zum ersten Mal NUR aus ROOM2040 slot 4 @0x0131C (Galerie, floor 5) betreten. Ein Spieler,
   dessen 2050-Kiste faellt, bevor er in 2040 war, hat diesen Zugang nicht. Nachbesserung: Satz streichen oder als
   "nur nach erstem Galerie-Besuch" kennzeichnen; die Softlock-Frage fuer 2050 haengt damit allein am Weg N2 bzw. an
   der Fallfaehigkeit N3.
2. **N2 -- Ersatzweg 2050 -> 2020 -> 2030 -> 2060 -> 2040 ist an der exe nicht gangbar (Dossier nennt ihn als Weg
   "ohne Kiste").** Eigener Lauf R1 (2020, Ankunft aus 2050 slot 1 (11350,-1800,-3150) Band 1, nach Westen): Spieler
   bleibt bei x 218 stehen = Ostkante -250 der Band-1-Zelle r0[15] @0x004F8 (u0 0x00) + 450 + 18; R1b (Spieler per
   RE15_PLAYER_POS in die Luecke (-1300,-3150)): beim Laden auf z -4318 = r0[15]-Suedkante -3850 - 450 - 18
   herausgeschoben. Der Nordstreifen von 2020 (Tuer slot 1 -> 2030) ist vom Suedstreifen (Ankunft aus 2050) im Port
   nicht erreichbar. Das Glied 2030 Band 9 -> slot 3 -> 2060 ist dagegen gemessen OK (R2: `DOOR FIRE slot=3 ...
   spawn=(7600,0,3950)`, `loaded room2060.rdt`). Nachbesserung: entweder am Original messen, ob FUN_8003b0a4 (Maske 1)
   r0[15] ebenfalls sperrt (dann gibt es den Ersatzweg auch im Original nicht und der Dossier-Satz ist zu streichen),
   oder den Port-Mechanismus benennen, der eine u0-0-Zelle als Spielerwand behandelt (collision_constrain_impl
   re15_collision.c:727 ueberspringt sie; Kandidaten: Bodenfunktion FUN_8001c6e8 / `(type | u0<<8) & mask` in
   re15_collision_room_coll, `re15_collision_objects`) und belegen, dass er byte-true ist. Glied 2060 (Tritt aus der
   Startlage in die Kerbe, Klettern, Tuer): Lauf R3b, s. Nachtrag unten.
3. **N3 -- Schritt 1 und 2 der "selben Pruefung" fehlen fuer ROOM2050/2051 obj 6.** Weder an der exe noch am Original ist
   gemessen, ob die Kiste bis zur Absprungzelle r0[49] (x 6350..8750 z -13950..-12150, Wort 0x3202) kommt. Die Sonde
   (`soll = -1`) meldet "1 Ausloeser", aber ihr Pfad (West-Schub bei Kistenmitte z -8850) verlangt eine Spielerstellung,
   die r0[38] (z <= -8100, x <= -2600) und r0[45] (x <= -3350, z -8100..-7100) mit exakten Regeln ausschliessen -- ein
   Toleranz-Fehlpositiv der Sonde (halb/432/Kistenschrumpfung), das die Dossier-Aussage "kann im Port ueber die Kante
   fallen" nicht traegt. Meine eigenen Laeufe: Ost-Schub 90 Einheiten dann r0[38] (R4b), Sued-Schub bis z -7446 (R4c/d);
   der West-Schub aus der Tasche (-3040,-6600) ist bei Kistenmitte >= -7532 rechnerisch in Reichweite (|dz| <= 900
   @0x8002d224), aber nicht zu Ende gefahren (Anlauf-/Nachlauf-Animation mo=8, s. o.). Nachbesserung: den Fall an der exe
   fahren (Skript-Hinweise oben) und -- da 2040 ueber die Galerie der einzige Zugang zum Nordgang/11A0/STAGE3 ist
   (11A0 slot 1 @0x00F7C -> ROOM3000; der andere Zugang 20A0 slot 2 @0x017A2/@0x017C6 braucht (3,206), gesetzt nur in
   20A0 @0x01FF0, erreichbar aus ROOM2090 slot 1) -- bei Fallfaehigkeit am Original messen (room-capture 2050) und
   dann nach Punkt 3 verfahren (blockt das Original -> Port blockt; faellt sie auch dort -> Grenze benennen + Pin).
4. **N4 -- Beleg-Datei `R_belege/zensus_diag.txt` (b131cbd8) ist veraltet** und widerspricht der Tabelle 4.1, die sie als
   Beleg nennt (dort 3050: "Spieler-Startlage gesperrt -> keine Suche" mit Start (-3526,2083); heute 13373 Zellen /
   8 Ausloeser mit Start (-3596,1523) = 30E0 slot 6 @0x00ABA). Nachbesserung: Datei aus `probe_r36_dachkiste diag` neu
   schreiben.
5. **N5 (klein) -- ROOM3050 am Original nicht gemessen** (Schritt 2); die Records machen den Fall zum vorgesehenen Weg
   (Item nur auf der gefallenen Kiste, Latch ueber die Kerbe), offen bleibt nur, ob das Original denselben Schubpfad
   (West bis r*[31], Sued in r*[46]) zulaesst. Im Dossier unter OFFEN nennen (fehlt dort), Messweg: re15-room-capture
   STAGE3/3050 + orig_1090_push.py-Muster mit obj 1.

Keine Maengel am Code (reiner Messschrieb + Riegel, kein Verhalten), keine Vertragsverletzung, keine Regression in den
beruehrten Modulen (31/31 Unit-Gruppe, Riegel 1/1 mit sechs Laeufen). M1 und M3 der Abnahme 0 sind behoben (M1 von mir am
RDT/SCA bestaetigt, M3 mit `fall3050` + `soll = 1` diskriminierend); M2 wurde durch einen anderen, ebenfalls unbelegten und
im Port nicht gangbaren Weg ersetzt (N1/N2).

## Gesamt

**Softlock-Frage ROOM1090/1091 (Kern des Auftrags): NEIN -- Port = Original, unveraendert bestaetigt.** Punkte 1-3
erfuellt. Punkt 4 teilweise: 3050 belegt und gepinnt; fuer 2050/2051 ist die Dossier-Antwort "haengt an der Kiste, aber
2040 bleibt erreichbar" in beiden Begruendungen nicht haltbar (N1 zirkulaer, N2 im Port gesperrt) und die Fallfaehigkeit
weder im Port noch im Original gemessen (N3) -> **bestanden = false**. Die Nachbesserung ist Messung + Dossier (kein
Verhaltens-Code), es sei denn, N2/N3 ergeben einen Port-Defekt (u0-0-Zelle als Wand) oder eine Original-Sperre.

Messpfade: Riegel `re15_port/build/r36_dachkiste_wd/lauf_e3d996d9/{leon,elza,west,fall3050,weg2040,treppe2040}/`;
eigene Laeufe Scratchpad `abn1/{r1_2020,r1b_2020gap,r2_2030,r3_2060,r3b_2060,r4a_2050,r4b_2050,r4c_2050,r4d_2050}/`
(debug.log, climb.txt, objfall.txt, push.txt; unversioniert, Inhalte oben zitiert).

## Nachtrag — Lauf R3b (ROOM2060, Tritt obj 2 aus der Startlage in die Kerbe) und exe-Haenger

R3b (JUMP 2060, Spieler (7600,0,3950) Blick 2048 = Ankunft aus 2030 slot 3; Skript mit den gemessenen Kinematik-Werten,
Gehstrecken an Waenden endend): der Spieler erreicht den Nordkorridor (z -7832 an der Nordwand r0[4]), geht nach
Westen und bleibt bei x -2328 stehen (climb.txt T840-T870, keine Band-0-Zelle mit Ostkante bei -2778 im eigenen Dump;
vermutlich derselbe Mechanismus wie in 2020, s. N2), dreht auf 3104 (+z) und bewegt sich dann nicht mehr (mo=7, af
zaehlt bis 112 und bleibt); ab climb.txt T1138 / debug.log `[walk] F870` schreibt die exe nichts mehr (Dateien
18:47:40 eingefroren, Prozess `re15_pc_r36abn1.exe` lebt noch um 18:52) — **ein Haenger der exe in ROOM2060 bei
(-2328,0,-7336) rot 3104**. Das Glied "Tritt aus der Startlage in die Kerbe -> Klettern -> Tuer slot 1" ist damit von
mir NICHT gemessen; der Haenger selbst ist kein Code dieser Spur (kein Engine-Diff ausser dem Push-Log-Haken, der hier
nicht lief: push.txt leer), aber ein Hinweis fuer die Integration (Harness/Spur A Klettern/Kollision: Spieler-Bewegung
+z aus einer Klemme heraus in ROOM2060 friert ein; Reproduktion: `RE15_DEBUG_JUMP=2060@250 RE15_PLAYER_POS=7600,3950,
2048,0 RE15_INPUT_SCRIPT=W2,U1.3,W0.3,L0.37,W0.3,U4.8,W0.3,R0.37,W0.3,U4.6,W0.3,R0.37,W0.3,U4.47,...`, Scratchpad
`abn1/r3b_2060/`). Fuer die Nachbesserung N2 bedeutet das: das 2060-Glied bleibt zusaetzlich offen.
