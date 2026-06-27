/*
 * RE1.5 Rebuilt — structured PSX VRAM allocator for DYNAMIC 8bpp textures.
 *
 * Replaces the ad-hoc per-texture hardcoded VRAM X/Y + CLUT rows (which forced a
 * manual "is this slot free?" check for every new room/enemy). The PERSISTENT,
 * truly-fixed regions are NOT managed and stay where they are:
 *   - display framebuffers (0,0)-(320,480)
 *   - subtitle/debug fonts, BG-cache (640..960,256..496), shadow, sprite.pri atlas
 *   - the player Leon TIM (tex 640..832 / CLUT rows 480..482)
 * Everything PER-ROOM (object props, NPCs like Elliot, enemies like em21, …) asks
 * this allocator for a slot. It hands out non-colliding 64×256 texture pages + CLUT
 * rows from a free pool and tracks them; re15_vram_reset_room() reclaims them on
 * room-enter so the next room's set re-packs from scratch. One mechanism, always
 * collision-free, scales to any room.
 */
#ifndef RE15_VRAM_H
#define RE15_VRAM_H

/* Reclaim ALL per-room texture slots + CLUT rows (call once at boot and on every
 * room-enter, BEFORE loading that room's props/NPCs/enemies). */
void re15_vram_reset_room(void);

/* Allocate a texture slot for an 8bpp image `px_w` pixels wide × up to 256 rows
 * tall (rounded up to 64-col VRAM pages). On success writes the VRAM top-left to
 * *out_x/*out_y and returns 1; returns 0 if the pool is exhausted (out unchanged).
 *
 * `need_y0`: pass 1 for textures rendered via PER-TRI baked tpage + an X-shift (the
 * baked tpage encodes the Y-half, so the slot MUST be at VRAM Y=0 — e.g. Elliot's
 * 2-page body/face texture, em21). Pass 0 for textures drawn with a SINGLE tpage
 * OVERRIDE (any Y is fine — e.g. obj props, the heli); these prefer the Y=256 row so
 * the scarce Y=0 row stays free for the per-tri ones. */
int  re15_vram_alloc_tex(int px_w, int need_y0, int *out_x, int *out_y);

/* Allocate `n_rows` consecutive 256-entry CLUT rows at VRAM X0. Returns the first
 * row's Y, or -1 if the CLUT band is full. */
int  re15_vram_alloc_clut(int n_rows);

#endif /* RE15_VRAM_H */
