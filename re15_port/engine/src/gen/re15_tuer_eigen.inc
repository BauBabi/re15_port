/* Erzeugt von re15_port/tools/tueren/tuer_archiv_bauen.py - NICHT von Hand aendern.
 * Runde 33 / Thema T (analysis/befunde_runde33/tueren_rest_plan.md, tueren_rest_pilot.md).
 * ⛔ PORT-WAHL, KEINE Original-Adresse: port-eigene Tuerarchive im RE2-Aufbau (Tonteil, SCD, MD1
 * bytegleich dem Basis-Archiv, nur die TIM neu) und ihre Zuordnung zu RE1.5-Tuerseiten.
 * Dateien: shared_assets/RE15DOOR/<kennung>.DO2; Groessen wie die RE2-Tabelle @0x8009a520
 * (Tonteil, Modellteil, Sektor), fnv = FNV-1a 32 ueber die ganze Datei. */
static const re15_tuer_eigen_t re15_tuer_eigen[3] = {
    /* 1: P07G = DOOR07 + Textur 'Lueftungsschlitze + Flecken weg, Farbe = Median der RE1.5-Blaetter (G1)' (Ton F3 (06 07 08 22 2F, Blechtuer)) */
    { "P07G", 0x07, 19608, 35428, 10, 55908, 0x885641ACu },
    /* 2: P1DG = DOOR1D + Textur 'Lueftungsgitter weg, glattes Blech, Farbe nach RE1.5 (graugruen/blaugrau)' (Ton eigen DOOR1D (Blech-Doppeltuer, V2/V3)) */
    { "P1DG", 0x1D, 23928, 36100, 12, 60676, 0xB736A098u },
    /* 3: P16M = DOOR16 + Textur 'Silberrohr -> Messing (goldgelb, ROOM12607.bmp), Lichtverlauf der Rohre bleibt' (Ton eigen DOOR16 (4 Sprossen, kein Schliesston)) */
    { "P16M", 0x16, 11000, 46572, 6, 58860, 0xF3556E1Cu },
};

/* Zuordnung RE1.5-Tuerseite -> Port-Archiv (Spalten wie gen/tuer_zuordnung.inc; re2_nr = Basis-Archiv
 * = var 15, eigen = Index+1 in re15_tuer_eigen[]). Variante nach tueren_02_re2.md 2.4 (Herkunft je Zeile). */
