/*
 * RE1.5 Rebuilt — player vs room SCA collision (2026-06-04).
 *
 * The room's collision is the SCA block (re15_rdt_t.sca): shaped rectangles
 * defining the walkable space. The player must stay within the union of the
 * walkable entries — this stops the player from "flying" off the rooftop /
 * through the boundary. Mirrors RE1.5 FUN_8003b0a4 / RE2 FUN_8004fba0 (per-
 * shape test + position clamp); this first pass treats every entry as its
 * bounding rectangle (covers type 1 rect; per-shape circle/diagonal/slope
 * refinement is a follow-up) and slides along walls.
 */
#ifndef RE15_COLLISION_H
#define RE15_COLLISION_H

#include <stdint.h>
#include "re15_rdt.h"

/* 1 if (x,z) is inside the walkable SCA geometry (or no SCA data → 1). */
int  re15_collision_on_floor(const re15_rdt_t *rdt, int32_t x, int32_t z);

/* Constrain a move from (old_x,old_z) to (*x,*z): if the target leaves the
 * walkable floor, slide along whichever axis stays on it; if neither does,
 * keep the old position. */
void re15_collision_constrain(const re15_rdt_t *rdt,
                              int32_t old_x, int32_t old_z,
                              int32_t *x, int32_t *z);

/* Object (Obj_model_set prop) push-out: keep the player out of every SOLID prop's
 * authored collision box (FUN_8002cabc). Call AFTER re15_collision_constrain in the
 * player-move pass (walls first, then objects). Reads g_scd.props directly. */
void re15_collision_objects(int32_t *x, int32_t *z);

/* Reset the tracked floor band (call on room change so the previous room's band
 * doesn't leak into the new room). */
void re15_collision_reset_band(void);

/* Set the floor band explicitly (doors carry it in the spawn Y). */
void re15_collision_set_band(int band);
/* Convert a spawn Y to a floor band, as the original does: -(Y / 0x708). */
int  re15_collision_band_from_y(int32_t y);
/* Current tracked floor band (diagnostic). */
int  re15_collision_debug_band(void);
/* Set the band from the player Y only if it isn't set yet (initial spawn). */
void re15_collision_ensure_band(int32_t y);

/* Report the lowest and highest floor bands (= floor>>4) present among the
 * room's SCA cells. Used by the stair traversal to pick the descent/ascent
 * target (the original walks bands down/up to the next populated floor —
 * FUN_8001c2dc). Either out-pointer may be NULL. If there is no SCA data both
 * are left untouched / set to the current band. */
void re15_collision_band_range(const re15_rdt_t *rdt, int *min_band, int *max_band);

/* The next SCA band strictly BELOW / ABOVE `cur` (the next floor down/up a stair
 * leads to — the original descends ONE band-level per stair, data2=2). Returns
 * `cur` unchanged when there is no lower/higher band. */
int  re15_collision_next_band_below(const re15_rdt_t *rdt, int cur);
int  re15_collision_next_band_above(const re15_rdt_t *rdt, int cur);

/* Containment floor query (FUN_8003b7f0 / FUN_8001c6e8): the BAND of the SCA
 * floor cell that CONTAINS (x,z), scanning DOWN from `start_band` to 0 (the
 * original returns the first walkable band found below the player). Returns the
 * band, or -1 if (x,z) is over no cell at any band <= start_band. The stair uses
 * this on a forward probe point to detect ARRIVAL on the destination platform
 * (the byte-true end condition: player.Y == floor-probe-ahead). */
int  re15_collision_floor_band_at(const re15_rdt_t *rdt,
                                  int32_t x, int32_t z, int start_band);

/* The centroid (average cell-centre) of all SCA cells of `band` — used by the
 * stair as the destination platform's walkable centre to auto-walk Leon to
 * (mirrors the original deriving the descent target from the stair geometry).
 * For a perimeter-ring platform this is the walkable interior. Returns 1 and
 * fills *cx,*cz on success; 0 if the band has no cells. */
int  re15_collision_band_centroid(const re15_rdt_t *rdt, int band,
                                  int32_t *cx, int32_t *cz);

#endif /* RE15_COLLISION_H */
