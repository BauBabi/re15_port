/*
 * RE1.5 Rebuilt — player vs room SCA collision.
 *
 * FAITHFUL port of the original PSX routines (ghidra_source_original_V2.txt):
 *   FUN_8003b0a4  main resolver  (re15_collision_constrain)
 *   FUN_8003b068  quadrant select (quadrant_of)
 *   FUN_8003bca8  rect push-out  (push_rect)
 *   FUN_8003d6a8  circle push-out (push_circle)
 *   FUN_8003b7f0  floor query    (re15_collision_on_floor)
 *
 * MODEL (restored 2026-06-07 after a 135-agent RE + a raw-byte geometry check):
 * the player floor BAND lives in a FIXED state (player+0x82, set from the spawn/
 * door Y via band_from_y; ROOM1170 = band 4). The resolver iterates the cells and
 * for each cell whose band EQUALS the player band (strict ==, band = floor>>4) and
 * whose u0 mask passes (1 & u0, =1 solid / =0 status-only) it PUSHES the player OUT
 * of that shape (motion-coded, depth-guarded). The player walks in the band-MATCH-
 * FREE complement (the big visual-floor cells are a DIFFERENT band and are skipped).
 * Raw ROOM1170 bytes confirm the player at the helipad centre (4990,-7350) AND the
 * outdoor door-0 landing (-11710,-26500) are both in band-4-FREE space, so band-4
 * cells = walls and push-out is correct (USER-CONFIRMED-good on the helipad). The
 * earlier containment experiment (confine to the band-matching union) was WRONG —
 * it let the player walk too far because cells0,1 (the visual floor, band3) are a
 * larger different-band region the band-4 player should not be confined to.
 *
 * Player radius PR = 450 (DAT_80073e94[6], code-verified). Object (Obj_model_set
 * prop) collision is a SEPARATE pass below (FUN_8002cabc) for the helipad box.
 */
#include "re15_collision.h"
#include <stdio.h>
#include <stdlib.h>
#include "re15_scd.h"        /* g_scd.props — the Obj_model_set object list */
#include "re15_math.h"       /* re15_squareroot0 — the engine's ONLY sqrt (BIOS 0x80065f60) */
#include "re15_actor.h"      /* g_actors — FUN_8002cfd4 liest player[0x00]/[0x40]/[0x44] */
#include "re15_climb.h"      /* re15_climb_standing_probe — DAT_800ac788 (@0x80031d20) */
#ifndef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#define PR 450

/* FUN_8003b068: 2-bit quadrant from sign(pos - ceiling) (origin = 0). */
static int quadrant_of(int32_t px, int32_t pz, int16_t ceil_x, int16_t ceil_z)
{
    unsigned zb = (unsigned)(pz - (int32_t)ceil_z) & 0x80000000u;
    unsigned xb = (unsigned)(px - (int32_t)ceil_x) & 0x80000000u;
    return (int)((zb | (xb >> 1)) >> 0x1e);
}

/* FUN_8003bca8 — rectangle push-out. Keeps the player on whichever side of the
 * cell his MOTION says (penetration-sign ^ motion-sign 2-bit code), with the
 * +0x12 skin margin and the 0x321(401)/800 axis-preference thresholds, and the
 * snap-to-prev block when he moved straight into a face. (origin = 0.) */
static int push_rect(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                     int32_t prevx, int32_t prevz, int32_t r)
{
    r &= 0xffff;
    int32_t liveX = *lx;
    int32_t penX  = (((int32_t)e->width + (int32_t)e->x + r) - liveX) + 0x12;
    int32_t pxm   = (((int32_t)e->x - r) - liveX) - 0x12;
    if (-pxm < penX) penX = pxm;
    int32_t liveZ = *lz;
    int32_t penZ  = (((int32_t)e->density + (int32_t)e->z + r) - liveZ) + 0x12;
    int32_t pzm   = (((int32_t)e->z - r) - liveZ) - 0x12;
    if (-pzm < penZ) penZ = pzm;

    unsigned code = ((unsigned)(penX ^ (liveX - prevx)) >> 0x1f)
                  | (((unsigned)(penZ ^ (liveZ - prevz)) >> 0x1e) & 2);
    int32_t ax = penX < 0 ? -penX : penX;
    int32_t az = penZ < 0 ? -penZ : penZ;

    if (code == 2) {
        if (((unsigned)(az + 400) & 0xffff) < 0x321) { *lz = penZ + liveZ; }      /* push Z */
        else if (ax < az)                            { *lx = penX + *lx; }        /* min-axis X */
        else                                         { *lz = penZ + *lz; }        /* min-axis Z */
        return 1;
    }
    if (code == 1) {
        if (800 < ((unsigned)(ax + 400) & 0xffff)) {                              /* deep → min-axis */
            if (ax < az) *lx = penX + *lx;
            else         *lz = penZ + *lz;
        } else { *lx = penX + liveX; }                                            /* push X */
        return 1;
    }
    if (code == 3) {                                                              /* both oppose → min-axis */
        if (ax < az) *lx = penX + *lx;
        else         *lz = penZ + *lz;
        return 1;
    }
    /* code == 0: moved straight into a face. If he didn't actually move, min-axis
     * un-stick; else snap back to prev (block). */
    if (*lx == prevx && *lz == prevz) {
        if (ax < az) *lx = penX + *lx;
        else         *lz = penZ + *lz;
        return 1;
    }
    *lx = prevx; *lz = prevz;
    return 0;
}

/* FUN_8003d6a8 — circle push-out (solid obstacle). Radial offset from the CURRENT
 * position by penetration/(dist+1). (origin = 0.) */
static int push_circle(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz, int32_t r)
{
    int32_t cr = (int32_t)e->width >> 1;
    int32_t dx = *lx - ((int32_t)e->x + cr);
    int32_t dz = *lz - ((int32_t)e->z + cr);
    /* dist via the BIOS SquareRoot0 (0x8003d724 jal 0x80065f60) — NOT an exact
     * floor(sqrt). The PSX passes the low 32 bits of the mult (mflo), so mask to
     * u32; the table approximation under-estimates by ±(1..N) and that reaches the
     * committed push (pen, denom, and the pen<1 gate). Audit wf_f066b2ae. */
    int32_t dist = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
    int32_t pen  = (cr + (r & 0xffff)) - dist;
    if (pen < 1) return 0;
    int32_t denom = dist + 1;
    *lx = (dx * pen) / denom + *lx;
    *lz = (dz * pen) / denom + *lz;
    return 1;
}

/* ⚠️ BEST-EFFORT raw-MIPS port (2026-06-09), UNVERIFIED until a slope-room runs.
 * LAB_8003d7e8 (type 8) / LAB_8003d930 (type 9) are CAPSULE dispatchers, NOT slope
 * push-out themselves: they classify the player along ONE axis vs the cell's two
 * end-edges and forward to the (verified) rect handler in the middle or the circle
 * handler at each rounded end. Mechanically transliterated from ghidra1_V2.txt
 * 8003d7e8/8003d930; resolved to our origin=0 frame (player abs = *lx/*lz, like
 * push_rect). ROOM1170 has NO type 8/9 cells (zero regression); other rooms with
 * type 8 (8 rooms) / type 9 (6 rooms) need in-game verification. The actual slope
 * push-out (types 2/4/5/6/7) is now IMPLEMENTED byte-true (push_diag2..7, dispatched re15_collision.c L575-579; RE workflow wf_b5520814). */
