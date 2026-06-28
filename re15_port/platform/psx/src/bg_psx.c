/*
 * RE1.5 Rebuilt — Background-frame subsystem, PSX backend
 * (Phase 4.5.6.3, 2026-05-18).
 *
 * RE2-faithful MDEC pipeline. Reference: RE2_Quellcode/FUN_8002bdf4.c
 * (the room-init MDEC decode) and PSn00bSDK examples/mdec/mdecimage.
 *
 * Pipeline at re15_bg_load_from_bss():
 *
 *   re15_bss_vlc_decode(bs, ...) — software VLC, not PSn00bSDK's
 *                                  DecDCTvlc. WHY: RE1.5's BSS labels
 *                                  itself version=3 but uses
 *                                  predictive small-table DC encoding
 *                                  (the V1-style path with DC_Y_TAB0
 *                                  / DC_UV_TAB0). PSn00bSDK's V3 path
 *                                  expects Huffman DC (Sony standard
 *                                  V3 = dc[128] / dc_len[9] tables),
 *                                  which produces blocky/wrong output
 *                                  on RE1.5 files. Our software
 *                                  decoder matches RE1.5's actual
 *                                  encoding (proven by visual parity
 *                                  with the extracted/ReF BMP and the
 *                                  PC target, which uses the same
 *                                  software VLC).
 *   DecDCTin(vlc_buf, 0)          — feed codes to MDEC, 16bpp output
 *   for col 0..320 step 16:
 *     DecDCTout(slice, 1920w)     — retrieve one 16×240 slice
 *     DecDCToutSync(0)            — wait for transfer
 *     LoadImage(rect, slice)      — upload slice to VRAM cache
 *
 * Per frame at re15_bg_blit():
 *   MoveImage(cache → framebuffer)
 *     — single GPU command, VRAM-to-VRAM copy. Issued before
 *       DrawOTagEnv so OT primitives layer on top of the background.
 *
 * VRAM cache region: (640, 256)..(960, 495) — 320×240 16bpp. Sits in
 * the right half of VRAM, below the test TIM at (640, 0..255) and
 * left of the debug font at (960, 0..).
 */

#include <stdint.h>
#include <stddef.h>
#include <psxgpu.h>
#include <psxpress.h>
#include "re15_engine.h"
#include "re15_bss.h"
#include "re15_bg.h"

/* Bundled BSS test asset (psn00bsdk_target_incbin in CMakeLists.txt). */
extern const uint8_t  test_bss[];
extern const uint32_t test_bss_size;

/* Phase 4.7 CD-streaming (2026-06-02): the 6 per-cut ROOM1170 BSS frames (cuts
 * 0/1/2/3/4/6, 64KB each = 384KB) are NO LONGER incbin'd. They're streamed from
 * the CD on cut-change into this single reused 64KB staging buffer, then VLC/MDEC
 * decoded. Net RAM: -384KB (incbin) + 64KB (staging) = ~320KB reclaimed. test.bss
 * stays resident as the boot/fallback BG. RE2 mirror: RE_15 FUN_800264e8 CD_read
 * BG-on-cut. */
#include "re15_cdfs.h"   /* uses the shared re15_cd_staging buffer (no own buffer) */
#include "re15_room.h"   /* g_current_room_id — per-room BG subdir path */

/* VRAM cache layout. Sized to exactly one decoded BG (320×240 16bpp). */
#define BG_VRAM_CACHE_X    640
#define BG_VRAM_CACHE_Y    256
#define BG_VRAM_CACHE_W    RE15_BSS_FRAME_WIDTH
#define BG_VRAM_CACHE_H    RE15_BSS_FRAME_HEIGHT

