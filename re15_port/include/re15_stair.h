/*
 * RE1.5 Rebuilt — stair traversal (2026-06-07, v4 player-controlled).
 *
 * Byte-true basis (multi-agent RE, ghidra_source_original_V2.txt): a stair (SCD
 * Aot_set type 12/13 zone) is an AUTO, PLAYER-CONTROLLED band transition. The
 * per-frame tick FUN_80031c44 latches DAT_800aca3c|0x4000 when the player walks
 * over a stair AOT one band below him (@18437); that latch SUPPRESSES the normal
 * wall collision FUN_8003b0a4 (@18442-44) so the player can walk THROUGH the
 * higher band's perimeter wall down onto the lower platform. He keeps walking
 * under his own pad input (XZ via the walk root-motion); the engine lowers his Y
 * (FUN_8002c444) and, on arrival, SNAPS Y to the floor probe and recomputes the
 * band from Y (band = -(Y/0x708), 0x708=1800/level). Floor height is purely a
 * function of the band (Y = band*-0x708), so changing the band IS the descent.
 *
 * Faithful-available port: the player keeps full pad control during the descent;
 * wall collision is suppressed; Y sinks toward the next band as he WALKS; he
 * lands (band committed, Y snapped) when he walks out the far side of the stair
 * zone = onto the lower platform. One band-level per stair (data2=2): band4
 * courtyard -> band2 Zwischenebene -> band0 pit (doors). Interim animation = the
 * normal WALK clip (the real dedicated stair clips live in a separate per-char
 * stair bank, DAT_800741e8/DAT_80073f70, not yet extracted; PL00.edd clip 13 is
 * a KNOCKBACK, user-confirmed — must NOT be used).
 */
#ifndef RE15_STAIR_H
#define RE15_STAIR_H

#include <stdint.h>
#include "re15_rdt.h"
#include "re15_emd.h"   /* re15_emd_skeleton_t / animation_t for the foot-lock FK */

/* 1 while a stair descent/ascent is in progress. */
int  re15_stair_active(void);

/* Advance an in-progress traversal: force the stair clip, sink Y toward the
 * target band as the player WALKS (Y tied to movement), and finalize (commit
 * band + snap Y) when he leaves the stair zone. Call EVERY frame the traversal
 * is active, AFTER re15_player_tick (which applies his pad XZ), and SKIP the
 * normal wall/object collision + door scan for that frame (the 0x4000 latch). */
/* `skel`/`anim` = the PL00 base bank (stair clips 20/21) for the FK foot-lock
 * root motion (FUN_800390e0): the body follows the planted foot so the feet do
 * not slide. May be NULL (then only the forward + Y step move the player). */
void re15_stair_tick(const re15_rdt_t *rdt,
                     const re15_emd_skeleton_t  *skel,
                     const re15_emd_animation_t *anim);

/* Try to START a traversal: if the player just walked INTO a stair zone (edge)
 * and a lower/upper band exists, begin the descent/ascent and return 1 (the
 * caller should then SKIP the door scan this frame). Else return 0. Call once
 * per gameplay frame, AFTER the normal collision pass, when NOT already active.
 * `action_pressed` is used only for a debug position probe. */
int  re15_stair_try_start(const re15_rdt_t *rdt, int action_pressed);

/* Clear any in-progress traversal (call on room change). */
void re15_stair_reset(void);

#endif /* RE15_STAIR_H */
