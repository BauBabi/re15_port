import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elza_ed import rep

# Der [PORT-S]-Vermerk auf 1071 war eine Port-ANNAHME. Er ist es nicht mehr:
# die Variantenregel ist belegt (@0x800397e4 srl 31 / @0x800397ec addu, Stage-0-
# Tabelle @0x8007429c in Schritten von 3). 1071 IST 1070 mit Spieler-Digit 1.
rep("re15_port/engine/src/re15_savepoint.c", [
(
""" *   [PORT-S] = PORT-Entscheidung, semantisch gestuetzt: 1071 = Elza-Spiegel des Telefons;
 *              1120/1121 = der Vorraum des Treppenhauses (sysmes 0x1c "West Staircase 1F");""",
""" *   [PORT-S] = PORT-Entscheidung, semantisch gestuetzt:
 *              1120/1121 = der Vorraum des Treppenhauses (sysmes 0x1c "West Staircase 1F");"""
),
(
"""    { 0x1070, 0x14, 1 }, { 0x1071, 0x14, 1 },   /* STAGE1 main20 — Telefon [PATCH; 1071 PORT-S] */""",
"""    /* ⛔ 1071 ist KEINE Port-Annahme mehr (stand hier bis Runde 35 als [PORT-S]).
     * Die ungerade Id IST derselbe Raum in Elzas Dateivariante: der Raumlader
     * addiert das Elza-Bit auf den CD-Dateiindex (`srl a0,a0,31` @0x800397e4,
     * `addu a0,a0,v0` @0x800397ec), und die Stage-0-Tabelle @0x8007429c laeuft in
     * Schritten von 3 — Elza-RDT = Leon-RDT + 1. Derselbe Ort, derselbe
     * Ortsnamen-Index. */
    { 0x1070, 0x14, 1 }, { 0x1071, 0x14, 1 },   /* STAGE1 main20 — Telefon [PATCH] */"""
),
])

rep("re15_port/include/re15_gameflow.h", [
(
"""    RE15_MODE_CHARSELECT,   /* Leon / Elza pick (PL00 / PL01-04)                                 */""",
"""    RE15_MODE_CHARSELECT,   /* Leon / Elza pick (PL00 / PL04 — DAT_800ACA5C 0 bzw. 4).
                             * ⛔ TOTE DEKLARATION: die Auswahl laeuft als Unterschleife IN
                             * RE15_MODE_TITLE (pc_run_player_select), dieser Modus wird nirgends
                             * gesetzt oder abgefragt. Byte-true ist das nicht falsch — das
                             * Original hat dafuer ebenfalls keinen eigenen Lebenszyklus-Zustand,
                             * sondern eine Task-Ersetzung (@0x80102c9c).                        */"""
),
])
