/*
 * menu_common.c — the byte-true RE1.5 STATUS/INVENTORY screen FSM (wave 2).
 *
 * Spec: shots/inv_wave2_spec.md (RE workflow wf_e8e48d36, 4 reports, citations complete)
 * + fresh file disasm of the DEBUG.BIN grid handler 0x800c62a0-0x800c65fc (byte-identical
 * to the mzd_inv_open.sav RAM module; DEBUG.BIN maps @0x800c0000) and of the EXE command
 * stage @0x8004a458-0x8004a700 (re15_disasm.py, this session).
 *
 * This module is the original's state-byte writer:
 *   s_stage    = DAT_800b5359 (gameplay transition stage 0..5, jump table @0x8001069c)
 *   s_phase    = DAT_800b25bf (master menu phase: 0 init / 1 run / 2 close)
 *   s_substate = DAT_800b25c1 (run sub-state: 0 tabs / 1 MAP / 2 FILE / 3 ITEM)
 *   g_inv_screen.tab/item_cursor/second_cursor/highlight/item_state/action_dir/
 *   tab_base_y/act_base_y = DAT_800b25bc/bd/be/ca/c2/d6/25ea/25ee (see re15_inv_screen.h)
 *
 * Pad-word mapping (the port uses the PSn00bSDK bit layout, re15_player.h): the original
 * reads the byte-swapped raw words DAT_800ac762 (edge) / DAT_800ac760 (held) — START=0x800,
 * L1=0x4, R1=0x8, Up/Right/Down/Left=0x1000/0x2000/0x4000/0x8000 — and the config-remapped
 * VIRTUAL edge DAT_800ac76c — confirm=0x4000 (CROSS), cancel=0x8000 (SQUARE). Port bits:
 * START=0x8, L1=0x400, R1=0x800, d-pad=0x00f0, CROSS=0x4000, SQUARE=0x8000. The port has a
 * single (config-remapped) pad word, used for both the raw and the virtual reads.
 *
 * SEs: FUN_80045024(bank<<24 | id<<16) — every SE in this flow is bank 4 (CORE00 resident)
 * -> re15_audio_core_se(id). The tab-select FSM itself is SILENT (EXE-wide jal scan:
 * zero SE sites in 0x8004974c-0x80049a58 / FUN_80046540 — spec fact).
 */
#include <string.h>
#include "re15_menu.h"
#include "re15_inv_screen.h"    /* g_inv_screen — the original's 25xx screen registers */
#include "re15_inventory.h"     /* g_inv, equip slot 25c8, compaction FUN_8004dadc */
#include "re15_item_use.h"      /* heal-USE sub-flow (wave-3 bridge in state 5) */
#include "re15_player.h"        /* RE15_PAD_BIT_* */
#include "re15_damage.h"        /* re15_player_set_equipped_weapon (close-phase commit) */
#include "re15_audio.h"         /* re15_audio_core_se (SE bank 4) + prime_weapon */
#include "re15_fade.h"          /* the byte-true fade channel engine (FUN_800217b0 family) */
#include "re15_scd.h"           /* g_scd.message_fsm_active — the stage-1 message-idle gate */

#define CAPACITY 10             /* DAT_800b0fbc (lbu @0x800c63e0; live 0x0a) */

/* ---- FSM state (the non-render bytes; render-relevant ones live in g_inv_screen) ---- */
static uint8_t s_stage    = 0;     /* DAT_800b5359 */
static uint8_t s_request  = 0;     /* DAT_800aca3c bit 0x8000 (open request, sticky)  */
static uint8_t s_latch    = 0;     /* DAT_800aca3c bit 0x40   (gated latch)           */
static uint8_t s_alive    = 0;     /* menu task slot-1 alive (status!=0 @0x800b29a4)  */
static uint8_t s_phase    = 0;     /* DAT_800b25bf */
static uint8_t s_substate = 0;     /* DAT_800b25c1 */
static uint8_t s_snapshot = 0;     /* DAT_800b25ce equip-id snapshot (close commit)   */
static uint8_t s_saved_cursor = 0; /* DAT_800b25d7 (equip-cell saved cursor; live 0)  */
static uint8_t s_stage2_sub = 0;   /* stage-2 micro-step (hold-black armed?)          */
static uint8_t s_hold     = 0;     /* FUN_80029ac8(2) 2-vsync yield counter           */
static uint8_t s_p0_entered = 0;   /* phase-0 init ran (the init body runs once)      */
static uint8_t s_close_sub  = 0;   /* close-phase micro-step                          */

/* D-pad auto-repeat (FUN_80030444 bit31-of-aca38 tick; config FUN_80030640(0xf000,0xf,4)
 * @0x800460cc-d8 = raw-layout d-pad mask + delay 15 + rate 4 -> press edge moves, first
 * repeat after 16 frames, then every 5 (@0x80030570-628). Port mask = d-pad 0x00f0. */
