# Runde 38 Spur W "rolltor20a0" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_rolltor20a0` (Zweig `r38/rolltor20a0`, HEAD 2ff291a9, Basis df08d83a).
Pruefer: Abnahme 1 (glaubt nur, was hier selbst am gebauten Stand gemessen ist). Bericht wird fortlaufend geschrieben.
Werkzeuge/Laeufe dieser Abnahme: Scratchpad `.../scratchpad/abnW1_rolltor/`.

Nutzer-Punkt (AUFTRAG.md Nachtrag 1, woertlich):
> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the
> camera. I cannot see anything... this is not like this in the original....

Maengel der Abnahme 0: M1 (ROOM5060/5061 Cut 4 deckend dunkles Halbviereck statt halbtransparent), M2 (Zensus einseitig).

## 0. Stand und Bau

* `git log master..HEAD`: 71 Commits (W, gemergte A-Commits, Abnahme 0, 17 Commits Nachbesserung 1 1579b758..2ff291a9).
  master hat seit der Basis df08d83a nur `doc(r38)`-Commits.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. `git status` leer; juengste Quelle
  `platform/pc/src/objabe_r38_pc.c` 08:45:03.44, Objekt `objabe_r38_pc.c.obj` 08:45:04.56, `re15_pc.exe` 08:45:04.79 ->
  die exe entspricht HEAD.


## 1. Code-Diff der Nachbesserung 1 (gelesen)

`git diff d6a55686..HEAD -- re15_port`: neu `engine/src/objabe_r38.c`, `include/re15_objabe_r38.h`,
`platform/pc/src/objabe_r38_pc.c`; Haken in `platform/pc/main.c` (Include, `re15_objabe_r38_pc_raum_neu()` am Anfang von
pc_load_room_prop_set, `re15_objabe_r38_pc_textur()` nach jedem Raum-Objekt-TIM-Upload, die drei Dreiecks-Aufrufe der
Prop-Schleife ueber `re15_objabe_r38_pc_dreieck(attr, uv->page, ...)`, Flach-Bedingung `(attr & 0x12) == 0x10` ->
`re15_objabe_r38_flach(attr, typ)`), `platform/pc/src/render_pc.c` (Include; `set_tri_blend` nimmt 0..7; Flush holt fuer
3..7 die STP-Teil-Textur, NULL = Stapel auslassen; `re15_render_textured_tri_lit` nimmt das Eckpunkt-Alpha aus
`s_tri_alpha` statt fest 0xFF; `re15_render_pc_renderer()` nicht mehr nur unter `__ANDROID__`), `objpoly_r38_pc.c`
(Halbviereck ueber denselben ABE-Weg); Riegel `tests/unit/test_r38_rolltor20a0_abe.c`,
`tests/integration/test_r38_rolltor20a0_abe.cmake`, Registrierung in `tests/unit/probes/r38_rolltor20a0.cmake`.

Selbst geprueft:
* Teil-Texturen (`re15_objabe_r38_teilen`) haben dasselbe Layout wie `re15_render_pc_upload_tim_slot` (Breite
  `tim->width`, Hoehe `tim->height x CLUT-Zeilen`, gleiche Texel-Dekodierung `re15_tim_texel_argb`, gleiches
  `SDL_CreateTexture`/`SDL_BLENDMODE_BLEND`) -> die UV-Rechnung des Flush (inv_w/inv_h des Slots) passt.
* Ohne ABE: unveraendert EIN `re15_render_textured_tri_lit` mit der normalen Slot-Textur (Mischmodus 0, Alpha 255).
  ABE-Objekt ohne STP-1-Texel: Durchgang 1 = normale Textur deckend, Durchgang 2 = NULL (ausgelassen) = GPU-Regel.
* Flush-Schleife: das innere `int bl` (Zeile 1188) verdeckt das aeussere `uint8_t bl`; `cur_blend = bl` steht ausserhalb
  des inneren Blocks und nimmt das aeussere (Modus des naechsten Dreiecks) — korrekt.
* Eckpunkt-Alpha: alle Setzer im Baum UND in `r38/integration` 127100ef (`git grep set_tri_alpha`) — main.c ESP-Effekte,
  glas1120_pc.c, re2fx_pc.c, objabe_r38_pc.c — setzen nach Gebrauch 255 zurueck (auch nach dem `break` der ESP-Schleife);
  door_scene_pc.c / hebetisch_cursor_pc.c setzen nur den Mischmodus. Kein Weg hinterlaesst Alpha != 255 fuer Figuren.
* Port-Modelle: `attr` = 0 (Feuerloescher) bzw. 0x0010 (Kartenitems), kein Bit 0x2; ihre Slots bekommen keine Teil-
  Textur (re15_objabe_r38_pc_raum_neu leert alle 64 Slots am Raumanfang) -> unveraendert.
* Typ-3-Teil der Flach-Regel: alle 54 Typ-3-Saetze (Zensus, selbst nachgefahren) tragen 0x001a oder 0x0012, also das
  Flach-Bit 0x10 — die Regel `Typ 3 -> flach` aendert ueber das Flach-Bit hinaus nichts. Geaendert (Licht -> Flach
  0x808080) werden genau die 69 ABE-Saetze mit Bit 0x10 (0x0012 x37, 0x001a x32).

## 2. Eigene Riegel der Spur (selbst gefahren, exe = HEAD)

`ctest --test-dir re15_port/build -R "r38_rolltor20a0" --timeout 1500 --output-on-failure -V` ->
**`100% tests passed, 0 tests failed out of 5`**, `Total Test time (real) = 475.29 sec` (Log `abnW1_rolltor/ctest_spur.log`):
* unit_r38_rolltor20a0 (R1..R7 wie Abnahme 0, u. a. `R1 ... 48 Vierecke wie im Original`, `R2/R3 ... neu 0 Pixel, alt
  67728`), unit_r38_rolltor20a0_windung (`O2: ... 0 raeumliche nicht in Original-Windung`).
