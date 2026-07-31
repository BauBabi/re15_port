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
#include "debug_jump_table.h"
#include <string.h>

/* Pad-Bits, wie das Original sie am Wort 0x800AC760 prueft. */
#define DBG_PAD_UP     0x1000   /* @0x800145F4 */
#define DBG_PAD_DOWN   0x4000   /* @0x80014628 */
#define DBG_PAD_RIGHT  0x2000   /* @0x800146D8 */
#define DBG_PAD_LEFT   0x8000   /* @0x800147C4 */
#define DBG_PAD_EXIT   0x0040   /* @0x8001466C */

#define DBG_ROOM_WRAP  0x31     /* 49 — @0x80014734 sltiu 0x31 */
#define DBG_ROOM_LAST  0x2F     /* 47 — Rueckwaerts-Wrap, `ori a2,zero,0x2f` @0x800147D8 */

/* Die JUMP-Liste ist eine TABELLE, kein Zaehler — das war die Ursache dafuer, dass mein frueherer
 * Detektor "Wert faellt um exakt N" nie anschlagen konnte. Byte-true aus @0x8001476C-0x800147A8:
 *     v1 = 3*idx; v1 <<= 2; v1 += idx            -> 13*idx
 *     v0 = 4*stage + stage = 5*stage; v0 <<= 5   -> 160*stage
 *     v0 -= stage -> 159*stage; v0 <<= 2 -> 636*stage; v0 += stage -> 637*stage
 *     v0 = (637*stage + 13*idx) << 1             -> Halbwort-Offset
 *     room = lhu[0x800C263A + v0]
 * Also: Eintrag = 13 Halbwoerter, Stage = 637 Halbwoerter (= 49 Eintraege * 13), erstes Halbwort ist
 * die Raum-ID. Eine ID von 0 heisst LEERER SLOT: @0x800147B0 springt dann zurueck auf das naechste
 * Inkrement (@0x800146E8), das Menue ueberspringt Luecken also selbsttaetig. */
#define DBG_TBL_ENTRY_HW   13   /* @0x8001476C-78 */
#define DBG_TBL_STAGE_HW  637   /* @0x8001477C-94 */

#define DBG_ROW_MAX    2        /* @0x80014644 sltiu 0x2 -> Zeilen 0..2 */

/* Edge-Bits, gelesen am Halbwort 0x800AC762 (@0x800148B4 / @0x80014A38). */
#define DBG_EDGE_STAGE 0x0010   /* Dreieck -> Stage + 1, @0x800148B4 */
#define DBG_EDGE_LOAD  0x0080   /* Quadrat -> Raum laden, @0x80014A38 */
#define DBG_STAGE_WRAP 6        /* @0x800148E4 sltiu 0x6 / @0x800148F0 sb zero */

unsigned re15_debug_menu_table_offset(unsigned stage, unsigned idx)
{
    return (DBG_TBL_STAGE_HW * stage + DBG_TBL_ENTRY_HW * idx);   /* in Halbwoertern */
}

static re15_debug_menu_t s_dbg;

void re15_debug_menu_reset(void)
{
    memset(&s_dbg, 0, sizeof(s_dbg));
}

const re15_debug_menu_t *re15_debug_menu_state(void) { return &s_dbg; }

/* Pad-Wort des Ports -> Menue-Wort. Im Original liegen im Wort 0x800AC760 das D-Pad auf den Bits
 * 12-15 (die Maske `andi 0xf000` @0x80030514 fasst genau diese vier zusammen; das Menue nutzt
 * 0x1000/0x2000/0x4000/0x8000 als hoch/rechts/runter/links) und die Face-Tasten auf 4-7
 * (0x10 Dreieck @0x800148B4, 0x80 Quadrat @0x80014A38, 0x40 Abbruch @0x8001466C). Das Pad-Wort des
 * Ports hat beide Nibbles genau andersherum (re15_player.h: UP 0x10 ... SQUARE 0x8000) — es ist
 * dieselbe Belegung mit vertauschten Bytes, also genuegt ein Byte-Swap. */
uint16_t re15_debug_menu_pad(uint16_t phys)
{
    return (uint16_t)(((phys & 0x00FFu) << 8) | ((phys >> 8) & 0x00FFu));
}