#define REP_MASK (RE15_PAD_BIT_UP | RE15_PAD_BIT_RIGHT | RE15_PAD_BIT_DOWN | RE15_PAD_BIT_LEFT)
static uint8_t s_rep_delay = 15, s_rep_rate = 4, s_rep_ctr = 0;

int re15_menu_is_open(void)          { return s_alive; }
int re15_menu_gameplay_frozen(void)  { return s_alive || s_stage != 0 || s_latch; }
int re15_menu_stage(void)            { return s_stage; }
int re15_menu_phase(void)            { return s_phase; }
int re15_menu_substate(void)         { return s_substate; }

/* SE(4,id): FUN_80045024(0x04<<24 | id<<16) = CORE00 bank record id. */
static void se4(int id) { re15_audio_core_se(id); }

/* The close-phase equip compare operand: item id at the equip slot DAT_800b25c8
 * (@0x800465f4-661c lbu 0x800b10ac + 25c8*4). 25c8==0x80 makes both the snapshot and
 * the close read hit the same out-of-bounds byte -> always-equal; modeled as a fixed
 * sentinel so the compare is byte-equivalent. */
static uint8_t equip_id_now(void)
{
    int eq = re15_inv_equipped_slot();
    if (eq == 0x80) return 0xff;
    return g_inv.slots[eq & 0x0f].id;
}

/* ---------------------------------------------------------------------------------- */
/* OPEN poll (gameplay-step tail @0x8001cd64-cde8)                                    */
/* ---------------------------------------------------------------------------------- */
void re15_menu_start_poll(uint16_t pad_pressed, int hit_react_ok)
{
    if (s_alive || s_stage != 0) return;            /* poll runs only in normal gameplay */
    if (pad_pressed & RE15_PAD_BIT_START)           /* raw 0x800 edge @0x8001cd64-74     */
        s_request = 1;                              /* aca3c |= 0x8000 (sticky)          */
    if (s_request && hit_react_ok) {                /* gate: lbu 0x800acae7 @0x8001cd94  */
        s_latch = 1;                                /* aca3c |= 0x40 @0x8001cdc0         */
        s_stage = 1;                                /* (DAT_800b5359=1 is set by the     */
                                                    /* request path; pauseflags freeze   */
                                                    /* @0x8001cdd4 == frozen() here)     */
    }
    /* SELECT+START held 0x900 -> alternate task 0x8002dde4 (@0x8001cd28-60): identity
     * unresolved (likely the debug screen, spec Task-A open question) — not ported. */
}

/* ---------------------------------------------------------------------------------- */
/* TAB-SELECT (run sub-state 0, LAB_8004974c @0x80049800-49a18)                        */
/* ---------------------------------------------------------------------------------- */
static void tab_select(uint16_t pressed)
{
    /* head: pad-2 TRIANGLE debug refill (@0x8004975c-b4) — the port has no pad-2
     * input source; not wired (byte-true would need a second controller). */

    /* D-pad/L1/R1 edges, priority order, ABSOLUTE tab positions (no wrap/cycle);
     * each hit CONSUMES the frame (jumps to draw, skipping confirm/cancel). */
    if (pressed & RE15_PAD_BIT_L1) {            /* raw 0x4 @0x8004980c-30 */
        g_inv_screen.tab = 1;
        s_substate = 1;                         /* instant MAP launch — SKIPS the
                                                 * confirm path's common resets */
        return;
    }
    if (pressed & RE15_PAD_BIT_R1) {            /* raw 0x8 @0x80049834-4c */
        g_inv_screen.tab = 3;
        s_substate = 2;                         /* FILE starts next frame */
        return;
    }
    if (pressed & RE15_PAD_BIT_RIGHT) { g_inv_screen.tab = 3; return; }  /* @0x80049850-64 */
    if (pressed & RE15_PAD_BIT_LEFT)  { g_inv_screen.tab = 1; return; }  /* @0x8004986c-80 */
    if (pressed & RE15_PAD_BIT_DOWN)  { g_inv_screen.tab = 2; return; }  /* @0x80049888-9c */
    if (pressed & RE15_PAD_BIT_UP)    { g_inv_screen.tab = 0; return; }  /* @0x800498a4-b8 */

    /* CONFIRM = virtual CROSS 0x4000 (@0x800498c0-cc). NO sound effect on any confirm
     * path (spec fact: zero FUN_80045024 sites in this FSM). */
    if (pressed & RE15_PAD_BIT_CROSS) {
        if (g_inv_screen.tab != 4) {            /* guard @0x800498d4-dc (25bc shared with
                                                 * the standalone MAP screen's FSM) */
            /* 25ca dim flag: 1 iff tab==0, else 0 (@0x800498e4-f4 — the ONE set-to-1
             * site in the whole EXE, spec Task-C store scan) */
            g_inv_screen.highlight = (g_inv_screen.tab == 0) ? 1 : 0;
            /* common resets @0x800498f8-918: 25bd/25be/25c2/25c3 = 0 */
            g_inv_screen.item_cursor = 0;
            g_inv_screen.second_cursor = 0;
            g_inv_screen.item_state = 0;
            switch (g_inv_screen.tab) {         /* dispatch @0x8004991c-58 */
            case 0: s_substate = 3; return;     /* ITEM: 25c1=3 @0x80049960-64 */
            case 1: s_substate = 1; return;     /* MAP: 25c1=1 @0x80049968-78 (+ per-stage
                                                 * entry @0x80074c0c[stage] + CD-load task
                                                 * 0x8004c328 — WAVE-3 stub, see below) */
            case 2:                             /* EXIT @0x800499b4-c4 */
                g_inv_screen.highlight = 0;     /* sb zero,13(a0) = 25ca */
                s_phase = 2;                    /* 25bf++ -> close phase */
                return;
            case 3: s_substate = 2; return;     /* FILE: 25c1=2 @0x800499c8-cc */
            }
        }
        /* tab==4: falls through to the cancel poll (@0x800498d4 beq -> 0x800499d0) */
    }

    /* CANCEL poll: raw START edge OR virtual cancel 0x8000 -> 25bf++ (@0x800499d0-a18).
     * No SE, no 25ca write. */
    if ((pressed & RE15_PAD_BIT_START) || (pressed & RE15_PAD_BIT_SQUARE))
        s_phase = 2;
}