static const re15_tuer_zeile_t re15_tuer_zeilen_eigen[72] = {
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
    /* S014 T006 ROOM1040 -> ROOM1030 | ROOM1040.RDT@0x10B8 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1040, 0, 0, -19650,  -3800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 14, 6, 0x010B8, 1 },
    /* S015 T012 ROOM1040 -> ROOM1060 | ROOM1040.RDT@0x10D8 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1040, 0, 0, -19650, -13000,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 15, 12, 0x010D8, 1 },
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
    /* S018 T001 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xAFA main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700,   9200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 18, 1, 0x00AFA, 1 },
    /* S019 T000 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xB1A main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700,   6100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 19, 0, 0x00B1A, 1 },
    /* S020 T002 ROOM1051 -> ROOM1000 | ROOM1051.RDT@0xB3A main00 Slot 3 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  13700, -13450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 20, 2, 0x00B3A, 1 },
    /* S021 T013 ROOM1051 -> ROOM10A0 | ROOM1051.RDT@0xB5A main00 Slot 4 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  17200, -13700,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 21, 13, 0x00B5A, 1 },
    /* S023 T015 ROOM1060 -> ROOM1120 | ROOM1060.RDT@0xD12 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 8,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 23, 15, 0x00D12, 1 },
    /* S024 T016 ROOM1060 -> ROOM10C0 | ROOM1060.RDT@0xD32 main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 4,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 24, 16, 0x00D32, 1 },
    /* S025 T012 ROOM1060 -> ROOM1040 | ROOM1060.RDT@0xD52 main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1060, 0, 0,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 25, 12, 0x00D52, 1 },
    /* S023 T015 ROOM1061 -> ROOM1120 | ROOM1061.RDT@0xD12 main00 Slot 0 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 8,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 23, 15, 0x00D12, 1 },
    /* S024 T016 ROOM1061 -> ROOM10C0 | ROOM1061.RDT@0xD32 main00 Slot 1 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 4,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 24, 16, 0x00D32, 1 },
    /* S025 T012 ROOM1061 -> ROOM1040 | ROOM1061.RDT@0xD52 main00 Slot 2 | P07G (Basis DOOR07) V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1061, 0, 0,  27100,  25400,   500,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 25, 12, 0x00D52, 1 },
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
    { 0x10B0, 0, 0, -21370,   4610,   750,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 38, 23, 0x016F2, 3 },
    /* S038 T023 ROOM10B1 -> ROOM1260 | ROOM10B1.RDT@0x16F2 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x10B1, 0, 0, -21370,   4610,   750,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 38, 23, 0x016F2, 3 },
    /* S039 T016 ROOM10C0 -> ROOM1060 | ROOM10C0.RDT@0xE42 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10C0, 0, 0,   -600,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 39, 16, 0x00E42, 1 },
    /* S039 T016 ROOM10C1 -> ROOM1060 | ROOM10C1.RDT@0xE42 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10C1, 0, 0,   -600,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 39, 16, 0x00E42, 1 },
    /* S045 T026 ROOM10D0 -> ROOM1100 | ROOM10D0.RDT@0x11D6 main00 Slot 16 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x10D0, 0, 0, -19300,  24000,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 45, 26, 0x011D6, 2 },
    /* S045 T026 ROOM10D1 -> ROOM1100 | ROOM10D1.RDT@0x11D6 main00 Slot 16 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x10D1, 0, 0, -19300,  24000,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 45, 26, 0x011D6, 2 },
    /* S049 T026 ROOM1100 -> ROOM10D0 | ROOM1100.RDT@0x99A main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1100, 0, 0, -21330, -24790,   400,  1250, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 49, 26, 0x0099A, 2 },
    /* S049 T026 ROOM1101 -> ROOM10D0 | ROOM1101.RDT@0x99A main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x1101, 0, 0, -21330, -24790,   400,  1250, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 49, 26, 0x0099A, 2 },
    /* S056 T015 ROOM1120 -> ROOM1060 | ROOM1120.RDT@0xC76 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1120, 0, 0,   -700,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 56, 15, 0x00C76, 1 },
    /* S056 T015 ROOM1121 -> ROOM1060 | ROOM1121.RDT@0xC76 main00 Slot 0 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1121, 0, 0,   -700,   9550,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 56, 15, 0x00C76, 1 },
    /* S078 T022 ROOM1180 -> ROOM10A0 | ROOM1180.RDT@0xA34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 78, 22, 0x00A34, 1 },
    /* S078 T022 ROOM1181 -> ROOM10A0 | ROOM1181.RDT@0xA34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 78, 22, 0x00A34, 1 },
    /* S095 T052 ROOM11B0 -> ROOM1260 | ROOM11B0.RDT@0x1010 main00 Slot 5, ROOM11B0.RDT@0x1034 main00 Slot 5 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11B0, 0, 0,    -62, -20288,   833,   833, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 95, 52, 0x01010, 3 },
    /* S095 T052 ROOM11B1 -> ROOM1260 | ROOM11B1.RDT@0xFFA main00 Slot 5 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11B1, 0, 0,    -62, -20288,   833,   833, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 95, 52, 0x00FFA, 3 },
    /* S099 T053 ROOM11D0 -> ROOM1260 | ROOM11D0.RDT@0x1228 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11D0, 0, 0,   2600, -22000,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 99, 53, 0x01228, 3 },
    /* S099 T053 ROOM11D1 -> ROOM1260 | ROOM11D1.RDT@0x1228 main00 Slot 1 | P16M (Basis DOOR16) V5 (hinab -> V5) */
    { 0x11D1, 0, 0,   2600, -22000,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 99, 53, 0x01228, 3 },
    /* S100 T054 ROOM11E0 -> ROOM11F0 | ROOM11E0.RDT@0x1512 main00 Slot 0 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x11E0, 0, 0, -25100,  -9550,  2000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 100, 54, 0x01512, 2 },
    /* S102 T021 ROOM11E0 -> ROOM10A0 | ROOM11E0.RDT@0x1552 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E0, 0, 0,   4750,  -5650,   550,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 102, 21, 0x01552, 1 },
    /* S100 T054 ROOM11E1 -> ROOM11F0 | ROOM11E1.RDT@0x1512 main00 Slot 0 | P1DG (Basis DOOR1D) V2 (1D-Doppeltuer Seite A (wie T097)) */
    { 0x11E1, 0, 0, -25100,  -9550,  2000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 100, 54, 0x01512, 2 },
    /* S102 T021 ROOM11E1 -> ROOM10A0 | ROOM11E1.RDT@0x1552 main00 Slot 2 | P07G (Basis DOOR07) V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E1, 0, 0,   4750,  -5650,   550,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 102, 21, 0x01552, 1 },
    /* S104 T054 ROOM11F0 -> ROOM11E0 | ROOM11F0.RDT@0xCC6 main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11F0, 0, 0,    250,   1150,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 104, 54, 0x00CC6, 2 },
    /* S104 T054 ROOM11F1 -> ROOM11E0 | ROOM11F1.RDT@0xCC6 main00 Slot 0 | P1DG (Basis DOOR1D) V3 (1D-Doppeltuer Seite B (wie T097)) */
    { 0x11F1, 0, 0,    250,   1150,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 104, 54, 0x00CC6, 2 },
    /* S119 T022 ROOM1230 -> ROOM10A0 | ROOM1230.RDT@0xB34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Zwilling von S078) */
    { 0x1230, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 119, 22, 0x00B34, 1 },
    /* S119 T022 ROOM1231 -> ROOM10A0 | ROOM1231.RDT@0xB34 main00 Slot 1 | P07G (Basis DOOR07) V0 (Zwilling von S078) */
    { 0x1231, 0, 0,   -177,  -7772,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0xFF, 119, 22, 0x00B34, 1 },
    /* S128 T052 ROOM1260 -> ROOM11B0 | ROOM1260.RDT@0x112A main00 Slot 0 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1,  27650,   9450,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 128, 52, 0x0112A, 3 },
    /* S130 T023 ROOM1260 -> ROOM10B0 | ROOM1260.RDT@0x116A main00 Slot 2 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1, -20250,  -5750,   750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 130, 23, 0x0116A, 3 },
    /* S131 T053 ROOM1260 -> ROOM11D0 | ROOM1260.RDT@0x118A main00 Slot 3 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1260, 0, 1,  21850,  -8050,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 131, 53, 0x0118A, 3 },
    /* S128 T052 ROOM1261 -> ROOM11B0 | ROOM1261.RDT@0x112A main00 Slot 0 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1,  27650,   9450,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 128, 52, 0x0112A, 3 },
    /* S130 T023 ROOM1261 -> ROOM10B0 | ROOM1261.RDT@0x116A main00 Slot 2 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1, -20250,  -5750,   750,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 130, 23, 0x0116A, 3 },
    /* S131 T053 ROOM1261 -> ROOM11D0 | ROOM1261.RDT@0x118A main00 Slot 3 | P16M (Basis DOOR16) V4 (hinauf -> V4) */
    { 0x1261, 0, 1,  21850,  -8050,   500,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 131, 53, 0x0118A, 3 },
};
