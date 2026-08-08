/*
 * RE1.5 Rebuilt — SCD VM (Phase 4.4, 2026-05-18).
 *
 * Platform-agnostic. Shared between PSX and PC targets.
 *
 * Tick loop:
 *   for each active thread:
 *     loop:
 *       fetch opcode at thread.pc
 *       dispatch via opcode_table
 *       opcode returns 0/1/2
 *       0 = pop frame (call_depth--)
 *       1 = continue (next opcode this tick)
 *       2 = yield (move to next thread)
 *       if kill_pending: mark inactive, break loop
 *
 * Phase 4.4 implements just enough opcodes to run a hello-script.
 * Each subsequent phase adds more opcodes to widen RE1.5 coverage.
 */
#include <string.h>
#include <stdlib.h>    /* getenv (spawn diagnostics) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif
#include "re15_scd.h"
#include "re15_engine.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 for Dir_set */
#include "re15_aot.h"        /* Phase 4.4.6: AOT trigger ops */
#include "re15_fade.h"       /* the screen-fade channel engine (SCD 0x56 config / 0x57 kick) */
#include "re15_inventory.h"  /* Phase 4.4.7 */
#include "re15_actor.h"      /* Phase 4.4.8 */
#include "re15_esp.h"        /* Phase ESP-C: op-0x3a effect particle spawn */
#include "re15_damage.h"     /* re15_enemy_apply_hitbox — wire the spawned enemy's +0x78 hitbox */
#include "re15_msg.h"        /* re15_msg_is_choice — YES/NO vs plain Message_on gating */
#include "re15_savepoint.h"  /* FE-4: phone save-point registry + pending signal */
#include "re15_itembox.h"    /* ITEM BOX: safe-room box-AOT registry + pending signal
                              * (save-phone precedent, shots/itembox_spec.md §6) */
#include "re15_room.h"       /* g_current_room_id (save-point room match) */
#include "re15_to_re2.h"     /* RE1.5 → RE2 adapter layer */

scd_vm_t g_scd;

/* Set by the door AOT scan when a same-room door's entry scenario triggers a sub00
 * cutscene case (e.g. door 0 → outdoor → scenario 11 → sub14). Consumed by the main
 * loop (scd_room_reenter + clear). -1 = none. */
int g_scd_pending_scenario = -1;

/* Latched to 1 by the game-step self-room scenario consume (the BYTE-TRUE intro handoff:
 * ROOM1170 sub11 @file 0x16d0 Aot_on(3) → door 3 → this reenter → sub15 Ck(4,242,1) → Elliot).
 * The intro must fire EXACTLY ONE self-reenter (the original's single Aot_on(3)); the PC-side
 * hand-deferred sub00-spawn fallback (main.c) checks this so it can no longer fire a redundant
 * SECOND reenter — which, because sub02 @file 0x142c Set(4,242)=0 has cleared the Elliot gate by
 * then, would take sub15's ELSE branch @file 0x1886 and spawn the 7 crows (type 0x21) mid-intro.
 * Reset to 0 on every room entry (re15_room_apply_pending) so a genuine later re-entry re-arms. */
int g_scd_self_reenter_fired = 0;

/* Is actor `slot` the work-entity of an active SCD thread right now? A cutscene binds an actor via
 * Work_set (e.g. ROOM1170 sub02's Work_set(2,0) = Elliot) and then drives its animation with
 * Plc_dest / Plc_motion. While that binding holds, the SCD OWNS the actor's clip — the enemy brain
 * must not run (mirrors the original AI-freeze DAT_800aca40&0x20000000 during a scripted event). The
 * NPC executor uses this to yield during BOTH the Plc_dest walk AND the Plc_motion gestures, so its
 * idle-clip advance can't stomp the walk clip (float) nor wrap the gesture clip at the wrong EM040
 * length (the arm-wave loop). */
int re15_scd_slot_event_controlled(int slot)
{
    if (slot < 0) return 0;
    for (int i = 0; i < SCD_THREAD_COUNT; i++)
        if (g_scd.threads[i].active && (int)g_scd.threads[i].work_slot == slot)
            return 1;
    return 0;
}

/* Forward-declare opcode handlers */
typedef int (*scd_op_fn_t)(scd_thread_t *t);

static int op_nop      (scd_thread_t *t);
static int op_evt_end  (scd_thread_t *t);
static int op_evt_next (scd_thread_t *t);   /* Phase 4.5.13-RE2 */
static int op_evt_exec (scd_thread_t *t);   /* Phase 4.5.11 */
static int op_sleep    (scd_thread_t *t);
static int op_sleeping (scd_thread_t *t);
static int op_do       (scd_thread_t *t);   /* Phase 4.5.13-RE2 */
static int op_edwhile  (scd_thread_t *t);   /* Phase 4.5.13-RE2 */
static int op_for      (scd_thread_t *t);   /* AO4-round 2026-05-26 */
static int op_for2     (scd_thread_t *t);   /* For2 = For, work-var count (LAB_8003f5d0) */
static int op_next     (scd_thread_t *t);   /* AO4-round 2026-05-26 */
static int op_while    (scd_thread_t *t);   /* [#8] 0x0F */
static int op_ewhile   (scd_thread_t *t);   /* [#8] 0x10 */
static int op_break    (scd_thread_t *t);   /* [#8] 0x1A */
static int scd_eval_pred_chain(scd_thread_t *t, uint8_t chain_len);  /* [#8] While/Edwhile AND/OR predicate chain */
static int op_goto     (scd_thread_t *t);
static int op_gosub    (scd_thread_t *t);
static int op_return   (scd_thread_t *t);
static int op_if       (scd_thread_t *t);
static int op_else     (scd_thread_t *t);
static int op_end_if   (scd_thread_t *t);
static int op_dbg_text (scd_thread_t *t);
static int op_cut_chg  (scd_thread_t *t);
static int op_message_on(scd_thread_t *t);
static int op_switch   (scd_thread_t *t);
static int op_case     (scd_thread_t *t);
static int op_default  (scd_thread_t *t);
static int op_end_switch(scd_thread_t *t);
static int op_se_on    (scd_thread_t *t);
static int op_fade_adjust(scd_thread_t *t);   /* 0x57 — fade-channel step (was mis-named Sce_bgmtbl_set) */
static int op_flag_set2  (scd_thread_t *t);   /* 0x59 — indexed flag SET/CLEAR/TOGGLE (was mis-named Xa_on) */
static int op_se_vol   (scd_thread_t *t);
/* Phase 4.4.4 */
static int op_ck       (scd_thread_t *t);
static int op_set      (scd_thread_t *t);
static int op_pos_set  (scd_thread_t *t);
static int op_dir_set  (scd_thread_t *t);
static int op_plc_motion(scd_thread_t *t);
/* Phase 4.5.13-B/C: cinematic player opcodes */
static int op_plc_dest (scd_thread_t *t);
static int op_plc_neck (scd_thread_t *t);
static int op_plc_ret  (scd_thread_t *t);
static int op_plc_flg  (scd_thread_t *t);
/* Phase 4.4.6 */
static int op_aot_set  (scd_thread_t *t);
static int op_aot_reset(scd_thread_t *t);
/* Phase 4.4.8 */
static int op_work_set  (scd_thread_t *t);   /* Phase 4.5.9-G */
static int op_speed_set (scd_thread_t *t);   /* Phase 4.5.13-RE2 F6 */
static int op_add_speed (scd_thread_t *t);   /* Phase 4.5.13-RE2 F6 */
static int op_member_set(scd_thread_t *t);
static int op_member_cmp(scd_thread_t *t);
static int op_cmp       (scd_thread_t *t);   /* 0x23 — work-var compare predicate */
static int op_save      (scd_thread_t *t);   /* 0x24 — work-var immediate store */
static int op_copy      (scd_thread_t *t);   /* 0x25 — work-var copy */
static int op_calc      (scd_thread_t *t);   /* 0x26 — work-var ALU w/ immediate */
static int op_calc2     (scd_thread_t *t);   /* 0x27 — work-var ALU w/ work-var */
static int op_sce_em_set(scd_thread_t *t);
/* Phase 4.5.9-C */
static int op_door_aot_set(scd_thread_t *t);
static int op_item_aot_set(scd_thread_t *t);
/* Phase 4.5.12 — Obj_model_set dynamic walker (replaces fixed-38 fallback) */
static int op_obj_model_set(scd_thread_t *t);
static int op_unknown  (scd_thread_t *t);

/* Opcode dispatch table — 256 entries, unimplemented → op_unknown */
static scd_op_fn_t s_op_table[256];

/* Phase 4.5.11: current RDT pointer for Evt_exec sub_scd lookup.
 * Set by scd_register_current_rdt() at boot from scd_room_setup.c. */
static const re15_rdt_t *s_current_rdt = NULL;
void scd_register_current_rdt(const re15_rdt_t *rdt) { s_current_rdt = rdt; }

/* Per-frame controller press-EDGE mask, published by re15_game_step (game_step_common.c)
 * each frame. Read by the YES/NO message cursor in op_message_on (UP/DOWN toggle). */
uint16_t g_scd_pad_edge = 0;
uint16_t g_scd_pad_held = 0;   /* full held pad word — dialog CROSS fast-forward (wf_6aad95ad) */

/* Per-frame HELD action-button (Square) state, published by re15_game_step. Read by the
 * dialog FSM's fast-forward (op_message_on TYPING state — holding speeds the typewriter). */
uint8_t g_scd_action_held = 0;

/* Phase 4.5.9-C: opcode-size table extracted from the Java disassembler.
 * Lets `op_unknown` advance PC safely past unimplemented opcodes instead
 * of killing the thread on first unknown. 0 = unregistered (treat as
 * 1-byte for defensive advance, matching disassembler line 760). 0xFF
 * = dynamic-size opcode (only 0x2D Obj_model_set today) — op_unknown
 * delegates it to op_obj_model_set (the real FIXED 34-byte advance,
 * LAB_80040914). Size-table audit 2026-07-12: every fixed-size entry
 * matches its handler's PC-advance (no desync). */
static const uint8_t s_opcode_sizes[256] = {
    [0x00] = 1,  [0x01] = 2,  [0x02] = 1,  [0x03] = 4,
    [0x04] = 4,  [0x05] = 2,  [0x06] = 4,  [0x07] = 4,
    [0x08] = 2,  [0x09] = 4,  [0x0A] = 3,  [0x0B] = 1,
    [0x0C] = 1,  [0x0D] = 6,  [0x0E] = 2,  [0x0F] = 4,
    [0x10] = 2,  [0x11] = 4,  [0x12] = 2,  [0x13] = 4,
    [0x14] = 6,  [0x15] = 4,  [0x16] = 2,  [0x17] = 6,
    [0x18] = 2,  [0x19] = 2,  [0x1A] = 2,  [0x1B] = 6,
    [0x1C] = 1,  [0x1D] = 1,  [0x1E] = 1,
    /* 0x1F NOT REGISTERED */
                                            [0x20] = 1,
    [0x21] = 4,  [0x22] = 4,  [0x23] = 6,  [0x24] = 4,
    [0x25] = 3,  [0x26] = 6,  [0x27] = 4,  [0x28] = 1,
    [0x29] = 2,  [0x2A] = 1,  [0x2B] = 4,  [0x2C] = 20,
    [0x2D] = 0xFF,                                  /* Obj_model_set dyn */
    [0x2E] = 3,  [0x2F] = 4,
    [0x30] = 1,  [0x31] = 1,  [0x32] = 8,  [0x33] = 8,
    [0x34] = 4,  [0x35] = 3,  [0x36] = 12, [0x37] = 4,
    [0x38] = 12, [0x39] = 4,  [0x3A] = 16, [0x3B] = 32,  /* 0x38: 3->12 byte-true (LAB_800417ac +0xc) */
    [0x3C] = 2,  [0x3D] = 3,  [0x3E] = 6,  [0x3F] = 4,  /* 0x3D: 4->3 byte-true (LAB_80041238 @0x80041254 `addiu pc,pc,3`; was an uncorrected RE2 length, unmapped -> op_unknown desynced +1) */
    [0x40] = 8,  [0x41] = 10, [0x42] = 1,  [0x43] = 4,
    [0x44] = 20, [0x45] = 3,  [0x46] = 10, [0x47] = 2,
    [0x48] = 16, [0x49] = 8,  [0x4A] = 2,  [0x4B] = 3,
    [0x4C] = 18, [0x4D] = 10, [0x4E] = 5,  [0x4F] = 22,  /* byte-true (jump table 0x800744a8): 0x4C LAB_80040858 +0x12, 0x4D LAB_800408a8 +0xa, 0x4E LAB_80041980 +5, 0x4F LAB_80016f20 +0x16 */
    /* 0x52..0x5E: RE1.5 lengths (disasm-verified from jump table 0x800744a8,
     * 2026-05-31), NOT retail-RE2. RE1.5 differs here (BIO 1.5→2.0 shift):
     * 0x52=4 (was RE2 6), 0x53=3 (RE2 6), 0x56=6 (RE2 4), 0x57=4 (RE2 8),
     * 0x5A=6 (RE2 2), 0x5B=4 (RE2 2), 0x5C=4 (RE2 3), 0x5D=4 (RE2 2),
     * 0x5E=4 (RE2 2). See reference_re15_vs_re2_scd_opcodes. */
    [0x50] = 22, [0x51] = 4,  [0x52] = 4,  [0x53] = 3,
    [0x54] = 6,  [0x55] = 6,  [0x56] = 6,  [0x57] = 4,
    [0x58] = 4,  [0x59] = 4,  [0x5A] = 6,  [0x5B] = 4,
    [0x5C] = 4,  [0x5D] = 4,  [0x5E] = 4,
    /* [#34] 0x5F..0x8E: RE2 imports — NOT in RE1.5. Dispatch table PTR_LAB_800744a8
     * ends at 0x5E (95 entries, last @0x80074620); dispatcher FUN_8003f0a0 has no
     * bounds check → these would be OOB. No real RE1.5 opcode length → 1-byte
     * defensive advance (= op_unknown / RE1.5 default LAB_8003f1d8 pc+=1, return 1). */
    [0x5F] = 1,
    [0x60] = 1,  [0x61] = 1,  [0x62] = 1,  [0x63] = 1,
    [0x64] = 1,  [0x65] = 1,  [0x66] = 1,  [0x67] = 1,
    [0x68] = 1,  [0x69] = 1,  [0x6A] = 1,  [0x6B] = 1,
    [0x6C] = 1,  [0x6D] = 1,  [0x6E] = 1,  [0x6F] = 1,
    [0x70] = 1,  [0x71] = 1,  [0x72] = 1,  [0x73] = 1,
    [0x74] = 1,  [0x75] = 1,  [0x76] = 1,  [0x77] = 1,
    [0x78] = 1,  [0x79] = 1,  [0x7A] = 1,  [0x7B] = 1,
    [0x7C] = 1,  [0x7D] = 1,  [0x7E] = 1,  [0x7F] = 1,
    [0x80] = 1,  [0x81] = 1,  [0x82] = 1,  [0x83] = 1,
    [0x84] = 1,  [0x85] = 1,  [0x86] = 1,  [0x87] = 1,
    [0x88] = 1,  [0x89] = 1,  [0x8A] = 1,  [0x8B] = 1,
    [0x8C] = 1,  [0x8D] = 1,  [0x8E] = 1,
    /* 0x8F..0xFD = unregistered (0) — treat as unknown, advance 1B. */
    [0xFE] = 5,                              /* custom Dbg_text — Phase 4.4 */
};

/* Phase 4.4.6.1: event-handler registry — pc per event_id 0..255. */
static const uint8_t *s_event_handlers[256];

static void register_opcodes(void)
{
    for (int i = 0; i < 256; i++) s_op_table[i] = op_unknown;

    s_op_table[SCD_OP_NOP]        = op_nop;
    s_op_table[SCD_OP_EVT_END]    = op_evt_end;
    /* Phase 4.5.13-RE2: yield + do-while control flow */
    s_op_table[0x02]              = op_evt_next;
    s_op_table[0x11]              = op_do;
    s_op_table[0x12]              = op_edwhile;
    s_op_table[0x0D]              = op_for;
    s_op_table[0x0E]              = op_next;
    s_op_table[0x0F]              = op_while;   /* [#8] */
    s_op_table[0x10]              = op_ewhile;  /* [#8] */
    s_op_table[0x1A]              = op_break;   /* [#8] */
    s_op_table[0x1B]              = op_for2;    /* For2 = For with a work-var count (LAB_8003f5d0) */
    s_op_table[SCD_OP_EVT_EXEC]   = op_evt_exec;   /* Phase 4.5.11 */
    s_op_table[SCD_OP_SLEEP]      = op_sleep;
    s_op_table[SCD_OP_SLEEPING]   = op_sleeping;
    s_op_table[SCD_OP_GOTO]       = op_goto;
    s_op_table[SCD_OP_GOSUB]      = op_gosub;
    s_op_table[SCD_OP_RETURN]     = op_return;
    s_op_table[SCD_OP_IF]         = op_if;
    s_op_table[SCD_OP_ELSE]       = op_else;
    s_op_table[SCD_OP_END_IF]     = op_end_if;
    s_op_table[SCD_OP_DBG_TEXT]   = op_dbg_text;
    /* Phase 4.4.2 */
    s_op_table[SCD_OP_CUT_CHG]    = op_cut_chg;
    s_op_table[SCD_OP_MESSAGE_ON] = op_message_on;
    s_op_table[SCD_OP_SWITCH]     = op_switch;
    s_op_table[SCD_OP_CASE]       = op_case;
    s_op_table[SCD_OP_DEFAULT]    = op_default;
    s_op_table[SCD_OP_END_SWITCH] = op_end_switch;
    /* Phase 4.4.3: audio */
    s_op_table[SCD_OP_SE_ON]          = op_se_on;
    s_op_table[SCD_OP_SCE_BGMTBL_SET] = op_fade_adjust;   /* RE1.5 0x57 = fade step, not BGM table */
    s_op_table[SCD_OP_XA_ON]          = op_flag_set2;     /* RE1.5 0x59 = indexed flag write, not XA */
    s_op_table[SCD_OP_SE_VOL]         = op_se_vol;
    /* Phase 4.4.4: flags + player state */
    s_op_table[SCD_OP_CK]             = op_ck;
    s_op_table[SCD_OP_SET]            = op_set;
    s_op_table[SCD_OP_POS_SET]        = op_pos_set;
    s_op_table[SCD_OP_DIR_SET]        = op_dir_set;
    s_op_table[SCD_OP_PLC_MOTION]     = op_plc_motion;
    /* Phase 4.5.13-B/C: cinematic player opcodes */
    s_op_table[SCD_OP_PLC_DEST]       = op_plc_dest;
    s_op_table[SCD_OP_PLC_NECK]       = op_plc_neck;
    s_op_table[SCD_OP_PLC_RET]        = op_plc_ret;
    s_op_table[SCD_OP_PLC_FLG]        = op_plc_flg;
    /* Phase 4.4.6: AOT triggers */
    s_op_table[SCD_OP_AOT_SET]        = op_aot_set;
    s_op_table[SCD_OP_AOT_RESET]      = op_aot_reset;
    /* Phase 4.4.8: actors */
    s_op_table[SCD_OP_WORK_SET]       = op_work_set;
    /* Phase 4.5.13-RE2 F6: velocity vector + integrator */
    s_op_table[SCD_OP_SPEED_SET]      = op_speed_set;
    s_op_table[SCD_OP_ADD_SPEED]      = op_add_speed;
    s_op_table[SCD_OP_MEMBER_SET]     = op_member_set;
    s_op_table[SCD_OP_MEMBER_CMP]     = op_member_cmp;
    s_op_table[0x23]                  = op_cmp;   /* Cmp predicate (work-var) */
    s_op_table[0x24]                  = op_save;  /* [#7] work-var ops (LAB_80040018/4c/8c/dc) */
    s_op_table[0x25]                  = op_copy;
    s_op_table[0x26]                  = op_calc;
    s_op_table[0x27]                  = op_calc2;
    s_op_table[SCD_OP_SCE_EM_SET]     = op_sce_em_set;
    /* Phase 4.5.9-C: Door / Item AOT opcodes (real RDT bytecode) */
    s_op_table[0x3B]                  = op_door_aot_set;
    s_op_table[0x50]                  = op_item_aot_set;
    /* 0x4E ist NICHT Item_aot_set (war falscher Alias) — Item_aot_set existiert nur an
     * 0x50 (LAB_80040644). 0x4E (LAB_80041980, 5 Bytes, DAT_800b2368-Semantik) fällt auf
     * op_unknown → korrekter 5-Byte-Skip bis die Handler-Semantik portiert ist. [BYTE_TRUE_AUDIT #4] */
    /* Phase 4.5.12: dynamic-size Obj_model_set */
    s_op_table[0x2D]                  = op_obj_model_set;
    /* Phase 4.5.13 base impls for opcodes used in ROOM1170 cinematic. */
    extern int op_aot_on(scd_thread_t *t);
    extern int op_cut_auto(scd_thread_t *t);
    extern int op_member_set2(scd_thread_t *t);
    extern int op_add_aspeed(scd_thread_t *t);
    extern int op_sce_espr_on(scd_thread_t *t);
    extern int op_sce_espr_kill(scd_thread_t *t);
    extern int op_plc_gun(scd_thread_t *t);
    extern int op_cut_replace(scd_thread_t *t);
    extern int op_cut_old(scd_thread_t *t);
    extern int op_flr_set(scd_thread_t *t);
    extern int op_sca_id_set(scd_thread_t *t);
    extern int op_evt_kill(scd_thread_t *t);
    extern int op_evt_chain(scd_thread_t *t);
    extern int op_aot_reset_basic(scd_thread_t *t);
    s_op_table[0x47]                  = op_aot_on;
    s_op_table[0x3C]                  = op_cut_auto;
    s_op_table[0x35]                  = op_member_set2;
    s_op_table[0x31]                  = op_add_aspeed;
    s_op_table[0x3A]                  = op_sce_espr_on;
    s_op_table[0x4C]                  = op_sce_espr_kill;
    s_op_table[0x4A]                  = op_plc_gun;
    s_op_table[0x4B]                  = op_cut_replace;
    s_op_table[0x2A]                  = op_cut_old;
    s_op_table[0x38]                  = op_flr_set;
    s_op_table[0x37]                  = op_sca_id_set;
    {
        extern int op_sca_floor_set(scd_thread_t *t);
        s_op_table[0x39]              = op_sca_floor_set;   /* SCA floor-byte set */
    }
    s_op_table[0x05]                  = op_evt_kill;
    s_op_table[0x03]                  = op_evt_chain;
    extern int op_sce_espr_control(scd_thread_t *t);
    extern int op_sce_bgm_control(scd_thread_t *t);
    extern int op_sce_fade_set(scd_thread_t *t);
    extern int op_sce_shake_on(scd_thread_t *t);
    extern int op_flag_ck2(scd_thread_t *t);   /* 0x58 = flag-bank bit-check predicate (was misnamed op_plc_rot) */
    extern int op_weapon_chg(scd_thread_t *t);
    extern int op_plc_cnt(scd_thread_t *t);
    extern int op_member_calc(scd_thread_t *t);
    extern int op_kage_set(scd_thread_t *t);
    extern int op_cut_be_set(scd_thread_t *t);
    extern int op_xa_vol(scd_thread_t *t);
    extern int op_sce_key_ck(scd_thread_t *t);
    extern int op_keep_item_ck(scd_thread_t *t);
    extern int op_sce_item_lost(scd_thread_t *t);
    extern int op_plc_gun_eff(scd_thread_t *t);
    extern int op_mizu_div_set(scd_thread_t *t);
    s_op_table[0x52]                  = op_sce_espr_control;
    s_op_table[0x54]                  = op_sce_bgm_control;
    s_op_table[0x53]                  = op_sce_fade_set;
    s_op_table[0x5C]                  = op_sce_shake_on;
    s_op_table[0x58]                  = op_flag_ck2;   /* RE1.5 0x58 = flag-check predicate */
    s_op_table[0x5A]                  = op_weapon_chg;
    s_op_table[0x5B]                  = op_plc_cnt;
    s_op_table[0x55]                  = op_member_calc;
    {
        extern int op_fade_config(scd_thread_t *t);
        s_op_table[0x56]              = op_fade_config;   /* RE1.5 0x56 = fade-channel CONFIG
                                                           * (@0x80042a58 -> FUN_800217b0), not
                                                           * RE2 Member_calc2 */
    }
    /* [#34] 0x5F/0x60/0x61/0x62/0x63: RE2 imports — do NOT exist in RE1.5.
     * Dispatch table PTR_LAB_800744a8 ends at 0x5E; leaving them unregistered routes
     * them to op_unknown (= RE1.5 default LAB_8003f1d8 pc+=1, return 1). The op_kage_set/
     * op_cut_be_set/op_xa_vol/op_sce_item_lost/op_plc_gun_eff defs stay but are now unused.
     * 0x51/0x5D/0x5E ARE valid RE1.5 opcodes (≤0x5E) — keep registered. */
    s_op_table[0x51]                  = op_sce_key_ck;
    s_op_table[0x5E]                  = op_keep_item_ck;
    s_op_table[0x5D]                  = op_mizu_div_set;
}

/* Internal: enqueue an audio event. Drops if full (ring buffer overflow). */
/* Renamed to the public name; the old static `enqueue_audio` is kept
 * as a thin alias so existing op handlers below don't need touching. */
void scd_audio_queue_push(const scd_audio_event_t *evt);
static void enqueue_audio(const scd_audio_event_t *evt) { scd_audio_queue_push(evt); }
void scd_audio_queue_push(const scd_audio_event_t *evt)
{
    g_scd.audio_total++;
    if (g_scd.audio_count >= SCD_AUDIO_QUEUE_SIZE) return;  /* drop */
    g_scd.audio_queue[g_scd.audio_head] = *evt;
    g_scd.audio_head = (uint8_t)((g_scd.audio_head + 1) % SCD_AUDIO_QUEUE_SIZE);
    g_scd.audio_count++;
}

/* Public consumer API — used by Phase 4.6 audio subsystem. */
int scd_audio_queue_pop(scd_audio_event_t *out)
{
    if (g_scd.audio_count == 0) return 0;
    /* Tail index = (head - count + N) % N */
    uint8_t tail = (uint8_t)((g_scd.audio_head + SCD_AUDIO_QUEUE_SIZE - g_scd.audio_count)
                             % SCD_AUDIO_QUEUE_SIZE);
    *out = g_scd.audio_queue[tail];
    g_scd.audio_count--;
    return 1;
}

/* ========================================================================= */
/* VM control                                                                 */
/* ========================================================================= */

void scd_vm_init(void)
{
    memset(&g_scd, 0, sizeof(g_scd));   /* clears tick_count + all *_pending flags */
    g_scd.work_slot = -1;               /* Phase 4.5.9-F: no work entity at boot */
    /* RVD auto-camera default = ON (byte-true INVERSE of DAT_800aca3c bit 0x100: the main loop
     * @0x8001cce0-e4 SKIPS the RVD scan FUN_80014230 only when the bit is SET; zero-init = CLEAR
     * = scan runs. Cut_chg SETS the bit @0x800402d4 -> port cut_auto_enabled=0; the room-(re)load
     * chain FUN_800396fc masks the low 16 bits off @0x80039710-30 = auto ON at every room entry.)
     * The old zero-default froze the camera on the entry cut for any room not entered through a
     * door (damals der RE15_START_ROOM-Boot, inzwischen entfernt — heute ist der Boot-Raum selbst
     * der einzige nicht per Tuer betretene Fall). [audit wf_559c230f CUTSEL-AUTOSCAN-DEFAULT-INVERTED] */
    g_scd.cut_auto_enabled = 1;
    /* Gegner-Spawn-Limit auf die Vorgabe des Originals (FUN_8003ecec @0x8003ed58/60
     * `ori v0,zero,0xff` + `sh v0,4084(at)` -> 0x800b0ff4 = work_vars[0x12]; Zaehler
     * @0x8003ed7c `sh zero,4082(at)` -> 0x800b0ff2 = work_vars[0x11]). MUSS auch hier stehen,
     * nicht nur in scd_room_reenter: der memset oben nullt work_vars, und ein Maximum von 0
     * wuerde jeden Spawn verwerfen (op_sce_em_set @0x80042224 `slt`). */
    g_scd.work_vars[0x12] = 0xFF;
    g_scd.work_vars[0x11] = 0;
    re15_fade_init();                   /* boot-park the fade channels (LAB_80021138); room
                                         * loads do NOT reset them (engine globals) */
    memset((void *)s_event_handlers, 0, sizeof(s_event_handlers));  /* Phase 4.4.6.1 */
    register_opcodes();
    re15_game_state_init();             /* Phase 4.4.4: clear player state + flags */
    re15_aot_init();                    /* Phase 4.4.6: clear AOT slots */
    re15_inv_init();                    /* Phase 4.4.7: clear inventory */
    re15_actor_init();                  /* Phase 4.4.8: clear actor slots */
}

int scd_thread_start(int slot, const uint8_t *pc)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) return -1;
    scd_thread_t *t = &g_scd.threads[slot];
    if (t->active) return -1;
    memset(t, 0, sizeof(*t));
    t->active = 1;
    t->call_depth = 0;
    t->pc = pc;
    t->work_slot = -1;   /* unset (no implicit target) per F11 thread init */
    t->work_prop_idx = -1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[scd] thread start slot=%d first_op=0x%02X\n",
            slot, pc ? pc[0] : 0xFF);
#endif
    return 0;
}

/* UNBEDINGTER Thread-Reseed — byte-true FUN_8003ee3c -> FUN_8003edec (@0x8003ee1c-38:
 * `sb 1,1(a0)` active, `sb 0,0(a0)` level, `sb -1,4(a0)` / `sb -1,8(a0)`, `sw pc,28(a0)`).
 * Unterschied zu scd_thread_start: das Original prueft NICHT, ob der Slot laeuft — es
 * ueberschreibt ihn. Genau das braucht der Per-Frame-sub01-Reseed in scd_vm_tick(); ausserdem
 * darf er nicht wie scd_thread_start pro Frame eine Logzeile schreiben. */
void scd_thread_reseed(int slot, const uint8_t *pc)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) return;
    scd_thread_t *t = &g_scd.threads[slot];
    memset(t, 0, sizeof(*t));
    t->active = 1;
    t->call_depth = 0;
    t->pc = pc;
    t->work_slot = -1;
    t->work_prop_idx = -1;
}

void scd_thread_kill(int slot)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) return;
    g_scd.threads[slot].kill_pending = 1;
}

/* Phase 4.4.6.1: event-handler registry API (table declared at file top). */
void scd_event_register(uint8_t event_id, const uint8_t *pc)
{
    s_event_handlers[event_id] = pc;
}

const uint8_t *scd_event_get_handler(uint8_t event_id)
{
    return s_event_handlers[event_id];
}

/* RE1.5-PROTOTYPE POWER-GATE RECONSTRUCTION (2026-06-14, USER-directed).
 * Some intended puzzle gates were never wired in this unreleased build: ROOM1140's painting
 * switch fires event 3 = sub03 (open the roller door) UNCONDITIONALLY, even though the
 * ROOM11F0 "Reserve Power Control Panel" exists to route power and sub05 ("The switch is not
 * responding") is present but unreachable, and the puzzle's solved-output flag (2,0) has NO
 * consumer anywhere in the game. We reconstruct the intended behaviour with the room's OWN
 * real subs + the real flag: while the power flag is OFF, redirect the painting event to its
 * "not responding" sub. Data-driven so other prototype gaps can be added as one row. */
static const struct {
    unsigned room;        /* room id this gate applies to */
    uint8_t  event;       /* the event id the AOT fires (the "powered" sub) */
    uint8_t  flag_zone;   /* power flag set by the generator/puzzle room */
    uint8_t  flag_idx;
    uint8_t  alt_event;   /* sub to run instead while the power flag is OFF */
} s_power_gates[] = {
    /* ROOM1140 painting: needs ROOM11F0 reserve power (5,12 solved → Set(2,0)). Power OFF
     * → sub05 "The switch is not responding"; ON → sub03 opens the roller door. */
    { 0x1140, 3, 2, 0, 5 },
};

extern unsigned g_current_room_id;

int scd_event_fire(uint8_t event_id)
{
    /* The AOT event id indexes the room's sub_scd[] table (same as Evt_exec) — run
     * that sub in a free event slot. (Globalization 2026-06-13: s_event_handlers[] was
     * vestigial — scd_event_register is never called, so it was always NULL and the
     * generic-AOT event path was dead. The real RE1.5 AOT event runs sub_scd[event_id]:
     * e.g. ROOM1130's switch fires event 2 → sub_scd[2] = the message + door-open sub.) */
    for (unsigned k = 0; k < sizeof(s_power_gates) / sizeof(s_power_gates[0]); k++) {
        if (s_power_gates[k].room == g_current_room_id && s_power_gates[k].event == event_id &&
            !re15_game_flag_get(s_power_gates[k].flag_zone, s_power_gates[k].flag_idx)) {
            event_id = s_power_gates[k].alt_event;   /* power OFF → "not responding" sub */
            break;
        }
    }
    if (event_id >= RE15_RDT_MAX_SUB_SCD || !s_current_rdt) return -1;
    const uint8_t *pc = s_current_rdt->sub_scd[event_id];
    if (!pc) return -1;
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++) {
        if (!g_scd.threads[slot].active) {
            if (scd_thread_start(slot, pc) == 0) return slot;
        }
    }
    return -1;   /* all event slots busy — event dropped (matches RE2 behavior) */
}

