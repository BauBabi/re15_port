/*
 * RE1.5 Rebuilt — sprite.pri FOREGROUND ATLAS, PSX backend (2026-06-09).
 *
 * Byte-true port of the original foreground-occlusion source: each camera cut that
 * has foreground occluders (railings, boxes, light fixtures) carries an 8-bit atlas
 * (our BSS-SLD decode — SldDecoder/FUN_800c47e8, VRAM-verified byte-exact). On
 * cut-change we CD-load that cut's atlas TIM and upload it to a free VRAM slot; the
 * sprite.pri SPRTs (emitted in render.c, byte-true FUN_800392d4/FUN_80039590) sample
 * it so the foreground occludes the actors.
 *
 * VRAM placement (our layout — the original used (320,256)/(0,480), which here is
 * the heli prop + CLUT band): image at (384,256), CLUT at (384,490). Both sit in the
 * free region (384..640, 256..496) — RIGHT of the heli (320..384), BELOW the obj
 * props (y<256), LEFT of the BG cache (x>=640). The ROOM1170 masks only sample
 * srcY<=88, so 128 uploaded rows is ample.
 */
#include <stdint.h>
#include <stdio.h>
#include <psxgpu.h>
#include "re15_cdfs.h"
#include "re15_room.h"
#include "re15_tim.h"

/* The obj-prop bump allocator (asset_psx.c) packs TWO 256-tall slots per 64-wide
 * column from x384, so the 5 ROOM1170 props (obj 0/1/3/4/5; obj2=heli is separate)
 * fill (384,0),(384,256),(448,0),(448,256),(512,0) — leaving the 6th slot (512,256)
 * UNUSED and x576..640 untouched. So the atlas image lands at (512,256) in that free
 * (512..640, 256..512) block; CLUT at (0,502) (below the framebuffers at y<480 and the
 * obj/leon/elliot/heli CLUT band at y480..491). An EARLIER (384,256) placement
 * overwrote the 2nd prop row = obj01 = the helipad box → "box top broken". (Multi-room
 * caveat: a 6-prop room would claim (512,256) → reserve a dedicated atlas slot then.) */
#define RE15_PRI_VRAM_X    512
#define RE15_PRI_VRAM_Y    256
#define RE15_PRI_VRAM_ROWS 128
#define RE15_PRI_CLUT_X    0
#define RE15_PRI_CLUT_Y    502

/* Read by render.c's re15_render_pri_sprites(). */
int      re15_pri_psx_ok    = 0;   /* 1 = current cut has a loaded foreground atlas */
uint16_t re15_pri_psx_tpage = 0;   /* 8-bit tpage handle for the atlas              */
uint16_t re15_pri_psx_clut  = 0;   /* CLUT handle for the atlas                      */

/* CD-load + upload the cut's foreground atlas (\BSS\ROOM####\PRI##.TIM). Returns 1
 * if the cut has a foreground atlas (overdraw on), 0 otherwise. Idempotent per cut. */
int re15_pri_psx_load_cut(int cut_idx)
{
    re15_pri_psx_ok = 0;
    if (cut_idx < 0) return 0;

    char name[40];
    sprintf(name, "\\BSS\\ROOM%04X\\PRI%02d.TIM;1", g_current_room_id, cut_idx);
    int n = re15_cd_load_file(name, re15_cd_staging, RE15_CD_STAGING_SIZE);
    if (n <= 0) return 0;                       /* this cut has no foreground */

    re15_tim_t t;
    if (re15_tim_parse(re15_cd_staging, n, &t) != 0 || t.bpp != 8 || !t.has_clut) return 0;

    int rows = (t.height < RE15_PRI_VRAM_ROWS) ? t.height : RE15_PRI_VRAM_ROWS;
    RECT pr = { RE15_PRI_VRAM_X, RE15_PRI_VRAM_Y, (short)(t.width / 2), (short)rows };
    LoadImage(&pr, (const uint32_t *) t.pixels);
    RECT cr = { RE15_PRI_CLUT_X, RE15_PRI_CLUT_Y, 256, 1 };
    LoadImage(&cr, (const uint32_t *) t.clut);
    DrawSync(0);

    re15_pri_psx_tpage = getTPage(1, 0, RE15_PRI_VRAM_X, RE15_PRI_VRAM_Y);
    re15_pri_psx_clut  = getClut(RE15_PRI_CLUT_X, RE15_PRI_CLUT_Y);
    re15_pri_psx_ok    = 1;
    return 1;
}
