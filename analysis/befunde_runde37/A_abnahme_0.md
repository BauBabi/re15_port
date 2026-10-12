# Runde 37 — Spur A "pri1100": Abnahme 0 (unabhaengig)

Datum: 2026-10-12. Baum `.claude/worktrees/r37_pri1100`, Zweig `r37/pri1100`, Stand 9e71f0a1 (Basis f1e019ca).
Pruefer: Abnahme 0 (kein Code geaendert; nur diese Datei committet).
Gegenstand: nur Spur A (AUFTRAG.md #1). Der im Workflow mitgeschickte Nutzer-Nachtrag (Beretta M93R) gehoert zu
Spur G (r37_beretta) und ist hier nicht bewertet.
Messordner (ausserhalb des Baums): Scratchpad `ab0/` (Laeufer, Masken-Saetze `m_alt` = f1e019ca = v0.8.24, `m_v0823`,
`m_neu` = HEAD, Laeufe `runs/<name>/`).

## 0. Stand / Bau
- `git log master..HEAD`: 6 Commits (a0d82ed0 .. 9e71f0a1). master ist seit f1e019ca nur um Doku-/Orchestrator-Commits
  gewachsen (AUFTRAG/VERTRAG/UEBERGABE/orchestrator); die Spur-Aenderungen = `git diff master...HEAD` (27 Dateien).
- Bau: `bash re15_port/tools/local_build.sh configure` -> OK; `... build` -> `ninja: no work to do`, `LOCAL-BUILD-OK (build)`
  (exe 10:54:03, juengste Code-Quelle main.c 10:53:50 -> exe aktuell; Masken werden zur Laufzeit aus
  `<baum>/re15_port/shared_assets/PSX/MASKS` gelesen, Arbeitsbaum sauber = HEAD).


## 1. Punkt 1 (Wortlaut AUFTRAG.md #1)
> Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die
> DAteien liegen mit F9.

### 1.1 Die Marken des Nutzers (Hauptbaum, nur gelesen)
- `C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_1100_F892_marke1.bmp` (2026-10-12 10:07) +
  `befund.log` Z. 15850ff: R1100 C1, Leon (-14914,0,-25645) rot 4488, 105 Masken, Zombie 1 typ 0x10 st=1 ss=7/1
  (-16157,0,-23876) rot 1024 dist 2157, Zombie 2 (-14269,0,-25091). Bild: Kopf + Arm von Zombie 1 rechts der
  Tuerrahmenkante UEBER der rechten Wand (320er-Bild x ~170..180, y ~140..148).
- `befund_1100_F240_marke1.bmp` (2026-10-05) + `befund.log` Z. 4770ff: Leon (-15607,0,-23728) rot -1104, Zombie 1 an
  der Spawnlage (-16453,0,-23817): nur der Kopf schwebt ueber der Wand.
- Die Zahlen im Dossier 1.1 stimmen mit dem Log ueberein (selbst gelesen). Der Nutzer war per Debug-Sprung in 1100
  (F7: (-20690,-25000) rot 0 = Sprung-Spawn) und lief dann nach Osten.

### 1.2 Messung am gebauten Stand (eigene Messkopie `re15_pc_ab0.exe` neben der exe; Masken je Lauf ueber
`RE15_CD_ROOT=<ab0/m_alt|m_v0823>` vorgeschaltet, sonst die HEAD-Masken des Baums; Log-Zeile `[pri] cut=1 ...
masks=52` (neu) bzw. `masks=105` (alt/v0.8.23) belegt, welcher Satz geladen war)
Gemeinsam: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1100@250 RE15_FORCE_CUT=1 RE15_FRAMEDUMP=150:f.ppm RE15_EXIT_AT=150#1100`.
Auswertung: Bildpunkte mit Kanaldifferenz > 24 im Wandfenster x168..191 y126..149 gegen dieselbe Szene OHNE Zombie 1
(`RE15_GEGNER_POS=1,-14000,-11000,1024@5#1100`, Zombie hinter der Kamera).

