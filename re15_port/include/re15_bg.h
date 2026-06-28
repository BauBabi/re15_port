/*
 * RE1.5 Rebuilt — Background-frame subsystem (Phase 4.5.6.3, 2026-05-18).
 *
 * RE2-faithful BG pipeline:
 *   1. Boot:    decode one BSS chunk (MDEC) → VRAM cache region.
 *   2. Every    frame: MoveImage from VRAM cache → active framebuffer
 *      BEFORE   the OT walk paints meshes/HUD on top.
 *
 * This mirrors RE2's FUN_8002bdf4 (initial decode) + FUN_8002b968 (re-blit
 * each frame) split. On PSX the heavy lifting is the MDEC chip via
 * libpsxpress. On PC we'll port the Java software IDCT in Phase 4.5.6.4.
 *
 * Cross-platform API — same signatures, two backends. The PC backend is
 * a stub in 4.5.6.3 (returns "not loaded") and gets the real software
 * IDCT in 4.5.6.4.
 */
#ifndef RE15_BG_H
#define RE15_BG_H

#include <stddef.h>
#include <stdint.h>

/* One-time init. PSX backend calls DecDCTReset(0). Must be called AFTER
 * mesh_psx_init() because DecDCTvlc() requires the GTE to be enabled. */
void re15_bg_init(void);

/* Decode an entire BSS chunk and cache the resulting 320x240 frame.
 * Returns 0 on success, negative on error:
 *   -1: chunk parse failed (truncated / NULL)
 *   -2: chunk has no video data (id != VLC_ID — e.g. zero-padded tail)
 *   -3: MDEC decode failed (corrupt bitstream)
 *   -100: PC stub (not implemented yet — Phase 4.5.6.4) */
int re15_bg_load_from_bss(const uint8_t *bss_chunk, size_t size);

/* Convenience entry point — load the bundled test asset. On PSX the
 * test BSS is incbin'd into the EXE; on PC the asset loader reads from
 * disk. Both eventually call re15_bg_load_from_bss(). */
int re15_bg_load_test_asset(void);

/* Phase 4.5.10: load the BG frame matching a specific camera cut.
 * Indexed 0..12 (13 cuts). Reuses re15_bg_load_from_bss() after fetching
 * the right asset (PSX: from incbin table; PC: from disk path
 * <room_prefix>_bgNN.bss). Returns 0 on success, negative on failure. */
int re15_bg_load_cut(int cut_idx);   /* room-aware (g_current_room_id), any room */

/* Multi-room: discard the cached BG so the next per-cut load re-decodes
 * (call on room-enter; the resident frame belongs to the previous room). */
void re15_bg_invalidate(void);

/* Per-frame: blit the cached BG to (dst_x, dst_y) in VRAM (which is the
 * active framebuffer's origin). No-op if no BG is loaded.
 *
 * PSX:  MoveImage(cache_rect, dst_x, dst_y) — a VRAM-to-VRAM copy via
 *       the GPU (single GP0(0x80) command).
 * PC:   stub in 4.5.6.3; SDL_RenderCopy in 4.5.6.4. */
void re15_bg_blit(int dst_x, int dst_y);

/* Loaded-state query — render.c uses this to decide whether to skip
 * the gradient fallback. */
int re15_bg_is_loaded(void);

/* AZ-round 2026-05-28: sprite.pri foreground-occluder patch.
 * Re-blit a rectangle of the cached BG image onto the framebuffer to
 * overdraw characters where they should be hidden behind BG geometry
 * (railings, pillars, boxes, etc). Called after character render with
 * mask data from re15_pri_parse_section().
 *
 *   src_x/y  = sample pixel in BG image (320×240, top-left origin)
 *   dst_x/y  = framebuffer screen pixel
 *   w, h     = patch dimensions (square or rect from mask)
 *
 * No-op if BG not loaded or rect fully off-screen. */
void re15_bg_blit_pri_patch(int src_x, int src_y,
                            int dst_x, int dst_y,
                            int w, int h);

#endif /* RE15_BG_H */
