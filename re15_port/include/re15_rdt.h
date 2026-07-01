/*
 * RE1.5 Rebuilt — RDT (Room Data Table) parser (Phase 4.5.9, 2026-05-19).
 *
 * The RDT is RE1.5's per-room container: header counts, camera cuts
 * (RID), trigger zones (RVD), SCD scripts, lighting, collision,
 * messages, model pointers, audio cues. We parse the MINIMUM needed
 * for the Phase 4.4.7 demo to drive itself off real RE1.5 data
 * instead of hardcoded ROOM1100 cuts:
 *
 *   1. Header (offsets 0x00..0x5F) — counts + address table
 *   2. RID block (camera cuts, 32 B/entry, count = header.nCut)
 *   3. RVD block (trigger zones, 20 B/entry, 0xFFFFFFFF terminator)
 *   4. SCD main script entry (pointer-table at +0x40 → bytecode body)
 *
 * Other subsystems (lighting, collision, messages, models) are deferred
 * to later phases. The parser walks the RDT in-place — no allocation;
 * `re15_rdt_t` just holds pointers into the source buffer.
 *
 * Spec source: src/main/java/de/re15/extractors/RDTExtractor.java
 * (Header.read @ L991-1004, Addresses.read @ L1074-1098).
 */

#ifndef RE15_RDT_H
#define RE15_RDT_H

#include <stdint.h>
#include <stddef.h>
#include "re15_camera.h"

/* Per-zone runtime view. AQ-round (2026-05-26): store full 4-vertex quad
 * (xs[4], zs[4]) per RVD entry so the AOT scanner can do PSX-canonical
 * point-in-quad test instead of AABB approximation. RVD trapezoids in
 * ROOM1170 are deliberately non-axis-aligned and DISJOINT; AABB-collapse
 * caused overlaps → ping-pong cuts (Cut 1↔Cut 2 in Leon's south walk).
 * Per PSX FUN_80014368 (4-corner cross-product). The cx/cz/half_w/half_h
 * fields are kept for backward-compat scan paths (DOOR/ITEM/GENERIC) that
 * use AABB. */
typedef struct {
    int16_t  xs[4];        /* quad vertices (LE-decoded from RDT) */
    int16_t  zs[4];
    int32_t  cx;           /* AABB center (derived for DOOR/ITEM if reused) */
    int32_t  cz;
    int32_t  half_w;
    int32_t  half_h;
    uint8_t  cam_from;     /* source cut id */
    uint8_t  cam_to;       /* target cut id (0 → terminator-decorative) */
} re15_rdt_zone_t;

#define RE15_RDT_MAX_ZONES 64   /* [#32a] per-room RVD entry cap. FUN_80014230 @0x80014230
                                 * has NO counter/0x20-cap — the scan is sentinel-bounded
                                 * only; 32 truncated large rooms. 64 matches RE15_AOT_MAX. */

/* SCA collision entry (BIO 1.5 / "BH2 prototype" layout, lib_sca v1.5 — 12 B,
 * naturally packed, no padding). The room's walkable/collision geometry: each
 * entry is a shape at corner (x,z) with extent (width on X, density on Z), a
 * shape `type` (1=rectangle, 3=circle, diagonals/slope/stairs/climb), `u0`/`u1`
 * flag bytes, and a `floor` height level. Pointer aliases straight into the
 * resident RDT buffer (matches the on-disc bytes 1:1). */
typedef struct {
    uint16_t width;     /* X extent          */
    uint16_t density;   /* Z extent (depth)  */
    int16_t  x;         /* corner X          */
    int16_t  z;         /* corner Z          */
    uint8_t  type;      /* shape id          */
    uint8_t  u0;        /* flag bit-array    */
    uint8_t  u1;        /* flag bit-array    */
    uint8_t  floor;     /* floor level       */
} re15_sca_entry_t;

