/* Erzeugt von re15_port/tools/tueren/tuer_zuordnung_gen.py - NICHT von Hand aendern.
 * Quelle: analysis/befunde_runde31/tueren_03/zuordnung.json (Dossier tueren_03_zuordnung.md).
 * ⛔ PORT-WAHL aus dem Bildvergleich, KEINE Original-Adresse: RE1.5 waehlt kein Archiv
 * (Payload+12/+13 = 0 in 649 von 653 Door_aot_set); RE2-Angleichung (Beta -> Retail).
 * 184 Tuerseiten, 368 Zeilen (je Raumdatei xxx0/xxx1 eine). Schluessel = Raum + Flaeche + Band
 * (NICHT der Slot): Rechteck als Mitte/Halbmass wie op_door_aot_set, Viereck als Punkte pc+6..21.
 * Spalten: raum, form (0 Rechteck/1 Viereck), band, x, z, hw, hh, qx[4], qz[4], re2_nr,
 *          variante (var 12), bit7 (var 14), spender (Griff-Tausch, 0xFF = keiner), seite, tuer,
 *          off = RDT-Datei-Offset eines Door_aot_set dieser Seite in dieser Raumdatei. */
static const re15_tuer_zeile_t re15_tuer_zeilen[368] = {
    /* S007 T005 ROOM1020 -> ROOM1040 | ROOM1020.RDT@0x1CC2 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1020, 0, 0, -27150,  -8200,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 7, 5, 0x01CC2 },
    /* S007 T005 ROOM1021 -> ROOM1040 | ROOM1021.RDT@0x1CC2 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1021, 0, 0, -27150,  -8200,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 7, 5, 0x01CC2 },
    /* S008 T010 ROOM1030 -> ROOM1050 | ROOM1030.RDT@0x1C6A main00 Slot 0 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1030, 0, 0,   2250, -12700,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 8, 10, 0x01C6A },
    /* S009 T007 ROOM1030 -> ROOM1070 | ROOM1030.RDT@0x1C8A main00 Slot 1 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1030, 0, 0, -16150,  -3800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 9, 7, 0x01C8A },
    /* S008 T010 ROOM1031 -> ROOM1050 | ROOM1031.RDT@0x1CD8 main00 Slot 0 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1031, 0, 0,   2250, -12700,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 8, 10, 0x01CD8 },
    /* S009 T007 ROOM1031 -> ROOM1070 | ROOM1031.RDT@0x1CF8 main00 Slot 1 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1031, 0, 0, -16150,  -3800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 9, 7, 0x01CF8 },
    /* S016 T005 ROOM1040 -> ROOM1020 | ROOM1040.RDT@0x10F8 main00 Slot 3 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1040, 0, 0, -23550,  16700,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 16, 5, 0x010F8 },
    /* S016 T005 ROOM1041 -> ROOM1020 | ROOM1041.RDT@0x10F8 main00 Slot 3 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1041, 0, 0, -23550,  16700,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 16, 5, 0x010F8 },
    /* S017 T010 ROOM1050 -> ROOM1030 | ROOM1050.RDT@0xADA main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1050, 0, 0,  21650,  12350,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 17, 10, 0x00ADA },
    /* S017 T010 ROOM1051 -> ROOM1030 | ROOM1051.RDT@0xADA main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1051, 0, 0,  21650,  12350,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 17, 10, 0x00ADA },
    /* S026 T007 ROOM1070 -> ROOM1030 | ROOM1070.RDT@0x145E main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1070, 0, 0,    560,   6200,   700,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 26, 7, 0x0145E },
    /* S026 T007 ROOM1071 -> ROOM1030 | ROOM1071.RDT@0x145E main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1071, 0, 0,    560,   6200,   700,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 26, 7, 0x0145E },
    /* S037 T024 ROOM10B0 -> ROOM1170 | ROOM10B0.RDT@0x16D2 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x10B0, 0, 5,  14335,  -6840,  1665,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 37, 24, 0x016D2 },
    /* S037 T024 ROOM10B1 -> ROOM1170 | ROOM10B1.RDT@0x16D2 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x10B1, 0, 5,  14335,  -6840,  1665,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 37, 24, 0x016D2 },
    /* S042 T027 ROOM10D0 -> ROOM10F0 | ROOM10D0.RDT@0x1052 main00 Slot 2, ROOM10D0.RDT@0x1174 main00 Slot 0 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) Griff-Tausch DOOR07 */
    { 0x10D0, 0, 0,    800,  -7050,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0x07, 42, 27, 0x01052 },
    /* S043 T028 ROOM10D0 -> ROOM10E0 | ROOM10D0.RDT@0x1196 main00 Slot 14 | DOOR1B V3 (Doppeltuer, beide Fluegel: Seite A weg (T2 2.4 Punkt 2; 1B RE2-Paare 2/3)) */
    { 0x10D0, 0, 0,   6750,   4350,  1800,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0xFF, 43, 28, 0x01196 },
    /* S042 T027 ROOM10D1 -> ROOM10F0 | ROOM10D1.RDT@0x1052 main00 Slot 2, ROOM10D1.RDT@0x1174 main00 Slot 0 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) Griff-Tausch DOOR07 */
    { 0x10D1, 0, 0,    800,  -7050,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0x07, 42, 27, 0x01052 },
    /* S043 T028 ROOM10D1 -> ROOM10E0 | ROOM10D1.RDT@0x1196 main00 Slot 14 | DOOR1B V3 (Doppeltuer, beide Fluegel: Seite A weg (T2 2.4 Punkt 2; 1B RE2-Paare 2/3)) */
    { 0x10D1, 0, 0,   6750,   4350,  1800,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 3, 0, 0xFF, 43, 28, 0x01196 },
    /* S046 T028 ROOM10E0 -> ROOM10D0 | ROOM10E0.RDT@0xC2E main00 Slot 0 | DOOR1B V2 (Doppeltuer, beide Fluegel: Seite B hin) */
    { 0x10E0, 0, 0,  -8050,   4300,  1800,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0xFF, 46, 28, 0x00C2E },
    /* S046 T028 ROOM10E1 -> ROOM10D0 | ROOM10E1.RDT@0xC2E main00 Slot 0 | DOOR1B V2 (Doppeltuer, beide Fluegel: Seite B hin) */
    { 0x10E1, 0, 0,  -8050,   4300,  1800,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1B, 2, 0, 0xFF, 46, 28, 0x00C2E },
    /* S047 T027 ROOM10F0 -> ROOM10D0 | ROOM10F0.RDT@0xF32 main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10F0, 0, 0,   9300,   -300,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 47, 27, 0x00F32 },
    /* S047 T027 ROOM10F1 -> ROOM10D0 | ROOM10F1.RDT@0xF32 main00 Slot 0 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x10F1, 0, 0,   9300,   -300,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 47, 27, 0x00F32 },
    /* S050 T030 ROOM1100 -> ROOM1110 | ROOM1100.RDT@0x9BA main00 Slot 1 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1100, 0, 0, -26880, -10936,   450,   800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 50, 30, 0x009BA },
    /* S050 T030 ROOM1101 -> ROOM1110 | ROOM1101.RDT@0x9BA main00 Slot 1 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1101, 0, 0, -26880, -10936,   450,   800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 50, 30, 0x009BA },
    /* S051 T030 ROOM1110 -> ROOM1100 | ROOM1110.RDT@0xA4E main00 Slot 0 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1110, 0, 0,  -3250,  -4950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 51, 30, 0x00A4E },
    /* S052 T031 ROOM1110 -> ROOM1110 | ROOM1110.RDT@0xA6E main00 Slot 1 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1110, 0, 0,  -3100,   -600,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 52, 31, 0x00A6E },
    /* S053 T031 ROOM1110 -> ROOM1110 | ROOM1110.RDT@0xA8E main00 Slot 2 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1110, 0, 0,  -3800,   -600,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 53, 31, 0x00A8E },
    /* S054 T032 ROOM1110 -> ROOM1110 | ROOM1110.RDT@0xAAE main00 Slot 3 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1110, 0, 0,  -3100,   4400,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 54, 32, 0x00AAE },
    /* S055 T032 ROOM1110 -> ROOM1110 | ROOM1110.RDT@0xACE main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1110, 0, 0,  -3800,   4400,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 55, 32, 0x00ACE },
    /* S051 T030 ROOM1111 -> ROOM1100 | ROOM1111.RDT@0xA4E main00 Slot 0 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1111, 0, 0,  -3250,  -4950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 51, 30, 0x00A4E },
    /* S052 T031 ROOM1111 -> ROOM1110 | ROOM1111.RDT@0xA6E main00 Slot 1 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1111, 0, 0,  -3100,   -600,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 52, 31, 0x00A6E },
    /* S053 T031 ROOM1111 -> ROOM1110 | ROOM1111.RDT@0xA8E main00 Slot 2 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1111, 0, 0,  -3800,   -600,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 53, 31, 0x00A8E },
    /* S054 T032 ROOM1111 -> ROOM1110 | ROOM1111.RDT@0xAAE main00 Slot 3 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1111, 0, 0,  -3100,   4400,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 54, 32, 0x00AAE },
    /* S055 T032 ROOM1111 -> ROOM1110 | ROOM1111.RDT@0xACE main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1111, 0, 0,  -3800,   4400,   300,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 55, 32, 0x00ACE },
    /* S058 T033 ROOM1120 -> ROOM1130 | ROOM1120.RDT@0xCD8 main00 Slot 2 | DOOR09 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1120, 0, 0,  -8450,  -2900,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 1, 0, 0x04, 58, 33, 0x00CD8 },
    /* S058 T033 ROOM1121 -> ROOM1130 | ROOM1121.RDT@0xCD8 main00 Slot 2 | DOOR09 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1121, 0, 0,  -8450,  -2900,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 1, 0, 0x04, 58, 33, 0x00CD8 },
    /* S060 T033 ROOM1130 -> ROOM1120 | ROOM1130.RDT@0x8AE main00 Slot 1 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1130, 0, 0,  -3050,  -2150,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 60, 33, 0x008AE },
    /* S061 T034 ROOM1130 -> ROOM1150 | ROOM1130.RDT@0x8CE main00 Slot 2 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1130, 0, 0,  -6650,  16350,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 61, 34, 0x008CE },
    /* S062 T036 ROOM1130 -> ROOM1170 | ROOM1130.RDT@0x8EE main00 Slot 3 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1130, 0, 0,   2700,  14650,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 62, 36, 0x008EE },
    /* S060 T033 ROOM1131 -> ROOM1120 | ROOM1131.RDT@0x8AE main00 Slot 1 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1131, 0, 0,  -3050,  -2150,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 60, 33, 0x008AE },
    /* S061 T034 ROOM1131 -> ROOM1150 | ROOM1131.RDT@0x8CE main00 Slot 2 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1131, 0, 0,  -6650,  16350,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 61, 34, 0x008CE },
    /* S062 T036 ROOM1131 -> ROOM1170 | ROOM1131.RDT@0x8EE main00 Slot 3 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1131, 0, 0,   2700,  14650,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 62, 36, 0x008EE },
    /* S064 T037 ROOM1140 -> ROOM1170 | ROOM1140.RDT@0xA7A main00 Slot 1, ROOM1140.RDT@0xAB2 main00 Slot 1 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1140, 0, 0,  -7250,   -250,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 64, 37, 0x00A7A },
    /* S064 T037 ROOM1141 -> ROOM1170 | ROOM1141.RDT@0xA7A main00 Slot 1, ROOM1141.RDT@0xAB2 main00 Slot 1 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1141, 0, 0,  -7250,   -250,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 64, 37, 0x00A7A },
    /* S065 T034 ROOM1150 -> ROOM1130 | ROOM1150.RDT@0xD5E main00 Slot 0 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1150, 0, 0, -16050, -11500,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 65, 34, 0x00D5E },
    /* S065 T034 ROOM1151 -> ROOM1130 | ROOM1151.RDT@0xD5E main00 Slot 0 | DOOR09 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) Griff-Tausch DOOR04 */
    { 0x1151, 0, 0, -16050, -11500,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x09, 0, 0, 0x04, 65, 34, 0x00D5E },
    /* S066 T043 ROOM1160 -> ROOM1180 | ROOM1160.RDT@0x7C6 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1160, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 66, 43, 0x007C6 },
    /* S067 T043 ROOM1160 -> ROOM1230 | ROOM1160.RDT@0x7EA main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1160, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 67, 43, 0x007EA },
    /* S066 T043 ROOM1161 -> ROOM1180 | ROOM1161.RDT@0x7C6 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1161, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 66, 43, 0x007C6 },
    /* S067 T043 ROOM1161 -> ROOM1230 | ROOM1161.RDT@0x7EA main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1161, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 67, 43, 0x007EA },
    /* S069 T024 ROOM1170 -> ROOM10B0 | ROOM1170.RDT@0x1226 main00 Slot 1 | DOOR16 V5 (hinab) */
    { 0x1170, 0, 4,   5660,  -7940,   620,  1460, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 69, 24, 0x01226 },
    /* S072 T036 ROOM1170 -> ROOM1130 | ROOM1170.RDT@0x12F8 main00 Slot 4 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1170, 0, 0, -21230, -17925,  1140,   455, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 72, 36, 0x012F8 },
    /* S073 T037 ROOM1170 -> ROOM1140 | ROOM1170.RDT@0x1338 main00 Slot 5 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1170, 0, 0, -21600, -27005,  1140,   605, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 73, 37, 0x01338 },
    /* S069 T024 ROOM1171 -> ROOM10B0 | ROOM1171.RDT@0x126C main00 Slot 2 | DOOR16 V5 (hinab) */
    { 0x1171, 0, 4,   5660,  -7940,   620,  1460, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 69, 24, 0x0126C },
    /* S072 T036 ROOM1171 -> ROOM1130 | ROOM1171.RDT@0x128E main00 Slot 3 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1171, 0, 0, -21230, -17925,  1140,   455, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 72, 36, 0x0128E },
    /* S073 T037 ROOM1171 -> ROOM1140 | ROOM1171.RDT@0x12CE main00 Slot 4 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1171, 0, 0, -21600, -27005,  1140,   605, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 73, 37, 0x012CE },
    /* S076 T043 ROOM1180 -> ROOM1160 | ROOM1180.RDT@0x9EE main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 76, 43, 0x009EE },
    /* S077 T043 ROOM1180 -> ROOM11D0 | ROOM1180.RDT@0xA12 main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 77, 43, 0x00A12 },
    /* S079 T042 ROOM1180 -> ROOM1190 | ROOM1180.RDT@0xA54 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,  -8118,   9355,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 79, 42, 0x00A54 },
    /* S081 T044 ROOM1180 -> ROOM1190 | ROOM1180.RDT@0xA94 main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1180, 0, 0,     96,  12161,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 81, 44, 0x00A94 },
    /* S076 T043 ROOM1181 -> ROOM1160 | ROOM1181.RDT@0x9EE main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 76, 43, 0x009EE },
    /* S077 T043 ROOM1181 -> ROOM11D0 | ROOM1181.RDT@0xA12 main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 77, 43, 0x00A12 },
    /* S079 T042 ROOM1181 -> ROOM1190 | ROOM1181.RDT@0xA54 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,  -8118,   9355,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 79, 42, 0x00A54 },
    /* S081 T044 ROOM1181 -> ROOM1190 | ROOM1181.RDT@0xA94 main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1181, 0, 0,     96,  12161,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 81, 44, 0x00A94 },
    /* S082 T042 ROOM1190 -> ROOM1180 | ROOM1190.RDT@0x28B2 sub13 Slot 4 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1190, 0, 0,   3000,  -9650,  1100,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 82, 42, 0x028B2 },
    /* S083 T042 ROOM1190 -> ROOM1230 | ROOM1190.RDT@0x28D6 sub13 Slot 4 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1190, 0, 0,   3000,  -9650,  1100,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 83, 42, 0x028D6 },
    /* S084 T044 ROOM1190 -> ROOM1180 | ROOM1190.RDT@0x2B1C sub14 Slot 6 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1190, 0, 0,  19500, -11550,  1300,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 84, 44, 0x02B1C },
    /* S085 T044 ROOM1190 -> ROOM1230 | ROOM1190.RDT@0x2B40 sub14 Slot 6 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1190, 0, 0,  19500, -11550,  1300,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 85, 44, 0x02B40 },
    /* S082 T042 ROOM1191 -> ROOM1180 | ROOM1191.RDT@0x28B2 sub13 Slot 4 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1191, 0, 0,   3000,  -9650,  1100,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 82, 42, 0x028B2 },
    /* S083 T042 ROOM1191 -> ROOM1230 | ROOM1191.RDT@0x28D6 sub13 Slot 4 | DOOR13 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1191, 0, 0,   3000,  -9650,  1100,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 1, 0, 0xFF, 83, 42, 0x028D6 },
    /* S084 T044 ROOM1191 -> ROOM1180 | ROOM1191.RDT@0x2B1C sub14 Slot 6 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1191, 0, 0,  19500, -11550,  1300,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 84, 44, 0x02B1C },
    /* S085 T044 ROOM1191 -> ROOM1230 | ROOM1191.RDT@0x2B40 sub14 Slot 6 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1191, 0, 0,  19500, -11550,  1300,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 85, 44, 0x02B40 },
    /* S094 T051 ROOM11B0 -> ROOM11C0 | ROOM11B0.RDT@0xFAA main00 Slot 1 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11B0, 0, 0, -25480, -28190,  1250,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 94, 51, 0x00FAA },
    /* S094 T051 ROOM11B1 -> ROOM11C0 | ROOM11B1.RDT@0xF9C main00 Slot 1 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11B1, 0, 0, -25480, -28190,  1250,  1750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 94, 51, 0x00F9C },
    /* S096 T051 ROOM11C0 -> ROOM11B0 | ROOM11C0.RDT@0x1712 main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11C0, 0, 0, -25100,  17250,  2000,  1350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 96, 51, 0x01712 },
    /* S096 T051 ROOM11C1 -> ROOM11B0 | ROOM11C1.RDT@0x16AE main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11C1, 0, 0, -25100,  17250,  2000,  1350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 96, 51, 0x016AE },
    /* S097 T043 ROOM11D0 -> ROOM1180 | ROOM11D0.RDT@0x11E2 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11D0, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 97, 43, 0x011E2 },
    /* S098 T043 ROOM11D0 -> ROOM1230 | ROOM11D0.RDT@0x1206 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11D0, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 98, 43, 0x01206 },
    /* S097 T043 ROOM11D1 -> ROOM1180 | ROOM11D1.RDT@0x11E2 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11D1, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 97, 43, 0x011E2 },
    /* S098 T043 ROOM11D1 -> ROOM1230 | ROOM11D1.RDT@0x1206 main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11D1, 0, 0,   -300, -16500,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 98, 43, 0x01206 },
    /* S101 T055 ROOM11E0 -> ROOM1200 | ROOM11E0.RDT@0x1532 main00 Slot 1 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E0, 0, 0,   2000, -24250,  1000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 101, 55, 0x01532 },
    /* S103 T056 ROOM11E0 -> ROOM1210 | ROOM11E0.RDT@0x15A2 main00 Slot 5, ROOM11E0.RDT@0x16C4 main00 Slot 3 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11E0, 0, 0,  10450,   5500,   750,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 103, 56, 0x015A2 },
    /* S101 T055 ROOM11E1 -> ROOM1200 | ROOM11E1.RDT@0x1532 main00 Slot 1 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x11E1, 0, 0,   2000, -24250,  1000,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 101, 55, 0x01532 },
    /* S103 T056 ROOM11E1 -> ROOM1210 | ROOM11E1.RDT@0x15A2 main00 Slot 5, ROOM11E1.RDT@0x16C4 main00 Slot 3 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x11E1, 0, 0,  10450,   5500,   750,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 103, 56, 0x015A2 },
    /* S105 T055 ROOM1200 -> ROOM11E0 | ROOM1200.RDT@0x7BA main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1200, 0, 0, -20045, -27095,   845,   385, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 105, 55, 0x007BA },
    /* S105 T055 ROOM1201 -> ROOM11E0 | ROOM1201.RDT@0x7BA main00 Slot 0 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1201, 0, 0, -20045, -27095,   845,   385, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 105, 55, 0x007BA },
    /* S106 T056 ROOM1210 -> ROOM11E0 | ROOM1210.RDT@0x1CC6 main00 Slot 0 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1210, 0, 0, -27250,  -2400,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 106, 56, 0x01CC6 },
    /* S107 T058 ROOM1210 -> ROOM1220 | ROOM1210.RDT@0x1CE6 main00 Slot 1 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1210, 0, 0, -21450,  -6100,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 107, 58, 0x01CE6 },
    /* S108 T061 ROOM1210 -> ROOM1220 | ROOM1210.RDT@0x1D06 main00 Slot 2 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1210, 0, 0, -17150,  -9800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 108, 61, 0x01D06 },
    /* S109 T057 ROOM1210 -> ROOM1220 | ROOM1210.RDT@0x1D26 main00 Slot 3 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1210, 0, 0, -21650, -13600,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 109, 57, 0x01D26 },
    /* S110 T060 ROOM1210 -> ROOM1220 | ROOM1210.RDT@0x1D46 main00 Slot 4 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1210, 0, 0, -17150, -17300,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 110, 60, 0x01D46 },
    /* S111 T059 ROOM1210 -> ROOM1220 | ROOM1210.RDT@0x1D66 main00 Slot 5 | DOOR1C V0 (Griff auf keiner Seite sichtbar -> Seite A V0 / Seite B V1 (RE2-Standardpaar)) */
    { 0x1210, 0, 0, -17150, -25500,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 111, 59, 0x01D66 },
    /* S106 T056 ROOM1211 -> ROOM11E0 | ROOM1211.RDT@0x1CC6 main00 Slot 0 | DOOR19 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1211, 0, 0, -27250,  -2400,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 0, 0, 0xFF, 106, 56, 0x01CC6 },
    /* S107 T058 ROOM1211 -> ROOM1220 | ROOM1211.RDT@0x1CE6 main00 Slot 1 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1211, 0, 0, -21450,  -6100,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 107, 58, 0x01CE6 },
    /* S108 T061 ROOM1211 -> ROOM1220 | ROOM1211.RDT@0x1D06 main00 Slot 2 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1211, 0, 0, -17150,  -9800,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 108, 61, 0x01D06 },
    /* S109 T057 ROOM1211 -> ROOM1220 | ROOM1211.RDT@0x1D26 main00 Slot 3 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1211, 0, 0, -21650, -13600,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 109, 57, 0x01D26 },
    /* S110 T060 ROOM1211 -> ROOM1220 | ROOM1211.RDT@0x1D46 main00 Slot 4 | DOOR1C V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1211, 0, 0, -17150, -17300,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 110, 60, 0x01D46 },
    /* S111 T059 ROOM1211 -> ROOM1220 | ROOM1211.RDT@0x1D66 main00 Slot 5 | DOOR1C V0 (Griff auf keiner Seite sichtbar -> Seite A V0 / Seite B V1 (RE2-Standardpaar)) */
    { 0x1211, 0, 0, -17150, -25500,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 0, 0, 0xFF, 111, 59, 0x01D66 },
    /* S112 T058 ROOM1220 -> ROOM1210 | ROOM1220.RDT@0xDD2 main00 Slot 0 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1220, 0, 0, -21750,  -6400,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 112, 58, 0x00DD2 },
    /* S113 T061 ROOM1220 -> ROOM1210 | ROOM1220.RDT@0xDF2 main00 Slot 1 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1220, 0, 0, -17050,  -9900,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 113, 61, 0x00DF2 },
    /* S114 T057 ROOM1220 -> ROOM1210 | ROOM1220.RDT@0xE12 main00 Slot 2 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1220, 0, 0, -21750, -13900,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 114, 57, 0x00E12 },
    /* S115 T060 ROOM1220 -> ROOM1210 | ROOM1220.RDT@0xE32 main00 Slot 3 | DOOR1C V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x1220, 0, 0, -17050, -17800,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 115, 60, 0x00E32 },
    /* S116 T059 ROOM1220 -> ROOM1210 | ROOM1220.RDT@0xE52 main00 Slot 4 | DOOR1C V1 (Griff auf keiner Seite sichtbar -> Seite A V0 / Seite B V1 (RE2-Standardpaar)) */
    { 0x1220, 0, 0, -17050, -25000,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 116, 59, 0x00E52 },
    /* S112 T058 ROOM1221 -> ROOM1210 | ROOM1221.RDT@0xDD2 main00 Slot 0 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1221, 0, 0, -21750,  -6400,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 112, 58, 0x00DD2 },
    /* S113 T061 ROOM1221 -> ROOM1210 | ROOM1221.RDT@0xDF2 main00 Slot 1 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1221, 0, 0, -17050,  -9900,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 113, 61, 0x00DF2 },
    /* S114 T057 ROOM1221 -> ROOM1210 | ROOM1221.RDT@0xE12 main00 Slot 2 | DOOR1C V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1221, 0, 0, -21750, -13900,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 114, 57, 0x00E12 },
    /* S115 T060 ROOM1221 -> ROOM1210 | ROOM1221.RDT@0xE32 main00 Slot 3 | DOOR1C V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x1221, 0, 0, -17050, -17800,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 115, 60, 0x00E32 },
    /* S116 T059 ROOM1221 -> ROOM1210 | ROOM1221.RDT@0xE52 main00 Slot 4 | DOOR1C V1 (Griff auf keiner Seite sichtbar -> Seite A V0 / Seite B V1 (RE2-Standardpaar)) */
    { 0x1221, 0, 0, -17050, -25000,   250,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1C, 1, 0, 0xFF, 116, 59, 0x00E52 },
    /* S117 T043 ROOM1230 -> ROOM1160 | ROOM1230.RDT@0xAEE main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1230, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 117, 43, 0x00AEE },
    /* S118 T043 ROOM1230 -> ROOM11D0 | ROOM1230.RDT@0xB12 main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1230, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 118, 43, 0x00B12 },
    /* S120 T042 ROOM1230 -> ROOM1190 | ROOM1230.RDT@0xB54 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1230, 0, 0,  -8118,   9355,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 120, 42, 0x00B54 },
    /* S122 T044 ROOM1230 -> ROOM1190 | ROOM1230.RDT@0xBC4 main00 Slot 6, ROOM1230.RDT@0xCE6 main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1230, 0, 0,     96,  12161,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 122, 44, 0x00BC4 },
    /* S117 T043 ROOM1231 -> ROOM1160 | ROOM1231.RDT@0xAEE main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1231, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 117, 43, 0x00AEE },
    /* S118 T043 ROOM1231 -> ROOM11D0 | ROOM1231.RDT@0xB12 main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1231, 0, 0,  -4837, -16839,  1204,   722, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 118, 43, 0x00B12 },
    /* S120 T042 ROOM1231 -> ROOM1190 | ROOM1231.RDT@0xB54 main00 Slot 2 | DOOR13 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x1231, 0, 0,  -8118,   9355,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 120, 42, 0x00B54 },
    /* S122 T044 ROOM1231 -> ROOM1190 | ROOM1231.RDT@0xBC4 main00 Slot 6, ROOM1231.RDT@0xCE6 main00 Slot 4 | DOOR19 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x1231, 0, 0,     96,  12161,  1148,   467, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x19, 1, 0, 0xFF, 122, 44, 0x00BC4 },
    /* S129 T067 ROOM1260 -> ROOM2000 | ROOM1260.RDT@0x114A main00 Slot 1 | DOOR16 V5 (hinab) */
    { 0x1260, 0, 1,  -8750,  -8400,   950,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 129, 67, 0x0114A },
    /* S132 T068 ROOM1260 -> ROOM2000 | ROOM1260.RDT@0x11AA main00 Slot 4 | DOOR16 V5 (hinab) */
    { 0x1260, 0, 1,  19650,  -3300,   950,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 132, 68, 0x011AA },
    /* S129 T067 ROOM1261 -> ROOM2000 | ROOM1261.RDT@0x114A main00 Slot 1 | DOOR16 V5 (hinab) */
    { 0x1261, 0, 1,  -8750,  -8400,   950,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 129, 67, 0x0114A },
    /* S132 T068 ROOM1261 -> ROOM2000 | ROOM1261.RDT@0x11AA main00 Slot 4 | DOOR16 V5 (hinab) */
    { 0x1261, 0, 1,  19650,  -3300,   950,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 132, 68, 0x011AA },
    /* S133 T067 ROOM2000 -> ROOM1260 | ROOM2000.RDT@0x161E main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x2000, 0, 1,   4400, -19700,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 133, 67, 0x0161E },
    /* S134 T068 ROOM2000 -> ROOM1260 | ROOM2000.RDT@0x163E main00 Slot 1 | DOOR16 V4 (hinauf) */
    { 0x2000, 0, 1, -27100, -24200,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 134, 68, 0x0163E },
    /* S135 T069 ROOM2000 -> ROOM2010 | ROOM2000.RDT@0x165E main00 Slot 2 | DOOR23 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2000, 0, 1, -27100,  -2650,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 0, 0, 0xFF, 135, 69, 0x0165E },
    /* S137 T070 ROOM2000 -> ROOM2050 | ROOM2000.RDT@0x169E main00 Slot 4 | DOOR16 V4 (hinauf) */
    { 0x2000, 0, 1, -27100,  10300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 137, 70, 0x0169E },
    /* S133 T067 ROOM2001 -> ROOM1260 | ROOM2001.RDT@0x161E main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x2001, 0, 1,   4400, -19700,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 133, 67, 0x0161E },
    /* S134 T068 ROOM2001 -> ROOM1260 | ROOM2001.RDT@0x163E main00 Slot 1 | DOOR16 V4 (hinauf) */
    { 0x2001, 0, 1, -27100, -24200,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 134, 68, 0x0163E },
    /* S135 T069 ROOM2001 -> ROOM2010 | ROOM2001.RDT@0x165E main00 Slot 2 | DOOR23 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2001, 0, 1, -27100,  -2650,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 0, 0, 0xFF, 135, 69, 0x0165E },
    /* S137 T070 ROOM2001 -> ROOM2050 | ROOM2001.RDT@0x169E main00 Slot 4 | DOOR16 V4 (hinauf) */
    { 0x2001, 0, 1, -27100,  10300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 137, 70, 0x0169E },
    /* S138 T069 ROOM2010 -> ROOM2000 | ROOM2010.RDT@0x746 main00 Slot 0 | DOOR23 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2010, 0, 0,  -5300,  -4925,   500,   875, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 1, 0, 0xFF, 138, 69, 0x00746 },
    /* S138 T069 ROOM2011 -> ROOM2000 | ROOM2011.RDT@0x746 main00 Slot 0 | DOOR23 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2011, 0, 0,  -5300,  -4925,   500,   875, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 1, 0, 0xFF, 138, 69, 0x00746 },
    /* S142 T075 ROOM2030 -> ROOM2080 | ROOM2030.RDT@0x1AEA main00 Slot 1 | DOOR06 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2030, 0, 9,  -7700,   6400,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 142, 75, 0x01AEA },
    /* S142 T075 ROOM2031 -> ROOM2080 | ROOM2031.RDT@0x1AEA main00 Slot 1 | DOOR06 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2031, 0, 9,  -7700,   6400,   500,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 142, 75, 0x01AEA },
    /* S146 T078 ROOM2040 -> ROOM2060 | ROOM2040.RDT@0x12BC main00 Slot 1 | DOOR1E V0 (Gitterklappe, V0 wie RE2 ROOM4010/6000) */
    { 0x2040, 0, 4, -12800,   2300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 146, 78, 0x012BC },
    /* S147 T079 ROOM2040 -> ROOM2040 | ROOM2040.RDT@0x12DC main00 Slot 2 | DOOR06 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2040, 0, 5,  -2400, -19800,   400,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 147, 79, 0x012DC },
    /* S148 T079 ROOM2040 -> ROOM2040 | ROOM2040.RDT@0x12FC main00 Slot 3 | DOOR06 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2040, 0, 5,  -3250, -19800,   400,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 148, 79, 0x012FC },
    /* S149 T077 ROOM2040 -> ROOM2070 | ROOM2040.RDT@0x131C main00 Slot 4 | DOOR0A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2040, 0, 5, -20450, -19800,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 0, 0, 0xFF, 149, 77, 0x0131C },
    /* S146 T078 ROOM2041 -> ROOM2060 | ROOM2041.RDT@0x12BC main00 Slot 1 | DOOR1E V0 (Gitterklappe, V0 wie RE2 ROOM4010/6000) */
    { 0x2041, 0, 4, -12800,   2300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1E, 0, 0, 0xFF, 146, 78, 0x012BC },
    /* S147 T079 ROOM2041 -> ROOM2040 | ROOM2041.RDT@0x12DC main00 Slot 2 | DOOR06 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2041, 0, 5,  -2400, -19800,   400,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 1, 0, 0xFF, 147, 79, 0x012DC },
    /* S148 T079 ROOM2041 -> ROOM2040 | ROOM2041.RDT@0x12FC main00 Slot 3 | DOOR06 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2041, 0, 5,  -3250, -19800,   400,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 148, 79, 0x012FC },
    /* S149 T077 ROOM2041 -> ROOM2070 | ROOM2041.RDT@0x131C main00 Slot 4 | DOOR0A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x2041, 0, 5, -20450, -19800,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 0, 0, 0xFF, 149, 77, 0x0131C },
    /* S150 T070 ROOM2050 -> ROOM2000 | ROOM2050.RDT@0x1C4A main00 Slot 0 | DOOR16 V5 (hinab) */
    { 0x2050, 0, 3,   9100,   5700,  1000,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 150, 70, 0x01C4A },
    /* S150 T070 ROOM2051 -> ROOM2000 | ROOM2051.RDT@0x1C4A main00 Slot 0 | DOOR16 V5 (hinab) */
    { 0x2051, 0, 3,   9100,   5700,  1000,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 150, 70, 0x01C4A },
    /* S159 T048 ROOM2070 -> ROOM11A0 | ROOM2070.RDT@0xAD6 main00 Slot 4, ROOM2070.RDT@0xAFA main00 Slot 4 | DOOR0A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2070, 0, 0, -19400, -19250,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 1, 0, 0xFF, 159, 48, 0x00AD6 },
    /* S160 T077 ROOM2070 -> ROOM2040 | ROOM2070.RDT@0xB1C main00 Slot 5 | DOOR0A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2070, 0, 0,   5500, -19200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 1, 0, 0xFF, 160, 77, 0x00B1C },
    /* S159 T048 ROOM2071 -> ROOM11A0 | ROOM2071.RDT@0xAD6 main00 Slot 4, ROOM2071.RDT@0xAFA main00 Slot 4 | DOOR0A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2071, 0, 0, -19400, -19250,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 1, 0, 0xFF, 159, 48, 0x00AD6 },
    /* S160 T077 ROOM2071 -> ROOM2040 | ROOM2071.RDT@0xB1C main00 Slot 5 | DOOR0A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x2071, 0, 0,   5500, -19200,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x0A, 1, 0, 0xFF, 160, 77, 0x00B1C },
    /* S161 T075 ROOM2080 -> ROOM2030 | ROOM2080.RDT@0xBA2 main00 Slot 0 | DOOR06 V0 (Griff unsichtbar -> Komplement der Gegenseite (V1), RE2-Paar 0/1) */
    { 0x2080, 0, 5, -23900, -16700,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 161, 75, 0x00BA2 },
    /* S162 T083 ROOM2080 -> ROOM2090 | ROOM2080.RDT@0xBC2 main00 Slot 1 | DOOR16 V5 (hinab) */
    { 0x2080, 0, 8,  -9150, -25650,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 162, 83, 0x00BC2 },
    /* S161 T075 ROOM2081 -> ROOM2030 | ROOM2081.RDT@0xBA2 main00 Slot 0 | DOOR06 V0 (Griff unsichtbar -> Komplement der Gegenseite (V1), RE2-Paar 0/1) */
    { 0x2081, 0, 5, -23900, -16700,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x06, 0, 0, 0xFF, 161, 75, 0x00BA2 },
    /* S162 T083 ROOM2081 -> ROOM2090 | ROOM2081.RDT@0xBC2 main00 Slot 1 | DOOR16 V5 (hinab) */
    { 0x2081, 0, 8,  -9150, -25650,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 5, 0, 0xFF, 162, 83, 0x00BC2 },
    /* S163 T083 ROOM2090 -> ROOM2080 | ROOM2090.RDT@0x996 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x2090, 0, 0,  -8900, -25500,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 163, 83, 0x00996 },
    /* S163 T083 ROOM2091 -> ROOM2080 | ROOM2091.RDT@0x996 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x2091, 0, 0,  -8900, -25500,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 163, 83, 0x00996 },
    /* S168 T067 ROOM20B0 -> ROOM1260 | ROOM20B0.RDT@0x1962 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x20B0, 0, 1,   4400, -19700,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 168, 67, 0x01962 },
    /* S169 T068 ROOM20B0 -> ROOM1260 | ROOM20B0.RDT@0x1982 main00 Slot 1 | DOOR16 V4 (hinauf) */
    { 0x20B0, 0, 1, -27100, -24200,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 169, 68, 0x01982 },
    /* S170 T069 ROOM20B0 -> ROOM2010 | ROOM20B0.RDT@0x19A2 main00 Slot 2 | DOOR23 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x20B0, 0, 1, -27100,  -2650,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 0, 0, 0xFF, 170, 69, 0x019A2 },
    /* S172 T070 ROOM20B0 -> ROOM2050 | ROOM20B0.RDT@0x19E2 main00 Slot 4 | DOOR16 V4 (hinauf) */
    { 0x20B0, 0, 1, -27100,  10300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 172, 70, 0x019E2 },
    /* S168 T067 ROOM20B1 -> ROOM1260 | ROOM20B1.RDT@0x1962 main00 Slot 0 | DOOR16 V4 (hinauf) */
    { 0x20B1, 0, 1,   4400, -19700,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 168, 67, 0x01962 },
    /* S169 T068 ROOM20B1 -> ROOM1260 | ROOM20B1.RDT@0x1982 main00 Slot 1 | DOOR16 V4 (hinauf) */
    { 0x20B1, 0, 1, -27100, -24200,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 169, 68, 0x01982 },
    /* S170 T069 ROOM20B1 -> ROOM2010 | ROOM20B1.RDT@0x19A2 main00 Slot 2 | DOOR23 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x20B1, 0, 1, -27100,  -2650,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x23, 0, 0, 0xFF, 170, 69, 0x019A2 },
    /* S172 T070 ROOM20B1 -> ROOM2050 | ROOM20B1.RDT@0x19E2 main00 Slot 4 | DOOR16 V4 (hinauf) */
    { 0x20B1, 0, 1, -27100,  10300,   500,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x16, 4, 0, 0xFF, 172, 70, 0x019E2 },
    /* S174 T087 ROOM3000 -> ROOM3010 | ROOM3000.RDT@0x1482 main00 Slot 1 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3000, 0, 0,  -5555, -17520,   335,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 174, 87, 0x01482 },
    /* S174 T087 ROOM3001 -> ROOM3010 | ROOM3001.RDT@0x1482 main00 Slot 1 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3001, 0, 0,  -5555, -17520,   335,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 174, 87, 0x01482 },
    /* S175 T087 ROOM3010 -> ROOM3000 | ROOM3010.RDT@0x1FD2 main00 Slot 0 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3010, 0, 0, -24200,  11530,   800,   910, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 175, 87, 0x01FD2 },
    /* S176 T089 ROOM3010 -> ROOM3060 | ROOM3010.RDT@0x1FF2 main00 Slot 1 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3010, 0, 0,  -6563,   9810,   705,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 176, 89, 0x01FF2 },
    /* S178 T090 ROOM3010 -> ROOM3020 | ROOM3010.RDT@0x203A main00 Slot 3 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3010, 0, 0,  -6500, -18300,   700,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 178, 90, 0x0203A },
    /* S179 T091 ROOM3010 -> ROOM30D0 | ROOM3010.RDT@0x205E main00 Slot 3 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3010, 0, 0,  -6500, -18300,   700,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 179, 91, 0x0205E },
    /* S180 T088 ROOM3010 -> ROOM30E0 | ROOM3010.RDT@0x20EA main00 Slot 6 | DOOR24 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3010, 0, 1, -26750, -19820,   450,  1060, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 180, 88, 0x020EA },
    /* S175 T087 ROOM3011 -> ROOM3000 | ROOM3011.RDT@0x1FD2 main00 Slot 0 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3011, 0, 0, -24200,  11530,   800,   910, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 175, 87, 0x01FD2 },
    /* S176 T089 ROOM3011 -> ROOM3060 | ROOM3011.RDT@0x1FF2 main00 Slot 1 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3011, 0, 0,  -6563,   9810,   705,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 176, 89, 0x01FF2 },
    /* S178 T090 ROOM3011 -> ROOM3020 | ROOM3011.RDT@0x203A main00 Slot 3 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3011, 0, 0,  -6500, -18300,   700,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 178, 90, 0x0203A },
    /* S179 T091 ROOM3011 -> ROOM30D0 | ROOM3011.RDT@0x205E main00 Slot 3 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3011, 0, 0,  -6500, -18300,   700,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 179, 91, 0x0205E },
    /* S180 T088 ROOM3011 -> ROOM30E0 | ROOM3011.RDT@0x20EA main00 Slot 6 | DOOR24 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3011, 0, 1, -26750, -19820,   450,  1060, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 180, 88, 0x020EA },
    /* S181 T090 ROOM3020 -> ROOM3010 | ROOM3020.RDT@0xB3A main00 Slot 0 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3020, 0, 0, -26750, -22500,   300,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 181, 90, 0x00B3A },
    /* S181 T090 ROOM3021 -> ROOM3010 | ROOM3021.RDT@0xB3A main00 Slot 0 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3021, 0, 0, -26750, -22500,   300,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 181, 90, 0x00B3A },
    /* S186 T097 ROOM3040 -> ROOM3060 | ROOM3040.RDT@0x125A main00 Slot 0, ROOM3040.RDT@0x127E main00 Slot 0 | DOOR1D V2 (Doppeltuer DOOR1D, Seite A: V2 (T2 2.4 Punkt 2)) */
    { 0x3040, 0, 0,  -8800, -26950,  1850,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 186, 97, 0x0125A },
    /* S186 T097 ROOM3041 -> ROOM3060 | ROOM3041.RDT@0x125A main00 Slot 0, ROOM3041.RDT@0x127E main00 Slot 0 | DOOR1D V2 (Doppeltuer DOOR1D, Seite A: V2 (T2 2.4 Punkt 2)) */
    { 0x3041, 0, 0,  -8800, -26950,  1850,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 2, 0, 0xFF, 186, 97, 0x0125A },
    /* S190 T089 ROOM3060 -> ROOM3010 | ROOM3060.RDT@0xE76 main00 Slot 0 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3060, 0, 0, -27150, -22400,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 190, 89, 0x00E76 },
    /* S191 T099 ROOM3060 -> ROOM3090 | ROOM3060.RDT@0xE96 main00 Slot 1 | DOOR24 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3060, 0, 0,  -3200, -26650,  1300,   650, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 191, 99, 0x00E96 },
    /* S192 T097 ROOM3060 -> ROOM3040 | ROOM3060.RDT@0xED6 main00 Slot 2 | DOOR1D V3 (Doppeltuer DOOR1D, Seite B: V3) */
    { 0x3060, 0, 0,   3100, -16550,  1600,   650, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 192, 97, 0x00ED6 },
    /* S190 T089 ROOM3061 -> ROOM3010 | ROOM3061.RDT@0xE76 main00 Slot 0 | DOOR15 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3061, 0, 0, -27150, -22400,   750,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 0, 0, 0xFF, 190, 89, 0x00E76 },
    /* S191 T099 ROOM3061 -> ROOM3090 | ROOM3061.RDT@0xE96 main00 Slot 1 | DOOR24 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x3061, 0, 0,  -3200, -26650,  1300,   650, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 191, 99, 0x00E96 },
    /* S192 T097 ROOM3061 -> ROOM3040 | ROOM3061.RDT@0xED6 main00 Slot 2 | DOOR1D V3 (Doppeltuer DOOR1D, Seite B: V3) */
    { 0x3061, 0, 0,   3100, -16550,  1600,   650, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1D, 3, 0, 0xFF, 192, 97, 0x00ED6 },
    /* S200 T099 ROOM3090 -> ROOM3060 | ROOM3090.RDT@0x1370 main00 Slot 1 | DOOR24 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3090, 0, 0, -24665,   1675,   945,   375, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 200, 99, 0x01370 },
    /* S200 T099 ROOM3091 -> ROOM3060 | ROOM3091.RDT@0x1378 main00 Slot 1 | DOOR24 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x3091, 0, 0, -24665,   1675,   945,   375, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 1, 0, 0xFF, 200, 99, 0x01378 },
    /* S207 T091 ROOM30D0 -> ROOM3010 | ROOM30D0.RDT@0x1246 main00 Slot 0 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30D0, 0, 0, -26750, -22500,   300,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 207, 91, 0x01246 },
    /* S207 T091 ROOM30D1 -> ROOM3010 | ROOM30D1.RDT@0x1246 main00 Slot 0 | DOOR15 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x30D1, 0, 0, -26750, -22500,   300,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x15, 1, 0, 0xFF, 207, 91, 0x01246 },
    /* S210 T088 ROOM30E0 -> ROOM3010 | ROOM30E0.RDT@0x9F2 main00 Slot 0 | DOOR24 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30E0, 0, 0, -18500, -22840,   400,   660, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 210, 88, 0x009F2 },
    /* S210 T088 ROOM30E1 -> ROOM3010 | ROOM30E1.RDT@0x9F6 main00 Slot 0 | DOOR24 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x30E1, 0, 0, -18500, -22840,   400,   660, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x24, 0, 0, 0xFF, 210, 88, 0x009F6 },
    /* S215 T109 ROOM4000 -> ROOM4020 | ROOM4000.RDT@0x107A main00 Slot 0 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x4000, 0, 0, -26600,  -8950,  1000,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 215, 109, 0x0107A },
    /* S216 T110 ROOM4000 -> ROOM4010 | ROOM4000.RDT@0x109C main00 Slot 1 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4000, 0, 0, -16300,  -3500,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 216, 110, 0x0109C },
    /* S217 T111 ROOM4000 -> ROOM4030 | ROOM4000.RDT@0x10E4 main00 Slot 4 | DOOR25 V0 (nur V0) */
    { 0x4000, 0, 0, -14130, -26500,   900,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 217, 111, 0x010E4 },
    /* S215 T109 ROOM4001 -> ROOM4020 | ROOM4001.RDT@0x1062 main00 Slot 0 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x4001, 0, 0, -26600,  -8950,  1000,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 215, 109, 0x01062 },
    /* S216 T110 ROOM4001 -> ROOM4010 | ROOM4001.RDT@0x1084 main00 Slot 1 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4001, 0, 0, -16300,  -3500,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 216, 110, 0x01084 },
    /* S217 T111 ROOM4001 -> ROOM4030 | ROOM4001.RDT@0x10CC main00 Slot 6 | DOOR25 V0 (nur V0) */
    { 0x4001, 0, 0, -14130, -26500,   900,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 217, 111, 0x010CC },
    /* S220 T110 ROOM4010 -> ROOM4000 | ROOM4010.RDT@0x4D2 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4010, 0, 0, -20850, -24100,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 220, 110, 0x004D2 },
    /* S220 T110 ROOM4011 -> ROOM4000 | ROOM4011.RDT@0x4D2 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4011, 0, 0, -20850, -24100,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 220, 110, 0x004D2 },
    /* S224 T111 ROOM4030 -> ROOM4000 | ROOM4030.RDT@0x45E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x4030, 0, 0, -23050, -19750,   950,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 224, 111, 0x0045E },
    /* S225 T116 ROOM4030 -> ROOM4040 | ROOM4030.RDT@0x47E main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x4030, 1, 0,      0,      0,     0,     0, {-24190, -25170, -26630, -25250}, {-25490, -26560, -25220, -23930}, 0x25, 0, 0, 0xFF, 225, 116, 0x0047E },
    /* S224 T111 ROOM4031 -> ROOM4000 | ROOM4031.RDT@0x45E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x4031, 0, 0, -23050, -19750,   950,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 224, 111, 0x0045E },
    /* S225 T116 ROOM4031 -> ROOM4040 | ROOM4031.RDT@0x47E main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x4031, 1, 0,      0,      0,     0,     0, {-24190, -25170, -26630, -25250}, {-25490, -26560, -25220, -23930}, 0x25, 0, 0, 0xFF, 225, 116, 0x0047E },
    /* S227 T120 ROOM4040 -> ROOM4070 | ROOM4040.RDT@0xDDE main00 Slot 0 | DOOR29 V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x4040, 0, 0,  -4000, -18800,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 227, 120, 0x00DDE },
    /* S228 T116 ROOM4040 -> ROOM4030 | ROOM4040.RDT@0xE34 main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x4040, 0, 0,   3300,   9850,   600,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 228, 116, 0x00E34 },
    /* S229 T118 ROOM4040 -> ROOM4050 | ROOM4040.RDT@0xE54 main00 Slot 3 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4040, 0, 0, -27000,  17250,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 229, 118, 0x00E54 },
    /* S230 T119 ROOM4040 -> ROOM4050 | ROOM4040.RDT@0xE74 main00 Slot 4 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4040, 0, 0, -22600,   7050,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 230, 119, 0x00E74 },
    /* S227 T120 ROOM4041 -> ROOM4070 | ROOM4041.RDT@0xDDE main00 Slot 0 | DOOR29 V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x4041, 0, 0,  -4000, -18800,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 227, 120, 0x00DDE },
    /* S228 T116 ROOM4041 -> ROOM4030 | ROOM4041.RDT@0xE34 main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x4041, 0, 0,   3300,   9850,   600,   600, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 228, 116, 0x00E34 },
    /* S229 T118 ROOM4041 -> ROOM4050 | ROOM4041.RDT@0xE54 main00 Slot 3 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4041, 0, 0, -27000,  17250,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 229, 118, 0x00E54 },
    /* S230 T119 ROOM4041 -> ROOM4050 | ROOM4041.RDT@0xE74 main00 Slot 4 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4041, 0, 0, -22600,   7050,   350,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 230, 119, 0x00E74 },
    /* S231 T118 ROOM4050 -> ROOM4040 | ROOM4050.RDT@0x1B56 main00 Slot 0 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,   8850, -11600,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 231, 118, 0x01B56 },
    /* S233 T125 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1B96 main00 Slot 2 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,   7100,  -7450,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 233, 125, 0x01B96 },
    /* S234 T124 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1BB6 main00 Slot 3 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  -1000,  -9450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 234, 124, 0x01BB6 },
    /* S235 T119 ROOM4050 -> ROOM4040 | ROOM4050.RDT@0x1BD6 main00 Slot 4 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  -7650, -26800,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 235, 119, 0x01BD6 },
    /* S237 T122 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1C16 main00 Slot 6 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  -9300, -22700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 237, 122, 0x01C16 },
    /* S238 T121 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1C36 main00 Slot 7 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0, -17450, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 238, 121, 0x01C36 },
    /* S241 T122 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1C96 main00 Slot 10 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,  -9300,  -3700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 241, 122, 0x01C96 },
    /* S242 T125 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1CB6 main00 Slot 11 | DOOR29 V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x4050, 0, 0,  -9300, -17750,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 242, 125, 0x01CB6 },
    /* S243 T121 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1CD6 main00 Slot 12 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0,   1600,  -2100,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 243, 121, 0x01CD6 },
    /* S244 T124 ROOM4050 -> ROOM4050 | ROOM4050.RDT@0x1CF6 main00 Slot 13 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4050, 0, 0, -15850,  10650,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 244, 124, 0x01CF6 },
    /* S231 T118 ROOM4051 -> ROOM4040 | ROOM4051.RDT@0x1B56 main00 Slot 0 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,   8850, -11600,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 231, 118, 0x01B56 },
    /* S233 T125 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1B96 main00 Slot 2 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,   7100,  -7450,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 233, 125, 0x01B96 },
    /* S234 T124 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1BB6 main00 Slot 3 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  -1000,  -9450,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 234, 124, 0x01BB6 },
    /* S235 T119 ROOM4051 -> ROOM4040 | ROOM4051.RDT@0x1BD6 main00 Slot 4 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  -7650, -26800,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 235, 119, 0x01BD6 },
    /* S237 T122 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1C16 main00 Slot 6 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  -9300, -22700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 237, 122, 0x01C16 },
    /* S238 T121 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1C36 main00 Slot 7 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0, -17450, -24750,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 238, 121, 0x01C36 },
    /* S241 T122 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1C96 main00 Slot 10 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,  -9300,  -3700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 241, 122, 0x01C96 },
    /* S242 T125 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1CB6 main00 Slot 11 | DOOR29 V1 (Griff unsichtbar -> Komplement der Gegenseite (V0), RE2-Paar 0/1) */
    { 0x4051, 0, 0,  -9300, -17750,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 242, 125, 0x01CB6 },
    /* S243 T121 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1CD6 main00 Slot 12 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0,   1600,  -2100,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 243, 121, 0x01CD6 },
    /* S244 T124 ROOM4051 -> ROOM4050 | ROOM4051.RDT@0x1CF6 main00 Slot 13 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4051, 0, 0, -15850,  10650,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 244, 124, 0x01CF6 },
    /* S245 T120 ROOM4070 -> ROOM4040 | ROOM4070.RDT@0x11D6 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4070, 0, 12, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 245, 120, 0x011D6 },
    /* S246 T128 ROOM4070 -> ROOM50A0 | ROOM4070.RDT@0x11FE main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x4070, 0, 8, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 246, 128, 0x011FE },
    /* S247 T127 ROOM4070 -> ROOM5100 | ROOM4070.RDT@0x1238 main00 Slot 2 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4070, 0, 0, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 247, 127, 0x01238 },
    /* S245 T120 ROOM4071 -> ROOM4040 | ROOM4071.RDT@0x11D6 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4071, 0, 12, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 245, 120, 0x011D6 },
    /* S246 T128 ROOM4071 -> ROOM50A0 | ROOM4071.RDT@0x11FE main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x4071, 0, 8, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 246, 128, 0x011FE },
    /* S247 T127 ROOM4071 -> ROOM5100 | ROOM4071.RDT@0x1238 main00 Slot 2 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x4071, 0, 0, -12950, -18950,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 247, 127, 0x01238 },
    /* S249 T129 ROOM4080 -> ROOM40B0 | ROOM4080.RDT@0xAFC main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x4080, 0, 0,   1950, -25200,   350,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 249, 129, 0x00AFC },
    /* S250 T130 ROOM4080 -> ROOM40A0 | ROOM4080.RDT@0xB1E main00 Slot 3 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4080, 0, 0,  16650, -18600,   750,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 250, 130, 0x00B1E },
    /* S251 T131 ROOM4080 -> ROOM4090 | ROOM4080.RDT@0xB3E main00 Slot 4 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x4080, 0, 0,  21300,  -5400,   350,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 251, 131, 0x00B3E },
    /* S249 T129 ROOM4081 -> ROOM40B0 | ROOM4081.RDT@0xAFC main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x4081, 0, 0,   1950, -25200,   350,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 249, 129, 0x00AFC },
    /* S250 T130 ROOM4081 -> ROOM40A0 | ROOM4081.RDT@0xB1E main00 Slot 3 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x4081, 0, 0,  16650, -18600,   750,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 250, 130, 0x00B1E },
    /* S251 T131 ROOM4081 -> ROOM4090 | ROOM4081.RDT@0xB3E main00 Slot 4 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x4081, 0, 0,  21300,  -5400,   350,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 251, 131, 0x00B3E },
    /* S252 T131 ROOM4090 -> ROOM4080 | ROOM4090.RDT@0xA96 main00 Slot 0 | DOOR31 V0 (DOOR31 zweiteilig, Handrad links) */
    { 0x4090, 0, 0, -19700, -24850,  1200,  2350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x31, 0, 0, 0xFF, 252, 131, 0x00A96 },
    /* S252 T131 ROOM4091 -> ROOM4080 | ROOM4091.RDT@0xA96 main00 Slot 0 | DOOR31 V0 (DOOR31 zweiteilig, Handrad links) */
    { 0x4091, 0, 0, -19700, -24850,  1200,  2350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x31, 0, 0, 0xFF, 252, 131, 0x00A96 },
    /* S253 T130 ROOM40A0 -> ROOM4080 | ROOM40A0.RDT@0xDB6 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x40A0, 0, 0, -25600, -13900,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 253, 130, 0x00DB6 },
    /* S253 T130 ROOM40A1 -> ROOM4080 | ROOM40A1.RDT@0xDB6 main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x40A1, 0, 0, -25600, -13900,   900,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 253, 130, 0x00DB6 },
    /* S255 T129 ROOM40B0 -> ROOM4080 | ROOM40B0.RDT@0x602 main00 Slot 0 | DOOR26 V0 (Handrad unsichtbar; RE2 setzt an derselben gemalten Tuer (RE2 ROOM60F0 c01, Vorlaeufer-Hintergrund) DOOR26 V0) */
    { 0x40B0, 0, 0, -17825, -14575,   475,  1725, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 255, 129, 0x00602 },
    /* S255 T129 ROOM40B1 -> ROOM4080 | ROOM40B1.RDT@0x602 main00 Slot 0 | DOOR26 V0 (Handrad unsichtbar; RE2 setzt an derselben gemalten Tuer (RE2 ROOM60F0 c01, Vorlaeufer-Hintergrund) DOOR26 V0) */
    { 0x40B1, 0, 0, -17825, -14575,   475,  1725, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 255, 129, 0x00602 },
    /* S256 T135 ROOM5000 -> ROOM5030 | ROOM5000.RDT@0x7FE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5000, 0, 0,   -550,  -2100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 256, 135, 0x007FE },
    /* S257 T136 ROOM5000 -> ROOM5110 | ROOM5000.RDT@0x822 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5000, 0, 0,   -550,  -2100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 257, 136, 0x00822 },
    /* S258 T134 ROOM5000 -> ROOM5010 | ROOM5000.RDT@0x844 main00 Slot 1 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x5000, 0, 0,  -9600,   7100,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 258, 134, 0x00844 },
    /* S259 T133 ROOM5000 -> ROOM5020 | ROOM5000.RDT@0x864 main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5000, 0, 0, -20050,  -1900,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 259, 133, 0x00864 },
    /* S260 T115 ROOM5000 -> ROOM4020 | ROOM5000.RDT@0x8A4 main00 Slot 3 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x5000, 0, 0, -10550, -15600,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 260, 115, 0x008A4 },
    /* S256 T135 ROOM5001 -> ROOM5030 | ROOM5001.RDT@0x7FE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5001, 0, 0,   -550,  -2100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 256, 135, 0x007FE },
    /* S257 T136 ROOM5001 -> ROOM5110 | ROOM5001.RDT@0x822 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5001, 0, 0,   -550,  -2100,   500,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 257, 136, 0x00822 },
    /* S258 T134 ROOM5001 -> ROOM5010 | ROOM5001.RDT@0x844 main00 Slot 1 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x5001, 0, 0,  -9600,   7100,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 258, 134, 0x00844 },
    /* S259 T133 ROOM5001 -> ROOM5020 | ROOM5001.RDT@0x864 main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5001, 0, 0, -20050,  -1900,   500,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 259, 133, 0x00864 },
    /* S260 T115 ROOM5001 -> ROOM4020 | ROOM5001.RDT@0x8A4 main00 Slot 3 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x5001, 0, 0, -10550, -15600,  2000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 260, 115, 0x008A4 },
    /* S261 T134 ROOM5010 -> ROOM5000 | ROOM5010.RDT@0x39A main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x5010, 0, 0, -23950, -17700,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 261, 134, 0x0039A },
    /* S261 T134 ROOM5011 -> ROOM5000 | ROOM5011.RDT@0x39A main00 Slot 0 | DOOR29 V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x5011, 0, 0, -23950, -17700,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 0, 0, 0xFF, 261, 134, 0x0039A },
    /* S262 T133 ROOM5020 -> ROOM5000 | ROOM5020.RDT@0x11DE main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5020, 0, 0, -12350, -26400,  1850,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 262, 133, 0x011DE },
    /* S262 T133 ROOM5021 -> ROOM5000 | ROOM5021.RDT@0x11DE main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5021, 0, 0, -12350, -26400,  1850,   350, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 262, 133, 0x011DE },
    /* S264 T138 ROOM5030 -> ROOM5040 | ROOM5030.RDT@0xE66 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5030, 0, 0, -26100, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 264, 138, 0x00E66 },
    /* S265 T135 ROOM5030 -> ROOM5000 | ROOM5030.RDT@0xE86 main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5030, 0, 0, -15400,  -4000,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 265, 135, 0x00E86 },
    /* S266 T140 ROOM5030 -> ROOM5070 | ROOM5030.RDT@0xEA6 main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5030, 0, 0,  -4500, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 266, 140, 0x00EA6 },
    /* S267 T139 ROOM5030 -> ROOM50A0 | ROOM5030.RDT@0xEC6 main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x5030, 0, 0, -15300, -26300,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 267, 139, 0x00EC6 },
    /* S264 T138 ROOM5031 -> ROOM5040 | ROOM5031.RDT@0xE66 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5031, 0, 0, -26100, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 264, 138, 0x00E66 },
    /* S265 T135 ROOM5031 -> ROOM5000 | ROOM5031.RDT@0xE86 main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5031, 0, 0, -15400,  -4000,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 265, 135, 0x00E86 },
    /* S266 T140 ROOM5031 -> ROOM5070 | ROOM5031.RDT@0xEA6 main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5031, 0, 0,  -4500, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 266, 140, 0x00EA6 },
    /* S267 T139 ROOM5031 -> ROOM50A0 | ROOM5031.RDT@0xEC6 main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x5031, 0, 0, -15300, -26300,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 267, 139, 0x00EC6 },
    /* S268 T138 ROOM5040 -> ROOM5030 | ROOM5040.RDT@0x168E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5040, 0, 0,   2400, -14900,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 268, 138, 0x0168E },
    /* S270 T141 ROOM5040 -> ROOM5050 | ROOM5040.RDT@0x16CE main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5040, 0, 0, -24100,   -350,   800,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 270, 141, 0x016CE },
    /* S268 T138 ROOM5041 -> ROOM5030 | ROOM5041.RDT@0x168E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5041, 0, 0,   2400, -14900,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 268, 138, 0x0168E },
    /* S270 T141 ROOM5041 -> ROOM5050 | ROOM5041.RDT@0x16CE main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5041, 0, 0, -24100,   -350,   800,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 270, 141, 0x016CE },
    /* S271 T141 ROOM5050 -> ROOM5040 | ROOM5050.RDT@0x70A main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5050, 0, 0, -26425, -18250,   475,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 271, 141, 0x0070A },
    /* S272 T141 ROOM5050 -> ROOM5120 | ROOM5050.RDT@0x72E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5050, 0, 0, -26425, -18250,   475,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 272, 141, 0x0072E },
    /* S271 T141 ROOM5051 -> ROOM5040 | ROOM5051.RDT@0x70A main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5051, 0, 0, -26425, -18250,   475,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 271, 141, 0x0070A },
    /* S272 T141 ROOM5051 -> ROOM5120 | ROOM5051.RDT@0x72E main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5051, 0, 0, -26425, -18250,   475,  1050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 272, 141, 0x0072E },
    /* S275 T140 ROOM5070 -> ROOM5030 | ROOM5070.RDT@0x9BE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5070, 0, 0,   2550,  -5650,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 275, 140, 0x009BE },
    /* S276 T158 ROOM5070 -> ROOM6020 | ROOM5070.RDT@0x9DE main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x5070, 0, 0, -28900,  13300,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 276, 158, 0x009DE },
    /* S277 T157 ROOM5070 -> ROOM6020 | ROOM5070.RDT@0x9FE main00 Slot 2 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x5070, 0, 0, -28900, -24650,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 277, 157, 0x009FE },
    /* S275 T140 ROOM5071 -> ROOM5030 | ROOM5071.RDT@0x9BE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5071, 0, 0,   2550,  -5650,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 275, 140, 0x009BE },
    /* S276 T158 ROOM5071 -> ROOM6020 | ROOM5071.RDT@0x9DE main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x5071, 0, 0, -28900,  13300,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 276, 158, 0x009DE },
    /* S277 T157 ROOM5071 -> ROOM6020 | ROOM5071.RDT@0x9FE main00 Slot 2 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x5071, 0, 0, -28900, -24650,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 277, 157, 0x009FE },
    /* S278 T143 ROOM5080 -> ROOM6010 | ROOM5080.RDT@0x6FA main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x5080, 0, 0,  -9100, -18050,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 278, 143, 0x006FA },
    /* S278 T143 ROOM5081 -> ROOM6010 | ROOM5081.RDT@0x6FA main00 Slot 0 | DOOR1A V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x5081, 0, 0,  -9100, -18050,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 0, 0, 0xFF, 278, 143, 0x006FA },
    /* S279 T145 ROOM5090 -> ROOM5090 | ROOM5090.RDT@0x104E main00 Slot 0 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5090, 0, 0,   -550,   5125,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 279, 145, 0x0104E },
    /* S280 T147 ROOM5090 -> ROOM6030 | ROOM5090.RDT@0x106E main00 Slot 1 | DOOR2A V0 (Drehriegel links) */
    { 0x5090, 0, 0,  28150,   5000,   450,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 280, 147, 0x0106E },
    /* S281 T144 ROOM5090 -> ROOM5090 | ROOM5090.RDT@0x108E main00 Slot 2 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5090, 0, 0,   -550,  -9175,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 281, 144, 0x0108E },
    /* S282 T145 ROOM5090 -> ROOM5090 | ROOM5090.RDT@0x10AE main00 Slot 3 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5090, 0, 0,  25600,  -9175,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 282, 145, 0x010AE },
    /* S283 T144 ROOM5090 -> ROOM5090 | ROOM5090.RDT@0x10CE main00 Slot 4 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5090, 0, 0,  25600, -23325,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 283, 144, 0x010CE },
    /* S279 T145 ROOM5091 -> ROOM5090 | ROOM5091.RDT@0x104E main00 Slot 0 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5091, 0, 0,   -550,   5125,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 279, 145, 0x0104E },
    /* S280 T147 ROOM5091 -> ROOM6030 | ROOM5091.RDT@0x106E main00 Slot 1 | DOOR2A V0 (Drehriegel links) */
    { 0x5091, 0, 0,  28150,   5000,   450,   950, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 280, 147, 0x0106E },
    /* S281 T144 ROOM5091 -> ROOM5090 | ROOM5091.RDT@0x108E main00 Slot 2 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5091, 0, 0,   -550,  -9175,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 281, 144, 0x0108E },
    /* S282 T145 ROOM5091 -> ROOM5090 | ROOM5091.RDT@0x10AE main00 Slot 3 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5091, 0, 0,  25600,  -9175,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 282, 145, 0x010AE },
    /* S283 T144 ROOM5091 -> ROOM5090 | ROOM5091.RDT@0x10CE main00 Slot 4 | DOOR2A V2 (zweifluegelig, wie RE2-Paar 2/2 (ROOM7040 c12)) */
    { 0x5091, 0, 0,  25600, -23325,   450,  1975, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 2, 0, 0xFF, 283, 144, 0x010CE },
    /* S285 T128 ROOM50A0 -> ROOM4070 | ROOM50A0.RDT@0x154A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x50A0, 0, 0, -22880, -16325,   700,  1115, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 285, 128, 0x0154A },
    /* S286 T139 ROOM50A0 -> ROOM5030 | ROOM50A0.RDT@0x156A main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x50A0, 0, 0, -12470,   5410,  1700,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 286, 139, 0x0156A },
    /* S285 T128 ROOM50A1 -> ROOM4070 | ROOM50A1.RDT@0x154A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x50A1, 0, 0, -22880, -16325,   700,  1115, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 285, 128, 0x0154A },
    /* S286 T139 ROOM50A1 -> ROOM5030 | ROOM50A1.RDT@0x156A main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x50A1, 0, 0, -12470,   5410,  1700,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 286, 139, 0x0156A },
    /* S287 T149 ROOM50B0 -> ROOM5140 | ROOM50B0.RDT@0x48E main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50B0, 0, 0, -14013, -27000,  2137,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 287, 149, 0x0048E },
    /* S288 T148 ROOM50B0 -> ROOM6000 | ROOM50B0.RDT@0x4AE main00 Slot 1 | DOOR2D V5 (HANGAR -> GATE PLATFORM = hinab, wie RE2 ROOM6170 -> ROOM7000 (DOOR2D V5)) */
    { 0x50B0, 0, 0, -20000,  -5750,   800,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 5, 0, 0xFF, 288, 148, 0x004AE },
    /* S287 T149 ROOM50B1 -> ROOM5140 | ROOM50B1.RDT@0x496 main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50B1, 0, 0, -14013, -27000,  2137,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 287, 149, 0x00496 },
    /* S288 T148 ROOM50B1 -> ROOM6000 | ROOM50B1.RDT@0x4B6 main00 Slot 1 | DOOR2D V5 (HANGAR -> GATE PLATFORM = hinab, wie RE2 ROOM6170 -> ROOM7000 (DOOR2D V5)) */
    { 0x50B1, 0, 0, -20000,  -5750,   800,   450, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 5, 0, 0xFF, 288, 148, 0x004B6 },
    /* S289 T150 ROOM50C0 -> ROOM50D0 | ROOM50C0.RDT@0x90A main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50C0, 0, 0, -24750, -26150,  1450,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 289, 150, 0x0090A },
    /* S290 T114 ROOM50C0 -> ROOM4020 | ROOM50C0.RDT@0x94A main00 Slot 1 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x50C0, 0, 0,  -7700,  -9500,   800,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 290, 114, 0x0094A },
    /* S289 T150 ROOM50C1 -> ROOM50D0 | ROOM50C1.RDT@0x90A main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50C1, 0, 0, -24750, -26150,  1450,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 289, 150, 0x0090A },
    /* S290 T114 ROOM50C1 -> ROOM4020 | ROOM50C1.RDT@0x94A main00 Slot 1 | DOOR27 V0 (V0..V3 gleich, RE2-Paare 0/0) */
    { 0x50C1, 0, 0,  -7700,  -9500,   800,  1700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x27, 0, 0, 0xFF, 290, 114, 0x0094A },
    /* S291 T150 ROOM50D0 -> ROOM50C0 | ROOM50D0.RDT@0x11DA main00 Slot 0 | DOOR26 V0 (Handrad unsichtbar; Anordnung Aufkleber links / TYPE-P rechts = V0 (Pruefer 7)) */
    { 0x50D0, 0, 3, -24160,  -1959,  1600,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 291, 150, 0x011DA },
    /* S292 T152 ROOM50D0 -> ROOM50F0 | ROOM50D0.RDT@0x11FA main00 Slot 1 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D0, 0, 0,  -2021, -14842,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 292, 152, 0x011FA },
    /* S293 T153 ROOM50D0 -> ROOM50E0 | ROOM50D0.RDT@0x121A main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D0, 0, 3,  -2021,  -3592,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 293, 153, 0x0121A },
    /* S294 T151 ROOM50D0 -> ROOM5100 | ROOM50D0.RDT@0x123A main00 Slot 3 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D0, 0, 0,  -2021, -24592,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 294, 151, 0x0123A },
    /* S291 T150 ROOM50D1 -> ROOM50C0 | ROOM50D1.RDT@0x11DA main00 Slot 0 | DOOR26 V0 (Handrad unsichtbar; Anordnung Aufkleber links / TYPE-P rechts = V0 (Pruefer 7)) */
    { 0x50D1, 0, 3, -24160,  -1959,  1600,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 291, 150, 0x011DA },
    /* S292 T152 ROOM50D1 -> ROOM50F0 | ROOM50D1.RDT@0x11FA main00 Slot 1 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D1, 0, 0,  -2021, -14842,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 292, 152, 0x011FA },
    /* S293 T153 ROOM50D1 -> ROOM50E0 | ROOM50D1.RDT@0x121A main00 Slot 2 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D1, 0, 3,  -2021,  -3592,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 293, 153, 0x0121A },
    /* S294 T151 ROOM50D1 -> ROOM5100 | ROOM50D1.RDT@0x123A main00 Slot 3 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50D1, 0, 0,  -2021, -24592,   329,  1058, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 294, 151, 0x0123A },
    /* S295 T153 ROOM50E0 -> ROOM50D0 | ROOM50E0.RDT@0xA86 main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50E0, 0, 0, -26150, -14600,   450,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 295, 153, 0x00A86 },
    /* S295 T153 ROOM50E1 -> ROOM50D0 | ROOM50E1.RDT@0xA86 main00 Slot 0 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x50E1, 0, 0, -26150, -14600,   450,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 295, 153, 0x00A86 },
    /* S296 T152 ROOM50F0 -> ROOM50D0 | ROOM50F0.RDT@0x94E main00 Slot 0 | DOOR26 V1 (zweiteilig, Handrad rechts) */
    { 0x50F0, 0, 0, -25350, -23600,   450,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 1, 0, 0xFF, 296, 152, 0x0094E },
    /* S296 T152 ROOM50F1 -> ROOM50D0 | ROOM50F1.RDT@0x94E main00 Slot 0 | DOOR26 V1 (zweiteilig, Handrad rechts) */
    { 0x50F1, 0, 0, -25350, -23600,   450,  2000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 1, 0, 0xFF, 296, 152, 0x0094E },
    /* S297 T127 ROOM5100 -> ROOM4070 | ROOM5100.RDT@0x802 main00 Slot 2 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x5100, 0, 0, -23200, -15800,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 297, 127, 0x00802 },
    /* S298 T151 ROOM5100 -> ROOM50D0 | ROOM5100.RDT@0x822 main00 Slot 3 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5100, 0, 0, -19200,  -1050,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 298, 151, 0x00822 },
    /* S297 T127 ROOM5101 -> ROOM4070 | ROOM5101.RDT@0x802 main00 Slot 2 | DOOR29 V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x5101, 0, 0, -23200, -15800,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x29, 1, 0, 0xFF, 297, 127, 0x00802 },
    /* S298 T151 ROOM5101 -> ROOM50D0 | ROOM5101.RDT@0x822 main00 Slot 3 | DOOR26 V0 (zweiteilig, Handrad links) */
    { 0x5101, 0, 0, -19200,  -1050,   800,  2050, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 0, 0, 0xFF, 298, 151, 0x00822 },
    /* S299 T154 ROOM5110 -> ROOM5120 | ROOM5110.RDT@0xD5A main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5110, 0, 0, -26100, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 299, 154, 0x00D5A },
    /* S300 T136 ROOM5110 -> ROOM5000 | ROOM5110.RDT@0xD7A main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5110, 0, 0, -15400,  -4000,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 300, 136, 0x00D7A },
    /* S301 T156 ROOM5110 -> ROOM5130 | ROOM5110.RDT@0xD9A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5110, 0, 0,  -4500, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 301, 156, 0x00D9A },
    /* S302 T155 ROOM5110 -> ROOM5140 | ROOM5110.RDT@0xDBA main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x5110, 0, 0, -15300, -26300,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 302, 155, 0x00DBA },
    /* S299 T154 ROOM5111 -> ROOM5120 | ROOM5111.RDT@0xD5A main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5111, 0, 0, -26100, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 299, 154, 0x00D5A },
    /* S300 T136 ROOM5111 -> ROOM5000 | ROOM5111.RDT@0xD7A main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5111, 0, 0, -15400,  -4000,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 300, 136, 0x00D7A },
    /* S301 T156 ROOM5111 -> ROOM5130 | ROOM5111.RDT@0xD9A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5111, 0, 0,  -4500, -15400,  1200,  1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 301, 156, 0x00D9A },
    /* S302 T155 ROOM5111 -> ROOM5140 | ROOM5111.RDT@0xDBA main00 Slot 3 | DOOR25 V0 (nur V0) */
    { 0x5111, 0, 0, -15300, -26300,  1800,  1200, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 302, 155, 0x00DBA },
    /* S303 T154 ROOM5120 -> ROOM5110 | ROOM5120.RDT@0x1406 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5120, 0, 0,   3300, -14900,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 303, 154, 0x01406 },
    /* S304 T141 ROOM5120 -> ROOM5050 | ROOM5120.RDT@0x143A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5120, 0, 0, -24100,   -350,   800,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 304, 141, 0x0143A },
    /* S303 T154 ROOM5121 -> ROOM5110 | ROOM5121.RDT@0x1406 main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5121, 0, 0,   3300, -14900,   800,  1100, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 303, 154, 0x01406 },
    /* S304 T141 ROOM5121 -> ROOM5050 | ROOM5121.RDT@0x143A main00 Slot 2 | DOOR25 V0 (nur V0) */
    { 0x5121, 0, 0, -24100,   -350,   800,  1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 304, 141, 0x0143A },
    /* S306 T156 ROOM5130 -> ROOM5110 | ROOM5130.RDT@0x9BE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5130, 0, 0,   2550,  -5650,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 306, 156, 0x009BE },
    /* S307 T158 ROOM5130 -> ROOM6020 | ROOM5130.RDT@0x9DE main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x5130, 0, 0, -28900,  13300,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 307, 158, 0x009DE },
    /* S308 T157 ROOM5130 -> ROOM6020 | ROOM5130.RDT@0x9FE main00 Slot 2 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x5130, 0, 0, -28900, -24650,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 308, 157, 0x009FE },
    /* S306 T156 ROOM5131 -> ROOM5110 | ROOM5131.RDT@0x9BE main00 Slot 0 | DOOR25 V0 (nur V0) */
    { 0x5131, 0, 0,   2550,  -5650,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 306, 156, 0x009BE },
    /* S307 T158 ROOM5131 -> ROOM6020 | ROOM5131.RDT@0x9DE main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x5131, 0, 0, -28900,  13300,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 307, 158, 0x009DE },
    /* S308 T157 ROOM5131 -> ROOM6020 | ROOM5131.RDT@0x9FE main00 Slot 2 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x5131, 0, 0, -28900, -24650,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 308, 157, 0x009FE },
    /* S309 T149 ROOM5140 -> ROOM50B0 | ROOM5140.RDT@0x1A5E main00 Slot 0 | DOOR26 V1 (zweiteilig, Handrad rechts) */
    { 0x5140, 0, 0,  12515, -10030,  2525,   820, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 1, 0, 0xFF, 309, 149, 0x01A5E },
    /* S310 T155 ROOM5140 -> ROOM5110 | ROOM5140.RDT@0x1A7E main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5140, 0, 0, -12470,   5410,  1700,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 310, 155, 0x01A7E },
    /* S309 T149 ROOM5141 -> ROOM50B0 | ROOM5141.RDT@0x1A5E main00 Slot 0 | DOOR26 V1 (zweiteilig, Handrad rechts) */
    { 0x5141, 0, 0,  12515, -10030,  2525,   820, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 1, 0, 0xFF, 309, 149, 0x01A5E },
    /* S310 T155 ROOM5141 -> ROOM5110 | ROOM5141.RDT@0x1A7E main00 Slot 1 | DOOR25 V0 (nur V0) */
    { 0x5141, 0, 0, -12470,   5410,  1700,   750, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x25, 0, 0, 0xFF, 310, 155, 0x01A7E },
    /* S311 T148 ROOM6000 -> ROOM50B0 | ROOM6000.RDT@0xD58 main00 Slot 0 | DOOR2D V4 (GATE PLATFORM -> HANGAR = hinauf; V4 als Gegenstueck zu RE2s V5 (Skript 1, 301 Bilder) - Richtung von Skript 1 NICHT per Simulator belegt, RE2 nutzt 2D nur mit V5) */
    { 0x6000, 0, 2,   2800,   2750,   800,   550, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 4, 0, 0xFF, 311, 148, 0x00D58 },
    /* S312 T160 ROOM6000 -> ROOM6010 | ROOM6000.RDT@0xD78 main00 Slot 1 | DOOR2E V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6000, 0, 2,  -4900,  -8350,   450,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 1, 0, 0xFF, 312, 160, 0x00D78 },
    /* S313 T159 ROOM6000 -> ROOM6010 | ROOM6000.RDT@0xDB8 main00 Slot 2 | DOOR2E V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6000, 0, 2,  -4900, -20400,   450,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 1, 0, 0xFF, 313, 159, 0x00DB8 },
    /* S314 T161 ROOM6000 -> ROOM6030 | ROOM6000.RDT@0xDDA main00 Slot 3 | DOOR2A V0 (Riegel unsichtbar -> V0 wie RE2s Zug-Aussentuer ROOM7000 c5 (DOOR2A V0)) */
    { 0x6000, 0, 2,   1763, -10677,  1070,   427, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 314, 161, 0x00DDA },
    /* S311 T148 ROOM6001 -> ROOM50B0 | ROOM6001.RDT@0xD58 main00 Slot 0 | DOOR2D V4 (GATE PLATFORM -> HANGAR = hinauf; V4 als Gegenstueck zu RE2s V5 (Skript 1, 301 Bilder) - Richtung von Skript 1 NICHT per Simulator belegt, RE2 nutzt 2D nur mit V5) */
    { 0x6001, 0, 2,   2800,   2750,   800,   550, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2D, 4, 0, 0xFF, 311, 148, 0x00D58 },
    /* S312 T160 ROOM6001 -> ROOM6010 | ROOM6001.RDT@0xD78 main00 Slot 1 | DOOR2E V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6001, 0, 2,  -4900,  -8350,   450,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 1, 0, 0xFF, 312, 160, 0x00D78 },
    /* S313 T159 ROOM6001 -> ROOM6010 | ROOM6001.RDT@0xDB8 main00 Slot 2 | DOOR2E V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6001, 0, 2,  -4900, -20400,   450,   900, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 1, 0, 0xFF, 313, 159, 0x00DB8 },
    /* S314 T161 ROOM6001 -> ROOM6030 | ROOM6001.RDT@0xDDA main00 Slot 3 | DOOR2A V0 (Riegel unsichtbar -> V0 wie RE2s Zug-Aussentuer ROOM7000 c5 (DOOR2A V0)) */
    { 0x6001, 0, 2,   1763, -10677,  1070,   427, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 314, 161, 0x00DDA },
    /* S315 T160 ROOM6010 -> ROOM6000 | ROOM6010.RDT@0xF4C main00 Slot 0 | DOOR2E V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x6010, 0, 0,  -5215,  -8038,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 0, 0, 0xFF, 315, 160, 0x00F4C },
    /* S316 T159 ROOM6010 -> ROOM6000 | ROOM6010.RDT@0xF88 main00 Slot 1, ROOM6010.RDT@0xFAC main00 Slot 1 | DOOR2E V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x6010, 0, 0,  -5215, -20638,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 0, 0, 0xFF, 316, 159, 0x00F88 },
    /* S317 T143 ROOM6010 -> ROOM5080 | ROOM6010.RDT@0xFCE main00 Slot 2 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6010, 0, 0, -27015, -25588,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 317, 143, 0x00FCE },
    /* S315 T160 ROOM6011 -> ROOM6000 | ROOM6011.RDT@0xF4C main00 Slot 0 | DOOR2E V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x6011, 0, 0,  -5215,  -8038,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 0, 0, 0xFF, 315, 160, 0x00F4C },
    /* S316 T159 ROOM6011 -> ROOM6000 | ROOM6011.RDT@0xF88 main00 Slot 1, ROOM6011.RDT@0xFAC main00 Slot 1 | DOOR2E V0 (Griff links -> V0 (T2 2.4 Punkt 1)) */
    { 0x6011, 0, 0,  -5215, -20638,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2E, 0, 0, 0xFF, 316, 159, 0x00F88 },
    /* S317 T143 ROOM6011 -> ROOM5080 | ROOM6011.RDT@0xFCE main00 Slot 2 | DOOR1A V1 (Griff rechts -> V1 (T2 2.4 Punkt 1)) */
    { 0x6011, 0, 0, -27015, -25588,   435,  1112, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x1A, 1, 0, 0xFF, 317, 143, 0x00FCE },
    /* S318 T158 ROOM6020 -> ROOM5130 | ROOM6020.RDT@0xB46 main00 Slot 0 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x6020, 0, 0,  -4500,   1350,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 318, 158, 0x00B46 },
    /* S319 T157 ROOM6020 -> ROOM5130 | ROOM6020.RDT@0xB66 main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x6020, 0, 0,  -4050, -17700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 319, 157, 0x00B66 },
    /* S318 T158 ROOM6021 -> ROOM5130 | ROOM6021.RDT@0xB46 main00 Slot 0 | DOOR26 V2 (einteilig, Baender links, Handrad rechts) */
    { 0x6021, 0, 0,  -4500,   1350,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 2, 0, 0xFF, 318, 158, 0x00B46 },
    /* S319 T157 ROOM6021 -> ROOM5130 | ROOM6021.RDT@0xB66 main00 Slot 1 | DOOR26 V3 (einteilig, Baender rechts, Handrad links) */
    { 0x6021, 0, 0,  -4050, -17700,  1000,   500, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x26, 3, 0, 0xFF, 319, 157, 0x00B66 },
    /* S320 T162 ROOM6030 -> ROOM6030 | ROOM6030.RDT@0xC32 main00 Slot 0 | DOOR2A V0 (Riegel unsichtbar -> Komplement zu S321 (V1)) */
    { 0x6030, 0, 0,  -4650, -23350,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 320, 162, 0x00C32 },
    /* S321 T162 ROOM6030 -> ROOM6030 | ROOM6030.RDT@0xC52 main00 Slot 1 | DOOR2A V1 (Drehriegel rechts) */
    { 0x6030, 0, 1,  -8850, -23500,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 1, 0, 0xFF, 321, 162, 0x00C52 },
    /* S323 T147 ROOM6030 -> ROOM5090 | ROOM6030.RDT@0xC92 main00 Slot 3 | DOOR2A V1 (Drehriegel rechts) */
    { 0x6030, 0, 0, -27200, -23250,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 1, 0, 0xFF, 323, 147, 0x00C92 },
    /* S320 T162 ROOM6031 -> ROOM6030 | ROOM6031.RDT@0xC32 main00 Slot 0 | DOOR2A V0 (Riegel unsichtbar -> Komplement zu S321 (V1)) */
    { 0x6031, 0, 0,  -4650, -23350,   450,   850, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 0, 0, 0xFF, 320, 162, 0x00C32 },
    /* S321 T162 ROOM6031 -> ROOM6030 | ROOM6031.RDT@0xC52 main00 Slot 1 | DOOR2A V1 (Drehriegel rechts) */
    { 0x6031, 0, 1,  -8850, -23500,   450,   700, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 1, 0, 0xFF, 321, 162, 0x00C52 },
    /* S323 T147 ROOM6031 -> ROOM5090 | ROOM6031.RDT@0xC92 main00 Slot 3 | DOOR2A V1 (Drehriegel rechts) */
    { 0x6031, 0, 0, -27200, -23250,   450,  1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x2A, 1, 0, 0xFF, 323, 147, 0x00C92 },
};

