/*
 * RE1.5 Rebuilt — Rendering (Phase 4.1).
 *
 * Double-buffered framebuffer + OT (ordering table) for GPU primitive sort.
 * Adapted from PSn00bSDK hello example with re15-specific naming.
 *
 * Phase 4.1: just init + clear + present.
 * Phase 4.2 will add: 2D primitive helpers, sprite atlas, font upload.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <psxgpu.h>
#include <psxapi.h>   /* GetRCnt/SetRCnt/StartRCnt — hblank profiler */
#include "re15_engine.h"
#include "font_width.h"   /* RE1.5 per-glyph advance table (DEBUG.BIN[0x4416]) */
#include "re15_bg.h"   /* Phase 4.5.6.3: BG blit before OT walk */
#include "re15_pri.h"  /* sprite.pri foreground-occlusion masks (FUN_800392d4) */
#include "re15_msg.h"  /* shared .msg text layout walk (re15_msg_layout) */

/* OT_LENGTH = 1024 matches RE2 retail (RE2_Quellcode/main.c:23
 * ClearOTagR(DAT_800ce22c, 0x400)). With ~8 OTZ units per bucket this
 * gives per-primitive depth resolution fine enough that 15 bones at
 * varying screen depths can each land in their own bucket. Previous
 * value (16) was copied from PSn00bSDK's 2D `hello` example and
 * collapsed every bone's primitives into ~2 buckets, causing painter-
 * algorithm order to be FIFO-undefined → arm-through-torso artifacts.
 * RAM cost: 1024 × 4 B × 2 double-buffers = 8 KB. Trivial. */
#define OT_LENGTH      1024
/* BUFFER_LENGTH (Phase 4.6.0): increased from 8192 → 32768.
 *
 * Original size was the PSn00bSDK hello-world default; it's plenty for a
 * bouncing square + a few text lines but too tight for a full MD1 mesh
 * (50 quads × 72B textured + 20 tris × 36B = ~4.3 KB) plus a 30-strip
 * gradient background (480B) plus 10+ debug-text lines via FntSort
 * (~2-4 KB worst case).
 *
 * Symptom of overflow: next_packet walks past buffer[] into the NEXT
 * render_buffer_t in the array (OT and DRAWENV get corrupted), which
 * manifests as the mesh flickering invisible on alternating frames as
 * the active-buffer swaps and corruption persists.
 *
 * 32 KB is generous but well under PSX's 2 MB RAM. The two render_buffer_t
 * instances eat ~66 KB total now — fine.
 *
 * Phase 4.7+ TODO: replace static buffer with bounds-checked allocator
 * that emits a primitive-count diagnostic to screen if approaching limit. */
/* 2026-06-01 (audit #42): was 32768 — Leon ALONE (196 tris + 276 quads×2 prims)
 * exceeds 32KB → has_room_for silently DROPPED geometry. BUT 256KB×2 buffers =
 * 512KB BSS overflowed the ~141KB RAM headroom (audit #4: only ~141KB free above
 * _end) → black screen. 64KB×2 = 128KB fits (~81KB headroom remains) and, with
 * the FT3-quad win (#3), holds a typical frame. Full no-drop sizing needs the
 * RAM-reclaim (free the ~358KB dead incbin TIM copies) — tracked separately. */
#define BUFFER_LENGTH  0x10000

typedef struct {
    DISPENV  disp_env;
    DRAWENV  draw_env;
    uint32_t ot[OT_LENGTH];
    uint8_t  buffer[BUFFER_LENGTH];
} render_buffer_t;

typedef struct {
    render_buffer_t buffers[2];
    uint8_t        *next_packet;
    int             active_buffer;
} render_ctx_t;

static render_ctx_t s_ctx;

/* #1a port (2026-06-01): cinematic letterbox bar height in 320x240 space.
 * 0 = off (gameplay), 24 = canonical PSX bar height (FUN_80020f8c POLY_F4
 * y2 = 0x18). Driven each frame from the shared g_scd letterbox FSM. */
static int s_letterbox_h = 0;

/* PERF PROFILER readout string (file scope: computed in end_frame, drawn in
 * begin_frame when the packet buffer is freshly cleared so the has-room guard
 * in re15_debug_text never silently drops it on a buffer-full heavy frame).
 * Set RE15_PROFILE_HUD to 0 to hide the on-screen readout (the cheap RCnt
 * timing stays — only the text draw is gated). Flip to 1 to re-enable the
 * perf overlay during fps work (readout: F<min>-<avg> c<cpu> lg<logic>
 * rb<render-build> gw<gpu-wait>, in ~63.6µs hblank ticks; 526 = 30fps cap). */