* integration_r38_rolltor20a0 271.5 s: a_leon / b_elza / d_leon_karte je 9 Bilder Cut 11 Anteil **0.9420..0.9503**,
  `OBJPOLY Cut 11 obj 1: 450 Bilder, Abweichungen 0`; c_kiste `F1278 room=20A0 p=0 LANDUNG y=-3600 band=2`.
* unit_r38_rolltor20a0_abe: A1 9 Regel-Faelle; A2 10 Original-Saetze (`ROOM5060 obj 4 @0x02D72 Attribut 0x0012: ABE 1,
  ABR 0 bei 2/2 Polygonen = Original` ... `ROOM4090 obj 2 @0x00B36 Attribut 0x000e: ABE 1, ABR 3 bei 152/152`); A3
  STP-Teilung `ROOM11B0 obj 0 ... STP 1 1513, STP 0 31255, falsch 0`; A4 `max. Abweichung ABR0 1, ABR1 0, ABR2 0, ABR3 1`.
* integration_r38_rolltor20a0_abe 203.7 s, je 31 Bilder: 5060c4 (Leon) und 5061c4_elza `(32.2,15.4,26.5)` gegen Original
  `(31.6,15.3,26.1)` Abstand 0.6; 5060c3 `(124.8,47.7,46.9)` gegen `(125.4,46.2,44.0)` 2.9; 20a0c2 `(0.4,0.4,0.3)` 0.0;
  4090c0 `(140.6,138.9,147.2)` gegen `(140.5,139.4,147.9)` 0.7.
* Die Original-Referenzwerte des Riegels habe ich mit einem eigenen Skript (`abnW1_rolltor/tools/mittel.py`, PIL) an den
  PNGs nachgerechnet: `orig_abe_5060_cut4_buf0.png` Dreieck 1749 px (31.6,15.3,26.1); `orig_abe_20a0_cut2_buf0.png`
  7840 px (0.4,0.4,0.3); `orig_abe_4090_cut0_buf0.png` 21280 px (140.5,139.4,147.9); `orig_abe_5060_cut3_buf0.png`
  2000 px (125.4,46.2,44.0) — identisch mit den Konstanten in `test_r38_rolltor20a0_abe.c`.

## 3. RE-Gate (selbst disassembliert, `info/Re1.5/PSX.EXE` md5 b55fdaa5..., `re15_disasm.py dis`)

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| ABE = Attribut & 2: FUN_8002c18c @0x8002c2ac/@0x8002c2b4/@0x8002c2b8 | `lw v0,12(s1)` (Pool+0x0C) / `andi v0,v0,0x2` / `sltu s2,zero,v0` | stimmt |
| Parameter 4 an alle Zeichner @0x8002c304/@0x8002c32c/@0x8002c364/@0x8002c38c | je `addu a3,s2,zero` vor `jal 0x80022f0c` @0x8002c320, `jal 0x80023098` @0x8002c350, `jal 0x800254a0` @0x8002c380, `jal 0x800256b0` @0x8002c3b0; a2 = s1+112 (`addiu s0,s1,112` @0x8002c2fc im Verzoegerungsplatz) | stimmt |
| Typ 3 @0x8002c2bc/@0x8002c2c0/@0x8002c2c8/@0x8002c2cc | `lbu v1,8(s1)` @0x8002c2b0 (Typ = Pool+8), `ori v0,zero,0x3`, `bne v1,v0,0x8002c2d8`, `jal 0x8003e64c`, Verzoegerung `addu a1,s2,zero` | stimmt |
| FUN_8003e64c nur Flach-Zeichner, Farbe obj+0x70, ABE | @0x8003e664 `addiu s1,s3,112`, @0x8003e668 `addu a2,s1,zero`, @0x8003e66c `addu a3,s2,zero`, @0x8003e6ac `jal 0x80022f0c`; @0x8003e6b4/@0x8003e6b8 dasselbe, @0x8003e6dc `jal 0x80023098` | stimmt |
| Flach = Attribut & 0x10 @0x8002c2f4 | `andi v0,v0,0x10` / @0x8002c2f8 `beq v0,zero,0x8002c360` (-> Licht-Zeichner) | stimmt |
| Code = Grundcode \| ABE<<1 | FUN_80022f0c @0x80022f18 `sll a3,a3,1` / @0x80022f1c `ori a3,a3,0x34` / @0x80022f38 `sb a3,3(a2)`; FUN_80023098 @0x800230ac/@0x800230b0 `ori a3,a3,0x3c`/@0x800230d0 `sb a3,3(s0)`; FUN_800254a0 @0x800254ac/@0x800254b0 (0x34)/@0x800254d4; FUN_800256b0 @0x800256bc/@0x800256c0 (0x3c)/@0x800256ec; Wort 1 des Primitivs @0x80023024 `lw v0,0(a2)` / @0x8002302c `sw v0,4(a1)` | stimmt |
| ABR = Texpage \| (Attribut & 0xC) << 3 | FUN_80025940 @0x800259bc `lw v0,0(a0)` / @0x800259c4 `andi v0,v0,0xc` / @0x800259cc `sll t7,v0,19` / @0x80025a08 `lw v0,-4(t1)` / @0x80025a14 `or v0,t7,v0` / @0x80025a18 `sw v0,-8(a3)`; FUN_80025a98 @0x80025b1c/@0x80025b24/@0x80025b2c/@0x80025b68/@0x80025b70/@0x80025b74 (`sw v0,-24(a3)`); a0 = Pool+0x0C aus FUN_8002b898 @0x8002b920 `addiu s0,s0,12` / `jal 0x80025940` @0x8002b92c | stimmt |
| Flach-Farbe 0x808080 (FUN_8002b898) | @0x8002b8fc `lui v1,0x80` / @0x8002b900 `ori v1,v1,0x8080` / @0x8002b91c `sw v1,112(s0)`; Ecken 1/2 des GT3 beim Bind `sw t5,-16(a3)` / `sw t5,-4(a3)` mit t5 = 0x808080 (@0x800259e0/e4) | stimmt |
| psx-spx GPU (Mischformeln, STP je CLUT-Eintrag, 0000h durchsichtig, Klemmung) | `docs/graphicsprocessingunitgpu.md:381`, `:1129-1132`, `:1158-1161`, `:1416-1436` woertlich | stimmt |

