/* test_re2z_devour_anchor.c — PIN `unit_re2z_devour_anchor`.
 *
 * NUTZER-REPORT 2026-08-23 (RE2-KI-Modus): "Wenn Leon stirbt und von Zombies gefressen wird,
 * ist der Zombie bei der Finisher-Position irgendwie AM BEIN, nicht mehr wie sonst am
 * Oberkoerper/Kopf."
 *
 * IST (gemessen, probe_re2z_devour — ROOM1140, echter game_step, geladene RE2-Baenke, beide
 * Aktoren mit der Bankwahl DES RENDERERS posiert): der Zombie stand den GESAMTEN Fress-Vorgang
 * bewegungslos auf seiner Todesstoss-Position, waehrend Leons Kollaps-Clip ihn wegzog. Der
 * naechstliegende Leon-Knochen zum Zombiekopf war in 727 von 800 gemessenen Ticks (91 %) ein
 * BEIN-Knochen (b2 Oberschenkel 339x, b3 Knie 291x, b6 75x, b5 22x).
 * PL00-Knochenhoehen stehend (Clip 0 F0, y negativ = oben): b0 -1804 (Huefte), b1 -1825,
 * b2 -1706, b3 -1011, b4 -266 (Fuss), b5 -1701, b6 -969, b7 -166 (Fuss), b8 -2513
 * (Oberkoerper/Kopf), b9/b12 -2499 (Schultern), b10/b13 -208x (Ellenbogen), b11/b14 -168x
 * (Haende).
 *
 * SOLL (selbst disassembliert, info/re2leon/COMMON/BIN/EMOVL10_S0.BIN RAW @0x80100000):
 *   EXEC[6] "ueber der Leiche" @0x80103954, P0 @0x801039B0:
 *     801039b0 ori   v0,v0,0x18      ; v0 = lui 0xf @0x80103990 -> Clip-Wort 0x000F0018
 *     801039b4 lui   a0,0x800d
 *     801039b8 addiu a0,a0,-1032     ; a0 = 0x800CFBF8 = DIE SPIELER-STRUKTUR
 *     801039bc addu  a1,s1,zero      ; a1/a2 = Bank-Paar des Dispatchers (Paar 2)
 *     801039c0 addu  a2,s2,zero
 *     801039c4 addu  a3,zero,zero
 *     801039c8 sw    v0,332(s0)      ; +0x14C = 0x18, +0x14D = 0 (Frame!), +0x14E = 0xF
 *     801039d0 jal   0x80015b94      ; ** ANKER **
 *     801039d4 sb    v0,6(s0)        ; Delay-Slot: Phase 1 -> FALLTHROUGH
 *   P1 @0x80103A08:
 *     80103a08 addu  a0,s0,zero      ; a0 = DER ZOMBIE
 *     80103a14 jal   0x80015cb8      ; ** PLATZIERUNG **
 *     80103a28 jal   0x8002959c      ; Advance (a3 = 256 @0x80103A2C)
 *   0x80015B94 (info/re2leon/PSX.EXE):
 *     80015bc0/c4 lui s0,0x800d / lw s0,-7376(s0)  ; s0 = *(0x800CE330) = getickte Entity
 *     80015bd0    jal 0x80015db0                   ; off = Wurzel-Offset(+0x14C, +0x14D)
 *     80015c24    lh  a0,118(s0)                   ; gedreht mit dem Entity-Yaw
 *     80015c40-78 sh (pos-off) -> 356/358/360(s0)  ; s0->Anker = pos - RotY*off
 *     80015c7c/88/94 sh ... -> 356/358/360(s3)     ; KOPIE in a0 = DEN SPIELER
 *   0x80015CB8: `lh a0,118(s1)` @0x80015D3C + `sw (Anker+off) -> 56/60/64(s1)` @0x80015D58-90
 *   0x80015DB0: Clip `lbu v0,332(t1)` @0x80015DBC, Frame `lbu v0,333(t1)` @0x80015DF4
 *
 * Der Port hatte BEIDE Aufrufe nicht (Anker als "OPEN" kommentiert, Platzierung fehlte ganz).
 *
 * GEPRUEFT WIRD:
 *   (A) Anker-Gleichung am Eintritts-Tick, UNABHAENGIG aus den Bank-Keyframes nachgerechnet
 *   (B) Spieler-Anker == Zombie-Anker (die Kopie @0x80015C7C/C88/C94)
 *   (C) Platzierungs-Gleichung in JEDEM P1-Tick (pos == Anker + RotY*off(Clip 0x18, Frame))
 *   (D) Der Zombie legt ueber den Clip die AUTORISIERTE Strecke zurueck (0 ohne Platzierung)
 *   (E) NEGATIV-KONTROLLE/Nutzer-Symptom: der naechste Leon-Knochen zum Zombiekopf ist
 *       ueberwiegend OBERKOERPER/ARM (b0, b8..b14), nicht BEIN (b1..b7)
 * (C)/(D)/(E) werden rot, sobald die Platzierung @0x80103A14 zurueckgedreht wird; (A)/(B)
 * werden rot, sobald der Anker @0x801039D0 zurueckgedreht wird.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern uint32_t re15_re2_rand(void);
extern int      re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern void     re15_re2z_onesave_reset(void);

static int s_fail = 0;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  OK   " __VA_ARGS__); printf("\n"); } \
    else      { printf("  FAIL " __VA_ARGS__); printf("\n"); s_fail++; } \
} while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
static int s_pl00_ok = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* UNABHAENGIGE Nachrechnung von 0x80015DB0: Wurzel-Offset des Keyframes zu (Clip, Frame). */
static int clip_root_off(const re15_enemy_bank_t *b, int clip, int frame,
                         int16_t *sx, int16_t *sy, int16_t *sz)
{
    if (!b || clip < 0 || clip >= b->anim.clip_count) return 0;
    const re15_emd_clip_t *c = &b->anim.clips[clip];
    if (c->frame_count <= 0) return 0;
    int slot = frame % c->frame_count;
    int kf = (int)(b->anim.frames[c->first_frame + slot] & 0xFFFu);
    return re15_emd_get_keyframe_speed(&b->skel, kf, sx, sy, sz) ? 1 : 0;
}

