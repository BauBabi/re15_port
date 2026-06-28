/*
 * RE1.5 Rebuilt — NCCT lighting (Phase 4.5.15-BF, 2026-05-28).
 *
 * Per-camera-cut lighting + per-vertex N·L shading.
 *
 * Disk layout (40 B per cut, matches FUN_80053fc0.c offset reads):
 *   +0x00  uint8  global_scale  (light direction multiplier)
 *   +0x01  uint8  type_flags[3] (one byte per light slot — 0=positional,
 *                                non-zero=directional; the Java parser's
 *                                lightTypes[2] int16 misreads the 4 bytes)
 *   +0x04  uint8  colors[3][3]  3 RGB triples
 *   +0x0D  uint8  ambient[3]    SetBackColor input
 *   +0x10  int16  positions[3][3] light positions (directional: used as raw
 *                                dir; positional: world XYZ source)
 *   +0x22  uint16 brightness[3] per-light brightness (range, attenuation)
 *
 * PSX pipeline (RE_15_Quellcode_V2/render_calc_dynamic_lights.c):
 *   For each light i in 0..2:
 *     if type_flags[i] == 0 (POSITIONAL):
 *       d   = (actor_pos - light_pos[i]) >> 4
 *       dist = sqrt(d.x² + d.z²)           // XZ-plane only
 *       range = brightness[i] >> 4
 *       atten = max(0, range - dist) / range
 *       color = light_color[i] × atten
 *       dir   = d                          // (actor → light) vector
 *     else (DIRECTIONAL):
 *       dir   = light_pos[i]               // raw, used directly
 *       color = light_color[i]
 *     n = VectorNormal(dir)                // Q12 unit
 *     L_matrix[i] = -global_scale × n      // GTE light direction row
 *     C_matrix[i] = color << 4             // GTE light color row (Q4)
 *   SetBackColor(ambient.r, ambient.g, ambient.b)
 *
 * Per-vertex GTE compute (gte_ncct):
 *   For each RGB channel c:
 *     lit_c = (ambient[c] << 4)
 *     for i in 0..2:
 *       d = (L_matrix[i] · N) >> 12        // Q12 dot, N is Q12 unit normal
 *       if d < 0: d = 0
 *       lit_c += (C_matrix[i,c] × d) >> 12
 *     out_c = clamp(lit_c >> 4, 0, 255)
 *
 * Our PC port mirrors this in software; SDL_RenderGeometry consumes the
 * per-vertex bytes directly as SDL_Color modulation onto the texel.
 */

#ifndef RE15_LIGHT_H
#define RE15_LIGHT_H

#include <stdint.h>
#include <stddef.h>

#define RE15_LIGHT_MAX_CUTS 16

typedef struct {
    uint8_t  global_scale;       /* byte +0x00 */
    uint8_t  type_flags[3];      /* byte +0x01..+0x03 */
    uint8_t  colors[3][3];       /* +0x04..+0x0C */
    uint8_t  ambient[3];         /* +0x0D..+0x0F */
    int16_t  positions[3][3];    /* +0x10..+0x21 */
    uint16_t brightness[3];      /* +0x22..+0x27 */
} re15_light_cut_t;

typedef struct {
    int              cut_count;
    re15_light_cut_t cuts[RE15_LIGHT_MAX_CUTS];
} re15_light_set_t;

/* PSX face_rgb (RGBC.R/G/B) constant. Every actor init writes 0x808080 to its
 * CVECTOR face_rgb field (RE_15_Quellcode_V2/FUN_80039b2c.c:27 — also props
 * FUN_8002b898.c:16, BG models FUN_80017048.c:11 etc.). No SCD opcode and no
 * runtime path modifies it for ROOM1170 (verified by agent sweep 2026-05-29).
 * Hardcode as a constant — promote to per-actor field only if a future room
 * needs damage-flash / fade hooks. The GTE NCCT pipeline applies this as
 * (CODE × IR) << 4 >> 12 >> 4, producing the documented 50% headroom: max
 * ambient (0xFF) → screen ~0x7F, leaving room for additive light contributions
 * to reach 0xFF without pre-clipping (nocash psx-spx GTE Stage 3+4). */
