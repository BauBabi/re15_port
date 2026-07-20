/*
 * menu_common.c — the byte-true RE1.5 STATUS/INVENTORY screen FSM (waves 2+3).
 *
 * Spec: shots/inv_wave2_spec.md (RE workflow wf_e8e48d36, 4 reports, citations complete)
 * + shots/inv_wave3_spec.md (wf_cbdbc8a1 — the ITEM state-5 USE dispatcher/classifier,
 * the equip/unequip/swap per-step anim tables, the prompt-less heal sub-FSM and the
 * cant-use message path, all raw-MIPS cited)
 * + fresh file disasm of the DEBUG.BIN grid handler 0x800c62a0-0x800c65fc (byte-identical
 * to the mzd_inv_open.sav RAM module; DEBUG.BIN maps @0x800c0000) and of the EXE command
 * stage @0x8004a458-0x8004a700 (re15_disasm.py, wave-2 session).
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
#include "re15_item_use.h"      /* heal classifier gate + applier table @0x80010fbc (wave 3) */
#include "re15_actor.h"         /* g_actors[PLAYER].hp — the heal 0x2f direct write */
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
static uint8_t s_c3 = 0;           /* DAT_800b25c3 — state-5 flow selector (wave 3;
                                    * zeroed by the master task @0x80046050-6c)       */
static uint8_t s_c4 = 0;           /* DAT_800b25c4 — flow step counter (written only
                                    * by the flows; terminals restore 0)              */

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
            s_c3 = 0;
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
             * Kept shape-true: SE(4,0xa), 25c2=5, 25c3=2 (UNEQUIP-anim entry @0x800c650c),
             * 25c4=0 (@0x800c6518), 25cd=0x3e, cursors restored, name tail SKIPPED
             * (j 0x800c65f0). */
            se4(6);
            se4(0x0a);
            g_inv_screen.item_state = 5;
            s_c3 = 2;                                    /* @0x800c650c */
            s_c4 = 0;                                    /* @0x800c6518 */
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
/* ITEM state 5 (USE) — WAVE 3, spec shots/inv_wave3_spec.md.                          */
/* Byte-true state 5 = per-frame jal FUN_8004aa24 (@0x8004a674): dispatcher            */
/* @0x8004aa24-aa60 does jalr PTR[0x80074c28 + 25c3*4]. Table @0x80074c28:             */
/*   [0]=0x8004aa64 classifier   [1]=0x8004ab88 equip anim   [2]=0x8004ad10 unequip    */
/*   [3]=0x8004adcc heal         [4]=0x8004b074 nop (jr ra)  [5]=0x8004b07c swap       */
/*   [6]=0x8004b250 cant-use msg [7]=0x8004b37c (vestigial — no c3:=7 writer)          */
/* Terminal invariant: EVERY flow resets c3=0 AND c4=0 before leaving state 5, so the  */
/* classifier always runs first on re-entry (verified per-terminal in the spec).       */
/* Register mirrors: 25c3=s_c3, 25c4=s_c4 (declared in the state block above),         */
/* 25c8=re15_inv_equipped_slot (+g_inv_screen mirror), 25c9=re15_inv_prev_equip_slot,  */
/* 25cd=arms_rgb, 2608=arms_slide, 25dc/25de=equip_x/equip_y, 25d4=wipe_mode,          */
/* 25ee=act_base_y.                                                                    */
/* ---------------------------------------------------------------------------------- */
int re15_menu_item_c3(void) { return s_c3; }
int re15_menu_item_c4(void) { return s_c4; }