| Szene | Masken | Wandfenster n>24 | Bild |
|---|---|---|---|
| F892 (Leon -14914,-25645 rot 392; `RE15_GEGNER_POS=1,-16157,-23876,1024@5#1100`) | v0.8.24 (alt) | **28** (max 176) | Kopf + Arm ueber der Wand |
| F892 | v0.8.23 | 23 | Kopf ueber der Wand |
| F892 | **neu (HEAD)** | **0** (max 0) | verdeckt; Stiefel/Hand in der Oeffnung sichtbar |
| F892 | keine (`RE15_NO_PRI=1`, gegen "ohne Zombie, ohne Masken") | 110 | ganzer Oberkoerper |
| F240 (Leon -15607,-23728 rot -1104; Zombie 1 an der Spawnlage, ohne Haken) | v0.8.24 | **25** | Kopf + Arm |
| F240 | v0.8.23 | 16 | nur der Kopf (= Nutzer-Marke F240) |
| F240 | **neu** | **0** | verdeckt |
| F240 | keine | 161 | |

- **Nachbau = Nutzer-Marke, bildpunktgenau:** Nutzer-Bild 960x720 auf 320x240 abgetastet (Pixelmitten) gegen den
  Nachbau: F892 gegen `f892_alt` im Wandfenster **0 Punkte > 24, max. Differenz 0**; F240 gegen `f240_v0823` ebenso
  **0 / max 0**. Der Nachbau zeigt also exakt das Bild des Nutzers; mit den neuen Masken ist das Wandfenster
  bildgleich zur Szene ohne Zombie.
- **Echter Weg (Tuer, ohne FORCE_CUT/PLAYER_POS in 1100/GEGNER_POS):** `RE15_DEBUG_JUMP=10D0@250
  RE15_PLAYER_POS=-19300,24000,2048` (in ROOM10D0 im Tuer-AOT: Door_aot_set @ROOM10D0.RDT+0x11D6, aot 16, sce 2,
  sat 0x31, Rechteck x-19800 z22000 w1000 d4000 -> stage 0 room 0x10 (-20975,0,-25000)), `RE15_PRESS=square@60`
  (Tuer), in 1100 `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200 RE15_INPUT_SCRIPT=U2.5,W10`
  -> `[room] PC loaded room1100.rdt`, Leon laeuft von (-20690,-25000) nach (-15115,-24995), die Kamera wechselt
  SELBST auf Cut 1 (state.txt F280 cam=1). Bilder F300/F340/F380/F420, Wandfenster gegen dieselbe Fahrt ohne
  Zombie: **neu 0**, alt 25, ohne Masken 163 (alle vier Bilder gleich; Zombie 1 liegt in st=1 ss=7/1 still).
- Leon an der Ecke (F240-Lage, Zombie weg): neu gegen alt im Leon-Kasten x150..172 y100..149 **0 Punkte**; beide
  Masken-Saetze verdecken dieselben 31 Randpunkte (x166..171) gegen "ohne Masken" -> Leon unveraendert.
- Bilder (Scratchpad `ab0/`): `f892_streifen.png`, `f240_streifen.png`, `tuerweg_streifen.png`,
  `nutzer_vs_nachbau.png`, `f240_leon.png`.

**Zwischenstand Punkt 1 fuer Leon (ROOM1100): erfuellt** — Zombie an der F9-Marke (beide Marken, Nachbau bildpunktgenau)
und auf dem echten Tuerweg hinter der Westwand verdeckt; was in der Oeffnung liegt, bleibt sichtbar.

### 1.3 Nebenwirkungen (selbst gemessen)
- **Cut 2 (Blick nach Norden), Szene des Schatten-Riegels** (Leon (-15100,-18500) rot -1024, `RE15_FORCE_CUT=2`,
  Bild 150): Log `[pri] cut=2 ... masks=53` (neu) / `masks=99` (alt). neu gegen alt **0 Punkte > 8 im ganzen Bild**
  (443 Punkte mit Differenz 1..8 = 8-bpp-Palette des Atlas). Zombie 5 hinter der Wand in beiden verdeckt, der
  liegende Zombie im Querflur gleich sichtbar (`c2_streifen.png`). Die drei Randpunkte des Riegels
  (53,237)/(52,238)/(51,239): neu == alt (8,8,8)/(8,8,8)/(16,16,16).