/* SCD dispatch return codes (byte-true to FUN_8003f0a0's inner loop):
 *   1 = continue to the next opcode (the main `while (iVar1 == 1)`)
 *   2 = yield this tick (thread sleeps / Evt_next)
 *   0 = frame return — Evt_end(0x01) / Return(0x19): pop the call frame
 *   3 = If-block predicate FALSE → the dispatcher pops the BLOCK stack
 *       (thread+0x140) and jumps PC to the If's saved block_end.
 * (The PSX loop conflates 0 with the false-pop via frame_tag; our engine keeps
 *  0 = frame-return and uses a distinct 3 so Evt_end/Gosub are unaffected. The
 *  observable control flow — which opcodes run, PC, block balance — is identical.) */
#define SCD_R_CONTINUE   1
#define SCD_R_YIELD      2
#define SCD_R_FRAME_RET  0
#define SCD_R_IF_FALSE   3

/* Raum-Init-Lauf vs. Gameplay-Lauf. Das Original hat dafuer ZWEI getrennte Call-Sites:
 *   INIT     FUN_8003ef6c ruft den Dispatcher FUN_8003f0a0 DIREKT (@0x8003f018) — kein sub01-Reseed;
 *            Slot 1 wird dort mit sub00 geladen (a1 = 0, @0x8003efd8).
 *   GAMEPLAY FUN_8003f038 (Haupt-Loop @0x8001cdec) macht ZUERST den Reseed und ruft DANN den
 *            Dispatcher (@0x8003f088).
 * Der Port faedelt beide durch scd_vm_tick(), deshalb dieser Schalter: waehrend des Raum-Init-Laufs
 * bleibt der Reseed aus, sonst wuerde sub00 geklobbert, BEVOR es sein erstes Opcode ausfuehrt
 * (ROOM10D0 haelt seine Sce_em_set-Spawns in sub00 — ohne die Unterdrueckung spawnt der Raum nichts). */
static int s_vm_room_init = 0;
void scd_vm_set_room_init(int on) { s_vm_room_init = on ? 1 : 0; }

void scd_vm_tick(void)
{
    g_scd.tick_count++;
    /* SUB01-RESEED — byte-true FUN_8003f038: JEDEN Gameplay-Frame wird Thread-Slot 1 auf sub_scd[1]
     * zurueckgesetzt (@0x8003f064 `ori a0,zero,0x1`, @0x8003f070 `lw v0,68(v0)` = RDT+0x44,
     * @0x8003f084 `ori a1,zero,0x1` -> jal FUN_8003ee3c @0x8003f080). Der Reset ist UNBEDINGT:
     * FUN_8003ee3c -> FUN_8003edec @0x8003ee1c-38 schreibt Level/Flags/PC neu, egal ob der Slot
     * gerade laeuft. Ab dem ersten Gameplay-Frame gehoert Slot 1 damit permanent sub01; das dort
     * beim Init geladene sub00 wird verdraengt.
     *
     * Der Port fuehrte sub01 stattdessen nur EINMAL beim Raum-Eintritt aus (plus einen
     * examine_poll_pending-Sonderweg, den es im Original nicht gibt). Fuer die Raeume, deren
     * Trigger IN sub01 sitzt, hiess das: der Trigger feuerte live nie und detonierte erst beim
     * naechsten Raum-Load im Tuer-Frame — ROOM1030s Zombie-Cutscene "triggert komisch" und
     * ROOM1040s Schalter tat beim Druecken gar nichts (Nutzer-Report). */
    if (!s_vm_room_init && s_current_rdt && s_current_rdt->sub_scd[1])
        scd_thread_reseed(1, s_current_rdt->sub_scd[1]);
    g_scd.examine_poll_pending = 0;   /* Port-Konstrukt, vom Per-Frame-Reseed abgeloest */


    /* BO-round 2026-05-29: REVERTED the BN-round Evt_exec next-tick defer.
     * Disasm-verified (verify-fun-catalog workflow, Evt_exec @0x8003f2a0 +
     * scd_execute_scripts @0x8003f0a0): PSX does NOT defer spawned threads —
     * its plain slot-iteration loop runs an Evt_exec-spawned thread in the
     * SAME tick if it lands in a higher-indexed slot than the spawner. The
     * next-tick defer was a non-canonical engine addition (and it did NOT fix
     * Elliot's first-frame pose anyway — that has a separate, still-open root
     * cause). Back to PSX-canonical same-tick slot iteration. */
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        scd_thread_t *t = &g_scd.threads[i];
        if (!t->active) continue;

        /* Run opcodes until thread yields or dies */
        int safety = 1024;   /* prevent infinite loops in malformed bytecode */
        while (safety-- > 0) {
            if (t->kill_pending) {
                t->active = 0;
                break;
            }
            if (!t->pc) { t->active = 0; break; }

            uint8_t op = *t->pc;
            /* RE15_SCD_TRACE=1: jeden ausgefuehrten Opcode mitschreiben. Die VM ist die einzige
             * verlaessliche Quelle dafuer, WAS ein Skript wirklich ausfuehrt — ein Offline-Walker
             * desynchronisiert in Daten-Regionen und erfindet Opcodes, die nie laufen. */
            if (getenv("RE15_SCD_TRACE"))
                fprintf(stderr, "[scd] slot=%d op=0x%02X\n", (int)(t - g_scd.threads), op);
            int r = s_op_table[op](t);

            if (r == SCD_R_YIELD) break;   /* yield: thread done for this tick */
            if (r == SCD_R_IF_FALSE) {
                /* byte-true (FUN_8003f0a0): a predicate inside an If-block
                 * evaluated FALSE → pop the block-stack and jump PC to the
                 * block_end the If pushed (= past End_if, or the else-body for
                 * if/else). The predicate already advanced PC past itself; the
                 * pop overwrites it. */
                {
                    int cd = (int)t->call_depth;   /* [#17] block stack is per Gosub-frame */
                    if (t->block_sp[cd] > 0) {
                        t->block_sp[cd]--;
                        t->pc = t->block_stack[cd][(int)t->block_sp[cd]];
                    }
                }
                /* else: FALSE predicate with no open block (malformed) → PC is
                 * already past the predicate; just continue. */
                continue;
            }
            if (r == SCD_R_FRAME_RET) {
                /* Pop frame — per RE2 FUN_80014058: restore caller PC from
                 * call_stack[depth-1], then decrement depth. If we were
                 * already at root frame (depth==0), thread terminates. */
                if (t->call_depth > 0) {
                    t->call_depth--;
                    t->pc = t->call_stack[(int)t->call_depth];
                } else {
                    /* root frame returned → thread done */
                    t->active = 0;
                    break;
                }
            }
            /* r == 1: continue loop (next opcode) */
        }
    }

    /* FRAME-ENDE-WISCH — byte-true FUN_8003ebf4, gerufen vom VM-Executor FUN_8003f0a0 @0x8003f18c
     * UNBEDINGT am Ende JEDES VM-Laufs (direkt hinter der Slot-Schleife @0x8003f17c-88):
     *   @0x8003ebf4 `addiu v0,zero,-1` -> @0x8003ebfc/ec04/ec0c/ec14 `sh v0` auf
     *                0x800b0fd0/d2/d4/d6  = work_vars[0..3] := -1
     *   @0x8003ec1c `sw zero,0x800b102c` = Flag-Bank 5, Wort 1 (Bits 0x20..0x3F) := 0
     *
     * Das sind KEINE Raum-Latches, sondern ein EIN-FRAME-Handshake: der AOT-Scan laeuft nach der VM
     * (@0x8001ce1c) und schreibt work_vars[0]/[1] (FUN_80042bac @0x80042f3c/@0x80042ee4) sowie die
     * Zonen-Bits 0x20+ — im naechsten Frame liest sub01 sie, danach sind sie wieder weg.
     *
     * Der Port hatte den Wisch gar nicht, also latchten die Bits dauerhaft. Folge (Nutzer-Report):
     * ROOM1040s Schalter setzt Flag(5,0x21); beim naechsten BETRETEN des Raums war das Bit noch
     * gesetzt, sub01s Gate `Ck(5,0x21)==1 && Ck(5,0)==0` sofort wahr, und die Rolltor-Frage
     * "It's a shutter switch. Will you push it?" erschien im Tuer-Frame, obwohl der Spieler
     * 31.400 Einheiten vom Schalter entfernt stand.
     *
     * NUR Wort 1: Bits >= 0x40 liegen im Original in Bank 6/7 (FUN_8004ef90 `bank + (bit>>5)*4`) —
     * dort haelt der Port u.a. den Gegner-walk_flag_bit. Wort 0 (Bits 0x00..0x1F) ist echter
     * Raum-Scratch und wird beim Raumladen gewischt (FUN_8003ecec @0x8003ed74), nicht hier.
     *
     * NACHGEHOLT 2026-08-08 (war als "bewusst nicht uebernommen" markiert): derselbe FUN_8003ebf4
     * setzt auch work_vars[0..3] := -1. Die Pruefung, warum das KEIN harmloses Weglassen ist:
     *   - Das Original initialisiert 0x800b0fd0..d6 beim Raumladen NICHT. FUN_8003ecec fasst nur
     *     0x800b0ff4 (@0x8003ed60), 0x800b1028 (@0x8003ed74), 0x800b0ff2 (@0x8003ed7c) und
     *     0x800aca50/52 (@0x8003ed84/8c) an. work_vars[0..3] leben ALLEIN vom Ein-Bild-Handshake:
     *     hier auf -1, danach schreibt der AOT-Scan die getroffenen Slots.
     *   - Der Port memset't sie dagegen auf 0 und wischte nie -> sie standen dauerhaft auf 0.
     *   - `op_cmp` liest VORZEICHENBEHAFTET (`lh` @0x8003fad8, int16_t). -1 und 0 sind damit
     *     unterscheidbar: `Cmp(work_vars[N]==0)` war im Port beim Betreten WAHR, im Original FALSCH.
     * Das fehlende Wischen war also die Ursache spurioser Skript-Ausloesungen, nicht deren Schutz.
     * Reihenfolge stimmt unveraendert: VM-Tick -> dieser Wisch -> re15_aot_scan schreibt die
     * Treffer -> das NAECHSTE Bild liest sie (main.c:3024 tickt die VM, re15_game_step scannt).
     * Nur [0..3] — work_vars[0x11]/[0x12] (Gegner-Zaehler/Cap) liegen ausserhalb und bleiben. */
    g_scd.work_vars[0] = -1;
    g_scd.work_vars[1] = -1;
    g_scd.work_vars[2] = -1;
    g_scd.work_vars[3] = -1;
    g_game.flags[5][1] = 0;
}

/* ENDIAN POLICY (corrected 2026-06-08 — the V-round "all-BE 35-agent rule"
 * below was STALE/WRONG):
 * ALL SCD coordinate, heading and control-flow operands are LITTLE-ENDIAN —
 * the MIPS R3000 `lh`/`lhu` loads read LE. The old "stored BIG-ENDIAN on disk
 * (BIO1-inherited convention), engine byte-swaps at load" theory was a circular
 * assumption: the V-round only confirmed that BE *reads* yield some value, never
 * that the value is geometrically correct. Every forensic re-check (Pos_set,
 * Dir_set, Sce_em_set, Plc_dest X/Z, Door/Item/Aot rects, switch/case/if/else
 * block_length+value, Sleep, For) has resolved to LE. The V4 heli-pad-rect
 * "evidence" was a coincidental in-range hit; LE decodes the same bytes to the
 * actual door-arrival targets verified by camera reverse-projection.
 *
 * scd_read_be_s16 below is RETAINED only for the genuinely mixed-endian cases
 * that survive (Member_cmp 0x3E value path, Member_set non-coordinate IDs,
 * Se_on positional cues, Plc_rot angle) — NOT as a general SCD-operand rule. */