/* ---- cant-use message mini-FSM (the msg-VM slice for the ONE menu message) --------
 * Open = FUN_80027e68(a0=0x00a80018, a1=0x8400, a2=0, a3=0) @0x8004b2d8-b2f8: 8534/36
 * = (0x18,0xa8), 8522=a1&0x80=0 (NO Yes/No), string = desc-bank entry 0 (DEBUG.BIN
 * @0x800c50de+0x90 = "You can't use it here."), 8520:=0x80 (active). Ticked per
 * RENDERED frame by the msg VM FUN_80028134 (single caller = the present-chain fn
 * FUN_80010000 via FUN_800280b4 — runs while task 0 is suspended); the port ticks it
 * once per menu frame at the end of menu_task_step (same 1-tick/frame cadence).
 * Typewriter (status screen, DAT_800b5456==0 -> s1=1 @0x80028148/0x80028168): first
 * glyph after 8525 = 1<<1 = 2 frames, then 8524 = 2<<1 = 4 frames/glyph
 * (@0x80028194-281c4). Terminator 01 00 -> VM state 5 PRESS-WAIT. FAST-FORWARD is
 * structurally impossible (held-CROSS FF @0x800281ec-f4 gated on 8522!=0). DISMISS
 * (state 5 @0x8002868c-86d0): virtual EDGE word & 0xc000 (CROSS confirm OR SQUARE
 * cancel) -> input eat DAT_800ac768:=0xffff (virtual HELD word — no menu held-reader
 * consumes it, the grid d-pad repeat reads the RAW held word, so no port action),
 * 8520 &= 0x7f. NO auto-dismiss, NO SE on dismiss (no jal 0x80045024 in the block). */
static uint8_t s_msg_active = 0;   /* DAT_800b8520 bit 0x80 */
static uint8_t s_msg_reveal = 0;   /* glyphs revealed (0..22) */
static uint8_t s_msg_timer  = 0;   /* 8525/8524 countdown */

int re15_menu_msg_active(void) { return s_msg_active; }

static void msg_open(void)
{
    s_msg_active = 1;              /* 8520 := 0x80 */
    s_msg_reveal = 0;
    s_msg_timer  = 2;              /* 8525 = 1<<1 (first glyph after 2 frames) */
}

static void msg_vm_tick(uint16_t pressed)
{
    if (!s_msg_active) { g_inv_screen.msg_reveal = 0; return; }
    if (s_msg_reveal < RE15_INV_CANTUSE_GLYPHS) {
        if (--s_msg_timer == 0) { s_msg_reveal++; s_msg_timer = 4; }  /* 8524 = 2<<1 */
    } else if (pressed & (RE15_PAD_BIT_CROSS | RE15_PAD_BIT_SQUARE)) {
        s_msg_active = 0;          /* 8520 &= 0x7f @0x800286c4 (edge test andi 0xc000) */
    }
    g_inv_screen.msg_reveal = s_msg_active ? s_msg_reveal : 0;
}

/* [0] classifier @0x8004aa64 — exact branch order (id = inv[25bd*4].id @0x800b10ac). */
static void state5_classifier(void)
{
    uint8_t cur = g_inv_screen.item_cursor;
    uint8_t id  = g_inv.slots[cur].id;
    if (id < 0x15) {                              /* WEAPON gate sltiu id,0x15 @0x8004aa90
                                                   * (id!=0 guaranteed: grid confirm gates
                                                   * id @0x800c64b4 before state 3) */
        int eq = re15_inv_equipped_slot();
        /* equipped-id read @0x8004aac4: lbu inv[25c8*4]. For 25c8==0x80 this is the
         * OUT-of-inventory byte @0x800b12ac (base+0x200), read BEFORE the 0x80 check —
         * byte-true as coded. Live value = 0x00 (mzd_inv_open.sav, measured this wave);
         * the cursor id is never 0 here, so the compare can never match on that path. */
        uint8_t eq_id = (eq == 0x80) ? 0x00 : g_inv.slots[eq & 0x0f].id;
        s_c4 = 0;                                 /* sb zero->25c4 @0x8004aaa8 (ALL weapon
                                                   * outcomes) */
        if (eq_id == id) {                        /* ID equality bne @0x8004aadc (NOT slot
                                                   * equality — settles the report split) */
            s_c3 = 2;                             /* UNEQUIP @0x8004aaec */
            g_inv_screen.arms_rgb = 0x3e;         /* 25cd @0x8004aaf8 */
        } else if (eq == 0x80) {                  /* delay-slot ori v0,0x80 @0x8004aae0;
                                                   * bne a0,v0 @0x8004ab04 */
            g_inv_screen.arms_rgb = 0x80;         /* 25cd @0x8004ab14 */
            s_c3 = 1;                             /* EQUIP @0x8004ab20 */
        } else {
            g_inv_screen.arms_rgb = 0x3e;         /* 25cd @0x8004ab2c-38 */
            s_c3 = 5;                             /* SWAP @0x8004ab3c */
        }
        return;
    }
    if ((uint8_t)(id - 0x22) < 0xe && id != 0x25) {  /* sltiu 0xe @0x8004ab48; beq 0x25
                                                      * @0x8004ab54 (Red excluded) */
        s_c3 = 3;                                 /* HEAL @0x8004ab60 — c4 NOT reset here
                                                   * (relies on the terminal invariant) */
        return;
    }
    s_c3 = 6;                                     /* cant-use @0x8004ab6c-ab74 (ammo
                                                   * 0x15-0x21, Red 0x25, ids >= 0x30) */
    s_c4 = 0;                                     /* @0x8004ab7c */
}