static int push_caps8(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    /* X-axis capsule: rect when player X is between the two X-edges, circle cap past each. */
    int32_t x_max = (int16_t)((int32_t)e->width + (int32_t)e->x - ((int32_t)e->density >> 1));
    int32_t x_min = (int16_t)((int32_t)e->x + ((int32_t)e->density >> 1));
    unsigned c = ((((unsigned)(*lx - x_max)) & 0x80000000u)
                | ((((unsigned)(*lx - x_min)) & 0x80000000u) >> 1)) >> 0x1e;
    if (c == 2) return push_rect(e, lx, lz, prevx, prevz, r);   /* between → rect */
    if (c == 1) return 0;                                       /* impossible → no-op */
    re15_sca_entry_t tmp = *e;                                  /* circle cap (temp cell @DAT_8008f6a8) */
    tmp.width   = e->density;                                   /* temp[0] = height */
    tmp.density = e->density;                                   /* temp[2] = height */
    tmp.z       = e->z;                                         /* temp[6] = z */
    tmp.x = (c == 0)                                            /* temp[4] */
          ? (int16_t)((int32_t)e->width + (int32_t)e->x - (int32_t)e->density)  /* right cap */
          : e->x;                                               /* c==3: left cap */
    return push_circle(&tmp, lx, lz, r);
}

static int push_caps9(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    /* Z-axis capsule (type-9 = type-8 with X/Z swapped). */
    int32_t z_max = (int16_t)((int32_t)e->density + (int32_t)e->z - ((int32_t)e->width >> 1));
    int32_t z_min = (int16_t)((int32_t)e->z + ((int32_t)e->width >> 1));
    unsigned c = ((((unsigned)(*lz - z_max)) & 0x80000000u)
                | ((((unsigned)(*lz - z_min)) & 0x80000000u) >> 1)) >> 0x1e;
    if (c == 2) return push_rect(e, lx, lz, prevx, prevz, r);
    if (c == 1) return 0;
    re15_sca_entry_t tmp = *e;
    tmp.width   = e->width;                                     /* temp[0] = width */
    tmp.density = e->width;                                     /* temp[2] = width */
    tmp.x       = e->x;                                         /* temp[4] = x */
    tmp.z = (c == 0)                                            /* temp[6] */
          ? (int16_t)((int32_t)e->density + (int32_t)e->z - (int32_t)e->width)  /* far cap */
          : e->z;                                               /* c==3: near cap */
    return push_circle(&tmp, lx, lz, r);
}

/* --- floor band state (mirrors player+0x82; set from spawn/door Y, fixed otherwise) --- */
static int s_coll_band = -1;
void re15_collision_reset_band(void)            { s_coll_band = -1; }
void re15_collision_set_band(int band)          { s_coll_band = band; }
int  re15_collision_band_from_y(int32_t y)      { return -(y / 0x708); }
int  re15_collision_debug_band(void)            { return s_coll_band; }
void re15_collision_ensure_band(int32_t y)      { if (s_coll_band < 0) s_coll_band = -(y / 0x708); }

/* Scan the SCA cells for the lowest/highest band present (band = floor>>4).
 * The stair uses this to find the floor a descent/ascent lands on (mirrors the
 * original band-walk in FUN_8001c2dc, which steps the band toward the next
 * populated floor). The 5 quadrant groups are byte-identical duplicates, so a
 * full linear scan over sca_count is fine. */
void re15_collision_band_range(const re15_rdt_t *rdt, int *min_band, int *max_band)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return;
    int lo = 255, hi = -1;
    for (int i = 0; i < rdt->sca_count; i++) {
        int b = rdt->sca[i].floor >> 4;
        if (b < lo) lo = b;
        if (b > hi) hi = b;
    }
    if (hi < 0) return;                 /* no cells */
    if (min_band) *min_band = lo;
    if (max_band) *max_band = hi;
}

int re15_collision_next_band_below(const re15_rdt_t *rdt, int cur)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return cur;
    int best = -1;
    for (int i = 0; i < rdt->sca_count; i++) {
        int b = rdt->sca[i].floor >> 4;
        if (b < cur && b > best) best = b;
    }
    return best < 0 ? cur : best;
}

int re15_collision_next_band_above(const re15_rdt_t *rdt, int cur)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return cur;
    int best = 256;
    for (int i = 0; i < rdt->sca_count; i++) {
        int b = rdt->sca[i].floor >> 4;
        if (b > cur && b < best) best = b;
    }
    return best == 256 ? cur : best;
}

/* FUN_8003b7f0 / FUN_8001c6e8 containment floor query: the band of the cell that
 * contains (x,z), scanning DOWN from start_band. Cell AABB = [x..x+width] ×
 * [z..z+density] (the unsigned point-in-rect idiom the original uses). */
int re15_collision_floor_band_at(const re15_rdt_t *rdt,
                                 int32_t x, int32_t z, int start_band)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return -1;
    for (int b = start_band; b >= 0; b--) {
        for (int i = 0; i < rdt->sca_count; i++) {
            const re15_sca_entry_t *e = &rdt->sca[i];
            if ((e->floor >> 4) != b) continue;
            if ((unsigned)(x - (int32_t)e->x) < (unsigned)e->width &&
                (unsigned)(z - (int32_t)e->z) < (unsigned)e->density)
                return b;
        }
    }
    return -1;
}

/* FUN_8003b7f0 — floor query: 1 if (x,z) sits on a cell of the player's band. */
int re15_collision_on_floor(const re15_rdt_t *rdt, int32_t x, int32_t z)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0 || s_coll_band < 0) return 1;
    int q = quadrant_of(x, z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (s_coll_band != (e->floor >> 4)) continue;
        if ((unsigned)(x - ((int32_t)e->x)) < (unsigned)e->width &&
            (unsigned)(z - ((int32_t)e->z)) < (unsigned)e->density) return 1;
    }
    return 0;
}

/* ---- FUN_8003b7f0 mit explizitem Band/Radius (Attributwort statt bool) -------------
 * Byte-true: das Original liest die Zelle als 6 x u16 (Stride 12) und vergleicht das
 * Band als VORZEICHENBEHAFTETES Nibble aus Bit 15..12 des Wortes @zelle+0x0a
 * (`sll v0,v0,16; sra v0,v0,28` @0x8003b8a8-ac) gegen `a2 = band & 0xff`
 * (@0x8003b88c). Im Port ist dieses Wort `u1 | (floor<<8)`. */
uint16_t re15_collision_floor_typeword(const re15_rdt_t *rdt, int32_t x, int32_t z,
                                       int band, int32_t r)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    int q = quadrant_of(x, z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    const int32_t t0 = (int32_t)(int16_t)r;              /* `sll/sra 16` @0x8003b888-90 */
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        uint16_t w = (uint16_t)((uint16_t)e->u1 | ((uint16_t)e->floor << 8));
        int cb = (int)(((int32_t)((uint32_t)w << 16)) >> 28);      /* @0x8003b8a8-ac */
        if (cb != (band & 0xff)) continue;                         /* @0x8003b8b0 */
        if ((uint32_t)(x - ((int32_t)e->x - t0)) >=
            (uint32_t)((int32_t)e->width   + 2 * t0)) continue;    /* @0x8003b8b8-d4 */
        if ((uint32_t)(z - ((int32_t)e->z - t0)) >=
            (uint32_t)((int32_t)e->density + 2 * t0)) continue;    /* @0x8003b8dc-f8 */
        return w;                                                  /* @0x8003b900-04 */
    }
    return 0;                                                      /* @0x8003b91c */
}

