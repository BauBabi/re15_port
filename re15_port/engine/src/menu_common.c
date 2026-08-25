/*
 * menu_common.c — the byte-true RE1.5 STATUS/INVENTORY screen FSM (waves 2+3+5 + MAP
 * + FILE).
 *
 * MAP wave: the in-status MAP tab (sub-state 25c1==1) is the FUN_8004c058 4-state
 * runner (slide/interactive/reverse/restore) + the per-stage entry inits @0x80074c0c
 * (room jump tables -> DAT_800b260d/260e) + the FUN_80046fd8/FUN_800473f8 draw set
 * (re15_inv_screen.c); all raw-MIPS cited inline this wave (re15_disasm.py session).
 *
 * FILE wave: the in-status FILE tab (sub-state 25c1==2) is the DEBUG.BIN runner
 * FUN_800c6ca0 @0x800c6ca0 (dispatch @0x800c6fe8 on 25c2): 30-frame enter/exit slides,
 * the 3-page row list (masks @0x800c6c98, names 0x48-0x65, highlight 0x800c742c), the
 * embedded 7-page "Operation Report" reader (@0x800ccd34, selection-INDEPENDENT — every
 * row opens the same document @0x800c704c-70) with the 22-frame page-turn animation
 * (driver 0x800c77bc) and corner arrows (0x800c7528/0x800c7670). All raw bytes
 * disassembled from shared_assets/PSX/BIN/DEBUG.BIN this wave (file==RAM proven).
 *
 * Spec: shots/inv_wave2_spec.md (RE workflow wf_e8e48d36, 4 reports, citations complete)
 * + shots/inv_wave3_spec.md (wf_cbdbc8a1 — the ITEM state-5 USE dispatcher/classifier,
 * the equip/unequip/swap per-step anim tables, the prompt-less heal sub-FSM and the
 * cant-use message path, all raw-MIPS cited)
 * + shots/inv_wave5_spec.md (EXCHANGE/combine pair engine: state 7 @0x8004b33c, second
 * cursor FUN_80048904, matcher FUN_8004e900, executor FUN_8004e054 actions 0-6, the
 * 17-step result-anim walker @0x80010ff4 — select-sub cancel path + all step bodies
 * re-disassembled this wave, cites inline)
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
 * VIRTUAL edge DAT_800ac76c — confirm=0x4000 <- RAW SQUARE, cancel=0x8000 <- RAW CROSS
 * (preset-0 table @0x80073dbc[14..15] via FUN_80030444; wave-6 finding 4 — the old
 * "0x4000=CROSS/0x8000=SQUARE" labels came from the identity-feed misread). Port bits:
 * START=0x8, L1=0x400, R1=0x800, d-pad=0x00f0, CROSS=0x4000, SQUARE=0x8000. Raw reads use
 * the physical word directly; virtual reads go through re15_pad_virtual_word().
 *
 * SEs: FUN_80045024(bank<<24 | id<<16) — every SE in this flow is bank 4 (CORE00 resident)
 * -> re15_audio_core_se(id). The tab-select FSM itself is SILENT (EXE-wide jal scan:
 * zero SE sites in 0x8004974c-0x80049a58 / FUN_80046540 — spec fact).
 */
#include <string.h>
#include "re15_menu.h"
#include "re15_inv_screen.h"    /* g_inv_screen — the original's 25xx screen registers */
#include "re15_inv_ui.h"        /* wave 5: the embedded EXE blob — combine pair lists
                                 * @0x80074C88.. + per-item prop table @0x80074DA8
                                 * (matcher FUN_8004e900 / executor FUN_8004e054 data) */
#include "re15_inventory.h"     /* g_inv, equip slot 25c8, compaction FUN_8004dadc */
#include "re15_item_use.h"      /* heal classifier gate + applier table @0x80010fbc (wave 3) */
#include "re15_actor.h"         /* g_actors[PLAYER].hp — the heal 0x2f direct write */
#include "re15_player.h"        /* RE15_PAD_BIT_* */
#include "re15_engine.h"        /* re15_pad_virtual_word (wave-6 finding 4) */
#include "re15_damage.h"        /* re15_player_set_equipped_weapon (close-phase commit) */
#include "re15_audio.h"         /* re15_audio_core_se (SE bank 4) + prime_weapon */
#include "re15_fade.h"          /* the byte-true fade channel engine (FUN_800217b0 family) */
#include "re15_scd.h"           /* g_scd.message_fsm_active — the stage-1 message-idle gate */
#include "re15_room.h"          /* g_current_room_id — the MAP per-stage init reads the
                                 * stage/room registers lh DAT_800b0fe0/0fe2 (@0x8004997c
                                 * / FUN_8004b568 head) */
