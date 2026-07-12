/*
 * item_modal_common.c — Item-get "You got X" presentation MODAL (byte-true FUN_8001db28).
 *
 * The hardcoded RE1.5 item-pickup presentation (state byte DAT_80072d3b, 9-case jump table
 * @0x800106b4). Ported from the raw disasm (workflow wq41xdnn2, 3 finders + self-verified). Every
 * behavioural constant cites its address. Shared PC+PSX (engine tier). See re15_item_modal.h for the
 * flow + the faithful-line ledger.
 *
 * FLOW (normal): 1 INIT -> 2 ZOOM-IN (17f) -> 3 gate -> 4 FLIP (9f) -> 5 grant-check+SE -> 6 fade gate
 *   -> 7 INSERT (state=0 DONE).   FULL-inventory: state 7 branches -> 8 SHRINK-away (17f, no grant).
 *
 * GEOMETRY (FUN_8001e1c8): corner tables X@0x80072d3c = {-56,+56,-56,+56}, Y@0x80072d44 =
 *   {-36,-36,+36,+36} (TL,TR,BL,BR); box 112x72, screen-centred by +160 (0xa0 @0x8001e328) / +108
 *   (0x6c @0x8001e338). Screen = 320x240 -> centre (160,108).
 */
#include "re15_item_modal.h"
#include "re15_math.h"       /* re15_rcos / re15_rsin (byte-true BIOS trig, Q12) */
#include "re15_inventory.h"  /* re15_inv_grant + g_inv (the deferred grant = FUN_8004dc4c INSERT)  */
#include "re15_aot.h"        /* g_aot — deactivate the item AOT on confirm                          */
#include "re15_scd.h"        /* re15_game_flag_set — the taken-bit (zone 9)                         */

/* ---- byte-true corner tables (FUN_8001e1c8 @0x80072d3c / @0x80072d44) ---- */
static const int16_t s_corner_x[4] = { -56, +56, -56, +56 };
static const int16_t s_corner_y[4] = { -36, -36, +36, +36 };

/* ---- FSM state (mirrors the PSX globals) ---- */
static uint8_t s_state    = 0;      /* DAT_80072d3b (0=idle, 1..8)          */
static int32_t s_f630     = 0;      /* DAT_8008f630 (frame counter)         */
static int32_t s_f634     = 0;      /* DAT_8008f634 (animation angle, Q12)  */
static uint8_t s_type     = 0;      /* DAT_800afbb6 (grant id)              */
static uint8_t s_amount   = 0;
static uint8_t s_taken    = 0;      /* zone-9 taken-bit payload             */
static int     s_aot_slot = -1;     /* g_aot slot to deactivate on confirm  */
static int     s_grant    = -1;     /* DAT_8008f62c (free slot, -1 = full)  */

/* state-5/6 MESSAGE BOX (byte-true FUN_80027e68 a1=0x100 + FUN_80028134 VM): the pickup is NOT silent —
 * it opens a text prompt. s_prompt: 0=none, 1="WILL YOU TAKE THE <item>." Yes/No, 2="YOU CAN'T CARRY ANY
 * MORE ITEMS" (inventory full). s_choice: 0=Yes, 1=No (mirrors the DAT_800b8520 bit-0 toggle). s_msg_no
 * carries the "No" result into case 7 (@0x8001e068 andi 0x1 -> state 8). */
static int     s_prompt   = 0;
static int     s_choice   = 0;
static int     s_msg_no   = 0;

/* current-frame quad (screen space), recomputed each tick */
static int  s_qx[4], s_qy[4];
static int  s_face      = 0;        /* 0=front, 1=back (coin-flip reverse)  */
static int  s_visible   = 0;        /* draw the quad this frame?            */

/* First free inventory slot, or -1 (byte-true pre-check FUN_8004df2c @0x8001df14; a slot exists iff
 * some slot id == 0 — the same scan re15_inv_grant does before the INSERT). */
static int inv_free_slot(void)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) return i;
    return -1;
}

/* FUN_8001e1c8 mode 0 (state 2 zoom): sx = cornerX*17/(f630+1), sy = cornerY*17/(f630+1), then rotate
 * (sx,sy) by `angle` (FUN_8004f008 = a Y-axis RotMatrix applied to the 3D point {sx,_,sy}, which is
 * algebraically a planar 2D rotation of (sx,sy)), then +160 / +108. MIPS `div` truncates toward zero,
 * which C integer division matches. */
static void quad_scale_rotate(int num, int den, int angle)
{
    int32_t c = re15_rcos(angle & 0xfff);   /* Q12 */
    int32_t s = re15_rsin(angle & 0xfff);
    for (int i = 0; i < 4; i++) {
        int32_t sx = (int32_t)s_corner_x[i] * num / den;
        int32_t sy = (int32_t)s_corner_y[i] * num / den;
        /* Ry(angle) on {vx=sx, vz=sy}: vx' = c*sx + s*sy ; vz' = -s*sx + c*sy  (>>12) */
        int32_t rx = (c * sx + s * sy) >> 12;
        int32_t rz = (-s * sx + c * sy) >> 12;
        s_qx[i] = (int)(rx + 160);
        s_qy[i] = (int)(rz + 108);
    }
}

