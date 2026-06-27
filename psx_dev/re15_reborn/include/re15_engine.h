/*
 * RE1.5 Rebuilt — Engine public API (Phase 4, 2026-05-18).
 *
 * Layered architecture (built up phase-by-phase):
 *   Phase 4.1: boot + main loop + input + text rendering    ← we are here
 *   Phase 4.2: VRAM management + 2D primitives + double-buffer
 *   Phase 4.3: CD asset loading (RDT, MD1, TIM)
 *   Phase 4.4: SCD VM in C (RE1.5's script engine)
 *   Phase 4.5: 3D rendering (MD1 models, MDEC backgrounds)
 *   Phase 4.6: Audio (libsnd/libsd, BGM, SFX, voice — RE2-faithful)
 *   Phase 4.7: Game state (player, inventory, save/load)
 *   Phase 4.8: Combat / weapon FSM
 *   Phase 4.9: Final integration
 */
#ifndef RE15_ENGINE_H
#define RE15_ENGINE_H

#include <stdint.h>

/* Engine version */
#define RE15_REBORN_VERSION_MAJOR  0
#define RE15_REBORN_VERSION_MINOR  1
#define RE15_REBORN_VERSION_PATCH  0
#define RE15_REBORN_VERSION_STRING "v0.1.0 Phase 4.1"

/* Display constants */
#define SCREEN_XRES  320
#define SCREEN_YRES  240

/* Engine state — minimal until Phase 4.2 */
typedef struct {
    uint32_t frame_count;
    uint16_t pad_current;     /* current frame pad bits */
    uint16_t pad_previous;    /* previous frame pad bits */
    uint16_t pad_pressed;     /* bits set this frame only */
    uint16_t pad_released;    /* bits cleared this frame only */
} re15_engine_state_t;

extern re15_engine_state_t g_engine;

/* Subsystem init prototypes */
void re15_input_init(void);
void re15_input_tick(void);
void re15_render_init(void);
void re15_render_begin_frame(void);
void re15_render_end_frame(void);

/* PERF profiler intra-frame sub-phase timers (hblank ticks). main.c writes the
 * per-frame deltas; the profiler in re15_render_end_frame averages them. */
extern int g_re15_pf_logic;   /* SCD + walker + FSM + audio tick */
extern int g_re15_pf_rbuild;  /* 3D actor + prop render-build (skeletal+emit) */

/* #1a port: cinematic letterbox bar height (px, 320x240 space). 0 = off.
 * Same contract as the PC re15_render_pc_set_letterbox(). */
void re15_render_set_letterbox(int h);

/* Pre-intro (2026-06-03): narrator prologue over black + helipad fade-in.
 * blackout: paint the framebuffer black (skip the BG blit) while the sub11
 * narrator plays. fade: subtractive black-overlay strength 0..255 (0xF0→0 over
 * 15 frames = the canonical FUN_80021a0c ramp) to fade the helipad in. */
void re15_render_set_blackout(int on);
void re15_render_set_fade(int level);

/* Direct-emit (canonical FUN_800254a0): alloc a prim slot in the active buffer
 * (NULL if full) + link it into OT[z]. Lets mesh_psx write XY straight from the
 * GTE (gte_stsxy3_ft3) with no per-tri CPU round-trip. */
void *re15_render_prim_alloc(int bytes);
void  re15_render_prim_link(void *prim, int z);

/* RE2-canonical INLINE per-poly emit (FUN_8002d0e8 / FUN_800254a0): instead of a
 * prim_alloc + prim_link FUNCTION CALL per polygon (×~2000/frame), snapshot the
 * emit cursor once per mesh, then advance the pointer + inline addPrim() directly
 * in the hot loop. emit_begin caches next_packet/buffer-end/OT base; emit_end
 * writes the advanced cursor back. The OT base is `unsigned int *` (PSX OT word
 * entries) — index it by bucket and addPrim(&ot[bucket], p) inline. */
typedef struct { unsigned char *next; unsigned char *end; unsigned int *ot; } re15_emit_t;
void re15_render_emit_begin(re15_emit_t *e);
void re15_render_emit_end(const re15_emit_t *e);

/* Debug text drawing (Phase 4.1) — uses PSn00bSDK FntSort */
void re15_debug_text(int x, int y, int z, const char *text);

