# Stufe 3 Bahnsteig-Form: Bewegung, Angriffe, Einbau (laufendes Dossier)

## Integration: Absturz Software-Renderer K5c

Befund (Lauf A2, `integration_a2/pistole_absturz_k5c.txt`): `integration_r41_birkin_pistole` stuerzte mit dem Asset K5c (BAHNSTEIG_FORM.EMD
sha256 1ab56076..., .TIM 7187ff09..., TIM 512x256 8 bpp, 16 CLUT-Zeilen -> Port-Textur 512x4096) bei F1029 ab, Leon und Elza:
`AUSNAHME 0xC0000005` in `SDL_BlitTriangle_Slow+1394`. Der Riegel laeuft mit `RE15_SOFTWARE_RENDER=1` (SDLs CPU-Rasterer); mit K4 (1408x1024)
gruen, im beschleunigten Renderer (direct3d, opengles2) kein Absturz. Belege dieses Abschnitts: `integration_a2/sw_absturz_k5c/`.

### SW-1. Ursache (gemessen): 32-Bit-Ueberlauf in SDLs CPU-Rasterer, nicht das Asset

(a) **Asset-UVs liegen alle in der Textur.** Eigener Leser (`sw_absturz_k5c/uv_pruef.py`, Formel des Ports: Texel-x = u + (page & 0xF) * 128
wie `main.c` page_off, v vor dem Zeilenversatz, CLUT-Index = VRAM-Zeile - 480 wie `render_pc.c` v_offset) ueber alle 32 Meshes (auch
Kiefer/Zaehne), 3828 Flaechen: 0 Ecken ausserhalb, Texel-x 44..436 (Breite 512), v 0..254, Seiten 0x80..0x83, CLUT-Indizes 0..15
(`a_uv_pruef_k5c.txt`). Die Clips aendern keine UV (kein UV-Versatz fuer Typ 0x30, `eye_du` nur fuer 0x36).

(b) **SDL 2.28.5 rechnet den Anteil der dritten Ecke in int.** `src/render/software/SDL_triangle.c`:
- Z. 618-619 `s2_x_area.x = s2->x * area;` / `s2_x_area.y = s2->y * area;` (Typ `SDL_Point` = int, Z. 447),
- Z. 171-172 `srcy = (int)(((Sint64)w0 * s2s0_y + (Sint64)w1 * s2s1_y + s2_x_area.y) / area)` (nur die Gewichte in 64 Bit),
- Z. 771 `src = (info->src + (srcy * info->src_pitch) + (srcx * srcbpp))` ohne Grenzpruefung.
area = |cross(d0,d1,d2)| (Z. 472, 571) in Festkomma FP_BITS 1 (Z. 36): dst = (int)(xy * scale) << 1 (`SDL_render_sw.c:576-578`),
src = (int)(uv * texture->h) (`SDL_render_sw.c:573-574`). Mit 16 CLUT-Zeilen liegt s2.y bis 4095 statt bis 1023 (K4): die Flaechengrenze aus
`re15_gpu_grenze_r41.h` (4 * 1023 * 511 = 2090892) haelt das Produkt nur fuer Texel-Lagen bis 1027 unter 2^31.

(c) **Messhaken** `RE15_SWTRI_LOG=<datei>` (render_pc.c `re15_swtri_messen`, nur Env, kein Spielverhalten) rechnet SDLs Ganzzahlen je an
`SDL_RenderGeometry` gegebenem Dreieck nach. Lauf leon (Riegel-Umgebung, bis F1100), exe vor dem Fix (`b_swtri_vor_fix_k5c16.txt`):
- 18 Dreiecke ueber 2^31-1, alle Slot 11 (Form, 512x4096), F968..F1029, alle mit Ecken an der GTE-Saettigung (dst -2046 = -1023 px) und
  Texel-y 3044..3664 (CLUT-Zeilen 11..14). Groesstes F1016: area 976560 x s2.y 3606 = 3521475360.
- F1029 (Absturzbild): area 859884 x s2.y 3634 = 3124818456 -> gewickelt -1170148840, srcy rund 4995 Zeilen vor der Textur;
  `AUSNAHME 0xC0000005 ... +0x239b06` (= SDL_BlitTriangle_Slow+1394 wie im Befund), state.log endet bei F1029 wie im Befund.
  Die frueheren 17 Ueberlaeufe lasen fremden, zufaellig gemappten Speicher (kein Absturz, falsche Texel).
- Andere Slots im selben Lauf: 512x4096 nur Slot 11; Slot 50/51 (globale Effektseiten DATA/TEX.TIM, 256x4096) hoechstens 289849560,
  Slot 0 (Spieler 384x768) 6196800.
- Gegenprobe Hoehe (`b_swtri_gegenprobe_k5c8.txt`): dieselbe exe, BAHNSTEIG_FORM.TIM auf CLUT-Zeilen 0..7 gekuerzt (Port 512x2048, gleiche
  UVs, Zeilen 8..15 fallen auf Zeile 0): 0 Ueberlaeufe, groesstes Produkt 943560296, kein Absturz, Lauf bis F1100.
- K4 (3163f6045, Port 1408x1024), gleiche Messung (`b_swtri_k4.txt`): 0 Ueberlaeufe, groesstes Produkt 1187923968 = 55 % der Grenze -
  darum war der Riegel mit K4 gruen.

### SW-2. Fix (render_pc.c, nur CPU-Rasterer)

- `re15_sw_geometrie` ersetzt im Flush der texturierten Dreiecke `SDL_RenderGeometry`, aber nur wenn `s_cpu_rasterer` (RE15_SOFTWARE_RENDER
  oder Rueckfall ohne GPU). Jedes Dreieck, dessen Produkt max(|s2.x * area|, |s2.y * area|) nach SDLs Ganzzahlen (`re15_sw_s2_produkt`,
  Formeln oben mit Zeilenbeleg) ueber INT_MAX laege, wird an der laengsten Bildkante halbiert (Lage, UV, Farbe der Mitte gemittelt), bis
  jede Haelfte darunter liegt (Abbruch nach 12 Halbierungen -> Teil verworfen, Log). Alle anderen Dreiecke gehen unveraendert und in
  derselben Reihenfolge an SDL. Die ersten 8 Teilungen je Prozess stehen als `[sw-teilung]` in debug.log.
- Der beschleunigte Renderer bekommt die Dreiecke unveraendert (SDL_RenderGeometry wie bisher).
- Die Flaechengrenze aus Spur Q bleibt (PSX-Hardwareverhalten); der neue Schutz deckt auch Skalierung > 1 (RE15_WINDOW_SCALE: area waechst
  mit scale^2) und die 4096 hohen Effektseiten 50/51 ab.
- Upload (`re15_render_pc_upload_tim_slot`, ohne Verhaltensaenderung): liegt die gestapelte Textur ueber `max_texture_width/height` aus
  `SDL_GetRendererInfo`, steht `[tim] slot N: Textur WxH (n CLUT-Zeilen) ueber der Grenze des Renderers ...` im Log (SDL lehnt dann ab,
  `SDL_render.c:1266-1268`); schlaegt `SDL_CreateTexture` fehl, steht `SDL_GetError()` im Log, der Slot gilt als nicht geladen
  (`loaded = 0`, w/h 0; aktiver Slot: `s_tim_texture = NULL` wie bisher ueber update_active_slot_globals).
- Asset unveraendert (K5c, Nutzerentscheid E30). Commits 4dde57452 (Fix) und b0623e4d6 (Riegel unit_r41_uv_textur).

Nach dem Fix (`c_nach_fix.txt`): Lauf leon bis F1100 ohne AUSNAHME, `[sw-teilung] F968 ... F1016 Slot 11 Textur 512x4096: area 976560 x s2 =
3521475360 > 2^31-1 -> geteilt`; state.log bis F1028 byte-gleich mit dem Lauf vor dem Fix (Spiel deterministisch, der Fix aendert nur Pixel).

### SW-3. Bildgleich im beschleunigten Renderer (`d_bildgleich_d3d.txt`)

`re15_port/tools/form_spiel.sh` (eigene exe-Kopie, echtes Fenster, Renderer direct3d, Fenster 640x480, Asset K5c ohne Umleitung) mit
`RE15_FRAMEDUMP=300-3000/50:fd_` je vor dem Fix (exe aus 72c80256f) und nach dem Fix, Skript `sw_absturz_k5c/bild_lauf.sh`:

| Lage | Bild | Pixel-Diff vor/nach |
|---|---|---|
| 5080 Schnitt 1, -18564,-18386 Gier 80, Clip 0 Bild 11 | RE15_FRAMEDUMP F300/F350/F400 | 0 / 0 / 0 |
| 5080 Schnitt 1 | gdigrab Fenster | 0 |
| 5090 Schnitt 11, 941,-23776 Gier 3874, Clip 12 Bild 0 | RE15_FRAMEDUMP F300/F350/F400 | 0 / 0 / 0 |
| 5090 Schnitt 11 | gdigrab Fenster | 48419 (x 70..639, y 225..479) |

Die 5090-gdigrab-Abweichung ist der Aufnahmezeitpunkt des Feuers: zwei Laeufe mit DERSELBEN exe nach dem Fix weichen dort ebenso ab
(41500 Pixel, x 80..639, y 226..479), die Framedumps desselben Bildes sind in allen drei Laeufen gleich (0). Keine Asset-UV berichtigt.

### SW-4. Tests

- Neuer Riegel `unit_r41_uv_textur` (`tests/unit/test_r41_uv_textur.c`): fuer RE2/BAHNSTEIG_FORM, PSX/EMD/EM30_HANGAR, PSX/EMD/EM29_ROT,
  RE2/ANNETTE_4D, RE2/ANNETTE_4D_BLUT und alle 55 RE2-Banken aus RE2/CDEMD0.EMS mit EMD + TIM: jede UV-Ecke in der Textur (Formel des
  Zeichners), Port-Texturhoehe <= 4096. Alle gruen (BAHNSTEIG_FORM 512x4096, Texel-x 44..436, v 0..254). Gegenprobe eingebaut (Kopie mit
  Mesh 0 Dreieck 0 u0 255 Seite 0xF -> 3 Ecken ausserhalb, rot erkannt) und als Aufruf `test_r41_uv_textur gegenprobe` (Exit 1,
  "GEGENPROBE ROT (erwartet)").
- `bash re15_port/tools/local_build.sh build`: `=== LOCAL-BUILD-OK (build)` (Endstand; die letzte Aenderung danach war nur ein Kommentar in
  re15_gpu_grenze_r41.h, Disassembly der exe vor/nach identisch).
- `integration_r41_birkin_pistole` (Leon und Elza, RE15_SOFTWARE_RENDER=1 wie im Riegel): `1/1 Test #1419: integration_r41_birkin_pistole ...   Passed  477.88 sec`;
  beide Laeufe bis F6500, je 8 `[sw-teilung]`-Zeilen (Log-Grenze 8), keine AUSNAHME.
- `ctest -R "form|bahnsteig|birkin_form|r41_form|integration_r41_g5_tod|integration_r41_g5_westen|annette_g5" -j 2 --timeout 1800`:
  `100% tests passed, 0 tests failed out of 19` (kein roter Fenster-Riegel).
- `ctest -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)" -j 6`:
  `100% tests passed, 0 tests failed out of 1068` (mit unit_r41_uv_textur).

### SW-5. Offen

- Die Teilung nimmt fuer die betroffenen Dreiecke (alle mit Ecken an der GTE-Saettigung, fast ganz ausserhalb des Bildes) eine Mitte, deren
  Bildlage SDL auf ganze Pixel abschneidet: an der Teilungskante kann ein Randpixel anders fallen als bei einem (nicht ueberlaufenden)
  ganzen Dreieck. Betrifft nur den CPU-Rasterer (Tests, Rueckfall ohne GPU), nicht den Renderer des Spielers.
- Nicht gemessen: ob SDLs Gewichte selbst (w0/w1/w2, int in TRIANGLE_BEGIN_LOOP, SDL_triangle.c:156-163) bei grossen Fensterskalen
  ueberlaufen koennen (area waechst mit scale^2). Im Riegel (RE15_WINDOW_SCALE 1) gilt area <= 4 * 1023 * 511 = 2090892 (Grenze Spur Q).

## Integration 1000 HP (Lauf A2)

Auftrag: Integration Runde 41 Welle 2, Lauf A2, Befund 1 der Gegenpruefung A (integration_r41_form_wagenweg, _elza, _zurueck_leon,
_zurueck_elza rot mit der GR-HP 1000). Integrationsbaum `.claude/worktrees/r41_integration` (Zweig r41/integration, Kopf bf5001400, Asset
K5c seit 275a57906). Die Form-HP 1000 sind Nutzerentscheid F2 (G4-Ctor `addiu v0,zero,1000` @0x80100678) und bleiben. Belege
`integration_a2/` (Messlaeufe, Gegenproben, Einordnung, Werkzeug). Alle Laeufe: eigene exe-Kopie, Renderer des Nutzers, Fenster
320x240, RE15_FRAMEDUMP jedes Bild, Eingabe Tastenskript w8 des Riegels.

### A2-1. (a) Sichtlinien-Tor in der Suedwestecke: ORIGINAL

Mechanismus (selbst disassembliert, `integration_a2/sichtlinie_tor.txt`, PSX.EXE des Auslieferungsstands):
- Treffer-Pfad FUN_80011f50: `jal 0x8001b9b4` @0x80012168 mit a0 = Ziel + 0x34, `bnez v0,0x80012540` @0x80012170 -> Schuss ohne Treffer.
- FUN_8001b9b4: Delta Ziel - Spieler (+0x34/+0x38/+0x3c), Schleife ueber die Regionen 3..0 (`ori s0,zero,3` @0x8001ba08,
  `bnez v0,0x8001ba14` @0x8001ba34), je Region `jal 0x8003dcc4` @0x8001ba20 mit a2 = 0xf00 (`ori a2,zero,0xf00` @0x8001ba1c) und
  a3 = 0x300 (`ori a3,zero,0x300` @0x8001ba24, Verzoegerungsplatz).
- FUN_8003dcc4: je SCA-Eintrag Band `lbu v1,0x82(v1)` @0x8003de60 gegen `(w << 16) >> 28` (@0x8003de64/68, `bne` @0x8003de6c), Maske
  `and v1,t5,a0` @0x8003de7c gegen 0x300 (`bne` @0x8003de94), dann die zwei Straddle-Tests je Diagonale (OuterProduct0); Treffer
  `ori v0,zero,1` @0x8003e09c, `sw fp,0x1b4(v1)` @0x8003e0a4. Keine Abfrage des Gegnertyps.
- Zelle 126: ROOM5090.RDT SCA @0x860, Eintrag 12 Byte, Datei-Offset 0xe60 `5d16 d007 57fe c095 06ff 0003`: x -425..5300,
  z -27200..-25200, Typ 6, Wort +10 = 0x0300 -> Band 0, Nibble 3. Dieselbe Zelle in allen fuenf Regionen (Test-Integrator,
  `scratchpad/integ_w2b/mess_los/sca_zelle126.txt`).

Der Port bildet das ab (schuss_r36.c `re15_schuss_r36_sichtlinie_blockiert` -> treffer_r39.c `re15_treffer_r39_los_blockiert`);
Messung des Test-Integrators (treffer.log): 267 Sperren F3270-F5490, alle zelle=126, nachgerechnet: von (1950,-26080), (1501,-26259),
(-12,-26433) kreuzt die Linie zur Form Zelle 126, aus dem Wagenband z -24749 nicht. **Kein Port-Fehler, kein Fix.** Folge: ein stehender
Pistolenschuetze, den die Angriffe der Form ueber z -25200 nach Sueden schieben, kann die 1000-HP-Form nicht mehr treffen; das
Riegel-Szenario (150 HP, 10 Treffer) war durch die GR-HP widerlegt, nicht durch einen Regress.

### A2-2. (b) Riegel auf 1000 HP (test_r41_form_wagenweg.cmake, probes/r41_form_kamera.cmake)

- Waffe: leon/elza/zurueck_* Raketenwerfer (`RE15_GIVE=18:200 RE15_EQUIP=18`; Item 0x12 "ROCKET LAUNCHER", inventory_common.c:418;
  wie integration_r41_g5_tod). Schaden an der Riesenform 301: RE2-Leon-EXE @0x800A5D24 Bytes `2d b5 d4 12` = w0 0x12D4B52D,
  & 0x3FF = 301 (Zeiger @0x800A6B58 = 0x800A6A88 + 0x34*4 -> 0x800A5BE4, + 16*20 = 0x800A5D24; selbst nachgelesen in
  info/re2leon/SLUS_007.48 und PSX.EXE). Tod nach HP / 301 + 1 = 4 Raketen (hp < 0). warten_* behalten die Pistole (Lauf k5090).
  Kein Messhaken mehr (RE15_R39_MESS_SPIELER_HP entfaellt; echte Spieler-HP in allen Teilen, gemessen HP 100 -> 95).
- Eingabe: Mit sofortigem Feuer (w8 + 2 s) stirbt die Form nach ihrem Auftritt am Westende (x 3925, Raketen F1256-F1349), mit
  Zurueckweichen bei x ~9300. Dann zeigt die Szene Cut 12 -> 11 bzw. 15 -> 12 -> 11, und die Gegenprobe "Szene fest Cut 12" war in
  allen vier Teilen GRUEN (`integration_a2/gegenprobe/vorserie_sofortfeuer/`, Zwischenstand des Riegels mit 2 s; Laeufe rk1/zl5): der Riegel haette M-A nicht mehr gefangen. Neu:
  leon/elza w8 + 5 s, zurueck_* w8 + 2 s + D2.0 + 3 s, dann Feuer. Der Ansturm der Form (ss1 4) trifft den Spieler (F1322 bzw. F1336,
  HP 100 -> 95), danach betaeubt jede Rakete sie (Zustand 2) bis zum Tod in der Lage der Abnahme-2-M-A-Laeufe: Form x 16664 / Leon
  x 15104 (v0_w: 17290 / 14998), Form x 20136 / Leon x 19310 (v0_v: 20124 / 17815). Probiert: 6 s (w6l) wie 5 s, zurueck sofort (zl5).
- Pruefung 3: >= HP / 301 + 1 Treffer und jeder Abzug genau 301. Pruefung 4: Fenster ab dem Eintritt in Wagen C (der Angriff faellt vor
  die erste Rakete). Pruefung 5 unveraendert (Abzug 1204 >= 1000 - 301).