- **Westbahn mit Leon, Zombie 1 per Haken weg** (`RE15_PLAYER_POS=-15250,-13500,1024`, Skript `U4,R0.5,U3,W1`,
  ohne FORCE_CUT, 46 Bilder 60..330/6, Leon x=-15132 = an der Wand, Kamera 0 -> 1): neu gegen alt 224 Punkte > 24,
  davon 217 an der hellen Tuerrahmenkante x170..176 y55..100 (Palette, s. unten) und **7 an Leons Stiefel**:
  F186 (169..171,145..147) und F264 (199,180..181). RE15_TRILOG (Laeufe bis Bild 185 und 186, Differenz = Bild 186):
  die verdeckten Dreiecke sind Leon-Dreiecke (slot 0, clut 0x7840) mit **z 9856/9860**; neuer Schluessel dort
  9802 (Tiefe 152), alter 10378 (161). Rueckprojektion der Eckpunkte (Cut-1-Kamera, `geom.cut_view`):
  **x -15703..-15886** = westlich der gezeichneten Wandflaeche x=-15690 (die Stiefelspitze steckt beim Drehen in der
  Wand; Wandtiefe am Pixel (170,145) = 9473). Die neue Verdeckung ist damit geometrisch richtig; sichtbar: 2..5
  Bildpunkte der Stiefelspitze an der Wandkante in 2 von 46 Bildern. Das Dossier (4.3 "an Leon selbst keine
  Abweichung") gilt nur fuer seine Bahn, nicht allgemein -> B2 (geometrisch richtig) / M2 (Nutzertext).
- **Helle Tuerrahmenkante (Palette, vorbestehend):** Szene F892 ohne Zombie, gegen das Bild ohne Masken
  (= Hintergrund): neue Masken **37 Punkte > 24, max 54** (x172..174 y61..95); v0.8.24-Masken 25 Punkte, max 42
  (x173..174 y63..94). Grünkanal Spalte x=173: Hintergrund 186..210, alt 168, neu 160; dazu x=172 y=68: neu 160
  statt 136. Vorbestehend in beiden Fassungen, mit dem neuen Atlas leicht staerker (Beobachtung B1, kein Mangel des
  Punkts; `rahmen_streifen.png`).

### 1.4 Tests (gezielt, kein Volllauf)
`ctest --test-dir re15_port/build -R "r37_pri1100|unit_pri_|unit_sld_atlas|r36_pri|integration_pri_masken|
unit_r34n_g_maskgrp|room1100|integration_headless_run|unit_r22_10f0_figur|unit_bss_room_source|unit_map_innenwand"
--timeout 600 --output-on-failure` (PATH msys64 zuerst) -> **100% tests passed, 0 tests failed out of 19**
(integration_r37_pri1100_marke 70.0 s, integration_r36_pri_schatten 28.2 s).
- `test_r37_pri1100 leck`: Cut 1 52 Rechtecke, 8280 Punkte > 320 hinter der Flaeche, frei 0; 1300 davor, verdeckt 0;
  Cut 2 53 Rechtecke, 7552 / 0, 1345 / 0. `vergleich <ab0/m_alt/MASKS>`: Cut 1 frei **1061** (groesstes Leck 3132),
  Cut 2 frei **778** (2630) -> der Riegel ist trennscharf. `marke`: 14/14 verdeckt, mit v0.8.24-Masken 0/14.
- Riegel der Runde 36 (`test_r36_pri.c`, Randpunkt-Ausnahme) selbst gegengebaut (gcc, `RE15_PORT_SRC_DIR` auf einen
  Scratch-Baum mit ROOM1100.RDT + je einem Maskensatz, gegen die gebauten libre15_engine.a/libre15_test_support.a):
  neu -> OK (0 innen, +3 Rand); v0.8.24 -> OK; **v0.8.23 -> FAIL (31 innen, +12 Rand)** = Befund der Runde 36 bleibt
  erkannt. Die UNVERAENDERTE master-Fassung gegen die neuen Masken -> FAIL "UEBER dem Schatten 3" (wie im Dossier).
  Die Lockerung betrifft nur Silhouettenrandpunkte und ist im exe-Bild ohne Wirkung (s. 1.3).

## 2. Reproduzierbarkeit der Masken-Daten (selbst nachgebaut)
Scratch-Wurzel `ab0/repro_root` mit Kopien von auswahl.json (HEAD), ROOM1100.RDT, `build/bg_ppm/ROOM1100*.ppm` und
`build/p2/dump_klemmpfad.txt` (Eingaben des Bau-Agenten aus dem Baum), dann
`python -B <baum>/re15_port/tools/maske/raum.py ROOM1100 --stufe 16 --statistik max --out out --bild bild`:
Cut 1 "zerlegung Stufe 4 ... 52 Rechtecke, 48451 Atlaspunkte", Soll 27449 px, fehlt 0 / zuviel 0, VOR n=164
verdeckt 0, HINTER n=1511 frei 0; Cut 2 "Stufe 3 ... 53 Rechtecke, 48911 Atlaspunkte", 24358 px, VOR 176/0,
HINTER 9015/0. **Alle 7 Dateien (MSK, PRI01/02 .TIM/.PBM/.STAND) bitgleich** zu den committeten. Die Binaerdaten
sind also das Ergebnis des dokumentierten Bauwegs, nicht von Hand gesetzt.

## 3. Echtes Fenster (gdigrab) — Gegenprobe zur Framedump-Messung
`RE15_WINDOW_SCALE=3`, Szene F892 (neue Masken, sonst wie 1.2, `RE15_EXIT_AT=900#1100`), nach 16 s
`ffmpeg -f gdigrab -i "title=RE1.5 Rebuilt — PC" -frames:v 1` -> `runs/gdi_neu/gdi.png` (960x720, Fenster rendert).
Auf 320x240 abgetastet: Wandfenster gegen Framedump "ohne Zombie" **0 Punkte, max. Differenz 0**; gegen die
Nutzer-Marke F892 28 Punkte (= Kopf + Arm, jetzt verdeckt). Der Nutzer sieht im echten Fenster dasselbe wie der
Framedump.

## 4. Robustheit: Zombie 1 an 16 Lagen um die Ecke (Haken, nur Messung)
Szene F892 (Leon wie Marke, Cut 1), `RE15_GEGNER_POS=1,<x>,<z>,1024@5#1100`, x in {-16700,-16400,-16100,-15800},
z in {-23700,-23950,-24200,-24450}; je Lage neu / alt / ohne Masken. Gezaehlt: Punkte > 24 gegen "ohne Zombie"
INNERHALB der Masken-Silhouette (27449 Punkte, `maskpix.py` aus MSK+TIM).

| x \ z | -23700 | -23950 | -24200 | -24450 |
|---|---|---|---|---|
| -16700 (alt / neu) | 46 / **0** | 16 / **0** | 0 / 0 | 0 / 0 |
| -16400 | 66 / **0** | 17 / **0** | 7 / **0** | 0 / 0 |
| -16100 | 65 / **0** | 25 / **0** | 17 / **0** | 0 / 0 |
| -15800 | 56 / **20** | 14 / **7** | 9 / **4** | 0 / 0 |

Summe alt 338, neu 31. Rest nur bei x=-15800 (Zombiemitte 110 Einheiten hinter der Wandflaeche x=-15690): 4..20
Punkte des Kopfes an der Wandkante x166..170. RE15_TRILOG (Bild 150 isoliert) an diesen Punkten: Zombie-Dreiecke
(Textur-Slot 11, clut 0x7800/0x7840) mit z ZWISCHEN Wandtiefe und Schluessel, z.B. (170,145): Wand 9473 (Strahl gegen
x=-15690, Pixelmitte; bau_p2-Modell 9552), Dreieck 9743, Schluessel 9802 -> das ist das im Dossier §6 OFFEN genannte
Restband ("weniger als 256 (C1) ... kann noch ueber die Wand gemalt werden"; an Pixelmitten gemessen bis ~330,
weil bau_p2 je Bildpunkt die fernste Tiefe ansetzt). Beobachtung B3, kein Mangel des Punkts (die Marken-Lagen und
alle Lagen >= ~400 hinter der Flaeche sind leckfrei; die Grenze 105 Rechtecke / 256x256-Atlas ist im Dossier
begruendet).
- Gesamtes Weltgitter wie `unit_r37_pri1100_leck` (Schritt 100, Hoehen 0/-500/-1000/-1500, Randpunkte aus), alt
  gegen neu je Punkt: Cut 1 von 8723 Punkten hinter der Flaeche neu verdeckt / alt frei **1247**, neu frei / alt
  verdeckt **3** (158..192 hinter der Flaeche, im Restband); Cut 2 948 gegen 1. Silhouette alt/neu: Cut 1 27450 /
  27449 (einziger Unterschied (166,119) = 1-Einheiten-Versatz Suedblock), Cut 2 24358 / 24358 identisch.
- Feinere Stufen selbst geprueft (`raum.py ... --stufe 2|3 --nur-pruefen` im Scratch): C1 Stufe 2 "mindestens 69751
  Atlaspunkte (Grenze 65536)", C1 Stufe 3 55552 Punkte (scheitert an der Packung), C2 Stufe 2 58923 (Packung) — die
  Zahlen des Dossiers stimmen. Grenze 105 Masken = `RE15_PRI_MAX_MASKS_PER_CUT` (re15_pri.h: RDT-Kopf Byte[7], Spielmaximum
  105 in ROOM3000/3001 Cut 3; `8003928c lbu v0,7(a0)` selbst disassembliert). Moeglicher Folgeschritt (nicht verlangt): eine bessere Packung fuer C1
  Stufe 3 (55552 < 65536) wuerde das Restband von 256 auf 192 senken.

## 5. RE-Gate
- Code-Diff (`git diff master...HEAD -- re15_port/engine re15_port/include re15_port/platform re15_port/tools`):
  KEINE Verhaltenskonstante. Neu nur der Messhaken `RE15_GEGNER_POS` (pri1100_r37.c, ohne Variable `s_slot=-1` ->
  sofortiger Ruecksprung; PSX-Rumpf leer) und 1 Zeile main.c mit Kommentar "Runde 37 Spur A". raum.py: zwei
  Eintraege aus `P2_ALTE_KETTE` entfernt + Kommentar.
- Daten: ROOM1100 hat im Original keine Masken -> **NUTZER-VORGABE** (F205/F595), im Dossier §2, in auswahl.json
  `_warum` und in beiden Fix-Commit-Texten so gekennzeichnet. Selbst geprueft (info/Re1.5/PSX/STAGE1/ROOM1100.RDT,
  sha256 = Baum-Kopie): Kopf `00 0d 01 00 00 00 00 00` (Byte[7]=0), Kamera-Tabelle @0x24 -> 0x60, alle 13
  pri_offset (Satz +0x1C, 0x608..0x638) zeigen auf `ff ff ff ff`.
- Stichprobe 3 Adressen selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, PSX.EXE):
  1. `80039650 lh a0,2(s3)` / `80039658 sll a0,a0,2` / `8003965c addu a0,a0,s6` / `80039660 jal 0x8006b538` /
     Delay `80039664 addu a0,v0,a0` (v0 = `lbu 0(s5)` << 12 = Puffer-Offset). Sprungziel 0x8006b538 = AddPrim
     (`lui a2,0xff; ori a2,a2,0xffff; lui a3,0xff00; lw v1,0(a1); lw v0,0(a0); and/or; sw v1,0(a1)` ...). ✓
  2. `80025654 sra v0,v1,6` / `80025658 beq v0,zero,0x80025690` / `8002565c sra v1,v1,4` / `80025660 sll v1,v1,2` /
     `80025664 addu v1,s0,v1` -> OT-Wort = otz>>4. ✓
  3. `80066c70 addiu t0,zero,341` / `80066c74 .word 0x48c8e800` = ctc2 t0,cr29 (ZSF3) (und `80066c7c addiu t0,
     zero,256` / `0x48c8f000` = ctc2 t0,cr30 ZSF4). ✓
  Zusaetzlich: `8001ce54 jal 0x80039590` (Masken) vor `8001d09c jal 0x8001e8c8` (Spieler-Polygone). ✓
