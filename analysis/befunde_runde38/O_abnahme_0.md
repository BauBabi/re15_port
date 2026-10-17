# Runde 38 Spur O "montage" — Abnahme 0 (unabhaengig)

Datum: 2026-10-17. Baum `.claude/worktrees/r38_montage` (Zweig `r38/montage`, HEAD 800a2fd9, Basis d70c234e).
Pruefer: unabhaengige Abnahme 0 (glaubt dem Bau-Agenten nichts, was nicht selbst am gebauten Stand gemessen ist).
Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).

**STAND: ABGESCHLOSSEN — bestanden** (Punkt 1 erfuellt; Bau-, RE-, Vertrags- und Test-Gate halten; keine blockierenden
Maengel; nicht blockierende Hinweise H1..H4 in §8). Gezielte Tests selbst gefahren: 110/110 gruen.

## Wortlaut (AUFTRAG.md, massgeblich)

> Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies durchbrechen, sollen sich die Zombies jeweils auch Richtung
> Kamera bewegen, also so, wie es bei den Zombies hinter dem Rolltor schon passiert. Die anderen sollen das auch machen.
> Das wirkt auch bedrohlicher.

## Stand der Abnahme

- [x] Schritt 1: AUFTRAG/VERTRAG/Dossier gelesen, `git log master..HEAD`, `git diff master --stat`, Code-Diff gelesen
- [x] Schritt 2: Bau (local_build.sh configure + build), gezielte Tests (§2, §2b, §2c)
- [x] Schritt 3: eigene Messung an der exe (echter Weg), Gegenprobe Release v0.8.25, Bilder, RE1.5-KI, zweiter Weg (§3)
- [x] Schritt 4: RE-Gate (9 Adressen selbst disassembliert, §4, §3.6, §3.9)
- [x] Schritt 5: Vertrag / Pfade / Nebenwirkungen (§5)
- [x] Urteil (§6..§9)

## 1. Gelesen

* Commits der Spur (nicht-Merge, `--grep r38-montage`): 23 Commits 91033ee4 .. 800a2fd9, dazu Merge 05e7af25 (r37/re2zombie =
  Runde-37-Spur L, Tip ca4f0879; NICHT in master, aber in r38/integration).
* master hat seit der Basis d70c234e KEINE Code-Aenderung (`git diff d70c234e master --stat -- re15_port` leer) -> der
  Code-Diff gegen master = L-Merge + Spur O.
* Dateien der Spur O (nur diese Commits): engine/src/irons_tod_1150.c, include/re15_irons_tod.h (VERTRAG §1.4: gemeinsame
  Dateien der Spur O), engine/src/montage_r38.c (neu), include/re15_montage_r38.h (neu), tests/unit/test_r38_montage.c (neu),
  tests/unit/probes/r38_montage.cmake (neu), tests/integration/test_r38_montage.cmake (neu), Dossier + O_belege/.
  L beruehrt keine dieser Dateien (`git diff d70c234e ca4f0879 --stat -- <O-Dateien>` leer).

## 2. Bau + gezielte Tests (selbst gefahren)

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. Der Stand ist gebaut: re15_pc.exe 10:24:11 ist juenger als alle Engine-/Include-Quellen
  (montage_r38.c 10:20:31, re15_montage_r38.h 10:20:24, irons_tod_1150.c 09:10:59, re15_irons_tod.h 09:01:22; letzter
  Engine-Commit b490ed78 10:20:54), die exe enthaelt die neuen Zeilen (`[montage-r38] ... hinter dem Tor freigegeben`).
* `ctest --test-dir re15_port/build -R "r38_montage" -j3 --timeout 600 --output-on-failure` ->
  **`100% tests passed, 0 tests failed out of 9`** (Total 99.28 s; integration_r38_montage 99.26 s).
* Direktlauf `test_r38_montage s1030_re2` (Ausgabe gelesen, nicht nur der exit): Cut 7 5/5 naeher (kleinste 3267, kleinste
  Haelfte 1521), Baustein 5 "9 gesehen, 9 ab dem ersten Bild angehalten, Raum-Records im Warte-Rechteck 0", Cut 12 "alle 9 ...
  kleinste 4198 ... kleinste Haelfte 1415", Kriech-Bit 6 von 6, Spieler auf der Achse (-12869,-11445), Gegenprobe v0.8.25
  ROT (Cut 7 0 von 5 naeher; Cut 12 in beiden Haelften nur 3 von 9). = die Zahlen des Dossiers.

## 2b. Gezielte Testgruppen der beruehrten Module (selbst gefahren)

