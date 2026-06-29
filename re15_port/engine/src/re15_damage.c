/*
 * RE1.5 Rebuilt — Player damage resolution (#13).
 *
 * Byte-true port of the PLAYER branch of FUN_80012d60 (the unified hit/damage
 * resolver). Every constant here cites the RE1.5 PSX.EXE disassembly
 * (ghidra1_V2.txt) — see re15_damage.h for the subsystem overview and the
 * deferred in-game trigger (enemy attack-action FSM + actor hitbox test).
 *
 * Instruction map of the player branch (ghidra1_V2.txt):
 *   80012e24-e30  if (player+0x93 & 1)  -> skip (one hit per attack window)
 *   80012e44-e64  player+0x9a (HP) -= dmg_table[type]          (lhu/sub/sh)
 *   80012e58/e68  type<2 gate (sltiu type,2)
 *   80012e70      FUN_800453d0(10)        = play hit SE #10    (DEFERRED audio)
 *   80012e78-eb4  2× FUN_8001af20()&1 -> if both odd, +0x98 |= 2 (bleed/poison)
 *   80012ebc      player+0x4 (state) = 2  (hurt)
 *   80012ec8-ed4  player+0x5 = FUN_8001a7a8(player,hbX,hbZ) + 2 (front/back anim)
 *   80012ee0      player+0x6 = 0
 *   80012eec      player+0x93 |= 1        (set guard; delay slot -> always runs)
 *   80012ee8-efc  if (HP < 0) state=3 (death), +0x5=0, +0x6=0
 */
#include "re15_damage.h"

/* DAT_8006f418 — ghidra1_V2.txt:223455-223478 (11×s16 LE). */
const int16_t re15_damage_table[11] = {
    10,   /* 0  zombie bite/grab (FUN_80017fa4 lunge, attack_type 0) */
    20,   /* 1                                                       */
    1000, /* 2  instakill class                                      */
    1000, /* 3                                                       */
    1000, /* 4                                                       */
    50,   /* 5                                                       */
    100,  /* 6                                                       */
    200,  /* 7                                                       */
    300,  /* 8                                                       */
    1000, /* 9                                                       */
    0,    /* 10                                                      */
};

/* DAT_8006f430 — ghidra1_V2.txt:223480-223490 (enemy-branch reaction codes). */
const uint8_t re15_react_table[11] = {
    0x03, 0x03, 0x09, 0x0A, 0x0B, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14
};

/* Bleed/poison RNG. The PSX calls FUN_8001af20 twice (@80012e78/e80) — a stateless
 * hash (x + (x>>7)) & 0xff fed leftover register a0 (incidental, NON-reproducible by
 * construction — same documented stance as the idle-timer rand in player_common.c).
 * We preserve the EXACT hash and the EXACT 1/4 trigger probability (both draws odd)
 * by feeding a documented xorshift32 entropy source in place of the unreachable
 * register value. Cycle-identical reproduction is impossible; the OBSERVABLE
 * behaviour (a type<2 hit bleeds ~1/4 of the time) is faithful. */
static uint32_t s_rng = 0x2545f491u;

void re15_damage_seed_rng(uint32_t seed) { s_rng = seed ? seed : 1u; }

static uint32_t dmg_rng(void)
{
    uint32_t x = s_rng;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;   /* xorshift32 entropy */
    s_rng = x;
    return (x + (x >> 7)) & 0xffu;              /* FUN_8001af20 hash */
}

/* FUN_8001a7a8 (@8001a7a8) — 1 if the hit came from the player's FRONT hemisphere
 * (within ±90° of facing), else 0; selects the hurt-anim sub-state (2 = back, 3 =
 * front). The PSX computes
 *     ang = FUN_8001a6d4(hbX, hbZ, player+0x34, player+0x3c)
 *         = atan2(player.z - hbZ, player.x - hbX)   (engine RAW angle convention)
 *     rel = ang - player_heading(+0x6a)
 *     front = ((rel + 0x400) & 0xfff) < 0x800
 * re15_atan2_q12 IS the port of FUN_8001a6d4 (actor_locomotion.c). The player's raw
 * heading = rot_y(mesh) + 1024 (the port's standing -1024 mesh offset, used the same
 * way in stair_common.c / skeleton_common.c). The +0x34/+0x3c collision centre and
 * +0x6a heading map to the port's x/z/rot_y — a faithful mapping that is currently
 * UNOBSERVABLE (the port has no distinct front/back hurt clips), so this is the one
 * detail not yet runtime-pinned; it is derived from the disasm, not guessed. */
