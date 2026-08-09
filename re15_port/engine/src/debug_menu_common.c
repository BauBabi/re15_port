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
 * Also: Eintrag = 13 Halbwoerter, Stage = 637 Halbwoerter (= 49 Eintraege * 13). Das erste Halbwort
 * ist der Belegt-Marker (0 = LEERER SLOT: @0x800147B0 springt zurueck auf das naechste Inkrement
 * @0x800146E8 — Luecken werden selbsttaetig uebersprungen; 2 in Slot 0 = Stage gesperrt). Die
 * Raumnummer steht NICHT im Satz: der JUMP-Executor schreibt Stage/Index direkt (@0x8001D644/60),
 * Raum = (stage+1)<<8 | idx (RE15_DBG_JUMP_ROOM, hardware-verifiziert BRIEFING=0x114). */
#define DBG_TBL_ENTRY_HW   13   /* @0x8001476C-78 */
#define DBG_TBL_STAGE_HW  637   /* @0x8001477C-94 */

#define DBG_ROW_MAX    2        /* @0x80014644 sltiu 0x2 -> Zeilen 0..2 */

/* Edge-Bits, gelesen am Halbwort 0x800AC762 (@0x800148B4 / @0x80014A38). */
#define DBG_EDGE_STAGE  0x0010  /* Dreieck -> Stage + 1, @0x800148B4 */
#define DBG_EDGE_LOAD   RE15_DBG_EDGE_LOAD   /* Quadrat -> Raum laden, @0x80014A38 */
#define DBG_EDGE_EXIT   0x0040  /* Kreuz-EDGE (0x800AC762 & 0x40) -> Zustand 2 = schliessen:
                                 * lhu ac762 @0x80014664, andi 0x40 @0x8001466C, sw 2 @0x8001467C */
#define DBG_EDGE_SELECT 0x0100  /* SELECT-EDGE: oeffnet (@0x8001444C andi 0x100) bzw. schliesst,
                                 * wenn schon offen (@0x80014468 bne -> @0x80014524 sw 2) */
#define DBG_STAGE_WRAP 6        /* @0x800148E4 sltiu 0x6 / @0x800148F0 sb zero */

/* Stage-Sperr-Marker: Dreieck ueberspringt Stages, deren Slot-0-Halbwort == 2 ist
 * ("ori a0,zero,0x2" @0x800148C8, "lhu" @0x80014924, "beq v0,a0" @0x8001492C). */
#define DBG_STAGE_DISABLED 2

unsigned re15_debug_menu_table_offset(unsigned stage, unsigned idx)
{
    return (DBG_TBL_STAGE_HW * stage + DBG_TBL_ENTRY_HW * idx);   /* in Halbwoertern */
}

static re15_debug_menu_t s_dbg;

/* ==== JUMP-Tabelle zur LAUFZEIT aus DEBUG.BIN ====================================================
 * Das Original laedt DEBUG.BIN RAW nach 0x800c0000; die Tabelle liegt ab RAM 0x800c263a = Datei-
 * Offset 0x263a (Satz = 13 Halbwoerter, Stage = 637 Halbwoerter, @0x8001476C-0x800147A8). Halbwort
 * @+0: 0 = leerer Slot (@0x800147B0), 2 in Slot 0 = Stage gesperrt (@0x8001492C); Name @+8
 * (Anzeige nutzt &0x800c2642 + 0x4fa*stage + 0x1a*idx — der %s-Aufruf @0x80014B60/0x80014BB4).
 * Faellt auf die generierte Tabelle debug_jump_table.h zurueck, solange kein Abbild gesetzt ist. */
#define DBG_TBL_FILE_OFF 0x263a
#define DBG_TBL_BYTES    (6 * 0x4fa)          /* 6 Stages x 49 Saetze x 26 B */
static uint8_t s_tbl[DBG_TBL_BYTES];
static int     s_tbl_ok = 0;

