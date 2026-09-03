/*
 * RE1.5 Rebuilt — Sprite priority parser (cross-platform).
 *
 * Direct port of SpritePriParser.java. Reads a single Section (per cut)
 * starting at `data + offset` and flattens all Group→Mask entries into
 * the output mask list.
 *
 * Per PSX RE1.5 RDT layout: a "NULL section" (no masks for this cut) is
 * encoded as 4 bytes 0xFFFFFFFF — when parsed, groupCount=0xFFFF and
 * maskCount=0xFFFF, both treated as terminator by the Java parser.
 *
 * Group destination offset semantics: per ROOM1170 cut 1 inspection,
 * baseRaw=0x7800 (TPage), destX/destY are signed offsets the renderer
 * adds to each mask's dstX/dstY low byte to obtain absolute screen
 * coordinates. We store the mask dstX/dstY pre-resolved.
 */
#include "re15_pri.h"

#include <string.h>

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static int16_t read_s16_le(const uint8_t *p)
{
    return (int16_t)read_u16_le(p);
}

int re15_pri_parse_section(const uint8_t *data, size_t data_size,
                           uint32_t offset, re15_pri_cut_t *out)
{
    if (!out) return 0;
    memset(out, 0, sizeof(*out));
    if (!data || offset + 4 > data_size) return 0;

    const uint8_t *p   = data + offset;
    size_t         rem = data_size - offset;

    uint16_t group_count       = read_u16_le(p + 0);
    uint16_t mask_count_decl   = read_u16_le(p + 2);

    /* NULL section: byte-true (FUN_800392d4) the terminator is ONLY the first u32 ==
     * 0xFFFFFFFF (i.e. group_count==0xFFFF && mask_count==0xFFFF), NOT group/mask==0.
     * (Audit #9 2026-06-09: the old `==0` test was a non-byte-true deviation — a real
     * section is never declared as null by a zero count.) */
    if (group_count == 0xFFFF && mask_count_decl == 0xFFFF) return 0;
    if (group_count == 0 || mask_count_decl == 0) return 0;   /* empty → nothing to draw */
    if (group_count > 256 || mask_count_decl > 256) return 0;

    size_t need = (size_t)4 + (size_t)group_count * 8;
    if (rem < need) return 0;

    /* Read groups — we need destX/destY anchors. */
    int16_t  group_dx[256] = {0};
    int16_t  group_dy[256] = {0};
    uint16_t group_n [256] = {0};
    const uint8_t *gp = p + 4;
    for (int i = 0; i < (int)group_count; i++) {
        group_n [i] = read_u16_le(gp + 0);
        /* baseRaw at +2 — TPage, unused for our re-blit */
        group_dx[i] = read_s16_le(gp + 4);
        group_dy[i] = read_s16_le(gp + 6);
        gp += 8;
    }

    /* Read masks, assign to groups in declaration order. */
    const uint8_t *mp     = gp;
    int            cursor = 0;
    int            grp    = 0;
    int            grp_used = 0;
    int            out_count = 0;

    /* BUILD BOUND = the sum of the GROUP counts, not the declared count. The
     * original's build is a NESTED loop bounded purely by the group table (outer
     * over groupCount @0x8003954c, inner over group[i].count @0x8003950c); the
     * declared halfword is not a loop bound anywhere in FUN_800392d4. The two
     * differ in real data: ROOM1210/ROOM1211 cut 4 declare 75 but the groups sum
     * to 77 (and hdr[7]=77), so the original builds 77 where the port built 75. */
    int build_total = 0;
    for (int i = 0; i < (int)group_count; i++) build_total += (int)group_n[i];

    for (int i = 0; i < build_total; i++) {
        if ((size_t)(mp - data) + 8 > data_size) break;

        uint8_t  srcX     = mp[0];
        uint8_t  srcY     = mp[1];
        uint8_t  dstX_lo  = mp[2];
        uint8_t  dstY_lo  = mp[3];
        uint16_t depth    = read_u16_le(mp + 4);
        /* mp[6..7] = the 16-bit SIZE field. byte-true (FUN_800392d4 @ lines 63/71):
         * rectangular iff `(field & 0xf000) == 0` (= the HIGH NIBBLE of mp[7] is 0),
         * NOT `mp[7]==0`. For a square mask the dim = `(field>>12)<<3` = high-nibble*8
         * (our (size_b>>4)*8, already correct). (2026-06-09 byte-true fix; sprite.pri
         * overdraw is still disabled, so this is unobservable for now.) */
        uint8_t  size_b   = mp[7];

        mp += 8;

        int rectangular = ((size_b & 0xf0) == 0);
        uint16_t width, height;
        if (rectangular) {
            if ((size_t)(mp - data) + 4 > data_size) break;
            width  = read_u16_le(mp + 0);
            height = read_u16_le(mp + 2);
            mp += 4;
        } else {
            /* BO-round 2026-05-29 (Java audit): square-mask dimension = the HIGH
             * nibble × 8, NOT the raw byte (PSX FUN_800392d4: (size>>4)<<3). The
             * raw-byte read was 2× too large. ⚠️ Untested at runtime — sprite.pri
             * overdraw is currently disabled; verify when re-enabled. */
            width = height = (uint16_t)((size_b >> 4) * 8);
        }

        /* Advance group cursor when current group's count exhausted. */
        while (grp < (int)group_count && grp_used >= group_n[grp]) {
            grp++;
            grp_used = 0;
        }
        int16_t ax = (grp < (int)group_count) ? group_dx[grp] : 0;
        int16_t ay = (grp < (int)group_count) ? group_dy[grp] : 0;
        grp_used++;

        if (out_count < RE15_PRI_MAX_MASKS_PER_CUT) {
            re15_pri_mask_t *m = &out->masks[out_count++];
            m->srcX   = srcX;
            m->srcY   = srcY;
            /* dstX/dstY: low byte from mask + group anchor (signed), stored as a
             * signed-short TRUNCATING add exactly like the original (no [0,0xFFFF]
             * clamp — a prim partly off the left/top edge has legitimate negative
             * screen coords). */
            m->dstX   = (uint16_t)(int16_t)((int)dstX_lo + (int)ax);
            m->dstY   = (uint16_t)(int16_t)((int)dstY_lo + (int)ay);
            m->depth  = depth;
            m->width  = width;
            m->height = height;
        }

        cursor++;
    }

    out->mask_count = out_count;
    /* DRAW COUNT: the original overwrites RDT header byte[0] with the declared
     * count truncated to 8 bits (@0x80039358: `sb t2,0(a0)`, t2 = pri_header>>16)
     * and the per-frame sprite emitter walks that many records. Build != draw. */
    out->draw_count = (int)(mask_count_decl & 0xFF);
    if (out->draw_count > out_count) out->draw_count = out_count;
    return out_count;
}