static int hit_from_front(const re15_actor_t *p, int32_t src_x, int32_t src_z)
{
    int32_t ang = re15_atan2_q12(p->z - src_z, p->x - src_x);
    int32_t rel = (ang - ((int32_t)p->rot_y + 1024) + 0x400) & 0xfff;
    return rel < 0x800;
}

int re15_player_take_damage(re15_actor_t *p, uint8_t attack_type,
                            int32_t src_x, int32_t src_z)
{
    if (!p) return 0;

    /* hit-once guard (@80012e24-e30): one damage application per attack window. */
    if (p->hit_react & 0x1) return 0;

    uint8_t type = (uint8_t)(attack_type & 0xff);
    int16_t dmg  = (type < 11) ? re15_damage_table[type] : 0;

    /* HP -= dmg (@80012e44-e64). HP is s16; the death test below is SIGNED. */
    p->hp = (int16_t)(p->hp - dmg);

    /* type<2 → hit SE + bleed/poison roll (@80012e68-eb8). The SE (FUN_800453d0(10))
     * is DEFERRED until the SE-id table is wired; the bleed roll is byte-true. */
    if (type < 2) {
        if ((dmg_rng() & 1) && (dmg_rng() & 1))
            p->status_flags |= 0x2;            /* DAT_800acaec bit0x2 = bleed/poison */
    }

    /* hurt state (@80012ebc-eec). */
    p->state       = 2;                                                  /* +0x4 hurt */
    p->sub_state_1 = (uint8_t)(hit_from_front(p, src_x, src_z) + 2);     /* +0x5 anim */
    p->sub_state_2 = 0;                                                  /* +0x6      */
    p->hit_react  |= 0x1;                                                /* +0x93 guard */

    /* death override (@80012ee8-efc): signed HP < 0 → death state, clear sub-states. */
    if (p->hp < 0) {
        p->state       = 3;                                             /* +0x4 death */
        p->sub_state_1 = 0;
        p->sub_state_2 = 0;
    }
    return 1;
}

/* Enemy branch of FUN_80012d60 (@80012f08-80013034): apply a resolved hit to an
 * ENEMY actor — the counterpart to the player branch. Same dmg table, but the
 * reaction sub-state is the hit-clip from re15_react_table (not the front/back
 * selector) and there is NO bleed roll (player-only). The resolver LOOP owns the
 * per-frame bits the original sets around this: the +0x93 &= 1 clear (@80012f88),
 * the FUN_8001a7a8 collision-confirm → +0x93 bit0x80 (@80012fa8), the self-exclusion
 * vs the attacker +0x188 (@80012f40), and the +0x90 0x3000000 state-mask gate
 * (@80012f54). This applies the per-enemy damage ONCE per attack window.
 * Returns 1 if the hit landed, 0 if the enemy was already hit this attack. */
int re15_enemy_take_damage(re15_actor_t *e, uint8_t attack_type)
{
    if (!e) return 0;
    /* hit-once guard +0x93 bit0x1 (@80012fb4-c0): already hit → mark bit0x2, no re-damage. */
    if (e->hit_react & 0x1) {
        e->hit_react |= 0x2;                                       /* +0x93 |= 2 (@80012fcc) */
        return 0;
    }
    uint8_t type = (uint8_t)(attack_type & 0xff);
    int16_t dmg  = (type < 11) ? re15_damage_table[type] : 0;
    e->sub_state_3 = 0;                                            /* +0x7 = 0 (@80012fd4) */
    e->sub_state_2 = 1;                                            /* +0x6 = 1 (@80012fd8) */
    e->sub_state_1 = (type < 11) ? re15_react_table[type] : 0;     /* +0x5 = reaction clip (@80012fe8) */
    e->hp = (int16_t)(e->hp - dmg);                               /* +0x9a -= dmg (@80012ffc) */
    e->hit_react |= 0x1;                                           /* +0x93 |= 1 (@8001300c) */
    e->state = 2;                                                  /* +0x4 = 2 hurt (@80013018) */
    if (e->hp < 0) e->state = 3;                                  /* signed HP<0 → death (@80013020) */
    return 1;
}