Rate-Woerter in den hinzugefuegten Code-Zeilen der Nachbesserung (`git diff d6a55686..HEAD -- re15_port | grep "^+" |
grep -i -E "deferred|tunable|interim|for now|faithful|plausib|TODO|approx|PORT-WAHL|NUTZER-VORGABE|ungefaehr|geschaetzt|vermut"`):
**keine**; kein neuer `getenv`. SDL-Alpha 128/64 sind die SDL-Abbildung der psx-spx-Faktoren 0,5/0,25 (derselbe Weg wie
main.c ESP-Effekte / glas1120_pc.c / re2fx_pc.c), Riegel A4 belegt <= 1 Stufe. Neue Konstanten 0x0002, 0x000C, 0x0010,
Typ 3 tragen je @0x....

Zensus (M2) selbst nachgefahren: `python zensus_beidseitig.py <shared_assets/PSX>` und `python zensus_abe.py ...` ->
Ausgabe **byte-gleich** mit den eingecheckten `zensus_beidseitig.txt` / `zensus_abe.txt` (diff leer). Inhalt: MEHR 3
(ROOM1170 obj 3 Cut 0; ROOM5060/5061 obj 4 Cut 4), WENIGER 22 (18 aus §8 + ROOM2050 obj 4 Cut 11/12/13 und ROOM20A0
obj 6 Cut 3, alle Typ 3), Eltern 4 Saetze gerechnet, pc[5]=0x80 (ROOM4030/4031) gemeldet — das sind genau die Faelle,
die meine Abnahme-0-Skripte (`zensus_mehr.py`, `zensus_typ3.py`) gefunden hatten.

## 4. Nutzer-Punkt am ECHTEN Tuerweg nach N1 (Leon 2090 -> 20A0, Elza 2091 -> 20A1)

Derselbe Weg wie in Abnahme 0 §3/§4 (ROOM2090.RDT @0x009B6 Door_aot_set slot 1 -> ROOM20A0 Cut 13, Band 2, zu Fuss zur
Leiter, Schalter, Ja, Cut 11, Kiste nach Norden). Laeufer `abnW1_rolltor/lauf_w1.sh` (Messkopie
`re15_pc_abnW1_<lauf>.exe`, RE15_WINDOW_SCALE=1), Wegplaene `wp_tuer_20a0.txt` / `wp_tuer_20a1_elza.txt` (Abnahme 0):
* Leon `t1_tuer_leon_n1`: `RE15_PSELECT_AUTO=1 RE15_KEIN_GATOR=1 RE15_DEBUG_JUMP=2090@250
  RE15_PLAYER_POS=6300,-16250,2048,1 RE15_FRAMEDUMP=300-3600/10:fd_ RE15_EXIT_AT=3600#20A0`.
* Elza `t5_tuer_elza_n1`: dasselbe mit `RE15_PSELECT_AUTO_SWITCH=1`, 2091/20A1; debug.log
  `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, wegplan `F46 room=20A1 ... RAUM 20A1 geladen`.

Ablauf (wegplan/objfall, beide = Abnahme 0 Bild fuer Bild): Rolladen `F1263 ... obj1=(7942,-10800,-20510)`; Kiste
Leon `F2282 AUSLOESER wort=0x4206 band=4`, `F2301 LANDUNG y=-3600 band=2`, `F2320 LANDUNG y=-1800 band=1`; Elza
`F2288 AUSLOESER`, `F2307 LANDUNG band=2`, `F2326 LANDUNG band=1`.

| Messung | Abnahme 0 (vor N1) | nach N1 (diese Abnahme) |
|---|---|---|
| Cut 11, Raumhintergrund-Anteil (`test_r38_rolltor20a0 --bild <ppm> ROOM20A.BSS 11 0`, Bild UND Vorbild cam=11; Helfer `tools/bg11.sh`) | Leon 69 Bilder 0.8998..0.9678, Elza 70 Bilder 0.9026..0.9684 | **Leon 69 Bilder 0.8998..0.9678, Elza 70 Bilder 0.9026..0.9684 — Datei fuer Datei identisch** (`diff` leer) |
| Pixelvergleich aller 331 Bilder gegen den Abnahme-0-Lauf (`tools/diff_dumps.py`, max\|d\| > 24) | — | Leon und Elza je **36 Bilder verschieden: nur Cut 1 (8 Bilder, 8042 px = das Wasser im Kanal, jetzt B - F) und Cut 2 (28 Bilder, 73 px = die gruenen Flecken am Kanalende, §8.2)**; Cut 11/12/13/14/15 0 px |
| Bild | `abnW0_rolltor/t1_bogen_cut11.png` | `abnW1_rolltor/t1_bogen_cut11_n1.png`, `t5_bogen_cut11_n1.png` (Plattform, Kiste, Faesser, Regal, Figur; kein Rolladen vor der Kamera) |

v0.8.25 am selben Weg (Abnahme 0): 69/69 Bilder 0.1305 (ganzes Bild Lamellen). **Der Nutzer-Punkt bleibt nach N1 erfuellt,
Leon und Elza, echter Tuerweg.** Vergleichsbild der geaenderten Schnitte: `abnW1_rolltor/tuer_leon_n1_vs_vorN1_cut1_2.png`.

## 5. M1 und die vier Mischarten am Spiel gegen die ORIGINAL-Bildpuffer (eigenes Werkzeug)

Werkzeug `abnW1_rolltor/tools/orig_vgl.py` (PIL, unabhaengig vom Riegel-Werkzeug): Mittel RGB und MAD je Kanal der
Messflaeche gegen das Original-PNG, Anzahl Pixel mit max|d| > 24, Bildbogen Original | Port.
Vorbemerkung: ein Lauf mit `RE15_PLAYER_POS` an der Original-Stelle OHNE `RE15_FORCE_CUT` bleibt im Port in Cut 0
(`b1_5060c4_leon`, `b3_5060c3`, `b4_20a0c2`, `b5_20a0c3`: state.txt 73/73 Bilder `cam=0`) — der Port wechselt den Schnitt
nur ueber die Zonen-Kette vom aktuellen Schnitt aus, genau wie das Original-Werkzeug sie Zone fuer Zone abgeht. Darum
messe ich die Schnitte 2/3/4 zu Fuss (Wegplaene durch die Zonen-Mitten des Original-Logs, Gegner weg, §5.2) und nicht
mit erzwungenem Schnitt.

### 5.1 ROOM4090 Cut 0 (ABR 3, Nebel obj 0-2, Attribut 0x000e) — Start-Schnitt, ohne Erzwingen
Lauf `b6_4090c0` (`RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=4090@250 RE15_PLAYER_POS=-21000,-24600,0,0
RE15_FRAMEDUMP=270-330/2:fd_ RE15_EXIT_AT=332#4090`, cam=0 in 73/73 Bildern), Gegenprobe v0.8.25 `b6a_4090c0_alt`:

