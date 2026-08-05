/* ============================================================================
 *  Flag store -> door/event gate — roadmap S1-4 / PROG-2.
 *
 *  PROG-2 wires the byte-true flag store to the door/event gates. The store +
 *  every op was verified byte-true against the original (this test PINS them):
 *    - re15_game_flag_get / _set  == FUN_8004efe4 (check) / FUN_8004ef90 (set):
 *        word = idx>>5, mask = 0x80000000 >> (idx&0x1f)  (MSB-first in the word).
 *    - op_ck   (0x21, LAB_8003fcf4): the If-predicate — cond = (flag!=0) ^ (expected==0);
 *        returns CONTINUE (run body) / IF_FALSE (dispatcher pops block-stack -> block_end).
 *    - op_set  (0x22, LAB_8003fdd0): pc[3] = 1 OR / 0 CLEAR / 7 TOGGLE, else no-op.
 *    - op_flag_set2 (0x59, LAB_8003fe90): the INDEXED write — idx = work_vars[pc[2]]
 *        (DAT_800b0fd0, lhu @0x8003feb8), bank = pc[1], MSB-first bit; 1/0/7 as op_set.
 *
 *  It drives the STAGE1 door lock exactly: a room SCD does
 *      If(Ck(flag)) { Door_aot_set (open) }  Else { Aot_set MESSAGE (locked) }
 *  so toggling the flag flips which AOT installs (the sce1<->sce2 swap of PROG-1).
 *
 *  Two checks, both through the REAL VM (scd_thread_start + scd_vm_tick), no
 *  engine-internal pokes on the write side:
 *   (1) round-trip: the indexed 0x59 write (via the work_vars/DAT_800b0fd0 scratch)
 *       and the direct 0x22 write land in the store where 0x21/get read them back —
 *       SET / CLEAR / TOGGLE each byte-true;
 *   (2) end-to-end gate: `If Ck(flag) { Set witness } EndIf` run with the gate flag
 *       clear vs set — the witness (the guarded op = the "door/event") fires IFF the
 *       flag is set. This is the acceptance ("setting flag X opens the X-gated
 *       event on the next scan") reduced to its plumbing; the DOOR<->MESSAGE install
 *       flip itself is pinned by integration_door_lock (PROG-1).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_scd.h"

#define SLOT 0
#define OP_EVT_NEXT 0x02

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

/* Run a whole bytecode fragment: the CONTINUE ops chain within one tick until the
 * trailing 0x02 (Evt_next) yields. The prior run parks the thread ALIVE at that
 * sentinel, so free the slot before re-starting (scd_thread_start refuses a live slot). */
static void run_frag(const uint8_t *bc)
{
    g_scd.threads[SLOT].active = 0;
    scd_thread_start(SLOT, bc);
    scd_vm_tick();
}