void re15_debug_menu_set_bin(const uint8_t *debug_bin, unsigned size)
{
    s_tbl_ok = 0;
    if (!debug_bin || size < DBG_TBL_FILE_OFF + DBG_TBL_BYTES) return;
    memcpy(s_tbl, debug_bin + DBG_TBL_FILE_OFF, DBG_TBL_BYTES);
    s_tbl_ok = 1;
}

/* Satz-Halbwort @+0 (lhu @0x800147A8/0x80014924/0x8001496C). */
static unsigned dbg_slot_hw(unsigned st, unsigned idx)
{
    unsigned off = re15_debug_menu_table_offset(st, idx) * 2;
    return (unsigned)s_tbl[off] | ((unsigned)s_tbl[off + 1] << 8);
}

/* Belegt = Halbwort != 0 (@0x800147B0 beq v0,zero -> weiterzaehlen). */
static int dbg_used(unsigned st, unsigned idx)
{
    if (s_tbl_ok) return dbg_slot_hw(st, idx) != 0;
    return RE15_DBG_SLOT_USED(st, idx);
}

/* ==== D-Pad-Auto-Repeat (byte-true FUN_80030444-Tail + Config FUN_80030640) ======================
 * Das Menue bewegt Zeile/Raumindex NUR in Frames, in denen der Repeat-Puls feuert
 * (`if (DAT_800aca38 < 0)`, Gate @0x800145E0 bgez -> skip). Der Puls entsteht im Pad-Leser:
 *   frischer Druck (edge & mask):    feuern,  Zaehler := DELAY   (@0x800305A4/B0, sb @0x800305E0,
 *                                    Sign-Set @0x800305F4/FC)
 *   kein Edge, Zaehler != 0:         nicht feuern; gehalten -> Zaehler-1 (@0x800305C8/0x80030600/0C,
 *                                    Sign-Clear @0x80030610-28)
 *   kein Edge, Zaehler == 0:         feuern,  Zaehler := RATE    (@0x800305D4, sb @0x800305E0)
 * Kadenz mit DELAY=5/RATE=1: Schritt bei Frame 0, 6, 8, 10, ... Das Menue konfiguriert beim
 * Oeffnen FUN_80030640(0xf000, 5, 1): ori a0,0xf000 @0x80014470, ori a1,5 @0x80014474,
 * ori a2,1 @0x8001447C. */
#define DBG_REP_MASK  0xf000    /* @0x80014470 */
#define DBG_REP_DELAY 5         /* @0x80014474 */
#define DBG_REP_RATE  1         /* @0x8001447C */
static uint8_t s_rep_counter;   /* Original: DAT_80073e2c (geteilt; hier menue-lokal) */

static int dbg_repeat_fire(uint16_t held, uint16_t edge)
{
    if ((edge & DBG_REP_MASK) == 0 && s_rep_counter != 0) {
        if (held & DBG_REP_MASK) s_rep_counter--;      /* @0x80030600/0x8003060C */
        return 0;                                      /* Sign-Clear @0x80030610-28 */
    }
    s_rep_counter = (edge & DBG_REP_MASK) ? DBG_REP_DELAY : DBG_REP_RATE;  /* @0x800305E0 */
    return 1;                                          /* Sign-Set @0x800305F4/FC */
}

