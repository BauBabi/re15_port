/*
 * RE1.5 Rebuilt — Game state (Phase 4.4.4, 2026-05-19).
 *
 * Hosts the SCD-driven world state: player position/heading/animation
 * and 512 game flags. Written by opcode handlers (Pos_set, Dir_set,
 * Plc_motion, Set), read by the renderer + conditional opcodes (Ck).
 *
 * Target-agnostic — same on PSX and PC since g_game is just data.
 */

#include <string.h>
#include <stdio.h>     /* Flag-Trace (RE15_FLAG_TRACE) */
#include <stdlib.h>
#include "re15_scd.h"

re15_game_state_t g_game;

/*=========================================================================
 * DAT_800aca40 / DAT_800b853c — die globalen Pause-Flags.  Semantik + alle
 * Adress-Belege stehen bei den RE15_PAUSE_*-Defines in re15_scd.h.
 *=========================================================================*/
uint32_t g_re15_pauseflags       = 0;   /* DAT_800aca40 */
uint32_t g_re15_pauseflags_saved = 0;   /* DAT_800b853c */

/* DAT_800b8520 & 0x80 — "eine Message ist offen".  Der Port hat sein eigenes
 * Message-Lifetime-Modell (g_scd.message_*), aber der Open-GUARD @0x80027e74
 * braucht genau dieses eine Bit: der Port fuehrt den Message_on-Opcode bzw. den
 * Examine-AOT waehrend eines offenen Textes erneut aus, und ohne Guard wuerde
 * @0x80027ec8 den Snapshot mit dem BEREITS EINGEFRORENEN Wert ueberschreiben —
 * der Restore koennte den Freeze dann nie mehr aufheben. */
static int s_pause_open = 0;

void re15_pauseflags_open(uint32_t mask)
{
    /* Open-Guard: FUN_80027e68 @0x80027e74 `lbu v0,DAT_800b8520` / @0x80027e7c
     * `andi v0,v0,0x80` / @0x80027e80 `beq` -> sonst `j 0x800280ac` mit v0 = -1.
     * Ein zweiter Open waehrend eines offenen Textes speichert NICHTS und
     * ver-ODERt NICHTS. */
    if (s_pause_open) return;
    s_pause_open = 1;
    g_re15_pauseflags_saved = g_re15_pauseflags;   /* @0x80027eb4 lw + @0x80027ec8 sw */
    g_re15_pauseflags      |= mask;                /* @0x80027ecc or + @0x80027ed0 sw */
}

void re15_pauseflags_close(void)
{
    /* FUN_80028134: `lw a0,DAT_800b853c` -> `sw a0,0x800aca40` an drei Stellen —
     * @0x80028594/@0x800285a4 (Select-Confirm), @0x800286bc/@0x800286cc (End-Wait),
     * @0x80028708/@0x8002871c (Hold-N-Timeout).  Alle drei loeschen zusaetzlich
     * das Open-Bit (`andi v0,v0,0x7f` @0x80028598/@0x800286c0/@0x8002870c). */
    if (!s_pause_open) return;
    s_pause_open      = 0;
    g_re15_pauseflags = g_re15_pauseflags_saved;
}

void re15_pauseflags_clear(void)
{
    /* Raumwechsel-FSM: @0x8001ca44 und @0x8001caec `sw zero,-13760(at)` = 0x800aca40. */
    g_re15_pauseflags       = 0;
    g_re15_pauseflags_saved = 0;
    s_pause_open            = 0;
}

void re15_game_state_init(void)
{
    /* Phase 4.5.9-D: g_game only holds flags[] now — player lives in
     * g_actors[0]. */
    memset(&g_game, 0, sizeof(g_game));
    re15_pauseflags_clear();
}

/* Byte-faithful flag store: per-zone u32 array, MSB-first WITHIN the 32-bit word.
 *   word = idx >> 5,  mask = 0x80000000 >> (idx & 0x1f)
 * Matches the original FUN_8004ef90 (set) / FUN_8004efe4 (check) exactly
 * (= RE2 FUN_80077360 / FUN_8007730c). idx spans 0..255 per zone. */
int re15_game_flag_get(uint8_t zone, uint8_t idx)
{
    if (zone >= RE15_FLAG_ZONES) return 0;
    uint32_t word = (uint32_t)(idx >> 5);             /* 0..7 */
    uint32_t mask = 0x80000000u >> (idx & 0x1f);      /* MSB-first in the word */
    return (g_game.flags[zone][word] & mask) ? 1 : 0;
}