#define RE15_PROFILE_HUD 0
static char s_pf_line[64] = "perf: warming up";

/* Intra-frame sub-phase timers (hblank ticks) written by main.c each frame via
 * GetRCnt brackets: logic = SCD+walker+FSM+audio; rbuild = the 3D actor+prop
 * render-build (skeletal pose + projection + prim emit, the suspected hot CPU
 * phase). The profiler in end_frame averages them into the on-screen readout so
 * we pin which CPU phase dominates the ~39 ms full-cast frame before optimising
 * the fragile skeletal path. Zeroed each frame by main.c. */
int g_re15_pf_logic  = 0;
int g_re15_pf_rbuild = 0;

void re15_render_set_letterbox(int h)
{
    if (h < 0) h = 0;
    if (h > SCREEN_YRES / 2) h = SCREEN_YRES / 2;
    s_letterbox_h = h;
}

/* Pre-intro / fade state (2026-06-03). The intro opens on the narrator prologue
 * ("We barricaded ourselves inside the police station…", ablauf3 frame 0) over a
 * BLACK screen, then the helipad FADES IN from black. */
static int s_blackout = 0;   /* 1 = paint the framebuffer black (skip the BG blit) */
static int s_fade_lvl = 0;   /* 0..255: subtractive black overlay strength (0 = off) */

static int has_room_for(int bytes_needed);   /* defined below; used by end_frame's fade quad */

/* Hold the framebuffer fully black (narrator-over-black prologue). */
void re15_render_set_blackout(int on) { s_blackout = on ? 1 : 0; }

/* Cinematic fade-from-black level (PSX FUN_80021a0c DAT_800b5568): 0xF0 = full
 * black, 0 = full scene. Ramped 0xF0→0 at 0x10/frame (15 steps) by the caller. */
void re15_render_set_fade(int level)
{
    if (level < 0)   level = 0;
    if (level > 255) level = 255;
    s_fade_lvl = level;
}

void re15_render_init(void)
{
    ResetGraph(0);
    FntLoad(960, 0);
    /* Phase 4.5.9-C: explicit FntOpen so HUD text uses the full 320-px
     * width. PSn00bSDK's implicit default is ~256-px wide which truncates
     * lines like "MD1: 17 meshes 796 verts 196 tris 276 quads" off the
     * right edge. (8, 8) margin keeps text out of the overscan border. */
    FntOpen(8, 8, SCREEN_XRES - 8, SCREEN_YRES - 8, 0, 512);

    /* PERF PROFILER (2026-06-02b): free-running hblank root counter (RCntCNT1,
     * NTSC ~15.7 kHz → 1 tick ≈ 63.6 µs, 16-bit wrap ≈ 4.2 s — deltas over a
     * 33 ms frame (~524 ticks) never approach wrap). Used in end_frame to split
     * the frame budget into CPU-build vs GPU-wait (DrawSync block) vs pacing pad,
     * so the 30 fps push is measured, not guessed. RCntCNT1 is independent of
     * VSync (which runs on the vblank IRQ), so it cannot perturb pacing. */
    SetRCnt(RCntCNT1, 0xffff, RCntMdNOINTR);
    StartRCnt(RCntCNT1);

    int w = SCREEN_XRES;
    int h = SCREEN_YRES;

    SetDefDrawEnv(&s_ctx.buffers[0].draw_env, 0, 0, w, h);
    SetDefDispEnv(&s_ctx.buffers[0].disp_env, 0, 0, w, h);
    SetDefDrawEnv(&s_ctx.buffers[1].draw_env, 0, h, w, h);
    SetDefDispEnv(&s_ctx.buffers[1].disp_env, 0, h, w, h);

    /* Phase 4.5.6.3: isbg=0 — DrawOTagEnv will NOT clear the framebuffer.
     * Instead each frame we MoveImage the cached BG (decoded MDEC frame
     * from VRAM cache region) into the active framebuffer BEFORE the OT
     * walk runs. This is RE2's background pipeline (FUN_8002b968):
     * a single GP0(0x80) CopyVramToVram per frame, no clear, full-screen
     * BG overpaints any stale pixels. The dark-blue clear color stays
     * configured as a fallback for boot frames before the BG loads. */
    setRGB0(&s_ctx.buffers[0].draw_env, 8, 16, 48);
    setRGB0(&s_ctx.buffers[1].draw_env, 8, 16, 48);
    s_ctx.buffers[0].draw_env.isbg = 0;
    s_ctx.buffers[1].draw_env.isbg = 0;

    s_ctx.active_buffer = 0;
    s_ctx.next_packet   = s_ctx.buffers[0].buffer;
    /* Clear BOTH OTs at init — without this, the first time we flip to
     * buffer 1 we'd walk uninitialised memory as the OT (or rely on the
     * end_frame ClearOTagR happening before DrawOTagEnv on buffer 1's
     * first use, which is fragile). PSn00bSDK's n00bdemo disp.c always
     * clears both. */
    ClearOTagR(s_ctx.buffers[0].ot, OT_LENGTH);
    ClearOTagR(s_ctx.buffers[1].ot, OT_LENGTH);

    SetDispMask(1);
}

