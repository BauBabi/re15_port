/*
 * RE1.5 Rebuilt — NCCT lighting implementation (Phase 4.5.15-BF, 2026-05-28).
 *
 * Ports PSX RE1.5's gte_ncct pipeline to software per-vertex shading.
 * See re15_light.h for the math; the comments in this file mirror the
 * PSX disasm function names in RE_15_Quellcode/V2.
 */

#include "re15_light.h"
#include <string.h>
/* RE2-CANONICAL integer sqrt. RE2's lighting (FUN_80053fc0 / FUN_800542dc) uses
 * libgte SquareRoot0 + VectorNormal — pure INTEGER, NO soft-float. The R3000 has
 * no FPU, so every `double` op is a libgcc soft-float call (a double divide ≈
 * ~1000 cycles); the old per-light double-sqrt + divides cost ~17-20ms/frame.
 * This bit-by-bit integer isqrt + all-integer light math removes that entirely.
 * (PC build is identical via a thin libm wrapper.) */
#ifdef PSN00BSDK
static uint32_t re15_isqrt(uint32_t x)
{
    uint32_t res = 0;
    uint32_t bit = 1u << 30;
    while (bit > x) bit >>= 2;
    while (bit) {
        uint32_t t = res + bit;
        if (x >= t) { x -= t; res = (res >> 1) + bit; }
        else        { res >>= 1; }
        bit >>= 2;
    }
    return res;
}
#else
#include <math.h>
static uint32_t re15_isqrt(uint32_t x) { return (uint32_t)sqrt((double)x); }
#endif

uint8_t          g_re15_light_tint[3]   = {255, 255, 255};
re15_light_set_t g_re15_room_lights;
int              g_re15_room_lights_ok  = 0;
int              g_re15_active_cut      = 0;

/* LE u16/s16 readers (PSX little-endian native). */
static inline uint16_t le_u16(const uint8_t *p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}
static inline int16_t le_s16(const uint8_t *p) {
    return (int16_t)le_u16(p);
}

int re15_light_parse(const uint8_t *data, size_t size, re15_light_set_t *out)
{
    if (!data || !out || size < 40) return -1;
    memset(out, 0, sizeof(*out));
    int n = (int)(size / 40);
    if (n > RE15_LIGHT_MAX_CUTS) n = RE15_LIGHT_MAX_CUTS;
    for (int i = 0; i < n; i++) {
        const uint8_t *p = data + i * 40;
        re15_light_cut_t *c = &out->cuts[i];
        c->global_scale  = p[0x00];
        c->type_flags[0] = p[0x01];
        c->type_flags[1] = p[0x02];
        c->type_flags[2] = p[0x03];
        memcpy(c->colors[0], p + 0x04, 3);
        memcpy(c->colors[1], p + 0x07, 3);
        memcpy(c->colors[2], p + 0x0A, 3);
        memcpy(c->ambient,   p + 0x0D, 3);
        for (int li = 0; li < 3; li++) {
            c->positions[li][0] = le_s16(p + 0x10 + li * 6 + 0);
            c->positions[li][1] = le_s16(p + 0x10 + li * 6 + 2);
            c->positions[li][2] = le_s16(p + 0x10 + li * 6 + 4);
        }
        c->brightness[0] = le_u16(p + 0x22);
        c->brightness[1] = le_u16(p + 0x24);
        c->brightness[2] = le_u16(p + 0x26);
    }
    out->cut_count = n;
    return 0;
}