/* MDEC working buffers.
 *
 * s_vlc_buf must hold the DecDCTvlc output. ROOM11000.bs has
 * run_length_words=19872; the Java reference decoder produces
 * (19872+2)*4 = 79496 int16 coefficients = 39748 uint32 words.
 * PSn00bSDK's DecDCTvlc packs at the same density. We size at 65536
 * (256 KB) for headroom — undersizing causes silent buffer overflow
 * into adjacent RAM, which manifests as macroblock-boundary artifacts
 * + jagged edges on PSX (caught 2026-05-18 vs the reference BMP).
 *
 * s_slice_buf: 16 px × 240 px × 16bpp / 4 B/word = 1920 words per
 * vertical strip. Used once per MDEC-decoded column. */
/* The MDEC VLC-decode buffer IS the shared PSX staging buffer (RE2 one-buffer model;
 * see re15_cdfs.h). It primarily holds the DecDCTvlc output here (~155 KB max for
 * ROOM11000), and the enemy loader borrows it transiently to stage an EM<NN>.EMD —
 * the two never run in the same instant. Sized RE15_PSX_STAGING_SIZE (270 KB) for the
 * largest enemy container; the BG decode's headroom note below still applies. */
uint32_t re15_psx_staging_buf[RE15_PSX_STAGING_SIZE / 4] __attribute__((aligned(8)));
static uint32_t s_slice_buf[ 1920] __attribute__((aligned(8)));

static int s_bg_loaded = 0;

void re15_bg_init(void)
{
    /* DecDCTReset(0) loads the default IDCT matrix + quant tables and
     * puts the MDEC in color output mode. Must be called once before
     * any DecDCTvlc/in/out. */
    DecDCTReset(0);
}

int re15_bg_load_from_bss(const uint8_t *bss_chunk, size_t size)
{
    re15_bss_chunk_t chunk;
    if (!re15_bss_parse_chunk(bss_chunk, size, &chunk)) return -1;
    if (!re15_bss_chunk_has_video(&chunk))              return -2;

    /* Phase 4.5.13-B3 (2026-05-21): drain any in-flight GPU/MDEC work
     * before re-arming the MDEC. Without this, the sub02 Cut_chg storm
     * (6+ cuts in ~75 s) races re15_bg_blit's MoveImage (DMA-ch2) with
     * the new DecDCTin (DMA-ch0); the next DecDCToutSync(0) polls a
     * status bit that never flips → blackscreen-0 fps hang. Re-resetting
     * the MDEC ensures a clean state machine after the previous decode. */
    DrawSync(0);
    DecDCTReset(0);

    /* Software VLC decode (NOT PSn00bSDK's DecDCTvlc — see file header
     * comment for the V3 DC-encoding mismatch). re15_bss_vlc_decode
     * writes the BSS preamble (run_length_words + VLC_ID) into the
     * first uint32 of the buffer, followed by the int16 MDEC codes
     * packed as uint32 pairs. That layout is byte-compatible with
     * what DecDCTin() expects for its input: the first word is copied
     * to MDEC0 (with mode bits OR'd in) and the rest is DMA'd to the
     * MDEC chip.
     *
     * Capacity: (run_length_words+2)*4 int16 = (run_length_words+2)*2
     * uint32. ROOM11000's run_length_words=19872 → 39748 uint32 = 155
     * KB. Our buffer is 256 KB — comfortable margin. */
    size_t vlc_capacity_int16 = sizeof(re15_psx_staging_buf) / sizeof(int16_t);
    int wrote = re15_bss_vlc_decode(chunk.vlc_payload,
                                     chunk.vlc_payload_size,
                                     chunk.run_length_words,
                                     chunk.quant, chunk.version,
                                     (int16_t *)re15_psx_staging_buf,
                                     vlc_capacity_int16);
    if (wrote < 0) return -3;

    /* Feed the software-decoded codes to the MDEC hardware chip. Mode
     * 0 = 16bpp color output (RGB 5-5-5), matches our framebuffer. */
    DecDCTin(re15_psx_staging_buf, 0);

    /* Loop through 16-pixel-wide vertical slices, uploading each to
     * the VRAM cache region. 16 px × 240 px × 16bpp = 7680 bytes
     * = 1920 32-bit words per slice. 320 / 16 = 20 slices. */
    int x = BG_VRAM_CACHE_X;
    for (int col = 0; col < BG_VRAM_CACHE_W; col += 16) {
        DecDCTout(s_slice_buf, 1920);
        /* Bounded poll instead of blocking DecDCToutSync(0). If the MDEC
         * state machine wedged (which the 2026-05-21 hang chase revealed
         * happens when DecDCTin is issued without DrawSync+Reset), bail
         * out with -4 rather than spinning forever. ~500k iter budget =
         * a few ms at PSX speed. */
        int t;
        for (t = 0; t < 500000; t++) {
            if (DecDCToutSync(1) == 0) break;
        }
        if (t >= 500000) return -4;

        RECT slice_rect = { (short)x, BG_VRAM_CACHE_Y, 16, BG_VRAM_CACHE_H };
        LoadImage(&slice_rect, s_slice_buf);
        DrawSync(0);

        x += 16;
    }

    s_bg_loaded = 1;
    return 0;
}