/* [1] EQUIP anim walker @0x8004ab88 (12-step fn-pointer table @0x80010f24: steps 0-4 and
 * 6-10 -> tick 0x8004ac7c, step 5 -> commit 0x8004abbc falling into the tick, step 11 ->
 * terminal 0x8004acec; 1 step = 1 frame, each step increments c4 itself). */
static void state5_equip_anim(void)
{
    if (s_c4 >= 0x0c) return;                     /* guard sltiu v0,c4,0xc @0x8004ab94 */
    if (s_c4 == 11) {                             /* terminal @0x8004acec-ad04 */
        g_inv_screen.item_state = 6;              /* 25c2 := 6 (slide back to grid) */
        s_c3 = 0; s_c4 = 0;
        return;
    }
    if (s_c4 == 5) {                              /* commit @0x8004abbc-ac78 */
        uint8_t cur = g_inv_screen.item_cursor;
        uint8_t old = (uint8_t)re15_inv_equipped_slot();
        re15_inv_set_equipped_slot(cur);          /* 25c8 := 25bd @0x8004abd0 */
        re15_inv_set_prev_equip_slot(old);        /* 25c9 := old 25c8 @0x8004abe0 (=0x80
                                                   * on this path — equip needs empty) */
        g_inv_screen.equipped_slot = cur;         /* (port mirror of the one 25c8 byte) */
        {   /* ammo-counter regs by the NEW slot's kind byte 0x800b10ae[slot*4] */
            uint8_t kind = g_inv.slots[cur].flags;
            if (kind == 0)      { g_inv_screen.equip_x = 0x96; g_inv_screen.equip_y = 0x35; } /* @0x8004ac04-14 */
            else if (kind == 1) { g_inv_screen.equip_x = 0x82; g_inv_screen.equip_y = 0x2f; } /* @0x8004ac34-48 */
            else                { g_inv_screen.equip_x = 0xaa; g_inv_screen.equip_y = 0x2f; } /* @0x8004ac64-78 */
        }
        /* falls THROUGH into the common tick */
    }
    /* common tick @0x8004ac7c-ace8: the cd fade is gated on the STALE 25c9 (the 3-writer
     * global) — after swap->unequip within a session, steps 0-4 skip the decrement and
     * the terminal cd is 0x5c instead of 0x3e (byte-true history quirk, must reproduce). */
    if (re15_inv_prev_equip_slot() == 0x80)       /* lbu 25c9; bne 0x80 @0x8004ac88 */
        g_inv_screen.arms_rgb = (uint8_t)(g_inv_screen.arms_rgb + 250); /* 25cd += 250
                                                   * (u8 wrap = -6) @0x8004ac9c-aca4 */
    g_inv_screen.arms_slide += 5;                 /* 2608 += 5 @0x8004acc4/acd0 */
    g_inv_screen.equip_y    += 1;                 /* 25de += 1 @0x8004acc8/acd8 (terminal
                                                   * lands 0x3b kind0 — one MORE than the
                                                   * reopen rest 0x3a; byte-true, persists
                                                   * until the next screen open) */
    s_c4++;                                       /* @0x8004acc0/ace0 */
}