/* Seitendaten-Container "R15M" — s. re15_pri.h. Bewusst streng: ein beschaedigter
 * Container liefert 0 (= kein Nachschlag) statt einen zufaelligen Offset in den
 * Original-Parser zu geben. */
uint32_t re15_pri_msk_section_offset(const uint8_t *blob, size_t size, int cut_idx)
{
    if (!blob || size < 12u) return 0u;
    if (blob[0] != 'R' || blob[1] != '1' || blob[2] != '5' || blob[3] != 'M') return 0u;

    uint32_t version = (uint32_t)blob[4] | ((uint32_t)blob[5] << 8)
                     | ((uint32_t)blob[6] << 16) | ((uint32_t)blob[7] << 24);
    if (version != 1u) return 0u;

    uint32_t cuts = (uint32_t)blob[8] | ((uint32_t)blob[9] << 8)
                  | ((uint32_t)blob[10] << 16) | ((uint32_t)blob[11] << 24);
    if (cut_idx < 0 || (uint32_t)cut_idx >= cuts) return 0u;
    if ((size_t)12u + (size_t)cuts * 4u > size) return 0u;

    size_t e = 12u + (size_t)cut_idx * 4u;
    uint32_t off = (uint32_t)blob[e] | ((uint32_t)blob[e+1] << 8)
                 | ((uint32_t)blob[e+2] << 16) | ((uint32_t)blob[e+3] << 24);
    if (off == 0u || (size_t)off + 4u > size) return 0u;
    return off;
}