int re15_bg_load_test_asset(void)
{
    return re15_bg_load_from_bss(test_bss, (size_t)test_bss_size);
}

/* Per-room per-cut BG loader — room-AWARE via g_current_room_id (the "room1170" in the
 * old name was historical; it streams \BSS\ROOM%04X\BG%02d.BSS for ANY current room). */
int re15_bg_load_cut(int cut_idx)
{
    /* Per-cut BG streaming (2026-06-04: whitelist hack removed). ROOM1170 has 13
     * camera cuts; the disc carries one dedicated BGnn.BSS per cut (BG00..BG12).
     * EVERY cut streams its own faithful background — no whitelist, no test.bss
     * substitution for "missing" cuts. test.bss is used ONLY if the CD read
     * genuinely fails. Per-decode fires only on actual cut change (s_last_cut). */
    static int s_last_cut = -1;
    if (s_bg_loaded && s_last_cut == cut_idx) return 0;

    const uint8_t *data = test_bss;
    size_t         size = (size_t)test_bss_size;
    if (cut_idx >= 0) {
        /* RE2-faithful: per-room BG frames live in \BSS\ROOM####\ (RE2:
         * \COMMON\BSS\ROOM###\), so the CD root stays tiny. g_current_room_id
         * selects the room's subdir. */
        char name[32];
        sprintf(name, "\\BSS\\ROOM%04X\\BG%02d.BSS;1", g_current_room_id, cut_idx);
        int n = re15_cd_load_file(name, re15_cd_staging, RE15_CD_STAGING_SIZE);
        if (n > 0) { data = re15_cd_staging; size = (size_t)n; }
    }

    int rc = re15_bg_load_from_bss(data, size);
    if (rc == 0) s_last_cut = cut_idx;
    return rc;
}

void re15_bg_blit(int dst_x, int dst_y)
{
    if (!s_bg_loaded) return;

    /* Single GP0(0x80) CopyVramToVram command — the GPU copies a
     * rectangle entirely within VRAM, no DMA involvement. ~30 µs for
     * a 320×240 region (per psx-spx benchmarks). MoveImage() is
     * synchronous in PSn00bSDK — blocks until the GPU acknowledges. */
    RECT src = { BG_VRAM_CACHE_X, BG_VRAM_CACHE_Y,
                 BG_VRAM_CACHE_W, BG_VRAM_CACHE_H };
    MoveImage(&src, dst_x, dst_y);
}

int re15_bg_is_loaded(void)
{
    return s_bg_loaded;
}

/* Multi-room (2026-06-04): force the next re15_bg_load_*_cut() to re-decode
 * even if the cut index is unchanged — the resident BG belongs to the OLD
 * room, so on room-enter the cached frame must be discarded (else the new
 * room shows the previous room's background). */
void re15_bg_invalidate(void)
{
    s_bg_loaded = 0;
}