int main(void)
{
    printf("=== PROG-2 flag store -> gate (0x59/0x22 write, 0x21 Ck gate) ===\n");
    scd_vm_init();
    re15_game_state_init();
    for (int i = 0; i < SCD_THREAD_COUNT; i++) g_scd.threads[i].active = 0;

    /* ---- (1) round-trip through the REAL indexed write op 0x59 (DAT_800b0fd0 scratch) ---- */
    /* 0x59 resolves idx = work_vars[pc[2]] — use a >31 index (like ROOM1021's Ck(3,133)) so the
     * per-zone word math (idx>>5=4) is exercised, not just the low word. bank = pc[1]. */
    g_scd.work_vars[3] = 133;
    { const uint8_t bc[] = { 0x59, 0x03, 0x03, 0x01, OP_EVT_NEXT }; run_frag(bc); }   /* SET   */
    CHECK("0x59 SET   flag(3,133)=1", re15_game_flag_get(3, 133) == 1);
    { const uint8_t bc[] = { 0x59, 0x03, 0x03, 0x00, OP_EVT_NEXT }; run_frag(bc); }   /* CLEAR */
    CHECK("0x59 CLEAR flag(3,133)=0", re15_game_flag_get(3, 133) == 0);
    { const uint8_t bc[] = { 0x59, 0x03, 0x03, 0x07, OP_EVT_NEXT }; run_frag(bc); }   /* TOGGLE*/
    CHECK("0x59 TOGGLE flag(3,133)->1", re15_game_flag_get(3, 133) == 1);
    { const uint8_t bc[] = { 0x59, 0x03, 0x03, 0x07, OP_EVT_NEXT }; run_frag(bc); }
    CHECK("0x59 TOGGLE flag(3,133)->0", re15_game_flag_get(3, 133) == 0);
    /* a >31 idx must NOT bleed into a neighbour bit (the old zone*32+idx packing collided) */
    CHECK("0x59 no bit-bleed (3,132/134 stay 0)",
          re15_game_flag_get(3, 132) == 0 && re15_game_flag_get(3, 134) == 0);

    /* the primary op_set (0x22) that the door-lock If(Ck) pairs with.
     * BANK 3, NICHT 5: Bank-5-Wort-1 (Bits 0x20..0x3F) ist Ein-Frame-Scratch — siehe die
     * Frame-Ende-Pruefung unten. Der Opcode-Pfad ist identisch. */
    { const uint8_t bc[] = { 0x22, 0x03, 0x2a, 0x01, OP_EVT_NEXT }; run_frag(bc); }   /* Set(3,42,OR) */
    CHECK("0x22 SET   flag(3,42)=1", re15_game_flag_get(3, 42) == 1);
    { const uint8_t bc[] = { 0x22, 0x03, 0x2a, 0x00, OP_EVT_NEXT }; run_frag(bc); }   /* clear */
    CHECK("0x22 CLEAR flag(3,42)=0", re15_game_flag_get(3, 42) == 0);

    /* ---- FRAME-ENDE-WISCH von Flag-Bank 5, Wort 1 (Bits 0x20..0x3F) ----
     * Byte-true FUN_8003ebf4 @0x8003ec1c `sw zero,0x800b102c`, gerufen vom VM-Executor
     * FUN_8003f0a0 @0x8003f18c UNBEDINGT am Ende jedes VM-Laufs (hinter der Slot-Schleife
     * @0x8003f17c-88). Diese Bits sind KEIN Raum-Latch, sondern ein Ein-Frame-Handshake:
     * der AOT-Scan laeuft nach der VM und setzt sie, sub01 liest sie im naechsten Frame.
     * Ohne den Wisch latchte ROOM1040s Schalter-Flag(5,0x21) dauerhaft und die Rolltor-Frage
     * erschien beim naechsten BETRETEN des Raums im Tuer-Frame (Nutzer-Report).
     * Bit 0x0a liegt in Wort 0 = echter Raum-Scratch und muss den Wisch UEBERLEBEN. */
    { const uint8_t bc[] = { 0x22, 0x05, 0x21, 0x01, OP_EVT_NEXT }; run_frag(bc); }
    CHECK("Bank5 Wort1: flag(5,0x21) am Frame-Ende gewischt", re15_game_flag_get(5, 0x21) == 0);
    { const uint8_t bc[] = { 0x22, 0x05, 0x0a, 0x01, OP_EVT_NEXT }; run_frag(bc); }
    CHECK("Bank5 Wort0: flag(5,0x0a) ueberlebt den Frame", re15_game_flag_get(5, 0x0a) == 1);

    /* ---- (2) end-to-end gate: If Ck(gate) { Set(witness) } EndIf ----
     * layout: If@0(4) Ck@4(4) Set@8(4) EndIf@12(2) Evt@14(1).
     * op_if block_end = pc+4+len; FALSE -> dispatcher jumps past the whole construct (14),
     * skipping the guarded Set; TRUE -> Ck CONTINUE, Set runs, EndIf, Evt. block_length=10. */
    const uint8_t GZ = 7, GI = 20, WZ = 7, WI = 21;   /* gate flag (7,20) -> witness (7,21) */
    const uint8_t gate[] = {
        0x06, 0x00, 10, 0x00,          /* If,  block_length = 10 -> block_end = past EndIf   */
        0x21, GZ,  GI, 0x01,           /* Ck(gate, expected=1): true IFF flag(7,20) set      */
        0x22, WZ,  WI, 0x01,           /* Set(witness, OR): the guarded "door/event" fire    */
        0x08, 0x00,                    /* EndIf                                              */
        OP_EVT_NEXT,                   /* yield                                              */
    };

    /* FALSE: gate flag clear -> the guarded op must NOT fire */
    re15_game_flag_set(GZ, GI, 0);
    re15_game_flag_set(WZ, WI, 0);
    run_frag(gate);
    CHECK("gate FALSE: flag clear -> witness NOT set", re15_game_flag_get(WZ, WI) == 0);

    /* TRUE: set the gate flag -> the guarded op fires on the next run */
    re15_game_flag_set(GZ, GI, 1);
    re15_game_flag_set(WZ, WI, 0);
    run_frag(gate);
    CHECK("gate TRUE:  flag set   -> witness set (X-gated event opens)", re15_game_flag_get(WZ, WI) == 1);

    if (g_fail) { printf("FLAG GATE: FAIL\n"); return 1; }
    printf("FLAG GATE: flag store round-trips (0x59 idx=work_vars/0x22) + drives the If(Ck) gate byte-true\n");
    return 0;
}
