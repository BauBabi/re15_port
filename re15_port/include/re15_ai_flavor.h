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
void re15_re2z_rng_reset(void);                   /* re-seed the RE2 PRNG on room load (also
                                                   * clears the FUN_800401d4 one-save latch)    */

/* ---- WELLE B: the FULL RE2 zombie brain -----------------------------------------------------
 * re15_re2z_tick REPLACES the RE1.5 state dispatch for an owned zombie when the RE2 flavor is
 * selected (hooked in re15_enemy_ai_live_tick / re15_zgirl_ai_tick). It runs the RE2 overlay's
 * root prolog (cooldown bank @0x8010045C-98) + state machine: ACTIVE @0x8010114C with the
 * decision-then-executor double dispatch (@0x801011A8-EC), HURT @0x80104F40, DEATH @0x80108250,
 * CORPSE @0x8010A440, state 8 @0x80109CFC. Presentation uses the REAL RE2 EM01x bank the Welle-A
 * loader fills (clip indices are RE2-native) + the ENEMSE SE bank via the audio hook below. */
int  re15_re2z_tick(int slot);                    /* 1 = handled (RE2 brain owns this actor)    */

/* PC registers the ENEMSE playback here (engine stays link-clean for the PSX target, which
 * never runs the RE2 flavor). bank_fn selects the ENEMSE bank (audio_pc load_re2_enemy_se). */
void re15_re2z_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));

/* ---- WELLE B shims exported from enemy_ai_common.c (wrap its statics) ---------------------- */
void re15_re2z_player_pin(void);                  /* s_player_grabbed = 1 (per-frame pin)       */
int  re15_re2z_mash(void);                        /* re15_mash_pressed() (FUN_8001598C twin)    */
void re15_re2z_footlock(int slot, re15_actor_t *e);  /* clip-driven walk movement              */
void re15_re2z_victim_begin(re15_actor_t *zombie, re15_actor_t *player, int behind);
void re15_re2z_victim_devour(re15_actor_t *zombie, int behind);  /* Kill-Tick-Richtung
                                                   * (dir<<8)|6 @0x80102928-50 — explizit    */
void re15_re2z_grab_anchor(re15_actor_t *e, re15_actor_t *pl, int clip);
void re15_re2z_grab_rootmotion(re15_actor_t *e);
void re15_re2z_move_root(re15_actor_t *e);        /* 0x80015e7c: per-frame clip root delta   */
void re15_re2z_se_play(int se_id);                /* ENEMSE-SE ueber den Audio-Hook (fuer den
                                                   * Frame-Flag-SFX-Pfad 0x801016c8)          */

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

/* Fill the gates from PORT state, then run the ladder. `player_claimed` is the port's equivalent of
 * PL+0x1D3 bit 0x80 (re15_player_is_grabbed). */
void re15_re2z_fill_gates(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_gates_t *g);
int  re15_re2z_walk_decide(const re15_actor_t *e, const re15_actor_t *pl,
                           int player_claimed, re15_re2z_decision_t *out);

#endif /* RE15_AI_FLAVOR_H */
