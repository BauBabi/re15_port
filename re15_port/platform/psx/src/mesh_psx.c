/*
 * RE1.5 Rebuilt — PSX 3D mesh renderer via GTE (Phase 4.5.5-fix, 2026-05-18).
 *
 * REWRITE after 30-agent forensic analysis of the rotation-flicker bug.
 * The earlier revision invented its own pipeline (software cross-product
 * cull, ±2048 absolute coord clamp, static z_bucket). All three departed
 * from the canonical PSn00bSDK / RE2 pattern AND failed to address the
 * actual hardware constraint that was dropping triangles.
 *
 * This file now follows the textbook PSX 3D-mesh pipeline used by:
 *   - PSn00bSDK's `examples/graphics/gte/main.c` (rotating textured cube)
 *   - PSn00bSDK's `examples/demos/n00bdemo/lightdemo.c`
 *   - RE2's `FUN_8002d0e8` / `FUN_80027bec` (decompiled production mesh
 *     renderers) — see RE2_Quellcode in this repo.
 *
 * Per-triangle pipeline (the canonical PSX rotation pipeline):
 *
 *   1. gte_ldv3(v0, v1, v2)   — load 3 model-space vertices into GTE
 *   2. gte_rtpt()             — rotate + translate + perspective-project
 *   3. gte_nclip()            — 2D screen-space cross product into MAC0
 *      gte_stopz(&p)          — read MAC0 sign
 *      if (p < 0) continue;   — backface (cross < 0 = facing away)
 *   4. gte_avsz3()            — average screen-Z of the 3 verts
 *      gte_stotz(&otz)        — read averaged Z
 *      if (otz < OTZ_NEAR_PLANE) continue;  — RE2 uses 8; we use the same
 *      if (otz > OTZ_FAR_CLAMP)  otz = OTZ_FAR_CLAMP;  — RE2 clamps to 0x1FFF
 *   5. bucket = OTZ_TO_BUCKET(otz, z_bucket)  — per-poly OT bucket placement
 *      (a static z_bucket lands every triangle in the same bucket → undefined
 *      LIFO draw order; per-poly OTZ restores painter's algorithm)
 *   6. gte_stsxy3(&sxy0, &sxy1, &sxy2)  — read projected screen XY
 *   7. PSX 11-bit edge-delta gate.  The GP0 command accepts vertex coords
 *      packed into 11 bits (s11, ±1024). Per psx-spx hardware reference
 *      (psx-spx graphicsprocessingunitgpu.md L332-336): "the maximum
 *      distance between two vertices is 1023 horizontally, 511 vertically.
 *      Polygons that exceed these dimensions are NOT rendered". That is
 *      the actual cause of "triangle disappears during rotation" — when
 *      a vertex saturates against the GTE limiter near the camera, the
 *      delta to a normally-projected sibling vertex exceeds 1023, and
 *      the WHOLE primitive is silently dropped by hardware. The previous
 *      ±2048 absolute clamp was wrong on TWO axes: (a) clamps absolute
 *      coords not deltas, (b) wrong threshold (2048 > the 1023 hardware
 *      limit). The correct check is per-edge |Δx| ≤ 1023 / |Δy| ≤ 511.
 *   8. Emit the primitive (POLY_FT3 textured, LINE_F2 wireframe).
 *
 * Quad path uses gte_rtpt for v0/v1/v2 + gte_rtps for v3, gte_avsz4 for
 * the 4-vertex Z average. nclip on the rtpt-projected v0/v1/v2 sign-tests
 * the quad's facing (valid for planar quads — the format guarantees
 * planarity).
 *
 * Why earlier revisions chose differently:
 *   - Software cross product was an over-correction for a perceived
 *     gte_nclip MAC0 overflow. lightdemo + RE2 both prove nclip works
 *     reliably for thousands of triangles per frame; the prior flicker
 *     was caused by the UV stride bug + buffer overflow + the missing
 *     edge-delta gate, not by nclip.
 *   - Static z_bucket was carry-over from the wireframe Phase 4.5.4
 *     debug renderer that intentionally bucketed every prim together.
 *   - ±2048 clamp was a speculative fix that didn't reference the
 *     11-bit GP0 spec.
 */

#include <stdint.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>           /* gte_ldv3 / gte_rtpt / gte_nclip / ... */
#include "re15_engine.h"
#include "re15_md1.h"
#include "re15_emd.h"           /* skeleton + keyframe accessors  */
#include "re15_skeleton.h"      /* per-bone pose computation       */
#include "re15_camera.h"        /* view-matrix + view × bone compose */
#include "re15_scd.h"           /* g_game.player_face_{sin,cos}_q12 */
#include "re15_light.h"         /* #1c: per-actor NCCT shading */

/* #1c port (2026-06-01): per-actor light context. Set up once per actor in
 * mesh_psx_render_skeletal (eval_pos = actor's own world pos, canonical
 * BH-round), consumed per-vertex in render_one_mesh_via_gte. When inactive
 * (no light set, or the non-skeletal demo path) the mesh draws flat-neutral. */
static re15_actor_lightctx_t s_lightctx;
static int                   s_lightctx_active = 0;

/* GTE HARDWARE NCCT (2026-06-01): the canonical PSX lighting path (FUN_800254a0
 * gte_ncct), NOT the software per-vertex port (that ran int64 muls/vertex →
 * ~6 fps + over-saturated). The hardware op runs in the GTE for ~free. When
 * s_lightctx_active, render_one_mesh_via_gte emits POLY_GT3/GT4 fed by gte_ncct
 * (per-vertex Gouraud colour from the loaded light/colour/back matrices);
 * otherwise flat-neutral POLY_FT3/FT4 (unmodified texels). */

/* The NCCT CODE colour = face_rgb 0x808080 (FUN_80039b2c.c:27); CODE=128 gives
 * the documented 50% headroom. Loaded via gte_ldrgb before the per-mesh ncct.
 *
 * CRITICAL: the 4th byte (cd) MUST be the GPU primitive command code, NOT 0.
 * gte_ncct passes the loaded cd byte UNCHANGED through the RGB FIFO, and
 * gte_strgb3_gt3/gt4 writes the full RGBC word (incl. cd) into the prim — so
 * the cd lands in the prim's command byte. The canonical does exactly this
 * (FUN_800254a0:27 `param_3->cd = param_4<<1 | 0x34`; FUN_800256b0 uses 0x3c).
 * With cd=0 the strgb zeroed setPolyGT3's 0x34 command byte → the GPU mis-read
 * the primitive → garish full-screen/garbage fills. 0x34 = POLY_GT3 (gouraud
 * textured tri), 0x3c = POLY_GT4 (gouraud textured quad). */
static const CVECTOR s_ncct_code_tri  = { 128, 128, 128, 0x34 };
static const CVECTOR s_ncct_code_quad = { 128, 128, 128, 0x3c };


/* GTE hardware NCCT: ON + working. Per-actor SetColorMatrix/SetBackColor +
 * per-bone SetLightMatrix (world LLM x bone rot) + per-poly gte_ncct -> GT3/GT4.
 * The earlier garish/garbage was NOT a scaling defect: a software sim of the
 * exact chain on the real cut-2 light.lit + unit normals gives byte ~50 (dark,
 * correct for the night helipad). The real bug was the gte_ldrgb CODE byte = 0
 * zeroing the prim command byte (gte_strgb writes the whole RGBC word incl. cd)
 * -> garbage GPU command. Fixed via s_ncct_code_tri/quad. Set 0 to disable. */
static const int s_ncct_enabled = 1;   /* ON + working (CODE-byte fix, 2026-06-01) */

