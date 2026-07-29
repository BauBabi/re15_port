/*
 * RE1.5 Rebuilt — AI FLAVOR switch (PORT OPTION, not present in any original).
 *
 * The user can pick which game's ZOMBIE brain runs:
 *   RE15 (default) — the byte-true RE1.5 zombie AI (enemy_ai_common.c). Unchanged, always the default,
 *                    so every byte-true campaign/test keeps passing.
 *   RE2            — the Resident Evil 2 (retail, Leon) zombie AI, RE'd from the RE2 enemy overlay
 *                    EMZ0.BIN (loads RAW @0x80100000; disassemble with
 *                    .claude/skills/re15-psx-disasm/scripts/re2_disasm.py --bin EMZ0.BIN).
 *
 * ONLY zombies switch. Dog/crow/spider/maggot/birkin/NPCs stay on the RE1.5 brain in both modes —
 * anything else would break six verified byte-true campaigns.
 */
#ifndef RE15_AI_FLAVOR_H
#define RE15_AI_FLAVOR_H

typedef enum {
    RE15_AI_FLAVOR_RE15 = 0,   /* byte-true RE1.5 (DEFAULT) */
    RE15_AI_FLAVOR_RE2  = 1    /* RE2 retail zombie brain   */
} re15_ai_flavor_t;

re15_ai_flavor_t re15_ai_flavor(void);
void             re15_ai_flavor_set(re15_ai_flavor_t f);

/* Does the RE2 brain own this actor type? (zombie family only) */
int re15_re2z_owns_type(unsigned type);

/* RE2 zombie brain entry points (enemy_ai_re2_zombie.c). */
#include <stdint.h>
#include "re15_actor.h"
void re15_re2z_gait_init(re15_actor_t *e);        /* seed the gait row/timer  @0x80101A7C-AC   */
int  re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist); /* @0x80101BAC */
void re15_re2z_rng_reset(void);                   /* re-seed the RE2 PRNG on room load          */

/* ---- W2: the attack-decision ladder DECISION[1] @0x80101714 --------------------------------
 * Kept as a PURE function over an explicit gate struct: the control flow is fully verified, but
 * several gate INPUTS still have no proven producer in the port. Wiring a ladder whose gates are
 * silently zero would make the RE2 zombie only ever grab — so the flow lands (and is unit-tested)
 * before the wiring does. RE15_RE2_AI.md tracks which producers are still open. */
typedef struct {
    uint32_t dist;          /* self+0x1F0, distance to the player      @0x80101744 */
    int      arc1024;       /* re15_ai_arc_test(...,1024): 0 = INSIDE  @0x8010174c */
    int      arc512;        /* re15_ai_arc_test(...,512)               @0x80101754 */
    uint8_t  self_23e;      /* gates blocks A/B and J                  @0x80101790 */
    uint32_t self_1f4;      /* block A payload                         @0x801017a4 */
    int      a_sector_hit;  /* FUN_80015714(self, self+0x1F8, 256)==0  @0x801017d0 */
    uint8_t  self_106;      /* blocks B (!=) and G (==)                @0x80101808 */
    uint8_t  pl_106;        /*                                         @0x8010180c */
    uint8_t  pl_1d3;        /* whole byte in B/J; bit 0x80 in G        @0x8010181c */
    int16_t  self_1d4;      /* block C, mask 0xC000                    @0x80101830 */
    uint32_t self_110;      /* block C, bit 0                          @0x80101844 */
    uint16_t global_cfbf6;  /* 0x800CFBF6, masks 0x15 (D) / 0x17 (E)   @0x80101874 */
    uint16_t self_21a;      /* block G, bits 0x20 / 0x40               @0x80101928 */
    int      g1_sector_hit; /* FUN_80015758(..., yaw+256, 256)==0      @0x80101948 */
    int      g2_sector_hit; /* FUN_80015758(..., yaw-256, 256)==0      @0x8010198c */
    uint8_t  pl_8;          /* the G / J fork, tested == 15            @0x801018e8 */
    int16_t  pl_156;        /* block K, tested == -32768               @0x801019e8 */
} re15_re2z_gates_t;

typedef struct {
    int      wrote;         /* did any block store to +0x4 at all */
    uint32_t word;          /* the winning state word (last writer wins) */
    int      early_out;     /* block A returned early (j 0x80101a1c @0x801017e0) */
    int      claim_player;  /* block G set PL[0x1D3] |= 0x80 */
    int      set_10e_4000;  /* block K set self+0x10E |= 0x4000 */
    int      rng_draws;     /* 0..2 — the draw COUNT is itself behaviour */
} re15_re2z_decision_t;

int re15_re2z_decide_walk(const re15_re2z_gates_t *g, re15_re2z_decision_t *out);

#endif /* RE15_AI_FLAVOR_H */