void re15_player_clear_hit_guard(re15_actor_t *p)
{
    if (p) p->hit_react &= (uint8_t)~0x1;
}

/* ====================================================================== *
 *  Attack-hitbox vs actor collision test — FUN_8002b5d0                   *
 *  (ghidra1_V2.txt:118005-118130). The per-target geometry test the       *
 *  resolver loop FUN_80012d60 runs against each enemy + the player.       *
 * ====================================================================== */

/* BIOS SquareRoot0 (floor sqrt) — the original calls it @8002b740. Bit-by-bit
 * integer isqrt, identical to re15_collision.c's coll_isqrt: each combat /
 * collision / light / camera module keeps its own private copy, the established
 * port convention (the PSX inlines SquareRoot0 at every call site). */
static int32_t dmg_isqrt(int64_t x)
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

/* CIRCULAR path of FUN_8002b5d0 (radius_min == radius_max @ hbdata+6/+0xa). The
 * angular-SECTOR path (radius interpolated over the target's facing via the BIOS
 * ratan2 + rsin/rcos) is DEFERRED: the port has only the catan-based atan2
 * (re15_atan2_q12 = FUN_8001a6d4), not the BIOS ratan2 the sector branch calls
 * @8002b65c — porting that is a separate byte-true step. Symmetric hitboxes
 * (the common case) take this circular branch directly @8002b6fc.
 *
 * (cx,cy,cz) = the target hitbox centre = target world pos (+0x34/+0x38/+0x3c)
 * + the local offset (the short[3] at target+0x7c). radius/height = hbdata+6/+8.
 * (atk_x,atk_y,atk_z) = attack origin (param_2[0..2]); atk_radius = param_1.
 * Instruction map (ghidra1_V2.txt):
 *   8002b6fc  R   = (radius&0xffff) + (atk_radius&0xffff)
 *   8002b704  dx  = atk_x - cx;   8002b714  reject unless |dx| <= R (incl.)
 *   8002b724  dz  = atk_z - cz;   8002b730  reject unless |dz| <= R (incl.)
 *   8002b740  dist= SquareRoot0(dx*dx + dz*dz)
 *   8002b75c  reject unless 0 < R - dist   (dist < R, strict)
 *   8002b768  h   = (atk_radius&0xffff) + height
 *   8002b76c  dy  = atk_y - cy;   8002b778  accept iff -h < dy && dy < h (strict) */
int re15_hitbox_overlap(int32_t cx, int32_t cy, int32_t cz,
                        int32_t radius, int32_t height,
                        int32_t atk_x, int32_t atk_y, int32_t atk_z,
                        int32_t atk_radius)
{
    int32_t R = (int32_t)((uint32_t)radius & 0xffff) + (int32_t)((uint32_t)atk_radius & 0xffff);

    /* X/Z AABB broad-phase — the original's unsigned |d|<=R idiom: (u32)(d+R) <= (u32)(2R). */
    int32_t dx = atk_x - cx;
    if ((uint32_t)(dx + R) > (uint32_t)(R * 2)) return 0;
    int32_t dz = atk_z - cz;
    if ((uint32_t)(dz + R) > (uint32_t)(R * 2)) return 0;

    /* Euclidean distance < R (strict, via 0 < R - dist). */
    int32_t dist = dmg_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
    if (R - dist <= 0) return 0;

    /* Y/height range: -h < dy < h, h = attack_radius + hitbox height. */
    int32_t h  = (int32_t)((uint32_t)atk_radius & 0xffff) + (int32_t)((uint32_t)height & 0xffff);
    int32_t dy = atk_y - cy;
    return (-h < dy && dy < h) ? 1 : 0;
}

/* FUN_8002b5d0 wrapper over a port actor: read its hitbox dims (the +0x78 struct)
 * + local offset (+0x7c) and run the overlap test. Circular path is byte-true; the
 * angular-SECTOR case (hit_radius_min != hit_radius_max) needs the BIOS ratan2 +
 * rsin/rcos directional-reach interpolation (@8002b65c) which the port hasn't yet
 * ported — it is DEFERRED and routed through the circular path on radius_min as a
 * documented placeholder (never reached in-game today: enemy hitbox values aren't
 * wired yet and the AI attack trigger is deferred). */