/* CRITICAL (2026-06-02): the PSn00bSDK gte_Set*Matrix macros (inline_c.h) read
 * the matrix from memory via `lw 0(%0)` etc. but their asm clobber list omits
 * "memory" — so the compiler is NOT told the asm reads *r0. For a matrix that is
 * computed locally and consumed ONLY by the macro (lm here, cm in the colour
 * loader), the compiler may legally drop/reorder the stores → the GTE loads a
 * STALE/garbage matrix → wildly wrong (over-bright) lighting. (Geometry's
 * SetRotMatrix is immune: its matrix is written by a real function call —
 * compose_view_bone — which forces the stores to memory.) The earlier
 * disappearing-then-reappearing over-exposure was exactly this: a debug loop that
 * happened to READ lm masked the bug. Fix: a compiler memory barrier right before
 * the macro forces all pending matrix stores to land. */
#define RE15_GTE_MATRIX_BARRIER() __asm__ volatile ("" ::: "memory")

/* Load the per-actor (bone-INDEPENDENT) GTE Colour Matrix (LCM) + Back Colour
 * (ambient) once. Mirrors render_model_with_skeleton.c:46 SetColorMatrix(LCM) +
 * FUN_80053fc0's SetBackColor(ambient). LCM is [channel][light] = the light
 * colours, so transpose ctx->C ([light][channel], already <<4 in setup_actor).
 * BK takes raw ambient bytes (gte_SetBackColor shifts <<4 internally). */
static void mesh_psx_load_light_color_matrices(const re15_actor_lightctx_t *ctx)
{
    MATRIX cm;
    for (int ch = 0; ch < 3; ch++)
        for (int li = 0; li < 3; li++)
            cm.m[ch][li] = (short)ctx->C[li][ch];
    RE15_GTE_MATRIX_BARRIER();   /* flush cm before the macro reads it (see note) */
    gte_SetColorMatrix(&cm);
    gte_SetBackColor((int)ctx->ambient[0], (int)ctx->ambient[1], (int)ctx->ambient[2]);
}

/* GTE Light Matrix = LLM_world × bone_world_rot, loaded PER BONE — the CANONICAL
 * RE1.5 path (60-agent RE, 2026-06-02; FUN_8001e8c8 builds LLM/LCM once per actor,
 * then per bone FUN_8001e9ec.c:30-31 does MulMatrix0(&DAT_80076d14[=LLM_world], m1
 * [=bone world rot], tmp) + SetLightMatrix(tmp)). ctx->L holds the WORLD-space
 * light rows (-global_scale × normalize(dir); setup_actor MUST be called with
 * rot=NULL). gte_ncct multiplies the RAW loaded bone-local normal by the LIGHT
 * matrix ONLY (cr8-12), never the rotation register (cr0-4) — so the bone's pose
 * MUST be carried by the light matrix, else posed limbs are lit as if un-posed.
 * Folding it here gives IR = LLM_world·(bone_rot·N_local) = LightDir_world·N_world.
 *
 * For the identity/body bone (bone_rot ≈ R_y(yaw)) this is bit-identical to the old
 * body-local "load once" matrix, so torso/head brightness is UNCHANGED (matches
 * ablauf3). The earlier ~1.4-2× over-bright was NOT a scale error: it was the bone
 * rotation applied TWICE (rot=NULL world L kept AND a separate per-bone MulMatrix on
 * top of an already-body-folded L). Applying it EXACTLY ONCE, here, is correct.
 *
 * Q12: one >>12 after the multiply keeps the result in the same scale as the world
 * rows (= libgte MulMatrix0). int32 (not int64): worst case scale·4096 × 4096 × 3 ≈
 * 100M ⊂ int32 (per the perf note in the bone loop). */
static void mesh_psx_load_bone_light_matrix(const re15_actor_lightctx_t *ctx,
                                            const int32_t bone_rot_q12[9])
{
    MATRIX lm;
    for (int i = 0; i < 3; i++) {            /* light row (cr8/10/12 rows) */
        for (int c = 0; c < 3; c++) {        /* world component */
            int32_t s = 0;
            for (int k = 0; k < 3; k++)
                s += ctx->L[i][k] * bone_rot_q12[k*3 + c];
            s >>= 12;
            if (s >  32767) s =  32767;
            if (s < -32768) s = -32768;
            lm.m[i][c] = (short)s;
        }
    }
    RE15_GTE_MATRIX_BARRIER();   /* flush lm before the macro reads it (see note) */
    gte_SetLightMatrix(&lm);
}

/* #2: per-render texture override. >=0 = use this tpage/clut for ALL polys
 * (props with their own relocated TIM); -1 = use the MD1's baked page via
 * remap_md1_tpage + baked clut (Leon, who spans 3 tpages so per-tri matters). */
static int s_tex_tpage_ovr = -1;
static int s_tex_clut_ovr  = -1;
/* #7 (RE'd from elliot.md1 bytes): a relocated character whose texture spans
 * MULTIPLE tpages (Leon=3, Elliot=2: body meshes baked page 0x0080 X=0, face/
 * arm meshes 0x0081 X=1) must keep the per-tri baked tpage and just shift its
 * X base — NOT collapse to one tpage. xshift = VRAM_x/64. -1 = use Leon's
 * global remap_md1_tpage (the default skeletal path). */
static int s_tex_tpage_xshift = -1;
/* #7 (audit): for a RELOCATED multi-CLUT-row texture (Elliot: row0 body / row1
 * face), we keep the MD1 per-tri uv->clut row distinction but shift the whole
 * block by the relocation Y-delta. clut word = (y<<6)|(x>>4), so add (yshift<<6).
 * Used only when s_tex_clut_ovr < 0 (single-row props/heli still use override). */
static int s_tex_clut_yshift = 0;

extern re15_md1_t           re15_test_md1;
extern int                  re15_test_md1_ok;
extern re15_emd_animation_t re15_test_anim;
extern re15_emd_skeleton_t  re15_test_skel;
extern int                  re15_test_skel_ok;

/* Forward declaration — defined later in this file. */
static void render_one_mesh_via_gte(const re15_md1_mesh_t *mesh, int mesh_idx,
                                    int z_bucket, int backface_cull, int mode);

/* Phase 4.5.7.5: PL00.tim spans 3 consecutive tpages horizontally (8bpp,
 * 384 pixels = 3 × 128px pages). MD1 stores per-triangle `page` baked
 * for RE1.5's original VRAM origin (0, 0), where the 3 pages have
 * tpage-x bits 0, 1, 2. asset_psx relocates pixel data away from the
 * framebuffer, so every per-tri tpage-x needs to be shifted by our
 * placement offset.
 *
 * Derivation: re15_test_tim_tpage low 4 bits = (our_vram_x / 64) (set
 * via getTPage at upload time). Add to the orig per-tri x bits, keep
 * bits 5..15 (abr, bpp, y) from the original triangle (its bpp/abr
 * matches our placement since both target 8bpp + non-transparent). */
extern int re15_test_tim_tpage;
static inline int remap_md1_tpage(uint16_t orig_page)
{
    int our_x_shift = re15_test_tim_tpage & 0x000F;
    int new_x       = ((orig_page & 0x000F) + our_x_shift) & 0x000F;
    return (int)((orig_page & 0xFFF0) | (uint16_t)new_x);
}

/* TIM upload handles set by asset_psx.c during re15_assets_init(). We use
 * these as the texture source for MESH_PSX_MODE_TEXTURED, ignoring the
 * MD1's baked-in tpage/clut (which point at original RE1.5 VRAM coords
 * that would conflict with our framebuffer layout). */
extern int re15_test_tim_tpage;
extern int re15_test_tim_clut;

/* Line + textured-triangle emitters defined in render.c — we forward
 * into them instead of touching the OT directly so the rendering
 * abstraction stays single-layer. */
extern void re15_render_line(int x0, int y0, int x1, int y1, int z,
                             uint8_t r, uint8_t g, uint8_t b);
extern void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                                     int x1, int y1, int u1, int v1,
                                     int x2, int y2, int u2, int v2,
                                     int tpage, int clut, int z,
                                     uint8_t r, uint8_t g, uint8_t b);