/* ⛔ ZONE 2 IST DAS PAUSE-WORT DAT_800aca40 — kein separater Flag-Speicher.
 *
 * Der `Set`-Opcode LAB_8003fdd0 (SCD-Dispatch @0x80074530) loest die Zone NICHT in ein
 * eigenes Flag-Array auf, sondern ueber eine ZEIGER-TABELLE in ein Engine-Global:
 *     @0x8003fdd8 lbu v1,0x1(v0)          v1 = zone
 *     @0x8003fddc lbu a1,0x2(v0)          a1 = bit
 *     @0x8003fde0 lbu a2,0x3(v0)          a2 = op
 *     @0x8003fdec sll v1,v1,0x2
 *     @0x8003fdf8 lui  at,0x8007
 *     @0x8003fdfc addiu at,at,0x4664
 *     @0x8003fe00 addu at,at,v1
 *     @0x8003fe04 lw   v1,0x0(at)  =>  PTR_DAT_80074664[zone]
 *     @0x8003fdf0 sra  v0,a1,0x5 / @0x8003fdf4 sll v0,v0,0x2   = Wort-Offset
 *     @0x8003fe0c addu a3,v0,v1                                 = &bank[wort]
 *     @0x8003fdd4 lui  t0,0x8000 + @0x8003fe4c/68/7c srlv v1,t0,a1 = 0x80000000 >> (bit&0x1f)
 *     op 1 -> or  @0x8003fe6c | op 0 -> nor+and @0x8003fe54-58 | op 7 -> xor @0x8003fe80
 * (`Ck` LAB_8003fcf4 und der indizierte Schreiber LAB_8003fe90 @0x8003fec4-d0 benutzen
 *  dieselbe Tabelle.)
 *
 * Die Tabelle @0x80074664 (ghidra1_V2.txt):
 *     [0] DAT_800aca38   [1] DAT_800aca3c   [2] DAT_800aca40   [3] DAT_800b0ff8
 *     [4] DAT_800b1018   [5] DAT_800b1028   [6] DAT_800b1030   [7] DAT_800b1038
 *     [8] DAT_800b1058   [9] DAT_800b1078   [10] DAT_800b1098
 * DAT_800aca40 == g_re15_pauseflags.  Damit ist
 *     Set(2,0,*) = Bit 0x80000000 = RE15_PAUSE_PLAYER (FUN_80031c44 @0x80031c78 `bltz`)
 *     Set(2,2,*) = Bit 0x20000000 = RE15_PAUSE_AI     (z.B. FUN_80100424 @0x8010043c)
 *     Set(2,7,*) = Bit 0x01000000 = RE15_PAUSE_PAD    (FUN_80030444 @0x800304f8)
 *
 * Der Port hatte Zone 2 in `g_game.flags[2][]` abgelegt — einen Speicher, den NICHTS liest.
 * Gemessen (probe_11f0_puzzle_pos): ROOM11F0 sub01 @0x12A4 haelt waehrend des Generator-
 * Raetsels jedes Bild `Set(2,0,1)` @0x12AC + `Set(2,2,1)` @0x12B0; im Port blieb
 * g_re15_pauseflags dabei 0x00000000, der Spieler lief also mit dem Cursor-D-Pad MIT:
 * 60 Bilder UP verschoben ihn um (+482,-885), 60 Bilder DOWN um (0,+1680).  Nach dem
 * Raetsel stand er 1006 Einheiten neben dem Panel — genau der Nutzer-Befund.
 *
 * Zensus ueber alle 206 ausgelieferten RDTs (mainScd+subScd, opcode-exakt):
 *   Zone 0: 2 Set / 0 Ck    Zone 1: 359 Set / 0 Ck    Zone 2: 514 Set / 0 Ck
 *   Zone 2 im Detail: Bit 0 = 78x, Bit 2 = 90x, Bit 7 = 346x; Operatoren nur 1 (set)
 *   und 0 (clear), KEIN Toggle (op 7).  KEIN Skript liest Zone 0/1/2 jemals (0 Ck,
 *   0 indizierte 0x58-Lesungen) — die drei Zonen sind reine Engine-Global-Schreiber.
 * Der Schatten-Eintrag in g_game.flags[2][] bleibt deshalb erhalten: er ist fuer das
 * SPIEL unbeobachtbar (kein Ck) und dient nur den Port-Hilfskonstruktionen, die
 * "das Skript hat flag(2,7) gesetzt" abfragen (platform/pc/main.c cine_active) — die
 * duerfen nicht auf das echte Wort umschalten, weil dort auch Message_on/Item-Modal/
 * Raumwechsel Bit 0x01000000 setzen (Masken 0xffff0000 / 0xff000000 / 0xff000000).
 *
 * OFFEN (eigener Befund, NICHT Teil dieses Fixes, weil kein gemessener Defekt dranhaengt):
 * Zone 1 == DAT_800aca3c und Zone 0 == DAT_800aca38 sind im Port ebenso ungebunden.
 * Zone-1-Bits im Auslieferungsstand: 27 (Maske 0x10, 247x — der Letterbox-/Cinematic-
 * Marker, den der Port ueber den Schatten liest), 28 (0x08, 82x), 0 (0x80000000, 12x),
 * 29 (0x04, 12x), 31 (0x01, 4x), 30 (0x02, 2x).  Zone 0: Bit 1 (0x40000000, 2x, nur
 * ROOM6040/6041).  Der Port hat fuer 0x800aca38/0x800aca3c kein Wort-Modell. */
void re15_game_flag_set(uint8_t zone, uint8_t idx, int value)
{
    if (zone >= RE15_FLAG_ZONES) return;
    uint32_t word = (uint32_t)(idx >> 5);
    uint32_t mask = 0x80000000u >> (idx & 0x1f);
    if (value) g_game.flags[zone][word] |= mask;
    else       g_game.flags[zone][word] &= ~mask;

    /* Der ECHTE Schreibvorgang des Originals (siehe Block oben): Zone 2, Wort 0. */
    if (zone == 2 && word == 0) {
        if (value) g_re15_pauseflags |= mask;    /* @0x8003fe6c `or`      */
        else       g_re15_pauseflags &= ~mask;   /* @0x8003fe54-58 nor+and */
    }
    /* RE15_FLAG_TRACE=1: die beiden Intro-Ende-Flags mitschreiben. Solange flag(1,27) oder
     * flag(2,7) steht, zieht main.c:2890 player_mode jeden Frame auf 2 und der Spieler bleibt
     * skript-gefuehrt — im nicht-interaktiven Lauf endet das Helipad-Intro genau deshalb nie. */
    if ((zone == 1 && idx == 27) || (zone == 2 && idx == 7)) {
        extern int re15_flag_trace_enabled(void);
        if (re15_flag_trace_enabled())
            fprintf(stderr, "[flag] z%u/%u = %d\n", zone, idx, value ? 1 : 0);
    }
}

int re15_flag_trace_enabled(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RE15_FLAG_TRACE") ? 1 : 0;
    return on;
}