/* ---------------------------------------------------------------------------------- */
/* GRID NAV (ITEM state 1) — DEBUG.BIN 0x800c62a0, disassembled from the FILE this     */
/* session (byte-identical to the savestate RAM module).                               */
/* ---------------------------------------------------------------------------------- */
static uint8_t s_rep_fire = 0;     /* DAT_800aca38 bit31 (the per-frame repeat tick) */

static void repeat_update(uint16_t pressed, uint16_t held)
{
    /* FUN_80030444 @0x80030570-628 (runs in the PAD REFRESH every frame, not only in
     * grid frames — a d-pad held through the entry slide keeps its countdown running):
     * fresh edge&mask -> tick + countdown=delay; held: countdown==0 -> tick +
     * countdown=rate, else countdown--. Config (mask,15,4) @0x800460cc — press moves,
     * first repeat after 16 frames, then every 5. */
    if (pressed & REP_MASK) { s_rep_ctr = s_rep_delay; s_rep_fire = 1; return; }
    if (held & REP_MASK) {
        if (s_rep_ctr == 0) { s_rep_ctr = s_rep_rate; s_rep_fire = 1; return; }
        s_rep_ctr--;
        s_rep_fire = 0;
        return;
    }
    s_rep_fire = 0;
}

static void grid(uint16_t pressed, uint16_t held)
{
    uint8_t *cur = &g_inv_screen.item_cursor;

    /* moves: HELD raw word, gated by the auto-repeat tick (bgez aca38 @0x800c62b4).
     * Direction blocks are EXCLUSIVE (each path jumps to the confirm section); priority
     * Right > Left > Down > Up. The SE(4,4) plays BEFORE the accept checks (jal
     * @0x800c62d8/632c/63ac/6420 precede the cursor tests) — i.e. also on a rejected
     * move. NO wrap, occupancy NOT checked. */
    if (s_rep_fire) {
        if (held & RE15_PAD_BIT_RIGHT) {                 /* @0x800c62c8-6318 */
            se4(4);
            if (*cur == 0x0a)            *cur = s_saved_cursor;  /* @0x800c62f8-6304 */
            else if ((*cur & 1) == 0)    (*cur)++;               /* even -> +1 */
        } else if (held & RE15_PAD_BIT_LEFT) {           /* @0x800c631c-6398 */
            se4(4);
            if (*cur == 0x0a)            *cur = s_saved_cursor;  /* @0x800c634c-6358 */
            else if (*cur & 1)           (*cur)--;               /* odd -> -1 */
            /* even (left column): the equip-cell 0xA jump @0x800c6384-98 is PATCHED
             * DEAD — @0x800c637c = 0x1000003b beq zero,zero,0x800c646c uncondition-
             * ally skips it (file == savestate RAM). Byte-true: nothing happens. */
        } else if (held & RE15_PAD_BIT_DOWN) {           /* @0x800c639c-640c */
            se4(4);
            if (*cur == 0x0a)            *cur = s_saved_cursor;  /* @0x800c63cc-63d8 */
            else if (*cur != CAPACITY - 2 && *cur != CAPACITY - 1) /* cap lbu @0x800c63e0 */
                *cur += 2;
        } else if (held & RE15_PAD_BIT_UP) {             /* @0x800c6410-6468 */
            se4(4);
            if (*cur >= 2) {                             /* sltiu 2 @0x800c6438 FIRST */
                if (*cur == 0x0a)        *cur = s_saved_cursor;  /* @0x800c644c-6458 */
                else                     *cur -= 2;              /* addiu +254 @0x800c6464 */
            }
        }
    }

    /* CONFIRM: virtual CROSS 0x4000 (@0x800c6470-78). SE(4,6) plays BEFORE the id
     * check (@0x800c6484) — also on an empty cell. */
    if (pressed & RE15_PAD_BIT_CROSS) {
        if (*cur != 0x0a) {
            uint8_t id = g_inv.slots[*cur].id;           /* lbu (s1+cur*4), s1=0x800b10ac
                                                          * inherited from FUN_8004a0cc
                                                          * @0x8004a0f8 */
            se4(6);
            if (id != 0) {
                g_inv_screen.action_dir = 0;             /* sb zero 25d6 @0x800c64c0 */
                g_inv_screen.item_state = 3;             /* 25c2=3 @0x800c64c8: command
                                                          * cluster slide-in */
            }
        } else if (re15_inv_equipped_slot() != 0x80) {
            /* VESTIGIAL direct-unequip @0x800c64d4-6534 — unreachable in the shipped
             * MZD build (the only nav route to 0xA is the patched-dead LEFT jump).
             * Kept shape-true: SE(4,0xa), 25c2=5, 25c3=2 (the equip-anim FSM entry,
             * WAVE 3), 25cd=0x3e, cursors restored, name tail SKIPPED (j 0x800c65f0). */
            se4(6);
            se4(0x0a);
            g_inv_screen.item_state = 5;
            g_inv_screen.arms_rgb = 0x3e;                /* 25cd @0x800c6520 */
            *cur = s_saved_cursor;                       /* @0x800c6524 */
            g_inv_screen.second_cursor = s_saved_cursor; /* @0x800c652c */
            return;
        } else {
            se4(6);
        }
    } else if (pressed & RE15_PAD_BIT_SQUARE) {
        /* CANCEL: virtual 0x8000 @0x800c6538-58: 25c2=2 (set BEFORE the SE) + SE(4,5). */
        g_inv_screen.item_state = 2;                     /* @0x800c6548 */
        se4(5);
    } else if (pressed & RE15_PAD_BIT_START) {
        /* raw START 0x800 @0x800c6560-98: SE(4,5) + 25bf++ = IMMEDIATE close phase,
         * no slide-back. */
        se4(5);
        s_phase = 2;
    }

    /* per-frame tail @0x800c659c-65ec: item-name print FUN_80028c1c(0x18,0xa8,1,
     * cursor==0xA ? 1 : id) + mirror 25be := 25bd. */
    g_inv_screen.name_item = (*cur == 0x0a) ? 1 : (int16_t)g_inv.slots[*cur].id;
    g_inv_screen.second_cursor = *cur;
}