| Bild | Mittel RGB (Boden x 20..299, y 160..235, 21280 px) | MAD je Kanal | Pixel max\|d\| > 24 |
|---|---|---|---|
| Original `orig_abe_4090_cut0_buf0.png` | (140.5,139.4,147.9) | — | — |
| nach N1 F270 / F300 / F330 | (140.6,138.9,147.2) | (3.2,3.2,3.5) | **7 von 21280** |
| v0.8.25 F300 | (56.5,55.7,55.1) | (85.6,85.3,94.2) | 19720 von 21280 |

Bild `abnW1_rolltor/vgl_4090c0.png`: Original und N1 zeigen den hellen, durchscheinenden Bodennebel; v0.8.25 dunkle
deckende Schlieren ueber dem ganzen Boden.

### 5.2 M1: ROOM5060 (Leon) und ROOM5061 (Elza) Cut 4 — ZU FUSS durch die Zonen-Kette erreicht
Wegplaene `wp_5060_c4.txt` / `wp_5061_c4.txt` (Gegner weg wie im Original-Abzug, dann `GEHE` durch die Zonen-Mitten
des Original-Logs (-23025,-12425) -> (-25475,-18725) -> (-23375,-23075) -> (-20926,-25850), Halt), Laeufe
`d1_5060c4_leon` (`RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=5060@250 RE15_FRAMEDUMP=250-2400/10:fd_ RE15_EXIT_AT=2400#5060`)
und `d2_5061c4_elza` (dasselbe + `RE15_PSELECT_AUTO_SWITCH=1`, 5061; debug.log `Spieler-Familie PL04`). state.txt:
Schnittfolge 0 -> 1 -> 2 -> 3 -> 4 (der Port schaltet ueber dieselbe Zonen-Kette), cam=4 ab F375, Halt bei (-21018,-25793).

| Messung (Halbviereck (1,2,3) von obj 4, 1749 px; Werkzeug `tools/serie.py` / `tools/orig_vgl.py`) | Ergebnis |
|---|---|
| Leon, alle **203** Bilder mit Bild UND Vorbild cam=4 (F380..F2400) | Mittel **(32.2,15.4,26.5)** in jedem Bild, Original (31.6,15.3,26.1): Abstand **0.6**, MAD (2.1,2.2,2.3), 8 von 1749 px > 24 |
| Elza, alle **203** Bilder cam=4 | dasselbe: (32.2,15.4,26.5), Abstand 0.6, MAD (2.1,2.2,2.3) |
| Vergleich Abnahme 0 | v0.8.25 (47.2,30.5,36.6) = Scheibe fehlte; Stand vor N1 (6.3,0.5,6.6) = deckend dunkles Dreieck (M1) |
| ganzes Bild F2400 gegen das Original | Leon MAD (2.1,1.2,1.6), Elza (3.1,1.4,1.7); Abweichungen nur an der Figur (Pose/Figur verschieden) |

Bilder `abnW1_rolltor/vgl_5060c4_ganz.png` (Original | Leon | Elza), `vgl_5060c4_zufuss.png`. **M1 ist behoben**: das linke
Fenster zeigt wie im Original das Gitter halb abgedunkelt durch die Scheibe, mit Leon und mit Elza, im Spiel erreichbar.

### 5.3 ABR 1 (ROOM5060 Cut 3) und ABR 2 (ROOM20A0 Cut 2 / Cut 3) — zu Fuss
* `d3_5060c3` (Wegplan `wp_5060_c3.txt`, Halt (-23476,-22966), cam=3 in 209 Bildern): Licht-Bodenplatte obj 5
  (Attribut 0x0006, ABR 1), Rechteck x 95..134 y 180..229: Mittel **(124.8,47.7,46.9)** in allen 209 Bildern, Original
  (125.4,46.2,44.0), Abstand **2.9**, MAD 3.8 (v0.8.25 laut Riegel (45.9,45.1,43.9), Abstand 79.5).
* `d4_20a0c2c3` (Wegplan `wp_20a0_c2c3.txt`, Gegner weg, `RE15_FX_LOG=fx.txt`): Cut 2 (Halt (8646,-22942), 10 Bilder):
  Kanal x 160..299 y 180..235 **(0.4,0.4,0.3) = Original**, Abstand 0.0, MAD 0.1. Cut 3 (Halt (18562,-22932), 193 Bilder):
  Kanal x 140..249 y 180..235 **(6.8,5.5,6.0)** gegen (7.1,5.8,6.4), Abstand 0.3, MAD 1.2, 0 von 6160 px > 24.