/* OT_LENGTH must match render.c. We replicate the constant here so the
 * mesh's per-poly bucket clamp can keep mesh prims out of bucket
 * OT_LENGTH-1 (reserved for the background gradient). Phase 4.7+ TODO:
 * promote OT_LENGTH into re15_engine.h.
 *
 * Phase 4.5.7.6 (2026-05-19): bumped 16→1024 to match RE2 retail
 * (RE2_Quellcode/main.c:23 ClearOTagR(..., 0x400)). The 16-bucket OT
 * was too coarse for skeletal characters — 15 bones with similar OTZ
 * landed in the same bucket and the painter-FIFO order produced visible
 * arm-through-torso overlaps on both PSX and PC. */
#define OT_LENGTH                1024
#define OT_BACKGROUND_BUCKET     (OT_LENGTH - 1)   /* deepest, drawn first */

/* Per-poly OTZ gating, mirrors RE2's FUN_80027bec.c:
 *   - OTZ < 8: vertex too close to camera → drop the triangle. The GTE's
 *     perspective-divide saturated, so projected coords are garbage.
 *   - OTZ > 0x1FFF: vertex far away → clamp to keep the bucket index
 *     in a sane range. We don't drop these (they're behind-camera but
 *     still emitted to far OT bucket; the GPU then per-pixel clips). */
#define OTZ_NEAR_PLANE_DROP   8
#define OTZ_FAR_CLAMP         0x1FFF

/* GTE FLAG (cop2 control reg 31) — error/saturation bitmask.
 * Bit 31 is the master error bit (OR of bits 30..23 and 18..13). After
 * gte_rtpt this is set when ANY vertex saturated during perspective
 * division — specifically:
 *   bit 13 = SY2 saturated to ±1024
 *   bit 14 = SX2 saturated to ±1024
 *   bit 17 = divide overflow (SZ3 < H/2 — vertex closer than near plane)
 *   bit 18 = SZ3 underflow (vertex behind camera)
 * Any of these means the projected coords are not reliable — the GPU
 * would either silently discard the primitive or draw it at the wrong
 * position. Dropping it pre-emit gives a clean fail-closed.
 *
 * The PSn00bSDK header (inline_c.h L1144) explicitly recommends this:
 *   "Divide overflows are simply saturated allowing for crude Z clipping.
 *    Check C2_FLAG to determine which overflow error has occurred".
 *
 * Phase 4.5.10-E: was 0x80000000 (bit 31 master OR) — too aggressive: also
 * fired on benign MAC saturation during rtpt with large camera-space coords
 * (>~5k), killing every tri → Leon invisible. Narrowed to 0x00060000 =
 * bits 17 + 18 only (SZ3 underflow + divide overflow), the actual
 * "Z-projection failed" signals per RE2 + PSn00bSDK example code. */
#define GTE_FLAG_ERROR_MASK   0x00060000UL

/* PSX GP0 vertex-coord deltas: hardware limit per psx-spx reference. */
#define EDGE_DX_MAX   1023
#define EDGE_DY_MAX   511

static inline int abs_int(int v) { return v < 0 ? -v : v; }

/* Check the 3 edges of a triangle against the GP0 11-bit delta limit.
 * Returns 1 if the triangle is renderable, 0 if it should be culled.
 * Reuses the same math the PSX GPU command processor uses to decide
 * whether to draw or silently drop a primitive — mirroring lets us
 * report the cull explicitly instead of seeing flicker. */
static int tri_within_psx_delta_limit(int sx0, int sy0,
                                      int sx1, int sy1,
                                      int sx2, int sy2)
{
    int dx01 = abs_int(sx1 - sx0), dy01 = abs_int(sy1 - sy0);
    int dx12 = abs_int(sx2 - sx1), dy12 = abs_int(sy2 - sy1);
    int dx20 = abs_int(sx0 - sx2), dy20 = abs_int(sy0 - sy2);

    if (dx01 > EDGE_DX_MAX || dy01 > EDGE_DY_MAX) return 0;
    if (dx12 > EDGE_DX_MAX || dy12 > EDGE_DY_MAX) return 0;
    if (dx20 > EDGE_DX_MAX || dy20 > EDGE_DY_MAX) return 0;
    return 1;
}

/* Map averaged screen-Z to an OT bucket index, keeping the result in
 * [z_base, OT_BACKGROUND_BUCKET - 1] so mesh prims sit ABOVE the
 * background gradient but below any HUD layer.
 *
 * Phase 4.5.7.6 (2026-05-19): RE2-faithful bucket = OTZ >> 1.
 * Verified against RE2_Quellcode/FUN_8002d718.c:82-88 which does
 * `if (0x1fff < otz) otz = 0x1fff; ot_slot = ot + ((otz >> 1) & 0xffc)`
 * — the `& 0xffc` is the byte-offset form of `& 0x3FF` on the word
 * index, i.e. bucket = (OTZ >> 1) mod 1024. With OTZ clamped to 0x1FFF
 * upstream and OT_LENGTH=1024 this gives ~8 OTZ units per bucket,
 * matching RE2 exactly.
 *
 * z_base is the minimum bucket (closest to camera) — caller pins this
 * to keep meshes out of the BG/HUD reserved buckets. */
static int otz_to_bucket(long otz, int z_base)
{
    /* 2026-06-01 (audit finding #44): RE2 FUN_8002d718 indexes a 1024-WORD OT by
     * BYTE offset ((otz>>1)&0xffc); byte/4 = word index = otz>>3. We were doing
     * otz>>1 as a WORD index (4× too coarse) → with otz clamped to 0x1FFF every
     * poly with otz>=2044 collapsed into bucket 1022 → far-field z-fighting
     * (arm-through-torso). otz is pre-clamped to 0x1FFF so otz>>3 = 0..1023. */
    int bucket = (int)(otz >> 3);
    if (bucket < z_base)                bucket = z_base;
    if (bucket >= OT_BACKGROUND_BUCKET) bucket = OT_BACKGROUND_BUCKET - 1;
    return bucket;
}

/* Guard against use-before-init. mesh_psx_init() sets this to 1. */
static int s_gte_initialized = 0;

/* One-time GTE bring-up. Call from main BEFORE the per-frame loop. */
void mesh_psx_init(void)
{
    /* InitGeom enables the GTE coprocessor and zeros its state. Must be
     * called before any other GTE op or matrix upload, or transforms
     * produce garbage. */
    InitGeom();

    /* "Screen distance" sets perspective FOV. Smaller value → wider FOV.
     * 300 is the PSn00bSDK convention for a 320x240 framebuffer. */
    gte_SetGeomScreen(300);

    /* Set the projection origin to the centre of the framebuffer so the
     * world origin (0,0,0) maps to screen centre. */
    gte_SetGeomOffset(SCREEN_XRES / 2, SCREEN_YRES / 2);

    /* NOTE: InitGeom sets ZSF3=341 (0x155), ZSF4=256 (0x100) — the otz averaging scale.
     * This is what fixes the PC sprite.pri depth constant to depth*64 (re15_pri.h). Read
     * straight out of <InitGeom> in our own ELF (no GTE run needed). */

    s_gte_initialized = 1;
}

/* Render the loaded MD1 mesh.
 *
 * z_bucket       — OT depth bucket MIN; per-poly OTZ adds an offset on top
 *                  so close polys land in z_bucket, far ones land deeper
 *                  (toward OT_BACKGROUND_BUCKET-1).
 * angle_4096     — Y-axis rotation in PSX trig units (4096 = full turn)
 * pos_x/y/z      — world-space translation. Z must be > screen distance.
 * backface_cull  — non-zero: cull rear-facing polys via gte_nclip
 * mode           — wireframe (LINE_F2) or textured (POLY_FT3)
 */