/* ---------------------------------------------------------------------------------- */
/* COMMAND SELECT (ITEM state 4) — EXE @0x8004a458-0x8004a700 (disasm this session)    */
/* ---------------------------------------------------------------------------------- */
static void command_select(uint16_t pressed)
{
    /* raw press-edge d-pad -> 25d6, EXCLUSIVE priority Right>Left>Down>Up, each with
     * SE(4,4) and a jump to the tail (skipping confirm/cancel): @0x8004a458-4fc. */
    if      (pressed & RE15_PAD_BIT_RIGHT) { g_inv_screen.action_dir = 3; se4(4); return; }
    else if (pressed & RE15_PAD_BIT_LEFT)  { g_inv_screen.action_dir = 1; se4(4); return; }
    else if (pressed & RE15_PAD_BIT_DOWN)  { g_inv_screen.action_dir = 2; se4(4); return; }
    else if (pressed & RE15_PAD_BIT_UP)    { g_inv_screen.action_dir = 0; se4(4); return; }

    /* CONFIRM: virtual 0x4000 @0x8004a504-14 -> SE(4,6) @0x8004a51c + jr via table
     * @0x8004a73c[25d6]: [0]->25c2=5 (USE @0x8004a550), [1]->25d6=0 + 25c2=9 (CHECK
     * @0x8004a558-64), [2]->25c2=8 (EXIT @0x8004a568-6c), [3]->25c2=7 (EXCHANGE
     * @0x8004a570), [4]-> the cancel check @0x8004a648 (guard for the CHECK flow's
     * 25d6 byte reuse). */
    if (pressed & RE15_PAD_BIT_CROSS) {
        se4(6);
        switch (g_inv_screen.action_dir) {
        case 0: g_inv_screen.item_state = 5; return;
        case 1: g_inv_screen.action_dir = 0; g_inv_screen.item_state = 9; return;
        case 2: g_inv_screen.item_state = 8; return;
        case 3: g_inv_screen.item_state = 7; return;
        default: break;                        /* [4] falls to the cancel check */
        }
    }
    /* CANCEL: virtual 0x8000 @0x8004a648-58 -> SE(4,5) @0x8004a660 + 25c2=6
     * (slide the cluster back out, return to GRID). */
    if (pressed & RE15_PAD_BIT_SQUARE) {
        se4(5);
        g_inv_screen.item_state = 6;
    }
}