int re15_hitbox_test(const re15_actor_t *target, const re15_attack_box_t *atk)
{
    if (!target || !atk) return 0;
    int32_t cx = target->x + target->hit_offset_x;          /* +0x34 + offset[0] */
    int32_t cy = target->y + target->hit_offset_y;          /* +0x38 + offset[1] */
    int32_t cz = target->z + target->hit_offset_z;          /* +0x3c + offset[2] */
    int32_t radius = target->hit_radius_min;                /* == hit_radius_max (circular) */
    return re15_hitbox_overlap(cx, cy, cz, radius, target->hit_height,
                               atk->x, atk->y, atk->z, atk->radius);
}

/* ====================================================================== *
 *  Damage-resolver LOOP — FUN_80012d60 (ghidra1_V2.txt:77607-77814).      *
 *  Tests one attack hitbox against the player block + every active enemy  *
 *  and applies the damage. The two branches (player/enemy take_damage)    *
 *  are ported above; this is the iteration + gating around them.          *
 * ====================================================================== */
int re15_resolve_attack(const re15_attack_box_t *atk, uint8_t attack_type,
                        int attacker_slot)
{
    if (!atk) return 0;

    int collected[RE15_ACTOR_MAX];
    int ncol = 0;
    int hits = 0;

    /* (1) Enemy pass — collect every active enemy the hitbox overlaps. The original
     * scans DAT_800acc2c (stride 0x1f4) until DAT_800aca4e active entries are seen
     * (@80012d7c-de4); the port iterates slots 1.. (slot 0 = the player block, tested
     * separately @80012df0) and skips inactive slots — same active set. */
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active) continue;                              /* (*puVar6 & 1) @80012d9c */
        if (re15_hitbox_test(e, atk))                          /* FUN_8002b5d0 @80012db4 */
            collected[ncol++] = i;                             /* local_78[] @80012dc8 */
    }

    /* (2) Player block — ALWAYS tested, regardless of the enemy pass (@80012df0).
     * re15_player_take_damage is the player branch (guard + HP + bleed + hurt/death
     * state @80012e18-f04); cVar9 is set on overlap even if the guard blocked re-damage. */
    if (re15_hitbox_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], atk)) {
        re15_player_take_damage(&g_actors[RE15_ACTOR_SLOT_PLAYER], attack_type,
                                atk->x, atk->z);
        hits++;                                                /* cVar9 = 1 @80012efe */
    }

    /* (3) Enemy application — reverse collected order (the original do-while
     * decrements bVar8 @80012f12-30). */
    for (int k = ncol - 1; k >= 0; k--) {
        int slot = collected[k];
        re15_actor_t *e = &g_actors[slot];

        /* GATE A — self-exclusion (@80012f40): the attacker never damages itself.
         * Original: (e+0x188+0x40) == (attacker+0x74), i.e. the same model-pool
         * pointer ↔ the same actor; the port maps that identity to slot equality. */
        if (slot == attacker_slot) continue;

        /* GATE B — terminal-state skip (@80012f54): the original skips when
         * (e+0x90 & 0x3000000) == 0x3000000 (the enemy's death / despawn terminal
         * flags). The bit WRITER is the enemy death/lifecycle FSM, which the port has
         * not yet implemented, so no enemy can be in that state today → the gate is
         * inert and is OMITTED here (behaviourally identical now, same documented-
         * deferral stance as the hit-SE below). FUN_80011f50 reads the same gate. */

        /* Per-attack collision bits (@80012f70-fac): clear all but the hit-once bit,
         * then set bit0x80 when the hit came from the target's FRONT (FUN_8001a7a8). */
        e->hit_react &= 1;
        if (hit_from_front(e, atk->x, atk->z))
            e->hit_react |= 0x80;

        /* type<2 → hit SE FUN_800453d0(10) @80012f80 — DEFERRED (audio SE-id table). */

        /* Enemy branch (@80012fb4-3034): applies the hit once per window (bit0 guard),
         * else marks the re-hit bit0x2. */
        re15_enemy_take_damage(e, attack_type);
        hits++;                                                /* cVar9 += 1 @80012fec */
    }
    return hits;
}
