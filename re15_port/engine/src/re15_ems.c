/* ============================================================================ *
 *  RE1.5 Rebuilt — EMS enemy-model archive index (see re15_ems.h).
 *
 *  Byte-true port of RE15MasterExtractor.parseEmsEntries + DEFAULT_EMS_ORDER
 *  (src/main/java/de/re15/extractors/RE15MasterExtractor.java) — the established
 *  extractor that splits CDEMD0/1.EMS into the per-type em<NN>.emd models.
 * ============================================================================ */
#include "re15_ems.h"

static uint32_t ems_u32(const uint8_t *p, size_t off)
{
    return (uint32_t)p[off]
         | ((uint32_t)p[off + 1] << 8)
         | ((uint32_t)p[off + 2] << 16)
         | ((uint32_t)p[off + 3] << 24);
}

static size_t ems_align_up(size_t v, size_t a)
{
    return (v + (a - 1)) & ~(a - 1);
}

int re15_ems_get_entry(const uint8_t *ems, size_t ems_size, int index,
                       size_t *out_offset, size_t *out_length)
{
    if (!ems || index < 0) return -1;

    size_t cursor = 0;
    int    i      = 0;

    while (cursor + 4 <= ems_size) {
        uint32_t dir_off = ems_u32(ems, cursor);
        /* a valid EMD blob opens with its section-directory offset: >= the 36-byte
         * footer and word-aligned. anything else = end of the archive. */
        if (dir_off < RE15_EMS_FOOTER || (dir_off & 3u) != 0) break;

        size_t len = (size_t)dir_off + RE15_EMS_FOOTER;
        if (len == 0 || cursor + len > ems_size) break;

        if (i == index) {
            if (out_offset) *out_offset = cursor;
            if (out_length) *out_length = len;
            return 0;
        }
        i++;

        size_t next = cursor + len;
        if (next >= ems_size) break;
        size_t aligned = ems_align_up(next, RE15_EMS_ALIGN);
        if (aligned <= cursor) break;
        cursor = aligned;

        /* skip the 4-byte zero gap some blobs carry, then re-align to a sector. */
        size_t skipped = 0;
        while (cursor + 4 <= ems_size && ems_u32(ems, cursor) == 0) { cursor += 4; skipped += 4; }
        if (skipped > 0) cursor = ems_align_up(cursor, RE15_EMS_ALIGN);
    }

    return -1;   /* index past the last blob */
}

/* DEFAULT_EMS_ORDER (RE15MasterExtractor): blob index -> enemy type. */
static const uint8_t s_ems_order[] = {
    0x10, 0x11, 0x12, 0x13, 0x16, 0x18, 0x1A, 0x20, 0x21, 0x24,
    0x25, 0x26, 0x27, 0x29, 0x2B, 0x2D, 0x30, 0x36, 0x40, 0x42,
    0x45, 0x47, 0x49, 0x4B, 0x4D
};

int re15_ems_index_for_type(uint8_t type)
{
    for (int i = 0; i < (int)sizeof s_ems_order; i++)
        if (s_ems_order[i] == type) return i;
    return -1;
}