void mesh_psx_render_test(int z_bucket, int32_t angle_4096,
                          int32_t pos_x, int32_t pos_y, int32_t pos_z,
                          int backface_cull, int mode)
{
    if (!re15_test_md1_ok || !s_gte_initialized) return;

    s_lightctx_active = 0;   /* #1c: demo path draws flat-neutral (no actor ctx) */
    s_tex_tpage_ovr = -1; s_tex_clut_ovr = -1; s_tex_clut_yshift = 0; s_tex_tpage_xshift = -1;  /* #2 baked default */

    /* Build per-frame rotation+translation matrix. PSX trig is naturally
     * modular (rsin/rcos handle wraparound for any int16), so we just
     * cast — no & 0xFFF mask, which used to drop the sign bit for
     * negative angles. */
    SVECTOR rotation_axes;
    rotation_axes.vx  = 0;
    rotation_axes.vy  = (int16_t) angle_4096;
    rotation_axes.vz  = 0;
    rotation_axes.pad = 0;

    MATRIX transform_matrix;
    RotMatrix(&rotation_axes, &transform_matrix);

    VECTOR translation;
    translation.vx = pos_x;
    translation.vy = pos_y;
    translation.vz = pos_z;
    TransMatrix(&transform_matrix, &translation);

    /* Upload to GTE control registers — persists across rtpt calls. */
    gte_SetRotMatrix(&transform_matrix);
    gte_SetTransMatrix(&transform_matrix);

    for (int mesh_idx = 0; mesh_idx < re15_test_md1.mesh_count; mesh_idx++) {
        const re15_md1_mesh_t *mesh = &re15_test_md1.meshes[mesh_idx];
        render_one_mesh_via_gte(mesh, mesh_idx, z_bucket, backface_cull, mode);
    }
}

/* ===== Per-mesh renderer (shared helper) =================================
 *
 * Renders one MD1 mesh's triangles + quads via the GTE pipeline. Assumes
 * gte_SetRotMatrix + gte_SetTransMatrix have ALREADY been called for the
 * desired transform. Used by both:
 *   - mesh_psx_render_test    (single global matrix for all meshes)
 *   - mesh_psx_render_skeletal (per-bone matrix per mesh)
 *
 * `mesh_idx` is the MD1 mesh index, used as the textured-tri's UV
 * source. */