/* FUN_8001e1c8 mode 1 (state 8 shrink): sx = cornerX*(16-f630)/16, sy = cornerY*(16-f630)/16, no
 * rotation, +160 / +108. */
static void quad_shrink(int n /* = 16 - f630 */)
{
    for (int i = 0; i < 4; i++) {
        int32_t sx = (int32_t)s_corner_x[i] * n / 16;
        int32_t sy = (int32_t)s_corner_y[i] * n / 16;
        s_qx[i] = (int)(sx + 160);
        s_qy[i] = (int)(sy + 108);
    }
}

/* state 4 FLIP: X = 160±56 (full width), Y = 108±delta with
 *   delta = (rcos(f634) * 36) >> 12   (signed round-toward-zero: @0x8001ddd4 addiu 4095 before sra 12).
 * verts 0/1 (top) = 108+delta, verts 2/3 (bottom) = 108-delta; f634 spins 0 -> 0x800 over 9 frames so
 * frame0 delta=+36 (flipped), frame4 delta=0 (edge-on line), frame8 delta=-36 (upright) = a coin flip. */
static void quad_flip(void)
{
    int32_t prod  = re15_rcos(s_f634 & 0xfff) * 36;                    /* @0x8001ddc0..c8 (*9 <<2) */
    int32_t delta = (prod < 0) ? ((prod + 4095) >> 12) : (prod >> 12); /* @0x8001ddd4 / @0x8001ddd8  */
    for (int i = 0; i < 4; i++) s_qx[i] = 160 + s_corner_x[i];
    s_qy[0] = s_qy[1] = 108 + (int)delta;
    s_qy[2] = s_qy[3] = 108 - (int)delta;
    s_face = (s_f630 < 4) ? 1 : 0;   /* back-face until the mid-flip texture-row swap @0x8001de24 */
}

void re15_item_modal_start(uint8_t item_type, uint8_t amount, uint8_t taken_bit, int aot_slot)
{
    if (s_state != 0) return;        /* byte-true guard @0x80043334 (don't restart while running) */
    s_state    = 1;                  /* sb 1,DAT_80072d3b @0x8004334c */
    s_type     = item_type;          /* DAT_800afbb6 @0x8004335c      */
    s_amount   = amount;
    s_taken    = taken_bit;
    s_aot_slot = aot_slot;
    s_grant    = -1;
    s_f630     = 0;
    s_f634     = 0;
    s_visible  = 0;
    s_face     = 0;
    s_prompt   = 0;
    s_choice   = 0;
    s_msg_no   = 0;
}

int re15_item_modal_active(void) { return s_state != 0; }
uint8_t re15_item_modal_state(void) { return s_state; }
int re15_item_modal_frame(void) { return (int)s_f630; }