void re15_render_begin_frame(void)
{
#if RE15_PROFILE_HUD
    /* PERF readout: emit NOW while next_packet sits at the freshly-cleared
     * buffer start (end_frame did ClearOTagR + reset next_packet). Drawn at
     * z=0 (front); the heavy mesh emit that follows appends after it. */
    re15_debug_text(8, 8, 0, s_pf_line);
#endif
}

void re15_render_end_frame(void)
{
    /* --- PERF PROFILER state (hblank ticks; see re15_render_init). Splits the
     * frame into CPU-build (main.c since last VSync(2)) | GPU-wait (DrawSync
     * block) | post (bg_blit+text+submit) | pacing pad → CPU- vs GPU-bound. --- */
    static uint16_t s_pf_last = 0;
    static int  s_pf_n = 0, s_pf_cpu = 0, s_pf_gpu = 0, s_pf_post = 0, s_pf_per = 0;
    static int  s_pf_per_mx = 0, s_pf_lg = 0, s_pf_rb = 0;

    uint16_t t0 = GetRCnt(RCntCNT1);   /* main.c scene-build for this frame done */

    /* DrawSync(0) waits for the GPU to finish the PREVIOUS frame's OT before we
     * flip the display + resubmit. Kept BEFORE the swap (moving it past PutDispEnv
     * would tear). The 30Hz VSync(2) pacing wait is now done AFTER submission
     * (see below) so the GPU draws this frame DURING the wait — CPU/GPU overlap. */
    DrawSync(0);
    uint16_t t1 = GetRCnt(RCntCNT1);   /* + GPU-overrun block (0 if GPU finished) */

    render_buffer_t *draw_buf = &s_ctx.buffers[s_ctx.active_buffer];
    render_buffer_t *disp_buf = &s_ctx.buffers[s_ctx.active_buffer ^ 1];

    PutDispEnv(&disp_buf->disp_env);

    /* Phase 4.5.6.3: blit the cached MDEC frame into the active draw
     * buffer's framebuffer BEFORE the OT walk paints meshes/HUD on top.
     * draw_env.clip.{x,y} is the framebuffer's VRAM origin (0,0 or
     * 0,240 depending on which double-buffer half is active).
     *
     * No-op until re15_bg_load_*() succeeds, which on PSX happens at
     * boot once mesh_psx_init() has initialised the GTE. */
    if (s_blackout) {
        /* Pre-intro narrator: the framebuffer is NOT auto-cleared (isbg=0) and we
         * skip the BG blit, so cover the stale pixels with a full-screen black TILE
         * at the backmost OT bucket. The narrator text (added to ot[0] in the main
         * loop) then paints on top → white text over pure black (ablauf3 frame 0). */
        re15_render_tile(0, 0, SCREEN_XRES, SCREEN_YRES, OT_LENGTH - 2, 0, 0, 0);
    } else {
        re15_bg_blit(draw_buf->draw_env.clip.x, draw_buf->draw_env.clip.y);
    }

    /* #1a port: cinematic letterbox — two black bars over BG + 3D (+ HUD).
     * Emitted into the frontmost OT bucket (z=0) of the draw buffer right
     * before the walk, so they paint last over the game. The active buffer
     * has NOT flipped yet, so re15_render_tile targets this draw buffer.
     * PSX FUN_80020f8c uses POLY_F4; flat black TILEs are equivalent here. */
    if (s_letterbox_h > 0) {
        re15_render_tile(0, 0, SCREEN_XRES, s_letterbox_h, 0, 0, 0, 0);
        re15_render_tile(0, SCREEN_YRES - s_letterbox_h, SCREEN_XRES,
                         s_letterbox_h, 0, 0, 0, 0);
    }

    /* Cinematic fade-from-black (PSX FUN_80021a0c): a full-screen SUBTRACTIVE
     * (ABR=2) POLY_F4 of grey (lvl,lvl,lvl) → framebuffer MINUS lvl. At lvl 0xF0
     * the scene goes black; at 0 it's the full scene. Added here (after the
     * letterbox, before the OT submit) so in ot[0] draw-order it paints over the
     * BG+3D in the framebuffer but UNDER the subtitle text (text was added to
     * ot[0] earlier in the main loop → it draws last/on top). */
    if (s_fade_lvl > 0 && has_room_for(sizeof(POLY_F4) + sizeof(DR_TPAGE))) {
        POLY_F4 *q = (POLY_F4 *) s_ctx.next_packet;
        s_ctx.next_packet += sizeof(POLY_F4);
        setPolyF4(q);
        setSemiTrans(q, 1);
        setRGB0(q, (uint8_t)s_fade_lvl, (uint8_t)s_fade_lvl, (uint8_t)s_fade_lvl);
        setXY4(q, 0, 0, SCREEN_XRES, 0, 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
        addPrim(&draw_buf->ot[0], q);
        DR_TPAGE *tp = (DR_TPAGE *) s_ctx.next_packet;
        s_ctx.next_packet += sizeof(DR_TPAGE);
        setDrawTPage(tp, 0, 1, getTPage(0, 2 /* ABR=2 = subtractive */, 0, 0));
        addPrim(&draw_buf->ot[0], tp);
    }

    /* Submit this frame's OT to the GPU NOW (DrawOTagEnv → _addque2 is
     * NON-BLOCKING in PSn00bSDK: it kicks the OT DMA and returns). RE2 mirror:
     * RE2_Quellcode/main.c:45-46,135-136 (submit, then the VSync gate is last). */
    DrawOTagEnv(&draw_buf->ot[OT_LENGTH - 1], &draw_buf->draw_env);

    s_ctx.active_buffer ^= 1;
    s_ctx.next_packet    = disp_buf->buffer;
    ClearOTagR(disp_buf->ot, OT_LENGTH);

    uint16_t t2 = GetRCnt(RCntCNT1);   /* this frame fully submitted (CPU done) */

    /* Canonical 30Hz pacing (RE'd: RE1.5 runs at 30fps). VSync(2) waits 2 vblank
     * fields → caps the loop at 30Hz. Done AFTER DrawOTagEnv so the GPU walks
     * this frame's OT during the wait instead of the CPU idling first → recovers
     * a full frame of CPU/GPU pipeline overlap (Step 1 of the RE2 resource
     * roadmap, 2026-06-02). On render-bound frames (>2 fields) this is a no-op. */
    VSync(2);
    uint16_t t3 = GetRCnt(RCntCNT1);   /* after the 30 Hz pacing pad */

    /* --- accumulate + summarise every 30 frames (hblank ticks, wrap-safe) --- */
    int cpu  = (uint16_t)(t0 - s_pf_last);  /* main.c scene build              */
    int gpuw = (uint16_t)(t1 - t0);         /* DrawSync(0) GPU-overrun block   */
    int post = (uint16_t)(t2 - t1);         /* bg_blit + text + submit kick    */
    int per  = (uint16_t)(t3 - s_pf_last);  /* full frame period (incl. pad)   */
    s_pf_last = t3;
    s_pf_cpu += cpu; s_pf_gpu += gpuw; s_pf_post += post; s_pf_per += per;
    s_pf_lg  += g_re15_pf_logic; s_pf_rb += g_re15_pf_rbuild;
    if (per > s_pf_per_mx) s_pf_per_mx = per;
    if (++s_pf_n >= 30) {
        int perA = s_pf_per / 30;
        /* NTSC ~15734 hblank/s; VSync(2) caps the period at ~524 ticks (=30fps).
         * fps = ticks/sec ÷ period; clamp to 30 (the pacing ceiling). 'per' is
         * avg period, 'mx' the worst (slowest) frame in the window → its min fps. */
        int fps   = perA        > 0 ? 15734 / perA        : 30;
        int fpsMn = s_pf_per_mx > 0 ? 15734 / s_pf_per_mx : 30;
        if (fps   > 30) fps   = 30;
        if (fpsMn > 30) fpsMn = 30;
        /* c=full CPU build, lg=logic(SCD+walker+FSM+audio), rb=3D render-build
         * (skeletal pose + per-poly emit, the measured 99% hot phase), gw=GPU-wait.
         * c-(lg+rb) = camera/bg/subtitle/overhead. */
        sprintf(s_pf_line, "F%2d-%2d c%3d lg%3d rb%3d gw%3d",
                fpsMn, fps, s_pf_cpu / 30, s_pf_lg / 30, s_pf_rb / 30, s_pf_gpu / 30);
        s_pf_n = 0; s_pf_cpu = s_pf_gpu = s_pf_post = s_pf_per = 0;
        s_pf_per_mx = 0; s_pf_lg = 0; s_pf_rb = 0;
    }

    /* Re-assert display mask every frame. PSn00bSDK's n00bdemo disp.c does
     * this in its render loop — if any subsystem ever calls SetDispMask(0)
     * (e.g. during a CD load) we automatically recover next frame instead
     * of staying blank. Cheap insurance against display-blank classes of
     * bug (one of the suspected sources of the recurring pixel-shift). */
    SetDispMask(1);
}

void re15_debug_text(int x, int y, int z, const char *text)
{
    /* Phase 4.5.13-B3 (2026-05-21): clamp z and bound next_packet write.
     * FntSort can emit up to ~400 B per long line — tightened margin
     * from 256 to 512 (B5 round). Without a guard it walks past
     * BUFFER_LENGTH and corrupts the OT[] of the NEXT render_buffer,
     * which causes every subsequent DrawOTagEnv to walk garbage → permanent
     * blackscreen. Bug found via 8-agent forensic during PSX cinematic
     * hang chase. */
    if (z < 0 || z >= OT_LENGTH) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    uint8_t *buf_end = buf->buffer + BUFFER_LENGTH;
    if (s_ctx.next_packet + 512 + 16 > buf_end) return;
    uint8_t *after = (uint8_t *)
        FntSort(&buf->ot[z], s_ctx.next_packet, x, y, text);
    /* Hard cap: refuse to advance past the buffer end even if FntSort
     * already wrote more than expected. */
    if (after > buf_end) return;
    s_ctx.next_packet = after;
}

/* RE1.5 subtitle TEXT renderer (FUN_80028868). The control-code walk is the SHARED
 * re15_msg_layout (msg_common.c, identical on both ports); the per-glyph emission is
 * this port's psx_msg_glyph_cb below — a 16×16 SPRT (NOT a POLY_FT4: a sprite blits
 * 1:1 with no UV interpolation, fixing the old "stretched" glyphs) from the relocated
 * TEX.TIM 4bpp font. Per-glyph advance = re15_font_width (DEBUG.BIN table); glyph cell
 * U=(code&0xF)<<4, V=(code&0xF0); CLUT 0 = 0x0000 → transparent background; z=0 (front). */
static int has_room_for(int bytes_needed);   /* defined below */

/* Per-glyph emission callback for the shared re15_msg_layout walk (PSX SPRT path).
 * RE-faithful glyph: a 16x16 SPRT (GPU sprite, cmd 0x64) exactly like the original
 * FUN_80028868 (`*puVar9 = 0x100010` = W16/H16, UV start (u,v)). A sprite blits texels
 * 1:1 with NO UV interpolation — unlike a POLY_FT4 whose (u..u+15)->(x..x+16) affine
 * map sampled only 15 texels stretched over 16 px (the old "stretched" text). Per-glyph
 * DR_TPAGE re-asserts the font page (sprites carry no tpage field). */
static void psx_msg_glyph_cb(int penx, int peny, unsigned char b, int attr, void *ctx)
{
    (void) ctx;
    if (b > 0x5F) return;                                      /* outside our 6-row atlas */
    if (!has_room_for(sizeof(SPRT) + sizeof(DR_TPAGE))) return;
    int u = (b & 0x0F) << 4, v = (b & 0xF0);
    SPRT *sp = (SPRT *) s_ctx.next_packet;
    s_ctx.next_packet += sizeof(SPRT);
    setSprt(sp);
    setXY0 (sp, (short)penx, (short)peny);
    setWH  (sp, 16, 16);
    setUV0 (sp, (uint8_t)u, (uint8_t)v);
    sp->clut = (uint16_t) re15_font_clut_id[attr & 7];
    setRGB0(sp, 128, 128, 128);                                /* neutral modulation */
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[0], sp);

    DR_TPAGE *tp = (DR_TPAGE *) s_ctx.next_packet;
    s_ctx.next_packet += sizeof(DR_TPAGE);
    setDrawTPage(tp, 0, 1, re15_font_tpage);
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[0], tp);
}
static int psx_msg_width_cb(unsigned char b, void *ctx) { (void) ctx; return re15_font_width[b]; }