static void render_one_mesh_via_gte(const re15_md1_mesh_t *mesh, int mesh_idx,
                                    int z_bucket, int backface_cull, int mode)
{
    (void)mesh_idx;  /* reserved for future per-mesh UV / tpage / clut routing */

    /* GTE NCCT: load the CODE colour with the POLY_GT3 command byte before the
     * tri loop (canonical FUN_800254a0:27-29). The light/colour/back matrices
     * were set by the caller (per actor + per bone). */
    if (s_lightctx_active && mode == MESH_PSX_MODE_TEXTURED)
        gte_ldrgb(&s_ncct_code_tri);

    /* RE2-canonical inline emit (FUN_8002d0e8): one cursor snapshot per mesh; the
     * hot loop advances E.next + addPrim()s inline — NO prim_alloc/prim_link call
     * per polygon. The textured paths below use E directly. */
    re15_emit_t E;
    re15_render_emit_begin(&E);

    /* ============ TRIANGLES =================================== */
        for (int tri_idx = 0; tri_idx < mesh->triangle_count; tri_idx++) {
            const re15_md1_triangle_t *tri = &mesh->triangles[tri_idx];

            if (tri->v0 >= mesh->tri_vertex_count) continue;
            if (tri->v1 >= mesh->tri_vertex_count) continue;
            if (tri->v2 >= mesh->tri_vertex_count) continue;

            /* --- Step 1+2: load + project. gte_ldv3 reads the MD1 vertices
             * DIRECTLY — re15_md1_vertex_t {s16 x,y,z,pad} is byte-identical to
             * SVECTOR {s16 vx,vy,vz,pad}, so the old per-poly copy-to-stack was
             * pure overhead. RE2 does the same (gte_ldv3 straight from the model
             * array). On the data-cache-less R3000 this removes ~21 uncached
             * halfword mem-ops/tri (the proven per-poly cost class). MD1 vertex
             * arrays are 4-aligned by the on-disc TMD layout the original engine
             * itself gte_ldv3's from. (rank-1, RE2-render audit 2026-06-02b.) */
            gte_ldv3((SVECTOR *)&mesh->tri_vertices[tri->v0],
                     (SVECTOR *)&mesh->tri_vertices[tri->v1],
                     (SVECTOR *)&mesh->tri_vertices[tri->v2]);
            gte_rtpt();

            /* --- Step 2a: GTE saturation check. If any vertex saturated
             * during the perspective divide, the projected screen coords
             * for that vertex are clamped to ±1024 — geometry is then
             * effectively garbage and the GPU would silently discard the
             * primitive (or draw a misplaced triangle). Read FLAG now,
             * before nclip/avsz3 clobber it. */
            {
                long gte_flag;
                gte_stflg(&gte_flag);
                if ((unsigned long)gte_flag & GTE_FLAG_ERROR_MASK) continue;
            }

            /* --- Step 3: GTE backface cull via 2D screen-space cross.
             * nclip's MAC0 result is the SIGNED 2D cross product of the
             * three projected (sx,sy) vertices. Negative = back-facing
             * in PSX screen-Y-down convention. RE2 uses < 0; PSn00bSDK's
             * gte/main.c uses < 0; we match.
             *
             * The 2D cross is computed on already-saturated SXY values
             * (limD1/D2 clamps to ±1024 — see psx-spx GTE doc), so the
             * pre-emit edge-delta check below stays the authoritative
             * filter for "would the GPU silently drop this prim". */
            if (backface_cull) {
                long nclip_result;
                gte_nclip();
                gte_stopz(&nclip_result);
                if (nclip_result < 0) continue;
            }

            /* --- Step 4: averaged Z for OT bucketing + near/far gating. */
            long otz;
            gte_avsz3();
            gte_stotz(&otz);
            if (otz < OTZ_NEAR_PLANE_DROP) continue;
            if (otz > OTZ_FAR_CLAMP) otz = OTZ_FAR_CLAMP;

            /* --- Step 5: per-poly OT bucket. */
            int bucket = otz_to_bucket(otz, z_bucket);

            /* --- Emit. CANONICAL direct path (FUN_800254a0): for the common
             * flat-textured case write the projected XY straight from the GTE
             * into the POLY_FT3 (gte_stsxy3_ft3) — NO CPU extraction of the
             * packed SXY, NO per-tri re15_render_textured_tri function-call
             * round-trip (those were the dominant per-tri CPU cost ×thousands).
             * lit + wireframe are not the hot path → keep their CPU extraction. */
            if (mode == MESH_PSX_MODE_TEXTURED) {
                const re15_md1_tri_uv_t *uv = &mesh->triangle_uvs[tri_idx];
                int tp = (s_tex_tpage_ovr >= 0) ? s_tex_tpage_ovr
                       : (s_tex_tpage_xshift >= 0)
                         ? ((uv->page & 0xFFF0) | (((uv->page & 0xF) + s_tex_tpage_xshift) & 0xF))
                         : remap_md1_tpage(uv->page);
                int cl = (s_tex_clut_ovr >= 0) ? s_tex_clut_ovr
                                               : ((int)uv->clut + (s_tex_clut_yshift << 6));
                int lit = (s_lightctx_active && mesh->tri_normals &&
                           tri->n0 < mesh->tri_normal_count &&
                           tri->n1 < mesh->tri_normal_count &&
                           tri->n2 < mesh->tri_normal_count);
                if (!lit) {
                    if (E.next + sizeof(POLY_FT3) + 16 <= E.end) {
                        POLY_FT3 *p = (POLY_FT3 *) E.next; E.next += sizeof(POLY_FT3);
                        setPolyFT3(p);
                        gte_stsxy3_ft3(p);               /* GTE SXY FIFO → prim, no CPU */
                        setUV3(p, uv->u0, uv->v0, uv->u1, uv->v1, uv->u2, uv->v2);
                        p->clut  = (uint16_t)cl;
                        p->tpage = (uint16_t)tp;
                        setRGB0(p, 128, 128, 128);
                        addPrim(&E.ot[bucket], p);
                    }
                } else {
                    /* GTE HARDWARE NCCT triangle (FUN_800254a0). Extract the
                     * projected XY to CPU NOW (SXY FIFO from gte_rtpt above) and
                     * use setXY3 — robust against any FIFO-shift; then run
                     * gte_ncct on the 3 NORMALS (uses the loaded CODE + per-bone
                     * light + per-actor colour/back matrices) and write the 3
                     * Gouraud RGBs straight from the colour FIFO (gte_strgb3_gt3,
                     * no position op between → FIFO intact). */
                    if (E.next + sizeof(POLY_GT3) + 16 <= E.end) {
                        POLY_GT3 *p = (POLY_GT3 *) E.next; E.next += sizeof(POLY_GT3);
                        setPolyGT3(p);
                        /* RE2-canonical direct emit (FUN_800254a0): SXY straight from
                         * the GTE (gte_rtpt above left it in the FIFO; gte_ldv3+gte_ncct
                         * touch V0-2/colour, NOT the SXY0-2 result regs → still valid)
                         * — NO CPU extraction + setXY3 per poly. gte_ldv3 reads the MD1
                         * normals DIRECTLY (SVECTOR-identical layout, no stack copy). */
                        gte_ldv3((SVECTOR *)&mesh->tri_normals[tri->n0],
                                 (SVECTOR *)&mesh->tri_normals[tri->n1],
                                 (SVECTOR *)&mesh->tri_normals[tri->n2]);
                        gte_ncct();
                        gte_stsxy3_gt3(p);               /* projected XY → prim, no CPU */
                        gte_strgb3_gt3(p);               /* NCCT RGB → prim */
                        setUV3(p, uv->u0, uv->v0, uv->u1, uv->v1, uv->u2, uv->v2);
                        p->clut  = (uint16_t)cl;
                        p->tpage = (uint16_t)tp;
                        addPrim(&E.ot[bucket], p);
                    }
                }
            } else {
                /* Wireframe (debug; mode is always TEXTURED in-game). re15_render_line
                 * uses the GLOBAL packet cursor → sync E out/in around it. */
                re15_render_emit_end(&E);
                uint32_t pxy0=0,pxy1=0,pxy2=0;
                gte_stsxy3(&pxy0,&pxy1,&pxy2);
                int sx0=(int16_t)(pxy0&0xFFFF), sy0=(int16_t)((pxy0>>16)&0xFFFF);
                int sx1=(int16_t)(pxy1&0xFFFF), sy1=(int16_t)((pxy1>>16)&0xFFFF);
                int sx2=(int16_t)(pxy2&0xFFFF), sy2=(int16_t)((pxy2>>16)&0xFFFF);
                re15_render_line(sx0,sy0,sx1,sy1,bucket,0,255,0);
                re15_render_line(sx1,sy1,sx2,sy2,bucket,0,255,0);
                re15_render_line(sx2,sy2,sx0,sy0,bucket,0,255,0);
                re15_render_emit_begin(&E);
            }
        }

        /* ============ QUADS ======================================= */
        /* Re-load the CODE colour with the POLY_GT4 command byte for the quad
         * NCCT (canonical FUN_800256b0 uses 0x3c, not the tri's 0x34). */
        if (s_lightctx_active && mode == MESH_PSX_MODE_TEXTURED)
            gte_ldrgb(&s_ncct_code_quad);

        for (int quad_idx = 0; quad_idx < mesh->quad_count; quad_idx++) {
            const re15_md1_quad_t *quad = &mesh->quads[quad_idx];

            if (quad->v0 >= mesh->quad_vertex_count) continue;
            if (quad->v1 >= mesh->quad_vertex_count) continue;
            if (quad->v2 >= mesh->quad_vertex_count) continue;
            if (quad->v3 >= mesh->quad_vertex_count) continue;

            /* Project first 3 verts (v0/v1/v2) via rtpt; the 4th via rtps below.
             * gte_ldv3/ldv0 read the MD1 quad vertices DIRECTLY (SVECTOR-identical
             * layout, no per-poly stack copy — rank-1, RE2-render audit). */
            gte_ldv3((SVECTOR *)&mesh->quad_vertices[quad->v0],
                     (SVECTOR *)&mesh->quad_vertices[quad->v1],
                     (SVECTOR *)&mesh->quad_vertices[quad->v2]);
            gte_rtpt();

            /* GTE saturation check for v0/v1/v2 (see triangle path). */
            {
                long gte_flag;
                gte_stflg(&gte_flag);
                if ((unsigned long)gte_flag & GTE_FLAG_ERROR_MASK) continue;
            }

            /* Backface cull on (v0,v1,v2) — valid for planar quads because
             * the cross-product sign is identical for any 3 verts of the
             * same planar convex polygon. RE1.5 MD1 quads ARE planar. */
            if (backface_cull) {
                long nclip_result;
                gte_nclip();
                gte_stopz(&nclip_result);
                if (nclip_result < 0) continue;
            }

            /* Store first 3 projected screen positions BEFORE the rtps
             * for v3 shifts the SXY FIFO — see psn00bSDK example
             * graphics/gte/main.c L201-209 for the canonical sequence. */
            uint32_t packed_xy0 = 0, packed_xy1 = 0, packed_xy2 = 0, packed_xy3 = 0;
            gte_stsxy3(&packed_xy0, &packed_xy1, &packed_xy2);

            /* Project v3 separately (rtps = single-vertex variant of rtpt),
             * loaded DIRECTLY from the MD1 array (no stack copy — rank-1). */
            gte_ldv0((SVECTOR *)&mesh->quad_vertices[quad->v3]);
            gte_rtps();

            /* GTE saturation check for v3. rtps reuses the same Limiter +
             * divide-overflow flags as rtpt — read FLAG now before avsz4
             * clobbers it. If v3 saturated, drop the whole quad (we can't
             * safely render either of the two split triangles). */
            {
                long gte_flag;
                gte_stflg(&gte_flag);
                if ((unsigned long)gte_flag & GTE_FLAG_ERROR_MASK) continue;
            }

            /* avsz4 averages SZ of all 4 verts now in the SZ FIFO. */
            long otz;
            gte_avsz4();
            gte_stotz(&otz);
            if (otz < OTZ_NEAR_PLANE_DROP) continue;
            if (otz > OTZ_FAR_CLAMP) otz = OTZ_FAR_CLAMP;

            int bucket = otz_to_bucket(otz, z_bucket);

            /* Read v3's projected XY (SXY2 holds the most recent rtps). */
            gte_stsxy(&packed_xy3);

            int sx0 = (int16_t)( packed_xy0        & 0xFFFF);
            int sy0 = (int16_t)((packed_xy0 >> 16) & 0xFFFF);
            int sx1 = (int16_t)( packed_xy1        & 0xFFFF);
            int sy1 = (int16_t)((packed_xy1 >> 16) & 0xFFFF);
            int sx2 = (int16_t)( packed_xy2        & 0xFFFF);
            int sy2 = (int16_t)((packed_xy2 >> 16) & 0xFFFF);
            int sx3 = (int16_t)( packed_xy3        & 0xFFFF);
            int sy3 = (int16_t)((packed_xy3 >> 16) & 0xFFFF);

            /* Quad winding is (v0, v1, v3, v2) — see md1 format docs. The
             * two split triangles for textured rendering are:
             *   T1: v0 → v1 → v3
             *   T2: v0 → v3 → v2
             * Edge-delta check each tri separately — one may fit and the
             * other not (degenerate near-camera quad). */

            if (mode == MESH_PSX_MODE_TEXTURED) {
                const re15_md1_quad_uv_t *uv = &mesh->quad_uvs[quad_idx];
                int page = (s_tex_tpage_ovr >= 0) ? s_tex_tpage_ovr
                       : (s_tex_tpage_xshift >= 0)
                         ? ((uv->page & 0xFFF0) | (((uv->page & 0xF) + s_tex_tpage_xshift) & 0xF))
                         : remap_md1_tpage(uv->page);
                int qcl  = (s_tex_clut_ovr >= 0) ? s_tex_clut_ovr
                                                 : ((int)uv->clut + (s_tex_clut_yshift << 6));
                int lit = (s_lightctx_active && mesh->quad_normals &&
                           quad->n0 < mesh->quad_normal_count &&
                           quad->n1 < mesh->quad_normal_count &&
                           quad->n2 < mesh->quad_normal_count &&
                           quad->n3 < mesh->quad_normal_count);

                /* edge-delta guards REMOVED (canonical: FUN_800256b0 has none). */
                if (lit) {
                    /* CANONICAL GTE HARDWARE NCCT quad = ONE POLY_GT4 (matches
                     * FUN_800256b0): gte_ncct shades the first 3 normals →
                     * gte_strgb3_gt4 (rgb0/1/2); the 4th via gte_ldv0+gte_nccs
                     * → gte_strgb into rgb3. Positions reuse the CPU-extracted
                     * sx0..sy3 (already projected above). Verts are strip-order
                     * v0,v1,v2,v3 → setXY4/setUV4 map 1:1. */
                    if (E.next + sizeof(POLY_GT4) + 16 <= E.end) {
                        POLY_GT4 *p = (POLY_GT4 *) E.next; E.next += sizeof(POLY_GT4);
                        setPolyGT4(p);
                        setXY4(p, sx0,sy0, sx1,sy1, sx2,sy2, sx3,sy3);
                        setUV4(p, uv->u0,uv->v0, uv->u1,uv->v1, uv->u2,uv->v2, uv->u3,uv->v3);
                        p->clut  = (uint16_t)qcl;
                        p->tpage = (uint16_t)page;
                        /* gte_ldv3/ldv0 read the MD1 quad normals DIRECTLY (no copy). */
                        gte_ldv3((SVECTOR *)&mesh->quad_normals[quad->n0],
                                 (SVECTOR *)&mesh->quad_normals[quad->n1],
                                 (SVECTOR *)&mesh->quad_normals[quad->n2]);
                        gte_ncct();
                        gte_strgb3_gt4(p);                 /* rgb0/1/2 */
                        gte_ldv0((SVECTOR *)&mesh->quad_normals[quad->n3]);
                        gte_nccs();
                        gte_strgb((CVECTOR *)&p->r3);      /* rgb3 */
                        addPrim(&E.ot[bucket], p);
                    }
                } else {
                    /* CANONICAL flat quad = ONE POLY_FT4 (matches FUN_800256b0),
                     * not 2 POLY_FT3 → half the prims (GPU setup) + one emit. MD1
                     * quad verts are in the GPU strip order v0,v1,v2,v3 (tri 0,1,2
                     * + tri 1,2,3 = the quad), so setXY4/setUV4 map 1:1. */
                    if (E.next + sizeof(POLY_FT4) + 16 <= E.end) {
                        POLY_FT4 *p = (POLY_FT4 *) E.next; E.next += sizeof(POLY_FT4);
                        setPolyFT4(p);
                        setXY4(p, sx0,sy0, sx1,sy1, sx2,sy2, sx3,sy3);
                        setUV4(p, uv->u0,uv->v0, uv->u1,uv->v1, uv->u2,uv->v2, uv->u3,uv->v3);
                        p->clut  = (uint16_t)qcl;
                        p->tpage = (uint16_t)page;
                        setRGB0(p, 128,128,128);
                        addPrim(&E.ot[bucket], p);
                    }
                }
            } else {
                /* Wireframe (debug): re15_render_line uses the GLOBAL cursor →
                 * sync E out/in around it. 4-edge loop in (v0,v1,v3,v2) order. */
                re15_render_emit_end(&E);
                if (abs_int(sx1 - sx0) <= EDGE_DX_MAX && abs_int(sy1 - sy0) <= EDGE_DY_MAX)
                    re15_render_line(sx0, sy0, sx1, sy1, bucket, 0, 255, 0);
                if (abs_int(sx3 - sx1) <= EDGE_DX_MAX && abs_int(sy3 - sy1) <= EDGE_DY_MAX)
                    re15_render_line(sx1, sy1, sx3, sy3, bucket, 0, 255, 0);
                if (abs_int(sx2 - sx3) <= EDGE_DX_MAX && abs_int(sy2 - sy3) <= EDGE_DY_MAX)
                    re15_render_line(sx3, sy3, sx2, sy2, bucket, 0, 255, 0);
                if (abs_int(sx0 - sx2) <= EDGE_DX_MAX && abs_int(sy0 - sy2) <= EDGE_DY_MAX)
                    re15_render_line(sx2, sy2, sx0, sy0, bucket, 0, 255, 0);
                re15_render_emit_begin(&E);
            }
        }

    /* RE2-canonical inline emit: write the advanced cursor back once per mesh. */
    re15_render_emit_end(&E);
}

