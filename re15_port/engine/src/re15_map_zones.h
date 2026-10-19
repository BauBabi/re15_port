/* GENERIERT von tools/gen_map_zones.py - KARTEN-ZONEN.
 * Ein RDT-Raum ist nicht immer EIN Ort: 26 der 103 Basis-Raeume zerfallen in
 * mehrere zusammenhaengende Bereiche (ROOM1170 z.B. in zwei, verbunden durch
 * eine Selbst-Tuer). Jede Zone traegt ihr eigenes Karten-Rechteck und ihre
 * eigene Marker-Abbildung; die aktive Zone folgt der Spielerposition.
 * Herleitung + Verfahren: analysis/nutzer_batch_2026-08-30b/map-zonen.md */
static const re15_map_zone_t s_map_zones[] = {
    /* ⛔ ROOM1000 IST NICHT DIE HALLE (Nutzer 2026-09-13: "im ROOM 1000 wenn ich drin
     * bin ist nicht der Raum auf der Map gezeichnet, sondern ich lande irgendwo ganz
     * anders in der Lobby laut Map"). Der Raum ist DREI getrennte Bereiche in EINEM
     * RDT - ein Ostraum (Kameras 0-2) und ZWEI Toiletten (Kameras 3-5 / 6-8), die eine
     * volle Trennwand scheidet; alle drei haengen nur an ROOM1050.
     * Bis hier pinnte ZONE_FIX_TEILT beide bekannten Zonen auf Blatt 2 Rect 4 - und das
     * ist nachweislich ROOM1030s HALLENKACHEL (dessen ausgelieferte Zeile @0x800768c8).
     * Folge: das ganze Hallenrechteck faerbte sich CURRENT und der Marker stand mitten
     * in der Halle (gemessen (167,159), im Abzug F118 gezeichnet bei (166.3,158.3)).
     * Die Kachel, die ROOM1000 gehoeren muesste, GIBT ES NICHT: oestlich der
     * Flur-Ostwand malt Blatt 2 unterhalb y=88 keinen einzigen Texel (480 Texel
     * gezaehlt, alle in y59..88 = Rect 5). Deshalb rect 255 + drei SCHEMA-Zeichnungen
     * (s_map_synth) an den gemessenen Tuerhoehen - dieselbe Loesung wie fuer andere
     * ungemalte Raeume. Das ORIGINAL zeichnet hier ohnehin nichts: die
     * Massstabszeile @0x800768B0 (idx 0) ist der Stub {0,0,1,1}, der jede Weltlage auf
     * (0,0) abbildet. Dossier analysis/befunde_runde8_2026-09-13/karte-1000-1050.md.
     * Herleitung der Kaesten: §3.3 (eingemessen an den Tuerhoehen, nicht gewaehlt). */
    { 0x1000,  14350, -16850,  24800,   1650,  2, 255, 0,   0,     0,     0,     0,     0, 1, 1,   1, 0 },
    { 0x1001,  14350, -16850,  24800,   1650,  2, 255, 0,   0,     0,     0,     0,     0, 1, 1,   1, 0 },
    { 0x1000,  -7000,  -7150,   3600,   1400,  2, 255, 1,   1,     0,     0,     0,     0, 1, 1,   2, 0 },
    { 0x1001,  -7000,  -7150,   3600,   1400,  2, 255, 1,   1,     0,     0,     0,     0, 1, 1,   2, 0 },
    { 0x1000,  -7000,   1900,   3600,  10400,  2, 255, 2, 101,     0,     0,     0,     0, 1, 1,   3, 0 },
    { 0x1001,  -7000,   1900,   3600,  10400,  2, 255, 2, 101,     0,     0,     0,     0, 1, 1,   3, 0 },
    /* ⛔ ROOM1010: NICHT Rect 1 - das ist ROOM1020s Kachel! Die alte hergeleitete
     * Zeile (ox 84, flip 0,1) spiegelte den GANZEN Raum in die Zeichnung des
     * Nachbarn (gemessen: 276 Gitterpunkte, Deckung 1,00 auf Rect-1-Kunst), und
     * die EIGENEN Kacheln des Raums - Rect 8 (Nordkammer) und Rect 7 (Sued) -
     * gehoerten niemandem und blieben schwarz (Nutzer 2026-09-12: "in ROOM 1010
     * wird mir der Raum auf der Karte noch nicht gezeichnet"). Der 2-Trigger-Fit
     * konnte flip_x nicht sehen: beide Tueren liegen auf EINER Senkrechten
     * (lokal x=4650 -> Kartenspalte 164), der Spiegel pivotiert genau um die
     * Tuerwand. Anker der Korrektur: 1020s AUSGELIEFERTE Zeile -> Tueren
     * (164,82)/(163,106); SCA-Teilerwand voller Breite z1450..1550 = gemalte
     * Grenze y93/94; ox per Spiegelrelation 2*164-84 = 244. Der Raum hat laut
     * SCA ZWEI unverbundene Kammern, vom Kuenstler als ZWEI Kacheln gemalt ->
     * zwei Zonen (Dossier analysis/befunde_runde3_2026-09-12/karte-1010.md). */
    { 0x1010,  -4400,  -7600,   6600,   1550,  2,   8, 0,   2,   244,    20,  2287,  2319, 1, 1,   0, 0 },
    { 0x1011,  -4400,  -7600,   6600,   1550,  2,   8, 0,   2,   244,    20,  2287,  2319, 1, 1,   0, 0 },
    { 0x1010,  -4400,   1450,   6600,  10350,  2,   7, 1, 100,   244,    20,  2287,  2319, 1, 1,   0, 0 },
    { 0x1011,  -4400,   1450,   6600,  10350,  2,   7, 1, 100,   244,    20,  2287,  2319, 1, 1,   0, 0 },
    { 0x1020, -28874, -28532,  -1044,  -3990,  2,   1, 0,   3,   100,   136,  2287,  2287, 0, 0,   0, 0 },
    { 0x1021, -28874, -28532,  -1044,  -3990,  2,   1, 0,   3,   100,   136,  2287,  2287, 0, 0,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (Abnahme-Mangel M5): ROOM1030. AUSGELIEFERT @0x800768C8 =
     * 6c 00 c2 00 7c 09 b5 08 = (108,194,2428,2229). Gegen die eigene Kunst (MAP03.PIX r4 uv(184,56)) liegt sie
     * bis 5 px daneben - Waende ROOM1030.RDT Band 0 -> gemalte Linie mit der ausgelieferten Zeile: z -1784 @0x01A70
     * -> y130 (gemalt 125), -5212 @0x01A40 -> 137 (134), -10568 @0x01A10 -> 148 (144), -22771 @0x01AE8 -> 174 (173);
     * x -26732 @0x01A4C -> 120 (119), -19254 @0x01A34 -> 138 (136), 2398 @0x01A1C -> 188 (186). Folge: 63 von 2288
     * begehbaren Rasterpunkten (Westfluegel, z -4710) neben der Kachel. Eine Zeile mit DENSELBEN Offsets (108,194)
     * und den an den acht Waenden gemessenen Massstaeben trifft alle auf <= 1 px (G_karten_werkzeug/zeile_fit.py):
     * sx 2428 -> 2403, sy 2229 -> 2373. Beta -> Retail: das Original rechnet hier mit einem Massstab, der zur
     * eigenen Zeichnung nicht passt (unfertig); Ziel ist RE2, dessen Marker per Konstruktion auf der Kunst liegt
     * (FUN_8006e120, ein Massstab /450 @0x8006e1dc-0x8006e20c). PORT-KORREKTUR, Offsets unveraendert. */
    { 0x1030, -27992, -31110,   4560,     16,  2,   4, 0,   4,   108,   194,  2403,  2373, 0, 0,   0, 0 },
    { 0x1031, -27992, -31110,   4560,     16,  2,   4, 0,   4,   108,   194,  2403,  2373, 0, 0,   0, 0 },
    { 0x1040, -32762, -19208, -15590,  24856,  2,   2, 0,   5,    94,   193,  2080,  2320, 0, 0,   0, 0 },
    { 0x1041, -32762, -19208, -15590,  24856,  2,   2, 0,   5,    94,   193,  2080,  2320, 0, 0,   0, 0 },
    /* ROOM1050 -> Rect 0, spiegelverkehrt eingemessen 2026-09-12 (karte-1050.md;
     * Nutzer: "Ab 1050 nicht mehr [klasse]"). Anker = 1030s Original-Zeile
     * @0x800768c8 auf (187,153): ox=187+124=311, oy=153-94=59, flip(1,1).
     * Zeugen: Tuer->1030 trifft die gemalte Tuernische (188..192,147..151);
     * Tuer->10A0 (197,98) gegen 10A0s Ostwand-Marke (197,94) = 4 px.
     * VON HAND wie die 1010er-Zeilen - ein gen_map_zones-Regen wuerde die
     * Hand-Staende dieses Headers zerstoeren (Seeds stehen trotzdem im
     * Generator fuer einen kuenftig konsolidierten Regen). */
    /* ⛔ NACHGEEICHT 2026-09-13 (Nutzer: "im Raum 1050 haengt der Marker oben noch
     * etwas in der Luft in der Map, steht genau an die Wand gezeichnet zu sein").
     * Die alte Zeile (oy 59, sy 2229) war in der z-Achse um 7 px daneben: die EIGENE
     * Suedwand des Raums (Innenkante z=-23377) fiel auf Karte y=77, die gemalte
     * Nordwand der Kachel liegt aber bei y=69/70 - der Spieler, der an der Wand klebt,
     * stand 8 px im Raum. Vier unabhaengige Anker legen oy=51 / sy=2322 fest, und mit
     * ihnen treffen ALLE auf den Pixel: die beiden eigenen Waende und die beiden
     * GEMALTEN Tuersymbole (§4.2 des Dossiers). Der Marker des Nutzers wandert von
     * (190,78) auf (190,71) - direkt an die gemalte Wand. ox/sx bleiben (311/2428),
     * die waren schon exakt. */
    { 0x1050,  12700, -24350,  26100,  15805,  2,   0, 0,   6,   311,    51,  2428,  2322, 1, 1,   0, 0 },
    { 0x1051,  12700, -24350,  26100,  15805,  2,   0, 0,   6,   311,    51,  2428,  2322, 1, 1,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (Abnahme-Mangel M4): Treppenhaus ROOM1060 - die Vorlagen
     * map1F/2F_correct.png ruecken die Treppensymbole aus der Wand (1F x127 -> x122, x135 -> x133); die
     * Bbox-Streckung auf das NOMINALE Rechteck (24x24, gemalt 18x22) legte den Marker auf der Treppe 2..5 px
     * daneben (gemessen: STAIR-AOT (25150,22000) -> (127,144), Symbol (122,144)) und 4 von 155 begehbaren
     * Rasterpunkten neben die Kachel. Original-Zeile @0x800768b0 + 6*8 = @0x800768E0 ist ein Stub (Stage-1-
     * Index 6 = 0x1060>>4 & 0xF); ersetzt durch eine Zeile fuer die Formel FUN_800473f8 (@0x8004741c-0x80047528),
     * PORT-ERGAENZUNG wie 10C0/1120/10E0: Wandflaeche -> gemalte Linie (MAP03.PIX uv(168,16), Spiegel 1,1 wie
     * bisher): x 26900 @0x004B8 -> 119, 19800 @0x004D0 -> 136; z 18200 @0x004E8 -> 134, 26950 @0x004C4 -> 155
     * (Reste 0; s = am exakten Massstab 2^20*17/7100 bzw. 2^20*21/8750 gewaehlt, G39nb1/fit2.py).
     * Gegenproben: STAIR-AOT-Mitten (ROOM1060.RDT Aot_set sce 13) (25150,22000) @0x00DD6 -> (123,144) und
     * (21400,22850) @0x00DFE -> (132,146) liegen in den 5x5-Kaesten der Vorlagen-Symbole (122,144)/(133,146);
     * Tuer -> ROOM1040 (Mitte (27100,25400), Rechteck (26600,24300) 1000x2200) -> (118,152), Marke (119,151). */
    { 0x1060,  17800,  16200,  28900,  28950,  2,  10, 0,   7,   260,    14,  2511,  2516, 1, 1,   0, 0 },
    { 0x1061,  17800,  16200,  28900,  28950,  2,  10, 0,   7,   260,    14,  2511,  2516, 1, 1,   0, 0 },
    { 0x1070,  -1856,  -1568,  23074,  17224,  2,   3, 0,   8,    79,   205,  2229,  2088, 0, 0,   0, 0 },
    { 0x1071,  -1856,  -1568,  23074,  17224,  2,   3, 0,   8,    79,   205,  2229,  2088, 0, 0,   0, 0 },
    /* Runde 35 Spur G (Nutzer: "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player
     * Cursor nicht"; Dossier analysis/befunde_runde35/G_karte.md). GEMESSEN: die alte,
     * hergeleitete Zeile (78,214,2080,2320, ohne Spiegel) legte den Kabinen-Innenraum auf
     * y 143..152 - UNTER die gemalte Kabine (Suedwand y=143) -, die Klemmung liess davon
     * x 113..118 / y 144..146 uebrig. Die Massstabszeile @0x800768f0 ist der Stub {0,0,1,1}.
     * 180 Grad gedreht (flip 1,1) - Yaw-Stetigkeit der Tuer ROOM1040 @0x1096 (Spawn
     * Yaw 0x0400 = -z, man geht in 1040 nach +z hinein).
     * NACHBESSERUNG 1 (Abnahme 0: Marker wanderte mit 795/805 nur 2 px je Achse): MASSSTAB
     * AUS RE2 RETAIL, weil RE1.5 hier ein Stub ist (@0x800768f0 {0,0,1,1}, Beta->Retail).
     * RE2-Kartenzeichner FUN_8006e120: x = (x+28000)/450 @0x8006e1dc-0x8006e218 (magic
     * 0x91a2b3c5, sra 8), y = -(z+28000)/450 @0x8006e1f8-0x8006e268, EIN Massstab fuer alle
     * Raeume, Versatz je Raum (Eintrag +8/+10 @0x8006e2cc/0x8006e2e4), keine Klemmung (AddPrim
     * @0x8006e2f0). In Zeilenform sx = sy = 2^20/450 = 2330. Versatz (ox,oy) = PORT-WAHL aus der
     * Kunst: die sichtbare Ring-Mitte (mx-1,my-1, DATA/TEX.TIM @0x14910) legt die westliche
     * und noerdliche SCA-Wand (x -15750, z -4150) auf die gemalten Wandpixel x118/y134 (Kachel
     * uv(168,40), MAP03.PIX @0x1454). PROBE des Massstabs: die GEGENwaende (x -11550, z -50)
     * fallen dann ebenfalls auf die gemalten Waende x109/y143 - die Kunst ist im RE2-Massstab
     * gemalt. Spieler-Reichweite (Wand -468, debug.log) -> sichtbar x110..117 y135..142 = der
     * gemalte Innenraum, 7 px je Achse. Klemmfenster: re15_karte_fahrstuhl_fenster. */
    { 0x1080, -16750,  -5150, -10550,    950,  2,   9, 0,   9,   155,    73,  2330,  2330, 1, 1,   0, 0 },
    { 0x1081, -16750,  -5150, -10550,    950,  2,   9, 0,   9,   155,    73,  2330,  2330, 1, 1,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (M4): Treppenhaus ROOM10A0 wie ROOM1060 - Vorlage map1F_correct.png
     * Treppen x187 -> x183, x196 -> x194; Bbox-Streckung legte die STAIR-AOT (21450,21650) auf (187,100).
     * Original-Zeile @0x800768b0 + 10*8 = @0x80076900 ist ein Stub. Zeile aus den Waenden (MAP03.PIX uv(128,16),
     * Spiegel 0,0 wie bisher): x 19850 @0x005DC -> 180, 27000 @0x005E8 -> 197; z 26300 @0x005D0 -> 88,
     * 17550 @0x00630 -> 109 (Reste 0). Gegenproben: Aot_set sce 13 (21450,21650) @0x00E40 -> (184,99) im Kasten
     * von (183,100), (25350,22350) @0x00E68 -> (193,98) im Kasten von (194,99); Tuer -> ROOM1050 (Mitte
     * (27050,24900), Rechteck (26700,23900) 700x2000) -> (197,91), Marke (197,92). */
    { 0x10A0,  17850,  15550,  29100,  28300,  2,   6, 0,  10,    57,   228,  2493,  2517, 0, 0,   0, 0 },
    { 0x10A1,  17850,  15550,  29100,  28300,  2,   6, 0,  10,    57,   228,  2493,  2517, 0, 0,   0, 0 },
    /* ⛔ ROOM1090 FEHLTE GANZ AUF DER KARTE (Nutzer 2026-09-13: "Fuer ROOM 1090 fehlt noch
     * das Kartenstueck"). Die Ursache war keine fehlende Kunst: s_map_floors fuehrte vier
     * Etagenzeilen fuer 0x1090/0x1091, s_map_zones aber keine einzige - damit gab
     * zone_index_at -1 zurueck, und Marker, Besucht-Bit und Rechteck-Faerbung fielen
     * zusammen aus.
     *
     * DIE KACHEL GIBT ES, sie war nur falsch vergeben. Blatt 2 rect 5 (Schirm (180,59)
     * 48x32, uv(112,64)) traegt genau EIN gemaltes Tuerblatt, bei (191,66). Die Tuer
     * ROOM1050 -> ROOM1090 projiziert mit 1050s AUSGELIEFERTER Massstabszeile (ox 311,
     * oy 51, sx 2428, sy 2322, flip 1,1) durch die Original-Formel FUN_800473f8
     * (@0x8004741c-0x80047528) auf (191,70) - 4 px daneben, dieselbe Guete wie der schon
     * akzeptierte Anker 1050<->10A0 (Glyph (194,92) gegen (197,92)). Kein anderes
     * Rechteck von Blatt 2 meldet an (191,70) etwas.
     * Zweiter, unabhaengiger Beleg aus der KUNST: eine Schablonensuche der Streifenkacheln
     * im 1F-Grundriss legt rect5 bei (205,130) ab (853/853 Texel, 100,0 %);
     * ⛔ DATEINAME BERICHTIGT (2026-09-14): hier stand MAP02.PIX. Das ist das B2-Blatt.
     * Die Blaetter sind gegen die Dateinamen um eins versetzt - selbst an den Titeln
     * abgelesen: MAP01=B1, MAP02=B2, MAP03=1F, MAP04=2F, also Seite N = MAP0(N+1).
     * Seite 2 (1F) ist damit MAP03.PIX und Seite 3 (2F) ist MAP04.PIX. Die Zuordnung
     * der Zeilen unten ist davon NICHT betroffen - sie arbeiten mit Seitennummern -,
     * aber wer die Belege nachmisst, sucht sonst im falschen Bild;
     * seine Suedwand faellt dort auf dieselbe Linie wie die Nordwand von rect0 (ROOM1050).
     * Die beiden Raeume teilen sich im Grundriss EINE Wand, und das Tuerblatt steckt darin.
     *
     * ROOM10B0 kann dieselbe Kachel nicht erklaeren: seine einzigen Nachbarn sind ROOM1170
     * (Blatt 5) und ROOM1260 (Raumindex 38 >= 0x26, bekommt vom Stage-Init gar keine
     * Seite) - auf Blatt 2 hat er NULL Tueren. Seine Zuordnung auf rect 5 kam allein aus
     * der Flaechen-/Formkosten des Loesers, ohne jeden Tuer-Anker.
     * ⛔ WAS ROOM10B0 STATTDESSEN BEKOMMT: vorerst rect 255, also kein gemaltes Rechteck.
     * Blatt 2 haelt fuer ihn eine ZWOELFTE, tabellenlose Kachel bereit (uv(32,88), Schirm
     * (94,125)); nutzbar wird sie erst, wenn Blatt 2 - wie Blatt 3 heute - eine eigene
     * Ersatz-Seitentabelle in s_map_rectfix bekommt. Bis dahin bleiben Marker und
     * Besucht-Bit von 10B0 unveraendert, nur die (falsche) Faerbung entfaellt. */
    { 0x10B0, -25040, -21700,  19888,   9088,  2, 255, 0,  11,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x10B1, -25040, -21700,  19888,   9088,  2, 255, 0,  11,     0,     0,     0,     0, 0, 0,   0, 0 },
    /* ROOM1090, untere Ebene (Hinterhof mit dem verunglueckten Polizeitransporter).
     * Heimatblatt ist KEINE Wahl: der Stage-1-Karteninit FUN_8004b568 schickt Raumindex
     * (0x1090>>4)&0xFF = 9 ueber die Kette 0..11, deren Schwanz @0x8004b680 `j 0x8004b888`
     * mit `ori v0,zero,0x2` im Verzoegerungsschlitz steht - Seite 2. (Gegenprobe:
     * @0x8004b6f4/f8 gibt fuer 12..17 die 3.) Eine geeichte Massstabszeile hat der Raum
     * nicht - @0x800768b0 + 9*8 = @0x800768F8 ist der Stub {0,0,1,1}, mit dem die
     * Originalformel jede Weltlage auf (0,0) abbildet; es gilt also die Bbox-Streckung.
     * Weltbox = Huelle aller SCA-Zellen beider Ebenen (Baender 0/1/2 unten, 4/5/6 oben,
     * 61 Zellen, selbst geparst): x -17500..10285, z -18592..11754 - in z auf -18700
     * erweitert, weil der Tuer-Trigger nach ROOM1100 bei (-5820,-18690) liegt und damit
     * 98 Einheiten hinter der Zellenhuelle. Dort STEHT der Spieler, wenn er die Tuer
     * nimmt; ohne die Erweiterung faellt er in diesem Augenblick aus seiner eigenen Zone.
     * ⛔ ZONEN-NUMMER 22, NICHT 37. Die 37 war der erste Griff und ist FALSCH: sie gehoert
     * schon ROOM2000 (Blatt 6). Die zid ist das Besucht-Bit - zwei Orte auf derselben
     * Nummer teilen es sich. Gemessen hat das unit_map_marke_haengt_an sofort gemeldet:
     * mit 37 schwebten auf Blatt 6 fuenf Tuermarken frei im Blau (vorher null). 22 ist die
     * einzige Luecke im dicht belegten Bereich 0..101. */
    /* Runde 39 Spur G (Nutzer 2026-10-17: "In ROOM 1090 ist Leons Positionsmarker der Map ausserhalb
     * des aktuellen Raums"). GEMESSEN (probe_r39_karten markerflaeche, Dossier G_karten.md P2): die
     * Bbox-Streckung bildete die Huelle BEIDER Ebenen (x -17500..10285, z -18700..11754) auf das Rechteck
     * ab - die obere Ebene landete gestaucht und nach unten versetzt (Nutzerpunkt (-12528,-9000,959) ->
     * (187,84), Index 0 = neben dem gemalten Gamma), 160 von 1225 begehbaren Rasterpunkten ausserhalb.
     * Die Original-Zeile @0x800768F8 (Index 9) ist der Stub 00 00 00 00 01 00 01 00; ersetzt wird sie
     * hier je Ebene durch eine Zeile fuer DIESELBE Formel FUN_800473f8 (@0x8004741c-0x80047528,
     * re15_map_zone_marker), PORT-ERGAENZUNG wie die ausgelieferten Zeilen der Nachbarn: jede Weltwand
     * der begehbaren Flaeche faellt auf ihre gemalte Wandlinie (Kleinste Quadrate, Werkzeug
     * G_karten_werkzeug/zeile_fit.py; Wand = SCA-Zelle des Bands, Datei-Offset ROOM1090.RDT).
     * HOF (Band 1, Blatt 2 rect 5, MAP03.PIX uv(112,64)): x -14546 (@0x01E84) -> 180, -4414 (@0x01EC0)
     * -> 203, 3972 (@0x01E00, Diagonale Typ 4) -> 222, Reste 0; z 7720 (@0x01DF4) -> 59, 3800 (@0x01E30)
     * -> 69, -4214 (@0x01F5C) -> 88, Reste 0/-1/0. Probe: die Tuer -> ROOM1050 (Mitte (-10080,3060))
     * faellt auf (190,70) = das gemalte Tuerblatt / die Marke (191,69). */
    { 0x1090, -17500, -18700,  10285,  11754,  2,   5, 0,  22,   140,   155,  2376,  2535, 0, 0,   0, 0 },
    { 0x1091, -17500, -18700,  10285,  11754,  2,   5, 0,  22,   140,   155,  2376,  2535, 0, 0,   0, 0 },
    /* Runde 39 Spur G ("natuerlich muss der Spielermarker da entsprechend passen"): GEMESSEN (probe_r39_karten
     * markerflaeche, Dossier G_karten.md P1-M): die Bbox-Streckung bildete den Raum auf das NOMINALE Rechteck 40x40
     * ab, gemalt sind nur x 102..135 - 123 von 999 begehbaren Rasterpunkten standen rechts neben der Kachel (x 136..138).
     * Zeile fuer die Formel FUN_800473f8 (@0x8004741c-0x80047528) wie ROOM1090, Spiegel 1,1 wie bisher; jede Weltwand
     * (ROOM10C0.RDT SCA, Band 0) auf ihre gemalte Linie (MAP04.PIX uv(128,16), s_map_rectfix[0]): x 6750 @0x003FC -> 102,
     * -7600 @0x003F0 -> 135, 3450 @0x0039C -> 109, -600 @0x003A8 -> 118 (Reste 0,0,0,1); z -6250 @0x003B4 -> 116,
     * 1750 @0x0039C -> 134, 6400 @0x0039C -> 143, 11100 @0x00384 -> 155 (Reste 0,0,1,0). */
    { 0x10C0,  -9600,  -8250,   8750,  13100,  3,   0, 0,  12,   191,    59,  2415,  2324, 1, 1,   0, 0 },
    { 0x10C1,  -9600,  -8250,   8750,  13100,  3,   0, 0,  12,   191,    59,  2415,  2324, 1, 1,   0, 0 },
    { 0x10D0, -21495, -10512,  10650,  30850,  3,   3, 0,  13,     0,     0,     0,     0, 1, 1,   0, 0 },
    { 0x10D1, -21495, -10512,  10650,  30850,  3,   3, 0,  13,     0,     0,     0,     0, 1, 1,   0, 0 },
    /* Runde 39 Spur G: wie ROOM10C0 - vorher 26 von 1199 Rasterpunkten neben der Kachel (2F rect 8 (102,76,48,48),
     * MAP04.PIX uv(160,80), gemalt x 102..147, Ecke rechts unten x 136..147 y 105..116 frei). Waende ROOM10E0.RDT Band 0
     * -> Linie (Spiegel 1,1): x 9000 @0x006A4 -> 102, -10100 @0x0065C -> 147, -5000 @0x00674 -> 135 (Reste 0);
     * z -7700 @0x00698 -> 76, 4200 @0x00668 -> 105, 9200 @0x006EC -> 116 (Reste 0,-1,0). */
    { 0x10E0, -12100,  -9700,  11050,  11250,  3,   8, 0,  14,   199,    18,  2468,  2492, 1, 1,   0, 0 },
    { 0x10E1, -12100,  -9700,  11050,  11250,  3,   8, 0,  14,   199,    18,  2468,  2492, 1, 1,   0, 0 },
    { 0x10F0,  -9800,  -5400,  11300,  16800,  3,   9, 0,  15,     0,     0,     0,     0, 1, 1,   0, 0 },
    { 0x10F1,  -9800,  -5400,  11300,  16800,  3,   9, 0,  15,     0,     0,     0,     0, 1, 1,   0, 0 },
    { 0x1100, -29200, -29100, -10500,  -7400,  3,   6, 0,  16,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x1101, -29200, -29100, -10500,  -7400,  3,   6, 0,  16,     0,     0,     0,     0, 0, 0,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (M5): ROOM1110, Original-Zeile @0x80076938 = Stub 00 00 00 00 01 00 01 00.
     * Die Bbox-Streckung legte den Tuergang nach ROOM1100 (Welt x -3250..-950, z -6650..-4000) auf x187 = neben die
     * Ostwand x186 (8 von 1237 Rasterpunkten). Zeile aus den Waenden des Evidence-Raums (MAP04.PIX s_map_rectfix[5]
     * uv(184,48), Spiegel 1,1 wie bisher): x 12650 @0x00734 -> 146, -3250 @0x0071C -> 186; z -6650 @0x00788 -> 114,
     * 5100 @0x00728 -> 144 (Reste 0). Gegenproben: Ostwand der kleinen Raeume x -13850 @0x007A0 -> 212 = gemalte
     * Ostlinie; der Tuergang (-2650,-6150..-4050) -> (184,115..120) = genau der gemalte Tuerschwenk x181..186
     * y115..120 der Kachel (Lage UND Spiegel bestaetigt). */
    { 0x1110, -15850,  -7650,  14950,   7600,  3,   5, 0,  17,   258,    49,  2638,  2677, 1, 1,   0, 0 },
    { 0x1111, -15850,  -7650,  14950,   7600,  3,   5, 0,  17,   258,    49,  2638,  2677, 1, 1,   0, 0 },
    /* Runde 39 Spur G: wie ROOM10C0 (dieselbe Kachel-Form auf 3F, MAP05.PIX uv(128,16) = rect 5 (120,119,40,40), gemalt
     * x 120..153): vorher 77 von 1080 Rasterpunkten neben der Kachel. Waende ROOM1120.RDT Band 0 -> Linie: x 6650
     * @0x00338 -> 120, -8500 @0x00350 -> 153, 3500 @0x00320 -> 127, -700 @0x00314 -> 136 (Reste 0); z -5800 @0x00344 ->
     * 119, 1650 @0x00320 -> 137, 6400 @0x00320 -> 146, 11200 @0x0032C -> 158 (Reste 0,-1,1,0). */
    { 0x1120, -10550,  -7800,   8650,  13200,  4,   5, 0,  18,   204,    60,  2277,  2372, 1, 1,   0, 0 },
    { 0x1121, -10550,  -7800,   8650,  13200,  4,   5, 0,  18,   204,    60,  2277,  2372, 1, 1,   0, 0 },
    { 0x1130,  -8650, -18400,   6000,  19750,  4,   4, 0,  19,    90,   192,  2296,  2312, 0, 0,   0, 0 },
    { 0x1131,  -8650, -18400,   6000,  19750,  4,   4, 0,  19,    90,   192,  2296,  2312, 0, 0,   0, 0 },
    { 0x1140, -10600, -25600,  16700,   1850,  4,   6, 0,  20,   108,   183,  2296,  2312, 0, 0,   0, 0 },
    { 0x1141, -10600, -25600,  16700,   1850,  4,   6, 0,  20,   108,   183,  2296,  2312, 0, 0,   0, 0 },
    { 0x1150, -28658, -27884, -14330,  -9560,  4,   2, 0,  21,   111,   130,  2296,  2312, 0, 0,   0, 0 },
    { 0x1151, -28658, -27884, -14330,  -9560,  4,   2, 0,  21,   111,   130,  2296,  2312, 0, 0,   0, 0 },
    { 0x1170, -13430, -10910,  15658,  17746,  5,   1, 0,  23,   100,   206,  2280,  2268, 0, 0,   0, 0 },
    { 0x1171, -13430, -10910,  15658,  17746,  5,   1, 0,  23,   100,   206,  2280,  2268, 0, 0,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (M5): ROOM1170 Zone 1 (Treppenaufgang zum Dach). Die ausgelieferte Zeile
     * @0x80076968 (100,206,2280,2268) gehoert Zone 0 (Hof); fuer Zone 1 legte die Bbox-Streckung den Westarm 5..10 px
     * neben die gemalte L-Form (3F: 51 von 311 Rasterpunkten neben der Kachel, Dach 12 von 186). Die L-Form passt mit
     * Spiegel 0,0 (NICHT gedreht): Band 0 = Kopfband (x -26926..-20000) + Arm nach Sueden (x -24183..-20000), Band 4 =
     * Streifen nach Osten (x -20471..-10432, z -24434..-27134) = gemalter Unterarm. Zeile aus den Waenden ROOM1170.RDT:
     * x -26926 @0x00B70, -24183 @0x00B94, -20000 @0x00B64, -20471 @0x00BF4, -10432 @0x00BD0; z -18272 @0x00B58,
     * -24434 @0x00BE8, -26881 @0x00B7C, -27134 @0x00BDC -> gemalte Linien (Reste <= 1). Dach (Blatt 5 rect 0 (140,80),
     * MAP06.PIX uv(192,16)): x -> 140/147/157/157/181, z -> 80/94/101/101. */
    { 0x1170, -28900, -28841,  -8784, -17070,  5,   0, 1,  24,   128,   113,  2597,  2511, 0, 0,   0, 0 },
    { 0x1171, -28900, -28841,  -8784, -17070,  5,   0, 1,  24,   128,   113,  2597,  2511, 0, 0,   0, 0 },
    /* Runde 35 Spur G (Nutzer: "In ROOM 1230 bekomme ich die Map von ROOM 11E0").
     * ROOM1230 IST ROOM1180 (Variante desselben Ortes): alle Tuer-Datensaetze identisch
     * (1180 @0x9EE..0xA94 == 1230 @0xAEE..0xBC4), gleiche SCA-Huelle; der Seiten-Setzer
     * @0x8004b568 schickt 1230 (Index 35, @0x8004b884) aber auf Blatt 1 = B2 - dort hat
     * der Ort kein Rechteck (Stub-Zeile @0x800769c8). Beide gehoeren auf Blatt 0 (B1)
     * **rect 0**: dessen 5 gemalte Nischen sind die 5 Tueren des Gangs (1190-Haupt, 1190-Ost,
     * 11B0, 10A0, 11D0 - G_karte.md B4). rect 6 (bisher) ist ein Kasten ohne Nische = der
     * B1-Absatz von ROOM10A0 (Gastzeile unten).
     * Die Kachel ist SCHEMATISCH (der Gang als Ring; Affin-Fit der Tueren: Rest bis 17 px),
     * deshalb FUENF Abschnitte, jeder auf seinen gemalten Streifen gestreckt (PORT-WAHL,
     * Rechnung analysis/befunde_runde35/G_karte_zeilen.py). Kaesten an gemeinsamen Kanten
     * um ZONE_SLACK (1500) eingezogen, damit kein kleinerer Nachbar ueber die Kante greift.
     *   idx0 Laengsgang Sued -> Streifen x160..167 y69..104   idx1 Zweig+Bein -> y61..75
     *   idx2 Versatz (10A0)  -> x145..167 y105..112           idx3 Suedgang -> Balken y116..123
     *   idx4 Laengsgang Nord (11B0) -> x160..167 y61..68                                      */
    /* ==== Runde 39 Spur G: B1 (Blatt 0) NEU als Schema-Zeichnung aus Kollision + Tueren ====================
     * Nutzer 2026-10-17: "Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da
     * passt nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu der Karte."
     * GEMESSEN (Dossier analysis/befunde_runde39/G_karten.md B1): die gemalten Kacheln von Blatt 0 (MAP01.PIX,
     * Liste @0x800762a0) sind ein SCHEMA - der Gang als Ring, Abschnitte mit sx/sy bis 4950/408 auf Streifen
     * gestreckt (Runde 35), ROOM1160 (Zwinger ohne Strom) ganz ohne Zeile, ROOM11C0 (Ring um eine Insel) als
     * Rechteck. Das Original selbst zeigt hier keinen Marker: die Massstabszeilen von 1180/11D0/1230 sind der Stub
     * {0,0,1,1} (@0x80076970 / @0x80076998 / @0x800769c8), 11C0 = (174,196,1,1) @0x80076990.
     * JETZT (PORT-ERGAENZUNG, Nutzer-Modell "Kollisionsboxen pro Raum, alle Tueren und wo sie hinfuehren, alle
     * Treppen ... der naechste Raum schliesst EXAKT an die Wand an", Memory reai-v2-zu-kompliziert):
     *  - jeder Ort = wenige Rechtecke ueber seiner begehbaren Flaeche, gemessen mit dem ECHTEN Kollisionsloeser
     *    (probe_r39_karten flut, Port von FUN_8003b0a4), Zellen in s_map_synth_cells (Index 3..20);
     *  - Lage je Tuerpaar: Wand auf Wand, Tuermitten laengs der Wand deckungsgleich (alle Paare gegenlaeufig, keine
     *    Drehung); 11C0 ohne Gang-Tuer mit der Westwand auf der von 11B0 - wie im Original r3/r4 beide x 168;
     *  - EIN Massstab fuer das Blatt: RE2 Retail FUN_8006e120 x = (x+28000)/450 (`lui v0,0x91a2` @0x8006e1dc,
     *    `ori v0,v0,0xb3c5` @0x8006e1e8, `sra v1,v1,8` @0x8006e20c, z negiert `subu v0,zero,v0` @0x8006e268) als
     *    Affine 65536/450 -> 146; der Marker rechnet mit DERSELBEN Affine (re15_map_zone_abbildung), also steht er
     *    in jeder begehbaren Lage in der Zeichnung seines Orts (Selbstprobe b1_plan.py: 0 von 23699 Rasterpunkten daneben);
     *  - Wandlinien zweier Orte bis 3 px Abstand werden EINE Linie ("Keine Doppelwaende").
     * Rechnung: analysis/befunde_runde39/G_karten_werkzeug/b1_plan.py (Lagen, Zellen, Affinen, Marken).
     * Weltboxen = Zonen-Zugehoerigkeit (je Raum EINE Zone, ausser 1190 Halle/Ostkammer). */
    /* Gang 1180 = 1230 (Varianten, Runde 35): EINE Zone statt fuenf gestreckter Abschnitte; Weltbox = Huelle der
     * begehbaren Flaeche (Flut r 0: x -9492..5508, z -16377..31023, +-50 Rasterhalbschritt). Die Zusatzbits der
     * frueheren Abschnitte 2..4 (s_zone_zusatzbit) bleiben reserviert und ungenutzt. */
    { 0x1180,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1181,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1230,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1231,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    /* Runde 36 Spur M (Nutzer: "Bei ROOM 1090 auf der Map, gibt es 2 Tueren, eine am Ende des
     * Ganges, wo die Hunde durch die Luke kommen, und eine wo man die Weste bekommt. Das muss als 2
     * separate Bereich in der Map eingeblendet sein und sauber getrennt." - gemessen ist das ROOM1190:
     * Hunde Sce_em_set Typ 0x20 @0x02900.., Luke Obj 4 @0x02A06 (sub13), Weste sub14 Aot slot 16
     * @0x02D90 -> sub15 Set(3,0x75) @0x02DD0; ROOM1090.RDT hat weder Hunde noch Weste).
     * ROOM1190 sind ZWEI getrennte Bereiche (Flutfuellung im Port-Kollisionsmodell, Dossier R3):
     * Haupthalle (Tuer slot 4 @0x028B2, Spawn aus dem Gang (3174,-11182)) und Ostkammer (Tuer slot 6
     * @0x02B1C, Spawn aus ROOM1230 slot 4 @0x00CE6 (19515,-12109)) - nicht verbunden. Vorher EINE
     * Zone auf rect 2: beide Bereiche leuchteten im selben 16x40-Kasten.
     *   idx0 Haupthalle -> Blatt 0 rect 1 (76,76,72,48): die AUSGELIEFERTE Zeile @0x80076978
     *        (75,127,1920,2301) projiziert sie dorthin (Innenwaende -> x89..142 y79..115), und die
     *        Gang-Tuer (Nische (137..141,71..75) der Gangkachel) liegt auf ihrer Oberkante y76.
     *        Kasten = SCA-Innenkanten: x -24536 (@0x1C70) .. 4624 (@0x1C34), z -26498 (@0x1C7C)
     *        .. -10280 (@0x1C28); Zeile unveraendert.
     *   idx1 Ostkammer -> Blatt 0 rect 2 (144,69,16,40): einzige Kachel mit Tuernische oben rechts
     *        (MAP01.PIX uv(160..164,17..21) = Schirm (152..156,70..74)) - die Tuer liegt in der
     *        Nordwand rechts (18200..20800). Ohne Zeile (Bbox-Streckung, PORT-ERGAENZUNG wie alle
     *        Stub-Raeume); Kasten = SCA-Innenkanten x 13998 (@0x1D00) .. 21360 (@0x1CE8), z -25472
     *        (@0x1D24) .. -11432 (@0x1CDC). zid 110 = naechste freie (0..109 belegt), Besucht-Bit
     *        2n+1 des Raums (zone_bit, idx 1). */
    /* Runde 39 Spur G: Halle -> Schema-Zeichnung 5, Ostkammer -> 6 (B1-Kopf oben; die Weltboxen bleiben). Die
     * ausgelieferte Zeile @0x80076978 (75,127,1920,2301) gilt fuer die gemalte Kachel r1, die nicht mehr gezeichnet
     * wird; Halle und Ostkammer liegen im 1190-Rahmen 8286/3900 Einheiten anders zueinander als am Gang (zwei
     * getrennte Modelle in EINEM Raum) - deshalb je eigene Affine aus der eigenen Tuer. */
    { 0x1190, -24536, -26498,   4624, -10280,  0, 255, 0,  26,     0,     0,     0,     0, 0, 0,   5, 0 },
    { 0x1191, -24536, -26498,   4624, -10280,  0, 255, 0,  26,     0,     0,     0,     0, 0, 0,   5, 0 },
    { 0x1190,  13998, -25472,  21360, -11432,  0, 255, 1, 110,     0,     0,     0,     0, 0, 0,   6, 0 },
    { 0x1191,  13998, -25472,  21360, -11432,  0, 255, 1, 110,     0,     0,     0,     0, 0, 0,   6, 0 },
    /* Runde 36 Spur M: ROOM11A0 VERLAESST Blatt 0 rect 1 (dort liegt jetzt die 1190-Haupthalle).
     * Seine ausgelieferte Zeile @0x80076980 (138,113,1776,2048) projiziert ihn auf y -2..62 =
     * UEBER alle sieben B1-Rechtecke, und er hat keine Tuer zu einem B1-Raum (Tueren: 2070, 3000,
     * 20A0, Selbst-Tueren; ROOM11A0.RDT). Seine Kachel ist Blatt 6 ("DRAINS B2") rect 2, wo er
     * schon als Gast-Zeile stand - die wird hiermit die Hauptzeile (die Gast-Zeilen unten entfallen,
     * die Etagenzeile Band 3 zeigt jetzt ebenfalls auf Blatt 6 rect 2). */
    { 0x11A0, -22175,  -5850,  31000,  26950,  6,   2, 0,  27,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x11A1, -22175,  -5850,  31000,  26950,  6,   2, 0,  27,     0,     0,     0,     0, 0, 0,   0, 0 },
    /* Runde 39 Spur G: 11B0 -> Schema 7 (Streifen + Werkstatt als EIN Raum: die Trennwand @0x00EC8 wird nach
     * "Reservestrom an" per Sca_id_set 37 03 0c 00 @0x0118C nicht-fest), 11C0 -> Schema 8 (Ring um die Insel).
     * Die gemalten r3/r4 und ihre Zeilen (@0x80076988 bzw. 11B0s Zeile fuer 11C0) entfallen. */
    { 0x11B0, -29100, -29100,   3800, -13200,  0, 255, 0,  28,     0,     0,     0,     0, 0, 0,   7, 0 },
    { 0x11B1, -29100, -29100,   3800, -13200,  0, 255, 0,  28,     0,     0,     0,     0, 0, 0,   7, 0 },
    { 0x11C0, -32200, -31100,  21640,  25999,  0, 255, 0,  29,     0,     0,     0,     0, 0, 0,   8, 0 },
    { 0x11C1, -32200, -31100,  21640,  25999,  0, 255, 0,  29,     0,     0,     0,     0, 0, 0,   8, 0 },
    /* Runde 36 Spur M (Nutzer: "In ROOM 11D0 lande ich auf der Map direkt in der Mitte, sobald
     * ich durch die Tuer gehe, statt am Tuereingang zu stehen"). GEMESSEN: Ankunft (-300,-17400)
     * (Spawn der Gang-Tuer, ROOM1180/1230 slot 0 @0x009EE/@0x00AEE) -> Marker (129,151) = Mitte
     * von rect 5, auf der Gang-Seite stand er an der Tuer (130,123). URSACHE: der Kasten war die
     * SCA-Huelle bis z -10950 - der Zwinger-Block reicht 5500 Einheiten NOERDLICH ueber die
     * Tuerwand hinaus, die Bbox-Streckung legte die Tuer deshalb in die Mitte. Die Kachel
     * (MAP01.PIX uv(128,56), ein leerer Kasten, Waende v56 / v104 = Schirm y124 / y172, Spalten
     * u128 / u181 = x96 / x149) zeigt die Gang-Tuer als OBERKANTE. Die Stadt-Zeile @0x80076998 ist
     * der Stub {0,0,1,1} (Original zeigt hier gar keinen Marker); PORT-ERGAENZUNG wie alle
     * Stub-Raeume: Kasten so, dass die WAENDE auf die gemalten Waende fallen -
     *   z oben  = -16500 = Innenkante der Tuerwand @0x08CC (x -2200..7750, z -16500..-15500)
     *   z unten = -24050 = Aussenkante der Suedwand @0x08A8 (z -24050..-23050): die Kachel
     *             bemalt nur 49 der 56 Zeilen, damit faellt die Innenkante -23050 genau auf y172
     *   x       = -11950..7750 unveraendert (Huelle): Ostwand-Innenkante 6750 (@0x089C) -> x149.
     * Wirkung: Ankunft -> (129,130) direkt unter der Tuermarke (129,124); 1260-Tuer x137 wie
     * ihre Marke (137,172). Der Zwinger-Gang noerdlich der Tuerwand klemmt an die Oberkante. */
    /* Runde 39 Spur G: 11D0 -> Schema 9 (Weltbox = Huelle der begehbaren Flaeche; die Streckungs-Box von Runde 36
     * entfaellt mit der Streckung). NEU ROOM1160: derselbe Zwinger OHNE Strom - ROOM1180 main00 waehlt fuer
     * Tuer-Slot 0 per Ck(4,243) @0x009EA zwischen Ziel 0x16 (= 1160, Strom aus, @0x009EE) und 0x1d (= 11D0,
     * @0x00A12); beide Tueren -> Gang liegen auf (-1300,-17000,2000,1000) (1160 @0x007C6, 11D0 @0x011E2) = derselbe
     * Rahmen. Bisher hatte 1160 KEINE Zeile: weder Marker noch Zeichnung, solange der Strom aus ist. */
    { 0x11D0, -11500, -23100,   5000, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x11D1, -11500, -23100,   5000, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    /* Runde 39 Spur G: ROOM11D0 Zone 1 = Kammer der Kanal-Leiter (begehbar x 1550..6050 z -23050..-19450, nur aus
     * ROOM1260 erreichbar: 1260 slot 3 -> Spawn (3900,-22000); gegen den Zwinger dicht durch @0x008C0 (x 1050..1550)
     * und @0x008B4 (z -19350..-18850)). zid 111 = naechste freie (Hoechstwert vorher 110). Weltbox um ZONE_SLACK
     * (1500, re15_map_zones.c) gegen den Zwinger eingezogen: x ab 2101 (Zwinger-Mitten <= 600), z bis -19901
     * (Zwinger-Mitten >= -18400) - sonst griffe der kleinere Kasten mit seinem Rand in den Zwinger. */
    { 0x11D0,   2101, -23100,   6100, -19901,  0, 255, 1, 111,     0,     0,     0,     0, 0, 0,  11, 0 },
    { 0x11D1,   2101, -23100,   6100, -19901,  0, 255, 1, 111,     0,     0,     0,     0, 0, 0,  11, 0 },
    { 0x1160,  -8550, -23100,   5050, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x1161,  -8550, -23100,   5050, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x11E0, -32312, -30800,  31100,  31150,  1,   0, 0,  31,    91,   157,  2304,  2240, 0, 0,   0, 0 },
    { 0x11E1, -32312, -30800,  31100,  31150,  1,   0, 0,  31,    91,   157,  2304,  2240, 0, 0,   0, 0 },
    /* Runde 35 Spur G (Nutzer: "In ROOM 11F0 / ROOM 1200 taucht nicht auf der Karte auf,
     * wenn man drin ist"; G_karte.md B4). GEMESSEN: 11E0, 11F0 und 1200 trugen ALLE rect 0
     * (die Garagen-Kachel von 11E0) - die eigenen Kacheln blieben ungezeichnet.
     * 11F0 -> rect 1 (100,101,40,56): die hergeleitete Zeile trifft dessen L-Form Pixel fuer
     * Pixel (Wand z2150 -> y107, Kunst 108; z5700 -> y100, Kunst 101; Ecke x1900 -> 109,
     * Kunst 108). 1200 -> rect 2 (141,101,32,40): eigene Zeile @0x800769b0 projiziert die
     * Huelle auf (137..176,99..143), die Tuer @0x7BA auf die gemalte Nische (165,139). */
    { 0x11F0,  -3100, -17500,  12950,   6700,  1,   1, 0,  32,    35,   180,  2304,  2240, 0, 0,   0, 0 },
    { 0x11F1,  -3100, -17500,  12950,   6700,  1,   1, 0,  32,    35,   180,  2304,  2240, 0, 0,   0, 0 },
    { 0x1200, -29300, -28771, -16330,  -8650,  1,   2, 0,  33,   129,   150,  3168,  2305, 0, 0,   0, 0 },
    { 0x1201, -29300, -28771, -16330,  -8650,  1,   2, 0,  33,   129,   150,  3168,  2305, 0, 0,   0, 0 },
    /* Runde 35 Spur G (Nutzer: "In ROOM 1210 ist der Korridor falsch und so gut wie alle
     * Tueren fehlen"). GEMESSEN: 1210 lag auf rect 4 (eine 16x24-ZELLE), 1220 auf rect 3
     * (den T-Korridor) - vertauscht. 1210 -> rect 3 (eigene Zeile @0x800769b8: Huelle
     * (184..230,68..133), Tueren je 1 px an den gemalten Nischen). 1220 (Stub @0x800769c0)
     * teilt den Weltrahmen mit 1210 (Tuerpaare: Tuer-Rechteck der einen Seite liegt auf dem
     * Spawn der anderen) -> die 1210-Zeile gilt; die 5 Zellen sind 5 Orte = rect 4/6/5/7/8.
     * KAESTEN AUS DER SCA von ROOM1220.RDT (Block @0x530, Band-0-Eintraege ab @0x548, 12 B
     * {u16 w, u16 d, s16 x, s16 z, ...}; Nachbesserung 1, Abnahme 0 G1). Innenflaechen:
     *   x West -27611 = @0x05C0 (x -28186..-27611) | x -21836 = @0x05CC/@0x05D8 (x -21836..)
     *   x Ost  -17075 = @0x0590 (x -17275..-17075) | x -11311 = @0x065C (x -11311..-10736)
     *   z Nord -4500 = @0x0560 (West, z -4500..-4300) | -4525 = @0x0578 (Ost, z -4525..-4300)
     *   Teiler 1/2 z -11925..-11725 = @0x062C (West) / @0x0668 (Ost)
     *   Teiler 2/3 z -19875..-19675 = @0x05B4 (West) / @0x0698 (Ost)
     *   z Sued -26750 = @0x05A8 (z -28750..-26750)
     * An den GEMEINSAMEN Teilern ist jeder Kasten um ZONE_SLACK = 1500 eingezogen (Port-
     * Toleranz des Zonentests, re15_map_zones.c:39 - sonst ueberlappten sich die Zellen):
     *   idx0 z -10225 = -11725 + 1500 | idx1 z -10225 | idx2 z -13425 = -11925 - 1500, unten
     *   -19675 (Teilerflaeche, die SW-Kammer z < -19875 hat keine Tuer und ist nicht gemalt) |
     *   idx3 z -18175 = -19675 + 1500 .. -13425 | idx4 z -26750 .. -21375 = -19875 - 1500. */
    { 0x1210, -28928, -28640,  -9776,   1366,  1,   3, 0,  34,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1211, -28928, -28640,  -9776,   1366,  1,   3, 0,  34,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 2 (Abnahme 1, Mangel N1): ROOM1210 ZONE 1 = die SUEDWEST-KAMMER (dritte
     * West-Zelle, offen zum Flur). Begehbar im Original: zwei eigene Kameraschnitte 6/7 (RVD ROOM1210.RDT @0x002DC
     * Schnitt 5->6 in der Oeffnung, @0x002F0/@0x00318/@0x0032C), keine Tuer (main00 Door_aot_set nur @0x01CC6..
     * @0x01D66), kein Sca_id_set in Leons Skript. Weder die Kachel MAP02.PIX r3 noch die Zeile @0x800769B8 noch
     * mapB2_correct.png zeichnen sie (RE1.5 unfertig) - Beta -> Retail: gezeichnet als Schema (synth 12), Marker mit
     * DERSELBEN Zeile wie der Flur (177,140,2496,2250) = @0x800769B8 - ein Raum, ein Rahmen, kein Sprung.
     * WELTBOX = Innenflaeche, um ZONE_SLACK (1500) eingezogen (wie 11D0 Zone 1): West x -27358 = Innenseite @0x01BEC,
     * Nord z -19898 = @0x01BF8, Sued z -26696 = @0x01BE0. Ost x -22128: dort wechselt die Zeile von Spalte 200 auf
     * die gemalte Flur-Westwand x201 (FUN_800473f8: px(-22128) = 200, px(-22127) = 201) - die Kammer endet, wo ihre
     * Zeichnung endet; der Durchgang x201 gehoert zum Flur (Zone 0). Elza 0x1211: dieselbe Kammer; dort schliesst
     * sub00 die Oeffnung, bis (4,0xEC) und (4,0xFC) gesetzt sind (ROOM1211.RDT Sca_id_set @0x01E0E/@0x01E5A auf
     * Zelle 15 = @0x01C34 x -22224..-21292 z -22928..-20330, u0 00 -> ff). */
    { 0x1210, -25858, -25196, -23628, -21398,  1, 255, 1, 112,   177,   140,  2496,  2250, 0, 0,  12, 0 },
    { 0x1211, -25858, -25196, -23628, -21398,  1, 255, 1, 112,   177,   140,  2496,  2250, 0, 0,  12, 0 },
    { 0x1220, -27611, -10225, -21836,  -4500,  1,   4, 0,  35,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1221, -27611, -10225, -21836,  -4500,  1,   4, 0,  35,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1220, -17075, -10225, -11311,  -4525,  1,   6, 1, 102,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1221, -17075, -10225, -11311,  -4525,  1,   6, 1, 102,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1220, -27611, -19675, -21836, -13425,  1,   5, 2, 103,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1221, -27611, -19675, -21836, -13425,  1,   5, 2, 103,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1220, -17075, -18175, -11311, -13425,  1,   7, 3, 104,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1221, -17075, -18175, -11311, -13425,  1,   7, 3, 104,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1220, -17075, -26750, -11311, -21375,  1,   8, 4, 105,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1221, -17075, -26750, -11311, -21375,  1,   8, 4, 105,   177,   140,  2496,  2250, 0, 0,   0, 0 },
    { 0x1250, -21850, -25300,  15700,   9750,  1,   0, 0,  36,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x1251, -21850, -25300,  15700,   9750,  1,   0, 0,  36,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x2000, -29000, -29000,   7450,  12800,  6,   0, 0,  37,    80,   196,  2340,  2308, 0, 0,   0, 0 },
    { 0x2001, -29000, -29000,   7450,  12800,  6,   0, 0,  37,    80,   196,  2340,  2308, 0, 0,   0, 0 },
    { 0x2010,  -7300,  -9550,   7400,   7100,  6,   0, 0,  38,    31,   191,  2340,  2308, 0, 0,   0, 0 },
    { 0x2011,  -7300,  -9550,   7400,   7100,  6,   0, 0,  38,    31,   191,  2340,  2308, 0, 0,   0, 0 },
    { 0x2030, -11000, -12350,  10000,  19900,  6,   0, 0,  39,    75,   238,  2340,  2308, 0, 0,   0, 0 },
    { 0x2031, -11000, -12350,  10000,  19900,  6,   0, 0,  39,    75,   238,  2340,  2308, 0, 0,   0, 0 },
    { 0x2040, -21500, -22400,  25750,   4350,  6,   1, 0,  40,   192,   223,  2607,  2308, 1, 0,   0, 0 },
    { 0x2041, -21500, -22400,  25750,   4350,  6,   1, 0,  40,   192,   223,  2607,  2308, 1, 0,   0, 0 },
    { 0x2050,  -9550, -15000,  11900,   9150,  6,   1, 0,  41,    -1,   186,  2340,  2308, 1, 1,   0, 0 },
    { 0x2051,  -9550, -15000,  11900,   9150,  6,   1, 0,  41,    -1,   186,  2340,  2308, 1, 1,   0, 0 },
    { 0x2060,  -7600, -10300,  10400,   7350,  6,   0, 0,  42,   205,    79,  1966,  3069, 1, 1,   0, 0 },
    { 0x2061,  -7600, -10300,  10400,   7350,  6,   0, 0,  42,   205,    79,  1966,  3069, 1, 1,   0, 0 },
    { 0x2070, -20450,  -9200,   6500,  -3600,  6,   0, 0,  43,    79,   224,  2340,  2308, 0, 0,   0, 0 },
    { 0x2071, -20450,  -9200,   6500,  -3600,  6,   0, 0,  43,    79,   224,  2340,  2308, 0, 0,   0, 0 },
    { 0x2070, -20450,   3650,   7200,   9100,  6,   0, 1,  44,    79,   224,  2340,  2308, 0, 0,   0, 0 },
    { 0x2071, -20450,   3650,   7200,   9100,  6,   0, 1,  44,    79,   224,  2340,  2308, 0, 0,   0, 0 },
    { 0x2070, -20400, -22000,   6450, -16450,  6,   3, 2,  45,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x2071, -20400, -22000,   6450, -16450,  6,   3, 2,  45,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x2080, -17650, -28900,  -5700,  -4750,  6,   0, 0,  46,   111,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x2081, -17650, -28900,  -5700,  -4750,  6,   0, 0,  46,   111,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x2080, -24900, -18950, -21450, -14400,  6,   0, 1,  47,   111,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x2081, -24900, -18950, -21450, -14400,  6,   0, 1,  47,   111,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x2090, -10900, -29000,   9200,  -3400,  6,   6, 0,  48,   110,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x2091, -10900, -29000,   9200,  -3400,  6,   6, 0,  48,   110,   187,  2340,  2308, 0, 0,   0, 0 },
    { 0x3000, -28622, -28370,  -3349, -11414,  7,   0, 0,  49,    71,   148,  2256,  2257, 0, 0,   0, 0 },
    { 0x3001, -28622, -28370,  -3349, -11414,  7,   0, 0,  49,    71,   148,  2256,  2257, 0, 0,   0, 0 },
    { 0x3010, -28640, -22836,  -4448,  16720,  7,   1, 0,  50,   113,   224,  2208,  2737, 0, 0,   0, 0 },
    { 0x3011, -28640, -22836,  -4448,  16720,  7,   1, 0,  50,   113,   224,  2208,  2737, 0, 0,   0, 0 },
    { 0x3020, -27200, -30618,  -5300, -11418,  7,   2, 0,  51,   156,   213,  2208,  2737, 0, 0,   0, 0 },
    { 0x3021, -27200, -30618,  -5300, -11418,  7,   2, 0,  51,   156,   213,  2208,  2737, 0, 0,   0, 0 },
    { 0x3030, -27902, -27848, -23060, -15104,  7,   6, 0,  52,   196,   138,  1956,  2144, 0, 0,   0, 0 },
    { 0x3031, -27902, -27848, -23060, -15104,  7,   6, 0,  52,   196,   138,  1956,  2144, 0, 0,   0, 0 },
    { 0x3040, -27900, -28900,  -4000,   1000,  7,   4, 0,  53,   183,   115,  2240,  2240, 0, 0,   0, 0 },
    { 0x3041, -27900, -28900,  -4000,   1000,  7,   4, 0,  53,   183,   115,  2240,  2240, 0, 0,   0, 0 },
    { 0x3050, -28175, -25268,   2000,   3950,  7,  12, 0,  54,   206,   195,  1591,  2240, 1, 0,   0, 0 },
    { 0x3051, -28175, -25268,   2000,   3950,  7,  12, 0,  54,   206,   195,  1591,  2240, 1, 0,   0, 0 },
    { 0x3060, -31556, -30382,  13400, -13430,  7,   3, 0,  55,   157,   147,  2496,  3984, 0, 0,   0, 0 },
    { 0x3061, -31556, -30382,  13400, -13430,  7,   3, 0,  55,   157,   147,  2496,  3984, 0, 0,   0, 0 },
    { 0x3070, -31755, -22340,  -1700,   2080,  7,   8, 0,  56,    64,   209,  2152,  2344, 0, 0,   0, 0 },
    { 0x3071, -31755, -22340,  -1700,   2080,  7,   8, 0,  56,    64,   209,  2152,  2344, 0, 0,   0, 0 },
    { 0x3080, -29100, -28360, -20000, -21910,  7,   8, 0,  57,    56,   179,  2048,  2048, 0, 0,   0, 0 },
    { 0x3081, -29100, -28360, -20000, -21910,  7,   8, 0,  57,    56,   179,  2048,  2048, 0, 0,   0, 0 },
    { 0x3090, -29800, -28700,  -9700,   3900,  7,  10, 0,  58,   207,   206,  2064,  2337, 0, 0,   0, 0 },
    { 0x3091, -29800, -28700,  -9700,   3900,  7,  10, 0,  58,   207,   206,  2064,  2337, 0, 0,   0, 0 },
    { 0x30A0, -29443,   7463, -15068,  21933,  7,   9, 0,  59,   233,   253,  2064,  2337, 0, 0,   0, 0 },
    { 0x30A1, -29443,   7463, -15068,  21933,  7,   9, 0,  59,   233,   253,  2064,  2337, 0, 0,   0, 0 },
    { 0x30B0, -25850, -29050,  -9650, -12200,  7,   9, 0,  60,   206,   206,  2064,  2337, 0, 0,   0, 0 },
    { 0x30B1, -25850, -29050,  -9650, -12200,  7,   9, 0,  60,   206,   206,  2064,  2337, 0, 0,   0, 0 },
    { 0x30C0, -27200, -14300,  -5275,   1600,  7,  12, 0,  61,   178,   208,  1560,  2337, 0, 0,   0, 0 },
    { 0x30C1, -27200, -14300,  -5275,   1600,  7,  12, 0,  61,   178,   208,  1560,  2337, 0, 0,   0, 0 },
    { 0x30D0, -27200, -30618,  -5300,   1650,  7,   2, 0,  62,   154,   213,  2497,  2716, 0, 0,   0, 0 },
    { 0x30D1, -27200, -30618,  -5300,   1650,  7,   2, 0,  62,   154,   213,  2497,  2716, 0, 0,   0, 0 },
    { 0x30E0,   1775,    250,  29450,  17300,  7,   3, 0,  63,    93,   207,  2056,  2432, 0, 0,   0, 0 },
    { 0x30E1,   1775,    250,  29450,  17300,  7,   3, 0,  63,    93,   207,  2056,  2432, 0, 0,   0, 0 },
    { 0x30E0, -29328, -28900, -17460, -17881,  7,   7, 1,  64,    93,   207,  2056,  2432, 0, 0,   0, 0 },
    { 0x30E1, -29328, -28900, -17460, -17881,  7,   7, 1,  64,    93,   207,  2056,  2432, 0, 0,   0, 0 },
    { 0x4000, -29720, -28478,   1492,   -380,  8,   0, 0,  65,   131,   112,  1760,  1760, 0, 0,   0, 0 },
    { 0x4001, -29720, -28478,   1492,   -380,  8,   0, 0,  65,   131,   112,  1760,  1760, 0, 0,   0, 0 },
    { 0x4010, -28100, -28766, -17048, -13790,  8,   1, 0,  66,   140,    76,  1536,  1617, 0, 0,   0, 0 },
    { 0x4011, -28100, -28766, -17048, -13790,  8,   1, 0,  66,   140,    76,  1536,  1617, 0, 0,   0, 0 },
    { 0x4020, -28200, -28400, -18450, -18600,  8,   2, 0,  67,   122,    87,  1760,  1760, 0, 0,   0, 0 },
    { 0x4021, -28200, -28400, -18450, -18600,  8,   2, 0,  67,   122,    87,  1760,  1760, 0, 0,   0, 0 },
    { 0x4030, -27416, -27956, -18884, -17948,  8,   3, 0,  68,   150,   121,  1281,  1288, 0, 0,   0, 0 },
    { 0x4031, -27416, -27956, -18884, -17948,  8,   3, 0,  68,   150,   121,  1281,  1288, 0, 0,   0, 0 },
    { 0x4040, -31900, -28922,   9525,  20258,  8,  12, 0,  69,   116,   164,  1281,  1288, 0, 0,   0, 0 },
    { 0x4041, -31900, -28922,   9525,  20258,  8,  12, 0,  69,   116,   164,  1281,  1288, 0, 0,   0, 0 },
    { 0x4050, -19650, -28900,  16350,  22050,  8,   2, 0,  70,    97,   129,  1281,  1288, 0, 0,   0, 0 },
    { 0x4051, -19650, -28900,  16350,  22050,  8,   2, 0,  70,    97,   129,  1281,  1288, 0, 0,   0, 0 },
    { 0x4070, -14950, -28550,  -2100, -15750,  8,   7, 0,  71,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x4071, -14950, -28550,  -2100, -15750,  8,   7, 0,  71,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x4080,  -7625, -30610,  29380,   5975,  8,   8, 0,  72,   125,   170,  1793,  1589, 0, 0,   0, 0 },
    { 0x4081,  -7625, -30610,  29380,   5975,  8,   8, 0,  72,   125,   170,  1793,  1589, 0, 0,   0, 0 },
    { 0x4090, -28908, -29132, -12708, -12252,  8,   9, 0,  73,   191,   143,  1540,  1844, 0, 0,   0, 0 },
    { 0x4091, -28908, -29132, -12708, -12252,  8,   9, 0,  73,   191,   143,  1540,  1844, 0, 0,   0, 0 },
    { 0x40A0, -30100, -30701,  -8562, -11690,  8,  10, 0,  74,   197,   180,  1793,  1712, 0, 0,   0, 0 },
    { 0x40A1, -30100, -30701,  -8562, -11690,  8,  10, 0,  74,   197,   180,  1793,  1712, 0, 0,   0, 0 },
    { 0x40B0, -28410, -28131,  -6259,  -8041,  8,  11, 0,  75,   159,   186,  1793,  1589, 0, 0,   0, 0 },
    { 0x40B1, -28410, -28131,  -6259,  -8041,  8,  11, 0,  75,   159,   186,  1793,  1589, 0, 0,   0, 0 },
    { 0x5000, -22050, -17600,   1400,   9100,  9,   0, 0,  76,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5001, -22050, -17600,   1400,   9100,  9,   0, 0,  76,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5010, -27500, -27300, -17300, -14100,  9,   1, 0,  77,   159,   115,  1537,  1801, 0, 0,   0, 0 },
    { 0x5011, -27500, -27300, -17300, -14100,  9,   1, 0,  77,   159,   115,  1537,  1801, 0, 0,   0, 0 },
    { 0x5020, -16433, -27525,   8450, -10821,  9,   7, 0,  78,   153,   114,  2086,  1710, 0, 0,   0, 0 },
    { 0x5021, -16433, -27525,   8450, -10821,  9,   7, 0,  78,   153,   114,  2086,  1710, 0, 0,   0, 0 },
    { 0x5030, -29828, -30968,   -404,   -110,  9,   3, 0,  79,   125,   153,  1792,  1792, 0, 0,   0, 0 },
    { 0x5031, -29828, -30968,   -404,   -110,  9,   3, 0,  79,   125,   153,  1792,  1792, 0, 0,   0, 0 },
    { 0x5040, -31880, -31466,   7504,   5020,  9,   5, 0,  80,    74,   152,  1793,  1729, 0, 0,   0, 0 },
    { 0x5041, -31880, -31466,   7504,   5020,  9,   5, 0,  80,    74,   152,  1793,  1729, 0, 0,   0, 0 },
    { 0x5050, -28732, -28150, -11662, -14710,  9,   4, 0,  81,    78,   123,  1793,  1729, 0, 0,   0, 0 },
    { 0x5051, -28732, -28150, -11662, -14710,  9,   4, 0,  81,    78,   123,  1793,  1729, 0, 0,   0, 0 },
    { 0x5060, -30350, -29324,   4768,  -5168,  9,   6, 0,  82,    86,   173,  1857,  1665, 0, 0,   0, 0 },
    { 0x5061, -30350, -29324,   4768,  -5168,  9,   6, 0,  82,    86,   173,  1857,  1665, 0, 0,   0, 0 },
    { 0x5080, -28966, -28814,  -6841,  -6654,  9,   9, 0,  83,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5081, -28966, -28814,  -6841,  -6654,  9,   9, 0,  83,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5090,  -5683, -29765,  30592,  11636,  9,   7, 0,  84,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5091,  -5683, -29765,  30592,  11636,  9,   7, 0,  84,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x50A0, -28500, -28700,   -700,   7010,  9,  14, 0,  85,   124,   204,  1696,  1681, 0, 0,   0, 0 },
    { 0x50A1, -28500, -28700,   -700,   7010,  9,  14, 0,  85,   124,   204,  1696,  1681, 0, 0,   0, 0 },
    { 0x50B0, -23800, -30328,  -7564,  -2448,  9,  11, 0,  86,   167,   177,  1696,  1681, 0, 0,   0, 0 },
    { 0x50B1, -23800, -30328,  -7564,  -2448,  9,  11, 0,  86,   167,   177,  1696,  1681, 0, 0,   0, 0 },
    { 0x50C0, -27992, -27848,  -2864,  -6770, 10,   0, 0,  87,   114,   122,  1700,  1648, 0, 0,   0, 0 },
    { 0x50C1, -27992, -27848,  -2864,  -6770, 10,   0, 0,  87,   114,   122,  1700,  1648, 0, 0,   0, 0 },
    { 0x50D0, -30650, -32000,   4068,   2426, 10,   1, 0,  88,   113,   160,  1700,  1648, 0, 0,   0, 0 },
    { 0x50D1, -30650, -32000,   4068,   2426, 10,   1, 0,  88,   113,   160,  1700,  1648, 0, 0,   0, 0 },
    { 0x50E0, -32000, -32000,  12880,   2955, 10,   2, 0,  89,   153,   142,  1700,  1648, 0, 0,   0, 0 },
    { 0x50E1, -32000, -32000,  12880,   2955, 10,   2, 0,  89,   153,   142,  1700,  1648, 0, 0,   0, 0 },
    { 0x50F0, -31200, -29405,   7535,  -2945, 11,   2, 0,  90,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x50F1, -31200, -29405,   7535,  -2945, 11,   2, 0,  90,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5100, -28230, -28100,  -6800,   1800, 11,   1, 0,  91,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5101, -28230, -28100,  -6800,   1800, 11,   1, 0,  91,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x5110, -29164, -28328,    -64,  -1480,  9,   3, 0,  92,   125,   154,  1792,  1792, 0, 0,   0, 0 },
    { 0x5111, -29164, -28328,    -64,  -1480,  9,   3, 0,  92,   125,   154,  1792,  1792, 0, 0,   0, 0 },
    { 0x5120, -29378, -31088,   6496,   4840,  9,   5, 0,  93,    74,   152,  1793,  1729, 0, 0,   0, 0 },
    { 0x5121, -29378, -31088,   6496,   4840,  9,   5, 0,  93,    74,   152,  1793,  1729, 0, 0,   0, 0 },
    { 0x5140, -28500, -28700,  16700,   7010,  9,  14, 0,  94,   124,   204,  1696,  1681, 0, 0,   0, 0 },
    { 0x5141, -28500, -28700,  16700,   7010,  9,  14, 0,  94,   124,   204,  1696,  1681, 0, 0,   0, 0 },
    { 0x6000,  -7867, -25284,  22283,   6701, 12,   7, 0,  95,   202,   158,  2161,  2304, 0, 0,   0, 0 },
    { 0x6001,  -7867, -25284,  22283,   6701, 12,   7, 0,  95,   202,   158,  2161,  2304, 0, 0,   0, 0 },
    { 0x6010, -32012, -32020,   1518,   4435, 12,   4, 0,  96,   203,   157,  2161,  2247, 0, 0,   0, 0 },
    { 0x6011, -32012, -32020,   1518,   4435, 12,   4, 0,  96,   203,   157,  2161,  2247, 0, 0,   0, 0 },
    { 0x6020, -13050,   -750,   4500,  15450, 12,   6, 0,  97,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x6021, -13050,   -750,   4500,  15450, 12,   6, 0,  97,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x6020, -13050, -19750,   4500,  -3550, 12,   5, 1,  98,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x6021, -13050, -19750,   4500,  -3550, 12,   5, 1,  98,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x6030, -29061, -28706,    678, -17910, 12,   7, 0,  99,   243,   137,  2161,  2304, 0, 0,   0, 0 },
    { 0x6031, -29061, -28706,    678, -17910, 12,   7, 0,  99,   243,   137,  2161,  2304, 0, 0,   0, 0 },
    /* Runde 39 Spur G, Nachbesserung 1 (M4): Gast-Zeilen der Treppenhaeuser wie ihre Hauptzeilen aus den Waenden.
     * 1060 2F (Blatt 3 rect 1 = s_map_rectfix, MAP04.PIX uv(168,16) an (118,134)): x 26900 -> 118, 19800 -> 135,
     *   z 18200 -> 134, 26950 -> 155; Treppen (25150,22000) -> (122,144), (21400,22850) -> (131,146) in den Kaesten
     *   der Vorlage (121,144)/(132,146).
     * 1060 3F (Blatt 4 rect 1 (136,137), MAP05.PIX uv(168,16)): x -> 136/153, z -> 137/158; Treppe Band 8
     *   (25150,22000) @0x00D72 -> (140,147) - die Marke folgt ihr (s_map_marks, 3F unveraendert in der Vorlage).
     * 10A0 B2 (Blatt 1 rect 9 (170,101), MAP02.PIX uv(128,16), 17x21 gemalt): x 19850 -> 170, 27000 -> 186,
     *   z 26300 -> 101, 17550 -> 121; Treppen (21450,20650) @0x00E90 -> (174,114) im Kasten von (173,115),
     *   (25350,22350) @0x00ECC -> (182,110) im Kasten von (183,112). Vorher Bbox: 8 von 140 Rasterpunkten neben
     *   der Kachel (x187), Treppe (186,112) auf der Wand. */
    { 0x1060,  17800,  16200,  28900,  28950,  3,   1, 0,   7,   259,    14,  2511,  2516, 1, 1,   0, 1 },
    { 0x1061,  17800,  16200,  28900,  28950,  3,   1, 0,   7,   259,    14,  2511,  2516, 1, 1,   0, 1 },
    { 0x1060,  17800,  16200,  28900,  28950,  4,   1, 0,   7,   277,    17,  2511,  2516, 1, 1,   0, 1 },
    { 0x1061,  17800,  16200,  28900,  28950,  4,   1, 0,   7,   277,    17,  2511,  2516, 1, 1,   0, 1 },
    { 0x10A0,  17850,  15550,  29100,  28300,  1,   9, 0,  10,    54,   234,  2346,  2397, 0, 0,   0, 1 },
    { 0x10A1,  17850,  15550,  29100,  28300,  1,   9, 0,  10,    54,   234,  2346,  2397, 0, 0,   0, 1 },
    /* ROOM1090, obere Ebene (Dachterrasse) - GAST-ZEILE auf Blatt 3, gleiche Zonen-Nummer
     * wie die Hauptzeile, sichtbar ueber das ETAGEN-Bit. Dass der Raum zwei Ebenen hat, ist
     * gemessen: die 16 Kamerasaetze (@+0x24 -> Dateioffset 0x60, 32 B je Satz) sind 8
     * Kameras in zwei Baenken, und deren Y zerfaellt in zwei Gruppen - cuts 0/1/2 bei
     * -12798/-12798/-11088 (Dach), cuts 3..7 bei -3006/-5526/-5526/-5526/-3294 (Hof).
     * Die SCA-Baender folgen dem (0/1/2 unten, 4/5/6 oben; band_from_y = -(y/0x708)).
     * Blatt 3 folgt der Generator-Regel "Band der Tuer -> Seite des Zielraums": die
     * Band-6-Tuer fuehrt nach ROOM1100, und dessen Seite ist 3.
     * RECHTECK 7, und zwar als einziger Kandidat: von Blatt 3s zehn Rechtecken sind nur 2
     * und 7 frei, und eine Schablonensuche im 2F-Grundriss (MAP04.PIX, s.o.) legt rect7 auf
     * x204..248 / y138..186 - es beruehrt rect6 (= ROOM1100) auf einer gemeinsamen
     * Wandlinie (dy = -1, 36 px Ueberlappung) und ueberlappt rect9 (= ROOM10F0). Genau
     * diese beiden sind die Nachbarn der oberen Ebene (hinein aus ROOM10F0 auf Band 5,
     * hinaus nach ROOM1100 auf Band 6). rect2 (x160..211 / y226..253) beruehrt ROOM10F0
     * gar nicht. Ein Tuer-Anker aus einer ausgelieferten Massstabszeile ist hier nicht zu
     * haben - ROOM10F0 (@0x80076928) und ROOM1100 (@0x80076930) tragen beide den Stub. */
    /* Runde 39 Spur G: Zeile der OBEREN Ebene (Band 5/6, Blatt 3 rect 7, MAP04.PIX uv(112,74), Gamma
     * aus Querband + Steg + Block), Herleitung wie die Hof-Zeile oben. Weltwaende ROOM1090.RDT ->
     * gemalte Linie: x -14672 (@0x01E54) -> 179, -6878 (@0x01FBC) -> 197, -4814 (@0x01E48) -> 204,
     * 2752 (@0x01F38) -> 222, Reste 0/1/-1/0; z 3404 (@0x01E6C) -> 67, 491 (@0x01FC8) -> 76, -5276
     * (@0x01F2C) -> 87, -16403 (@0x01F20) -> 114, Reste 1/-1/1/0. Acht Waende, hoechstens 1 px -
     * die Kachel IST die obere Ebene. Nutzerpunkt (-12528,959) -> (184,72) im Querband. */
    { 0x1090, -17500, -18700,  10285,  11754,  3,   7, 0,  22,   136,   150,  2597,  2435, 0, 0,   0, 1 },
    { 0x1091, -17500, -18700,  10285,  11754,  3,   7, 0,  22,   136,   150,  2597,  2435, 0, 0,   0, 1 },
    /* ⛔ AUSWEICH-KACHEL, NUR GEDULDET (2026-09-12): diese 1F-Gastlage von
     * ROOM10F0/10F1 sitzt auf Blatt 2 Rect 8 - einer AUSWEICH-Kachel des
     * Loesers; Rect 8 gehoert pixelgenau belegt ROOM1010-Nord (Dossier
     * karte-1010.md). Die Zeilen BLEIBEN, weil sie die Etagen-Buchfuehrung
     * des Bands 1 tragen (ohne sie leckte ein 1F-Besuch aufs 2F-Blatt -
     * Pin unit_map_floor_visited). Ihre TUERMARKE (171,77) ist entfernt
     * (die "Tuer, die gar nicht existiert", Nutzer 2026-09-12); 10F0s echte
     * 1F-Kachel ist unvermessen - OFFEN in karte-1010.md 5.3. */
    { 0x10F0,  -9800,  -5400,  11300,  16800,  2,   8, 0,  15,     0,     0,     0,     0, 1, 1,   0, 1 },
    { 0x10F1,  -9800,  -5400,  11300,  16800,  2,   8, 0,  15,     0,     0,     0,     0, 1, 1,   0, 1 },
    /* Runde 39 Spur G, Nachbesserung 1 (M5): ROOM1170 Zone 1 auf 3F (Blatt 4 rect 3 (152,89), MAP05.PIX uv(192,16) =
     * dieselbe Zeichnung wie auf dem Dach, +12/+9 versetzt): dieselben neun Waende -> 152/159/169/169/193 bzw.
     * 89/103/110/110 (Reste <= 1). Treppen (Aot_set sce 12/13): Band 0 (-25670,-20645) @0x0137A -> (156,95),
     * Band 2 (-25640,-23425) @0x0138E -> (156,101), (-23295,-26155) @0x013A2 -> (162,108), Band 4 (-20690,-25595)
     * @0x013B6 -> (168,107). */
    { 0x1170, -28900, -28841,  -8784, -17070,  4,   3, 1,  24,   140,   122,  2597,  2511, 0, 0,   0, 1 },
    { 0x1171, -28900, -28841,  -8784, -17070,  4,   3, 1,  24,   140,   122,  2597,  2511, 0, 0,   0, 1 },
    /* Runde 36 Spur M: die beiden 11A0-Gast-Zeilen (Blatt 6 rect 2, etage 1) sind ENTFALLEN -
     * Blatt 6 rect 2 ist jetzt die HAUPTzeile von ROOM11A0 (s.o. bei ROOM1190). */
    { 0x4070, -14950, -28550,  -2100, -15750, 11,   3, 0,  71,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4071, -14950, -28550,  -2100, -15750, 11,   3, 0,  71,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4070, -14950, -28550,  -2100, -15750,  9,   1, 0,  71,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4071, -14950, -28550,  -2100, -15750,  9,   1, 0,  71,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x50D0, -30650, -32000,   4068,   2426, 11,   0, 0,  88,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x50D1, -30650, -32000,   4068,   2426, 11,   0, 0,  88,     0,     0,     0,     0, 0, 0,   0, 1 },
    /* Runde 35 Spur G: dieselbe Kabine auf 2F (Ersatzrect Blatt 3 Nr. 4 = (109,134) wie 1F)
     * und 3F (Blatt 4 rect 0 (127,137), gemalte Waende x127/x136 y137/y146, MAP05.PIX @0x1454).
     * Abbildung wie die 1F-Zeile oben: RE2-Massstab 2330 (FUN_8006e120 @0x8006e1dc), Versatz
     * aus der Kunst (3F: Kasten um (+18,+3) verschoben -> 173/76). Nachbesserung 1. */
    { 0x1080, -16750,  -5150, -10550,    950,  3,   4, 0,   9,   155,    73,  2330,  2330, 1, 1,   0, 1 },
    { 0x1081, -16750,  -5150, -10550,    950,  3,   4, 0,   9,   155,    73,  2330,  2330, 1, 1,   0, 1 },
    { 0x1080, -16750,  -5150, -10550,    950,  4,   0, 0,   9,   173,    76,  2330,  2330, 1, 1,   0, 1 },
    { 0x1081, -16750,  -5150, -10550,    950,  4,   0, 0,   9,   173,    76,  2330,  2330, 1, 1,   0, 1 },
    /* Runde 35 Spur G: ROOM10A0 (Treppenhaus) auf B1 = Blatt 0 rect 6. Band 4: Ankunft aus
     * ROOM1180/1230 auf y -7200 (Tuer @0xA34/@0xB34, Spawn (21200,-7200,25500);
     * re15_collision_band_from_y = -(y/0x708) = 4). Die gemalte Nische der Gang-Kachel
     * (155..159,108..112) oeffnet genau auf rect 6 (153,114,16,24). */
    /* Runde 39 Spur G: B1-Absatz -> Schema 10 (Absatz Band 4 samt beiden Treppenlaeufen, Aot_set sce 13 Band 4
     * @0x00E7C / @0x00E90); die gemalte r6 (153,114,16,24) entfaellt, die Etagenzeile Band 4 zeigt auf rect 255. */
    { 0x10A0,  17850,  15550,  29100,  28300,  0, 255, 0,  10,     0,     0,     0,     0, 0, 0,  10, 1 },
    { 0x10A1,  17850,  15550,  29100,  28300,  0, 255, 0,  10,     0,     0,     0,     0, 0, 0,  10, 1 },
    { 0x4020, -28200, -28400, -18450, -18600,  9,  13, 0,  67,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4021, -28200, -28400, -18450, -18600,  9,  13, 0,  67,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4020, -28200, -28400, -18450, -18600, 10,   3, 0,  67,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x4021, -28200, -28400, -18450, -18600, 10,   3, 0,  67,     0,     0,     0,     0, 0, 0,   0, 1 },
};

/* MARKEN in Karten-Pixeln. kind = TUER mit WANDSEITE 0=Nord 1=Ost
 * 2=Sued 3=West (die Nische zeigt nach innen), 4/5 = Treppe mit
 * waagerechten bzw. senkrechten Sprossen.
 * Treppen stammen aus den SCD-Zonen Aot_set Typ 12/13 (die Band-Wechsel-
 * Zonen), Tueren aus den Tuer-Datensaetzen. Gezeichnet werden sie nur fuer
 * Zonen, die der Spieler schon gesehen hat.
 * auf_partner = 1: die Marke liegt auf der GEMALTEN Flaeche des Rechtecks
 * der zweiten Zone (zid2). Nur dann darf sie schon sichtbar sein, wenn
 * bloss die zweite Zone besucht ist - sonst schwebt sie im Leeren.
 * ACHTUNG - der KASTEN reicht dafuer nicht (Nutzer-Befund 2026-09-05,
 * "This door is flying"): er ist nur die Bounding-Box, die Kachel darin
 * enthaelt Schwarz. Blatt 3 Marke (141,117) liegt in Rect 0 (102,116)
 * 40x40, dessen Kachel traegt dort aber Index 0. Gemessen ueber alle
 * Marken mit Partner-Rechteck: 47 liegen auf dessen Kachel, 23 nicht. */
typedef struct { unsigned char page, rect; short mx, my;
                 unsigned char kind, zid, zid2, auf_partner;
               } re15_map_mark_t;
static const re15_map_mark_t s_map_marks[] = {
    /* Runde 36 Spur M: die Marke (0,1,116,116,2,27) ist ENTFALLEN - ROOM11A0 liegt nicht mehr auf
     * Blatt 0 rect 1 (s. Zonen-Zeilen ROOM1190/ROOM11A0); sie stand als Tuer an der Suedwand der
     * 1190-Haupthalle, die dort keine Tuer hat (ROOM1190: nur slot 4 @0x028B2 / slot 6 @0x02B1C).
     * Die Ostkammer-Tuer (151,69) gehoert jetzt der Ostkammer (zid 110 statt 26). */
    /* Runde 39 Spur G: alte Blatt-0-Marke(n) hier entfernt - B1 neu, Marken am Tabellenende */
    /* Runde 36 Spur M (Nutzer: "Die Tuer von ROOM 11C0 ist auf der Map um 90 Grad verdreht"):
     * war (172,100) kind 1 = SENKRECHTER Balken quer durch die WAAGERECHTE Wand y100 zwischen
     * 11B0 (rect 3, Unterkante MAP01.PIX uv v104) und 11C0 (rect 4, Oberkante v16). Das Original
     * MALT die Tuer waagerecht: Nische in der Nordwand der 11C0-Kachel, MAP01.PIX uv(187..191,
     * 17..21) (u187 v17 = oberes Nibble von Datei-Byte 0x8DD = 4) = Schirm (171..175,101..105), Katalog
     * analysis/kartensymbole/symbolkatalog.csv "Wand N, waagerecht". Beide Tuer-Datensaetze
     * projizieren mit der ausgelieferten 11B0-Zeile @0x80076988 (159,110,2272,1954) auf x173
     * (11B0 slot 1 Mitte (-25480,-28190) -> (173,103)). Jetzt kind 2 (Suedwand von 11B0 =
     * waagerecht, Zeichner liest nur die Achse, re15_inv_screen.c) bei (173,100). */
    /* Runde 39 Spur G: alte Blatt-0-Marke(n) hier entfernt - B1 neu, Marken am Tabellenende */
    /* Runde 35 Spur G: die Tueren des Gangs 1180/1230 auf seinem rect 0 (statt rect 6),
     * je auf der gemalten Nische (G_karte.md B4): 1190-Haupt (136..141,71..75) Wand y76,
     * 10A0 (155..159,108..112) Wand y113. Ungepaart (zid2 255): der 1190-Hauptraum (rect 1)
     * und der 10A0-Absatz fuehren auf diesem Blatt keine Zone, deren Kasten die Marke
     * beruehrt (Pin unit_map_durchgang). 1190-Ost und 11D0 tragen schon je eine Marke auf
     * DEMSELBEN Punkt ((151,69) rect 2, (129,124) rect 5) - die werden mit dem Gang
     * gepaart (auf_partner), statt eine zweite daraufzulegen. Die 11B0-Tuer bekommt KEINE Gang-Marke: die Kachel malt sie
     * UNTER dem Zweig (y78..86), der Gang laeuft in der Welt dort nach Norden weiter - ein
     * Zug dorthin liesse den Marker beim Nordwaertsgehen nach UNTEN rutschen. */
    /* Runde 36 Spur M: die Haupthallen-Tuer ist jetzt GEPAART mit der 1190-Haupthalle (zid 26,
     * Blatt 0 rect 1, deren Oberkante y76 die Marke traegt -> auf_partner 1). */
    /* Runde 39 Spur G: alte Blatt-0-Marke(n) hier entfernt - B1 neu, Marken am Tabellenende */
    {  1,  0,  105,  109, 2,  31,  32, 1 },
    {  1,  0,  165,  138, 0,  31,  33, 1 },   /* Runde 39 Spur G: y139 -> y138 auf die Wand (Vorlage mapB2_correct.png) */
    /* Runde 35 Spur G: ALLE sechs Tueren von ROOM1210 auf dem T-Korridor (rect 3), je auf
     * der gemalten Nische (G_karte.md B4): 11E0 x187 y72..76 (1210.RDT @0x1CC6), Zellen
     * West x201 (rect 4 Nische y80..84 @0x1CE6, rect 5 y97..101 @0x1D26), Zellen Ost x212
     * (rect 6 y89..93 @0x1D06, rect 7 y106..110 @0x1D46, rect 8 y123..127 @0x1D66).
     * Vorher: 5 Marken, davon (206,95) quer mitten im Gang und 3 der 5 Zellentueren fehlten. */
    {  1,  3,  186,   74, 3,  34,  31, 1 },   /* Runde 39 Spur G: x187 -> x186 auf die Flur-Ostwand (mapB2_correct.png) */
    {  1,  3,  201,   82, 3,  34,  35, 1 },
    {  1,  3,  212,   91, 1,  34, 102, 1 },
    {  1,  3,  201,   99, 3,  34, 103, 1 },
    {  1,  3,  212,  108, 1,  34, 104, 1 },
    {  1,  3,  212,  125, 1,  34, 105, 1 },
    {  1,  9,  176,  101, 2,  10,  31, 1 },
    {  1,  9,  177,  101, 0,  10, 255, 0 },
    {  1,  9,  173,  115, 4,  10, 255, 0 },   /* Runde 39 Spur G: x177 -> x173 (Sprossen x171..175, mapB2_correct.png) */
    {  1,  9,  183,  112, 4,  10, 255, 0 },   /* Runde 39 Spur G: x186 -> x183, nicht mehr in der Wand x186 (mapB2_correct.png) */
    /* ⛔ GESTRICHEN (Runde 30, karten-marken.md §4b / §5 Schritt 3a): EINE MARKE OHNE
     * EIGENE ZONENZEILE HAT KEIN BESUCHT-BIT UND KEINEN TRAEGER - sie kann nur an einem
     * FREMDEN haengen. Die fuenf Zeilen (1, 255, 131, 77 / 134, 133 / 138, 133 /
     * 139, 77 / 144, 80, .., zid 0) gehoerten den Tueren von ROOM1230
     * (ROOM1230.RDT @0x00AEE / @0x00B34 / @0x00B54 / @0x00B74 / @0x00BC4). ROOM1230
     * fuehrt keine Zonenzeile; der Generator setzte dafuer die Rueckfall-Nummer 0 ein
     * (`zid_of.get((b, zi), 0)`), und 0 ist ROOM1000 Zone 0. Gemessen: nur ROOM1000/z0
     * betreten -> die Marken erschienen auf Blatt 1; alles AUSSER ROOM1000/z0 betreten
     * -> unsichtbar. Im Stand des Nutzers vom 2026-09-27 standen drei davon frei im
     * Panel. Der Riegel unit_map_marke_zid haelt die Regel fest. */
    {  2,  1,  109,   83, 1,   3,   5, 1 },
    {  2,  1,  163,   81, 3,   2,   3, 1 },
    /* Tuer B (1010-Sued <-> 1020): Position bleibt, Bindung wandert an die neue
     * Sued-Zone 100 (Rect 7) - s. 1010-Zeilen oben. */
    {  2,  1,  163,  106, 3, 100,   3, 1 },
    /* ⛔ FAHRSTUHLTUER AUF DIE WAND (Nutzer 2026-09-12: "die Tuer im Korridor 1F
     * Richtung Fahrstuhl ist ganz leicht versetzt unten. Die muss auf der
     * gleichen Ebene sein wie in 2F oder 3F - direkt auf der Wand in der Map").
     * Die Kabinen-Suedwand der Kachel uv(168,40) laeuft auf y=143 ueber x109..118,
     * Mitte also (113,143) - exakt die Werte, die Blatt 3 (Zeile ~298) und
     * Blatt 4 (Zeile ~318) fuer DIESELBE Kabine ROOM1080 fuehren. Der Generator
     * setzte hier (114,145): zwei Pixel zu tief, weil snap_wall nur die
     * Silhouette kennt und die Kabine INNERHALB der gemalten Flaeche von Rect 2
     * liegt (bekanntes Muster, Memory reai-v2-schranke-strenger-als-befund). */
    {  2,  2,  113,  143, 2,   5, 255, 0 },
    {  2,  4,  119,  129, 3,   4,   5, 1 },
    {  2,  4,  145,  133, 1,   4,   8, 1 },
    /* 1000-Flur <-> 1050, KORRIGIERT (Runde 7, karte-1050-tueren.md; Nutzer:
     * 'in ROOM 1050 fehlen 3 Tueren auf der Map. Da stehe ich davor.'): die
     * drei West-Tueren nach ROOM1000 (Door_aot slot1/2/3 @0x00AFA/0x00B1A/
     * 0x00B3A, Mitten (13700,9200)/(13700,6100)/(13700,-13450)) projizieren
     * mit der flip(1,1)-Zeile auf die OSTWAND x206 der gedrehten Zeichnung.
     * Der Runde-6-Snap auf die Westkante x180 war ein Fehl-Snap unter dem
     * Druck der AUSSEN-Schiene (deren Beruehrungs-Modell gilt fuer
     * flip-montierte Nachbarn nicht; Ausnahme jetzt in test_map_durchgang).
     * Traeger Rect 0 / zid 6; zwei mit Partner-Zeichnung (zid2 1/0). */
    /* y-Werte auf die nachgeeichte 1050-Zeile gezogen (oy 51 / sy 2322, s.o.);
     * die Sued-WC-Tuer bindet an die neue Orts-Nummer 101. auf_partner bleibt 0:
     * die Partnerzone traegt kuenftig eine SCHEMA-Zeichnung, keine gemalte Kachel -
     * die Pruefung in re15_map_zones.c verlangt gemalte Flaeche. */
    {  2,  0,  206,  142, 1,   6, 101, 0 },
    {  2,  0,  206,  135, 1,   6,   1, 0 },
    {  2,  0,  206,   92, 1,   6,   0, 0 },
    /* (177,188)/(183,188) ersetzt (karte-1050.md): Klemm-Artefakte der alten
     * Rect-4-Zuordnung - sie lagen 40+ px unter jeder Kunst. Die 1090-Tuer
     * projiziert mit der neuen 1050-Zeile auf (191,77), Nordwand-Snap der
     * Rect-0-Kunst y=71. */
    {  2,  0,  191,   69, 0,   6, 255, 0 },
    {  2,  4,  186,  152, 3,   4,   6, 1 },
    /* ⛔ GESTRICHEN (Runde 30, Schritt 3a): (2, 5, 180, 64, 3, 11) und
     * (2, 5, 222, 75, 1, 11) - die beiden Tueren von ROOM10B0 (ROOM10B0.RDT
     * @0x016D2 / @0x016F2). Ihre Zone 11 fuehrt auf Blatt 2 rect 255 OHNE Schema,
     * wird also gar nicht gezeichnet; das Rechteck 5, an dem sie hingen, gehoert
     * seit Runde 10 ROOM1090 (Zonenzeile oben, zid 22). Die Marken erschienen damit
     * auf der Kachel eines FREMDEN Raums. */
    {  2,  6,  183,  100, 4,  10, 255, 0 },   /* Runde 39 Spur G: x187 -> x183 (Sprossen x181..185, map1F_correct.png) */
    {  2,  6,  194,   99, 4,  10, 255, 0 },   /* Runde 39 Spur G: x196 -> x194, nicht mehr in der Wand x197 (map1F_correct.png) */
    {  2,  6,  197,   92, 1,  10,   6, 1 },  /* 2026-09-12: dieselbe physische
                                               * Tuer wie 1050->10A0 (197,98);
                                               * Punkt auf bemalter Flaeche
                                               * BEIDER Kacheln (Rect 6 UND 0) */
    /* (171,77) entfernt: Marke der 10F0-Ausweich-Gastzeile auf Rect 8 -
     * die "Tuer, die gar nicht existiert" (Nutzer 2026-09-12). */
    {  2, 10,  119,  151, 3,   7,   5, 0 },   /* Runde 39 Spur G: x120 -> x119 auf die Wand (map1F_correct.png) */
    {  2, 10,  122,  144, 4,   7, 255, 0 },   /* Runde 39 Spur G: x127 -> x122 (Sprossen x120..124, map1F_correct.png) */
    {  2, 10,  133,  146, 4,   7, 255, 0 },   /* Runde 39 Spur G: x135 -> x133, nicht mehr in der Wand x136 (map1F_correct.png) */
    {  3,  0,  113,  143, 2,  12, 255, 0 },
    {  3,  0,  135,  126, 1,  12,  13, 1 },
    {  3,  1,  118,  149, 1,   7,  12, 1 },
    {  3,  1,  121,  144, 4,   7, 255, 0 },   /* Runde 39 Spur G: x126 -> x121 (Sprossen x119..123, map2F_correct.png) */
    {  3,  1,  132,  146, 4,   7, 255, 0 },   /* Runde 39 Spur G: x134 -> x132, nicht mehr in der Wand x135 (map2F_correct.png) */
    {  3,  3,  141,  105, 0,  13,  14, 1 },
    {  3,  3,  199,  149, 1,  13,  16, 1 },
    {  3,  5,  188,  128, 3,  17,  17, 1 },
    {  3,  5,  188,  139, 3,  17,  17, 1 },
    {  3,  6,  186,  118, 3,  16,  17, 0 },   /* Runde 39 Spur G: x188 -> x186 auf die Wand (map2F_correct.png) */
    /* ⛔ NEU GESETZT (Runde 30, Nutzer 2026-09-27: "2F ist jetzt unten eine Tuer
     * eingezeichnet auf der Karte die es nicht gibt"): die Tuer ROOM1090 (obere
     * Ebene) -> ROOM1100. Tuer-Datensatz ROOM1090.RDT @0x0213A:
     *   3b 01 02 31 06 00 | 10 e1 40 ac 68 10 7c 15 | 5c ae 00 00 a0 d7 | 00 04 | 00 10 05
     *   Door_aot_set Slot 1, Band 6, Trigger x -7920 z -21440 w 4200 d 5500
     *   (Mitte (-5820,-18690)), Ankunft (-20900,0,-10336), Ziel Stage 1 Raum 0x10
     *   = ROOM1100, cut 5.
     * Bisher stand sie als (3, 255, 188, 180, 3, 0, 255, 0): ohne Rechteck, mit der
     * Rueckfall-Nummer zid 0 (= ROOM1000/z0) - sie erschien frei im Panel, sobald
     * ROOM1000 betreten war (gemessen am Abzug befund_1070_F259_marke1.png:
     * (188,178..182), ringsum nur Panel).
     * FELDER:
     *   Blatt 3, rect 7   Gast-Zeile von ROOM1090 obere Ebene (Zonenzeile oben);
     *                     Etagenzeile (0x1090, 0, 6, 3, 7), Band 6 = Byte 4 des
     *                     Datensatzes.
     *   zid 22 / zid2 16  ROOM1090 / ROOM1100 (Ziel-Byte 0x10 im Datensatz).
     *   y = 114, kind 2   die GEMEINSAME Wand: Suedwand von rect 7 (67+48-1) =
     *                     Nordwand von rect 6 (s_map_rectfix: y 114). In DATA/MAP04.PIX
     *                     tragen BEIDE Kacheln dort Index 4 auf x 197..221 (gemessen,
     *                     Sonde probe_r30_karten-marken_rundlauf Abschnitt J).
     *   auf_partner 1     der Punkt liegt auf gemalter Flaeche von rect 6 (Index 4).
     *   x = 199           ⛔ Port-Wahl, keine Original-Adresse - PROJEKTION der Trigger-Mitte
     *                     (-5820,-18690) durch re15_map_zone_marker auf rect 7 - wie
     *                     bei JEDER vom Generator erzeugten Marke. ROOM1090
     *                     (@0x800768F8) und ROOM1100 (@0x80076930) tragen im Original
     *                     den Massstabs-Stub 00 00 00 00 01 00 01 00, die Abbildung
     *                     ist deshalb die Bbox-Streckung des Ports. Gegenprobe von der
     *                     anderen Seite: der Ankunftspunkt in ROOM1100 projiziert auf
     *                     (203,120) - 4 Punkte daneben; beide x liegen in der
     *                     Wandspanne 197..221. */
    {  3,  7,  199,  114, 2,  22,  16, 1 },
    {  3,  9,  156,   83, 1,  15,  13, 1 },
    {  4,  1,  136,  152, 1,   7,  18, 1 },
    {  4,  1,  140,  147, 4,   7, 255, 0 },   /* Runde 39 Spur G, Nachbesserung 1 (M4): x144 -> x140 = Projektion der
                                               * STAIR-AOT Band 8 (25150,22000) @0x00D72 mit der Wandzeile (vorher
                                               * Bbox-Projektion); derselbe Lauf wie 1F (122,144) / 2F (121,144). */
    /* ⛔ GESTRICHEN (Runde 30, Schritt 3a): (4, 1, 148, 137, 0, 22) - die Tuer
     * ROOM1160 -> ROOM1180 (ROOM1160.RDT @0x007C6). ROOM1160 fuehrt keine
     * Zonenzeile; die Nummer 22 wurde in Runde 10 an ROOM1090 vergeben, die Marke
     * hing seitdem am Besucht-Bit von ROOM1090 und am Rechteck des Treppenhauses. */
    {  4,  3,  156,   98, 4,  24, 255, 0 },   /* Runde 39 Spur G, Nachbesserung 1 (M5): x159/y99 = Bbox-Projektion der
                                               * Laufmitte; mit der Wandzeile liegt der Lauf Band 0 -> 2 (@0x0137A
                                               * (156,95) .. @0x0138E (156,101)) auf x156, Mitte y98. */
    {  4,  3,  168,  106, 5,  24, 255, 0 },   /* Runde 39 Spur G, Nachbesserung 1 (Abnahme-Mangel M1): y106 wie vor
                                               * Runde 39. Die Vorlage map3F_correct.png verschiebt die Treppe um EIN
                                               * Vorlagenpixel = 1/3 Spielpixel (Sprossen y104,33..109,33) - bei 1x
                                               * nicht darstellbar: y105..109 (y107) stellte alle drei Sprossen auf die
                                               * Linie y110 (x166 auf den Tuerbalken 1130/1170), y104..108 (y106) beruehrt
                                               * nur mit (170,104) die Wandecke (170,103); Blockmitten-Abtastung der
                                               * Vorlage (Abnahme 0) = y104..108. Projektion der Band-4-Treppe @0x013B6
                                               * mit der Wandzeile: y107 (9 Waende) bzw. y106 (5 Waende) - beide im Kasten. */
    {  4,  4,  145,   84, 3,  19,  21, 1 },
    {  4,  4,  160,  152, 1,  19,  20, 1 },
    {  4,  4,  166,   89, 2,  19,  24, 1 },   /* Runde 39 Spur G: x168 -> x166, Balken x164..168 endet vor der Ecke x169 (map3F_correct.png) */
    {  4,  5,  131,  146, 2,  18, 255, 0 },
    {  4,  5,  153,  126, 1,  18,  19, 1 },
    {  4,  6,  164,  110, 0,  20,  24, 1 },
    {  5,  0,  156,   97, 5,  24, 255, 0 },
    {  5,  1,  174,  101, 0,  23,  24, 1 },
    {  6,  0,   90,  179, 3,  37, 255, 0 },
    {  6,  0,   91,  131, 1,  37,  38, 1 },
    {  6,  0,  107,  139, 1,  37,  43, 1 },
    {  6,  0,  129,  153, 1,  39,  47, 0 },
    {  6,  0,  129,  184, 1,  39,  42, 1 },
    {  6,  0,  137,  185, 5,  39, 255, 0 },
    {  6,  0,  139,  153, 5,  46, 255, 0 },
    {  6,  0,  163,  168, 3,  39,  43, 1 },
    {  6,  0,  163,  169, 1,  37, 255, 0 },
    {  6,  0,  163,  173, 1,  46, 255, 0 },
    {  6,  1,   66,  157, 0,  40,  41, 1 },
    {  6,  1,   90,  125, 2,  41,  37, 1 },
    {  6,  1,   90,  147, 1,  40,  42, 1 },
    {  6,  1,   96,  163, 3,  40,  40, 0 },
    {  6,  1,   96,  163, 4,  40, 255, 0 },
    {  6,  2,  110,   71, 0,  27, 255, 0 },
    {  6,  2,  115,  100, 2,  27, 255, 0 },
    {  6,  2,  123,  100, 2,  27, 255, 0 },
    {  6,  2,  130,   95, 1,  27, 255, 0 },
    {  6,  3,  107,  140, 3,  45, 255, 0 },
    {  6,  3,  164,  140, 1,  45, 255, 0 },
    {  6,  6,  193,  156, 1,  48, 255, 0 },
    /* ⛔ GESTRICHEN (Runde 30, Schritt 3a): (6, 255, 166, 104, 2, 0, 39) und
     * (6, 255, 185, 110, 2, 0) - die Tueren von ROOM2020 (ROOM2020.RDT @0x00866 /
     * @0x0082A). ROOM2020 fuehrt keine Zonenzeile, die Marken trugen die
     * Rueckfall-Nummer 0 = ROOM1000/z0 und standen damit im Stand des Nutzers auf
     * Blatt 6, von dessen Raeumen er keinen betreten hatte (seine 20 Besucht-Bits
     * nennen nur Raeume der Blaetter 0..5). */
    {  7,  0,  128,  117, 1,  49,  50, 1 },
    {  7,  1,  121,  192, 3,  50,  64, 0 },
    {  7,  1,  135,  188, 4,  50, 255, 0 },
    {  7,  1,  167,  115, 1,  50, 255, 0 },
    {  7,  1,  167,  187, 1,  50,  51, 1 },
    {  7,  2,  189,  160, 5,  62, 255, 0 },
    {  7,  2,  202,  160, 0,  62, 255, 0 },
    {  7,  2,  215,  160, 1,  62, 255, 0 },
    {  7,  3,  204,  111, 3,  55, 255, 0 },
    {  7,  3,  204,  128, 3,  63,  54, 1 },
    {  7,  3,  207,  104, 0,  55,  53, 1 },
    {  7,  3,  207,  128, 2,  55, 255, 0 },
    {  7,  4,  191,   57, 3,  53, 255, 0 },
    {  7,  4,  203,   75, 5,  53, 255, 0 },
    {  7,  4,  231,   76, 5,  53, 255, 0 },
    {  7,  6,  206,  106, 2,  52,  63, 1 },
    {  7,  6,  208,  128, 2,  52,  61, 1 },
    {  7,  8,   72,  164, 1,  56, 255, 0 },
    {  7,  8,   73,  175, 0,  56,  64, 1 },
    {  7,  9,  224,  175, 1,  60,  58, 0 },
    {  7, 10,  224,  128, 0,  58, 255, 0 },
    {  7, 10,  224,  137, 3,  58, 255, 0 },
    {  7, 10,  244,  155, 2,  58,  59, 1 },
    {  7, 12,  194,  128, 0,  54, 255, 0 },
    {  7, 12,  200,  136, 5,  61, 255, 0 },
    {  7, 12,  215,  137, 1,  61, 255, 0 },
    {  8,  0,  138,   72, 3,  65, 255, 0 },
    {  8,  0,  156,   64, 1,  65,  66, 1 },
    {  8,  0,  160,   89, 2,  65,  68, 1 },
    {  8,  0,  179,   60, 0,  65, 255, 0 },
    {  8,  2,  128,   73, 1,  70,  70, 1 },
    {  8,  2,  132,   77, 0,  70,  70, 1 },
    {  8,  2,  132,   79, 0,  70, 255, 0 },
    {  8,  2,  138,   73, 3,  70,  70, 1 },
    {  8,  3,  144,  121, 2,  68, 255, 0 },
    {  8,  3,  152,  128, 2,  68, 255, 0 },
    {  8,  7,  142,  164, 3,  71, 255, 0 },
    {  8,  7,  153,  176, 5,  71, 255, 0 },
    {  8,  8,  172,  125, 2,  72, 255, 0 },
    {  8,  8,  208,  150, 1,  72,  74, 1 },
    {  8,  9,  209,  128, 3,  73,  72, 1 },
    {  8, 11,  177,  163, 1,  75,  72, 1 },
    {  8, 12,  147,  124, 1,  69, 255, 0 },
    {  8, 12,  149,  116, 3,  69, 255, 0 },
    {  8, 12,  156,  113, 1,  69, 255, 0 },
    {  9,  0,  130,   87, 3,  76, 255, 0 },
    {  9,  0,  153,  104, 2,  76, 255, 0 },
    {  9,  0,  168,   86, 1,  76, 255, 0 },
    {  9,  1,  168,   87, 3,  71, 255, 0 },
    {  9,  1,  168,   92, 3,  77,  76, 1 },
    {  9,  1,  175,  100, 5,  71, 255, 0 },
    {  9,  1,  176,   89, 5,  71, 255, 0 },
    {  9,  3,  134,  121, 3,  79, 255, 0 },
    {  9,  3,  152,  104, 0,  79,  76, 1 },
    {  9,  3,  156,  143, 2,  79, 255, 0 },
    {  9,  3,  173,  125, 1,  79, 255, 0 },
    {  9,  4,   90,  120, 2,  81, 255, 0 },
    {  9,  5,   90,  100, 1,  80,  81, 1 },
    {  9,  5,   90,  119, 3,  80, 255, 0 },
    {  9,  6,   95,  141, 3,  82, 255, 0 },
    {  9,  7,  180,   98, 1,  84,  84, 0 },
    {  9,  7,  180,  123, 1,  84,  84, 1 },
    {  9,  7,  190,   91, 4,  78, 255, 0 },
    {  9,  7,  192,  119, 0,  78, 255, 0 },
    {  9,  7,  221,   98, 1,  84, 255, 0 },
    {  9,  9,  251,   90, 1,  83, 255, 0 },
    {  9, 11,  186,  134, 0,  86, 255, 0 },
    {  9, 11,  196,  168, 2,  86, 255, 0 },
    {  9, 14,  138,  179, 3,  85, 255, 0 },
    {  9, 14,  154,  174, 0,  94, 255, 0 },
    { 10,  0,  124,  114, 2,  87,  88, 1 },
    { 10,  0,  152,   85, 3,  87, 255, 0 },
    { 10,  1,  162,  115, 1,  88,  89, 1 },
    { 11,  0,  162,  133, 3,  88,  91, 1 },
    { 11,  0,  183,  157, 1,  88, 255, 0 },
    { 11,  1,  120,  126, 3,  91, 255, 0 },
    { 11,  2,  162,  115, 3,  90, 255, 0 },
    { 11,  3,  158,  158, 3,  71, 255, 0 },
    { 11,  3,  169,  170, 5,  71, 255, 0 },
    { 11,  3,  170,  159, 5,  71, 255, 0 },
    { 12,  4,  213,  132, 3,  96, 255, 0 },
    { 12,  4,  223,  109, 5,  96, 255, 0 },
    { 12,  4,  223,  132, 5,  96, 255, 0 },
    { 12,  4,  231,  109, 4,  96, 255, 0 },
    { 12,  4,  231,  132, 4,  96, 255, 0 },
    { 12,  4,  251,  110, 1,  96,  95, 1 },
    { 12,  4,  257,  132, 1,  96,  95, 0 },
    { 12,  5,  235,  126, 0,  98, 255, 0 },
    { 12,  6,  117,  167, 2,  97, 255, 0 },
    { 12,  7,  250,  118, 3,  99, 255, 0 },
    { 12,  7,  256,  110, 2,  95,  99, 1 },
    { 12,  7,  262,  117, 3,  99,  99, 1 },
    { 12,  7,  265,  118, 5,  99, 255, 0 },
    /* Runde 36 Spur M (Nutzer: "Im Treppenhaus 10A0 auf Etage B1 fehlen die Treppen
     * eingezeichnet"). GEMESSEN: 1F (Blatt 2 rect 6) und B2 (Blatt 1 rect 9) tragen je ZWEI
     * Treppenmarken, B1 (Blatt 0 rect 6, Runde 35 Spur G als Gast-Zeile Band 4 von Hand
     * angelegt) KEINE - der Generator kannte Band 4 -> Blatt 0 nicht. Dieselbe Regel wie der
     * Generator (gen_map_zones.py: jedes Treppenende auf das Blatt SEINES Bandes, gepaarte Enden
     * am gemeinsamen Weltpunkt, Glyph kind 4 = Achse Z, sce 13 - Handler LAB_800435cc liest z
     * @0x800435dc-e0): die beiden Treppenenden auf Band 4 in ROOM10A0.RDT sind
     *   Aot_set slot 6 sce 13 Band 4 @0x00E7C (24000,20100,2700,4500) = Lauf 6<->4 (von 1F)
     *   Aot_set slot 7 sce 13 Band 4 @0x00E90 (20100,19500,2700,2300) = Lauf 4<->3 (nach B2)
     * Mitten (25350,22350) / (21450,20650) mit der B1-Zeile (Bbox x17850..29100 z15550..28300
     * auf rect 6 (153,114,16,24), re15_map_zone_marker) -> (163,125) / (158,128); beide im
     * gemalten Innenraum (MAP01.PIX uv(128,16): Index 1 in x154..167 y115..133).
     * VORBILD RE2: jede Etage traegt ihr Treppenhaus MIT Treppe in der eigenen Kachel (RE2
     * Raumkacheln @0x800aaa38 + MAPS.PIX, z.B. Area 2 Kachel 7 vis 37 / Area 3 Kachel 6 vis 16,
     * Dossier R1). */
    /* Runde 39 Spur G: alte Blatt-0-Marke(n) hier entfernt - B1 neu, Marken am Tabellenende */
    /* Runde 39 Spur G: B1-Marken der Schema-Zeichnung (b1_plan.py). Tuer = auf der GEMEINSAMEN Wandlinie, laengs
     * Mittel der beiden Trigger-Mitten, 3 px von jeder Ecke (Balken +-2); zid = Gang, wo der Gang beteiligt ist
     * (der Gang ist auf B1 immer zuerst betreten). Tuer-Datensaetze: 1180 slot 2 @0x00A54 <-> 1190 slot 4 @0x028B2,
     * 1230 slot 4 @0x00CE6 (Ruhe-Satz slot 6 @0x00BC4 sce 0) <-> 1190 slot 6 @0x02B1C, 1180 slot 3 @0x00A74 <->
     * 11B0 slot 0 @0x00F64, 1180 slot 1 @0x00A34 <-> 10A0 slot 1 @0x00DDA/@0x00DFE, 1180 slot 0 @0x009EE/@0x00A12
     * <-> 11D0 slot 0 @0x011E2 / 1160 slot 0 @0x007C6, 11B0 slot 1 @0x00FAA <-> 11C0 slot 0 @0x01712. Treppen = Aot_set
     * sce 13 Band 4 @0x00E7C / @0x00E90 (ROOM10A0.RDT) = die Projektionen ihrer Mitten. Die beiden Wege nach ROOM1260
     * sind BODENLOECHER, keine Wandtueren, und bekommen keine Marke: 11B0 slot 5 @0x01010 (Trigger = Kreiszelle
     * @0x00EA4 mitten in der Werkstatt) und 11D0 slot 1 @0x01228 (Trigger = Kreiszelle @0x00A10 in der Kammer). */
    {  0, 255,  127,  115, 2,  25,  26, 0 },   /* Gang <-> 1190 Halle */
    {  0, 255,  146,  109, 2,  25, 110, 0 },   /* Gang (1230) <-> 1190 Ostkammer */
    {  0, 255,  158,   72, 1,  25,  28, 0 },   /* Gang <-> 11B0 */
    {  0, 255,  145,  153, 2,  25,  10, 0 },   /* Gang <-> 10A0 B1-Absatz */
    {  0, 255,  136,  173, 2,  25,  30, 0 },   /* Gang <-> 11D0 / 1160 */
    {  0, 255,  163,   94, 2,  28,  29, 0 },   /* 11B0 <-> 11C0 */
    {  0, 255,  154,  162, 4,  10, 255, 0 },   /* 10A0 Treppe 6<->4 (1F)  @0x00E7C */
    {  0, 255,  145,  164, 4,  10, 255, 0 },   /* 10A0 Treppe 4<->3 (B2)  @0x00E90 - Nachbesserung 1 (Abnahme-Mangel M2):
                                               * y165 -> y164. Projektion der Mitte (21450,20650) = (145,165); deren
                                               * unterste Sprosse (143..147,167) lag Pixel an Pixel auf der Unterkante
                                               * y168 der Absatz-Zeichnung (Zelle (141,153,18,16)). y164: Sprossen y162/
                                               * 164/166, eine Zeile Abstand zu y168; die Projektion bleibt im Kasten. */
};

/* INNENWAENDE in Karten-Pixeln: Linie (x0,y0)-(x1,y1) im Rechteck der Zone.
 * Quelle: die soliden Typ-1-SCA-Zellen des Bands - das SIND die Waende
 * (re15_collision.c: der Spieler laeuft im band-freien Komplement; an 3727
 * Nutzer-Standorten zu 97,1 % bestaetigt). Gezeichnet wird die Mittellinie,
 * begrenzt auf die gemalte Flaeche, und nur wo BEIDSEITS Raum liegt -
 * Aussenwaende malt die Kachel selbst. Nur fuer besuchte Zonen. */
typedef struct { unsigned char page, rect; short x0, y0, x1, y1;
                 unsigned char zid; } re15_map_wall_t;
static const re15_map_wall_t s_map_walls[] = {
    {  3,  5,  188,  122,  188,  144,  17 },
    {  3,  5,  189,  133,  212,  133,  17 },
    /* ⛔ GESTRICHEN (Runde 30, Nutzer 2026-09-27: "Roof ist irgendwie die Wand unten
     * blau"): (5, 1, 148, 155, 182, 155, 23) war KEINE Innenwand. Alle 35 Punkte
     * der Linie liegen auf Kachel-Index 4 - der vom Kuenstler GEMALTEN Suedwand von
     * Blatt 5 rect 1 (Rechteck @0x800764C8 = (148,101) 48x56 uv(0,32); Kachelzeile
     * v=86 in DATA/MAP06.PIX ab @Datei 0x2B00: 35 x Index 4), die Zeile darunter
     * (v=87 ab @0x2B80) traegt Index 0 = dort liegt kein Raum. Der Tabellenkopf
     * verlangt "nur wo BEIDSEITS Raum liegt". tools/gen_map_zones.py verwirft eine
     * solche Linie seitdem (Regel "liegt zu 100 % auf Index 4"); der Riegel
     * unit_map_innenwand prueft es fuer jede Zeile. Dossier karten-marken.md §4a. */
    {  8,  2,  127,   78,  131,   78,  70 },
    {  8,  2,  134,   78,  136,   78,  70 },
    {  9,  7,  213,  107,  221,  107,  84 },
    {  9,  7,  177,  116,  221,  116,  84 },
    {  9,  7,  213,  135,  221,  135,  84 },
    {  9,  7,  195,  119,  195,  126,  84 },
    {  9,  7,  213,  143,  221,  143,  84 },
};

/* TEILBEREICHE eines Rechtecks: wo Innenwaende einen Ort zerschneiden,
 * traegt jeder Teil seinen EIGENEN Zustand (aktuell/besucht). Weltbox
 * sagt, wo der Spieler drin ist; (tx,ty,tw,th) ist der Ausschnitt des
 * Rechtecks, der dafuer eingefaerbt wird. Leere Tabelle = jedes Rechteck
 * hat genau einen Zustand, wie bisher. */
typedef struct { unsigned char page, rect; int wx0, wz0, wx1, wz1;
                 short tx, ty, tw, th; unsigned char zid; } re15_map_teil_t;
static const re15_map_teil_t s_map_teile[] = {
    {  3,  5,   -2909,   -7522,   14950,    7087,  146,  114,  42,  31,  17 },
    {  3,  5,  -13521,   -3806,   -3686,    1320,  189,  122,  24,  11,  17 },
    {  3,  5,  -13521,    1961,   -3686,    7087,  189,  134,  24,  11,  17 },
    {  9,  7,   -5683,  -13414,   26019,   -4716,  173,  117,  49,  18,  84 },
    {  9,  7,   20532,     850,   29067,   11288,  213,   86,  14,  21,  84 },
    {  9,  7,   20532,  -26982,   29067,  -18632,  213,  144,  14,  17,  84 },
    {  9,  7,   20532,   -3672,   26019,     155,  213,  108,   9,   8,  84 },
    {  9,  7,   20532,  -17589,   26019,  -14458,  213,  136,   9,   7,  84 },
};

/* ETAGEN: Band -> (Kartenseite, Rechteck). Aus den Tueren des Raums
 * abgeleitet (Band der Tuer -> Seite des Zielraums), Ziel-Rechteck ueber
 * die gleiche Kachel-uv gefunden. Siehe tools/gen_map_zones.py. */
typedef struct { unsigned short room; unsigned char zone, band, page, rect; } re15_map_floor_t;
static const re15_map_floor_t s_map_floors[] = {
    { 0x1060, 0,  0,  2, 10 },
    { 0x1061, 0,  0,  2, 10 },
    { 0x1060, 0,  4,  3,  1 },
    { 0x1061, 0,  4,  3,  1 },
    { 0x1060, 0,  8,  4,  1 },
    { 0x1061, 0,  8,  4,  1 },
    { 0x1080, 0,  0,  2,  9 },
    { 0x1081, 0,  0,  2,  9 },
    { 0x1080, 0,  0,  3,  4 },
    { 0x1081, 0,  0,  3,  4 },
    { 0x1080, 0,  0,  4,  0 },
    { 0x1081, 0,  0,  4,  0 },
    { 0x1090, 0,  1,  2,   5 },
    { 0x1091, 0,  1,  2,   5 },
    { 0x1090, 0,  6,  3,   7 },
    { 0x1091, 0,  6,  3,   7 },
    { 0x10A0, 0,  1,  1,  9 },
    { 0x10A1, 0,  1,  1,  9 },
    { 0x10A0, 0,  8,  2,  6 },
    { 0x10A1, 0,  8,  2,  6 },
    { 0x10F0, 0,  0,  3,  9 },
    { 0x10F1, 0,  0,  3,  9 },
    { 0x10F0, 0,  1,  2,  8 },
    { 0x10F1, 0,  1,  2,  8 },
    { 0x1170, 1,  0,  4,  3 },
    { 0x1171, 1,  0,  4,  3 },
    { 0x1170, 1,  4,  5,  0 },
    { 0x1171, 1,  4,  5,  0 },
    { 0x11A0, 0,  0,  6,  2 },
    { 0x11A1, 0,  0,  6,  2 },
    /* Runde 36 Spur M: war Band 3 -> Blatt 0 rect 1 (jetzt die 1190-Haupthalle). ROOM11A0 liegt
     * auf beiden Baendern auf seiner Drains-Kachel Blatt 6 rect 2 (Zonen-Zeile s.o.). Zeile an
     * ihrem Platz geaendert (Pin unit_map_speichern_laden: Zeile 48 = ROOM50D0), neues
     * Etagen-Bit 26 in s_etage_bit (re15_map_zones.c), Bit 15 bleibt unbenutzt. */
    { 0x11A0, 0,  3,  6,  2 },
    { 0x11A1, 0,  3,  6,  2 },
    { 0x3080, 0,  0,  7,  8 },
    { 0x3081, 0,  0,  7,  8 },
    { 0x4020, 0,  0,  8,  2 },
    { 0x4021, 0,  0,  8,  2 },
    { 0x4020, 0,  0,  9, 13 },
    { 0x4021, 0,  0,  9, 13 },
    { 0x4020, 0,  0, 10,  3 },
    { 0x4021, 0,  0, 10,  3 },
    { 0x4070, 0,  0, 11,  3 },
    { 0x4071, 0,  0, 11,  3 },
    { 0x4070, 0,  8,  9,  1 },
    { 0x4071, 0,  8,  9,  1 },
    { 0x4070, 0, 12,  8,  7 },
    { 0x4071, 0, 12,  8,  7 },
    { 0x50D0, 0,  0, 11,  0 },
    { 0x50D1, 0,  0, 11,  0 },
    { 0x50D0, 0,  3, 10,  1 },
    { 0x50D1, 0,  3, 10,  1 },
    /* Runde 35 Spur G: ROOM10A0 B1-Absatz (Ankunft y -7200 aus 1180/1230 -> Band 4). AM ENDE
     * angehaengt: Pin unit_map_speichern_laden prueft Zeile 48 = ROOM50D0. */
    { 0x10A0, 0,  4,  0, 255 },   /* Runde 39 Spur G: Schema statt r6 (sonst malte rect_state die alte Kachel) */
    { 0x10A1, 0,  4,  0, 255 },
};

/* SCHEMA-ZEICHNUNGEN aus der KOLLISIONS-BOX. Fuer Zonen, denen kein
 * Karten-Rechteck des Originals zugeordnet werden konnte (rect == 255 in
 * s_map_zones). Massstab = Median der ausgelieferten Zeilen @0x800768b0
 * derselben Stage, Lage ueber eine Tuer zu einem gezeichneten Nachbarn.
 * ACHTUNG PORT-ERGAENZUNG: das Original zeichnet diese Raeume gar nicht,
 * und seine Kunst ist auch nicht massstabsgetreu zur Kollision. Siehe
 * tools/gen_map_zones.py, Abschnitt SCHEMA-ZEICHNUNG. */
/* Die AFFINE ABBILDUNG bildet Weltkoordinaten auf Kartenpixel ab:
 *     mx = (A*wx + B*wz) / 65536 + C ,  my = (D*wx + E*wz) / 65536 + F
 * Drehung und Spiegelung des Raums stecken in den Vorzeichen, der
 * Massstab im Betrag. A==0 && D==0 heisst: keine Abbildung (Rueckfall auf
 * die Bbox-Streckung in den Kasten). */
typedef struct { short x, y, w, h; unsigned short erste, n;
                 int a, b, c, d, e, f; } re15_map_synth_t;
static const re15_map_synth_t s_map_synth[] = {
    /* ROOM1000 - drei Schema-Kaesten oestlich des Flurs (karte-1000-1050.md §3.3).
     * Die Lage ist EINGEMESSEN, nicht gewaehlt: die Flurseite des Raums ist ueber die
     * drei Tueren nach ROOM1050 exakt verankert, und dort malt Blatt 2 unterhalb y=88
     * nichts. Reihenfolge = zone.synth-1: Ostraum / Nord-WC / Sued-WC. */
    /* ⛔ x=207, NICHT 206 (gemessen 2026-09-13, nachdem unit_map_synth die
     * Ueberlappung meldete): die Flur-Ostwand ist auf Blatt 2 DREI Pixel breit -
     * bemalt sind die Spalten 204, 205 und 206, ab 207 ist die Flaeche frei (jede
     * Kastenzeile getroffen, also eine durchgehende senkrechte Linie). Ein Kasten mit
     * linker Kante 206 haette die Wand mit seiner Fuellung uebermalt. Er setzt jetzt
     * an sie an; C wandert um denselben Pixel mit. */
    { 207,  89, 15, 33, 0, 1, -152, 0, 260, 0, 145, 122 },  /* ROOM1000 Ostraum */
    { 207, 123, 15, 16, 1, 1, -152, 0, 211, 0, 145, 135 },  /* ROOM1000 Nord-WC */
    { 207, 139, 15, 15, 2, 1, -152, 0, 211, 0, 145, 135 },  /* ROOM1000 Sued-WC */
    /* Runde 39 Spur G: B1 (Blatt 0), b1_plan.py - Affine mx = (146*x)>>16 + c, my = (-146*z)>>16 + f je Ort
     * (RE2-Massstab 1/450, s. B1-Kopf in s_map_zones), Kasten = Huelle der Zellen. */
    { 124,  67,  35, 107,  3, 6, 146, 0, 146, 0, -146, 137 },  /* synth 4:  Gang 1180/1230 */
    {  69, 115,  64,  37,  9, 1, 146, 0, 121, 0, -146,  93 },  /* synth 5:  1190 Halle */
    { 132, 109,  19,  32, 10, 1, 146, 0, 103, 0, -146,  84 },  /* synth 6:  1190 Ostkammer */
    { 158,  69,  65,  26, 11, 1, 146, 0, 219, 0, -146,  35 },  /* synth 7:  11B0 */
    { 158,  94,  90,  78, 12, 4, 146, 0, 220, 0, -146, 134 },  /* synth 8:  11C0 Ring */
    { 110, 162,  38,  27, 16, 3, 146, 0, 136, 0, -146, 137 },  /* synth 9:  11D0 = 1160 */
    { 141, 153,  18,  16, 19, 1, 146, 0,  98, 0, -146, 212 },  /* synth 10: 10A0 B1-Absatz */
    { 138, 180,  12,   9, 20, 1, 146, 0, 136, 0, -146, 137 },  /* synth 11: 11D0 Leiter-Kammer (Zone 1) */
    /* Runde 39 Spur G, Nachbesserung 2: ROOM1210 Suedwest-Kammer (Zone 1) auf B2. KEINE Affine (A = D = 0): der Marker
     * rechnet mit der Zeile der Zone = @0x800769B8 wie der Flur. Kasten/Zelle aus den Waenden (FUN_800473f8, Zeile
     * @0x800769B8): West @0x01BEC px 184..188 -> Linie x186 (= Ostwand r9, MAP02.PIX Index 4), Nord @0x01BF8 py 110..114
     * -> y112 (= Unterkante r5), Sued @0x01BE0 py 129..132 -> y129 (= Flur-Unterkante r3); Ost = die gemalte
     * Flur-Westwand x201 (r3 Spalte 14, @0x01C40 px 200..203) - die Zelle endet bei x200, ihre Ostkante entfaellt mit
     * gezeichnetem r3 (karte_r39.c s_kanten), der Durchgang y117..119 (@0x01BC8 endet z -20746 -> py116, @0x01C40 beginnt
     * z -22520 -> py120) oeffnet r3 (karte_r39.c s_korr). Keine Doppelwand: jede Linie liegt auf der gemalten. */
    { 186, 112,  15,  18, 21, 1,   0, 0,   0, 0,    0,   0 },  /* synth 12: ROOM1210 Suedwest-Kammer (Zone 1) */
};
typedef struct { short x, y, w, h; } re15_map_synth_cell_t;
static const re15_map_synth_cell_t s_map_synth_cells[] = {
    { 207,  89, 15, 33 }, { 207, 123, 15, 16 }, { 207, 139, 15, 15 },
    /* Runde 39 Spur G: B1-Zellen (b1_plan.py), x/y = linke obere Ecke, w/h inklusive der Wandlinien. Je Ort
     * DISJUNKT zerlegt: die Fuellung mischt halbtransparent (abe 1), eine Ueberdeckung mischte doppelt. */
    { 150,  67,   9,  35 }, { 124, 102,  35,   8 }, { 124, 110,   9,   6 }, { 150, 110,   9,  36 }, { 132, 146,  27,   8 },
    { 133, 154,   9,  20 },                                                     /* Gang (synth 4) */
    {  69, 115,  64,  37 },                                                     /* 1190 Halle (5) */
    { 132, 109,  19,  32 },                                                     /* 1190 Ostkammer (6) */
    { 158,  69,  65,  26 },                                                     /* 11B0 (7) */
    { 158,  94,  90,  22 }, { 180, 116,  14,  34 }, { 230, 116,  18,  34 }, { 180, 150,  68,  22 },   /* 11C0 (8) */
    { 110, 162,  16,  19 }, { 132, 173,  16,   8 }, { 110, 181,  29,   8 },   /* 11D0 (9) */
    { 141, 153,  18,  16 },                                                     /* 10A0 B1 (10) */
    { 138, 180,  12,   9 },                                                     /* 11D0 Kammer (11) */
    { 186, 112,  15,  18 },                                                     /* ROOM1210 Kammer (12), Index 21 */
};
/* ACHTUNG - ERSATZ-SEITENTABELLE. Das Original fuehrt fuer Blatt 3 (2F)
 * die Tabelle von Blatt 2 (1F): verschiedene Adressen (@0x8007636C /
 * @0x800763F0), byte-identische Eintraege, Blatt 3 nur ohne den elften.
 * Gemessen: baut man eine Seite nach ihrer Tabelle zusammen, kommt der
 * fertige Grundriss heraus, den dasselbe Blatt in seiner unteren Haelfte
 * traegt - Blatt 2 zu 93,0 %, Blatt 4 zu 98,7 %, Blatt 3 nur zu 57,3 %.
 * Die Zeilen hier sind aus den KACHELN DES 2F-BLATTS hergeleitet (Ecke +
 * Groesse auf 8 aufgerundet, Lage per Schablonensuche im Grundriss des
 * Blatts, 8/10 zu 100,0 %); Zusammenbau 93,1 %. Herleitung und Belege:
 * tools/gen_map_zones.py (RECT_FIX) und analysis/karte_grundriss/BEFUND.md.
 * page == 255 beendet die Liste. */
typedef struct { unsigned char page, u, v; short x, y, w, h; } re15_map_rectfix_t;
static const re15_map_rectfix_t s_map_rectfix[] = {
    {   3, 128,  16,  102,  116,  40,  40 },
    {   3, 168,  16,  118,  134,  24,  24 },
    {   3, 192,  16,  135,  155,  56,  32 },
    {   3,   0,  32,  135,   76,  72,  88 },
    {   3, 168,  40,  109,  134,  16,  16 },
    {   3, 184,  48,  146,  114,  72,  32 },
    {   3,  72,  64,  186,  114,  40,  48 },
    {   3, 112,  74,  179,   67,  48,  48 },
    {   3, 160,  80,  102,   76,  48,  48 },
    {   3, 208,  80,  156,   76,  48,  40 },
};