- Zahlen des Befunds selbst nachgerechnet (`geometrie.quader_auf_band`, alter Atlas): alte Kachel (166,129) 24x24
  t=161 -> Schluessel 10378, Wandtiefe darin 7118..10275, Leckband C1 max 3253, 11245 Punkte > 500, 4210 > 1000. ✓
- Test-Konstanten belegt: ZELLE_X -15600 / Z -23200..-12100 = SCA-Zelle 2 (selbst gelesen: x -17600..-15600,
  z -23200..-12100), Flaechen -15690/-15710 = auswahl.json (Spur O), Schwellen (Stufe+1)*65536/1023 (ZSF3/otz>>4).
- Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/vorlaeufig/geschaetzt: **0
  Treffer**. Kein Env-Schalter als Abschluss (der Haken aendert ohne Variable nichts).
- Erklaert der Fix den Befund? Ja: Nachbau mit den v0.8.24-Masken ist im Wandfenster bildpunktgenau die Nutzer-Marke
  (max. Differenz 0), die Ursache (Kachel-Maximum, Schluessel 10378 > Kopf-z 9991..10438) ist an den Daten belegt,
  und mit den neuen Masken ist genau dieses Fenster gleich der Szene ohne Zombie (Framedump UND echtes Fenster).

## 6. Vertrag (VERTRAG.md)
- Pfade (`git diff master...HEAD --name-only`, 27 Dateien): Dossier + Bilder, auswahl.json (Quelle der
  ROOM1100-Masken), engine/src/pri1100_r37.c + include/re15_pri1100_r37.h (neu, wie §1.4 verlangt), platform/pc/main.c
  (1 Zeile, Kommentar "Runde 37 Spur A"), shared_assets/PSX/MASKS/ROOM1100.* (§1.6 erlaubt fuer A), tools/maske/raum.py
  (2 Eintraege), tests/unit/probes/r37_pri1100.cmake (§1.5), tests/unit/test_r37_pri1100.c,
  tests/integration/test_r37_pri1100_marke.cmake (`cmake_minimum_required(VERSION 3.16)` ✓; `file(COPY_FILE)` wie 10
  bestehende Integrationstests, Wurzel verlangt 3.21), tests/unit/test_r36_pri.c (Riegel angepasst, s. 1.4).
  KEINE Aenderung an release/, platform/android/, Test-CMakeLists, anderen shared_assets. ✓
