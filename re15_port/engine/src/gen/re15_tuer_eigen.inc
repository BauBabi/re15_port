/* Erzeugt von re15_port/tools/tueren/tuer_archiv_bauen.py - NICHT von Hand aendern.
 * Runde 33 / Thema T (analysis/befunde_runde33/tueren_rest_plan.md, tueren_rest_pilot.md).
 * ⛔ PORT-WAHL, KEINE Original-Adresse: port-eigene Tuerarchive im RE2-Aufbau (Tonteil, SCD, MD1
 * bytegleich dem Basis-Archiv, nur die TIM neu) und ihre Zuordnung zu RE1.5-Tuerseiten.
 * Dateien: shared_assets/RE15DOOR/<kennung>.DO2; Groessen wie die RE2-Tabelle @0x8009a520
 * (Tonteil, Modellteil, Sektor), fnv = FNV-1a 32 ueber die ganze Datei. */
static const re15_tuer_eigen_t re15_tuer_eigen[30] = {
    /* 1: P07G = DOOR07 + Textur 'Lueftungsschlitze + Flecken weg, Farbe = Median der RE1.5-Blaetter (G1); Stufe 2: silbernes Rechteckschild unter dem Druecker gemalt (gemessen, Pilot-Punkt a)' (Ton F3 (06 07 08 22 2F, Blechtuer)) */
    { "P07G", 0x07, 19608, 35428, 10, 55908, 0x982915E5u },
    /* 2: P07T = DOOR07 + Textur 'wie P07G, Farbe = Median der drei Treppenhausseiten ROOM1060 (hell graugruen gemalt, Gegenseiten dunkel; Pilot-Punkt c: je Seite ein Archiv)' (Ton F3 (06 07 08 22 2F, Blechtuer)) */
    { "P07T", 0x07, 19608, 35428, 10, 55908, 0xD9329A38u },
    /* 3: P06F = DOOR06 + Textur 'DOOR08-Blatt: Nieten/Rost weg, umlaufende gerundete Randnut + breit gerahmte Felder, hellgrau' (Ton F3 (Stahltuer)) */
    { "P06F", 0x06, 19608, 37004, 10, 57484, 0xBFC0EFDEu },
    /* 4: P06U = DOOR06 + Textur 'wie P06F, aber gepraegte UNGLEICHE Felder (oben hoch, unten fast quadratisch, S188)' (Ton F3 (Stahltuer)) */
    { "P06U", 0x06, 19608, 37004, 10, 57484, 0x7F0166E4u },
    /* 5: P1B3 = DOOR1B + Textur 'DOOR23-Blatt (achteckiges Profil + Ausbuchtung) je Fluegel; Griff-Tausch Spender DOOR23 (Riegelstange quer)' (Ton F5 (1B 30, Doppeltuer)) */
    { "P1B3", 0x1B, 21176, 40744, 11, 63272, 0xA734099Fu },
    /* 6: P1BD = DOOR1B + Textur 'wie P1B3, Farbe der ROOM2070-Seiten (kaltes blaugruenes Licht, dunkel gemalt) - je Seite ein Archiv' (Ton F5 (1B 30, Doppeltuer)) */
    { "P1BD", 0x1B, 21176, 40744, 11, 63272, 0xB6ADC0ACu },
    /* 7: P1DG = DOOR1D + Textur 'Lueftungsgitter + Griffkasten weg, glattes Blech, Farbe nach RE1.5 (graugruen/blaugrau)' (Ton eigen DOOR1D (Blech-Doppeltuer, V2/V3)) */
    { "P1DG", 0x1D, 23928, 36100, 12, 60676, 0x9544905Eu },
    /* 8: P1DK = DOOR1D + Textur 'Stahlrahmen je Fluegel mit DREI Feldern ohne Glas (DOOR1A-Teilung, Kartenleser weg), dunkelgrau (T014)' (Ton eigen DOOR1D) */
    { "P1DK", 0x1D, 23928, 36100, 12, 60676, 0x369CDFD5u },
    /* 9: P1DL = DOOR1D + Textur 'Stahlrahmen je Fluegel mit hohem Feld oben + Querriegel + unterem Feld (T045), grau' (Ton eigen DOOR1D) */
    { "P1DL", 0x1D, 23928, 36100, 12, 60676, 0x7740C4F4u },
    /* 10: P04B = DOOR04 + Textur 'Blau -> braunes Holz (Farbton, Lage der Kassetten und Maserung bleiben)' (Ton F2 (01 04 09 11, Holz)) */
    { "P04B", 0x04, 23928, 44984, 12, 69560, 0xC23A92B4u },
    /* 11: P0CD = DOOR0C + Textur 'helles gelbbraunes Holz, je Fluegel hohes schmales Drahtglasfenster, Panikstange gemalt' (Ton eigen DOOR0C (Holz-Doppeltuer mit Glas)) */
    { "P0CD", 0x0C, 23880, 36892, 12, 61468, 0xBCFF74C5u },
    /* 12: P16M = DOOR16 + Textur 'Silberrohr -> Messing (goldgelb, ROOM12607.bmp), Lichtverlauf der Rohre bleibt' (Ton eigen DOOR16 (4 Sprossen, kein Schliesston)) */
    { "P16M", 0x16, 11000, 46572, 6, 58860, 0xF3556E1Cu },
    /* 13: P16R = DOOR16 + Textur 'Silberrohr -> dunkles Rostbraun (ROOM11A00.bmp)' (Ton eigen DOOR16) */
    { "P16R", 0x16, 11000, 46572, 6, 58860, 0x36589A6Au },
    /* 14: P1EL = DOOR1E + Textur 'senkrechte Staebe -> waagerechte Lamellen (Durchsicht-Texel 0 wie im Original)' (Ton eigen DOOR1E (Klappe)) */
    { "P1EL", 0x1E, 22872, 48908, 12, 73484, 0x22695894u },
    /* 15: P1EU = DOOR1E + Textur 'Staebe nur im unteren Drittel, oben dunkle Oeffnung' (Ton eigen DOOR1E) */
    { "P1EU", 0x1E, 22872, 48908, 12, 73484, 0xE8C27496u },
    /* 16: P25G = DOOR25 + Textur 'Schild SHAFT TYPE-L, Warnaufkleber, Rippen weg -> glattes graues Blech' (Ton eigen DOOR25 (Hubtuer)) */
    { "P25G", 0x25, 17624, 34300, 9, 52732, 0x1F709C51u },
    /* 17: P14A = DOOR14 + Textur 'groebere Rauten, hellgrauer Rahmen' (Ton eigen DOOR14 (Schiebe-Gittertor)) */
    { "P14A", 0x14, 24024, 39128, 12, 63704, 0x8701F033u },
    /* 18: P2DS = DOOR2D + Textur 'Bedientafel + rote Lampe weg (Texel 0 = nicht gezeichnet), Warnrand bleibt' (Ton eigen DOOR2D (Hubbuehne)) */
    { "P2DS", 0x2D, 22712, 53668, 12, 78244, 0xC9CE31CDu },
    /* 19: P07R = DOOR07 + Textur 'DOOR22-Blatt ohne Nietrand und ohne gemalten D-Riegel, rostbraun' (Ton F3 (= DOOR22-Ton, bytegleich)) */
    { "P07R", 0x07, 19608, 35428, 10, 55908, 0xEE008E6Du },
    /* 20: P1AP = DOOR1A + Textur 'DOOR23-Profil, Ausbuchtung am Griff gerade gezogen, braun' (Ton F4 (15 1A 23 = DOOR23-Ton)) */
    { "P1AP", 0x1A, 21304, 37628, 11, 60156, 0xAF6CD1A1u },
    /* 21: P1DO = DOOR1D + Textur 'Lueftungsgitter -> schmales dunkles Schild + gelbes Warndreieck, orange' (Ton eigen DOOR1D) */
    { "P1DO", 0x1D, 23928, 36100, 12, 60676, 0x73A9EA71u },
    /* 22: P26W = DOOR26 + Textur 'einteiliges Schott dunkel, zwei gelb-schwarze Warnstreifen, TYPE-P-Schilder weg, Handrad rot' (Ton F7 (26 31, Schott)) */
    { "P26W", 0x26, 21832, 58808, 11, 81336, 0xB7A1B23Eu },
    /* 23: P24B = DOOR24 + Textur 'Aushang weg, Sichtfenster flacher/schmaler' (Ton F6 (24 29)) */
    { "P24B", 0x24, 22728, 42996, 12, 67572, 0x7F07B2C7u },
    /* 24: P07D = DOOR07 + Textur 'schwarzgrau glatt, kleines Feld mit abgeschnittener Ecke, Piktogramm Frau, blaues Schild' (Ton F3) */
    { "P07D", 0x07, 19608, 35428, 10, 55908, 0x137A5E10u },
    /* 25: P07H = DOOR07 + Textur 'wie P07D, Piktogramm Mann' (Ton F3) */
    { "P07H", 0x07, 19608, 35428, 10, 55908, 0xD68E49EDu },
    /* 26: P27S = DOOR27 + Textur 'Laborseite ROOM5060: linkes Feld gelbes Strahlenwarnschild, rechts Fensterkasten + rotes Dreieck, blaugrau' (Ton eigen DOOR27 (Labor-Schiebetuer)) */
    { "P27S", 0x27, 23176, 53932, 12, 78508, 0x08FF493Fu },
    /* 27: P27K = DOOR27 + Textur 'Gangseite ROOM5040/5120: quadratisches Sichtfenster, rotes Dreieck, gelb-schwarzer Aufkleber, schmales Feld rechts (anders gemalt als die Laborseite: je Seite ein Archiv)' (Ton eigen DOOR27 (Labor-Schiebetuer)) */
    { "P27K", 0x27, 23176, 53932, 12, 78508, 0xD8E7EB92u },
    /* 28: P27O = DOOR27 + Textur 'Gangseite ROOM5120 (S305): Malerei wie P27K, aber orange gemalt (Raumlicht) - je Seite ein Archiv' (Ton eigen DOOR27 (Labor-Schiebetuer)) */
    { "P27O", 0x27, 23176, 53932, 12, 78508, 0x3976B3B5u },
    /* 29: P1AZ = DOOR1A + Textur 'zwei Felder + senkrechte Rahmenleiste rechts + dunkles Rechteck oben, graugruen' (Ton F4) */
    { "P1AZ", 0x1A, 21304, 37628, 11, 60156, 0x3683DE80u },
    /* 30: P07M = DOOR07 + Textur 'Holz-/Rostrahmen, oben + unten Maschendraht (DOOR14-Raute, Loecher Texel 0), Mittelriegel' (Ton F3) */
    { "P07M", 0x07, 19608, 35428, 10, 55908, 0xC00248C3u },
};