/* Griff-Tausch (tueren_02_re2.md 2.5/3): Griff-Mesh des Spenders am Anhaengepunkt des Archivs,
 * mit Spender-Textur/CLUT. ⛔ PORT-WAHL: RE2 tauscht Griffe nur archivintern per Bit 7 (DOOR01/05).
 * Werte [SIM] (tools/tor/tuerkatalog.py): mesh_archiv = Griff-Mesh des Archivs (Kind des Blatts),
 * mesh_spender = Griff-Mesh des Spenders, rot_vorn/rot_hinten = Grund-Drehung des Spendergriffs
 * (x > 0 vorn, x < 0 hinten; Door_model_set im ersten Bild), aus_archiv/aus_spender = groesster
 * Ausschlag rot x beim Oeffnen (Knauf dreht, Druecker kippt, Stange steht). */
static const re15_griff_tausch_t re15_griff_tausche[2] = {
    /* DOOR09 <- DOOR04: Anker Archiv [130, -3224, -3372], Anker Spender [130, -3124, -3308] */
    { 0x09, 0x04, 1, 1, {0, 0, 0}, {0, 2048, 0}, 1500, 0 },
    /* DOOR13 <- DOOR07: Anker Archiv [130, -3364, -3342], Anker Spender [130, -3352, -3404] */
    { 0x13, 0x07, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 1500, -702 },
};