#define RE15_FACE_RGB_CODE  128

/* MVP per-cut fallback tint (used when an actor has no per-vertex normals). */
extern uint8_t g_re15_light_tint[3];

/* Globally-loaded room lights + load-ok flag. */
extern re15_light_set_t g_re15_room_lights;
extern int              g_re15_room_lights_ok;

/* Active cut index — updated by re15_light_apply_cut and re15_light_setup_actor
 * caller. The NCCT context derives from `g_re15_room_lights.cuts[g_re15_active_cut]`. */
extern int              g_re15_active_cut;

/* Parse 40-byte/cut binary light.lit into out. Returns 0 on success. */
int  re15_light_parse(const uint8_t *data, size_t size, re15_light_set_t *out);

/* Update g_re15_light_tint + g_re15_active_cut for `cut_idx`. Fallback tint:
 *   tint[c] = clamp(ambient[c] + (color0+color1+color2)[c] / 6, 255). */
void re15_light_apply_cut(const re15_light_set_t *lights, int cut_idx);

/* ---------------- Per-actor NCCT lighting context ---------------- */

typedef struct {
    /* GTE-equivalent light direction matrix, post-actor-rotation-inverse.
     * Each row L[i] = -global_scale × normalize(dir_in_actor_local_space).
     * Q12 scaled (so dot with Q12 normal gives Q24 → >>12 = Q12). */
    int32_t L[3][3];
    /* Per-light color × atten, ×16 (Q4 promotion to match PSX C_matrix). */
    int32_t C[3][3];      /* [light][RGB] */
    uint8_t ambient[3];
    int     active_lights; /* number of lights with non-zero contribution */
} re15_actor_lightctx_t;

/* Build a per-actor lighting context for the active cut.
 *
 * actor_pos_world : actor's world position (used for positional falloff)
 * actor_rot_q12   : actor's Q12 world rotation matrix (3×3 row-major)
 *
 * The function computes the three light directions in actor-local space
 * (so vertex normals in MD1 model-space can be dot-producted directly)
 * and bakes per-light color × attenuation. After this, every vertex
 * shading is a fixed cost: 3 dot products + 3 multiply-adds per channel. */
void re15_light_setup_actor(const re15_light_cut_t *cut,
                            const int32_t           actor_pos_world[3],
                            const int32_t           actor_rot_q12[9],
                            re15_actor_lightctx_t  *ctx_out);

/* CANONICAL per-bone light context (software path). Given a WORLD-space context
 * `world` (built via re15_light_setup_actor with actor_rot_q12 = NULL) and a
 * bone's WORLD rotation matrix (Q12, row-major — e.g. R_y(yaw) × pose.rot), this
 * produces `out` with out->L[i] = bone_rot^T × world->L[i]. Dotting out->L with a
 * RAW bone-local normal then equals L_world · N_world — i.e. the same result as
 * the PSX GTE path SetLightMatrix(LLM_world × bone_rot) + ncct on the raw normal.
 * Mirrors FUN_8001e9ec (per-bone SetLightMatrix). C/ambient/active are copied.
 * Call ONCE per bone; the colour matrix + ambient stay bone-independent. */
void re15_light_ctx_rotate_for_bone(const re15_actor_lightctx_t *world,
                                    const int32_t                bone_rot_q12[9],
                                    re15_actor_lightctx_t       *out);

/* Per-vertex software gte_ncct.
 *
 * nx,ny,nz : MD1 vertex normal (Q12 unit, |N|≈4096)
 * out r/g/b : 0..255 clamped vertex color
 *
 * If ctx is NULL or has no active lights, falls back to g_re15_light_tint. */
void re15_light_shade_vertex(const re15_actor_lightctx_t *ctx,
                             int16_t nx, int16_t ny, int16_t nz,
                             uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* RE15_LIGHT_H */