/* ---- FUN_8002da4c @0x8002da4c — Punkt gegen Prop-Box mit Band-Gate ------------------
 *   @0x8002da50-58 `obj[0x82] != (band & 0xff)` -> 0
 *   @0x8002da60-88 dx = x - (box.cx + obj.x) ; dz = z - (box.cz + obj.z)
 *   @0x8002da8c-a4 `(u32)(2*(hx+m)) < (u32)(dx + (hx+m))` -> 0   (unsigned-Wrap-Trick)
 *   @0x8002dab4-c0 dito fuer Z, Rueckgabe = !(...)                                    */
int re15_collision_prop_box_hit(int prop_idx, int32_t x, int32_t z,
                                int32_t margin, int band)
{
    if (prop_idx < 0 || prop_idx >= 16) return 0;
    const int32_t ox = g_scd.props[prop_idx].x, oz = g_scd.props[prop_idx].z;
    if ((int)g_scd.props[prop_idx].band != (band & 0xff)) return 0;
    const int32_t dx = x - ((int32_t)g_scd.props[prop_idx].box_cx + ox);
    const int32_t dz = z - ((int32_t)g_scd.props[prop_idx].box_cz + oz);
    const uint32_t hx = (uint32_t)(uint16_t)g_scd.props[prop_idx].box_hx + (uint32_t)margin;
    if ((uint32_t)(hx << 1) < (uint32_t)((uint32_t)dx + hx)) return 0;
    const uint32_t hz = (uint32_t)(uint16_t)g_scd.props[prop_idx].box_hz + (uint32_t)margin;
    return ((uint32_t)(hz << 1) < (uint32_t)((uint32_t)dz + hz)) ? 0 : 1;
}

/* ---- FUN_8001c6e8 @0x8001c6e8 — Bodenhoehe (SCA-Baender + Objekt-Oberkanten) --------
 * Decompilat: RE_15_Quellcode_V2/FUN_8001c6e8.c. Ablauf je Band (start_band-1 .. 0):
 *   1) `(param_4 & 0x10000) == 0` -> Objekt-Pass ueber den Pool (rueckwaerts),
 *      `FUN_8002da4c(p, obj, (s16)((r - (r>>31)) >> 1), band)`; Treffer ->
 *      `(s16)(obj.y + box.hy * -2)` = Objekt-OBERKANTE.
 *   2) SCA-Zellen des Quadranten: `band<<12 == ((s16)zelle[0x0a] & 0xf002)` UND
 *      `((s16)zelle[0x08] & mask) != 0` UND Punkt in der um r GESCHRUMPFTEN Zelle
 *      -> `(s16)((band+1) * -0x708)`.
 * Fallen alle Baender durch -> 0.                                                     */
int16_t re15_collision_room_coll(const re15_rdt_t *rdt, int32_t x, int32_t z,
                                 int32_t r, int start_band, uint32_t mask)
{
    const int32_t rr = (int32_t)(int16_t)r;                  /* iVar9 = param_2 << 16 >> 16 */
    const int32_t rs = (rr < 0) ? -1 : 0;                    /* iVar2 = local_30 >> 0x1f    */
    const int32_t om = (int32_t)(int16_t)((rr - rs) >> 1);   /* Objekt-Margin               */
    int q = 0, start = 0, end = 0;
    if (rdt && rdt->sca && rdt->sca_count > 0) {
        q = quadrant_of(x, z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
        for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
        end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
        if (end > rdt->sca_count) end = rdt->sca_count;
    }
    for (int b = start_band - 1; b >= 0; b--) {
        if (!(mask & 0x10000u)) {
            for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
                if (p >= 16 || !g_scd.props[p].active) continue;
                if (!re15_collision_prop_box_hit(p, x, z, om, b)) continue;
                return (int16_t)((int32_t)g_scd.props[p].y
                                 - 2 * (int32_t)(uint16_t)g_scd.props[p].box_hy);
            }
        }
        for (int i = start; i < end; i++) {
            const re15_sca_entry_t *e = &rdt->sca[i];
            uint16_t w10 = (uint16_t)((uint16_t)e->u1 | ((uint16_t)e->floor << 8));
            uint16_t w08 = (uint16_t)((uint16_t)e->type | ((uint16_t)e->u0 << 8));
            if ((uint32_t)((int32_t)(int16_t)w10 & 0xf002) != (uint32_t)(b << 12)) continue;
            if ((((int32_t)(int16_t)w08) & (int32_t)mask) == 0) continue;
            if ((uint32_t)(x - ((int32_t)e->x + rr)) >=
                (uint32_t)((int32_t)e->width   - 2 * rr)) continue;
            if ((uint32_t)(z - ((int32_t)e->z + rr)) >=
                (uint32_t)((int32_t)e->density - 2 * rr)) continue;
            return (int16_t)((b + 1) * -0x708);
        }
    }
    return 0;
}

int re15_collision_band_centroid(const re15_rdt_t *rdt, int band,
                                 int32_t *cx, int32_t *cz)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    int64_t sx = 0, sz = 0; int n = 0;
    for (int i = 0; i < rdt->sca_count; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if ((e->floor >> 4) != band) continue;
        sx += (int32_t)e->x + (int32_t)e->width / 2;
        sz += (int32_t)e->z + (int32_t)e->density / 2;
        n++;
    }
    if (n == 0) return 0;
    if (cx) *cx = (int32_t)(sx / n);
    if (cz) *cz = (int32_t)(sz / n);
    return 1;
}

/* ================= SCA DIAGONAL / SLOPE cells (types 2/4/5/6/7) =====================
 * Byte-true from RE workflow wf_b5520814 (ghidra1_V2.txt line ranges cited per handler). These were
 * DEFERRED (the player walked through diagonal walls in e.g. ROOM11C0 = 12x type2 + 2x type4 +
 * 1x type6). Each handler has push_rect's signature and mutates *lx/*lz in place. KEY findings:
 *   - the DAT_80010c34/c38 "edge constants" are INERT ZEROS (a scratch-cell init), NOT geometry;
 *     the diagonal is derived from the cell (x_max = e->x+width, z_max = e->z+density, centre).
 *   - origin=0: the resident ref-cell add is dropped, exactly like push_rect/push_circle.
 *   - the /18 pre-scale in the cross products keeps the absolute-coord products inside int32
 *     (byte-true: it affects the exact edge classification, so it is kept). */

/* MIPS `mult;mflo` = LOW 32 bits of the signed product (wraps mod 2^32); make the wrap explicit. */
static int32_t m32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }

/* FUN_8003b068 via the cell CENTRE (corner + half-extent), the classifier type-2 opens with. */
static int quadrant_of_cell(const re15_sca_entry_t *e, int32_t px, int32_t pz)
{
    int16_t cx = (int16_t)((int32_t)e->x + (int32_t)(e->width  >> 1));   /* ghidra 145484-145489 */
    int16_t cz = (int16_t)((int32_t)e->z + (int32_t)(e->density >> 1));  /* ghidra 145486-145491 */
    return quadrant_of(px, pz, cx, cz);
}

/* type-2 shared diagonal-normal projection tail (byte-identical in all four quadrant sub-cases). */
static void diag2_push(int32_t den, int32_t pen, int32_t den2, int32_t *off_x, int32_t *off_z)
{
    int32_t q1    = (den * pen) / den2;      /* ghidra 145559-145574 */
    int32_t q1sq  = q1 * q1;                 /* 145580-145581 */
    int32_t denom = q1sq + pen * pen;        /* 145582-145584 */
    if (denom == 0) { *off_x = 0; *off_z = 0; return; }
    *off_x = (q1 * pen * pen) / denom;       /* 145576-145597 */
    *off_z = (q1sq * pen)     / denom;       /* 145598-145612 */
}