static inline uint8_t clamp_u8(int v) {
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

void re15_light_apply_cut(const re15_light_set_t *lights, int cut_idx)
{
    if (!lights || cut_idx < 0 || cut_idx >= lights->cut_count) {
        g_re15_light_tint[0] = 255;
        g_re15_light_tint[1] = 255;
        g_re15_light_tint[2] = 255;
        return;
    }
    g_re15_active_cut = cut_idx;
    const re15_light_cut_t *c = &lights->cuts[cut_idx];
    /* BO-round 2026-05-29 (hack audit): the old `ambient + sum/6` was an
     * invented isotropic-average. PSX has NO such path: meshes are either
     * Mode A (full per-vertex gte_ncct, FUN_800254a0) or Mode B (flat, copies
     * face_rgb = 0x808080 = NEUTRAL, FUN_80022f0c). The no-normal fallback is
     * therefore PSX-neutral 128 per channel (psx_prim_to_sdl_vert(128)=255 =
     * texture unmodified). Currently dead code — all ROOM1170 MD1s carry
     * normals → the per-actor NCCT path runs — but canonical if ever hit. */
    g_re15_light_tint[0] = 128;
    g_re15_light_tint[1] = 128;
    g_re15_light_tint[2] = 128;
}

/* Multiply 3-vector by the TRANSPOSE of a Q12 3×3 matrix.
 * Used to rotate a world-space light direction into actor-local space:
 * if actor_rot is M (world = M · local), then local = M^T · world.
 * Q12 input matrix × integer vector → integer vector. */
static void mat3_T_apply_q12(const int32_t M[9],
                             int32_t vx, int32_t vy, int32_t vz,
                             int32_t out[3])
{
    /* M is row-major: M[r*3+c]. Transpose means use column r as row. */
    out[0] = (int32_t)(((int64_t)M[0]*vx + (int64_t)M[3]*vy + (int64_t)M[6]*vz) >> 12);
    out[1] = (int32_t)(((int64_t)M[1]*vx + (int64_t)M[4]*vy + (int64_t)M[7]*vz) >> 12);
    out[2] = (int32_t)(((int64_t)M[2]*vx + (int64_t)M[5]*vy + (int64_t)M[8]*vz) >> 12);
}

/* Normalize an int32 vector to Q12 unit (magnitude 4096). All-integer — mirrors
 * RE2's VectorNormal (FUN_800542dc), no soft-float. */
static void vec3_normalize_q12(int32_t x, int32_t y, int32_t z,
                               int32_t out[3])
{
    int32_t len = (int32_t)re15_isqrt((uint32_t)((int64_t)x*x + (int64_t)y*y + (int64_t)z*z));
    if (len < 1) {
        out[0] = out[1] = out[2] = 0;
        return;
    }
    out[0] = (int32_t)(((int64_t)x << 12) / len);
    out[1] = (int32_t)(((int64_t)y << 12) / len);
    out[2] = (int32_t)(((int64_t)z << 12) / len);
}

void re15_light_setup_actor(const re15_light_cut_t *cut,
                            const int32_t           actor_pos_world[3],
                            const int32_t           actor_rot_q12[9],
                            re15_actor_lightctx_t  *ctx_out)
{
    memset(ctx_out, 0, sizeof(*ctx_out));
    if (!cut) return;
    ctx_out->ambient[0] = cut->ambient[0];
    ctx_out->ambient[1] = cut->ambient[1];
    ctx_out->ambient[2] = cut->ambient[2];
    int active = 0;
    int scale  = (int)cut->global_scale;
    /* BO-round 2026-05-29 (hack audit): removed the `scale==0 → 1` guard. PSX
     * FUN_800542dc multiplies the L-rows by -(scale); scale=0 means the light
     * contributes nothing (ambient only) — the guard fabricated a directional
     * light the asset says is off. scale is a multiplier (L166-168), not a
     * divisor, so no div-by-zero. No ROOM1170 cut has scale=0 → intro unaffected. */
    for (int i = 0; i < 3; i++) {
        int32_t dir_world[3];
        int32_t color_R, color_G, color_B;
        if (cut->type_flags[i] == 0) {
            /* POSITIONAL: dir = (actor_pos - light_pos) >> 4, falloff via
             * XZ-plane distance, brightness >> 4 is the range. */
            int32_t dx = (actor_pos_world[0] - (int32_t)cut->positions[i][0]) >> 4;
            int32_t dy = (actor_pos_world[1] - (int32_t)cut->positions[i][1]) >> 4;
            int32_t dz = (actor_pos_world[2] - (int32_t)cut->positions[i][2]) >> 4;
            int32_t range = (int32_t)(cut->brightness[i] >> 4);
            if (range <= 0 || cut->brightness[i] == 0) {
                /* Light effectively off — skip contribution. */
                continue;
            }
            /* All-integer falloff, mirroring RE2 FUN_80053fc0:
             *   dist = SquareRoot0(dx²+dz²); atten = range - dist (clamp >0);
             *   colour_out = colour_byte * atten / range.
             * (XZ-only distance; was double — now integer, no soft-float.) */
            int32_t dist  = (int32_t)re15_isqrt((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
            int32_t atten = range - dist;
            if (atten <= 0) {
                /* Out of range — no contribution. */
                continue;
            }
            color_R = (int)cut->colors[i][0] * atten / range;
            color_G = (int)cut->colors[i][1] * atten / range;
            color_B = (int)cut->colors[i][2] * atten / range;
            dir_world[0] = dx;
            dir_world[1] = dy;
            dir_world[2] = dz;
        } else {
            /* DIRECTIONAL: position is the direction vector itself. */
            dir_world[0] = (int32_t)cut->positions[i][0];
            dir_world[1] = (int32_t)cut->positions[i][1];
            dir_world[2] = (int32_t)cut->positions[i][2];
            color_R = cut->colors[i][0];
            color_G = cut->colors[i][1];
            color_B = cut->colors[i][2];
        }
        /* Skip if direction or color are zero. */
        if (color_R == 0 && color_G == 0 && color_B == 0) continue;
        if (dir_world[0] == 0 && dir_world[1] == 0 && dir_world[2] == 0) continue;

        /* Normalize the world-space direction (Q12 unit), then negate scaled
         * (PSX FUN_800542dc:30-35 writes -scale × normal) and rotate into
         * actor-local space via M^T. */
        int32_t n_world[3];
        vec3_normalize_q12(dir_world[0], dir_world[1], dir_world[2], n_world);
        n_world[0] = -scale * n_world[0];
        n_world[1] = -scale * n_world[1];
        n_world[2] = -scale * n_world[2];
        /* World-space light dir × actor's inverse rotation = actor-local dir.
         * Skip the inverse-rotation if actor_rot_q12 == identity (e.g.
         * props with no body rotation): saves cycles, same result. */
        if (actor_rot_q12) {
            int32_t local[3];
            mat3_T_apply_q12(actor_rot_q12,
                             n_world[0], n_world[1], n_world[2], local);
            ctx_out->L[i][0] = local[0];
            ctx_out->L[i][1] = local[1];
            ctx_out->L[i][2] = local[2];
        } else {
            ctx_out->L[i][0] = n_world[0];
            ctx_out->L[i][1] = n_world[1];
            ctx_out->L[i][2] = n_world[2];
        }
        /* Color matrix row Q4 (PSX FUN_800542dc:38-40 writes color<<4). */
        ctx_out->C[i][0] = color_R << 4;
        ctx_out->C[i][1] = color_G << 4;
        ctx_out->C[i][2] = color_B << 4;
        active++;
    }
    /* CANONICAL (2026-06-02, fresh Ghidra RE): the cut ALWAYS has 3 light slots
     * and FUN_80053fc0 processes all 3 IN PLACE (an off/culled light just gets
     * colour 0, never packed out). Above we already write each surviving light to
     * its OWN slot index `i` (L[i]/C[i]) and leave culled slots memset-zero — so
     * the array is correct in-place. The BUG was reporting active_lights = `active`
     * (the count of surviving lights): the per-vertex shade loops are bound by
     * active_lights, so if an EARLY light is range-culled (ROOM1170 cut0 light0
     * routinely is), active=2 made the loop read slots [0..1] and DROP a real light
     * sitting in slot [2] — manifesting as too-dark / lit-from-wrong-direction /
     * too-bright-on-one-side, shifting as actors move in/out of a light's XZ range.
     * Fix: report 3 (process every slot; zeroed culled slots contribute nothing),
     * matching FUN_80053fc0 and the PSX-native loaders (mesh_psx.c iterates i<3). */
    (void)active;
    ctx_out->active_lights = 3;
}

void re15_light_ctx_rotate_for_bone(const re15_actor_lightctx_t *world,
                                    const int32_t                bone_rot_q12[9],
                                    re15_actor_lightctx_t       *out)
{
    *out = *world;   /* copy C[][], ambient[], active_lights */
    for (int i = 0; i < world->active_lights; i++) {
        int32_t l[3];
        /* out->L[i] = bone_rot^T × world->L[i] (one >>12). Then
         * out->L[i] · N_local = world->L[i] · (bone_rot · N_local) = L_world · N_world. */
        mat3_T_apply_q12(bone_rot_q12,
                         world->L[i][0], world->L[i][1], world->L[i][2], l);
        out->L[i][0] = l[0];
        out->L[i][1] = l[1];
        out->L[i][2] = l[2];
    }
}

/* GTE IR clamp with lm=1: clamp to 0..+0x7FFF.
 * nocash psx-spx "GTE Saturation" — FLAG bits 24/23/22 set on clip.
 * mednafen gte.cpp Lm_B: min = -32768 + (lm<<15) = 0 when lm=1. */
static inline int32_t sat_lm1(int32_t v) {
    if (v < 0)        return 0;
    if (v > 0x7FFF)   return 0x7FFF;
    return v;
}

void re15_light_shade_vertex(const re15_actor_lightctx_t *ctx,
                             int16_t nx, int16_t ny, int16_t nz,
                             uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (!ctx) {
        *r = g_re15_light_tint[0];
        *g = g_re15_light_tint[1];
        *b = g_re15_light_tint[2];
        return;
    }

    /* ===== Canonical PSX gte_ncct per-vertex pipeline =====
     * Opcode 0x0118043F: sf=1 (SAR 12), lm=1 (IR clamp 0..0x7FFF).
     * 3 stages: LLM·V → BK+LCM·IR → CODE·IR → RGB FIFO.
     * Refs: nocash psx-spx GTE §"NCCT", duckstation gte.cpp Execute_NCCT,
     * mednafen gte.cpp A_MV/Lm_B/Lm_C, nugget inline_n.h:816-820,
     * RE_15_Quellcode_V2/FUN_800254a0.c:29-59 (caller chain). */

    /* --- Stage 1: LLM × V → IR_dot[i] (Q12 per-light intensity)
     *   L[i] rows pre-built in setup_actor as -global_scale × normalize(dir_local).
     *   Result IR_dot[i] is N·L per light, clamped 0..0x7FFF by lm=1. */
    int32_t IR_dot[3] = {0, 0, 0};
    for (int i = 0; i < ctx->active_lights; i++) {
        int64_t acc = (int64_t)ctx->L[i][0] * nx
                    + (int64_t)ctx->L[i][1] * ny
                    + (int64_t)ctx->L[i][2] * nz;
        int32_t MAC = (int32_t)(acc >> 12);   /* sf=1: SAR 12 */
        IR_dot[i] = sat_lm1(MAC);
    }

    /* --- Stage 2: (BK << 12) + LCM × IR_dot → IR_c (per-channel lit, Q1.3.12)
     *   PSX gte_SetBackColor stores BK_reg = byte_ambient << 4 (Q4). Stage 2
     *   then accumulates at <<12 alignment, so byte_ambient enters as <<16.
     *   LCM[c][i] = color_byte[i][c] << 4 (Q4), stored in ctx->C[i][c].
     *   For each channel:
     *     MAC2 = (byte_ambient << 16 + Σ LCM × IR_dot) SAR 12
     *     IR_c = clamp(MAC2, 0, 0x7FFF)   // lm=1 */
    int32_t IR_c[3];
    for (int ch = 0; ch < 3; ch++) {
        int64_t acc = (int64_t)ctx->ambient[ch] << 16;
        for (int i = 0; i < ctx->active_lights; i++) {
            acc += (int64_t)ctx->C[i][ch] * (int64_t)IR_dot[i];
        }
        int32_t MAC2 = (int32_t)(acc >> 12);
        IR_c[ch] = sat_lm1(MAC2);
    }

    /* --- Stage 3+4: (CODE × IR_c) SHL 4 → MAC3, push RGB FIFO = sat_u8(MAC3 SAR 4)
     *   CODE = 0x80 per face_rgb 0x808080 (RE15_FACE_RGB_CODE).
     *   Net: byte_out = sat_u8((CODE × IR_c) >> 12)
     *        With CODE=128: byte_out = sat_u8(IR_c >> 5)
     *   This is the canonical NCCT post-fold — produces 50% headroom such
     *   that ambient byte 0xFF alone yields screen ~0x7F, leaving the upper
     *   half of the dynamic range for additive light contributions. */
    const int32_t CODE = RE15_FACE_RGB_CODE;
    int32_t mac3_R = (int32_t)(((int64_t)CODE * (int64_t)IR_c[0]) << 4 >> 12);
    int32_t mac3_G = (int32_t)(((int64_t)CODE * (int64_t)IR_c[1]) << 4 >> 12);
    int32_t mac3_B = (int32_t)(((int64_t)CODE * (int64_t)IR_c[2]) << 4 >> 12);
    *r = clamp_u8(mac3_R >> 4);
    *g = clamp_u8(mac3_G >> 4);
    *b = clamp_u8(mac3_B >> 4);
}
