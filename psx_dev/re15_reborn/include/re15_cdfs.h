/*
 * RE1.5 Rebuilt — minimal CD file loader (Phase 4.7, 2026-06-02).
 *
 * RE2-canonical resource model: stream assets from the CD on demand instead of
 * incbin'ing everything into the EXE (which pins ~1MB in .data forever and was
 * pushing BSS past the 2MB RAM ceiling). Mirrors RE2's file-id -> LBA -> CdRead
 * load path (FUN_800132b0 / DsRead2); here we use PSn00bSDK libcd CdSearchFile
 * (ISO9660 lookup by name) + CdRead + CdReadSync.
 */
#ifndef RE15_CDFS_H
#define RE15_CDFS_H

#include <stdint.h>

/* Initialise the CD subsystem (CdInit). Call once at boot before any load. */
void re15_cdfs_init(void);

/* Shared LARGE transient staging buffer (RE2 one-buffer model: the released engine
 * funnels MDEC-background VLC decode, audio VAB, AND model/TIM loads through ONE staging
 * region at 0x80198000 — they never run in the same instant, so a single buffer serves
 * all and nothing extra is reserved per asset type). Defined in bg_psx.c (its primary
 * user is the MDEC VLC decode). Sized to the largest single load = the biggest enemy EMD
 * (EM2B = 262768 B → 129 sectors = 264192) with headroom; BG VLC needs only ~155 KB. */
#define RE15_PSX_STAGING_SIZE  0x42000   /* 270336 bytes */
extern uint32_t re15_psx_staging_buf[RE15_PSX_STAGING_SIZE / 4];

/* CD SOURCE staging — a SEPARATE buffer from re15_psx_staging_buf above. It holds the
 * raw CD bytes of: the per-cut BSS background chunk (bg_psx.c), boot TIMs, and audio
 * VAB/SEQ chunks. It MUST stay distinct from re15_psx_staging_buf because the background
 * path DECODES from here (source) INTO re15_psx_staging_buf (VLC/MDEC dest) — RE2 itself
 * keeps the MDEC source (0x80190000) and the decode dest (0x80198000) as two regions for
 * exactly this reason. (A 2026-06-14 attempt to alias them into one buffer corrupted the
 * BG decode + overflowed into camera/actor globals → "camera stuck, models misplaced";
 * reverted. The author's earlier "one shared staging buffer" RE2 note over-simplified —
 * a read-source and a write-dest of the SAME decode can never be the same memory.) */
#define RE15_CD_STAGING_SIZE  0x19000   /* 102400 bytes */
extern unsigned char re15_cd_staging[RE15_CD_STAGING_SIZE];

/* Load a whole file from the CD (ISO9660 path, e.g. "\\BG00.BSS;1") into `buf`.
 * Reads ceil(size/2048) sectors synchronously. Returns the file's byte size on
 * success, or -1 if the file was not found / the read failed / it exceeds
 * `maxbytes`. The buffer must be word-aligned and >= the file's sector-rounded
 * size (CdRead writes whole 2048-byte sectors). */
int re15_cd_load_file(const char *iso_path, void *buf, int maxbytes);

#endif /* RE15_CDFS_H */