/* LEON, GERENDERT: PL00-Knochen + Keyframe-Pool der Opfer-Bank des Greifers (main.c). */
static int leon_bones(const re15_actor_t *pl, uint8_t gtype, re15_skel_pose_t *out, int *nbone)
{
    if (!s_pl00_ok) return 0;
    re15_enemy_bank_t *vb = re15_enemy_find(gtype);
    if (!vb || !vb->victim_ok || vb->anim_victim.clip_count <= 0) return 0;
    if ((int)pl->motion >= vb->anim_victim.clip_count) return 0;
    re15_emd_skeleton_t vs = s_pl00_skel;
    vs.keyframe_data       = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, pl, (int)pl->motion,
                                   (uint32_t)pl->anim_frame);
    if (kf < 0) return 0;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, out);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    *nbone = vs.bone_count;
    return 1;
}

/* ZOMBIE, GERENDERT: Bankwahl wie der Renderer (re15_re2z_poses_loco_bank; EXEC[6] -> Paar 2). */
static int zombie_bones(const re15_actor_t *e, re15_skel_pose_t *out, int *nbone)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_skeleton_t  *s = &b->skel;
    const re15_emd_animation_t *a = &b->anim;
    if (b->loco_ok && re15_re2z_poses_loco_bank(e)) { s = &b->skel_loco; a = &b->anim_loco; }
    if (s->bone_count <= 0 || a->clip_count <= 0) return 0;
    int kf = re15_compute_actor_kf(a, s, e, -1, (uint32_t)e->anim_frame);
    if (kf < 0) return 0;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(s, kf, out);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    *nbone = s->bone_count;
    return 1;
}

static double dist3(const int32_t a[3], const int32_t b[3])
{
    double dx = (double)a[0] - b[0], dy = (double)a[1] - b[1], dz = (double)a[2] - b[2];
    return sqrt(dx * dx + dy * dy + dz * dz);
}

