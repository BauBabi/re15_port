/*
 * RE1.5 libsnd entry points (Phase 3, 2026-05-18).
 *
 * RE1.5 statically links libsnd. These entry-point addresses are
 * verified by 20-agent forensic analysis (2026-05-18).
 *
 * For our RE2-faithful save flow: we DO NOT CALL THESE during save.
 * The whole point of Phase 3 is to NOT touch libsnd state during save.
 * They're here for completeness and for non-save subsystems we may add later.
 */
#ifndef LIBSND_H
#define LIBSND_H

#include "psx_types.h"

/* Function-pointer constants — call as `((fn_t)0x80059D3C)(args)` */

/* Voice key-on with explicit (voice, vab, prog, tone, note, fine, voll, volr) */
typedef int (*SsUtKeyOnV_t)(int voice, int vab, int prog, int tone,
                            int note, int fine, int voll, int volr);
#define SsUtKeyOnV_re15  ((SsUtKeyOnV_t) 0x80059D3C)

/* Voice key-off by slot index */
typedef int (*SsVoKeyOff_t)(int voice);
#define SsVoKeyOff_re15  ((SsVoKeyOff_t) 0x80060C34)

/* SsStart — activate sound system */
typedef int (*SsStart_t)(int flag);
#define SsStart_re15     ((SsStart_t) 0x8006019C)

/* Inner VH-parse helper that overwrites G_LIBSND_PROG_ATR/VH/TONE_ATR.
 * Heavily implicated in post-save corruption — DO NOT CALL during save. */
typedef int (*SubVHParse_t)(int a0, int a1);
#define SubVHParse_re15  ((SubVHParse_t) 0x8005AA60)

/* model_setup — broken caller passed garbage a1. With correct a1 = canonical
 * scratch buf pointer (*0x800AC77C), it loads SEQ + VAB and sets cached ptrs. */
typedef int (*ModelSetup_t)(int weapon_type, void *scratch_buf);
#define ModelSetup_re15  ((ModelSetup_t) 0x80043D8C)

/* Save FSM dispatcher (the function we want to REPLACE in Phase 3.2). */
typedef int (*SaveFsm_t)(int arg);
#define SaveFsm_re15     ((SaveFsm_t) 0x80046540)

/* card_cleanup (RE1.5's "kill audio voices + clear locks" routine) */
typedef int (*CardCleanup_t)(int port, int flag);
#define CardCleanup_re15 ((CardCleanup_t) 0x80026594)

#endif