int re15_debug_menu_open(void) { return s_dbg.open; }

void re15_debug_menu_toggle(void)
{
    s_dbg.open = !s_dbg.open;
    s_dbg.want_load = 0;
}

/* Ein Schritt in der JUMP-Liste. Der Wrap liegt bei 0x31 (@0x80014734 sltiu 0x31 -> @0x8001474C
 * sb zero) bzw. bei 0x2F rueckwaerts (@0x800147D8 ori a2,zero,0x2f). LEERE SLOTS WERDEN
 * UEBERSPRUNGEN: @0x800147B0 `beq v0,zero,0x800146E8` springt zurueck auf das Inkrement, wenn das
 * erste Halbwort des Satzes (der Namensanfang) 0 ist. Ohne das steht das Menue auf Luecken still.
 * Die Schleifengrenze ist der volle Ring — findet sich kein belegter Slot, bleibt der Index stehen. */
static void dbg_step_room(int dir)
{
    uint8_t st = s_dbg.stage;
    int idx = s_dbg.room_idx[st];
    int guard;
    for (guard = 0; guard < DBG_ROOM_WRAP; guard++) {
        if (dir > 0) {
            idx++;
            if (idx >= DBG_ROOM_WRAP) idx = 0;              /* @0x80014734/4C */
        } else {
            idx--;
            if (idx < 0) idx = DBG_ROOM_LAST;               /* @0x800147D8/F8 */
        }
        if (RE15_DBG_SLOT_USED(st, idx)) {                  /* @0x800147B0 */
            s_dbg.room_idx[st] = (uint8_t)idx;
            return;
        }
    }
}

const char *re15_debug_menu_room_name(void)
{
    return re15_dbg_jump_name[s_dbg.stage][s_dbg.room_idx[s_dbg.stage]];
}

/* Ein Tick des Menues mit dem REMAPPTEN Pad-Wort (Bit-Bedeutung wie 0x800AC760).
 * Rueckgabe: 1 = ein Raum soll geladen werden (dann steht er in ->load_room). */
int re15_debug_menu_tick(uint16_t held, uint16_t edge)
{
    if (!s_dbg.open) return 0;
    s_dbg.want_load = 0;

    if (held & DBG_PAD_EXIT) {               /* @0x8001466C: verlassen */
        s_dbg.open = 0;
        return 0;
    }
    if ((held & DBG_PAD_UP) && s_dbg.row > 0)          /* @0x800145F4-18, Untergrenze 0 */
        s_dbg.row--;
    if ((held & DBG_PAD_DOWN) && s_dbg.row < DBG_ROW_MAX)  /* @0x80014628-44, sltiu 0x2 */
        s_dbg.row++;

    if (s_dbg.row != 1)                      /* nur die JUMP-Zeile, @0x80014698 */
        return 0;

    if (held & DBG_PAD_RIGHT)                /* @0x800146D8: Raumindex + 1 */
        dbg_step_room(+1);
    if (held & DBG_PAD_LEFT)                 /* @0x800147C4: Raumindex - 1 */
        dbg_step_room(-1);

    if (edge & DBG_EDGE_STAGE) {             /* @0x800148B4: Dreieck = Stage + 1 */
        uint8_t st = (uint8_t)(s_dbg.stage + 1);
        if (st >= DBG_STAGE_WRAP) st = 0;                   /* @0x800148E4/F0 */
        s_dbg.stage = st;
        /* @0x80014974: hat der neue Index dieser Stage keinen Eintrag, ruecke ihn auf einen
         * belegten Slot vor — dieselbe Leerslot-Regel wie beim Schrittwechsel. */
        if (!RE15_DBG_SLOT_USED(st, s_dbg.room_idx[st]))
            dbg_step_room(+1);
    }

    if (edge & DBG_EDGE_LOAD) {              /* @0x80014A38: Quadrat = Raum laden */
        s_dbg.load_room = RE15_DBG_JUMP_ROOM(s_dbg.stage, s_dbg.room_idx[s_dbg.stage]);
        s_dbg.want_load = 1;                 /* Original: 0x800B5359 = 1, @0x80014A48 */
        s_dbg.open = 0;
    }
    return s_dbg.want_load;
}
