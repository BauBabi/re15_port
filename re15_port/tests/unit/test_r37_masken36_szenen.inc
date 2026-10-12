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
    { "h4040c4",  0x4040,  4, -23200,   -922, 0, 1 },   /* exe: 146 von 208 */
    { "h4080c1",  0x4080,  1,  21875,  -5210, 0, 1 },   /* exe: 182 von 186 */
    { "v4040c7",  0x4040,  7,  -7200, -20022, 0, 0 },   /* exe: 0 von 1170 */
    { "v40A0c7",  0x40A0,  7, -17592, -24095, 0, 0 },   /* exe: 0 von 3442 */
    { "h5020c0",  0x5020,  0,  -6933, -22525, 0, 1 },   /* exe: 741 von 794 */
    { "h5020c9",  0x5020,  9, -13133, -21625, 0, 1 },   /* exe: 199 von 322 */
    { "v5020c9",  0x5020,  9, -12133, -13325, 0, 0 },   /* exe: 0 von 5007 */
    { "h6010c12", 0x6010, 12, -18612,  -7920, 0, 1 },   /* exe: 820 von 1440 */
    { "h6010c13", 0x6010, 13, -18812, -20920, 0, 1 },   /* exe: 903 von 1250 */
    { "v6010c12", 0x6010, 12, -14412,  -5720, 0, 0 },   /* exe: 0 von 9489 */
    { "v6000c3",  0x6000,  3,   4933,  -6384, 2, 0 },   /* exe: 0 von 4495 (Band 2) */
};