/* [2] UNEQUIP anim walker @0x8004ad10 (table @0x80010f54: steps 0-4,6-10 -> tick
 * 0x8004ad50, step 5 -> 0x8004ad44, step 11 -> terminal 0x8004ada8). */
static void state5_unequip_anim(void)
{
    if (s_c4 >= 0x0c) return;                     /* sltiu 0xc @0x8004ad1c */
    if (s_c4 == 11) {                             /* terminal @0x8004ada8-adc0 */
        g_inv_screen.item_state = 6;
        s_c3 = 0; s_c4 = 0;
        return;
    }
    if (s_c4 == 5) {                              /* @0x8004ad44-ad4c: ori 0x80 -> 25c8
                                                   * (equipped slot cleared MID-anim) */
        re15_inv_set_equipped_slot(0x80);
        g_inv_screen.equipped_slot = 0x80;
    }
    /* tick @0x8004ad50-ada4 — UNCONDITIONAL (no 25c9 gate; exact mirror of equip) */
    g_inv_screen.arms_rgb = (uint8_t)(g_inv_screen.arms_rgb + 6);  /* 25cd += 6 @0x8004ad64-68 */
    s_c4++;                                       /* @0x8004ad7c-84 */
    g_inv_screen.arms_slide -= 5;                 /* 2608 -= 5 @0x8004ad88/ad94 */
    g_inv_screen.equip_y    -= 1;                 /* 25de -= 1 @0x8004ad8c/ad9c */
}

/* [5] SWAP inline walker @0x8004b07c (no table: slti c4,11 @0x8004b08c; c4==0 -> commit
 * 0x8004b0c4 falling into the tick, c4 1-10 -> tick 0x8004b184, c4==11 -> terminal
 * 0x8004b234). NO 2608 write anywhere (the gun does not slide in a swap). */
static void state5_swap_anim(void)
{
    if (s_c4 == 11) {                             /* terminal @0x8004b234-b244 */
        g_inv_screen.item_state = 6;
        s_c3 = 0; s_c4 = 0;
        return;
    }
    if (s_c4 > 11) return;                        /* slti fallthrough (unreachable) */
    if (s_c4 == 0) {                              /* commit @0x8004b0c4-b180 */
        uint8_t cur = g_inv_screen.item_cursor;
        uint8_t old = (uint8_t)re15_inv_equipped_slot();
        re15_inv_set_equipped_slot(cur);          /* 25c8 := 25bd @0x8004b0d8 */
        re15_inv_set_prev_equip_slot(old);        /* 25c9 := old slot @0x8004b0e8 (!=0x80:
                                                   * swap requires something equipped) */
        g_inv_screen.equipped_slot = cur;
        {   /* NEW slot kind: NOTE kind0 = (0xac,0x3a) here vs equip's (0x96,0x35)! */
            uint8_t kind = g_inv.slots[cur].flags;
            if (kind == 0)      { g_inv_screen.equip_x = 0xac; g_inv_screen.equip_y = 0x3a; } /* @0x8004b108-11c */
            else if (kind == 1) { g_inv_screen.equip_x = 0x82; g_inv_screen.equip_y = 0x2f; } /* @0x8004b13c-150 */
            else                { g_inv_screen.equip_x = 0xaa; g_inv_screen.equip_y = 0x2f; } /* @0x8004b16c-180 */
        }
        /* falls into the tick */
    }
    /* tick @0x8004b184-b228 (c4 0..10): reads the kind of the CURRENT 25c8 */
    {
        int eq = re15_inv_equipped_slot();        /* lbu kind of 25c8 @0x8004b188-b1a0 */
        uint8_t kind = g_inv.slots[eq & 0x0f].flags;
        if (kind == 0) g_inv_screen.equip_x -= 2; /* 25dc -= 2 @0x8004b1b4-b1c4 (0xac ->
                                                   * 0x96 after 11 ticks = resting) */
        else           g_inv_screen.equip_y += 1; /* 25de += 1 @0x8004b1d4-b1e4 */
    }
    /* shared cd guard — DEAD in a legal swap (step 0 always wrote 25c9 != 0x80 first),
     * kept exactly as coded @0x8004b1ec-b210. */
    if (re15_inv_prev_equip_slot() == 0x80)
        g_inv_screen.arms_rgb = (uint8_t)(g_inv_screen.arms_rgb + 250);
    s_c4++;                                       /* @0x8004b218-b228 */
}