void re15_render_msg_text(int x, int y, const unsigned char *raw, int len)
{
    if (!re15_font_ok) return;
    re15_msg_layout(x, y, raw, len, psx_msg_glyph_cb, psx_msg_width_cb, NULL);
}

/* YES/NO selection cursor — the REAL filled right-pointing triangle the original draws
 * (verified against a screenshot: "Will you push it?  ▶ Yes   No"). It is a 16×16 glyph
 * in the SAME TEX.TIM font we already upload (font code 0x02 = atlas cell (2,0), TEX.TIM
 * (288,32); a clean ▶). We render it exactly like a text glyph (16×16 SPRT from the font
 * page) — NOT a hand-drawn shape — so it matches the option text size/look 1:1. The .msg
 * text layout reserves 0x02 as a control code, so we emit the SPRT directly here. */
void re15_render_msg_cursor(int x, int y)
{
    if (!re15_font_ok) return;
    if (!has_room_for(sizeof(SPRT) + sizeof(DR_TPAGE))) return;
    const int b = 0x02;                                   /* ▶ cursor glyph cell */
    int u = (b & 0x0F) << 4, v = (b & 0xF0);
    SPRT *sp = (SPRT *) s_ctx.next_packet;
    s_ctx.next_packet += sizeof(SPRT);
    setSprt(sp);
    setXY0 (sp, (short)x, (short)y);
    setWH  (sp, 16, 16);
    setUV0 (sp, (uint8_t)u, (uint8_t)v);
    sp->clut = (uint16_t) re15_font_clut_id[0];
    setRGB0(sp, 128, 128, 128);
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[0], sp);

    DR_TPAGE *tp = (DR_TPAGE *) s_ctx.next_packet;
    s_ctx.next_packet += sizeof(DR_TPAGE);
    setDrawTPage(tp, 0, 1, re15_font_tpage);
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[0], tp);
}

