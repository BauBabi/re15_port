/* Erzeugt von tests/unit/test_r37_masken36.c (Modus "erzeuge") - NICHT von Hand aendern.
 * Runde 37 Spur P: Generator-Rechtecke (R15M ohne eigenen Atlas), die an einem erreichbaren Stehplatz einen
 * Figurpunkt ohne solide Zelle davor verdecken. Grundlagen und Messaufbau: Kopf von test_r37_masken36.c,
 * Dossier analysis/befunde_runde37/P_masken36.md. Spalten: Raum der MSK-Datei, Cut, Zahl, Bitfeld je Rechteck. */
static const re15_masken36_zeile_t re15_masken36_zeilen[] = {
    { 0x3050, 15,  70, { 0xFFFFFFFFu, 0xFFFFFFFFu, 0x0000003Fu, 0x00000000u } },   /* 70 von 70 Masken, Stehplaetze 0 */
    { 0x3051, 15,  70, { 0xFFFFFFFFu, 0xFFFFFFFFu, 0x0000003Fu, 0x00000000u } },   /* 70 von 70 Masken, Stehplaetze 0 */
    { 0x0000, 0xFF, 0, { 0u, 0u, 0u, 0u } }   /* Ende */
};