`ctest --test-dir re15_port/build -R "r35_cut1150|r36_szene1150|r36_elza_irons|r37_szene11c0|r37_zombieaktiv|irons|r34n_a_rolltor|unit_1030_|cam_1030|room1040_switch|r36_gegnerzahl_raum10[34]0|r35_fenster|r36_elza_fenster|room1070|r36_szenenleon_irons" -j3 --timeout 900 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 56`** (Total 526.35 s; darunter integration_r35_cut1150 526 s (Lauf A + Lauf B
ohne Cut-7-Teil), integration_r36_szene1150, integration_r37_szene11c0, integration_r37_zombieaktiv, integration_r36_elza_irons,
integration_r36_elza_fenster, integration_r35_fenster, integration_r30_irons_tisch_*). Parallel liefen fremde ctest-Laeufe
anderer Baeume; kein Haken rot, kein Nachfahren noetig.

## 2c. Testgruppen des L-Merges (in diesem Baum enthalten, selbst gefahren)

`ctest --test-dir re15_port/build -R "r37_re2zombie|r36_zombieki|re2z_|r34_granaten|r35_granate|10d0_situp|r36_blut" -j3
--timeout 900 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 45`** (244.85 s).
Summe der selbst gefahrenen gezielten Tests: 9 + 56 + 45 = **110 / 110 gruen**.

## 3. Eigene Messung an der echten exe (echter Weg)

### 3.1 Messweg und unabhaengiges Mass