/* Dialog page-break indicator: a small DOWN-pointing triangle (byte-true FUN_80028134
 * state 2 = "press action for the next page"). Drawn blinking at the bottom of the text. */
void re15_render_msg_down_arrow(int x, int y)
{
    if (!has_room_for(sizeof(POLY_F3))) return;
    POLY_F3 *p = (POLY_F3 *) s_ctx.next_packet;
    s_ctx.next_packet += sizeof(POLY_F3);
    setPolyF3(p);
    setXY3(p, (short)(x - 5), (short)y,
              (short)(x + 5), (short)y,
              (short)x,       (short)(y + 7));   /* down triangle: top-left, top-right, point */
    setRGB0(p, 200, 200, 200);
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[0], p);
}

/* Helper: check that emitting `bytes_needed` more primitive bytes won't
 * walk past the active buffer's allocation. Returns 1 if safe, 0 if the
 * caller should skip this primitive (silently — preserves frame rate over
 * crashing). The 16-byte safety margin keeps us off the exact boundary
 * where addPrim's next-pointer write could clobber adjacent memory. */
static int has_room_for(int bytes_needed)
{
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    uint8_t *buf_end = buf->buffer + BUFFER_LENGTH;
    return (s_ctx.next_packet + bytes_needed + 16) <= buf_end;
}

