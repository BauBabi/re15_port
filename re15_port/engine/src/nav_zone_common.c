/*
 * RE1.5 NAV-ZONE / PATHFINDING subsystem — byte-true port of the six undecompiled EXE
 * functions that walk the RDT block.blk section (*(DAT_800ac778+0x38)), RE'd 2026-07-04
 * (raw capstone disasm; none exist in RE_15_Quellcode_V2):
 *
 *   FUN_8003a0fc  zone-from-position (last-to-first point-in-box, u16-wrap half-open)
 *   FUN_8003a07c  random zone (rand % count)
 *   FUN_8003a1a4  edge crossing between two adjacent zones (pure midpoint)
 *   FUN_8003a31c  edge crossing with ENTITY-COORDINATE preference (the final hop:
 *                 keep the walker's own lateral offset through the "doorway" when it
 *                 fits inside the overlap minus 2*hitbox-radius on each side)
 *   FUN_8003a524  path search: exhaustive DFS over simple paths (visited-masked, 16
 *                 deep), cost = Σ SquareRoot0(((Δx)>>3)^2 + ((Δz)>>3)^2) through the
 *                 edge crossings + the final leg; OUTPUT = the FIRST HOP of the best
 *                 path into three SHARED globals (0x800AFBAE/B0/B2 — persist across
 *                 entities; kept stale when unreachable!)
 *   FUN_80039e7c  the per-tick steer-target updater every enemy root calls BEFORE its
 *                 state dispatch: same zone -> steer = raw target pos (every tick);
 *                 cross-zone -> pathfind on the +0x91 repath-timer 0-tick (cycle 7),
 *                 then copy the shared globals (even if stale) into +0x1bc/+0x1be.
 *                 Waypoint mode (+0x1d8 bit3, one-shot): target zone/point overridden
 *                 by node +0x1d6's box midpoint (logical >>1) — the wander-roam.
 *
 * ROOM1140 ground truth (RAM-verified vs stage_saves/mzd_stage1_briefing.sav): 5 zones
 * ringing the conference table (links 0-1-4-3-2-0); player spawn -> zone 0, the feeder
 * aisle -> zone 2; live feeder steer target == the 2->0 crossing (-4400, own z) — the
 * zombies PATH AROUND THE TABLE, they do not beeline.
 */
#include <stdint.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_damage.h"     /* re15_engine_rand8 */
#include "re15_math.h"       /* re15_squareroot0 — the BIOS sqrt the nav cost actually uses */

/* Per-segment nav cost sqrt. RE1.5's DFS path cost (FUN_8003a524) computes each segment length via
 * BIOS SquareRoot0 (jal 0x80065f60 @0x8003a6e8 and @0x8003a728), then SUMS them and picks the
 * cheapest path by `slt`. SquareRoot0 is the 192-entry table approximation that UNDER-estimates
 * floor(sqrt) (#16) — the old EXACT bit-by-bit isqrt here (despite its "BIOS SquareRoot0" comment)
 * diverged and could FLIP which path the DFS selects. Route through the byte-true replica. */
static uint32_t nav_isqrt(uint32_t x) { return re15_squareroot0(x); }

/* ---- node accessors (nodes base = re15_rdt_t.blocks, stride 12, LE) ------------------- */
static const uint8_t *nav_tbl(void)   { return g_room_rdt_ok ? g_room_rdt.blocks : 0; }
static int            nav_count(void) { return g_room_rdt_ok ? g_room_rdt.block_count : 0; }
static int16_t  nrd16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }
static uint16_t nrdu16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
#define NODE(i)      (nav_tbl() + (i) * 12)
#define N_X1(n)      nrd16((n) + 0)
#define N_Z1(n)      nrd16((n) + 2)
#define N_X2(n)      nrd16((n) + 4)
#define N_Z2(n)      nrd16((n) + 6)
#define N_LINKS(n)   nrdu16((n) + 10)

/* Shared cross-entity pathfind output (0x800AFBAE / 0x800AFBB0 / 0x800AFBB2). Byte-true:
 * NOT reset between entities — an unreachable query leaves the previous entity's result. */
static uint16_t s_nav_out_zone = 0;
static int16_t  s_nav_out_x = 0, s_nav_out_z = 0;
/* Zone scratch globals (0x800B828C / 0x800B8290). */
static uint8_t  s_nav_zone_self = 0xff, s_nav_zone_target = 0xff;

/* FUN_8003a0fc — zone id for a position: scan nodes LAST TO FIRST, half-open point-in-box
 * via the u16-wrap trick ((u16)(x-x1) < (u16)(x2-x1)); no hit -> 0xff. */
uint8_t re15_nav_zone_from_pos(int16_t x, int16_t z)
{
    int n = nav_count();
    if (!nav_tbl() || n <= 0) return 0xff;
    for (int i = n - 1; i >= 0; i--) {
        const uint8_t *nd = NODE(i);
        if ((uint16_t)(x - N_X1(nd)) < (uint16_t)(N_X2(nd) - N_X1(nd)) &&
            (uint16_t)(z - N_Z1(nd)) < (uint16_t)(N_Z2(nd) - N_Z1(nd)))
            return (uint8_t)i;
    }
    return 0xff;
}