Weg: Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene` (Spielstand), CONTINUE in ROOM1130
(`[save] CONTINUE: resumed in room 1130 (hp=100)`), Eingabe `W1,A0.2,W200` -> `[aot] DOOR FIRE slot=2` -> Tuersequenz ->
`[room] PC loaded room1150.rdt` -> Szene (`Zustand 1: Szene startet (Programm 0)`, Arm faellt, Tuerknall) -> Montage
`ROOM1130 Zustand 2` -> `ROOM1040 Zustand 3` -> `ROOM1030 Zustand 4` -> `ROOM11C0 Zustand 5` -> `EXIT_AT: Bild 60 in Raum 11c0`.
Messkopie `re15_pc_abn0o_<lauf>.exe` (eigener Name, nach dem Lauf geloescht), Skript
`<scratchpad>/abn/tools/abn_lauf.sh` (RE15_NO_INTRO/NOAUDIO, RE15_WINDOW_SCALE=1, beschleunigter Renderer, RE15_IT_LOG=1).

UNABHAENGIGES Mass (nicht der Mess-Haken der Spur): die `[irons-tod-mess]`-Zeilen des Runde-35-Hakens RE15_IT_LOG (je Bild
jeder Aktor mit Lage/Ziel/grid/Flags, irons_tod_1150.c mess_zeile, seit f0e306c6 unveraendert), ausgewertet mit eigenem
Skript `<scratchpad>/abn/tools/abn_ausw.py`; die Kameras liest das Skript SELBST aus den ausgelieferten RDTs (RDT+0x24
camera_start + Cut*32, +4/+0xC/+0x10/+0x18, re15_camera.h). Je Schnitt und Zombie: Abstand zur Kamera-Bodenprojektion
Anfang -> Mitte -> Ende, Weg, Anteil der Bewegung, der auf die Kamera zu geht (radial/weg), Stand-Anteil.

### 3.2 Lauf mA (Stand 800a2fd9 gebaut) — `runs/mA/abn_ausw.txt`

| Schnitt | Zombies | Abstand zur Kamera Anfang -> Mitte -> Ende (radial/weg) | Urteil |
|---|---|---|---|
| 1130 Cut 0 (176 Bilder) | 1..5 (Typ 16/10/10/11/11) | 6821->5293->3961 (0.94), 5648->3680->1969 (0.83), 4565->3253->1734 (0.95), 3521->1459->438 (0.82; Stopp hinter der Kamera Bild 142), 4478->2261->1295 (0.60; Stopp Bild 161) | alle 5 auf die Kamera zu, beide Haelften |
| 1040 Cut 1 Rolltor (405) | 1..5 (Typ 16) | Ende 7136 / 5663 / 8286 / 5395 / 7046; Bilanz `5 Zombies, durchs Tor 5, letzter bei Bild 270 von 405; ... Kappe 0` | Vorbild unveraendert (s. 3.3) |
| 1030 Cut 7 (207) | Kopien 7..11 (Typ 10/10/10/11/11) | 8272->6492->4671 (0.94), 6235->3809->1476 (0.86), 4644->2916->1377 (0.92), 7222->4589->2371 (0.93), 5599->3114->562 (0.81) | alle 5 auf die Kamera zu |
| 1030 Cut 7 (Hintergrund) | 1..6 + Kriecher 12..14 hinter dem Tor | g 0d -> 2d (Bit 0x20) bzw. 00 -> 20, Weg 0 (Aktor 3/6: einmaliger Ausschub aus der Suedwand beim Erscheinen, 1 Bild) | Baustein 5 haelt sie |
| 1030 Cut 12 (335) | 1..6 (Raum-Records), 12..14 (Kriecher) | 12906->9875->8400 (0.71), 11389->8153->5982 (0.77), 13727->11102->8691 (0.83), 14331->10396->9091 (0.71), 11303->9201->7105 (0.74), 14332->11667->9736 (0.84), 9827->8015->5404 (0.69), 10558->8451->5954 (0.74), 9560->7248->4925 (0.72); alle fl -> 0x1004 (Kriech-Bit) | alle 9 auf die Kamera zu, beide Haelften (kleinste Haelfte -1305) |
| 1030 Cut 12 (hinter der Kamera) | Kopien 7..11 | vorn 0 %, ab Bild 1 angehalten (g -> 20) | nicht im Bild |
| 11C0 Cut 13 (55) | keine Zombies (Ada/Marvin) | — | nicht betroffen |

Spieler-HP 100 in jedem Schnitt. Der Mess-Haken der Spur (`[montage-r38]`-Zeilen im selben debug.log) liefert dieselben
Anfangs-/Endwerte (z.B. 1130 Aktor 1 `6821 -> 5480 -> 3961`, 1030 Cut 12 Aktor 2 `11389 -> 8512 -> 5982`, Ende-Zeile
`ROOM1030 Cut 12 Ende Bild 335: Spieler (-12869,-11445) HP min 100, Navigator-Ziel fremd 0, Kriech-Regel 6, KI RE2`) und
ist Zahl fuer Zahl gleich der Dossier-Tabelle "Dritter Lauf" (lauf_2c011746) -> der Lauf ist deterministisch reproduziert.
Baustein 5 im Log: 9x `wartet auf Cut 12`, mit dem ersten Bild von Cut 12 9x `hinter dem Tor freigegeben`; Kriech-Regel
6x (Bild 31/47/111/112/234/238).

### 3.3 Gegenprobe: Release-exe v0.8.25 auf demselben Weg — `runs/v0825_A/abn_ausw.txt`

Release-Paket `release/pkg-win/re15_port_v0.8.25/` (exe sha256 9cd642a2...; Kopie mit Assets nach `<scratchpad>/abn/v0825/`,
Lauf `abn_lauf_v0825.sh`, nur gelesen, nichts im Hauptbaum veraendert). Gleicher Weg, gleiche Bilanz-/Raumfolge.

| Schnitt | v0.8.25 | jetzt (mA) |
|---|---|---|
| 1130 Cut 0 | alle 5 STEHEN (Weg 0/0/0/0/18, Abstand +0..-7, g 02) = der Nutzer-Befund | alle 5 -2831..-3679 naeher |
| 1040 Cut 1 Rolltor | 12391->10688->7136, 13004->10246->5663, 13941->11567->8286, 14897->10743->5395, 15496->11181->7046 | **identisch** (jede Zahl, Weg und radial/weg gleich) |
| 1030 Cut 7 | Kopien 7..11 STEHEN (Weg 0); die Zombies hinter dem Tor laufen schon los (unsichtbar) | Kopien -3267..-5037 naeher; hinter dem Tor angehalten |
| 1030 Cut 12 | 2: +1, 13: -350, 14: +8, 1: -709, 3: -711, 6: -635 (Aufrechte stehen am Tor / gehen umher), nur 4/5/12 kriechen naeher | alle 9 -4198..-5407 naeher |

-> Die Aenderung ERKLAERT den Befund: vorher standen die Zombies in 1130 Cut 0 und 1030 Cut 7 (stehende Listen grid 0x02 /
0x00), in Cut 12 kamen die aufrechten nicht durch; jetzt gehen/kriechen alle auf die Kamera zu; der Rolltor-Schnitt ist
Bild fuer Bild gleich.

### 3.4 Bild 1130 Cut 0 (RE15_FRAMEDUMP "5-175/10", RE15_WINDOW_SCALE=1, beschleunigter Renderer, EXIT_AT 176#1130)

`<scratchpad>/abn/runs/fd1130/sheet_1130.png` (18 Bilder) und `sheet_1130_3.png` (F15/F95/F175): F15 die Gruppe steht
vor der Briefing-Room-Tuer; F95 zwei Zombies gross im Vordergrund (verlassen das Bild unten), die anderen in der Mitte;
F175 die uebrigen deutlich groesser/naeher als in F15. Sichtbar: sie gehen auf die Kamera zu.

### 3.5 Bild 1030 Cut 7 + Cut 12 (RE15_FRAMEDUMP "10-540/20", RE15_WINDOW_SCALE=1, beschleunigter Renderer, EXIT_AT 541#1030)

`<scratchpad>/abn/runs/fd1030/sheet_1030_cut7.png` (F10..F190): die fuenf Kopien kommen den Gang vor der 1070-Tuer herauf,
ab F130 fuellen sie das Bild. `sheet_1030_cut12.png` (F210 = letztes Cut-7-Bild, ab F230 Cut 12) und `cut12_gross.png`
(F250/F390/F530, 2x): F250 die aufrechten Zombies hinter dem Gitter gehen aufs Tor zu, vorn kriechen die ersten; F390 nur
noch zwei aufrechte (Aktor 3/6, gehen laut Messung noch auf das Tor zu: 12255 -> 10149 in 135 Bildern), die Kriecher kommen
nach vorn; F530 kein Aufrechter mehr hinter dem Gitter, die Kriecher sind gross im Vordergrund.
Verlauf je 15 Bilder (`tools/verlauf.py`, Lauf mA): jeder der neun nimmt ueber den Schnitt ab; Kriecher pendeln dabei
(Kriech-Zyklus), z.B. Aktor 4 von Bild 241 bis 316 9508..9626 (Durchkriechen der Torzelle, 2,5 s ohne Netto-Gewinn), danach
9106/9091. Das ist Bewegung durch das Tor auf die Kamera zu, kein Stehen.

### 3.6 RE-Pruefung zweier Code-Kommentare (Stichprobe zusaetzlich)

* `ori DAT_800aca3c,0x100` @0x800402d4 (RVD-Automatik nach Cut_chg aus): `dis 0x80040280 16` / `dis 0x800402b8 12`:
  `lui v1,0x800b` / `addiu v1,v1,-13764` (= 0x800aca3c) @0x800402ac-b0, `ori v0,v0,0x100` @0x800402d4, `sw v0,0(v1)` @0x800402d8
  -> bestaetigt.
* Pos_set LAB_80041048: `lw a1,340(a0)` @0x8004104c, `sw v0,52(a1)` @0x80041058, `sw v0,56(a1)` @0x80041064,
  `sw v0,60(a1)` @0x80041070, `addiu v1,v1,8` @0x8004106c -> bestaetigt.

### 3.7 RE1.5-KI (Menue-Option, NICHT Standard) an der exe — `runs/mR15/abn_ausw.txt`, Gegenprobe `runs/v0825_R15/abn_ausw.txt`

Gleicher Weg mit `RE15_AI_FLAVOR=re15` (Ende-Zeilen `KI RE1.5`):

| Schnitt | jetzt (RE1.5-KI) | v0.8.25 (RE1.5-KI) |
|---|---|---|
| 1130 Cut 0 | alle 5 naeher: -3766 / -1058 / -4142 / -2407 / -2023 (radial/weg 0.57..0.89) | alle 5 stehen (Weg 0, einer 18) |
| 1040 Cut 1 Rolltor | 1107 Bilder, Ende 803/1575/8397/9856/264, radial/weg 0.43..0.81 | 916 Bilder, Ende 815/1708/8271/9783/318, radial/weg 0.52..0.75 (`letzter bei Bild 781 von 916`) |
| 1030 Cut 7 Kopien | -962 / **-857** / -2453 / -1209 / -1240 (radial/weg 0.36..0.55; Aktor 9 zweite Haelfte nur -199) | alle 5 stehen |
| 1030 Cut 12 | alle 9 naeher, kleinste -1218 (Aktor 4, endet hinter dem Tor bei (-4262,-26019), radial/weg 0.33); Aktor 5 (Bit 4) erste Haelfte -48 (dreht, = Dossier OFFEN 2); Kriecher 12..14 stehen 51..67 % der Bilder | 13: +403, 14: +0, 1/3/4: -905..-971 |

Bewertung: Unter der RE1.5-KI gehen die Montage-Zombies in jedem Schnitt netto auf die Kamera zu, aber mit dem
Pendel-/Wende-Charakter dieser KI — denselben zeigt das Vorbild selbst unter RE1.5 (Rolltor v0.8.25 916 Bilder, radial/weg
0.52..0.75; jetzt 1107 Bilder, 0.43..0.81). Die Rolltor-LOGIK ist unveraendert (in ROOM1040 wirkt montage_r38.c nur als
Messung, halt = 0; unter RE2 an der exe Bild fuer Bild gleich, 3.8); der Laengenunterschied unter RE1.5 kommt aus dem
Zustand beim Eintritt in 1040 (die 1130-Zombies laufen jetzt unter der RE1.5-KI) — die Einzelursache (z.B. RNG-Stand) ist
NICHT nachverfolgt. "so, wie es bei den Zombies hinter dem Rolltor schon passiert" gilt damit je KI.
ABER: der RE1.5-Riegel `unit_r38_montage_s1030_re15` fordert auch unter RE1.5 PFLICHT "alle fuenf Kopien >= 1000 naeher"
(test_r38_montage.c `ok1 = (s7.n == 5 && s7.naeher == 5 && ...)`, nur die Haelften sind "gemeldet") und ist gruen nur fuer
seine feste Saat (in-process kleinste 1160); an der exe ueber den echten Weg kommen zwei Kopien nur 857 / 962 naeher ->
die Aussage des Dossiers ("dasselbe unter der RE1.5-KI") stimmt fuer die exe nicht; Cut 7 fehlt in OFFEN 2 (Hinweise H2/H3).

### 3.8 Rolltor Bild fuer Bild gegen die Release-exe v0.8.25

Vergleich aller `[irons-tod-mess]`-Zeilen in ROOM1040 (je Bild jeder Aktor: Platz, Typ, x, z) zwischen Lauf mA (jetzt) und
Lauf v0825_A (Release v0.8.25), KI RE2: **405 / 405 Bilder, 0 abweichend**. Der Rolltor-Schnitt ist an der echten exe
unveraendert (nicht nur im Riegel `unit_r38_montage_rolltor`, der mit/ohne Aenderung im selben Binary vergleicht).

### 3.9 Mechanismus der alten Klemme (Behauptung "FUN_8003bca8 code 0 nimmt jeden Schritt zurueck")

`dis 0x8003be34 30` (PSX.EXE): `lw a3,0(a1)` / `bne a3,v0` @0x8003be38-40 und `lw a0,8(a1)` / `bne a0,v0` @0x8003be4c-54
(neue Lage gegen die vorige), bei Bewegung `lh v0,0(a2)` / `sw v0,0(a1)` @0x8003be8c-94 (x) und `lh v1,4(a2)` /
`sw v1,8(a1)` @0x8003be98-a0 (z), Rueckgabe `addu v0,zero,zero` @0x8003be9c -> bestaetigt. Die Klemme selbst ist an der exe
weg: in Lauf mA kommt Aktor 2 (Bit 1, vorher +120) 11389 -> 5982 naeher und endet unter dem Tor (-12253,-21246).

### 3.10 Robustheit (zweiter Weg an der exe): 1070 leer + verschobene Eingabe — `runs/mB/abn_ausw.txt`

Karte `nach10f0 ersteszene tot1070` (Debug-Log `Umzug: 1140 lebend 5 -> 1130 (9,74)=1; 1070 lebend 0 -> 1030 (9,76)=0`),
Eingabe `W30,A0.2,W200` statt `W1,...`: 1130 Cut 0 und 1040 Cut 1 Zahl fuer Zahl wie mA; ROOM1030 zeigt ohne Cut-7-Teil
gleich Cut 12 (357 Bilder, Baustein 5 greift nicht): alle 9 naeher, -2229..-6405 (radial/weg 0.38..0.84), alle mit
Kriech-Bit, kleinste Haelfte -685; Spieler auf der Achse (-12869,-11445), HP 100. Der Weg "ohne Cut-7-Teil" ist damit auch
an der exe gemessen (der Spur-Riegel misst ihn nur im Prozess, integration_r35_cut1150 Lauf B nur mit den r35-Kriterien).

## 4. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, Binaerdateien des Baums)

Sauberkeits-Check der EXE: `bytes 0x80026e4c` -> `08 00 e0 03 21 10 00 00` = Auslieferungs-Stub (nicht die gepatchte EXE).

| # | Behauptung | selbst gelesen | Urteil |
|---|---|---|---|
| R1 | Einzelstopp/Warten: STAGE1.BIN Zombie-Wurzel `lbu v0,9(a0)` @0x80100450, `andi v0,v0,0x20` @0x80100458, `bne v0,zero,0x80100658` @0x8010045c | `dis 0x80100424 20`: genau diese drei Instruktionen; davor die globale Pause `lw v0,-13760(v0)` (g_pauseflags) / `and v0,v0,v1` (0x20000000) / `bne v0,zero,0x80100658` @0x8010043c. SPRUNGZIEL `dis 0x80100650 14`: @0x80100658..0x80100670 nur `jal 0x8001b064` (a0 = entity+0xb0, a1 = lh 442(entity)), `jr ra` @0x80100680 = derselbe Ausgang wie die Pause, keine KI | bestaetigt |
| R2 | Durchbruch-Form grid 0x0d: `andi v0,v0,0x1f` @0x80100f28, `bne v0,v1(0xd)` @0x80100f2c, `ori v0,zero,0x201` / `sw v0,4(v1)` @0x80100f40-44 | `dis 0x80100f18 18`: `80100f20 lbu v0,9(a0)`, `80100f24 ori v1,zero,0xd`, `80100f28 andi v0,v0,0x1f`, `80100f2c bne v0,v1,0x80100f60`, `80100f34 sb v0,148(a0)` (0x27), `80100f40 ori v0,zero,0x201`, `80100f44 sw v0,4(v1)`, `80100f54 sb zero,9(v0)` | bestaetigt |
| R3 | Installer: Wurzel 0x80100424 fuer Typ 0x10/0x11/0x16 | `dis 0x8011e868 18`: `addiu v0,v0,1060` (0x80100424) @0x8011e86c, `sw v0,11244(at)` = 0x80072bec @0x8011e88c, `sw v0,11248(at)` = 0x80072bf0 @0x8011e894, `sw v0,11268(at)` = 0x80072c04 @0x8011e8a4; Tabelle @0x80072bac -> Index 0x10/0x11/0x16 | bestaetigt |
| R4 | Member-Id 12 = entity+0x9: FUN_8004116c Tabelle @0x80010c8c, [12] -> `sb a2,9(a0)` @0x800411f8 | `dis 0x8004116c 12`: `sltiu v0,a1,0x14`, Tabelle 0x80010c8c, `jr v0`; `table 0x80010c8c 20`: [12] -> 0x800411f4; `dis 0x800411cc 24`: `800411f4 j 0x80041230` / `800411f8 sb a2,9(a0)`; [15] `sb a2,11(a0)` @0x80041210, [16] `sh a2,452(a0)` @0x80041218 | bestaetigt |
| R5 | Form ROOM1141 sub02 `34 0c 30 00` @0x00E70/@0x00E78 vor Cut_chg 0 @0x00E80, Freigabe `34 0c 10 00` @0x00F64/@0x00F74 | `xxd -s 0xE60` / `-s 0xF58` von info/Re1.5/PSX/STAGE1/ROOM1141.RDT (md5 = shared_assets-Kopie): Bytes exakt so; `scd_dump_room.py --sub sub02`: `0x00E6C Work_set 2e 02 01`, `0x00E70 Member_set 34 0c 30 00`, `0x00E74 Work_set 2e 02 02`, `0x00E78 Member_set 34 0c 30 00`, `0x00E7C Sleep 09 0a 01 00`, `0x00E80 Cut_chg 29 00` | Bytes bestaetigt; **aber**: die beiden gehaltenen Entities sind in ROOM1141 Typ **0x27** (Sce_em_set `44 01 27 30 ..` @0x00B1A, `44 02 27 30 ..` @0x00B2E; `44 01 27 10` @0x00B54, `44 02 27 10` @0x00B68) = die Gorillas, NICHT Zombies wie das Dossier schreibt ("die Szene haelt die beiden Zombies an"). Die Form traegt trotzdem: die Typ-0x27-Wurzel (Installer `sw v0,11336(at)` = 0x80072c48 -> 0x80116db8 @0x8011e90c) prueft dasselbe Bit: `dis 0x80116db8 28`: `lbu v0,9(v1)` @0x80116dec, `andi v0,v0,0x20` @0x80116df4, `bne v0,zero,0x80116e88` @0x80116df8 -> Hinweis H1 (Doku) |
| R6 | Navigator FUN_80039e7c: `ori v0,v1,0x7` @0x80039eb8 (+0x91), gleiche Zone `bne a0,a1` @0x80039fbc, `sh s1,444(v0)` @0x80039fd0, Kopie der Globals @0x8003a040/@0x8003a058 | `dis 0x80039eac 12`: `ori v0,v1,0x7` / `sb v0,145(a0)` @0x80039eb8-bc, `addiu v1,v1,-1` @0x80039ed4; `dis 0x80039fac 16`: `bne a0,a1,0x80039ffc` @0x80039fbc, `sh s1,444(v0)` @0x80039fd0, `sh s0,446(v1)` @0x80039fe0; `dis 0x8003a034 12`: `lhu v0,-1104(v0)` (0x800afbb0) / `sh v0,444(v1)` @0x8003a040, `lhu v1,-1102(v1)` (0x800afbb2) / `sh v1,446(a0)` @0x8003a058 | bestaetigt |
| R7 | Radius: FUN_8003a31c `lhu v0,6(v0)` @0x8003a408-410 (+0x78-Satz +6) | `dis 0x8003a3f8 8`: `lw v0,120(a0)` @0x8003a408, `lhu v0,6(v0)` @0x8003a410 | Instruktionen bestaetigt (der Wert 400 ist Daten, nur Riegel-Schwelle) |

## 5. Vertrag / Pfade / Nebenwirkungen

* Pfade (`git diff ca4f0879 HEAD --name-only`, ohne L-Inhalt): engine/src/irons_tod_1150.c, engine/src/montage_r38.c (neu),
  include/re15_irons_tod.h, include/re15_montage_r38.h (neu), tests/unit/test_r38_montage.c (neu), tests/unit/probes/
  r38_montage.cmake (neu), tests/integration/test_r38_montage.cmake (neu), Dossier/Belege. KEINE release/, platform/android/,
  shared_assets/PSX/, KEINE Test-CMakeLists (Registrierung nur ueber probes/r38_montage.cmake). `git diff master --name-only`
  zeigt zusaetzlich orchestrator/RELEASE_NOTES_v0.8.26_ENTWURF.md u.a. — nur weil master sie nach der Basis geaendert hat
  (`git diff d70c234e HEAD --stat -- analysis/befunde_runde38/orchestrator/ ...` leer).
* Gemeinsame Dateien: irons_tod_1150.c / re15_irons_tod.h sind laut VERTRAG §1.4 der Spur O zugeteilt. Der Eingriff ist
  groesser als "1-5 Zeilen" (irons_tod_1150.c +88: zwei Listen, Umparken, Gegenprobe-Helfer, 4 Opcode-Laengen, 2 Haken),
  betrifft aber nur die Montage-Programme selbst; kein anderer Runde-38-Zweig aendert die Datei
  (`git diff d70c234e r38/integration --stat -- irons_tod_1150.c re15_irons_tod.h zombieaktiv_r37.c` leer) und
  `git merge-tree --write-tree r38/integration HEAD` ist konfliktfrei.
* it_opcode_len: die vier neuen Laengen (0x00/0x2e/0x32/0x34 = 1/3/8/4) aendern die Laeufe slots_zuweisen /
  zaehler_1040_setzen nicht: im 1040-Programm steht OP_ZAEHLER_1040 vor jedem OP_WORK, im 1030-Programm stehen alle
  `44`-Saetze vor OP_PARK; Laengen = `scd_dump_room.py` SIZES (0x00:1, 0x2E:3, 0x32:8, 0x34:4).
* Ressourcen: keine Bank-9-Bits, keine Nachrichten-Ids, keine Items. Neu benutzt wird nur Bank 5 Bits 0..19 (Kriech-Regel,
  = die Original-Bits von ROOM1030 sub06 `22 05 i 01`); Bank 5 Wort 0 ist raumlokal (scd_room_setup.c `g_game.flags[5][0] = 0`,
  FUN_8003ecec @0x8003ed74). Signale der Montage in Bank 5: 12 (nur in ROOM1150 gelesen, irons_tod_1150.c:1359), 13 (nur
  S11C0/11C0), 20/28/29/30 — keine Kollision mit 0..19 im Raum 1030.
* Szenarien: die Szene startet nur in Leons ROOM1150 (re15_irons_tod_install `RE15_IT_RAUM_1150`); montage_r38.c wirkt
  nur im Montage-Zustand S1130/S1040/S1030 im zugehoerigen Raum. Elza-Riegel unit/integration_r36_elza_irons und
  _elza_fenster gruen (2b). Spaeteres Betreten: Nachspawn-Listen (EM_1130_NACH, EM_1070_LISTE, EM_KRIECHER_NACH)
  unveraendert; RE15_IT_PARK_1030 nur im Montage-Tuersatz (irons_tod_1150.c:333/1081) und in test_r35_cut1150.c.
* Neue Engine-Quelle montage_r38.c: engine/CMakeLists.txt GLOB -> PC/Linux automatisch; fuer Android gilt die bekannte
  Release-Regel (app/.cxx-Cache neu konfigurieren) — Hinweis fuer die Integration, kein Mangel der Spur.

## 6. Urteil je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1. Leon, Szene nach Irons' Tod: wo die Zombies durchbrechen, bewegen sich die Zombies jeweils Richtung Kamera wie die hinter dem Rolltor; die anderen auch | **erfuellt** | Echte exe, echter Weg (Karte + CONTINUE 1130 -> Tuer -> 1150 -> Szene -> Montage), unabhaengiges Mass (RE15_IT_LOG, Kameras aus dem RDT): in JEDEM Durchbruch-Schnitt kommen ALLE Zombies der Kamera naeher, in beiden Haelften: 1130 Cut 0 5/5 (-2831..-3679), 1030 Cut 7 5/5 (-3267..-5037), 1030 Cut 12 9/9 (-4198..-5407); Weg ohne Cut-7-Teil 9/9. Gegenprobe Release v0.8.25 auf demselben Weg: 1130 und 1030-Cut-7 stehen (Weg 0), in Cut 12 stehen/pendeln 2/13/14 (+1/-350/+8) -> die Aenderung erklaert den Befund. Rolltor-Vorbild an der exe Bild fuer Bild unveraendert (405/405). Bilder: fd1130/sheet_1130.png, fd1030/sheet_1030_cut7.png, fd1030/sheet_1030_cut12.png, cut12_gross.png. Standard-KI RE2; zur RE1.5-KI (Menue-Option) s. H2. |

## 7. Gates

| Gate | Ergebnis |
|---|---|
| Bau | haelt: configure/build OK, exe = HEAD-Stand (§2) |
| RE-Gate | haelt: jede verhaltensrelevante Konstante traegt @0x.../Datei-Offset oder ist als NUTZER-VORGABE/Ableitung aus RID/BLK/SCA gekennzeichnet (EM_*_DURCH grid 0x0d @0x80100f40-44 / @0x80100ab8-abc; RE15_IT_PARK_1030 aus BLK @0x01C10 / SCA @0x01A4C / RID @0x140; RE15_R38_PARK_1030_12_* aus RID @0x80/@0x1E0; Kriech-Konstanten @0x027AA/@0x024E0/@0x02758/@0x024E8/@0x024D4..; Zonen @0x01BBC/@0x01C34; Mess-Raster ohne Spielwirkung). 9 Adressen selbst disassembliert (R1..R7, 3.6, 3.9) — alle enthalten das Behauptete; Sprungziel 0x80100658 geprueft. Keine Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO) und kein neues getenv im Spur-Diff. Doku-Fehler im Dossier (Gorillas statt Zombies) = H1, ohne Wirkung auf Code/Kommentare |
| Vertrag | haelt: nur erlaubte Pfade, keine Bank-9-Bits/Ids/Items, gemeinsame Dateien = die der Spur zugeteilten, konfliktfrei mit r38/integration (§5) |
| Tests | haelt: Riegel je Punkt vorhanden und messend (8 Unit-Teile mit roter v0.8.25-Gegenprobe + integration_r38_montage an der echten exe); Zahlen im Prozess = exe (RE2); 110/110 gezielte Tests gruen; Einschraenkung RE1.5 s. H3 |
| Nebenwirkungen | geprueft: Rolltor bitgleich (exe), Elza-Riegel gruen, Weg ohne Cut-7-Teil an der exe, Nachspawn-Listen unveraendert, Bank 5 raumlokal |

## 8. Maengel / Hinweise

Maengel (blockierend): **keine**.

Hinweise (nicht blockierend, fuer Orchestrator/Integration/Release-Notes):

* **H1 (Doku, Dossier O_montage.md "Baustein 5"):** "die Szene haelt die beiden Zombies an" ist falsch — ROOM1141 sub02
  haelt mit `34 0c 30 00` @0x00E70/@0x00E78 die beiden Entities aus Sce_em_set `44 01 27 30 ..` @0x00B1A / `44 02 27 30 ..`
  @0x00B2E = Typ **0x27 (Gorillas)**. Die Form traegt trotzdem, weil die Typ-0x27-Wurzel 0x80116db8 dasselbe Bit prueft
  (`lbu v0,9(v1)` @0x80116dec, `andi v0,v0,0x20` @0x80116df4, `bne v0,zero,0x80116e88` @0x80116df8); die Code-Kommentare
  (re15_montage_r38.h, montage_r38.c) sagen nichts Falsches. Ein-Satz-Korrektur im Dossier.
* **H2 (Verhalten unter der RE1.5-KI, Menue-Option, NICHT Standard):** an der exe (`RE15_AI_FLAVOR=re15`, Lauf mR15) gehen
  die Montage-Zombies netto auf die Kamera zu (alle in jedem Schnitt naeher; vorher standen sie), aber mit dem Pendel-/
  Wende-Charakter dieser KI: 1030 Cut 7 kommen vier von fuenf Kopien nur 857..1240 naeher (radial/weg 0.36..0.55, im Bild
  `runs/fdR15/sheet_R15_cut7.png` bleiben sie als Gruppe an der Tuer, einer geht bis zur Kamera); 1030 Cut 12 stehen am
  Schnittende noch drei Aufrechte hinter dem Gitter (`sheet_R15_cut12.png`). Das Vorbild zeigt unter RE1.5 denselben
  Charakter (Rolltor 916 Bilder in v0.8.25, radial/weg bis 0.52). Dossier OFFEN 2 und der Release-Notes-Satz nennen nur den
  einen wendenden Zombie in Cut 12 — Cut 7 unter RE1.5 fehlt dort und sollte ergaenzt werden.
* **H3 (Riegel unter RE1.5):** `unit_r38_montage_s1030_re15` fordert auch unter RE1.5 PFLICHT "alle fuenf Kopien >= 1000
  naeher" (test_r38_montage.c kriterien_1030 ok1; nur die Haelften sind dort "gemeldet") und ist gruen nur fuer seine feste
  RNG-Saat (in-process kleinste 1160); die exe ueber den echten Weg liefert 857 / 962 (anderer RNG-Stand nach 1130/1040).
  Der RE1.5-Teil misst also nicht, was die exe unter RE1.5 tut; unter RE2 (Standard) sind Prozess- und exe-Zahlen gleich.
  Vorschlag: das 1000er-Kriterium unter RE1.5 wie die Haelften nur melden (oder OFFEN entsprechend ergaenzen).
* **H4 (Integration):** neue Engine-Quelle engine/src/montage_r38.c — fuer den Android-Bau den app/.cxx-GLOB-Cache neu
  konfigurieren (bekannte Release-Regel).

## 9. Ergebnis

**bestanden = ja.** Punkt 1 erfuellt (Standard-KI, echte exe, echter Weg, Gegenprobe gegen die Release-exe v0.8.25,
Bilder); Bau-, RE-, Vertrags- und Test-Gate halten; keine blockierenden Maengel; Hinweise H1..H4 nicht blockierend.

Messdateien (Scratchpad dieser Abnahme, nicht im Repo): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn/`
(`runs/{mA,mB,mR15,v0825_A,v0825_R15,fd1130,fd1030,fdR15}/`, `tools/abn_lauf.sh`, `tools/abn_lauf_v0825.sh`,
`tools/abn_ausw.py`, `tools/verlauf.py`, `tools/sheet.py`, `ctest_r38_montage.txt`, `ctest_gruppe.txt`, `ctest_lmerge.txt`).