* Aber: der OBERE Teil des Kanals in Cut 3 (x 140..259, y 145..179 — ausserhalb der Messflaeche des Riegels) weicht ab:
  Mittel (17.3,17.0,15.3) gegen Original (19.0,17.2,17.6), MAD 5.3, **305 px > 24** (Bereich x 130..269, y 140..189) — die
  gruenen Flecken der vom Port gestarteten Effekt-Schleife (§8.2, Hinweis H1). Bild `vgl_20a0c3_zufuss.png`.
* Ganzes Bild gegen das Original (`vgl_20a0_c2_ganz.png`, `vgl_20a0_c3_ganz.png`, max|d| > 24 rot): Cut 2 F300 1576 px,
  Cut 3 F1000 1089 px — ausschliesslich an Leon (andere Pose/Lage) und an den Flecken; Kanal, Waende, Gelaender und das
  Sprenkelmuster des Wassers decken sich. Keine zusaetzlich gezeichnete Wasser-Kachel (vorbestehender Kachel-Verwurf
  FUN_80014368 ohne Bildwirkung in diesen Schnitten).

### 5.4 Nicht-ABE-Darstellung unveraendert (Laeufe der Abnahme 0 mit der exe NACH N1 wiederholt, Bild fuer Bild)
Batch `batch_e.sh`, dieselben Parameter wie die Abnahme-0-Laeufe (exe VOR N1), `tools/diff_dumps.py` mit Toleranz **0**:

| Lauf | Inhalt | Bilder verschieden (tol 0) |
|---|---|---|
| `e_t3_intro1170` gegen `t3_intro1170_neu` | New-Game-Intro ROOM1170 (Hubschrauber, Rotor, Figuren; MEHR-Fall des Zensus) | **0 von 281** |
| `e_t11_10d0_c12` gegen `t11_10d0_cut12_neu` | ROOM10D0 Cut 12 (Marvin, Rolladen) | **0 von 5** |
| `e_t12_1050_c5` gegen `t12_1050_cut5_neu` | ROOM1050 Cut 5 (Rolltor, Licht-Zeichner) | **0 von 5** |
| `e_t9_2050_c11` gegen `t9_2050_cut11_neu` | ROOM2050 Cut 11 (Typ-3-WENIGER-Fall) | **0 von 5** |
| `e_t7_5060_c4` gegen `t7_5060_cut4_neu` | ROOM5060 Cut 4 (ABE, M1) | 5 von 5 — Dreieck jetzt (32.2,15.4,26.5) statt (6.3,0.5,6.6); v0.8.25 `t8` (47.2,30.5,36.6) (`vgl_5060c4_drei.png`) |
| `e_t10_20a0_c3` gegen `t10_20a0_cut3_neu` | ROOM20A0 Cut 3 (ABE-Wasser) | 5 von 5 (Wasser B - F, Flecken) |

Damit ist belegt: das Eckpunkt-Alpha im beleuchteten Weg und der Zwei-Durchgang-Flush aendern an Figuren und deckenden
Objekten **kein einziges Pixel**; N1 wirkt nur auf Objekte mit Attribut-Bit 0x2. Leistung (Hinweis H2): 4090 mit
`RE15_FPS=240`, 2500 Bilder: neu 33.8 / 33.9 s, v0.8.25 34.3 / 34.2 s — am PC kein Unterschied.

## 6. Nebenwirkungen der Nachbesserung in Raeumen OHNE Original-Bild (exe nach N1 gegen v0.8.25)

Batch `abnW1_rolltor/batch_c.sh`: je Raum `RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=<raum>@250 RE15_FORCE_CUT=<cut>
RE15_FRAMEDUMP=270-330/30:fd_ RE15_EXIT_AT=332#<raum> RE15_CUT_SYNC_LOG=cutsync.txt`, Schnitt = der mit der groessten
ABE-Bedeckung laut Zensus; Vergleich F300 neu | v0.8.25 | Differenz (`tools/vergleich.py`, Bilder `c_<raum>_c<cut>.png`).
Alle 24 Laeufe exit 0, state.txt cam = erzwungener Schnitt.

| Raum / Cut | ABE-Objekt | px max\|d\|>24 | gezeichnete Dreiecke neu / v0.8.25 (cutsync `tris=`) | Bildbefund |
|---|---|---|---|---|
| 1040 / 3 | Glas obj 3 (0x0012, ABR 0) | 2648 | 1234 / 1234 | v0.8.25: Fenster als deckend dunkelgruene Flaeche; neu: Scheibe 50 % ueber dem gemalten Hintergrund (Gitter, Gestalt dahinter sichtbar) |
| 11A0 / 4 | Wasser (0x001a, ABR 2) | 2275 | 48 / 36 | v0.8.25 graue deckende Flaeche; neu dunkler Kanal mit den gemalten Lampen-Spiegelungen |
| 11B0 / 12 | Kiste obj 0 (0x0012, STP gemischt) | 17015 | 108 / 83 | Kiste flach 0x808080 (heller, beide Seiten gleich hell); der Schattenteil unter der Kiste jetzt 50 % durchscheinend (Kanaldeckel sichtbar) statt deckend schwarz |
| 11B0 / 6 | dieselbe | 5042 | 118 / 83 | wie Cut 12 |
| 1260 / 12 | Wasser | 0 (>8: 0) | 140 / 88 | keine sichtbare Aenderung |
| 2000 / 15 | Wasser | 523 (>8: 20571) | 462 / 392 | grau deckend -> dunkler Kanal mit hellen Spiegelflecken des Hintergrunds |
| 2050 / 9 | Wasser | 0 (>8: 0) | 1311 / 1521 | keine sichtbare Aenderung |
| 2080 / 6 | Wasser | 2971 (>8: 57549) | 432 / 216 | v0.8.25 deckte die gemalte Vordergrund-Reling mit dunkelgruenem Wasser zu; neu Reling und Becken sichtbar |
| 2090 / 5 | Wasser (Gator-Raum) | 329 (>8: 40143) | 4077 / 4348 | dunkelgruen deckend -> schwarz mit Sprenkeln (wie das Original-Wasser 20A0 Cut 3) |
| 20B0 / 2 | Wasser | 5349 | 270 / 138 | grau deckend -> dunkler Kanal mit Spiegelflecken |
| 2070 / 12 | — | 0 | 0 / 0 | kein Objekt im Bild |
| 5030 / 9 | Typ 3 (0x0012, ABR 0) Monitore | 4863 | 44 / 22 | Monitor-Zeilen 50 % ueber den gemalten Bildschirmen statt deckend grau |