/* ===== Skeletal multi-mesh renderer ====================================
 *
 * Phase 4.5.7.3: render PL00 (or any skeletal model) by computing the
 * world-space pose for each bone at the given keyframe, then walking
 * the MD1 mesh list and rendering mesh[i] via the bone[i] matrix.
 *
 * Each per-bone Q12 rotation matrix from re15_skel_compute_pose() is
 * cast to PSX MATRIX format (s16 entries, same Q12 scale) and uploaded
 * to the GTE before rendering that mesh. Translation is the bone's
 * world position plus a caller-supplied model origin offset.
 *
 * mesh_count is clamped to the skeleton's bone_count so extra MD1
 * meshes (e.g. weapon attach slots not in the skeleton) are skipped.
 * Phase 4.5.7.4 will add proper per-frame keyframe advancement —
 * here keyframe_index is a static input. */
/* Phase 4.5.8.2 (2026-05-19): camera-aware skeletal render.
 *
 * Caller supplies a pre-built view matrix (from re15_camera_build_view).
 * pos_x/y/z is the model's WORLD-SPACE origin — added to each bone's
 * world translation BEFORE the view transform. This lets the camera
 * place Leon at any world coord (e.g. the target point of the active
 * cut for the demo) instead of the previous compile-time +Z=6000 hack.
 *
 * Per bone, we compose `view × bone_world` in C (Q12 matrix multiply)
 * and upload the result to the GTE. RE2 does this composition via
 * FUN_8002ce94 (GTE-accelerated CompMatrixLV); we use the same C path
 * on both targets for consistency — performance is fine for 15-bone
 * humanoid models. */
