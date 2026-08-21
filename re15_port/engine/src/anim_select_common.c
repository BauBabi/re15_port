/*
 * RE1.5 Rebuilt — SHARED actor animation selection + keyframe lookup.
 *
 * Unify 2026-06-06: extracted verbatim from the verified PSX main.c (the actor
 * render loop's bank/clip selection block + compute_actor_kf) so BOTH renderers
 * drive the same game logic. See re15_anim_select.h for the architecture.
 *
 * Provenance of the sentinel -> clip mapping (all code-verified this session):
 *   105 Walk  -> PL00W01 clip5      100 Run   -> PL00W01 clip0
 *   200 idle neutral / 210 settle / 211 sway / 212 hair  -> PL00W01 clip 3/1/2/4
 *     (healthy idle-fidget FSM, switchD_8003206c)
 *   213/214 injured idle -> PL00 base clip 22 (HP<50) / 23 (HP<30)
 *   else -> the actor's own cinematic bank, motion as the direct clip index.
 */
#include "re15_anim_select.h"
#include "re15_enemy.h"     /* generic enemy registry (re15_enemy_find) */
#include "re15_skeleton.h"  /* g_anim_kf_tween — the 0x8000 marker tween side channel */
#include "re15_engine.h"    /* g_engine.frame_count — the POSE-STREAM trace timestamp */
#include "re15_enemy_ai.h"  /* re15_actor_clip_len — the frame clock's view, for the cross-check */
#include <stdio.h>
#include <stdlib.h>

/* Per-actor keyframe lookup: motion + anim_frame -> skeleton keyframe index,
 * against the SELECTED anim/skel. (reverse playback, walk loop, freeze-on-end
 * all preserved.) `cur` = the already-fps-adjusted current anim frame. */
/* PLAYBACK-RICHTUNG — EINE Definition fuer den ganzen Port.
 *
 * Im Original ist die Richtung KEIN Aktor-Zustand, sondern das 3. Argument (a2) JEDES
 * einzelnen anim_set-Aufrufs. FUN_8001f314 waehlt damit den Frame-Index:
 *   8001f338: beq   t2,zero,0x8001f358   ; a2 == 0 -> VORWAERTS
 *   8001f344: lbu   v0,149(t0)           ;   sonst: v0 = +0x95 (Cursor)
 *   8001f34c: subu  v0,t1,v0             ;   v0 = Framezahl - Cursor
 *   8001f354: addiu v0,v0,-1             ;   v0 = Framezahl - Cursor - 1   <<< SPIEGELUNG
 *   8001f358: lbu   v0,149(t0)           ; vorwaerts: v0 = +0x95
 *   8001f364: sll   v0,v0,2
 *   8001f368: addu  a2,v1,v0             ; a2 = &frame_entry[index]
 *   8001f36c: sw    a2,360(t0)           ; +0x168 = ZEIGER AUF DAS FRAME-WORT
 * Der Port bildet die Richtung als Aktor-Bit anim_flags & 0x80 ab; damit MUSS jeder
 * Clip-Setzer sie explizit setzen ODER loeschen (ein stehengebliebenes Bit spielt den
 * naechsten Clip rueckwaerts — im Original unmoeglich, weil a2 pro Aufruf kommt).
 *
 * WARUM ZENTRAL: +0x168 ist der EINZIGE Frame-Zeiger im Original, und AUSSER dem Renderer
 * liest ihn auch der Frame-Wort-SE-Dekoder FUN_8001b38c (@0x8001b3a4 `lw v0,360(v0)` /
 * @0x8001b3ac `lw v0,0(v0)` / @0x8001b3b4 `srl s0,v0,22`). Pose und SE lesen also IMMER
 * denselben, richtungsrichtigen Slot. Eine zweite Kopie der Spiegel-Regel (oder ein
 * Dekoder, der sie weglaesst) laesst SEs bei Rueckwaerts-Clips an den gespiegelten
 * Frames feuern = falsche Reihenfolge/Zeitpunkte. */
int re15_actor_anim_reverse(const re15_actor_t *a)
{
    if (!a) return 0;
    return ((a->anim_flags & 0x80) || re15_actor_toggle_reverse(a)) ? 1 : 0;
}

int re15_actor_playback_slot(const re15_actor_t *a, uint32_t cur, int frame_count)
{
    if (frame_count <= 0) return 0;
    uint32_t m = cur % (uint32_t)frame_count;
    return re15_actor_anim_reverse(a)
         ? (int)((uint32_t)(frame_count - 1) - m)      /* @0x8001f34c-54 */
         : (int)m;                                     /* @0x8001f358 */
}