/* FUN_8003a07c — random zone id (the wander-roam target); 0xff when no table. */
uint8_t re15_nav_rand_zone(void)
{
    int n = nav_count();
    if (!nav_tbl() || n <= 0) return 0xff;
    return (uint8_t)(re15_engine_rand8() % (unsigned)n);
}

/* FUN_8003a1a4 / FUN_8003a31c — the crossing point of the shared edge between adjacent
 * zones a and b. Z-adjacent (a.z1==b.z2 or a.z2==b.z1): z = the matching edge value,
 * x = (max(x1s)+min(x2s))>>1 (arithmetic). Else X-adjacent: x = a.x1 (b.x1 iff a.x2==b.x1),
 * z = (max(z1s)+min(z2s))>>1. `pref` (the a31c variant, final hop only): if the entity's
 * own coordinate on the overlap axis fits within [overlapMin+2r, overlapMax-2r] (r =
 * hit_radius_min, the +0x78 record +6 = 400), use IT instead of the midpoint. */
static void nav_edge_cross(int a, int b, int16_t *cx, int16_t *cz, const re15_actor_t *pref)
{
    const uint8_t *na = NODE(a), *nb = NODE(b);
    int16_t ax1 = N_X1(na), az1 = N_Z1(na), ax2 = N_X2(na), az2 = N_Z2(na);
    int16_t bx1 = N_X1(nb), bz1 = N_Z1(nb), bx2 = N_X2(nb), bz2 = N_Z2(nb);
    if (az1 == bz2 || az2 == bz1) {                      /* Z-adjacent: overlap on X */
        *cz = (az1 == bz2) ? az1 : az2;
        int lo = (ax1 > bx1) ? ax1 : bx1;
        int hi = (ax2 < bx2) ? ax2 : bx2;
        int mid = (lo + hi) >> 1;                        /* sra */
        if (pref) {
            int r2 = 2 * (int)pref->hit_radius_min;
            if (lo + r2 < pref->x && pref->x < hi - r2) mid = (int)pref->x;
        }
        *cx = (int16_t)mid;
    } else {                                             /* X-adjacent: overlap on Z */
        *cx = (ax2 == bx1) ? bx1 : ax1;
        int lo = (az1 > bz1) ? az1 : bz1;
        int hi = (az2 < bz2) ? az2 : bz2;
        int mid = (lo + hi) >> 1;
        if (pref) {
            int r2 = 2 * (int)pref->hit_radius_min;
            if (lo + r2 < pref->z && pref->z < hi - r2) mid = (int)pref->z;
        }
        *cz = (int16_t)mid;
    }
}

static uint32_t nav_seg_cost(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    int32_t dx = (x0 - x1) >> 3, dz = (z0 - z1) >> 3;    /* the >>3 pre-scale */
    return nav_isqrt((uint32_t)(dx * dx + dz * dz));
}

/* FUN_8003a524 — exhaustive DFS over simple paths from start to goal; output = the FIRST
 * HOP (zone + crossing point) of the cheapest path, written to the SHARED globals. `e` =
 * the pathing entity (DAT_800ac784) — seeds depth-0 waypoint + the final-hop preference. */
void re15_nav_pathfind(const re15_actor_t *e, uint8_t start, uint8_t goal,
                       int16_t tx, int16_t tz)
{
    if (start == goal) {                                  /* same zone: raw pass-through */
        s_nav_out_zone = goal; s_nav_out_x = tx; s_nav_out_z = tz;
        return;
    }
    if (!nav_tbl() || start >= (uint8_t)nav_count() || goal >= (uint8_t)nav_count()) return;
    uint16_t A[16];                                      /* remaining-neighbor mask / depth */
    uint16_t B[16];                                      /* visited mask */
    uint16_t C[16];                                      /* accumulated cost — u16 byte-true (FUN_8003a524
                                                          * stores each hop via `sh`, truncating mod 65536
                                                          * @0x8003a840; the port had u32 = no wrap. Dormant
                                                          * in shipped zone graphs but byte-exact now). */
    int16_t  Dx[16], Dz[16];                             /* per-depth waypoint */
    uint8_t  E[16];                                      /* zone id / depth */
    A[0] = N_LINKS(NODE(start));
    if (A[0] == 0) return;                               /* isolated: NO output (stale globals) */
    B[0] = (uint16_t)(1u << start);
    C[0] = 0;
    Dx[0] = (int16_t)e->x; Dz[0] = (int16_t)e->z;
    E[0] = start;
    uint16_t goal_mask = (uint16_t)(1u << goal);
    uint32_t best = 0x7fffffffu;
    int depth = 0;
    for (;;) {
        uint16_t m = A[depth];
        if (m == 0) {                                     /* exhausted -> pop */
            if (depth == 0) break;
            depth--;
            continue;
        }
        if (m & goal_mask) {                              /* close the path into the goal */
            A[depth] = (uint16_t)(m & ~goal_mask);
            int16_t cx, cz;
            nav_edge_cross(E[depth], goal, &cx, &cz, e);  /* the a31c entity-pref variant */
            uint32_t total = C[depth]
                           + nav_seg_cost(Dx[depth], Dz[depth], cx, cz)
                           + nav_seg_cost(tx, tz, cx, cz);
            if (total < best) {
                best = total;
                if (depth >= 1) {                         /* first hop of THIS path */
                    s_nav_out_zone = E[1];
                    s_nav_out_x = Dx[1]; s_nav_out_z = Dz[1];
                } else {                                  /* direct neighbor: hop = the crossing */
                    s_nav_out_zone = goal;
                    s_nav_out_x = cx; s_nav_out_z = cz;
                }
            }
            continue;                                     /* do NOT descend into the goal */
        }
        /* expand the lowest set bit (ctz order) */
        int nb = 0; while (!(m & (1u << nb))) nb++;
        A[depth] = (uint16_t)(m & ~(1u << nb));
        if (depth + 1 >= 16) continue;                    /* stack cap */
        int16_t cx, cz;
        nav_edge_cross(E[depth], nb, &cx, &cz, 0);        /* intermediate: pure midpoint */
        C[depth + 1] = C[depth] + nav_seg_cost(Dx[depth], Dz[depth], cx, cz);
        Dx[depth + 1] = cx; Dz[depth + 1] = cz;
        E[depth + 1] = (uint8_t)nb;
        B[depth + 1] = (uint16_t)(B[depth] | (1u << nb));
        A[depth + 1] = (uint16_t)(N_LINKS(NODE(nb)) & ~B[depth + 1]);
        depth++;
    }
}