- Laufende = Framedump-Ende (ein Wert): F2000 fuer die Raketenteile (Taumelende gemessen F1705 / F1779), F1931 fuer warten_*;
  "Taumeln erreicht das Laufende" ist rot. Die Fenster der Pruefungen 6 und 7 kommen aus dem state.log.
- Pruefung 7 getrennt in AUFTRITT (Bedingung der Wache (a): Form 1/0, Clip 10, Clip-Bild < 60 = Cut 11) und KAMPF (danach bis vor die
  Szene), Schranken = gemessener Stand (Begruendung A2-3):

| Teil | Auftritt > 600 / max | Kampf > 600 / max | Szene 6a min / 6b max / 6c max / 6d max | Taumeln |
|---|---|---|---|---|
| leon | 33 / 628 | 0 / 553 | 751 / 6683 / 639 / 600 | F1462-F1705, 244 Bilder, Cut 13-12-11 |
| elza | 33 / 628 | 0 / 531 | 1025 / 5333 / 523 / 600 | F1462-F1705, 244 Bilder |
| zurueck_leon | 33 / 628 | 4 / 623 | 2108 / 6001 / 185 / 629 | F1495-F1779, 285 Bilder |
| zurueck_elza | 33 / 628 | 3 / 617 | 2108 / 1428 / 158 / 605 | F1495-F1779, 285 Bilder |
| warten_leon | 33 / 628 | 5 / 698 | - | keine Szene bis F1931 |
| warten_elza | 33 / 628 | 4 / 653 | - | keine Szene bis F1931 |

Schranken: Auftritt 33 / 650 (650 = PORT-WAHL NB4-10); Kampf je Teil = Messwert; Szene das Pruefmass der Abnahme 2 M-A
(768 / 25600 / 500 / 600), gehoben nur, wo der Stand es ueberschreitet: leon 6a 751 + 6c 639, elza 6c 523, zurueck_leon 6d 629,
zurueck_elza 6d 605. Zwei ctest-Laeufe mit denselben Zahlen.

Gegenproben mit dem fertigen Riegel (`integration_a2/gegenprobe/gp_gegen*.txt`; exe aus demselben Baum, nur birkin_formen.c geaendert,
danach zurueckgesetzt und neu gebaut):
- A "Szene fest Cut 12" (re15_bf_rueckstoss_start und Taumel-Kamera `bf_kamera(0x0C)` wie 2e3713c74): leon ROT (23 Verstoesse,
  F1463-F1468 933-998 Promille fast schwarz), elza ROT (21, bis 998), zurueck_leon ROT (19, Form 0 sichtbar F1496-F1499, 775),
  zurueck_elza ROT (20).
- B "Wache nach dem Bildmass" (Commit 128d52e2e rueckwaerts = Runde 1 b9ab865af): warten_leon ROT (50 Kampfbilder > 600, max 728),
  warten_elza ROT (47, max 721), zurueck_leon ROT (63, max 804), zurueck_elza ROT (62, max 803), leon ROT (3, max 617), elza gruen.

### A2-3. (c) Kampfkamera mit K5c: Einordnung (keine Aenderung an der Wache)

Asset: K4 (BAHNSTEIG_FORM.EMD/.TIM aus 3163f6045, sha256 16eb6ea8... / ec280802...) per RE15_RE2_ASSET_ROOT gegen K5c, gleiche exe:
in den Raketenlaeufen 0 Bilder mit anderem Spielzustand oder Schnitt (F1150-F1349 bzw. F1406), nur die Helligkeit aendert sich
(`integration_a2/asset_k5c_gegen_k4.txt`): leon Kampfkamera K4 0 Bilder > 600 / max 596, K5c 33 / 628; zurueck_leon K4 2 / 621, K5c
49 / 778; warten_leon K4 9 / 621 (= alte Schranke), K5c 38 / 698. Die Gegenpruefung A hatte noch K4 gemessen (ihr Lauf = pi4).

Je Bild nachgerechnet (`integration_a2/kampfkamera_einordnung.txt`, klass.py: Zonenregel re15_bf_schnitt_aus_lage und Achsenregel der
Wache mit k_gang_kam Cut 12 Auge x 18526 RID @0x1e0, Cut 13 Auge x 11050 RID @0x200, BF_NAH_ACHSE 4500, Mitte = x + cos(rot) * 1700;
RVD-Bytes selbst gelesen: Anker Cut 12 @0x4a4, Cut 13 @0x4e0, Streifen 13 -> 12 @0x4f4 x 12249..13873, 12 -> 13 @0x4cc x 14000..15400):
1. Auftritt F1172-F1211, Cut 11 (Wache (a), Kamera von Bild 2, PORT-WAHL Nachbesserung 1 M4): Form frontal bei x 1015-1787, 13000-15600
   Bildpunkte; Leon nicht im Bild. K5c 602-628, K4 559-596 Promille. Ursache: das dunklere Asset. Kein Kamerafehler.
2. zurueck F1324-F1327 (Raketenlauf) und F1347-F1390 (sofortiges Feuer), Cut 13: Zonenkamera der Spielerlage (Leon x 17975, Anker
   @0x4e0), die Form laeuft im Ansturm am Auge vorbei bzw. steht an ihm (Form vor dem Auge -820 .. 3300) zwischen Kamera und Spieler,
   36-57 % des Bildes. Achsenregel: Spieler hinter der Form ja, aber Gegenkamera Cut 12 nur 551 vor ihrem Auge (< 1500) -> kein Wechsel,
   wie in 2e3713c74 (M-B-Rest NB4-10: dort hat der Raum keinen Schnitt im Pruefmass). ORIGINAL-Zonenkamera + PORT-WAHL Achsenregel.
3. warten F1917-F1921, Cut 13: Spieler vor der Form (Spieler 1112-2854 vor dem Auge, Form 4388), Form 42000-45000 Bildpunkte gross
   hinter ihm; Achsenregel greift nicht (Spieler nicht hinter der Form). Zonenkamera (Spieler im Streifen bzw. noch im Cut 13).
4. Langer Pistolenkampf (Gegenpruefung, K4, pi4): F1922 Cut 13 = Haltezeit der Wache (30 Bilder nach dem Wechsel bei F1911; Leon war im
   Streifen @0x4f4 bei x 12610, F1910 kurz Cut 12): der Treffer F1916-F1920 stoesst Leon auf x 10774 HINTER das Auge von Cut 13 (-276),
   die Haltezeit haelt Cut 13 weiter. F1951-F2004 Cut 12: nach der Haltezeit die Zonenkamera der Spielerlage (x 10774 < 14000), die Form
   (Mitte 14204, 4322 vor dem Auge) zwischen Kamera und Leon; Gegenkamera Cut 13 hat Leon hinter dem Auge (-276 < 1500) -> kein Wechsel.
   K5c (pi5): dieselbe Klasse, Cut 12 F1892-F2298 136 Bilder bis 753.

Ergebnis: Kein Abweichen der Wache von der Achsenregel 2e3713c74 (Funktion bytegleich, gelobter Stand, STUFE3 Abnahme 4), nicht
umgebaut. Gemeldet (offen): die Haltezeit von 30 Bildern prueft nicht nach, ob der Spieler noch vor dem Auge der gehaltenen Kamera
steht (pi4 F1920-F1950: Leon hinter dem Auge von Cut 13).

### A2-4. Neu gesehen, nicht geaendert (offen)

- Szenenkamera bf_schnitt_waehlen (PORT-WAHL Nachbesserung 4) in der w-Lage mit 1000 HP: Cut 12 F1621-F1635 (leon) bzw. F1633-F1634
  (elza) zeigt die Form klein am Wagenende (x 4142 -> 3022, 2081-2810 bedeckte Bildpunkte) zu 510-639 Promille hinter Leon/Elza
  (x 15104), bevor bei x <= 3000 Cut 11 kommt. Mit K4 dasselbe (637). Das Pruefmass 6c (500) ist dort ueberschritten; der Riegel haelt
  den Stand (639 / 523), damit ein Rueckschritt auffaellt.
- 6d in der v-Lage: zurueck_leon F1577-F1580 616-629, F1712 603; zurueck_elza 603-605 - nur mit K5c.
- 6d Rand: leon/elza in der w-Lage max genau 600 (Schranke 600, kein Abstand); mit 6 s Warten (w6l) 601-608.

### A2-5. Tests

- `ctest -R "form|bahnsteig|birkin_form|r41_form|integration_r41_birkin_pistole|integration_r41_g5_tod|integration_r41_g5_westen" -j 2
  --timeout 1800` (Kopf 2312b9e0a, exe = Bau des Kopfs; parallel lief ein fremder ctest im Baum r41_gatorfix):
  **95% tests passed, 1 tests failed out of 19** (Total Test time (real) = 848.10 sec), Log `integration_a2/ctest_satzA.txt`. Gruen: die
  sechs Wagenweg-Teile, integration_r41_g5_tod, integration_r41_g5_westen, die Form-/Bahnsteig-Riegel. Rot: integration_r41_birkin_pistole.
- Wagenweg allein, zweimal: "100% tests passed, 0 tests failed out of 6" (`integration_a2/ctest_wagenweg_lauf1.txt` / `_lauf2.txt`,
  gleiche Zahlen je Teil).
- **integration_r41_birkin_pistole: ABSTURZ mit K5c** (`integration_a2/pistole_absturz_k5c.txt`), NICHT durch diesen Lauf verursacht
  (nur Testdateien geaendert): Leon und Elza, zweimal (ctest und Einzellauf), state.log endet bei F1029, debug.log
  `[flow] AUSNAHME 0xC0000005 ... Adresse +0x238b06` = SDL_BlitTriangle_Slow + 1394 <- SDL_SW_BlitTriangle <- SW_RunCommandQueue <-
  SDL_RenderPresent (Riegel mit RE15_SOFTWARE_RENDER=1; Form x 7777, Zustand 1/20, Clip 8, Cut 13, Spieler x 17755). Dieselbe exe mit
  dem K4-Asset (RE15_RE2_ASSET_ROOT): kein Absturz, Riegel gruen (Rueckstoss F2657, G5 tot F5283). Der A2-Merger hatte nur Unit-Filter
  gefahren. Offen fuer Spur FORM / Renderer (Lesezugriff ausserhalb der Textur im Software-Dreieck mit dem K5c-Modell).

## Nachbesserung 4 (Rueckstoss-Kamera)

Auftrag: Abnahme 2 (`STUFE3_abnahme_2.md`) M-A (blockierend), M-B (mittel), M-D (Doku). Zweig r41/form_nb4 ab 2e3713c74, Worktree
`.claude/worktrees/r41_form_nb4`. Belege `nb4/` (Leisten `vorher_*.png` / `nachher_*.png`, Laeufe `nb4/laeufe/<lauf>/` mit kam.log,
deck.log, dunkel.txt, pruef.txt, env.txt, exe_sha.txt; Eichung `nb4/eichung_*.txt`; Gegenprobe `nb4/gegenprobe/`; Werkzeug `nb4/werkzeug/`;
RDT-Auszug `nb4/rdt_5090_kameras_zonen.txt`). Alle Bilder: eigene exe-Kopie, Renderer des Nutzers (kein AUTOSHOT/SOFTWARE_RENDER),
RE15_FRAMEDUMP-Serie, Fenster 640x480, Eingabe = Tastenskript w8 der Abnahme (Standardweg) bzw. w8 + D2.0 (2 s zurueck).

Commits: 7ebda1d64 (NUTZER_ENTSCHEIDE E23/E29), c81a3a931 (Messschiene RE15_SLOTDECKUNG), 4a22c3dbc (Schnittwahl), 63b3b02a0 (Riegel),
b9ab865af (Dossier). Runde 2 (Abnahme 3 N1/N2/E1, NB4-10/NB4-11): 128d52e2e (Wache zurueck auf die Achsenregel), 0c53ef647 (Riegel
Kampfkamera), Dossier + Belege im Folgecommit.

### NB4-1. Reproduktion (exe 2e3713c74, Laeufe v0_*)

| Lauf | Taumeln (Clip 13, HP 1) | Schnitte | Bilder > 60 % fast schwarz (Luma < 24) | Bild |
|---|---|---|---|---|
| v0_w_leon | F1486-F1748, Form bei x 17290, Leon x 14998 | 12 -> 11 | 9 von 132 gemessenen (F1488-F1504), max 100,0 % | vorher_w_leon.png |
| v0_w_elza | F1486-F1748 | 12 -> 11 | 9 (F1488-F1504), max 99,7 % | vorher_w_elza.png |
| v0_v_leon | F1544-F1840, Form x 20124, Leon x 17815 | 12 -> 11 | 6 (F1612-F1636), max 65,2 %; F1546-F1771 Leons Ruecken/Kopf gross, Form dahinter | vorher_v_leon.png |
| v0_v_elza | F1544-F1842, Elza x 17665 | 12 -> 11 | 0, max 52,0 %; Elzas Kopf/Ruecken gross | vorher_v_elza.png |

Wie Abnahme 2 M-A. Mit der Messschiene (unten) und birkin_formen.c von 2e3713c74 gezaehlt: zurueck_leon Form 0 sichtbare Bildpunkte in
Cut 12, Leon bis 56690 Bildpunkte (74 % des Bildes), Form bis 1000 Promille verdeckt (NB4-6 Gegenprobe).

### NB4-2. Kameras und Zonen des Raums (ROOM5090 = ROOM5091 in RID/RVD, selbst gelesen: `nb4/rdt_5090_kameras_zonen.txt`)

RID ab 0x60 (Kopf +0x24), 32 Byte je Cut (flag, fov, pos xyz, ziel xyz); H = fov >> 7 (re15_camera_build_view = FUN_80053ca4). Wagen C:

| Cut | RID @ | Auge | Ziel | H | Blick |
|---|---|---|---|---|---|
| 11 | 0x1c0 | (7626,-3256,-25360) | (-1504,-986,-21970) | 208 | -x (Wagenende West) |
| 12 | 0x1e0 | (18526,-2656,-24660) | (1496,1514,-20370) | 208 | -x |
| 13 | 0x200 | (11050,-3110,-24812) | (15974,-1894,-23306) | 231 | +x |
| 14 | 0x220 | (23870,-7450,-29130) | (22450,-1442,-23356) | 188 | +z, Auge ausserhalb des Wagens |
| 15 | 0x240 | (46,-1236,-24302) | (5190,-2246,-20090) | 208 | +x+z (sub04 Cut_chg 0x0f @0x130E, Kampfstart) |

RVD ab 0x260 (Kopf +0x28), 20 Byte je Zone (be, Boden, von, nach, 4 Ecken x/z); die erste Zone jeder Gruppe ist der Such-Anker
(FUN_80014230 / FUN_80014324, rdt_common.c), die uebrigen sind Wechselstreifen:

| Zone @ | von -> nach | x | Bedeutung |
|---|---|---|---|
| 0x47c / 0x4a4 / 0x4e0 / 0x544 | 11/12/13/15 -> 0 | -5600..28100, z -29700..-16975 | Anker Cut 11/12/13/15 |
| 0x51c | 14 -> 0 | 17000..28050 | Anker Cut 14 |
| 0x490 | 11 -> 12 | 2800..4200 | |
| 0x4b8 | 12 -> 11 | 1203..2703 | |
| 0x4cc | 12 -> 13 | 14000..15400 | |
| 0x4f4 | 13 -> 12 | 12249..13873 | |
| 0x508 | 13 -> 14 | 20250..21750 | |
| 0x530 | 14 -> 13 | 18502..20252 | |

Original-Kameraregel = "welche Zone welchen Schnitt zeigt": x < 2750 Cut 11, < 14000 Cut 12, < 20250 Cut 13, sonst Cut 14
(re15_bf_schnitt_aus_lage, Riegel unit_birkin_szenen test_schnitt_aus_lage gegen die RVD-Bytes). Eine Original-Routine, die einen Schnitt
nach der Lage eines GEGNERS waehlt, gibt es fuer diese Szene nicht (die Rueckstoss-Szene ist Gestaltung, Nutzer 2026-10-30); der Raum
kennt nur die Zonenregel auf der Spielerlage und den Cut_chg des Skripts. Die Nachbesserung wendet deshalb die Zonenregel auf die FORM an
und prueft jedes Bild, keine eigene Kamera.

### NB4-3. Behebung (birkin_formen.c, Commit 4a22c3dbc)

`bf_schnitt_waehlen` statt fester Schnitte, fuer die Szene (Motiv Form) und die Nahsicht-Wache (Motiv Spieler):
1. Kameras des Wagens: Anker enthaelt das Motiv. Steht das Auge ausserhalb des z-Bereichs der Wechselstreifen (Wagen C z -28225..-18700),
   zeigt die Kamera nur ihren Anzeigebereich (x-Spanne ihrer Streifen, zum Wagenende offen): Cut 14 nur fuer x >= 18502. Gemessen (Lauf
   n1_v F1624-F1656): Form bei x 12044-14764 in Cut 14 nicht zu sehen (Hintergrund davor, 73,7 % schwarz); Leon bei x 17714 ebenso (n3_a
   F1125-F1179).
2. Bildmass je Kamera (`bf_bild_mass`): Kugeln r 700 um alle 27 Knochen der Formpose (re15_skel_compute_pose wie re15_enemy_part_point),
   Spieler 5 Kugeln r 500 y 0..-3000 (gatorszene_r41.c spieler_punkte, dort an der exe gemessen); Projektion wie der Zeichner (160/120,
   H). Masse: Formrahmen, Formknochen im Bild, Spielerrahmen, Verdeckung (Rahmenschnitt, der Naehere deckt), "nah" = Kugel naeher als 1000
   an der Bildebene (gezeigter Schnitt: erst an der Bildebene, Haltegrenze gegen Flattern, Laeufe n3: Cut 11/15 schalteten je Bild).
3. Szene: zuerst die Original-Zonenkamera der FORMLAGE, dann der aktuelle Schnitt, dann die Zonenkamera der Spielerlage, dann die gute
   Kamera mit dem groessten Formbild; 30 Bilder nach einem Schnitt geht der aktuelle vor; gibt es keine gute: Zonenkamera der Formlage.
   Cut 11 ab x <= 3000 (BF_RS_TAUMEL_KAM_X, Nachbesserung 1) und die Sturz-/Pausen-Kamera bleiben unveraendert.