/* LAB_8003d00c @0x8003d00c (ghidra1_V2.txt:145450-145916) — type-2 main diagonal push-out. */
static int push_diag2(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t old_x, int32_t old_z, int32_t r)
{
    (void)old_x; (void)old_z;
    r &= 0xffff;
    int32_t px = *lx, pz = *lz;
    int32_t x_max = (int16_t)((int32_t)e->width   + (int32_t)e->x);
    int32_t z_max = (int16_t)((int32_t)e->density + (int32_t)e->z);
    int32_t cx    = (int16_t)((int32_t)e->x + (int32_t)(e->width   >> 1));
    int32_t cz    = (int16_t)((int32_t)e->z + (int32_t)(e->density >> 1));
    int q = quadrant_of_cell(e, px, pz);
    int32_t den, den2, edge, pcmp, pen, off_x = 0, off_z = 0;
    if (q == 0) {
        int32_t dz = cz - (z_max + r);
        den  = (x_max + r) - cx;
        edge = (dz * (px - cx)) / den;
        pcmp = pz - (z_max + r);
        if (!(pcmp < edge)) return 0;
        pen  = pcmp - edge;
        den2 = (z_max + r) - cz;
        diag2_push(den, pen, den2, &off_x, &off_z);
        *lx = px - off_x;
    } else if (q == 1) {
        int32_t hz  = (z_max + r) - cz;
        int32_t xlo = (int32_t)e->x - r;
        den  = cx - xlo;
        edge = (hz * (px - xlo)) / den;
        pcmp = pz - cz;
        if (!(pcmp < edge)) return 0;
        pen  = pcmp - edge;
        den2 = hz;
        diag2_push(den, pen, den2, &off_x, &off_z);
        *lx = px + off_x;
    } else if (q == 2) {
        int32_t zlo   = (int32_t)e->z - r;
        int32_t den_z = cz - zlo;
        den  = (x_max + r) - cx;
        edge = (den_z * (px - cx)) / den;
        pcmp = pz - zlo;
        if (!(edge < pcmp)) return 0;
        pen  = pcmp - edge;
        den2 = den_z;
        diag2_push(den, pen, den2, &off_x, &off_z);
        *lx = px + off_x;
    } else {
        int32_t zlo = (int32_t)e->z - r;
        int32_t a1v = zlo - cz;
        int32_t xlo = (int32_t)e->x - r;
        den  = cx - xlo;
        edge = (a1v * (px - xlo)) / den;
        pcmp = pz - cz;
        if (!(edge < pcmp)) return 0;
        pen  = pcmp - edge;
        den2 = cz - zlo;
        diag2_push(den, pen, den2, &off_x, &off_z);
        *lx = px - off_x;
    }
    *lz = pz - off_z;
    return 1;
}

/* LAB_8003beb0 (ghidra1_V2.txt:144231-144520) — type-4 anti-diagonal push-out. */
static int push_diag4(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    r &= 0xffff;
    int32_t px = *lx, pz = *lz;
    int32_t ex = e->x, ez = e->z, W = e->width, D = e->density;
    int32_t x_max = ex + W, z_max = ez + D;
    int32_t Q  = m32(-(r + D), (px - ex + r)) / (W + r);
    int32_t s8 = pz - z_max;
    if (!(Q < s8)) return 0;
    int32_t ex18 = ex / 18, ez18 = ez / 18, xm18 = x_max / 18, zm18 = z_max / 18;
    int32_t pxx  = prevx / 18, pzz = prevz / 18;
    int32_t cross = (ez18 - zm18) * pxx - (xm18 - ex18) * pzz - ex18 * ez18 + xm18 * zm18;
    if (cross >= 0) {
        int32_t a2 = s8 - Q;
        int32_t Kx = m32(W + r, a2) / (D + r);
        int32_t denom = m32(Kx, Kx) + m32(a2, a2);
        if (denom == 0) return 1;
        int32_t dx = m32(m32(Kx, a2), a2) / denom;
        int32_t dz = m32(m32(Kx, Kx), a2) / denom;
        *lx = px - dx;
        *lz = pz - dz;
        return 1;
    }
    if (!((uint32_t)(px - ex) < (uint32_t)(W + r))) return 0;
    if (!((uint32_t)(pz - ez) < (uint32_t)(D + r))) return 0;
    int32_t penX = (x_max + r - px) + 0x12;
    int32_t penZ = (z_max + r - pz) + 0x12;
    unsigned code = ((unsigned)(penX ^ (px - prevx)) >> 31)
                  | (((unsigned)(penZ ^ (pz - prevz)) >> 30) & 2);
    int32_t ax = penX < 0 ? -penX : penX, az = penZ < 0 ? -penZ : penZ;
    if (code == 2) {
        if (((unsigned)(az + 0x190) & 0xffff) < 0x321) { *lz = penZ + pz; return 1; }
        if (ax < az) *lx = penX + *lx; else *lz = penZ + *lz;
        return 1;
    }
    if (code == 1) {
        if (((unsigned)(ax + 0x190) & 0xffff) < 0x321) { *lx = penX + px; return 1; }
        if (ax < az) *lx = penX + *lx; else *lz = penZ + *lz;
        return 1;
    }
    if (code == 3) { if (ax < az) *lx = penX + *lx; else *lz = penZ + *lz; return 1; }
    *lx = prevx; *lz = prevz;
    return 0;
}

/* LAB_8003c734 (ghidra1_V2.txt:144830-145138) — type-5 "/" diagonal push-out. */
static int push_diag5(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    r &= 0xffff;
    int32_t X = (int16_t)e->x, Z = (int16_t)e->z;
    int32_t W = (uint16_t)e->width, D = (uint16_t)e->density;
    int32_t XM = (int16_t)(X + W), ZM = (int16_t)(Z + D);
    int32_t DXR = (XM + r) - X, DZR = ZM - (Z - r);
    int32_t PX = *lx, PZ = *lz;
    int32_t LINE  = (DZR * (PX - X)) / DXR;
    int32_t ZTERM = PZ - (Z - r);
    if (!(LINE < ZTERM)) return 0;
    int32_t pX = (int16_t)prevx, pZ = (int16_t)prevz;
    int32_t X18=X/18, Z18=Z/18, XM18=XM/18, ZM18=ZM/18, PRX18=pX/18, PRZ18=pZ/18;
    int32_t det = (ZM18-Z18)*PRX18 - (XM18-X18)*PRZ18 - X18*ZM18 + XM18*Z18;
    if (det >= 0) {
        int32_t PENZ = ZTERM - LINE;
        int32_t PROJ = (DXR * PENZ) / DZR;
        int32_t denom = PROJ*PROJ + PENZ*PENZ;
        if (denom == 0) return 1;
        int32_t dxp = (PROJ*PENZ*PENZ) / denom;
        int32_t dzp = (PROJ*PROJ*PENZ) / denom;
        *lx = PX + dxp;
        *lz = PZ - dzp;
        return 1;
    }
    if ((uint32_t)(PX - (X - r)) >= (uint32_t)(W + r)) return 0;
    if ((uint32_t)(PZ -  Z)      >= (uint32_t)(D + r)) return 0;
    int32_t ex = ((X - r) - PX) - 0x12;
    int32_t ez = ((ZM + r) - PZ) + 0x12;
    int32_t mX = PX - pX, mZ = PZ - pZ;
    unsigned code = (((unsigned)(ex ^ mX)) >> 31) | ((((unsigned)(ez ^ mZ)) >> 30) & 2u);
    int32_t aex = ex < 0 ? -ex : ex, aez = ez < 0 ? -ez : ez;
    if (code == 0) { *lx = pX; *lz = pZ; return 0; }
    if (code == 2) { if (((unsigned)(aez + 400) & 0xffff) < 0x321) { *lz = PZ + ez; return 1; } }
    else if (code == 1) { if (((unsigned)(aex + 400) & 0xffff) < 0x321) { *lx = PX + ex; return 1; } }
    if (aex < aez) *lx = PX + ex; else *lz = PZ + ez;
    return 1;
}