/* Phase 4.2: solid-color rectangle primitive (TILE on PSX, fillrect on PC) */
void re15_render_tile(int x, int y, int w, int h, int z,
                      uint8_t r, uint8_t g, uint8_t b);

/* Phase 4.6.0: full-screen vertical gradient background. Called once per
 * frame BEFORE other primitives so all later draws land on top.
 * The PSX implementation emits N horizontal TILE strips into the highest
 * Z bucket (= rendered first). The PC implementation fills the framebuffer
 * with interpolated rows in `re15_render_begin_frame()`-ish style.
 *
 * Phase 4.6.1+ will replace the gradient with MDEC-decoded BSS frames
 * (the actual RE1.5 room backgrounds). */
void re15_render_background_gradient(uint8_t r_top,    uint8_t g_top,    uint8_t b_top,
                                     uint8_t r_bottom, uint8_t g_bottom, uint8_t b_bottom);

/* Phase 4.5: line primitive (LINE_F2 on PSX, Bresenham on PC) */
void re15_render_line(int x0, int y0, int x1, int y1, int z,
                      uint8_t r, uint8_t g, uint8_t b);

/* Phase 4.5.5: textured triangle.
 *   (x0,y0)..(x2,y2)  screen-space vertex positions (after projection)
 *   (u0,v0)..(u2,v2)  texture UVs in the active texture page (0..255 each)
 *   tpage, clut       PSX texture page handle + CLUT handle
 *                     (PC ignores these — uses the loaded TIM directly)
 *   z                 OT depth bucket on PSX, render order on PC
 *   r, g, b           tint colour (128 = neutral, 255 = brighter, 64 = darker)
 *
 * On PSX emits a POLY_FT3 primitive. On PC enqueues an SDL_RenderGeometry call
 * that's flushed in re15_render_end_frame() — so it draws ON TOP of the
 * software framebuffer that holds 2D text/tile/sprite primitives. */
void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                              int x1, int y1, int u1, int v1,
                              int x2, int y2, int u2, int v2,
                              int tpage, int clut, int z,
                              uint8_t r, uint8_t g, uint8_t b);

/* BF-round (2026-05-28): per-vertex tinted variant for NCCT lighting.
 * Each vertex carries its own RGB (computed via re15_light_shade_vertex);
 * SDL_RenderGeometry interpolates colors across the triangle (Gouraud
 * shading). On PSX backend, emit POLY_GT3 with per-vertex setRGB0/1/2.
 *
 * If all 3 colors are equal, semantically equivalent to re15_render_textured_tri. */
void re15_render_textured_tri_lit(int x0, int y0, int u0, int v0,
                                  int x1, int y1, int u1, int v1,
                                  int x2, int y2, int u2, int v2,
                                  int tpage, int clut, int z,
                                  uint8_t r0, uint8_t g0, uint8_t b0,
                                  uint8_t r1, uint8_t g1, uint8_t b1,
                                  uint8_t r2, uint8_t g2, uint8_t b2);

/* RE1.5 character shadow (FUN_8001b064): one subtractive textured floor quad
 * per actor. The 4 screen corners are the projected floor-quad corners
 * (half-extents 500x600 around the actor, at floor Y, yaw-rotated). The PC
 * backend draws the extracted TEX.TIM blob with ABR-mode-2 (subtractive)
 * blending. Order of corners: 0=(-X,+Z) 1=(-X,-Z) 2=(+X,+Z) 3=(+X,-Z). */
void re15_render_shadow_quad(int x0, int y0, int x1, int y1,
                             int x2, int y2, int x3, int y3);

/* Phase 4.3: asset / test sprite (PSX target only) */
void re15_assets_init(void);
void re15_render_test_tim(int x, int y, int z);

/* Phase 4.4: SCD demo bytecode (shared) */
const uint8_t *scd_fallback_bytecode(void);