/* sprite.pri FOREGROUND OCCLUSION emit (byte-true FUN_800392d4 / FUN_80039590).
 * Per mask: a SPRT sampling the per-cut foreground atlas (re15_pri_psx_* — uploaded
 * by pri_psx.c), UV=srcX/srcY, screen XY=dstX/dstY (signed), size=w/h. The atlas's
 * index 0 → CLUT[0]=0x0000 → the GPU draws that texel TRANSPARENT, so only the
 * railing/box pixels paint. Added to a frontmost OT bucket (ot[1], just behind the
 * ot[0] subtitle text) so the foreground occludes the actors — matching the
 * USER-verified PC overdraw. (Per-mask OT depth = a future byte-true refinement.) */
extern int      re15_pri_psx_ok;
extern uint16_t re15_pri_psx_tpage;
extern uint16_t re15_pri_psx_clut;

void re15_render_pri_sprites(const re15_pri_cut_t *pri)
{
    if (!re15_pri_psx_ok || !pri || pri->mask_count <= 0) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    for (int i = 0; i < pri->mask_count; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        if (!has_room_for(sizeof(SPRT) + sizeof(DR_TPAGE))) break;
        /* Per-mask OT bucket from the SHARED depth model (re15_pri.h): depth*2 (the
         * original authors depth in its otz>>4 OT; our mesh buckets actors via otz>>3,
         * render.c:21 → ×2). The PC port derives the matching camera-Z from the SAME
         * model. Save-state-verified (RE15RBRNEXE_2): a fixture now occludes only an
         * actor genuinely behind it, never Leon when he stands in front. */
        int z = RE15_PRI_MASK_OT_BUCKET(m->depth);
        if (z < 1)              z = 1;
        if (z >= OT_LENGTH - 1) z = OT_LENGTH - 2;
        SPRT *sp = (SPRT *) s_ctx.next_packet;
        s_ctx.next_packet += sizeof(SPRT);
        setSprt(sp);
        setXY0(sp, (short)(int16_t)m->dstX, (short)(int16_t)m->dstY);
        setWH (sp, (short)m->width, (short)m->height);
        setUV0(sp, (uint8_t)m->srcX, (uint8_t)m->srcY);
        sp->clut = re15_pri_psx_clut;
        setRGB0(sp, 128, 128, 128);                 /* neutral modulation */
        addPrim(&buf->ot[z], sp);

        DR_TPAGE *tp = (DR_TPAGE *) s_ctx.next_packet;
        s_ctx.next_packet += sizeof(DR_TPAGE);
        setDrawTPage(tp, 0, 1, re15_pri_psx_tpage);
        addPrim(&buf->ot[z], tp);
    }
}