/* LAB_8003cb9c (ghidra1_V2.txt:145139-145449) — type-6 main-diagonal push-out. */
static int push_diag6(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    r &= 0xffff;
    int32_t x = (int16_t)e->x, z = (int16_t)e->z;
    int32_t x_max = (int16_t)((int32_t)e->x + (int32_t)e->width);
    int32_t z_max = (int16_t)((int32_t)e->z + (int32_t)e->density);
    int32_t px = *lx, pz = *lz;
    int32_t s0   = (z_max + r) - z;
    int32_t divW = x_max - (x - r);
    int32_t s7q  = (s0 * (px - (x - r))) / divW;
    int32_t s3   = pz - z;
    if (!(s3 < s7q)) return 0;
    int32_t X=x/18, Xm=x_max/18, Z=z/18, Zm=z_max/18, PX=prevx/18, PZ=prevz/18;
    int32_t cross = (Zm - Z) * PX - (Xm - X) * PZ - X * Zm + Xm * Z;
    if (cross > 0) {
        if (!((uint32_t)(px - x)       < (uint32_t)((int32_t)e->width   + r))) return 0;
        if (!((uint32_t)(pz - (z - r)) < (uint32_t)((int32_t)e->density + r))) return 0;
        int32_t penX = ((x_max + r) - px) + 0x12;
        int32_t penZ = ((z - r) - pz) - 0x12;
        unsigned code = ((unsigned)(penX ^ (px - prevx)) >> 31)
                      | (((unsigned)(penZ ^ (pz - prevz)) >> 30) & 2);
        int32_t ax = penX < 0 ? -penX : penX, az = penZ < 0 ? -penZ : penZ;
        if (code == 0) { *lx = prevx; *lz = prevz; return 0; }
        if (code == 1 && ((unsigned)(ax + 0x190) & 0xffff) < 0x321) { *lx = penX + px; return 1; }
        if (code == 2 && ((unsigned)(az + 0x190) & 0xffff) < 0x321) { *lz = penZ + pz; return 1; }
        if (ax < az) *lx = penX + px; else *lz = penZ + pz;
        return 1;
    }
    int32_t a2    = (pz - z) - s7q;
    int32_t q     = (divW * a2) / s0;
    int32_t denom = q * q + a2 * a2;
    if (denom == 0) return 1;
    int32_t off_x = (q * a2 * a2) / denom;
    int32_t off_z = (q * q * a2) / denom;
    *lx = px + off_x;
    *lz = pz - off_z;
    return 1;
}

/* LAB_8003c2cc (ghidra1_V2.txt:144521-144829) — type-7 anti-diagonal push-out. */
static int push_diag7(const re15_sca_entry_t *e, int32_t *lx, int32_t *lz,
                      int32_t prevx, int32_t prevz, int32_t r)
{
    r &= 0xffff;
    int32_t px = *lx, pz = *lz;
    int32_t x0 = (int16_t)e->x, z0 = (int16_t)e->z;
    int32_t x_max = (int16_t)((int32_t)e->x + (int32_t)e->width);
    int32_t z_max = (int16_t)((int32_t)e->z + (int32_t)e->density);
    int32_t zr = z_max + r, widthr = (x_max + r) - x0;
    int32_t s3 = ((int32_t)(z0 - zr) * (int32_t)(px - x0)) / widthr;
    int32_t s6 = pz - zr;
    if (!(s6 < s3)) return 0;
    int32_t ez18 = (int16_t)(z0 / 18), zmax18 = (int16_t)(z_max / 18);
    int32_t ex18 = (int16_t)(x0 / 18), xmax18 = (int16_t)(x_max / 18);
    int32_t px18 = prevx / 18, pz18 = prevz / 18;
    int32_t cross = (ez18 - zmax18) * px18 - (xmax18 - ex18) * pz18 - ex18 * ez18 + xmax18 * zmax18;
    if (cross > 0) {
        if (!((uint32_t)(px - (x0 - r)) < (uint32_t)((int32_t)e->width   + r))) return 0;
        if (!((uint32_t)(pz - (z0 - r)) < (uint32_t)((int32_t)e->density + r))) return 0;
        int32_t ax = (x0 - r - px) - 0x12, az = (z0 - r - pz) - 0x12;
        unsigned code = ((unsigned)(ax ^ (px - prevx)) >> 0x1f)
                      | (((unsigned)(az ^ (pz - prevz)) >> 0x1e) & 2);
        int32_t aax = ax < 0 ? -ax : ax, aaz = az < 0 ? -az : az;
        if (code == 2) {
            if (((unsigned)(aaz + 0x190) & 0xffff) < 0x321) *lz = az + pz;
            else if (aax < aaz) *lx = ax + px; else *lz = az + pz;
            return 1;
        }
        if (code == 1) {
            if (((unsigned)(aax + 0x190) & 0xffff) < 0x321) *lx = ax + px;
            else if (aax < aaz) *lx = ax + px; else *lz = az + pz;
            return 1;
        }
        if (code == 3) { if (aax < aaz) *lx = ax + px; else *lz = az + pz; return 1; }
        *lx = prevx; *lz = prevz;
        return 0;
    }
    int32_t a2 = s6 - s3;
    int32_t p  = (widthr * a2) / ((z_max + r) - z0);
    int32_t denom = p * p + a2 * a2;
    if (denom == 0) return 1;
    int32_t push_x = (p * a2 * a2) / denom;
    int32_t push_z = (p * p * a2) / denom;
    *lx = px - push_x;
    *lz = pz - push_z;
    return 1;
}

/* FUN_8003b0a4 — main resolver (PUSH-OUT of band-matching solid cells). Byte-true it is fully
 * parameterized on (a1=radius, a2=solid-mask, a3=entity): the CELL is kept solid iff (mask & u0),
 * the cell's band must equal the ENTITY's +0x82 band, and the broad-phase/push are inflated by the
 * ENTITY's own radius box[+0x78][6]. The PLAYER caller passes (450, mask 1, its band); every enemy
 * caller passes (box[6]=hit_radius_min, mask 4 @entity+0x1d7, its own band @+0x82 = band_from_y). The
 * port had baked in the player values (r=450, mask=1, band=s_coll_band) for ALL actors, so enemies
 * were clamped against the wrong cells (ROOM11C0 maggot walked through its enemy-solid u0=0x04 wall)
 * and with the wrong radius (a 1600-radius maggot under-clamped at 450 sank into thin walls). */