4. Wache (in Runde 2 zurueckgenommen, NB4-10): aktueller Schnitt, Zonenkamera der Spielerlage, gute Kamera mit dem groessten Spielerbild; keine gute: Zonenkamera der
   Spielerlage (das ist der Zonen-Schnitt, die Wache greift dann nicht ein); Haltezeit 30 Bilder wie Nachbesserung 1.
5. Messschiene `RE15_BF_KAMLOG=<datei>`: je Bild alle Masse aller Wagenkameras (kein Spielverhalten).

Konstanten: BF_BM_FORM_MAX 900 / BF_BM_PL_MAX 490 / BF_BM_DECK_MAX 650 Promille Rahmen = Pruefmass der Abnahme 2 (60 % Form, ein Drittel
Spieler) umgerechnet mit dem kleinsten gemessenen Verhaeltnis Rahmen/Bildpunkte (NB4-4); BF_BM_FORM_R 700, BF_BM_NAH 1000,
BF_BM_INNEN_MIN 500, BF_RS_KAM_HALT 30 sind PORT-WAHL/Gestaltung mit Beleg im Kommentar. Keine RE1.5-Verhaltenskonstante neu.

### NB4-4. Eichung Kugelmodell gegen gezeichnete Bildpunkte (Messschiene RE15_SLOTDECKUNG, Commit c81a3a931)

`RE15_SLOTDECKUNG=<datei>` (render_pc.c): die gezeichneten Dreiecke in Zeichenfolge samt Vordergrund-Masken in einen eigenen
Kennungspuffer, je TIM-Slot sichtbare/bedeckte Bildpunkte. Spieler = Slot 0, Form = Slot 12 (Leon) / 11 (Elza).

| Mass | Modell (Promille Rahmen) -> gezeichnet (Promille Bild) | kleinstes Verhaeltnis | Quelle |
|---|---|---|---|
| Formflaeche | 485 -> 237, 549 -> 333, 592 -> 240, 468 -> 277 | 1,65 | eichung_n2_w_leon.txt, eichung_n2_v_leon.txt |
| Spielerflaeche | 201 -> 62-69, 169 -> 72, 665 -> 447 (nah), 500 -> 258 | 1,49 | eichung_n3_a_leon.txt |
| Verdeckung der Form | 318 -> 99, 463 -> 311, 500 -> 355 | 1,41 | eichung_n2_w_leon.txt |

Ein Spielermodell mit 3 Kugeln bis y -1500 unterschaetzte die Naehe (130 -> 188, Lauf n4_a; eichung_n4_v_leon_spielermodell3.txt: Cut 12
Spieler 711 vor dem Auge Modell 65, gezeichnet 400-500 Promille) und ist verworfen; zusaetzlich gilt "Spielerkugel nah" als Ausschluss.

### NB4-5. Messwerte nachher (exe sha256 b4cc39be..., Laeufe n5_*; Bilder nachher_*.png)

| Lauf | Schnitte im Taumeln | Form min sichtbar | Spieler max | Form max verdeckt | max fast schwarz | Bilder > 60 % |
|---|---|---|---|---|---|---|
| n5_w_leon F1486-F1748 | 13 -> 12 (ab F1545, Form x 13702) -> 11 (x <= 3000) | 1247 | 5866 (7,6 %) | 405 | 53,4 % | 0 |
| n5_w_elza | 13 -> 12 -> 11 | 1596 | 4147 | 238 | 52,4 % | 0 |
| n5_v_leon F1544-F1840 | 13 -> 15 (ab F1662, Form x 11724) -> 11 (F1768) | 2796 | 2161 | 166 | 54,6 % | 0 |
| n5_v_elza F1544-F1842 | 13 -> 15 -> 11 | 2723 | 1626 | 113 | 53,9 % | 0 |

Keine Wechsel hin und zurueck (kam.log je Lauf: genau 2 bzw. 3 Schnitte). Im 2-s-Lauf ist die Form in Cut 13 kurz vor dem Wechsel auf
Cut 15 gross (F1640-F1660, Form 1500-2400 vor dem Auge, Bild nachher_v_leon.png Kacheln F1640/F1652), aber nicht schwarz (max 54,6 %)
und vor dem Spieler. In Cut 15 steht die Form rechts im Bild, ganz zu sehen; Cut 15 ist die Kamera, die der Raum selbst fuer die Form im
Wagen benutzt (sub04 Cut_chg 0x0f @0x130E).

### NB4-6. M-B (Nahsicht-Wache nach dem Wurf, Lauf n6_a_leon, Bild nachher_a_leon_wache.png)

Stand Runde 1; die Wache nach dem Bildmass ist in Runde 2 zurueckgenommen, M-B-Rest siehe NB4-10.


Ablauf wie a5090_leon der Abnahme (Debug-Sprung Wagen B, ohne Feuer). Die Wache haelt nach dem Wurf Cut 12 (F742-F1017: Spieler 17-19 %
des Bildes, Form hinter ihm) und schaltet bei F1018 auf Cut 13 zurueck. **Rest offen:** F1018-F1180 (Leon x 17070-17714, Form x
11883-14505 laeuft auf ihn zu) zeigt Cut 13 die Form mit 28-62 % des Bildes, 50-71 % fast schwarz, Leon zeitweise bis 960 Promille
verdeckt. Fuer diese Lage hat der Raum keinen passenden Schnitt (kam.log je Bild): Cut 13 Form 650-3450 vor dem Auge (nah), Cut 12 Leon
800-1300 vor dem Auge (Spielerkugel nah; gezeichnet 26-48 % des Bildes, n3_a F1029-F1093), Cut 15 Form zwischen Auge und Leon (Leon
verdeckt), Cut 14 Leon hinter dem Hintergrund (n3_a F1125-F1179), Cut 11 Leon hinter dem Auge. Nach Auftrag gilt dann die
Original-Zonenkamera der Spielerlage = Cut 13 (x 14000..20250). Gegenueber Abnahme 2 (Cut 13 F1051-F1174) ist dieser Rest NICHT kleiner geworden (F1018-F1180); geaendert ist nur, dass die Wache
nach dem Bildmass schon ab F742 (Abnahme 2: F845) auf Cut 12 schaltet. **M-B ist damit NICHT geloest**; der Rest
braucht eine Entscheidung (Cut 12 mit grossem Spielerruecken oder Cut 13 mit grosser Form), siehe NB4-9 Offen.

### NB4-7. Tests

- Riegel (probes/r41_form_kamera.cmake, test_r41_form_wagenweg.cmake): integration_r41_form_wagenweg, _elza, _zurueck_leon,
  _zurueck_elza; je Taumelbild 6a Form >= 768 sichtbare Bildpunkte, 6b Spieler <= 25600, 6c Form <= 500 Promille verdeckt, 6d <= 600
  Promille fast schwarz (probe_r41_form_kamera_bild). Renderer des Nutzers (der Software-Renderer dunkelt Cut 11 um 40-50 Promille ab:
  zurueck_elza 599 statt 544).
- Gegenprobe (nb4/gegenprobe/): exe 2e3713c74 -> rot "kein deck.log"; birkin_formen.c von 2e3713c74 + Messschiene -> leon und elza 6d
  rot (F1487-F1502: 776-1000 Promille), zurueck_leon 231 und zurueck_elza 94 Verstoesse (Form 0 sichtbar, Leon bis 56690, Form bis 1000
  Promille verdeckt).
- Summenzeilen: NB4-9.

### NB4-8. M-D

(a) erledigt: NUTZER_ENTSCHEIDE.md E23 (2026-11-03, "passt erst einmal", Pose nicht drehen) und E29 (2026-11-07, "K4 bleibt fuer v0.8.30
im Spiel, der G4-Ansatz danach", Wortlaut "Das koennte schon passen mit der riesen Form ..."), Commit 7ebda1d64. (b) und (c) (Kommentar
"drei Segmente ... K3" in enemy_ai_common.c, EXE-Angabe der Spieler-Ctor-Adressen) nicht Teil dieses Auftrags, offen.

### NB4-9. Testlage und Offen (re15_port/build, Kopf 63b3b02a0, ctest aus Git-Bash, PATH msys64 zuerst, SDL_ASSERT=always_ignore)

- `ctest -R "form|bahnsteig|birkin_form|r41_form|integration_r41_birkin_pistole|integration_r41_g5_tod|integration_r41_g5_westen" -j 4`:
  **100% tests passed, 0 tests failed out of 17** (Total Test time (real) = 427.44 sec), Log `nb4/ctest_form_birkin_g5.txt`; darin die vier
  Wagenweg-Teile (130-133 s), integration_r41_birkin_pistole 243,9 s, integration_r41_g5_tod 264,5 s, integration_r41_g5_westen 285,5 s,
  unit_birkin_bahnsteig, unit_birkin_formen, unit_r41_form_emd, integration_r34_plattform_takt/_esp_eintritt. Kein roter Riegel.
- Zusaetzlich `ctest -R "^unit_(birkin|r41_form|bb_|g5|n2_griff|r41_birkin)" -j 6`: 100% tests passed, 0 tests failed out of 13.
- Offen (Stand Runde 1, Runde 2 siehe NB4-11): (1) M-B-Rest F1018-F1180 (NB4-6): fuer Leon 800-1300 vor dem Auge von Cut 12 mit der Form vor Cut 13 hat der Raum keinen Schnitt
  innerhalb des Pruefmasses; Entscheidung noetig. (2) M-C (Gebruell unter Dauerfeuer) und M-D (b)/(c) nicht Teil dieses Auftrags.
  (3) Die Messschiene RE15_SLOTDECKUNG zaehlt Schatten nicht und Masken als Atlas-Alpha; der Linux-Lauf des Riegels steht aus (Pixelgrenzen
  sind Windows-gemessen, Memory reai-v2-pixelgrenze-rasterer; die Schranken haben Abstand: Form min 1247 gegen 768, Spieler max 5866 gegen
  25600, verdeckt max 405 gegen 500, schwarz max 550 gegen 600).

### NB4-10. Runde 2 (Abnahme 3 N1/N2/E1: Kampfkamera vor der Szene)

Abnahme 3 (`STUFE3_abnahme_3.md`): M-A behoben, aber die Nahsicht-Wache nach dem Bildmass (NB4-3 Punkt 4) machte die Kampfkamera
VOR der Szene schlechter als Stand 2e3713c74, bei gleichem Spiel je Bild: N1 (blockierend) k-Weg (w8 + 24 s warten) 38 statt 7 Bilder
ueber 60 % fast schwarz, Cut 12 mit der Form 1660-2160 vor dem Auge, Kurzschnitte 4 und 7 Bilder (Rueckfall auf die Zonenabbildung
x 13960 -> Cut 12, Zurueckgeben ohne Haltezeit); N2 v-Weg 28 Bilder dunkler Aussenblick Cut 14; E1 e-Weg Spielerruecken 37,9 %; M-B-Fenster
groesser als in Abnahme 2 (a_leon 92 statt 62 Bilder).

**Behebung (Commit 128d52e2e):** `bf_nahsicht_wache` (b) ist wieder die Achsenregel von 2e3713c74 (Cut 12 Auge x 18526 RID @0x1e0,
Cut 13 Auge x 11050 RID @0x200, BF_NAH_ACHSE 4500, Haltezeit 30; Funktion bytegleich bis auf einen Kommentar). Das Bildmass
(`bf_schnitt_waehlen`) bleibt nur fuer die Rueckstoss-Szene; Kampfzweig und `bf_bild_kampf_gut` sind entfernt. Keine neue Konstante.
Begruendung: Abnahme 3 hat beide Staende mit gleicher Eingabe gemessen; die Achsenregel ist im Kampf in allen vier Wegen gleich gut oder
besser (k 7 statt 38, v 0 statt 28, e 0 statt 6, a 62 statt 92), die Szenenwahl in allen acht Szenenlaeufen ohne Verstoss.

**Gemessen** (exe aa370951 = Kopf 128d52e2e, eigene Kopie je Lauf, Renderer des Nutzers, RE15_FRAMEDUMP jedes Bild 640x480, Eingaben =
`abn3/env/*.env` bytegleich mit Abnahme 3, Werkzeug `abn3/werkzeug/` mass.py / vergleich.py / blatt.py, Laufskript
`nb4/werkzeug/r2_lauf.sh`; alle Zahlen in `nb4/r2_auswertung.txt`):

| Lauf | Fenster | Runde 1 (b9b3d574, Abnahme 3) | nachher | Stand 2e3713c74 (Abnahme 3) |
|---|---|---|---|---|
| k_leon (24 s warten) | F1150-F1931 | 38 Bilder > 60 %, max 70,4 %, 36 in Cut 12 | **7**, max 61,8 %, Cut 13 F1332-F1931 | 7, max 61,8 % |
| k_elza | F1150-F1931 | 38, max 69,7 % | **1**, max 61,5 % | - |
| v_leon / v_elza (2 s zurueck), Kampf | F1150-F1543 | 28 / 28 (Cut 14 F1340-F1367) | **0 / 0**, max 58,9 / 59,0 % | 0 |
| e_leon / e_elza (1 s zurueck), Kampf | F1150-F1623 | 6 / 0, Leon 29124 BP = 37,9 % | **0 / 0**, Leon max 15319 BP = 19,9 % | 0 |
| w_leon / w_elza, Kampf | F1150-F1485 | 0 / 0 | 0 / 0 | - |
| a_leon (M-B, Wagen B, steht) | F700-F1200 | 92 (Cut 13 ab F1018) | **62** (Cut 13 ab F1048: F1049-F1108, F1124-F1125) | 62 |
| a_elza | F700-F1200 | 83 (Cut 13 ab F1012) | **53** (Cut 13 ab F1042) | - |

Spiel und Kamera je Bild gleich der Vorher-exe f4deb1a3 (vergleich.py): k_leon 782/782, a_leon 501/501, v_leon F1150-F1543 394/394,
e_leon F1150-F1623 474/474. Rueckstoss-Szene je Bild gleich Runde 1 (Spiel und Kamera ab Szenenbeginn: w/v/e/u Leon und Elza
565/557/477/450 Bilder, 0 Unterschiede), damit unveraendert: 0 Verstoesse in allen acht Szenenlaeufen, Form min 1162 BP sichtbar,
Spieler max 6055 BP (7,9 %), Form max 405 Promille verdeckt, max 54,6 % fast schwarz; Schnitte w 13 -> 12 -> 11, v 13 -> 15 -> 11,
e 13 -> 12 -> 11, u 12 -> 11.

Leisten (`nb4/`, selbst angesehen): `r2_kampf_k_<figur>_runde1.png` / `_nachher.png` (F1716-F1860: Runde 1 Cut 12 mit der schwarzen
Formmasse und Kurzschnitten, nachher durchgehend Cut 13, Leon/Elza in der Mitte sichtbar, die Form rechts von hinten),
`r2_kampf_v_<figur>_*` (F1320-F1390: Runde 1 Aussenblick Cut 14, nachher Cut 13), `r2_kampf_e_<figur>_*` (F1310-F1360),
`r2_szene_w_<figur>_nachher.png`, `r2_szene_v_<figur>_nachher.png` (Szene mit der Kopf-exe, wie `nachher_*.png` aus NB4-5),
`r2_mb_a_<figur>_nachher.png` (M-B-Rest). `nachher_a_leon_wache.png` (NB4-6) zeigt die zurueckgenommene Wache aus Runde 1.

**M-B-Rest (dokumentiert, PORT-WAHL des Orchestrators, kein Nutzerentscheid):** a_leon F1049-F1125 Cut 13 mit der Form gross vor Leon
(bis 71 % fast schwarz). Abnahme 3 (Abschnitt 3.3) hat bestaetigt, dass der Raum dort keinen Schnitt im Pruefmass hat (Cut 12: Leon
1308-812 vor dem Auge, gezeichnet 26-48 % des Bildes; Cut 14 hinter dem Hintergrund; Cut 11/15 Leon hinter dem Auge bzw. verdeckt).
Nach dem Auftrag gilt die Original-Zonenkamera Cut 13 (RVD-Anker @0x4e0, Zonenregel x 14000..20250). Das Fenster ist wieder das von
Abnahme 2 (dort F1051-F1174 gezaehlt; gleiche exe-Logik wie 2e3713c74, gleiche 62 Bilder), nicht groesser.

**Riegel (Commit 0c53ef647):** test_r41_form_wagenweg.cmake Pruefung 7 "Kampfkamera": je Bild F1150 bis vor die Szene bzw. bis zum
Laufende Anteil fast schwarzer Bildpunkte (probe_r41_form_kamera_bild, Framedump jetzt jedes Bild 1150-2000), Hoechstzahl Bilder
> 600 Promille leon/elza/zurueck_* 0, warten_* 9, kein Bild ueber 650 Promille. Neue Teile integration_r41_form_wagenweg_warten_leon /
_warten_elza (Tastenskript w8, 24 s warten statt 2 s = Lauf k5090, echte Spieler-HP, Laufende F1931; Pruefungen 3 und 5 entfallen dort,
die Form wird bis F1931 nur einmal getroffen). Schranken = Stand 2e3713c74 in diesem Riegel gemessen (320x240): warten_leon 9 Bilder > 600,
max 622; warten_elza 1, max 619; die anderen 0, max 596. Gegenprobe exe b9b3d574 (Runde 1, `nb4/gegenprobe/r2_*.txt`): warten_leon 38
Bilder > 600, max 707; warten_elza 38, max 701; zurueck_leon 28, max 775; zurueck_elza 28, max 773 -> alle vier rot. Der erste Riegellauf
mit der Schranke 7 (Abnahme-3-Zahl bei 640x480) war fuer warten_leon rot (9 Bilder, max 622; `nb4/r2_ctest_wagenweg_erstlauf.txt`): die
Fensteraufloesung 320x240 zaehlt zwei Grenzbilder (601-622) mehr; die Schranke ist deshalb im Riegel selbst gemessen und der Abstand zur
Gegenprobe liegt in der 650.

Kleinkram: die Abschlusszeile des Riegels nennt jetzt die Figur ("Elza getroffen", Abnahme 3 H6).

### NB4-11. Testlage Runde 2 (re15_port/build, Kopf 0c53ef647, ctest aus Git-Bash, PATH msys64 zuerst, SDL_ASSERT=always_ignore)