Befund: in keinem Bild Muell, fehlende Objekte oder Loecher; jede Aenderung ist die Anwendung derselben, an 5 Schnitten
gegen das Original gemessenen Regel (ABE/ABR/STP, Flach-Farbe). Die Dreieckszahl waechst durch den zweiten Durchgang
(4090 Cut 0: **2130 statt 1232** Dreiecke in der Queue, Kapazitaet `TEXTRI_QUEUE_MAX` 8192) — Hinweis H2.

## 7. M2 — Zensus in beide Richtungen

Behoben und nachgefahren (§3 letzter Absatz): `zensus_beidseitig.py` / `zensus_abe.py` reproduzieren die eingecheckten
Ausgaben byte-gleich; MEHR 3 / WENIGER 22 inkl. Typ 3 / Eltern 4 gerechnet / pc[5]=0x80 gemeldet / ABE sichtbar 364.
Jeder MEHR-Fall ist am Spiel gemessen: ROOM1170 Cut 0 (Intro) **0 von 281 Bildern** gegen den Stand vor N1 (§5.4) und
<= 96 px gegen v0.8.25 (Abnahme 0 §5.2); ROOM5060/5061 Cut 4 = Original (§5.2). Die Typ-3-WENIGER-Faelle: 2050 Cut 11
0 px gegen vor N1 (§5.4), 20A0 Cut 3 = Wasser (§5.3). Dossier §8 traegt den Verweis auf §10.7; die falsche Aussage
"= wie das Original" fuer 5060 Cut 4 ist dort korrigiert. **M2 ist behoben.**

## 8. Weitere Pruefungen

### 8.1 Vertrag (VERTRAG.md)
* Pfade (`git diff master...HEAD --name-only`): keine `release/`, `platform/android/`, `shared_assets/PSX/`, keine
  Test-CMakeLists (Registrierung ueber `tests/unit/probes/r38_rolltor20a0.cmake`).
* Bank-9-Bits / Nachrichten-Ids / Items / Ereignisse: keine (`grep game_flag_set|MSG_ID|k_port|bank9|Se_on|se_play` in
  den hinzugefuegten Zeilen: 0).
* Gemeinsame Dateien: `render_pc.c` N1 24 geaenderte Zeilen an 5 Stellen (Include, Wertebereich set_tri_blend, Zugriff
  auf den Renderer, Flush-Haken, Eckpunkt-Alpha), jede mit "Runde 38 Spur W"; `main.c` N1 30 Zeilen (Include, zwei
  Lade-Haken, Flach-Bedingung, Aufrufe ueber re15_objabe_r38_pc_dreieck). Logik in eigenen Dateien. Groesser als
  "1-5 Zeilen" je Datei, aber Haken-Charakter (Hinweis H3, wie H2 der Abnahme 0).
* Lesender Merge-Test gegen den aktuellen `r38/integration` 127100ef (`git merge-tree --write-tree`): Konflikt NUR in
  `platform/pc/main.c`, Include-Zeilen (E/I/T/D gegen W) — beide behalten. render_pc.c merget ohne Konflikt. Im
  Integrationsbaum und in `r38/wasser` 28cb1b9c (Spur N, noch nicht integriert; merget konfliktfrei) gibt es keinen
  weiteren Setzer von `re15_render_pc_set_tri_alpha` (git grep) — der beleuchtete Weg liest das Alpha also gefahrlos.
* Riegel je Punkt: integration_r38_rolltor20a0 (Nutzer-Punkt, 4 Laeufe), integration_r38_rolltor20a0_abe (M1, 5 Laeufe
  gegen Original-Werte; trennscharf: deckend 25.3 / v0.8.25 15.6 / 79.5 / 85 liegen weit ausserhalb der Toleranzen 4..8),
  unit_r38_rolltor20a0(_windung|_abe).

