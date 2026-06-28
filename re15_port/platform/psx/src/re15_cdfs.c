/*
 * RE1.5 Rebuilt — minimal CD file loader (Phase 4.7, 2026-06-02).
 * See re15_cdfs.h. RE2-canonical: stream assets from CD instead of incbin.
 */
#include <psxcd.h>
#include <stdint.h>
#include "re15_cdfs.h"

/* CD SOURCE staging buffer (see re15_cdfs.h). MUST be separate from re15_psx_staging_buf:
 * the BG path loads the compressed BSS chunk HERE and decodes it INTO re15_psx_staging_buf. */
unsigned char re15_cd_staging[RE15_CD_STAGING_SIZE];

void re15_cdfs_init(void)
{
    CdInit();   /* idempotent; safe to call once at boot */
}

/* Defined in audio_psx.c: finish any in-flight async dialogue-voice CD read
 * before we start another read, so the two can't collide on the single drive
 * (an in-flight voice read otherwise aborts the cut-change BG load). No-op when
 * no voice is loading. */
extern void re15_audio_voice_flush(void);

int re15_cd_load_file(const char *iso_path, void *buf, int maxbytes)
{
    re15_audio_voice_flush();
    CdlFILE fp;
    /* ISO9660 lookup by name (RE2 mirror: file-id -> LBA). */
    if (!CdSearchFile(&fp, (char *)iso_path)) return -1;          /* not found */
    if (fp.size <= 0)                          return -1;

    int sectors      = (fp.size + 2047) >> 11;                    /* ceil /2048 */
    /* CdRead DMAs WHOLE sectors (sectors*2048), which can exceed fp.size. Guard
     * the actual write extent against the buffer, not just the logical size —
     * otherwise a near-buffer-size file overruns into the adjacent global. */
    if ((sectors << 11) > maxbytes)            return -1;          /* whole-sector write must fit */
    /* Seek target = the file's LBA, then read all sectors at 2x. */
    CdControl(CdlSetloc, (const uint8_t *)&fp.pos, 0);
    if (!CdRead(sectors, (uint32_t *)buf, CdlModeSpeed)) return -1;
    if (CdReadSync(0, 0) < 0)                             return -1;
    return fp.size;
}