/* ---------------------------------------------------------------------------------- */
/* ITEM state 5 (USE) — WAVE-3 BRIDGE.                                                */
/* Byte-true state 5 = per-frame jal FUN_8004aa24 (@0x8004a674), the id-classifier     */
/* chain with its own sub-FSM on 25c3 (equip/unequip swap anims, heal wipe-wait c4,    */
/* key-item handlers) — NOT yet RE-ported. The bridge keeps the port's existing        */
/* working features reachable at the byte-true call site:                              */
/*   - WEAPON (classifier weapon gate `sltiu id,0x15` @0x8004aa64): equip = the equip  */
/*     RECORD 25c8 := cursor slot; USE on the already-equipped item = unequip          */
/*     (classifier c3=2; 25c8 := 0x80). The PLAYER-side weapon commit happens at       */
/*     CLOSE (@0x80046688), not here — byte-true. The swap ANIM (25c2=5 sub-states)    */
/*     is WAVE 3.                                                                     */
/*   - HEAL (0x22..0x2f, @0x8004ab48): the port's item_use prompt FSM (item_use_       */
/*     common.c; arms the ECG wipe via re15_inv_screen_heal_wipe on consume).          */
/*   - other ids: nothing.                                                            */
/* All exits return to state 6 (slide back to the grid) = the heal flow's byte-true    */
/* exit (master state 6 @0x8004b038 tail).                                            */
/* ---------------------------------------------------------------------------------- */
static void use_bridge(uint16_t pressed)
{
    uint8_t cur = g_inv_screen.item_cursor;
    uint8_t id;

    if (re15_item_use_active()) {                 /* heal prompt owns the pad */
        re15_item_use_tick(pressed);
        if (!re15_item_use_active()) {
            /* heal done (consumed or declined) -> state 6. A consume already ran the
             * compaction (FUN_8004dadc inside re15_inv_remove_slot) + the ECG wipe. */
            g_inv_screen.item_state = 6;
        }
        return;
    }

    id = g_inv.slots[cur].id;
    if (id != 0 && id < 0x15) {                   /* WEAPON gate sltiu 0x15 @0x8004aa64 */
        int eq = re15_inv_equipped_slot();
        if (eq == cur) re15_inv_set_equipped_slot(0x80);  /* unequip (classifier c3=2) */
        else           re15_inv_set_equipped_slot(cur);   /* equip record 25c8 := slot */
        re15_inv_screen_sync_equip();             /* ARMS panel regs (LAB_80049524 rules;
                                                   * the live swap-anim updates = WAVE 3) */
        g_inv_screen.item_state = 6;
        return;
    }
    if (re15_item_use_is_heal(id)) {              /* heal branch @0x8004ab48 */
        re15_item_use_start(id, cur);
        return;
    }
    g_inv_screen.item_state = 6;                  /* ammo/key handlers: WAVE 3 */
}