### 8.2 Die gruenen Flecken in ROOM20A0/20A1 (Hinweis H1, selbst nachgeprueft)
* Der Port startet beim Betreten von 20A0/20A1 die im Original ungetriggerte sub02 (`scd_room_setup.c`
  `if (rid == 0x20A0 || rid == 0x20A1) scd_event_fire(2);`, Kartierung `analysis/nutzer_batch_2026-08-29/
  schlafender-content.md:194` "Echte deaktivierte Ambient-Effektschleife ... Visuelle Identitaet UNSICHER — nicht gerendert
  geprueft", Nutzer-Auftrag 2026-08-30).
* `RE15_FX_LOG` (Lauf `d4_20a0c2c3`): **17 aktive Effekte id=6** (Slot 36), Weltlagen x 20842..21800, y -360..-720 (unter
  der Wasserflaeche obj 6 y -1800), Bildschirm sx 156..249, sy 152..175 = genau die Flecken; sie stehen von F300 bis F2400
  unveraendert auf ihren letzten Anim-Bildern (frame 14/19/24, Zaehler 0) — also stehende Flecken, keine Tropfen.
* Original: ESP-Pool @0x800A73B8 in Cut 2 dreimal gelesen = 0 aktive Plaetze (`W_belege/orig_abe_20a0_esp_log.txt`).
* Sichtbar seit N1, weil das Wasser jetzt wie im Original B - F rechnet statt deckend grau: Cut 2 **73 px** (auf dem
  Tuerweg zur Leiter, Leon UND Elza, §4), Cut 3 **305 px** > 24 gegen das Original (§5.3).
* Der Bau-Agent fuehrt das korrekt unter "Fuer den Nutzer" und OFFEN (Ursache = Port-Aktivierung, nicht der
  Zeichenweg); dass die Sprites auf dem letzten Anim-Bild stehen bleiben, steht dort nicht.

### 8.3 Dinge, die ich geprueft und in Ordnung befunden habe
* Typ-3-Kachel-Verwurf FUN_8003e64c/FUN_80014368 (Punkt-im-Viereck der Kachelmitte gegen den Bereich @0x800AC790,
  Decompilat FUN_80014368.c) bleibt vorbestehend unportiert — dokumentiert (OFFEN); in den gemessenen Schnitten 20A0
  Cut 2/3 ohne Bildwirkung (MAD 0.1/1.2 in den Kanal-Flaechen).
* 11B0/11B1 Kiste (einziges Objekt mit STP-0- und STP-1-Texeln): Bild `c_11B0_c12.png`/`c_11B0_c6.png` — Koerper flach
  (Regel @0x8002c2f4, Farbe @0x8002b8fc-91c), der Schattenteil unter der Kiste halbtransparent; ohne Original-Bild
  (Debug-Sprung haelt dort die Kamera, Dossier §10.6). Mechanik belegt, Bild nicht gegengeprueft (Hinweis H4).

## 9. Testgruppen der beruehrten Module (selbst gefahren, seriell, exe = HEAD)

Derselbe 81er-Ausdruck wie der Bau-Agent (Dossier §10.10: Prop-Schleife = alle Bild-/Lade-Riegel mit Raum- und
Port-Objekten, Hebetisch/Panels/Cursor, Loescher, Schluessel, Kisten, eigene + A-Riegel; render_pc.c-Mischwege = re2fx,
Fenster-Glas 1120, ESP-Eintritt, fx_region_cull, Tueren), Ausdruck in `abnW1_rolltor/regex81.txt`, `ctest -N` -> 81 Tests.
Nachgeprueft, dass das die Pixel-Riegel in ABE-Raeumen abdeckt: von allen Integrations-Riegeln, die ABE-Raeume betreten,
werten nur `integration_r36_kartenitems_bild` (2040/2090) und `integration_r37_kiste` (2010) Framedumps aus — beide im
Ausdruck; die uebrigen (Titel-/Inventar-/Karten-Bilder, Logs) sehen kein Raum-Objekt. In `r38/integration` haben
`test_r38_werkstatt11b0`, `_montage`, `_elzatoene` keine Framedumps in ABE-Raeumen.
`ctest --test-dir re15_port/build -R "<regex81>" --timeout 1500 --output-on-failure` ->
**`100% tests passed, 0 tests failed out of 81`**, `Total Test time (real) = 3346.18 sec` (Log `abnW1_rolltor/ctest_gruppen.log`;
laengste: integration_r35_cut1150 509.8 s, r36_kartenitems_bild 482.6 s, r37_feuerloescher 402.2 s, r38_rolltor20a0
284.8 s, r37_kistenregel 268.8 s, r38_rolltor20a0_abe 197.5 s, r38_cursor 184.5 s). Kein Flattern, nichts nachgefahren.
Nicht gefahren: die volle Suite (Sache der Integration) und integration_r36_dachkiste (prueft Kisten-Logs; N1 aendert nur
die Darstellung — die Kiste 20A0/20A1 faellt am echten Tuerweg unveraendert, §4).

## 10. Urteil je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| "in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the camera. I cannot see anything... this is not like this in the original" (+ Elza 20A1) | **erfuellt** | Echter Tuerweg 2090 -> 20A0 (Leon) und 2091 -> 20A1 (Elza) mit der exe NACH N1: Cut 11 mit offenem Rolladen 69/69 bzw. 70/70 Bilder Raumhintergrund 0.8998..0.9684 — Datei fuer Datei identisch mit dem in Abnahme 0 abgenommenen Stand, 0 px Unterschied in Cut 11/12; v0.8.25 am selben Weg 0.1305 (ganzes Bild Lamellen); Kiste faellt weiter (Band 4 -> 2 -> 1); Riegel integration_r38_rolltor20a0 gruen (§2, §4) |

Maengel der Abnahme 0: **M1 behoben** (5060/5061 Cut 4 zu Fuss erreicht, Leon und Elza je 203 Bilder Abstand 0.6 zum
Original-Bild statt 25.3; dieselbe Regel an ABR 1/2/3 gegen Original-Bilder 5060 Cut 3, 20A0 Cut 2/3, 4090 Cut 0
bestaetigt, §5), **M2 behoben** (Zensus beidseitig, reproduzierbar, jeder MEHR-Fall am Spiel gemessen, §7).

## 11. Gates

* **RE-Gate: haelt.** Alle neuen Konstanten (0x0002, 0x000C/<<3, 0x0010, Typ 3, Codes 0x34/0x3C | ABE<<1, Flach-Farbe
  0x808080) selbst an 25+ Stellen in `info/Re1.5/PSX.EXE` disassembliert (§3), Mischformeln/STP woertlich psx-spx; keine
  Rate-Woerter, kein Env-Schalter; Commit 2ff291a9 traegt die Adressen. Der Fix erklaert den Befund M1 (Code 0x3E + ABR 0
  + STP 1 aller Texel -> 50/50; gemessen = Original).
* **Original-Belege der Laufzeit:** Logs `orig_abe_*_log.txt` passen zu den RDT-Saetzen (Lage/Attribut von 5060 Pool 2-5,
  20A0 Pool 4/6 byte-gleich mit `Obj_model_set` @0x02D2E/@0x02D50/@0x02D72/@0x02D94/@0x01998/@0x019DC); Referenzwerte an den
  PNGs nachgerechnet; der Port trifft sie in Laeufen ZU FUSS (nicht nur mit erzwungenem Schnitt).
* **Vertrag: haelt** (§8.1; Hinweis H3 zur Haken-Groesse).
* **Riegel: vorhanden, messend, trennscharf, gruen** (§2: 5/5; §9: 81er-Gruppe).
* **Nebenwirkungen: geprueft** — Nicht-ABE-Darstellung bildgleich (0 px in 296 Bildern, §5.4); ABE-Raeume ohne
  Original-Bild zeigen die Regel ohne Fehlbilder (§6); einzige nicht-originale Sichtbarkeit = die vom Port gestartete
  Effekt-Schleife in 20A0/20A1 (§8.2), vom Bau-Agenten unter "Fuer den Nutzer" gefuehrt.

## 12. Maengel

Keine.

## 13. Hinweise (nicht blockierend, nachpruefbar)

* **H1 ROOM20A0/20A1 gruene Flecken** (§8.2): 17 ESP-Effekte id=6 der vom Port gestarteten sub02 (`scd_room_setup.c`,
  "schlafender Content", Nutzer-Auftrag 2026-08-30) stehen auf ihren letzten Anim-Bildern (frame 14/19/24) unter dem
  Wasser und scheinen seit N1 durch das jetzt originale B - F-Wasser: Cut 2 73 px (Tuerweg zur Leiter, Leon und Elza),
  Cut 3 305 px gegen das Original. Original: 0 aktive ESP-Plaetze. Im Dossier gefuehrt ("Fuer den Nutzer", OFFEN), aber
  ohne die Angabe, dass die Sprites stehen bleiben. Die Messflaeche des Riegels 20a0 Cut 3 (y 180..235) liegt unterhalb der
  Flecken. Empfehlung an den Orchestrator: NACHLAUF-Eintrag (Aktivierung von 20A0 sub02 bzw. ESP-Ende von Effekt 6
  pruefen) — nicht Gegenstand dieser Spur, der Zeichenweg der Spur ist dort am Original-Bild bestaetigt.
* **H2 Last:** jedes ABE-Dreieck = zwei Queue-Eintraege mit wechselndem Mischmodus (Stapel bricht je Dreieck; in 4090 ist
  der deckende Durchgang leer, weil alle Texel STP 1 tragen). 4090 Cut 0: 2130 statt 1232 Dreiecke (Kapazitaet 8192).
  Am PC kein messbarer Unterschied (`RE15_FPS=240`, 2500 Bilder: 33.8/33.9 s gegen 34.3/34.2 s); Android nicht gemessen.
* **H3 Haken-Groesse:** render_pc.c (24 Zeilen an 5 Stellen) und main.c (30 Zeilen) groesser als "1-5 Zeilen", aber reine
  Haken mit "Runde 38 Spur W", Logik in eigenen Dateien. `gcc -Wshadow`: das innere `int bl` im Flush (render_pc.c:1188)
  verdeckt das aeussere — korrekt, aber leicht misszuverstehen.
* **H4 ohne Original-Bild:** 1040/1041 Glas, 11A0/11A1, 1260/1261, Stage-2-Wasser ausser 20A0, 5030/5031, 11B0/11B1 Kiste
  (Koerper jetzt flach 0x808080 = heller, Schatten darunter 50 % durchscheinend; 11B0 ist der Raum der Marvin-Szene der
  Spur C). Mechanik belegt (§3), Bilder §6 — der Bau-Agent fuehrt sie unter "Fuer den Nutzer".
* **H5 Integration:** main.c-Include-Konflikt (beide behalten); Android braucht einen frischen Configure (neue .c-Dateien
  objabe_r38.c, objabe_r38_pc.c, objpoly_r38*.c, nclip_r38.c); nach dem Merge von Spur N (Wasser-Spritzer) das Bild der
  Spritzer ueber dem jetzt subtraktiven Wasser ansehen.

## 14. Ergebnis

**Bestanden.** Der Nutzer-Punkt ist am echten Tuerweg mit Leon und Elza erfuellt und durch die Nachbesserung nicht
veraendert (Cut 11 bildgleich). M1 und M2 der Abnahme 0 sind behoben und am Spiel gegen Original-Bilder gemessen, zu Fuss
erreicht, Leon und Elza. RE-Gate, Vertrag, Riegel und Nebenwirkungen halten; die eine neue, nicht-originale Sichtbarkeit
(H1) stammt aus Port-Inhalt und ist dem Nutzer im Dossier angekuendigt.

Werkzeuge/Bilder dieser Abnahme (Scratchpad `.../scratchpad/abnW1_rolltor/`): `lauf_w1.sh`, `batch_ab.sh`, `batch_c.sh`,
`batch_d.sh`, `batch_e.sh`, `wp_*.txt`, `tools/{mittel,orig_vgl,serie,auswahl,bogen,paar,vergleich,diff_dumps}.py`,
`tools/bg11.sh`, `regex81.txt`, Logs `ctest_spur.log`, `ctest_gruppen.log`; Bilder `t1_bogen_cut11_n1.png`,
`t5_bogen_cut11_n1.png`, `tuer_leon_n1_vs_vorN1_cut1_2.png`, `vgl_5060c4_ganz.png`, `vgl_5060c4_zufuss.png`,
`vgl_5060c4_drei.png`, `vgl_4090c0.png`, `vgl_20a0c3_zufuss.png`, `vgl_20a0_c2_ganz.png`, `vgl_20a0_c3_ganz.png`,
`c_<raum>_c<cut>.png` (12 Raeume).