void mesh_psx_render_skeletal(int z_bucket,
                              const re15_camera_view_t *view,
                              const re15_md1_t *md1,
                              const re15_emd_skeleton_t *skel,
                              int32_t pos_x, int32_t pos_y, int32_t pos_z,
                              int16_t face_sin_q12, int16_t face_cos_q12,
                              int backface_cull, int mode,
                              int keyframe_index,
                              int tpage, int clut, int clut_yshift,
                              int tpage_xshift)
{
    if (!md1 || !skel || !s_gte_initialized) return;
    if (!view) return;

    /* #2/#7: texture source. Leon: all -1/0 → per-tri baked page (Leon global
     * remap) + baked clut. Elliot: tpage_ovr=-1 + tpage_xshift=13 (his texture
     * spans 2 tpages, keep per-tri page, shift X), clut_ovr=-1 + clut_yshift=+3
     * (keep per-tri CLUT row body-vs-face, shift to his relocated 2-row block). */
    s_tex_tpage_ovr    = tpage;
    s_tex_clut_ovr     = clut;
    s_tex_clut_yshift  = clut_yshift;
    s_tex_tpage_xshift = tpage_xshift;

    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (re15_skel_compute_pose(skel, keyframe_index, poses) != 0) return;

    /* Phase 4.4.5.1: model Y-rotation from caller-supplied face sin/cos.
     * R_y = | cos  0  sin |
     *       | 0    1   0  |
     *       | -sin 0  cos |
     * For yaw=0 (cos=4096, sin=0) this is identity. Caller chooses the
     * heading source: player uses g_game.player_face_*, actors compute
     * from rot_y via re15_sin_q12 / re15_cos_q12. */
    int32_t face_s = (int32_t)face_sin_q12;
    int32_t face_c = (int32_t)face_cos_q12;
    int32_t yaw_rot[9] = {
         face_c, 0,  face_s,
         0,      0x1000, 0,
        -face_s, 0,  face_c
    };

    /* GTE HARDWARE NCCT (canonical, replaces the software per-vertex path):
     * build this actor's light context with eval_pos = the actor's OWN world
     * pos (matches PSX FUN_80053fc0), but pass rot=NULL so ctx->L stays in
     * WORLD space — we fold each bone's world rotation into the GTE Light
     * Matrix per bone below (LLM_world × bone_rot, exactly like the original
     * render_bone_with_lighting: MulMatrix0(&DAT_80076d14, m1) + SetLightMatrix).
     * The Color Matrix (light colours) + Back Color (ambient) are bone-
     * independent → loaded into the GTE ONCE here (FUN_800542dc / SetBackColor). */
    s_lightctx_active = 0;
    if (s_ncct_enabled && g_re15_room_lights_ok &&
        g_re15_active_cut >= 0 &&
        g_re15_active_cut < g_re15_room_lights.cut_count) {
        int32_t apos[3] = { pos_x, pos_y, pos_z };
        /* CANONICAL (2026-06-02): rot=NULL so ctx->L stays in WORLD space. The
         * Colour Matrix (light colours) + Back Colour (ambient) are bone-
         * independent → loaded into the GTE ONCE here (FUN_8001e8c8.c:18-19).
         * The Light Matrix is folded with each bone's world rotation INSIDE the
         * bone loop below (LLM_world × bone_rot, FUN_8001e9ec.c:30-31). */
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut],
                               apos, NULL, &s_lightctx);
        mesh_psx_load_light_color_matrices(&s_lightctx);
        s_lightctx_active = 1;
    }

    /* Phase 4.5.10-L: revert to 1:1 mesh-bone mapping.
     * Phase 4.5.10-H's bone_mesh_index theory was wrong (that table is the
     * EMR child-traversal list, not a bone→mesh permutation). 1:1 is
     * verified correct per revengi/BioModels/lib_bio. */
    int n_bones = skel->bone_count;
    if (n_bones > md1->mesh_count) n_bones = md1->mesh_count;
    for (int b = 0; b < n_bones; b++) {
        int mesh_idx = b;

        const re15_skel_pose_t *pose = &poses[b];

        /* Pre-rotate pose by R_y(yaw):
         *   yawed_rot   = R_y · pose.rot              (Q12 · Q12 → Q12)
         *   yawed_trans = R_y · pose.trans            (Q12 · world → world after >>12)
         * Then bone-world translation = (pos_x/y/z) + yawed_trans. */
        /* PERF (audit P0.1): int32, NOT int64. R3000 has a HW 32-bit multiply
         * but emulates 64-bit (libgcc __muldi3, ~10x slower). Q12 rot (±4096) ×
         * Q12 rot = ±16.7M, sum of 3 = ±50M; Q12 × bone-local trans = tens of M
         * — all well within int32 (±2.1B). Bit-identical result, far cheaper. */
        int32_t yawed_rot[9];
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int32_t s = 0;
                for (int k = 0; k < 3; k++) {
                    s += yaw_rot[r*3 + k] * pose->rot[k*3 + c];
                }
                yawed_rot[r*3 + c] = s >> 12;
            }
        }
        int32_t yawed_trans[3];
        for (int r = 0; r < 3; r++) {
            int32_t s = 0;
            for (int k = 0; k < 3; k++) {
                s += yaw_rot[r*3 + k] * pose->trans[k];
            }
            yawed_trans[r] = s >> 12;
        }
        int32_t bone_world_trans[3] = {
            pos_x + yawed_trans[0],
            pos_y + yawed_trans[1],
            pos_z + yawed_trans[2],
        };

        /* combined = view × (R_y × bone) */
        int32_t combined_rot[9];
        int32_t combined_trans[3];
        re15_camera_compose_view_bone(view, yawed_rot, bone_world_trans,
                                       combined_rot, combined_trans);

        MATRIX m;
        /* Phase 4.5.10-E: Q12 chain (view_rot × yaw_rot × pose_rot) can
         * accumulate to ±36864 — beyond s16 range. Without clamp the
         * (short) cast wraps and the rotation matrix becomes garbage →
         * Leon's mesh degenerates to invisible/wrong-positioned tris. */
        for (int qi = 0; qi < 9; qi++) {
            int32_t v = combined_rot[qi];
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            ((short *)m.m)[qi] = (short)v;
        }
        m.t[0] = combined_trans[0];
        m.t[1] = combined_trans[1];
        m.t[2] = combined_trans[2];

        gte_SetRotMatrix(&m);
        gte_SetTransMatrix(&m);

        /* CANONICAL per-bone GTE Light Matrix = LLM_world × bone_world_rot
         * (FUN_8001e9ec.c:30-31). yawed_rot[9] = R_y(yaw) × pose.rot is exactly
         * this bone's WORLD rotation (no camera/view component — the LLM must be
         * in the same world frame as the cut's light dirs, NOT view space). The
         * gte_SetRotMatrix above only touches cr0-4 (vertex projection); gte_ncct
         * reads the light matrix (cr8-12), so it must be reloaded per bone here. */
        if (s_lightctx_active)
            mesh_psx_load_bone_light_matrix(&s_lightctx, yawed_rot);

        const re15_md1_mesh_t *mesh = &md1->meshes[mesh_idx];
        render_one_mesh_via_gte(mesh, mesh_idx, z_bucket, backface_cull, mode);
    }
    s_tex_tpage_ovr = -1;   /* restore baked default for later calls */
    s_tex_clut_ovr  = -1;
    s_tex_clut_yshift = 0;
    s_tex_tpage_xshift = -1;
}

/* ===== Non-skeletal prop renderer (#2, 2026-06-01) =====================
 *
 * Render a whole MD1 as ONE rigid object at a world pos+yaw, THROUGH the
 * camera view (unlike mesh_psx_render_test which uses a standalone matrix).
 * Used for actor-type props that have no skeleton — first consumer is the
 * helicopter body (actor type 0x47). A single world matrix transforms every
 * mesh; the NCCT light ctx is set up once for the prop (eval_pos = its world
 * pos) so it shades like the characters. */