void re15_item_modal_tick(uint16_t pad_edge)
{
    int again = 1;
    while (again) {
        again = 0;
        switch (s_state) {
        case 0:
            s_visible = 0;
            return;

        case 1:  /* INIT (@0x8001db74): freeze is the caller's early-return; seed counters. */
            s_f630    = 0;
            s_f634    = 0x800;       /* sh 0x800,DAT_8008f634 @0x8001dcd8 */
            s_visible = 0;           /* nothing drawn on the init frame   */
            s_state   = 2;
            return;

        case 2:  /* ZOOM-IN, 17 frames (@0x8001dce4): scale 17/(f630+1) + spin f634. */
            quad_scale_rotate(17, (int)s_f630 + 1, (int)s_f634);
            s_face    = 0;
            s_visible = 1;
            s_f630++;                /* @0x8001dd10 */
            s_f634 += 0x80;          /* @0x8001dd14 */
            if (s_f630 >= 0x11) {    /* sltiu v0,v0,0x11 @0x8001dd24 */
                s_f630  = 0;
                s_f634  = 0;         /* reset before the flip */
                s_state = 3;
            }
            return;

        case 3:  /* LOAD GATE (@0x8001dd58): PSX waits on the VRAM DMA (DAT_800b29a4==0). The port has
                  * no async load — advance next tick; hold the full box (last state-2 quad) meanwhile. */
            s_visible = 1;
            s_state   = 4;
            return;

        case 4:  /* FLIP, 9 frames (@0x8001ddb0). At f630==9 FALLS THROUGH to state 5 (same tick). */
            quad_flip();
            s_visible = 1;
            s_f630++;                /* @0x8001def8 */
            s_f634 += 0x100;         /* @0x8001ddf8 */
            if (s_f630 >= 0x9) {     /* sltiu v0,v0,0x9 @0x8001df08 -> fall through @0x8001df14 */
                s_state = 5;
                again   = 1;         /* run state 5 this same tick */
            }
            break;

        case 5:  /* GRANT-CHECK + OPEN MESSAGE BOX (@0x8001df14): pre-check the free slot (FUN_8004df2c),
                  * then FUN_80027e68(a1=0x100) opens the item prompt — script[0] "WILL YOU TAKE THE
                  * <item>." Yes/No if there's room, script[1] "YOU CAN'T CARRY ANY MORE ITEMS" if full
                  * (a2 = weapon/slot branch @0x8001df40-df48). NOT a fade, NOT audio. state=6. */
            s_grant   = inv_free_slot();
            s_prompt  = (s_grant < 0) ? 2 : 1;   /* 2 = can't-carry, 1 = Yes/No take-prompt */
            s_choice  = 0;                        /* default Yes (DAT_800b8520 bit0 = 0) */
            s_msg_no  = 0;
            s_state   = 6;
            return;

        case 6:  /* MESSAGE-BOX WAIT (@0x8001dffc gate: while DAT_800b8520 & 0x80): the item picture keeps
                  * drawing; dismiss on PLAYER INPUT (not a timer). The message VM (FUN_80028134 sub-4/5)
                  * reads DAT_800ac76c: & 0x3000 toggles Yes/No (DAT_800b8520 ^= 1), & 0x4000 confirms;
                  * the full-prompt just waits for & 0xc000. */
            s_visible = 1;
            if (s_prompt == 2) {                                  /* can't-carry: any confirm dismisses */
                if (!(pad_edge & 0xc000)) return;                 /* DAT_800ac76c & 0xc000 (CROSS/SQUARE) */
                s_prompt = 0; s_state = 7; again = 1; break;
            }
            if (pad_edge & 0x3000) s_choice ^= 1;                 /* TRIANGLE/CIRCLE toggle -> No-flag ^=1 */
            if (!(pad_edge & 0x4000)) return;                     /* wait for CROSS confirm */
            s_msg_no = s_choice;                                  /* No selected -> case7 leaves the item */
            s_prompt = 0; s_state = 7; again = 1; break;

        case 7:  /* INSERT / DONE (@0x8001e048): full inventory OR "No" -> state 8 (no grant, item stays);
                  * else INSERT (FUN_8004dc4c) + taken-bit + deactivate the AOT, state=0 (DONE). The
                  * byte-true gate is `if ((char)DAT_8008f62c < 0 || (DAT_800b8520 & 1)) -> state 8`
                  * (@0x8001e054 bltz + @0x8001e068 andi 0x1) — the second term is the Yes/No "No". */
            if (s_grant < 0 || s_msg_no || re15_inv_grant(s_type, s_amount) != 0) {
                s_f630  = 0;         /* @0x8001e0f0 */
                s_f634  = 0;         /* @0x8001e0f8 */
                s_state = 8;         /* sb 8,DAT_80072d3b @0x8001e100 (state 8 runs next tick) */
                return;
            }
            if (s_taken) re15_game_flag_set(9, s_taken, 1);   /* FUN_8004ef90 (zone-9 taken flag) */
            if (s_aot_slot >= 0 && s_aot_slot < RE15_AOT_MAX)
                g_aot.slots[s_aot_slot].active = 0;            /* the item leaves the world */
            s_visible = 0;
            s_state   = 0;           /* sb zero,DAT_80072d3b @0x8001e0e0 = DONE */
            return;

        case 8:  /* SHRINK-away, 17 frames (@0x8001e10c): full-inventory path only, no grant. */
            quad_shrink(16 - (int)s_f630);
            s_face    = 0;
            s_visible = 1;
            s_f630++;                /* @0x8001e140 */
            s_f634 += 0x80;          /* @0x8001e144 */
            if (s_f630 >= 0x11) {    /* sltiu v0,v0,0x11 @0x8001e154 */
                s_visible = 0;
                s_state   = 0;       /* sb zero @0x8001e16c (DONE; item stays in world) */
            }
            return;

        default:
            s_state = 0;
            s_visible = 0;
            return;
        }
    }
}

int re15_item_modal_quad(int out_x[4], int out_y[4], uint8_t *out_type, int *out_face)
{
    if (!s_visible) return 0;
    for (int i = 0; i < 4; i++) { out_x[i] = s_qx[i]; out_y[i] = s_qy[i]; }
    if (out_type) *out_type = s_type;
    if (out_face) *out_face = s_face;
    return 1;
}

int re15_item_modal_prompt(uint8_t *out_type, int *out_choice)
{
    if (s_prompt == 0) return 0;
    if (out_type)   *out_type   = s_type;
    if (out_choice) *out_choice = s_choice;
    return s_prompt;   /* 1 = "WILL YOU TAKE THE X." Yes/No, 2 = "YOU CAN'T CARRY ANY MORE ITEMS" */
}