- Bank-9-Bits / Nachrichten-Ids / AOT-Slots / Ereignisse: keine belegt. ✓
- Messhaken: eigener Name, 1 Zeile im gemeinsamen main.c. ✓
- Paket: release/make_package.sh Z.275 prueft MASKS/ROOM1100.MSK, _PRI01.TIM, _PRI02.TIM (vorhanden); die entfernten
  .BELEG werden dort nicht verlangt. APK-Asset-Gate muss beim Release neu gebaut werden (Dossier §7, Sache der
  Integration).

## 7. Zweites Szenario (Elza) — derselbe Befund im selben Raum
- `info/Re1.5/PSX/STAGE1/ROOM1100.RDT` und `ROOM1101.RDT` sind **bytegleich** (`cmp -l` = 0 Bytes), beide spawnen
  Zombie 1 an derselben Ecke (Datei-Offset 0x9FE/0xA66: `bb bf 00 00 f7 a2` = (-16453,0,-23817)).
- Der Port bildet den Maskennamen aus der vollen Raum-Id (`entladen_pc.c:86` `"MASKS/ROOM%04X.MSK"`, `bg_pc.c:195`;
  Variante aus dem aktuellen Raum mitgeschleppt, aot_common.c:700-703). `MASKS/ROOM1101.*` gibt es nicht ->
  Elzas 1101 hat **gar keine** Wandmasken (vorher wie nachher).
