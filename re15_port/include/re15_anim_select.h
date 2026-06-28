/*
 * RE1.5 Rebuilt — SHARED actor animation SELECTION (view-model).
 *
 * Unify 2026-06-06: the "which skeleton / animation / clip for this actor"
 * decision is GAME LOGIC, not platform code, so it lives here and drives BOTH
 * the PSX (GTE) and PC (SDL) renderers. Each build fills a re15_anim_banks_t
 * from its own bank storage (PSX file-scope globals re15_w01_x, re15_pl00_x, ...,
 * PC main() locals) and calls re15_actor_anim_select(); the platform renderer
 * then consumes the resulting re15_anim_view_t (skel/anim/mesh/clip + texture
 * relocation) without re-deciding any game logic.
 *
 * Ported verbatim from the verified PSX main.c selection block + compute_actor_kf
 * (locomotion 105/100 -> PL00W01 walk/run; idle-fidget 200/210/211/212 ->
 * PL00W01; HP-gated injured idle 213/214 -> PL00 base; Elliot type 0x47 -> his
 * own bank + 2-tpage/2-CLUT relocation; else the actor's own cinematic bank).
 */
#ifndef RE15_ANIM_SELECT_H
#define RE15_ANIM_SELECT_H

#include <stdint.h>
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_actor.h"

/* Bank pointers the platform supplies (it owns the storage; we only read). */
typedef struct {
    /* The actor's DEFAULT bank + mesh (Leon / the bundled test model). */
    const re15_md1_t           *def_mesh;
    const re15_emd_skeleton_t  *def_skel;
    const re15_emd_animation_t *def_anim;

    /* PL00W01 weapon track — real Walk_Forward (clip5)/Run (clip0) + the
     * healthy idle-fidget poses (clips 1/2/3/4). */
    const re15_emd_skeleton_t  *w01_skel;
    const re15_emd_animation_t *w01_anim;
    int                         w01_ok;

    /* PL00 base track — HP-gated injured idle (clip22 <50 HP / clip23 <30 HP). */
    const re15_emd_skeleton_t  *pl00_skel;
    const re15_emd_animation_t *pl00_anim;
    int                         pl00_ok;

    /* Elliot (NPC type 0x47) own bank + relocated texture (body/face span two
     * tpages + two CLUT rows). Ignored for the player. */
    const re15_md1_t           *elliot_mesh;
    const re15_emd_skeleton_t  *elliot_skel;
    const re15_emd_animation_t *elliot_anim;
    int                         elliot_ok;
    int                         elliot_tpage_xshift;
    int                         elliot_clut_yshift;
    /* Enemy type 0x21 (em21.emd) — own EMD mesh/skel/anim + relocated TIM. */
    const re15_md1_t           *em21_mesh;
    const re15_emd_skeleton_t  *em21_skel;
    const re15_emd_animation_t *em21_anim;
    int                         em21_ok;
    int                         em21_tpage_xshift;
    int                         em21_clut_yshift;
} re15_anim_banks_t;

/* The selector's output — the per-actor render view-model. */
typedef struct {
    const re15_md1_t           *mesh;
    const re15_emd_skeleton_t  *skel;
    const re15_emd_animation_t *anim;
    int clip_override;   /* >=0 = use as the clip index; <0 = the actor's own motion */
    int tpage;           /* texture overrides: -1 = per-tri / baked texture */
    int clut;
    int yshift;          /* CLUT-row shift (Elliot's relocated palette) */
    int xshift;          /* tpage X shift (Elliot's VRAM base); -1 = none */
    int pc_tex_slot;     /* PC render texture slot for a generic enemy bank; -1 = none
                          * (PSX ignores this; it uses tpage/clut/xshift/yshift). */
} re15_anim_view_t;

/* Pick the skeleton/animation/mesh/clip + texture relocation for one actor from
 * its motion sentinel. is_player gates the Elliot (type 0x47) NPC branch off for
 * slot 0. Platform-free — references only the supplied banks. */
void re15_actor_anim_select(const re15_actor_t *a, int is_player,
                            const re15_anim_banks_t *banks,
                            re15_anim_view_t *out);

/* motion/clip_override + the (already fps-adjusted) current anim frame `cur` ->
 * skeleton keyframe index, against the SELECTED anim/skel. Preserves the walk
 * loop, reverse playback (anim_flags 0x80) and freeze-on-end behaviour. `cur`
 * is passed explicitly so the platform owns the fps policy (PSX 30Hz = raw
 * anim_frame; PC halves it at 60fps). */
int re15_compute_actor_kf(const re15_emd_animation_t *anim,
                          const re15_emd_skeleton_t  *skel,
                          const re15_actor_t *a, int clip_override,
                          uint32_t cur);

/* Foot-plant query for the footstep SE (byte-true LAB_80030af0/d28). Returns 0 =
 * no foot-plant on the current frame, else 7 (left) / 4 (right). Caller gates on
 * walk/run motion + the walk-source (W01) anim. */
int re15_actor_footstep(const re15_emd_animation_t *anim,
                        const re15_actor_t *a, int clip_override, uint32_t cur);

#endif /* RE15_ANIM_SELECT_H */