/* FUN_8001bf04 (@0x8001bf04) — der RICHTUNGS-Code, den der Klemmpfad in die obere Nibble von
 * entity+0x90 legt. Zwei Sign-Bit-Paare klassifizieren den Aktor gegen die Zellen-Spanne:
 *
 *   cx = signbit(px - (cell.x + cell.width)) << 1 | signbit(px - cell.x)
 *   cx == 3 (links der Zelle)                 -> 0x000
 *   cx == 0 (rechts der Zelle)                -> 0x800
 *   cx == 2 (X INNERHALB): dasselbe fuer Z
 *        cz == 3 (vor  cell.z)                -> 0xc00
 *        cz == 0 (hinter cell.z + density)    -> 0x400
 *        sonst (auch Z innerhalb)             -> 0xf00   (uVar2-Default)
 *   sonst                                     -> 0xf00
 *
 * Die Leser holen den Winkel mit `(+0x90 & 0xf0) << 4` exakt wieder heraus (Zombie @0x8010207c,
 * Hund @0x8011038c, ZGirl @0x8010bcb0), d.h. der Wert IST ein 12-bit-Yaw (4096 = 360 Grad).
 *
 * ⚠️ Original-Position = entity_pos + `*(short(*)[3])(entity+0x7c)` (@0x8003b148-74). Der Port
 * modelliert diesen Box-Mittelpunkt-Versatz NICHT — WEDER hier NOCH in der Broadphase/Quadranten-
 * wahl darueber (das ist Port-Stand seit dem verifizierten Spieler-Klemmpfad, nicht neu). Beide
 * benutzen dieselbe Koordinate, damit bleiben Treffer und Richtung konsistent. */
static int32_t coll_contact_dir(int32_t px, int32_t pz, const re15_sca_entry_t *e)
{
    uint32_t ax = (uint32_t)(px - ((int32_t)e->x + (int32_t)e->width));
    uint32_t bx = (uint32_t)(px -  (int32_t)e->x);
    unsigned cx = ((ax & 0x80000000u) | ((bx & 0x80000000u) >> 1)) >> 30;
    if (cx == 2) {
        uint32_t az = (uint32_t)(pz - ((int32_t)e->z + (int32_t)e->density));
        uint32_t bz = (uint32_t)(pz -  (int32_t)e->z);
        unsigned cz = ((az & 0x80000000u) | ((bz & 0x80000000u) >> 1)) >> 30;
        if (cz == 0) return 0x400;
        if (cz == 3) return 0xc00;
        return 0xf00;
    }
    if (cx == 0) return 0x800;
    if (cx == 3) return 0x000;
    return 0xf00;
}

static int collision_constrain_impl(const re15_rdt_t *rdt,
                                    int32_t old_x, int32_t old_z,
                                    int32_t *x, int32_t *z,
                                    int band, int32_t r, unsigned mask,
                                    uint8_t *contact, uint16_t *cell_attr)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    if (band < 0) return 0;

    int q = quadrant_of(*x, *z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    if (start < 0) start = 0;

    /* CLEAR vor der Zellenschleife — byte-true @0x8003b1d0-ec:
     *   `lbu v1,144(a0)` / `andi v1,v1,0xf0` / `sb v1,144(a0)`   (EIN Byte, obere Nibble bleibt!)
     *   `sw zero,436(v1)`                                        (+0x1b4 = 0)
     * Damit ueberlebt das Richtungs-Nibble einen kontaktfreien Frame, waehrend Bit3 + die
     * u1-Bits verschwinden — genau darauf bauen die Ausweich-Zustaende auf (sie lesen im
     * Folge-Frame nur noch `&0xf0`). Der Original-Quirk direkt danach (`lw v1,436(v1)` == 0,
     * dann `sh zero,10(v1)` -> Schreiben nach Adresse 0x0000000A im PSX-Kernel-RAM,
     * @0x8003b1fc-204) hat keine beobachtbare Wirkung und wird NICHT portiert. */
    if (contact)   *contact = (uint8_t)(*contact & 0xf0);
    if (cell_attr) *cell_attr = 0;

    int any_hit = 0;                                   /* uVar14 / v0 = FUN_8003b0a4-Rueckgabe */

    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;             /* strict band == (line 19473) */
        if ((mask & e->u0) == 0) continue;                 /* (a2 & u0): 0 = not solid to this actor */
        /* broad-phase: the actor (radius-inflated) overlaps the cell? */
        if ((unsigned)(*x - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r * 2) &&
            (unsigned)(*z - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r * 2)) {
            /* RE15_COLL_TRACE=1 — which cell actually pushed, and by how much. The paired-replay
             * harness (tools/parity_run.py) showed the player clamped at x=-8132 in ROOM1140 while
             * the PSX walks straight past; this names the cell responsible. */
            int32_t bx = *x, bz = *z;
            if      (e->type == 1) push_rect(e, x, z, old_x, old_z, r);
            else if (e->type == 3) push_circle(e, x, z, r);
            else if (e->type == 8) push_caps8(e, x, z, old_x, old_z, r);   /* ⚠️ best-effort, unverified */
            else if (e->type == 9) push_caps9(e, x, z, old_x, old_z, r);   /* ⚠️ best-effort, unverified */
            /* slope/diagonal push-out — byte-true from RE workflow wf_b5520814 (was DEFERRED, the
             * player walked through diagonal walls; ROOM11C0/1190 have type 2/4/6). */
            else if (e->type == 2) push_diag2(e, x, z, old_x, old_z, r);
            else if (e->type == 4) push_diag4(e, x, z, old_x, old_z, r);
            else if (e->type == 5) push_diag5(e, x, z, old_x, old_z, r);
            else if (e->type == 6) push_diag6(e, x, z, old_x, old_z, r);
            else if (e->type == 7) push_diag7(e, x, z, old_x, old_z, r);
            if ((*x != bx || *z != bz) && getenv("RE15_COLL_TRACE")) {
                static int n = 0;
                if (n++ < 200)
                    fprintf(stderr, "[coll] cell#%d typ=%d band=%d u0=%u rect=(%d,%d,%dx%d) "
                                    "r=%d  (%d,%d)->(%d,%d)\n",
                            i, e->type, (int)(e->floor >> 4), (unsigned)e->u0,
                            (int)e->x, (int)e->z, (int)e->width, (int)e->density,
                            (int)r, bx, bz, *x, *z);
            }
            /* KONTAKT-RUECKSCHREIBUNG — byte-true @0x8003b4b0-ec, NACH dem Push-Handler und
             * unabhaengig davon, ob der Push die Position bewegt hat (kein `break`: die LETZTE
             * treffende Zelle gewinnt, genau wie die Original-Schleife @0x8003b510-20):
             *   8003b4b8  jal 0x8001bf04        (a0 = pos-Vektor NACH dem Push, a2 = Zelle)
             *   8003b4c0  sra v0,v0,4
             *   8003b4c4  addiu v0,v0,8
             *   8003b4c8  lbu v1,0(s2)          s2 = cell+0x0a  -> u1
             *   8003b4d4  andi v1,v1,0x3
             *   8003b4d8  addu v0,v0,v1
             *   8003b4dc  sb  v0,144(a0)        entity+0x90  (STORE BYTE)
             *   8003b4ec  sw  s4,436(v0)        entity+0x1b4 = &cell                        */
            if (contact)
                *contact = (uint8_t)((coll_contact_dir(*x, *z, e) >> 4) + 8 + (e->u1 & 3));
            if (cell_attr)
                *cell_attr = (uint16_t)((uint16_t)e->u1 | ((uint16_t)e->floor << 8));
            any_hit = 1;
        }
    }
    return any_hit;
}

/* PLAYER wall clamp — byte-true args (radius 450, solid-mask 1, the player's current band). */
void re15_collision_constrain(const re15_rdt_t *rdt,
                              int32_t old_x, int32_t old_z,
                              int32_t *x, int32_t *z)
{
    collision_constrain_impl(rdt, old_x, old_z, x, z, s_coll_band, PR, 1u, NULL, NULL);
}