static int16_t scd_read_be_s16(const uint8_t *p)
{
    return (int16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

/* Helper: little-endian s16 reader — the DEFAULT for SCD coordinate, heading
 * and control-flow operands (R3000 `lh`/`lhu` are LE). Plc_dest (0x40) X/Z,
 * which the PSX.EXE handler at 0x80041BE4 reads with `lhu`, are raw
 * libgte-compatible halfwords passed straight to the actor's +0x1BC/+0x1BE
 * dest fields. (corrected 2026-06-08: the old "all-BE 35-agent rule holds for
 * SCD-level args" note was WRONG — LE is the rule; BE survives only for a few
 * genuinely mixed-endian value paths.) */
static int16_t scd_read_le_s16(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* ========================================================================= */
/* Opcode implementations                                                     */
/* ========================================================================= */

/* 0x00 — Nop: advance PC by 1, continue */
static int op_nop(scd_thread_t *t)
{
    t->pc++;
    return 1;
}

/* 0x01 — Evt_end: terminate thread (or pop frame) */
static int op_evt_end(scd_thread_t *t)
{
    t->pc++;
    return 0;   /* pop frame, exit if depth==0 */
}

/* 0x02 — Evt_next: yield this thread for one tick. Advances PC past the
 * 1-byte opcode and returns 2 (= yield, resume next tick at this PC).
 * Used inside do-while wait-loops (sub09/sub10): the script burns one
 * frame per iteration, giving other systems (Plc_dest walker, animation,
 * input) time to update state before the next condition re-evaluation. */
static int op_evt_next(scd_thread_t *t)
{
    t->pc += 1;
    return 2;   /* yield → next tick resumes after Evt_next */
}

/* 0x11 — Do: begin a do-while block.
 * Encoding: [0x11][_][block_length BE u16].
 *
 * Stores the post-header PC (start of loop body) on the do_stack so
 * Edwhile can jump back. block_length is the byte count of body + the
 * trailing Edwhile + condition opcode; we don't strictly need it because
 * Edwhile finds its own footing, but it's preserved for skipping the
 * entire block in future revisions (e.g. break support). */
static int op_do(scd_thread_t *t)
{
    /* [#8] Do (LAB_8003f8bc) — 4-byte header, NO predicate. Counter++, push
     * loop-back = body (pc+4), exit = body + block_length (lh @pc+2). */
    int16_t block_len = scd_read_le_s16(t->pc + 2);
    if (t->loop_count < 4) {
        int idx = t->loop_count++;
        const uint8_t *body = t->pc + 4;
        t->loop_back[idx] = body;
        t->loop_exit[idx] = body + block_len;
        /* PSX also writes a break-flag byte op[+4] → thread+depth*4+0xc+idx; no
         * port opcode reads thread+0x4 yet — omitted (NOCH OFFEN). */
    }
    t->pc += 4;
    return SCD_R_CONTINUE;
}

/* 0x12 — Edwhile: end-of-body marker.
 * Encoding: [0x12][condition_length].
 *
 * After Edwhile, the bytecode contains the loop condition (typically a
 * 4-byte Ck (0x21) opcode). We evaluate that condition; if TRUE → jump
 * back to the matching Do's body start; if FALSE → pop the do_stack and
 * advance PC past the condition.
 *
 * The Java disassembler stores `condition_length` at offset +1; here we
 * use it as the byte count to skip when falling through. We support the
 * common Ck case directly (most Sub-script wait loops poll a flag).    */
static int op_edwhile(scd_thread_t *t)
{
    /* [#8] Edwhile (LAB_8003f930) — 2-byte header + AND/OR predicate chain.
     * chain_len = pc[1]; each link runs via the real dispatcher. TRUE → jump to
     * loop-back (+0x20); FALSE → Counter-- and fall through (PC sits at chain end).
     * Replaces the old Ck-only special case (now byte-true for any predicate). */
    uint8_t chain_len = t->pc[1];
    t->pc += 2;
    int loop = scd_eval_pred_chain(t, chain_len);
    if (loop && t->loop_count > 0) {
        t->pc = t->loop_back[t->loop_count - 1];
    } else if (t->loop_count > 0) {
        t->loop_count--;
    }
    return SCD_R_CONTINUE;
}

/* 0x0D — For: begin a counted loop.
 * Encoding: [0x0D][_pad][block_length LE u16][count LE u16]. Total 6 bytes.
 *
 * AO4-round 2026-05-26: implemented to fix heli takeoff. ROOM1170
 * sub06/07/08 use this for `for(n; n<count; n++) { Add_speed; ... }`.
 * Before: 0x0D fell through to op_unknown (advance 6 bytes), body ran
 * once, thread terminated — heli body moved only 50 units once and
 * the takeoff was invisible. Rotors meanwhile lifted off because
 * sub04/sub05's infinite Goto-loop kept integrating the rotor prop's
 * vel_y that sub07/sub08 had set (and never reset) — visual symptom
 * was "rotors fly away, body stays put".
 *
 * Endianness: count and block_length are little-endian halfwords on
 * PSX (consistent with all other s16 reads per AO3-round). Sub06's
 * `0D 00 06 00 9C 00` decodes as block_length=6, count=156. */
static int op_for(scd_thread_t *t)
{
    /* [#8] For (LAB_8003f540) — 6-byte header [0x0D][_][block_len LE][count LE].
     * Counter++, loop-back = body (pc+6), exit = body + block_len, For-count = count.
     * count==0 → skip the block via block_len (kept as a defensive port early-out;
     * byte-true For would push count=0 → Next underflows to 0xFFFF = 65536 iters,
     * which no known RE1.5 script relies on and the dispatcher safety-cap bounds). */
    int16_t  block_len = scd_read_le_s16(t->pc + 2);
    uint16_t count     = (uint16_t)scd_read_le_s16(t->pc + 4);
    const uint8_t *body = t->pc + 6;
    if (count == 0) {
        t->pc = body + block_len;
        return SCD_R_CONTINUE;
    }
    if (t->loop_count < 4) {
        int idx = t->loop_count++;
        t->loop_back[idx]    = body;
        t->loop_exit[idx]    = body + block_len;
        t->loop_for_cnt[idx] = count;
    }
    t->pc += 6;
    return SCD_R_CONTINUE;
}

/* 0x0E — Next: end of For block.
 * Encoding: [0x0E][_pad]. Total 2 bytes.
 *
 * Decrement top-of-stack count. If still > 0, jump back to body start;
 * else pop and fall through past the Next opcode. */
static int op_next(scd_thread_t *t)
{
    /* [#8] Next (LAB_8003f674) — 2 bytes. Decrement For-count; !=0 → jump to
     * loop-back (+0x20); ==0 → Counter-- and PC += 2. */
    if (t->loop_count > 0) {
        int idx = t->loop_count - 1;
        if (t->loop_for_cnt[idx] != 0) t->loop_for_cnt[idx]--;
        if (t->loop_for_cnt[idx] != 0) {
            t->pc = t->loop_back[idx];
            return SCD_R_CONTINUE;
        }
        t->loop_count--;
    }
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* 0x1B — For2 (LAB_8003f5d0): a For loop whose iteration COUNT comes from a script WORK VARIABLE
 * instead of a literal — the dynamic-count sibling of op_for. 6-byte header:
 *   [0x1B][_] [block_len LE s16 @+2] [_ @+4] [var_idx u8 @+5]
 * block_len is read @+2 with `lh` exactly like For (@0x8003f5f4). The count is
 * `work_vars[var_idx]` read UNSIGNED (`lhu a1, 0x800b0fd0 + var_idx*2` @0x8003f620 — DAT_800b0fd0
 * = g_scd.work_vars, the same array op_cmp/op_ck read). Everything else (the loop-frame push, and
 * the Next/Break that pop it) is identical to For, so op_next/op_break need no change.
 * count==0 skips the block (same defensive port early-out as op_for; byte-true For2 would push a
 * 0 count -> Next decrements-then-tests so it underflows to 0xFFFF = 65536 iterations, which no
 * known RE1.5 script relies on and the dispatcher safety-cap bounds). */
static int op_for2(scd_thread_t *t)
{
    int16_t  block_len = scd_read_le_s16(t->pc + 2);
    uint16_t count     = (uint16_t)g_scd.work_vars[t->pc[5]];
    const uint8_t *body = t->pc + 6;
    if (count == 0) {
        t->pc = body + block_len;
        return SCD_R_CONTINUE;
    }
    if (t->loop_count < 4) {
        int idx = t->loop_count++;
        t->loop_back[idx]    = body;
        t->loop_exit[idx]    = body + block_len;
        t->loop_for_cnt[idx] = count;
    }
    t->pc += 6;
    return SCD_R_CONTINUE;
}

/* [#8] Byte-true predicate chain (While LAB_8003f6f4 @0x8003f78c-810, Edwhile
 * LAB_8003f930 @0x8003f964-9ec). Evaluates the opcode chain over `chain_len`
 * bytes starting at t->pc. Each link runs via the REAL dispatcher s_op_table[op]
 * (= PSX `jalr PTR_LAB_800744a8`) and advances t->pc itself; predicate handlers
 * return SCD_R_CONTINUE(true)/SCD_R_IF_FALSE(false) → normalize to 0/1 BEFORE the
 * bitwise AND/OR. Between links a u16 connector: ==0 → AND, !=0 → OR (PSX
 * `and/or s2,s2,v0`). `<` guard (PSX uses `!=`) protects against a misaligned /
 * op_unknown link overshooting chain_end. */
static int scd_eval_pred_chain(scd_thread_t *t, uint8_t chain_len)
{
    const uint8_t *chain_end = t->pc + chain_len;
    int acc = (s_op_table[*t->pc](t) == SCD_R_CONTINUE) ? 1 : 0;
    while (t->pc < chain_end) {
        uint16_t conn = (uint16_t)t->pc[0] | ((uint16_t)t->pc[1] << 8);
        t->pc += 2;
        if (t->pc >= chain_end) break;
        int v = (s_op_table[*t->pc](t) == SCD_R_CONTINUE) ? 1 : 0;
        acc = conn ? (acc | v) : (acc & v);
    }
    return acc;
}

/* [#8] While (LAB_8003f6f4) — 4-byte header + predicate chain. Counter++, loop-back
 * = the While header (Ewhile jumps back here to re-test), exit = (pc+4)+block_len.
 * Chain TRUE → fall into the body; FALSE → PC = exit, Counter--. */
static int op_while(scd_thread_t *t)
{
    uint8_t chain_len = t->pc[1];
    int16_t block_len = scd_read_le_s16(t->pc + 2);
    int idx = -1;
    if (t->loop_count < 4) {
        idx = t->loop_count++;
        t->loop_back[idx] = t->pc;                 /* re-enter While → re-test */
        t->loop_exit[idx] = (t->pc + 4) + block_len;
    }
    t->pc += 4;
    if (scd_eval_pred_chain(t, chain_len)) return SCD_R_CONTINUE;   /* TRUE → body */
    if (idx >= 0) { t->pc = t->loop_exit[idx]; t->loop_count--; }   /* FALSE → exit */
    return SCD_R_CONTINUE;
}

/* [#8] Ewhile (LAB_8003f878) — 2 bytes, UNCONDITIONAL loop-back to the While header
 * (which re-tests), then Counter-- (balances While's re-entry ++). */
static int op_ewhile(scd_thread_t *t)
{
    if (t->loop_count > 0) {
        int idx = t->loop_count - 1;
        t->pc = t->loop_back[idx];
        t->loop_count--;
        return SCD_R_CONTINUE;
    }
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* [#8] Break (LAB_8003fca8) — 2 bytes. Jump to the innermost loop's exit (+0x60)
 * and pop (Counter--). (PSX also restores a break-flag byte to thread+0x4 — no
 * port consumer found yet, omitted.) */
static int op_break(scd_thread_t *t)
{
    if (t->loop_count > 0) {
        int idx = t->loop_count - 1;
        t->pc = t->loop_exit[idx];
        t->loop_count--;
        return SCD_R_CONTINUE;
    }
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* 0x04 — Evt_exec: spawn sub_scd[sub_id] in an event slot.
 * Encoding: [op, cond, _, sub_id]. Byte-true (LAB_8003f2a0 → FUN_8003ee3c):
 * cond = (u8)PC[1] (lbu @0x8003f2b0), sub_id = (u8)PC[3] (lbu @0x8003f2b4 — the
 * SINGLE byte at pc[3]; pc[2] is unused, ROOM1170 has it 0x18). PC advances 4
 * BEFORE the spawn (@0x8003f2b8/bc), then FUN_8003ee3c(cond, sub_id) picks the
 * slot.
 *
 * [#10] Slot selection = FUN_8003ee3c in normal mode (DAT_800b3f7a==0 — the
 * agent-verified room-gameplay default; the !=0 alt mode is set only by special
 * routines):
 *   cond < 10  → use cond DIRECTLY as the slot (sltiu<0xa @0x8003ee54/58), reinit
 *                it unconditionally (PSX overwrites a busy slot via FUN_8003edec).
 *                Asset: ROOM1021 sub02 Evt_exec(9,0) → slot 9 (was DROPPED before
 *                this fix — cond=9 was neither 0xFF nor in [10..23]).
 *   cond >= 10 → auto-allocate the first free event slot. PSX normal mode scans
 *                [2..9] (+fallback 9); the port reserves slot 2 and uses a wider
 *                event range [SCD_EVENT_SLOT_FIRST..LAST] — the exact PSX range is
 *                coupled to the deferred #24 thread-pool rework (24→14), so the
 *                port scans its own event range here. ROOM1170's Evt_exec(255,11)
 *                intro cinematic stays in this range (no regression).
 *
 * ROOM1170 main00 fires Evt_exec(255, sub11) — the intro cinematic (the
 * disassembler-numbered "sub14" file maps to SCD table index 11). */
static int op_evt_exec(scd_thread_t *t)
{
    uint8_t  cond   = t->pc[1];
    uint8_t  sub_id = t->pc[3];
    t->pc += 4;                 /* advance BEFORE spawn (byte-true @0x8003f2b8) */

    if (!s_current_rdt) return 1;
    if (sub_id >= RE15_RDT_MAX_SUB_SCD) return 1;
    const uint8_t *target_pc = s_current_rdt->sub_scd[sub_id];
    if (!target_pc) return 1;   /* no such sub → drop */

    int slot;
    if (cond < SCD_EVENT_SLOT_FIRST) {        /* cond < 10 → direct slot */
        slot = cond;
        g_scd.threads[slot].active = 0;       /* force overwrite (byte-true reinit) */
    } else {                                   /* cond >= 10 → auto-allocate */
        slot = -1;
        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
            if (!g_scd.threads[s].active) { slot = s; break; }
        }
        if (slot < 0) return 1;               /* all event slots busy → drop */
    }

    scd_thread_start(slot, target_pc);
    return 1;
}

/* 0x09 — Sleep: read u16 from PC+2 into sleep slot, then yield.
 *               PC advances 1 byte (so Sleeping at PC+1 picks up the slot).
 * 0x0A — Sleeping: decrement counter at sleep slot; PC advances 3 when done. */
static int op_sleep(scd_thread_t *t)
{
    /* Sleep advances PC by 1 (past opcode byte). Sleeping reads u16 at
     * Sleeping_pc+2 (i.e. original Sleep_pc+3).
     *
     * Phase 4.5.13-E correction: RE1.5 PSX.EXE @0x8003F3E0 reads the
     * duration via MIPS `lhu` = LOAD HALFWORD UNSIGNED = LITTLE-ENDIAN
     * on PSX (the CPU is LE). Our previous BE read was wrong: the Java
     * disassembler-reported "Sleep(5120)" stems from a Java-side BE
     * interpretation of bytes the game itself reads as LE. The actual
     * encoded value is 256× smaller — Sleep(5120) is really 20 ticks,
     * Sleep(12800) is 50 ticks, Sleep(25600) is 100 ticks.
     *
     * At 30 Hz SCD VM tick, that's 0.67s / 1.67s / 3.3s — matches
     * dialog-pacing intuition. The old comment claiming BE was correct
     * was based on incorrect "feels too short" reasoning. */
    t->pc++;
    uint16_t duration = (uint16_t)t->pc[1] | ((uint16_t)t->pc[2] << 8);
    int slot = t->sleep_count[(int)t->call_depth];
    if (slot < SCD_SLEEP_SLOT_COUNT) {
        t->sleep_slots[(int)t->call_depth][slot] = duration;
        t->sleep_count[(int)t->call_depth] = (uint8_t)(slot + 1);
    }
    return 1;   /* continue → next opcode is Sleeping at t->pc */
}

static int op_sleeping(scd_thread_t *t)
{
    int slot = t->sleep_count[(int)t->call_depth] - 1;
    if (slot < 0) {
        /* No sleep slot — malformed, just advance and continue */
        t->pc += 3;
        return 1;
    }
    uint16_t *counter = &t->sleep_slots[(int)t->call_depth][slot];
    /* BO-round 2026-05-29 (Tier-3 #6 fix): Sleep(N) yields EXACTLY N ticks —
     * the next opcode fires on tick N+1, NOT on the tick the counter reaches 0.
     * PSX LAB_8003f428 advances pc+3 + frees the slot, then falls through to
     * LAB_8003f488 where the branch-delay `ori v0,zero,0x2` makes the return
     * UNCONDITIONALLY 2 (yield). Our old `return 1` ran the next opcode in the
     * SAME tick → every Sleep was 1 tick short → cumulative cut-timing drift
     * (intro #6). Now return 2 here too. */
    if (*counter > 0) (*counter)--;
    if (*counter == 0) {
        /* Cast: slot is signed int (from `count - 1`), sleep_count[] is u8.
         * slot is guaranteed >= 0 here (we returned early on slot < 0) and
         * always < SCD_SLEEP_SLOT_COUNT (16) so it fits in u8 cleanly. */
        t->sleep_count[(int)t->call_depth] = (uint8_t)slot;
        t->pc += 3;
        return 2;
    }
    return 2;
}

/* 0x17 — Goto: [op offset_lo offset_hi] — signed 16-bit relative jump from PC */
static int op_goto(scd_thread_t *t)
{
    /* Goto = 6 bytes: [op flag0 flag1 _pad rel_lo rel_hi] per
     * info293.txt + PSX-spec (information73.txt:412-419). The signed
     * 16-bit relative jump lives at pc[4..5] LE, NOT pc[1..2] (which
     * are unknown flag/cond fields — Java disassembler line 885-888
     * also records them as "field0/field1"). Verified vs sub04.scd:
     *
     *   sub04 @ 0x0A: 17 FF FF 00 FE FF
     *     - pc[1..2] = FF FF (flags, IGNORED) — old code read this
     *       as -1 → loop landed on Evt_next → infinite yield, rotor
     *       Add_speed never re-fired after first tick.
     *     - pc[4..5] = FE FF LE = -2 (TRUE rel_jump) → loop lands on
     *       Add_speed @ 0x08 → rotor spins each tick as intended.
     *
     * AO-round 2026-05-26 (rotor non-spin investigation): 5/5 RE
     * agents independently identified this as the bug. */
    /* [#17] byte-true Goto unwind (LAB_8003fb9c @0x8003fbc0-bf60): pc[1] = the
     * If-block nesting level to unwind TO (PSX block_sp = base + (s8)pc[1]*4 + 4 →
     * flat per-frame index (s8)pc[1]+1; empty = -1 → 0); pc[2] = the loop counter to
     * restore (PSX thread+frame+0x8). EMPIRICAL: 15/22 real Gotos use pc[1]!=0xFF
     * (a real out-of-block jump that MUST unwind, else the next If's FALSE-pop
     * targets a stale block_end); pc[2] is 0xFF in all 22 (→ loop_count reset to 0).
     * The verified rotor Goto (sub04 `17 FF FF 00 FE FF`) has pc[1]=pc[2]=0xFF and no
     * surrounding block → unwind to 0 = no-op (no regression). */
    int8_t  unwind_to = (int8_t)t->pc[1];   /* block level (-1 = empty) */
    int8_t  loop_to   = (int8_t)t->pc[2];   /* loop count  (-1 = empty) */
    int16_t off = (int16_t)((uint16_t)t->pc[4] | ((uint16_t)t->pc[5] << 8));
    t->pc += off;
    int cd  = (int)t->call_depth;
    int cap = (int)(sizeof(t->block_stack[cd]) / sizeof(t->block_stack[cd][0]));
    int bs  = (int)unwind_to + 1;
    if (bs < 0)   bs = 0;
    if (bs > cap) bs = cap;
    t->block_sp[cd] = (int8_t)bs;
    int lc = (int)loop_to + 1;
    if (lc < 0) lc = 0;
    if (lc > 4) lc = 4;
    t->loop_count = (int8_t)lc;
    return 1;
}

/* 0x18 — Gosub: [op offset_lo offset_hi] — push return PC, jump */
/* 0x18 — Gosub: 2 bytes [op, sub_id u8]. Per Java SCDScriptDisassembler
 * line 254 (`registerOpcode(0x18, "Gosub", 2, ...)`) and decompiler
 * rendering as `goto SubN()`, the arg is an INDEX into the RDT's
 * sub_scd[] table, NOT a PC-relative offset. Used by sub00 (`18 0F` →
 * goto Sub15) and sub02 (`goto Sub09/Sub10` arrival-wait loops).
 *
 * Calls into the sub-script in the SAME thread (push return PC on the
 * call stack so op_return pops back). On call_stack overflow we skip
 * the gosub silently to avoid corrupting unrelated state. */
static int op_gosub(scd_thread_t *t)
{
    uint8_t sub_id = t->pc[1];
    const uint8_t *target = (s_current_rdt && sub_id < RE15_RDT_MAX_SUB_SCD)
                          ? s_current_rdt->sub_scd[sub_id] : NULL;
    if (target && t->call_depth + 1 < SCD_CALL_DEPTH_MAX) {
        t->call_stack[(int)t->call_depth] = t->pc + 2;
        t->call_depth++;
        /* [#17] byte-true Gosub (LAB_8003fbe8): the callee frame gets a FRESH,
         * EMPTY If-block stack (PSX inits the new frame's block-level to -1 and
         * rebases block_sp). The caller's block_stack[caller_depth] is untouched
         * and restored implicitly on Return (call_depth--). */
        t->block_sp[(int)t->call_depth] = 0;
        t->pc = target;
    } else {
        /* target missing or call stack overflow — advance past gosub */
        t->pc += 2;
    }
    return 1;
}

/* 0x19 — Return: pop call stack */
static int op_return(scd_thread_t *t)
{
    if (t->call_depth > 0) {
        t->call_depth--;
        t->pc = t->call_stack[(int)t->call_depth];
    } else {
        t->active = 0;
    }
    return 1;
}

/* (2026-06-09) The old pre-evaluation look-ahead `scd_eval_condition` was
 * REMOVED — the byte-true block-stack model (op_if pushes block_end + the
 * predicate opcodes return their boolean; FUN_8003f0a0 pops on FALSE)
 * replaces it. See op_if below + op_ck/op_cmp/op_member_cmp/op_sce_key_ck. */

/* 0x06 — If (Ifel_ck): byte-true block-stack model (LAB_8003f328 + dispatcher
 * FUN_8003f0a0, 2026-06-09 — replaces the prior pre-evaluation look-ahead):
 *   block_length = lhu @pc[2] (LE);  block_end = (pc + 4) + block_length;
 *   push block_end onto the thread block-stack;  ALWAYS enter the body (pc += 4).
 * The body's FIRST opcode is the predicate (Ck 0x21 / Cmp 0x23 / Member_cmp 0x3E
 * / Sce_key_ck 0x51); it runs as a real opcode and RETURNS its boolean — TRUE→1
 * (body runs), FALSE→SCD_R_IF_FALSE → the dispatcher pops the block-stack and
 * jumps PC to block_end. Else(0x07)/End_if(0x08) pop on the TRUE path. block_end
 * = the opcode AFTER the closing structure (past End_if, or the else-body start
 * for if/else) — the same PC target as the old `pc += block_length + 4`, but now
 * reached via the canonical predicate-return path so nested/multi-predicate and
 * block-stack interactions match the original. LE verified ROOM1170 main00 @0x9E
 * `06 00 2C 00`: LE 0x2C=44 → block_end 0xA2+0x2C=0xCE (next real opcode). */
static int op_if(scd_thread_t *t)
{
    uint16_t block_length = (uint16_t)t->pc[2] | ((uint16_t)t->pc[3] << 8);
    const uint8_t *block_end = t->pc + 4 + block_length;
    int cd  = (int)t->call_depth;   /* [#17] block stack is per Gosub-frame */
    int cap = (int)(sizeof(t->block_stack[cd]) / sizeof(t->block_stack[cd][0]));
    if ((int)t->block_sp[cd] < cap) t->block_stack[cd][(int)t->block_sp[cd]++] = block_end;
    t->pc += 4;                 /* always enter the body; the predicate decides */
    return SCD_R_CONTINUE;
}

/* 0x07 — Else_ck: 4 bytes [op, _reserved, block_length LE u16].
 * Reached at end of an if-body when its condition was TRUE — must
 * unconditionally skip past the else-body.
 *
 * Skip formula: `pc += block_length` (no +4 offset, unlike Ifel_ck).
 * Verified at ROOM1170 main00 +0xCA (skip 52 → +0xFE = Break, then
 * Break+EndSwitch reach +0x102 = Door_aot_set(4)) and +0x13E (skip 38
 * → +0x164 = Door_aot_set(6)). The +0xCA case also works with +4 but
 * the +0x13E case overshoots into mid-opcode garbage with +4. */
static int op_else(scd_thread_t *t)
{
    /* byte-true LAB_8003f368: pop the block-stack (the If that opened this), then
     * PC = pc + else_block_length (skip the else-body on the TRUE path). */
    int16_t skip = (int16_t)((uint16_t)t->pc[2] | ((uint16_t)t->pc[3] << 8));
    int cd = (int)t->call_depth;   /* [#17] per Gosub-frame */
    if (t->block_sp[cd] > 0) t->block_sp[cd]--;
    t->pc += skip;
    return SCD_R_CONTINUE;
}

/* 0x08 — End_if: byte-true LAB_8003f3a4 — pop the block-stack, PC += 2. */
static int op_end_if(scd_thread_t *t)
{
    int cd = (int)t->call_depth;   /* [#17] per Gosub-frame */
    if (t->block_sp[cd] > 0) t->block_sp[cd]--;
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* 0xFE — Debug text (custom): [op x y len text...] writes text via re15_debug_text */
static int op_dbg_text(scd_thread_t *t)
{
    int x = t->pc[1];
    int y = t->pc[2];
    int len = t->pc[3];
    char buf[64];
    if (len > 63) len = 63;
    for (int i = 0; i < len; i++) buf[i] = (char)t->pc[4 + i];
    buf[len] = '\0';
    re15_debug_text(x, y, 0, buf);
    t->pc += 4 + len;
    return 1;
}

/* ----- Phase 4.4.2 opcodes ---------------------------------------------- */

/* 0x29 — Cut_chg: camera change. 4 bytes [op cam_id arg2 arg3].
 * Records request into VM state; render layer reads and clears. */
static int op_cut_chg(scd_thread_t *t)
{
    /* Per Java disassembler line 231: Cut_chg is 2 bytes [op, cam_id]. */
    g_scd.cam_id_prev       = g_scd.cam_id;
    g_scd.cam_id            = t->pc[1];
    g_scd.cam_arg2          = 0;
    g_scd.cam_arg3          = 0;
    g_scd.cam_change_pending = 1;
    /* byte-true LAB_800402a0 @0x800402d4 `ori DAT_800aca3c,0x100` -> Bit 0x100 SET =
     * Auto-Cam-Scan AUS (Gate @0x8001cce0 `andi 0x100`->bne ueberspringt jal FUN_80014230).
     * cut_auto_enabled ist die Port-Inverse des Bits. [#16] */
    g_scd.cut_auto_enabled = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[scd F%d] Cut_chg(%u)\n",
            (int)g_engine.frame_count, (unsigned)t->pc[1]);
#endif
    t->pc += 2;
    return 1;
}

/* 0x2B — Message_on: on-screen message. 4 bytes [op msg_id arg2 arg3].
 * Records into VM state, immediate fall-through (does NOT block thread). */

/* AW-round 2026-05-28: replace the hardcoded 90-frame display with the
 * per-message duration computed from the .msg file's embedded text-control
 * codes (PSX dialog FSM at 0x80028134). The host registers its msg lookup
 * via re15_msg_duration_provider; if NULL, fall back to 90. */
typedef int (*re15_msg_duration_fn)(uint8_t msg_id);
static re15_msg_duration_fn s_msg_duration_provider = 0;
void re15_scd_set_msg_duration_provider(re15_msg_duration_fn fn)
{
    s_msg_duration_provider = fn;
}

/* Show the current Message_on (set state + queue the dialogue voice + compute the
 * on-screen duration). Shared by the subtitle + query paths. */
static void msg_show(scd_thread_t *t)
{
    g_scd.message_id     = t->pc[1];
    g_scd.message_arg2   = t->pc[2];
    g_scd.message_arg3   = t->pc[3];
    g_scd.message_active = 1;
    /* NOTE: the dialogue voiceover is NO LONGER queued here. msg_show is shared by the
     * plain-subtitle path AND the YES/NO query prompt (the ROOM1130 switch), and a UI
     * prompt must NOT speak. Voice is queued ONLY by the plain-subtitle caller, gated to
     * the room that actually has authored voice. (Bug: VOICE files are the global room1170
     * intro clips indexed by message_id; a non-1170 room's message_id collided → e.g. the
     * 1130 switch spoke Elliot's intro line. See re15_scd_queue_voice below.) */
    int dur = 90;
    if (s_msg_duration_provider) {
        int d = s_msg_duration_provider(t->pc[1]);
        if (d > 0) dur = d;
    }
    g_scd.message_display_frames = (uint16_t)dur;
}

/* Show a .msg line by index directly (no SCD thread) — used by EXAMINE/MESSAGE AOTs
 * (the ROOM1130 back-door "It's not necessary to go back"). Same state writes as
 * msg_show: a plain subtitle that auto-dismisses after its own duration. */
void re15_scd_show_message(uint8_t index)
{
    /* FE-4: EXAMINE-AOT direct message path — same save-point check as op_message_on. A
     * save-point phone opens the save MENU instead of the flavor message: request the menu
     * and DON'T open a dialog. Opening the typewriter dialog here was the freeze source —
     * re15_dialog_open arms a wait-for-button FSM (message_fsm_active, msg_common.c:496) that
     * the modal menu then hides, leaving Leon frozen behind it until dismissed; and the
     * platform had to force-clear it, which reopened the msg_block re-examine gate → the
     * menu↔room flicker. Skipping the dialog closes both. (No re-examine cooldown: the AOT
     * fires on a fresh action-button EDGE only, so each deliberate press = one menu open.) */
    if (re15_savepoint_is(g_current_room_id, index)) {
        re15_savepoint_latch_loc(g_current_room_id);  /* Ortsindex (Patch-Analog AOT_TYPE1_HOOK
                                                       * @0x8007087c: Telefon-Pfad schreibt den
                                                       * Index VOR dem Kartenmenue) */
        re15_savepoint_set_pending(1);
        return;
    }
    /* ITEM BOX (RE1.5-hybrid, default-on like the save system): the safe-room box
     * AOT's "Itembox is not available in this preview" message opens the BOX screen
     * instead (same interception level as the save phone above). The byte-true
     * shipped message behavior stays available under RE15_BOX_PREVIEW_MSG=1
     * (test/dev flag; itembox_spec.md §6 trigger decision). */
    if (!getenv("RE15_BOX_PREVIEW_MSG") && re15_itembox_is(g_current_room_id, index)) {
        re15_itembox_set_pending(1);
        return;
    }
    /* EXAMINE / MESSAGE-AOT line (e.g. the ROOM1130 back-door "It's not necessary to go
     * back") → the SAME byte-true typewriter FSM as dialog, NON-blocking: it types out and
     * waits for the action button (or its own end-hold) to dismiss. No voiceover. */
    re15_dialog_open((int)index, 0);
    g_scd.message_arg2 = 0;
    g_scd.message_arg3 = 0;
}

/* Rooms with authored dialogue VOICE (VOICE##.VAG indexed by message_id). DATA-DRIVEN —
 * add a voiced room = one table row. (A future per-room (message_id → voice_id) remap, for
 * rooms that reuse message ids with different voice clips, would live here too.) */
extern unsigned g_current_room_id;
static int re15_room_has_voice(unsigned room_id)
{
    static const unsigned voiced[] = { 0x1170 };   /* helipad intro (the only voiced room so far) */
    for (unsigned k = 0; k < sizeof(voiced)/sizeof(voiced[0]); k++)
        if (voiced[k] == room_id) return 1;
    return 0;
}

/* #1A (2026-07-02): rooms whose Message_on captions render FULL-TEXT (all-at-once), NOT the
 * per-glyph typewriter — the cinematic INTRO captions (ROOM1240 pre-intro narrator sub02 msg
 * ids 0..5; ROOM1170 helipad). [FL-observed] — matched to the user's direct observation of the
 * ORIGINAL showing full-text. The statically-visible EXE printer (Message_on 0x2B → LAB_800404f4
 * hardcodes param_2=0x300 → FUN_80027e68 sets DAT_800b8521=0 unconditionally → FUN_80028134) is
 * byte-true TYPEWRITER-ONLY with no full-text branch, so the original's full-text captions come
 * from a path absent from ghidra1_V2.txt (DEBUG.BIN @0x800C0000, savestate-only). Byte-true closure
 * needs a live ROOM1240 pre-intro savestate to read DAT_800b8521/8524/852c — see RE15_ROOM_FIXES.md #1A.
 * Separate from re15_room_has_voice: 1240's narrator has NO VOICE##.VAG, so it must NOT queue voice. */
int re15_room_full_text(unsigned room_id)
{
    static const unsigned ft[] = { 0x1170, 0x1240 };
    for (unsigned k = 0; k < sizeof(ft)/sizeof(ft[0]); k++)
        if (ft[k] == room_id) return 1;
    return 0;
}

static int op_message_on(scd_thread_t *t)
{
    extern uint8_t g_aot_action_pressed;
    uint8_t arg2 = t->pc[2];

    /* FE-4: a PHONE save-point message ("You can save your progress with this. Save is not available
     * in this preview") — the port REPLACES it with the working save MENU, so open the menu and DO
     * NOT display the message at all. Return here (advancing past the 4-byte Message_on) so the
     * typewriter dialog below never runs — otherwise this SCD-thread path (GENERIC AOT → sub →
     * Message_on) shows the dormant "not available" flavor text while the direct MESSAGE-AOT path
     * (re15_scd_show_message) opens the menu, which is exactly the "sometimes menu, sometimes
     * message" split the user sees. Placed HERE (the single Message_on entry) because msg_show
     * only runs for the full-text cinematic rooms {0x1170,0x1240}; the phones take the plain
     * typewriter path (re15_dialog_open) below. (No re-examine cooldown: the examine AOT fires
     * on a fresh action-button EDGE only, so each deliberate press = one menu open.) */
    if (re15_savepoint_is(g_current_room_id, t->pc[1])) {
        re15_savepoint_latch_loc(g_current_room_id);  /* Ortsindex (Patch-Analog SCD_SAVE_RET
                                                       * @0x800708c0: Schreibmaschinen-Pfad) */
        re15_savepoint_set_pending(1);
        if (getenv("RE15_MSG_LOG"))
            fprintf(stderr, "[msg] room=%04x id=%d SAVEPOINT (menu, message suppressed)\n",
                    g_current_room_id, t->pc[1]);
        t->pc += 4;   /* skip the flavor message — the save menu is its replacement */
        return 1;
    }
    /* ITEM BOX (RE1.5-hybrid, default-on): the box AOT's SCD sub fires Message_on
     * with the "Itembox is not available in this preview" flavor text (e.g. ROOM1150
     * sub07 -> msg 3) — suppress it and request the BOX screen, exactly like the save
     * phone above. RE15_BOX_PREVIEW_MSG=1 = the byte-true shipped message instead. */
    if (!getenv("RE15_BOX_PREVIEW_MSG") && re15_itembox_is(g_current_room_id, t->pc[1])) {
        re15_itembox_set_pending(1);
        if (getenv("RE15_MSG_LOG"))
            fprintf(stderr, "[msg] room=%04x id=%d ITEMBOX (box screen, message suppressed)\n",
                    g_current_room_id, t->pc[1]);
        t->pc += 4;   /* skip the flavor message — the box screen is its replacement */
        return 1;
    }
    if (getenv("RE15_MSG_LOG"))
        fprintf(stderr, "[msg] room=%04x id=%d savepoint=0\n", g_current_room_id, t->pc[1]);

    /* YES/NO QUERY: a selection prompt that BLOCKS the SCD thread until the player confirms.
     * BYTE-TRUE TRIGGER (RE'd 2026-06-14 from FUN_80027e68 / FUN_80028134): the YES/NO state
     * is entered SOLELY by the .msg body's 0x03 control code (caseD_3 → FSM state 4), NOT by
     * arg2. Message_on's pc[2..3] are the message COLOR (opcode 0x2B: param_2=0x300 const,
     * param_4 = pc[2..3]<<16 = color), NOT a flag — a plain message and a query both carry
     * pc[2]=0x80. So gate on re15_msg_is_choice() ALONE. We park at this opcode (yield WITHOUT
     * advancing PC) and run a wait FSM via g_scd.message_query: 1 = wait for the opening action
     * button to RELEASE (debounce), 2 = wait for a fresh PRESS = the answer. Plain messages
     * keep the PSX-canon non-blocking fall-through below. (void)arg2 — kept for the comment. */
    (void)arg2;
    /* CHOICE message (its .msg carries a 0x03 yes/no code): BLOCK the SCD thread and run the
     * byte-true typewriter FSM (re15_dialog_open + re15_dialog_step, driven by re15_msg_tick).
     * We park here each frame and poll message_active; the FSM types the pages (action button
     * advances each 0x02 page break, HELD fast-forwards), then the 0x03 YES/NO writes (12,31)
     * and clears message_active → we unblock. The painting "A beautiful painting [▼] There's a
     * switch below [▼] Will you push it? Yes/No" is the canonical case. */
    if (re15_msg_is_choice(t->pc[1])) {
        if (g_scd.message_query == 0 && !g_scd.message_fsm_active) {
            re15_dialog_open((int)t->pc[1], 1);    /* blocking: query=1, fsm_active=1 */
            g_scd.message_arg2 = t->pc[2];
            g_scd.message_arg3 = t->pc[3];
            return 2;                              /* park (PC not advanced) */
        }
        if (g_scd.message_active)                  /* FSM still running (driven by re15_msg_tick) */
            return 2;                              /* keep parking */
        /* FSM done: the YES/NO answer is already written to flag (12,31). Unblock. */
        g_scd.message_query = 0;
        t->pc += 4;
        return 1;                                  /* → Evt_next + the YES/NO branch */
    }

    /* Plain line. FULL-TEXT cinematic captions (the intro: ROOM1240 pre-intro narrator + ROOM1170
     * helipad) use the legacy all-at-once timed display; every OTHER room's EXAMINE / gameplay text
     * uses the byte-true per-glyph typewriter FSM, NON-blocking (the thread continues; the FSM types
     * it out + dismisses on the action button / its own hold). VOICE is queued only for rooms that
     * actually have authored VOICE##.VAG (re15_room_has_voice = {0x1170}) — NOT 1240's narrator (msg
     * ids 0..5 have no voice clip; queuing would collide with 1170's voice bank). See #1A above. */
    if (re15_room_full_text(g_current_room_id)) {
        msg_show(t);                               /* full-text all-at-once (message_fsm_active=0) */
        g_scd.message_fsm_active = 0;
        if (re15_room_has_voice(g_current_room_id)) {
            scd_audio_event_t vev;
            memset(&vev, 0, sizeof vev);
            vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
            vev.sample_id = t->pc[1];
            scd_audio_queue_push(&vev);
        }
    } else {
        re15_dialog_open((int)t->pc[1], 0);        /* non-blocking typewriter */
        g_scd.message_arg2 = t->pc[2];
        g_scd.message_arg3 = t->pc[3];
    }
    t->pc += 4;
    return 1;
}

/* 0x13 — Switch: 4-byte header [0x13][var_index][block_length:u16 LE].
 *
 * [#9] BYTE-TRUE (LAB_8003fa5c, dispatch 0x800744f4). Unlike the earlier port,
 * the PSX Switch handler does the ENTIRE table scan itself and jumps straight to
 * the matching Case body / Default body / past-Eswitch — the standalone Case
 * (0x14) handler is reached only on C-style fall-through (see op_case).
 *
 * Sequence (all addresses in ghidra1_V2.txt, verified instruction-by-instruction):
 *   - var_index = pc[1] (@0x8003fa78 lbu), block_length = u16 pc[2..3]
 *     (@0x8003fa74 lhu), table = pc+4 (@0x8003fa7c addiu a3,a3,0x4).
 *   - PUSH onto the unified loop model exactly like Do (LAB_8003f8bc): increment
 *     loop_count (@0x8003fa90/94) and store loop_exit[idx] = (pc+4)+block_length
 *     (@0x8003faa8/ac). This lets Break (0x1A) and the standalone Eswitch (0x16)
 *     pop a switch the same way they pop a loop. (PSX also saves a break-flag byte
 *     to thread+0x4 — no port consumer yet, omitted, same as op_do.)
 *   - cmp_val = (s16) work_vars[var_index] via `lh` (@0x8003fad8, signed).
 *   - Scan from `table` (LAB_8003fadc):
 *       0x15 Default  → pc = default_pos+2  (@0x8003faf0), loop_count STAYS pushed.
 *       0x16 Eswitch  → pc = eswitch_pos+2  (@0x8003fb20), loop_count POPPED
 *                       (@0x8003fb24/28: no Case matched and no Default).
 *       0x14 Case     → block_len=u16@+2 (@0x8003fafc lhu), value=s16@+4
 *                       (@0x8003fb00 lh). Match (cmp==value): pc=case_pos+6
 *                       (@0x8003fb0c), loop_count STAYS pushed. No match:
 *                       a3 += 6 + block_len (@0x8003fb0c +6 then @0x8003fb14
 *                       +block_len) → next entry, keep scanning. */
static int op_switch(scd_thread_t *t)
{
    uint8_t        var_index = t->pc[1];
    uint16_t       block_len = (uint16_t)scd_read_le_s16(t->pc + 2);
    int16_t        cmp_val   = g_scd.work_vars[var_index]; /* lh — signed, u8 idx in range */
    const uint8_t *table     = t->pc + 4;

    /* PUSH switch context (mirror op_do; loop_back/for_cnt unused for a switch). */
    int pushed = 0;
    if (t->loop_count < 4) {
        int idx = t->loop_count++;
        t->loop_exit[idx] = table + block_len;
        pushed = 1;
    }

    /* Scan the Case/Default/Eswitch table (LAB_8003fadc). */
    const uint8_t *a3 = table;
    for (;;) {
        uint8_t op = a3[0];
        if (op == SCD_OP_DEFAULT) {            /* 0x15 → default body */
            a3 += 2;
            break;
        }
        if (op == SCD_OP_END_SWITCH) {         /* 0x16 → no match, no default */
            a3 += 2;
            if (pushed) t->loop_count--;       /* pop the just-pushed context */
            break;
        }
        /* 0x14 Case: block_len u16 @+2 (lhu), value s16 @+4 (lh). */
        uint16_t case_len = (uint16_t)scd_read_le_s16(a3 + 2);
        int16_t  case_val = scd_read_le_s16(a3 + 4);
        if (cmp_val == case_val) {             /* match → enter case body */
            a3 += 6;
            break;
        }
        a3 += 6 + case_len;                    /* skip header + body → next entry */
    }
    t->pc = a3;
    return SCD_R_CONTINUE;
}

/* 0x14 — Case standalone: 6-byte header [0x14][_][block_length:u16][value:u16].
 *
 * [#9] BYTE-TRUE (LAB_8003fb38): PC += 6, NOTHING else — no compare, no
 * loop_count change. Switch (0x13) already scans the table and jumps to the
 * matching body, so this handler is only reached on C-style fall-through (a
 * matched Case body runs off its end into the NEXT Case opcode); skipping the
 * 6-byte header drops execution into that next body. */
static int op_case(scd_thread_t *t)
{
    t->pc += 6;
    return SCD_R_CONTINUE;
}

/* 0x15 — Default standalone: 2-byte header [0x15][_].
 * [#9] BYTE-TRUE (LAB_8003fb50): PC += 2 (was +4). Reached only on fall-through
 * into a Default body. */
static int op_default(scd_thread_t *t)
{
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* 0x16 — Eswitch: 2-byte marker [0x16][_].
 * [#9] BYTE-TRUE (LAB_8003fb68): POP the switch context (loop_count--) then
 * PC += 2 (was: advance only). Reached when a matched Case body runs to the end
 * of the switch without a Break, balancing the Switch push. */
static int op_end_switch(scd_thread_t *t)
{
    if (t->loop_count > 0) t->loop_count--;
    t->pc += 2;
    return SCD_R_CONTINUE;
}

/* ----- Phase 4.4.3 audio opcodes ----------------------------------------- */

/* 0x36 — Se_on (12 bytes). KORRIGIERTES Layout per LAB_80041624-Disasm
 * (ghidra1_V2.txt:151737-151815; Dossier analysis/rolltor_sound.md D3, CONFIRMED):
 *   +1 bank            (lbu a3,0x1)
 *   +2/+3 id|FLAGS     (lh a0,0x2 — low byte = sample id, HIGH byte = FLAGS; das Packing an
 *                       FUN_80045024 ist (bank<<24)|(id&0xff)<<16|(a0>>8) @0x80041718-34.
 *                       flags != 0 -> Positional-Pfad FUN_80045a64 (@0x800451c0-cc) — OPEN)
 *   +4/+5 Modus|Pool   (lh a1,0x4 — Ursprungs-Modus 0..5: Entity-Pool 0x800acc2c+idx*0x1f4
 *                       bzw. Objekt-Pool 0x800b3f98+idx*0x94; Position = Ursprung + Offsets)
 *   +6..+11 Offsets    (lhu, LE s16 x/y/z — ADDIERT auf den Ursprung)
 * Die alte Deutung "+3 volume, +4 pan" war falsch (das RE15_SE_DEBUG-Log "vol=0 pan=1" zeigte
 * in Wahrheit flags=0 und Ursprungs-Modus 1). Se_on hat KEINE Volume-/Pan-Operanden — Volume
 * kommt aus dem Tone (+2), Pan aus dem Positional-Pfad. */
static int op_se_on(scd_thread_t *t)
{
    scd_audio_event_t evt = {0};
    evt.kind      = SCD_AUDIO_SE_ON;
    evt.bank      = t->pc[1];
    evt.sample_id = t->pc[2];
    if (getenv("RE15_SE_DEBUG"))
        fprintf(stderr, "[se] SCD Se_on: bank=%u id=%u flags=%u origin=%u/%u\n",
                t->pc[1], t->pc[2], t->pc[3], t->pc[4], t->pc[5]);
    evt.volume    = 0;                       /* kein Volume-Operand (s.o.) — Tone[+2] entscheidet */
    evt.pan       = 0;                       /* kein Pan-Operand — Positional-Pfad (OPEN)        */
    evt.raw_w0    = (uint16_t)((t->pc[3] << 8) | t->pc[4]);   /* FLAGS | Ursprungs-Modus */
    evt.raw_w1    = (uint16_t)t->pc[5];                       /* Pool-Index              */
    /* Positional cues = LITTLE-ENDIAN s16 world coords (sound-source X,Y,Z,
     * offset from a listener origin). Verified vs PSX handler FUN_80041624:
     * +6/+8/+10 are each read with `lhu` (R3000 LE), no byte-swap, then passed
     * to the positional-audio emit FUN_80045024. (corrected 2026-06-08: was
     * scd_read_be_s16 — same wrong "BIO1 BE" assumption as Pos_set/Dir_set.
     * STAGE3+ rooms DO use nonzero pos, e.g. room3091/sub07 raw 46 a0..dc 05 →
     * LE=(-24506,0,1500) matches the room band; BE=(18080,-9211) is garbage.) */
    evt.pos_x = scd_read_le_s16(&t->pc[6]);
    evt.pos_y = scd_read_le_s16(&t->pc[8]);
    evt.pos_z = scd_read_le_s16(&t->pc[10]);
    enqueue_audio(&evt);
    t->pc += 12;
    return 1;
}

/* (0x57) — byte-true IDENTIFIED (audit wf_1db9c802 AUDIO-OP57-IS-FADE-NOT-BGM): the handler
 * @0x80042ab4 reads ch = pc[1] (lbu) and step = ONE u16 LE @pc[2..3] (lhu) and calls
 * FUN_800216ec(ch, step, 0, NULL) = write `step` into fade channel `ch` of the 0x44-stride
 * SCREEN-FADE array @DAT_800b5458 (the fade engine FUN_80021880: level += step, brightness =
 * level>>7 — a VISUAL op, not audio; the old "Sce_bgmtbl_set" name was RE2-only and the u16
 * was split into two wrong byte fields). PC += 4. The port stores the decoded step in the
 * g_scd fade-channel model; RENDER consumption is the fade/letterbox session (#21) — until
 * then the state is correctly decoded but visually unconsumed (ROOM11F0/11F1 + ROOM3080). */
static int op_fade_adjust(scd_thread_t *t)
{
    /* CORRECTED semantics (trace wf_2c73ab52, self-verified @0x800216ec): the u16 is the START
     * LEVEL, not the step — FUN_800216ec writes it to the channel level only when the configured
     * step (0x56) is 0 (static overlay, brightness = level>>7); with a nonzero step the value is
     * IGNORED and the ramp auto-starts at 0 / 0x7fff. All shipped SCD fades are static pulses. */
    re15_fade_kick(t->pc[1], (uint16_t)scd_read_le_s16(&t->pc[2]));
    t->pc += 4;
    return 1;
}
/* Runtime flag-op census (env RE15_FLAG_CENSUS): logs every Ck/Set/indexed flag op with the
 * current room so the STAGE1 progression map is built from the byte-true VM (no offline SCD
 * walker desync). READ = Ck/0x58 (gates a door/event), WRITE = Set/0x59 (sets a flag). */
static int flag_census_on(void)
{
    static int c = -1;
    if (c < 0) c = getenv("RE15_FLAG_CENSUS") ? 1 : 0;
    return c;
}

/* 0x59 — byte-true LAB_8003fe90 (@0x800744a8[0x59] -> 0x8003fe90) is an INDEXED FLAG-BANK
 * BIT SET/CLEAR/TOGGLE, NOT Xa_on (an RE2 misattribution; the handler has zero XA/CD/SPU
 * interaction — audit wf_1db9c802 AUDIO-OP59-NOT-XA). It is the WRITE sibling of the 0x58
 * check (op_flag_ck2): idx = work_vars[pc[2]] (lhu @0x8003feb8, DAT_800b0fd0), bank = pc[1]
 * (pointer table @0x80074664 via pc[1]*4 lw @0x8003fed0), word = (s16)idx >> 5 (sll16/sra21
 * @0x8003fed4-dc), mask = 0x80000000 >> (idx & 0x1f) (lui 0x8000 @0x8003fe94 + srlv);
 * pc[3] == 1 -> SET (or/sw @0x8003ff3c-4c), 0 -> CLEAR (nor/and @0x8003ff24-38),
 * 7 -> TOGGLE (xor @0x8003ff50-5c). PC += 4. The old op_xa_on queued a spurious XA audio
 * event and silently dropped ROOM1030/1031's scripted flag write. Same word-range guard as
 * op_flag_ck2 (the port flag model covers idx 0..255 per zone, the identical MSB-first
 * convention). */
static int op_flag_set2(scd_thread_t *t)
{
    uint8_t bank = t->pc[1];
    int16_t raw  = g_scd.work_vars[t->pc[2]];
    int     word = (int)raw >> 5;
    uint8_t op   = t->pc[3];
    if (word >= 0 && word < RE15_FLAG_WORDS_ZONE && bank < RE15_FLAG_ZONES) {
        uint8_t idx = (uint8_t)raw;
        if (flag_census_on())
            fprintf(stderr, "[flagcensus] room=%04X WRITE 0x59 bank=%d idx=%d op=%d\n",
                    g_current_room_id, bank, idx, op);
        if      (op == 1) re15_game_flag_set(bank, idx, 1);
        else if (op == 0) re15_game_flag_set(bank, idx, 0);
        else if (op == 7) re15_game_flag_set(bank, idx, re15_game_flag_get(bank, idx) ? 0 : 1);
        /* other op values: no write (the original's switch falls through) */
    }
    t->pc += 4;
    return 1;
}

/* 0x80 — Se_vol (2 bytes). Global SFX volume scale. Args: +1 scale */
static int op_se_vol(scd_thread_t *t)
{
    scd_audio_event_t evt = {0};
    evt.kind   = SCD_AUDIO_SE_VOL;
    evt.volume = t->pc[1];
    enqueue_audio(&evt);
    t->pc += 2;
    return 1;
}

/* === Phase 4.4.4: flags + player state ====================================
 *
 * Ck (0x21) / Set (0x22): RE1.5 game-flag bit ops. 4-byte encoding:
 *   [op, flag_zone, flag_index, flag_value]
 *
 * `flag_zone` selects which 32-flag bank (= 4 bytes of g_game.flags), so
 * the absolute bit index is (flag_zone * 32 + flag_index). RE1.5 zones
 * 0..15 cover 512 bits total = our RE15_GAME_FLAGS_BYTES * 8 = 512.
 *
 * Ck stores its boolean result into locals[0] by convention so the
 * existing op_if (which tests locals[flag_byte] != 0) can be the
 * follow-up: emit `Ck zone idx val; If 0 ... skip`. Phase 4.4.5+ will
 * extend this to a proper sub-conditional chain matching RE2's
 * FUN_80053f50, but the single-Ck-then-If pattern is by far the most
 * common in extracted scripts. */
/* Ck (0x21): test flag bit, store boolean into locals[0] for next op_if.
 * Set (0x22): write flag bit.
 *
 * Phase 4.5.9-F fix per A18: zone selects a per-zone bit-table (RE2 layout),
 * idx is a bit-index up to 255 inside that table — NOT `zone*32+idx`.
 * Real scripts use idx > 31 routinely (e.g. ROOM1021 Ck(3,133,1)), which
 * the old packing collided into the wrong zone. */
static int op_ck(scd_thread_t *t)
{
    uint8_t  zone  = t->pc[1];
    uint8_t  idx   = t->pc[2];
    uint8_t  value = t->pc[3];
    int      got   = re15_game_flag_get(zone, idx);
    if (flag_census_on())
        fprintf(stderr, "[flagcensus] room=%04X READ  Ck   zone=%d idx=%d exp=%d got=%d\n",
                g_current_room_id, zone, idx, value, got);
    /* byte-true LAB_8003fcf4: cond = (flag!=0) XOR (value==0)  (@0x8003fd48 `sltu`
     * = flag-Bool, @0x8003fd50 `xor` mit a1=(value==0)@0x8003fd30). Korrigiert den
     * value>=2-Fall; identisch zum alten (got==value) fuer value in {0,1}. [#19] */
    int      cond  = ((got != 0) ^ (value == 0)) ? 1 : 0;
    t->locals[0]   = (uint8_t)cond;   /* vestigial; op_if no longer reads it */
    t->pc += 4;
    /* byte-true: Ck IS the predicate (LAB_8003fcf4 returns (flag!=0)^(expected==0)).
     * Return the boolean as the dispatch code — TRUE→run the If body, FALSE→the
     * dispatcher pops the block-stack and jumps to the If's block_end. */
    return cond ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}

static int op_set(scd_thread_t *t)
{
    /* Byte-true (LAB_8003fdd0): pc[3] is an OPERATION SELECTOR, not a boolean —
     * 1 → OR (set bit), 0 → AND/clear bit, 7 → XOR (toggle), any other value → NO-OP.
     * (Verified 2026-06-09 vs the disasm; the old `value ? 1 : 0` matched only because
     * ROOM1170 issues op 0/1 exclusively.) */
    uint8_t  zone = t->pc[1];
    uint8_t  idx  = t->pc[2];
    uint8_t  op   = t->pc[3];
    if (flag_census_on())
        fprintf(stderr, "[flagcensus] room=%04X WRITE Set  zone=%d idx=%d op=%d\n",
                g_current_room_id, zone, idx, op);
    if      (op == 1) re15_game_flag_set(zone, idx, 1);                           /* OR     */
    else if (op == 0) re15_game_flag_set(zone, idx, 0);                           /* clear  */
    else if (op == 7) re15_game_flag_set(zone, idx, re15_game_flag_get(zone, idx) ? 0 : 1); /* toggle */
    /* else 2..6: no-op, exactly like the original */
    t->pc += 4;
    return 1;
}

/* Pos_set (0x32) — set actor world position. 8-byte encoding:
 *   [op, register, x(2), y(2), z(2)]  (each s16 LE — corrected 2026-06-08;
 *   the old "_be" labels were the disproven all-BE assumption, see below)
 *
 * TARGET: byte-true LAB_80041048 writes X/Y/Z to the WORK ENTITY (thread+0x154, selected by the
 * preceding Work_set) — pc[1] is NOT read as a target register (the old "pc[1]==0→player, else
 * no-op" model was disproven 2026-06-09; re-confirmed by the stub-opcode audit wf_9143c15b). The
 * impl below routes to work_slot (actor) or work_prop_idx (prop). See the body comment.
 *
 * Endianness: LITTLE-ENDIAN s16 (R3000 `lh`). The earlier "BE" claim was
 * CIRCULAR — it only confirmed BE *reads* x=17106, never that 17106 is the
 * correct position. It is NOT: room1170/sub14 puts Leon there for the outdoor
 * dialog under cut 12, whose camera frames (-11916,-29804); BE x=17106 z=31896
 * lands Leon at camera-space z=-56049 (BEHIND the camera → every triangle
 * near-clipped → NO model on screen, the user's "model nicht da" bug). LE
 * decodes the SAME bytes to (-11710,-7200,-26500) = EXACTLY door 0's arrival
 * target (the courtyard Leon just entered) and sits at cut 12's look-at →
 * visible. Same class + same symptom as Sce_em_set positions (LE-verified by
 * screenshot reverse-projection: "BE puts them off-pad far away") and Plc_dest
 * X/Z (PSX.EXE 0x80041BE4 `lhu`). The "all-BE" 35-agent rule was a wrong
 * assumption; every forensic check has resolved to LE. (2026-06-08) */
static int op_pos_set(scd_thread_t *t)
{
    /* Byte-true (LAB_80041048): writes X/Y/Z to the WORK ENTITY (thread+0x154, selected
     * by the preceding Work_set) — pc[1] is NOT read as a target register. (Verified
     * 2026-06-09; the old `if(pc[1]==0)→hardcoded player` coincided for ROOM1170 because
     * its single Pos_set (sub14) follows Work_set(1,0)→player.) Coords are LE. */
    int16_t x = scd_read_le_s16(&t->pc[2]);
    int16_t y = scd_read_le_s16(&t->pc[4]);
    int16_t z = scd_read_le_s16(&t->pc[6]);
    int8_t ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    if (ws >= 0 && ws < RE15_ACTOR_MAX) {
        g_actors[ws].x = (int32_t)x;
        g_actors[ws].y = (int32_t)y;
        g_actors[ws].z = (int32_t)z;
    } else {
        int8_t pi = (t->work_prop_idx >= 0) ? t->work_prop_idx : g_scd.work_prop_idx;
        if (pi >= 0 && pi < (int)g_scd.prop_count && g_scd.props[pi].active) {
            g_scd.props[pi].x = (int32_t)x;
            g_scd.props[pi].y = (int32_t)y;
            g_scd.props[pi].z = (int32_t)z;
        }
    }
    t->pc += 8;
    return 1;
}

/* Dir_set (0x33) — set actor/prop heading. Byte-true LAB_80041080 (@0x80041080): writes ALL THREE
 * rotation axes (rot_x=pc[2], rot_y=pc[4], rot_z=pc[6], each LE s16) to the WORK ENTITY (thread+0x154,
 * selected by the preceding Work_set) at +0x68/+0x6a/+0x6c — pc[1] is NEVER read as a target register.
 *   80041080 lw v1,28(a0)=pc ; 80041084 lw a1,340(a0)=work ; lhu 2(v1)->sh 104(a1) (rot_x) ;
 *   lhu 4(v1)->sh 106(a1) (rot_y) ; lhu 6(v1)->sh 108(a1) (rot_z) ; addiu v1,8 (pc+=8).
 * This mirrors the already-fixed op_pos_set. The OLD port form (read pc[1] as reg, gate on reg==0,
 * hardcode the player, decode only rot_y) coincided for ROOM1170 (Dir_set after Work_set(1,0)->player,
 * pc[1]==0) but mis-targeted + dropped fields elsewhere: ROOM11F0 sub[17] does 10x `Work_set(3,N);
 * Dir_set 33 00 00 00 00 00 00 02` to rotate props 2..11 to rot_z=0x200 — the port instead set the
 * PLAYER's rot_y to 0 ten times and left every prop unrotated. (ROOM11D0/11D1 sub[2] the same.) */
static int op_dir_set(scd_thread_t *t)
{
    int16_t rx = scd_read_le_s16(&t->pc[2]);
    int16_t ry = scd_read_le_s16(&t->pc[4]);
    int16_t rz = scd_read_le_s16(&t->pc[6]);
    int8_t ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    if (ws >= 0 && ws < RE15_ACTOR_MAX) {
        g_actors[ws].rot_x = rx;
        g_actors[ws].rot_y = ry;
        g_actors[ws].rot_z = rz;
    } else {
        int8_t pi = (t->work_prop_idx >= 0) ? t->work_prop_idx : g_scd.work_prop_idx;
        if (pi >= 0 && pi < (int)g_scd.prop_count && g_scd.props[pi].active) {
            g_scd.props[pi].rot_x = rx;
            g_scd.props[pi].rot_y = ry;
            g_scd.props[pi].rot_z = rz;
        }
    }
    t->pc += 8;
    return 1;
}

/* Plc_motion (0x3F) — set player animation clip. 4-byte encoding:
 *   [0x3F, motion_id, arg2, arg3]
 *
 * motion_id maps to an EDD clip index that the renderer cycles through.
 * arg2/arg3 are flags (looping mode, blending) — captured into upper
 * bits of player_motion for later use. */
static int op_plc_motion(scd_thread_t *t)
{
    /* 4-byte encoding per Java SCDScriptDisassembler.java:253 generic
     * parser: [op, arg1=entity, arg2=motion_id, arg3=flags].
     * In sub02/sub14 ROOM1170 the call is `Plc_motion(0, 15, 0)` =
     * entity=0 (player), motion_id=15 (rbj clip 15 = "Hey!" gesture).
     * Earlier this read pc[1] which is the entity byte (always 0),
     * making every Plc_motion silently switch to clip 0 — masked by
     * main.c's startup `motion = 15` which made the bug invisible. */
    uint8_t  entity     = t->pc[1];
    /* T-round (2026-05-25): per PSX disasm 0x80041bac/bc8 of Plc_motion handler,
     * pc[2] is the motion_id (low byte → actor+0x94) and pc[3] is the
     * anim flag word (high byte → actor+0x1c4). Was incorrectly read as
     * one 16-bit motion id — values 256+ wrapped to wrong clip. */
    uint8_t  motion_id  = t->pc[2];
    uint8_t  anim_flags = t->pc[3];
    /* Phase 4.5.13-RE2 (2026-05-21): route to correct actor slot.
     * Previously this hard-coded slot 0 (player), which meant sub-scripts
     * controlling NPCs (e.g. Elliot's gesture commands) hijacked Leon's
     * animation. RE2's handler reads actor* from thread context @+0x154
     * (per-thread "current actor"); we approximate via the entity byte:
     *   entity == 0 → player (slot 0)
     *   entity != 0 → use g_scd.work_slot if a recent Work_set selected
     *                 a NPC slot; else fall back to entity as slot index.
     * This is a closer match to RE2 semantics than the previous always-
     * player routing.                                                  */
    /* I-round fix (2026-05-24, post-diagnostic):
     * Per F11 canonical PSX.EXE disasm at 0x80041b90, Plc_motion ALWAYS
     * targets the per-thread work_slot (lw v0, 0x154(a0)), NEVER the
     * entity arg (pc[1] = substate byte, written to actor+0x05).
     *
     * Symptom that justified this fix: live debug.log shows Elliot
     * (NPC slot 1) frozen at motion=0 T-pose for entire intro while
     * Leon (player) plays ALL of sub02's 11 Plc_motion gestures (wave,
     * talk, etc.). User confirmed visually — Elliot is a T-pose statue
     * while Leon "in his place" does Elliot's "Hey!" wave.
     *
     * Sub02 does Work_set(2, 0) before its Plc_motion calls, intending
     * to select Elliot. Our Work_set now treats kind=2 same as kind=3
     * (NPC pool), so work_slot = 1 = Elliot when sub02 fires. */
    int slot;
    /* Read PER-THREAD work_slot (was global, stomped by parallel sub04/05). */
    if (t->work_slot >= 0 && t->work_slot < RE15_ACTOR_MAX) {
        slot = t->work_slot;
    } else if (entity == 0) {
        slot = RE15_ACTOR_SLOT_PLAYER;
    } else {
        slot = (entity < RE15_ACTOR_MAX) ? entity : RE15_ACTOR_SLOT_PLAYER;
    }
    /* Ein Plc_motion beendet einen laufenden Spieler-Event-Reach (Mode-6-Halt): das Original
     * ueberschreibt +0x5 (Sub) mit pc[1] — der 0x800517f0-Idle laeuft nicht weiter. */
    if (slot == RE15_ACTOR_SLOT_PLAYER) {
        extern void re15_player_event_reach_end(void);
        re15_player_event_reach_end();
    }
    re15_actor_set_motion(&g_actors[slot], (int16_t)motion_id);
    /* Player animation-bank select (see re15_actor.h + memory anim_bank_selection_mechanism for
     * the full RE). The entity operand is a RESULT-CORRECT fold-vs-hold selector: entity!=0 →
     * PL00.EDD (the original's COMMON bank; ROOM1150 kneel clip 11 = the real stand→kneel fold
     * py -1810→-761, and clip-11 DOWN/UP in sub08); entity==0 → the room RBJ overlay (ROOM1170
     * gestures, and clip-11 HOLD/settle in sub08). The entity byte does NOT pick the bank in the
     * original (it only lands in actor+0x05) but co-varies with it in all shipped data; clip 11
     * is deliberately authored in both banks (PL00.EDD=fold, RBJ=settle). */
    g_actors[slot].anim_use_pl00 = (entity != 0) ? 1 : 0;
    /* BYTE-TRUE Plc_motion actor retarget (opcode handler @0x80041b90, disasm + savestate +
     * audit wf_29b40e5d): the original does NOT freeze/yield — it puts the actor in the shared
     * STATE-4 EXECUTOR and lets it run EVERY frame:
     *   @0x80041bb0  sb v1(=4),4(v0)    -> +0x4 (state)       = 4   (executor 0x80050be8)
     *   @0x80041bc4  sb a2(=pc[1]),5(v0)-> +0x5 (sub_state_1) = pc[1]  (=0 for the shipped poses -> the
     *                                       motion phase-FSM sub-table[0]=0x80050cb8 = re15_npc_sub_motion)
     *   @0x80041bb4  sb zero,6(v0)      -> +0x6 (phase)       = 0
     * The executor's motion-FSM plays +0x94 once and HOLDS (phase 2) OR breathe-LOOPS if the LOOP flag
     * +0x1c4 & 0x04 is set (ROOM1150's SCD sets it via Plc_flg(0,4,0) -> the lying NPC's subtle idle
     * loop, hardware-confirmed). Because the NPC is now in state 4, the INIT (state 0) never re-runs to
     * stomp +0x94 back to idle-2 — the byte-true reason a posed NPC keeps its pose clip. (Replaced the
     * earlier scd_anim_owned dispatch-yield proxy, which wrongly HELD ROOM1150's breathe-loop; the two
     * blockers it hit are now fixed: spawn anim_flags=0 @0x8004216c, and the executor is the single
     * frame-advancer via the re15_actors_anim_advance state-4 skip.)
     * Player (slot 0) is not NPC-ticked — its Plc_motion gestures drive the player anim path — so only
     * NPC slots are retargeted. */
    if (slot != RE15_ACTOR_SLOT_PLAYER) {
        g_actors[slot].state       = 4;        /* +0x4 = 4 (executor)          @0x80041bb0 */
        g_actors[slot].sub_state_1 = entity;   /* +0x5 = pc[1] (sub)           @0x80041bc4 */
        g_actors[slot].sub_state_2 = 0;        /* +0x6 = 0 (motion phase reset) @0x80041bb4 */
    }
    /* AP-round 2026-05-26 (per PSX disasm @0x80041b90): Plc_motion sets
     * state=4 which the FSM at 0x80050cdc translates to state=1 with
     * `sb zero, +0x95` (= anim_frame reset to 0). So a Plc_motion call
     * ALWAYS restarts the clip from frame 0, even if motion_id matches
     * the currently-playing motion. This is what makes sub02's pattern
     * work:
     *   Plc_motion(15);   // start forward
     *   Sleep(7680);       // 30 ticks forward play
     *   Plc_motion(15);    // RESTART → anim_frame=0 again
     *   Plc_flg(0,128,0); // set REVERSE bit
     *   Sleep(7680);       // 30 ticks reverse play
     * The second Plc_motion seeds the reverse phase from a clean cur=0.
     * Per agent verification (ghidra1_V2.txt:152125-152146). */
    g_actors[slot].anim_frame = 0;
    /* Plc_motion → state=4 → the per-frame motion FSM seeds the FRAC crossfade
     * (FUN_8001f3bc +0x8f=7), UNLESS the no-blend bit 0x40 is set. BYTE-TRUE: the bit is in
     * the FLAGS word +0x1c4 (= pc[3] = anim_flags, ghidra1_V2.txt:176982/152140), NOT the
     * motion id pc[2]. (No effect on room1150 clip 10 where pc[3]=0; correct for clips that
     * set 0x40.) Cutscene clips need this blend: clip 10's frame-0 is ALREADY the full kneel,
     * so the kneel-DOWN is produced ENTIRELY by the crossfade while the frame counter advances
     * the blend target — the two stepping 1:1 stages bend-then-reach. */
    /* Byte-true crossfade seed = 7 (disasm: all ~27 entity+0x8f sites = ori 0x7, unit 0x200).
     * The crossfade only smooths the FIRST few frames of a motion change; the body's actual
     * motion (e.g. the ROOM1150 stand→kneel pelvis fold) lives in the CLIP keyframes — which is
     * why playing the correct bank (PL00.EDD clip 11, set above via anim_use_pl00) is what makes
     * the kneel descend smoothly, NOT a longer crossfade. */
    if (!(anim_flags & 0x40)) g_actors[slot].anim_frac = 7;
    /* T-round: pc[3] high byte → actor.anim_flags (RE2 +0x1c4).
     * Plc_motion UNCONDITIONALLY overwrites anim_flags (clears prior
     * REVERSE bit etc.) — Plc_flg AFTER Plc_motion is what re-installs
     * the bit for reverse playback. */
    g_actors[slot].anim_flags = anim_flags;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[scd] Plc_motion(entity=%u, motion=%u, flags=0x%02X) → slot=%d\n",
            (unsigned)entity, (unsigned)motion_id, (unsigned)anim_flags, slot);
#endif
    t->pc += 4;
    return 1;
}

/* === Phase 4.5.13-B: Plc_dest (0x40) — walk-to waypoint ===================
 *
 * 8-byte encoding. Verified against PSX.EXE handler @ 0x80041BE4:
 *   [+0]  op (0x40)
 *   [+1]  register     (0 = player, !=0 = NPC slot)
 *   [+2]  hi byte = motion mode (0x05 RUN, 0x07 sub5b, 0x08 sub6,
 *                                0x09 TURN/IDLE; via 6-entry jump table
 *                                at RAM 0x80010DCC selecting per-mode
 *                                clip LUT). Stored to actor+0x05.
 *   [+3]  lo byte = animation slot (stored to actor+0x1C3); also serves
 *                   as the arrival-flag bit index (low nibble convention).
 *   [+4..5] x dest, LE s16  (PSX.EXE: lhu $v0, 4($v0); sh $v0, 0x1BC)
 *   [+6..7] z dest, LE s16  (PSX.EXE: lhu $v1, 6($v0); sh $v1, 0x1BE)
 *
 * There is NO Y coordinate — Plc_dest is 2D pathfinding (XZ plane);
 * Y is supplied by animation root-motion / floor height.
 *
 * Game behavior: actor walks toward (x, z) at the speed implied by the
 * motion mode; on arrival the engine sets flag(5, lo_byte) = 1. SCD
 * scripts poll via the paired Sub09 / Sub10 gosubs.
 *
 * Empirical correlation in ROOM1170 sub02:
 *   bytes 00 09 20 → hi=09 (TURN) lo=0x20=32 → Sub09 polls Ck(5,32) ✓
 *   bytes 00 05 20 → hi=05 (RUN)  lo=0x20=32 → Sub09                ✓
 *   bytes 00 05 21 → hi=05 (RUN)  lo=0x21=33 → Sub10 polls Ck(5,33) ✓
 *   bytes 00 09 21 → hi=09 (TURN) lo=0x21=33 → Sub10                ✓
 *
 * Minimal-but-correct semantics: instant teleport + immediate arrival
 * flag set. Sub09/Sub10 then return on first poll, cinematic proceeds. */
static int op_plc_dest(scd_thread_t *t)
{
    uint8_t  reg       = t->pc[1];
    uint8_t  mode      = t->pc[2];  /* 0x05 RUN, 0x09 TURN, etc. */
    uint8_t  flag_bit  = t->pc[3];
    int16_t  x         = scd_read_le_s16(&t->pc[4]);
    int16_t  z         = scd_read_le_s16(&t->pc[6]);

    /* Plc_dest @ 0x80041be4 is STASH-ONLY + clip pre-select (corrected by the
     * 2026-06-09 deep-RE — the earlier "handlers write Speed 75/96/200 to +0x8c"
     * note was WRONG): the opcode writes dest (+0x1bc/+0x1be), mode (+0x05), the
     * completion-flag id (+0x1c3), and pre-selects the motion clip into +0x1c8 from
     * a per-mode byte table — then RETURNS. The actual walking is a SEPARATE per-frame
     * player FSM (state DAT_800aca59 = mode → table 0x80073e30: 4=WALK 0x80030af0,
     * 5=RUN 0x80030d28, 9=TURN 0x80031360). The 75/96/200 literals are YAW TURN-RATES
     * (a2 to FUN_8001aac4/ab9c), NOT speeds; the translation speed is actor+0x8c
     * (MoveSpeed, authored outside Plc_dest) integrated by FUN_800245d8 as
     * RotMatrixY(yaw)·(0,0,speed). Our re15_actor_step_walk (actor_locomotion.c) ports
     * this: atan2 heading (FUN_8001a6d4) + fixed-rate yaw slew + arrival dist<100(WALK)/
     * <300(RUN) + mode-9 rotate-in-place + completion-flag set. For our port we set the
     * motion clip directly here by mode (RUN/WALK→walk clip, TURN→unchanged). */
    /* I2-round (2026-05-24): per-thread work_slot routing (same as Plc_motion,
     * per F11 PSX canon — Plc_dest @ 0x80041BE4 also reads thread+0x154).
     * Was hardcoded slot=PLAYER, so Elliot's scripted walk (sub02 first
     * Plc_dest after Work_set(2,0)) was making LEON walk instead. */
    int slot;
    if (t->work_slot >= 0 && t->work_slot < RE15_ACTOR_MAX) {
        slot = t->work_slot;
    } else if (reg == 0) {
        slot = RE15_ACTOR_SLOT_PLAYER;
    } else {
        slot = reg;
    }
    if (slot >= 0 && slot < RE15_ACTOR_MAX) {
        re15_actor_t *a = &g_actors[slot];
        /* MODE = der EXECUTOR-SUB (byte-true @0x80041c14-18: `sb 4,0x4(a1); sb mode,0x5(a1)` —
         * Plc_dest setzt IMMER State 4 + Sub = mode). Nur die Modi 4/5/7/8/9 sind WALK-Subs
         * (Player-Tabelle 0x80073e30 / NPC 0x80076ca0: [4]=WALK [5]=RUN [7]/[8] Varianten
         * [9]=TURN) — die POSE-/EVENT-Subs 0-3 und 6 laufen KEINEN Schritt: Sub 6 =
         * EVENT-REACH @0x800517f0 (Clip 1 einmal -> Clip-2-Idle-Loop; die dest-Felder
         * +0x1bc/+0x1be werden im ganzen Body nie gelesen). Der alte Port startete fuer JEDEN
         * Mode den Walker -> `Plc_dest(0,6,0x3f,0,0)` liess Marvin UND Leon in ROOM10D0 ewig
         * Richtung Weltursprung laufen (walk_active haengt, NPC-Tick yieldet, Marvin 1700
         * Ticks eingefroren + weggedreht — marvin_10d0.md D3, CONFIRMED). */
        int is_walk = (mode == 0x04 || mode == 0x05 || mode == 0x07 ||
                       mode == 0x08 || mode == 0x09);
        /* NPC-Familie (0x40-0x4d) laeuft AUCH die Walk-Modes byte-true in der State-4-Sub-VM
         * (@0x80041c14-18 setzt IMMER state=4/+0x5=mode; die NPC-Walk-Subs @0x80076ca0
         * [4]=0x80051148 [5]=0x80051484 [7]=0x80051908 [8]=0x80051b00 sind in
         * re15_npc_sub_walk portiert — marvin_glide_end.md Fix #1). AUSNAHME Elliot (0x47):
         * bleibt auf dem Walker-Sonderpfad (PLD-Bank/Sentinels; vor der Umstellung per
         * Savestate messen, marvin_10d0.md O3). Der Spieler behaelt seine eigene Walker-FSM
         * (Player-Tabelle 0x80073e30, separat byte-true). */
        int npc_subvm = 0;
        if (slot != RE15_ACTOR_SLOT_PLAYER) {
            uint8_t ty = a->type;
            npc_subvm = (ty >= 0x40 && ty <= 0x4d && ty != 0x47);
        }
        if (!is_walk || npc_subvm) {
            /* Re-Init-Guard (@0x80041bf8-c0c): skip nur wenn +0x1c4&4 UND +0x5 == mode. */
            int skip_init = (a->state == 4 && a->sub_state_1 == mode && (a->anim_flags & 0x04));
            a->walk_active = 0;
            a->walk_dest_x = x; a->walk_dest_z = z;       /* dest gestasht (+0x1bc/+0x1be) */
            a->steer_x = x; a->steer_z = z;               /* Sub-VM-Steer-Felder (= dieselben
                                                           * Original-Bytes +0x1bc/+0x1be) */
            a->walk_flag_bit = flag_bit;                  /* +0x1c3 */
            if (is_walk)                                  /* Port-Bookkeeping (R9): Ck(5,bit)-Polls
                                                           * duerfen keine stale Ankunft sehen */
                re15_game_flag_set(5, flag_bit, 0);
            if (!skip_init) {
                if (slot != RE15_ACTOR_SLOT_PLAYER) {
                    a->state = 4;                          /* @0x80041c14 */
                    a->sub_state_1 = mode;                 /* @0x80041c18 -> Executor-Sub */
                    a->sub_state_2 = 0;
                    a->anim_flags  = 0;                    /* sh 0,0x1c4 @0x80041c4c */
                } else {
                    /* PLAYER Mode 6: derselbe Sub @0x800517f0 (Tabelle 0x80073e30[6]) auf dem
                     * Spieler-+0x170-Kanal = die PLW-Paar-B-Bank (Clip 1 @0x80051854 einmal ->
                     * Clip 2 @0x800518c8 Loop) — dieselbe Maschine wie der Kraehen-Mode-6-Halt
                     * (crow_victim_anim.md §1.4). Engine-FSM in game_step_common. */
                    extern void re15_player_event_reach_begin(void);
                    if (mode == 0x06) re15_player_event_reach_begin();
                }
            }
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[scd] Plc_dest(slot=%d mode=0x%02X dest=(%d,%d)) -> state4/sub%d (%s)\n",
                    slot, mode, x, z, mode, is_walk ? "Sub-VM-WALK" : "POSE/EVENT, kein Walk");
#endif
            t->pc += 8;
            return 1;
        }
        if (slot == RE15_ACTOR_SLOT_PLAYER) {         /* Walk-Mode ersetzt einen Mode-6-Halt */
            extern void re15_player_event_reach_end(void);
            re15_player_event_reach_end();
        }
        a->walk_dest_x   = x;
        a->walk_dest_z   = z;
        a->walk_mode     = mode;
        a->walk_flag_bit = flag_bit;
        a->walk_active   = 1;
        /* BO-round (Tier-3 A5): start the PSX 3-state walker FSM in state 0
         * (LAB_80030AF0 / DAT_800aca5a). The variable-length align phase
         * (state 1) replaces the old fixed walk_pad=2 freeze; speed/slew/
         * arrival are driven per-state in re15_actor_step_walk. */
        a->walk_fsm      = 0;
        /* R9 critical fix: clear arrival flag so subsequent Sub10/Sub09
         * polls actually wait for THIS walker's arrival, not see a stale
         * 1 from a previous Plc_dest. Without this, all 5 Plc_dests in
         * sub02 collapsed to ~5 ticks instead of ~45s.                  */
        re15_game_flag_set(5, flag_bit, 0);

        /* BO-round (Tier-3 A2): PSX Plc_dest (LAB_80041be4) writes NO motion_id.
         * The walk/run clip is started by the walker when it enters active
         * stepping (state 1→2), so the actor STANDS during the align phase and
         * only then animates — matching Elliot's first-frame stand on ablauf
         * 217020→walk 217022. (Old AC-round motion-set here made him walk on his
         * very first visible frame.) walk_mode already carries the mode. */
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[scd] Plc_dest(slot=%d mode=0x%02X dest=(%d,%d) flag=%d) walk_active=1\n",
                slot, mode, x, z, flag_bit);
#endif
    }
    t->pc += 8;
    return 1;
}

/* === Phase 4.5.13-C: Plc_neck (0x41) — head/neck orientation ==============
 *
 * 10-byte encoding (per SCDScriptDisassembler.java case 0x41):
 *   [+0]  op (0x41)
 *   [+1]  mode  (0..4: 0=look-fwd, 2=track-target, 3=relative, 4=look-at)
 *   [+2]  yaw   (u8)
 *   [+3]  pitch (u8)
 *   [+4..5] roll   (BE s16)
 *   [+6..7] target (BE s16) — angle or target_id
 *   [+8..9] speed  (BE s16)
 *
 * Visual head/neck rotation. Mode 4 (look-at) is used in sub02 line 76
 * `Plc_neck(4, 3, 0, 0, 0, 15360)` to make Leon look at the helicopter
 * while delivering "Hey!" line. We stash the target angle on the player
 * actor's lookat field so the render path can apply it as bone[3] (head)
 * rotation. The per-bone neck FSM is now ported BYTE-TRUE (FUN_80037358 +
 * FUN_8003790c) per-frame in skeleton_common.c:311-386; the static-lookat
 * store here just feeds it the world look-at target.                  */
static int op_plc_neck(scd_thread_t *t)
{
    /* BO-round 2026-05-29 (hack audit): the operands are four s16 halfwords
     * (yaw, pitch, roll, speed) read by lhu on PSX (LAB_80041e98). The old
     * code split the FIRST s16 into two u8 "angles" (<<4) and mis-aligned the
     * rest — a fabricated conversion. Read proper s16 (LE = PSX lhu). The head-look
     * neck FSM is now ported BYTE-TRUE in skeleton_common.c:311-386 — the REAL
     * head-look is FUN_80037358 + FUN_8003790c (NOT FUN_80024e40, which is a
     * SEPARATE spring secondary-motion chain); we store the world look-at
     * target here and the skeleton slews the head bone per-frame.
     * NOTE: ROOM1170 does not use Plc_neck, so this is dead code for the intro. */
    /* BYTE-TRUE (2026-06-16, FUN_80024e40 RE): the operands are a WORLD LOOK-AT TARGET
     * (x,y,z) at +0x160/162/164, NOT raw angles; the mode picks +0x1b8 flag bits. We STORE
     * them here and let the per-frame neck FSM (re15_neck_update) do the atan2 + damped slew.
     * (ROOM1150 sub08 Plc_neck(1,-22000,-720,-26131,…) ≈ Irons' pos = "Leon look at Irons";
     * the old code wrote -22000 straight onto the head yaw → grotesque head clip through
     * Leon. Plc_neck(0,…) at the end = mode 0 = release → head eases back to forward.) */
    uint8_t mode = t->pc[1];
    /* ZIEL = die per-Thread-WORK-ENTITY (`lw v1,0x154(a0)` @0x80041e9c) — NICHT der Spieler.
     * Der alte Hardcode auf g_actors[PLAYER] leitete jedes NPC-Plc_neck (Work_set(2,N)) auf
     * Leons Kopf um — die Ursache von "Leons Kopf komisch verdreht" in ROOM10D0 (Marvins 5
     * Neck-Kommandos, u.a. mit Leons eigener Position als Ziel) und der Grund, warum NPCs nie
     * den Kopf bewegten. Routing identisch zu op_plc_motion/op_plc_dest (t->work_slot).
     * cutscene_headlook.md B1 (CONFIRMED; 158 NPC-Necks in 30 Raeumen game-wide). */
    int nslot = (t->work_slot >= 0 && t->work_slot < RE15_ACTOR_MAX)
                    ? t->work_slot : RE15_ACTOR_SLOT_PLAYER;
    re15_actor_t *p = &g_actors[nslot];
    p->neck_tx    = scd_read_le_s16(&t->pc[2]);   /* +0x160 (Welt-X / Sweep-Zaehler)   @0x80041f20-28 */
    p->neck_ty    = scd_read_le_s16(&t->pc[4]);   /* +0x162 (Welt-Y / relativer Yaw)   @0x80041f34-3c */
    p->neck_tz    = scd_read_le_s16(&t->pc[6]);   /* +0x164 (Welt-Z / Pitch-Betrag)    @0x80041f48-50 */
    p->neck_speed = scd_read_le_s16(&t->pc[8]);   /* low->+0x9e yaw / high->+0x9f pitch @0x80041f5c-6c */
    /* +0x1b8 = 0x80 | Modus-Bits (caseD_0..4 @0x80041ed8-f10): 0=|0x12 Release, 1=|0x04
     * Welt-Punkt, 2=|0x08 relativ, 3=|0x2a rel. Pitch-Sweep (Nicken), 4=|0x58 Yaw-Sweep
     * (Kopfschuetteln, Zaehler in +0x160). */
    static const uint8_t mode_bits[5] = { 0x12, 0x04, 0x08, 0x2a, 0x58 };
    p->neck_flags = (uint8_t)(0x80 | (mode < 5 ? mode_bits[mode] : 0));
    p->neck_sweep = 0;                            /* neuer Befehl -> Sweep-Latch neu armieren */
    /* RE15_NECK_LOG: prove op 0x41 fires + its target/mode (SDL stderr → void). */
    { static FILE *nl = NULL; static int nli = 0;
      if (!nli) { nli = 1; const char *pp = getenv("RE15_NECK_LOG"); if (pp && *pp) nl = fopen(pp, "w"); }
      if (nl) { fprintf(nl, "PLC_NECK slot=%d mode=%u tgt=(%d,%d,%d) speed=%d flags=0x%02x\n",
                        nslot, mode, p->neck_tx, p->neck_ty, p->neck_tz, p->neck_speed, p->neck_flags);
                fflush(nl); } }
    t->pc += 10;
    return 1;
}

/* (re15_neck_update removed — the per-frame Plc_neck head-look slew lives in
 * re15_skel_compute_pose at the head bone (bone 8), where the root bone matrix is
 * available so the look angle is computed in the correct ROOT-LOCAL frame. The old
 * body-relative slew here was retired to avoid double-slewing.) */

/* === Phase 4.5.13-D: Plc_ret (0x42) — return cinematic control ============
 *
 * 1-byte opcode. Marks the end of a cinematic block; control returns to
 * normal gameplay. The SCD thread itself continues with the trailing
 * nop+return that the disassembler emits after Plc_ret.
 *
 * Despawns scripted NPCs (Elliot, Pilot etc.). The original game achieves
 * this by walking them off-camera via Plc_dest before Plc_ret, but our
 * Plc_dest pathfinder may not move them as far as needed — and the user
 * observed Elliot still visible "in the boxes" at scene end. Force-clear
 * non-player NPCs here. Cinematic props (heli/rotor) are handled
 * separately via sub06/07/08/12/13 Speed_set/Add_speed which now route
 * per-thread (rotor follows body off-pad). */
static int op_plc_ret(scd_thread_t *t)
{
    /* Szenen-Ende beendet auch einen laufenden Spieler-Event-Reach (Mode-6-Halt). */
    { extern void re15_player_event_reach_end(void); re15_player_event_reach_end(); }
    /* BK-round 2026-05-29: Plc_ret is the canonical CINEMATIC-END handoff.
     * PSX LAB_80041f88 writes player-mode DAT_800aca58=1 → a cleanup FSM returns
     * control to the player over the next frames. We lack that multi-frame FSM,
     * so we apply its net effect immediately:
     *   - end the cinematic latch  → letterbox off, AOT-suppress lifts, player
     *     input + RVD auto-camera take over (cut3 holds until the player walks
     *     into an RVD zone; verified no cameraFrom=3 zone covers Leon's end pos
     *     (2664,-7336), so cut3 correctly holds — PSX-faithful);
     *   - drop Leon to clip-0 rest pose (PSX cleanup ends in idle; matches AV-round)
     *     so he doesn't freeze mid-gesture;
     *   - clear the lingering subtitle (msg7's timer would otherwise outlive the
     *     cut by ~170 frames — see re_cut_timing_drift_2026_05_29);
     *   - re-settle AOT edge-state at the player's pos so no door/item AOT that
     *     was suppressed during the cinematic fires spuriously the instant
     *     suppression lifts. */
    /* FAITHFUL to the original (PSX 0x80041f88): write the player work-object's
     * mode byte DAT_800aca58 = 1 (gameplay) and drop Leon out of scripted motion
     * (the original clears the walk sub-FSM scratch +5..7 → the gameplay handler
     * renders idle; our net equivalent is set_motion(0)). Then advance PC by 1.
     * Nothing else: the letterbox close and the scripted-NPC retirement are driven
     * by the cinematic-flag (1,27)/(2,7) clear in the main-loop player-mode FSM —
     * sub02 does Set(2,7,0)/Set(1,27,0) immediately before this opcode. (Removed
     * the engine-fabricated letterbox_countdown arming + the type==0x47 NPC
     * despawn sweep, which the audit flagged as op_plc_ret doing far more than the
     * canonical opcode.) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Idle/rest = the IDLE sentinel (200 → W01 clip 3, arms-down), NOT 0: motion 0 renders
     * the DEFAULT bank clip 0 = the per-room cinematic RBJ overlay (a gesture, not a rest).
     * Gameplay's player_tick re-drives the idle FSM next frame anyway; 200 is the correct
     * bank-independent rest for the 1-frame handoff. (Same fix family as the post-walk reset.) */
    re15_actor_set_motion(pl, 200);   /* = the +5..7 walk-scratch clear → idle/rest pose */
    pl->anim_frame    = 0;
    g_scd.player_mode = 1;          /* gameplay/PAD live next frame (zero delay) */
    t->pc += 1;
    return 1;
}

/* === Phase 4.5.13-C: Plc_flg (0x43) — set player animation flag ===========
 *
 * 4-byte encoding (per SCDScriptDisassembler.java case 0x43):
 *   [+0]  op (0x43)
 *   [+1]  entity (0 = player)
 *   [+2]  flag bit value (typ. 0x80 = "anim cycle complete")
 *   [+3]  value (0 or 1)
 *
 * In sub02/sub14 this fires as `Plc_flg(0, 128, 0)` between the start
 * and the hold of a cinematic anim, signaling the player-animation FSM
 * to clear its "playing" bit so the next Plc_motion/Sleep block can
 * advance.
 *
 * T-PLC-FLG (50-agent dive) discovered our no-op was causing the user's
 * "repeating waving animations": the renderer's `(anim_frame >> 1) %
 * clip->frame_count` wraps at clip end, restarting the loop. The script's
 * second Plc_motion(15) + Plc_flg(0,128,0) is meant to FREEZE the actor
 * on the last frame so it doesn't replay during the long Sleep.
 *
 * Fix: when flag bit 0x80 is set with value 0, set the actor's
 * `anim_freeze` flag — the renderer clamps anim_frame to the clip's
 * last frame instead of wrapping. */
static int op_plc_flg(scd_thread_t *t)
{
    /* T-round (2026-05-25): rewrite per PSX disasm 0x80041fb8.
     * Encoding: [op, subop, mask_lo, mask_hi] (4 bytes).
     *   subop 0 = OR   actor.anim_flags |= mask
     *   subop 1 = SET  actor.anim_flags  = mask
     *   subop 2 = XOR  actor.anim_flags ^= mask
     *   subop 3+= no-op
     * mask is 16-bit (LE/lhu). Byte-true playback-mode bits (RE'd from the
     * FUN_80050cb8 phase FSM — entity+0x1c4): 0x04 = LOOP (re-arm the clip on
     * end; else play-once-then-HOLD-LAST), 0x08 = double-cycle/ping-pong (a 2nd
     * advance per frame while running), 0x40 = SNAP (zero the inter-clip blend
     * countdown on (re)init), 0x80 = REVERSE direction (whole clip played
     * backward via the mirrored ((count-subframe)-1) lookup — NOT a
     * forward-then-settle; that is bit 0x08). ROOM1170 sub02's Plc_flg(0,128,0)
     * sets 0x80 (reverse); ROOM1150 sub02's Plc_flg(0,4,0) sets 0x04 (loop, so
     * Irons' lying breathe clip cycles).
     *
     * Also: per-thread work_slot routing (matches Plc_motion / Plc_dest).
     * Was hardcoded to entity-based actor_slot, missing the Work_set context. */
    uint8_t  subop    = t->pc[1];
    uint16_t mask     = (uint16_t)t->pc[2] | ((uint16_t)t->pc[3] << 8);
    int actor_slot;
    if (t->work_slot >= 0 && t->work_slot < RE15_ACTOR_MAX) {
        actor_slot = t->work_slot;
    } else {
        actor_slot = RE15_ACTOR_SLOT_PLAYER;
    }
    if (actor_slot >= 0 && actor_slot < RE15_ACTOR_MAX) {
        re15_actor_t *a = &g_actors[actor_slot];
        switch (subop) {
        case 0: a->anim_flags |= mask;       break;
        case 1: a->anim_flags  = (uint16_t)mask; break;
        case 2: a->anim_flags ^= mask;       break;
        default: break;
        }
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[scd F%u] Plc_flg(subop=%u, mask=0x%04X) → slot=%d anim_flags=0x%04X\n",
                (unsigned)g_engine.frame_count, subop, mask, actor_slot,
                (unsigned)a->anim_flags);
#endif
    }
    t->pc += 4;
    return 1;
}

/* === Phase 4.4.6: AOT triggers =============================================
 *
 * Aot_set (0x2C) — declare a rectangular trigger zone. 20-byte encoding
 * per Java SCDScriptDisassembler.java:0x2C (matches Door_aot_set rect):
 *
 *   [+0]  op (0x2C)
 *   [+1]  slot id   (0..RE15_AOT_MAX-1)
 *   [+2]  type      (script-defined; mirror to aot.type)
 *   [+3]  floor     (currently unused)
 *   [+4]  chain
 *   [+5]  switch_flag
 *   [+6..7]  rect x   (LE s16) — NW corner (corrected 2026-06-08; was "BE s16",
 *                               the disproven all-BE rule — handler reads LE)
 *   [+8..9]  rect z   (LE s16)
 *   [+10..11] rect w  (LE s16) — full width
 *   [+12..13] rect d  (LE s16)
 *   [+14]  act       (event_id / trigger action)
 *   [+15]  priority
 *   [+16..19] extras (ignored)
 *
 * Aot_reset (0x46) — 10-byte encoding (slot at +1, rest ignored). */
static int op_aot_set(scd_thread_t *t)
{
    int     slot     = (int)t->pc[1];
    uint8_t type     = t->pc[2];
    /* EXTENDED 4-POINT AOT (byte-true FUN_80042bac @0x80042dc4: `lbu 1(s0); andi 0x80; beq`, s0=pc+2
     * so the tested byte is pc[3]): when pc[3]&0x80 is SET the geometry is a 4-VERTEX POLYGON at
     * pc[6..21] (eight LE s16 @s0+4..s0+18 = v0(pc6,pc8) v1(pc10,pc12) v2(pc14,pc16) v3(pc18,pc20),
     * tested by point-in-polygon FUN_80014368) — NOT a rect — and EVERY trailing field shifts +8
     * (act pc[14]->22, eventId pc[17]->25). The port used to read the naive rect pc[6..13] + pc[17]
     * for every form, so a long-form AOT (e.g. ROOM1090 sub00 `2C .. B1 ..`) installed a garbage rect
     * from the first two polygon vertices and read a polygon byte (0xF8) as the event -> a dead,
     * wrong-shaped trigger. The SHORT form (pc[3]&0x80 clear) is byte-for-byte unchanged. */
    int     long_form = (t->pc[3] & 0x80) != 0;
    int16_t vx[4] = {0,0,0,0}, vz[4] = {0,0,0,0};
    int32_t cx, cz, hw, hh;
    if (long_form) {
        for (int i = 0; i < 4; i++) { vx[i] = scd_read_le_s16(&t->pc[6 + i*4]); vz[i] = scd_read_le_s16(&t->pc[8 + i*4]); }
        int32_t minx = vx[0], maxx = vx[0], minz = vz[0], maxz = vz[0];
        for (int i = 1; i < 4; i++) {
            if (vx[i] < minx) minx = vx[i]; if (vx[i] > maxx) maxx = vx[i];
            if (vz[i] < minz) minz = vz[i]; if (vz[i] > maxz) maxz = vz[i];
        }
        cx = (minx + maxx) / 2; cz = (minz + maxz) / 2;   /* AABB fallback centre for the AOT scan */
        hw = (maxx - minx) / 2; hh = (maxz - minz) / 2;
    } else {
        /* AOT rect is LITTLE-ENDIAN (2026-06-04 fix, same as Door_aot_set). NW corner + extent -> */
        int16_t rect_x = scd_read_le_s16(&t->pc[6]);
        int16_t rect_z = scd_read_le_s16(&t->pc[8]);
        int16_t rect_w = scd_read_le_s16(&t->pc[10]);
        int16_t rect_d = scd_read_le_s16(&t->pc[12]);
        cx = (int32_t)rect_x + (int32_t)rect_w / 2;       /* center + half-extents (matches Door/Item) */
        cz = (int32_t)rect_z + (int32_t)rect_d / 2;
        hw = (int32_t)(rect_w < 0 ? -rect_w : rect_w) / 2;
        hh = (int32_t)(rect_d < 0 ? -rect_d : rect_d) / 2;
    }
    uint8_t event_id = long_form ? t->pc[22] : t->pc[14];   /* act (+8 in the long form) */
    /* SCD Aot_set types 12/13 = the STAIR band-transition zones. Route them to the
     * stair setter so stair_common.c can action-trigger the descent/ascent — see
     * re15_aot.h. The runtime AOT record IS the raw SCD byte stream (installer stores
     * pc+2 @0x80040578-84), so the sce-12/13 handler fields map as:
     *   chain  = pc[4]      = rec+0x2  (band gate @0x80042cac-ccc; ROOM1170 {0,2,2,4})
     *   side   = pc[14..15] = rec+0xC  u16 LE (lhu @0x80043530/58 resp. @0x800435fc/24)
     *   count  = pc[16]     = rec+0xE  (lbu @0x800435b8 / @0x8004367c; bands & 7)
     *   axis   = the sce itself (12 = X, 13 = Z)
     *   corner/extent = the RAW rect fields along the axis (X: pc[6..7]/pc[10..11],
     *   Z: pc[8..9]/pc[12..13] — lh 0x4/0x6(a2) + lhu 0x8/0xa(a2) in the handlers).
     * NOT floor (pc[3]=0x31, a constant flag byte). All shipped stair records are
     * SHORT form; a hypothetical long form falls back to the AABB-derived values. */
    if (type == 12 || type == 13) {
        uint8_t  chain = t->pc[4];
        uint16_t side; uint8_t count; int32_t st_corner, st_extent;
        if (!long_form) {
            side      = (uint16_t)(t->pc[14] | ((uint16_t)t->pc[15] << 8));
            count     = t->pc[16];
            st_corner = (int32_t)scd_read_le_s16(&t->pc[type == 13 ? 8 : 6]);
            st_extent = (int32_t)scd_read_le_s16(&t->pc[type == 13 ? 12 : 10]);
        } else {
            side      = (uint16_t)(t->pc[22] | ((uint16_t)t->pc[23] << 8));
            count     = t->pc[24];
            st_corner = (type == 13) ? cz - hh : cx - hw;
            st_extent = (type == 13) ? hh * 2 : hw * 2;
        }
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[STAIRREG] slot=%d type=%d chain=%d side=%u count=%u corner=%d extent=%d centre=(%d,%d) half=(%d,%d)\n",
                slot, type, chain, side, count, st_corner, st_extent, cx, cz, hw, hh);
#endif
        re15_aot_set_stair(slot, cx, cz, hw, hh, chain, side, count,
                           (uint8_t)type, st_corner, st_extent);
    } else {
        /* BYTE-TRUE event AOT (globalization 2026-06-13): the REAL event/sub id is
         * pc[17] (Java disasm "eventId"), NOT pc[14] ("act"/SAT). An Aot_set with
         * eventId != 0xFF is an ACTION-triggered EVENT (e.g. the ROOM1130 roller-door
         * SWITCH: type=3, act=0xFF, eventId=2 → runs sub_scd[2] = Message_on + Set(3,107,1)
         * + Aot_reset). Install it as GENERIC (NOT the SCD `type` byte — type 3 collided
         * with our CAM_SWITCH enum, so the switch was wrongly treated as an RVD camera
         * zone + never fired). The aot scan fires GENERIC on action → scd_event_fire →
         * sub_scd[eventId]. AOTs with eventId==0xFF (no event, e.g. room1170 slot-5) keep
         * the prior type/act behaviour untouched. */
        uint8_t ev = long_form ? t->pc[25] : t->pc[17];   /* eventId (+8 in the long form) */
        if (type == 0) {
            /* sce=0 INERT install (aot_sce_census fix 1): the record is registered
             * (installer stores pc+2 unconditionally @0x80040580) but handler[0]
             * @0x8004305C is a latch-restore no-op and the ACTION scan skips sce-0
             * records (@0x80042f48-50). 35 shipped 0x2C installs — most carry a
             * PRE-LOADED event payload {0xFF,0x18,sub} (e.g. ROOM1150 slot 1 sub04,
             * ROOM2030 slots 4-7) waiting for an Aot_reset retype to arm them; the old
             * ev=pc[17] heuristic ARMED them at install (they fired their subs live).
             * Register with geometry + band so a later re15_aot_retype can arm. */
            re15_aot_set(slot, RE15_AOT_TYPE_NONE, 0, cx, cz, hw, hh);
        } else if (type == 5) {
            /* sce=5 marker/examine (aot_sce_census fix 4): handler[5] @0x8004318C is a
             * pure NOP — the effect is the SCAN's work-var latch (centre -> work_vars[1]
             * @0x80042ee4, forward -> work_vars[0] @0x80042f3c; value = slot). Route by
             * sce, NOT by the ev byte: the payload is unused by the handler, and the 8
             * shipped ROOM1190/1191 installs carry payload 0xFF80 @+2 (pc[17]=0xFF) which
             * the old ev-heuristic mis-installed as port-enum 5 = MESSAGE (garbage msg 0);
             * the 142 auto 0x44 installs were mis-typed as player-action examines. The
             * scan branches action (flags bit 0x10) vs per-frame AUTO pool latch. */
            re15_aot_set(slot, RE15_AOT_TYPE_EXAMINE_WORKVAR, (uint8_t)slot, cx, cz, hw, hh);
        } else if (type == 1) {
            /* sce=1 MESSAGE (byte-true LAB_80043084: FUN_80027e68(0,0x300, u16@0 = room-msg
             * index, u16@2<<16 = pause bits); 427 shipped zones, ALL flags 0x31 action+forward).
             * These carry pause 0xFFFF at pc[16..17], so the old ev-heuristic (ev==0xFF) fell
             * through to `type` = port enum 1 = DOOR with zero door_params = INERT: no examine
             * text fired anywhere from a direct Aot_set. [wf_f536e1ee divergence #3] */
            uint8_t msg = long_form ? t->pc[22] : t->pc[14];   /* payload u16@0 low byte */
            re15_aot_set(slot, RE15_AOT_TYPE_MESSAGE, msg, cx, cz, hw, hh);
        } else if (type == 4) {
            /* sce=4 FLAG_CHG (byte-true LAB_80043120: table 0x80074664[u16@0=group], bit u16@2
             * MSB-first, on/off u16@4; idempotent per-frame for the AUTO variants). 30 shipped
             * zones (2x action, 4x auto-player, 10x auto-ENEMY, 8x both pools) — the old
             * heuristic installed them as EXAMINE_WORKVAR (flag never set + work_vars[0]
             * corruption on action). [wf_f536e1ee divergence #4] */
            re15_aot_set(slot, RE15_AOT_TYPE_FLAG_CHG, 0, cx, cz, hw, hh);
            if (slot >= 0 && slot < RE15_AOT_MAX) {
                g_aot.flag_params[slot].group = long_form ? t->pc[22] : t->pc[14];
                g_aot.flag_params[slot].bit   = long_form ? t->pc[24] : t->pc[16];
                g_aot.flag_params[slot].on    = (long_form ? t->pc[26] : t->pc[18]) ? 1 : 0;
            }
        } else if (type == 8) {
            /* sce=8 WATER (LAB_8004330c): entity+0x88 = u16@0 per frame, all 3 pools —
             * 32 zones (STAGE1 sewers 11A0/11A1/1260/1261 + STAGE2). [wf_f536e1ee step 4] */
            re15_aot_set(slot, RE15_AOT_TYPE_WATER, 0, cx, cz, hw, hh);
            if (slot < RE15_AOT_MAX)
                g_aot.env_params[slot].p0 = long_form ? scd_read_le_s16(&t->pc[22])
                                                      : scd_read_le_s16(&t->pc[14]);
        } else if (type == 7) {
            /* sce=7 STAIR-RAMP Y (LAB_800431cc): mode u16@0, step u16@2, height u16@4 —
             * 4 zones (ROOM5060/5061). */
            re15_aot_set(slot, RE15_AOT_TYPE_RAMP, 0, cx, cz, hw, hh);
            if (slot < RE15_AOT_MAX) {
                int b = long_form ? 22 : 14;
                g_aot.env_params[slot].p0 = scd_read_le_s16(&t->pc[b]);
                g_aot.env_params[slot].p1 = scd_read_le_s16(&t->pc[b + 2]);
                g_aot.env_params[slot].p2 = scd_read_le_s16(&t->pc[b + 4]);
            }
        } else if (ev == 0) {
            /* eventId==0 is NOT an event sub (sub_scd[0]=init is never an AOT target). It's an
             * EXAMINE→work-var AOT (ROOM1150 slot-2 type-5 over Irons): on the action examine
             * the scan writes work_vars[0]=its index (=slot) + flags a one-shot sub01 re-poll,
             * which fires sub03 (`Cmp(work_vars[0]==2)`). Was wrongly a GENERIC firing sub00. */
            re15_aot_set(slot, RE15_AOT_TYPE_EXAMINE_WORKVAR, (uint8_t)slot, cx, cz, hw, hh);
        } else if (ev != 0xFF) {
            /* BYTE-TRUE auto-vs-action (FUN_80042bac, 2026-06-15): the AOT entry the
             * scan reads = &pc[2], so the trigger-FLAGS byte tested for the action gate
             * is pc[3] (the SCD "floor" operand). The scan does `(flags & 0x10) == param_3`:
             *   - the per-frame AUTO scan FUN_800436a8 passes param_3=0  → matches flags bit
             *     0x10 CLEAR = fire on WALK-IN (no button).
             *   - the player-state ACTION scan passes param_3=0x10 → matches bit 0x10 SET =
             *     fire on action press.
             * ROOM1130 switch / ROOM1140 painting use 0x31 (bit set → GENERIC action). ROOM1150
             * AOT6 (the Irons cutscene, sub08) uses 0x41 (bit CLEAR → AUTO on walk-in) — our old
             * code forced every event AOT to GENERIC, so the auto cutscene never fired. */
            uint8_t flags = t->pc[3];
            uint8_t atype = (flags & 0x10) ? RE15_AOT_TYPE_GENERIC
                                           : RE15_AOT_TYPE_AUTO_EVENT;
            re15_aot_set(slot, atype, ev, cx, cz, hw, hh);
        } else {
            re15_aot_set(slot, type, event_id, cx, cz, hw, hh);
        }
        /* #14(c): pc[4] is the runtime AOT band for EVERY Aot_set type (same field the stair reads as
         * `chain` and the door as its band). Store it so the scan's generic band-gate (byte-true
         * FUN_80042cac: (band&0x80) || player_band==band) can keep a wrong-floor event AOT from firing. */
        if (slot >= 0 && slot < RE15_AOT_MAX) {
            g_aot.slots[slot].band      = t->pc[4];
            g_aot.slots[slot].sce_flags = t->pc[3];   /* record[1] test bits (wf_f536e1ee step 2) */
        }
        /* LONG form: install the 4-vertex polygon so the AOT scan uses point-in-quad (FUN_80014368)
         * instead of the AABB fallback (mirrors the RVD quad install in rdt_common.c). */
        if (long_form && slot >= 0 && slot < RE15_AOT_MAX) {
            for (int k = 0; k < 4; k++) { g_aot.slots[slot].xs[k] = vx[k]; g_aot.slots[slot].zs[k] = vz[k]; }
            g_aot.slots[slot].has_quad = 1;
        }
#ifdef RE15_AOTBAND_DIAG
        fprintf(stderr, "[AOTBAND] room set slot=%d type=%d flags=0x%02x band=pc4=0x%02x ev=0x%02x\n",
                slot, (int)t->pc[2], (int)t->pc[3], (int)t->pc[4], (int)t->pc[17]);
#endif
    }
    /* PC-Vorschub konditional pc[3]&0x80: 28 vs 20. Byte-true: LAB_80040534 @0x80040590
     * `lbu v0,0x3(v1)`; @0x80040598 `andi 0x80`; @0x8004059c `beq` -> +0x1c(28)/+0x14(20)
     * (ghidra1_V2.txt). [BYTE_TRUE_AUDIT #5] */
    t->pc += (t->pc[3] & 0x80) ? 28 : 20;
    return 1;
}

static int op_aot_reset(scd_thread_t *t)
{
    /* Aot_reset(slot, sce, flags, p0_lo, p0_hi, p1_lo, p1_hi, p2_lo, p2_hi, _) — 10 bytes
     * (@0x800407ac `addiu v0,a2,10`). FULL RETYPE, byte-true LAB_80040738 (aot_sce_census
     * fix 2): rec[0]=pc[2] (@0x80040764), rec[1]=(rec[1]&0x80)+pc[3] (@0x8004076c-78),
     * payload halfwords +0/+2/+4 = pc[4..9] (@0x80040788-a8). It NEVER clears the record —
     * the new sce selects the handler AND the 3 payload halfwords replace the old params:
     *   sce 0 → inert (ROOM1130 sub02 Aot_reset(4,0,…) kills the roller switch; 188 uses)
     *   sce 1 → MESSAGE msg=p0 (ROOM1130 sub01 Aot_reset(3,1,0x31,{1,0xFFFF,0}); 82 uses)
     *   sce 2 → DOOR re-arm + next_pos_x/y/z = p0/p1/p2 (32 uses, e.g. ROOM1140 sub02
     *           slot 1 arms the sce-0 door with spawn (-21937,0,-25713))
     *   sce 3 → EVENT sub = p1>>8 (payload byte +3; 67 uses, e.g. ROOM1140 sub01
     *           Aot_reset(5,3,0x31,{0xFF,0x1805,0}) re-points the painting to sub05)
     *   sce 9 → ITEM re-arm {type=p0, amount=p1, tk_bit=p2} (8 uses, rooms 1190/40A0)
     *   sce 4/5/7/8 → flag / marker / ramp / water params.
     * The old port code kept the OLD params for every sce>=2 (re15_aot_reassert_event) —
     * the census divergence this replaces. */
    re15_aot_retype((int)t->pc[1], t->pc[2], t->pc[3],
                    (uint16_t)(t->pc[4] | (t->pc[5] << 8)),
                    (uint16_t)(t->pc[6] | (t->pc[7] << 8)),
                    (uint16_t)(t->pc[8] | (t->pc[9] << 8)));
    t->pc += 10;
    return 1;
}

/* === Phase 4.4.8: Actor / NPC opcodes =====================================
 *
 * Member_set (0x34) — 6 B: [op, slot, member_id, value_lo, value_hi, _]
 * Member_cmp (0x3E) — 6 B: [op, slot, member_id, cmp_op, value_lo, value_hi]
 *   cmp_op: 0 = ==, 1 = !=, 2 = <, 3 = >. Writes boolean to locals[0]
 *   so a follow-up If can branch on it (same convention as Ck).
 * Sce_em_set (0x44) — 22 B: [op, slot, type, behavior, hp_lo, hp_hi,
 *   pos_x×4, pos_y×4, pos_z×4, rot_y_lo, rot_y_hi, flags_lo, flags_hi]. */
/* Phase 4.5.9-E: SCD script slots reference the ENEMY pool. Slot 0 in
 * a script = first enemy = actor pool slot 1. The player (actor 0) is
 * addressed via Plc_* opcodes, never via Member_set / Sce_em_set. */
#define SCRIPT_SLOT_TO_ACTOR(s)  ((int)(s) + 1)

/* Work_set (0x2E) — 3 bytes. RE2 BioClone docs (info293.txt:588-592):
 *   pc[1]=kind, pc[2]=index
 *     kind 1: player           → actor slot 0
 *     kind 3: enemy NN         → actor slot NN+1 (our +1 offset)
 *     kind 2: room object NN   → not in actor pool → work_slot = -1
 *     kind 4: room entity NN   → not in actor pool → work_slot = -1
 *
 * Member_set / Member_cmp use g_scd.work_slot as their implicit target.
 * When kind=2/4 → work_slot=-1 → Member_set is a no-op (RE2-correct: those
 * entities live in a separate room table we haven't ported yet). This
 * stops sub02's `Work_set(2,..); Member_set(12,-30208)` from sinking the
 * player below the floor. */
/* Zero the 6 per-thread velocity/accel slots (thread+0x158..+0x163) — the entry clear shared by
 * Work_set (0x2E, LAB_80040d2c @0x80040d2c-40) and the indirect 0x53 (LAB_80040e18 @0x80040e18-30).
 * Verified 2026-06-09: byte-correct for a Work_set→Add_speed without an intervening Speed_set. */
static void scd_work_clear(scd_thread_t *t)
{
    for (int _v = 0; _v < 6; _v++) { t->vel[_v] = 0; t->accel[_v] = 0; }
}

/* Bind the PER-THREAD work entity (thread+0x154, per F11 canon) by selector `kind`, index `idx` —
 * the shared body of Work_set (0x2E) and the indirect 0x53. Also runs scd_work_clear.
 *   kind 1 → player;  kind 2 → NPC actor (idx+1, e.g. Work_set(2,0)=Elliot before Plc_motion);
 *   kind 3 → prop by OBJ_ID (strictly the prop pool — Work_set(3,0) must NOT capture actor idx+1,
 *            which floated the helipad fixture as a spurious box);  else → unbound (-1).
 * g_scd.work_slot mirrors for backward-compat readers. */
static void scd_work_bind(scd_thread_t *t, uint8_t kind, uint8_t idx)
{
    scd_work_clear(t);
    t->work_prop_idx = -1;
    g_scd.work_prop_idx = -1;
    if (kind == 1) {
        t->work_slot = RE15_ACTOR_SLOT_PLAYER;
        g_scd.work_slot = RE15_ACTOR_SLOT_PLAYER;
    } else if (kind == 2) {
        int slot = (int)idx + 1;
        if (slot < RE15_ACTOR_MAX && g_actors[slot].active) {
            t->work_slot = (int8_t)slot;
            g_scd.work_slot = (int8_t)slot;
        } else {
            t->work_slot = -1;
            g_scd.work_slot = -1;
        }
    } else if (kind == 3) {
        t->work_slot = -1;
        g_scd.work_slot = -1;
        for (int i = 0; i < g_scd.prop_count; i++) {
            if (g_scd.props[i].active && g_scd.props[i].obj_id == idx) {
                t->work_prop_idx = (int8_t)i;
                g_scd.work_prop_idx = (int8_t)i;
                break;
            }
        }
    } else {
        t->work_slot = -1;
        g_scd.work_slot = -1;     /* room / object entities → no actor route */
    }
}

static int op_work_set(scd_thread_t *t)
{
    /* Byte-true LAB_80040d2c — Work_set binds by the DIRECT index pc[2] (cf. 0x53 = indirect). */
    scd_work_bind(t, t->pc[1], t->pc[2]);
    t->pc += 3;
    return 1;
}

/* Speed_set (0x2F) — 4 bytes [op][axis][value LE s16].
 *
 * AO2-round 2026-05-26: VALUE IS LITTLE-ENDIAN per PSX disasm proof.
 * `LAB_80040f14` at ghidra1_V2.txt:151147 uses MIPS `lhu a1, 0x2(v0)` —
 * little-endian halfword load. The Java disassembler reads BE for
 * display purposes (e.g. sub04 `2F 04 00 02` shows as "Speed_set(4,2)")
 * but PSX engine reads LE → value = 0x0200 = **512**, not 2. That's
 * 256× faster than what we had, and matches user's "too slow rotor"
 * symptom. Sub06's `2F 01 CE FF` is LE -50 (not BE -12545) for vel_y.
 *
 * Old (broken) comment claimed BE: "`2F 01 CE FF` = Speed_set(1, -12545)
 *   ← CE FF as BE s16 = -12545 ✓" — wrong; PSX reads LE = -50. The
 *   heli rises ~1500 units/sec at 30Hz × -50/tick = consistent with
 *   ablauf timing.
 *
 * Axis byte maps to actor velocity components per RE2 canonical layout
 * (vel vector at +0x1D4 is X/Y/Z then rot-speed X/Y/Z):
 *   0 → vel_x  (linear, integrated by Add_speed)
 *   1 → vel_y  (linear, integrated by Add_speed)
 *   2 → vel_z  (linear, integrated by Add_speed)
 *   3 → rot_x speed   (typically integrated by Add_aspeed)
 *   4 → rot_y speed   (vel_w / angular yaw — also integrated by Add_speed
 *                      in RE1.5 cinematic helicopter rotors: sub04/sub05
 *                      ROOM1170 fire `Speed_set(4, 2); Add_speed;` in a
 *                      goto-loop to spin rotors at +2/4096 per tick)
 *   5 → rot_z speed
 *   6 → vel_w alias (kept for legacy callers; same as axis 4)
 *
 * Writes to whichever actor `g_scd.work_slot` currently points at (set
 * by prior Work_set). For props (work_prop_idx valid, no actor) axis 0/1/2
 * write linear velocity and axis 4 writes vel_ry (angular Y) — used by
 * helicopter rotors which exist only as Obj_model_set props, not actors. */
static int op_speed_set(scd_thread_t *t)
{
    uint8_t axis  = t->pc[1];
    int16_t value = scd_read_le_s16(&t->pc[2]);
    /* AO5-round 2026-05-26: velocity is per-THREAD, not per-entity. PSX
     * LAB_80040f14 stores at script+0x158+axis*2 (the calling thread's
     * own vel slot). The "work entity" pointed-to by Work_set is only
     * consumed at Add_speed time. Without per-thread vel, sub04 (rotor
     * spin, work=main_rotor, vel_ry=512) and sub07 (rotor takeoff,
     * work=main_rotor, vel_y=-50) both wrote to the SAME prop.vel_*,
     * so sub04's infinite Add_speed loop double-applied sub07's vel_y
     * → main rotor lifted off 2× faster than body. */
    /* Byte-true (LAB_80040f14: sh at thread+0x158+axis*2): the slot array is 12 s16 —
     * 0..5 = velocity, 6..11 = acceleration. Axis 6..11 (e.g. the ROOM1130 door's
     * Speed_set(7,…) = ay) land in accel[axis-6]; Add_aspeed then folds accel into vel. */
    if (axis < 6)        t->vel[axis]       = value;
    else if (axis < 12)  t->accel[axis - 6] = value;
    t->pc += 4;
    return 1;
}

/* Add_speed (0x30) — 1 byte. Integrates velocity into work entity's pos+rot
 * once. AO5-round 2026-05-26: velocity is read from THIS thread's t->vel[]
 * (PSX LAB_80040f40 reads from script+0x158, not from the entity), and
 * applied to the work entity pointed to by Work_set (actor or prop).
 *
 * AO7-round 2026-05-26: PER-THREAD ONLY. The previous `g_scd.work_slot`
 * fallback leaked cross-thread state — when sub02 ran `Work_set(2,0)` (set
 * global work_slot=1=Elliot) or `Work_set(1,0)` (set global to player),
 * concurrent sub06/07/08 takeoff threads (with t->work_slot=-1, intended
 * for props) saw the global as a "current actor target" and dumped their
 * vel_y=-50 onto Elliot/player every tick → Elliot drifted off-screen
 * (-7200 + 156*-50 = -15000), Leon corrupted. Per-thread state is the
 * RE2 canonical model (script+0x154 stores per-instance, NOT shared). */
static int op_add_speed(scd_thread_t *t)
{
    /* Byte-true (LAB_80040f40): integrate ALL 6 velocity slots — X/Y/Z (32-bit add)
     * AND rot_x/rot_y/rot_z (16-bit `sh`, NATURAL 16-bit wrap, no &0x0FFF mask). The
     * old code only did vel[4]→rot_y masked to 12 bits. (Verified 2026-06-09; the angle
     * consumers re15_sin/cos_q12 are periodic on both ports — PSX isin masks, PC sinf is
     * periodic — so the 16-bit-wrapped rotation reads identically. ROOM1170 rotors set
     * only vel[4] (Speed_set axis 4 → rot_y), so vel[3]/vel[5] are 0 here = no-op.) */
    int8_t ws = t->work_slot;
    int8_t pi = t->work_prop_idx;
    if (ws >= 0 && ws < RE15_ACTOR_MAX) {
        re15_actor_t *a = &g_actors[ws];
        a->x += (int32_t)t->vel[0];
        a->y += (int32_t)t->vel[1];
        a->z += (int32_t)t->vel[2];
        a->rot_x = (int16_t)(a->rot_x + t->vel[3]);
        a->rot_y = (int16_t)(a->rot_y + t->vel[4]);
        a->rot_z = (int16_t)(a->rot_z + t->vel[5]);
    } else if (pi >= 0 && pi < 16 && g_scd.props[pi].active) {
        g_scd.props[pi].x += (int32_t)t->vel[0];
        g_scd.props[pi].y += (int32_t)t->vel[1];
        g_scd.props[pi].z += (int32_t)t->vel[2];
        g_scd.props[pi].rot_x = (int16_t)(g_scd.props[pi].rot_x + t->vel[3]);
        g_scd.props[pi].rot_y = (int16_t)(g_scd.props[pi].rot_y + t->vel[4]);
        g_scd.props[pi].rot_z = (int16_t)(g_scd.props[pi].rot_z + t->vel[5]);
    }
    t->pc += 1;
    return 1;
}

/* Member_set (0x34) — 4 bytes per Java disassembler.
 *   [op, member_id, value_be(2)]
 *
 * RE2's Member_set has NO slot field — target is the "work entity" set by
 * the most recent Work_set (0x2E). If no work entity is selected, the op
 * is a no-op. Critically the SIZE must be 4 (was 6 before, which advanced
 * PC by 2 extra bytes per call → all subsequent opcodes ran on misaligned
 * data). Was the first half of the player-invisible blocker (#1 / Phase
 * 4.5.9-F); the second half (Member_set was clobbering player.y) is fixed
 * here in Phase 4.5.9-G by honoring work_slot instead of always writing
 * to slot 0. */
static int op_member_set(scd_thread_t *t)
{
    uint8_t member_id = t->pc[1];
    /* I4-round (2026-05-25): position fields LE (matches Obj_model_set LE
     * and Sce_em_set LE for cinematic positions). Sub02 line 107
     * Member_set(0, 26634) → LE = 2664 puts Leon at cut 4 target.
     * For props the LE values for Work_set+Member_set sequences may not
     * all match — sub02's intent for those is unclear without runtime
     * test. Going with consistency: all positions LE. */
    /* AG-round (2026-05-26): extend LE decode to IDs 3/4/5 (rot_x/rot_y/rot_z).
     * Per agent E2 PSX disasm + RE_15_modified_V2/entity_set_field.c, IDs
     * 3/4/5 write to actor+0x68/+0x6A/+0x6C (rotations). PSX uses `lh`/`lhu`
     * (LE) for these script args. Sub02 line 12 `Member_set(4, 4)` decoded BE
     * = 4 (0.35° facing +X) made Leon spawn facing wrong direction; LE = 1024
     * (90° = clean cardinal direction). Same fix applies to rot_x/rot_z. */
    /* Byte-true (LAB_800410b8): the handler reads the value with `lh` = LITTLE-ENDIAN
     * for EVERY member id (verified 2026-06-09 vs ghidra1_V2.txt). The old conditional
     * BE path for ids outside {0-5,0x0B-0x10} was a non-byte-true deviation — ROOM1170
     * only uses ids 0/1/2 (already LE), so this changes nothing here but is now correct
     * for any id. (NOTE: this is Member_set 0x34; Member_cmp 0x3E is a SEPARATE opcode —
     * do not assume the same endianness without its own verification.) */
    int16_t value = scd_read_le_s16(&t->pc[2]);
    /* I2-round (2026-05-24): per-thread work_slot. */
    int8_t ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    if (ws >= 0) {
        /* [#11] byte-true: the RE1.5 Member id goes STRAIGHT to the actor table
         * (FUN_8004116c). The old RE1.5→RE2 translation wrote id12 → Leon.y
         * ("player under the floor") and mis-mapped/dropped ids 6-19. */
        re15_actor_set_member((int)ws, member_id, (int32_t)value);
    } else {
        /* I4-round (2026-05-25, post R2 finding): when work_slot is invalid
         * but work_prop_idx is set, write to the PROP's position. Sub02
         * uses Work_set(3,N) where N targets a prop (kind=3 falls through
         * to prop pool when no matching NPC actor). The pilot's position
         * during the dialog cut is set this way per R2's RE.
         *
         * Per R2: Work_set(3, idx) targets prop by IDX (not obj_id),
         * since our prop pool is indexed in spawn order = obj_id order
         * for ROOM1170 (obj 0..5 → props[0..5]).             */
        int8_t pi = (t->work_prop_idx >= 0) ? t->work_prop_idx : g_scd.work_prop_idx;
        if (pi >= 0 && pi < (int)g_scd.prop_count && g_scd.props[pi].active) {
            switch (member_id) {
            case 0x00: case 0x0B: g_scd.props[pi].x = (int32_t)value; break;
            case 0x01: case 0x0C: g_scd.props[pi].y = (int32_t)value; break;
            case 0x02: case 0x0D: g_scd.props[pi].z = (int32_t)value; break;
            /* AS-round 2026-05-26: case 0x03/0x04/0x05 were collapsed to
             * rot_y. Per re15_to_re2_member_id() + RE_15_modified_V2
             * entity_set_field.c, RE1.5 IDs 3/4/5 = rot_x/rot_y/rot_z
             * (NOT all yaw). Sub12 `Member_set(4, 3584)` sets the heli
             * BODY's rot_y to Q12 315° (= -45°) so its nose aligns with
             * the (+X,+Z) flight vector during cut 6 takeoff — without
             * this case the body stayed at its sub15 spawn rot_y=2048
             * (180°), making it render as a SIDE view instead of the
             * canonical head-on silhouette seen in ablauf 219075+. */
            case 0x03: case 0x0E: g_scd.props[pi].rot_x = (int16_t)value; break;
            case 0x04: case 0x0F: g_scd.props[pi].rot_y = (int16_t)value; break;
            case 0x05: case 0x10: g_scd.props[pi].rot_z = (int16_t)value; break;
            default: break;
            }
#ifdef RE15_PLATFORM_PC
            if (member_id <= 5) {
                fprintf(stderr, "[prop-member F%u] prop[%d] obj=0x%02X m=%d → val=%d\n",
                        (unsigned)g_engine.frame_count, pi,
                        (unsigned)g_scd.props[pi].obj_id, member_id, value);
            }
#endif
        }
    }
    t->pc += 4;
    return 1;
}

/* 0x23 — Cmp: byte-true predicate (LAB_8003ff68 @0x8003ff68). Compare the global
 * work-var DAT_800b0fd0[pc[1]] (g_scd.work_vars[var], s16) against the LE s16
 * immediate at pc[4..5]; operator = high byte of the u16 @pc[2] (= pc[3]); 7 ops
 * (==,>,<,<=,>=,!=,bittest), op>=7 → false; PC += 6. Returns its boolean as the
 * dispatch code (FALSE pops the If block-stack). ROOM1170 uses no Cmp. */
static int op_cmp(scd_thread_t *t)
{
    /* [#6] LAB_8003ff68 (Cmp 0x23): var index = pc[2] — `andi 0xff` @0x8003ff80 takes the
     * LOW byte of the u16 LE @pc[2]. pc[1] is NOT read by this handler (the array/targetType
     * selector belongs to the Ck family 0x1F/0x20/0x21, not to Cmp). The prior PSX-boot crash
     * came from a HALF-fix (var moved to pc[2] but the operator table left wrong); var +
     * operators are byte-true here TOGETHER. ROOM1170 (boot) has no Cmp → boot path unaffected;
     * ROOM1150's gate has pc[1]==pc[2]==0. PSX-boot still worth a runtime re-check. */
    uint8_t var_idx = t->pc[2];
    uint8_t op      = t->pc[3];
    int16_t imm     = scd_read_le_s16(&t->pc[4]);
    int16_t v       = g_scd.work_vars[var_idx];
    int cond;
    switch (op) {                                  /* switchD_8003ffc0 {0:==,1:>,2:>=,3:<,4:<=,5:!=,6:&} */
    case 0: cond = (v == imm); break;
    case 1: cond = (v >  imm); break;
    case 2: cond = (v >= imm); break;              /* slt v0,a0,a1;xori @0x8003ffdc */
    case 3: cond = (v <  imm); break;              /* slt v0,a0,a1      @0x8003ffe8 */
    case 4: cond = (v <= imm); break;              /* slt v0,a1,a0;xori @0x8003fff0 */
    case 5: cond = (v != imm); break;
    case 6: cond = ((v & imm) != 0); break;
    default: cond = 0; break;
    }
    t->pc += 6;
    return cond ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}

/* ===== [#7] SCD work-variable ops (DAT_800b0fd0 = g_scd.work_vars, s16[256]) ===== */

/* ALU helper — byte-true to FUN_80040140 @0x80040140 (12-entry jump table @0x80010c5c,
 * sltiu op,0xc → op>=12 = no-op). Operand load width is load-bearing: ops 0,1,5,6,7,8,9,0xA
 * load *dst via `lhu` (unsigned), ops 2,3,4,0xB via `lh` (signed). Result stored back via
 * `sh` (low 16). DIV/MOD by 0 raise break 0x1c00 on PSX → mirror as "no write". */
static void scd_work_alu(uint8_t op, int16_t *dst, int16_t imm)
{
    uint16_t u = (uint16_t)*dst;                          /* lhu path */
    int16_t  s = *dst;                                    /* lh  path */
    unsigned shift = (unsigned)((uint16_t)imm & 0x1f);    /* sllv/srav use low 5 bits */
    switch (op) {
    case 0:  *dst = (int16_t)(u + (uint16_t)imm);                       break; /* ADD  addu  */
    case 1:  *dst = (int16_t)(u - (uint16_t)imm);                       break; /* SUB  subu  */
    case 2:  *dst = (int16_t)((int32_t)s * (int32_t)imm);               break; /* MUL  mult  */
    case 3:  if (imm != 0) *dst = (int16_t)((int32_t)s / (int32_t)imm); break; /* DIV  (÷0→break) */
    case 4:  if (imm != 0) *dst = (int16_t)((int32_t)s % (int32_t)imm); break; /* MOD  (÷0→break) */
    case 5:  *dst = (int16_t)(u | (uint16_t)imm);                       break; /* OR   or    */
    case 6:  *dst = (int16_t)(u & (uint16_t)imm);                       break; /* AND  and   */
    case 7:  *dst = (int16_t)(u ^ (uint16_t)imm);                       break; /* XOR  xor   */
    case 8:  *dst = (int16_t)(~u);                                      break; /* NOT  nor zero (imm ignored) */
    case 9:  *dst = (int16_t)(u << shift);                              break; /* SHL  sllv  */
    case 10: *dst = (int16_t)((uint16_t)((uint32_t)u >> shift));        break; /* SHR  lhu+srav (logical) */
    case 11: *dst = (int16_t)((int32_t)s >> shift);                     break; /* SHR  lh +srav (arithmetic) */
    default:                                                            break; /* op>=12 → no write */
    }
}

/* 0x24 — Save: work_vars[pc[1]] = LE_s16(pc[2..3]); PC += 4. (LAB_80040018) */
static int op_save(scd_thread_t *t)
{
    g_scd.work_vars[t->pc[1]] = scd_read_le_s16(&t->pc[2]);
    t->pc += 4;
    return SCD_R_CONTINUE;
}

/* 0x25 — Copy: work_vars[pc[1]] = work_vars[pc[2]]; PC += 3. (LAB_8004004c) */
static int op_copy(scd_thread_t *t)
{
    g_scd.work_vars[t->pc[1]] = g_scd.work_vars[t->pc[2]];
    t->pc += 3;
    return SCD_R_CONTINUE;
}

/* 0x26 — Calc: op=pc[2], dst=pc[3], imm=LE_s16(pc[4..5]);
 *   work_vars[dst] = ALU(op, work_vars[dst], imm); PC += 6. (LAB_8004008c) */
static int op_calc(scd_thread_t *t)
{
    scd_work_alu(t->pc[2], &g_scd.work_vars[t->pc[3]], scd_read_le_s16(&t->pc[4]));
    t->pc += 6;
    return SCD_R_CONTINUE;
}

/* 0x27 — Calc2: op=pc[1], dst=pc[2], src=pc[3];
 *   work_vars[dst] = ALU(op, work_vars[dst], work_vars[src]); PC += 4. (LAB_800400dc) */
static int op_calc2(scd_thread_t *t)
{
    scd_work_alu(t->pc[1], &g_scd.work_vars[t->pc[2]], g_scd.work_vars[t->pc[3]]);
    t->pc += 4;
    return SCD_R_CONTINUE;
}

/* Member get/set for an OBJECT work entity (Work_set kind 3 → prop pool). Byte-true to
 * FUN_80041358/FUN_800410b8 reading `work_entity + <offset>`: the combination-lock dial uses
 * member 15 = object+0xb = the NOTCH (which grid cell the cursor is over); position/rotation
 * ids map to the prop's world fields so Add_speed-driven props read consistently. */
int32_t re15_prop_get_member(int prop_idx, uint8_t member_id)
{
    if (prop_idx < 0 || prop_idx >= (int)(sizeof g_scd.props / sizeof g_scd.props[0])) return 0;
    switch (member_id) {
    case 0:  return g_scd.props[prop_idx].x;              /* +0x34 */
    case 1:  return g_scd.props[prop_idx].y;              /* +0x38 */
    case 2:  return g_scd.props[prop_idx].z;              /* +0x3c */
    case 3:  return (int32_t)g_scd.props[prop_idx].rot_x; /* +0x68 */
    case 4:  return (int32_t)g_scd.props[prop_idx].rot_y; /* +0x6a */
    case 5:  return (int32_t)g_scd.props[prop_idx].rot_z; /* +0x6c */
    case 15: return (int32_t)g_scd.props[prop_idx].member_0b;  /* +0x0b NOTCH */
    default: return 0;
    }
}
void re15_prop_set_member(int prop_idx, uint8_t member_id, int32_t value)
{
    if (prop_idx < 0 || prop_idx >= (int)(sizeof g_scd.props / sizeof g_scd.props[0])) return;
    switch (member_id) {
    case 0:  g_scd.props[prop_idx].x = value; break;
    case 1:  g_scd.props[prop_idx].y = value; break;
    case 2:  g_scd.props[prop_idx].z = value; break;
    case 3:  g_scd.props[prop_idx].rot_x = (int16_t)value; break;
    case 4:  g_scd.props[prop_idx].rot_y = (int16_t)value; break;
    case 5:  g_scd.props[prop_idx].rot_z = (int16_t)value; break;
    case 15: g_scd.props[prop_idx].member_0b = (uint8_t)value; break;  /* +0x0b NOTCH */
    default: break;
    }
}

/* Member_cmp (0x3E) — 6 bytes.
 *   [op, _reserved, member_id, cmp_op, value_be(2)]
 *
 * Java reads value as BE s16 at +4..+5. Also targets work entity. */
static int op_member_cmp(scd_thread_t *t)
{
    uint8_t  member_id = t->pc[2];
    uint8_t  cmp_op    = t->pc[3];
    /* byte-true: LAB_80041290 @0x800412ac `lh s1,0x4(v0)` = signed LITTLE-endian
     * (PSX lh), identisch zu Member_set @0x800410d4. War faelschlich BE. [#12] */
    int16_t  arg       = scd_read_le_s16(&t->pc[4]);
    int8_t   ws        = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    int8_t   wp        = (t->work_prop_idx >= 0) ? t->work_prop_idx : g_scd.work_prop_idx;
    /* [#11] byte-true: RE1.5 Member id straight to the entity GET table (FUN_80041358) — the
     * WORK ENTITY, which may be an actor (Work_set kind 1/2) OR an OBJECT (kind 3, e.g. the
     * combination-lock dial). The original reads `*(byte*)(work_entity + 0xb)` for member 15
     * regardless of entity kind; the port routes the object case to the prop pool so the keypad
     * confirm `Member_cmp(15==notch)` reads the dial NOTCH (member_0b). [S1-4 PROG-3 keypad] */
    int32_t  cur       = (ws >= 0)
                       ? re15_actor_get_member((int)ws, member_id)
                       : (wp >= 0 && wp < (int)g_scd.prop_count)
                       ? re15_prop_get_member(wp, member_id)
                       : 0;
    int      result    = 0;
    switch (cmp_op) {                                    /* Operator-Tabelle LAB_80041290 @0x800412f8.. */
    case 0: result = (cur == (int32_t)arg); break;       /* ==  (0x800412f8) */
    case 1: result = (cur >  (int32_t)arg); break;       /* >   (0x80041308) */
    case 2: result = (cur >= (int32_t)arg); break;       /* >=  (0x8004130c) */
    case 3: result = (cur <  (int32_t)arg); break;       /* <   (0x8004131c) */
    case 4: result = (cur <= (int32_t)arg); break;       /* <=  (0x80041320) */
    case 5: result = (cur != (int32_t)arg); break;       /* !=  (0x8004132c) */
    case 6: result = ((cur & (int32_t)arg) != 0); break; /* &   (0x80041338) */
    default: result = 0; break;                          /* cmp_op>=7 -> false (sltiu<0x7) */
    }
    t->locals[0] = (uint8_t)(result ? 1 : 0);
    t->pc += 6;
    /* byte-true: Member_cmp IS the predicate → return its boolean (FALSE makes the
     * dispatcher pop the If block-stack). Value path ist LITTLE-ENDIAN (lh @0x800412ac),
     * Operatoren 0..6 per switchD @0x800412f8 (==,>,>=,<,<=,!=,&). [#12] */
    return result ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}

/* Phase 4.5.9-E: corrected Sce_em_set encoding per Java disassembler.
 * SCDScriptDisassembler.java:0x44 — 20 bytes total. Position/heading args are
 * LITTLE-ENDIAN (corrected 2026-06-08: the old "all multi-byte args BIG-ENDIAN
 * (BIO1 legacy), same convention as Pos_set / Dir_set / Door_aot_set /
 * Item_aot_set / Aot_set" claim was the disproven all-BE rule — every one of
 * those handlers actually reads LE; the LE pos/dirY here is the very evidence).
 *
 * Layout:
 *   [0]  op (0x44)        [1]  state (slot id, RE2 "be" field)
 *   [2]  enemy_id (type)  [3]  script_id (behavior / initial state)
 *   [4]  sound_bank       [5]  texture
 *   [6]  kill_flag        [7]  spawn_flag
 *   [8..9]   x   (LE s16)
 *   [10..11] y   (LE s16)
 *   [12..13] z   (LE s16)
 *   [14..15] dirX (LE s16 — always 0 per RE15Editor; NPCs only yaw)
 *   [16..17] dirY (LE u16 — actual yaw, T-SCEEM-DIR confirmed: 8/8 NPCs in
 *                  sub15 produce valid Q12 angles 0..4095 only via LE[16..17])
 *   [18..19] dirZ (LE s16 — always 0) */
/* BYTE-TRUE enemy spawn-pose decoder — DISASSEMBLED from the STAGE1.BIN enemy-AI overlay
 * (CD-loaded to RAM 0x80100000; NOT in PSX.EXE, which is why it can't be read from
 * ghidra1_V2.txt). Each per-type AI's state-0 init reads the Sce_em_set behavior byte
 * (entity+0x9 = pc[3]) and stores the INITIAL EDD action index into entity+0x94. We seed
 * a->motion with that index so the enemy spawns in the correct pose; the generic enemy
 * branch in anim_select_common.c loops it.
 *
 *   Zombie family (0x10/0x11/0x12/0x16/0x18/0x1c-0x1f) — decoder @ 0x80100c20:
 *     flags = behavior & 0x80;  sel = behavior & 0x1f;  default action 0, then the
 *     later-wins groups below (every one a literal `ori vN,IMM; sb vN,0x94`). Proven:
 *       0x88 → sel 8,flags → action 0x13 (19)  = lying-down
 *       0x86 → sel 6        → action 0x27 (39)  = feeding/eating
 *       0x02 → flags clear  → action 0     = upright idle
 *   Irons (em45) — state-0 init @ 0x8011d2b8 writes `sb 2,0x94`, BUT room1150's entry
 *     cutscene immediately overwrites it: sub00 → Evt_exec(0x1802) → sub02 = Work_set(2,0);
 *     Plc_motion(0,3,0); Plc_flg(0,4,0). Plc_motion(0,3) routes to the work entity (Irons,
 *     g_actors[1]) and sets action index 3; Plc_flg bit 0x04 = HOLD-LAST-FRAME. So the
 *     ON-SCREEN pose is action 3 held, NOT 2. (MEASURED em45 FK: action 2 = a 50-frame
 *     UPRIGHT thrash loop, action 3 = a rise/sit-up transition ending upright — NEITHER is
 *     flat-lying. NO em45 clip is prone; the true wounded-lying Irons is a separate animation
 *     the original streams for this scene, not in CDEMD0/em45.emd.) We seed 3 as the spawn
 *     default so the held pose is right even before sub02 ticks; the cutscene drives it. */
static uint8_t re15_enemy_spawn_action(uint8_t type, uint8_t behavior)
{
    /* em45 (Chief Irons): RECORD-1 clip 3 = the wounded-lying breathing pose (sub02
     * Plc_motion(0,3)+Plc_flg(0,4)=loop). NOTE (2026-06-15): clip 2 was tried (an agent
     * claimed it = straight legs) but ALL RECORD-1 clips render with a bent left knee on
     * em45's skeleton — the straight-leg discrepancy vs the original is NOT a clip choice
     * (see [[enemy_position_anim_pipeline_re_2026_06_15]] — unresolved leg source). */
    if (type == 0x45) return 3;
    if (type == 0x10 || type == 0x11 || type == 0x12 || type == 0x16 ||
        type == 0x18 || (type >= 0x1c && type <= 0x1f)) {
        int flags = behavior & 0x80;
        int sel   = behavior & 0x1f;
        uint8_t clip = 0;
        if (flags) {
            if (sel == 4 || sel == 7 || sel == 9)    clip = 0x0C;
            if (sel == 5 || sel == 8 || sel == 0x0A) clip = 0x0E;
            if (sel == 1 || sel == 3)                clip = 0x0C;
            if (sel == 4 || sel == 7 || sel == 9)    clip = 0x12;
            if (sel == 5 || sel == 8 || sel == 0x0A) clip = 0x13;
        }
        if      (sel == 6)    clip = 0x27;
        else if (sel == 0x0B) clip = 0x03;
        else if (sel == 0x0D) clip = 0x27;
        else if (sel == 0x0E) clip = 0x2A;
        return clip;
    }
    return 0;   /* other types: their overlay decoders not yet RE'd → spawn at idle action 0 */
}

static int op_sce_em_set(scd_thread_t *t)
{
    uint8_t  slot     = t->pc[1];
    uint8_t  type     = t->pc[2];
    uint8_t  behavior = t->pc[3];
    /* Position fields LE per F1/F2/F3 50-agent dive: reverse-projecting
     * ablauf screenshots through camera matrix shows LE-decoded positions
     * land Elliot on-pad (Y=-7200 floor) and pilot/heli near helicopter,
     * matching the original game. BE puts them off-pad far away.
     * (Sleep + Member_set stay BE per F14 — those have their own evidence.) */
    int16_t  x   = scd_read_le_s16(&t->pc[8]);
    int16_t  y   = scd_read_le_s16(&t->pc[10]);
    int16_t  z   = scd_read_le_s16(&t->pc[12]);
    /* dir = DirectionY at pc[16..17], LE. Per agent T-SCEEM-DIR forensic
     * audit of all 8 NPCs in sub15.scd: only Layout B (LE @ pc[16..17])
     * yields valid Q12 yaws (1199, 400, 2000, 3000, 3600, 320, 3550, 3300).
     * Layout A (BE @ pc[14..15]) yields 0 for every NPC — impossible
     * (each NPC must face a direction). Spec source: RE15Editor's
     * Enemy_sceEmSet_44.java struct. */
    int16_t  dir = scd_read_le_s16(&t->pc[16]);

    if (getenv("RE15_SPAWN_DIAG"))
        fprintf(stderr, "[spawn-diag] Sce_em_set type=0x%02X behavior=0x%02X slot=%u pos=(%d,%d,%d) dir=%d\n",
                type, behavior, slot, x, y, z, dir);

    /* EM-STATUS SPAWN GATE (byte-true FUN_800420a0 @0x80042120-38): pc[7] is the kill-flag index; the
     * spawn is SUPPRESSED when that flag is already set in the stage's em-status zone (a killed or
     * story-cleared enemy stays dead across room re-entry). pc[7]==0xFF bypasses the gate (@0x80042128).
     * The index is latched onto the actor (+0x1C6 @0x80042170) so its death-commit sets the same flag.
     * The em-status banks 0x800b1038/0x800b1058 ARE re15_game_flag zones 7/8 (op_set table [7]/[8],
     * 256-bit MSB-first — identical primitive), and g_game.flags persists across room loads. */
    extern uint8_t re15_em_status_zone(void);
    uint8_t  persist  = t->pc[7];
    int      suppress = (persist != 0xFF) && re15_game_flag_get(re15_em_status_zone(), persist);

    /* GLEICHZEITIG-LIMIT (byte-true @0x80042214-3c) — der Grund, warum ROOM1030 im Original nur
     * SECHS Zombies zeigt, obwohl 20 Records im RDT stehen:
     *   @0x80042214  `lh v0,0(a1)`        v0 = Lebend-Zaehler   (0x800b0ff2 = work_vars[0x11])
     *   @0x8004221c  `lh v1,4084(v1)`     v1 = Maximum          (0x800b0ff4 = work_vars[0x12])
     *   @0x80042224  `slt v0,v0,v1`       Zaehler < Maximum ?
     *   @0x80042228  `bne v0,zero,...`    ja  -> weiter, Zaehler++ (`sh v0,0(a1)` @0x8004223c
     *                                     mit v0 = Zaehler+1 aus dem Delay-Slot @0x8004222c)
     *   @0x80042230  `ori v0,zero,0x8000` nein -> Record VERWORFEN
     *   @0x80042238  `sw v0,0(s0)`             (Entity-Wort +0x0 = 0x8000, kein Spawn)
     * Das Maximum kommt aus dem Opcode Save(0x12, n) (0x24), den ROOM1030 direkt VOR seine
     * Spawn-Liste setzt: RDT-Datei-Offset 0x1de2 = `24 12 06 00`, unmittelbar gefolgt vom ersten
     * `44 00 16 0d`. Vorgabe ist 0xFF (praktisch unbegrenzt) aus der Raum-SCD-Init FUN_8003ecec
     * @0x8003ed58/60 — siehe scd_room_setup.c, dieselbe Funktion setzt auch Zaehler und
     * Flag-Bank-5-Wort-0 zurueck.
     * Game-weit benutzen den Opcode nur ROOM1030/1031 (6) und ROOM1040/1041 (5); ueberall sonst
     * bleibt die Vorgabe 0xFF stehen und das Gate ist wirkungslos.
     *
     * REIHENFOLGE: das Kill-Flag-Gate oben laeuft ZUERST (@0x80042128-38) und verbraucht KEINEN
     * Zaehler — ein bereits erledigter Record macht beim Wieder-Betreten Platz fuer den naechsten
     * aus der Warteschlange. */
    if (!suppress) {
        int16_t live = g_scd.work_vars[0x11];
        int16_t cap  = g_scd.work_vars[0x12];
        if (live < cap) g_scd.work_vars[0x11] = (int16_t)(live + 1);
        else            suppress = 1;                 /* Entity-Wort 0x8000 = Record verworfen */
    }

    int actor_slot = SCRIPT_SLOT_TO_ACTOR(slot);
    if (!suppress && actor_slot < RE15_ACTOR_MAX) {
        re15_actor_t *a = &g_actors[actor_slot];
        a->em_flag_id = persist;            /* +0x1C6 latch (0x80042170) — the death-commit's flag index */
        a->active = 1;
        a->type   = type;
        re15_enemy_apply_hitbox(a, type);   /* byte-true +0x78 hitbox dims (savestate-verified) */
        /* BYTE-TRUE (LAB_800420a0, ghidra1_V2.txt:152547/152580; RE'd 2026-06-29): the
         * Sce_em_set behavior byte pc[3] is the entity's grid_id (+0x9) — the AI sub-dispatch
         * index (FUN_8011d9f4 reads +0x9 & 0xf) PLUS the spawn-pose selector (FUN_80100424 reads
         * +0x9 & 0x1f / & 0x80) PLUS the flag bits (0x40 stationary / 0x20 skip). The MAIN state
         * (+0x4) is initialised to 0 (INIT). The earlier `state = behavior` was a pre-AI-RE
         * mis-map: with the enemy AI now dispatching the main state on +0x4 (PTR_FUN_801217a0)
         * and the sub-mode on +0x9, behavior-in-+0x4 would mis-dispatch every spawned enemy.
         * (The pc[18..19]!=0 "spawn straight to IDLE" variant is DEFERRED — LATENT in STAGE1 (every
         * roster has pc[18]=0 -> state 0). Full byte-true spec (@0x800425a4-e8, RE wf_4a2da55b): read
         * `lh pc[18]` (s2=pc+2, so lh 16(s2)=pc[18]); if !=0 -> +0x4=4, +0x5=(pc[19]-1)&0xff,
         * +0x6=0, +0x7=0, +0x94=pc[18]&0xff (motion, NOT the spawn_action pose), +0x1c4=pc[14] (u16
         * anim_flags). This alternate REPLACES the normal state/motion setup, so wiring it means a
         * conditional restructure of this handler — deferred until a stage authors pc[18]!=0.) */
        a->grid_id = behavior;   /* +0x9 (sub-dispatch + pose-sel + flags) */
        a->state   = 0;          /* +0x4 = INIT */
        a->repath_timer = (uint8_t)(slot & 7);   /* +0x91 = spawn_index & 7 (@0x80042244) — staggers
                                                  * the per-tick nav-DFS across the spawned enemies */
        { extern void re15_enemy_spawn_count_inc(void);
          re15_enemy_spawn_count_inc(); }        /* DAT_800aca4e ++ (@0x80042558-60): the behavior-
                                                  * table pick counts SPAWNS, never decremented */
        a->motion = re15_enemy_spawn_action(type, behavior);  /* byte-true spawn pose */
        /* BYTE-TRUE anim-flags init: Sce_em_set (@0x8004216c) does `sh zero,452(s0)` = +0x1c4 = 0.
         * The original spawns EVERY enemy/NPC with anim_flags = 0 (verified two ways: the disasm
         * store, and the ROOM11B0 savestate where all 3 idle NPCs read +0x1c4 = 0x0000). The prior
         * `settle ? 0 : 0x04` heuristic was a GUESS — it wrongly stamped LOOP(0x04) on every
         * non-settle spawn, so an SCD Plc_motion pose (which the intro re-enter re-spawns AFTER the
         * pose) got its LOOP bit set back on and looped instead of holding. The continuous idle/walk
         * clips DON'T need the 0x04 bit to loop — their looping is driven by the sub-VM handler
         * (e.g. sub 6 idle-FSM @0x800517f0), independent of +0x1c4 & 0x04; that bit ONLY gates the
         * motion-FSM (sub 0 @0x80050cb8) play-once-HOLD. (audit wf_29b40e5d) */
        a->anim_flags = 0;   /* +0x1c4 = 0 @0x8004216c */
        a->flags  = 0x01;          /* visible */
        a->x = (int32_t)x;
        a->y = (int32_t)y;
        a->z = (int32_t)z;
        a->rot_x  = 0;
        a->rot_y  = dir;
        a->rot_z  = 0;
        a->floor  = t->pc[4];   /* byte-true: Sce_em_set seeds the enemy FLOOR/BAND +0x82 from spawn byte
                                 * pc[4] (@0x800421c8 `lbu v0,2(pc+2)` = pc[4]; @0x800421d0 `sb v0,0x82`).
                                 * The port left it struct-zero, so band-gated AI/collision (e.g. the WATER/
                                 * RAMP env-stamp gate, and any per-floor event AOT) read the wrong band for
                                 * a non-floor-0 enemy. (audit wf_27ae1ea7) */
        /* hp=100 is harmless nominal init. Sce_em_set (0x44) carries NO hp
         * field — the `hp` struct member is RE2 +0x1C2 (Member ID 0x13), a
         * SEPARATE field written only via Member_set, not by this opcode.
         *
         * What PSX FUN_800420a0 DOES write from this opcode: entity +0x1BA
         * <- -(pc[4]*1800). w71w6amhp (2026-05-29) reads +0x1BA as a
         * floor/ground-height Y clamp for bone-physics (FUN_8001f024); two
         * earlier agents read it as HP / a root-motion stride-limiter — the
         * meaning is still CONTESTED. It is also INERT here: the reborn engine
         * has no bone-physics consumer, so the value is never read. We do NOT
         * store -(pc[4]*1800) — into hp it would corrupt the Member-0x13 path
         * (+0x1C2 != +0x1BA), and a dedicated +0x1BA field would be dead.
         * Wire it (own field, real value) when bone-physics is ported. */
        /* Leave hp at the struct-cleared 0 (Sce_em_set carries NO hp; the original FUN_800420a0 does
         * not write +0x9a here). A nominal 100 here DEFEATED every non-zombie INIT's byte-true HP seed,
         * which is guarded `if (e->hp <= 0)` (dog/maggot/alligator/tyrant/… only set the table HP when
         * hp<=0): a 100 seed skipped it and locked the enemy at 100 instead of its table row (maggot
         * 180, dog 65-111, boss 300, …). Zombies are unaffected (their INIT sets HP unconditionally).
         * Audit wf_555f18eb. */
        a->hp     = 0;
        a->speed_h = 0;
        if ((uint8_t)(actor_slot + 1) > g_actor_count)
            g_actor_count = (uint8_t)(actor_slot + 1);
    }
    t->pc += 20;                   /* per Java disassembler size table */
    return 1;
}

/* === Phase 4.5.9-C: RDT-driven Door / Item AOTs ============================
 *
 * Door_aot_set (0x3B) — 32 bytes. RE1.5 encoding (per RDTExtractor.java
 * door table + RE2 FUN_8003a9f4 door-pass logic):
 *
 *   [+0]  op (0x3B)
 *   [+1]  slot id
 *   [+2]  sce      (purpose-specific flag)
 *   [+3]  sat      (state)
 *   [+4]  floor
 *   [+5]  super
 *   [+6..7]   rect x  (s16)
 *   [+8..9]   rect z  (s16)
 *   [+10..11] rect w  (s16)  — full width (we convert to half on install)
 *   [+12..13] rect d  (s16)
 *   [+14..15] next_pos_x (s16)
 *   [+16..17] next_pos_y (s16)
 *   [+18..19] next_pos_z (s16)
 *   [+20..21] next_cdir_y (s16)  — heading in target frame
 *   [+22..23] next_stage_room  (u8 stage, u8 room — single-room scope ignored)
 *   [+24..25] next_cut + next_n_floor
 *   [+26..27] target stage_room re-pack
 *   [+28]    light  flags
 *   [+29]    sound
 *   [+30..31] reserved
 *
 * We only consume the fields needed by re15_aot_set_door (rect + target
 * cut + spawn position). Remaining fields are skipped — RE2's full door
 * load (Sca_id, model swap) is deferred. */
static int op_door_aot_set(scd_thread_t *t)
{
    uint8_t  slot       = t->pc[1];
    /* s16 fields are LITTLE-ENDIAN (2026-06-04 fix). They were read BE ("BIO1
     * inheritance") which made the trigger rect huge + mis-placed — door 0's
     * rect became x[-9211,4109] z[-7106,16440] (half the room) instead of the
     * real LE x[1500,3600] z[14400,16100], so pressing action far from any door
     * spuriously fired it. Same class as the Plc_dest X/Z LE fix (op_plc_dest);
     * the RDT RVD zones were always LE (rdt_common parse_zones) — only these
     * SCD-set AOTs were wrong. Verified vs raw main00.scd bytes + the user's
     * "the door wasn't anywhere near where I stood". */
    int16_t  rect_x  = scd_read_le_s16(&t->pc[6]);
    int16_t  rect_z  = scd_read_le_s16(&t->pc[8]);
    int16_t  rect_w  = scd_read_le_s16(&t->pc[10]);
    int16_t  rect_d  = scd_read_le_s16(&t->pc[12]);
    int16_t  nx      = scd_read_le_s16(&t->pc[14]);
    int16_t  ny      = scd_read_le_s16(&t->pc[16]);
    int16_t  nz      = scd_read_le_s16(&t->pc[18]);
    int16_t  ncdir_y = scd_read_le_s16(&t->pc[20]);
    uint8_t  target_cut = t->pc[24];
    /* Compute axis-aligned rect from RDT's (x,z,w,d) where (x,z) is the
     * NW corner and (w,d) is the full extent (so center = (x+w/2, z+d/2),
     * half-extents = (w/2, d/2)). */
    int32_t cx = (int32_t)rect_x + (int32_t)rect_w / 2;
    int32_t cz = (int32_t)rect_z + (int32_t)rect_d / 2;
    int32_t hw = (int32_t)(rect_w < 0 ? -rect_w : rect_w) / 2;
    int32_t hh = (int32_t)(rect_d < 0 ? -rect_d : rect_d) / 2;
    re15_aot_set_door((int)slot, cx, cz, hw, hh,
                       target_cut,
                       (int32_t)nx, (int32_t)ny, (int32_t)nz,
                       ncdir_y);
    /* Cross-room destination (pc[22]=stage, pc[23]=room — Java
     * SCDScriptDisassembler case 0x3B). The AOT scan uses these to trigger a
     * room load when the dest differs from the current room; a dest resolving
     * to the current room is a same-room teleport (the existing DOOR path). */
    if ((int)slot >= 0 && (int)slot < RE15_AOT_MAX) {
        g_aot.door_params[slot].dest_stage = t->pc[22];
        g_aot.door_params[slot].dest_room  = t->pc[23];
        /* pc[3] = the door's FLOOR byte (e.g. 0x31). The AOT door scan gates on the
         * player standing on this floor (band == (floor>>4)+1). */
        g_aot.door_params[slot].floor      = t->pc[3];
        /* pc[4] = the door object's BAND (obj[0x82]); the original gates the door
         * interaction on player_band == obj[0x82] (FUN_8002bd44 @0x8002bf38). */
        g_aot.door_params[slot].band       = t->pc[4];
        g_aot.slots[slot].sce_flags        = t->pc[3];  /* record[1] — bit 0x80 preserved
                                                         * across Aot_reset (@0x8004076c) */
        /* sce = pc[2] (record[0], the byte the scan dispatches on @0x80042f74/9c).
         * sce==0 -> the door installs REGISTERED but INERT (aot_sce_census fix 1:
         * 37 shipped sce-0 doors; 16 are armed later by Aot_reset sce=2, 21 are
         * permanently dead — e.g. ROOM1250 slots 0-2, 1180 slot 4, 5070 slots 1-2).
         * door_params stay stored so the retype arms a working door. The old code
         * never read pc[2] and armed every install. */
        if (t->pc[2] == 0)
            g_aot.slots[slot].type = RE15_AOT_TYPE_NONE;
    }
    /* PC-Vorschub konditional pc[3]&0x80: 4P-Tür (40 B) vs Standard (32 B). Byte-true:
     * LAB_800405bc @0x80040618 `lbu v0,0x3(v1)`; @0x80040620 `andi 0x80`; @0x80040624 `beq`
     * -> @0x80040630 +0x28(40) wenn Bit gesetzt, sonst @0x80040634 +0x20(32) (ghidra1_V2.txt).
     * STAGE1-Türen (z.B. 1240) haben pc[3]=0x31 (Bit clear) -> bleiben 32. 4P-Feld-Offsets
     * (Rect-Punkte ab +0x20) = Folgearbeit; hier nur PC-Sync. [BYTE_TRUE_AUDIT #5] */
    t->pc += (t->pc[3] & 0x80) ? 40 : 32;
    return 1;
}

/* Item_aot_set (0x50) — 22 bytes. Encoding:
 *   [+0]  op (0x50)
 *   [+1]  slot
 *   [+2]  sce
 *   [+3]  sat
 *   [+4]  floor
 *   [+5]  super
 *   [+6..7]   rect x  (s16)
 *   [+8..9]   rect z  (s16)
 *   [+10..11] rect w  (s16)
 *   [+12..13] rect d  (s16)
 *   [+14..15] item_type (s16, low byte = type)
 *   [+16..17] amount    (s16)
 *   [+18..19] item_pickup_action (s16)
 *   [+20..21] flags
 *
 * Same rect-corner-to-center conversion as Door_aot_set. */
static int op_item_aot_set(scd_thread_t *t)
{
    uint8_t  slot   = t->pc[1];
    /* AOT rect is LITTLE-ENDIAN (2026-06-04 fix, same as Door_aot_set). */
    int16_t  rect_x = scd_read_le_s16(&t->pc[6]);
    int16_t  rect_z = scd_read_le_s16(&t->pc[8]);
    int16_t  rect_w = scd_read_le_s16(&t->pc[10]);
    int16_t  rect_d = scd_read_le_s16(&t->pc[12]);
    /* item_type/amount are LITTLE-ENDIAN u16 at +14/+16 (like the rect fields above) — the type is
     * the LE LOW byte pc[14], the count pc[16]. A prior "BE fix" wrongly read the HIGH bytes
     * pc[15]/pc[17], which are 0x00 for every STAGE1 item, so every pickup granted type 0 / amount 0
     * and re15_inv_grant rejected it = you picked up NOTHING (the AOT still deactivated, so the item
     * silently vanished). Verified against the raw RDT: ROOM1050.RDT @0xb9a `...15 00 1e 00...` =
     * handgun ammo (0x15) x30 (0x1e); ROOM1070 @0x156e = 0x15 x15; ROOM1000 @0xc24 = 0x31 x1. The
     * original pickup handler reads lhu block+14 (type) / lbu block+16 (amount) (LAB_80043500). */
    /* The EXTENDED-AOT long form (pc[3]&0x80) inserts 8 extra bytes before the item fields, so
     * type/amount shift +8: byte-true FUN @0x80040668 branches on (pc[3]&0x80) and reads type/amount
     * at record+26/+28 (long, @0x80040670-74) vs record+18/+20 (short, @0x80040680-84) — a +8 delta.
     * The short-form base (pc[14]/pc[16]) is RDT-verified; the long form is therefore pc[22]/pc[24].
     * The port read pc[14]/pc[16] unconditionally while advancing +30 for the long form = garbage
     * item/amount. This is the long-form sibling of the fixed short-form LE-byte bug. */
    int      long_form = (t->pc[3] & 0x80) != 0;
    uint8_t  item_t = long_form ? t->pc[22] : t->pc[14];   /* LE low byte = item type */
    uint8_t  amount = long_form ? t->pc[24] : t->pc[16];   /* LE low byte = amount */
    /* TAKEN-BIT persistence (byte-true @0x800406d4-0x80040718, catalog wf_f536e1ee #14-step-1):
     * the installer re-checks FUN_8004efe4(DAT_800b1078 = flag zone 9, bit = payload[2] =
     * pc[18]/pc[26]) — taken -> the AOT installs DISABLED and the prop object[pc[20]/pc[28]]
     * is hidden (flags=0x80000000). Without this every taken item respawned on room re-entry. */
    uint8_t  tk_bit = long_form ? t->pc[26] : t->pc[18];
    uint8_t  tk_prop = long_form ? t->pc[28] : t->pc[20];
    int32_t cx = (int32_t)rect_x + (int32_t)rect_w / 2;
    int32_t cz = (int32_t)rect_z + (int32_t)rect_d / 2;
    int32_t hw = (int32_t)(rect_w < 0 ? -rect_w : rect_w) / 2;
    int32_t hh = (int32_t)(rect_d < 0 ? -rect_d : rect_d) / 2;
    /* Register ALWAYS (the installer stores rec=pc+2 unconditionally @0x800406d0) with
     * geometry + item params, then decide liveness by the record's sce byte:
     *   - sce = pc[2] == 0 -> INERT install (aot_sce_census fix 1: 2 shipped, ROOM1190/
     *     1191 sub13 slot 5 {0x37,1,tk 136} — armed later by sub10's Aot_reset sce=9).
     *   - taken-bit set -> the installer RETYPES the record to sce 0 in place
     *     (@0x800406f4 `sb zero,0(v0)`) + hides the prop (pool flag |= 0x80000000
     *     @0x800406f8-718). The record STAYS registered/retypeable — the old port code
     *     left the slot uninstalled, which broke a later re-arm of a taken item. */
    re15_aot_set_item_tk((int)slot, cx, cz, hw, hh, item_t, amount, tk_bit);
    if (slot < RE15_AOT_MAX) {
        g_aot.slots[slot].sce_flags = t->pc[3];   /* 0x31 forward / 0x51 centre (all shipped
                                                   * items are ACTION-gated — wf_f536e1ee #5) */
        g_aot.slots[slot].band      = t->pc[4];
        int inert = (t->pc[2] == 0);
        if (tk_bit && re15_game_flag_get(9, tk_bit)) {
            if (tk_prop < g_scd.prop_count)
                g_scd.props[tk_prop].active = 0;           /* hide the pickup prop */
            inert = 1;                                     /* @0x800406f4 rec[0]=0 */
        }
        if (inert)
            g_aot.slots[slot].type = RE15_AOT_TYPE_NONE;
    }
    /* PC-Vorschub konditional pc[3]&0x80: 30 vs 22. Byte-true: LAB_80040644 @0x8004065c
     * `lbu v0,0x3(a2)`; @0x80040664 `andi 0x80`; @0x80040668 `beq` -> +0x1e(30)/+0x16(22)
     * (ghidra1_V2.txt). [BYTE_TRUE_AUDIT #5] */
    t->pc += (t->pc[3] & 0x80) ? 30 : 22;
    return 1;
}

/* Phase 4.5.11: Obj_model_set (0x2D) is dynamic-sized — base 8 bytes plus
 * a variable-length payload terminated either by a 4+ zero-byte run before
 * a valid structural opcode, or by hitting a hard structural terminator.
 * Port of Java SCDScriptDisassembler.determineObjModelEnd (~line 2920) and
 * resolveObjModelTerminator (~line 2978). Without this, our previous fixed
 * 38-byte fallback drifts PC by ±2-6 bytes per Obj_model_set, derailing all
 * downstream opcodes (Door_aot_set, Switch, Evt_exec) in main00 scripts. */
static int op_obj_model_set(scd_thread_t *t)
{
    /* Capture prop position/rotation.
     *   pc[0]=op, pc[1]=obj_id, pc[2..7]=header(6B), pc[8..9]=pad,
     *   pc[10..11]=x, pc[12..13]=y, pc[14..15]=z, pc[18..19]=rot_y.
     * Endianness: LE per F2/F3 reverse-projection from ablauf
     * screenshots — LE puts heli @ (-1161,-7200,1991) near pad center
     * (cam 2 dot=0.995); BE puts it at (30715,-7965,-14585) ~15k units
     * past the pad's east edge. Matches the Sce_em_set position LE fix. */
    /* REVERTED I6 (2026-05-25 user feedback): BE made composition WORSE
     * vs ablauf, not better. Back to LE per PSX.EXE `lh` disasm (P-round)
     * — the engine reads these bytes little-endian. The remaining
     * composition mismatch lives elsewhere (camera projection, prop scale,
     * or DAT_800aca74 world anchor offset — to be re-investigated). */
    uint8_t obj_id   = t->pc[1];
    /* RENDER TYPE = pc[2] (byte-true: LAB_80040914 reads `lbu 0x2(a2)` and stores
     * it to the object pool +0x08 — the exact byte FUN_8002c18c branches on:
     * ==3 → FUN_8003e64c sprite-grid path, !=3 → mesh + whole-object FUN_80014368
     * quad cull). VERIFIED against the real room1170 .scd: ALL 6 props (obj00/01
     * box, obj02-05 heli/rotor/pilot) carry pc[2]=0 → type 0 → mesh + quad cull;
     * room1170 has NO type-3 prop. The OLD `t->pc[4]` read was WRONG — pc[4] is
     * the object's +0x82 BAND field (box=3, heli=5, same field doors use as
     * obj[0x82]), not the render type; it gated rendering only by coincidence
     * (both happened to be ∈ {3,5}). See [[re_obj_type3_is_sprite_pri_not_mesh_2026_06_09]]. */
    uint8_t obj_type = t->pc[2];
    int16_t px = scd_read_le_s16(&t->pc[10]);
    int16_t py = scd_read_le_s16(&t->pc[12]);
    int16_t pz = scd_read_le_s16(&t->pc[14]);
    /* Full Euler rotation triplet at pc[16/18/20] LE. The tail rotor
     * (Obj 0x04) has rot_x=1024 (Q12 90°) to mount its spin axis
     * horizontally on the heli body. Java disassembler reads the same
     * bytes BE, so `Obj_model_set(0x04, ..., 4, 0, 0, ...)` byte pair
     * 0x00 0x04 reads LE as 1024 = 90°. */
    int16_t rx = scd_read_le_s16(&t->pc[16]);
    int16_t ry = scd_read_le_s16(&t->pc[18]);
    int16_t rz = scd_read_le_s16(&t->pc[20]);
    /* Object collision box (6 LE shorts at pc[22..33]): centre X/Y/Z + half-extent
     * X/Y/Z. Authored per-prop in the opcode (verified: obj00 box {0,-900,0,900,
     * 900,900} solid; obj01 heli all-zero = non-collidable). Drives the faithful
     * object push-out (FUN_8002cabc) in re15_collision.c. The 0x2D opcode is a
     * fixed 34 bytes here so pc[22..33] is always present. */
    int16_t bcx = scd_read_le_s16(&t->pc[22]);
    int16_t bcy = scd_read_le_s16(&t->pc[24]);
    int16_t bcz = scd_read_le_s16(&t->pc[26]);
    int16_t bhx = scd_read_le_s16(&t->pc[28]);
    int16_t bhy = scd_read_le_s16(&t->pc[30]);
    int16_t bhz = scd_read_le_s16(&t->pc[32]);
    if (g_scd.prop_count < 16) {
        int i = g_scd.prop_count++;
        g_scd.props[i].active = 1;
        g_scd.props[i].obj_id = obj_id;
        g_scd.props[i].obj_type = obj_type;
        g_scd.props[i].band   = t->pc[4];   /* FLOOR band -> pool+0x82 (byte-true
                                             * LAB_80040914 @0x8004096c-74 `lbu v0,4(a2);
                                             * sb v0,130(a1)`) — read by the AOT scan's
                                             * per-entity band gate in the OBJECT-pool
                                             * pass (sce-5 markers, @0x80042cac) */
        g_scd.props[i].x = (int32_t)px;
        g_scd.props[i].y = (int32_t)py;
        g_scd.props[i].z = (int32_t)pz;
        g_scd.props[i].rot_x = rx;
        g_scd.props[i].rot_y = ry;
        g_scd.props[i].rot_z = rz;
        g_scd.props[i].vel_x  = 0;
        g_scd.props[i].vel_y  = 0;
        g_scd.props[i].vel_z  = 0;
        g_scd.props[i].vel_ry = 0;
        g_scd.props[i].box_cx = bcx;
        g_scd.props[i].box_cy = bcy;
        g_scd.props[i].box_cz = bcz;
        g_scd.props[i].box_hx = bhx;
        g_scd.props[i].box_hy = bhy;
        g_scd.props[i].box_hz = bhz;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[scd] Obj_model_set[%d] id=0x%02X type=%u pos=(%d,%d,%d) rot=(%d,%d,%d)\n",
                i, obj_id, obj_type, px, py, pz, rx, ry, rz);
#endif
    }

    /* Byte-true opcode size = FIXED 34 bytes (0x22). LAB_80040914 advances the
     * thread PC by exactly +0x22 in EVERY (type & 0xc0) branch — `addiu v0,a2,
     * 0x22; sw v0,0x1c(t0)` at 80040aa4/80040aa8; the type bits only select the
     * model-data table (pool +0x74), never the opcode length. CONFIRMED on the
     * real room1170 main00.scd: consecutive 0x2D opcodes sit exactly 34 bytes
     * apart (offsets 80 → 114). The OLD zero-run heuristic (scan for a 4-byte
     * zero run, then a "terminator" opcode, mirroring the Java disassembler's
     * size GUESS) was non-byte-true — it merely happened to land correctly for
     * room1170's data. Replaced with the deterministic +34 the hardware uses. */
    t->pc += 34;   /* OBJ_MODEL_SET_SIZE — byte-true, matches LAB_80040914 */
    return 1;
}

/* Phase 4.5.9-C: unknown opcode → SKIP using the size table so real
 * RDT scripts can flow past opcodes we haven't implemented yet. This
 * mirrors the Java disassembler's defensive 1-byte advance for fully
 * unregistered opcodes (line 760) and uses each opcode's declared size
 * for registered-but-no-handler cases.
 *
 * Side effects of skipping: any state the opcode would mutate stays
 * untouched, so e.g. an enemy that should spawn via a missing Plc_dest
 * just stays put. Better than thread death. */
static int op_unknown(scd_thread_t *t)
{
    uint8_t op   = *t->pc;
    uint8_t size = s_opcode_sizes[op];
    if (size == 0)    size = 1;     /* unregistered — defensive 1B advance */
    if (size == 0xFF) {
        /* Dynamic opcodes — route to specialized handler. */
        if (op == 0x2D) return op_obj_model_set(t);
        size = 34;                  /* dead fallback (0x2D is the only 0xFF and delegates above) */
    }
    t->pc += size;
    return 1;                       /* keep thread alive, continue next opcode */
}

/* =========================================================================
 * Phase 4.5.13 base implementations for opcodes used in ROOM1170 cinematic.
 * Most have minimal "advance PC + basic state mutation" behavior — enough
 * for the cinematic flow to progress without missing visible behavior.
 *=========================================================================*/

/* Aot_on (0x47) — 2 bytes [op, slot]. FIRE-NOW, byte-true LAB_800407bc (aot_sce_census
 * fix 3): resolve the record (DAT_800ac9b0[pc[1]] @0x800407cc-ec) and `jalr
 * PTR_8007469c[rec[0]]` @0x8004082c with a0 = the record payload (@0x80040804/808) — the
 * slot's sce handler runs ONCE immediately, bypassing every geometry/band/action test.
 * NOT an "activate" opcode (the old port behaviour, which only set active=1 and left the
 * fire to the geometry scan). 73 shipped uses, all on sce-2 doors (66 — incl. ROOM1240
 * sub02/sub03 slot 0: `47 00` is the LAST opcode before Evt_end = the intro→ROOM1170
 * handoff; ROOM1080 sub07-10; ROOM4020…) or sce-9 items (7 — scripted grants, e.g.
 * ROOM1051 sub03 slot 12). Dispatch per port type in re15_aot_fire_slot(). */
int op_aot_on(scd_thread_t *t)
{
    uint8_t slot = t->pc[1];
    if (slot < RE15_AOT_MAX) {
        g_aot.slots[slot].active = 1;
        re15_aot_fire_slot((int)slot);
    }
    t->pc += 2;
    return 1;
}

/* Cut_auto (0x3C) — 2 bytes [op, flag]. Toggle automatic cut switching. */
int op_cut_auto(scd_thread_t *t)
{
    /* flag = 1 enable auto-cuts, else disable. Byte-true LAB @0x800403ac: flag==1 CLEARs bit 0x100 of
     * DAT_800aca3c (mask 0xFFFFFEFF), else SETs it (inverse convention — clear = enabled). This IS wired
     * byte-true: op_cut_auto stores g_scd.cut_auto_enabled, and the RVD CAM_SWITCH scan already GATES on
     * it (aot_common.c:372 `if (inside && g_scd.cut_auto_enabled && floor_ok)`). The earlier "scan fires
     * UNCONDITIONALLY / canonical TODO" note was STALE — the gate was since added (freshness audit
     * wf_40eb900f + tier-1 RE wf_4a2da55b confirmed the gate is present + byte-true). No change needed. */
    g_scd.cut_auto_enabled = t->pc[1];
    t->pc += 2;
    return 1;
}

/* Member_set2 (0x35) — 3 bytes [op, member_id, value_var_idx].
 * Like Member_set, but the value comes from the GLOBAL work-var array, not an immediate.
 * Byte-true LAB_80041108: value = (s16)work_vars[pc[2]] — @0x80041138 `lh a2, 0x800b0fd0+pc[2]*2`
 * (DAT_800b0fd0 = g_scd.work_vars[256], the same array op_cmp/op_member_cmp read). Cross-check
 * 2026-06-30 fixed a bug: this used a per-thread `locals[]` u8 placeholder (truncated/zeroed the
 * value and dropped idx 128..255), so any Member_set2 driven by a work-var wrote the wrong value. */
int op_member_set2(scd_thread_t *t)
{
    uint8_t member_id = t->pc[1];
    uint8_t var_idx   = t->pc[2];
    int32_t value     = (int32_t)g_scd.work_vars[var_idx];   /* lh, signed; var_idx 0..255 in range */
    /* BO-round 2026-05-29 (hack audit): per-thread work_slot (AO7 class) — the
     * bare global g_scd.work_slot let a concurrent thread's Work_set leak in.
     * Mirror op_member_set (per-thread primary). */
    int8_t ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    if (ws >= 0) {
        re15_actor_set_member((int)ws, member_id, value);   /* [#11] RE1.5 id direct */
    }
    t->pc += 3;
    return 1;
}

/* Add_aspeed (0x31) — 1 byte. BYTE-TRUE (LAB_80040fd4): apply ACCELERATION to
 * VELOCITY — vel[i] += accel[i] for i=0..5 (the original folds slots 6..11 into
 * 0..5 of the thread's own velocity array at thread+0x158; it does NOT touch the
 * work entity — Add_speed does that next, integrating vel into position). This is
 * how an ACCELERATING motion is built (e.g. the ROOM1130 roller door's rise:
 * Speed_set(7,-2)=accel_y, Add_aspeed ramps vel_y, Add_speed lifts the prop).
 * (The prior rot_y+=vel_ry hack was never exercised by ROOM1170 — its rotor spins
 * via Speed_set(4)+Add_speed, NOT Add_aspeed — so this byte-true rewrite is safe.) */
int op_add_aspeed(scd_thread_t *t)
{
    for (int i = 0; i < 6; i++)
        t->vel[i] = (int16_t)(t->vel[i] + t->accel[i]);
    t->pc += 1;
    return 1;
}

/* Sce_espr_on (0x3A) — 16 bytes. Spawn an effect particle (muzzle flash, blood, …) into the
 * op-0x3a model_inst pool. Byte-true operand map (FUN_80041864 -> FUN_80019700):
 *   pc[2] = effect-id (the bank; FUN_80019700 cat = a0>>24 = pc[2] -> DAT_800b2248[id*4])
 *   pc[3] = sub-index   pc[4] = owner category (0 static / 1 global / 2 entity / 3 pool)
 *   pc[8/10/12] = local offset (LE s16, added to the owner-transform position)   pc[14] = param
 * Owner-transform: the original snapshots a2[0..7] (the owner's matrix) into the slot; the port
 * uses the work-slot actor's world position (the entity running this script) + the local offset.
 * The owner-category -> actor mapping is BYTE-TRUE (self-verified 2026-07-12; per-category base + RotY rotation, commit ee01665b) — see the switch below.
 * Anim/draw run from the room bank via re15_esp_fx_tick / the platform draw. */
int op_sce_espr_on(scd_thread_t *t)
{
    uint8_t effect_id = t->pc[2];
    uint8_t sub_index = t->pc[3];
    uint8_t owner_cat = t->pc[4];   /* andi a0,a1,0xff @0x80041888 — owner category 0..5 */
    uint8_t owner_idx = t->pc[5];   /* a1>>8 @0x800418d4 — owner index within the category array */
    int16_t ox = (int16_t)((uint16_t)t->pc[8]  | ((uint16_t)t->pc[9]  << 8));
    int16_t oy = (int16_t)((uint16_t)t->pc[10] | ((uint16_t)t->pc[11] << 8));
    int16_t oz = (int16_t)((uint16_t)t->pc[12] | ((uint16_t)t->pc[13] << 8));
    int16_t param = (int16_t)((uint16_t)t->pc[14] | ((uint16_t)t->pc[15] << 8));

    /* Byte-true owner-transform base (FUN_80041864 category table @0x80010db4, self-verified
     * 2026-07-12): cat0 = the IDENTITY matrix @0x80072d4c → ABSOLUTE (base 0); cat1 = player
     * position (0x800aca74 = player+0x20); cat2 = enemy[idx].pos (0x800acc4c + idx*0x1F4 = the SAME
     * enemy array Work_set sel 2 uses, +0x20) → g_actors[idx+1] (enemy[0]=g_actors[1]); cat3 =
     * obj[idx].pos (0x800b3fb8 + idx*0x94) → the prop pool. Every espr base = the Work_set entity
     * base + 0x20 (the position field). The OLD stub ignored pc[4]/pc[5] and used the work-slot
     * actor for ALL categories — wrong for cat0 (should be absolute) and cat1 (player) when the
     * running entity isn't the player. BYTE-TRUE ROTATION (savestate-verified 2026-07-13): the effect
     * draws under the owner's WORLD matrix @+0x20 — FUN_80019700 (@0x800197f8-5c) copies that matrix
     * into the inst +0x4c..+0x68, and the player's +0x20 matrix reads as the standard RotY(rot_y)
     * [cos,0,sin][0,1,0][-sin,0,cos] with the actor position as translation (rot_y=4000 -> M=[4051,0,-601]
     * [0,4096,0][601,0,4051], cos=4051 sin=-601). So world = ownerpos + RotY(owner.rot_y)*offset. cat0
     * (identity matrix @0x80072d4c) = absolute, NO rotation. Actors (cat1/2/default) are RotY-only; cat3
     * props also carry rot_x/z (RotY here is byte-true when those are 0, else closer than no-rotate). */
    int32_t bx = 0, by = 0, bz = 0, brot = 0; int rotate = 0;
    switch (owner_cat) {
    case 0: break;                                             /* absolute / identity — no rotation */
    case 1:                                                    /* player */
        bx = g_actors[RE15_ACTOR_SLOT_PLAYER].x;
        by = g_actors[RE15_ACTOR_SLOT_PLAYER].y;
        bz = g_actors[RE15_ACTOR_SLOT_PLAYER].z;
        brot = g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y; rotate = 1; break;
    case 2: {                                                  /* enemy[idx] = g_actors[idx+1] */
        int slot = (int)owner_idx + 1;
        if (slot > 0 && slot < RE15_ACTOR_MAX && g_actors[slot].active) {
            bx = g_actors[slot].x; by = g_actors[slot].y; bz = g_actors[slot].z;
            brot = g_actors[slot].rot_y; rotate = 1; }
        break; }
    case 3:                                                    /* prop pool obj[idx] (direct index) */
        if (owner_idx < g_scd.prop_count && g_scd.props[owner_idx].active) {
            bx = g_scd.props[owner_idx].x; by = g_scd.props[owner_idx].y; bz = g_scd.props[owner_idx].z;
            brot = g_scd.props[owner_idx].rot_y; rotate = 1; }
        break;
    default: {                                                 /* cat4/5: work-slot fallback (as before) */
        int8_t ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
        if (ws >= 0 && ws < RE15_ACTOR_MAX) {
            bx = g_actors[ws].x; by = g_actors[ws].y; bz = g_actors[ws].z;
            brot = g_actors[ws].rot_y; rotate = 1; }
        break; }
    }
    int32_t wx, wy, wz;
    if (rotate) {   /* world = ownerpos + RotY(rot_y)*offset (GTE-accumulate then >>12, sf=1) */
        int32_t cs = re15_cos_q12((int)(brot & 0xfff));
        int32_t sn = re15_sin_q12((int)(brot & 0xfff));
        wx = bx + ((cs * (int32_t)ox + sn * (int32_t)oz) >> 12);   /* row0: cos*ox + sin*oz */
        wy = by + (int32_t)oy;                                     /* row1: oy (RotY leaves Y) */
        wz = bz + ((-sn * (int32_t)ox + cs * (int32_t)oz) >> 12);  /* row2: -sin*ox + cos*oz */
    } else {
        wx = bx + (int32_t)ox; wy = by + (int32_t)oy; wz = bz + (int32_t)oz;   /* cat0 identity */
    }
    re15_esp_fx_spawn(re15_esp_room_bank(), effect_id, sub_index, wx, wy, wz, param);
    t->pc += 16;
    return 1;
}

/* Sce_espr_kill (0x4C) — 18 bytes (war fälschlich 5). Byte-true: dispatch table
 * 0x800745d8→LAB_80040858, `addiu v1,v1,0x12` @0x80040890 = +18 (ghidra1_V2.txt;
 * Store nach DAT_800b2360-Slot). Voll-Semantik (Slot-Tabelle) = 2. Runde; dieser
 * Stub hält nur den PC byte-genau in Sync. [BYTE_TRUE_AUDIT #4] */
int op_sce_espr_kill(scd_thread_t *t)
{
    t->pc += 18;
    return 1;
}

/* Plc_gun (0x4A) — 2 bytes [op, state]. Set player gun state (drawn/holstered). */
int op_plc_gun(scd_thread_t *t)
{
    /* state: 0 = holstered, 1 = drawn. Just record. */
    g_scd.plc_gun_state = t->pc[1];
    t->pc += 2;
    return 1;
}

/* Cut_replace (0x4B) — 3 bytes [op, a, b]. Byte-true LAB_80040414: SWAP cut ids a<->b in
 * cam_from AND cam_to of EVERY RVD camera zone (terminator 0xFF). The camera transitions
 * AND the per-cut region-quad cull then use the swapped cuts. ROOM1130 sub00 (gate opened,
 * flag(3,107)=1) runs Cut_replace(9,8) so on re-entry the gate-approach zone frames the
 * closed gate via cut 8 (= the original.png view) instead of cut 9 — the camera change the
 * user saw missing. The swap persists until the room reloads (which re-parses the zones). */
int op_cut_replace(scd_thread_t *t)
{
    uint8_t a = t->pc[1], b = t->pc[2];
    if (s_current_rdt) {
        re15_rdt_t *rdt = (re15_rdt_t *)s_current_rdt;   /* parsed zones copy — safe to mutate */
        for (int i = 0; i < rdt->zone_count; i++) {
            if      (rdt->zones[i].cam_from == a) rdt->zones[i].cam_from = b;
            else if (rdt->zones[i].cam_from == b) rdt->zones[i].cam_from = a;
            if      (rdt->zones[i].cam_to   == a) rdt->zones[i].cam_to   = b;
            else if (rdt->zones[i].cam_to   == b) rdt->zones[i].cam_to   = a;
        }
    }
    {
        extern void re15_aot_cut_replace(uint8_t a, uint8_t b);
        re15_aot_cut_replace(a, b);   /* also swap the live CAM_SWITCH AOTs (cam_from/target) */
    }
    /* KEIN Kamera-Wechsel im Schwanz — das Original macht hier ein ZEIGER-FIXUP, keine Umschaltung.
     *
     * Der frueher hier stehende Block ("if (active_cut == a) -> cam_id = b, cam_change_pending = 1",
     * eingebaut 2026-06-30 mit dem Guess-Tell "faithful-line" im Kommentar) war eine Fehllesung von
     * LAB_80040414. Selbst disassembliert:
     *   @0x80040438 `lbu a2,1(v1)`   a2 = pc[1] = a (alter Cut)
     *   @0x80040440 `lbu a1,2(v1)`   a1 = pc[2] = b (neuer Cut)
     *   @0x8004044c-a8  die TAUSCH-Schleife (Stride 20 @0x80040498, Ende bei Byte 0xff
     *                   @0x800404a4): jeder Datensatz, der a traegt, bekommt b — UND umgekehrt.
     *   @0x800404ac-b8 `lw v0,0x800ac794` + `lbu v0,2(v0)`  = das cam_from-Byte des ANKER-Datensatzes,
     *                   gelesen NACH dem Tausch.
     *   @0x800404c0    `bne v0,a2, ...`  -> nur weiter, wenn der Anker JETZT a traegt.
     *   @0x800404c8/cc `jal FUN_800142f4` mit `a0 = a1` (= b).
     * FUN_800142f4 @0x800142f4-14 schreibt `sb a0,0x800afbb5` (der angezeigte Cut-Index) und
     * re-resolviert per FUN_80014324 den ANKER-ZEIGER `sw v0,0x800ac794`.
     *
     * Traegt der Anker nach dem Tausch a, dann trug er VOR dem Tausch b — der Aufruf schreibt also
     * genau den Wert zurueck, der ohnehin angezeigt wurde, und richtet nur den durch das
     * Umetikettieren stale gewordenen Zeiger neu aus. Das Original wechselt hier NIE die Ansicht.
     *
     * Der Port hat kein Gegenstueck zu diesem Anker-Zeiger (er schlaegt Zonen ueber den Index nach),
     * also ist die byte-true Wirkung auf sein Datenmodell: nichts. Der alte Block testete zudem die
     * Bedingung, die das Original gerade AUSSCHLIESST (active == a), und routete das Ergebnis ueber
     * `cam_change_pending` — was room_common.c als "das Skript hat einen Cut gesetzt" wertet und
     * damit den Tuer-Eintritts-Cut verwirft. Gemessen an ROOM1030: die Tuer 1040->1030 will Cut 8,
     * der Port lieferte Cut 9 (byte-gleich zu Cut 0 = die Halle mit den 20 Zombies) — der Spieler
     * stand unsichtbar am Tuer-Spawn, die Kamera auf den Zombies (Nutzer-Report 2026-08-06).
     * Der Defekt war bis e53fb043 unerreichbar, weil ROOM1030s Cut_replace hinter Ck(4,15)==1
     * (@Datei 0x1ff0) haengt und flag(4,15) nur sub01 @0x2198 setzt — sub01 lief vorher nie. */
    t->pc += 3;
    return 1;
}

/* Cut_old (0x2A) — 1 byte. Revert to previous cut. */
int op_cut_old(scd_thread_t *t)
{
    g_scd.cam_id = g_scd.cam_id_prev;
    g_scd.cam_change_pending = 1;
    /* byte-true FUN_8004032c @0x8004032c loescht DAT_800aca3c Bit 0x100 -> Auto-Cam-Scan
     * AN. Port-Inverse: cut_auto_enabled = 1. [#16] */
    g_scd.cut_auto_enabled = 1;
    t->pc += 1;
    return 1;
}

static re15_sca_entry_t *sca_entry_at(uint8_t region, uint8_t index);   /* defined below */

/* Flr_set (0x38) — 12 bytes. Byte-true LAB_800417ac (dispatch table 0x80074588; `addiu a1,a1,0xc`
 * @0x800417f0 = +12). RE'd 2026-07-12 from the raw handler: it MUTATES a live SCA collision entry
 * — the same region_table[region+1][index] addressing as its siblings Sca_id_set (0x37 → u0) and
 * Sca_floor_set (0x39 → floor), i.e. sca_entry_at(). Operand layout (pc[1] is NOT read — a reserved
 * selector byte): region = pc[2] (lbu +2), index = pc[3] (lbu +3), and it writes four halfwords of
 * the 12-byte entry: entry+0 = u16@pc[8] (width), +2 = u16@pc[10] (density), +4 = s16@pc[4] (x),
 * +6 = s16@pc[6] (z) — `sh a3,0 / sh a2,2 / sh t0,4 / sh t1,6` @0x800417f8-804. So it repositions/
 * resizes a collision shape at runtime (type/u0/u1/floor untouched). Was a PC-only stub.
 * NOTE (RE15_FLR_LOG live sweep 2026-07-12): does NOT fire on plain room LOAD in any tested STAGE1
 * room (1000/1010/1080/1110/1130/1140/1190/11E0/1260 + *1 scenario-B variants) — it is EVENT-
 * triggered (a sub_scd run on a puzzle/door event), like siblings 0x37/0x39. The offline SCD-walker's
 * per-room 0x38 counts are DESYNC false-positives (0x38 bytes in data regions), so the live effect is
 * unconfirmed; the logic is byte-true to LAB_800417ac and unit-tested via the real dispatch
 * (test_flr_set_mutates). */
int op_flr_set(scd_thread_t *t)
{
    re15_sca_entry_t *e = sca_entry_at(t->pc[2], t->pc[3]);   /* region=pc[2], index=pc[3] */
    if (e) {
        e->width   = (uint16_t)(t->pc[8]  | (t->pc[9]  << 8));   /* +0 = u16 @pc[8]  */
        e->density = (uint16_t)(t->pc[10] | (t->pc[11] << 8));   /* +2 = u16 @pc[10] */
        e->x       = (int16_t)((uint16_t)(t->pc[4] | (t->pc[5] << 8)));   /* +4 = s16 @pc[4] */
        e->z       = (int16_t)((uint16_t)(t->pc[6] | (t->pc[7] << 8)));   /* +6 = s16 @pc[6] */
    }
    t->pc += 12;
    return 1;
}

/* Resolve a live SCA entry by (region, index) the way the original handlers do
 * (FUN @LAB_8004175c / LAB_80041814): the 5 SCA partition groups are stored
 * contiguously after the 24-B header in region order, so region pointer +
 * index*12 == flat[(sum of the preceding group counts) + index]. The SCA aliases
 * the resident RDT buffer (const) — cast away to patch in place; the change
 * persists until the room reloads, exactly like the original which mutates the
 * loaded SCA in RAM. No per-group bound (the original trusts the script and the
 * groups are contiguous), only a global guard against memory corruption. */
static re15_sca_entry_t *sca_entry_at(uint8_t region, uint8_t index)
{
    if (!s_current_rdt || !s_current_rdt->sca || region >= 5) return NULL;
    int base = 0;
    for (int i = 0; i < region; i++) base += s_current_rdt->sca_rgn[i];
    int fi = base + index;
    if (fi < 0 || fi >= s_current_rdt->sca_count) return NULL;
    return (re15_sca_entry_t *)&s_current_rdt->sca[fi];
}

/* Sca_id_set (0x37) — 4 bytes [op, region, index, value]. Byte-true port of
 * LAB_8004175c: writes `value` to the SCA entry's u0 flag byte (offset +0x9).
 * Bit 0 of u0 gates solidity in re15_collision.c (0 = status-only / non-solid).
 * The ROOM1130 roller-door sub02 sets u0=0 on index 10 of every group once the
 * shutter has risen → opens the corridor the player can then walk through. */
int op_sca_id_set(scd_thread_t *t)
{
    re15_sca_entry_t *e = sca_entry_at(t->pc[1], t->pc[2]);
    if (e) e->u0 = t->pc[3];
    t->pc += 4;
    return 1;
}

/* op 0x39 — 4 bytes [op, region, index, value]. Byte-true port of LAB_80041814:
 * same addressing as Sca_id_set but writes the entry's FLOOR byte (offset +0xb).
 * ROOM1130 sub02 sets floor=0 on the freed door cells (band 0 = the corridor
 * level). (The Java disassembler mislabels this opcode "Dir_ck" — cosmetic.) */
int op_sca_floor_set(scd_thread_t *t)
{
    re15_sca_entry_t *e = sca_entry_at(t->pc[1], t->pc[2]);
    if (e) e->floor = t->pc[3];
    t->pc += 4;
    return 1;
}

/* Evt_kill (0x05) — 2 bytes [op, slot]. Disable event thread. */
int op_evt_kill(scd_thread_t *t)
{
    uint8_t slot = t->pc[1];
    if (slot < SCD_THREAD_COUNT) {
        g_scd.threads[slot].active = 0;
        g_scd.threads[slot].kill_pending = 1;
    }
    t->pc += 2;
    return 1;
}

/* Evt_chain (0x03) — 4-byte [op, _, _, sub_id]. [#10] BYTE-TRUE (LAB_8003f270 →
 * FUN_8003edec): reinitialize the CURRENT thread IN PLACE to run sub_scd[sub_id].
 * sub_id = (u8)PC[3] (lbu @0x8003f280). FUN_8003edec does NOT advance PC and does
 * NOT memset the whole thread — it only resets the control-flow state and repoints
 * PC (locals/sleep/work + call depth survive):
 *   active=1 (@0x8003edf0) · loop/nesting counter -1 (@0x8003ee04 → port empty=0)
 *   block-stack ptr reset (@0x8003ee14) · PC = sub_table_base+base[sub_id*2]
 *   (@0x8003ee38). Was a No-Op before this fix (the chained sub never ran). */
int op_evt_chain(scd_thread_t *t)
{
    uint8_t sub_id = t->pc[3];
    const uint8_t *target = (s_current_rdt && sub_id < RE15_RDT_MAX_SUB_SCD)
                          ? s_current_rdt->sub_scd[sub_id] : NULL;
    if (!target) { t->pc += 4; return 1; }   /* no such sub → advance (port guard) */
    t->pc         = target;   /* thread[0x1c] */
    t->loop_count = 0;        /* thread[0x8] (PSX -1 = port's empty sentinel 0) */
    t->block_sp[(int)t->call_depth] = 0;   /* [#17] thread[0x140] block-stack reset (current frame) */
    /* active stays 1; call_depth/work_slot preserved (FUN_8003edec leaves them). */
    return 1;
}

/* (0x52) — RE1.5 = 4 bytes. byte-true LAB_8004295c is a PAD-AND CONDITION CHECK predicate
 * (NOT a sprite control; RE2's Sce_espr_control is a different opcode): param=pc[1], mask=LE u16
 * @pc[2]; cond = (mask & DAT_800ac76c) != 0 ? param : (param^1); PC+=4. It is instruction-for-
 * instruction identical to Sce_key_ck (0x51 @0x80042920) except the source word is DAT_800ac76c —
 * the PRESSED-EDGE (newly-pressed) sibling of 0x51's held DAT_800ac768. Both are the VIRTUAL
 * (config-remapped) words built by FUN_80030444 @0x800304b8-e4 from the preset table
 * @0x80073dbc — script masks are VIRTUAL-space (wave-6 finding 4: 0x4000 <- raw SQUARE,
 * 0x8000 <- raw CROSS, 0x1000/0x2000 <- raw d-pad L/R). The port publishes 76c as
 * g_scd_pad_edge = re15_pad_virtual_word(pad_pressed) (game_step_common.c), so the mask is
 * applied to the real virtual press-edge. (No-input still yields param^1 — regression-free
 * without live input.) */
int op_sce_espr_control(scd_thread_t *t)
{
    extern uint16_t g_scd_pad_edge;
    uint8_t  param = t->pc[1];
    uint16_t mask  = (uint16_t)(t->pc[2] | (t->pc[3] << 8));   /* LE u16 @pc[2] */
    int      cond  = (mask & g_scd_pad_edge) != 0 ? (param != 0) : ((param ^ 1) != 0);
    t->pc += 4;
    return cond ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}
/* Sce_bgm_control (0x54) — 6 bytes [op, slot, ctl, _, _, _]. The PSX handler
 * (jump-table idx 0x54 → FUN_80044da4) is the SsSeq software-sequencer slot
 * control. byte[1]=seq slot, byte[2]=ctl (1=SsSeqSetVol+SsSeqPlay loop, 2=SsSeqStop,
 * 3=replay, 4=pause, 5=decrescendo fade). ROOM1170 sub02 issues PLAY/STOP on slot 1
 * at the cut boundaries — that slot is the helicopter-rotor SUB layer, so this is the
 * canonical per-cut rotor on/off (heli-arrival + sky-view = PLAY, Leon's dialogue
 * close-ups = STOP). RE 2026-05-31 (code-traced, ghidra1_V2.txt:156716). NOTE: in
 * RE1.5 this is opcode 0x54; retail RE2 puts the same Sce_bgm_control at 0x51 and
 * reuses 0x54 for Sce_espr3d_on (BIO 1.5→2.0 opcode-table shift). The old
 * "Sce_espr_control2" label (Java extractor) was wrong and has been corrected. */
int op_sce_bgm_control(scd_thread_t *t)
{
    /* FULL payload (byte-true @0x800429b4-e8: the handler packs FIVE operand bytes
     * pc[1]<<28|pc[2]<<24|pc[3]<<16|pc[4]<<8|pc[5] for FUN_80044da4): pc[3] = part
     * (0 = VAB master, else program part-1), pc[4] = volume+1, pc[5] = pan+1 — nonzero
     * bytes write vol-1/pan-1 into the slot's VAB mvol/mpan (VabHdr+0x18/+0x19 or
     * ProgAtr+1/+4), which note-ons re-read. The old producer dropped pc[3..5], so
     * ROOM1090's program mute + ROOM11F0/11F1's master scale never happened.
     * [audit wf_1db9c802 SCD-BGMCTL-OPERAND-PAYLOAD] */
    scd_audio_event_t evt = {0};
    evt.kind      = SCD_AUDIO_SEQ_CTL;
    evt.bank      = t->pc[1];   /* sequence slot (0 MAIN / 1 SUB SEQ0 / 2 SUB SEQ1) */
    evt.volume    = t->pc[2];   /* control op */
    evt.sample_id = t->pc[3];   /* part selector */
    evt.raw_w0    = (uint16_t)t->pc[4];   /* volume byte (vol-1 written when nonzero) */
    evt.pan       = t->pc[5];   /* pan byte (pan-1 written when nonzero) */
    enqueue_audio(&evt);
    t->pc += 6;
    return 1;
}
/* (0x53) — 3 bytes. MISLABELED "Sce_fade_set": the RE1.5 handler LAB_80040e18 is NOT a fade — it is
 * an INDIRECT Work_set, byte-for-byte identical to Work_set (0x2E, 0x80040d2c) EXCEPT the entity
 * index is read indirectly: idx = (s16)work_vars[pc[2]] (`lh 0x800b0fd0+pc[2]*2` @0x80040e58), where
 * Work_set uses pc[2] directly. sel=pc[1] (lbu +1 @0x80040e40). It clears the 6 vel/accel words then
 * binds the per-thread work entity: sel 1→player(0x800ACA54), 2→enemy[idx×0x1F4], 3→obj[idx×0x94],
 * 5→*(effect_ptr_tbl[idx]); sel 0/4/other take `j 0x80040f08` with NO store (work entity UNCHANGED,
 * only the vel clear applies). RE'd + self-verified 2026-07-12 (wf_9143c15b; the tool annotates the
 * player store 0x800ACA54 as 0x800aba84 — a lui+addiu bug: v1 is reloaded to 0x800b0fd0 @0x80040e5c).
 * Emitted instruction-aligned in ROOM1030 (5×, sel=2, ids work_vars[4/5]=0); ROOM1140/1150 have zero
 * aligned 0x53. sel==5 (effect/sprite-ptr pool @0x800B23F4) has no port equivalent → left unbound
 * (faithful residual). Was a pure PC-advance stub. */
int op_sce_fade_set(scd_thread_t *t)
{
    uint8_t sel = t->pc[1];                       /* lbu 1(pc) @0x80040e40 */
    int16_t idx = g_scd.work_vars[t->pc[2]];      /* lh work_vars[pc[2]] @0x80040e58 (signed) */
    if (sel == 1 || sel == 2 || sel == 3)         /* store paths; sel 0/4/5/other: no rebind */
        scd_work_bind(t, sel, (uint8_t)idx);      /* clears vel/accel + binds the indirect entity */
    else
        scd_work_clear(t);                        /* sel 0/4/5: only the unconditional vel clear */
    t->pc += 3;                                   /* @0x80040e44 */
    return 1;
}
/* Sce_shake_on (0x5C) — RE1.5 = 4 bytes (disasm LAB_80042a10; retail RE2 = 3). */
int op_sce_shake_on(scd_thread_t *t)      { t->pc += 4; return 1; }
/* Plc_rot (0x58) — 4 bytes. NOTE: the "Plc_rot/rotate" name is from retail RE2;
 * RE1.5's real 0x58 handler (LAB_8003fd54) is a Ck-style FLAG-CHECK evaluator
 * (reads a packed flag descriptor at +2, returns a boolean), NOT a rotation —
 * and NO RE1.5 room dispatches 0x58 (0 calls in all 1920 .scd). The +2 operand
 * is read `lhu` = LITTLE-ENDIAN (LAB_8003fd54 @8003fd5c). We keep the (unused,
 * harmless) rot_y write but read LE for correctness/consistency.
 * (corrected 2026-06-08: was scd_read_be_s16; verified LE vs the PSX handler.) */
/* 0x58 — byte-true LAB_8003fd54 is a FLAG-BANK BIT-CHECK PREDICATE, NOT player rotation. The
 * previous `op_plc_rot` (set g_actors[ws].rot_y = s16@pc[2]) was an RE2 misattribution — RE1.5
 * 0x58 reads a flag bank and returns a boolean for the enclosing If (cross-check 2026-06-30).
 * 4 bytes [op, bank, desc_lo, desc_hi]. desc = u16@pc[2], bank selector = pc[1]:
 *   raw  = work_vars[desc & 0xff]                 (s16, DAT_800b0fd0)
 *   word = (s16)raw >> 5,  bit = desc & 0x1f       (MSB-first mask 0x80000000>>bit)
 *   cond = (flagbank[bank][word] & mask) != 0,  then XOR ((desc>>8)==0)   (the optional invert,
 *          @0x8003fd8c-90 `sra/sltiu` + @0x8003fdcc `xor`).  PC += 4; return cond.
 * The port flag model (re15_game_flag_get -> g_game.flags[bank][8], 256 bits/zone, MSB-first —
 * the identical convention, game_state.c:29-32) covers word 0..7; word>=8 (raw>=256, or raw<0)
 * is beyond it -> cond=0 (guard; the original would read adjacent flag memory, unmodelled). */
int op_flag_ck2(scd_thread_t *t)
{
    uint16_t desc = (uint16_t)(t->pc[2] | (t->pc[3] << 8));
    uint8_t  bank = t->pc[1];
    int16_t  raw  = g_scd.work_vars[desc & 0xff];
    int      word = (int)raw >> 5;
    int      bit  = (int)(desc & 0x1f);
    int      cond = (word >= 0 && word < RE15_FLAG_WORDS_ZONE)
                  ? re15_game_flag_get(bank, (uint8_t)((word << 5) | bit))
                  : 0;
    if (((desc >> 8) & 0xff) == 0) cond ^= 1;      /* (desc>>8)==0 -> invert */
    t->pc += 4;
    return cond ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}
/* (0x5A) — 6 bytes. MISLABELED "Weapon_chg" (RE2 2-byte name): the RE1.5 handler LAB_80041474 is a
 * work-entity member RMW with an IMMEDIATE operand — the sibling of 0x5B (Plc_cnt), which takes the
 * operand from work_vars. Self-verified 2026-07-12 (wf_9143c15b caught its own "benign" misclass):
 *   s1 = lhu @pc[2] (@0x80041494); op = s1&0xff = pc[2] (andi @0x800414a4); field = s1>>8 = pc[3]
 *   (srl @0x800414a0); value = (s16)lh @pc[4] (@0x80041498). dest = FUN_80041554(work_entity, field)
 *   = the SAME 0..19 field resolver as Member_set/0x5B; FUN_80040140(op, dest, value) = scd_work_alu
 *   16-bit RMW → member[pc[3]] OP= (s16)pc[4..5]. Returns 1 (not a predicate); pc+=6. pc[1] unused.
 * Writes position/rotation/flags the game READS (render/collision) — was a PC-advance stub. */
int op_weapon_chg(scd_thread_t *t)
{
    uint8_t op        = t->pc[2];   /* op_selector = (lhu pc+2) & 0xff */
    uint8_t field_idx = t->pc[3];   /* member_index = (lhu pc+2) >> 8 */
    int16_t value     = (int16_t)((uint16_t)t->pc[4] | ((uint16_t)t->pc[5] << 8));   /* lh pc+4 */
    int8_t  ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;
    if (ws >= 0 && field_idx < 0x14) {
        int16_t cur = (int16_t)re15_actor_get_member((int)ws, field_idx);
        scd_work_alu(op, &cur, value);
        re15_actor_set_member((int)ws, field_idx, (int32_t)cur);
    }
    t->pc += 6;
    return 1;
}
/* (0x5B) — 4 bytes [0x5B, op, field_idx, var_idx]. byte-true LAB_800414e0 (self-verified 2026-07-12,
 * wf_9143c15b): a work-ENTITY member read-modify-write. dest = FUN_80041554(work_entity=thread+0x154,
 * pc[2]) resolves member offset (sltiu pc[2],0x14 → the SAME 0..19 field map as Member_set FUN_8004116c);
 * operand = (s16)work_vars[pc[3]] (`lh 0x800b0fd0+pc[3]*2` @0x80041524); FUN_80040140(op=pc[1], dest,
 * operand) applies the shared 12-op ALU (= scd_work_alu) as a 16-bit RMW → work_entity.member[pc[2]]
 * OP= work_vars[pc[3]]. Returns 1 (NOT a predicate). Was a pure PC-advance stub. Per-thread work_slot
 * like Member_set2. (Field-semantic model: the port get/set_member is the faithful 0..19 mapping; the
 * PSX 16-bit RMW's high-half/neighbor-byte nuance is absorbed by modelling fields semantically.) */
int op_plc_cnt(scd_thread_t *t)
{
    uint8_t op        = t->pc[1];   /* ALU operator (a0 → FUN_80040140 @0x80041528) */
    uint8_t field_idx = t->pc[2];   /* work-entity member index 0..19 (FUN_80041554 @0x800414f8) */
    uint8_t var_idx   = t->pc[3];   /* work_vars operand index (lh @0x80041524) */
    int8_t  ws = (t->work_slot >= 0) ? t->work_slot : g_scd.work_slot;   /* thread+0x154 target */
    if (ws >= 0 && field_idx < 0x14) {                                    /* sltiu a1,0x14 @0x80041554 */
        int16_t cur = (int16_t)re15_actor_get_member((int)ws, field_idx);
        scd_work_alu(op, &cur, g_scd.work_vars[var_idx]);                 /* (s16) operand */
        re15_actor_set_member((int)ws, field_idx, (int32_t)cur);
    }
    t->pc += 4;                                                           /* @0x80041538 */
    return 1;                                                             /* ori v0,1 @0x80041534 */
}
/* Member_calc (0x55) — 6 bytes (RE1.5 == RE2). */
int op_member_calc(scd_thread_t *t)       { t->pc += 6; return 1; }
/* Member_calc2 (0x56) — RE1.5 = 6 bytes (disasm LAB_80042a58 reads b@1/b@2/b@3/h@4,
 * PC+=6; retail RE2 = 4). */
/* (0x56) — byte-true FADE-CHANNEL CONFIG (handler @0x80042a58, trace wf_2c73ab52 self-verified):
 * `ori a3,7` bucket, ch=pc[1] | ABR=pc[2]<<8 (`lbu 2; sll 8; or`), rgb mask=pc[3], step=lh pc[4]
 * -> jal FUN_800217b0, PC+=6. The old "Member_calc2" name was the RE2 carryover for the same
 * 6-byte length; the handler is the fade config that precedes every scripted 0x57 fade
 * (ROOM11F0/11F1 @0x174a, ROOM3080 @0x81e: `56 00 02 07 00 00` = ch0 subtractive white static). */
int op_fade_config(scd_thread_t *t)
{
    re15_fade_config(t->pc[1], t->pc[2], t->pc[3], scd_read_le_s16(&t->pc[4]), 7);
    t->pc += 6;
    return 1;
}
/* Kage_set (0x60) — 14 bytes. Shadow volume config. */
int op_kage_set(scd_thread_t *t)          { t->pc += 14; return 1; }
/* Cut_be_set (0x61) — 4 bytes. Camera transition config. */
int op_cut_be_set(scd_thread_t *t)        { t->pc += 4; return 1; }
/* Xa_vol (0x5F) — 2 bytes. XA volume. */
int op_xa_vol(scd_thread_t *t)            { t->pc += 2; return 1; }
/* Sce_key_ck (0x51) — 4 bytes. */
int op_sce_key_ck(scd_thread_t *t)
{
    /* byte-true predicate LAB_80042920: param=pc[1], mask=LE u16 @pc[2]; cond = (mask &
     * DAT_800ac768) != 0 ? param : (param^1); PC+=4. The `bne v1,zero` takes the key-pressed
     * branch (delay slot `addu v0,a1` = param); the no-key fall-through is `xori v0,a1,0x1` =
     * param^1. DAT_800ac768 = the per-frame HELD VIRTUAL (config-remapped) pad word built by
     * FUN_80030444 @0x800304b8-e4 from the preset table @0x80073dbc — script masks are
     * VIRTUAL-space (wave-6 finding 4: 0x4000 <- raw SQUARE, 0x8000 <- raw CROSS, d-pad
     * virt 0x1/0x2/0x4/0x8 + 0x10/0x20 + menu-L/R 0x1000/0x2000 <- raw d-pad). Published as
     * g_scd_pad_held = re15_pad_virtual_word(pad_current) (game_step_common.c). So the mask is
     * applied directly. (Earlier this hard-coded the no-key case `param^1` because g_scd_pad_held did not
     * yet exist; now it is byte-true. No-input still yields param^1 — the common case — so nothing
     * changes unless a mask button is actually held that frame; e.g. ROOM1080's 8 per-frame
     * `Ifel_ck{Sce_key_ck(1,<bit>)}` input-poll predicates now respond to the pad.) */
    extern uint16_t g_scd_pad_held;
    uint8_t  param = t->pc[1];
    uint16_t mask  = (uint16_t)(t->pc[2] | (t->pc[3] << 8));   /* LE u16 @pc[2] */
    int      cond  = (mask & g_scd_pad_held) != 0 ? (param != 0) : ((param ^ 1) != 0);
    t->pc += 4;
    return cond ? SCD_R_CONTINUE : SCD_R_IF_FALSE;
}
/* (0x5E) — RE1.5 = 4 bytes (disasm LAB_80042b04; retail RE2 Keep_Item_ck = 2). */
int op_keep_item_ck(scd_thread_t *t)      { t->pc += 4; return 1; }
/* Sce_item_lost (0x62) — 2 bytes. */
int op_sce_item_lost(scd_thread_t *t)     { t->pc += 2; return 1; }
/* Plc_gun_eff (0x63) — 1 byte. Trigger gun muzzle flash. */
int op_plc_gun_eff(scd_thread_t *t)       { t->pc += 1; return 1; }
/* (0x5D) — RE1.5 = 4 bytes (disasm LAB_8004203c; retail RE2 Mizu_div_set = 2). */
int op_mizu_div_set(scd_thread_t *t)      { t->pc += 4; return 1; }
