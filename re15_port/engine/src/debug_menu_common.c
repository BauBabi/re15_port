/*
 * RE1.5 Rebuilt — das ORIGINAL-DEBUG-MENUE ("UTILITY MENU"), byte-true portiert.
 *
 * WARUM ES IM PORT GEBRAUCHT WIRD (Nutzer-Vorgabe): fuer den 1:1-Vergleich muessen Original und Port
 * einen Raum ueber DENSELBEN Weg betreten. Solange das Original per Debug-JUMP hineinspringt und der
 * Port per RE15_START_ROOM gesetzt wird, vergleicht der Harness zwei verschiedene Situationen — die
 * Positions- und Yaw-Differenzen, an denen ich mich festgebissen habe, kamen genau daher. Mit dem
 * Menue im Port entfaellt die Frage "was macht der JUMP mit der Spielerposition" vollstaendig: beide
 * Seiten fahren denselben Code.
 *
 * QUELLE: PSX.EXE, Menue-Funktion @0x80014444. Alle Konstanten unten tragen ihre Adresse.
 *
 *   Auswahlzeile   0x800BBE5D, Werte 0..2
 *     @0x800145F4  Pad-Bit 0x1000 -> Auswahl - 1, Untergrenze 0 (@0x8001460C beq v0,zero)
 *     @0x80014628  Pad-Bit 0x4000 -> Auswahl + 1, Obergrenze via `sltiu v0,v0,0x2` @0x80014644
 *     @0x8001461C  gelesen wird das Pad-Wort 0x800AC760 (das REMAPPTE, geschrieben @0x80030564
 *                  aus 0x800AC758; Nachbarn 0x800AC768 = held, 0x800AC76C = edge)
 *
 *   Zeile 1 = JUMP (@0x80014698 `beq v1,1`), Zeile 2 = MEMORY VIEWER (@0x800146A8 `beq v1,2`)
 *     @0x8001466C  Pad-Bit 0x40 -> Menue verlassen (setzt 0x8008F618 = 2)
 *     @0x800146D8  Pad-Bit 0x2000 -> Raumindex + 1
 *     Stage-Index      0x800BBE5E      (@0x800146E4 `addiu a1,a1,1`)
 *     Raumindex je Stage 0x800BBE5F + stage   (@0x800146F4/0x8001470C)
 *     Wrap: `sltiu v0,v0,0x31` @0x80014734 -> >= 0x31 (49) faellt auf 0 zurueck (@0x8001474C)
 *
 * Der Port haelt dieselben drei Zustandsbytes in derselben Bedeutung, damit ein RAM-Vergleich gegen
 * das Original moeglich bleibt. Gezeichnet wird NICHT hier (Text laeuft im Original ueber
 * FUN_800279C8 @0x80014B10/BCC/BE0) — die Darstellung haengt am Renderer und folgt getrennt.
 */
#include "re15_debug_menu.h"
#include <string.h>

/* Pad-Bits, wie das Original sie am Wort 0x800AC760 prueft. */
#define DBG_PAD_UP     0x1000   /* @0x800145F4 */
#define DBG_PAD_DOWN   0x4000   /* @0x80014628 */
#define DBG_PAD_RIGHT  0x2000   /* @0x800146D8 */
#define DBG_PAD_EXIT   0x0040   /* @0x8001466C */

#define DBG_ROOM_WRAP  0x31     /* 49 — @0x80014734 sltiu 0x31 */
#define DBG_ROW_MAX    2        /* @0x80014644 sltiu 0x2 -> Zeilen 0..2 */

static re15_debug_menu_t s_dbg;

void re15_debug_menu_reset(void)
{
    memset(&s_dbg, 0, sizeof(s_dbg));
}

const re15_debug_menu_t *re15_debug_menu_state(void) { return &s_dbg; }

int re15_debug_menu_open(void) { return s_dbg.open; }

void re15_debug_menu_toggle(void)
{
    s_dbg.open = !s_dbg.open;
    s_dbg.want_load = 0;
}

/* Ein Tick des Menues mit dem REMAPPTEN Pad-Wort (Bit-Bedeutung wie 0x800AC760).
 * Rueckgabe: 1 = ein Raum soll geladen werden (dann steht er in ->load_room). */
int re15_debug_menu_tick(uint16_t vpad_edge)
{
    if (!s_dbg.open) return 0;
    s_dbg.want_load = 0;

    if (vpad_edge & DBG_PAD_EXIT) {          /* @0x8001466C: verlassen */
        s_dbg.open = 0;
        return 0;
    }
    if ((vpad_edge & DBG_PAD_UP) && s_dbg.row > 0)          /* @0x800145F4-18, Untergrenze 0 */
        s_dbg.row--;
    if ((vpad_edge & DBG_PAD_DOWN) && s_dbg.row < DBG_ROW_MAX)  /* @0x80014628-44, sltiu 0x2 */
        s_dbg.row++;

    if (s_dbg.row == 1) {                    /* JUMP-Zeile, @0x80014698 */
        if (vpad_edge & DBG_PAD_RIGHT) {     /* @0x800146D8: Raumindex + 1 */
            uint8_t st = s_dbg.stage;
            uint8_t idx = (uint8_t)(s_dbg.room_idx[st] + 1);
            if (idx >= DBG_ROOM_WRAP) idx = 0;              /* @0x80014734/4C */
            s_dbg.room_idx[st] = idx;
        }
    }
    return s_dbg.want_load;
}
