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

#endif /* RE15_AI_FLAVOR_H */