/* Zuordnung RE1.5-Tuerseite -> Port-Archiv (Spalten wie gen/tuer_zuordnung.inc; re2_nr = Basis-Archiv
 * = var 15, eigen = Index+1 in re15_tuer_eigen[]). Variante nach tueren_02_re2.md 2.4 (Herkunft je Zeile).
 * eigen = 0 in dieser Tabelle: G12-Durchgang ohne Tuerblatt -> RE2-Archiv OHNE Objekt (DOOR36, Blende + Ton),
 * Wahl nach Huellkurve (tools/tueren/tuer_g12_ton.py, analysis/befunde_runde33/tueren_rest_bau.md). */
static const re15_tuer_zeile_t re15_tuer_zeilen_eigen[220] = {
    /* S000 T002 ROOM1000 -> ROOM1050 | ROOM1000.RDT@0xBBE main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1000, 0, 0,  22850, -13400,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE, 1 },
    /* S001 T000 ROOM1000 -> ROOM1050 | ROOM1000.RDT@0xBDE main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1000, 0, 0,   1700,    100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 1, 0, 0x00BDE, 1 },
    /* S002 T001 ROOM1000 -> ROOM1050 | ROOM1000.RDT@0xBFE main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1000, 0, 0,   1700,   3200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 2, 1, 0x00BFE, 1 },
    /* S000 T002 ROOM1001 -> ROOM1050 | ROOM1001.RDT@0xBBE main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1001, 0, 0,  22850, -13400,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE, 1 },
    /* S001 T000 ROOM1001 -> ROOM1050 | ROOM1001.RDT@0xBDE main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1001, 0, 0,   1700,    100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 1, 0, 0x00BDE, 1 },
    /* S002 T001 ROOM1001 -> ROOM1050 | ROOM1001.RDT@0xBFE main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1001, 0, 0,   1700,   3200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 2, 1, 0x00BFE, 1 },
    /* S003 T004 ROOM1010 -> ROOM1020 | ROOM1010.RDT@0x8AE main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1010, 0, 0,   4650,   6900,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 3, 4, 0x008AE, 1 },
    /* S004 T003 ROOM1010 -> ROOM1020 | ROOM1010.RDT@0x8CE main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1010, 0, 0,   4650,  -3950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 4, 3, 0x008CE, 1 },
    /* S003 T004 ROOM1011 -> ROOM1020 | ROOM1011.RDT@0x8AE main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1011, 0, 0,   4650,   6900,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 3, 4, 0x008AE, 1 },
    /* S004 T003 ROOM1011 -> ROOM1020 | ROOM1011.RDT@0x8CE main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1011, 0, 0,   4650,  -3950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 4, 3, 0x008CE, 1 },
    /* S005 T003 ROOM1020 -> ROOM1010 | ROOM1020.RDT@0x1C82 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1020, 0, 0,  -2750,  -7250,   750,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 5, 3, 0x01C82, 1 },
    /* S006 T004 ROOM1020 -> ROOM1010 | ROOM1020.RDT@0x1CA2 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1020, 0, 0,  -2950, -18450,   750,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 6, 4, 0x01CA2, 1 },
    /* S005 T003 ROOM1021 -> ROOM1010 | ROOM1021.RDT@0x1C82 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1021, 0, 0,  -2750,  -7250,   750,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 5, 3, 0x01C82, 1 },
    /* S006 T004 ROOM1021 -> ROOM1010 | ROOM1021.RDT@0x1CA2 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1021, 0, 0,  -2950, -18450,   750,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 6, 4, 0x01CA2, 1 },
    /* S010 T006 ROOM1030 -> ROOM1040 | ROOM1030.RDT@0x1CAA main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1030, 0, 0, -26900,  -3650,   600,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 10, 6, 0x01CAA, 1 },
    /* S010 T006 ROOM1031 -> ROOM1040 | ROOM1031.RDT@0x1D18 main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1031, 0, 0, -26900,  -3650,   600,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 10, 6, 0x01D18, 1 },
    /* S013 T011 ROOM1040 -> ROOM1080 | ROOM1040.RDT@0x1096 main00 Slot 0 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x1040, 0, 0, -21786, -10300,  1150,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 13, 11, 0x01096, 16 },
    /* S014 T006 ROOM1040 -> ROOM1030 | ROOM1040.RDT@0x10B8 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1040, 0, 0, -19650,  -3800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 14, 6, 0x010B8, 1 },
    /* S015 T012 ROOM1040 -> ROOM1060 | ROOM1040.RDT@0x10D8 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1040, 0, 0, -19650, -13000,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 15, 12, 0x010D8, 1 },
    /* S013 T011 ROOM1041 -> ROOM1080 | ROOM1041.RDT@0x1096 main00 Slot 0 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x1041, 0, 0, -21786, -10300,  1150,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 13, 11, 0x01096, 16 },
    /* S014 T006 ROOM1041 -> ROOM1030 | ROOM1041.RDT@0x10B8 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1041, 0, 0, -19650,  -3800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 14, 6, 0x010B8, 1 },
    /* S015 T012 ROOM1041 -> ROOM1060 | ROOM1041.RDT@0x10D8 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1041, 0, 0, -19650, -13000,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 15, 12, 0x010D8, 1 },
    /* S018 T001 ROOM1050 -> ROOM1000 | ROOM1050.RDT@0xAFA main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1050, 0, 0,  13700,   9200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 18, 1, 0x00AFA, 1 },
    /* S019 T000 ROOM1050 -> ROOM1000 | ROOM1050.RDT@0xB1A main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1050, 0, 0,  13700,   6100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 19, 0, 0x00B1A, 1 },
    /* S020 T002 ROOM1050 -> ROOM1000 | ROOM1050.RDT@0xB3A main00 Slot 3 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1050, 0, 0,  13700, -13450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 20, 2, 0x00B3A, 1 },
    /* S021 T013 ROOM1050 -> ROOM10A0 | ROOM1050.RDT@0xB5A main00 Slot 4 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1050, 0, 0,  17200, -13700,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 21, 13, 0x00B5A, 1 },
    /* S022 T014 ROOM1050 -> ROOM1090 | ROOM1050.RDT@0xB7A main00 Slot 5 | P1DK (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x1050, 0, 0,  19850, -23400,  1750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 22, 14, 0x00B7A, 8 },
    /* S018 T001 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xAFA main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700,   9200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 18, 1, 0x00AFA, 1 },
    /* S019 T000 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xB1A main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700,   6100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 19, 0, 0x00B1A, 1 },
    /* S020 T002 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xB3A main00 Slot 3 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700, -13450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 20, 2, 0x00B3A, 1 },
    /* S021 T013 ROOM1051 -> ROOM10A0 | ROOM1051.RDT@0xB5A main00 Slot 4 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  17200, -13700,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 21, 13, 0x00B5A, 1 },
    /* S022 T014 ROOM1051 -> ROOM1090 | ROOM1051.RDT@0xB7A main00 Slot 5 | P1DK (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x1051, 0, 0,  19850, -23400,  1750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 22, 14, 0x00B7A, 8 },
    /* S023 T015 ROOM1060 -> ROOM1120 | ROOM1060.RDT@0xD12 main00 Slot 0 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 8,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 23, 15, 0x00D12, 2 },
    /* S024 T016 ROOM1060 -> ROOM10C0 | ROOM1060.RDT@0xD32 main00 Slot 1 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 4,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 24, 16, 0x00D32, 2 },
    /* S025 T012 ROOM1060 -> ROOM1040 | ROOM1060.RDT@0xD52 main00 Slot 2 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 0,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 25, 12, 0x00D52, 2 },
    /* S023 T015 ROOM1061 -> ROOM1120 | ROOM1061.RDT@0xD12 main00 Slot 0 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 8,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 23, 15, 0x00D12, 2 },
    /* S024 T016 ROOM1061 -> ROOM10C0 | ROOM1061.RDT@0xD32 main00 Slot 1 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 4,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 24, 16, 0x00D32, 2 },
    /* S025 T012 ROOM1061 -> ROOM1040 | ROOM1061.RDT@0xD52 main00 Slot 2 | P07T (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 0,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 25, 12, 0x00D52, 2 },
    /* S030 T014 ROOM1090 -> ROOM1050 | ROOM1090.RDT@0x211A main00 Slot 0 | P1DK (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1090, 0, 1, -10080,   3060,  2350,   800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 30, 14, 0x0211A, 8 },
    /* S031 T019 ROOM1090 -> ROOM1100 | ROOM1090.RDT@0x213A main00 Slot 1 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x1090, 0, 6,  -5820, -18690,  2100,  2750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 31, 19, 0x0213A, 0 },
    /* S030 T014 ROOM1091 -> ROOM1050 | ROOM1091.RDT@0x211A main00 Slot 0 | P1DK (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1091, 0, 1, -10080,   3060,  2350,   800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 30, 14, 0x0211A, 8 },
    /* S031 T019 ROOM1091 -> ROOM1100 | ROOM1091.RDT@0x213A main00 Slot 1 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x1091, 0, 6,  -5820, -18690,  2100,  2750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 31, 19, 0x0213A, 0 },
    /* S033 T013 ROOM10A0 -> ROOM1050 | ROOM10A0.RDT@0xDB2 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A0, 0, 8,  27050,  24900,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 33, 13, 0x00DB2, 1 },
    /* S034 T022 ROOM10A0 -> ROOM1180 | ROOM10A0.RDT@0xDDA main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A0, 0, 4,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 34, 22, 0x00DDA, 1 },
    /* S035 T022 ROOM10A0 -> ROOM1230 | ROOM10A0.RDT@0xDFE main00 Slot 1 | P07G (Basis DOOR07) V1 (Zwilling von S034) */
    { 0x10A0, 0, 4,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 35, 22, 0x00DFE, 1 },
    /* S036 T021 ROOM10A0 -> ROOM11E0 | ROOM10A0.RDT@0xE20 main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A0, 0, 1,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 36, 21, 0x00E20, 1 },
    /* S033 T013 ROOM10A1 -> ROOM1050 | ROOM10A1.RDT@0xDB2 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A1, 0, 8,  27050,  24900,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 33, 13, 0x00DB2, 1 },
    /* S034 T022 ROOM10A1 -> ROOM1180 | ROOM10A1.RDT@0xDDA main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A1, 0, 4,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 34, 22, 0x00DDA, 1 },
    /* S035 T022 ROOM10A1 -> ROOM1230 | ROOM10A1.RDT@0xDFE main00 Slot 1 | P07G (Basis DOOR07) V1 (Zwilling von S034) */
    { 0x10A1, 0, 4,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 35, 22, 0x00DFE, 1 },
    /* S036 T021 ROOM10A1 -> ROOM11E0 | ROOM10A1.RDT@0xE20 main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x10A1, 0, 1,  21250,  26350,  1000,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 36, 21, 0x00E20, 1 },
    /* S038 T023 ROOM10B0 -> ROOM1260 | ROOM10B0.RDT@0x16F2 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x10B0, 0, 0, -21370,   4610,   750,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 38, 23, 0x016F2, 12 },
    /* S038 T023 ROOM10B1 -> ROOM1260 | ROOM10B1.RDT@0x16F2 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x10B1, 0, 0, -21370,   4610,   750,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 38, 23, 0x016F2, 12 },
    /* S039 T016 ROOM10C0 -> ROOM1060 | ROOM10C0.RDT@0xE42 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10C0, 0, 0,   -600,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 39, 16, 0x00E42, 1 },
    /* S040 T017 ROOM10C0 -> ROOM1080 | ROOM10C0.RDT@0xE82 main00 Slot 1 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x10C0, 0, 0,   1500,   6400,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 40, 17, 0x00E82, 16 },
    /* S041 T025 ROOM10C0 -> ROOM10D0 | ROOM10C0.RDT@0xEA4 main00 Slot 2 | P0CD (Basis DOOR0C) V1 (0C beide Fluegel Seite A weg/hin) */
    { 0x10C0, 0, 0,  -7600,  -2250,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0C, 1, 0, 0xFF, 41, 25, 0x00EA4, 11 },
    /* S039 T016 ROOM10C1 -> ROOM1060 | ROOM10C1.RDT@0xE42 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10C1, 0, 0,   -600,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 39, 16, 0x00E42, 1 },
    /* S040 T017 ROOM10C1 -> ROOM1080 | ROOM10C1.RDT@0xE82 main00 Slot 1 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x10C1, 0, 0,   1500,   6400,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 40, 17, 0x00E82, 16 },
    /* S041 T025 ROOM10C1 -> ROOM10D0 | ROOM10C1.RDT@0xEA4 main00 Slot 2 | P0CD (Basis DOOR0C) V1 (0C beide Fluegel Seite A weg/hin) */
    { 0x10C1, 0, 0,  -7600,  -2250,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0C, 1, 0, 0xFF, 41, 25, 0x00EA4, 11 },
    /* S044 T025 ROOM10D0 -> ROOM10C0 | ROOM10D0.RDT@0x11B6 main00 Slot 15 | P0CD (Basis DOOR0C) V0 (0C beide Fluegel Seite B weg/hin) */
    { 0x10D0, 0, 0,   8650,  11400,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0C, 0, 0, 0xFF, 44, 25, 0x011B6, 11 },
    /* S045 T026 ROOM10D0 -> ROOM1100 | ROOM10D0.RDT@0x11D6 main00 Slot 16 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x10D0, 0, 0, -19300,  24000,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 45, 26, 0x011D6, 7 },
    /* S044 T025 ROOM10D1 -> ROOM10C0 | ROOM10D1.RDT@0x11B6 main00 Slot 15 | P0CD (Basis DOOR0C) V0 (0C beide Fluegel Seite B weg/hin) */
    { 0x10D1, 0, 0,   8650,  11400,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0C, 0, 0, 0xFF, 44, 25, 0x011B6, 11 },
    /* S045 T026 ROOM10D1 -> ROOM1100 | ROOM10D1.RDT@0x11D6 main00 Slot 16 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x10D1, 0, 0, -19300,  24000,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 45, 26, 0x011D6, 7 },
    /* S048 T029 ROOM10F0 -> ROOM1090 | ROOM10F0.RDT@0xF52 main00 Slot 1 | P1EL (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x10F0, 0, 1,  -4750,  -1750,   600,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 48, 29, 0x00F52, 14 },
    /* S048 T029 ROOM10F1 -> ROOM1090 | ROOM10F1.RDT@0xF52 main00 Slot 1 | P1EL (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x10F1, 0, 1,  -4750,  -1750,   600,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 48, 29, 0x00F52, 14 },
    /* S049 T026 ROOM1100 -> ROOM10D0 | ROOM1100.RDT@0x99A main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1100, 0, 0, -21330, -24790,   400,  1250, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 49, 26, 0x0099A, 7 },
    /* S049 T026 ROOM1101 -> ROOM10D0 | ROOM1101.RDT@0x99A main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1101, 0, 0, -21330, -24790,   400,  1250, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 49, 26, 0x0099A, 7 },
    /* S056 T015 ROOM1120 -> ROOM1060 | ROOM1120.RDT@0xC76 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1120, 0, 0,   -700,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 56, 15, 0x00C76, 1 },
    /* S057 T018 ROOM1120 -> ROOM1080 | ROOM1120.RDT@0xCB6 main00 Slot 1 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x1120, 0, 0,   1300,   6400,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 57, 18, 0x00CB6, 16 },
    /* S056 T015 ROOM1121 -> ROOM1060 | ROOM1121.RDT@0xC76 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1121, 0, 0,   -700,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 56, 15, 0x00C76, 1 },
    /* S057 T018 ROOM1121 -> ROOM1080 | ROOM1121.RDT@0xCB6 main00 Slot 1 | P25G (Basis DOOR25) V0 (DOOR25 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x1121, 0, 0,   1300,   6400,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 57, 18, 0x00CB6, 16 },
    /* S059 T035 ROOM1130 -> ROOM1140 | ROOM1130.RDT@0x88E main00 Slot 0 | P04B (Basis DOOR04) V2 (Fluegel rechts -> V2 (T2 2.4 Punkt 2)) */
    { 0x1130, 0, 0,   -300, -13900,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x04, 2, 0, 0xFF, 59, 35, 0x0088E, 10 },
    /* S059 T035 ROOM1131 -> ROOM1140 | ROOM1131.RDT@0x88E main00 Slot 0 | P04B (Basis DOOR04) V2 (Fluegel rechts -> V2 (T2 2.4 Punkt 2)) */
    { 0x1131, 0, 0,   -300, -13900,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x04, 2, 0, 0xFF, 59, 35, 0x0088E, 10 },
    /* S063 T035 ROOM1140 -> ROOM1130 | ROOM1140.RDT@0xA52 main00 Slot 0 | P04B (Basis DOOR04) V2 (Fluegel rechts -> V2 (T2 2.4 Punkt 2)) */
    { 0x1140, 0, 0,  -8600, -17750,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x04, 2, 0, 0xFF, 63, 35, 0x00A52, 10 },
    /* S063 T035 ROOM1141 -> ROOM1130 | ROOM1141.RDT@0xA52 main00 Slot 0 | P04B (Basis DOOR04) V2 (Fluegel rechts -> V2 (T2 2.4 Punkt 2)) */
    { 0x1141, 0, 0,  -8600, -17750,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x04, 2, 0, 0xFF, 63, 35, 0x00A52, 10 },
    /* S078 T022 ROOM1180 -> ROOM10A0 | ROOM1180.RDT@0xA34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 78, 22, 0x00A34, 1 },
    /* S080 T045 ROOM1180 -> ROOM11B0 | ROOM1180.RDT@0xA74 main00 Slot 3 | P1DL (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x1180, 0, 0,   5706,  28874,   436,  2311, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 80, 45, 0x00A74, 9 },
    /* S078 T022 ROOM1181 -> ROOM10A0 | ROOM1181.RDT@0xA34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 78, 22, 0x00A34, 1 },
    /* S080 T045 ROOM1181 -> ROOM11B0 | ROOM1181.RDT@0xA74 main00 Slot 3 | P1DL (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x1181, 0, 0,   5706,  28874,   436,  2311, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 80, 45, 0x00A74, 9 },
    /* S087 T048 ROOM11A0 -> ROOM2070 | ROOM11A0.RDT@0xF5A main00 Slot 0 | P07M (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A0, 0, 0,   6500,  -3850,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 87, 48, 0x00F5A, 30 },
    /* S088 T047 ROOM11A0 -> ROOM3000 | ROOM11A0.RDT@0xF7C main00 Slot 1 | P16R (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x11A0, 0, 1,      0,  24950,   650,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 88, 47, 0x00F7C, 13 },
    /* S089 T049 ROOM11A0 -> ROOM11A0 | ROOM11A0.RDT@0xF9C main00 Slot 2 | P07R (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A0, 0, 1,  15300,   2750,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 89, 49, 0x00F9C, 19 },
    /* S090 T049 ROOM11A0 -> ROOM11A0 | ROOM11A0.RDT@0xFC6 main00 Slot 3 | P07R (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A0, 0, 3,   7500,  11500,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 90, 49, 0x00FC6, 19 },
    /* S091 T050 ROOM11A0 -> ROOM20A0 | ROOM11A0.RDT@0x1006 main00 Slot 4 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A0, 0, 1,  29000,   1300,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 91, 50, 0x01006, 20 },
    /* S087 T048 ROOM11A1 -> ROOM2070 | ROOM11A1.RDT@0xF5A main00 Slot 0 | P07M (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A1, 0, 0,   6500,  -3850,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 87, 48, 0x00F5A, 30 },
    /* S088 T047 ROOM11A1 -> ROOM3000 | ROOM11A1.RDT@0xF7C main00 Slot 1 | P16R (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x11A1, 0, 1,      0,  24950,   650,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 88, 47, 0x00F7C, 13 },
    /* S089 T049 ROOM11A1 -> ROOM11A0 | ROOM11A1.RDT@0xF9C main00 Slot 2 | P07R (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A1, 0, 1,  15300,   2750,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 89, 49, 0x00F9C, 19 },
    /* S090 T049 ROOM11A1 -> ROOM11A0 | ROOM11A1.RDT@0xFC6 main00 Slot 3 | P07R (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A1, 0, 3,   7500,  11500,  1100,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 90, 49, 0x00FC6, 19 },
    /* S091 T050 ROOM11A1 -> ROOM20A0 | ROOM11A1.RDT@0x1006 main00 Slot 4 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11A1, 0, 1,  29000,   1300,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 91, 50, 0x01006, 20 },
    /* S092 T045 ROOM11B0 -> ROOM1180 | ROOM11B0.RDT@0xF64 main00 Slot 0 | P1DL (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11B0, 0, 0, -27830, -16890,  1600,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 92, 45, 0x00F64, 9 },
    /* S093 T045 ROOM11B0 -> ROOM1230 | ROOM11B0.RDT@0xF88 main00 Slot 0 | P1DL (Basis DOOR1D) V3 (Zwilling von S092) */
    { 0x11B0, 0, 0, -27830, -16890,  1600,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 93, 45, 0x00F88, 9 },
    /* S095 T052 ROOM11B0 -> ROOM1260 | ROOM11B0.RDT@0x1010 main00 Slot 5, ROOM11B0.RDT@0x1034 main00 Slot 5 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11B0, 0, 0,    -62, -20288,   833,   833, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 95, 52, 0x01010, 12 },
    /* S092 T045 ROOM11B1 -> ROOM1180 | ROOM11B1.RDT@0xF56 main00 Slot 0 | P1DL (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11B1, 0, 0, -27830, -16890,  1600,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 92, 45, 0x00F56, 9 },
    /* S093 T045 ROOM11B1 -> ROOM1230 | ROOM11B1.RDT@0xF7A main00 Slot 0 | P1DL (Basis DOOR1D) V3 (Zwilling von S092) */
    { 0x11B1, 0, 0, -27830, -16890,  1600,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 93, 45, 0x00F7A, 9 },
    /* S095 T052 ROOM11B1 -> ROOM1260 | ROOM11B1.RDT@0xFFA main00 Slot 5 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11B1, 0, 0,    -62, -20288,   833,   833, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 95, 52, 0x00FFA, 12 },
    /* S099 T053 ROOM11D0 -> ROOM1260 | ROOM11D0.RDT@0x1228 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11D0, 0, 0,   2600, -22000,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 99, 53, 0x01228, 12 },
    /* S099 T053 ROOM11D1 -> ROOM1260 | ROOM11D1.RDT@0x1228 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11D1, 0, 0,   2600, -22000,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 99, 53, 0x01228, 12 },
    /* S100 T054 ROOM11E0 -> ROOM11F0 | ROOM11E0.RDT@0x1512 main00 Slot 0 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x11E0, 0, 0, -25100,  -9550,  2000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 100, 54, 0x01512, 7 },
    /* S102 T021 ROOM11E0 -> ROOM10A0 | ROOM11E0.RDT@0x1552 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E0, 0, 0,   4750,  -5650,   550,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 102, 21, 0x01552, 1 },
    /* S100 T054 ROOM11E1 -> ROOM11F0 | ROOM11E1.RDT@0x1512 main00 Slot 0 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x11E1, 0, 0, -25100,  -9550,  2000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 100, 54, 0x01512, 7 },
    /* S102 T021 ROOM11E1 -> ROOM10A0 | ROOM11E1.RDT@0x1552 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E1, 0, 0,   4750,  -5650,   550,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 102, 21, 0x01552, 1 },
    /* S104 T054 ROOM11F0 -> ROOM11E0 | ROOM11F0.RDT@0xCC6 main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11F0, 0, 0,    250,   1150,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 104, 54, 0x00CC6, 7 },
    /* S104 T054 ROOM11F1 -> ROOM11E0 | ROOM11F1.RDT@0xCC6 main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11F1, 0, 0,    250,   1150,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0x07, 104, 54, 0x00CC6, 7 },
    /* S119 T022 ROOM1230 -> ROOM10A0 | ROOM1230.RDT@0xB34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Zwilling von S078) */
    { 0x1230, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 119, 22, 0x00B34, 1 },
    /* S121 T045 ROOM1230 -> ROOM11B0 | ROOM1230.RDT@0xB74 main00 Slot 3 | P1DL (Basis DOOR1D) V2 (Zwilling von S080) */
    { 0x1230, 0, 0,   5706,  28874,   436,  2311, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 121, 45, 0x00B74, 9 },
    /* S119 T022 ROOM1231 -> ROOM10A0 | ROOM1231.RDT@0xB34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Zwilling von S078) */
    { 0x1231, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 119, 22, 0x00B34, 1 },
    /* S121 T045 ROOM1231 -> ROOM11B0 | ROOM1231.RDT@0xB74 main00 Slot 3 | P1DL (Basis DOOR1D) V2 (Zwilling von S080) */
    { 0x1231, 0, 0,   5706,  28874,   436,  2311, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0x07, 121, 45, 0x00B74, 9 },
    /* S128 T052 ROOM1260 -> ROOM11B0 | ROOM1260.RDT@0x112A main00 Slot 0 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1,  27650,   9450,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 128, 52, 0x0112A, 12 },
    /* S130 T023 ROOM1260 -> ROOM10B0 | ROOM1260.RDT@0x116A main00 Slot 2 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1, -20250,  -5750,   750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 130, 23, 0x0116A, 12 },
    /* S131 T053 ROOM1260 -> ROOM11D0 | ROOM1260.RDT@0x118A main00 Slot 3 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1,  21850,  -8050,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 131, 53, 0x0118A, 12 },
    /* S128 T052 ROOM1261 -> ROOM11B0 | ROOM1261.RDT@0x112A main00 Slot 0 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1,  27650,   9450,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 128, 52, 0x0112A, 12 },
    /* S130 T023 ROOM1261 -> ROOM10B0 | ROOM1261.RDT@0x116A main00 Slot 2 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1, -20250,  -5750,   750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 130, 23, 0x0116A, 12 },
    /* S131 T053 ROOM1261 -> ROOM11D0 | ROOM1261.RDT@0x118A main00 Slot 3 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1,  21850,  -8050,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 131, 53, 0x0118A, 12 },
    /* S136 T071 ROOM2000 -> ROOM2070 | ROOM2000.RDT@0x167E main00 Slot 3 | P1B3 (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2000, 0, 1, -19800,  -6200,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 136, 71, 0x0167E, 5 },
    /* S136 T071 ROOM2001 -> ROOM2070 | ROOM2001.RDT@0x167E main00 Slot 3 | P1B3 (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2001, 0, 1, -19800,  -6200,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 136, 71, 0x0167E, 5 },
    /* S139 T073 ROOM2020 -> ROOM2050 | ROOM2020.RDT@0x82A main00 Slot 0 | P1AP (Basis DOOR1A) V1 (Griff unsichtbar -> Komplement zu S151) */
    { 0x2020, 0, 1,  11400,  -3850,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 139, 73, 0x0082A, 20 },
    /* S140 T072 ROOM2020 -> ROOM2030 | ROOM2020.RDT@0x866 main00 Slot 1, ROOM2020.RDT@0x88A main00 Slot 1 | P1AP (Basis DOOR1A) V0 (Griff unsichtbar -> Komplement zu S143) */
    { 0x2020, 0, 1, -14200,   3300,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 140, 72, 0x00866, 20 },
    /* S139 T073 ROOM2021 -> ROOM2050 | ROOM2021.RDT@0x82A main00 Slot 0 | P1AP (Basis DOOR1A) V1 (Griff unsichtbar -> Komplement zu S151) */
    { 0x2021, 0, 1,  11400,  -3850,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 139, 73, 0x0082A, 20 },
    /* S140 T072 ROOM2021 -> ROOM2030 | ROOM2021.RDT@0x866 main00 Slot 1, ROOM2021.RDT@0x88A main00 Slot 1 | P1AP (Basis DOOR1A) V0 (Griff unsichtbar -> Komplement zu S143) */
    { 0x2021, 0, 1, -14200,   3300,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 140, 72, 0x00866, 20 },
    /* S141 T076 ROOM2030 -> ROOM2070 | ROOM2030.RDT@0x1ACA main00 Slot 0 | P1B3 (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2030, 0, 3,   7350,   -200,   500,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 141, 76, 0x01ACA, 5 },
    /* S143 T072 ROOM2030 -> ROOM2020 | ROOM2030.RDT@0x1B2A main00 Slot 2 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2030, 0, 9,   -450, -10400,   900,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 143, 72, 0x01B2A, 20 },
    /* S144 T074 ROOM2030 -> ROOM2060 | ROOM2030.RDT@0x1B4C main00 Slot 3 | P1DO (Basis DOOR1D) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2030, 0, 9,  -7700,  -7250,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 0, 0, 0xFF, 144, 74, 0x01B4C, 21 },
    /* S141 T076 ROOM2031 -> ROOM2070 | ROOM2031.RDT@0x1ACA main00 Slot 0 | P1B3 (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2031, 0, 3,   7350,   -200,   500,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 141, 76, 0x01ACA, 5 },
    /* S143 T072 ROOM2031 -> ROOM2020 | ROOM2031.RDT@0x1B2A main00 Slot 2 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2031, 0, 9,   -450, -10400,   900,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 143, 72, 0x01B2A, 20 },
    /* S144 T074 ROOM2031 -> ROOM2060 | ROOM2031.RDT@0x1B4C main00 Slot 3 | P1DO (Basis DOOR1D) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2031, 0, 9,  -7700,  -7250,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 0, 0, 0xFF, 144, 74, 0x01B4C, 21 },
    /* S145 T080 ROOM2040 -> ROOM2050 | ROOM2040.RDT@0x129C main00 Slot 0 | P1EU (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2040, 0, 4,  23800,  -5550,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 145, 80, 0x0129C, 15 },
    /* S145 T080 ROOM2041 -> ROOM2050 | ROOM2041.RDT@0x129C main00 Slot 0 | P1EU (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2041, 0, 4,  23800,  -5550,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 145, 80, 0x0129C, 15 },
    /* S151 T073 ROOM2050 -> ROOM2020 | ROOM2050.RDT@0x1C6A main00 Slot 1 | P1AP (Basis DOOR1A) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2050, 0, 3,  -5150,   4450,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 151, 73, 0x01C6A, 20 },
    /* S152 T080 ROOM2050 -> ROOM2040 | ROOM2050.RDT@0x1C8A main00 Slot 2 | P1EU (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2050, 0, 4,  -7700,  -5600,   500,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 152, 80, 0x01C8A, 15 },
    /* S151 T073 ROOM2051 -> ROOM2020 | ROOM2051.RDT@0x1C6A main00 Slot 1 | P1AP (Basis DOOR1A) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2051, 0, 3,  -5150,   4450,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 151, 73, 0x01C6A, 20 },
    /* S152 T080 ROOM2051 -> ROOM2040 | ROOM2051.RDT@0x1C8A main00 Slot 2 | P1EU (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2051, 0, 4,  -7700,  -5600,   500,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 152, 80, 0x01C8A, 15 },
    /* S153 T074 ROOM2060 -> ROOM2030 | ROOM2060.RDT@0xD9A main00 Slot 0 | P1DO (Basis DOOR1D) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2060, 0, 0,   8450,   3950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 1, 0, 0xFF, 153, 74, 0x00D9A, 21 },
    /* S154 T078 ROOM2060 -> ROOM2040 | ROOM2060.RDT@0xDBA main00 Slot 1 | P1EL (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2060, 0, 1,    450,  -8350,   650,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 154, 78, 0x00DBA, 14 },
    /* S153 T074 ROOM2061 -> ROOM2030 | ROOM2061.RDT@0xD9A main00 Slot 0 | P1DO (Basis DOOR1D) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2061, 0, 0,   8450,   3950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 1, 0, 0xFF, 153, 74, 0x00D9A, 21 },
    /* S154 T078 ROOM2061 -> ROOM2040 | ROOM2061.RDT@0xDBA main00 Slot 1 | P1EL (Basis DOOR1E) V0 (DOOR1E nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x2061, 0, 1,    450,  -8350,   650,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 154, 78, 0x00DBA, 14 },
    /* S155 T071 ROOM2070 -> ROOM2000 | ROOM2070.RDT@0xA3A main00 Slot 0 | P1BD (Basis DOOR1B) V2 (1B beide Fluegel Seite B (wie T028)) */
    { 0x2070, 0, 0, -19450,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0x23, 155, 71, 0x00A3A, 6 },
    /* S156 T076 ROOM2070 -> ROOM2030 | ROOM2070.RDT@0xA5A main00 Slot 1 | P1BD (Basis DOOR1B) V2 (1B beide Fluegel Seite B (wie T028)) */
    { 0x2070, 0, 0,   5500,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0x23, 156, 76, 0x00A5A, 6 },
    /* S157 T081 ROOM2070 -> ROOM2030 | ROOM2070.RDT@0xA7A main00 Slot 2 | P1BD (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2070, 0, 0, -19450,   6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 157, 81, 0x00A7A, 6 },
    /* S158 T082 ROOM2070 -> ROOM2000 | ROOM2070.RDT@0xA9A main00 Slot 3 | P1BD (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2070, 0, 0,   6250,   6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 158, 82, 0x00A9A, 6 },
    /* S155 T071 ROOM2071 -> ROOM2000 | ROOM2071.RDT@0xA3A main00 Slot 0 | P1BD (Basis DOOR1B) V2 (1B beide Fluegel Seite B (wie T028)) */
    { 0x2071, 0, 0, -19450,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0x23, 155, 71, 0x00A3A, 6 },
    /* S156 T076 ROOM2071 -> ROOM2030 | ROOM2071.RDT@0xA5A main00 Slot 1 | P1BD (Basis DOOR1B) V2 (1B beide Fluegel Seite B (wie T028)) */
    { 0x2071, 0, 0,   5500,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0x23, 156, 76, 0x00A5A, 6 },
    /* S157 T081 ROOM2071 -> ROOM2030 | ROOM2071.RDT@0xA7A main00 Slot 2 | P1BD (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2071, 0, 0, -19450,   6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 157, 81, 0x00A7A, 6 },
    /* S158 T082 ROOM2071 -> ROOM2000 | ROOM2071.RDT@0xA9A main00 Slot 3 | P1BD (Basis DOOR1B) V3 (1B beide Fluegel Seite A (wie T028)) */
    { 0x2071, 0, 0,   6250,   6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0x23, 158, 82, 0x00A9A, 6 },
    /* S164 T084 ROOM2090 -> ROOM20A0 | ROOM2090.RDT@0x9B6 main00 Slot 1 | P26W (Basis DOOR26) V3 (einteilig, Rad links -> V3 (T2 2.4 Punkt 3)) */
    { 0x2090, 0, 1,   7250, -16350,   500,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 164, 84, 0x009B6, 22 },
    /* S164 T084 ROOM2091 -> ROOM20A0 | ROOM2091.RDT@0x9B6 main00 Slot 1 | P26W (Basis DOOR26) V3 (einteilig, Rad links -> V3 (T2 2.4 Punkt 3)) */
    { 0x2091, 0, 1,   7250, -16350,   500,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 164, 84, 0x009B6, 22 },
    /* S165 T084 ROOM20A0 -> ROOM2090 | ROOM20A0.RDT@0x1746 main00 Slot 0 | P26W (Basis DOOR26) V2 (Rad unsichtbar -> Komplement V2) */
    { 0x20A0, 0, 2,   4950,   5000,  1500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 165, 84, 0x01746, 22 },
    /* S166 T085 ROOM20A0 -> ROOM20A0 | ROOM20A0.RDT@0x1766 main00 Slot 1 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x20A0, 0, 2,  16500, -21050,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 166, 85, 0x01766, 0 },
    /* S167 T050 ROOM20A0 -> ROOM11A0 | ROOM20A0.RDT@0x17A2 main00 Slot 2, ROOM20A0.RDT@0x17C6 main00 Slot 2 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x20A0, 0, 2,  25800,     50,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 167, 50, 0x017A2, 20 },
    /* S165 T084 ROOM20A1 -> ROOM2090 | ROOM20A1.RDT@0x1746 main00 Slot 0 | P26W (Basis DOOR26) V2 (Rad unsichtbar -> Komplement V2) */
    { 0x20A1, 0, 2,   4950,   5000,  1500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 165, 84, 0x01746, 22 },
    /* S166 T085 ROOM20A1 -> ROOM20A0 | ROOM20A1.RDT@0x1766 main00 Slot 1 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x20A1, 0, 2,  16500, -21050,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 166, 85, 0x01766, 0 },
    /* S167 T050 ROOM20A1 -> ROOM11A0 | ROOM20A1.RDT@0x17A2 main00 Slot 2, ROOM20A1.RDT@0x17C6 main00 Slot 2 | P1AP (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x20A1, 0, 2,  25800,     50,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 167, 50, 0x017A2, 20 },
    /* S171 T086 ROOM20B0 -> ROOM2030 | ROOM20B0.RDT@0x19C2 main00 Slot 3 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x20B0, 0, 1,   5950,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 171, 86, 0x019C2, 0 },
    /* S171 T086 ROOM20B1 -> ROOM2030 | ROOM20B1.RDT@0x19C2 main00 Slot 3 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x20B1, 0, 1,   5950,  -6400,   500,  1500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 171, 86, 0x019C2, 0 },
    /* S173 T047 ROOM3000 -> ROOM11A0 | ROOM3000.RDT@0x1462 main00 Slot 0 | P16R (Basis DOOR16) V5 (hinab -> V5) */
    { 0x3000, 0, 0, -29995,  -9995,     5,     5, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 173, 47, 0x01462, 13 },
    /* S173 T047 ROOM3001 -> ROOM11A0 | ROOM3001.RDT@0x1462 main00 Slot 0 | P16R (Basis DOOR16) V5 (hinab -> V5) */
    { 0x3001, 0, 0, -29995,  -9995,     5,     5, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 173, 47, 0x01462, 13 },
    /* S183 T095 ROOM3030 -> ROOM30E0 | ROOM3030.RDT@0x1D2 main00 Slot 0 | P24B (Basis DOOR24) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3030, 0, 0, -25700, -16500,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 183, 95, 0x001D2, 23 },
    /* S184 T094 ROOM3030 -> ROOM30C0 | ROOM3030.RDT@0x1FA main00 Slot 1 | P06F (Basis DOOR06) V0 (auf keiner Seite sichtbar -> Seite A V0 / B V1) */
    { 0x3030, 0, 0, -25700, -26800,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 184, 94, 0x001FA, 3 },
    /* S185 T094 ROOM3030 -> ROOM30D0 | ROOM3030.RDT@0x21E main00 Slot 1 | P06F (Basis DOOR06) V0 (Zwilling von S184) */
    { 0x3030, 0, 0, -25700, -26800,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 185, 94, 0x0021E, 3 },
    /* S183 T095 ROOM3031 -> ROOM30E0 | ROOM3031.RDT@0x1D2 main00 Slot 0 | P24B (Basis DOOR24) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3031, 0, 0, -25700, -16500,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 183, 95, 0x001D2, 23 },
    /* S184 T094 ROOM3031 -> ROOM30C0 | ROOM3031.RDT@0x1FA main00 Slot 1 | P06F (Basis DOOR06) V0 (auf keiner Seite sichtbar -> Seite A V0 / B V1) */
    { 0x3031, 0, 0, -25700, -26800,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 184, 94, 0x001FA, 3 },
    /* S185 T094 ROOM3031 -> ROOM30D0 | ROOM3031.RDT@0x21E main00 Slot 1 | P06F (Basis DOOR06) V0 (Zwilling von S184) */
    { 0x3031, 0, 0, -25700, -26800,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 185, 94, 0x0021E, 3 },
    /* S187 T096 ROOM3040 -> ROOM3050 | ROOM3040.RDT@0x12A0 main00 Slot 1 | P06U (Basis DOOR06) V0 (Griff unsichtbar -> Komplement zu S188) */
    { 0x3040, 0, 0, -26925,  -4975,   375,   925, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 187, 96, 0x012A0, 4 },
    /* S187 T096 ROOM3041 -> ROOM3050 | ROOM3041.RDT@0x12A0 main00 Slot 1 | P06U (Basis DOOR06) V0 (Griff unsichtbar -> Komplement zu S188) */
    { 0x3041, 0, 0, -26925,  -4975,   375,   925, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 187, 96, 0x012A0, 4 },
    /* S188 T096 ROOM3050 -> ROOM3040 | ROOM3050.RDT@0x1E86 main00 Slot 0 | P06U (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3050, 0, 0, -23951,   2183,  1095,   310, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 188, 96, 0x01E86, 4 },
    /* S189 T098 ROOM3050 -> ROOM30E0 | ROOM3050.RDT@0x1EA6 main00 Slot 1 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3050, 0, 4,  -3526,   2083,  1070,   260, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 189, 98, 0x01EA6, 3 },
    /* S188 T096 ROOM3051 -> ROOM3040 | ROOM3051.RDT@0x1E86 main00 Slot 0 | P06U (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3051, 0, 0, -23951,   2183,  1095,   310, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 188, 96, 0x01E86, 4 },
    /* S189 T098 ROOM3051 -> ROOM30E0 | ROOM3051.RDT@0x1EA6 main00 Slot 1 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3051, 0, 4,  -3526,   2083,  1070,   260, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 189, 98, 0x01EA6, 3 },
    /* S195 T102 ROOM3070 -> ROOM3080 | ROOM3070.RDT@0x3302 main00 Slot 0 | P14A (Basis DOOR14) V0 (Griff unsichtbar -> V0 (Seite A)) */
    { 0x3070, 0, 0, -26800, -11960,   600,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x14, 0, 0, 0xFF, 195, 102, 0x03302, 17 },
    /* S196 T103 ROOM3070 -> ROOM30E0 | ROOM3070.RDT@0x3322 main00 Slot 1 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3070, 0, 0, -12400, -19100,  1100,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 196, 103, 0x03322, 3 },
    /* S195 T102 ROOM3071 -> ROOM3080 | ROOM3071.RDT@0x330A main00 Slot 0 | P14A (Basis DOOR14) V0 (Griff unsichtbar -> V0 (Seite A)) */
    { 0x3071, 0, 0, -26800, -11960,   600,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x14, 0, 0, 0xFF, 195, 102, 0x0330A, 17 },
    /* S196 T103 ROOM3071 -> ROOM30E0 | ROOM3071.RDT@0x332A main00 Slot 1 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3071, 0, 0, -12400, -19100,  1100,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 196, 103, 0x0332A, 3 },
    /* S198 T105 ROOM3090 -> ROOM30C0 | ROOM3090.RDT@0x132A main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3090, 0, 0, -26795,  -1020,   295,   720, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 198, 105, 0x0132A, 3 },
    /* S199 T105 ROOM3090 -> ROOM30D0 | ROOM3090.RDT@0x134E main00 Slot 0 | P06F (Basis DOOR06) V0 (Zwilling von S198) */
    { 0x3090, 0, 0, -26795,  -1020,   295,   720, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 199, 105, 0x0134E, 3 },
    /* S201 T107 ROOM3090 -> ROOM30A0 | ROOM3090.RDT@0x1390 main00 Slot 2 | P06F (Basis DOOR06) V1 (Griff unsichtbar -> Komplement zu S203) */
    { 0x3090, 0, 0, -13135,  -9065,   755,   285, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 201, 107, 0x01390, 3 },
    /* S202 T106 ROOM3090 -> ROOM30B0 | ROOM3090.RDT@0x13B0 main00 Slot 3 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3090, 0, 0, -24325, -24330,   375,   870, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 202, 106, 0x013B0, 3 },
    /* S198 T105 ROOM3091 -> ROOM30C0 | ROOM3091.RDT@0x1332 main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3091, 0, 0, -26795,  -1020,   295,   720, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 198, 105, 0x01332, 3 },
    /* S199 T105 ROOM3091 -> ROOM30D0 | ROOM3091.RDT@0x1356 main00 Slot 0 | P06F (Basis DOOR06) V0 (Zwilling von S198) */
    { 0x3091, 0, 0, -26795,  -1020,   295,   720, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 199, 105, 0x01356, 3 },
    /* S201 T107 ROOM3091 -> ROOM30A0 | ROOM3091.RDT@0x1398 main00 Slot 2 | P06F (Basis DOOR06) V1 (Griff unsichtbar -> Komplement zu S203) */
    { 0x3091, 0, 0, -13135,  -9065,   755,   285, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 201, 107, 0x01398, 3 },
    /* S202 T106 ROOM3091 -> ROOM30B0 | ROOM3091.RDT@0x13B8 main00 Slot 3 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3091, 0, 0, -24325, -24330,   375,   870, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 202, 106, 0x013B8, 3 },
    /* S203 T107 ROOM30A0 -> ROOM3090 | ROOM30A0.RDT@0x722 main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30A0, 0, 0, -26550,  11800,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 203, 107, 0x00722, 3 },
    /* S203 T107 ROOM30A1 -> ROOM3090 | ROOM30A1.RDT@0x722 main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30A1, 0, 0, -26550,  11800,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 203, 107, 0x00722, 3 },
    /* S204 T106 ROOM30B0 -> ROOM3090 | ROOM30B0.RDT@0x912 main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff unsichtbar -> Komplement zu S202) */
    { 0x30B0, 0, 0, -23850, -24400,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 204, 106, 0x00912, 3 },
    /* S204 T106 ROOM30B1 -> ROOM3090 | ROOM30B1.RDT@0x912 main00 Slot 0 | P06F (Basis DOOR06) V0 (Griff unsichtbar -> Komplement zu S202) */
    { 0x30B1, 0, 0, -23850, -24400,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 204, 106, 0x00912, 3 },
    /* S205 T105 ROOM30C0 -> ROOM3090 | ROOM30C0.RDT@0xB5A main00 Slot 0 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30C0, 0, 0,  -5750,   -300,   350,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 205, 105, 0x00B5A, 3 },
    /* S206 T094 ROOM30C0 -> ROOM3030 | ROOM30C0.RDT@0xB7A main00 Slot 1 | P06F (Basis DOOR06) V1 (auf keiner Seite sichtbar -> Seite A V0 / B V1) */
    { 0x30C0, 0, 4, -11800,   1050,   900,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 206, 94, 0x00B7A, 3 },
    /* S205 T105 ROOM30C1 -> ROOM3090 | ROOM30C1.RDT@0xB5A main00 Slot 0 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30C1, 0, 0,  -5750,   -300,   350,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 205, 105, 0x00B5A, 3 },
    /* S206 T094 ROOM30C1 -> ROOM3030 | ROOM30C1.RDT@0xB7A main00 Slot 1 | P06F (Basis DOOR06) V1 (auf keiner Seite sichtbar -> Seite A V0 / B V1) */
    { 0x30C1, 0, 4, -11800,   1050,   900,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 206, 94, 0x00B7A, 3 },
    /* S208 T105 ROOM30D0 -> ROOM3090 | ROOM30D0.RDT@0x1266 main00 Slot 1 | P06F (Basis DOOR06) V1 (Zwilling von S205) */
    { 0x30D0, 0, 0,  -5750,   -300,   350,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 208, 105, 0x01266, 3 },
    /* S209 T094 ROOM30D0 -> ROOM3030 | ROOM30D0.RDT@0x1286 main00 Slot 2 | P06F (Basis DOOR06) V1 (Zwilling von S206) */
    { 0x30D0, 0, 4, -11800,   1050,   900,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 209, 94, 0x01286, 3 },
    /* S208 T105 ROOM30D1 -> ROOM3090 | ROOM30D1.RDT@0x1266 main00 Slot 1 | P06F (Basis DOOR06) V1 (Zwilling von S205) */
    { 0x30D1, 0, 0,  -5750,   -300,   350,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 208, 105, 0x01266, 3 },
    /* S209 T094 ROOM30D1 -> ROOM3030 | ROOM30D1.RDT@0x1286 main00 Slot 2 | P06F (Basis DOOR06) V1 (Zwilling von S206) */
    { 0x30D1, 0, 4, -11800,   1050,   900,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 209, 94, 0x01286, 3 },
    /* S211 T103 ROOM30E0 -> ROOM3070 | ROOM30E0.RDT@0xA12 main00 Slot 1 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30E0, 0, 0, -25750, -21200,  1350,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 211, 103, 0x00A12, 3 },
    /* S213 T098 ROOM30E0 -> ROOM3050 | ROOM30E0.RDT@0xABA main00 Slot 6 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30E0, 0, 0,   3700,   4625,   325,   975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 213, 98, 0x00ABA, 3 },
    /* S214 T095 ROOM30E0 -> ROOM3030 | ROOM30E0.RDT@0xADA main00 Slot 7 | P24B (Basis DOOR24) V0 (Griff unsichtbar -> Komplement zu S183) */
    { 0x30E0, 0, 0,   7125,   2200,   900,   300, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 214, 95, 0x00ADA, 23 },
    /* S211 T103 ROOM30E1 -> ROOM3070 | ROOM30E1.RDT@0xA16 main00 Slot 1 | P06F (Basis DOOR06) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30E1, 0, 0, -25750, -21200,  1350,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 211, 103, 0x00A16, 3 },
    /* S213 T098 ROOM30E1 -> ROOM3050 | ROOM30E1.RDT@0xA36 main00 Slot 2 | P06F (Basis DOOR06) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30E1, 0, 0,   3700,   4625,   325,   975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 213, 98, 0x00A36, 3 },
    /* S214 T095 ROOM30E1 -> ROOM3030 | ROOM30E1.RDT@0xA56 main00 Slot 3 | P24B (Basis DOOR24) V0 (Griff unsichtbar -> Komplement zu S183) */
    { 0x30E1, 0, 0,   7125,   2200,   900,   300, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 214, 95, 0x00A56, 23 },
    /* S218 T112 ROOM4000 -> ROOM3070 | ROOM4000.RDT@0x1104 main00 Slot 5 | P2DS (Basis DOOR2D) V4 (hinauf (ROOM4000 -> ROOM3070 Level-1 nach oben) -> V4 (tueren_04_bau.md 1.3)) */
    { 0x4000, 0, 0,  -3580,  -4000,  2150,  1900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 4, 0, 0xFF, 218, 112, 0x01104, 18 },
    /* S218 T112 ROOM4001 -> ROOM3070 | ROOM4001.RDT@0x10EC main00 Slot 7 | P2DS (Basis DOOR2D) V4 (hinauf (ROOM4000 -> ROOM3070 Level-1 nach oben) -> V4 (tueren_04_bau.md 1.3)) */
    { 0x4001, 0, 0,  -3580,  -4000,  2150,  1900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 4, 0, 0xFF, 218, 112, 0x010EC, 18 },
    /* S226 T117 ROOM4030 -> ROOM4080 | ROOM4030.RDT@0x4A6 main00 Slot 2 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x4030, 1, 0,      0,      0,     0,     0, {-20490, -19400, -20740, -21840}, {-24080, -25070, -26460, -25240}, 0x36, 0, 0, 0xFF, 226, 117, 0x004A6, 0 },
    /* S226 T117 ROOM4031 -> ROOM4080 | ROOM4031.RDT@0x4A6 main00 Slot 2 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x4031, 1, 0,      0,      0,     0,     0, {-20490, -19400, -20740, -21840}, {-24080, -25070, -26460, -25240}, 0x36, 0, 0, 0xFF, 226, 117, 0x004A6, 0 },
    /* S232 T126 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1B76 main00 Slot 1 | P07H (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  10650,  -9450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 232, 126, 0x01B76, 25 },
    /* S236 T123 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1BF6 main00 Slot 5 | P07D (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  -5850, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 236, 123, 0x01BF6, 24 },
    /* S239 T123 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1C56 main00 Slot 8 | P07D (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,   9000, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 239, 123, 0x01C56, 24 },
    /* S240 T126 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1C76 main00 Slot 9 | P07H (Basis DOOR07) V1 (Griff unsichtbar -> Komplement zu S232) */
    { 0x4050, 0, 0,  -1200, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 240, 126, 0x01C76, 25 },
    /* S232 T126 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1B76 main00 Slot 1 | P07H (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  10650,  -9450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 232, 126, 0x01B76, 25 },
    /* S236 T123 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1BF6 main00 Slot 5 | P07D (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  -5850, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 236, 123, 0x01BF6, 24 },
    /* S239 T123 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1C56 main00 Slot 8 | P07D (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,   9000, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 239, 123, 0x01C56, 24 },
    /* S240 T126 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1C76 main00 Slot 9 | P07H (Basis DOOR07) V1 (Griff unsichtbar -> Komplement zu S232) */
    { 0x4051, 0, 0,  -1200, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 240, 126, 0x01C76, 25 },
    /* S248 T117 ROOM4080 -> ROOM4030 | ROOM4080.RDT@0xA86 main00 Slot 0 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x4080, 0, 0,  -4250,   2550,   600,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 248, 117, 0x00A86, 0 },
    /* S248 T117 ROOM4081 -> ROOM4030 | ROOM4081.RDT@0xA86 main00 Slot 0 | OBJEKTLOS (Basis DOOR36) V0 (kein Tuerobjekt -> DOOR36 V0 (Blende + Ton)) */
    { 0x4081, 0, 0,  -4250,   2550,   600,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x36, 0, 0, 0xFF, 248, 117, 0x00A86, 0 },
    /* S269 T142 ROOM5040 -> ROOM5060 | ROOM5040.RDT@0x16AE main00 Slot 1 | P27K (Basis DOOR27) V0 (DOOR27 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x5040, 0, 0, -21400, -23850,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 269, 142, 0x016AE, 27 },
    /* S269 T142 ROOM5041 -> ROOM5060 | ROOM5041.RDT@0x16AE main00 Slot 1 | P27K (Basis DOOR27) V0 (DOOR27 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x5041, 0, 0, -21400, -23850,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 269, 142, 0x016AE, 27 },
    /* S273 T142 ROOM5060 -> ROOM5040 | ROOM5060.RDT@0x2AAE main00 Slot 0 | P27S (Basis DOOR27) V0 (DOOR27 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x5060, 0, 0, -26300, -11600,   800,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 273, 142, 0x02AAE, 26 },
    /* S274 T142 ROOM5060 -> ROOM5120 | ROOM5060.RDT@0x2AD2 main00 Slot 0 | P27S (Basis DOOR27) V0 (Zwilling von S273) */
    { 0x5060, 0, 0, -26300, -11600,   800,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 274, 142, 0x02AD2, 26 },
    /* S273 T142 ROOM5061 -> ROOM5040 | ROOM5061.RDT@0x2AAE main00 Slot 0 | P27S (Basis DOOR27) V0 (DOOR27 nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)) */
    { 0x5061, 0, 0, -26300, -11600,   800,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 273, 142, 0x02AAE, 26 },
    /* S274 T142 ROOM5061 -> ROOM5120 | ROOM5061.RDT@0x2AD2 main00 Slot 0 | P27S (Basis DOOR27) V0 (Zwilling von S273) */
    { 0x5061, 0, 0, -26300, -11600,   800,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 274, 142, 0x02AD2, 26 },
    /* S305 T142 ROOM5120 -> ROOM5060 | ROOM5120.RDT@0x145A main00 Slot 3 | P27O (Basis DOOR27) V0 (Zwilling von S269) */
    { 0x5120, 0, 0, -21400, -23850,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 305, 142, 0x0145A, 28 },
    /* S305 T142 ROOM5121 -> ROOM5060 | ROOM5121.RDT@0x145A main00 Slot 3 | P27O (Basis DOOR27) V0 (Zwilling von S269) */
    { 0x5121, 0, 0, -21400, -23850,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 305, 142, 0x0145A, 28 },
    /* S322 T161 ROOM6030 -> ROOM6000 | ROOM6030.RDT@0xC72 main00 Slot 2 | P1AZ (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6030, 0, 0, -17950, -20200,   750,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 322, 161, 0x00C72, 29 },
    /* S322 T161 ROOM6031 -> ROOM6000 | ROOM6031.RDT@0xC72 main00 Slot 2 | P1AZ (Basis DOOR1A) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6031, 0, 0, -17950, -20200,   750,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 322, 161, 0x00C72, 29 },
};

/* Griff-Tausch der Port-Archive (Spalten wie re15_griff_tausche der Runde 31, dazu spender_eigen =
 * Spender-MD1/TIM aus diesem Port-Archiv). Werte [SIM] (tools/tor/tuerkatalog.py via
 * tuer_zuordnung_gen.griff_daten). PORT-WAHL: RE2 tauscht Griffe nur archivintern (Bit 7). */
static const re15_griff_tausch_t re15_griff_tausche_eigen[5] = {
    /* P1B3: DOOR1B <- DOOR23 (Rotation von DOOR23): Anker Archiv [130, -2940, -3410], Anker Spender [130, -3524, -2952], hinten [-130, -2460, -3370] / [-130, -3524, -2952] -> Versatz vorn [0, -584, 458] hinten [0, -1064, 418] */
    { 0x1B, 0x23, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 0, -450, 0, {0, -584, 458}, {0, -1064, 418}, 5 },
    /* P1BD: DOOR1B <- DOOR23 (Rotation von DOOR23): Anker Archiv [130, -2940, -3410], Anker Spender [130, -3524, -2952], hinten [-130, -2460, -3370] / [-130, -3524, -2952] -> Versatz vorn [0, -584, 458] hinten [0, -1064, 418] */
    { 0x1B, 0x23, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 0, -450, 0, {0, -584, 458}, {0, -1064, 418}, 6 },
    /* P1DG: DOOR1D <- P1DG (Rotation von DOOR07): Anker Archiv [130, -2850, -3410], Anker Spender [130, -3352, -3404] */
    { 0x1D, 0x07, 1, 1, {0, 0, 0}, {2048, 2048, 0}, -245, -702, 7, {0, 0, 0}, {0, 0, 0}, 7 },
    /* P1DK: DOOR1D <- P1DK (Rotation von DOOR07): Anker Archiv [130, -2850, -3410], Anker Spender [130, -3352, -3404] */
    { 0x1D, 0x07, 1, 1, {0, 0, 0}, {2048, 2048, 0}, -245, -702, 8, {0, 0, 0}, {0, 0, 0}, 8 },
    /* P1DL: DOOR1D <- P1DL (Rotation von DOOR07): Anker Archiv [130, -2850, -3410], Anker Spender [130, -3352, -3404] */
    { 0x1D, 0x07, 1, 1, {0, 0, 0}, {2048, 2048, 0}, -245, -702, 9, {0, 0, 0}, {0, 0, 0}, 9 },
};