#include "re15_itembox.h"       /* ITEM BOX subscreen (substate 4 [DESIGN] — entered
                                 * ONLY from the box AOT via re15_menu_request_box,
                                 * never from the START tab select; itembox_spec.md §6) */

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
static uint8_t s_box_target = 0;   /* ITEM BOX open request [DESIGN]: the task spawns
                                    * into substate 4 instead of the tab select. The
                                    * open/close freeze+fade path is the SHARED stage
                                    * FSM (the save-phone precedent: world-side
                                    * trigger, byte-true menu transition mechanics). */

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
/* MAP entry (both entry paths land at the dispatch @0x8004997c-98: per-stage init     */
/* call [0x80074c0c + stage*4] + CD-load task spawn 0x80029a98(2, 0x8004c328)          */
/* @0x8004999c-a8; the loader reads file id u16 @0x80074c4c[260e] -> 0x801a8000 —      */
/* port: the PC rasterizer loads the PIX synchronously off g_inv_screen.map_page,      */
/* the CD-busy poll @0x8004c184-90 never blocks).                                      */
/* ---------------------------------------------------------------------------------- */
static void map_entry(void)
{
    unsigned rid = g_current_room_id;
    /* stage = lh DAT_800b0fe0 (0-based; ROOM1140 -> 0), room = lh DAT_800b0fe2
     * (ROOM1140 -> 0x14) — the room-setup writer @0x8001d808 derives them from the
     * room id nibbles. */
    re15_inv_map_stage_init((int)((rid >> 12) & 0xfu) - 1, (int)((rid >> 4) & 0xffu));
    g_inv_screen.map_room = re15_inv_map_room();
    g_inv_screen.map_page = re15_inv_map_page();
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
                                                 * confirm path's common resets
                                                 * (25bd/be/c2/c3 NOT reset) */
        map_entry();                            /* j 0x8004997c @0x8004982c = the same
                                                 * init+CD-load dispatch as the confirm */
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

    /* CONFIRM = VIRTUAL 0x4000 (@0x800498c0-cc reads DAT_800ac76c) <- RAW SQUARE per
     * the preset-0 remap @0x80073dbc[14] (wave-6 finding 4: the previous physical-
     * CROSS bind came from the identity-feed misread). NO sound effect on any confirm
     * path (spec fact: zero FUN_80045024 sites in this FSM). */
    if (re15_pad_virtual_word(pressed) & 0x4000) {
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
            case 1:                             /* MAP: 25c1=1 @0x80049968-78 */
                s_substate = 1;
                map_entry();                    /* dispatch @0x8004997c-98 + spawn
                                                 * @0x8004999c-a8 */
                return;
            case 2:                             /* EXIT @0x800499b4-c4 */
                g_inv_screen.highlight = 0;     /* sb zero,13(a0) = 25ca */
                s_phase = 2;                    /* 25bf++ -> close phase */
                return;
            case 3: s_substate = 2; return;     /* FILE: 25c1=2 @0x800499c8-cc */
            }
        }
        /* tab==4: falls through to the cancel poll (@0x800498d4 beq -> 0x800499d0) */
    }

    /* CANCEL poll: raw START edge (lhu 0x800ac762 & 0x800 @0x800499d4-dc) OR VIRTUAL
     * cancel 0x8000 (lw 0x800ac76c @0x800499ec) <- RAW CROSS (@0x80073dbc[15]) ->
     * 25bf++ (@0x800499d0-a18). No SE, no 25ca write. */
    if ((pressed & RE15_PAD_BIT_START) || (re15_pad_virtual_word(pressed) & 0x8000))
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

    /* CONFIRM: VIRTUAL 0x4000 (@0x800c6470-78 reads DAT_800ac76c) <- RAW SQUARE
     * (@0x80073dbc[14], wave-6 finding 4). SE(4,6) plays BEFORE the id check
     * (@0x800c6484) — also on an empty cell. */
    if (re15_pad_virtual_word(pressed) & 0x4000) {
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
    } else if (re15_pad_virtual_word(pressed) & 0x8000) {
        /* CANCEL: VIRTUAL 0x8000 @0x800c6538-58 <- RAW CROSS (@0x80073dbc[15]):
         * 25c2=2 (set BEFORE the SE) + SE(4,5). */
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

    /* CONFIRM: VIRTUAL 0x4000 @0x8004a504-14 (lw 0x800ac76c @0x8004a508) <- RAW
     * SQUARE (@0x80073dbc[14], wave-6 finding 4) -> SE(4,6) @0x8004a51c + jr via table
     * @0x8004a73c[25d6]: [0]->25c2=5 (USE @0x8004a550), [1]->25d6=0 + 25c2=9 (CHECK
     * @0x8004a558-64), [2]->25c2=8 (EXIT @0x8004a568-6c), [3]->25c2=7 (EXCHANGE
     * @0x8004a570), [4]-> the cancel check @0x8004a648 (guard for the CHECK flow's
     * 25d6 byte reuse). */
    if (re15_pad_virtual_word(pressed) & 0x4000) {
        se4(6);
        switch (g_inv_screen.action_dir) {
        case 0: g_inv_screen.item_state = 5; return;
        case 1: g_inv_screen.action_dir = 0; g_inv_screen.item_state = 9; return;
        case 2: g_inv_screen.item_state = 8; return;
        case 3: g_inv_screen.item_state = 7; return;
        default: break;                        /* [4] falls to the cancel check */
        }
    }
    /* CANCEL: VIRTUAL 0x8000 @0x8004a648-58 <- RAW CROSS (@0x80073dbc[15]) ->
     * SE(4,5) @0x8004a660 + 25c2=6 (slide the cluster back out, return to GRID). */
    if (re15_pad_virtual_word(pressed) & 0x8000) {
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

/* ---- msg-slice VM (waves 3+4) — FUN_80028134 states 1/2/5 for the desc bank -------
 * Open = FUN_80027e68(a0 = y<<16|x, a1=0x8400, a2=entry, a3): 8534/36 = (x,y), string
 * = desc bank @0x800c50de + u16[bank + entry*2] (a1&0xc00==0x400 path), 8520:=0x80
 * (active). Wave-3 caller: cant-use (0x00a80018, entry 0) @0x8004b2d8; wave-4 caller:
 * CHECK desc (0x00a00013, entry = item id) @0x800c6728-3c. Ticked per RENDERED frame
 * by the msg VM FUN_80028134 (single caller = the present-chain fn FUN_80010000 via
 * FUN_800280b4 — runs while task 0 is suspended); the port ticks it once per menu
 * frame at the end of menu_task_step (same 1-tick/frame cadence).
 * Typewriter (status screen, DAT_800b5456==0 -> bVar2=1 @FUN_80028134 case 0): first
 * glyph after 8525 = 1<<1 = 2 frames, then 8524 = 2<<1 = 4 frames/glyph; ONE
 * tick-code (0x00 space or glyph 0x0c..0xf7, LAB_80028434) per expiry, controls
 * processed for free in the same expiry (the joined_r0x80028258 loop). Controls in
 * this bank (generator-asserted census): 01 00 -> VM state 5 PRESS-WAIT; 02 00 ->
 * VM state 2 PAGE-WAIT; 08 = newline (emitter-side only). FAST-FORWARD stays
 * structurally impossible: the held-CROSS FF @0x80028220-30 needs 8524 < 4, but the
 * menu runs 8524 = 4 — even with the CHECK flow's 8522:=1 (@0x800c6744-48) the
 * cadence is IDENTICAL (the 8525==1 early-advance @0x80028228 lands on the same
 * frames), so 8522 is modeled as a no-op.
 * PAGE-WAIT (state 2 @FUN_80028134 case 2): blink countdown 8525-- per frame, arrow
 * drawn while (8525 & 0x18<<1 = 0x30) != 0; virtual EDGE & 0xc000 -> input eat
 * ac768:=0xffff, state 1, display start 8528 := current 852c (page restart),
 * 8525 := 1<<1 = 2. DISMISS (state 5 @0x8002868c-86d0): virtual EDGE & 0xc000 ->
 * input eat, 8520 &= 0x7f. NO auto-dismiss, NO SE inside the VM (the CHECK FSM
 * plays SE(4,5) itself on the msg-gone poll @0x800c6798). */
static uint8_t  s_msg_active = 0;   /* DAT_800b8520 bit 0x80 */
static uint8_t  s_msg_state  = 0;   /* DAT_800b8521: 1 typing / 2 page-wait / 5 press-wait */
static uint8_t  s_msg_count  = 0;   /* DAT_800b8525 countdown (typing + blink) */
static uint16_t s_msg_cur    = 0;   /* DAT_800b852c - entry base (next unrevealed byte) */

int re15_menu_msg_active(void) { return s_msg_active; }

static void msg_open(int entry, int x, int y)
{
    s_msg_active = 1;              /* 8520 := 0x80 */
    s_msg_state  = 1;              /* case 0 -> 1 on the first tick (falls through) */
    s_msg_count  = 2;              /* 8525 = 1<<1 (first glyph after 2 frames) */
    s_msg_cur    = 0;
    g_inv_screen.msg_entry    = (int16_t)entry;
    g_inv_screen.msg_x        = (int16_t)x;    /* DAT_800b8534 */
    g_inv_screen.msg_y        = (int16_t)y;    /* DAT_800b8536 */
    g_inv_screen.msg_page_off = 0;             /* DAT_800b8528 = string start */
    g_inv_screen.msg_reveal   = 0;
    g_inv_screen.msg_arrow    = 0;
}

static void msg_vm_tick(uint16_t pressed)
{
    const uint8_t *s;
    if (!s_msg_active) { g_inv_screen.msg_reveal = 0; g_inv_screen.msg_arrow = 0; return; }
    s = re15_inv_desc_entry(g_inv_screen.msg_entry);
    if (!s) { s_msg_active = 0; g_inv_screen.msg_reveal = 0; return; }
    switch (s_msg_state) {
    case 1:                                   /* typewriter (case 1 @0x800281dc+) */
        s_msg_count--;                        /* 8525-- (delay-slot store @0x800281f8) */
        if ((int8_t)s_msg_count >= 1) break;  /* advance when (char)8525 < 1 */
        for (;;) {                            /* LAB_80028250 + control loop */
            uint8_t c = s[s_msg_cur];
            if (c == 0x01) { s_msg_state = 5; break; }        /* 01 00 -> state 5 */
            if (c == 0x02) {                                  /* 02 00 -> state 2; the
                                                               * VM pointer advances
                                                               * PAST the control */
                s_msg_cur += 2;
                s_msg_state = 2;
                break;
            }
            if (c == 0x08) { s_msg_cur++; continue; }         /* newline: free */
            s_msg_cur++;                                      /* space/glyph = 1 tick */
            g_inv_screen.msg_reveal++;
            s_msg_count = 4;                                  /* reload 8524 = 2<<1 */
            break;
        }
        break;
    case 2:                                   /* PAGE-WAIT */
        /* VIRTUAL 76c & 0xc000 = either action button (set-equal to raw CROSS|SQUARE
         * under preset 0 @0x80073dbc[14..15] — routed via the virtual word anyway). */
        if (re15_pad_virtual_word(pressed) & 0xc000) {
            s_msg_state = 1;
            g_inv_screen.msg_page_off = s_msg_cur;   /* 8528 := 852c (page restart) */
            g_inv_screen.msg_reveal = 0;
            g_inv_screen.msg_arrow = 0;
            s_msg_count = 2;                         /* 8525 := 1<<1 */
        } else {
            s_msg_count--;                           /* blink countdown (u8 wrap) */
            g_inv_screen.msg_arrow = (uint8_t)((s_msg_count & 0x30) != 0);
        }
        break;
    case 5:                                   /* PRESS-WAIT */
        if (re15_pad_virtual_word(pressed) & 0xc000) {   /* VIRTUAL 76c & 0xc000 */
            s_msg_active = 0;                 /* 8520 &= 0x7f @0x800286c4 */
            g_inv_screen.msg_reveal = 0;
            g_inv_screen.msg_arrow = 0;
            g_inv_screen.msg_entry = -1;
        }
        break;
    default:
        s_msg_active = 0;
        break;
    }
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
                                                   * FUN_80027e68(0x00a80018,0x8400,0,0)
                                                   * = desc-bank entry 0 at (0x18,0xa8) */
        msg_open(0, 0x18, 0xa8);
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

/* ---------------------------------------------------------------------------------- */
/* ITEM state 9 (CHECK/EXAMINE) — WAVE 4: per-frame jal FUN_800c6630 @0x8004a6e8.      */
/* DEBUG.BIN module (maps 1:1 @0x800c0000; file==RAM byte-verified vs mzd_inv_open.sav */
/* for 0x800c6630+0x300 / 0x800c6220 / 0x800c0258 this wave). FUN_800c6630 dispatches  */
/* on DAT_800b25d6 (lbu @0x800c6640 — the command-cursor byte DOUBLES as the CHECK     */
/* sub-state; the command dispatch [1] pre-writes 25d6=0 @0x8004a55c) via the 5-entry  */
/* jump table @0x800c6864 = {0x800c6664, 0x800c6704, 0x800c6760, 0x800c67b8,           */
/* 0x800c67ec}. Byte-true byproduct: the g5 command-highlight prim keeps drawing at    */
/* button-table[25d6] while 25d6 cycles 0..4 (offscreen with the cluster at 25ee=264   */
/* until the state-4 slide-in, where it briefly rides the center-logo cell 4).         */
/* ---------------------------------------------------------------------------------- */
static void state9_check(void)
{
    switch (g_inv_screen.action_dir) {            /* 25d6 = the CHECK sub-state */
    case 0:
        /* @0x800c6664-66bc: while 25ee < 251 (slti 251 @0x800c667c): the four panel-x
         * registers 25d8/25dc/25e4/25f0 -= 36 (lhu 0/4/12/24(t0=0x800b25d8), addiu
         * -36, sh @0x800c6688-66b4) AND 25ee += 14 (delay-slot sh @0x800c66bc) — the
         * left half of the screen slides off while the command cluster slides out.
         * Else @0x800c66c0-6700: id = helper 0x800c6600 (cursor==0xA -> 1, else
         * inv[25bd*4].id @0x800c6604-24); jal 0x800c6878(id) = ITPS block load: CD
         * sectors (base 0x1740 = {u16@0x8006f590,u8@0x8006f592}) + id*6 @0x800c68b0-bc
         * -> buffer *(0x800ac77c), then TIM upload 0x800c0258 (OpenTIM/ReadTIM
         * 0x8006bbbc/bbcc + LoadImage 0x80068c88 of the block's EMBEDDED crect/prect
         * + DrawSync @0x800c0270-2ac); photo struct @0x800c6220 := {1, -207, 26}
         * (@0x800c66d0-66ec); 25d6 := 1 (@0x800c66f0-f8). */
        if (g_inv_screen.act_base_y < 251) {
            g_inv_screen.arms_x   -= 36;          /* 25d8 @0x800c6698/66a8 */
            g_inv_screen.equip_x  -= 36;          /* 25dc @0x800c669c/66ac */
            g_inv_screen.cond_x   -= 36;          /* 25e4 @0x800c66a0/66b0 */
            g_inv_screen.idcard_x -= 36;          /* 25f0 @0x800c66a4/66b4 */
            g_inv_screen.act_base_y += 14;        /* 25ee @0x800c6684/66bc */
        } else {
            uint8_t cur = g_inv_screen.item_cursor;
            uint8_t id = (cur == 0x0a) ? 1 : g_inv.slots[cur].id;  /* 0x800c6600 */
            g_inv_screen.exam_item = id;
            g_inv_screen.exam_upload_seq++;       /* the 0x800c6878 load+upload event
                                                   * (rasterizer honors the embedded
                                                   * rects -> id-0x24 stale quirk)   */
            g_inv_screen.exam_visible = 1;        /* sh 1 @0x800c66e4 */
            g_inv_screen.exam_x = -207;           /* addiu -207 @0x800c66d4/66e8 */
            g_inv_screen.exam_y = 26;             /* ori 0x1a @0x800c66d8/66ec */
            g_inv_screen.action_dir = 1;          /* sb 1 -> 25d6 @0x800c66f0-f8 */
        }
        break;
    case 1:
        /* @0x800c6704-675c: x += 22 EVERY frame (delay-slot sh @0x800c671c); when the
         * new x == 13 (ori 0xd @0x800c6710; -207 + 22*10 = 13 -> 10 frames): open the
         * desc message FUN_80027e68(a0=0x00a00013 -> (0x13,0xa0), a1=0x8400, a2=id
         * from helper @0x800c6720-24, a3=0xff000000) @0x800c6728-3c; then 8522 := 1
         * (@0x800c6740-48 — FF enable, cadence-dead at menu pacing, see the VM header);
         * 25d6 := 2 (@0x800c674c-54). */
        g_inv_screen.exam_x += 22;
        if (g_inv_screen.exam_x == 13) {
            msg_open(g_inv_screen.exam_item, 0x13, 0xa0);
            g_inv_screen.action_dir = 2;
        }
        break;
    case 2:
        /* @0x800c6760-67b4: while (8520 & 0x80) stay (@0x800c6760-70); on msg gone:
         * SE(4,5) (lui a0,0x405 @0x800c6798 + jal 0x80045024) + 25d6 := 3
         * (@0x800c67a4-b0). The virtual-edge block @0x800c6780-6790 (lw ac76c &
         * 0xc000) is DEAD code — skipped by `j 0x800c6798` @0x800c6778. */
        if (s_msg_active) break;
        se4(5);
        g_inv_screen.action_dir = 3;
        break;
    case 3:
        /* @0x800c67b8-67e8: x -= 22 EVERY frame (delay-slot sh @0x800c67d0); when the
         * new x == -207 (@0x800c67c4-cc): photo visible := 0 (sh zero @0x800c67d4) +
         * 25d6 := 4 (@0x800c67d8-e0). */
        g_inv_screen.exam_x -= 22;
        if (g_inv_screen.exam_x == -207) {
            g_inv_screen.exam_visible = 0;
            g_inv_screen.action_dir = 4;
        }
        break;
    case 4:
    default:
        /* @0x800c67ec-6850: if 25ee == 0xa6=166 (@0x800c67f8-fc): EXIT — 25c2 := 4
         * (command stage, sb @0x800c6844) + 25d6 := 1 (CHECK re-highlighted, sb
         * @0x800c6850); else the four x regs += 36 (@0x800c6804-6830) and 25ee -= 14
         * (delay-slot addiu -14 @0x800c6800 / sh @0x800c6838). */
        if (g_inv_screen.act_base_y == 0xa6) {
            g_inv_screen.item_state = 4;
            g_inv_screen.action_dir = 1;
        } else {
            g_inv_screen.arms_x   += 36;
            g_inv_screen.equip_x  += 36;
            g_inv_screen.cond_x   += 36;
            g_inv_screen.idcard_x += 36;
            g_inv_screen.act_base_y -= 14;
        }
        break;
    }
}

/* ---------------------------------------------------------------------------------- */
/* ITEM state 7 (EXCHANGE/combine) — WAVE 5, spec shots/inv_wave5_spec.md.             */
/* Byte-true state 7 = per-frame jal FUN_8004b33c (@0x8004a6ac): dispatcher            */
/* @0x8004b33c-64 does jalr PTR[0x80074c44 + 25c3*4]; table @0x80074c44 =              */
/* {[0]=0x8004b37c select, [1]=0x8004b408 result-anim walker} (bytes '7c b3 04 80     */
/* 08 b4 04 80'). c3 is 0 on entry (terminal invariant), so the select sub runs first. */
/* All pair/prop data is read from the EMBEDDED EXE blob (re15_inv_ui.h) at its        */
/* original addresses — nothing is re-typed.                                           */
/* ---------------------------------------------------------------------------------- */
#define X_PROP_TBL 0x80074DA8u   /* per-item prop table, stride 12: {cap u32 @+0,
                                  * pair_ptr u32 @+4, kind u8 @+8, pair_count u8 @+9}
                                  * (matcher reads +4/+9 @0x8004e9d8/@0x8004e9e8;
                                  * executor reads +0 @0x8004e338/@0x8004e444) */
static uint8_t  xu8(uint32_t addr) { return *RE15_INV_PTR(addr); }
static uint32_t xu32(uint32_t addr)
{
    const unsigned char *p = RE15_INV_PTR(addr);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/* Matcher outputs (globals in the original, read back by the executor):
 * s_x_result = DAT_800b25d5 (sb @0x8004ea18), s_x_pic = DAT_800b260c (sb @0x8004ea28). */
static uint8_t s_x_result = 0;
static uint8_t s_x_pic = 0;

/* Second-cursor mover FUN_80048904 @0x80048904 (jal from the select sub @0x8004b384).
 * Gated on the D-pad auto-repeat tick (bgez DAT_800aca38 @0x80048910 returns when
 * bit 31 — the FUN_80030444 fire flag — is clear; port mirror = s_rep_fire), then reads
 * the HELD word DAT_800ac760 (@0x8004891c). Pure 2-column bounds moves on 25be, NO
 * item/occupancy restriction and NO SE (zero jal in the fn). The reject paths differ:
 * RIGHT at the cap bound falls THROUGH to the LEFT check (@0x8004894c beq ->
 * 0x80048970) and LEFT at 0 falls to the DOWN check (@0x80048998 beq -> 0x800489b4),
 * while the odd/even rejects consume the frame (@0x80048954/@0x800489a0). */
static void second_cursor_move(uint16_t held)
{
    uint8_t *cur = &g_inv_screen.second_cursor;
    if (!s_rep_fire) return;                     /* bgez aca38 @0x80048910 */
    if (held & RE15_PAD_BIT_RIGHT) {             /* andi 0x2000 @0x80048924 */
        if (*cur < CAPACITY - 1) {               /* slt cur,cap-1 @0x80048948 (cap =
                                                  * lbu 0x800b0fbc @0x8004893c) */
            if (*cur & 1) return;                /* odd rejects+consumes @0x80048950-54 */
            (*cur)++;                            /* @0x80048964-6c */
            return;
        }
        /* cur >= cap-1: falls through to LEFT (@0x8004894c) */
    }
    if (held & RE15_PAD_BIT_LEFT) {              /* andi 0x8000 @0x8004897c */
        if (*cur != 0) {                         /* 0 falls through to DOWN @0x80048998 */
            if (!(*cur & 1)) return;             /* even rejects+consumes @0x800489a0 */
            (*cur)--;                            /* @0x800489a8-b0 + sb @0x80048a38 */
            return;
        }
    }
    if (held & RE15_PAD_BIT_DOWN) {              /* andi 0x4000 @0x800489c0 */
        if (*cur == CAPACITY - 2) return;        /* beq cap-2 @0x800489e4 */
        if (*cur == CAPACITY - 1) return;        /* beq cap-1 @0x800489ec */
        *cur += 2;                               /* @0x800489f4-a04 */
        return;
    }
    if (held & RE15_PAD_BIT_UP) {                /* delay andi 0x1000 @0x800489c8;
                                                  * beq @0x80048a08 */
        if (*cur < 2) return;                    /* sltiu 2 @0x80048a20 */
        *cur -= 2;                               /* addiu 254 @0x80048a34 + sb @0x80048a38 */
    }
}

/* Matcher FUN_8004e900 @0x8004e900: kind-byte normalization (+2==2 -> slot-1
 * @0x8004e910-38 for 25bd, @0x8004e948-7c for 25be), same-slot reject (@0x8004e98c),
 * empty-partner reject (idB==0 @0x8004e9bc), prop-row lookup by the FIRST cursor's
 * item id (idA*12: pair_count @+9 lbu 0x80074db1+ @0x8004e9d8, pair_ptr @+4 lw
 * 0x80074dac+ @0x8004e9e8; count==0 -> 0 @0x8004e9ec; first partner byte 0 -> 0
 * @0x8004e9f4-ea04), then a linear 4-byte {partner,result,action,pic} scan for
 * partner==idB (@0x8004ea38-5c). On match: result -> 25d5 (@0x8004ea18), pic -> 260c
 * (@0x8004ea28), RETURN the action byte (@0x8004ea2c). No item-class check. */
static int exchange_match(void)
{
    int a = g_inv_screen.item_cursor;
    int b = g_inv_screen.second_cursor;
    uint8_t idA, idB, cnt, i;
    uint32_t plist;
    if (g_inv.slots[a].flags == 2) a -= 1;       /* @0x8004e910-38 */
    if (g_inv.slots[b].flags == 2) b -= 1;       /* @0x8004e948-7c */
    if (a == b) return 0;                        /* beq @0x8004e98c */
    idB = g_inv.slots[b].id;                     /* lbu 0x800b10ac+b*4 @0x8004e9a8 */
    idA = g_inv.slots[a].id;                     /* lbu 0x800b10ac+a*4 @0x8004e9b8 */
    if (idB == 0) return 0;                      /* beq @0x8004e9bc */
    cnt   = xu8(X_PROP_TBL + (uint32_t)idA * 12u + 9u);
    plist = xu32(X_PROP_TBL + (uint32_t)idA * 12u + 4u);   /* PSX address in the blob */
    if (cnt == 0) return 0;                      /* beq @0x8004e9ec */
    if (xu8(plist) == 0) return 0;               /* @0x8004e9f4-ea04 */
    for (i = 0; i < cnt; i++) {                  /* @0x8004ea38-5c, stride 4 */
        if (xu8(plist + (uint32_t)i * 4u) == idB) {
            s_x_result = xu8(plist + (uint32_t)i * 4u + 1u);  /* -> 25d5 @0x8004ea18 */
            s_x_pic    = xu8(plist + (uint32_t)i * 4u + 3u);  /* -> 260c @0x8004ea28 */
            return xu8(plist + (uint32_t)i * 4u + 2u);        /* action  @0x8004ea2c */
        }
    }
    return 0;                                    /* fall-out @0x8004ea60 */
}

/* Executor FUN_8004e054 @0x8004e054 — the six-action pair engine. Head registers
 * (@0x8004e054-160): s2 = id[RAW 25bd], s1 = id[RAW 25be] (@0x8004e0a4/@0x8004e0c4);
 * s4/s3 = kind-normalized 25bd/25be (@0x8004e0c8-11c); total s0 = qty[s3n] + qty[s4n]
 * (@0x8004e13c-160). pic!=0 && action!=0 -> CD-load file 0x19 to 0x801a0000
 * (jal 0x80013b60 @0x8004e174; id 0x19 = 16800 bytes @0x8006f43c+0x19*8 == exactly
 * MIXITEM.PIX — platform side loads the file lazily, engine no-op). Then jr
 * @0x80011298[action] (sltiu 7 @0x8004e180; >=7 returns 0). RETURNS the action byte
 * (every tail is `andi v0,s5,0xff` -> the caller's !=0 gate == action!=0). */
/* Shared reload body (the action-2 @0x8004e320 / action-3 @0x8004e42c mirror pair;
 * action 5 jumps into exactly these bodies @0x8004e670/@0x8004e678). dst = the merge
 * target slot, src = the emptied slot, cap_id = the RAW id whose prop cap gates. */
static void x_reload(int dst, int src, uint8_t cap_id, int total)
{
    uint8_t cap = xu8(X_PROP_TBL + (uint32_t)cap_id * 12u);   /* prop[+0] lbu
                                                  * @0x8004e338 / @0x8004e444 */
    if (cap < total) {                           /* sltu @0x8004e340 / @0x8004e44c ->
                                                  * PARTIAL @0x8004e3e4 / @0x8004e4f0 */
        g_inv.slots[dst].qty = cap;              /* @0x8004e3f4 / @0x8004e500 */
        g_inv.slots[src].qty = (uint8_t)(total - cap);   /* @0x8004e410-420 /
                                                  * @0x8004e51c-52c (ids UNCHANGED)      */
    } else {                                     /* FULL @0x8004e34c / @0x8004e458 */
        if (g_inv.slots[dst].id != s_x_result)   /* GLOCK 18 -> 0x04 quirk @0x80074c99:
                                                  * the id byte changes with NO icon call
                                                  * in this branch (no jal in 0x8004e34c-
                                                  * e3e0) -> the cell art freezes         */
            re15_inv_icon_freeze_tile(dst, g_inv.slots[dst].id);
        g_inv.slots[dst].id = s_x_result;        /* @0x8004e370 / @0x8004e47c */
        g_inv.slots[dst].qty = (uint8_t)(g_inv.slots[dst].qty
                                         + g_inv.slots[src].qty);  /* @0x8004e398-3a8 /
                                                  * @0x8004e4a4-4b4 */
        g_inv.slots[src].id = 0;                 /* @0x8004e3b8 / @0x8004e4c4 */
        g_inv.slots[src].qty = 0;                /* @0x8004e3c8 / @0x8004e4d4 */
        g_inv.slots[src].flags = 0;              /* @0x8004e3d8 / @0x8004e4e4 */
        re15_inv_icon_blank(src);                /* j 0x8004e310 -> jal 0x8004947c (a0 =
                                                  * the cleared slot @0x8004e3e0/@0x8004e4ec) */
    }
}

static int exchange_exec(int action)
{
    int an = g_inv_screen.item_cursor;           /* s4 = norm 25bd */
    int bn = g_inv_screen.second_cursor;         /* s3 = norm 25be */
    uint8_t idA_raw = g_inv.slots[an].id;        /* s2 */
    uint8_t idB_raw = g_inv.slots[bn].id;        /* s1 */
    int total;
    if (g_inv.slots[an].flags == 2) an -= 1;     /* @0x8004e0c8-e0dc */
    if (g_inv.slots[bn].flags == 2) bn -= 1;     /* @0x8004e100-11c */
    total = (int)g_inv.slots[bn].qty + (int)g_inv.slots[an].qty;  /* addu s0 @0x8004e160 */

    switch (action & 0xff) {                     /* jump table @0x80011298 [0..6] */
    case 0:                                      /* [0] @0x8004e318: return action (=0) */
    default:
        break;
    case 1: {                                    /* [1] @0x8004e1ac MIX-MERGE */
        int lower = (an < bn) ? an : bn;         /* sltu @0x8004e1b4 picks the branch */
        int other = (an < bn) ? bn : an;
        g_inv.slots[lower].qty = (uint8_t)(g_inv.slots[lower].qty
                                           + g_inv.slots[other].qty);  /* @0x8004e1e8-1f8
                                                  * / @0x8004e27c-28c */
        g_inv.slots[lower].id = s_x_result;      /* 25d5 -> id[lower] @0x8004e214/@0x8004e2a8 */
        re15_inv_icon_mix_upload(lower, s_x_pic);/* jal 0x800492b8(lower,0,buf+(pic-1)*1200)
                                                  * @0x8004e240/@0x8004e2d4 */
        g_inv.slots[other].id = 0;               /* @0x8004e2ec */
        g_inv.slots[other].qty = 0;              /* @0x8004e2fc */
        g_inv.slots[other].flags = 0;            /* @0x8004e30c */
        re15_inv_icon_blank(other);              /* jal 0x8004947c @0x8004e310 (a0 = the
                                                  * cleared slot @0x8004e24c/@0x8004e2dc) */
        break;
    }
    case 2:                                      /* [2] @0x8004e320 RELOAD into A (cap
                                                  * from prop[s2] = id at RAW 25bd)      */
        x_reload(an, bn, idA_raw, total);
        break;
    case 3:                                      /* [3] @0x8004e42c RELOAD into B (mirror;
                                                  * cap from prop[s1] = id at RAW 25be)  */
        x_reload(bn, an, idB_raw, total);
        break;
    case 4: {                                    /* [4] @0x8004e538 SWAP+TRANSFORM —
                                                  * DORMANT: the only action-4 pairs are
                                                  * the GL orphans @0x80074cb8/bc whose
                                                  * owners 0x0f-0x11 have pair_count==0
                                                  * (prop rows read from the blob) —
                                                  * unreachable via the shipped data.    */
        uint8_t sid = g_inv.slots[bn].id;        /* old B triple saved sp+16/17/18
                                                  * @0x8004e54c-588 */
        uint8_t sqty = g_inv.slots[bn].qty;
        uint8_t skind = g_inv.slots[bn].flags;
        g_inv.slots[bn].id = s_x_result;         /* @0x8004e598 */
        g_inv.slots[bn].qty = g_inv.slots[an].qty;    /* @0x8004e5a8-5bc */
        g_inv.slots[bn].flags = g_inv.slots[an].flags;/* @0x8004e5cc-5e0 */
        re15_inv_icon_mix_upload(bn, s_x_pic);   /* jal 0x800492b8 @0x8004e60c */
        /* A <- old B triple with NO icon op for A -> A's cell art stays (freeze) */
        re15_inv_icon_freeze_tile(an, idA_raw);
        g_inv.slots[an].id = sid;                /* @0x8004e628 */
        g_inv.slots[an].qty = sqty;              /* @0x8004e640 */
        g_inv.slots[an].flags = skind;           /* @0x8004e658 */
        break;
    }
    case 5:                                      /* [5] @0x8004e664 SELF-STACK: delegates
                                                  * to the WHOLE action-2 body when norm A
                                                  * is the lower slot (bne @0x8004e670 ->
                                                  * 0x8004e324, delay slot replicates the
                                                  * first action-2 instr; cap from
                                                  * prop[s2]) else the action-3 body
                                                  * (j @0x8004e678 -> 0x8004e430, cap
                                                  * from prop[s1]). Return stays 5.      */
        if (an < bn) x_reload(an, bn, idA_raw, total);
        else         x_reload(bn, an, idB_raw, total);
        break;
    case 6: {                                    /* [6] @0x8004e680 CRAFTING */
        int lo, hi;
        uint8_t qlo, qhi;
        if (bn < an) { lo = bn; hi = an; }       /* swap s4<->s3 @0x8004e688-698 */
        else         { lo = an; hi = bn; }
        qlo = g_inv.slots[lo].qty;               /* lbu @0x8004e6b8 */
        qhi = g_inv.slots[hi].qty;               /* lbu @0x8004e6c8 */
        if (qlo == qhi) {                        /* bne @0x8004e6d0 -> EQUAL @0x8004e6d8 */
            g_inv.slots[lo].id = s_x_result;     /* @0x8004e6f0 (qty[lo] UNCHANGED) */
            re15_inv_icon_mix_upload(lo, s_x_pic);   /* jal 0x800492b8 @0x8004e71c */
            g_inv.slots[hi].id = 0;              /* @0x8004e730 */
            g_inv.slots[hi].qty = 0;             /* @0x8004e740 */
            g_inv.slots[hi].flags = 0;           /* @0x8004e750 */
            re15_inv_icon_blank(hi);             /* jal 0x8004947c @0x8004e754 (a0=s3) */
        } else if (qhi < qlo) {                  /* delay sltu @0x8004e6d4; taken branch
                                                  * @0x8004e76c: the LOWER stack is bigger */
            g_inv.slots[hi].id = g_inv.slots[lo].id;      /* leftover takes the source id
                                                  * @0x8004e778/@0x8004e798 */
            g_inv.slots[hi].qty = (uint8_t)(qlo - qhi);   /* @0x8004e7bc-7cc */
            re15_inv_icon_copy(lo, hi);          /* jal 0x80049390(lo,hi) @0x8004e7d0 —
                                                  * the leftover cell gets the source art */
            g_inv.slots[lo].qty = qhi;           /* s0 = the pre-overwrite qty[hi] (min)
                                                  * @0x8004e7b8/@0x8004e7f0 */
            g_inv.slots[lo].id = s_x_result;     /* @0x8004e800 */
            re15_inv_icon_mix_upload(lo, s_x_pic);   /* jal 0x800492b8 @0x8004e82c */
        } else {                                 /* qhi > qlo @0x8004e83c */
            g_inv.slots[lo].flags = 0;           /* sb zero kind[lo] @0x8004e858 — only
                                                  * this branch writes a kind (byte-true
                                                  * asymmetry, do not converge)          */
            g_inv.slots[lo].id = s_x_result;     /* @0x8004e868 (qty[lo] stays = min) */
            re15_inv_icon_mix_upload(lo, s_x_pic);   /* jal 0x800492b8 @0x8004e894 */
            g_inv.slots[hi].qty = (uint8_t)(qhi - qlo);   /* @0x8004e8a8-8d0 (id[hi]
                                                  * unchanged)                           */
        }
        break;
    }
    }
    return action & 0xff;                        /* tail @0x8004e8d8: v0 = andi s5,0xff */
}

/* State-7 sub [0] = select @0x8004b37c (fresh disasm this wave): per frame
 *   jal 0x80048904 (second-cursor mover) @0x8004b384;
 *   CANCEL first: lw DAT_800ac76c @0x8004b390; andi 0x8000 @0x8004b398; bne ->
 *     0x8004b3f0 with delay `ori v0,zero,0x6` @0x8004b3a0 -> sb 6 -> 25c2 @0x8004b3f0-f4
 *     (state 6 slide-out). NO SE anywhere in 0x8004b37c-b404 (zero jal 0x80045024 —
 *     the EXCHANGE cancel/confirm are SILENT, unlike the command stage's SE(4,5)/(4,6));
 *   CONFIRM: andi 0x4000 @0x8004b3a4; beq -> exit @0x8004b3a8; jal 0x8004e900 (matcher)
 *     @0x8004b3b0; jal 0x8004e054(a0 = action & 0xff) @0x8004b3b8-bc; the EXECUTOR's
 *     return (== the action, tail @0x8004e8d8) gates: !=0 -> jal 0x8004dadc (compaction)
 *     @0x8004b3cc, 25c4:=0 @0x8004b3dc, 25c3:=1 @0x8004b3e4 (result anim); ==0 (no
 *     pair / same-slot / empty) -> sb 6 -> 25c2 @0x8004b3c4-c8 + 0x8004b3f0-f4. */
/* ⛔ BEWUSSTE NACHRUESTUNG NACH RE2-VORBILD — KEINE byte-true RE1.5-Wiederherstellung.
 *
 * Nutzer-Auftrag 2026-08-27: "beim Combine von items fehlen Sound und Effekte. das liegt
 * daran das 1.5 noch eine beta ist. schaue wie das echte re2 das macht und rueste Sound und
 * Effekt nach."
 *
 * BEFUND, damit niemand das fuer eine Regression haelt:
 *   (a) RE1.5 ist hier WIRKLICH stumm. Der komplette Erfolgspfad @0x8004b37c..@0x8004b404
 *       enthaelt genau vier `jal` — 0x80048904 (Cursor), 0x8004e900 (Matcher),
 *       0x8004e054 (Ausfuehrung), 0x8004dadc (Kompaktierung) — und KEINEN Aufruf von
 *       Se_on (FUN_80045024). Auch der Ergebnis-Walker @0x8004b408 hat null `jal`.
 *   (b) Der EFFEKT fehlt dagegen NICHT: RE1.5 hat einen eigenen 17-Frame-Puls
 *       (`sltiu v0,v1,0x11` @0x8004b414, Sprungtabelle @0x80010ff4 = 8x 0x8004b43c
 *       GROW / 8x 0x8004b4a8 SHRINK / 1x 0x8004b524 TERMINAL). Der ist im Port
 *       vollstaendig da (state7_result_anim unten, Jitter-Register in re15_inv_screen.c).
 *       RE2 hat an derselben Stelle stattdessen einen 10-Frame-Slide — ein ANDERER Effekt,
 *       kein zusaetzlicher. Es wird deshalb NUR der Sound nachgeruestet.
 *
 * RE2-VORBILD (info/re2leon/PSX.EXE, FUN_8006b358 = der Zwilling von 0x8004b33c;
 * selbst disassembliert):
 *     8006b57c: beq  a2,v0,0x8006b58c   ; Cursor unveraendert -> kein SE
 *     8006b580: lui  a0,0x404           ; DELAY-SLOT: Record 4 = Cursor
 *     8006b584: jal  0x8005ba28         ; Se_on
 *     8006b588: addu a1,zero,zero
 *     8006b5a0: jal  0x800695b0         ; Combine ausfuehren -> v0 = Ergebnis
 *     8006b5ac: andi v0,v0,0xff
 *     8006b5b0: beq  v0,zero,0x8006b5bc
 *     8006b5b4: lui  a0,0x407           ; DELAY-SLOT, immer: Record 7 = FEHLGESCHLAGEN
 *     8006b5b8: lui  a0,0x406           ; nur wenn != 0:     Record 6 = ERFOLG
 *     8006b5bc: jal  0x8005ba28
 *     8006b5cc: beq  v0,zero,0x8006b5ec ; CANCEL-Bit
 *     8006b5d0: lui  a0,0x405           ; Record 5 = Abbruch
 *     8006b5d4: jal  0x8005ba28
 *
 * WARUM DAS OHNE FREMD-ASSET GEHT: es sind RE1.5s EIGENE Bank-4-Records. Das Inventar
 * spielt sie an anderer Stelle laengst — Cursor 4 (@0x8004a478/4a0/4c8/4f0), Abbruch 5
 * (@0x8004a660), Bestaetigung 6 (@0x80033e54/@0x8004a51c). Record 7 ist in RE1.5s EXE
 * NIRGENDS referenziert (eigener Voll-Scan nach `lui a0,0x0407`: null Treffer), existiert
 * aber als gueltiger Eintrag in der eigenen Bank: SOUND/CORE00.EDH Byte 0x1c = 00 00 83 00
 * (der erste LEER-Record ist 11 @0x2c = ff ff ff ff). Es wird also nichts importiert. */
static void state7_select(uint16_t pressed, uint16_t held)
{
    uint8_t cur_before = g_inv_screen.second_cursor;
    second_cursor_move(held);                    /* jal 0x80048904 @0x8004b384 */
    if (g_inv_screen.second_cursor != cur_before)
        se4(4);                                  /* NACHGERUESTET: RE2 @0x8006b57c-88 */
    if (re15_pad_virtual_word(pressed) & 0x8000) {   /* VIRTUAL cancel BEFORE confirm
                                                      * @0x8004b398 <- RAW CROSS */
        se4(5);                                  /* NACHGERUESTET: RE2 @0x8006b5cc-d8 */
        g_inv_screen.item_state = 6;             /* @0x8004b3f0-f4 (im Original stumm) */
        return;
    }
    if (re15_pad_virtual_word(pressed) & 0x4000) {   /* VIRTUAL confirm @0x8004b3a4
                                                      * <- RAW SQUARE (@0x80073dbc[14]) */
        int action = exchange_match();           /* jal 0x8004e900 @0x8004b3b0 */
        int ret = exchange_exec(action);         /* jal 0x8004e054 @0x8004b3b8 */
        /* NACHGERUESTET, Reihenfolge wie RE2 (ausfuehren -> Ergebnis pruefen -> SE):
         * @0x8006b5b0 `beq v0,zero` -> Record 7, sonst Record 6. */
        se4(ret != 0 ? 6 : 7);
        if (ret != 0) {                          /* bne @0x8004b3c4 */
            re15_inv_compact();                  /* jal 0x8004dadc @0x8004b3cc (also
                                                  * shifts/blanks the icon cells)        */
            g_inv_screen.equipped_slot =         /* port mirror refresh (the compaction
                                                  * may shift the one 25c8 byte)         */
                (uint8_t)re15_inv_equipped_slot();
            s_c4 = 0;                            /* sb zero -> 25c4 @0x8004b3dc */
            s_c3 = 1;                            /* sb 1 -> 25c3 @0x8004b3e4 */
        } else {
            g_inv_screen.item_state = 6;         /* @0x8004b3f0-f4 */
        }
    }
}

/* State-7 sub [1] = result-anim walker @0x8004b408 (fresh disasm this wave): guard
 * sltiu c4,0x11 @0x8004b414; jr [0x80010ff4 + c4*4] — table bytes = [0..7]=0x8004b43c
 * GROW, [8..15]=0x8004b4a8 SHRINK, [16]=0x8004b524 TERMINAL (17 entries, 1 step =
 * 1 frame, each step increments c4 itself). */
static void state7_result_anim(void)
{
    if (s_c4 >= 0x11) return;                    /* sltiu 0x11 @0x8004b414 */
    if (s_c4 <= 7) {
        /* GROW @0x8004b43c-b4a4: d0+1 (sb @0x8004b47c), d1+1 (@0x8004b484),
         * d2-1 (@0x8004b48c), d3-1 (@0x8004b494), c4+1 (@0x8004b49c) */
        g_inv_screen.comb_d0++;
        g_inv_screen.comb_d1++;
        g_inv_screen.comb_d2--;
        g_inv_screen.comb_d3--;
        s_c4++;
    } else if (s_c4 <= 15) {
        /* SHRINK @0x8004b4a8-b520: 25be := 25bd (lbu @0x8004b4d4, sb @0x8004b4f0 —
         * the 2nd cursor snaps onto the cursor EVERY shrink frame), d0-1 (@0x8004b4f8),
         * d1-1 (@0x8004b500), d2+1 (@0x8004b508), d3+1 (@0x8004b510), c4+1 (@0x8004b518) */
        g_inv_screen.second_cursor = g_inv_screen.item_cursor;
        g_inv_screen.comb_d0--;
        g_inv_screen.comb_d1--;
        g_inv_screen.comb_d2++;
        g_inv_screen.comb_d3++;
        s_c4++;
    } else {
        /* TERMINAL @0x8004b524-b55c: 25c2:=6 (@0x8004b52c), c4:=0 (@0x8004b534),
         * c3:=0 (@0x8004b53c), d0..d3:=0 (@0x8004b544/b54c/b554/b55c) */
        g_inv_screen.item_state = 6;
        s_c4 = 0; s_c3 = 0;
        g_inv_screen.comb_d0 = 0; g_inv_screen.comb_d1 = 0;
        g_inv_screen.comb_d2 = 0; g_inv_screen.comb_d3 = 0;
    }
}

/* The per-frame state-7 body (jal FUN_8004b33c @0x8004a6ac): jalr PTR[0x80074c44 +
 * 25c3*4] (@0x8004b33c-64; table bytes '7c b3 04 80 08 b4 04 80'). */
static void state7_exchange(uint16_t pressed, uint16_t held)
{
    switch (s_c3) {
    case 0:  state7_select(pressed, held); break;  /* [0] @0x8004b37c */
    case 1:  state7_result_anim();         break;  /* [1] @0x8004b408 */
    default: break;                                /* unreachable (terminal invariant) */
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
    case 7:  /* EXCHANGE: byte-true per-frame jal FUN_8004b33c @0x8004a6ac — WAVE 5
              * (the combine pair engine; sub-dispatch on 25c3 via @0x80074c44).
              * Entry keeps 25be at its grid-mirror value (== 25bd) and c3/c4 at 0
              * (terminal invariant) — the select sub runs first. */
        state7_exchange(pressed, held);
        break;
    case 8:  /* EXIT slide-out @0x8004a6bc-6d8: 25ee +14 while <251 -> 264, then
              * state 2 (tab-cluster slide-back + tab select). */
        if (g_inv_screen.act_base_y < 251) g_inv_screen.act_base_y += 14;
        else                               g_inv_screen.item_state = 2;
        break;
    case 9:  /* CHECK: per-frame jal FUN_800c6630 @0x8004a6e8 (WAVE 4 — panel slide,
              * ITPS photo slide-in, desc message, exit 25c2=4 + 25d6=1). Pad-free
              * here: the only input is the msg-VM page/dismiss edge in msg_vm_tick. */
        state9_check();
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
    s_msg_active = 0; s_msg_state = 0; s_msg_cur = 0;  /* (port hygiene: the menu message
                                            * cannot survive a close — it must be
                                            * dismissed to leave state 5/9; guards the
                                            * debug toggle) */
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
    g_inv_screen.box_mode = 0;             /* ITEM BOX teardown (≙ RE2 close 5c00:=0) */
    s_box_target = 0;
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

/* ---------------------------------------------------------------------------------- */
/* MAP-in-status runner (FUN_8004c058 @0x8004c058, per-frame from LAB_8004974c         */
/* @0x80049a1c). Sub-FSM on 25c2 (g_inv_screen.item_state) with frame counter 25c3    */
/* (s_c3) — the SAME bytes the ITEM FSM uses (s1 = 0x800b25c2 @0x8004c060-64).        */
/* States: 0 slide-out / 1 interactive / 2 reverse slide / >=3 no-op (@0x8004c098).   */
/* ---------------------------------------------------------------------------------- */
static void map_mode(uint16_t pressed)
{
    switch (g_inv_screen.item_state) {
    case 0:
        if (s_c3 < 0x19) {                      /* sltiu 0x19 @0x8004c0bc — 25 frames */
            g_inv_screen.list_x     += 15;      /* 25e0 @0x8004c0e0-e8  */
            g_inv_screen.ecg_y      += 9;       /* 25e6 @0x8004c0f4-fc  */
            g_inv_screen.cond_x     -= 9;       /* 25e4 @0x8004c108-110 */
            g_inv_screen.equip_y    -= 7;       /* 25de @0x8004c11c-124 */
            g_inv_screen.arms_y     -= 7;       /* 25da @0x8004c130-138 */
            g_inv_screen.idcard_x   -= 8;       /* 25f0 @0x8004c144-14c */
            g_inv_screen.tab_base_y += 7;       /* 25ea @0x8004c154 + join @0x8004c2b4-b8 */
            s_c3++;                             /* sb a0,1(s1) @0x8004c2c0 */
            return;
        }
        /* Slide done: StoreImage rect (448,256,64,256) -> 0x801a0000 (@0x8004c158-78)
         * + DrawSync (@0x8004c17c), CD-busy poll DAT_800b2a24 (@0x8004c184-90, retries
         * the frame while the 0x8004c328 load runs), LoadImage of the map gfx from
         * 0x801a8000 into the same rect (@0x8004c198-b0) + DrawSync, then the prim
         * arena build FUN_80046fd8 (@0x8004c1bc). Port: the PC rasterizer keeps the
         * MAP page as its own texture (loaded synchronously off map_page) — the VRAM
         * round-trip is state-free here and the busy poll never blocks. */
        g_inv_screen.item_state++;              /* c2 0->1 @0x8004c1c4 + 0x8004c204-210 */
        s_c3 = 0;                               /* sb zero,1(s1) @0x8004c1cc */
        return;
    case 1:
        /* interactive: VIRTUAL cancel edge 0x8000 (lw 0x800ac76c @0x8004c1d0-e0)
         * <- RAW CROSS (@0x80073dbc[15], wave-6 finding 4) OR raw L1 edge 0x4
         * (lhu 0x800ac762 @0x8004c1e8-f8) -> c2++ (@0x8004c200-210).
         * NO other input: no pan, no room step — a static viewer. */
        if ((re15_pad_virtual_word(pressed) & 0x8000) || (pressed & RE15_PAD_BIT_L1))
            g_inv_screen.item_state++;
        return;
    case 2:
        if (s_c3 < 0x19) {                      /* sltiu 0x19 @0x8004c21c */
            g_inv_screen.list_x     -= 15;      /* 25e0 @0x8004c240-248 */
            g_inv_screen.ecg_y      -= 9;       /* 25e6 @0x8004c254-25c */
            g_inv_screen.cond_x     += 9;       /* 25e4 @0x8004c268-270 */
            g_inv_screen.equip_y    += 7;       /* 25de @0x8004c27c-284 */
            g_inv_screen.arms_y     += 7;       /* 25da @0x8004c290-298 */
            g_inv_screen.idcard_x   += 8;       /* 25f0 @0x8004c2a4-2ac */
            g_inv_screen.tab_base_y -= 7;       /* 25ea @0x8004c2b0-2b8 */
            s_c3++;                             /* sb a0,1(s1) @0x8004c2c0 */
            return;
        }
        /* Exit: LoadImage restore of the saved rect from 0x801a0000 (@0x8004c2c4-e4)
         * + DrawSync (state-free in the port texture model), then the verified exit
         * contract @0x8004c2f0-304: 25ca=0, 25c1=0, 25c2=0, 25c3=0 (tab kept). */
        g_inv_screen.highlight = 0;             /* sb zero 25ca @0x8004c2f4 */
        s_substate = 0;                         /* sb zero 25c1 @0x8004c2fc */
        g_inv_screen.item_state = 0;            /* sb zero 0(s1) @0x8004c300 */
        s_c3 = 0;                               /* sb zero 1(s1) @0x8004c304 */
        return;
    default:
        return;                                 /* j 0x8004c308 (@0x8004c098/0x8004c0ac) */
    }
}

/* ---------------------------------------------------------------------------------- */
/* FILE-in-status runner (DEBUG.BIN FUN_800c6ca0 @0x800c6ca0, per-frame from            */
/* LAB_8004974c; file==RAM proven). Dispatch table @0x800c6fe8[25c2] (8 entries):       */
/*   0 = enter slide 0x800c6cd4   1 = list 0x800c6d70 (inner dispatch @0x800c7008:     */
/*       {0 page level 0x800c6dbc, 1 row select via 0x800c6ec8 -> 0x800c7010})         */
/*   2 = exit slide 0x800c6ed8    3 = reader 0x800c6f90                                */
/*   4..7 = page-turn anim 0x800c6fb0 (25c2 := driver 0x800c77bc(25c2))                */
/* Frame counter = the shared 25c3 (byte 1(a1), a1=0x800b25c2 @0x800c6ca8-cac).        */
/* All input = RAW edge word DAT_800ac762 (byte-swapped layout: CROSS=0x40,            */
/* SQUARE=0x80, R1=0x8, d-pad 0x1000/2000/4000/8000) -> port canonical bits.           */
/* ---------------------------------------------------------------------------------- */
static uint16_t s_file_bob_ctr = 0;   /* u16 @0x800c75fc (bob counter)                 */
static uint16_t s_file_bob_off = 0;   /* u16 @0x800c75fe (bob offset)                  */

/* Page-turn driver 0x800c77bc: a0 = 25c2 (4..7), returns the next 25c2; mutates the
 * reader page [0x800c6c97], phase u16 @0x800c78a4 and text x s16 @0x800c78a6.
 * Phase < 10 (sltiu 0xa @0x800c77d4): states 4/5 x += 28 (@0x800c77f0), states 6/7
 * x -= 28 (@0x800c77fc), phase++ (@0x800c77e4/0x800c7808). Phase >= 10: state 4 ->
 * page-- (u8 wrap @0x800c7854-58) + x=-240 (@0x800c785c) + state 5 (@0x800c7864);
 * state 6 -> page++ + x=0x140=320 (@0x800c7868-70) + state 7 (@0x800c7878); states
 * 5/7 -> state 3 (@0x800c787c); all phase-complete paths zero the phase (@0x800c7880).
 * Net curve (fwd): x 0xc -> -268 over 10 frames (old page slides LEFT), snap 320,
 * 320 -> 40 over 10 frames (new page slides IN) — landing exactly on the reader's
 * fixed x=0x28; bwd mirrored via 0x44 / -240. */
static int file_anim_step(int state)
{
    if (g_inv_screen.file_anim_phase < 10) {
        if (state >= 4) {
            if (state < 6)      g_inv_screen.file_text_x += 28;
            else if (state < 8) g_inv_screen.file_text_x -= 28;
        }
        g_inv_screen.file_anim_phase++;
        return state;
    }
    g_inv_screen.file_anim_phase = 0;
    if (state == 4) {
        g_inv_screen.file_reader_page--;         /* u8 wrap (andi 0xff @0x800c7858) */
        g_inv_screen.file_text_x = -240;
        return 5;
    }
    if (state == 6) {
        g_inv_screen.file_reader_page++;
        g_inv_screen.file_text_x = 320;
        return 7;
    }
    if (state == 5 || state == 7)
        return 3;
    return state;                                /* other: keep (j 0x800c7884)     */
}

static void file_mode(uint16_t pressed)
{
    switch (g_inv_screen.item_state) {
    case 0:
        /* enter slide @0x800c6cd4-6d6c: 30 frames (sltiu 0x1e @0x800c6cdc). Every
         * slide frame zeroes [0x800c6c94..6c97] (sw zero @0x800c6cec = sub-state,
         * page, row, reader page). */
        if (s_c3 < 0x1e) {
            g_inv_screen.file_sub = 0;
            g_inv_screen.file_page = 0;
            g_inv_screen.file_row = 0;
            g_inv_screen.file_reader_page = 0;
            g_inv_screen.list_x     -= 15;      /* 25e0 @0x800c6d08-10 */
            g_inv_screen.ecg_y      -= 9;       /* 25e6 @0x800c6d1c-24 */
            g_inv_screen.arms_x     -= 7;       /* 25d8 @0x800c6d30-38 */
            g_inv_screen.equip_x    -= 7;       /* 25dc @0x800c6d44-4c */
            g_inv_screen.tab_base_y += 7;       /* 25ea @0x800c6d58 + tail @0x800c6f60 */
            g_inv_screen.idcard_y   -= 8;       /* 25f2 @0x800c6d60 + tail @0x800c6f68 */
            s_c3++;                             /* sb a0,1(a1) @0x800c6f70 (shared tail) */
            return;
        }
        s_c3 = 0;                               /* sb zero,1(a1) @0x800c6d6c */
        g_inv_screen.item_state++;              /* 0 -> 1 via the ++ tail @0x800c6e14-20 */
        return;

    case 1:
        /* list: rows/title/highlight drawn build-side (jals @0x800c6d70-8c); inner
         * dispatch @0x800c6d94-b8 on [0x800c6c94]. */
        if (g_inv_screen.file_sub == 0) {
            /* page level 0x800c6dbc — branch-chain priority LEFT > RIGHT > SQUARE >
             * CROSS/R1 (@0x800c6dc8-df8). */
            if (pressed & RE15_PAD_BIT_LEFT) {            /* raw 0x8000 @0x800c6dc8 */
                if (g_inv_screen.file_page == 0) g_inv_screen.file_page = 2;
                else g_inv_screen.file_page--;            /* bgez wrap @0x800c6e3c-48 */
                se4(4);                                   /* @0x800c6e50-58 */
            } else if (pressed & RE15_PAD_BIT_RIGHT) {    /* raw 0x2000 @0x800c6dd4 */
                g_inv_screen.file_page++;
                if (g_inv_screen.file_page >= 3) g_inv_screen.file_page = 0;
                se4(4);                                   /* sltiu 3 @0x800c6e78-84; SE @0x800c6e8c */
            } else if (pressed & RE15_PAD_BIT_SQUARE) {   /* raw 0x80 @0x800c6de0 */
                se4(6);                                   /* @0x800c6ea0-a8 */
                g_inv_screen.file_sub = 1;                /* @0x800c6eac-b4 */
                g_inv_screen.file_row = 0;                /* @0x800c6ebc */
            } else if ((pressed & RE15_PAD_BIT_CROSS) ||  /* raw 0x40 @0x800c6dec */
                       (pressed & RE15_PAD_BIT_R1)) {     /* raw 0x8  @0x800c6df4-f8 */
                se4(5);                                   /* @0x800c6e00-08 */
                g_inv_screen.item_state++;                /* 1 -> 2 @0x800c6e14-20 */
            }
        } else {
            /* row select 0x800c7010 — priority CROSS > SQUARE > DOWN > UP
             * (@0x800c7020-3c). The visibility mask is NOT checked: SQUARE on any
             * row (name or underscores) opens the reader. */
            if (pressed & RE15_PAD_BIT_CROSS) {           /* raw 0x40 @0x800c7020-24 */
                se4(5);                                   /* @0x800c707c-84 */
                g_inv_screen.file_sub = 0;                /* @0x800c7088-8c */
            } else if (pressed & RE15_PAD_BIT_SQUARE) {   /* raw 0x80 @0x800c7028-2c */
                se4(6);                                   /* @0x800c704c-54 */
                g_inv_screen.item_state = 3;              /* 25c2=3 @0x800c7058-60 */
                g_inv_screen.file_reader_page = 0;        /* @0x800c7064-68 */
                s_file_bob_ctr = 0;                       /* sw zero 0x800c75fc */
                s_file_bob_off = 0;                       /*   @0x800c706c-70   */
                g_inv_screen.file_bob_off = 0;
            } else if (pressed & RE15_PAD_BIT_DOWN) {     /* raw 0x4000 @0x800c7030-34 */
                se4(4);                                   /* @0x800c7098-a0 */
                g_inv_screen.file_row++;                  /* sltiu 0xa wrap @0x800c70b4-c4 */
                if (g_inv_screen.file_row >= 10) g_inv_screen.file_row = 0;
            } else if (pressed & RE15_PAD_BIT_UP) {       /* raw 0x1000 @0x800c7038-3c */
                se4(4);                                   /* @0x800c70d0-d8 */
                if (g_inv_screen.file_row == 0) g_inv_screen.file_row = 9;
                else g_inv_screen.file_row--;             /* bgez wrap @0x800c70ec-f8 */
            }
        }
        return;

    case 2:
        /* exit slide @0x800c6ed8-6f70: 30 frames, exact reverse deltas; then the
         * verified exit contract @0x800c6f74-8c. */
        if (s_c3 < 0x1e) {                      /* sltiu 0x1e @0x800c6ee0 */
            g_inv_screen.list_x     += 15;      /* 25e0 @0x800c6f04-0c */
            g_inv_screen.ecg_y      += 9;       /* 25e6 @0x800c6f18-20 */
            g_inv_screen.arms_x     += 7;       /* 25d8 @0x800c6f2c-34 */
            g_inv_screen.equip_x    += 7;       /* 25dc @0x800c6f40-48 */
            g_inv_screen.tab_base_y -= 7;       /* 25ea @0x800c6f54 + @0x800c6f60 */
            g_inv_screen.idcard_y   += 8;       /* 25f2 @0x800c6f58 + @0x800c6f68 */
            s_c3++;                             /* sb a0,1(a1) @0x800c6f70 */
            return;
        }
        g_inv_screen.highlight = 0;             /* sb zero 25ca @0x800c6f78 */
        s_substate = 0;                         /* sb zero 25c1 @0x800c6f80 */
        g_inv_screen.item_state = 0;            /* sb zero 0(a1) @0x800c6f84 */
        s_c3 = 0;                               /* sb zero 1(a1) @0x800c6f8c */
        return;

    case 3: {
        /* reader @0x800c6f90-a8: text (build-side) + arrows 0x800c7528 + input
         * 0x800c7110. The arrow drawer draws with the CURRENT bob offset, THEN
         * updates the counter (@0x800c75ac-e4: ==0x1e -> off=4, ==0x3c -> reset,
         * else ++) — modeled by latching the drawn value first. */
        g_inv_screen.file_bob_off = s_file_bob_off;
        if (s_file_bob_ctr == 0x1e)      { s_file_bob_off = 4; s_file_bob_ctr++; }
        else if (s_file_bob_ctr == 0x3c) { s_file_bob_ctr = 0; s_file_bob_off = 0; }
        else                             s_file_bob_ctr++;
        /* input 0x800c7110 (s0 = end = u16[0x800ccd34]>>1 = 7 @0x800c7124-30) —
         * priority CROSS > SQUARE > LEFT > RIGHT (@0x800c712c-4c). */
        {
            int end = 7;   /* s0 = u16 @0x800ccd34 (=0xe) >> 1 (@0x800c7124-30);
                            * the doc header is static DEBUG.BIN data (gen census) */
            uint8_t *pg = &g_inv_screen.file_reader_page;
            if (pressed & RE15_PAD_BIT_CROSS) {           /* raw 0x40 @0x800c712c-34 */
                se4(5);                                   /* @0x800c7170-78 */
                g_inv_screen.item_state = 1;              /* 25c2=1 @0x800c717c-84 */
            } else if (pressed & RE15_PAD_BIT_SQUARE) {   /* raw 0x80 @0x800c7138-3c */
                if (*pg == end) {                         /* bne skip @0x800c7168 */
                    se4(5);
                    g_inv_screen.item_state = 1;          /* falls into @0x800c7170 */
                }
            } else if (pressed & RE15_PAD_BIT_LEFT) {     /* raw 0x8000 @0x800c7140-44 */
                if (*pg == end) {                         /* beq @0x800c7210 */
                    (*pg)--;                              /* @0x800c7228 */
                    se4(4);                               /* @0x800c722c-34 */
                } else if (*pg == 0) {
                    *pg = 0;                              /* sb zero @0x800c7224 (no SE) */
                } else {
                    g_inv_screen.item_state = 4;          /* 25c2=4 @0x800c7240-48 */
                    g_inv_screen.file_anim_phase = 0;     /* sh zero 78a4 @0x800c7258 */
                    g_inv_screen.file_text_x = 0x44;      /* sh 0x44 78a6 @0x800c7254-5c */
                    se4(8);                               /* @0x800c7260-68 */
                }
            } else if (pressed & RE15_PAD_BIT_RIGHT) {    /* raw 0x2000 @0x800c7148-4c */
                if (*pg + 1 == end) {                     /* beq t1,s0 @0x800c71a4 */
                    (*pg)++;                              /* -> the END position @0x800c71e8 */
                    se4(4);                               /* @0x800c71ec-f4 */
                } else if (*pg == end) {                  /* t1==end+1 @0x800c71ac -> close
                                                           * check @0x800c715c -> @0x800c7170 */
                    se4(5);
                    g_inv_screen.item_state = 1;
                } else {
                    g_inv_screen.item_state = 6;          /* 25c2=6 @0x800c71b4-bc */
                    g_inv_screen.file_anim_phase = 0;     /* sh zero 78a4 @0x800c71cc */
                    g_inv_screen.file_text_x = 0xc;       /* sh 0xc 78a6 @0x800c71c8-d0 */
                    se4(8);                               /* @0x800c71d4-dc */
                }
            }
        }
        return;
    }

    case 4: case 5: case 6: case 7:
        /* @0x800c6fb0-c4: 25c2 := driver(25c2); the text is drawn at the driver's
         * s16 @0x800c78a6 (build-side, lh @0x800c6fc8-d0). */
        g_inv_screen.item_state = (uint8_t)file_anim_step(g_inv_screen.item_state);
        return;

    default:
        return;
    }
}

static void menu_task_dispatch(uint16_t pressed, uint16_t held)
{
    switch (s_phase) {
    case 0:
        if (!s_p0_entered) {
            s_p0_entered = 1;
            phase0_init();
            /* ITEM BOX target [DESIGN]: spawn straight into the box subscreen.
             * The shared phase-0 init above already ran the compaction-on-open
             * (re15_inv_compact — the RE1.5 menu-open site @0x800464a0 AND the
             * RE2 box-open site FUN_80069714 @0x80068c60 = quirk 9) and took the
             * equip snapshot (25ce — RE2's close-time weapon-model-reload driver,
             * quirk 14). */
            if (s_box_target) { s_substate = 4; re15_itembox_screen_open(); }
        }
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
        case 1: map_mode(pressed); break;       /* jal 0x8004c058 @0x80049a1c */
        case 2: file_mode(pressed); break;      /* DEBUG.BIN FUN_800c6ca0 (FILE wave) */
        case 3: item_mode(pressed, held); break;
        case 4:                                 /* ITEM BOX [DESIGN §6] — RE2 FSM
                                                 * shape (re15_itembox.c); returns
                                                 * 1 on the exit request (RE2
                                                 * panel 0 = exit start) -> the
                                                 * shared close phase (fade-out +
                                                 * equip commit).               */
            if (re15_itembox_screen_tick(pressed, held)) s_phase = 2;
            break;
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
    /* MAP wave: the draw-side gate mirror + per-frame marker. The draw chain runs
     * AFTER the run sub-state in the same frame (jal 0x80049a5c @0x80049a44), so the
     * gate word(25c0)&0xffffff==0x00010100 (@0x80049bb4-cc) already sees c2=1 on the
     * upload frame, and FUN_800473f8 (@0x80049bcc) recomputes the marker from the LIVE
     * player world X/Z (lw 0x800aca88/0x800aca90 @0x8004741c/0x8004746c) before every
     * AddPrim — modeled by refreshing the marker fields after the FSM step. */
    g_inv_screen.substate = s_substate;
    if (s_substate == 1 && g_inv_screen.item_state == 1)
        re15_inv_map_marker(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                            g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                            g_inv_screen.map_room,
                            &g_inv_screen.map_marker_x, &g_inv_screen.map_marker_y);
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
/* ITEM BOX open request (world-side trigger — the save-phone precedent).             */
/* [DESIGN] The box AOT examine sets the pending signal (scd_vm.c intercept); the     */
/* platform calls this, which runs the SHARED byte-true open transition (stage 1      */
/* fade-out @0x8001ca64-88 -> stage 2 hold-black + task spawn @0x8001ca98-cb4c) and   */
/* lands in substate 4 instead of the tab select. Freeze semantics identical to the   */
/* START menu (task-0 suspension model @0x800460bc).                                  */
/* ---------------------------------------------------------------------------------- */
void re15_menu_request_box(void)
{
    if (s_alive || s_stage != 0) return;
    s_box_target = 1;
    s_latch = 1;
    s_stage = 1;
}

/* Bridge for the box transfer reject (re15_itembox.c): open desc-bank entry 0
 * ("You can't use it here.") at (0x18,0xa8) — the RE1.5 cant-use message infra
 * (FUN_80027e68(0x00a80018,0x8400,0,0) @0x8004b2d8), standing in for RE2's box
 * reject FUN_8002fe38(0xaf0010,0xe400,8,0) [RE1.5-adapted, itembox_spec.md §6].
 * The box FSM's state 5 polls re15_menu_msg_active() (≙ DAT_800e873c bit 0x80). */
void re15_menu_box_reject_msg(void)
{
    msg_open(0, 0x18, 0xa8);
}

/* DEBUG (harnesses/tests): instant box-screen open — full phase-0 init (equip
 * snapshot + compaction-on-open) with the fades skipped; close via the normal
 * EXIT path or re15_menu_toggle. */
void re15_menu_toggle_box(void)
{
    if (!s_alive && s_stage == 0) {
        phase0_init();
        re15_fade_kill(0);
        s_alive = 1;
        s_p0_entered = 1;
        s_phase = 1;
        s_substate = 4;
        s_box_target = 1;
        re15_itembox_screen_open();
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
        s_box_target = 0;
        g_inv_screen.box_mode = 0;
        s_msg_active = 0; s_msg_state = 0; s_msg_cur = 0;
        g_inv_screen.item_state = 0;
        g_inv_screen.name_item = -1;
        g_inv_screen.msg_reveal = 0;
        g_inv_screen.msg_arrow = 0;
        g_inv_screen.exam_visible = 0;
    }
}
