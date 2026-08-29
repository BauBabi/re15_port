/* GENERIERT (Ableitung: analysis/nutzer_batch_2026-08-30b/map-zuordnung.md):
 * Raum-Id -> (Karten-Seite, Rect-Index der EXE-Rect-Liste @0x80076840).
 * HERKUNFT je Basis-Raum: 'seed' = Mehrheits-Voting aller AOT-Zentren des Raums,
 * projiziert durch die EXE-EIGENE Marker-Zeile des Raums (@0x800768B0 + 8*Slot,
 * Slot = Stage-Basis {0,38,50,65,77,98} + Raum-Index, Formel FUN_800473f8
 * @0x8004741c-528), kleinste-Rect-Containment; 'seed-med' = relative Mehrheit +
 * Median-Punkt-Bestaetigung. Szenario-Varianten (0xSRR1) erben die Basis-Zuordnung
 * (Seite/Slot haengen nur am Raum-Index: FUN_8004b568 liest 0x800b0fe2).
 * 65 von 120 Basis-Raeumen tragen im Auslieferungsstand nur die PLATZHALTER-Zeile
 * {0,0,1,1} (Stock-Marker dort defekt) und bleiben hier bewusst UNZUGEORDNET ->
 * ihr Rect behaelt den Stock-Neutralton. Mehrere Raeume je Rect sind legitim
 * (Flur-/Phasen-Teilung, z.B. 5040+5120). */
typedef struct { unsigned short room; unsigned char page, rect; } re15_map_room_rect_t;
static const re15_map_room_rect_t s_map_room_rects[] = {
    { 0x1020,  2,  1 },  /* seed 19/27 med=1 */
    { 0x1021,  2,  1 },  /* seed 19/27 med=1 */
    { 0x1030,  2,  4 },  /* seed-med 22/40 med=4 */
    { 0x1031,  2,  4 },  /* seed-med 22/40 med=4 */
    { 0x1040,  2,  2 },  /* seed 12/17 med=2 */
    { 0x1041,  2,  2 },  /* seed 12/17 med=2 */
    { 0x1070,  2,  3 },  /* seed 28/30 med=3 */
    { 0x1071,  2,  3 },  /* seed 28/30 med=3 */
    { 0x1150,  4,  2 },  /* seed 13/13 med=2 */
    { 0x1151,  4,  2 },  /* seed 13/13 med=2 */
    { 0x1170,  5,  1 },  /* seed 7/7 med=None */
    { 0x1171,  5,  1 },  /* seed 7/7 med=None */
    { 0x1190,  0,  1 },  /* seed-med 24/46 med=1 */
    { 0x1191,  0,  1 },  /* seed-med 24/46 med=1 */
    { 0x11B0,  0,  3 },  /* seed 20/20 med=3 */
    { 0x11B1,  0,  3 },  /* seed 20/20 med=3 */
    { 0x1200,  1,  2 },  /* seed 6/8 med=2 */
    { 0x1201,  1,  2 },  /* seed 6/8 med=2 */
    { 0x2000,  6,  0 },  /* seed 10/16 med=0 */
    { 0x2001,  6,  0 },  /* seed 10/16 med=0 */
    { 0x3000,  7,  0 },  /* seed 4/4 med=0 */
    { 0x3001,  7,  0 },  /* seed 4/4 med=0 */
    { 0x3030,  7,  3 },  /* seed 6/6 med=3 */
    { 0x3031,  7,  3 },  /* seed 6/6 med=3 */
    { 0x3040,  7,  4 },  /* seed 20/26 med=4 */
    { 0x3041,  7,  4 },  /* seed 20/26 med=4 */
    { 0x3070,  7,  7 },  /* seed 6/8 med=7 */
    { 0x3071,  7,  7 },  /* seed 6/8 med=7 */
    { 0x3080,  7,  0 },  /* seed 2/2 med=0 */
    { 0x3081,  7,  0 },  /* seed 2/2 med=0 */
    { 0x4010,  8,  1 },  /* seed 24/24 med=1 */
    { 0x4011,  8,  1 },  /* seed 24/24 med=1 */
    { 0x4030,  8,  3 },  /* seed 12/14 med=3 */
    { 0x4031,  8,  3 },  /* seed 12/14 med=3 */
    { 0x4090,  8,  9 },  /* seed 8/8 med=9 */
    { 0x4091,  8,  9 },  /* seed 8/8 med=9 */
    { 0x40A0,  8, 10 },  /* seed 22/22 med=10 */
    { 0x40A1,  8, 10 },  /* seed 22/22 med=10 */
    { 0x5010,  9,  1 },  /* seed 21/21 med=1 */
    { 0x5011,  9,  1 },  /* seed 21/21 med=1 */
    { 0x5030,  9,  3 },  /* seed-med 7/13 med=3 */
    { 0x5031,  9,  3 },  /* seed-med 7/13 med=3 */
    { 0x5040,  9,  4 },  /* seed 12/12 med=4 */
    { 0x5041,  9,  4 },  /* seed 12/12 med=4 */
    { 0x5060,  9,  6 },  /* seed 40/40 med=6 */
    { 0x5061,  9,  6 },  /* seed 40/40 med=6 */
    { 0x50A0,  9, 10 },  /* seed 12/16 med=10 */
    { 0x50A1,  9, 10 },  /* seed 12/16 med=10 */
    { 0x50C0, 10,  0 },  /* seed-med 6/10 med=0 */
    { 0x50C1, 10,  0 },  /* seed-med 6/10 med=0 */
    { 0x5110,  9,  3 },  /* seed-med 4/8 med=3 */
    { 0x5111,  9,  3 },  /* seed-med 4/8 med=3 */
    { 0x5120,  9,  4 },  /* seed 10/12 med=4 */
    { 0x5121,  9,  4 },  /* seed 10/12 med=4 */
    { 0x6030, 12,  5 },  /* seed-med 5/8 med=5 */
    { 0x6031, 12,  5 },  /* seed-med 5/8 med=5 */
};