/* [3] HEAL @0x8004adcc — NO prompt, NO message; 3-substate sub-FSM on c4 (s0=100
 * @0x8004ae14, s1=id-0x22 latched from the cursor slot at fn entry each frame). */
static void state5_heal(void)
{
    uint8_t cur = g_inv_screen.item_cursor;
    uint8_t id  = g_inv.slots[cur].id;
    switch (s_c4) {
    case 0:                                       /* wipe-arm @0x8004ae48-aea0 */
        if ((uint8_t)(id - 0x22) < 0xd) {
            /* per-ITEM wipe-type table @0x80010f84 (ported: 0x23/0x26 -> mode 2,
             * 0x2e -> none, others -> mode 1) — the wipe runs BEFORE the consume. */
            re15_inv_screen_heal_wipe(id);
        } else {
            /* idx >= 0xd (only 0x2f NUT reachable): hp := 0x4d = 77 immediately,
             * NO wipe (@0x8004ae98-aea0: ori 0x4d; sh -> 0x800acaee = player hp). */
            g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 0x4d;
        }
        s_c4++;                                   /* common tail @0x8004aea4-aebc */
        break;
    case 1:                                       /* wipe wait @0x8004aec0-aedc: lbu 25d4;
                                                   * bne stay (renderer FUN_80048a44 self-
                                                   * clears after 32/36 frames) */
        if (g_inv_screen.wipe_mode != 0) break;
        s_c4 = 2;                                 /* sb 2 -> 25c4 */
        break;
    case 2:                                       /* consume + apply + exit @0x8004aee4-b070 */
        re15_inv_remove_slot(cur);                /* zero id/qty/kind @0x800b10ac/ad/ae +
                                                   * cursor*4 (@0x8004aee4-af28) + the
                                                   * compaction jal FUN_8004dadc @0x8004af2c */
        g_inv_screen.equipped_slot =              /* (port mirror refresh — the compaction
                                                   * may shift the equip slot; the original
                                                   * has the ONE 25c8 byte) */
            (uint8_t)re15_inv_equipped_slot();
        re15_item_use_apply(id);                  /* applier table @0x80010fbc (id latched
                                                   * BEFORE the zeroing — s1 from fn entry) */
        g_inv_screen.ecg_sweep = 0x20;            /* ECG sweep reset 2600:=0x20 @0x8004b038 */
        s_c3 = 0; s_c4 = 0;                       /* @0x8004b044-b050 */
        g_inv_screen.item_state = 6;              /* 25c2 := 6 @0x8004b058 (slide to grid) */
        break;
    default:
        break;
    }
}