int re15_compute_actor_kf(const re15_emd_animation_t *anim,
                          const re15_emd_skeleton_t  *skel,
                          const re15_actor_t *a, int clip_override,
                          uint32_t cur)
{
    if (!anim || !skel || anim->clip_count <= 0) return 0;
    /* clip_override >= 0 = use it as the clip index (W01 walk-source remaps the
     * 105/100 sentinels to W01 clips 5/0, idle sentinels to fidget clips); else
     * the actor's own motion. */
    int clip_idx = (clip_override >= 0) ? clip_override : (int)a->motion;
    if (clip_idx < 0) clip_idx = 0;
    if (clip_idx >= anim->clip_count) clip_idx = clip_idx % anim->clip_count;
    const re15_emd_clip_t *clip = &anim->clips[clip_idx];
    if (clip->frame_count <= 0) return 0;

    int slot;
    int reverse = re15_actor_anim_reverse(a);   /* eine Definition, s.o. */
    /* ROOM1030 Kriechtor-Toggle, HINWEG (Hinlegen): f314 posiert bei a2 = (s8)+0x9F != 0
     * den GESPIEGELTEN Slot (fc - frame) - 1 — Rohbytes @0x8001f338 (beq a2) +
     * @0x8001f344-54 (lbu +0x95 / subu fc / addiu -1); Aufruf @0x8010506c. Eigene
     * Zustands-Abfrage statt anim_flags-Bit 0x80 (das Port-Reverse-Bit auf +0x1C4 ist
     * port-erfunden — Dossier §2). Der alte Richtungs-Anker "B3 §6 py-Rampe -1744->-175 =
     * Hinlegen = vorwaerts" ist WIDERLEGT (Keyframe-Rohdaten: Clip 0x12 frame 0 = py -175
     * LIEGEND, frame 97 = py -1744 STEHEND) — Ableitung in re15_actor_toggle_reverse. */
    /* BYTE-TRUE post-walk hold: a scripted Plc_dest walk/run that has ARRIVED sets
     * anim_freeze (actor_locomotion.c) — the original holds the walk clip, and FUN_8001f3bc
     * wraps the frame counter to 0 + freezes at clip-end → the arms-down walk-start pose.
     * Freeze at frame 0 here (the next Plc_motion's set_motion clears anim_freeze). This is
     * NOT the pad-walk (which never sets anim_freeze and keeps looping below). */
    if (a->anim_freeze) {
        slot = 0;
    } else if (a->walk_active || clip_override >= 0) {
        slot = re15_actor_playback_slot(a, cur, clip->frame_count);
    } else if (reverse) {
        uint32_t idx = (cur >= (uint32_t)clip->frame_count)
                       ? (uint32_t)(clip->frame_count - 1) : cur;
        slot = (int)((uint32_t)(clip->frame_count - 1) - idx);
    } else if (cur >= (uint32_t)clip->frame_count) {
        /* HOLD-LAST: byte-true (FUN_8001f314) a frame entry with bit 0x8000 is a
         * TERMINAL marker — never posed (the original routes it to FUN_8001f8b4,
         * which skips to the prior real frame). Our old `frame_count-1` could land
         * on the marker, whose 0xFFF bits are a garbage keyframe index → the
         * ROOM1150 Irons "twisted legs" on the held last frame. Skip trailing
         * markers so we hold the last RENDERED pose. */
        int last = clip->frame_count - 1;
        while (last > 0 && (anim->frames[clip->first_frame + last] & 0x8000u))
            last--;
        slot = last;
    } else {
        slot = (int)cur;
    }
    /* POSE-STREAM TRACE (RE15_ANIM_TRACE=<path>) — this is the ONLY place a pose is resolved, so a
     * line here is the ground truth of "what was actually animated this frame". Two things are
     * logged that a state log can never show: the RESOLVED frame slot (which is what the eye sees)
     * and, side by side, the frame count the AI's clip clock would use for the same actor. The
     * checker (tools/check_anim_trace.py) turns both into pass/fail:
     *   - slot sequences of a looping clip must be a clean 0..fc-1 ramp; a mid-cycle jump is
     *     exactly the "animations loop oddly" regression, caught automatically;
     *   - fc_render must equal fc_clock for every single line, or the state machine is timing
     *     against a different clip than the one on screen. */
    { const char *tp = getenv("RE15_ANIM_TRACE");
      if (tp) {
          static FILE *tf = NULL; static int tried = 0;
          if (!tried) { tried = 1; tf = fopen(tp, "w"); }
          if (tf) fprintf(tf, "%u %d %d %d %d %d %u %d %d %d %d\n",
                          (unsigned)g_engine.frame_count, (int)(a - g_actors), (int)a->type,
                          (int)a->motion, clip_idx, clip->frame_count, (unsigned)cur, slot,
                          re15_actor_clip_len(a),
                          re15_actor_uses_loco_bank(a) ? 1 : 0, reverse,
                          re15_actor_clip_len_legacy(a));
      } }

    /* 0x8000 MARKER frames are TWEENs, never posed as a keyframe (byte-true FUN_8001f314
     * @0x8001f370-f37c routes them to FUN_8001f8b4; trace wf_518cceff adversarially CONFIRMED):
     * the original poses  ((0x1000-w)*A + w*B) >> 12  for the root translation AND every bone
     * angle, where A = the last REAL frame posed (== the bracketing real frame BEFORE the marker),
     * B = the next REAL frame in playback direction, w = marker & 0xfff. The reverse path
     * complements w (@0x8001f940-f948), which yields the SAME absolute pose — so resolve it
     * direction-independently: A = first real frame scanning BACKWARD (wrap), B = first real
     * scanning FORWARD (wrap), w = fraction toward B. Arm the g_anim_kf_tween side channel with
     * (A, w) and return B; the next re15_skel_compute_pose performs the lerp (root GPF12/GPL12 +
     * per-bone FUN_80020510). Shipped markers: EM013 clip 31 (w=0x27) + EM02B clip 6 (5x w=0x800). */
    g_anim_kf_tween.active = 0;
    if (anim->frames[clip->first_frame + slot] & 0x8000u) {
        int fc     = clip->frame_count;
        int w      = (int)(anim->frames[clip->first_frame + slot] & 0x0FFFu);
        int before = slot, after = slot;
        for (int n = 0; n < fc && (anim->frames[clip->first_frame + before] & 0x8000u); n++)
            before = (before + fc - 1) % fc;
        for (int n = 0; n < fc && (anim->frames[clip->first_frame + after] & 0x8000u); n++)
            after = (after + 1) % fc;
        int kf_from = (int)(anim->frames[clip->first_frame + before] & 0xFFFu);
        if (kf_from >= skel->keyframe_count)
            kf_from = skel->keyframe_count > 0 ? skel->keyframe_count - 1 : 0;
        g_anim_kf_tween.active  = 1;
        g_anim_kf_tween.kf_from = kf_from;
        g_anim_kf_tween.weight  = w;
        slot = after;
    }
    int kf = (int)(anim->frames[clip->first_frame + slot] & 0xFFFu);
    if (kf >= skel->keyframe_count)
        kf = skel->keyframe_count > 0 ? skel->keyframe_count - 1 : 0;
    return kf;
}