void re15_debug_menu_reset(void)
{
    memset(&s_dbg, 0, sizeof(s_dbg));
    s_rep_counter = 0;
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

/* Beim OEFFNEN stellt das Original den Cursor auf den AKTUELLEN Raum:
 *   DAT_800bbe5e := DAT_800b0fe0 (Stage)      — lbu @0x80014490, sb @0x800144B0
 *   (&DAT_800bbe5f)[stage] := DAT_800b0fe2    — lbu @0x800144C0, sb @0x800144E0
 * room_id = volle Port-ID (0x1140): stage = (id>>12)-1, idx = (id>>4)&0xff. */
void re15_debug_menu_sync_cursor(unsigned room_id)
{
    unsigned st  = (room_id >> 12);
    unsigned idx = (room_id >> 4) & 0xff;
    if (st < 1 || st > DBG_STAGE_WRAP) return;
    st -= 1;
    s_dbg.stage = (uint8_t)st;
    if (idx < DBG_ROOM_WRAP) s_dbg.room_idx[st] = (uint8_t)idx;
}

/* Ein Schritt in der JUMP-Liste. Vorwaerts-Wrap bei 0x31 (@0x80014734 sltiu 0x31 ->
 * @0x8001474C sb zero). RUECKWAERTS ist der Wrap byte-genau ASYMMETRISCH: bei idx==0 wird ERST
 * 0x2F gesetzt (ori a2,0x2f @0x800147D8, sb a2 @0x8001480C) und DANN dekrementiert
 * (addiu -1 @0x8001482C, sb @0x8001483C) — der erste Kandidat nach 0 ist also 0x2E; die Slots
 * 0x2F/0x30 sind linkslaufend unerreichbar (rechtslaufend erreichbar). LEERE SLOTS WERDEN
 * UEBERSPRUNGEN: @0x800147B0 `beq v0,zero` springt zurueck auf das Inkrement.
 * Der guard ist Port-Robustheit (Original terminiert nur, weil die Tabelle immer belegte Slots
 * hat — eine leere Stage wuerde dort haengen); findet sich kein Slot, bleibt der Index stehen. */
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
            if (idx == 0) idx = DBG_ROOM_LAST;              /* sb 0x2f @0x8001480C ... */
            idx--;                                          /* ... DANN -1 @0x8001482C -> 0x2E */
        }
        if (dbg_used(st, idx)) {                            /* @0x800147B0 */
            s_dbg.room_idx[st] = (uint8_t)idx;
            return;
        }
    }
}

const char *re15_debug_menu_room_name(void)
{
    if (s_tbl_ok) {
        /* Anzeige-Zeiger des Originals: &0x800c2642 + 0x4fa*stage + 0x1a*idx = Satz + 8
         * (%s-Argument des Namens-Aufrufs, FUN_800279c8 @0x80014BB4). Die 14 Namensbytes im
         * MZD-DEBUG.BIN sind alle NUL-terminiert (max. strlen 14, gemessen ueber alle Slots). */
        unsigned off = re15_debug_menu_table_offset(s_dbg.stage, s_dbg.room_idx[s_dbg.stage]) * 2;
        return (const char *)(s_tbl + off + 8);
    }
    return re15_dbg_jump_name[s_dbg.stage][s_dbg.room_idx[s_dbg.stage]];
}

/* Ein Tick des Menues mit dem REMAPPTEN Pad-Wort (Bit-Bedeutung wie 0x800AC760).
 * Rueckgabe: 1 = ein Raum soll geladen werden (dann steht er in ->load_room). */
