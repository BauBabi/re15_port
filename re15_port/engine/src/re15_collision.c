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
#include "re15_scd.h"        /* g_scd.props — the Obj_model_set object list */
#ifndef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#define PR 450

/* integer sqrt == the BIOS SquareRoot0 the original calls (floor(sqrt)). */
static int32_t coll_isqrt(int64_t x)
{
    if (x <= 0) return 0;
    uint64_t v = (uint64_t)x, res = 0, bit = (uint64_t)1 << 62;
    while (bit > v) bit >>= 2;
    while (bit) {
        if (v >= res + bit) { v -= res + bit; res = (res >> 1) + bit; }
        else res >>= 1;
        bit >>= 2;
    }
    return (int32_t)res;
}

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
    int32_t dist = coll_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
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
 * push-out (types 2/4/5/6/7) stays DEFERRED — see [[slope_collision_deferred_2026_06_09]]. */
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
static void collision_constrain_impl(const re15_rdt_t *rdt,
                                     int32_t old_x, int32_t old_z,
                                     int32_t *x, int32_t *z,
                                     int band, int32_t r, unsigned mask)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return;
    if (band < 0) return;

    int q = quadrant_of(*x, *z, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    if (start < 0) start = 0;

    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;             /* strict band == (line 19473) */
        if ((mask & e->u0) == 0) continue;                 /* (a2 & u0): 0 = not solid to this actor */
        /* broad-phase: the actor (radius-inflated) overlaps the cell? */
        if ((unsigned)(*x - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r * 2) &&
            (unsigned)(*z - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r * 2)) {
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
        }
    }
}

/* PLAYER wall clamp — byte-true args (radius 450, solid-mask 1, the player's current band). */
void re15_collision_constrain(const re15_rdt_t *rdt,
                              int32_t old_x, int32_t old_z,
                              int32_t *x, int32_t *z)
{
    collision_constrain_impl(rdt, old_x, old_z, x, z, s_coll_band, PR, 1u);
}

/* ENEMY wall clamp — byte-true args: the enemy's OWN radius (box[+0x78][6] = hit_radius_min), the
 * enemy solid-mask 4 (entity+0x1d7, @0x80100624), and the enemy's own band from its Y (+0x82, which
 * the port does not maintain for enemies, so derive it like the player does). Every ground-enemy
 * wall-clamp caller in enemy_ai_common.c must route through this instead of the player clamp. */
void re15_collision_constrain_enemy(const re15_rdt_t *rdt,
                                    int32_t old_x, int32_t old_z,
                                    int32_t *x, int32_t *z,
                                    int32_t radius, int32_t enemy_y)
{
    collision_constrain_impl(rdt, old_x, old_z, x, z,
                             re15_collision_band_from_y(enemy_y), radius, 4u);
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
void re15_collision_objects(int32_t *x, int32_t *z)
{
    for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++) {
        if (!g_scd.props[i].active) continue;
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