/* ---------------------------------------------------------------------------------- */
/* ITEM mode (run sub-state 3) — FUN_8004a0cc, FSM on 25c2 (jump table @0x8004a714)    */
/* ---------------------------------------------------------------------------------- */
static void item_mode(uint16_t pressed, uint16_t held)
{
    /* head: pad-2 refill + SELECT debug item spawner (@0x8004a0e0-a35c) — the shipped
     * build's debug features (browse/spawn ITEMALL items on SELECT). DEFERRED: needs the
     * ITEMALL browse-upload path; not part of the wave-2 acceptance. */
    switch (g_inv_screen.item_state) {
    case 0:  /* entry slide @0x8004a394-3c0: 25ea +14/frame while <251 (slti 251);
              * lands exactly 166+14*7=264, then 25c2:=1. */
        if (g_inv_screen.tab_base_y < 251) g_inv_screen.tab_base_y += 14;
        else                               g_inv_screen.item_state = 1;
        break;
    case 1:
        grid(pressed, held);
        break;
    case 2:  /* exit slide @0x8004a3d4-418: 25ea -14/frame while >=167 (slti 167);
              * lands 166, then 25ca:=0, 25c1:=0, 25c2:=0, 25c3:=0 (tab kept). */
        if (g_inv_screen.tab_base_y >= 167) g_inv_screen.tab_base_y -= 14;
        else {
            g_inv_screen.highlight = 0;           /* @0x8004a3f4-3f8 */
            s_substate = 0;                       /* @0x8004a400 */
            g_inv_screen.item_state = 0;          /* @0x8004a408 */
        }
        break;
    case 3:  /* command-cluster slide-in @0x8004a41c-454: 25ee -14 while >=167 -> 166,
              * then state 4. */
        if (g_inv_screen.act_base_y >= 167) g_inv_screen.act_base_y -= 14;
        else                                g_inv_screen.item_state = 4;
        break;
    case 4:
        command_select(pressed);
        break;
    case 5:
        use_bridge(pressed);                      /* byte-true: jal FUN_8004aa24 @0x8004a674 */
        break;
    case 6:  /* cancel slide-out @0x8004a684-6a8: 25ee +14 while <251 -> 264, then
              * state 1 (back to GRID, not tab-select). */
        if (g_inv_screen.act_base_y < 251) g_inv_screen.act_base_y += 14;
        else                               g_inv_screen.item_state = 1;
        break;
    case 7:  /* EXCHANGE: byte-true = per-frame jal FUN_8004b33c @0x8004a6ac (combine
              * flow) — WAVE-5 stub: return to the command stage. (FUN_8004b33c's own
              * exit contract not RE'd — spec Task-C open question.) */
        g_inv_screen.item_state = 4;
        break;
    case 8:  /* EXIT slide-out @0x8004a6bc-6d8: 25ee +14 while <251 -> 264, then
              * state 2 (tab-cluster slide-back + tab select). */
        if (g_inv_screen.act_base_y < 251) g_inv_screen.act_base_y += 14;
        else                               g_inv_screen.item_state = 2;
        break;
    case 9:  /* CHECK: byte-true = per-frame jal FUN_800c6630 @0x8004a6e8 (photo spin
              * + description) — WAVE-3 stub via its terminal contract @0x800c683c-50:
              * 25c2:=4 (command stage) + 25d6:=1 (CHECK re-highlighted). */
        g_inv_screen.item_state = 4;
        g_inv_screen.action_dir = 1;
        break;
    default:
        g_inv_screen.item_state = 1;
        break;
    }
}

/* ---------------------------------------------------------------------------------- */
/* Master menu task (LAB_8004603c phase loop @0x80046070-8c)                           */
/* ---------------------------------------------------------------------------------- */
static void phase0_init(void)
{
    /* FUN_800460b8 init order (spec Task A):
     * (1) FUN_80029bf8(0) task-0 SUSPEND @0x800460bc-c4 — modeled by
     *     re15_menu_gameplay_frozen() (game_step returns; platform 30Hz block gated).
     * (2) FUN_80030640(0xf000,0xf,4) @0x800460cc-d8 — D-pad auto-repeat config.       */
    s_rep_delay = 15; s_rep_rate = 4; s_rep_ctr = 0;
    /* (3) DAT_800b5456=0 @0x800460dc — frame-pacing mode input (FUN_80061fc0 vsync-
     *     divider; the port has no divider — PC runs a fixed 30fps cap; noted).
     * (4) DrawSync/geometry/CLUT init @0x800460e4-fc — wave-1 build-time geometry.
     * (5) DAT_800b25ce equip snapshot @0x8004649c.                                    */
    s_snapshot = equip_id_now();
    /* (6) FUN_8004dadc inventory compaction @0x800464a0. */
    re15_inv_compact();
    /* (7) screen init LAB_80049524 (prim arenas + register init = re15_inv_screen_open;
     *     25bc/bd/be/d6=0 @0x800463e0-f8, 25ca=0 @0x8004643c, 25d4=0 @0x8004645c)      */
    re15_inv_screen_open();
    s_substate = 0;
    s_saved_cursor = 0;                    /* DAT_800b25d7 (live init value 0) */
    s_close_sub = 0;
    /* fade-in arm @0x800496c4-704: FUN_800217b0(0x200,-0x1800,7,0) + FUN_800216ec
     * (value ignored, level:=0x7fff) — ~6 drawn frames while the menu draws. */
    re15_fade_config(0, 2, 7, (int16_t)-0x1800, 0);
    re15_fade_kick(0, 0);
}