/* ENEMY wall clamp — byte-true args: the enemy's OWN radius (box[+0x78][6] = hit_radius_min), the
 * enemy solid-mask (entity+0x1d7 — GENAU ZWEI Original-Leser: Zombie-Root @0x80100624
 * `lbu a2,471(a0)` + ZGirl-Root @0x8010aac8; 4 = aufrecht, 8 = kriechend @0x801050f4/@0x8010374c;
 * alle uebrigen Aufrufer von FUN_8003b0a4 uebergeben hart 4), and the enemy's own band from its Y
 * (+0x82, which the port does not maintain for enemies, so derive it like the player does). Every
 * ground-enemy wall-clamp caller in enemy_ai_common.c must route through this instead of the
 * player clamp. */
void re15_collision_constrain_enemy(const re15_rdt_t *rdt,
                                    int32_t old_x, int32_t old_z,
                                    int32_t *x, int32_t *z,
                                    int32_t radius, int32_t enemy_y, uint32_t mask)
{
    collision_constrain_impl(rdt, old_x, old_z, x, z,
                             re15_collision_band_from_y(enemy_y), radius, mask, NULL, NULL);
}

/* Siehe re15_collision.h: derselbe Pfad, aber mit dem +0x90/+0x1b4-Rueckschreiben von
 * FUN_8003b0a4 und dessen Rueckgabewert. */
int re15_collision_constrain_contact(const re15_rdt_t *rdt,
                                     int32_t old_x, int32_t old_z,
                                     int32_t *x, int32_t *z,
                                     int32_t radius, int32_t enemy_y, uint32_t mask,
                                     uint8_t *contact, uint16_t *cell_attr)
{
    return collision_constrain_impl(rdt, old_x, old_z, x, z,
                                    re15_collision_band_from_y(enemy_y), radius, mask,
                                    contact, cell_attr);
}

/* FUN_8002cabc / FUN_8002bd44 — OBJECT (Obj_model_set prop) push-out.
 *
 * Separate from the SCA room pass: the original runs an object-collision pass so
 * placed solid objects (the helipad BOX) block the player even though they have no
 * SCA cell. Each prop carries an authored 6-short collision box in its
 * Obj_model_set operands (centre + half-extents); non-zero half-extents = SOLID
 * (all-zero = non-collidable, e.g. the heli/rotors). We port the HORIZONTAL (X/Z)
 * least-penetration push to the box face ± the combined half-extent; player
 * half-extent = the radius PR (the player box is square in XZ). Y gate omitted
 * (the player vertical box isn't extracted; on the flat helipad the floor crate
 * always overlaps in Y). USER-CONFIRMED working. */
/* ---- FUN_8002cfd4 @0x8002cfd4 — die EINGRENZUNG auf die Objektoberflaeche -------------------
 * Steht der Spieler AUF einem Objekt, laeuft im Original statt der Ausschiebung DIESE Funktion:
 * sie haelt ihn im (um sein halbes Eigenmass geschrumpften) XZ-Kasten des Objekts fest — das ist
 * der Mechanismus, der ihn oben auf der Kiste haelt.
 *
 *   8002cfe0  andi v0, player[0x00], 0x40      -> gesetzt: return  (@0x8002cfe4 bne)
 *   8002cff4  andi v0, obj[0x00],    0x2       -> gesetzt: return  (@0x8002cff8 bne)
 *   8002d008  t1 = obj[0x34]                   8002d014  t3 = obj[0x3c]   (Objekt-POSITION,
 *                                              ohne box.cx/cz — anders als FUN_8002da4c)
 *   8002d010  t6 = *(s16*)(player[0x7c] + 0)   8002d024  t4 = *(s16*)(player[0x7c] + 4)
 *   8002d028  a0 = *(u16*)(player[0x78] + 6)   8002d034  a0 >>= 1
 *   8002d038  t0 = obj_hx - a0                 8002d044  v1 = obj_hz - a0   <<< DERSELBE a0
 *   8002d01c  t8 = t1 - (player[0x34] + t6)    8002d054  t7 = t3 - (player[0x3c] + t4)
 *   8002d05c/64  AUSSERHALB-Test `(u32)(2*h) < (u32)(d + h)` — Bit0 = X, Bit1 = Z
 *   8002d06c  beide drin -> return
 *   8002d074/88  dieselben zwei Tests mit der VORposition player[0x40]/[0x44], `lh` = SIGNIERT
 *   8002d0ac-b0  a0 = (vorher ^ jetzt) & jetzt  = die Achsen, die GERADE verlassen wurden
 *   8002d0c0-cc  X: dx>=0 -> obj.x - t0, sonst obj.x + t0   `sw 0x34(a3)`
 *   8002d0d0-f4  Z: dz>=0 -> obj.z - v1, sonst obj.z + v1   `sw 0x3c(a3)`
 *
 * Port: der Spieler hat keine Kollisionsbox im RAM — die PR-Kollision behandelt ihn als Kreis
 * IM Punkt, also ist die rotierte Boxmitte player[0x7c] = {0,0} (t6 = t4 = 0), und
 * `*(u16*)(player[0x78]+6)` ist genau der Klemmradius PR (@0x80031d6c `lhu a1,0x6(DAT_800acacc)`,
 * DAT_800acacc == player+0x78). */
static void prop_contain(int p, int32_t *x, int32_t *z)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->flags & 0x40u)             return;                  /* @0x8002cfe0-e4 */
    if (g_scd.props[p].flags & 0x2u)   return;                  /* @0x8002cff4-f8 */

    const int32_t ox   = (int32_t)g_scd.props[p].x;             /* @0x8002d008 */
    const int32_t oz   = (int32_t)g_scd.props[p].z;             /* @0x8002d014 */
    const int32_t half = (int32_t)((uint32_t)PR >> 1);          /* @0x8002d028/@0x8002d034 */
    const int32_t sx   = (int32_t)(uint16_t)g_scd.props[p].box_hx - half;  /* @0x8002d02c/38 */
    const int32_t sz   = (int32_t)(uint16_t)g_scd.props[p].box_hz - half;  /* @0x8002d030/44 */
    const int32_t dx   = ox - *x;                               /* @0x8002d018-1c (t6 = 0) */
    const int32_t dz   = oz - *z;                               /* @0x8002d050-54 (t4 = 0) */

    unsigned now = 0;
    if ((uint32_t)(sx << 1) < (uint32_t)(dx + sx)) now |= 1u;    /* @0x8002d040/@0x8002d064 */
    if ((uint32_t)(sz << 1) < (uint32_t)(dz + sz)) now |= 2u;    /* @0x8002d04c/@0x8002d05c-68 */
    if (now == 0) return;                                       /* @0x8002d06c */

    /* Die Vorposition ist der u16-SPIEGEL +0x40/+0x44 — `lh` liest ihn VORZEICHENBEHAFTET. */
    const int32_t pdx = ox - (int32_t)(int16_t)pl->pos_s_x;      /* @0x8002d074-80 */
    const int32_t pdz = oz - (int32_t)(int16_t)pl->pos_s_z;      /* @0x8002d088-94 */
    unsigned was = 0;
    if ((uint32_t)(sx << 1) < (uint32_t)(pdx + sx)) was |= 1u;   /* @0x8002d0a4 */
    if ((uint32_t)(sz << 1) < (uint32_t)(pdz + sz)) was |= 2u;   /* @0x8002d098-a8 */

    const unsigned fresh = (was ^ now) & now;                    /* @0x8002d0ac-b0 */
    if (fresh & 1u) *x = (dx >= 0) ? (ox - sx) : (ox + sx);      /* @0x8002d0b4-cc */
    if (fresh & 2u) *z = (dz >= 0) ? (oz - sz) : (oz + sz);      /* @0x8002d0d0-f4 */
}

