/*
 * RE1.5 Rebuilt — RDT parser (Phase 4.5.9, 2026-05-19).
 *
 * Pure C, no malloc. The output `re15_rdt_t` holds pointers into the
 * caller's data buffer — caller is responsible for keeping it alive.
 *
 * Reference: src/main/java/de/re15/extractors/RDTExtractor.java
 * (Header.read, Addresses.read, readPointerOffsets, RIDParser, ZoneRvdParser).
 */

#include <string.h>
#include "re15_rdt.h"
#include "re15_aot.h"

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static int16_t  read_s16_le(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* Smallest RDT section start strictly greater than `start`, capped at `size`.
 * Considers the header directory (0x08..0x5c, 4-B stride) AND — when a model
 * pointer table is given — its own `model_entries` u32 entries (so a prop TIM
 * ends where the following MD1 begins). Mirrors RDTExtractor's boundary set
 * (addBoundary + nextBoundary). Used to bound embedded prop/audio sub-blocks. */
static uint32_t rdt_next_boundary(const uint8_t *data, size_t size, uint32_t start,
                                  uint32_t model_tbl, int model_entries)
{
    uint32_t best = (uint32_t)size;
    for (uint32_t h = 0x08; h <= 0x5c; h += 4) {
        uint32_t p = read_u32_le(&data[h]);
        if (p > start && p < best && (size_t)p <= size) best = p;
    }
    if (model_tbl != 0) {
        for (int i = 0; i < model_entries; i++) {
            if ((size_t)model_tbl + (size_t)i * 4 + 4 > size) break;
            uint32_t p = read_u32_le(&data[model_tbl + (uint32_t)i * 4]);
            if (p > start && p < best && (size_t)p <= size) best = p;
        }
    }
    return best;
}

/* Parse the RDT+0x30 model pointer table → embedded prop TIM/MD1 slices.
 * 2*nOmodel u32 LE offsets, alternating TIM,MD1; each body sliced to the next
 * boundary. (RDTExtractor.parseModelPointers @ RDTExtractor.java:951-980.) */
static void parse_props(const uint8_t *data, size_t size, re15_rdt_t *out)
{
    out->prop_count = 0;
    uint32_t tbl  = read_u32_le(&data[0x30]);
    int      npr  = (int)out->nOmodel;
    if (tbl == 0 || npr <= 0) return;
    if (npr > RE15_RDT_MAX_PROPS) npr = RE15_RDT_MAX_PROPS;
    int entries = npr * 2;                          /* TIM,MD1 per prop */
    if ((size_t)tbl + (size_t)entries * 4 > size) return;

    for (int k = 0; k < npr; k++) {
        uint32_t tim_off = read_u32_le(&data[tbl + (uint32_t)(2 * k)     * 4]);
        uint32_t md1_off = read_u32_le(&data[tbl + (uint32_t)(2 * k + 1) * 4]);
        if (tim_off != 0 && (size_t)tim_off < size) {
            uint32_t end = rdt_next_boundary(data, size, tim_off, tbl, entries);
            out->prop_tim[k]      = data + tim_off;
            out->prop_tim_size[k] = (int)(end - tim_off);
        }
        if (md1_off != 0 && (size_t)md1_off < size) {
            uint32_t end = rdt_next_boundary(data, size, md1_off, tbl, entries);
            out->prop_md1[k]      = data + md1_off;
            out->prop_md1_size[k] = (int)(end - md1_off);
        }
    }
    out->prop_count = npr;
}

/* Slice the two embedded VAB banks (snd0 @0x08-0x10, snd1 @0x14-0x1c). Each
 * component runs to the next header boundary. (RDTExtractor.writeSound:228-264.) */
static void parse_audio(const uint8_t *data, size_t size, re15_rdt_t *out)
{
    static const uint32_t edt_off[2] = { 0x08, 0x14 };
    static const uint32_t vh_off [2] = { 0x0c, 0x18 };
    static const uint32_t vb_off [2] = { 0x10, 0x1c };
    for (int b = 0; b < 2; b++) {
        uint32_t e = read_u32_le(&data[edt_off[b]]);
        uint32_t h = read_u32_le(&data[vh_off [b]]);
        uint32_t v = read_u32_le(&data[vb_off [b]]);
        if (e != 0 && (size_t)e < size) {
            out->snd_edt[b]      = data + e;
            out->snd_edt_size[b] = (int)(rdt_next_boundary(data, size, e, 0, 0) - e);
        }
        if (h != 0 && (size_t)h < size) {
            out->snd_vh[b]      = data + h;
            out->snd_vh_size[b] = (int)(rdt_next_boundary(data, size, h, 0, 0) - h);
        }
        if (v != 0 && (size_t)v < size) {
            out->snd_vb[b]      = data + v;
            out->snd_vb_size[b] = (int)(rdt_next_boundary(data, size, v, 0, 0) - v);
        }
    }
}

/* Parse the RVD block at `data + zone_start`. Each entry is 20 bytes;
 * terminator is `const=0xFFFF && camFrom=0xFF && camTo=0xFF`. */
static void parse_zones(const uint8_t *data, size_t size, uint32_t zone_start,
                         re15_rdt_t *out)
{
    out->zone_count = 0;
    if (zone_start == 0 || zone_start >= size) return;

    const uint8_t *p = data + zone_start;
    while ((size_t)(p - data) + 20 <= size &&
           out->zone_count < RE15_RDT_MAX_ZONES) {
        uint16_t marker  = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
        uint8_t  camFrom = p[2];
        uint8_t  camTo   = p[3];
        if (marker == 0xFFFF && camFrom == 0xFF && camTo == 0xFF) {
            break;   /* RVD terminator */
        }

        /* 4 corners (x, y) — actually XZ since RE rooms use Y-up world
         * but the RVD plane is the floor (XZ in our PSX convention). */
        int16_t xs[4], zs[4];
        for (int i = 0; i < 4; i++) {
            xs[i] = read_s16_le(&p[4 + i*4]);
            zs[i] = read_s16_le(&p[6 + i*4]);
        }
        int16_t min_x = xs[0], max_x = xs[0];
        int16_t min_z = zs[0], max_z = zs[0];
        for (int i = 1; i < 4; i++) {
            if (xs[i] < min_x) min_x = xs[i];
            if (xs[i] > max_x) max_x = xs[i];
            if (zs[i] < min_z) min_z = zs[i];
            if (zs[i] > max_z) max_z = zs[i];
        }

        re15_rdt_zone_t *z = &out->zones[out->zone_count++];
        /* AQ-round: keep the 4 quad corners so the scanner can do PSX-
         * canonical point-in-quad (FUN_80014368). AABB fields retained
         * for AOT compat / debug overlay. */
        for (int i = 0; i < 4; i++) {
            z->xs[i] = xs[i];
            z->zs[i] = zs[i];
        }
        z->cx       = ((int32_t)min_x + (int32_t)max_x) / 2;
        z->cz       = ((int32_t)min_z + (int32_t)max_z) / 2;
        z->half_w   = ((int32_t)max_x - (int32_t)min_x) / 2;
        z->half_h   = ((int32_t)max_z - (int32_t)min_z) / 2;
        z->cam_from = camFrom;
        z->cam_to   = camTo;

        p += 20;
    }
}

/* Parse the main SCD pointer table to locate the entry script. The table
 * is a sequence of u16 LE offsets (relative to mainScdStart); the first
 * offset's value equals the number of bytes occupied by the table
 * itself, so [0]=2 means a 1-entry table pointing at script body @ +2.
 *
 * We use a conservative heuristic: read the first u16, treat it as the
 * count's encoded form (= table size in bytes / 2 = entry count after
 * div), and the entry script lives at mainScdStart + that_value. */
static void parse_main_scd(const uint8_t *data, size_t size,
                            uint32_t main_scd_start, uint32_t sub_scd_start,
                            re15_rdt_t *out)
{
    out->main_scd      = NULL;
    out->main_scd_size = 0;
    if (main_scd_start == 0 || main_scd_start >= size) return;
    if (sub_scd_start <= main_scd_start || sub_scd_start > size) return;

    uint16_t first_off = (uint16_t)data[main_scd_start]
                       | ((uint16_t)data[main_scd_start + 1] << 8);
    if (first_off < 2 || (size_t)(main_scd_start + first_off) >= size) return;

    out->main_scd      = data + main_scd_start + first_off;
    out->main_scd_size = sub_scd_start - (main_scd_start + first_off);
}

/* Phase 4.5.9-E: parse sub-SCD pointer table + populate per-entry script
 * pointers. ROOM1100 has 9 sub scripts; we cap at RE15_RDT_MAX_SUB_SCD. */
static int parse_sub_scd_table(const uint8_t *data, size_t size,
                                uint32_t sub_scd_start, re15_rdt_t *out)
{
    if (sub_scd_start == 0 || sub_scd_start + 2 > size) return 0;
    uint16_t first_off = (uint16_t)data[sub_scd_start]
                       | ((uint16_t)data[sub_scd_start + 1] << 8);
    if (first_off < 2 || (first_off & 1)) return 0;
    int count = (int)(first_off / 2);
    if (count > RE15_RDT_MAX_SUB_SCD) count = RE15_RDT_MAX_SUB_SCD;
    for (int i = 0; i < count; i++) {
        uint16_t off = (uint16_t)data[sub_scd_start + i*2]
                     | ((uint16_t)data[sub_scd_start + i*2 + 1] << 8);
        if (sub_scd_start + off >= size) {
            out->sub_scd[i] = NULL;
            continue;
        }
        out->sub_scd[i] = data + sub_scd_start + off;
    }
    return count;
}

int re15_rdt_parse(const uint8_t *data, size_t size, re15_rdt_t *out)
{
    if (!data || !out) return -1;
    if (size < 0x60) return -2;     /* header is 0x60 bytes */

    memset(out, 0, sizeof(*out));
    out->raw       = data;          /* base for offset-relative consumers (sprite.pri) */
    out->raw_size  = (int)size;

    /* Header counters */
    out->nSprite   = data[0x00];
    out->nCut      = data[0x01];
    out->nOmodel   = data[0x02];
    out->nItem     = data[0x03];
    out->nDoor     = data[0x04];
    out->nRoom_at  = data[0x05];
    out->reverb_lv = data[0x06];

    /* Address table */
    uint32_t camera_start    = read_u32_le(&data[0x24]);
    uint32_t zone_start      = read_u32_le(&data[0x28]);
    uint32_t main_scd_start  = read_u32_le(&data[0x40]);
    uint32_t sub_scd_start   = read_u32_le(&data[0x44]);

    /* RID block (32 B/entry, byte-for-byte matches re15_camera_cut_t) */
    if (camera_start != 0 && (size_t)camera_start + (size_t)out->nCut * 32 <= size) {
        out->cuts      = (const re15_camera_cut_t *)(data + camera_start);
        out->cut_count = (int)out->nCut;
    } else {
        out->cuts      = NULL;
        out->cut_count = 0;
    }

    /* SCA collision block (offset 0x20). Header (24 B): ceilingX u16, ceilingZ
     * u16, count[5] u32; then sum(counts) entries of 12 B each (lib_sca v1.5 =
     * "BH2 prototype"). The 5 counts are the per-quadrant partition; we parse
     * them all contiguously and (for now) test against all. sca aliases the
     * resident buffer. */
    out->sca = NULL; out->sca_count = 0; out->ceiling_x = 0; out->ceiling_z = 0;
    out->sca_rgn[0] = out->sca_rgn[1] = out->sca_rgn[2] = out->sca_rgn[3] = out->sca_rgn[4] = 0;
    {
        uint32_t sca_start = read_u32_le(&data[0x20]);
        if (sca_start != 0 && (size_t)sca_start + 24 <= size) {
            const uint8_t *h = data + sca_start;
            out->ceiling_x = (uint16_t)(h[0] | (h[1] << 8));
            out->ceiling_z = (uint16_t)(h[2] | (h[3] << 8));
            int total = 0;
            for (int g = 0; g < 5; g++) {
                int c = (int)read_u32_le(&h[4 + g * 4]);
                out->sca_rgn[g] = c;            /* per-quadrant partition counts */
                total += c;
            }
            /* Only bound is the exact byte fit (FUN_8003aea0 trusts the counts). */
            if (total > 0 && (size_t)sca_start + 24 + (size_t)total * 12 <= size) {
                out->sca       = (const re15_sca_entry_t *)(data + sca_start + 24);
                out->sca_count = total;
            }
        }
    }

    /* LIGHT block (offset 0x2c): per-cut NCCT light set, 40 B/cut, count = nCut.
     * (Verified: room1170's 520-B light.lit == RDT+0x2c target.) `lights` aliases
     * the resident buffer; consumed via re15_light_parse for per-room lighting. */
    out->lights = NULL; out->lights_size = 0;
    {
        uint32_t light_start = read_u32_le(&data[0x2c]);
        int      light_size  = out->cut_count * 40;
        if (light_start != 0 && light_size > 0 &&
            (size_t)light_start + (size_t)light_size <= size) {
            out->lights      = data + light_start;
            out->lights_size = light_size;
        }
    }

    /* MESSAGE block (offset 0x3c): the .msg subtitle/examine table. Standard RE
     * format — u16 off[]; off[0] = (n_off)*2 = the table size in bytes; bodies
     * follow. EVERY off[0..n_off-1] is a message START (no trailing terminator —
     * the original indexes off[id] directly and each .msg body self-terminates on
     * its 0x01 end-code). The block therefore extends past off[n_off-1] to cover
     * the LAST message's body; its true end is the start of the NEXT RDT block (the
     * smallest header directory pointer greater than msg_start). The old bound
     * `messages_size = off[n_off-1]` cut the block off AT the last message's start,
     * so room1170's main16 (id 16, off[16]=785) lay entirely outside → Message_on(16)
     * found no body and the subtitle never displayed. */
    out->messages = NULL; out->messages_size = 0;
    {
        uint32_t msg_start = read_u32_le(&data[0x3c]);
        if (msg_start != 0 && (size_t)msg_start + 4 <= size) {
            const uint8_t *b = data + msg_start;
            int off0  = b[0] | (b[1] << 8);          /* = n_off * 2 = table size */
            int n_off = off0 / 2;
            /* Find the next RDT block boundary after the message block: smallest
             * header directory pointer (RDT+0x08..0x5c, 4-byte stride) strictly
             * greater than msg_start. That is the byte-true end of the msg block. */
            uint32_t block_end = (uint32_t)size;
            for (uint32_t h = 0x08; h <= 0x5c; h += 4) {
                uint32_t p = read_u32_le(&data[h]);
                if (p > msg_start && p < block_end && (size_t)p <= size)
                    block_end = p;
            }
            if (n_off >= 1 && (size_t)msg_start + (size_t)n_off * 2 <= size &&
                block_end > msg_start) {
                out->messages      = b;
                out->messages_size = (int)(block_end - msg_start);
            }
        }
    }

    /* FLOOR block (RDT+0x34) — footstep-SOUND regions (FUN_800437d4 consumer).
     * Alias the resident buffer; the lookup (re15_rdt_floor_sound) walks it. */
    out->flr = NULL;
    {
        uint32_t flr_start = read_u32_le(&data[0x34]);
        if (flr_start != 0 && (size_t)flr_start + 2 <= size) {
            out->flr = data + flr_start;
        }
    }

    /* block.blk (RDT+0x38): CANONICALLY UNUSED — the original NEVER dereferences
     * DAT_800ac778+0x38 (disasm-confirmed 2026-06-09 via a full DAT_800ac778 xref
     * sweep: 0x10..0x60 used, 0x38 never). We are byte-exact by NOT parsing it. */
    out->blocks = NULL; out->block_count = 0;

    /* RVD block */
    parse_zones(data, size, zone_start, out);

    /* SCD scripts */
    parse_main_scd(data, size, main_scd_start, sub_scd_start, out);
    out->sub_scd_count = parse_sub_scd_table(data, size, sub_scd_start, out);

    /* Embedded prop models (0x30 table) + room audio banks (0x08..0x1c). */
    parse_props(data, size, out);
    parse_audio(data, size, out);

    return 0;
}

/* Byte-true port of FUN_800437d4 (@0x800437d4): the FLR footstep-region lookup.
 * Entry stream starts at flr+2 (after the u16 count), 12 B/entry, terminates on the
 * first entry with x==0. Match = unsigned `(px16 - x) < width && (pz16 - z) < depth`
 * (the original passes the player X/Z as s16 — `short` params — so truncate). On
 * match returns the soundType byte (entry+8); else 0. */
int re15_rdt_floor_sound(const re15_rdt_t *rdt, int32_t px, int32_t pz)
{
    if (!rdt || !rdt->flr) return 0;
    int32_t spx = (int16_t)px, spz = (int16_t)pz;
    const uint8_t *e = rdt->flr + 2;             /* first entry (skip u16 count) */
    for (;;) {
        int16_t ex = (int16_t)(uint16_t)(e[0] | (e[1] << 8));
        if (ex == 0) return 0;                   /* x==0 terminator */
        int16_t  ez = (int16_t)(uint16_t)(e[2] | (e[3] << 8));
        uint16_t ew = (uint16_t)(e[4] | (e[5] << 8));
        uint16_t ed = (uint16_t)(e[6] | (e[7] << 8));
        if ((uint32_t)(spx - ex) < (uint32_t)ew &&
            (uint32_t)(spz - ez) < (uint32_t)ed) {
            return e[8];                         /* soundType byte */
        }
        e += 12;
    }
}

int re15_rdt_apply_zones_as_aots(const re15_rdt_t *rdt, int slot_offset)
{
    if (!rdt) return 0;
    int installed = 0;
    int slot      = slot_offset;
    for (int i = 0; i < rdt->zone_count; i++) {
        const re15_rdt_zone_t *z = &rdt->zones[i];
        /* PSX-canonical ANCHOR skip (FUN_80014230 @0x80014230, deep-RE
         * 2026-05-29): the RVD zone table is grouped by cam_from; the FIRST
         * zone of each cam_from group is a search-anchor that the PSX scanner
         * never collision-tests (FUN_80014324 finds the group, the scan starts
         * at the SECOND zone). In ROOM1170 the anchors happen to carry cam_to=0,
         * so the old "drop all cam_to==0" filter skipped them by coincidence —
         * but it ALSO dropped real triggers like zone[4] (cam_from=1,cam_to=0,
         * the cut1→cut0 backtrack). cam_to=0 on a NON-anchor zone is a real
         * "switch to cut 0" command. Correct rule: skip zone iff it is the
         * first zone with its cam_from. (Requires RE15_AOT_MAX=64 — the full
         * zone set exceeds 32; the prior naive "skip index 0" also installed
         * the room-spanning anchors → snap-back to cut 0, AND truncated at 32.)
         * Relies on RDT zones being grouped by cam_from (verified ROOM1170). */
        if (i == 0 || rdt->zones[i - 1].cam_from != z->cam_from) continue;
        /* Install RVD camera zones from the TOP of the AOT pool DOWNWARD (high slots),
         * leaving the LOW slots for the SCD's own Aot_set/Door_aot_set (whose ids are
         * 0..20 across all rooms). This keeps the SCD run order UNCHANGED — RVD still
         * installs before main00 — but main00's low-id Aot_set no longer stomps an RVD
         * CAM_SWITCH. (Bug: ROOM1060 main00 does Aot_set(16), which overwrote RVD-at-16 →
         * its stair-descent cut0→cut1 switch was killed; ROOM1170's main00 only uses ids
         * ≤10 so it never hit this.) `slot_offset` is now the FLOOR (don't go below it). */
        slot = RE15_AOT_MAX - 1 - installed;
        if (slot < slot_offset) break;
        if (re15_aot_set_cam_switch(slot, z->cx, z->cz, z->half_w, z->half_h,
                                     z->cam_from, z->cam_to) == 0) {
            /* AQ-round 2026-05-26: install full quad corners on the AOT
             * slot so the scanner can use PSX-canonical point-in-quad
             * (FUN_80014368) instead of AABB approximation. RVD zones
             * in ROOM1170 are non-axis-aligned trapezoids — z5 (1→2)
             * and z8 (2→1) AABBs overlap by ~1300 units in Z, causing
             * cut ping-pong. True quads are disjoint. */
            re15_aot_t *a = &g_aot.slots[slot];
            for (int k = 0; k < 4; k++) {
                a->xs[k] = z->xs[k];
                a->zs[k] = z->zs[k];
            }
            a->has_quad = 1;
            installed++;
        }
    }
    return installed;
}

/* BO-round (Tier-3): per-cut region quad = the ANCHOR (first zone whose
 * cam_from == cut_id) of the RVD zone table. Mirrors PSX FUN_80014324 →
 * DAT_800ac790, which FUN_8002c18c then feeds to the per-object point-in-quad
 * cull. Returns 1 + fills xs/zs on success, 0 if no zone has that cam_from. */
int re15_rdt_get_region_quad(const re15_rdt_t *rdt, int cut_id,
                             int16_t xs_out[4], int16_t zs_out[4])
{
    if (!rdt) return 0;
    for (int i = 0; i < rdt->zone_count; i++) {
        const re15_rdt_zone_t *z = &rdt->zones[i];
        if ((int)z->cam_from == cut_id) {   /* first match = anchor */
            for (int k = 0; k < 4; k++) { xs_out[k] = z->xs[k]; zs_out[k] = z->zs[k]; }
            return 1;
        }
    }
    return 0;
}
