/* Runde 37 Spur P: an der exe gemessene Szenen (Dossier analysis/befunde_runde37/P_masken36.md §4, Laeufe p37/runs/<name>_A/B/V/N).
 * Lage = nach der Kollision gemessene Lage (RE15_STATE_LOG), Cut = RE15_FORCE_CUT, Band 0 ausser ROOM6000 (Band 2).
 * hinter: 1 = HINTER einem Objekt (Fuss + >= halbe Figur verdeckt), 0 = DAVOR (ueberlappt die Maske, nichts verdeckt),
 *         2 = FREI (frueher von Generator-Rechtecken fehlverdeckt, jetzt kein Punkt verdeckt). */
static const szene_t SZENEN[] = {
    { "h3050c2",  0x3050,  2, -22000, -20768, 0, 1 },   /* exe: 1123 von 1234 Figurpunkten verdeckt */
    { "v3050c2",  0x3050,  2, -20400, -14368, 0, 0 },   /* exe: 0 von 684 */
    { "h30A0c3",  0x30A0,  3, -19800,  19763, 0, 1 },   /* exe: 1933 von 2824 */
    { "v30A0c4",  0x30A0,  4, -21843,  17763, 0, 0 },   /* exe: 0 von 8210 */
    { "f3050c5",  0x3050,  5, -13300, -16368, 0, 2 },   /* exe: ohne Ausschluss 538 geloescht, jetzt 0 */
    /* Nachbesserung 1: h4040c4 / h4080c1 / v4040c7 entfallen - mit der Zellform aus dem Resolver (schraege Gaenge) sind
     * die Rechtecke dort ausgeschlossen, weil sie an erreichbaren Stellen eine Figur VOR dem Wandblock uebermalten
     * (exe alte P-Tabelle: ROOM4040 Cut 4 (-22000,-3722) 57 von 397 Punkten, jetzt 0). Ersatz in ROOM4080 Cut 5: */
    { "h4080c5",  0x4080,  5,  17275, -12710, 0, 1 },   /* exe: 879 von 883 (alte und neue Tabelle) */
    { "v4080c5",  0x4080,  5,   9675,  -9310, 0, 0 },   /* exe: 0 von 8374 */
    { "f30A1c0",  0x30A1,  0, -24284,  13657, 0, 2 },   /* M1, Elza gelaufen ab der Tuer: alte Tabelle 265 von 464, jetzt 0 */
    { "v40A0c7",  0x40A0,  7, -17592, -24095, 0, 0 },   /* exe: 0 von 3442 */
    { "h5020c0",  0x5020,  0,  -6933, -22525, 0, 1 },   /* exe: 741 von 794 */
    { "h5020c9",  0x5020,  9, -13133, -21625, 0, 1 },   /* exe: 199 von 322 */
    { "v5020c9",  0x5020,  9, -12133, -13325, 0, 0 },   /* exe: 0 von 5007 */
    { "h6010c12", 0x6010, 12, -18612,  -7920, 0, 1 },   /* exe: 820 von 1440 */
    { "h6010c13", 0x6010, 13, -18812, -20920, 0, 1 },   /* exe: 903 von 1250 */
    { "v6010c12", 0x6010, 12, -14412,  -5720, 0, 0 },   /* exe: 0 von 9489 */
    { "v6000c3",  0x6000,  3,   4933,  -6384, 2, 0 },   /* exe: 0 von 4495 (Band 2) */
};
