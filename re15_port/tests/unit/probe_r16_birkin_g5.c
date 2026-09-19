/* probe_r16_birkin_g5.c — MESSUNG (kein Pin), Runde 16, Thema birkin-g5.
 *
 * Nutzer: "1. Muss er eigentlich hinten durch das Zug Rechteck rein kommen, genauso wie in
 * RE2. 2. Sind weder die Augen, noch die Zaehne noch sonst irgendwas von ihm animiert.
 * 3. Sind die Tentakel nicht richtig animiert, greifen nicht an usw."
 *
 * Drei Messungen auf dem ECHTEN Ablauf (ROOM5090.RDT -> SCD-VM -> re15_game_step ->
 * re15_enemy_ai_run_all, Muster probe_5090_birkin.c):
 *
 *  A) AUFTRITT: wo steht der Boss (Ursprung + Massen-FRONT) je Bild ab Kampfstart, ab wann
 *     liegt er im Anker-Viereck von Cut 12 (RVD @0x04A4, from=12 to=0: x -5600..28100 —
 *     der Port zeichnet einen Gegner NUR, wenn sein URSPRUNG im Viereck liegt,
 *     main.c:7839 re15_aot_point_in_quad), wie weit ist die Front vom Westende (SCA-Zelle
 *     [23] x=-5673) und vom Spieler, wann ist das Intro fertig (erster Clip 5 = sub1 ZUG).
 *     Dazu die reine Geometrie fuer den Skriptwert Pos_set(1200) @0x12FE und fuer einen
 *     Start "Massenfront am Westende" (x = -5673 - 4494; 4494 = Blob-Bind 1800 @0x80100680 +
 *     Mesh-2-Bbox x_max 2694, g5-morph.md 2.3).
 *  B) ANIMATION: (1) bewegt sich Bone 1 (Kopf) im Clip-Daten-Pfad des Ports — Pose-Deltas
 *     aus re15_skel_compute_pose je Bild; (2) liefert das Modul einen Morph-Puffer und wie
 *     gross ist die Vertex-Verschiebung je Bild (Blob-Maschine); (3) welche der RE2-
 *     Mechanismen (Kopf-Tracking, Augen-UV, 2-Bone-Skinning) haben ueberhaupt einen
 *     Konsumenten im Port — das ist Code-Befund, hier nur als Ausgabe der vorhandenen
 *     Felder (rot_x/rot_z der Arme, render_scale).
 *  C) TENTAKEL: 3000 Bilder mit dem Spieler in Reichweite (jedes Bild auf |dx|=6500 zum
 *     Boss gepinnt: < 7000 = Schlagkommandos @0x80100b4c, >= 6000 = kein Biss/Devour
 *     @0x80103e08/@0x801008ac). Gezaehlt je Arm: Clip-Histogramm (7/8 Peitsche sub3,
 *     21/23 Spiess sub13, 14-16 Austritt sub8, 9-11 Wedeln, 17-20 Tod, 0 Ruhe/Zug),
 *     Streckung, rot_x/rot_z, HP-Treffer am Spieler (HP je Bild zurueckgesetzt).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern const re15_md1_vertex_t *re15_g5_morph_verts(uint8_t type, int mesh_index);
extern int  re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern uint8_t re15_g5_tentakel_maske(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_re2_banks(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    for (int k = 0; k < 2; k++) {
        uint8_t type = k ? 0x37u : 0x36u;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) return 0;
        if (re2_ems_load_bank(ems, n, type, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

/* Spieler-Modus-/Letterbox-Zweig des ECHTEN Hauptlaufs (main.c:4340-4360) — wie in
 * probe_5090_birkin.c, sonst haengt sub04 vor Cut_chg(12). */
static int s_cine_was_active = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* Massen-FRONT: Ursprung + 4494 entlang der Blickrichtung (Renderer-Konvention
 * main.c: nyaw = {c,0,s / 0,1,0 / -s,0,c}, lokal +X = vorn). */