/* [6] cant-use message @0x8004b250 — 3-substate mini-FSM on c4. */
static void state5_cantuse(void)
{
    switch (s_c4) {
    case 0:                                       /* slide the command cluster OUT bottom
                                                   * @0x8004b2a0-b2d4: lh 25ee; slti 251 */
        if (g_inv_screen.act_base_y < 251) g_inv_screen.act_base_y += 14; /* addiu 14 */
        else                               s_c4 = 1;   /* ori 1 -> 25c4 */
        break;
    case 1:                                       /* message open @0x8004b2d8-b2f8: jal
                                                   * FUN_80027e68(0x00a80018,0x8400,0,0) */
        msg_open();
        s_c4 = 2;
        break;
    case 2:                                       /* wait (8520 & 0x80)==0 @0x8004b2fc-b324 */
        if (s_msg_active) break;
        g_inv_screen.item_state = 1;              /* 25c2 := 1 — GRID directly (the command
                                                   * stage does NOT return; the cluster is
                                                   * already out at 264) */
        s_c3 = 0; s_c4 = 0;
        break;
    default:
        break;
    }
}

/* The per-frame state-5 body (jal FUN_8004aa24 @0x8004a674). */
static void state5_use(void)
{
    switch (s_c3) {                               /* jalr PTR[0x80074c28 + c3*4] */
    case 0: state5_classifier();   break;         /* [0] @0x8004aa64 */
    case 1: state5_equip_anim();   break;         /* [1] @0x8004ab88 */
    case 2: state5_unequip_anim(); break;         /* [2] @0x8004ad10 */
    case 3: state5_heal();         break;         /* [3] @0x8004adcc */
    case 4: break;                                /* [4] @0x8004b074 = pure `jr ra` no-op —
                                                   * dead/reserved (no c3:=4 setter found;
                                                   * spec open question)                   */
    case 5: state5_swap_anim();    break;         /* [5] @0x8004b07c */
    case 6: state5_cantuse();      break;         /* [6] @0x8004b250 */
    case 7: break;                                /* [7] @0x8004b37c = the EXCHANGE sub-0 fn
                                                   * (state-7 table @0x80074c44 overlap) —
                                                   * vestigial here, no c3:=7 writer        */
    default: break;
    }
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
        state5_use();                             /* byte-true: jal FUN_8004aa24 @0x8004a674
                                                   * (pad-free — the only input in state 5
                                                   * is the msg-VM dismiss, which lives in
                                                   * msg_vm_tick like the original's flush) */
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
    s_c3 = 0;                              /* master task zeroing @0x80046050-6c (25c3;
                                            * 25c4 is NOT in that list — the terminal
                                            * invariant keeps it 0) */
    s_msg_active = 0; s_msg_reveal = 0;    /* (port hygiene: the menu message cannot
                                            * survive a close — it must be dismissed to
                                            * leave state 5; guards the debug toggle) */
    g_inv_screen.msg_reveal = 0;
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
    s_c3 = 0;                              /* 25c3 in the teardown zero list @0x80046748-6c */
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

static void menu_task_dispatch(uint16_t pressed, uint16_t held)
{
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

static void menu_task_step(uint16_t pressed, uint16_t held)
{
    /* name print is queued per-frame by the grid tail only (@0x800c659c) */
    g_inv_screen.name_item = -1;
    /* the pad-refresh auto-repeat tick runs EVERY frame (FUN_80030444) */
    repeat_update(pressed, held);
    menu_task_dispatch(pressed, held);
    /* The msg VM (FUN_80028134) has EXACTLY ONE caller — the present-chain fn
     * FUN_80010000 (via FUN_800280b4, gated on 8520&0x80 @0x800280c8-d4), which runs
     * from the frame FLUSH (0x80020f3c <- FUN_80020bb0 <- 0x800544e8) — i.e. once per
     * rendered frame AFTER the task step, while task 0 stays suspended. So the message
     * types out / accepts dismissal here, and the c4==2 poll sees a dismissal on the
     * FOLLOWING frame (byte cadence). */
    if (s_alive) msg_vm_tick(pressed);
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
        s_c3 = 0; s_c4 = 0;
        s_msg_active = 0; s_msg_reveal = 0;
        g_inv_screen.item_state = 0;
        g_inv_screen.name_item = -1;
        g_inv_screen.msg_reveal = 0;
    }
}