void re15_collision_prop_contain(int prop_idx, int32_t *x, int32_t *z)
{
    if (prop_idx < 0 || prop_idx >= 16 || prop_idx >= (int)g_scd.prop_count) return;
    prop_contain(prop_idx, x, z);
}

void re15_collision_objects(int32_t *x, int32_t *z)
{
    /* FUN_8002bd44 @0x8002beec-0x8002bf0c: ist das gerade bearbeitete Objekt DAS Objekt, auf dem
     * der Spieler steht (DAT_800ac788), dann laeuft NICHT die Ausschiebung, sondern die
     * Eingrenzung FUN_8002cfd4 — und der Push-Out wird per `j LAB_8002c0ec` @0x8002bf0c
     * komplett uebersprungen (LAB_8002c0ec @0x8002c0ec liegt HINTER dem
     * `jal FUN_8002cabc` @0x8002c0e4).
     *   8002bef0  lw   v0, DAT_800ac788
     *   8002bef8  bne  s2, v0, LAB_8002bf14      ; anderes Objekt -> normaler Pfad
     *   8002bf04  jal  FUN_8002cfd4 (a0=player, a1=obj)
     *   8002bf0c  j    LAB_8002c0ec
     * Ohne das schob der Port den Spieler aus genau der Kiste heraus, auf der er steht
     * (gemessen ROOM1090: z 4266 -> 3403 = Kistenrand-450, in EINEM Frame). */
    const int stand = re15_climb_standing_probe(*x, *z);
    for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++) {
        if (!g_scd.props[i].active) continue;
        if (i == stand) { prop_contain(i, x, z); continue; }     /* @0x8002bef8/@0x8002bf0c */
        int32_t bhx = (int32_t)(uint16_t)g_scd.props[i].box_hx;
        int32_t bhy = (int32_t)(uint16_t)g_scd.props[i].box_hy;
        int32_t bhz = (int32_t)(uint16_t)g_scd.props[i].box_hz;
        if (bhx == 0 && bhy == 0 && bhz == 0) continue;   /* non-collidable */

        int32_t bx = (int32_t)g_scd.props[i].box_cx + g_scd.props[i].x;
        int32_t bz = (int32_t)g_scd.props[i].box_cz + g_scd.props[i].z;
        int32_t dx = bx - *x;
        int32_t dz = bz - *z;
        int32_t sumX = bhx + PR;
        int32_t sumZ = bhz + PR;
        int32_t adx = dx < 0 ? -dx : dx;
        int32_t adz = dz < 0 ? -dz : dz;
        if (adx > sumX) continue;
        if (adz > sumZ) continue;

        int64_t cz = (int64_t)dx * sumZ; if (cz < 0) cz = -cz;
        int64_t cx = (int64_t)dz * sumX; if (cx < 0) cx = -cx;
        if (cz <= cx) {
            int32_t s = (dz >= 0) ? -sumZ : sumZ;
            *z = bz + s;
        } else {
            *x = (dx < 0) ? bx + sumX : bx - sumX;
        }
    }
}

/* ---- FUN_8002cabc mit a2 = 1 (SCHIEBE-Modus) ------------------------------------------------
 * Identische Achsenwahl wie re15_collision_objects (|dx*t4| gegen |dz*t5| @0x8002cce8-d10), nur
 * schreibt der Original-Zweig hier das OBJEKT statt den Aktor:
 *   X-Achse @0x8002cd20-44:  obj.x = actor.x + actorbox.cx - objbox.cx  (+t5 wenn dx>=0, sonst -t5)
 *   Z-Achse @0x8002cd88-ac:  obj.z = actor.z + actorbox.cz - objbox.cz  (+t4 wenn dz>=0, sonst -t4)
 * mit t5 = objbox.hx + actorbox.hx und t4 = objbox.hz + actorbox.hx (JA: beide Summen nehmen
 * actorbox[+6], @0x8002cb3c-4c `lhu v0,6(t2)` wird fuer BEIDE Achsen benutzt — der Aktor ist ein
 * Zylinder). Der Port hat keine Aktor-Box im RAM; wie in re15_collision_objects steht PR (450,
 * der byte-true Spieler-Radius aus FUN_8003b0a4) fuer actorbox.hx und actorbox.cx/cz = 0.
 * dx/dz sind wie im Original obj-MINUS-actor (@0x8002cb1c/@0x8002cb38). */
int re15_collision_push_prop(int prop_idx, int32_t actor_x, int32_t actor_z,
                             int32_t *ox, int32_t *oz)
{
    if (prop_idx < 0 || prop_idx >= 16 || !ox || !oz) return 0;
    const int32_t bhx = (int32_t)(uint16_t)g_scd.props[prop_idx].box_hx;
    const int32_t bhz = (int32_t)(uint16_t)g_scd.props[prop_idx].box_hz;
    const int32_t ccx = (int32_t)g_scd.props[prop_idx].box_cx;
    const int32_t ccz = (int32_t)g_scd.props[prop_idx].box_cz;
    const int32_t t5 = bhx + PR;                       /* objbox.hx + actorbox.hx */
    const int32_t t4 = bhz + PR;                       /* objbox.hz + actorbox.hx */
    int32_t dx = (*ox + ccx) - actor_x;                /* objCX - actorCX   @0x8002cb1c */
    int32_t dz = (*oz + ccz) - actor_z;                /* objCZ - actorCZ   @0x8002cb38 */
    int32_t adx = dx < 0 ? -dx : dx;
    int32_t adz = dz < 0 ? -dz : dz;
    if (adx > t5) return 0;                            /* @0x8002cb58 AABB-Broadphase */
    if (adz > t4) return 0;                            /* @0x8002cb6c */

    int64_t px = (int64_t)dx * t4; if (px < 0) px = -px;   /* |t8*t4| @0x8002cc74/cce8 */
    int64_t pz = (int64_t)dz * t5; if (pz < 0) pz = -pz;   /* |t7*t5| @0x8002ccec/ccfc */
    if (pz < px) {                                     /* @0x8002cd0c `slt v0,v0,v1` -> X-Achse */
        *ox = actor_x - ccx + ((dx >= 0) ? t5 : -t5);  /* @0x8002cd30-44 (bgez t8) */
    } else {
        *oz = actor_z - ccz + ((dz >= 0) ? t4 : -t4);  /* @0x8002cd98-ac (bgez t7) */
    }
    return 1;                                          /* s0++ @0x8002cd4c / @0x8002cdb4 */
}

/* ---- FUN_8003b558(obj, 2) — reiner Kollisionstest der Objekt-Grundflaeche -------------------
 * Das Original baut aus obj[+0x78] (Boxmitte) eine Sonde, sucht ueber FUN_8003b068 die
 * SCA-Zellenliste des Bandes und meldet != 0, sobald eine Zelle mit (a1 & u0) getroffen wird
 * (@0x8003b558-b7bc). Hier derselbe Zellen-Filter wie in collision_constrain_impl (Band strikt
 * gleich, (mask & u0) != 0, um `r` aufgeblasene Broadphase) — nur ohne Push-Out. */
int re15_collision_box_blocked(const re15_rdt_t *rdt, int32_t x, int32_t z,
                               int band, int32_t r, unsigned mask)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    if (band < 0) return 0;
    int q = quadrant_of(x, z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    if (start < 0) start = 0;
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;
        if ((mask & e->u0) == 0) continue;
        if ((unsigned)(x - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r * 2) &&
            (unsigned)(z - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r * 2))
            return 1;
    }
    return 0;
}