static void close_phase(void)
{
    if (s_close_sub == 0) {
        /* fade-out arm @0x80046544-7c: FUN_800217b0(0x200,+0x1800,7,0) + kick (a1=0x200
         * ignored, level:=0). The screen KEEPS DRAWING during the ~6 frames (the close
         * loop calls the per-screen draw @0x80046594-f0; is_open stays 1 here). */
        re15_fade_config(0, 2, 7, (int16_t)0x1800, 0);
        re15_fade_kick(0, 0);
        s_close_sub = 1;
        return;
    }
    if (s_close_sub == 1) {
        if (!re15_fade_done(0)) return;
        /* equip-changed check @0x800465f4-661c: lbu inv[25c8*4] vs snapshot 25ce. */
        if (equip_id_now() != s_snapshot) {
            /* hold-black + 2-frame yield covering the commit @0x80046620-50. */
            re15_fade_config(0, 2, 7, 0, 0);
            re15_fade_kick(0, 0x7fff);
            s_hold = 2;
            s_close_sub = 2;
            return;
        }
        s_close_sub = 3;                    /* unchanged -> straight to teardown */
    }
    if (s_close_sub == 2) {
        if (s_hold > 0) { s_hold--; return; }
        {   /* equip commit @0x80046654-66c8: DAT_800aca5d = (25c8==0x80 ? 1 :
             * inv[25c8].id) @0x80046668-88; weapon model rebuild FUN_80036b68 +
             * resource load FUN_80043d8c(aca5d, 0x80198000) @0x800466a4-c8 (port:
             * set_equipped_weapon + ARMS SE bank re-prime). */
            int eq = re15_inv_equipped_slot();
            int wid = (eq == 0x80) ? 1 : g_inv.slots[eq & 0x0f].id;
            re15_player_set_equipped_weapon(wid);
            re15_audio_prime_weapon(wid);
        }
        s_close_sub = 3;
    }
    /* common teardown @0x800466f8-46780: DAT_800b5457=2 backdrop re-upload + 5456=2
     * pacing restore (no port divider), zero 25bf/25c0/25c1/25c2/25c3, aca3c&=~0x200;
     * FUN_80029c2c(0) resume + FUN_80029afc self-kill @0x8004677c-84. */
    s_phase = 0; s_substate = 0; s_p0_entered = 0; s_close_sub = 0;
    g_inv_screen.item_state = 0;
    g_inv_screen.name_item = -1;
    s_alive = 0;
    /* Task-0 resume continuation @0x8001cb50-74: aca3c &= ~(0x40|0x8000); 5359=3;
     * falls THROUGH into stage 3 @0x8001cbb8 in the same round (no unfaded frame):
     * gameplay fade-in arm FUN_800217b0(0x200,-0x1800,7,0)+kick @0x8001cc00-18,
     * then 5359=4 @0x8001cc20-28. */
    s_request = 0; s_latch = 0;
    re15_fade_config(0, 2, 7, (int16_t)-0x1800, 0);
    re15_fade_kick(0, 0);
    s_stage = 4;
}

static void menu_task_step(uint16_t pressed, uint16_t held)
{
    /* name print is queued per-frame by the grid tail only (@0x800c659c) */
    g_inv_screen.name_item = -1;
    /* the pad-refresh auto-repeat tick runs EVERY frame (FUN_80030444) */
    repeat_update(pressed, held);

    switch (s_phase) {
    case 0:
        if (!s_p0_entered) { s_p0_entered = 1; phase0_init(); }
        /* fade-in loop @0x8004970c-2c: draw + vsync until FUN_8002178c(0)!=0 — input
         * is NOT processed while fading in. */
        if (!re15_fade_done(0)) return;
        s_phase = 1;                            /* 25bf++ @0x800464d4-e8 */
        return;
    case 1:
        /* LAB_80046500 -> LAB_8004974c (screen 0): sub-state dispatch @0x800497b8-f8,
         * then the draw chain (built platform-side from g_inv_screen). */
        switch (s_substate) {
        case 0: tab_select(pressed); break;
        case 1:
            /* MAP-in-status (FUN_8004c058): 25-frame register slide + VRAM save +
             * map-gfx CD upload + interactive L1-toggle — a separate screen, WAVE-3
             * STUB: return immediately per the byte-true exit contract @0x8004c2c4-304
             * (25ca=0, 25c1=0, 25c2=0, 25c3=0). */
            g_inv_screen.highlight = 0;
            g_inv_screen.item_state = 0;
            s_substate = 0;
            break;
        case 2:
            /* FILE screen (DEBUG.BIN FUN_800c6ca0): 30-frame slide + 3-page viewer —
             * WAVE-3 STUB: return immediately per the exit contract @0x800c6f74-8c
             * (25ca=0, 25c1=0, 25c2=0, 25c3=0). */
            g_inv_screen.highlight = 0;
            g_inv_screen.item_state = 0;
            s_substate = 0;
            break;
        case 3: item_mode(pressed, held); break;
        default: s_substate = 0; break;
        }
        return;
    case 2:
        close_phase();
        return;
    default:
        s_phase = 0;
        return;
    }
}