/* Direct-emit support (canonical, mirrors FUN_800254a0): hand the mesh renderer
 * a primitive slot in the active buffer so it can write XY straight from the GTE
 * (gte_stsxy3_ft3) without the per-tri CPU extraction + function-call round-trip.
 * Returns NULL if the buffer is full. re15_render_prim_link inserts it into OT[z]. */
void *re15_render_prim_alloc(int bytes)
{
    if (!has_room_for(bytes)) return (void *)0;
    void *p = s_ctx.next_packet;
    s_ctx.next_packet += bytes;
    return p;
}

void re15_render_prim_link(void *prim, int z)
{
    if (z < 0 || z >= OT_LENGTH || !prim) return;
    addPrim(&s_ctx.buffers[s_ctx.active_buffer].ot[z], prim);
}

/* RE2-canonical inline emit (FUN_8002d0e8): snapshot the emit cursor once so the
 * mesh hot loop can advance the packet pointer + addPrim() inline, with NO
 * prim_alloc/prim_link function call per polygon. */
void re15_render_emit_begin(re15_emit_t *e)
{
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    e->next = s_ctx.next_packet;
    e->end  = buf->buffer + BUFFER_LENGTH;
    e->ot   = buf->ot;
}

void re15_render_emit_end(const re15_emit_t *e)
{
    s_ctx.next_packet = e->next;
}

void re15_render_tile(int x, int y, int w, int h, int z,
                      uint8_t r, uint8_t g, uint8_t b)
{
    if (!has_room_for(sizeof(TILE))) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    TILE *tile = (TILE *) s_ctx.next_packet;

    setTile(tile);
    setXY0 (tile, (short)x, (short)y);
    setWH  (tile, (short)w, (short)h);
    setRGB0(tile, r, g, b);
    addPrim(&buf->ot[z], tile);

    s_ctx.next_packet += sizeof(TILE);
}

/* Phase 4.6.0: vertical gradient background as 30 horizontal TILE strips in
 * the highest-Z OT bucket (= drawn first → painted under everything else).
 * 30 strips of 8 px each → 240 px total = full screen height. */
void re15_render_background_gradient(uint8_t r_top,    uint8_t g_top,    uint8_t b_top,
                                     uint8_t r_bottom, uint8_t g_bottom, uint8_t b_bottom)
{
    const int N_STRIPS  = 30;
    const int STRIP_H   = SCREEN_YRES / N_STRIPS;
    /* OT_LENGTH-1 is the deepest bucket — addPrim'd here means it's drawn
     * first by DrawOTagEnv (which walks from highest index down to 0). */
    const int bg_z = OT_LENGTH - 1;

    for (int i = 0; i < N_STRIPS; i++) {
        /* Linear interpolation from top colour to bottom. */
        int t   = i * 255 / (N_STRIPS - 1);     /* 0..255 across the strips */
        uint8_t r = (uint8_t)((r_top * (255 - t) + r_bottom * t) / 255);
        uint8_t g = (uint8_t)((g_top * (255 - t) + g_bottom * t) / 255);
        uint8_t b = (uint8_t)((b_top * (255 - t) + b_bottom * t) / 255);
        re15_render_tile(0, i * STRIP_H, SCREEN_XRES, STRIP_H, bg_z, r, g, b);
    }
}

void re15_render_line(int x0, int y0, int x1, int y1, int z,
                      uint8_t r, uint8_t g, uint8_t b)
{
    if (!has_room_for(sizeof(LINE_F2))) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];
    LINE_F2 *ln = (LINE_F2 *) s_ctx.next_packet;
    setLineF2(ln);
    setXY2  (ln, (short)x0, (short)y0, (short)x1, (short)y1);
    setRGB0 (ln, r, g, b);
    addPrim(&buf->ot[z], ln);
    s_ctx.next_packet += sizeof(LINE_F2);
}

/* Phase 4.5.5: textured triangle (POLY_FT3 primitive).
 * The PSX GPU draws a 3-vertex flat-shaded textured polygon. Texture pixels
 * are sampled from the active tpage at the given UVs, optionally indexed
 * through a CLUT for 4bpp/8bpp textures. */