- Gemessen (Elza, `RE15_TITLE_SHOT_CHAR=1`): Sprung `RE15_DEBUG_JUMP=1100@250` -> `[room] PC loaded room1101.rdt`,
  `[pri] cut=1 pri_offset=0x60C masks=0 fg_atlas=0`; state.txt: Zombie 1 `st=1 ss1=7 @(-16453,-23817,r1024)`.
  **Echter Tuerweg** wie 1.2, aber als Elza (`RE15_DEBUG_JUMP=10D0@250` -> ROOM10D1, Tuer `square@60` ->
  `[room] PC loaded room1101.rdt`, `U2.5`, Kamera selbst auf Cut 1): Bild F400 im Wandfenster gegen die Szene ohne
  Zombie **163 Punkte**, innerhalb der Wandsilhouette 207 Punkte — Kopf, Arm und Oberkoerper von Zombie 1 liegen
  ueber der Wand (`elza_vs_leon_tuerweg.png`, `elza_1101.png`); Leon an derselben Stelle: 0.
- ROOM1101 liegt auf Elzas **Pflichtweg** (Runde 36 D1_elza_pflichtweg.md §1.2: 10D1 -> 1101 -> 1111 -> 1101 -> 10D1,
  Eintritt aus 10D1 genau bei (-20975,-25000) im Suedgang; §H3 "1101 5" Gegner). Der Nutzer sieht dort den Befund
  seiner Marke unveraendert (staerker: der ganze Oberkoerper statt Kopf+Arm).