/* FOOT-PLANT query (byte-true LAB_80030af0 walk / LAB_80030d28 run): resolves the
 * SAME clip+slot as re15_compute_actor_kf, then tests the current frame entry's
 * foot-plant flag. Returns 0 = no foot-plant this frame, else the foot code 7
 * (left) / 4 (right) from bit 0x1000 — the exact value the handler passes as
 * FUN_80045630's `foot` arg. Caller gates on walk/run motion. */
int re15_actor_footstep(const re15_emd_animation_t *anim,
                        const re15_actor_t *a, int clip_override, uint32_t cur)
{
    if (!anim || anim->clip_count <= 0) return 0;
    if (a->anim_freeze) return 0;   /* arrived (frozen at frame 0) → stationary, no footstep */
    int clip_idx = (clip_override >= 0) ? clip_override : (int)a->motion;
    if (clip_idx < 0) clip_idx = 0;
    if (clip_idx >= anim->clip_count) clip_idx = clip_idx % anim->clip_count;
    const re15_emd_clip_t *clip = &anim->clips[clip_idx];
    if (clip->frame_count <= 0) return 0;
    /* walk/run always loop the clip (clip_override>=0) — slot = cur % frame_count
     * (reverse for back-walk), matching compute_actor_kf's loop branch. */
    int reverse = (a->anim_flags & 0x80) ? 1 : 0;
    uint32_t mod = cur % (uint32_t)clip->frame_count;
    int slot = reverse ? (int)((uint32_t)(clip->frame_count - 1) - mod) : (int)mod;
    uint32_t entry = re15_emd_get_frame_entry(anim, clip_idx, slot);
    if (!(entry & 0x4000)) return 0;            /* no foot-plant this frame */
    return (entry & 0x1000) ? 4 : 7;            /* left / right foot (bit 0x1000) */
}