/* FUN_80039e7c — the per-tick steer-target update (called by every enemy root BEFORE the
 * state dispatch). Returns 1 = steer updated, 2 = untouched. Byte-true order:
 *   1. repath timer +0x91: (low7==0 -> |=7), always -1; bit 0x80 set -> return 2
 *   2. root state 4 (pose/cinematic FSM) -> return 2
 *   3. zones: self from own pos, target from (tx,tz)
 *   4. wp_mode: target zone := wp_node, goal point := node box midpoint (LOGICAL >>1)
 *   5. same zone -> steer := goal point (EVERY tick; the timer only throttles the DFS)
 *   6. cross-zone: only on the timer 0-tick -> DFS, then copy the shared globals
 *      UNCONDITIONALLY (stale-on-unreachable is byte-true). */
int re15_nav_update_steer(re15_actor_t *e, int16_t tx, int16_t tz,
                          uint8_t wp_node, int wp_mode)
{
    if ((e->repath_timer & 0x7f) == 0) e->repath_timer |= 7;
    e->repath_timer--;
    if (e->repath_timer & 0x80) return 2;                /* nav-disabled latch (never set in
                                                          * the shipped game — checked-only) */
    if (e->state == 4) return 2;                         /* +0x4==4 pose FSM: no steering */
    if (!nav_tbl() || nav_count() <= 0) {                /* no BLK graph: raw target (the
                                                          * pre-nav port behavior) */
        e->steer_x = tx; e->steer_z = tz;
        return 1;
    }
    s_nav_zone_self   = re15_nav_zone_from_pos((int16_t)e->x, (int16_t)e->z);
    s_nav_zone_target = re15_nav_zone_from_pos(tx, tz);
    int16_t gx = tx, gz = tz;
    if (wp_mode) {                                       /* wander-roam: node midpoint */
        s_nav_zone_target = wp_node;
        if (wp_node < (uint8_t)nav_count()) {
            const uint8_t *nd = NODE(wp_node);
            gx = (int16_t)(((int)N_X1(nd) + (int)N_X2(nd)) >> 1);   /* srl on u32 — positive
                                                                     * sums in shipped data */
            gz = (int16_t)(((int)N_Z1(nd) + (int)N_Z2(nd)) >> 1);
        }
    }
    s_nav_out_zone = s_nav_zone_target;                  /* the @0x80039fb8 pre-store */
    if (s_nav_zone_self == s_nav_zone_target) {
        e->steer_x = gx; e->steer_z = gz;
        s_nav_out_x = gx; s_nav_out_z = gz;
        return 1;
    }
    if (e->repath_timer != 0) return 2;                  /* DFS only on the 0-tick */
    re15_nav_pathfind(e, s_nav_zone_self, s_nav_zone_target, gx, gz);
    e->steer_x = s_nav_out_x;                            /* unconditional copy (@0x8003a040/58) */
    e->steer_z = s_nav_out_z;
    return 1;
}

/* Test/diagnostic accessors. */
uint8_t re15_nav_dbg_zone_self(void)   { return s_nav_zone_self; }
uint8_t re15_nav_dbg_zone_target(void) { return s_nav_zone_target; }

/* Den von re15_nav_pathfind zuletzt bestimmten Zwischenpunkt auslesen. Nur-Lesen, keine
 * Zustandsaenderung — der Testautopilot steuert damit den Raum-Graphen entlang statt auf der
 * Luftlinie in eine Wand zu laufen. */
void re15_nav_dbg_waypoint(int16_t *x, int16_t *z)
{
    if (x) *x = s_nav_out_x;
    if (z) *z = s_nav_out_z;
}