typedef struct {
    /* Header counters (offsets 0x00..0x06) */
    uint8_t nSprite, nCut, nOmodel, nItem, nDoor, nRoom_at, reverb_lv;

    /* Camera cut table — pointer into the source buffer, byte-for-byte
     * matches re15_camera_cut_t (BIO15, 32 B / entry). */
    const re15_camera_cut_t *cuts;
    int                      cut_count;

    /* Zone table — converted into AABB rects at parse time. cam_from /
     * cam_to are kept so the renderer can dispatch on context. */
    re15_rdt_zone_t          zones[RE15_RDT_MAX_ZONES];
    int                      zone_count;

    /* Main SCD script entry — feed to scd_thread_start(0, …). */
    const uint8_t           *main_scd;
    size_t                   main_scd_size;
    int                      sub_scd_count;
    /* Phase 4.5.9-E: sub SCD scripts. Pointers into the source buffer;
     * caller spawns them as concurrent threads via scd_thread_start. */
#define RE15_RDT_MAX_SUB_SCD 16
    const uint8_t           *sub_scd[RE15_RDT_MAX_SUB_SCD];

    /* SCA collision (room walkable/blocker geometry). Parsed from RDT offset
     * 0x20: 24-B header {ceilingX u16, ceilingZ u16, count[5] u32} then the
     * entries (sum of the 5 group counts). `sca` aliases into the resident RDT
     * buffer. See re15_collision.c. */
    const re15_sca_entry_t  *sca;
    int                      sca_count;
    int                      sca_rgn[5];   /* the 5 per-quadrant partition counts (FUN_8003aea0) */
    uint16_t                 ceiling_x;
    uint16_t                 ceiling_z;

    /* Per-cut NCCT lighting set (light.lit). Parsed from RDT offset 0x2c — the
     * LIGHT block (verified: the 520-B room1170 light.lit is byte-for-byte at
     * RDT+0x2c's target; the runtime room-init reads it with stride 40). Size =
     * cut_count * 40. `lights` aliases the resident RDT buffer; feed to
     * re15_light_parse for per-room lighting. */
    const uint8_t           *lights;
    int                      lights_size;

    /* Per-room MESSAGE table (.msg subtitle/examine text). Parsed from RDT offset
     * 0x3c. Standard RE format: u16 off[n_off] where off[0] = n_off*2 (table size)
     * → message COUNT = off[0]/2 - 1; message i = block[off[i] .. off[i+1]] (the
     * .msg-encoded body). `messages` aliases the resident RDT buffer; SCD
     * Message_on(N) indexes it directly. (Verified: room1170 msg0 = the intro's
     * first subtitle, byte-identical to the old incbin's msg0.) */
    const uint8_t           *messages;
    int                      messages_size;

    /* ANIMATION (RBJ) — the per-room CINEMATIC animation container, parsed from RDT
     * header offset 0x5C (animationStart, the last section address; RDTExtractor
     * writeAnimation:362-368 slices exactly this). RE1.5 has NO standalone RBJ files
     * on the disc — every room's cinematic clip table + keyframe pool live INSIDE its
     * RDT here (Irons kneel = ROOM1150, the helipad intro = ROOM1170, …). Container
     * layout (re15_emd_parse_rbj / FUN_8001b3f8): u32 dirOff, u32 recordCount(low byte),
     * then recordCount*8-B directory at +dirOff → total length = dirOff + recordCount*8
     * (the directory sits at the container's END; verified byte-identical to the extracted
     * ROOM1170.RBJ). `animation` aliases the resident RDT buffer. NULL/0 if absent/odd. */
    const uint8_t           *animation;
    int                      animation_size;

    /* FLOOR (floor.flr, RDT offset 0x34) — FOOTSTEP-SOUND region table (NOT floor
     * height; height comes from the band formula / SCA cells). Format (verified vs
     * FUN_800437d4): u16 count, then 12-B entries {s16 x, s16 z, u16 width, u16
     * depth, u8 soundType, u8[3] pad}; iteration starts at +2 and TERMINATES on the
     * first entry whose x==0. The lookup (re15_rdt_floor_sound) returns the
     * soundType of the rect containing the player → the walk handler plays footstep
     * SE `base + (soundType & 0x7f)`. `flr` aliases the resident RDT buffer. */
    const uint8_t           *flr;        /* RDT+0x34 → floor.flr (footstep regions) */

    /* Embedded room PROP models (RDT+0x30 → model pointer table). The table is
     * 2*nOmodel u32 LE offsets, alternating TIM,MD1 (RDTExtractor.parseModelPointers
     * @ RDTExtractor.java:951-980). Each body is sliced [offset .. next-boundary],
     * where the boundary set = the RDT header directory (0x08..0x5c) UNION the model
     * table's own entries (so a TIM ends where its MD1 begins, the last MD1 ends at
     * the next header section). Pointers alias the resident RDT buffer → byte-true,
     * no copy. prop_count == nOmodel. Lets the engine load props straight from the
     * RDT instead of incbin/CD per room. */
#define RE15_RDT_MAX_PROPS 16
    const uint8_t           *prop_md1[RE15_RDT_MAX_PROPS];
    int                      prop_md1_size[RE15_RDT_MAX_PROPS];
    const uint8_t           *prop_tim[RE15_RDT_MAX_PROPS];
    int                      prop_tim_size[RE15_RDT_MAX_PROPS];
    int                      prop_count;

    /* Embedded room AUDIO banks (VAB). Two banks: snd0 (EDT@0x08, VH@0x0c, VB@0x10)
     * and snd1 (EDT@0x14, VH@0x18, VB@0x1c). Each component sliced [offset ..
     * next-header-boundary] (RDTExtractor.writeSound @ RDTExtractor.java:228-264).
     * Pointers alias the resident RDT buffer. NOTE: the snd1 VB carries a 24-byte
     * tail (snd0.snd + snd1.snd metadata, 12 B each) — snd_vb_size[1] INCLUDES it,
     * matching the on-disc bytes; a VAB upload that needs the bare body should drop
     * the trailing 24 B (see RDTExtractor:248-263). The footstep SE uses bank 0. */
    const uint8_t           *snd_vh[2];
    int                      snd_vh_size[2];
    const uint8_t           *snd_vb[2];
    int                      snd_vb_size[2];
    const uint8_t           *snd_edt[2];
    int                      snd_edt_size[2];

    /* Base of the resident RDT buffer + its size — for offset-relative consumers
     * like re15_pri_parse_section (sprite.pri sections are addressed by the camera
     * cut's RDT-relative pri_offset). Set by re15_rdt_parse(out->raw=data). */
    const uint8_t           *raw;
    int                      raw_size;

    /* block.blk (RDT offset 0x38) — CANONICALLY UNUSED. The original engine NEVER
     * dereferences DAT_800ac778+0x38 (disasm-confirmed 2026-06-09: a full xref
     * sweep of every DAT_800ac778 offset finds 0x10..0x60 used but NOT 0x38; room
     * load FUN_8001d600/FUN_800396fc, the collision FUN_8003aea0, and the per-frame
     * loops all skip it). So we are byte-exact by NOT parsing it; the 12-B layout
     * was only a speculative BLKParser.java guess. `blocks` stays NULL. */
    const int16_t           *blocks;     /* always NULL (block.blk is unused) */
    int                      block_count;
} re15_rdt_t;