/* Phase 4.5.2..4.5.5: PSX 3D mesh renderer via GTE.
 *   mesh_psx_init()        — one-time GTE setup, call at boot after re15_render_init
 *   mesh_psx_render_test() — emit MD1 as triangles (wireframe or textured).
 *
 *   Parameters:
 *     z_bucket      OT depth bucket (lower Z = drawn later = on top)
 *     angle_4096    Y-axis rotation, PSX trig convention (4096 = full turn)
 *     pos_x/y/z     world-space translation (Phase 4.5.4)
 *     backface_cull non-zero = drop back-facing polygons via gte_nclip
 *     mode          0 = wireframe (LINE_F2)
 *                   1 = textured triangles (POLY_FT3) using loaded test TIM
 */
#define MESH_PSX_MODE_WIREFRAME  0
#define MESH_PSX_MODE_TEXTURED   1

void mesh_psx_init(void);
void mesh_psx_render_test(int z_bucket, int32_t angle_4096,
                          int32_t pos_x, int32_t pos_y, int32_t pos_z,
                          int backface_cull, int mode);

/* Phase 4.5.8.2: skeletal multi-mesh render with camera. Each MD1 mesh
 * is drawn with `view × bone_world` composed and uploaded to the GTE.
 * pos_x/y/z is the model's WORLD origin (added to bone-world translation
 * before applying view). Caller is responsible for re-running
 * re15_camera_build_view + mesh_psx_set_camera_fov whenever the active
 * cut changes. */
#include "re15_camera.h"
#include "re15_md1.h"   /* re15_md1_t for the skeletal + prop renderers */
#include "re15_emd.h"   /* re15_emd_skeleton_t for the skeletal renderer */
void mesh_psx_render_skeletal(int z_bucket,
                              const re15_camera_view_t *view,
                              const re15_md1_t *md1,
                              const re15_emd_skeleton_t *skel,
                              int32_t pos_x, int32_t pos_y, int32_t pos_z,
                              int16_t face_sin_q12, int16_t face_cos_q12,
                              int backface_cull, int mode,
                              int keyframe_index,
                              int tpage, int clut, int clut_yshift,
                              int tpage_xshift);
/* #2: render a whole MD1 as one rigid prop at world pos+yaw through the
 * camera view (non-skeletal; first consumer = helicopter actor type 0x47). */
void mesh_psx_render_prop(int z_bucket,
                          const re15_camera_view_t *view,
                          const re15_md1_t *md1,
                          int32_t pos_x, int32_t pos_y, int32_t pos_z,
                          int16_t face_sin_q12, int16_t face_cos_q12,
                          int backface_cull, int mode,
                          int tpage, int clut);
void mesh_psx_set_camera_fov(const re15_camera_view_t *view);

/* RE1.5 character floor shadow (FUN_8001b064/af5c): one subtractive (ABR=2)
 * textured POLY_FT4 floor quad — half-extents ±500 X / ±600 Z, centered on the
 * actor world pos at floor Y (wy), rotated by the CAMERA yaw (built from the
 * cut forward fwd_x/fwd_z = target−pos, XZ), projected through the view via the
 * GTE. Needs re15_shadow_ok (blob uploaded). Call per on-stage actor, after the
 * BG and before/with the meshes; the OT depth-sorts it under the character. */
void mesh_psx_render_actor_shadow(int z_bucket, const re15_camera_view_t *view,
                                  int32_t wx, int32_t wy, int32_t wz,
                                  int32_t fwd_x, int32_t fwd_z);
extern int re15_shadow_tpage;   /* getTPage(2,2,..) — 16bpp + ABR2 subtractive */
extern int re15_shadow_ok;

/* RE1.5 subtitle TEXT system (FUN_80028868): render a RAW .msg body with the real
 * TEX.TIM glyph font — control codes 0x05 (per-speaker CLUT colour) + 0x08 (newline)
 * honoured, per-glyph advance from the DEBUG.BIN width table. Box origin (34,180). */
void re15_render_msg_text(int x, int y, const unsigned char *raw, int len);
/* YES/NO dialog cursor: a small right-pointing '>' triangle at (x,y). */
void re15_render_msg_cursor(int x, int y);
/* Dialog page-break down-arrow (FUN_80028134 state 2 "press for next page"). */
void re15_render_msg_down_arrow(int x, int y);
extern int re15_font_tpage;       /* 4bpp glyph-font tpage (getTPage(0,0,..)) */
extern int re15_font_ok;
extern int re15_font_clut_id[8];  /* clut id per 0x05 speaker attribute 0..7 */

#endif