#define G5_FRONT 4494
static void g5_front(const re15_actor_t *e, int32_t *fx, int32_t *fz)
{
    int32_t c = re15_cos_q12((int)e->rot_y), s = re15_sin_q12((int)e->rot_y);
    *fx = e->x + (int32_t)(((int64_t)c * G5_FRONT) >> 12);
    *fz = e->z - (int32_t)(((int64_t)s * G5_FRONT) >> 12);
}

static int find_boss(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) return s;
    return -1;
}
static int tent_slots(int *out)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x37u && n < 8) out[n++] = s;
    return n;
}

/* Keyframe-Index des laufenden Clips (wie g5_root_motion im Modul). */
static int actor_kf(const re15_actor_t *e, const re15_enemy_bank_t *b)
{
    const re15_emd_animation_t *an = &b->anim;
    int clip = (int)e->motion;
    if (clip < 0 || clip >= an->clip_count) return -1;
    int fc = an->clips[clip].frame_count; if (fc <= 0) return -1;
    int fi = an->clips[clip].first_frame + (int)(e->anim_frame % (uint32_t)fc);
    return (int)(an->frames[fi] & 0xFFFu);
}

static int32_t morph_max_delta(const re15_enemy_bank_t *b, int *vgroesser_500)
{
    const re15_md1_vertex_t *mv = re15_g5_morph_verts(0x36u, 2);
    const re15_md1_mesh_t *m = &b->md1.meshes[2];
    int32_t mx = 0; int big = 0;
    if (!mv) return -1;
    for (int v = 0; v < m->tri_vertex_count; v++) {
        int32_t dx = mv[v].x - m->tri_vertices[v].x;
        int32_t dy = mv[v].y - m->tri_vertices[v].y;
        int32_t dz = mv[v].z - m->tri_vertices[v].z;
        if (dx < 0) dx = -dx; if (dy < 0) dy = -dy; if (dz < 0) dz = -dz;
        if (dx > mx) mx = dx; if (dy > mx) mx = dy; if (dz > mx) mx = dz;
        if (dx > 500 || dy > 500 || dz > 500) big++;
    }
    if (vgroesser_500) *vgroesser_500 = big;
    return mx;
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    (void)argc; (void)argv;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== R16 birkin-g5: Auftritt / Animation / Tentakel (ROOM5090, RE2-EM036/EM037) ===\n");

    int16_t qx[4], qz[4];
    int has_q = re15_rdt_get_region_quad(&s_rdt, 12, qx, qz);
    printf("Cut-12-Ankerviereck (RVD @0x04A4): has=%d (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
           has_q, qx[0], qz[0], qx[1], qz[1], qx[2], qz[2], qx[3], qz[3]);

    /* --- reine Geometrie der zwei Startpositionen --- */
    {
        const int32_t wand_w = -5673;                    /* SCA-Zelle [23] x, g5-optik.md 5 */
        const int32_t starts[2] = { 1200, wand_w - G5_FRONT };
        const char *namen[2] = { "Skript Pos_set(1200) @0x12FE", "Front am Westende (-5673-4494)" };
        for (int k = 0; k < 2; k++) {
            int32_t x0 = starts[k];
            int in_o = has_q ? re15_aot_point_in_quad(x0, -23400, qx, qz) : -1;
            int in_f = has_q ? re15_aot_point_in_quad(x0 + G5_FRONT, -23400, qx, qz) : -1;
            printf("  Start %-32s: Ursprung x=%6d (im Viereck=%d)  Front x=%6d (im Viereck=%d)  "
                   "Front-Abstand Westwand=%d  nach Intro (+10960) Ursprung=%d Front=%d\n",
                   namen[k], x0, in_o, x0 + G5_FRONT, in_f, x0 + G5_FRONT - wand_w,
                   x0 + 10960, x0 + 10960 + G5_FRONT);
        }
    }

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;   /* Tuer 4 @0x10CE */
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }
    re15_enemy_bank_t *eb36 = re15_enemy_find(0x36);
    re15_enemy_bank_t *eb37 = re15_enemy_find(0x37);
    printf("EM036: %d Bones %d Clips %d Meshes morph_ok=%d | EM037: %d Bones %d Clips %d Meshes\n",
           eb36->skel.bone_count, eb36->anim.clip_count, eb36->md1.mesh_count, eb36->morph_ok,
           eb37->skel.bone_count, eb37->anim.clip_count, eb37->md1.mesh_count);

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    for (int f = 0; f < 30; f++) frame();
    int trigger_f = -1;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        frame();
        if (trigger_f < 0 && s_shown == 12) trigger_f = f;
        if (trigger_f >= 0 && f > trigger_f + 3) break;
    }
    int bslot = find_boss();
    printf("KAMPF-GATE: Kamera 12 bei Bild %d, Spieler x=%d, Boss-Slot %d, Boss pos=(%d,%d) grid=%02X clip=%d\n",
           trigger_f, (int)pl->x, bslot, bslot >= 0 ? (int)g_actors[bslot].x : 0,
           bslot >= 0 ? (int)g_actors[bslot].z : 0,
           bslot >= 0 ? (unsigned)g_actors[bslot].grid_id : 0u,
           bslot >= 0 ? (int)g_actors[bslot].motion : -1);
    if (bslot < 0) { printf("FEHLT: kein Boss\n"); return 77; }
    re15_actor_t *e = &g_actors[bslot];

    /* ---------------- A) AUFTRITT + B) ANIMATION im selben Lauf (Spieler steht) --------- */
    printf("\n-- A/B) 1500 Bilder ab Kampfgate, Spieler steht bei x=%d --\n", (int)pl->x);
    printf("   Bild | boss x,z | yaw | clip/af | Urspr.imQ | Front x (imQ) | Front-Spieler | |dx| | morphMax(#>500) | bone1 rot[0],rot[2] trans | tent scale/rotx/rotz\n");
    int first_in_q = -1, first_front_in_q = -1, first_clip5 = -1, first_front_past = -1;
    int32_t dx_min = 0x7fffffff;
    int32_t morph_max_run = 0; int morph_null = 0;
    int32_t prev_b1r0 = 0x7fffffff; int b1_changes = 0;
    int clip_hist[16]; memset(clip_hist, 0, sizeof clip_hist);
    int tslots[8]; int nt = 0;
    for (int f = 0; f < 1500; f++) {
        frame();
        if (nt == 0) nt = tent_slots(tslots);
        int32_t fx, fz; g5_front(e, &fx, &fz);
        int in_o = has_q ? re15_aot_point_in_quad(e->x, e->z, qx, qz) : -1;
        int in_f = has_q ? re15_aot_point_in_quad(fx, fz, qx, qz) : -1;
        int32_t dx = pl->x - e->x; if (dx < 0) dx = -dx;
        if (dx < dx_min) dx_min = dx;
        if (in_o && first_in_q < 0) first_in_q = f;
        if (in_f && first_front_in_q < 0) first_front_in_q = f;
        if (e->motion == 5 && first_clip5 < 0) first_clip5 = f;
        if (e->x < pl->x && fx >= pl->x && first_front_past < 0) first_front_past = f;
        if (e->motion >= 0 && e->motion < 16) clip_hist[e->motion]++;
        int big = 0; int32_t mm = morph_max_delta(eb36, &big);
        if (mm < 0) morph_null++; else if (mm > morph_max_run) morph_max_run = mm;
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        int kf = actor_kf(e, eb36);
        int32_t b1r0 = 0, b1r2 = 0, b1t[3] = {0,0,0};
        if (kf >= 0 && re15_skel_compute_pose(&eb36->skel, kf, poses) == 0) {
            b1r0 = poses[1].rot[0]; b1r2 = poses[1].rot[2];
            b1t[0] = poses[1].trans[0]; b1t[1] = poses[1].trans[1]; b1t[2] = poses[1].trans[2];
            if (prev_b1r0 != 0x7fffffff && b1r0 != prev_b1r0) b1_changes++;
            prev_b1r0 = b1r0;
        }
        if ((f % 30) == 0 || f == first_in_q || f == first_clip5 || f == first_front_past) {
            printf("   %4d | %6d,%6d | %4d | %2d/%3u | %d | %6d (%d) | %6d | %5d | %5d(%3d) | %5d,%5d (%d,%d,%d) |",
                   f, (int)e->x, (int)e->z, (int)e->rot_y, (int)e->motion, (unsigned)e->anim_frame,
                   in_o, (int)fx, in_f, (int)(fx - pl->x), (int)dx, (int)mm, big,
                   (int)b1r0, (int)b1r2, (int)b1t[0], (int)b1t[1], (int)b1t[2]);
            for (int i = 0; i < nt; i++) {
                re15_actor_t *t = &g_actors[tslots[i]];
                printf(" [%d c%d s%d rx%d rz%d]", i, (int)t->motion, (int)t->render_scale_q12,
                       (int)t->rot_x, (int)t->rot_z);
            }
            printf("\n");
        }
    }
    printf("  ERGEBNIS A: Ursprung erstmals im Cut-12-Viereck bei Bild %d, Front bei %d; Intro fertig (Clip 5) bei %d; "
           "Front ueberholt den Spieler bei Bild %d; kleinster |dx| Ursprung-Spieler = %d\n",
           first_in_q, first_front_in_q, first_clip5, first_front_past, (int)dx_min);
    printf("  ERGEBNIS B: Morph-Puffer NULL in %d/1500 Bildern, groesste Vertex-Verschiebung im Lauf = %d; "
           "Bone-1-Pose (rot[0]) aenderte sich in %d Bildern; Clip-Histogramm:", morph_null, (int)morph_max_run, b1_changes);
    for (int c = 0; c < 12; c++) printf(" c%d=%d", c, clip_hist[c]);
    printf("\n");
    printf("  Konsumenten im Port (Code-Befund, hier nur Felder): Boss rot_x=%d rot_z=%d render_scale=%d "
           "(Kopf-Tracking/Augen-UV/2-Bone-Naht: kein Feld, kein Konsument)\n",
           (int)e->rot_x, (int)e->rot_z, (int)e->render_scale_q12);

    /* ---------------- C) TENTAKEL: 3000 Bilder, Spieler in Reichweite --------------------- */
    printf("\n-- C) 3000 Bilder, Spieler jedes Bild auf |dx|=6500 OESTLICH des Bosses gepinnt, HP je Bild zurueck --\n");
    nt = tent_slots(tslots);
    printf("  Tentakel-Aktoren: %d\n", nt);
    int chist[8][32]; memset(chist, 0, sizeof chist);
    int hits = 0; int hit_clip[8][32]; memset(hit_clip, 0, sizeof hit_clip);
    int32_t scale_max[8] = {0}; int rotx_nonzero[8] = {0}, rotz_nonzero[8] = {0};
    int boss_clip_hist[16]; memset(boss_clip_hist, 0, sizeof boss_clip_hist);
    int32_t tipd_min[8]; for (int i = 0; i < 8; i++) tipd_min[i] = 0x7fffffff;
    int trace_n = 0;
    for (int f = 0; f < 3000; f++) {
        pl->x = e->x + 6500; pl->z = e->z; pl->y = 0;
        pl->hp = 100; pl->hit_react = 0;
        frame();
        if (e->motion >= 0 && e->motion < 16) boss_clip_hist[e->motion]++;
        int hit_now = (pl->hp < 100);
        if (hit_now) hits++;
        for (int i = 0; i < nt; i++) {
            re15_actor_t *t = &g_actors[tslots[i]];
            int c = (int)t->motion; if (c < 0 || c > 31) c = 31;
            chist[i][c]++;
            if (hit_now) hit_clip[i][c]++;
            if (t->render_scale_q12 > scale_max[i]) scale_max[i] = t->render_scale_q12;
            if (t->rot_x) rotx_nonzero[i]++;
            if (t->rot_z) rotz_nonzero[i]++;
            int32_t sp[3];
            if (re15_g5_tentakel_spitze(i, sp) == 0) {
                int64_t ddx = (int64_t)pl->x - sp[0], ddz = (int64_t)pl->z - sp[2];
                int64_t d2 = ddx*ddx + ddz*ddz; int32_t d = 0;
                while ((int64_t)d * d < d2) d += 50;
                if (d < tipd_min[i]) tipd_min[i] = d;
            }
        }
        {   /* Geometrie der Schlagfenster: Arm-Ursprung (Anker), Spitze, Boss, Spieler. */
            static int geo_n = 0;
            for (int i = 0; i < nt && geo_n < 16; i++) {
                re15_actor_t *t = &g_actors[tslots[i]];
                int c = (int)t->motion;
                int32_t sp[3];
                if ((c == 7 || c == 8 || c == 21 || c == 23) && t->render_scale_q12 >= 4000 &&
                    (t->anim_frame % 20u) == 10u && re15_g5_tentakel_spitze(i, sp) == 0) {
                    int32_t w0[3]; re15_enemy_bone_world_pos(e, 0, w0);
                    printf("   GEO f%4d arm%d c%2d af%3u yaw%4d s%4d anker=(%d,%d,%d) spitze=(%d,%d,%d) "
                           "bossUrspr=(%d,%d,%d) bossBone0welt=(%d,%d,%d) spieler=(%d,%d)\n",
                           f, i, c, (unsigned)t->anim_frame, (int)t->rot_y, (int)t->render_scale_q12,
                           (int)t->x, (int)t->y, (int)t->z, (int)sp[0], (int)sp[1], (int)sp[2],
                           (int)e->x, (int)e->y, (int)e->z, (int)w0[0], (int)w0[1], (int)w0[2],
                           (int)pl->x, (int)pl->z);
                    geo_n++;
                }
            }
        }
        if ((f % 100) == 0 && trace_n < 31) {
            trace_n++;
            printf("   f%4d boss clip=%2d x=%6d maske=%02X |", f, (int)e->motion, (int)e->x,
                   (unsigned)re15_g5_tentakel_maske());
            for (int i = 0; i < nt; i++) {
                re15_actor_t *t = &g_actors[tslots[i]];
                printf(" [%d c%2d af%3u s%4d rx%5d rz%5d y%6d]", i, (int)t->motion, (unsigned)t->anim_frame,
                       (int)t->render_scale_q12, (int)t->rot_x, (int)t->rot_z, (int)t->y);
            }
            printf("\n");
        }
    }
    printf("  Boss-Clips in C:"); for (int c = 0; c < 12; c++) printf(" c%d=%d", c, boss_clip_hist[c]); printf("\n");
    printf("  Spieler-HP-Treffer in 3000 Bildern: %d\n", hits);
    for (int i = 0; i < nt; i++) {
        printf("  Arm %d: Clips", i);
        for (int c = 0; c < 32; c++) if (chist[i][c]) printf(" c%d=%d", c, chist[i][c]);
        printf(" | scale_max=%d rot_x!=0 in %d, rot_z!=0 in %d Bildern | Spitze min-Abstand Spieler %d | Treffer bei Clip:",
               (int)scale_max[i], rotx_nonzero[i], rotz_nonzero[i], (int)tipd_min[i]);
        for (int c = 0; c < 32; c++) if (hit_clip[i][c]) printf(" c%d=%d", c, hit_clip[i][c]);
        printf("\n");
    }
    printf("  Zustaende erreicht: Peitsche(sub3: c7/c8)=%s  Spiess(sub13: c21/c23)=%s  Wedeln(sub10: c9-11)=%s  "
           "Zug(sub11: nur c0 -> nicht am Clip erkennbar)  Speer(sub9: nur c0/c2)\n",
           (chist[0][7]+chist[1][7]+chist[2][7]+chist[3][7]+chist[0][8]+chist[1][8]+chist[2][8]+chist[3][8]) ? "JA" : "NEIN",
           (chist[0][21]+chist[1][21]+chist[2][21]+chist[3][21]+chist[0][23]+chist[1][23]+chist[2][23]+chist[3][23]) ? "JA" : "NEIN",
           (chist[0][9]+chist[1][9]+chist[2][9]+chist[3][9]+chist[0][10]+chist[1][10]+chist[2][10]+chist[3][10]+chist[0][11]+chist[1][11]+chist[2][11]+chist[3][11]) ? "JA" : "NEIN");
    return 0;
}