- `ctest -R "form|bahnsteig|birkin_form|r41_form|integration_r41_birkin_pistole|integration_r41_g5_tod|integration_r41_g5_westen" -j 4`:
  **100% tests passed, 0 tests failed out of 19** (Total Test time (real) = 387.28 sec), Log `nb4/r2_ctest_satzA.txt`: die sechs
  Wagenweg-Teile (Kampfkamera leon/elza/zurueck_* 0 Bilder > 600, max 596; warten_leon 9, max 622; warten_elza 1, max 619; Szene
  unveraendert wie NB4-5), integration_r41_birkin_pistole, integration_r41_g5_tod, integration_r41_g5_westen und die Form-/Bahnsteig-Riegel.
- `ctest -R "^unit_(birkin|r41_form|bb_|g5|n2_griff|r41_birkin)" -j 6`: **100% tests passed, 0 tests failed out of 13** (Total Test time
  (real) = 203.60 sec), Log `nb4/r2_ctest_satzD.txt`.
- Kein roter Fenster-Riegel ausser dem ersten warten_leon-Lauf mit der Abnahme-3-Schranke 7 (NB4-10, Schranke im Riegel nachgemessen).
- Offen: M-B-Rest (NB4-10, PORT-WAHL Cut 13, dem Nutzer zeigen); M-C, M-D (b)/(c) nicht Teil des Auftrags; Hinweise H1-H4 der Abnahme 3
  (Cut 15 Form am rechten Rand, Cut 13 bis 55 % vor dem Wechsel, Form am Ende von Cut 12 klein, grau-gruener Effektblock im u-Weg,
  vorbestehend) unveraendert.

## Nachbesserung 3 (AN-Abnahme 2 N1: Riesenform kommt im echten Wagenweg mit HP 0 in Wagen C)

Auftrag: Mangel N1 aus `.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_abnahme_2.md` (Laeufe `kette_ohne`, `form_kette_ohne`).
Belege: `stufe3/nachbesserung3/` (vorher_kette.txt, nachher_wagenweg.txt, gegenprobe_wagenweg.txt, probe_hp1000.txt;
werkzeug/lauf.sh + kette_ohne_sw.env + zustand.py der Abnahme). Gegenprobe-exe: `re15_port/build/platform/pc/re15_pc_nb3_basis_917195879.exe`
(unversioniert).

### NB3-1. Reproduktion (exe 917195879 = Kopf r41/form vor der Behebung)

Eigener Lauf `re15_port/build/nb3/vorher_kette` mit der Eingabe von `kette_ohne` (Raumladung 5090 am 6030-Eintrittspunkt
(27200,4900), Flag (3,89), Wagen A -> B -> C zu Fuss, NOAUDIO/SOFTWARE_RENDER). HP des 0x30 (Slot 2), state.log:

| Ort | Bild | HP | st/ss1 | stun | Herkunft |
|---|---|---|---|---|---|
| Wagen A (Raumladung) | F1 | 300 | 1/9 | 9 | EM30-INIT (Bank beim Spawn noch nicht geladen: Spawn debug.log Z. 89, "Bahnsteig-Form ... Slot 12" erst Z. 95) |
| Wagen B (Tuer slot 0, Schnitt 9) | F498 | 150 | 1/0 | 0 | Eintritt re15_bb_tick (s_bb.slot war -1) |
| Wagen C (Tuer slot 2, Schnitt 14) | F946 | **0** | 1/0 | 0 | kein Eintritt |
| Kampfstart | F1169 | 0 | 1/0, Clip 10 | | |

Bildgleich mit der Abnahme (F498/F946/F1169). Der Unit-Test `test_n3_wagenweg` (unten) bildet denselben Weg ohne Fenster nach und misst
dieselben Werte (A 300, B 150, C 0) in ROOM5090 und ROOM5091.

### NB3-2. Ursache

- Die Wagentueren sind **Selbst-Tueren** (Ziel = ROOM5090): der Port nimmt dafuer `scd_room_reenter` (game_step_common.c,
  "Same-room SCENARIO re-entry"), das den Aktorpool wischt und main00/sub00 neu startet, aber **kein** `re15_enemy_reset` ruft (nur der
  Raumwechsel room_common.c tut das) - also auch kein `re15_bf_reset`/`re15_bb_reset`. Beleg: die Bank-Zeile "Bahnsteig-Form" steht nur
  einmal im debug.log, die drei Spawn-Zeilen `Sce_em_set type=0x30` dreimal.
- Sce_em_set legt die Form jedes Mal neu in Slot 2 an, Port mit `a->hp = 0` und `a->state = 0` (scd_vm.c op_sce_em_set).
- Der Eintritt von re15_bb_tick (Port-Gegenstueck des G4-Ctors, setzt HP/Kasten/Sicherung) war an `s_bb.slot != slot` gebunden. In Wagen B
  lief er (s_bb.slot -1 seit der Raumladung, die Form war vor der Bank ueber den EM30-INIT gegangen), in Wagen C stand s_bb.slot schon
  auf 2: kein Eintritt, die HP blieben bei der 0 aus Sce_em_set. Mit HP 0 fuehrt der erste Treffer zu hp < 0 -> Zustand 3 -> Rueckstoss.
- Der Debug-Sprung der Integrationstests in Wagen B hat nur EINE Selbst-Tuer (B -> C): dort laeuft der Eintritt, HP 150.

Original (selbst disassembliert, `re15_disasm.py`, G4-Overlay = `info/re2leon/PL0/PLD/CDEMD0.EMS` ab Datei 0x653000, 0xB2D4 Byte,
@0x80100000):
- RE1.5 FUN_8001d600: `beq v1,v0,0x8001d988` @0x8001d968 vergleicht nur die Stage, `jal 0x800396fc` @0x8001d988 (bf e5 00 0c) laedt den
  Raum unbedingt, auch bei Ziel = derselbe Raum. FUN_8001a4c0 nullt den Entity-Pool (`sw zero,0(at)` @0x8001a4e8, 20 Slots).
- RE1.5 Sce_em_set: `sw zero,4(s0)` @0x800421e0 (04 00 00 ae) = +0x4..+0x7 = 0 bei JEDER Anlage.
- G4-Wurzel: `lbu v0,4(s5)` @0x80100314 (04 00 a2 92), `lw v0,-20640(at)` @0x80100328 (60 af 22 8c) = Tabelle @0x8010af60
  {0x8010062c Ctor, 0x80100bbc, 0x80106d3c, 0x801076ec}, `jalr v0` @0x80100330. Zustand 0 = Ctor.
- G4-Ctor 0x8010062c: `sw v0,4(s1)` @0x80100654 (04 00 22 ae, v0 = 1: +0x4 = 1, +0x5..+0x7 = 0 = Auftritt), HP `sh v0,342(s1)`
  @0x8010067c (56 01 22 a6) mit 1000 @0x80100678 (e8 03 02 24) bzw. 1100/900.
- RE1.5-0x30 zum Vergleich: INIT `ori v0,zero,0x12c` / `sh v0,154(v1)` @0x80117120-24 STAGE5.BIN (@0x8011690c-10 STAGE3.BIN), ebenfalls
  bei jeder Anlage. Beide Engines geben der neu angelegten Form also ihre Start-HP; ein "Schaden behalten" ueber den Wagenwechsel gibt
  es im Original nicht (die Tuer legt ein neues Entity an).

### NB3-3. Behebung (Commit 20998731f; Integrationstest 8ae5263fb, Pruefung 5 und Dossier im Folgecommit)

- `re15_bb_tick` (birkin_bahnsteig.c): Eintritt bei `s_bb.slot != slot || e->state == 0`. Zustand 0 = neue Anlage (Sce_em_set @0x800421e0),
  wie die Ctor-Verteilung @0x80100314-330. Im selben Raum setzt der Eintritt nichts zurueck (die Form steht nach dem Eintritt in 1/2/3).
- `RE15_BB_FORM_HP` (include/re15_birkin_formen.h) = 150, PORT-MASS (die 0x96 der Mutation `ori v0,zero,0x96` / `sh v0,154(v1)`
  @0x80118094-98 STAGE3.BIN, 96 00 02 34 9a 00 62 a4). Beide Zuweisungen in birkin_bahnsteig.c und der M3-Unit-Check nehmen sie; der
  neue Integrationstest liest den Wert aus dem Kopf. **Integration mit Spur GR:** dort steht `#define RE15_BB_FORM_HP 1000` an derselben
  Stelle (vor re15_bf_morph_zu_g5); bei der Zusammenfuehrung nur den Wert (und GRs Herleitungskommentar) uebernehmen. GR aendert auch den
  Eintritt von re15_bb_tick (Zeilen um die HP-Zuweisung): die Bedingung `s_bb.slot != slot || e->state == 0` muss erhalten bleiben,
  integration_r41_form_wagenweg und unit_birkin_bahnsteig (test_n3_wagenweg) pruefen das.

### NB3-4. Messwerte nach der Behebung (integration_r41_form_wagenweg, Laeufe lauf_d2f26a55 und lauf_b7cdeef9 bildgleich)

| | vorher (exe 917195879) | nachher |
|---|---|---|
| HP Wagen B / Wagen C (F946) | 150 / **0** | 150 / 150 |
| Kampfstart (Clip 10) | F1169, HP 0 | F1169, HP 150 |
| Treffer auf die Form bis zur Rueckstoss-Szene | 0 (Szene beim ersten Treffer F1237) | 10 (F1237 .. F1464, je 15), Szene beim 11. F1486 |
| Kampfdauer ab erstem Treffer | 0 Bilder | 249 Bilder (8,3 s) |
| Angriffe der Form im Beschuss | keine | Rennen F1296 (Leon hp 100 -> 95 F1322), Biss F1377/F1446 (verfehlt); Zucken F1420 |

Unit (`test_n3_wagenweg`, 5090 und 5091): A 300 (EM30-INIT), B/C/C2 je 150, Zustand 1/0; Gegenprobe mit der alten Bedingung: C und C2
HP 0 (6 FAIL). Im selben Raum bleibt die HP nach Treffer und 30 Ticks stehen (kein Eintritt ohne neue Anlage).

### NB3-5. Tests

- Neu `integration_r41_form_wagenweg` (tests/integration/test_r41_form_wagenweg.cmake): Raumladung 5090 am 6030-Eintrittspunkt (derselbe
  Raumwechsel re15_room_apply_pending wie die 6030-Tuer), dann zu Fuss A -> B -> C (Tastenskript w8 der Abnahme), Zielen + Feuer. Prueft:
  beide Wagentueren und drei Spawns, HP in B und C = RE15_BB_FORM_HP, Treffer im Kampffenster >= RE15_BB_FORM_HP/30 (min. 3), HP der
  Form steigen im Kampf nie (Schaden bleibt), Leon nach dem ersten Treffer mindestens einmal getroffen; kommt die Rueckstoss-Szene,
  dann erst nach Abzug >= RE15_BB_FORM_HP - groesster Einzelabzug, sonst muss die Form am Laufende noch leben.
  Messhaken RE15_R39_MESS_SPIELER_HP=100 (kein Spielverhalten), damit der stehende Schuetze auch mit GRs 1000 HP lebt; Endbild
  1500 + 4 x HP, Zeitlimit wachsen mit dem Wert. Gruen (90 s); **Gegenprobe exe 917195879 rot** (C: HP 0, 0 Treffer, Szene nach 0 HP
  Abzug, Leon nicht getroffen).
- Probelauf mit RE15_BB_FORM_HP 1000 (nur der Kopf geaendert, danach zurueck auf 150 und neu gebaut): HP B/C 1000, 64 Treffer, Leon 42x
  getroffen, keine Rueckstoss-Szene bis F5500, Form bei HP 40. Ursache: Leon steht ab ~F3250 an der Suedwestecke (-12,-26433) und zielt
  auf den naeheren 0x4D-Aktor, den r41/form aus der RDT anlegt ((600,-25300), Zustand 4, hp -1; Munition 82 -> 11 ohne Treffer auf die
  Form). In der Integration verwirft Spur AN den 0x4D bei (3,89) ohne (9,133) ("0x4D verworfen", AN-Abnahme 2 Lauf kette_ohne); dort
  pruefen.
- `unit_birkin_bahnsteig` um `test_n3_wagenweg` (5090/5091) erweitert, gruen; Gegenprobe rot (s.o.).
- Suite `^unit_|^prop_|^probe_` ohne `releaseskript|android|r37_masken36_ausschluss|paket|titelversion`, `-j 4`: 1036/1036 gruen (210 s).
- Integration (alle mit dieser exe): integration_r41_birkin_pistole gruen (253 s), integration_r41_g5_tod gruen (273 s),
  integration_r41_g5_westen gruen (293 s), integration_r35_zgirl_5090 gruen (147 s).

### NB3-6. Offen / Hinweise

- Die Ursache ist allgemein: jeder Port-Zustand ausserhalb von g_actors, der an "Slot" haengt, ueberlebt eine Selbst-Tuer. Fuer die Form ist
  das mit dem Zustand-0-Eintritt erledigt; `re15_bf_reset` (Rueckstoss-Szene s_rs, Kameras) laeuft bei Selbst-Tueren weiterhin nicht. Im
  Wagenweg ist das folgenlos (die Szene beginnt erst in Wagen C), nicht geprueft fuer einen Tuerwechsel waehrend der Rueckstoss-Szene.
- Erreichbarkeit (Weg Torschalter -> Zugtuer, H2 der Abnahme) nicht Teil dieses Auftrags.

## Nachbesserung 2 (E22: Kopf Krokodil statt Nilpferd, Textur durchgehend; Zusatzpunkt START im Griff) - laufend

Auftrag: Orchestrator nach E22 (NUTZER_ENTSCHEIDE.md). Drei Spuren: Kopf (Hauptstrang, dieser Abschnitt), Textur (eigener Bericht
stufe3/nachbesserung2_textur.md), START im Griff (eigener Bericht stufe3/nachbesserung2_griff_start.md). Kein RE-Wert ausser der START-Sperre
(Adressen dort); Kopf und Textur sind Gestaltung eines neuen Modells (PORT-WAHL).