- Das Dossier erwaehnt Elza nicht; "Fuer den Nutzer" meldet ROOM1100 Cut 1/2 ohne Einschraenkung als behoben.
- Nicht von dieser Spur verursacht (vorbestehend seit den Nutzer-Masken F205/F595), aber derselbe Nutzerbefund im
  selben Raum. Behebung braucht eine Vertragsentscheidung (VERTRAG §1.6 erlaubt A nur `ROOM1100.*`): z.B.
  `MASKS/ROOM1101.MSK/_PRI01.TIM/_PRI02.TIM` (+PBM/STAND) aus demselben Bau (RDT + Hintergrund identisch) und
  Paket-Gate (make_package.sh Z.275, APK-Gate), oder ein Loader-Rueckfall auf die gerade Variante bei bytegleicher RDT.
- Zusammenhang (Hinweis, nicht Teil dieser Spur): auch 1060, 10A0, 10C0, 10E0, 10F0, 1110, 1120, 1160, 1180, 11D0,
  11F0, 1230 haben eine bytegleiche Elza-RDT, aber keine xxx1-Masken (in STAGE1 existiert nur ROOM1221.MSK).

## 8. Urteil
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1 — ROOM1100 (Leon), Zombie an der F9-Marke | **erfuellt** | Nachbau bildpunktgenau = Nutzer-Marken F892/F240 (max. Diff 0 im Wandfenster); neu 0 Punkte (alt 28/25), echter Tuerweg 0 (alt 25), echtes Fenster (gdigrab) 0; Ursache an den Daten belegt; Riegel trennscharf |
| 1 — derselbe Raum im zweiten Szenario (Elza, ROOM1101, Pflichtweg) | **nicht erfuellt** (vorbestehend) | keine Masken (`masks=0`), Zombie 1 auf dem echten Tuerweg 163 Punkte ueber der Wand |
| RE-Gate | haelt | 3 Adressen + AddPrim + Aufrufreihenfolge selbst disassembliert; NUTZER-VORGABE gekennzeichnet; keine Rate-Woerter; Fix erklaert den Befund |
| Vertrag | haelt | nur erlaubte Pfade, 1-Zeilen-Haken, keine Bits/Ids/Slots, Tests ueber probes/r37_pri1100.cmake |
| Tests | gruen + messend | 19/19 gezielt; leck/marke/integration mit Alt-Masken rot; Runde-36-Riegel mit v0.8.23 weiter rot |

