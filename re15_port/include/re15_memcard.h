/*
 * re15_memcard.h — byte-true PSX memory-card image (.mcr) backend for FE-4.
 *
 * The port stores saves in a real PSX memory-card image so a save is byte-true
 * to hardware (a PSX card manager / emulator could read it). Layout is the
 * canonical PSX card (psx-spx controllersandmemorycards §2455+), the same one
 * RE2 relies on via the BIOS file API:
 *   128 KB = 16 blocks × 0x2000; block 0 = 16 directory frames × 0x80.
 *   Directory entry: +0x00 alloc-state (0x51 used / 0xA0 free), +0x04 filesize,
 *     +0x08 next-block, +0x0A filename ASCII+NUL, +0x7F XOR checksum.
 *   File block: +0x00 title frame ("SC", icon flag, block count, Shift-JIS title,
 *     +0x60 CLUT), +0x80 icon frame (16×16 4bpp), then the save data.
 * RE1.5 adaptation: the on-card filename is RE1.5's own product code
 *   "BISLPS-002220"+slot (SLPS-00222 — verified in RE1.5's dormant card code),
 *   not RE2's "BASLUS-00748". One block per save, 5 slots (blocks 1..5).
 *
 * The .mcr file I/O here is the PC backend; the frame/checksum builders are the
 * byte-true format the PSX target would reuse behind the BIOS card API.
 */
#ifndef RE15_MEMCARD_H
#define RE15_MEMCARD_H

#include <stdint.h>
#include "re15_savedata.h"

#define RE15_MCR_BYTES     (128 * 1024)   /* full card image                     */
#define RE15_MC_BLOCK      0x2000         /* 8 KB per block                      */
#define RE15_MC_FRAME      0x80           /* 128 B per frame                     */
#define RE15_SAVE_SLOTS    5              /* saves per card (blocks 1..5)        */
#define RE15_MC_TITLE_LEN  32             /* title chars we surface to the UI    */

/* Write `sd` into `slot` (0..RE15_SAVE_SLOTS-1) of the card image at `path`,
 * with the BIOS-card-menu title `title` (ASCII/SJIS, ≤ RE15_MC_TITLE_LEN). The
 * image is created (all-free) if it does not exist. Returns 0 on success. */
int re15_memcard_save(const char *path, int slot, const re15_savedata_t *sd, const char *title);

/* Read the save in `slot` of the card image at `path` into `sd`. Returns 0 on
 * success, -1 if the slot is empty / invalid / the file is missing. */
int re15_memcard_load(const char *path, int slot, re15_savedata_t *sd);

/* Enumerate slots for the load screen: used[i] = 1 if slot i holds a valid save,
 * titles[i] = its BIOS title (NUL-terminated). Returns the number of used slots. */
int re15_memcard_list(const char *path, int used[RE15_SAVE_SLOTS],
                      char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN]);

#endif /* RE15_MEMCARD_H */