/* Parse `data` (size bytes) into `out`. Returns 0 on success, negative
 * on truncation / out-of-range fields. Validates pointer-table sanity
 * so a malformed RDT can't run the SCD VM off the end of the buffer. */
int re15_rdt_parse(const uint8_t *data, size_t size, re15_rdt_t *out);

/* Install all non-decorative RVD zones from `rdt` as CAM_SWITCH AOTs
 * starting at `slot_offset`. Skips zones with cam_to=0 (decorative per
 * ZoneRvdParser:71-73). Returns the number of AOTs installed. */
int re15_rdt_apply_zones_as_aots(const re15_rdt_t *rdt, int slot_offset);

/* Per-cut region quad (anchor zone with cam_from==cut_id) for the prop/NPC
 * region cull. Returns 1 + fills xs/zs[4], 0 if none. (PSX FUN_80014324.) */
int re15_rdt_get_region_quad(const re15_rdt_t *rdt, int cut_id,
                             int16_t xs_out[4], int16_t zs_out[4]);

/* FLOOR.flr footstep-region lookup — byte-true port of FUN_800437d4 (@0x800437d4).
 * Returns the soundType byte of the FLR rect containing (px,pz), or 0 if none / no
 * FLR block. Used by the footstep-SE trigger: SE id = base + (soundType & 0x7f). */
int re15_rdt_floor_sound(const re15_rdt_t *rdt, int32_t px, int32_t pz);

#endif /* RE15_RDT_H */