void mesh_psx_render_prop(int z_bucket,
                          const re15_camera_view_t *view,
                          const re15_md1_t *md1,
                          int32_t pos_x, int32_t pos_y, int32_t pos_z,
                          int16_t face_sin_q12, int16_t face_cos_q12,
                          int backface_cull, int mode,
                          int tpage, int clut)
{
    if (!md1 || !view || !s_gte_initialized) return;

    /* #2: route this prop's OWN texture (relocated VRAM slot) for every poly,
     * instead of the MD1's baked page (which assumes the original RE1.5 VRAM)
     * + Leon's remap. -1 = fall back to baked (handled in render_one_mesh). */
    s_tex_tpage_ovr = tpage;
    s_tex_clut_ovr  = clut;
    s_tex_clut_yshift = 0;   /* props/heli are single-CLUT-row → use the override */
    s_tex_tpage_xshift = -1; /* props/heli use the single tpage override above */

    int32_t face_s = (int32_t)face_sin_q12;
    int32_t face_c = (int32_t)face_cos_q12;
    int32_t world_rot[9] = {
         face_c, 0,  face_s,
         0,      0x1000, 0,
        -face_s, 0,  face_c
    };
    int32_t world_trans[3] = { pos_x, pos_y, pos_z };

    /* #2/#1c: GTE hardware NCCT for the prop (eval_pos = prop world pos).
     * CANONICAL (2026-06-02): rot=NULL → ctx->L world-space; load LCM+BackColor
     * once; then fold the prop's single rigid world_rot into the GTE Light Matrix
     * (a prop is one rigid "bone"). LLM_world × world_rot, FUN_80039ca0.c:23-24. */
    s_lightctx_active = 0;
    if (s_ncct_enabled && g_re15_room_lights_ok &&
        g_re15_active_cut >= 0 &&
        g_re15_active_cut < g_re15_room_lights.cut_count) {
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut],
                               world_trans, NULL, &s_lightctx);
        s_lightctx_active = 1;
    }

    int32_t combined_rot[9], combined_trans[3];
    re15_camera_compose_view_bone(view, world_rot, world_trans,
                                   combined_rot, combined_trans);

    MATRIX m;
    for (int qi = 0; qi < 9; qi++) {
        int32_t v = combined_rot[qi];
        if (v >  32767) v =  32767;
        if (v < -32768) v = -32768;
        ((short *)m.m)[qi] = (short)v;
    }
    m.t[0] = combined_trans[0];
    m.t[1] = combined_trans[1];
    m.t[2] = combined_trans[2];

    gte_SetRotMatrix(&m);
    gte_SetTransMatrix(&m);

    /* CRITICAL (2026-06-02): load the NCCT Light/Colour/Back matrices AFTER
     * gte_SetRotMatrix/SetTransMatrix, immediately before the mesh emit — NOT in
     * the setup block above. Empirically, loading them before the SetRot/SetTrans
     * pair leaves the GTE light-matrix registers (cr8-12) holding the PREVIOUSLY
     * rendered prop's matrix at gte_ncct time: for the intro that previous prop is
     * the SPINNING main/tail rotor (obj03/04, rendered earlier in the prop loop),
     * so the otherwise-static pilot (obj05) inherited a per-frame-varying light
     * matrix → its whole-body shade PULSED at the rotor's spin period (~0.42 s).
     * The skeletal path already (correctly) loads the per-bone light matrix here,
     * after SetRot/SetTrans — which is why animating actors (Elliot) never pulsed.
     * Verified: pilot shade rock-stable (Δ<25/255 over 60 frames) with the rotor
     * spinning and the game running. The PC software backend is unaffected (it
     * dots ctx->L directly, no shared GTE registers). */
    if (s_lightctx_active) {
        mesh_psx_load_light_color_matrices(&s_lightctx);
        mesh_psx_load_bone_light_matrix(&s_lightctx, world_rot);
    }

    for (int mi = 0; mi < md1->mesh_count; mi++) {
        render_one_mesh_via_gte(&md1->meshes[mi], mi, z_bucket, backface_cull, mode);
    }
    s_tex_tpage_ovr = -1;   /* restore Leon-baked default for later calls */
    s_tex_clut_ovr  = -1;
    s_tex_clut_yshift = 0;
    s_tex_tpage_xshift = -1;
}

/* RE1.5 CHARACTER FLOOR SHADOW — POLY_FT4 subtractive (ABR=2) blob quad.
 * Recipe RE'd from FUN_8001b064/FUN_8001af5c (2026-06-03): half-extents
 * ±500 X / ±600 Z, centered on the actor at floor Y, rotated by the CAMERA
 * yaw (RotMatrixY(cam+0x6a)), projected via the GTE (RotAverage4 analogue),
 * textured with the TEX.TIM blob (here a self-contained 16bpp+STP slot since
 * the PSX build has no resident TEX.TIM), prim RGB 0x808080 neutral — the
 * darkening is purely the ABR-2 subtractive blend of the blob. */
void mesh_psx_render_actor_shadow(int z_bucket, const re15_camera_view_t *view,
                                  int32_t wx, int32_t wy, int32_t wz,
                                  int32_t fwd_x, int32_t fwd_z)
{
    if (!view || !s_gte_initialized || !re15_shadow_ok) return;

    /* camera-yaw rotation R_y(cam) from the normalised cut forward (XZ) — SHARED
     * with the PC build via re15_camera_yaw_matrix (sin=fwd_x/len, cos=fwd_z/len). */
    int32_t cam_rot[9];
    re15_camera_yaw_matrix(fwd_x, fwd_z, cam_rot);
    int32_t wpos[3] = { wx, wy, wz };

    int32_t crot[9], ctrans[3];
    re15_camera_compose_view_bone(view, cam_rot, wpos, crot, ctrans);

    MATRIX m;
    for (int qi = 0; qi < 9; qi++) {
        int32_t v = crot[qi];
        if (v >  32767) v =  32767;
        if (v < -32768) v = -32768;
        ((short *)m.m)[qi] = (short)v;
    }
    m.t[0] = ctrans[0]; m.t[1] = ctrans[1]; m.t[2] = ctrans[2];
    gte_SetRotMatrix(&m);
    gte_SetTransMatrix(&m);

    /* 4 floor corners (Z-strip order = FUN_8001af5c): v0(-X,+Z)→uv(0,0),
     * v1(-X,-Z)→uv(W,0), v2(+X,+Z)→uv(0,H), v3(+X,-Z)→uv(W,H). */
    SVECTOR c0 = { -500, 0,  600, 0 };
    SVECTOR c1 = { -500, 0, -600, 0 };
    SVECTOR c2 = {  500, 0,  600, 0 };
    SVECTOR c3 = {  500, 0, -600, 0 };

    gte_ldv3(&c0, &c1, &c2);
    gte_rtpt();
    uint32_t xy0 = 0, xy1 = 0, xy2 = 0, xy3 = 0;
    gte_stsxy3(&xy0, &xy1, &xy2);
    gte_ldv0(&c3);
    gte_rtps();
    long otz;
    gte_avsz4();
    gte_stotz(&otz);
    if (otz < OTZ_NEAR_PLANE_DROP) return;          /* off-stage / behind camera */
    if (otz > OTZ_FAR_CLAMP) otz = OTZ_FAR_CLAMP;
    gte_stsxy(&xy3);

    POLY_FT4 *p = (POLY_FT4 *) re15_render_prim_alloc(sizeof(POLY_FT4));
    if (!p) return;
    setPolyFT4(p);
    setSemiTrans(p, 1);                              /* + tpage ABR=2 → dst-=texel */
    setXY4(p, (int16_t)(xy0 & 0xFFFF), (int16_t)((xy0 >> 16) & 0xFFFF),
              (int16_t)(xy1 & 0xFFFF), (int16_t)((xy1 >> 16) & 0xFFFF),
              (int16_t)(xy2 & 0xFFFF), (int16_t)((xy2 >> 16) & 0xFFFF),
              (int16_t)(xy3 & 0xFFFF), (int16_t)((xy3 >> 16) & 0xFFFF));
    setUV4(p, 0, 0,  25, 0,  0, 29,  25, 29);        /* blob is 26×30 (max idx 25,29) */
    p->tpage = (uint16_t) re15_shadow_tpage;
    p->clut  = 0;                                    /* 16bpp direct → CLUT unused */
    setRGB0(p, 128, 128, 128);                       /* neutral; darkening = ABR2 */
    re15_render_prim_link(p, otz_to_bucket(otz, z_bucket));
}

/* Update GTE projection params from the active camera view. Call on
 * every cut change (RE2: FUN_8003458c.c:47). Cheap — just writes the
 * H register. */
void mesh_psx_set_camera_fov(const re15_camera_view_t *view)
{
    if (!view || !s_gte_initialized) return;
    gte_SetGeomScreen((int)view->fov_screen_dist);
}
