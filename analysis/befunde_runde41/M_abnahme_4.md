# Spur M masken36 — Abnahme 4 (unabhaengig, 2026-10-25, nach Nachbesserung 4)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf c42c3049; Nachbesserung 4 =
5b8300c1..08e36f97, danach nur c42c3049 = Orchestrator-Eintrag M-E2 in AUFTRAG.md). Pruefer: Abnahme 4. Massstab: Wortlaut
AUFTRAG.md (Punkte 1/2, Nachtrag 11, Orchestrator-Hinweis 03:21, Nachtrag 27 / M-E1, **M-E2 14:29**), VERTRAG.md §1.3/§2,
CLAUDE.md RE-Gate, Maengel 1-3 der Abnahme 3 (`M_abnahme_3.md`). Bericht wird fortlaufend geschrieben (Sitzungslimit).
Messordner (Scratchpad): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/M41abn4/`.

## Stand
- [x] Lektuere AUFTRAG (Spurbaum bis M-E2 14:29, master bis 73ed0756), VERTRAG, Dossier NB4, Abnahme 3; `git log master..HEAD`, NB4-Diff `1ad2c96e..HEAD` gelesen
- [x] Bau: `local_build.sh configure` OK, `build` -> "ninja: no work to do"
- [x] Gezielte Tests: unit 32/32, Integration 28/28
- [x] Mangel 1 (Ost-Stirn 30A0/30A1 C0) selbst gemessen: Widerlegung bestaetigt fuer die Stirnflaeche; Rest-Loch roter Ordner (32 px)
- [x] Mangel 3 (Laufpruefung) selbst gemessen: behoben
- [x] Restliste nicht schlechter (M-E1): MASKS/Engine/Include/Plattform bytegleich NB3
- [x] M-E2: nicht umgesetzt (Entscheid nach der NB4)
- [x] Punkt 2 (STAGE2): unveraendert, eigene Elza-Stichprobe
- [x] RE-Gate, Vertrag
- [x] Urteil: **nicht bestanden** (Punkt 1 teilweise: M-E2 offen + roter Ordner; Punkt 2 erfuellt)

## Neuer Entscheid waehrend der Abnahme: M-E2 (AUFTRAG.md 2026-10-25 14:29, Spurbaum c42c3049 / master a90bce6f)
NACH dem Abschluss-Commit der NB4 (08e36f97, 14:26:21) hat der Orchestrator den Entscheid **M-E2** eingetragen ("ersetzt M-E1 fuer
Original-Teile"): "Spur M setzt die Teile des Original-Atlas ein, in ALLEN Kameras (auch auf der Restliste von Nachtrag 27), sofern je
Teil Lage (Bildvergleich gegen den Hintergrund) und Tiefe (Zelle mit RDT-Offset) belegt sind ... Den belegten Generator-Fix 'Tuer-Band
der Saaten' (AOT-Scan pruefen pc[4] @0x80042cac..0x80042ccc) mitnehmen; teilen_r41.py fuer 6000 C5 NICHT". master-UEBERGABE 14:30:
"M Abnahme 4 laeuft ... -> danach M-Lauf fuer M-E2. Integration nur mit bestandenem Stand." Der gebaute Stand (NB3-Masken, Vorschlaege
nicht angewandt) erfuellt M-E2 nicht - das ist kein Versaeumnis der NB4 (Entscheid kam danach), aber ein offener Auftrag (Mangel 1 dieser
Abnahme, unten).

## Mangel 1 der Abnahme 3 (Ost-Stirn der Buero-Trennwand 30A0/30A1 Cut 0) - selbst gemessen
Messmittel: eigener Hintergrund (`probe_bg_dump` Stage 3 -> `M41abn4/bg3/ROOM30A0{0..3}.ppm`), eigene Kamera (`py/geo.py`, Formel
wie `re15_camera_build_view` FUN_80053ca4, RDT+0x24 Satz 32 B: Cut 0 pos (-16896,-3600,9440) Ziel (-22078,-2090,11554) H 208), eigener
Masken-Leser (`py/mskio.py`: gezeichnete Deckung wie Lader+Renderer; Summe Spalten >= 282 = **3344**, = Zahl des Bau-Agenten),
SCA selbst gelesen (Zelle 6 @RDT+0x0045C `96 19 e2 04 63 98 2d 37 01 ff 00 03` = x -26525..-19975 z 14125..15375; Zelle 8 @0x00474
x -28575..-19375 z 18225..20675; Zelle 9 @0x00480 Typ 3 x -20669..-19484 z 17213..18398).

**(a) Was zeigt das Bild rechts der gezeichneten Platte? (ohne Annahme x = -19975)**
- Unterkante der Platte = Bodenberuehrung, Boden-Schnitt y = 0 (`py`, Strahlen der Grenzpunkte Platte -> Boden in Cut 0): (290,205) ->
  (-19951, 14396), (295,203) -> (-19957, 14596), (300,201.5) -> (-19949, 14781), (303,200.5) -> (-19947, 14898). Die Platte steht also
  auf x ~ -19950 und reicht am Boden von z ~14244 bis **~14900** (Zelle bis 15375).
- Rechts davon, Zeilen 160..196 x 310..319: mittlere Farbe RGB (75,81,94), B-R 18,9 = **Teppich** (Teppich links im Bild (80,87,100) B-R
  20,7; vor der Trennwand (60,65,77) B-R 16,6); die Platte ist neutral (20,21,22) B-R 2,2. Dazu weisse/beige Papiere (lum 145..160) an
  (304..311,192), (311..315,180), (310..320,166..171). Der Boden bei (-19945, 0, 15000..15300) projiziert nach (306..313, 200..198) und
  ist dort Teppich (lum 64..81).
- Die dunkle Flaeche Zeilen 123..154 x 314..319 (lum < 20, 109 Punkte) ist kein Teil der Trennwand: unter ihr (Zeilen 158..196, gleiche
  Spalten) ist Teppich bis z 17661 sichtbar (Strahl (312.5,160.5) -> Boden (-21471, 17661)); ein auf dem Boden stehender Gegenstand
  zwischen z 14900 und Leons z waere bis Zeile ~190..196 zu sehen (Unterkante (-20150..-19975, 0, 15375) -> (307.6..313.9, 192.9..196.4)).
  Die Strahlen der Flaeche treffen z = 18225 (Zelle 8, Rueckwand-Tische) in Hoehe 225..1400 (Cut-3-Gegenprobe: dort dunkel, lum 6..63).
- Ergebnis: an den Bildpunkten, an denen Leon in den Abnahme-3-Stellen sichtbar ist, zeigt das vorgerenderte Bild Teppich bzw. den
  Hintergrund hinter ihm (einzige Ausnahme: der rote Ordner ueber der Platte, 32 Bildpunkte, s. (c)). Ein vorgerenderter Bodenpunkt P ist nur sichtbar, wenn auf dem Strahl davor nichts steht; Leons Punkt liegt auf
  demselben Strahl vor P (ueber dem Boden) -> dort darf keine Maske liegen. Die Abnahme-3-Erwartung (Maske ueber der ganzen
  Zellenflaeche bis z 15375) wuerde Leon an einer Kante abschneiden, die es im Bild nicht gibt. **Mangel 1 ist in der Form der Abnahme 3 (Zellenflaeche als Gegenstand) widerlegt** - bis auf 32 Bildpunkte am roten Ordner ueber der Platte, die ein echtes Loch sind (s. (c), Mangel 2 dieser Abnahme).
- Bilder: `M41abn4/bilder/stirn_roh_vs_gezeichnet.png` (12x, links roh, rechts gruen = gezeichnet: Kante der Zeichnung = Kante der Platte),
  `ov30A0_c0_ost.png` (Zelle 6 magenta, Kante z 14870 gelb, Leon-Achsen rot).

**(b) An der exe (Messkopie `M41abn4/exe/re15_pc_m41abn4.exe`, md5 5b22426f = Bau-exe; `run.sh`: RE15_DEBUG_JUMP=30A0@250,
RE15_PLAYER_POS, RE15_FORCE_CUT=0, RE15_FRAMEDUMP=119-120/1:f, RE15_EXIT_AT=120#<raum>; Lauf gilt nur mit Framedump f000120 + EXIT_AT
NACH "loaded room30a0.rdt"; je Stelle mit Masken / RE15_NO_PRI / leer (Figur bei (-19800,19763) ausserhalb des Bilds); Auswertung
`py/szene2.py`: Figur = |B - C| > 24 mit C = Hintergrund, sichtbar = |A - A0| > 24, uebermalt = Figur und |A - B| > 24)**

| Stelle | Platz, Gier | Figur | sichtbar | davon auf gezeichneter Maske | uebermalt (alle auf gezeichneten Masken) |
|---|---|---|---|---|---|
| a30A0 (Abnahme 3) | (-20600,16000) 1024 | 1960 | 787 | 0 | 1157 |
| o3 (Abnahme 3) | (-20850,16300) 2048 | 1509 | 546 | 0 | 956 |
| o2 (Abnahme 3) | (-20475,16300) 2048 | 716 | 484 | 0 | 229 |
| o5e Elza ROOM30A1 | (-20475,16300) 2048 | 486 | 403 | 0 | 83 |
| v1 VOR der Platte (eigene) | (-19500,14500) 1024 | 2152 | 2130 | 824 Figurpunkte ueber Masken, alle sichtbar | **0** |
| v3e Elza VOR der Platte | (-19500,14500) 1024 | 1782 | 1775 | 369, alle sichtbar | **0** |
Sichtbar ist Leon/Elza nur ausserhalb jeder Maske (rechts der Plattenkante, ueber Teppich/Hintergrund); hinter der Platte ganz verdeckt;
vor ihr nichts uebermalt. Bildboegen `M41abn4/bilder/szene_a30A0.png`, `szene_o3.png`, `szene_o2.png`, `szene_o5e.png` (A0 | Spiel |
ohne Masken + Rand der Zeichnung): Leon steht sichtbar hinter dem Plattenende, sein rechter Teil ueber dem Boden dahinter.

**(c) Rest-Loch UEBER der Platte (neuer Befund, Mangel 2 unten):** rechts oberhalb der Stirnplatte stehen zwei Ordner auf dem Schrank
am Ostende (Cut 3 zeigt dort einen weissen Schubladenschrank mit gruenem und rotem Ordner, Vorderseite z ~15390 nach Bodenschnitt
(200,142) -> (-20737,15392)). Der gruene Ordner (Cut 0 x 304..315 y 108..117) ist gezeichnet (Maskenteile R24/R17/R100, Tiefe 76..87),
der **rote Ordner (Cut 0 x 310..317 y 106..111, 32 Bildpunkte) hat keine Maske** (`mskio`: 0 von 32 gezeichnet). Lage selbst bestimmt
(`py`, Triangulation Cut 0 + Cut 3 aus den Schwerpunkten der roten Bildpunkte (314.5,109) / (213.2,64.2)): (-20231, -2423, 14996),
Strahlabstand 164 (gruen: (-20285, -2394, 15059), 126). Gegenprobe ohne Triangulation: der Cut-0-Strahl durch (314.5,109) trifft in
Cut 3 bei z 14800 rotgrau (123,92,94), bei z 14996 gruen (34,105,83), am Rueckwand-Punkt z 18225 dagegen hellgrau (182,191,197) - der
rote Ordner steht also bei z ~14800..15000, VOR der Figur (Leon/Elza hinter der Trennwand bei z >= 15550). An der exe wird die Figur
darueber gezeichnet: a30A0 32/32, o3 32/32 Punkte auf dem roten Ordner sichtbar; im natuerlichen Gehlauf (`walk.sh`, Cut 0 durchgehend,
(-19000,16200) Gier 2048 "U5") Leon F51..F60 26/32/32/9, Elza (30A1) F54/F57 10/19 Punkte. Bild `M41abn4/bilder/roter_ordner_F54.png`
(links Hintergrund, rechts Spiel F54: der gruene Ordner liegt vor Leon, der rote daneben hinter ihm). Weder die NB4-Aussage "fehlt KEINE
Maske" am Ostende noch die Liste "Fuer den Nutzer: Freistellungen" (30A0 C0 nur "Lampe, Stuhl, Moebel vorn links") erfasst ihn; der
NB4-Vorschlag platzieren_r41.py setzt in 30A0 C0 nichts (Bogen NB4_bogen_ROOM30A0_C00.png "magenta neu 0").

**(d) Nebenbefund (kein Mangel der Spur, Hinweis):** Die Nachbarkamera Cut 2 passt an der Trennwand-Ecke nicht zu Cut 0: der Notizzettel
auf der Platte (Cut 0 (305,133)) liegt nach der RDT-Kamera von Cut 2 bei (-12,144) (ausserhalb), im Cut-2-Bild aber bei (0..5,116..148);
die graue Flaeche reicht in Cut 2 (Kamera RDT+0xA0) bis z ~15300 auf x = -19975, in Cut 0 nur bis ~14900. Die zwei vorgerenderten
Ansichten widersprechen sich dort um ~12..15 Bildpunkte; der Versatz ist gleichfoermig (auf der Ebene x = -19975 liegen in Cut 2 Rahmen->Platte bei z ~14730..14815 statt ~14255, Platte->Boden bei ~15290..15360 statt ~14870, je ~+500), also Bild oder Kamera von Cut 2 versetzt, nicht eine andere Platte. Fuer Cut 0 (die Stelle des Mangels)
entscheidet das Cut-0-Bild; Cut 2 wurde von der NB4 nicht beruehrt.

## Mangel 3 der Abnahme 3 (Laeufer ohne Laufpruefung) - selbst gemessen
- Code gelesen (`tests/integration/test_r41_masken36_stage2.cmake` Z. 51-77, 186-223): je Lauf werden `f.ppm` und `debug.log` vorher
  geloescht; gueltig nur, wenn NACH dem letzten `loaded room<zr>.rdt` (klein, wie das Log schreibt: "[room] PC loaded room30a0.rdt")
  die Zeile `[framedump] <w>x<h> -> f.ppm` und danach `[flow] EXIT_AT: Bild 120 in Raum <zr> erreicht` stehen; sonst bis zu 3 Versuche,
  dann FATAL_ERROR. Mechanik in main.c nachgelesen: Framedump-Anforderung vor `re15_render_end_frame()`, EXIT_AT-Pruefung danach
  (`(long)g_engine.frame_count >= s_ea_frame && g_current_room_id == s_ea_room`), `g_engine.frame_count = 0` beim Raumwechsel
  (main.c Z. 8237) - der Boot-Raum schreibt f.ppm bei seinem Bild 120 (im Log gesehen: zwei Framedumps je vollem Lauf).
- Eigene Gegenprobe mit ECHTEN Logs (`M41abn4/lp/`, Funktion unveraendert aus dem Laeufer ausgeschnitten, `cmake -P lp/pruef.cmake`):
  | Lauf | Herstellung | Ergebnis der Pruefung |
  |---|---|---|
  | voll | EXIT_AT=120#30A0 | ANGENOMMEN |
  | ex60 | EXIT_AT=60#30A0 (Zielraum geladen, f.ppm = Boot-Bild) | abgelehnt: kein EXIT_AT Bild 120 nach dem Laden |
  | t9 | `timeout 9` (Zielraum geladen, vor seinem Framedump getoetet = Fall d4040_09 der Abnahme 3) | abgelehnt (dito) |
  | t6/t7/t8 | `timeout 6/7/8` (vor dem Laden getoetet, f.ppm = Boot-Bild) | abgelehnt: Zielraum nie geladen |
  | nordump | voller Log, zweite Framedump-Zeile durch "SDL_RenderReadPixels failed" ersetzt | abgelehnt: kein Framedump im Zielraum |
- Riegel `integration_r41_masken36_laufpruefung` (PASS nur mit "Gegenprobe OK", -DGEGENPROBE=1): Ergebnis s. Tests. Im Gruppenlauf
  `integration_r41_masken36_stage2` 18 Szenen OK ohne Wiederholung (467 s).
- **Mangel 3 ist behoben.**

## Restliste "keine Stelle schlechter" (M-E1)
`git diff 1ad2c96e..HEAD -- re15_port/shared_assets re15_port/engine re15_port/include re15_port/platform` = **leer** (MASKS, Ausschluss-
Tabelle `gen/masken36_ausschluss_r37.inc`, `re15_pri.h`, Plattform-Code bytegleich zum Stand der Nachbesserung 3, den die Abnahme 3
gemessen hat). Die exe ist nach der Ruecknahme neu gebaut (re15_pri.h / .inc mtime 13:45:39, exe 13:46:45; `local_build.sh build` ->
"ninja: no work to do"); die md5 weicht von NB3 (597ba53a) nur durch eingebettete Bauangaben ab ("Oct 25 2026", Version aus git).
Im Spiel: 30A0 C0 "[pri] cut=0 ... masks=101 fg_atlas=1" wie NB3. Keine Stelle der Restliste ist schlechter geworden.

## Punkt 2 - die 53 Nutzer-Masken 11A0/STAGE2 (Sichtpruefung)
- NB4 und NB3 beruehren keine STAGE1/2-Maske (`git diff 1ad2c96e..HEAD --name-only -- re15_port/shared_assets` leer); im Zweig gegenueber
  master nur 2030 C7, 2040 C7, 20A0 C12 (NB0/NB1, abgenommen). 5924939d ist Vorfahr des Zweigs; `git log 5924939d..master -- pri/` =
  nur 919cc1ed (Uebersichtsbilder STAGE3-6 durch den Orchestrator), `git status pri/` leer: keine neuen Nutzer-PRIs.
- Riegel `integration_r41_masken36_stage2` gruen (18 Szenen; V 0 uebermalt, H 538/629 .. 824/871, K 86/779 bzw. 124/648 Elza).
- Eigene Stichprobe mit ELZA an Stellen, die der Riegel nur mit Leon misst (run.sh, Raum xxx1 mit Zwillingsmasken; Auswertung mit dem
  Werkzeug des Riegels `probe_r37_masken36_ppm` im Riegel-Fenster, C = eigener `probe_bg_dump` Stage 2):
  | Szene | Raum/Cut | Platz Band | S | H | Soll |
  |---|---|---|---|---|---|
  | e2040_09v | 2041 C9 | (5700,-10200) 3 | 2787 | **0** | V: 0 |
  | e2030_11v | 2031 C11 | (800,17050) 6 | 1369 | **0** | V: 0 |
  | e2040_09h | 2041 C9 | (4500,-16000) 3 | 441 | 417 | H >= S/2 |
  | e2080_03h | 2081 C3 | (-9700,-9900) 8 | 924 | 882 | H >= S/2 |
  | e11A1_01h | 11A1 C1 | (3025,15950) 1 | 871 | 824 | H >= S/2 |
  | e2050_00v | 2051 C0 | (8900,0) 3 | 3141 | 57 | V: 0 - siehe Hinweis H2 |
- Hinweis zu 2050/2051 C0 (kein Mangel, aber gemeldet): (8900,0) liegt in der Kamerazone 0 -> 1 @RDT+0x002BC (x 7800..10300,
  z -1100..400); im Spiel zeigt dort Cut 1, die V-Szene v2050_00 prueft keinen natuerlichen Cut-0-Blick. An natuerlichen Cut-0-Stellen
  (z 450..1600, x 8650..9150, Zone 1 -> 0 @0x002E4) verdeckt die Gelaender-Freistellung (pri/STAGE2/2050/00.png, Ebene y -5400) bei Leon
  344..505 und bei Elza 329..683 Figurpunkte (Unterschenkel/Fuesse, `M41abn4/bilder/p2_2050_leon_paare.png`, `p2_2051_sweep.png`).
  Gemessen: die RDT-Kamera C0 trifft die Streifenkante des Stegs (x 9900, y -5400) im Bild genau (`bilder/ov2050_c0.png`); das gemalte
  Gelaender liegt danach bei x ~8760..8850 (Handlauf y -6300 an drei Bildpunkten), die Kollisionszelle 30 @RDT+0x00A78 (x 7850..8350)
  450..950 weiter westlich. Mit dem gemalten Gelaender kreuzt der Sehstrahl zu Leons Schuhen bei (9150,900) den nahen Handlauf (Hoehe
  -6286 bei x 8800) - die Verdeckung der Unterschenkel durch den nahen Handlauf ist mit dem Bild vereinbar. Begehbar ist ausserdem der
  Streifen x 8350..~8800, der im Bild ausserhalb des Gelaenders liegt (Bestand des Originals, nicht Spur M).
- Urteil Punkt 2: wie in den Abnahmen 1..3 **erfuellt** (Stand unveraendert, keine neue Abweichung, die als Fehler belegt ist).

## RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)
1. **AOT-Band-Pruefung @0x80042cac..0x80042ccc** (NB4: Tuer-Band der Saaten, Werkzeug `--saat-tuer`, Grundlage von M-E2):
   `80042cac lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cb8 bne v0,zero,0x80042cd4` (0x80 = jedes Band) / `80042cc0 lbu v1,130(s1)`
   (Spieler +0x82) / `80042cc4 lbu v0,2(s0)` / `80042ccc bne v1,v0,0x8004301c` (anderes Band -> Satz uebersprungen). s0 = Zeiger aus der
   Zonen-Tabelle `fp = 0x800ac784 + 556 = 0x800ac9b0` (`80042c48 addiu fp,a0,556`, Zaehler `80042c30 lbu s7,-1100(s7)` = 0x800afbb4), wie
   RE15_FUN_CATALOG Zeile 151 ([2] = Band). ROOM6000 @0x00D58 selbst gelesen `3b 00 02 31 02 00 d0 07 98 08 40 06 4c 04` -> pc[4] = 02.
   **stimmt.**
2. **Figuren-Cull @0x80039d68/@0x80039d70** (NB4: Zitat-Berichtigung "Akteur-Cull FUN_8002c18c"): `80039d64 lw a1,0(s6)` /
   `80039d68 jal 0x80014368` / `80039d6c addu a0,s1,zero` / `80039d70 beq v0,zero,0x80039e3c`; Sprungziel 0x80014368 = Punkt-im-Viereck
   (`lh t5,4(a1)`, `lh t0,6(a1)`, `lw v0,8(a0)`, `subu`/`mult` Kreuzprodukte). **stimmt.**
3. **SLD-Entpacker FUN_800c47e8** (NB4: Kuenstler-Atlas der Stirnplatte): Aufruf `80021db8 jal 0x800c47e8` mit `80021db4 lw a1,0(a0)`
   (Groesse), `80021dbc addiu a0,a0,4` (Strom), `80021da8/ac` a2 = 0x801a5800 (Ziel) - wie der Kopf von `engine/src/sld_common.c`. **stimmt.**
4. Nachgelesen: FUN_80039270 `8003928c lbu v0,7(a0)` (Maskenzahl als Byte, `sll 2` / `sll 5`) = Beleg "keine feste Grenze 105" des
   Vorschlags (nicht angewandt). Zellbytes ROOM30A0 @0x0045C / @0x00474 selbst gelesen; ROOM30A0.RDT md5 5c3baa6e = info/Re1.5/PSX.
- Diff NB4 (`1ad2c96e..HEAD -- re15_port`) auf deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt|vorerst|ungefaehr|
  sieht richtig|platzhalter|heuristi: **0 Treffer**; Dossier-Abschnitt NB4 ebenso 0; **0 neue getenv** im ganzen Zweig.
- Neue Konstanten der NB4: STIRN_X/Z0/Z1 = Zelle 6 @RDT+0x0045C; STIRN_KANTE_Z 14950 = Messwert 14834..14904 + Rand (Riegel-Schwelle,
  kein Verhalten); PR 450 jetzt als PORT-WAHL gekennzeichnet (kacheln_r41.py, test_r37_masken36.c); Schwellen von platzieren_r41.py
  (RMS <= 18,91, Verhaeltnis >= 1,5) an 422 Generator-Teilen kalibriert - Werkzeug, nicht angewandt.
- Fix erklaert den Befund: Mangel 1 - ja fuer die Stirnflaeche (die Abnahme-3-Zahl entstand aus der Zellenflaeche als Verdecker; das
  Cut-0-Bild zeigt dort Teppich/Hintergrund, selbst gemessen), nicht fuer die 32 Punkte am roten Ordner (Mangel 2 dieser Abnahme); Mangel 3 - ja (Boot-Framedump als f.ppm; mit echten Abbruch-Logs abgelehnt).
- Kleine Doku-Abweichung (Hinweis): Kopf von `test_r41_masken36_fern_szenen.cmake` nennt die Kante "z 14802..14867", Dossier und
  `unit_r41_masken36_stirn` (Ausgabe selbst gesehen) "z 14834..14904".

## Vertrag (VERTRAG.md §1.3/§2)
- `git diff master...HEAD --name-only`: nichts unter release/, platform/android/, pri/, synchro/, keine CMakeLists/RELEASE_NOTES;
  shared_assets nur PSX/MASKS/. NB4-Pfade: Dossier, tests/integration/{test_r41_masken36_stage2.cmake, test_r41_masken36_fern_szenen.cmake},
  tests/unit/{probes/r41_masken36.cmake, test_r37_masken36.c, test_r41_masken36.c}, tools/maske/{kacheln_r41.py, platzieren_r41.py,
  teilen_r41.py}, M_bilder - alles Spur-Dateien; AUFTRAG.md nur durch Orchestrator-Commits (d2621352, c42c3049). Keine Bank-9-Bits,
  Nachrichten-Ids, Slots, Ereignisse (M hat keine Zuteilung).
- Riegel der NB4: `unit_r41_masken36_stirn` (messend: Zellbytes, 0 Kerben, Kante z 14834..14904, 0 Punkte jenseits, Leon = Elza;
  Ausgabe selbst gesehen), 6 Szenen im Riegel `_fern` (z30A0_o3/_o4/_a0 Art Z, v30A0_o2/_a0, v30A1_o5 Art V),
  `integration_r41_masken36_laufpruefung` (Gegenprobe). Linux: nur cmake-Skript/C ohne neue Abhaengigkeit; eigenes exe-Praefix r41m6,
  eigener Laufordner.

## M-E2 am gebauten Stand (Punkt 1, AUFTRAG 2026-10-25 14:29)
- MASKS/Ausschluss-Tabelle/re15_pri.h = NB3 (s.o.): die 437 nie platzierten Original-Atlas-Teile (`tools/maske/platzieren_r41.py`) sind
  NICHT eingesetzt, die Tuer-Band-Korrektur der Saaten (`test_r37_masken36 --saat-tuer`, Vorgabe `g_saat_tuer = 0`) ist NICHT angewandt;
  `teilen_r41.py` ist (richtig) nicht angewandt.
- Fuer den folgenden Lauf nachgelesen: der Vorschlag setzt die Tiefe je Teil nach der Generator-Regel (Kopf platzieren_r41.py Regel 5:
  "Bodenkontakt der Komponente in dieser Spalte auf y = 0, DEPTH_SAFETY 0,90"); M-E2 verlangt je Teil "Tiefe (Zelle mit RDT-Offset)".
  Kapazitaet: der Vorschlag braucht 128 Rechtecke je Cut (37 volle Cuts); Beleg der Original-Seite selbst gelesen (FUN_80039270
  `8003928c lbu v0,7(a0)`, Byte-Zahl).

## Tests (gezielt, Spurbaum, KEINE volle Suite)
1. `ctest --test-dir re15_port/build -R "^(unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_bss_room_source|prop_bss_chunk|unit_r38_itemorte|unit_r39_optik|unit_r39_titelversion_maske)" -j 3 --timeout 1800 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 32`** (unit_r37_masken36_ausschluss 358 s, unit_r41_masken36_fern 198 s; Protokoll
   `M41abn4/ctest_unit.log`). `test_r41_masken36 stirn` einzeln: 5 x PASS (Zelle 6 wie belegt, 0 Kerben, Kante z 14834..14904 in 77 Zeilen,
   0 Punkte jenseits z 14950 von 2877, Leon = Elza).
2. `ctest --test-dir re15_port/build -R "^(integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate|integration_r37_elzaems_elza5091|integration_r36_dachkiste|integration_r37_beretta|integration_r37_itemband)" -j 2 --timeout 2700 --output-on-failure -V`
   (Masken-/PRI-Riegel + Verhaltens-Riegel der Raeume mit geaenderten MASKS, wie Abnahme 3) -> **`100% tests passed, 0 tests failed out of 28`**
   (4105 s; `M41abn4/ctest_integ.log`): r41_masken36_stage2 18 Szenen OK, _zelle 16, _zellen3d 15, _fern 15 (davon die 6 NB4-Szenen:
   z30A0_o3/_o4/_a0 sichtbar 0, v30A0_o2/_a0, v30A1_o5 uebermalt 0), **_laufpruefung**: "abgebrochener Lauf abgelehnt: kein '[flow] EXIT_AT:
   Bild 120 in Raum 30a0' nach dem Laden des Zielraums" -> "voller Lauf angenommen (Versuch 2) - Gegenprobe OK"; in der ganzen Gruppe
   0 Wiederholungen ("Lauf ungueltig" 0 x). Parallel liefen eigene exe-Messungen (Last), kein Riegel wurde rot.

## Gates
- RE-Gate: **haelt** (3 Stellen selbst disassembliert + 1 nachgelesen, Zell- und Tuerbytes selbst gelesen, keine Rate-Woerter, keine neuen
  Schalter; neue Konstanten belegt oder als Riegel-Schwelle/PORT-WAHL gekennzeichnet).
- Vertrag: **haelt** (Pfade, keine Zuteilungen noetig, Registrierung nur in der Probe-Datei der Spur).
- Tests: unit 32/32, Integration 28/28.

## Urteile je Punkt (gegen den Wortlaut, AUFTRAG.md massgeblich inkl. M-E2)
| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | NB4-Teil bestaetigt: Mangel 1 der Abnahme 3 in seiner Form widerlegt (Cut-0-Bild zeigt rechts der Platte Teppich/Hintergrund HINTER der Figur; Platte bei x ~-19950, z 14244..~14900 aus dem Bodenschnitt; exe: sichtbar nur ausserhalb der Masken, VOR der Platte 0 uebermalt, Leon und Elza), Mangel 3 behoben (Laufpruefung mit echten Abbruch-Logs abgelehnt/angenommen), Restliste nicht schlechter (MASKS/Engine bytegleich NB3). OFFEN: (a) **M-E2** (14:29, nach der NB4): Original-Atlas-Teile in allen Kameras + Tuer-Band-Korrektur sind nicht eingesetzt; (b) Rest-Loch **roter Ordner** am Ostende 30A0/30A1 C0 (32 Bildpunkte vor der Figur, ohne Maske, auf dem natuerlichen Weg) |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **erfuellt** | unveraendert seit NB1 (keine neue Nutzer-PRI), Riegel stage2 gruen (18 Szenen), eigene Elza-Stichprobe V 0 / H 91..95 %; Hinweis 2050 C0 |

## Maengel (nummeriert, nachpruefbar)
1. **M-E2 nicht umgesetzt (Punkt 1; Entscheid AUFTRAG.md 2026-10-25 14:29, kam NACH dem NB4-Abschluss 08e36f97 14:26).** Gebaut ist
   der Stand NB3 (`git diff 1ad2c96e..HEAD -- re15_port/shared_assets re15_port/engine re15_port/include` leer): die 437 nie platzierten
   Teile des Original-Vordergrundatlas (`tools/maske/platzieren_r41.py`, 74 Sektionen, laut Dossier +160 539 Punkte) sind nicht
   eingesetzt, die belegte Tuer-Band-Korrektur der Saaten (`test_r37_masken36 --saat-tuer`; AOT-Scan @0x80042cac..0x80042ccc selbst
   disassembliert, s. RE-Gate) ist nicht angewandt. Erwartet laut M-E2: je Teil Lage (Bildvergleich gegen den Hintergrund) UND Tiefe aus
   einer Kollisionszelle mit RDT-Offset (der Vorschlag setzt die Tiefe bisher nach der Generator-Regel "Bodenkontakt y = 0, x 0,90",
   Kopf platzieren_r41.py Regel 5 - dafuer fehlt der Zellbeleg je Teil), Kapazitaet je Cut belegt (FUN_80039270 @0x8003928c), jede der 74
   Kameras mit mindestens einer HINTER- und einer DAVOR-Stelle an der exe (Leon, Elza wo erreichbar), Rueckschritt-Suche wie NB3, Riegel;
   `teilen_r41.py` (6000 C5) NICHT; danach "Fuer den Nutzer: Freistellungen" neu (nur was dann noch fehlt).
2. **ROOM30A0/30A1 Cut 0: roter Ordner auf dem Schrank am Ostende der Trennwand ohne Maske (Punkt 1, Rest des Abnahme-3-Mangels 1).**
   Bildpunkte x 310..317, y 106..111 (32; `mskio`: 0 gezeichnet), daneben der gruene Ordner x 304..315 y 108..117 gezeichnet (R24/R17/R100,
   Tiefe 76..87). Lage: Triangulation Cut 0 + Cut 3 (-20231, -2423, 14996), Strahlabstand 164; Gegenprobe entlang des Cut-0-Strahls
   (Cut 3 rot bei z ~14800, Rueckwand z 18225 hellgrau) - der Ordner steht in Zelle 6 @RDT+0x0045C (z 14125..15375) VOR der Figur.
   An der exe: a30A0 (-20600,16000) und o3 (-20850,16300) je 32/32 Punkte der Figur auf dem Ordner sichtbar; natuerlicher Gehlauf
   (-19000,16200) Gier 2048 "U5", Cut 0: Leon F51/F54/F57/F60 26/32/32/9, Elza F54/F57 10/19 (Bild `M41abn4/bilder/roter_ordner_F54.png`).
   Die NB4-Aussage an den Nutzer "Am Ostende ... fehlt KEINE Maske" trifft fuer diesen Gegenstand nicht zu; er steht auch nicht in der
   Liste "Fuer den Nutzer: Freistellungen". Erwartet: Ordner mit der Tiefe der Zelle 6 gezeichnet (Original-Atlas-Teil, falls vorhanden -
   im Zuge von M-E2 - sonst mit diesem Beleg in die Freistellungs-Liste fuer den Nutzer), Riegel-Szene Art Z an a30A0/o3 mit Fenster
   x 310..317 y 106..111 (V == 0).

## Hinweise (keine Maengel)
- H1: Kopf von `tests/integration/test_r41_masken36_fern_szenen.cmake` nennt die Plattenkante "z 14802..14867"; Dossier und Ausgabe von
  `unit_r41_masken36_stirn` "z 14834..14904" - Zahl angleichen.
- H2 (Punkt 2): die V-Szene `v2050_00` (8900,0) liegt in der Kamerazone 0 -> 1 @RDT+0x002BC (z -1100..400) - im Spiel ist dort Cut 1 aktiv;
  besser eine natuerliche Cut-0-Stelle (z >= 400). Dort verdeckt der nahe Handlauf die Unterschenkel (Leon 344..505, Elza 329..683 Punkte),
  was zum GEMALTEN Gelaender passt (x ~8800 nach der Kamera, die die Steg-Streifen bei x 9900 trifft); die Kollisionszelle 30 @RDT+0x00A78
  (x 7850..8350) liegt 450..950 westlich davon - der begehbare Streifen x 8350..~8800 liegt im Bild ausserhalb des Gelaenders (Bestand).
- H3: 30A0 Cut 2: Bild und RDT-Kamera (RDT+0xA0) sind an der Trennwand-Ecke gegeneinander versetzt (~12..15 Bildpunkte, auf x = -19975
  ~+500 in z, gleichfoermig); Tiefen, die fuer Cut 2 aus Zellen abgeleitet werden, sind dort entsprechend unsicher.

**Ergebnis: nicht bestanden** - Punkt 1 teilweise (Mangel 1 = Entscheid M-E2 offen, der nach dem NB4-Abschluss kam; Mangel 2 = roter
Ordner 30A0/30A1 C0 ohne Maske), Punkt 2 erfuellt. Die eigenen Ziele der NB4 halten: Mangel 1 der Abnahme 3 ist fuer die Stirnflaeche
widerlegt (selbst gemessen), Mangel 3 ist behoben (Riegel und eigene Abbruch-Logs), die Restliste ist nicht schlechter (bytegleich NB3).