| Schritt | Stand |
|---|---|
| T Textur | gemessen und behoben (nachbesserung2_textur.md): Gelenkkoerper 97 Einheiten je Texel (Mosaik), Seite 3 gespiegelt mit Wundflecken, Faecherkappen als Streifen; `s3_n2_textur.py bauen`, `haut_stoff` fuer Seite 3; Mosaik-Mass NB2 <= J in 7/7 Ansichten (Block 35,0 gegen J 44,3) |
| S START im Griff | behoben (nachbesserung2_griff_start.md): RE1.5 `andi v0,a0,0x40` / `bne` @0x8001cd04-08 ueberspringt den START-Block, der Birkin-Haken setzt aca3c 0x40 beim Packen (STAGE5 @0x8011b0b4/c0); Port re15_menu_start_poll verwirft START bei re15_bk_opfer_cmd(); Test unit_n2_griff_start |
| K Kopf | K4 Krokodil im Spiel-Asset (NB2-K1 Herleitung, NB2-K2 Endstand): IoU Oberkiefer 0,508 -> 0,573 (Ziel 0,70 mit diesem Gerippe nicht erreichbar, Referenz 0,647), Unterkiefer 0,801 -> 0,805, Umriss 0,954 31/31 / 0,949 7/7 |
| L Loecher/Clips/Tick | nicht verschlechtert (NB2-L): Nahtloecher 8768 -> 6363 px, Achsel+Huefte 319 -> 337 px, Clips/Posen/Reichweite wie NB1, Biss 3200-6000 am Kiefer |
| P Tests | 24/24 gezielt gruen, integration_r41_birkin_pistole gruen, APK-Asset-Gate OK (NB2-P); volle Suite offen |
| B Belege | stufe3/nachbesserung2/kopf/*, textur/*_endstand*, griff_start/ (NB2-S) |

### NB2-K1. Kopf: Form, Fit, Zielkonflikt der Maske

**Form (Kopfart 'kroko', varianten/K_alligator/generator/k_form.py `kopf_kroko`/`kiefer_kroko`, Gestaltung).** Schaedel hinten breit (Wk),
Augenwuelste oben (k_form.wulst), Schnauze in der Draufsicht gerade verjuengt auf taper x Wk (Grenze 0,30-0,50; RE2-Alligator EM23 Mesh 7
nur als Proportion gelesen: Spitze 790 / groesste Breite 1970 = 0,40, Laenge rund 4000), flache Oberseite (Querschnitt `ring_kroko`:
Lippenrand am breitesten, senkrechte Lippenwand, Flanke nach innen, Dach eben), Nasenhoecker als weiche Kuppe und Rosette an der Spitze,
11 obere Zaehne je Seite am Lippenrand nach unten aussen, Unterkiefer V-foermig mit 10 Zaehnen nach oben. UV nach Bogenlaenge
(`rohr(..., bogen=True)`), damit die lange Schnauze nicht gestreckt wird. K1-K3 bleiben unveraendert nachbaubar.

**Fit (k_kopf_fit2.py, Kamera Bild 2, Tiefenpuffer k_kern, Gelenkkoerper am Kopf wie K3 mitgerechnet).** Mass wie k_mass: IoU sichtbarer
Kopf 22 gegen Nutzer-Teil Oberkiefer, Kiefer 23 gegen Unterkiefer; Strafe, wenn der Umriss Bild 2 unter K3 (0,9426 in diesem Raster) oder
der Unterkiefer unter 0,80 faellt. Startlagen aus der globalen Suche k_kopf_suche.py (Halsrichtung x Schnauzenrichtung x Rollwinkel x
Kieferwinkel, je Lauf 1785-5355 Lagen).

| Lauf (_lauf/) | Form | IoU Oberkiefer (mit / ohne Koerper) | IoU Unterkiefer | Befund |
|---|---|---|---|---|
| K3 (NB1, Vergleich) | U-Schnauze, vorn breiter als der Ansatz | 0,498 / 0,574 (k_mass am Bau: 0,508) | 0,808 (0,801) | "rundlich, hippomaessig" (E22) |
| K4 (K4.log) | EM23-Proportion, Schaedel 470 breit, Schnauze 570 hoch | 0,30-0,34 | 0,66-0,76 | Kopf im Bild 10000-13000 px gegen 5832 px Maske: viel zu gross |
| Suche (K4_suche*.log) | flach, klein | Rasterbestes 0,40-0,48 | 0,25-0,58 | beste Richtung: Schnauze nach -z (Bild links), leicht nach unten = Lesung von K3; andere Richtungen deutlich schlechter |
| K4b | flach, taper <= 0,5 | 0,43 / 0,52 | 0,71 | Fit drueckt Verjuengung, Breite und Hoehe an die oberen Grenzen |
| K5 | + Rosette, kuerzere obere Zaehne, Kieferbreite | 0,47-0,50 | 0,75-0,77 | linke obere Maskenecke (216,280) bleibt frei, Schaedel ragt rechts oben hinaus |
| Suche K5 | Kiefer 26/40/53 Grad | Rasterbestes 1,245 (wie 26 Grad) | - | weiter offenes Maul hilft nicht |
| **Kfrei (Referenz, nicht gebaut)** | ohne Krokodil-Grenzen | **0,647 / 0,683** | 0,756 | nur mit Spitze breiter (taper 0,9) und hoeher als der Ansatz (Spitze 570, Ansatz 440): senkrechte, stumpfe Front |
| K6 | + Kieferlaenge/-tiefe frei | 0,617 | 0,826 | Nasenhoecker 240 = senkrechte Platte an der Spitze, Unterkiefer 161 laenger als der Oberkiefer: kein sauberes Krokodil |
| K7 | Hoecker weich und <= 140, Rosette <= 1,25, Kieferueberstand <= 60 | 0,602 / 0,603 (am Bau: 0,615) | 0,824 (0,818) | gebaut und im Spiel geprueft (Tests gruen), aber: Fit legte das Schaedeldach (-180) TIEFER als die Schnauze (-220) und machte die Schnauze 490 hoch auf ganzer Laenge: von vorn ein hoher Kasten (5080 frontal), kein flaches Krokodil -> verworfen |
| K8 | Schnauze flach (Ansatz 350-430, Spitze 250-330 hoch), Schaedeldach an die Schnauze gekoppelt (`schaedel` 40-200 hoeher), Hoecker <= 90 | Start 0: 0,568 / Start 1: 0,573 | 0,831 / 0,825 | Start 0 schlichter Keil; Start 1 Krokodilprofil (Schaedeldach +200 mit Augenwuelsten, Schnauze faellt zur flachen Spitze, Nasenhoecker 90, Rosette 1,2) |
| **K9 (gewaehlt -> Variante K4_kroko)** | K8 Start 1, Unterkiefer vorn 30 statt 59 ueberstehend (geschlossen praktisch buendig) | **0,570** (Fitraster, gerundete Winkel) | **0,807** | Krokodilprofil; Werte am Bau siehe NB2-K2 |

**Zielkonflikt (gemessen, nicht wegzufitten).** In Bild 2 endet der Hals (Knochen 21 aus der Brust-Pose von J) bei Bildpunkt (342,280);
der Kopfursprung kann sich nur rund 40 px (Halslaenge 503) davon entfernen. Die Masse des Oberkiefer-Teils liegt 85-125 px links unten
davon, die Schnauze muss also nach links zeigen, und ihr Ende liegt am BREITEN Ende der Maske (links 96 px hoch, rechts bei (330,316)
spitz). Eine nach vorn spitz zulaufende Schnauze fuellt das breite linke Ende nicht; die Oberkante der Maske faellt nach rechts zum Hals
hin ab, die eines Krokodilkopfs steigt zum Schaedel hin an. Darum erreicht selbst der Fit ohne Krokodil-Grenzen nur 0,647 und das nur mit
der stumpfen, hohen Front, die E22 ablehnt. Ziel 0,70 ist mit Gerippe und Koerperpose von J in dieser Kopf-Familie nicht erreichbar;
gewaehlt ist eine Krokodilform mit flacher Schnauze und hohem Schaedeldach (K9) statt der Form mit der hoechsten Zahl (K7, 0,615,
aber Kasten-Schnauze): Aussehen geht vor (Auftrag).

### NB2-K2. Kopf K4 im Spiel-Asset (Endstand)

Kette `stufe3/gen/s3_n2_kette.sh K9` (Protokoll stufe3/roh/n2_kopf/kette_K9.log): Variante `varianten/K_alligator/K4_kroko` (k_variante.sh:
Gelenkkoerper neu abgestimmt, Mundwinkel 0,8/1,0, Licht- und Kiefer-Eichung), Textur-Basis `stufe3/basis_n2` (s3_n2_textur.py bauen),
`s3_rig.BASIS_DIR` = basis_n2, Clips (s3_clips.py, unveraendert), Asset `stufe3/asset` = `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`
(EMD 291 816 Byte, 28 Clips, 799 Bilder; TIM 1408 x 256, 362 528 Byte), Testkopf `re15_port/tests/unit/form_j_posen.h` neu.

Kopfmasse K4 (Gestaltung, `K4_kroko/kopf.json`): Schnauzenspitze L 1620 ab Kopfursprung, Schaedel halbe Breite 382, Verjuengung 0,5,
Schnauze am Ansatz 428 hoch (Dach -158), Schaedeldach 200 hoeher (-358) mit Augenwuelsten, Spitze 330 hoch + Nasenhoecker 90, Rosette 1,2,
11 obere / 10 untere Zaehne je Seite, Unterkiefer 0,9 der Oberkieferbreite (Oberkiefer greift seitlich ueber), vorn 30 ueberstehend.

| Mass | K3 (NB1) | K4 (NB2) | Ziel |
|---|---|---|---|
| IoU Oberkiefer (k_mass, mit Gelenkkoerpern) | 0,508 | **0,573** | >= 0,70 (nicht erreichbar, NB2-K1; Referenz ohne Krokodil-Grenzen 0,647) |
| IoU Oberkiefer ohne uebrigen Oberkoerper | 0,625 | 0,627 | - |
| IoU Unterkiefer | 0,801 | **0,805** | >= 0,80 |
| Umriss Bild 1 / Punkte / Boden-Verdeckung | 0,954 / 31/31 / 5/7 | **0,954 / 31/31 / 5/7** | >= 0,954 31/31 |
| Umriss Bild 2 / Punkte / Boden-Verdeckung | 0,948 / 7/7 / 9/9 | **0,949 / 7/7 / 9/9** | >= 0,948 7/7 |
| Zeichnung Bild 1 / Bild 2 | 4/9 / 2/7 | 4/9 / 2/7 | - (Z4 und N2 neu, Z9 faellt: Gelenkkoerper Arm r - Brust im Hautton, NB2-O) |
| Kieferspitze / Zahnreihen-Mitte im Raum von Knochen 23 | (1139,-75,0) / (911,-75,0) | (1190,-85,0) / (928,-85,0) | - |
| Biss Clip 26 Bild 16-30: Spitze vor G4-Bisspunkt Teil 10 + 1400 | 1640-1690 | 1715-1737 | - |
| Krallen rechts / links (Paare, mittl. px) | 4/5 12,8 / 2/3 1,5 | 4/5 12,6 / 2/3 1,5 | unveraendert |

Tick (engine/src/birkin_bahnsteig.c, PORT-WAHL mit Messherkunft im Kommentar): Biss-Probe Teil 23 + (928,-85,0) statt (911,-75,0), Radius 1500
(@0x80104f88-a4), Biss-Band 0xce4 (@0x80102434-54) + 1740 = 5044 statt + 1700 (gleiche Regel: gemessene Reichweite, Maximum auf 10 aufgerundet).
Test test_n1_biss_kiefer: Probe (928,-85,0).

### NB2-T. Textur am Endstand (Kopf K4, Messwerkzeug der Textur-Spur, stufe3/nachbesserung2/textur/*_endstand.*)

Ursachen und Behebung im Bericht der Textur-Spur (stufe3/nachbesserung2_textur.md): Gelenkkoerper-Hautkacheln (vorher 97 Einheiten je Texel =
grosse flache Quadrate), Seite 3 aus `haut_stoff` statt gespiegelter Brust mit Wundflecken, Faecherkappen planar. Im Hauptstrang dazu (k_bau.py,
Kopfart 'kroko'): Fleischflaechen (Kiefer innen/aussen, Gaumen) tragen nur 0,4 der Hautstruktur (`FLEISCH_STRUKTUR`), weil das 2-3-mal hellere
Fleisch sonst den Texelkontrast entsprechend vervielfacht; die gemalten hellen Zahnflecken entfallen (K4 hat echte Zaehne). Gemessen vor der
Daempfung am K4-Asset: Kiefer Innen 12,5, Kante 35,0, Block 57,2.

| Teil (Bild, 7 Spielansichten) | J | NB1 | NB2 (Endstand) |
|---|---|---|---|
| Gesamt: Innen / Block / Texel px | 6,0 / **44,3** / 3,96 | 5,4 / 44,5 / 5,28 | 5,9 / **31,1** / 3,00 |
| Gelenkkoerper: Kante / Innen / Block / Texel px | - | 16,2 / 1,8 / 85,0 / 9,94 | 13,2 / 5,3 / 23,3 / 2,65 |
| Kopf: Kante / Innen / Block | 12,3 / 4,3 / 62,1 | 22,9 / 6,0 / 44,9 | 23,5 / 4,4 / 29,8 |
| Kiefer: Kante / Innen / Block | 9,2 / 2,1 / 51,8 | 32,0 / 13,7 / 66,7 | 27,6 / 5,7 / 27,2 |
| Rumpf (Referenz): Kante / Innen / Block | 10,7 / 6,0 / 37,8 | 13,7 / 6,0 / 38,7 | 13,1 / 6,1 / 38,9 |
| Grenze Gelenkkoerper/Haut (Kante) | - | 21,7 | 19,3 |

Je Ansicht Block J / NB1 / NB2: 5080 Gier 0 29,8 / 34,2 / 27,6; 1024 27,9 / 31,9 / 27,0; 2048 38,5 / 43,6 / 32,3; 3072 73,3 / 65,5 / 40,8;
5090 Gier 3874 35,0 / 34,9 / 24,6; 802 34,4 / 34,8 / 25,3; 1826 21,0 / 24,4 / 19,5. **Gegenprobe: NB2 <= J in allen 7 Ansichten.**
Netz (blickunabhaengig): Hellfleck Kopf 2,6 % -> 0,2 %, Kiefer 5,8 % -> 3,3 % (Zahnfeld, gewollt hell), Kantensprung Kopf 15,7 -> 8,4,
Gelenkkoerper 9,9 -> 7,8, gesamt 11,4 -> 10,3 (J 12,2). Rest: Kiefer-Kante 27,6 (Grenze Fleisch/Zahnfeld und Rand des Unterkiefers, J 9,2),
J-eigene Inselnaehte an Armen/Beinen/Klauen unveraendert (wie J).

Belege: `heatmap_J_NB1_NB2_endstand.png` (Buehne, je Ansicht Render | Texelbloecke | Kanten), `echt_J_NB1_NB2_endstand.png` (echtes Fenster,
eigene exe-Kopie, gdigrab; Spalten J | NB1 | NB2), Zahlen `bild_J_NB1_NB2_endstand.json`, `netz_J_NB1_NB2_endstand.json`, `tabelle_endstand.md`.

### NB2-L. Loecher, Posen, Clips, Tick (nicht verschlechtert)

- Loecher am Spiel-Asset (`k_loecher.py messen`, 13 Tick-Clips x 8 Bilder x 10 Ansichten, `stufe3/nachbesserung2/loecher/nachher.json`):
  Nahtloecher NB1 503 / 8768 px -> NB2 385 / 6363 px; ohne Mundwinkel 3183 -> 2984 px; Mundwinkel 5585 -> 3379 px; Achsel+Huefte 319 -> 337 px
  (Bein l 82 -> 70, Bein r 120 -> 141, Arm l 95 -> 103, Arm r 22 -> 23); Kopf - Hals 12 -> 0. Erst mit den K3-Groessen der Koerper an Bein r -
  Huefte (naht.json Feld `nachbesserung2`); mit der Proxy-Abstimmung waren es dort 458 px.
- Clips (`s3_pruef.py`, `nachbesserung2/pruef.json`): Bildzahl, tiefster Punkt, Drehung/Bild, Gleiten, Tempo wie NB1 (Rennen Bild 11 = Pose Bild 1
  333,5; Drohen 67; Hieb rechts 13,8; Sturz/Flucht gewollt). Posen Bild 1/Bild 2/Ruhe bytegleich im Asset (Pins). Reichweite der Hiebe unveraendert
  (`s3_reichweite.py`: rechts Bild 28-30, links 16-17, Einstiege frei).
- Biss: test_n1_biss_kiefer (5090/5091) trifft wie NB1 von 3200 bis 6000 mit Kieferprobe < 1500, Gegenprobe 2600-3000 ohne Treffer.

### NB2-S. Sichtbelege fuer den Nutzer (stufe3/nachbesserung2/)

| Blatt | Inhalt |
|---|---|
| `kopf/spielkamera_alt_neu.png` | echtes Fenster (eigene exe-Kopie, gdigrab, Renderer des Nutzers), Kopf aus den Spielkameras: 5080 frontal / 3/4 / seitlich, 5090 Lage/Pose Bild 2 (3/4 von vorn) / frontal / seitlich; links alt K3, rechts neu K4 |
| `kopf/bild2_masken_alt_neu.png` | Screenshot 2 mit Nutzer-Teilen Oberkiefer (orange) / Unterkiefer (rosa), sichtbarer Kopf/Kiefer des Modells; K3 / K4 / Referenz "nur Maske" |
| `kopf/rundum_spiellicht_alt_neu.png` | Offline-Buehne mit Spiel-Licht (5090 Schnitt 11), Kamera vor dem Kopf -90..+90 Grad, Ruhe (Maul zu) und Pose Bild 2; K3 / K4 |
| `textur/echt_J_NB1_NB2_endstand.png` | Textur im echten Fenster, 5 Ansichten, Spalten J / NB1 / NB2 |
| `textur/heatmap_J_NB1_NB2_endstand.png` | Textur-Heatmap (Buehne, 7 Spielansichten): Render / Texelbloecke / Kantenspruenge je Satz |
| `griff_start/echt_vorher_nachher.png` | Zusatzpunkt START im Griff (Bericht nachbesserung2_griff_start.md) |

Laeufe: `re15_port/build_n2/spiel/kopf_{K3,K4}_*`, `tex_{J,NB1,NB2E}_*`; Werkzeug `stufe3/gen/s3_n2_endbelege.sh`.

### NB2-P. Tests (re15_port/build, Endstand)

- Gezielt (24, `ctest -R "unit_birkin_*|unit_r41_form_emd|unit_n2_griff_start|unit_r41_birkin_wand*|unit_g5_*|probe_p2/p3_birkin*|unit_r17|r18/r20/r30_birkin*"`):
  24/24 gruen, darunter unit_birkin_bahnsteig (Asset-Formtreue gegen form_j_posen.h, Boeden, Biss am Kiefer), unit_n2_griff_start.
- integration_r41_birkin_pistole gruen (241 s): Leon und Elza je "Mutationen 0, Form-HP 150 bis 0, Rueckstoss 1, Sturz 1, G5 1, G5 tot 1".
- Paket-Gates: `release/apk_asset_gate.py --quellbaum` OK (4282 Dateien, Tuer-Soll erfuellt); make_package check_tree-Bedingung fuer
  BAHNSTEIG_FORM.EMD/.TIM (vorhanden, nicht leer, bytegleich mit stufe3/asset) erfuellt; make_package selbst nicht gelaufen (kein Release-Bau).
- Volle Suite nicht gelaufen (df.exe-Haenger der Release-/Titelversion-/Android-Bash-Tests, N12).

### NB2-O. Offen

- Oberkiefer-IoU 0,573 statt Ziel 0,70: mit Gerippe und Koerperpose von J nicht erreichbar (NB2-K1, Referenz 0,647 nur mit stumpfer Front).
  Hoeher kaeme man nur ueber eine andere Brust-/Halspose in Bild 2 (Halsende naeher an die Maske) - das aendert die abgenommene Form von J.
- Kiefer-Kantensprung 27,6 (J 9,2): Grenze heller Unterkiefer / dunkle Haut ist gewollt (E21 beiger Kiefer), Rest am Zahnfeld.
- Zeichnung (form_pruefung d, Spiel-Asset): Bild 1 4/9 wie NB1, aber anders verteilt: Z4 (helles Band am rechten Oberschenkel, Koerper Bein r
  wieder K3-gross) und N2 bestehen neu, Z9 (Schraegstrich neben dem rechten Oberarm, FORMBLATT2 "kann Hintergrund sein") faellt, weil der
  Gelenkkoerper Arm r - Brust jetzt den Hautton seiner Umgebung traegt (Textur-Spur 4); Bild 2 2/7 wie NB1.
- START-Sperre fuer andere Spieler-Haken (Zombie, Hund, Gorilla) nicht geprueft (nachbesserung2_griff_start.md).
- Volle Suite steht aus.

## Nachbesserung 1 - N0: Ursache "Texturen vollkommen kaputt" (Nutzerbefund zur K3-Vorschau, 2026-11-02 abends)

Befund: im echten Fenster (stufe3/nachbesserung1/loecher_echt_vorher_nachher.png, rechte Spalte) und offline (stufe3/formpruefung/
rundum_hell.png) grau-weiss karierte, fleckige Bloecke an Hals, Schulter, Ellbogen, Knie, Haenden und am Kopf; bei J nicht.

Ursache, gemessen (stufe3/gen/s3_n1_textur.py pruefen, Bytes des Assets):
1. **Gelenkkoerper (Hauptursache).** Die 31 Gelenkkoerper von K3 (varianten/K_alligator/generator/k_naht.py `kugel`, Zeilen 147-152)
   uebernehmen fuer JEDE ihrer 40 Flaechen die UV-Ecken, CLUT und Seite der jeweils naechsten Flaeche des Elternmeshes, unveraendert
   und in beliebiger Drehung. Aus Hautstuecken mit weissem Fleck, Kante oder Naht werden so je Koerper 14-64 verschiedene, gedrehte und
   gestreckte Texturstuecke nebeneinander: ein Schachbrett. Gemessen am K3-Asset: verschiedene UV-Stuecke je Koerper-Mesh 14-64,
   Helligkeitsspanne (p95-p5) je Koerper bis 186. Die Koerper sitzen genau dort, wo der Befund die Bloecke sieht (Hals, Schulter,
   Ellbogen, Knie, Hand, Kopf-Hals-Muffe und Mundwinkel am Kopf).
2. **Texel ausserhalb des TIM (Nebenursache, Kopf).** 35 Flaechen von K3 (Kopf 22: 27, Kiefer 23: 8) reichen auf Seite 3 bis u = 128,
   also Texel-x = 128 + 3*128 = 512 bei einem 512 breiten TIM: eine Spalte hinter dem rechten Rand.
3. **Kein Seiten-/CLUT-Fehler im Port.** Der Port rechnet Texel-x = u + (page & 0xF) * 128 (platform/pc/main.c `page_off`) und waehlt
   die CLUT-Zeile ueber das CLUT-Wort (render_pc.c re15_render_textured_tri: v_offset = CLUT-Index * TIM-Hoehe, Textur 512 x 1024 mit
   4 CLUT-Zeilen, re15_render_pc_upload_tim_slot); die Offline-Buehne (form_buehne) nutzt dieselbe Regel. Jede Flaeche liest Seite und
   CLUT-Zeile ihrer Quellflaeche (Tabelle Seite/Zeile/Texelband je Flaeche: kein Band ausserhalb der Seite ausser Punkt 2). Darum zeigt
   die Offline-Vorschau dieselben Flecken wie das Spiel; sie taeuscht hier nicht.

Behebung (Asset-Spur, stufe3/gen/s3_n1_textur.py, Basis stufe3/basis_n1): jeder Gelenkkoerper bekommt ein zusammenhaengendes,
fleckfreies Hautstueck seiner Umgebung (Breiten-/Laengengrad -> Rechteck 12-32 Texel, gleiche Seite und CLUT-Zeile wie die naechsten
Originalflaechen, Nachbarflaechen teilen ihre UV-Kanten); nur Ellbogen r setzt die Haut darunter fort (er liegt in Bild 1 auf dem weissen
Dreieck Z5). Kopf/Kiefer-UV auf die Seite geklemmt. Gemessen: Texel ausserhalb TIM/ohne CLUT 35 -> 0, Koerperecken mit UV-Sprung > 2 Texel
89 % -> 19 %, Koernung Koerper/Umgebung 1,10 (Mosaik) -> 0,83, Formpruefung unveraendert (Bild 1 0,954 31/31, Bild 2 0,948 7/7).
Verworfene Fassungen (2x2 einfarbig = Plastikbeulen, Projektion ueberall = wieder fleckig) und alle Zahlen: stufe3/nachbesserung1_asset.md
Abschnitt 1a. Beleg im echten Fenster (5080 Schnitt 4 Gier 0/1024/2048/3072, 5090 Schnitt 11 Pose Bild 2 und Gier 802/1826, links K3 vorher,
rechts nachher): stufe3/nachbesserung1/textur_vorher_nachher.png.

## Nachbesserung 1 - Stand (2026-11-02 abends)

Auftrag: K3 ins Spiel (E20/E21), Loecher (M2), Maengel M1-M8 der Abnahme 1 (STUFE3_abnahme_1.md), Kopf-Folgen fuer den Tick. Zwei Spuren:
Asset (Generator, Offline, eigener Bericht stufe3/nachbesserung1_asset.md) und C-Seite (Tick, Szene, Kamera, Ton, Koerper; dieser Abschnitt).
Alle neuen Zahlen sind PORT-WAHL fuer das neue Modell; was aus dem G4-Overlay (CDEMD0.EMS rec 1 @0x80100000) oder RE1.5 kommt, traegt
seine Adresse im Code.

| Punkt | Behebung | vorher -> nachher (gemessen) |
|---|---|---|
| A K3 im Asset | Generator auf K3_fit/basis_n1; Ruhe = J-Ruhe mit Kiefer zu; Kiefer zu in Stehen/Gehen/Platzhaltern, uebrige Maeuler x 0,65 (max. ~31 Grad); Pins Clip 10 Bild 45-52 / Clip 12 Bild 0 = K3-Pose Bild 2, Clip 0 Bild 11 = K3-Pose Bild 1; form_j_posen.h neu | Formpruefung Bild 1 0,948 -> 0,954 (31/31), Bild 2 0,935 -> 0,948 (7/7) = K3 offline |
| B Loecher | Gelenkkoerper K3, Hiebe ohne Huft-Gier (Spalt Bein R - Huefte in den Hieben) | Achsel+Huefte (13 Tick-Clips x 8 Bilder x 10 Ansichten) 4465 -> 319 px (-93 %); alle Naehte ohne Mundwinkel 16534 -> 3183 px |
| M1 Sturz unsichtbar | Sturz (Clip 12) beginnt im Wagen bei x <= -2500 statt -6283; Clip-Spur traegt die Form durch die Tuer (-5683); Taumeln ab x <= 3000 in Cut 11 | Abnahme: Clip 12 nie im Bild (x -6313 -> -14583); jetzt Cut 11 ab Taumeln x 3000, Sturz x -2551 -> Tuer bei Bild ~25 |
| M2 Loecher | s. B | s. B; Fenster-Stichprobe s. N2 |
| M3 Gebruell stumm | EDD-Ids 9-15 (Bank 24 FFFFFFFF) -> Raum-Ton 10 (STAGE5 @0x8011af58-5c) | 5090: 8 SILENT-Rufe (ton5090_re2se.log) -> 0; Raum-Ton 10 4x im Auftritt + 1x Sturz (n1 l5090_feuer debug.log) |
| M4 Kamera-Nahsicht | (a) Auftritt in Cut 11 (Kamera von Bild 2) statt Cut 15; (b) Nahsicht-Wache Cut 12/13 | (a) Cut 15: 89 % Bildflaeche Textur (Buehne x 1200) -> Cut 11 mit ganzer Form; (b) s. N4 |
| M5 Rueckstoss gleitet | neuer Clip 13 Taumeln (40 Bilder, Spur -80/Bild, Fuesse verriegelt) | 147 je Bild auf stehendem Clip 8 -> 80 je Bild aus der Clip-Spur, Gleiten der Fuesse 3,4 |
| M6 Wand / Drehung | Wandklemme erkannt (Wurzel < halber Spur-Schub, 6 Bilder) -> Weichen bricht ab, 90 Bilder Sperre; Flucht: zurueckweichen bis 6000, dann Drehen im Gang (Rate 32), dann Clip 11 | Test: Weichen an der Wand 120 -> <= 30 Bilder; Flucht dreht max. 32/Bild (vorher 128 auf stehenden Fuessen) |
| M7 Spieler in der Form | Koerper-Segmente Huefte/Brust am Gerippe (r 1100/1000, G4-Felder), Wandklemme r 1500, Kasten 1700/2600/1600, Schatten x -1300..4000 +-1500 | Spieler >= 1450 von der Rumpfachse (Test), Schwanz in der Flucht >= 2721 vom Spieler (Test) |
| M8a Blut | Anker Kopf (Teil 22) statt Brust | Blut > 1500 hoch, > 1800 vor der Wurzel (Test) |
| M8b Todeszweig | geprueft, nichts zu schreiben (N7) | - |
| M8c BAHNSTEIG_G4 | tools/birkin_bahnsteig.py schreibt nach build/extracted/bahnsteig_g4, shared_assets abgelehnt | - |
| M8e Spruenge | Asset-Spur | Rennen 106 -> 46 Grad, Drohen 54 -> 43, Krallen 51 -> 38 |
| D Biss am Kiefer | Probe Teil 23 + (911,-75,0), Radius 1500 (@0x80104f88-a4); Entscheid < 0xce4 oder 0xfa0..5000 | Test: alle Treffer mit Kiefer < 1500 vom Spieler; Gegenprobe Abstand 2600-3000: alter Brustpunkt 54 entfernt, kein Treffer |

### N1. Ton (M3)

ENEMSE-Bank 24 selbst gelesen (ENEMSE.VBS, TOC @0x800a7b1c Zeile 24: EDT @0x1d8000, 3752 Byte, SE-Karte 32 Worte; Skript
scratchpad n1/bank24.py): hoerbar sind Id 0-4, 6, 8 (VAG 2-5, 18-20, 0,21-0,72 s); Id 5 und 7 zeigen auf VAG 1 mit 48 Byte (Attrappe); Id
9-15 sind FFFFFFFF. Die G4-Paarzeile @0x800a7400 Zeile 24 = {0x26,0x00}: der G4 ist erster kind, also ohne +0x10 (FUN_8005bd6c
`if (*param_2 & 0x2000) param_1 += 0x10`), die zweite Kartenhaelfte (Id 16-30) erreicht er nie. Damit sind die G4-EDD-Gebruelle (Clip 10
Bild 30/51/81/113, Clip 27 Bild 36/60) auch in RE2 stumm. PORT-WAHL: bb_bild_ton spielt fuer EDD-Ids >= 9 den Birkin-Schrei des Raums,
Raum-Ton 10 (STAGE5 @0x8011af58 `jal 0x800453d0` / @0x8011af5c `ori a0,zero,0xa`, Todesphase des 0x30 bei Bild 0x3c, 0x95 == 0x3c
@0x8011af48-50). Die EDD-Daten der Clips bleiben G4-gleich (Test gegen CDEMD0.EMS unveraendert). Der Ton am Wagenende ohne Riesenform
bleibt re15_audio_room_se(10). Gemessen im echten Fenster 5090 (n1 l5090_feuer, debug.log `[se] Stimme: se=10 ... vag=11`): Auftritt F300,
F330, F360, F390 (= Clip 10 Bild 30/51/81/113) und Sturz; re2se.log ohne SILENT. Vorher (Abnahme ton5090_re2se.log): se 9 x2, 10 x1,
11 x4, 12 x1 SILENT. Testhaken re15_bb_ton_zaehler, Test test_n1_gebruell (4 Gebruelle im Auftritt).

### N2. Rueckstoss und Sturz (M1, M5)

- Phase 1 (birkin_formen.c bf_rs_riesenform_phase1): Clip 13 Taumeln im Kreis, Vortrieb aus der Clip-Spur (bb_spur/bb_schieben), Ruecken zum
  Ausgang (Gier lenkt mit 64 je Bild auf den Spiegelpunkt des Ausgangs), Cut 12 bis x 3000, ab x <= 3000 Cut 11 (Form >= 4600 vor dem
  Auge 7626, von vorn). Bei x <= -2500 und >= 90 Ticks Beschuss Sturz (Phase 3); vorher am Platz zucken (Clip 8 ab Bild 5).
- Gemessen n1 l5090_feuer (vor der Cut-11-Aenderung): HP 0 F708, Taumeln F725-F832 von x 5929 nach -2551 (80 je Bild, 3,6 s), Sturz Clip 12
  ab F832 in Cut 11, Tuer bei Bild ~25, G5 F933. Bildleiste: stufe3/nachbesserung1/m1_m5_rueckstoss_sturz.png.
- Vorher (Abnahme): Phase 1 147 je Bild auf Clip 8, Phase 3 erst bei x -6283, Clip 12 ausserhalb des Bildes.
- Elza 5091 (n1 e5091_feuer, mit Cut 11 ab x 3000): HP 0 F696, Taumeln F718, Cut 11 ab F752 (x 2954), Sturz F822 (x -2566), G5 F923.

### N3. Biss am Kiefer und Kopf-Folgen (D)

- Entscheid Orchestrator (Weg a): Trefferprobe am Kiefer, Teil 23 + (911,-75,0) (Mitte der vorderen Zahnreihe, 228 hinter der Spitze
  (1139,-75,0) von Mesh 23), Radius 1500 von G4 (@0x80104f88-a4); der alte Brustpunkt Teil 10 + (1400,0,0) entfaellt fuer den Biss.
- Entscheid Biss 0xce4 (@0x80102434-54) + 1700 = 5000 (PORT-WAHL). Umgesetzt als Band: Biss bei dist < 0xce4 wie G4 ODER 0xfa0 <= dist <
  5000. Grund: in der Nabe ueberschreibt der Biss den vorher gewaehlten Hieb (k824 umfasst k512); mit "dist < 5000" verdraengte er jeden Hieb
  (Hieb dist < 0xfa0) und damit Doppelschlag/Nachhieb. Gemessen im Fenster (n1 l5090_angriffe, Leon ohne Feuer): Biss aus 4300-5000, Griff,
  Weichen, Hieb links (Clip 4) + Nachhieb (Clip 9), Leon stirbt F1134. Zur Bestaetigung beim Orchestrator: das Band ist meine Auslegung.
- Test test_n1_biss_kiefer (5090/5091): Abstand 2600-6400 in 200er-Schritten; jeder Treffer mit Kieferprobe < 1500 vom Spieler, Treffer
  von 3200 bis 6000; Gegenprobe Abstand 2600-3000: der alte Brustpunkt laege 54-60 vom Spieler (haette getroffen), die gezeichnete Schnauze
  geht ueber ihn hinweg (Kieferprobe 1372-1378), kein Treffer.
- Hiebe: Krallenform unveraendert (Variante K: nur Ton), Einstiegsbilder des Doppelschlags treffen nicht (s3_reichweite, Asset-Spur).
- Kasten (Schuss-/Koerper-Kasten des Aktors): hit_offset_x 800 / r 1000 (0x30 @0x8011ee64) -> Mitte Huefte..Schnauze 1700, laengs 2600,
  quer 1600 (Haende +-1590). Schatten 2300 x 1300 um die Wurzel (G4 @0x801002cc/@0x80100300) -> Rechteck x -1300..4000, quer +-1500
  (re15_bb_schatten, platform/pc/main.c).

### N4. Kamera (M4)

- (a) Kampfstart: die Riesenform tritt in Cut 11 auf (Kamera von Bild 2) statt in Cut 15. Gemessen offline (form_buehne, Clip 10 Bild 30):
  Cut 15 bei x 1200 89 % des Bildes, x 3000 45 % und abgeschnitten; die Blickachse von Cut 15 verlaesst den Gang bei x 3220, eine Lage
  weiter weg gibt es nicht. Halt bis Clip 10 Bild 60 (Pose Bild 2 in Bild 45-52 liegt darin), dann Schnitt der Spielerlage und Zonen frei.
  Fenster: n1 l5090_angriffe Bild 248-266 (Cut 11, ganze Form vor dem Wagenende).
- (b) Nahsicht-Wache Cut 12/13: Form < 4500 vor dem Auge, Spieler dahinter -> Gegenkamera, wenn der Spieler dort >= 1500 vor dem Auge steht.
  Fenster n1 l5090_angriffe: nach dem Griff liegt Leon bei x 14932-16722 (Cut 13), die Form kommt von x 6400; bei F846 schaltet die Wache auf
  Cut 12 (Leon von hinten, die Form vor ihm, Bild 446-512) statt 8 s Form zwischen Kamera und Leon (Abnahme Bild 284-338).
  Rest: als Leon nach dem Hieb bei x 17218 steht (1300 vor dem Auge von Cut 12), greift die Wache nicht; Cut 13 zeigt die Form 5800 vor
  dem Auge gross (Bild 516-560), Leon klein dahinter sichtbar (nicht verdeckt).

### N5. Wand und Flucht (M6)

- Klemme: s_bb.klemm zaehlt Bilder, in denen die Wurzel weniger als die Haelfte des Spur-Schubs kam (gemessen am Tickanfang gegen die Lage
  vor dem Schub; Wandklemme und Koerper-Schub wirken danach). Weichen bricht nach 6 Bildern ab (Sperre 90 Bilder ohne Weichen), Gehen lenkt
  ab 10 Bildern zusaetzlich mit 32 (@0x801018ec-908) zum Spieler. Test test_n1_weichen_klemme: <= 30 Bilder Weichen an einer Wand in 120.
- Flucht (bb_flucht, drei Phasen): zurueckweichen bis 6000 (Schwanz 3520 + Spieler 450 + 2000), Klemme oder 60 Bilder; Drehen im Gang
  (Clip 5, Lenken 32) bis Ziel im 256er-Kegel oder 96 Bilder; dann Clip 11 mit Lenken 32. Test test_flucht (5080/5081): Drehen 56 Bilder
  max. 32 je Bild, Clip 11 max. 32 je Bild (vorher 128 auf stehenden Fuessen), Schwanz (Glieder 4-6) mindestens 2721 vom Spieler, Flag
  (5,0x1c) am Ende, Treffer halten die Flucht nicht auf.
- Fenster n1 l5080_leon: Flucht ab t=510 (Weichen), Drehen t=570-600, Clip 11 ab t=615, Raumtext "The power already has been supplied".

### N6. Koerper, Breite (M7)

- Spieler-Schub und Form-Schub (beide Richtungen wie G4 FUN_80034d0c) ueber zwei Segmente am Gerippe der gezeichneten Pose: Huefte Teil 0
  r 1100 (+0x9a = 0x44c @0x8010076c), Brust Teil 10 r 1000 (+0xba = 0x3e8 @0x80100784), Halbhoehe 2000 (+0x9e/+0xbe = 0x7d0
  @0x80100770/@0x801007a4); Feldfolge wie G5 @0x80100530-88. G4 setzt sie fest an die Wurzel (x 400 / 0); die K3-Form ist laenger, deshalb
  am Gerippe (PORT-WAHL). Ein Kopfsegment (probiert) hielt den Spieler vorn >= 4150 fern: kein Hieb, kein Griff mehr - verworfen. Griff-Paar
  ausgenommen. Test test_n1_koerper: Spieler auf Brust/Huefte gesetzt -> >= 1400 von Teil 0/10.
- Wandklemme der Wurzel r 1500 statt 1000 (Haende +-1590; Gaenge 3200/3500).
- Schwanz in der Flucht: s. N5.

### N7. Kleinkram (M8)

- a Blut: Anker Teil 22 (Kopf) statt Teil 10 (@0x80107390-b8 nimmt Teil 10); die Versaetze bleiben die des Overlays. Test: Blut > 1500
  hoch und > 1800 vor der Wurzel.
- b Todeszweig STAGE5 @0x8011aeb0-d0 selbst disassembliert: `lw v1,-14460(v1)` (g_entity(cur) 0x800ac784) / `ori v0,zero,0x12c` /
  `sh v0,156(v1)` und `ori v0,zero,0x3` / `sb v0,7(v1)` im Delay-Slot von `jal 0x8004ef90` @0x8011aecc. Das sind Felder des sterbenden
  Aktors selbst: +0x9c = ai_timer 300, +0x7 = sub_state_3 = Phase 3 (Port enemy_ai_common.c Todesmaschine Phase 2, `e->ai_timer = 300`,
  Phase 3 wartet auf grid & 0xf == 2). ROOM5080 liest sie nicht (SCD: kein Member_cmp; sub01 @0x0078A fragt nur Flag (5,0x1c)). Die Flucht
  setzt das Flag und nimmt den Aktor aus (active = 0): kein Leser mehr, nichts zu schreiben.
- c tools/birkin_bahnsteig.py schreibt nach build/extracted/bahnsteig_g4 und lehnt shared_assets ab (Probe: Abbruch mit Meldung).
- d Bodenausnahmen: Asset-Spur (Drohen 242 -> 67, Rennen Bild 11 = Pose Bild 1 334 unveraendert).
- e Spruenge: Asset-Spur.

### N8. Loecher im echten Fenster (M2, Stichprobe der Abnahme)

5080 Schnitt 4, RE15_BB_POSE -23500,-18200 (Asset-Spur, stufe3/nachbesserung1/loecher_echt_vorher_nachher.png): Clip 5 Bild 28 Gier 2048
1059 -> 414 px, Gier 0 1074 -> 764 px, Clip 10 Bild 100 Gier 2048 1649 -> 1213 px geschlossene Hintergrundinseln. Die verbliebenen Inseln
grenzen laut Kennungsbild der Buehne an BEIDE Vorderbeine (Arm L 11-13 gegen Oberarm R 16) bzw. BEIDE Hinterbeine (Oberschenkel 8 gegen 9,
Unterschenkel 26): der Blick unter dem Rumpf zwischen den Beinen, keine Naht an Achsel oder Huefte (vorher grenzte die Achsel-Insel an
Oberarm/Unterarm und Hals derselben Seite). Offline (13 Tick-Clips, 10 Ansichten) Achsel+Huefte 4465 -> 319 px; Rest 25 Ansichten zu
3-37 px (Rennen Bild 9-12 Bein L, Hieb links Bild 7, Nachhieb Bild 9, Taumeln Bild 0, Sturz, Drohen von unten). Nicht belegt: ob der Nutzer
mit "Loecher zwischen Bein und Oberkoerper" auch die Luecke zwischen den Beinen meint; schliessen liesse sie sich nur mit einem "Rock"
unter dem Rumpf, der in Bild 1/2 sichtbar waere.

### N9. Tests

- Gezielt (re15_port/build, Stand fbef4bcc7): unit_birkin_ai, _bahnsteig, _formen, _opfer, _szenen, unit_r41_form_emd,
  unit_r41_birkin_wand_rein/_g5/_griff, unit_g5_boss/_morph/_tentakel, probe_p2_birkin_g5, probe_p3_birkin_rest, unit_r17/18/20/30-Birkin-Pins,
  unit_r30_n_room5080_birkin_wand: 19/19 gruen; integration_r41_birkin_pistole gruen (N12; vorher rot: erwartete Clip 8 in der Rueckstoss-Szene,
  jetzt Clip 13 oder 8).
- Neu/angepasst in test_birkin_bahnsteig.c: Rueckstoss (Clip 13, Schritt <= 100 je Bild, Sturz im Wagen x -2500..-3500), Flucht (Phasen,
  Gier <= 40 je Bild, Schwanz > 900 vom Spieler, gemessen 2721), test_n1_biss_kiefer, test_n1_gebruell, test_n1_koerper,
  test_n1_weichen_klemme, test_n1_blut_kopf; Asset-Spur: test_asset/test_masse (Clip 13, Sturz-Spur, Drohen-Bodenausnahme), raum_laden.
- Android-Asset-Gate (release/apk_asset_gate.py --quellbaum): OK, 4282 Dateien, RE2 363. Windows-Paket-Gate (release/make_package.sh
  check_tree): prueft BAHNSTEIG_FORM.EMD/.TIM auf vorhanden, nicht leer, bytegleich - erfuellt (Dateien liegen im Quellbaum, Paket kopiert RE2
  ganz); make_package selbst nicht gelaufen (baut kein Paket ohne Release-Bau).

### N10. Sichtbelege (echtes Fenster, eigene exe-Kopie re15_pc_n1b, gdigrab hwnd=, Renderer des Nutzers; Laeufe re15_port/build/n1_spiel/)

Werkzeug stufe3/gen/s3_n1_spiel.sh (gleiche Eingaben wie die Abnahme), Leisten stufe3/gen/s3_n1_leiste.py. Laeufe: l5090_feuer, l5090_feuer2
(mit Cut 11 ab x 3000), l5090_angriffe, e5091_feuer (Elza), l5080_leon, e5081 (Elza), k3_bild1/k3_bild2 (Posen-Messhaken).

| Blatt (stufe3/nachbesserung1/) | Inhalt |
|---|---|
| m1_m5_rueckstoss_sturz.png | vorher Gleiten + leere Tuer (Abnahme), nachher Taumeln, Cut 11 von vorn, Sturz durch die Tuer |
| m4a_kampfstart.png | vorher Cut 15 nur Textur, nachher Auftritt in Cut 11 |
| m4b_nahsicht.png | vorher Form fuellt Cut 13 8 s, nachher Wache Cut 12 (Leon vorn, Form dahinter), Rest Bild 524-548 |
| m6_m7_5081.png | vorher Elza unter dem Bauch, nachher Elza neben dem Kopf, Blut am Kopf, Flucht mit Drehen |
| k3_spiel_neben_bild1_bild2.png | K3 im Spiel (Endasset) neben Screenshot 2 (Cut 11, Pose Bild 2) und Screenshot 1 (5080 Cut 1, Pose Bild 1) |
| textur_vorher_nachher.png, loecher_echt_vorher_nachher.png, kopf_frontal_spiel.png | Asset-Spur (N0, N8, Klaerung Kopfform) |

Ablauf gemessen (state.log/birkin_dbg.log): 5090 Leon mit Feuer: Auftritt Cut 11 F282-F341, Biss aus 4400-5000 (bis HP 0 bei F708), Taumeln
F725-F832, Sturz F832, G5 F933; Elza 5091 gleich (HP 0 F696, Sturz F822, G5 F923). 5090 Leon ohne Feuer: Biss, Griff (Leon fliegt nach x
14932), Wache Cut 12 F846, Weichen, Hieb links + Nachhieb, Leon stirbt F1134. 5080/5081: Fall, Auftritt, Rennen, Biss/Weichen am Westarm,
Griff, Flucht ab HP 0 (zurueck, Drehen, Clip 11), Raumtext msg 3; Leon und Elza gleich.

### N11. Offen

- Biss-Band 0xfa0..5000 ist meine Auslegung des Entscheids "+1700" (sonst kein Hieb mehr) - Bestaetigung Orchestrator. Folge im Spiel: auf
  4000-5000 beisst die Form, Hiebe erst naeher.
- Kamera Cut 13: steht Leon dicht (1300) vor dem Auge von Cut 12, greift die Wache nicht; die Form fuellt Cut 13 dann gross, Leon bleibt
  klein sichtbar (m4b Bild 524-548).
- Loecher zwischen den Beinen (N8) und Restspalte offline (25 Ansichten, 3-37 px).
- Kopfform ("Schildkroete statt Alligator") zurueckgestellt (E21-Nachtrag), Klaerungsblatt kopf_frontal_spiel.png.
- Integration mit r41/annette: dort Annette-Szene in 5090 vor dem Kampf (Cut 11/12, Hieb links Clip 4 trifft Annette). Diese Nachbesserung
  aendert in 5090 den Kampfstart-Schnitt (Auftritt in Cut 11 bis Clip 10 Bild 60), den Bisszweig der Nabe und den Koerper-Schub; Clip 4
  (Nummer, Bildzahl, Trefferbits 14-17) ist unveraendert, Trefferbild im Asset jetzt 16-17 statt 15-17 (Reichweite).
- Voller ctest (Unit/Prop/Probe): s. N12.

### N12. Testlage Endstand (re15_port/build, Stand fbef4bcc7)

- Gezielt: unit_birkin_bahnsteig, _szenen, _formen, unit_r41_form_emd gruen; integration_r41_birkin_pistole gruen (269 s): Leon und Elza je
  "Mutationen 0, Form-HP 150 bis 0, Rueckstoss 1, Sturz 1, G5 1, G5 tot 1".
- Unit/Prop/Probe voll (`ctest -R "^(unit|prop|probe)_" -j 8 --timeout 900`, 1945 s): **1054 von 1061 gruen**, 7 rot, alle
  Bau-Umgebungs-Skripttests ohne Spielcode: unit_r38_releaseskript_trocken/_ablauf/_fehler_bau/_fehler_umfeld/_cmd und
  unit_r39_titelversion_ablauf (Timeout 1200 s), unit_r35_android_bash_mutanten (rot nach 1444 s). Ursache der Timeouts gemessen: die Tests
  haengen in `df -Pk <Wegwerf-Repo>` (Platz-Pruefung von release_alles.sh) - Git-df.exe kehrt auf dieser Maschine seit dem Abend nicht
  zurueck (auch ein eigenes `df -h /c` um 19:5x hing); dieselben Tests haengen gleichzeitig in r41_moosgang (18:41), r41_gatorfix (19:42) und
  r41_sichel (20:27) an derselben Stelle. Diese Nachbesserung aendert nichts unter release/, android/ oder an diesen Tests (git diff leer).
  Wiederholung mit -j 3 brach mitsamt ctest ab; die haengenden eigenen Testprozesse (40 bash/df, nur Pfade unter r41_form) per PID beendet,
  8 df.exe-Leichen lassen sich nicht beenden (Kernelwarten).

Auftrag (Orchestrator, nach E19): J bekommt Gerippe-Pruefung, eigene Clips nach dem Muster RE2-G4, das G4-Verhalten aus Spur R auf die
neuen Clips, Einbau in 5080 und 5090 unter eigenem Asset-Namen, Tests, Sichtbelege. Bindend: `NUTZER_ENTSCHEIDE.md` E1-E19.
Arbeitsbaum `.claude/worktrees/r41_form`, Zweig `r41/form`. Dieses Dossier wird fortlaufend geschrieben; wer nach einem Abbruch
weitermacht, liest zuerst Abschnitt 0.

## 0. Stand und naechster Schritt

| Schritt | Stand |
|---|---|
| 1 Sichern + `git merge master` | erledigt: 8423920d4 (Formstand, `.gitignore` im Ordner), f89159cf4 (Merge, keine Konflikte) |
| 2 Gerippe pruefen | erledigt (Abschnitt 2): alle Teile haben Knochen, keine Aenderung an Form/Textur noetig |
| 3 Clips | gebaut (Abschnitt 3), Asset `stufe3/asset/BAHNSTEIG_FORM.EMD` = `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` |
| 4 Angriffsmuster | Tick auf die neuen Clips umgestellt, Flucht 5080, Sturz 5090 (Abschnitt 4) |
| 5 Einbau, Tests | Lader, Paket-Gate, Tests angepasst (Abschnitt 5). Mit dem Endstand (Bau `re15_port/build_s3`): unit_birkin_bahnsteig, unit_birkin_szenen gruen, integration_r41_birkin_pistole gruen (Leon + Elza: keine Mutation, Form-HP 150 bis 0, Rueckstoss, Sturz, G5, G5 faellt) |
| 6 Sichtbelege | erledigt: `stufe3/kontakt/` (00_uebersicht + 12 Clips), `stufe3/spiel/` (7 Blaetter, echtes Fenster) |
| Voller ctest | Lauf in `re15_port/build` (Stand Commit 0e10882f9, Asset waehrend des Laufs getauscht): siehe Abschnitt 8 |
| Abnahme 1 | NICHT BESTANDEN (STUFE3_abnahme_1.md, M1-M8) |
| Nachbesserung 1 | Abschnitte N0-N12 oben: K3 im Spiel, Texturfix, M1-M8, Kopf-Folgen; Commits a452893f1 (Asset), fbef4bcc7 (C-Seite) |

Commits dieser Stufe: 8423920d4 (Sichern), f89159cf4 (Merge master), a7f9922d4 / 79aefb203 (Generator WIP), 0e10882f9 (Einbau,
Tests), 072d1d3f7 (Sichtbelege, Pistolen-Test nachgezogen), 6200b3d35 (Asset-Nachschliff, Uebergabepunkt G5).

Generator dieser Stufe: `stufe3/gen/` (`s3_rig.py` Gerippe/Posen/IK, `s3_clips.py` Asset, `s3_angriffe.py` Clips, `s3_g4.py`
G4-Clipdaten, `s3_pruef.py`/`s3_dreh.py`/`s3_reichweite.py` Zahlenpruefung, `s3_testkopf.py` Testreferenz, `s3_kontakt.py`/
`s3_uebersicht.py`/`s3_blatt.py` Kontaktblaetter, `s3_spielfolge.sh`/`s3_folge_blatt.py` Spielbilder). Zwischenbilder unter
`stufe3/roh/` (nicht versioniert, nicht fuer den Nutzer).

## 1. Bestand vor dieser Stufe (gemessen im Quelltext nach dem Merge)

**5080/5081 (Bahnsteig).** RDT sub00 @0x00746 `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00 ..` = Typ 0x30, grid 0x33, Lage
(-18100,-5600,200), solange Flag (3,48) = 0 (@0x0072A). grid 0x33 friert die Wurzel ein (`re15_birkin_root`, Bit 0x20), bis sub03
nach der Generator-Folge Pos_set (-18100,-5600,-17800) @0x00822, Speed_set/Add_speed (Fall auf y 0) und Member_set(0x0C,0x13)
@0x0083E faehrt. Port heute: Original-EM30 kaempft (`re15_birkin_ai_tick`), Tod -> Todesmaschine Phase 2 setzt Flag (5,0x1c)
(@0x8011aecc STAGE5), sub01 @0x0078A sieht das, setzt (3,48)/(3,49), Musik, sub04. **Die Bahnsteig-Form kommt in 5080 nicht vor.**

**5090/5091 (Zug).** sub00 @0x0124A Typ 0x30 grid 0x33, sub04 Member_set(0x0C,0x13) @0x0130A. Port heute: EM30 kaempft, toedlicher
Treffer -> Mutation (Sub 6, Clip 0x14) -> `re15_bf_mutation_fertig` laedt die Bank neu -> RE2/BAHNSTEIG_G4.EMD (G4 umgeformt,
27 Knochen) -> `re15_bb_tick` (engine/src/birkin_bahnsteig.c, Spur R) -> beim Tod Rueckstoss-Szene (`re15_bf_rueckstoss_start`,
birkin_formen.c) -> G5 (Typ 0x36). Belegt in R_bahnsteig.md 8.1 (Leon 5090, Elza 5091 gleich).

**Soll nach E5/E6/E17 (Nutzer):** 5080 Riesenform, Kampf endet mit Flucht; 5090 Riesenform, wird aus dem Zug geschossen; dann G5.
Die Mutation zur Riesenform liegt nach E6/E17 am Ende des letzten Sichel-Form-Kampfs (Spur SF, Baum r41_sichel), also VOR dem Bahnsteig.
Folge fuer diese Stufe: in 5080 und 5090 traegt der 0x30 die neue Form ab der Raumladung; die EM30-Runde samt Mutation in 5090 entfaellt.

## 2. Gerippe J

Gelesen aus `varianten/J_dicker/FORM.EMD` (`stufe3/gen/s3_lesen.py`): 32 Knochen (Port-Grenze RE15_EMD_MAX_BONES = 32, voll), 32 Meshes,
Mesh i an Knochen i, Eltern vor dem Kind.

| Knochen | Eltern | Rolle |
|---|---|---|
| 0 | - | Huefte, Wurzel |
| 1-6 | 0,1,..,5 | Schwanz, Kette von sechs Gliedern nach -X (Versaetze 592..678) |
| 7, 10 | 0, 7 | Wirbel, Brust (= BB_TEIL_RUMPF 10) |
| 8/24/25, 9/26/27 | 0/8/24, 0/9/26 | Bein links/rechts: Oberschenkel, Unterschenkel, Fuss (kantig, Sohle) |
| 11/12/13 | 10/11/12 | Arm links: Oberarm, Unterarm, Hand (= Teil 13, Hieb Seite 1); Klauen 14, 15, 28, 30 |
| 16/17/18 | 10/16/17 | Arm rechts: Oberarm, Unterarm, Hand (= Teil 18, Hieb Seite 0); Klauen 19, 20, 29, 31 |
| 21/22/23 | 10/21/22 | Hals, Kopf, Kiefer |

Alle Teile, die eine Bewegung braucht, haben schon einen eigenen Knochen; Teil 10/13/18 sitzen dort, wo der Tick sie erwartet. Neue Knochen
gingen ohnehin nicht (32 von 32). Posen von J: Clip 0 = Bild 1, Clip 1 = Bild 2, Clip 2 = Ruhe (`form.json`).

**Bewegungspruefung.** Die Form ist starr je Knochen (das EMD kennt keine Gewichte, Mesh i haengt an Knochen i): eine Drehung verzerrt nie
ein Glied, sie kann nur an Gelenken Spalten oder Ueberlappungen oeffnen. Die Posen Bild 1 und Bild 2 von J drehen Beine, Arme, Kopf und
Schwanz schon weit aus der Ruhe (Unterschenkel bis 100 Grad, Fuss bis 160 Grad) und bestehen die Formpruefung; die Clips bleiben in diesem
Bereich. An Gerippe, Mesh und Textur ist nichts geaendert (MD1 und TIM bytegleich mit J, Test). Zahlenpruefung je Clip:
`stufe3/gen/s3_pruef.py` (tiefster Punkt je Bild, Gleiten stehender Glieder in der Welt, groesste Drehung je Bild), `s3_dreh.py`
(Spruenge je Knochen), `s3_reichweite.py` (Hiebe gegen die Trefferprobe). Sichtpruefung: Kontaktblaetter, Abschnitt 6.

## 3. Clips (Asset BAHNSTEIG_FORM.EMD, 28 Clips, 760 Bilder)

Generator: `stufe3/gen/s3_clips.py` (ohne Argumente, schreibt `stufe3/asset/`, deterministisch: md5 zweier Laeufe gleich), Posen und
Angriffe `s3_angriffe.py`, Gerippe/IK `s3_rig.py`. Danach `cp stufe3/asset/BAHNSTEIG_FORM.* re15_port/shared_assets/RE2/` und
`python stufe3/gen/s3_testkopf.py` (Testreferenz `re15_port/tests/unit/form_j_posen.h`). `form_emd.py rundlauf` auf dem Asset:
byte-gleich, `FE.pruefe` 0 Fehler, 0 Warnungen.

**Aus G4 (Daten, kein Code):** RE2 CDEMD0.EMS kind 0x34 rec 3 (Sektor 3310, Datei 0x677000; EDD Paar 1 ab 0x67b960, EMR Paar 1 ab
0x67d05c). Je Tick-Clip unveraendert: Bildzahl, EDD-Bits 16..31 (Treffer Bit 16, Ton Bit 27 + Id) und Spur (Keyframe +6/+10). Der Test
vergleicht jedes Bild (Clip 10 ausgenommen, s.u.). Damit gelten im Tick unveraendert: Doppelschlag k_doppel @0x8010b0f0, Trefferbilder
@0x80101cb8 / @0x80104f58, Griff Bild 8 @0x80105408, Zucken ab Bild 5 @0x80107224, Lenkschwellen 21/31 @0x80102924-68, Ueberrennen 201
@0x80102e80, Gang 2666 Einheiten je Durchlauf.

**PORT-WAHL:** alle Posen und Zeitpunkte in den Clips, Fussarbeit, Schwanzwelle, die neuen Clips 2, 11, 12 samt Laenge, Spur und Toenen
(Ton-Ids aus der G4-Bank 24), die Platzhalter 1, 6, 7, 13-25 (je ein Bild Ruhepose, vom Tick nie gespielt).

| Clip | Rolle | Bilder | Treffer | Ton (Bild: Id) | Herkunft |
|---|---|---|---|---|---|
| 0 | Rennen (Sprunggalopp) | 18 | - | 17: 1 | G4 Clip 0; **Bild 11 = Pose Bild 1** (Rueckansicht Bahnsteig) |
| 2 | Stehen/Atmen | 60 | - | - | PORT-WAHL; Bild 0 = Ruhe von J |
| 3 | Hieb rechts (Teil 18) | 60 | 26-30 | 25: 1, 32: 0 | G4 Clip 3 |
| 4 | Hieb links (Teil 13) | 50 | 14-17 | 13: 1, 19: 0 | G4 Clip 4 |
| 5 | Gehen (auch Weichen rueckwaerts) | 89 | - | 43: 0, 73: 0 | G4 Clip 5 |
| 8 | Zucken/Treffer | 30 | - | - | G4 Clip 8 |
| 9 | Nachhieb (beidhaendig auf den Boden, Bruellen) | 63 | - | 1: 0, 24: 6 | G4 Clip 9 |
| 10 | Drohen/Auftritt | 126 | - | 30: 11, 51: 10, 74: 1, 79: 0, 81: 12, 113: 11, 114: 1 | G4 Clip 10; **Bild 45-52 = Pose Bild 2** |
| 11 | Flucht (Ende 5080): Gebruell, Galopp, Sprung nach oben | 57 | - | 5: 10, 37: 1 | PORT-WAHL |
| 12 | Sturz aus dem Zug (Ende 5090) | 56 | - | 2: 10 | PORT-WAHL; **Bild 0 = Pose Bild 2** |
| 26 | Biss | 35 | 16-30 | 19: 5, 27: 5 | G4 Clip 26 |
| 27 | Griff (Packen Bild 8, Schleudern, Bruellen) | 100 | - | 14: 8, 36: 9, 60: 9, 82: 7 | G4 Clip 27 |

Die Posen von J stehen bytegleich im Asset (Wurzel + 32 Winkel; Test `pose_gleich`): Ruhe = Clip 2 Bild 0, Bild 1 = Clip 0 Bild 11
(auch zweimal in der Flucht, die den Galopp wiederholt), Bild 2 = Clip 10 Bild 45-52 und Clip 12 Bild 0. **Formpruefung von J am
Spiel-Asset gelaufen** (`stufe3/formpruefung.json` -> `stufe3/formpruefung/bericht.txt`, `form_pruefung.py pruefe`): Bild 1 IoU 0,948,
Pruefpunkte 31/31, Boden/Verdeckung 5/7; Bild 2 IoU 0,935, Pruefpunkte 7/7, Boden/Verdeckung 9/9 - Zahl fuer Zahl der Bericht von J;
`FE.pruefe` jetzt 0 Fehler, 0 Warnungen (J hatte die Warnung "Tick erwartet Clips, die fehlen").

Abweichung von G4 mit Grund, **Clip 10 Spur**: Pose Bild 2 steht in Bild 45-52 still; G4 bewegt die Figur dort um zusammen rund -400.
Dieses Tempo liegt jetzt in Bild 53-60, die Summe (47) ist die von G4. Der Auftritt bedient keine Schwelle des Ticks.

**Wie gebaut:** Gehen = Seitenfolge-Gang, zwei Zyklen je 89 Bilder, Fuesse und Haende per IK in der Welt verriegelt gegen die G4-Spur
(Gleiten <= 8 je Bild), Fuss rollt ab, Schwanz als Welle. Rennen = Sprunggalopp, Glieder je Bild per IK ueber Stuetzpunkte, Basis Pose
Bild 1. Hiebe: Arm hoch und weit zur Seite, dann quer nach vorn unten; der Einstieg des Doppelschlags (Clip 3 Bild 20, Clip 4 Bild 7)
liegt ausserhalb der Trefferprobe (Teil 13/18 Radius 1700, Versatz (0,600,0) Radius 1500, Spieler 2600 voraus, `s3_reichweite.py`),
getroffen wird in Bild 28-30 bzw. 15-17. Biss: Kopf zurueck, Maul auf, Vorstoss in Brusthoehe des Spielers (Kiefer bei y -1460), Schuetteln. Griff: beide Arme greifen (Krallen zu in
Bild 8), Schleudern, Bruellen mit wechselnden Armen. Drohen: Hocke, Gebruell, Pose Bild 2, Aufbaeumen mit beiden Armen.

**Grenzen, gemessen (`s3_pruef.py`):** tiefster Punkt <= 25 unter dem Boden ausser Rennen Bild 11 (Pose Bild 1 selbst: linker Fuss 334
unter der Ebene, so wurde J an Bild 1 gefittet) und Drohen Bild 33-38 (Schienbein streift auf dem Weg in die Hocke bis 242, Bild 35-38; je Bein eigenes Faltfenster, damit der Rueckweg nicht ueber den Boden schleift: Gleiten 245 -> 23). Schweben bis
90 im Ausfall der Hiebe und im Aufbaeumen. Groesste Drehung je Bild: Gehen 12 Grad, Rennen bis 106 Grad (Fuss beim Abheben). Flucht und
Sturz verlassen absichtlich den Boden.

## 4. Angriffsmuster und Ablauf im Spiel

Tick `engine/src/birkin_bahnsteig.c` (Spur R): Sub-Nummern, Entscheidungen, Reichweiten, Kegel, Sperren, Schaden, Trefferzaehler
unveraendert mit ihren @0x-Adressen aus dem G4-Overlay (CDEMD0.EMS rec 1). Die Clipnummern BB_C_* bleiben, die Form bedient sie.
Neu in Stufe 3 (PORT-WAHL nach E5/E6/E17; RE-Zahlen nur, wo genannt):

| Stelle | Was | Beleg / Kennzeichnung |
|---|---|---|
| birkin_formen.c `re15_bf_riesenform_raum`, `re15_bf_bahnsteig_bank_faellig` | Riesenform in 5080/5081 und 5090/5091 ab der Raumladung; in 5090 keine EM30-Runde, keine Mutation (`re15_bf_mutation_offen` = 0) | E5/E6/E17 |
| birkin_bahnsteig.c `bb_flucht` (Sub 21) | Tod in 5080/5081 -> Clip 11: abdrehen (128 je Bild im Gebruell, Gestaltung; danach 32 wie @0x801018ec-908), Galopp, Sprung nach oben; am Clip-Ende Flag (5,0x1c) wie der Birkin-Tod @0x8011aecc (STAGE5), den ROOM5080 sub01 @0x0078A abfragt; Aktor aus. Treffer halten die Flucht nicht auf. Riegel wie im Todeszweig @0x8011a458 / @0x8011a54c-50 | PORT-WAHL |
| birkin_formen.c Rueckstoss Phase 3, `re15_bb_sturz_tick` | am Zugende Clip 12 einmal (Spur schiebt rueckwaerts hinaus), Kamera Cut 11, dann Phase 2 (unsichtbar, Pause), G5 wie bisher | PORT-WAHL |
| enemy_ai_common.c `re15_birkin_root`, `re15_bb_wartet` | solange grid & 0x20 die Wurzel einfriert (@0x80116a7c-a88), spielt die Riesenform Clip 2 im Kreis (5080: Fall vom Dach, 5090: steht im Wagen) | PORT-WAHL |
| birkin_formen.c Messhaken `RE15_BIRKIN_HP_AT` | gilt jetzt auch in 5080 | Messhilfe |

Unveraendert: Schatten 2300 x 1300 (G4 @0x801002cc / @0x80100300) und Kastenversatz 800 (Gestaltung Spur R); die Form ist ohne Schwanz
rund 3600 lang, Haende bei z +-1590. Opfer-Bank EM30. Teile 10/13/18 = Brust / Hand links / Hand rechts.

**Uebergabe an den G5 (Abgrenzung zur Spur r41/g5auftritt, Vorgabe Orchestrator).** Diese Stufe baut in 5090 die Riesenform bis
einschliesslich ihres Sturzes aus dem Zug. Der Uebergabepunkt ist EINE Funktion in birkin_formen.c:
`bf_rs_form_ist_draussen(e, mit_gebruell)` - aufgerufen genau einmal, wenn die Form den Wagen verlassen hat (Riesenform: Ende von
Sturz-Clip 12 in Rueckstoss-Phase 3; ohne Riesenform, z. B. `RE15_BIRKIN_FORM=original`: am Wagenende wie bisher, mit Gebruell). Sie
setzt Phase 2 (Form unsichtbar, Pause `BF_RS_PAUSE`), danach laufen unveraendert `re15_bf_morph_zu_g5` und die Szenenkamera `s_kam_g5`.
Nicht angefasst: G5-Auftritt, G5-Kamera, `enemy_ai_boss_g5.c`. Wer den neuen G5-Auftritt baut, setzt an `bf_rs_form_ist_draussen` an.
Ablauf der Phasen in 5090: 1 Beschuss/Zurueckdraengen (Clip 8 im Kreis) -> 3 Sturz (Clip 12, Kamera Cut 11) -> Uebergabe -> 2 Pause -> G5.

## 5. Einbau und Tests

- Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`; `BAHNSTEIG_G4.EMD/.TIM` aus dem Repo entfernt (nicht mehr geladen;
  `tools/birkin_bahnsteig.py` bleibt als Beleg von Spur R). Lader `pc_enemy_load_bahnsteig` (main.c), debug.log
  `Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips`.
- Paket-Gate `release/make_package.sh`: BAHNSTEIG_FORM.EMD/.TIM statt BAHNSTEIG_G4.* (vorhanden, nicht leer, bytegleich).
- `form_spiel.sh`, `form_emd.py rundlauf` (Vorgabedatei) auf dem neuen Namen.
- `tests/unit/test_birkin_bahnsteig.c`: `test_asset` neu (Formtreue gegen `form_j_posen.h`: MD1/TIM-CRC, drei Posen; Clips gegen RE2-G4:
  Bildzahl, EDD-Bits, Spur; Mindestlaengen 31/21/6/9; Trefferbits 3/4/26; Tempo Gehen >= 31, Rennen + 10 >= 201; Stehen/Flucht/Sturz),
  `test_masse` (Boden mit begruendeten Ausnahmen, Hoehe Clip 5 = 3126 +-60 als Formtreue-Pin), neu `test_flucht` (5080, 5081) und
  `test_wartet` (5080, 5090, 5091). Entfallen: M1/M3/M4 (Tausch EM30 -> Form nach der Mutation in 5090, den es nicht mehr gibt).
  Arrays von 27 auf 32 Knochen.
- `tests/unit/test_birkin_szenen.c`: `test_bahnsteig` ohne EM30-Mutation, prueft den Sturz-Clip; `test_modellwahl` mit 5080/5081/5091/50A0.
- `tests/integration/test_r41_birkin_pistole.cmake`: 32 Meshes, keine Mutation, Sturz (Clip 12) zwischen Rueckstoss und G5.

## 6. Sichtbelege (fuer den Nutzer)

**Kontaktblaetter je Clip** `stufe3/kontakt/clip_NN_name.png` (`stufe3/gen/s3_kontakt.py`, Offline-Buehne `form_buehne.py`, Textur und
Gouraud ohne Raumlicht): Zeilen = rechte Seite, schraeg vorn links, schraeg hinten rechts; Spalten = Bilder des Clips; die Figur laeuft um
ihre Spur weiter, die Kamera folgt, das Bodenraster steht in der Welt (Gleiten waere sichtbar). Marken unter dem Bild: T = Trefferbild,
S<n> = Ton-Id. Flucht und Sturz mit fester Kamera.

**Spiel, echtes Fenster** (`stufe3/gen/s3_spielfolge.sh`: eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER,
ffmpeg gdigrab `hwnd=` auf das eigene Fenster, nur die eigene PID beendet; Blaetter mit `s3_folge_blatt.py`) unter `stufe3/spiel/`:

| Blatt | Lauf | Inhalt |
|---|---|---|
| `5090_leon_1_auftritt_angriffe.png` | Debug-Sprung 5090, Leon laeuft in den Wagen, 10 s ohne Feuer | Auftritt (Drohen) im Gang, Gang, Doppelhieb, Nachhieb, Griff |
| `5090_leon_2_rueckstoss_sturz_g5.png` | danach Zielen + Feuer gehalten (Pistole) | Rueckstoss-Szene, Sturz aus dem Wagenende in Cut 11 (Kamera von Bild 2), RE2-G5 kommt herein |
| `5080_leon_1_fall_kampf.png` | Debug-Sprung 5080, Generator-Folge (Skript lauf1 aus Runde 30) | Riesenform faellt nach dem Generator herab, Auftritt, Angriffe an Leon |
| `5080_leon_2_flucht.png` | Messhaken `RE15_BIRKIN_HP_AT=1100` (HP 0 statt langem Beschuss) | Flucht: abdrehen, Galopp aus dem Bild, danach Raumtext msg 3 wie nach dem Birkin-Tod |
| `5091_elza_uebersicht.png`, `5081_elza_uebersicht.png` | dieselben Laeufe mit Elza (`RE15_TITLE_SHOT_CHAR=1`) | gleicher Ablauf |

Gemessen in diesen Laeufen (birkin_dbg.log, state.log je Lauf unter `re15_port/build/s3_spiel/<name>/`): debug.log `Bahnsteig-Form: 32
Meshes, 32 Knochen, 28 Clips` in allen vier Raeumen. 5090/5091: Auftritt 126 Bilder, Gang, Hieb 3 -> Doppel 4 -> Nachhieb 9, Griff (Leon
fliegt), Treffer bis HP 0, Rueckstoss (Clip 8, HP 1), Sturz (Clip 12 bei x -6290 -> -15320), G5 (Typ 0x36). 5080/5081: nach der Generator-
Folge (msg 0/1 F247/F500) Fall vom Dach, Auftritt, Rennen, Weichen, Griff, Biss, Hieb; Flucht (Sub 21, Clip 11) bei F1694, fort bei
F1749, msg 3 bei F1751 (sub04 nach Flag (5,0x1c)). Elza 5081 und 5091 identisch.

Beobachtet: In Cut 12 (Kamera vor Leon) ist die Form im Nahkampf meist nur angeschnitten (sie kommt von hinter der Kamera); im Gang
(Cut 14/15) und am Generator ist sie ganz zu sehen. Die Form ist so gross, wie J nach den Screenshots ist (Hoehe im Gehen 3126); sie
fuellt den Wagen fast aus.

## 7. Offen

- Sturz-Clip 12 nach dem ersten Spiellauf ueberarbeitet (stand auf der Schwanzspitze senkrecht, weil Pose Bild 2 schon 52 Grad aufgerichtet
  ist): jetzt hoechstens 22 Grad dazu, Fall mit Fallkurve. Im Spiel neu zu sehen (Abschnitt 0).
- Drohen Bild 33-38: Schienbein streift bis 242 unter den Boden (Bild 35-38) (Weg in die Hocke von Bild 2), Rennen Bild 11 = Pose Bild 1 mit Fuss 334
  unter der Ebene (so an Bild 1 gefittet). Beides im Test als begruendete Ausnahme.
- Die Flucht laeuft vom Spieler weg; die Wandklemme der Wurzel bleibt an, an einer Wand rutscht die Form seitlich weiter, bis der Sprung
  sie aus dem Bild nimmt (5080 gemessen: von z -22553 bis -14139).
- Integration in master und Abgleich mit Spur SF (r41/sichel: Mutation am Ende der Sichel-Form, Effekte) stehen aus; beide Zweige aendern
  birkin_formen.c, main.c, test_birkin_szenen.c.

## 8. Testlage (gemessen 2026-11-02)

- Voller ctest in `re15_port/build` (Bau von Commit 0e10882f9, `-j 6`, Start 09:55, Dauer 14823 s): **1407 von 1408 gruen, 1 rot**:
  `integration_r41_birkin_pistole`. Ursache gemessen: die Anpassung dieses Tests aus 0e10882f9 war nicht im Commit (Patch lief im
  Hintergrund ohne Wirkung), der Lauf pruefte noch "Mutation erwartet 1" / "27 Meshes". Nachgezogen in 072d1d3f7; mit dem Endstand
  (Bau `re15_port/build_s3`, Asset 6200b3d35) **gruen**: Leon und Elza je Mutationen 0, Form-HP 150 bis 0, Rueckstoss 1, Sturz 1, G5 1,
  G5 tot 1.
- Mit dem Endstand neu gebaut und gelaufen (`build_s3`): `unit_birkin_bahnsteig` OK, `unit_birkin_szenen` OK.
- Gruen im vollen Lauf u. a. alle Birkin-/G5-Tests (`unit_birkin_*`, `unit_r41_birkin_wand_*`, `probe_p2_birkin_g5`, `unit_g5_*`,
  `unit_r30_n_room5080_birkin_wand`, `integration_r35_zgirl_5090`) und die Raumlaeufe `integration_r37_elzaems_leon5080`, `_elza5081`,
  `_elza5091`, `unit_r41_form_emd`.
