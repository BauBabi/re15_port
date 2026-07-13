/*
 * re15_memcard.c — byte-true PSX memory-card (.mcr) backend. See re15_memcard.h.
 * Builds a canonical PSX card image (directory frame + "SC" title frame + icon +
 * save data) so a save is readable by any PSX card manager, with RE1.5's own
 * on-card filename "BISLPS-002220"+slot.
 */
#include "re15_memcard.h"
#include <stdio.h>
#include <string.h>

/* ---- little-endian frame helpers ---------------------------------------- */
static void wr16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void wr32(uint8_t *p, uint32_t v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); p[2]=(uint8_t)(v>>16); p[3]=(uint8_t)(v>>24); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24); }

/* PSX directory/header frame checksum = XOR of bytes [0,0x7F) into byte 0x7F. */
static void mc_xor(uint8_t *frame) {
    uint8_t x = 0;
    for (int i = 0; i < 0x7F; i++) x ^= frame[i];
    frame[0x7F] = x;
}

/* RE1.5's on-card filename for a slot: "BISLPS-002220" + slot digit (SLPS-00222,
 * per RE1.5's own dormant card code FUN_80025c00 case 7). */
static void mc_filename(int slot, char out[16]) {
    memcpy(out, "BISLPS-002220", 13);
    out[13] = (char)('0' + slot);
    out[14] = 0;
    out[15] = 0;
}

/* Load the card image from disk, or synthesise a freshly-formatted one (block 0
 * "MC" header + 15 free directory entries). Returns 1 if freshly formatted. */
static int mc_read_image(const char *path, uint8_t *img) {
    FILE *f = fopen(path, "rb");
    if (f) {
        size_t n = fread(img, 1, RE15_MCR_BYTES, f);
        fclose(f);
        if (n == RE15_MCR_BYTES) return 0;
    }
    memset(img, 0, RE15_MCR_BYTES);
    img[0] = 'M'; img[1] = 'C';                 /* frame 0 = "MC" header */
    mc_xor(img);
    for (int i = 1; i <= 15; i++) {             /* frames 1..15 = free entries */
        uint8_t *fr = img + i * RE15_MC_FRAME;
        wr32(fr + 0x00, 0xA0);                  /* alloc-state = free/formatted */
        wr16(fr + 0x08, 0xFFFF);                /* next block = none */
        mc_xor(fr);
    }
    return 1;
}

int re15_memcard_save(const char *path, int slot, const re15_savedata_t *sd, const char *title) {
    if (!path || !sd || slot < 0 || slot >= RE15_SAVE_SLOTS) return -1;
    static uint8_t img[RE15_MCR_BYTES];
    mc_read_image(path, img);

    int blk = slot + 1;                          /* slot i -> block i+1 (blocks 1..5) */

    /* directory entry for the block */
    uint8_t *dir = img + blk * RE15_MC_FRAME;
    memset(dir, 0, RE15_MC_FRAME);
    wr32(dir + 0x00, 0x51);                      /* used, single-block file */
    wr32(dir + 0x04, RE15_MC_BLOCK);             /* filesize = 0x2000 */
    wr16(dir + 0x08, 0xFFFF);                    /* no next block */
    char fn[16]; mc_filename(slot, fn);
    memcpy(dir + 0x0A, fn, 15);                  /* filename ASCII+NUL */
    mc_xor(dir);

    /* the file block */
    uint8_t *b = img + blk * RE15_MC_BLOCK;
    memset(b, 0, RE15_MC_BLOCK);
    b[0] = 'S'; b[1] = 'C';                       /* title-frame magic */
    b[2] = 0x11;                                  /* icon display flag = 1 frame */
    b[3] = 0x01;                                  /* block count */
    { char t[RE15_MC_TITLE_LEN]; memset(t, 0, sizeof t);
      if (title) { size_t k = strlen(title); if (k >= sizeof t) k = sizeof t - 1; memcpy(t, title, k); }
      memcpy(b + 0x04, t, RE15_MC_TITLE_LEN); }  /* Shift-JIS title (ASCII is valid SJIS) */
    for (int i = 0; i < 16; i++)                 /* CLUT @0x60: 0=black, 1=white, rest grey */
        wr16(b + 0x60 + i * 2, (uint16_t)(i == 0 ? 0x0000 : 0x7FFF));
    for (int i = 0; i < 0x80; i++)               /* icon @0x80: 16x16 4bpp solid colour 1 */
        b[0x80 + i] = 0x11;
    memcpy(b + 0x100, sd, sizeof(*sd));          /* the save data */

    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t w = fwrite(img, 1, RE15_MCR_BYTES, f);
    fclose(f);
    return (w == RE15_MCR_BYTES) ? 0 : -1;
}

int re15_memcard_load(const char *path, int slot, re15_savedata_t *sd) {
    if (!path || !sd || slot < 0 || slot >= RE15_SAVE_SLOTS) return -1;
    static uint8_t img[RE15_MCR_BYTES];
    if (mc_read_image(path, img)) return -1;     /* freshly formatted => no saves */

    int blk = slot + 1;
    const uint8_t *dir = img + blk * RE15_MC_FRAME;
    if ((rd32(dir) & 0xF0) != 0x50) return -1;   /* not a used first block (0x51..0x53) */

    const uint8_t *b = img + blk * RE15_MC_BLOCK;
    if (b[0] != 'S' || b[1] != 'C') return -1;   /* not a valid file block */

    re15_savedata_t tmp;
    memcpy(&tmp, b + 0x100, sizeof(tmp));
    if (tmp.magic != RE15_SAVE_MAGIC) return -1;
    if (tmp.checksum != re15_savedata_checksum(&tmp)) return -1;
    *sd = tmp;
    return 0;
}

int re15_memcard_list(const char *path, int used[RE15_SAVE_SLOTS],
                      char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN]) {
    static uint8_t img[RE15_MCR_BYTES];
    int n = 0;
    int fresh = mc_read_image(path, img);
    for (int slot = 0; slot < RE15_SAVE_SLOTS; slot++) {
        used[slot] = 0;
        titles[slot][0] = 0;
        if (fresh) continue;
        int blk = slot + 1;
        const uint8_t *dir = img + blk * RE15_MC_FRAME;
        const uint8_t *b   = img + blk * RE15_MC_BLOCK;
        if ((rd32(dir) & 0xF0) == 0x50 && b[0] == 'S' && b[1] == 'C') {
            re15_savedata_t tmp;
            memcpy(&tmp, b + 0x100, sizeof(tmp));
            if (tmp.magic == RE15_SAVE_MAGIC && tmp.checksum == re15_savedata_checksum(&tmp)) {
                used[slot] = 1;
                memcpy(titles[slot], b + 0x04, RE15_MC_TITLE_LEN);
                titles[slot][RE15_MC_TITLE_LEN - 1] = 0;
                n++;
            }
        }
    }
    return n;
}