**bestanden = false** — der Wortlaut-Fall (Leon) ist sauber erfuellt; derselbe Nutzerbefund besteht im
bytegleichen Elza-Raum ROOM1101 auf Elzas Pflichtweg unveraendert (M1). Die Arbeit der Spur selbst ist korrekt
und kann so bleiben; es fehlt die Elza-Haelfte.

## 9. Maengel
- **M1 (blockierend):** ROOM1101 (Elza; RDT bytegleich zu ROOM1100, Zombie 1 an derselben Ecke, Pflichtweg
  10D1 -> 1101) laedt keine Wandmasken (`MASKS/ROOM1101.*` fehlt; Log `[pri] cut=1 pri_offset=0x60C masks=0`). Auf dem
  echten Tuerweg als Elza liegen Kopf, Arm und Oberkoerper von Zombie 1 ueber der Westwand (Wandfenster 163 Punkte,
  Bild F400; Leon 0). Das Dossier prueft/erwaehnt Elza nicht, "Fuer den Nutzer" meldet ROOM1100 uneingeschraenkt als
  behoben. Abhilfe: Elza-Masken fuer 1101 Cut 1/2 aus demselben Bau (oder Loader-Rueckfall bei bytegleicher RDT) +
  Paket-/APK-Gate + Riegel fuer 1101; braucht Vertragsfreigabe (§1.6 nur `ROOM1100.*`).
- **M2 (klein, nicht blockierend):** "Fuer den Nutzer"/Commit: "Leon ... unveraendert (alt gegen neu: 0 Bildpunkte an
  Leon)" stimmt nicht allgemein: dreht Leon an der Wand (Westbahn x=-15132, F186/F264), wird die in die Wand ragende
  Stiefelspitze jetzt verdeckt (5 bzw. 2 Punkte; Dreiecke z 9856/9860 hinter der Wandflaeche, Rueckprojektion
  x -15703..-15886). Geometrisch richtig, im Nutzertext aber zu nennen.

## 10. Beobachtungen (kein Mangel)
- B1 Helle Tuerrahmenkante Cut 1 (x172..174 y61..95): 8-bpp-Atlas-Palette weicht vom Hintergrund ab — vorbestehend
  (v0.8.24: 25 Punkte > 24, max 42), mit dem neuen Atlas 37 Punkte, max 54.
- B2 = M2 (Stiefelspitze), geometrisch korrekt.
- B3 Restband (Dossier §6 OFFEN, selbst gemessen): Zombie 1 nur 110 Einheiten hinter der Wandflaeche (x=-15800)
  -> 4..20 Punkte des Kopfes an der Wandkante (alt 9..56); alle Lagen >= ~400 dahinter leckfrei. Feinere Stufe
  scheitert an Atlasflaeche/Packung (selbst nachgerechnet); C1 Stufe 3 (55552 < 65536) scheitert nur an der Packung.

## 11. Messlaeufe (Scratchpad `ab0/`, Laeufer `run.sh`, Auswertung `vgl.py`, `maskpix.py`)
- Basis-Umgebung je Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
  RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`, exe `re15_port/build/platform/pc/re15_pc_ab0.exe` (Kopie, nach
  der Abnahme geloescht).
- `f892_{neu,alt,v0823,nopri,ohne,ohne_nopri,ohne_alt}`, `f240_{...}`, `c2_{neu,alt,nopri,v0823}`,
  `tw_{neu,alt,nopri,ohne}` (Tuerweg 10D0), `wb_{neu,alt,nopri}` + `wbt185/186` (Westbahn + TRILOG),
  `sw_{n,a,p}_<x>_<z>` (16 Lagen), `swt149/150` (TRILOG), `gdi_neu` (gdigrab), `elza_1101`, `elza_tw` (Elza-Tuerweg 10D1),
  `repro_root` (raum.py-Nachbau), `t36_{v0823,alt,neu}.exe` / `t36m_neu.exe` (Riegel-Gegenbau).