void re15_actor_anim_select(const re15_actor_t *a, int is_player,
                            const re15_anim_banks_t *banks,
                            re15_anim_view_t *out)
{
    /* Default: the actor's own model + texture, its own anim/skel, no clip
     * remap (cinematic motions index the actor's bank directly). */
    out->mesh   = banks->def_mesh;
    out->skel   = banks->def_skel;
    out->anim   = banks->def_anim;
    out->clip_override = -1;
    out->tpage  = -1;
    out->clut   = -1;
    out->yshift = 0;

    /* Player bank select: anim_use_pl00 → PL00.EDD (the original's COMMON bank) instead of the
     * room RBJ overlay (def_*). ROOM1150's kneel needs PL00 clip 11 (the real stand→kneel pelvis
     * fold py -1810→-761); the RBJ record-0 clip 11 is only a kneeling SETTLE (py const -761)
     * which forced the FRAC crossfade to fake the whole descent → "ruckartig/zu schnell". No
     * effect in RBJ-less rooms (def_* == PL00). NB: anim_use_pl00 is a RESULT-CORRECT selector
     * derived from the Plc_motion entity operand; it picks COMMON(PL00.EDD)-vs-RBJ, which the
     * entity byte co-varies with in all shipped data — see memory anim_bank_selection_mechanism
     * for the full RE (the original's exact COMMON-vs-RBJ pose selector is not fully nailed). */
    if (is_player && a->anim_use_pl00 && banks->pl00_ok) {
        out->skel = banks->pl00_skel;
        out->anim = banks->pl00_anim;
    }
    out->xshift = -1;
    out->pc_tex_slot = -1;

    /* Elliot (NPC type 0x47): his own PL05 skel/anim/md1 + relocated TIM. His
     * texture spans 2 tpages (body / face+arm) AND 2 CLUT rows, so keep BOTH
     * per-tri (tpage/clut = -1) and shift X by his VRAM base, clut by his row
     * delta — never a single override (that drew the skin meshes from the body
     * half). The texture relocation survives the motion overrides below. */
    if (!is_player && a->type == 0x47 && banks->elliot_ok) {
        out->mesh   = banks->elliot_mesh;
        out->skel   = banks->elliot_skel;
        out->anim   = banks->elliot_anim;
        out->tpage  = -1;
        out->xshift = banks->elliot_tpage_xshift;
        out->clut   = -1;
        out->yshift = banks->elliot_clut_yshift;
    }

    /* GENERIC enemy (globalization 2026-06-13; em21 folded in 2026-06-14f): ANY non-player,
     * non-Elliot Sce_em_set type whose EM<NN>.EMD is loaded in the shared registry uses its
     * own mesh/skel/anim + relocated texture — data-driven, NO per-type code. em21 (the
     * helipad crows, type 0x21) is no longer special: it loads via the same generic loader
     * into the per-room enemy arena (was a dedicated 73 KB resident buffer = room1170-only
     * ballast). Its own anim bank is used directly (motion = clip index; a fresh spawn has
     * motion 0 = idle clip 0) and the W01 locomotion sentinels below never match, so the
     * idle pose stands. (The banks->em21_* fields are now unused; kept for ABI simplicity.) */
    if (!is_player && a->type != 0x47) {
        re15_enemy_bank_t *eb = re15_enemy_find(a->type);
        if (eb) {
            out->mesh   = &eb->md1;
            out->skel   = &eb->skel;
            out->anim   = &eb->anim;
            out->tpage  = -1;
            out->xshift = eb->xshift;
            out->clut   = -1;
            out->yshift = eb->yshift;
            out->pc_tex_slot = eb->pc_tex_slot;
            /* Continuous poses (idle, feeding, breathing) LOOP — clip_override>=0 makes
             * re15_compute_actor_kf cycle the action. One-shot SETTLE poses (the lie-down /
             * downed family the AI plays ONCE then holds — a corpse must NOT keep "standing
             * up" on a loop) play once and freeze on their final lying frame: leave
             * clip_override = -1 so the play-once-hold path runs (it still uses a->motion as
             * the clip). The settle set = the zombie decoder's lie-down outputs (0x0C/0E/12/13).
             * a->motion is seeded byte-true from the behavior byte by re15_enemy_spawn_action. */
            /* LOOP the enemy's EDD action (re15_compute_actor_kf: clip_override>=0 → slot =
             * cur % frame_count). The byte-true original wraps the subframe to 0 at clip end
             * (FUN_8001f3bc) — it NEVER freezes the last frame, so the earlier `settle`
             * play-once-hold (-1) was wrong: it held the lie-down/get-up clip (0x13) on its
             * STANDING last frame → the "corpse" stood. Now all enemy actions loop; a downed
             * corpse is kept on its LYING start frame by freezing its anim_frame at 0
             * (player_common.c), so slot = 0 % frame_count = 0 = the prone pose.
             *
             * BYTE-TRUE PLAYBACK MODE (FUN_80050cb8 phase FSM): anim-flags (entity+0x1c4) bit
             * 0x04 = LOOP → clip_override = the action (re15_compute_actor_kf cycles slot =
             * cur % frame_count); CLEAR (default) = play-once-then-HOLD-LAST → clip_override =
             * -1. A continuous idle/feeding/walk enemy carries 0x04 (set at spawn) and loops; a
             * downed/corpse leaves 0x04 clear and is pinned to its prone start frame by the
             * anim_frame=0 freeze above. ROOM1150 Irons gets 0x04 from Plc_flg(0,4) so his
             * lying RBJ clip 3 loops = the wounded breathing. (bit 0x80 reverse is decoded in
             * re15_compute_actor_kf; the playback-MODE is entirely the 0x04 gate.) */
            out->clip_override = (a->anim_flags & 0x04) ? (int)a->motion : -1;
        }
    }

    /* Locomotion / idle pose source by motion sentinel. These swap only the
     * POSE source (skel + anim) and the clip; the mesh + texture chosen above
     * stay. Player-only seit 7b (2026-08-09): Elliot (0x47) traegt keine
     * Sentinels mehr — seine Plc_dest-Walks laufen in der NPC-Sub-VM (Clips
     * 5/0 der EM047-Own-Bank, Render-Override in main.c). */
    int m = (int)a->motion;
    /* KISTEN SCHIEBEN (Spieler-Substate 8) — die Pose kommt aus der COMMON-Bank, NIE aus dem
     * Raum-RBJ. Der Schiebe-Handler LAB_80035810 uebergibt an JEDER seiner vier anim_set-Stellen
     * dieselben beiden Globals:
     *   @0x800358bc `lw a0,DAT_800acad8` / @0x800358c4 `lw a1,DAT_800acbc0`   (Fall 1, Clip 0)
     *   @0x80035960 / @0x80035968                                            (Fall 3, Clip 0x11)
     *   @0x80035a2c / @0x80035a34                                            (Fall 5, Clip 0x12)
     *   @0x80035a80 / @0x80035a88                                            (Fall 6, 0x11 rueckw.)
     * und diese beiden Zeiger haben im GESAMTEN PSX.EXE genau EINEN Schreiber:
     *   @0x8003154c `sw v0,-0x3440(at)=>DAT_800acbc0`  v0 = PLD-Directory[0] = PL00.EDD
     *   @0x80031578 `sw v0,-0x3528(at)=>DAT_800acad8`  v0 = PLD-Directory[1] = PL00.EMR
     * (eigener Scan ueber ghidra1_V2.txt: 62 Vorkommen, davon exakt diese 2 Stores.) Das
     * Raum-RBJ (FUN_8001b3f8) repointet nur Entity-Felder, nie diese Globals.
     *
     * IST-MESSUNG vor dem Fix (probe_1090_push_anim, echte ROOM1090.RDT + echte PL00-Baenke):
     * der Port haengte Leons Default-Bank am RDT-Animationsblock (@0x5C, 2 Records, rec0 =
     * 25 Clips) — Clip 0x11 posierte 30 statt 10 Frames, Clip 0x12 20 statt 25, Clip 0
     * (Yaw-Rastung) 1 statt 34; Phase 6 lief rueckwaerts durch die Slots 29..20 eines fremden
     * Clips. 27 von 27 Schiebe-Bildern trugen die falsche Cliplaenge. */
    {
        extern int re15_player_push_substate(void);
        if (is_player && banks->pl00_ok && re15_player_push_substate()) {
            out->skel = banks->pl00_skel;
            out->anim = banks->pl00_anim;
            out->clip_override = m;      /* +0x94 ist im Original der DIREKTE Clip-Index */
            return;
        }
    }
    if (banks->w01_ok && (m == 105 || m == 100)) {
        /* Walk -> W01 clip5, Run -> W01 clip0 (PL00W01 weapon track). BYTE-TRUE, definitively RE'd
         * (wf_9970157f): the real-time move cmd (cmd 1, 0x80031de8) two-pass-dispatches on submode byte
         * 0x800aca59 via table @0x80073ff0: [1] = WALK executor 0x80032498 writes player+0x94 = 5
         * (@0x800324e8 `ori 0x5; sb`) at speed 0x4b=75; [2] = RUN executor 0x80032778 (a SEPARATE fn)
         * writes player+0x94 = 0 (@0x8003280c `sb zero`) at speed 0xc8=200. So walk=clip5/run=clip0 is
         * exact — this port map matches. (audit wf_2ff80cc7 claimed run->clip5 but traced the SCRIPTED
         * Plc_dest walker 0x80030d28 @0x80073e30[5], NOT this real-time run executor — refuted.) Both read
         * the same weapon bank ptr 0x800acbc8 (FUN_80036b68 loads PL00Wxx when weapon 0x800aca5d!=0). */
        out->skel = banks->w01_skel;
        out->anim = banks->w01_anim;
        out->clip_override = (m == 105) ? 5 : 0;
    } else if (banks->w01_ok && (m == 200 || m == 210 || m == 211 || m == 212)) {
        /* HEALTHY IDLE-FIDGET (byte-exact switchD_8003206c) plays the WEAPON
         * bank = PL00W01: neutral=clip3, settle=clip1, sway/breathe=clip2,
         * hand-thru-hair=clip4. */
        out->skel = banks->w01_skel;
        out->anim = banks->w01_anim;
        out->clip_override = (m == 200) ? 3 :
                             (m == 210) ? 1 :
                             (m == 211) ? 2 : 4;
    } else if (banks->pl00_ok && (m == 213 || m == 214)) {
        /* INJURED IDLE (HP-gated, switchD_8003206c case9/a) plays the COMMON
         * bank = PL00.edd: clip 22 when HP<50, clip 23 when HP<30. */
        out->skel = banks->pl00_skel;
        out->anim = banks->pl00_anim;
        out->clip_override = (m == 213) ? 22 : 23;
    } else if (banks->pl00_ok && (m == 220 || m == 221)) {
        /* STAIR gait = PL00 clip 21 (DESCEND) / 20 (ASCEND) — USER-CONFIRMED the
         * correct stepping animation (the legs step; the clip carries NO body
         * translation, so re15_stair_tick drives the XZ travel + Y). (W01 clip 13
         * was the knife-attack-ready anim — wrong.) */
        out->skel = banks->pl00_skel;
        out->anim = banks->pl00_anim;
        out->clip_override = (m == 220) ? 21 : 20;   /* 220 down=PL00 c21 / 221 up=c20 */
    } else if (banks->w01_ok && m == 230) {
        /* KLETTERN Phase 0/1 (LAB_80037fd8 case0/1): `+0x94 = 5` @0x80038030 und
         * `anim_set(*(0x800acbc4), *(0x800acbc8), 0, 0x200)` @0x8003809c — die
         * Bank ist die PLW/W01-Bank (ihre beiden Zeiger werden im ganzen EXE nur
         * von FUN_80036b68 @0x80036be4/@0x80036c04 aus dem PLW-Archiv gesetzt),
         * also W01 clip 5 (30 Frames). */
        out->skel = banks->w01_skel;
        out->anim = banks->w01_anim;
        out->clip_override = 5;
    } else if (banks->pl00_ok && m >= 231 && m <= 235) {
        /* KLETTERN aus der PL00-Bank (`anim_set(*(0x800acad8), *(0x800acbc0), …)`):
         *   231 Aufstieg      = clip 2 @0x80038134  (50 Frames)
         *   232 Absprung-Vorb.= clip 3 @0x80038368  (26 Frames)
         *   233 Fall          = clip 4 @0x800383d8  (20 Frames)
         *   234/235 Landung   = clip 5 / 6 @0x800386e4 / @0x80038700 (20 / 50 Frames) */
        out->skel = banks->pl00_skel;
        out->anim = banks->pl00_anim;
        out->clip_override = (m == 231) ? 2 : (m == 232) ? 3 : (m == 233) ? 4
                           : (m == 234) ? 5 : 6;
    } else if (m == 200) {
        /* IDLE neutral fallback when PL00W01 is unavailable: idle-bank clip 6
         * (1-frame static rest pose). */
        out->clip_override = 6;
    }
}