void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                              int x1, int y1, int u1, int v1,
                              int x2, int y2, int u2, int v2,
                              int tpage, int clut, int z,
                              uint8_t r, uint8_t g, uint8_t b)
{
    if (!has_room_for(sizeof(POLY_FT3))) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];

    POLY_FT3 *p = (POLY_FT3 *) s_ctx.next_packet;
    setPolyFT3(p);
    setXY3 (p, (short)x0, (short)y0, (short)x1, (short)y1, (short)x2, (short)y2);
    setUV3 (p, (uint8_t)u0, (uint8_t)v0,
               (uint8_t)u1, (uint8_t)v1,
               (uint8_t)u2, (uint8_t)v2);
    /* setClut macro takes 16-pixel-units x and full-pixel y. We pre-pack
     * the clut handle via getClut() at upload time, so write the word
     * directly to the prim's clut field. */
    p->clut  = (uint16_t)clut;
    p->tpage = (uint16_t)tpage;
    setRGB0(p, r, g, b);

    addPrim(&buf->ot[z], p);
    s_ctx.next_packet += sizeof(POLY_FT3);
}

/* #1c port (2026-06-01): Gouraud textured triangle (POLY_GT3) with per-vertex
 * colour — the PSX-native equivalent of the PC re15_render_textured_tri_lit.
 * The GPU modulates texel × vertex_colour / 0x80 (0x80 = neutral), so the NCCT
 * shade bytes (centred on 128) feed DIRECTLY here — NO psx_prim_to_sdl_vert
 * remap (that was only needed for SDL's /255 modulation on the PC backend). */
void re15_render_textured_tri_lit(int x0, int y0, int u0, int v0,
                                  int x1, int y1, int u1, int v1,
                                  int x2, int y2, int u2, int v2,
                                  int tpage, int clut, int z,
                                  uint8_t r0, uint8_t g0, uint8_t b0,
                                  uint8_t r1, uint8_t g1, uint8_t b1,
                                  uint8_t r2, uint8_t g2, uint8_t b2)
{
    if (!has_room_for(sizeof(POLY_GT3))) return;
    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];

    POLY_GT3 *p = (POLY_GT3 *) s_ctx.next_packet;
    setPolyGT3(p);
    setXY3(p, (short)x0, (short)y0, (short)x1, (short)y1, (short)x2, (short)y2);
    setUV3(p, (uint8_t)u0, (uint8_t)v0,
              (uint8_t)u1, (uint8_t)v1,
              (uint8_t)u2, (uint8_t)v2);
    p->clut  = (uint16_t)clut;
    p->tpage = (uint16_t)tpage;
    setRGB0(p, r0, g0, b0);
    setRGB1(p, r1, g1, b1);
    setRGB2(p, r2, g2, b2);

    addPrim(&buf->ot[z], p);
    s_ctx.next_packet += sizeof(POLY_GT3);
}

/* Phase 4.3: render the loaded test TIM at (x,y) using SPRT primitive.
 * Test-only helper — Phase 4.5 will generalize to arbitrary sprite handles. */
extern int re15_test_tim_loaded;
extern int re15_test_tim_tpage;
extern int re15_test_tim_clut;
extern int re15_test_tim_w;
extern int re15_test_tim_h;

void re15_render_test_tim(int x, int y, int z)
{
    if (!re15_test_tim_loaded) return;

    /* SPRT UVs are 8-bit (max 255). PSX texture pages are 256x256.
     * Clip displayed dimensions to fit both UV range and screen. */
    int draw_w = re15_test_tim_w;
    int draw_h = re15_test_tim_h;
    if (draw_w > 255) draw_w = 255;
    if (draw_h > 255) draw_h = 255;
    if (y + draw_h > SCREEN_YRES) draw_h = SCREEN_YRES - y;

    render_buffer_t *buf = &s_ctx.buffers[s_ctx.active_buffer];

    SPRT *sprt = (SPRT *) s_ctx.next_packet;
    setSprt(sprt);
    setXY0 (sprt, (short)x, (short)y);
    setWH  (sprt, (short)draw_w, (short)draw_h);
    setUV0 (sprt, 0, 0);
    setClut(sprt, re15_test_tim_clut & 0x3F, (re15_test_tim_clut >> 6) & 0x1FF);
    setRGB0(sprt, 128, 128, 128);
    addPrim(&buf->ot[z], sprt);
    s_ctx.next_packet += sizeof(SPRT);

    /* DR_TPAGE selects the texture page for the sprite */
    DR_TPAGE *tp = (DR_TPAGE *) s_ctx.next_packet;
    setDrawTPage(tp, 0, 1, re15_test_tim_tpage);
    addPrim(&buf->ot[z], tp);
    s_ctx.next_packet += sizeof(DR_TPAGE);
}