int main(void)
{
    printf("== PIN: RE2-Zombie-Finisher — Anker @0x801039D0 + Platzierung @0x80103A14 ==\n");

    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) {
        printf("SKIP: ROOM1140.RDT fehlt\n"); return 77;
    }
    {   size_t a = 0, b = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
        s_pl00_ok = (edd && emr &&
                     re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
                     re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0);
    }
    if (!s_pl00_ok) { printf("SKIP: PL00-Rig fehlt\n"); return 77; }

    int seeds = 2, devours = 0;
    int leg_ticks = 0, upper_ticks = 0;          /* (E) */
    int place_ticks = 0, place_bad = 0;          /* (C) */
    double max_travel = 0.0;                     /* (D) */

    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
        re15_re2z_onesave_reset();
        re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
        g_current_room_id = 0x1140;
        if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
        if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
        g_scd.work_vars[10] = 0;
        for (int i = 0; i < 120; i++) scd_vm_tick();
        if (!load_bank2(0x10)) { printf("SKIP: RE2-Bank EM010 fehlt\n"); return 77; }
        load_bank2(0x11); load_bank2(0x16);

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
        pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
        re15_collision_set_band(0);
        re15_player_set_aim_clip_len(12);
        re15_inv_load_briefing();
        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

        int zs = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) {
                int nib = g_actors[s].grid_id & 0x0f;
                if (zs < 0 && nib != 7 && nib != 8) zs = s;
            }
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (s != zs && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
                g_actors[s].active = 0;
        if (zs < 0) { printf("SKIP: kein RE2-Zombie in ROOM1140\n"); return 77; }
        re15_actor_t *z = &g_actors[zs];
        {   int ang = (seed * 4096) / seeds;
            pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
            pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
            pl->y = z->y;
            pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);
        }
        re15_enemy_bank_t *bank = re15_enemy_find(z->type);

        int entered = 0, dead = 0;
        int32_t entry_x = 0, entry_z = 0;
        for (int f = 0; f < 900; f++) {
            pl->floor = z->floor;
            if (!dead && re15_player_victim_state() != 2 && pl->hp > 0) pl->hp = 0;
            int was_six = (z->sub_state_1 == 6 && z->sub_state_2 != 0);
            frame_step();
            if (re15_player_victim_state() == 2) dead = 1;
            if (z->sub_state_1 != 6 || z->sub_state_2 == 0) continue;

            int16_t sx, sy, szo;
            int slot = (int)(z->anim_frame % 123u);
            if (!clip_root_off(bank, 0x18, (int)z->anim_frame, &sx, &sy, &szo)) continue;
            int32_t cs = re15_cos_q12(z->rot_y), sn = re15_sin_q12(z->rot_y);
            int32_t ox = (int32_t)(( (int64_t)cs * sx + (int64_t)sn * szo) >> 12);
            int32_t oz = (int32_t)((-(int64_t)sn * sx + (int64_t)cs * szo) >> 12);

            if (!entered) {                       /* EINTRITTS-TICK: P0 lief gerade */
                entered = 1; devours++;
                entry_x = z->x; entry_z = z->z;
                /* (A) Anker-Gleichung @0x80015C40-78 (Frame 0, weil das Clip-Wort +0x14D
                 *     gerade auf 0 gesetzt hat @0x801039C8) */
                int16_t ax, ay, az;
                clip_root_off(bank, 0x18, 0, &ax, &ay, &az);
                int32_t e_ox = (int32_t)(( (int64_t)cs * ax + (int64_t)sn * az) >> 12);
                int32_t e_oz = (int32_t)((-(int64_t)sn * ax + (int64_t)cs * az) >> 12);
                CHECK(z->anchor_x == z->x - e_ox && z->anchor_z == z->z - e_oz,
                      "(A) seed %d Anker == pos - RotY*off(Clip 0x18, F0) : "
                      "Anker(%d,%d) pos(%d,%d) off(%d,%d) [@0x801039D0 / @0x80015C40-78]",
                      seed, (int)z->anchor_x, (int)z->anchor_z, (int)z->x, (int)z->z,
                      (int)e_ox, (int)e_oz);
                /* (B) Kopie in den Spieler @0x80015C7C/C88/C94 */
                CHECK(pl->anchor_x == z->anchor_x && pl->anchor_z == z->anchor_z,
                      "(B) seed %d Spieler-Anker == Zombie-Anker : PL(%d,%d) Z(%d,%d) "
                      "[a0 = 0x800CFBF8 @0x801039B4-B8, Kopie @0x80015C7C/C88/C94]",
                      seed, (int)pl->anchor_x, (int)pl->anchor_z,
                      (int)z->anchor_x, (int)z->anchor_z);
            }
            (void)was_six; (void)slot;

            /* (C) Platzierung @0x80103A14 / @0x80015D58-90 */
            place_ticks++;
            if (z->x != z->anchor_x + ox || z->z != z->anchor_z + oz) {
                if (place_bad < 3)
                    printf("       (C) Abweichung seed %d f%d: pos(%d,%d) != Anker(%d,%d)+off(%d,%d)\n",
                           seed, f, (int)z->x, (int)z->z,
                           (int)z->anchor_x, (int)z->anchor_z, (int)ox, (int)oz);
                place_bad++;
            }
            /* (D) zurueckgelegte Strecke seit dem Eintritt */
            {   double dx = (double)z->x - entry_x, dz = (double)z->z - entry_z;
                double d = sqrt(dx * dx + dz * dz);
                if (d > max_travel) max_travel = d;
            }
            /* (E) an WELCHEM Leon-Knochen haengt der Zombiekopf? */
            {   re15_skel_pose_t lp[RE15_EMD_MAX_BONES], zp[RE15_EMD_MAX_BONES];
                int ln = 0, zn = 0;
                if (leon_bones(pl, z->type, lp, &ln) && zombie_bones(z, zp, &zn) && zn > 8) {
                    int32_t zh[3];
                    re15_skel_bone_to_world(zp[8].trans, z->rot_y, z->x, z->y, z->z, zh);
                    int best = -1; double bd = 1e18;
                    for (int bi = 0; bi < ln; bi++) {
                        int32_t w[3];
                        re15_skel_bone_to_world(lp[bi].trans, pl->rot_y, pl->x, pl->y, pl->z, w);
                        double d = dist3(zh, w);
                        if (d < bd) { bd = d; best = bi; }
                    }
                    if (best >= 1 && best <= 7) leg_ticks++;    /* PL00 b1..b7 = Beinketten */
                    else if (best >= 0)         upper_ticks++;  /* b0 Huefte, b8..b14 Rumpf/Arme */
                }
            }
        }
    }

    CHECK(devours == seeds, "Fress-Kollapse erreicht: %d von %d Seeds", devours, seeds);
    CHECK(place_ticks > 100 && place_bad == 0,
          "(C) Platzierung in JEDEM P1-Tick byte-genau: %d Ticks, %d Abweichungen "
          "[jal 0x80015cb8 @0x80103A14]", place_ticks, place_bad);
    CHECK(max_travel > 1000.0,
          "(D) Zombie folgt der autorisierten Wurzelbahn von Clip 0x18: max. Weg %.0f "
          "(Bank EM010 Paar 2 Clip 0x18: sx -514 @F0 .. +1155 @F122, 123 Frames; "
          "OHNE Platzierung ist der Weg 0)", max_travel);
    {   int tot = leg_ticks + upper_ticks;
        double up = tot ? (100.0 * upper_ticks / tot) : 0.0;
        CHECK(tot > 100 && up >= 80.0,
              "(E) Zombiekopf am OBERKOERPER/ARM statt am BEIN: %d/%d Ticks = %.1f %% "
              "(vor dem Fix: 9 %% — 91 %% an b2/b3/b5/b6 = Oberschenkel/Knie)",
              upper_ticks, tot, up);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(s_fail ? "\nFAIL: %d Pruefungen rot\n" : "\nOK: alle Pruefungen gruen\n", s_fail);
    return s_fail ? 1 : 0;
}