int re15_debug_menu_tick(uint16_t held, uint16_t edge)
{
    if (!s_dbg.open) return 0;
    s_dbg.want_load = 0;

    if (edge & DBG_EDGE_SELECT) {            /* SELECT bei offenem Menue -> Zustand 2 = zu
                                              * (@0x80014468 bne -> @0x80014524 sw 2) */
        s_dbg.open = 0;
        return 0;
    }

    /* Zeilenwahl NUR im Repeat-Puls-Frame (`if (DAT_800aca38 < 0)`, bgez-Skip @0x800145E0). */
    int fire = dbg_repeat_fire(held, edge);
    if (fire) {
        if ((held & DBG_PAD_UP) && s_dbg.row > 0)          /* @0x800145F4-18, Untergrenze 0 */
            s_dbg.row--;
        if ((held & DBG_PAD_DOWN) && s_dbg.row < DBG_ROW_MAX)  /* @0x80014628-44, sltiu 0x2 */
            s_dbg.row++;
    }

    if (edge & DBG_EDGE_EXIT) {              /* Kreuz-EDGE: andi ac762,0x40 @0x8001466C -> zu */
        s_dbg.open = 0;
        return 0;
    }

    if (s_dbg.row != 1)                      /* nur die JUMP-Zeile, @0x80014698 */
        return 0;

    if (fire) {                              /* Links/Rechts ebenfalls unterm Repeat-Gate
                                              * (bgez @0x800146C8-Bereich, gleiche Puls-Quelle) */
        if (held & DBG_PAD_RIGHT)            /* @0x800146D8: Raumindex + 1 */
            dbg_step_room(+1);
        if (held & DBG_PAD_LEFT)             /* @0x800147C4: Raumindex - 1 */
            dbg_step_room(-1);
    }

    if (edge & DBG_EDGE_STAGE) {             /* @0x800148B4: Dreieck = Stage + 1 (EDGE) */
        uint8_t st = s_dbg.stage;
        int guard;                           /* Schleifen-Guard = Port-Robustheit: das Original
                                              * laeuft ohne (DEBUG.BIN ist dort IMMER geladen und
                                              * im MZD-Bestand ist keine Stage gesperrt) — eine
                                              * durchgehend gesperrte Tabelle wuerde dort haengen. */
        for (guard = 0; guard < DBG_STAGE_WRAP; guard++) {
            st = (uint8_t)(st + 1);          /* @0x800148CC-D8 */
            if (st >= DBG_STAGE_WRAP) st = 0;               /* @0x800148E4/F0 */
            if (!(s_tbl_ok && dbg_slot_hw(st, 0) == DBG_STAGE_DISABLED))
                break;                       /* Marker 2: @0x800148C8 ori 2 / @0x8001492C beq */
        }
        s_dbg.stage = st;
        /* @0x80014974: ist der gemerkte Index dieser Stage unbelegt, laeuft er ab dem AKTUELLEN
         * Stand vorwaerts bis zum naechsten belegten Slot (Schleife @0x80014980-0x80014A24,
         * Wrap sltiu 0x31 @0x800149AC). */
        if (!dbg_used(st, s_dbg.room_idx[st]))
            dbg_step_room(+1);
    }

    if (edge & DBG_EDGE_LOAD) {              /* @0x80014A38: Quadrat = Raum laden */
        s_dbg.load_room = RE15_DBG_JUMP_ROOM(s_dbg.stage, s_dbg.room_idx[s_dbg.stage]);
        s_dbg.want_load = 1;                 /* Original: 0x800B5359 = 1, @0x80014A48 */
        s_dbg.open = 0;
    }
    return s_dbg.want_load;
}

/* Cursor fuer einen automatisierten Messlauf auf einen Zielraum stellen — MEHR NICHT.
 * Ausgeloest wird der Sprung danach vom normalen re15_debug_menu_tick() mit der Lade-Flanke,
 * also ueber exakt dieselbe Zeile @0x80014A38 wie ein Quadrat-Druck des Nutzers. Genau darum
 * geht es: es darf nur EINEN Sprung-Codepfad geben. Der frueher parallel existierende
 * RE15_START_ROOM-Schnellweg bootete an re15_room_apply_pending vorbei und zeigte deshalb
 * regelmaessig etwas anderes als das Spiel.
 * room_id ist die volle ID (0x1140); die Menue-Tabelle fuehrt sie als 0x114.
 * Rueckgabe 1 = Zielraum steht im Menue, 0 = kein Slot fuer diese ID. */
int re15_debug_menu_point_at(unsigned room_id)
{
    uint16_t want = (uint16_t)(room_id >> 4);
    for (int st = 0; st < DBG_STAGE_WRAP; st++) {
        for (int idx = 0; idx < DBG_ROOM_WRAP; idx++) {
            if (!dbg_used((unsigned)st, (unsigned)idx)) continue;
            if (RE15_DBG_JUMP_ROOM(st, idx) != want) continue;
            s_dbg.stage        = (uint8_t)st;
            s_dbg.room_idx[st] = (uint8_t)idx;
            s_dbg.row          = 1;          /* die JUMP-Zeile */
            s_dbg.open         = 1;
            return 1;
        }
    }
    return 0;
}