/* ---------------------------------------------------------------------------------- */
/* Per-frame FSM tick: transition stages (DAT_800b5359) + the menu task step.          */
/* ---------------------------------------------------------------------------------- */
void re15_menu_fsm_tick(uint16_t pad_pressed, uint16_t pad_held)
{
    if (s_alive) { menu_task_step(pad_pressed, pad_held); return; }

    switch (s_stage) {
    case 1:
        /* stage 1 @0x8001c9c8: wait for the message system (DAT_800b8520&0x80 -> skip
         * frame; port: the msg FSM active flag) — a lingering timed/paged message
         * finishes first (the msg tick runs platform-side, outside game_step). Then
         * save the FUN_80061fc0 pacing mode (@0x8001c9f4; no port divider) and arm the
         * gameplay fade-out FUN_800217b0(0x200,+0x1800,7,0)+FUN_800216ec(0,0,0,0)
         * @0x8001ca64-88; 5359=2, falling into the stage-2 poll. */
        if (g_scd.message_fsm_active) return;
        re15_fade_config(0, 2, 7, (int16_t)0x1800, 0);
        re15_fade_kick(0, 0);
        s_stage = 2;
        s_stage2_sub = 0;
        return;
    case 2:
        /* stage 2 @0x8001ca98: poll FUN_8002178c(0); on done: hold-black (step-0 kick
         * 0x7fff) @0x8001caa8-cc + FUN_80029ac8(2) 2-frame yield on black @0x8001cad0,
         * pauseflags=0 @0x8001cae8, then spawn the menu task FUN_80029a98(1,
         * LAB_8004603c) @0x8001cb34-44 — task 0 PARKS at the following yield for the
         * whole menu lifetime (savestate: task0 status 0x41, aca3c 0x8040). */
        if (s_stage2_sub == 0) {
            if (!re15_fade_done(0)) return;
            re15_fade_config(0, 2, 7, 0, 0);
            re15_fade_kick(0, 0x7fff);
            s_hold = 2;
            s_stage2_sub = 1;
            return;
        }
        if (s_hold > 0) { s_hold--; return; }
        s_alive = 1;                    /* task slot 1 fn=LAB_8004603c status=1 */
        s_phase = 0;
        s_p0_entered = 0;
        /* (aca38&0x08000000 SELECT+START alternate task 0x8002dde4 @0x8001cb08-30:
         * unresolved identity — not ported.) */
        return;
    case 3:
        /* normally executed inline by close_phase (same-round fall-through); kept for
         * completeness if entered externally. */
        re15_fade_config(0, 2, 7, (int16_t)-0x1800, 0);
        re15_fade_kick(0, 0);
        s_stage = 4;
        return;
    case 4:
        /* stage 4 @0x8001cc34-6c: pauseflag bookkeeping (saved|=flags; flags|=
         * 0xff000000 — world stays frozen during the fade-in = frozen() here); 5359=5. */
        s_stage = 5;
        return;
    case 5:
        /* stage 5 @0x8001cc70-94: wait FUN_8002178c(0) (~6 fade-in frames), then
         * 5359=0 + pauseflags restored to the saved normal value (7). */
        if (!re15_fade_done(0)) return;
        s_stage = 0;
        return;
    default:
        return;
    }
}

/* ---------------------------------------------------------------------------------- */
/* DEBUG toggle (harnesses: RE15_INV_SHOT / RE15_ITEM_USE_TEST): instant open at        */
/* tab-select (full phase-0 init, fades skipped) / instant close+commit.               */
/* ---------------------------------------------------------------------------------- */
void re15_menu_toggle(void)
{
    if (!s_alive && s_stage == 0) {
        phase0_init();
        re15_fade_kill(0);          /* skip the fade-in (debug only) */
        s_alive = 1;
        s_p0_entered = 1;
        s_phase = 1;
    } else if (s_alive) {
        int eq = re15_inv_equipped_slot();
        int wid = (eq == 0x80) ? 1 : g_inv.slots[eq & 0x0f].id;
        if (equip_id_now() != s_snapshot) {          /* the close-phase commit, no fades */
            re15_player_set_equipped_weapon(wid);
            re15_audio_prime_weapon(wid);
        }
        s_alive = 0; s_phase = 0; s_substate = 0; s_p0_entered = 0; s_close_sub = 0;
        s_request = 0; s_latch = 0; s_stage = 0;
        g_inv_screen.item_state = 0;
        g_inv_screen.name_item = -1;
    }
}
