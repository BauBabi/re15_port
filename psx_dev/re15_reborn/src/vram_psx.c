/*
 * RE1.5 Rebuilt — structured PSX VRAM allocator (see re15_vram.h). PSX-only:
 * it hands out concrete VRAM (x,y) coordinates for the GPU. The PC backend has
 * its own decoded-texture model and does not need this.
 */
#include "re15_vram.h"

/* The free pool of 64-wide × 256-tall dynamic texture slots. Deliberately EXCLUDES
 * the persistent/fixed regions:
 *   X 0..320   = display framebuffers        X 320..384 / 960..1024 = fonts
 *   X 640..832 = Leon (player) TIM           (320,256)=heli  (512,256)=sprite.pri atlas
 *   X 640..960 Y256 = BG cache               (960,256)=shadow
 * What's left for per-room props / NPCs / enemies (each 64×256 = one tpage):
 *   Y0:   384, 448, 512, 576, 832, 896
 *   Y256: 384, 448, 576           (512,256 is the sprite.pri atlas; 640+ is BG)
 * 9 slots — room1170's set (5 obj props + Elliot's 2 + em21's 1 = 8) fits with 1 spare.
 * Add rows/columns here as more VRAM is reclaimed for heavier rooms. */
static const struct { short x, y; } s_pool[] = {
    {384,0},{448,0},{512,0},{576,0},{832,0},{896,0},   /* Y=0: per-tri (Elliot/em21) + override spill */
    {384,256},{448,256},                                /* Y=256: override only. NOTE (512,256)+(576,256)
                                                         * are the sprite.pri atlas (128 cols, 512..640) — excluded. */
};
/* room1170's resident set = 5 obj props (1 page) + Elliot (2) + em21 (1) = 8 = exactly
 * these 8 slots. Heavier rooms (more enemies) will need VRAM reclaimed — e.g. free the
 * cinematic-only textures (Elliot, heli) once the cinematic ends so their slots re-open. */
#define POOL_N ((int)(sizeof(s_pool)/sizeof(s_pool[0])))

static unsigned char s_used[POOL_N];   /* 0 = free, 1 = claimed this room */

/* CLUT band at VRAM X0: Leon's CLUT is the persistent 480..482, the font CLUT lives
 * at X256 (different column, no conflict), and the sprite.pri atlas CLUT is at row
 * 502. So dynamic CLUTs get rows 483..501. */
#define CLUT_FIRST_ROW  483
#define CLUT_LAST_ROW   501
static int s_clut_next = CLUT_FIRST_ROW;

void re15_vram_reset_room(void)
{
    for (int i = 0; i < POOL_N; i++) s_used[i] = 0;
    s_clut_next = CLUT_FIRST_ROW;
}

/* Find the pool index of a free slot at exactly (x,y), or -1. */
static int slot_at(int x, int y)
{
    for (int i = 0; i < POOL_N; i++)
        if (!s_used[i] && s_pool[i].x == x && s_pool[i].y == y) return i;
    return -1;
}

int re15_vram_alloc_tex(int px_w, int need_y0, int *out_x, int *out_y)
{
    int cols  = (px_w + 1) / 2;          /* 8bpp: 2 pixels per 16-bit VRAM column */
    int pages = (cols + 63) / 64;        /* 64-col tpage granularity */
    if (pages < 1) pages = 1;

    /* Pass order: per-tri textures take Y=0 only; override textures try Y=256 first
     * (keeping Y=0 free for per-tri) then fall back to Y=0. */
    int passes  = need_y0 ? 1 : 2;
    int first_y = need_y0 ? 0 : 256;
    for (int pass = 0; pass < passes; pass++) {
        int want_y = (pass == 0) ? first_y : 256 - first_y;
        for (int i = 0; i < POOL_N; i++) {
            if (s_used[i] || s_pool[i].y != want_y) continue;
            int ok = 1;                  /* need `pages` consecutive free in this row */
            for (int p = 1; p < pages && ok; p++)
                if (slot_at(s_pool[i].x + p * 64, want_y) < 0) ok = 0;
            if (!ok) continue;
            if (out_x) *out_x = s_pool[i].x;
            if (out_y) *out_y = s_pool[i].y;
            for (int p = 0; p < pages; p++) {
                int j = slot_at(s_pool[i].x + p * 64, want_y);
                if (j >= 0) s_used[j] = 1;
            }
            return 1;
        }
    }
    return 0;   /* pool exhausted */
}

int re15_vram_alloc_clut(int n_rows)
{
    if (n_rows < 1) n_rows = 1;
    if (s_clut_next + n_rows - 1 > CLUT_LAST_ROW) return -1;
    int y = s_clut_next;
    s_clut_next += n_rows;
    return y;
}
