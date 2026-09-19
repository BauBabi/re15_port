/* probe_p2_birkin_g5.c — PIN (ctest), Phase 2 des Dossiers analysis/befunde_2026-09-19/birkin-g5.md
 * (Fix-Plan 4.1.5 / 4.2.5 / 4.3.7, Korrekturen aus birkin-g5.skeptiker.md).
 *
 * Derselbe ECHTE Ablauf wie probe_r16_birkin_g5.c (ROOM5090.RDT -> SCD-VM -> re15_game_step ->
 * re15_enemy_ai_run_all, RE2-Baenke EM036/EM037, Spieler an Tuer 4, Lauf nach Westen bis Cut 12).
 *
 *  A) AUFTRITT: Kampfstart setzt den Boss auf (-9000, -23400) (RE2 [T0] @0x801011d0/d8); die
 *     Massenfront (Ursprung + 4494) liegt ab Bild 0 im Cut-12-Viereck, der Ursprung ab Bild <= 160
 *     (gemessen 151); Intro-Ende (erster Clip 5) mit Ursprung 1960 +-60 (RE2: -9000 + 7014 + 3946 =
 *     1960); die Front ueberholt den Spieler im Intro NIE.
 *  B) GESICHT: 2-Bone-Skinning von Mesh 0 — Kopf-Vertices (Gewicht 4096) liegen exakt auf
 *     R1*(v+off_a)+t1, Rumpf-Vertices (Gewicht 0) auf R0*v+t0 (FUN_800197f4); Augen-Wanderer
 *     bleibt in [-15,15] und bewegt sich in >= 1 von 16 Bildern; Kopf-Tracking nach dem Intro an
 *     (+0x1C0 = 0 @0x801017d4), |Akku| <= 212 (@0x8010045c), Delta != 0 in mindestens einem Bild.
 *  C) TENTAKEL: nach dem Intro haengen alle vier Anker innerhalb 6000 (X) am Boss-Ursprung; mit
 *     dem Spieler 6500 oestlich (3000 Bilder, HP je Bild zurueck) >= 10 Treffer (>= 1 je 300),
 *     Peitsche UND Spiess werden erreicht, jede Spitze kommt dem Spieler < TIP_MAX nahe.
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
#include "re15_anim_select.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re2_ems.h"
#include "re15_g5_skin.h"
#include "gen/g5_skin_tables.inc"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern uint8_t re15_g5_tentakel_maske(void);
extern int32_t re15_g5_tentakel_scale_x(int slot);
extern int  re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x,
                                     uint16_t *kontakt);
extern int16_t re15_g5_head_delta(void);
extern int     re15_g5_track_akku(void);
extern unsigned re15_g5_track_flags(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("  PIN FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("  PIN ok: " __VA_ARGS__); printf("\n"); } } while (0)

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
static int32_t dist2d(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int64_t dx = (int64_t)ax - bx, dz = (int64_t)az - bz;
    int64_t d2 = dx * dx + dz * dz; int32_t d = 0;
    while ((int64_t)d * d < d2) d += 25;
    return d;
}

/* R*v>>12 + t, s16-Saettigung wie im Skinner (GTE IR). */
static void xf(const re15_skel_pose_t *p, int32_t x, int32_t y, int32_t z, int32_t out[3])
{
    int32_t r[3];
    r[0] = (int32_t)(((int64_t)p->rot[0]*x + (int64_t)p->rot[1]*y + (int64_t)p->rot[2]*z) >> 12);
    r[1] = (int32_t)(((int64_t)p->rot[3]*x + (int64_t)p->rot[4]*y + (int64_t)p->rot[5]*z) >> 12);
    r[2] = (int32_t)(((int64_t)p->rot[6]*x + (int64_t)p->rot[7]*y + (int64_t)p->rot[8]*z) >> 12);
    for (int i = 0; i < 3; i++) {
        if (r[i] > 32767) r[i] = 32767; if (r[i] < -32768) r[i] = -32768;
        out[i] = (int16_t)(r[i] + (int16_t)p->trans[i]);
    }
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    (void)argc; (void)argv;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== P2 birkin-g5 PIN: Auftritt / Gesicht / Tentakel (ROOM5090, RE2-EM036/EM037) ===\n");
    int16_t qx[4], qz[4];
    int has_q = re15_rdt_get_region_quad(&s_rdt, 12, qx, qz);
    if (!has_q) { printf("FEHLT: Cut-12-Viereck\n"); return 77; }

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
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }
    re15_enemy_bank_t *eb36 = re15_enemy_find(0x36);

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
    if (bslot < 0) { printf("FEHLT: kein Boss\n"); return 77; }
    re15_actor_t *e = &g_actors[bslot];
    printf("KAMPF-GATE: Kamera 12 bei Bild %d, Spieler x=%d, Boss pos=(%d,%d) clip=%d\n",
           trigger_f, (int)pl->x, (int)e->x, (int)e->z, (int)e->motion);
    int32_t pl_x_gate = pl->x;

    /* ---------------- A) AUFTRITT ------------------------------------------------------- */
    printf("-- A) Auftritt --\n");
    PIN(e->x == -9000 && e->z == -23400, "Kampfstart-Position (-9000,-23400) @0x801011d0/d8: ist (%d,%d)", (int)e->x, (int)e->z);
    int first_in_q = -1, first_front_in_q = -1, first_clip5 = -1, first_front_past = -1;
    int32_t x_end = 0, dx_end = 0, dx_min = 0x7fffffff;
    int track_on_seen = 0; int delta_nonzero = 0; int akku_max = 0;
    for (int f = 0; f < 1500; f++) {
        frame();
        int32_t fx, fz; g5_front(e, &fx, &fz);
        int in_o = re15_aot_point_in_quad(e->x, e->z, qx, qz);
        int in_f = re15_aot_point_in_quad(fx, fz, qx, qz);
        int32_t dx = pl->x - e->x; if (dx < 0) dx = -dx;
        if (dx < dx_min) dx_min = dx;
        if (in_o && first_in_q < 0) first_in_q = f;
        if (in_f && first_front_in_q < 0) first_front_in_q = f;
        if (e->motion == 5 && first_clip5 < 0) { first_clip5 = f; x_end = e->x; dx_end = pl->x - e->x; }
        /* NUR im Intro (bis Clip 5): danach ist das Ueberholen byte-true — RE2 kappt den Zug
         * bei absolut X=12000 (@0x80100fd0), die Massenfront liegt damit bei 16494 und die
         * Hitbox A r=6000 (@0x8010053c) schiebt den Spieler vor der Masse her. */
        if (first_clip5 < 0 && e->x < pl->x && fx >= pl->x && first_front_past < 0)
            first_front_past = f;
        if (first_clip5 >= 0) {
            /* NACH dem Intro den Spieler auf 6500 oestlich halten (wie Teil C): naeher als
             * 6000 (`sltiu 0x1770` @0x80103e08 / @0x801008ac) UND im 128er-Bogen
             * (@0x80103e34-3c) frisst die Masse ihn byte-true auf - sub4 haelt dann fuer
             * immer ("kein Routine-Exit, der Spieler ist tot" @0x80101ccc-d24) und die
             * Teile B/C haetten keinen laufenden Kampf mehr zu messen. */
            pl->x = e->x + 6500; pl->z = e->z; pl->y = 0;
            pl->hp = 100; pl->hit_react = 0;
            if (re15_g5_track_flags() == 0) track_on_seen = 1;
            if (re15_g5_head_delta() != 0) delta_nonzero++;
            { int a = re15_g5_track_akku(); if (a > 2048) a -= 4096; if (a < 0) a = -a; if (a > akku_max) akku_max = a; }
        }
    }
    printf("  Ursprung im Viereck ab %d, Front ab %d, Clip 5 bei %d (Ursprung %d, Spieler %d -> Abstand %d, Spieler am Gate %d), Front ueberholt bei %d, min|dx|=%d\n",
           first_in_q, first_front_in_q, first_clip5, (int)x_end, (int)pl->x, (int)dx_end, (int)pl_x_gate, first_front_past, (int)dx_min);
    PIN(first_front_in_q == 0, "Massenfront ab Bild 0 im Cut-12-Viereck (ist ab %d)", first_front_in_q);
    PIN(first_in_q >= 0 && first_in_q <= 160, "Ursprung bis Bild 160 im Viereck (gemessen 151; ist %d)", first_in_q);
    PIN(first_clip5 > 0 && x_end >= 1900 && x_end <= 2020, "Intro-Ende Ursprung 1960 +-60 = RE2 -9000+7014+3946 (ist %d bei Bild %d)", (int)x_end, first_clip5);
    PIN(first_front_past < 0, "Front ueberholt den Spieler IM INTRO nie (ist Bild %d)", first_front_past);

    /* ---------------- B) GESICHT -------------------------------------------------------- */
    printf("-- B) Gesicht/Koerper --\n");
    PIN(track_on_seen, "Kopf-Tracking nach dem Intro EIN (+0x1C0 = 0 @0x801017d4)");
    PIN(delta_nonzero > 0, "Kopf-Tracking liefert ein Delta != 0 (in %d Bildern nach dem Intro)", delta_nonzero);
    printf("  Kopf-Tracking: Delta != 0 in %d Bildern, groesster |Akku| = %d\n", delta_nonzero, akku_max);
    {   /* KLEMME (FUN_80017FDC @0x80018118-160, selbst disassembliert): geklemmt wird NICHT
         * der Akku (+0x98), sondern das ZIEL — `a1 = desired - yaw - root`, und wenn
         * `((a1 + limit) & 0xfff) > limit*2` (`slt v0,limit*2,v1` @0x80018148), dann
         * `desired = (yaw+root) -+ limit` (@0x80018154-60). Der Akku laeuft danach auf
         * `desired - (yaw + root + head_kf)` (@0x800181b8-cc) — er traegt also den
         * KEYFRAME-Winkel des Kopfes mit und kann selbst weit groesser als 212 sein.
         * Gepinnt wird deshalb die geklemmte BLICKRICHTUNG: head_kf + delta in [-212,212].
         * ⛔ HIER STAND `|Akku| <= 212` — ein Pin auf ein falsches Modell (gemessen 4161). */
        re15_g5_track_t t; int16_t dp = 0, dn = 0, dk = 0; int i;
        re15_g5_track_init(&t);
        for (i = 0; i < 400; i++)                      /* Ziel bei -Z (rechts): +limit */
            dp = re15_g5_track_tick(&t, 0, 0, 0, 0, 0, 0, 0, -10000);
        re15_g5_track_init(&t);
        for (i = 0; i < 400; i++)                      /* Ziel bei +Z (links): -limit */
            dn = re15_g5_track_tick(&t, 0, 0, 0, 0, 0, 0, 0, 10000);
        re15_g5_track_init(&t);
        for (i = 0; i < 400; i++)                      /* dasselbe Ziel, Kopf-Keyframe 500 */
            dk = re15_g5_track_tick(&t, 0, 0, 0, 500, 0, 0, 0, 10000);
        printf("  Klemme: Ziel -Z -> %d, Ziel +Z -> %d, Ziel +Z mit head_kf=500 -> %d (Blick %d)\n",
               (int)dp, (int)dn, (int)dk, (int)(500 + dk));
        /* ASYMMETRIE IST BYTE-TRUE: der Akku steht als u16 im Part (+0x98, `sh` @0x800181cc),
         * der Aufrufer wickelt ihn mit `addiu v0,v1,-4095` (@0x80100330), NICHT mit -4096 —
         * aus -212 (0xFF2C, &0xfff = 3884) wird deshalb 3884-4095 = -211. Dieselbe Klasse wie
         * SquareRoot0/catan: die Naeherung des Originals wird nachgebaut, nicht geglaettet. */
        PIN(dp == 212, "Ziel-Klemme +212 um (Yaw+Root) (limit @0x8010045c, @0x80018118-160): ist %d", (int)dp);
        PIN(dn == -211, "Gegenrichtung -211 (u16-Akku + `addiu -4095` @0x80100330): ist %d", (int)dn);
        PIN(500 + dk == dn, "Klemme gilt der BLICKRICHTUNG head_kf+Delta, nicht dem Akku"
                            " (@0x800181b8-cc): 500%+d = %d", (int)dk, (int)(500 + dk));
    }
    {   /* Skinning-Geometrie an der aktuellen Pose (Query-Modus mit Haken). */
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        int kf = re15_compute_actor_kf(&eb36->anim, &eb36->skel, e, -1, e->anim_frame);
        void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL; g_anim_query_actor = e;
        int rv = re15_skel_compute_pose(&eb36->skel, kf, poses);
        g_anim_query_actor = NULL; g_anim_pose_actor = sav;
        const re15_md1_vertex_t *sv = NULL, *sn = NULL;
        int rs = (rv == 0) ? re15_g5_skin_mesh0(poses, eb36->skel.bone_count, &eb36->md1.meshes[0], &sv, &sn) : -1;
        int bad_head = 0, bad_body = 0, n_head = 0, n_body = 0, moved = 0;
        if (rs == 0) {
            const re15_g5_skin_tab_t *T = &s_g5_skin_tabs[0];
            for (int i = 0; i < T->count; i++) {
                int v = T->pairs[i][0], w = T->pairs[i][1];
                const re15_md1_vertex_t *b = &eb36->md1.meshes[0].tri_vertices[v];
                int32_t exp[3];
                if (w == 4096) { n_head++; xf(&poses[1], b->x + T->off_a[0], b->y + T->off_a[1], b->z + T->off_a[2], exp);
                                 if (sv[v].x != exp[0] || sv[v].y != exp[1] || sv[v].z != exp[2]) bad_head++; }
                else if (w == 0) { n_body++; xf(&poses[0], b->x, b->y, b->z, exp);
                                   if (sv[v].x != exp[0] || sv[v].y != exp[1] || sv[v].z != exp[2]) bad_body++; }
                if (sv[v].x != b->x || sv[v].y != b->y || sv[v].z != b->z) moved++;
            }
        }
        printf("  Skinning: rv=%d Kopf %d/%d falsch, Rumpf %d/%d falsch, %d/186 Vertices bewegt; Bone1 trans=(%d,%d,%d)\n",
               rs, bad_head, n_head, bad_body, n_body, moved, (int)poses[1].trans[0], (int)poses[1].trans[1], (int)poses[1].trans[2]);
        PIN(rs == 0 && n_head == 30 && bad_head == 0, "Kopf-Vertices (w=4096, 30) exakt an der Bone-1-Pose (R1*(v+off_a)+t1)");
        PIN(rs == 0 && n_body == 122 && bad_body == 0, "Rumpf-Vertices (w=0, 122) exakt an der Bone-0-Pose (R0*v+t0)");
        PIN(moved > 0, "geskinntes Mesh 0 weicht von der Bind-Pose ab (%d Vertices)", moved);
    }
    {   /* Augen-Wanderer ueber 160 Bilder. */
        int in_range = 1, changes = 0, pv = 0, pu = 0, v0, u0;
        re15_g5_eye_get(0, &pv, &pu);
        for (int f = 0; f < 160; f++) {
            pl->x = e->x + 6500; pl->z = e->z; pl->y = 0;
            pl->hp = 100; pl->hit_react = 0;
            frame();
            for (int i = 0; i < 2; i++) {
                int v, u; re15_g5_eye_get(i, &v, &u);
                if (v < -15 || v > 15 || u < -15 || u > 15) in_range = 0;
            }
            re15_g5_eye_get(0, &v0, &u0);
            if (v0 != pv || u0 != pu) changes++;
            pv = v0; pu = u0;
        }
        printf("  Augen: im Bereich=%d, Auge 0 aenderte sich in %d/160 Bildern, Stand (%d,%d)\n", in_range, changes, pv, pu);
        PIN(in_range, "Augen-UV-Wanderer bleibt in [-15,15] (Klemmen @0x80105624)");
        PIN(changes >= 10, "Augen-UV bewegt sich in >= 1 von 16 Bildern (ist %d/160)", changes);
    }

    /* ---------------- C) TENTAKEL ------------------------------------------------------- */
    printf("-- C) Tentakel --\n");
    int tslots[8]; int nt = tent_slots(tslots);
    PIN(nt == 4, "vier 0x37-Arme gespawnt (ist %d)", nt);
    {   int far = 0;
        for (int i = 0; i < nt; i++) {
            int32_t d = g_actors[tslots[i]].x - e->x; if (d < 0) d = -d;
            printf("  Arm %d: Anker-x - Boss-x = %d, y=%d\n", i, (int)(g_actors[tslots[i]].x - e->x), (int)g_actors[tslots[i]].y);
            if (d > 6000) far++;
        }
        PIN(far == 0, "alle vier Anker innerhalb 6000 vom Boss-Ursprung (nach dem Intro; %d weit weg)", far);
    }
    int hits = 0, peitsche = 0, spiess = 0, kontakt_frames = 0;
    int32_t tipd_min[8]; for (int i = 0; i < 8; i++) tipd_min[i] = 0x7fffffff;
    int32_t scale_max[8] = {0};
    int boss_clip_hist[16]; memset(boss_clip_hist, 0, sizeof boss_clip_hist);
    for (int f = 0; f < 3000; f++) {
        pl->x = e->x + 6500; pl->z = e->z; pl->y = 0;
        pl->hp = 100; pl->hit_react = 0;
        frame();
        if (e->motion >= 0 && e->motion < 16) boss_clip_hist[e->motion]++;
        if (pl->hp < 100) hits++;
        for (int i = 0; i < nt; i++) {
            re15_actor_t *t = &g_actors[tslots[i]];
            int c = (int)t->motion;
            if (c == 7 || c == 8) peitsche++;
            if (c == 21 || c == 23) spiess++;
            { int32_t sx = re15_g5_tentakel_scale_x(tslots[i]); if (sx > scale_max[i]) scale_max[i] = sx; }
            { uint16_t k = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k); if (k & 6u) kontakt_frames++; }
            int32_t sp[3];
            if (re15_g5_tentakel_spitze(i, sp) == 0) {
                int32_t d = dist2d(pl->x, pl->z, sp[0], sp[2]);
                if (d < tipd_min[i]) tipd_min[i] = d;
            }
        }
    }
    printf("  3000 Bilder: Treffer=%d, Peitschen-Bilder=%d, Spiess-Bilder=%d, Kontakt-Bilder=%d; Boss-Clips:",
           hits, peitsche, spiess, kontakt_frames);
    for (int c = 0; c < 12; c++) printf(" c%d=%d", c, boss_clip_hist[c]);
    printf("\n");
    for (int i = 0; i < nt; i++)
        printf("  Arm %d: scale_max=%d, Spitze min-Abstand Spieler=%d\n", i, (int)scale_max[i], (int)tipd_min[i]);
    PIN(hits >= 10, "mit dem Spieler 6500 oestlich >= 1 Treffer je 300 Bilder (ist %d in 3000)", hits);
    PIN(peitsche > 0, "Peitsche (sub3) im Nahbereich erreicht (%d Bilder)", peitsche);
    PIN(spiess == 0, "Spiess (0xD01) im Nahbereich NIE: dist < 7000 setzt +0x16A = 0 (`sltiu 0x1b58`"
                     " @0x80100a78/@0x80100b48) -> Musterzeile 0 @0x80105674 = {0x301,0x301} (ist %d)", spiess);
    {   /* Zweiter Block mit dist >= 7000: dort bleibt der Wurf stehen (@0x80100a78 springt
         * ueber `sb zero,362`), die Musterzeile ist 0..7 und der Spiess 0xD01 kommt vor
         * (Zeilen 1/3/4/5/6/7 @0x80105674). */
        int spiess2 = 0, peitsche2 = 0, hits2 = 0;
        for (int f = 0; f < 2000; f++) {
            pl->x = e->x + 9000; pl->z = e->z; pl->y = 0;
            pl->hp = 100; pl->hit_react = 0;
            frame();
            if (pl->hp < 100) hits2++;
            for (int i = 0; i < nt; i++) {
                int c = (int)g_actors[tslots[i]].motion;
                if (c == 7 || c == 8) peitsche2++;
                if (c == 21 || c == 23) spiess2++;
            }
        }
        printf("  Spieler 9000 oestlich, 2000 Bilder: Peitschen-Bilder=%d, Spiess-Bilder=%d, Treffer=%d\n",
               peitsche2, spiess2, hits2);
        PIN(spiess2 > 0, "Spiess (sub13) bei dist >= 7000 erreicht (%d Bilder)", spiess2);
    }
    {   int near = 0;
        for (int i = 0; i < nt; i++) if (tipd_min[i] < 3000) near++;
        PIN(near == nt, "jede Spitze kommt dem Spieler < 3000 nahe (%d/%d)", near, nt);
    }
    PIN(kontakt_frames > 0, "Kind-Kollision meldet Kontakt (+0x220 & 6) in %d Arm-Bildern", kontakt_frames);

    printf("=== %s (%d Pins verletzt) ===\n", s_fail ? "FEHLER" : "OK", s_fail);
    return s_fail ? 1 : 0;
}
