/**
 * @file test_r35_raeume_arme.c
 * @brief Runde 35 Spur H, Punkt 4 — ROOM1210 Gitterarme: Griff und Leons Schuetteln (Clipping).
 *
 * Nutzer: "Die Zombie Arme in ROOM 1210, wenn sie einen greifen bewegen sich nicht synchron zu Leon
 * beim schuetteln, dadurch clipped er."
 *
 * Echter Weg wie unit_1210_arme_re2 (ROOM1210.RDT, zehn Arme, RE2-EM2D-Bank, game_step), Messarm 5.
 * MASSSTAB ist das RE2-ORIGINAL: r35_raeume_re2orig.inc traegt die RAM von zwoelf Griffen ROOM2050
 * (DuckStation-GDB, Halte-Bilder 16..34, Part-Welt-Matrizen von Leons Rumpf/Kopf und Unterarm/Hand des
 * Halters; Dossier H_raeume.md, Nachbesserung 3). Gemessen wird:
 *   (1) PIN + KOERPER-PUSH: B4 P0 setzt Leon auf part[Hand]+0x5C/+0x64 der GEMISCHTEN Parts (@0x80100C18-38,
 *       +0x14E @0x800296a8-bc); im selben Bild schiebt der Spieler-Pass FUN_800355C4 (@0x80026628) ihn mit
 *       FUN_80034D0C aus dem Arm-Segment r 800 (@0x80100338-3C) + Spieler r 450 (@0x8003bdc0-c4). Erwartet:
 *       Leon auf dem Strahl Ursprung->Pin im Abstand, den das ORIGINAL in den zehn sauberen Griffen zeigt
 *       (1251..1255; g3/g4 ausgenommen — dort schob der sichtbare Nachbar Satz 0 mit, Abnahme 3 M3).
 *   (2) GLEICHLAUF: Leon zeigt jedes Halte-Bild Arm-Bild + 1 (im Original-RAM ebenso, Clipwort-Bytes).
 *   (6a) PORT = ORIGINAL: der Port-Griff in der Lage eines Original-Griffs (gleiche Armhoehe, Arm vor Leon,
 *       Gesicht und Ruecken) zeigt Bild fuer Bild dieselbe Hand im KOPF-Rahmen Leons, denselben Blick-Akku
 *       (Part 8 +0x98/+0x9A) und dasselbe Blickziel wie die Original-RAM und dieselbe Zahl Arm-Vertices in
 *       Leons Kopf/Rumpf. Leon wird dafuer posiert WIE DER ZEICHNER (main.c: g_anim_pose_actor = Spieler,
 *       Nacken-FSM in re15_skel_compute_pose, jedes Bild), nicht ohne Kopfdrehung (Nachbesserung 4).
 *   (N1) RE2-ZIELWAHL FUN_8003DB38 auf der ORIGINAL-Geometrie: Kegel +-1500 (re15_ai_arc_test = FUN_80015614),
 *       Radius 7000, naechster Kandidat, Sicht aus re2_los.py — liefert in den neun sauberen Griffen und in zwei
 *       Laeufen mit allen zehn Armen im RAM das Ziel, das die RAM zeigt (Halter oder SELBST).
 *   (6b) Der Nutzerfall (Arm 5 in ROOM1210, Port-Hoehe -2500): nicht mehr Ueberschneidung als das Original
 *       an seinen Nachbarhoehen (Satz 5 -2480 / Satz 8 -2540 / Satz 0 -2580).
 *   (4a-d) Konstruktion: Ruecken-Blick = Gesicht-Blick + 2048 bei gleicher Hand-Bahn (Port) — im Original-RAM
 *       bei gleichem Pin und gleicher Hand-Bahn bestaetigt (Paare g1/g2, g3/g4, g5/g6, g7/g8).
 *   (5a/5b) RE1.5-KI: Typ 0x1A gehoert auch dort dem RE2-EM2D-Gehirn; derselbe Griff.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static re15_emd_animation_t s_pa; static re15_emd_skeleton_t s_ps;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("ok:   %s\n", name); } while (0)

/* ---- RE2-Original-RAM (erzeugt, re2_mess/re2_fixture.py) ------------------------------------- */
typedef struct { int16_t m[9]; int32_t t[3]; } r2o_mat_t;
typedef struct {
    int32_t lx, lz; int16_t lyaw; uint32_t lcw;         /* Leon Pin / Blick / Clipwort +0x14C          */
    int32_t ax, ay, az; int16_t ayaw; uint32_t acw;     /* Arm Ursprung / Blick / Clipwort             */
    r2o_mat_t L0, L8, A1, A2;                           /* Leon Part 0/8, Arm Unterarm/Hand (Welt)     */
    int16_t nyaw, npit; int8_t nziel;                   /* NB4: Blick-Akku Part8 +0x98/+0x9A, Ziel-Satz PL+0x1B8 (-1 SELBST) */
} r2o_bild_t;
#define R2O_BILDER 19
/* (6a) Vergleichsgrenzen — TEST-TOLERANZEN der Versuchsanordnung, kein Spielwert:
 *  - Lage: Der Original-Leon steht in den Vergleichs-Griffen 14 Einheiten seitlich und mit Blick 2037 (statt
 *    2048) zum Arm (Fixture: Leon - Arm (14,-1251), Blick 3061 bei Arm 1024), der Port-Leon auf dem Pin-Strahl
 *    ohne Seitenversatz (Blick 2055). 14 seitlich + 18 Blick-Einheiten (1,6 Grad) bei ~650 Hand-Abstand ergeben
 *    bis ~20 Einheiten; gemessen max 10 (Dossier Nachbesserung 3).
 *  - Kopf-Rahmen / Blick-Akku / Zahl der Ueberschneidungsbilder: seit Nachbesserung 4 dreht der Port Leons Kopf im
 *    Halten wie RE2 (FUN_8003DB38 + FUN_800177C0, s. enemy_ai_re2_zellenarm.c); die Grenzen sind die gemessene
 *    Restabweichung derselben Versuchsanordnung (Dossier H_raeume.md "Nachbesserung 4"), kein Spielwert. */
#define R35_6A_TOL   20
#define R35_6A_KTOL  20
#define R35_6A_NTOL  16
#define R35_6A_ZTOL  1
typedef struct { const char *name; int satz, hoehe, griff, var, wechsel; r2o_bild_t b[R2O_BILDER]; } r2o_lauf_t;
/* NB4 (N1b): Blickziel-Suchen aus RE2-Laeufen mit allen zehn Armen (re2_gdb_grab.py R2_EXTRA=1, extra.txt). */
typedef struct { int satz; uint8_t aktiv; uint16_t f10e; int32_t x, z; uint8_t sicht; } r2n_arm_t;
typedef struct { const char *name; int bild; int32_t lx, lz; int16_t lyaw; int ziel; r2n_arm_t arm[10]; } r2n_suche_t;
#include "r35_raeume_re2orig.inc"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static int load_re2_bank_1A(void)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(s_ems, s_ems_n, 0x2D, eb, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}
static void se_cap(int id, int f) { (void)id; (void)f; }
static void bank_cap(int b) { (void)b; }
static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static int aufsetzen(int *slots)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (!load_re2_bank_1A()) return 0;
    re15_re2arm_audio_hook(se_cap, bank_cap);
    re15_re2arm_look_debug(45, RE15_ACTOR_SLOT_PLAYER, NULL, NULL);   /* Spielstart: DAT_800a4004 .data 0x2D, INIT SELBST */
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

#include "r35_raeume_volumen.h"   /* Huelle Leon Kopf/Rumpf (PL00.MD1), vol_innen / vol_innen_welt */
static re15_skel_pose_t s_leon_pose[2];
/* Leons Opfer-Pose WIE DER ZEICHNER (main.c Spieler-Zweig: PL00-Knochen, Keyframes + Clips der Greifer-Opferbank,
 * `g_anim_pose_actor = player_ref` -> Ueberblendung + Nacken-FSM in re15_skel_compute_pose). Jedes Halte-Bild
 * genau einmal aufrufen, wie der Zeichner — die FSM schreitet je Aufruf einen Schritt. */
static int leon_pose(re15_actor_t *pl)
{
    re15_enemy_bank_t *vb = re15_enemy_find(0x1A);
    if (!vb || !vb->victim_ok) return 0;
    re15_emd_skeleton_t vs = s_ps;
    vs.keyframe_data = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, pl, (int)pl->motion, pl->anim_frame);
    if (kf < 0) return 0;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = pl;
    int rv = re15_skel_compute_pose(&vs, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    s_leon_pose[0] = poses[0]; s_leon_pose[1] = poses[8];
    return 1;
}
/* Punkt P in Leons Kopf-Rahmen (Knochen 8): lokal = R_b^T (R_y^T (P - Wurzel) - t_b), wie vol_innen. */
static void port_kopf_rahmen(const re15_actor_t *pl, const int32_t P[3], int32_t out[3])
{
    const re15_skel_pose_t *p = &s_leon_pose[1];
    int64_t cs = re15_cos_q12(pl->rot_y), sn = re15_sin_q12(pl->rot_y);
    int64_t dx = P[0] - pl->x, dy = P[1] - pl->y, dz = P[2] - pl->z;
    int64_t mx = ((cs * dx - sn * dz) >> 12) - p->trans[0], my = dy - p->trans[1], mz = ((sn * dx + cs * dz) >> 12) - p->trans[2];
    out[0] = (int32_t)((p->rot[0] * mx + p->rot[3] * my + p->rot[6] * mz) >> 12);
    out[1] = (int32_t)((p->rot[1] * mx + p->rot[4] * my + p->rot[7] * mz) >> 12);
    out[2] = (int32_t)((p->rot[2] * mx + p->rot[5] * my + p->rot[8] * mz) >> 12);
}
/* Punkt im WURZEL-Rahmen Leons (Lage x/z, Boden y, Blick): R_y^T (P - Wurzel) — dieselbe Konvention wie
 * vol_innen. Unabhaengig von Leons Kopfdrehung; (6a) prueft zusaetzlich den KOPF-Rahmen. */
static void wurzel_rahmen(int32_t x, int32_t z, int yaw, const int32_t P[3], int32_t out[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int64_t dx = P[0] - x, dz = P[2] - z;
    out[0] = (int32_t)((cs * dx - sn * dz) >> 12); out[1] = P[1]; out[2] = (int32_t)((sn * dx + cs * dz) >> 12);
}
/* Original: Hand (Part-Translation) im Kopf-Rahmen = M8^T (t_hand - t8). */
static void orig_kopf_rahmen(const r2o_bild_t *b, int32_t out[3])
{
    int64_t dx = b->A2.t[0] - b->L8.t[0], dy = b->A2.t[1] - b->L8.t[1], dz = b->A2.t[2] - b->L8.t[2];
    const int16_t *M = b->L8.m;
    out[0] = (int32_t)(((int64_t)M[0] * dx + (int64_t)M[3] * dy + (int64_t)M[6] * dz) >> 12);
    out[1] = (int32_t)(((int64_t)M[1] * dx + (int64_t)M[4] * dy + (int64_t)M[7] * dz) >> 12);
    out[2] = (int32_t)(((int64_t)M[2] * dx + (int64_t)M[5] * dy + (int64_t)M[8] * dz) >> 12);
}
/* Original: Arm-Vertices (EM2D-Mesh Hand-1 / Hand, aus der geladenen RE2-Bank) mit den RAM-Matrizen gegen die
 * Huelle (Leon Part 0/8 aus der RAM). */
static int orig_arm_in_leon(const r2o_lauf_t *l, int i)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) return -1;
    const r2o_bild_t *b = &l->b[i];
    const int hb = l->var ? 10 : 3;
    int innen = 0;
    for (int k = 0; k < 2; k++) {
        const r2o_mat_t *A = k ? &b->A2 : &b->A1;
        const re15_md1_mesh_t *m = &eb->md1.meshes[hb - 1 + k];
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
            int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
            for (int v = 0; v < nv; v++) {
                int32_t P[3];
                for (int j = 0; j < 3; j++)
                    P[j] = A->t[j] + (int32_t)(((int64_t)A->m[3*j] * vv[v].x + (int64_t)A->m[3*j+1] * vv[v].y +
                                                (int64_t)A->m[3*j+2] * vv[v].z) >> 12);
                if (vol_innen_welt(&s_vol[0], b->L0.m, b->L0.t, P) || vol_innen_welt(&s_vol[1], b->L8.m, b->L8.t, P))
                    innen++;
            }
        }
    }
    return innen;
}
static int orig_zyklus(const r2o_lauf_t *l, int *mx)
{
    int n = 0; if (mx) *mx = 0;
    for (int i = 0; i < R2O_BILDER; i++) {
        int v = orig_arm_in_leon(l, i);
        if (v > 0) n++;
        if (mx && v > *mx) *mx = v;
    }
    return n;
}
static const r2o_lauf_t *orig_lauf(const char *name)
{
    for (int i = 0; i < R2O_LAEUFE; i++) if (!strcmp(s_r2o[i].name, name)) return &s_r2o[i];
    return NULL;
}

/* Aufzeichnung je Halte-Bild f. */
typedef struct { int ok; int32_t h[3], lx, lz, rel[3], hw[3], kw[3]; int yaw; int pfr, afr; int vi, vs;
                 int nyaw, npit, nziel; } bild_t;
#define FAELLE 8
static bild_t s_auf[FAELLE][60];
static int    s_auf_yaw[FAELLE];
static int    s_zyk[FAELLE], s_zyk_max[FAELLE];        /* Volumenmass ueber EINEN Halte-Zyklus (19 Bilder) */
static int32_t s_pin[FAELLE][3], s_wurzel[FAELLE][3];  /* Pin-Quelle und Arm-Ursprung im Griff-Bild       */
static int    s_dist[FAELLE];                          /* Leon - Arm-Ursprung nach dem Griff-Bild          */
static int    s_gleich[FAELLE];
static int    s_halter[FAELLE];                        /* Slot des haltenden Arms (Messarm 5)              */
static int    s_ziele[FAELLE][4], s_nziele[FAELLE];    /* gewaehlte Blickziele im Halten (Slots, 0 = SELBST) */

/* Ein Griff des Messarms mit Leon-Blick `yaw0` vor dem Zugriff, Armhoehe `hoehe` (0 = Port-Wert
 * RE2ARM_1210_Y; sonst ueber den Mess-Haken RE15_ARM_ANKER des Moduls).
 * NB4 `wechsel`: -2 = Blickwahl laeuft frei (Nutzerfall, ROOM1210 mit allen zehn Armen). Sonst die Ausgangslage
 * eines Original-Mitschnitts: Ziel SELBST im ersten Halte-Bild (RAM aller zwoelf Griffe), die anderen neun Arme
 * schlafen (im Original war nur die Fuenfergruppe des Halters wach, die Gegenreihe 15000 entfernt, > 7000), erste
 * Suche FUN_8003DB38 in dem Port-Bild, das per Arm-Bild auf das Original-Halte-Bild `wechsel` faellt. */
static int griff(int yaw0, const char *name, int fall, int hoehe, int wechsel)
{
    memset(s_auf[fall], 0, sizeof s_auf[fall]);
    s_zyk[fall] = s_zyk_max[fall] = 0; s_auf_yaw[fall] = -1; s_gleich[fall] = 0;
    static char env[64];
    if (hoehe) snprintf(env, sizeof env, "RE15_ARM_ANKER=400,%d", hoehe);
    else snprintf(env, sizeof env, "RE15_ARM_ANKER=");
    putenv(env);
    int slots[RE15_ACTOR_MAX]; int n = aufsetzen(slots);
    if (n < 10) { CHECK("zehn Arme + EM2D-Bank", 0); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int a5 = slots[4]; re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    pl->x = hx + 3500; pl->z = hz + 3500;
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;      /* Member_set(12,1) @0x1EDA */
    for (int f = 0; f < 20 && arm->sub_state_2 != 2; f++) frame_step();
    pl->x = hx + 2000; pl->z = hz;
    frame_step();                                                     /* A0 -> 0x101 REACH */
    int32_t hand[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), hand);
    pl->x = hand[0] + 300; pl->z = hand[2]; pl->rot_y = (int16_t)yaw0;
    int grab = 0;
    for (int f = 0; f < 60 && !grab; f++) {
        frame_step();
        if (arm->sub_state_1 == 4) grab = 1;
    }
    char nm[400];
    snprintf(nm, sizeof nm, "[%s] Griff kommt zustande (A3 0x401 @0x80100A50)", name);
    CHECK(nm, grab);
    if (!grab) return -1;
    s_halter[fall] = a5; s_nziele[fall] = 0;
    int ziel_bild_afr = -1;
    if (wechsel != -2) {
        re15_re2arm_look_debug(255, RE15_ACTOR_SLOT_PLAYER, NULL, NULL);    /* SELBST, keine Suche */
        for (int i = 0; i < n; i++) if (slots[i] != a5) g_actors[slots[i]].grid_id = 0;   /* Schlaf */
        /* Original-Halte-Bild i (RAM am Bildanfang = Ende von Bild i-1) <-> Port-Aufzeichnung mit Arm-Bild
         * (acw(i) + 18) % 19 (dieselbe Ausrichtung wie vergleiche()); acw(i) = i % 19 (Clip 5 ab Bild 0). */
        if (wechsel >= 0) ziel_bild_afr = ((wechsel % 19) + 18) % 19;
    }
    int32_t rein[3];
    (void)re15_re2arm_hand_parts(a5, s_pin[fall], rein);
    s_wurzel[fall][0] = arm->x; s_wurzel[fall][1] = arm->y; s_wurzel[fall][2] = arm->z;
    s_dist[fall] = (int)lround(sqrt((double)(pl->x - arm->x) * (pl->x - arm->x) + (double)(pl->z - arm->z) * (pl->z - arm->z)));
    printf("  [%s] Arm y %d | Pin (gemischte Parts) (%d,%d) | Leon nach dem Griff-Bild (%d,%d) = %d vom Ursprung (%d,%d)\n",
           name, (int)arm->y, s_pin[fall][0], s_pin[fall][2], pl->x, pl->z, s_dist[fall], arm->x, arm->z);
    int gleich = 1, zyk = 0;
    for (int f = 0; f < 60; f++) {
        /* erste Suche im ausgerichteten Bild: Zaehler 0 VOR dem Schritt (FUN_8003DB38 laeuft im Spieler-Pass
         * dieses Schritts, @0x8003db90-a4); naechstes Arm-Bild = jetziges + 1 (Clip 5, Takt @0x80100D18). */
        if (ziel_bild_afr >= 0 && arm->motion == 5 && abs(f - wechsel) <= 9 &&
            ((int)arm->anim_frame + 1) % 19 == ziel_bild_afr) {
            re15_re2arm_look_debug(0, -1, NULL, NULL); ziel_bild_afr = -1;
        }
        frame_step();
        if (f == 0) s_auf_yaw[fall] = (int)pl->rot_y & 0xfff;      /* nach Hook-P0 (@0x801012E0-18) */
        if (arm->sub_state_1 != 4 || re15_player_victim_state() != 4) break;
        int pose = leon_pose(pl);                                   /* Zeichner: jedes Bild (Nacken-FSM) */
        {   int z = (int)pl->neck_target_slot, neu = 1;
            for (int q = 0; q < s_nziele[fall]; q++) if (s_ziele[fall][q] == z) neu = 0;
            if (neu && s_nziele[fall] < 4) s_ziele[fall][s_nziele[fall]++] = z; }
        if (pl->motion != 0 || arm->motion != 5) continue;
        if ((int)pl->anim_frame != (((int)arm->anim_frame + 1) % 19)) gleich = 0;
        if (f < 14) continue;     /* Aufzeichnung ab 14 (Ausrichtung (6a) braucht Bild 15); Mass ab 16 (Rate 15) */
        if (!pose) continue;
        bild_t *a = &s_auf[fall][f];
        a->nyaw = pl->neck_yaw; a->npit = pl->neck_pitch; a->nziel = pl->neck_target_slot;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), a->h);
        a->ok = 1; a->lx = pl->x; a->lz = pl->z; a->yaw = (int)pl->rot_y & 0xfff;
        a->pfr = pl->anim_frame; a->afr = arm->anim_frame;
        port_kopf_rahmen(pl, a->h, a->rel);
        wurzel_rahmen(pl->x, pl->z, pl->rot_y, a->h, a->hw);
        {   int32_t k8[3]; re15_skel_bone_to_world(s_leon_pose[1].trans, pl->rot_y, pl->x, 0, pl->z, k8);
            wurzel_rahmen(pl->x, pl->z, pl->rot_y, k8, a->kw); }
        a->vi = vol_arm_in_leon(arm, s_leon_pose, pl->rot_y, pl->x, pl->z, 0);
        a->vs = vol_arm_in_leon(arm, s_leon_pose, pl->rot_y, pl->x, pl->z, 1);
        if (f >= 16 && zyk < 19) { zyk++; if (a->vi > 0) s_zyk[fall]++; if (a->vi > s_zyk_max[fall]) s_zyk_max[fall] = a->vi; }
    }
    s_gleich[fall] = gleich;
    snprintf(nm, sizeof nm, "[%s] (2) Leon zeigt jedes Halte-Bild Arm-Bild + 1 (Clip 0/Clip 5, 19 Bilder)", name);
    CHECK(nm, gleich);
    printf("  [%s] Volumenmass ueber einen Halte-Zyklus (Bilder 16..34, Kopf wie gezeichnet): Arm in Leon in %d/19 "
           "(max %d Vertices) | Blickziele im Halten:", name, s_zyk[fall], s_zyk_max[fall]);
    for (int q = 0; q < s_nziele[fall]; q++)
        printf(" %d%s", s_ziele[fall][q], s_ziele[fall][q] == 0 ? "=SELBST" : (s_ziele[fall][q] == a5 ? "=Halter" : ""));
    printf("\n");
    putenv((char *)"RE15_ARM_ANKER=");
    return 0;
}

/* (6a) Port gegen Original-Lauf, Bild fuer Bild ueber das Arm-Bild ausgerichtet. */
static int vergleiche(int fall, const r2o_lauf_t *l, int *maxdev, int *bilder, int *maxkopf, int *maxnacken,
                      int *zielgleich)
{
    *maxdev = 0; *bilder = 0; *maxkopf = 0; *maxnacken = 0; *zielgleich = 1;
    for (int i = 0; i < R2O_BILDER; i++) {
        const r2o_bild_t *b = &l->b[i];
        /* Ausrichtung: am Haltepunkt B4 P1 (Bildanfang) zeigt +0x14D das Bild, das 0x8002959C IN diesem Bild
         * posiert und dann weiterzaehlt (@0x800295E8 / @0x80029B30); die Part-Matrizen in der RAM stammen vom
         * Zeichnen des VORbilds = Pose (+0x14D - 1). Der Port fragt die Pose seines aktuellen anim_frame ab
         * (nach dem Zaehlen) -> Port-Bild k <-> Original-Clipwort-Bild k + 1 (Leon wie Arm, Arm + 1 bleibt). */
        int afr = ((int)((b->acw >> 8) & 0xff) + 18) % 19;
        /* NB4: der Blick laeuft ueber die Zeit (Suche, Akku) -> dasselbe Arm-Bild im SELBEN Zyklus nehmen: Original-
         * Halte-Bild i = 16 + Index, Port-Aufzeichnung f am naechsten an i - 1 (Port-Bild f <-> RAM am Anfang von f+1). */
        const int oi = 16 + i;
        const bild_t *a = NULL; int bd = 1 << 30;
        for (int f = 14; f < 60; f++)
            if (s_auf[fall][f].ok && s_auf[fall][f].afr == afr && abs(f - (oi - 1)) < bd) { a = &s_auf[fall][f]; bd = abs(f - (oi - 1)); }
        if (!a) continue;
        int32_t o[3]; orig_kopf_rahmen(b, o);
        int32_t ohw[3], okw[3];
        wurzel_rahmen(b->lx, b->lz, b->lyaw, b->A2.t, ohw);
        wurzel_rahmen(b->lx, b->lz, b->lyaw, b->L8.t, okw);
        for (int j = 0; j < 3; j++) {
            int d = abs(ohw[j] - a->hw[j]); if (d > *maxdev) *maxdev = d;
            d = abs(okw[j] - a->kw[j]);     if (d > *maxdev) *maxdev = d;
            d = abs(o[j] - a->rel[j]);      if (d > *maxkopf) *maxkopf = d;      /* Hand im KOPF-Rahmen */
        }
        {   int dy = abs(((b->nyaw - a->nyaw + 2048) & 0xfff) - 2048), dp = abs(((b->npit - a->npit + 2048) & 0xfff) - 2048);
            if (dy > *maxnacken) *maxnacken = dy;
            if (dp > *maxnacken) *maxnacken = dp; }
        {   int oz = (b->nziel < 0) ? 0 : (b->nziel == l->satz ? 1 : 2);             /* SELBST / Halter / anderer */
            int pz = (a->nziel == RE15_ACTOR_SLOT_PLAYER) ? 0 : (a->nziel == s_halter[fall] ? 1 : 2);
            if (oz != pz) *zielgleich = 0; }
        if (getenv("R35_ARME_DUMP"))
            printf("    DUMP %s Port-Arm-Bild %2d: Hand/Wurzel O (%5d,%5d,%5d) P (%5d,%5d,%5d) | Kopf/Wurzel O (%5d,%5d,%5d) "
                   "P (%5d,%5d,%5d) | Hand/Kopf-Rahmen O (%4d,%4d,%4d) P (%4d,%4d,%4d) | Blick O (%4d,%4d) z%d P (%4d,%4d) z%d\n",
                   l->name, afr, ohw[0], ohw[1], ohw[2],
                   a->hw[0], a->hw[1], a->hw[2], okw[0], okw[1], okw[2], a->kw[0], a->kw[1], a->kw[2], o[0], o[1], o[2],
                   a->rel[0], a->rel[1], a->rel[2], b->nyaw, b->npit, b->nziel, a->nyaw, a->npit, a->nziel);
        (*bilder)++;
    }
    return 1;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: ROOM1210.RDT\n"); return 1; }
    {   size_t esz = 0, rsz = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        if (!edd || !emr || re15_emd_parse_animation(edd, esz, &s_pa) != 0 ||
            re15_emd_parse_skeleton(emr, rsz, &s_ps) != 0) { printf("FAIL: PL00\n"); return 1; } }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK("PL00.MD1 Kopf (Mesh 8) / Rumpf (Mesh 0) als Volumen geladen", vol_laden(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1"));

    /* ---- ORIGINAL-WERTE aus der RAM ------------------------------------------------------------ */
    if (!load_re2_bank_1A()) { printf("FAIL: RE2/CDEMD0.EMS (EM2D-Mesh fuer das Original-Volumenmass)\n"); return 1; }
    int odmin = 1 << 30, odmax = 0, ogleich = 1;
    /* (1)-Spanne NUR aus den sauberen Griffen: in g3/g4 (Halter Satz 2) hat der weiter sichtbare Nachbar Satz 0
     * geschoben — Leon steht dort 1295 vom Halter, aber 1251 vom Ursprung von Satz 0 (Abnahme 3, M3). */
    printf("  Original-RAM (ROOM2050, Halte-Bilder 16..34):\n");
    for (int k = 0; k < R2O_LAEUFE; k++) {
        const r2o_lauf_t *l = &s_r2o[k];
        int mx = 0, z = orig_zyklus(l, &mx);
        const r2o_bild_t *b = &l->b[0];
        int d = (int)lround(sqrt((double)(b->lx - b->ax) * (b->lx - b->ax) + (double)(b->lz - b->az) * (b->lz - b->az)));
        const int sauber = strcmp(l->name, "g3_west_gesicht") && strcmp(l->name, "g4_west_ruecken");
        if (sauber && d < odmin) odmin = d;
        if (sauber && d > odmax) odmax = d;
        for (int i = 0; i < R2O_BILDER; i++)
            if ((int)((l->b[i].lcw >> 8) & 0xff) != (int)(((l->b[i].acw >> 8) & 0xff) + 1) % 19) ogleich = 0;
        printf("    %-16s Satz %d y %5d Arm %c %s: Leon %d vom Ursprung, Arm in Leon %2d/19 (max %d)\n", l->name, l->satz,
               l->hoehe, l->var ? 'B' : 'A', l->griff ? "Ruecken" : "Gesicht", d, z, mx);
    }
    CHECK("(O1) Original-RAM: Leon = Arm-Bild + 1 in allen 12 x 19 Halte-Bildern (Clipwort-Bytes +0x14D)", ogleich);
    {   int paare = 1;
        const char *p[4][2] = { {"g1_ost_gesicht","g2_ost_ruecken"}, {"g3_west_gesicht","g4_west_ruecken"},
                                {"g5_r5_gesicht","g6_r5_ruecken"}, {"g7_r0_gesicht","g8_r0_ruecken"} };
        for (int k = 0; k < 4; k++) {
            const r2o_lauf_t *g = orig_lauf(p[k][0]), *r = orig_lauf(p[k][1]);
            if (!g || !r) { paare = 0; continue; }
            for (int i = 0; i < R2O_BILDER; i++) {
                const r2o_bild_t *a = &g->b[i], *c = &r->b[i];
                if (a->lx != c->lx || a->lz != c->lz || (((int)c->lyaw - (int)a->lyaw) & 0xfff) != 2048 ||
                    a->acw != c->acw || a->A2.t[0] != c->A2.t[0] || a->A2.t[2] != c->A2.t[2]) paare = 0;
            }
        }
        CHECK("(4d) Original-RAM: Ruecken-Griff = Gesicht-Griff mit Leon-Blick + 2048 bei GLEICHEM Pin und GLEICHER "
              "Hand-Bahn (vier Paare; Flip @0x8010130C-18)", paare);
    }

    /* (N1) RE2-Zielwahl FUN_8003DB38 auf der ORIGINAL-Geometrie, gerechnet mit DERSELBEN Port-Funktion
     * re15_re2arm_look_waehle, die re15_re2arm_player_look benutzt (Kegel re15_ai_arc_test = FUN_80015614 1500
     * @0x8003dc8c, Radius 7000 @0x8003c1b0, naechster @0x8003dcb0, +0x10E-Ausschluss @0x8003dc08). Die Sicht
     * 0x80050858 kommt aus re2_los.py (FUN_80050858 Zeile fuer Zeile auf ROOM2050 collision.sca) mit dem Zielpunkt,
     * den das Original nimmt: Part[+0x1C1]+0x5C = gezeichnete Lage (Halter: Ursprung; nie gezeichnet: (0,0,0)).
     * (N1b) Laeufe mit ALLEN zehn Armen im RAM (n4_g8c Ruecken, n4_g1b Gesicht): Bild der Suche + Ziel gemessen.
     * (N1a) die zwoelf Halte-Mitschnitte (RAM nur Leon + Halter): ENDE-Arme nie gezeichnet wie in n4_g8c/n4_g1b
     *       gemessen. Gilt fuer die neun sauberen Laeufe; in g1/g3/g4 blickt Leon auf einen ENDE-Nachbarn, der
     *       also gezeichnet war (g3/g4: der sichtbare, schiebende Nachbar, Abnahme 3 M3) — dort nur berichtet. */
    {   int ok = 1;
        for (int k = 0; k < R2N_SUCHEN; k++) {
            const r2n_suche_t *u = &s_r2n[k];
            re15_actor_t lp; memset(&lp, 0, sizeof lp);
            lp.x = u->lx; lp.z = u->lz; lp.rot_y = u->lyaw;
            re2look_kand_t kd[10];
            for (int a2 = 0; a2 < 10; a2++) {
                kd[a2].aktiv = u->arm[a2].aktiv; kd[a2].f10e = u->arm[a2].f10e; kd[a2].sicht_frei = u->arm[a2].sicht;
                kd[a2].x = u->arm[a2].x; kd[a2].z = u->arm[a2].z;
            }
            int w = re15_re2arm_look_waehle(&lp, 10, kd);
            int port = (w < 0) ? -1 : u->arm[w].satz;
            printf("  (N1b) %-20s Suche in Halte-Bild %d, Leon (%d,%d) Blick %d: Port-Wahl %s%d | RAM-Ziel danach %s%d\n",
                   u->name, u->bild, u->lx, u->lz, (int)u->lyaw, port < 0 ? "SELBST " : "Satz ", port,
                   u->ziel < 0 ? "SELBST " : "Satz ", u->ziel);
            if (port != u->ziel) ok = 0;
        }
        CHECK("(N1b) RE2-Zielwahl (re15_re2arm_look_waehle) = Ziel der RE2-RAM in den Laeufen mit allen zehn Armen "
              "(Ruecken: SELBST trotz Satz 4 im Kegel — nie gezeichnet, Sicht auf (0,0,0) verdeckt; Gesicht: der Halter)", ok);
        ok = 1;
        for (int k = 0; k < R2O_LAEUFE; k++) {
            const r2o_lauf_t *l = &s_r2o[k];
            re15_actor_t lp; memset(&lp, 0, sizeof lp);
            lp.x = l->b[0].lx; lp.z = l->b[0].lz; lp.rot_y = l->b[0].lyaw;
            const int g0 = (l->satz < 5) ? 0 : 5;
            re2look_kand_t kd[5];
            for (int n = 0; n < 5; n++) {
                kd[n].aktiv = 1; kd[n].f10e = 0; kd[n].sicht_frei = (uint8_t)((s_r2o_sicht[k] >> n) & 1);
                kd[n].x = s_r2o_saetze[g0 + n][0]; kd[n].z = s_r2o_saetze[g0 + n][2];
            }
            int w = re15_re2arm_look_waehle(&lp, 5, kd);
            int port = (w < 0) ? -1 : g0 + w;
            int orig = l->b[R2O_BILDER - 1].nziel;           /* Ziel in Halte-Bild 34 (alle Wechsel <= 31) */
            const int nachbar = !strcmp(l->name, "g1_ost_gesicht") || !strcmp(l->name, "g3_west_gesicht") ||
                                !strcmp(l->name, "g4_west_ruecken");
            printf("  (N1a) %-16s Leon (%d,%d) Blick %d: Port-Wahl %s%d | RAM %s%d (ab Halte-Bild %d)%s\n", l->name,
                   lp.x, lp.z, (int)lp.rot_y, port < 0 ? "SELBST " : "Satz ", port, orig < 0 ? "SELBST " : "Satz ", orig,
                   l->wechsel, nachbar ? " — ENDE-Nachbar war gezeichnet, nur berichtet" : "");
            if (!nachbar && port != orig) ok = 0;
        }
        CHECK("(N1a) RE2-Zielwahl = Ziel der RE2-RAM in den neun sauberen Halte-Mitschnitten (Gesicht: Halter ab dem "
              "gemessenen Wechselbild; Ruecken g2/g6/g8: SELBST, Kopf in der Animationspose)", ok);
    }

    /* ---- PORT ------------------------------------------------------------------------------------ */
    griff(2048, "Gesicht", 0, 0, -2);             /* Leon dem Fenster zugewandt -> kein Flip */
    griff(0, "Ruecken", 1, 0, -2);                /* vom Fenster abgewandt -> Flip +2048 (@0x80101304-18) */
    /* Lagen der Original-Griffe mit Arm vor Leon (Arm-Blick = Heimat-Blick): gleiche Hoehe, gleicher Griff. */
    static const struct { const char *lauf; int yaw0; int fall; } VG[5] = {
        { "g5_r5_gesicht", 2048, 3 }, { "g6_r5_ruecken", 0, 4 }, { "g7_r0_gesicht", 2048, 5 },
        { "g8_r0_ruecken", 0, 6 }, { "g11_r7_gesicht", 2048, 7 } };
    for (int k = 0; k < 5; k++) {
        const r2o_lauf_t *l = orig_lauf(VG[k].lauf);
        char nm[64]; snprintf(nm, sizeof nm, "wie %s (y %d)", VG[k].lauf, l ? l->hoehe : 0);
        if (l) {
            /* Wechselbild: Gesicht = gemessen; Ruecken (Original ohne Wechsel in 40 Bildern) = das des Gesicht-
             * Zwillings derselben Lage — dort laeuft dieselbe Suche, der Halter liegt nur hinter Leon. */
            int w = l->wechsel;
            if (w < 0) { const r2o_lauf_t *zw = orig_lauf(k == 1 ? "g5_r5_gesicht" : "g7_r0_gesicht"); w = zw ? zw->wechsel : 20; }
            griff(VG[k].yaw0, nm, VG[k].fall, l->hoehe, w);
        }
    }

    /* (1) Pin + Koerper-Push: Abstand wie im Original, auf dem Strahl Ursprung -> Pin. */
    {   int ok = 1;
        for (int c = 0; c < FAELLE; c++) {
            if (c == 2) continue;                 /* Fall 2 = RE1.5-KI (unten) */
            double px = s_pin[c][0] - s_wurzel[c][0], pz = s_pin[c][2] - s_wurzel[c][2];
            const bild_t *a = NULL;
            for (int f = 16; f < 60; f++) if (s_auf[c][f].ok) { a = &s_auf[c][f]; break; }
            if (!a) { ok = 0; continue; }
            double lx = a->lx - s_wurzel[c][0], lz = a->lz - s_wurzel[c][2];
            double kreuz = (px * lz - pz * lx) / (sqrt(px * px + pz * pz) * sqrt(lx * lx + lz * lz) + 1e-9);
            if (s_dist[c] < odmin - 1 || s_dist[c] > odmax + 1 || fabs(kreuz) > 0.01) ok = 0;
            printf("  (1) Fall %d: Leon %d vom Ursprung (Original %d..%d), Strahl-Abweichung sin %.4f\n", c, s_dist[c],
                   odmin, odmax, kreuz);
        }
        CHECK("(1) Pin auf der gemischten Hand (@0x80100C18-38), danach RE2-Koerper-Push FUN_80034D0C (r 800 @0x80100338-3C "
              "+ 450 @0x8003bdc0-c4): Leon auf dem Strahl Ursprung->Pin im Abstand des ORIGINALS (10 saubere Griffe, "
              "ohne g3/g4)", ok);
    }

    /* (6a) Port = Original in denselben Lagen. */
    {   int ok = 1;
        for (int k = 0; k < 5; k++) {
            const r2o_lauf_t *l = orig_lauf(VG[k].lauf);
            if (!l) { ok = 0; continue; }
            int maxdev = 0, bilder = 0, omx = 0, maxkopf = 0, maxnacken = 0, zielgleich = 0;
            vergleiche(VG[k].fall, l, &maxdev, &bilder, &maxkopf, &maxnacken, &zielgleich);
            int oz = orig_zyklus(l, &omx);
            printf("  (6a) %-15s %d Bilder: Wurzel-Rahmen max %d, KOPF-Rahmen max %d, Blick-Akku max %d, Ziel %s | Arm in "
                   "Leon Port %d/19 (max %d), Original %d/19 (max %d)\n", VG[k].lauf, bilder, maxdev, maxkopf, maxnacken,
                   zielgleich ? "gleich" : "VERSCHIEDEN", s_zyk[VG[k].fall], s_zyk_max[VG[k].fall], oz, omx);
            if (bilder < 19 || maxdev > R35_6A_TOL || maxkopf > R35_6A_KTOL || maxnacken > R35_6A_NTOL || !zielgleich ||
                abs(s_zyk[VG[k].fall] - oz) > R35_6A_ZTOL) ok = 0;
        }
        CHECK("(6a) Port = RE2-Original in fuenf Original-Lagen (Hoehe -2480/-2580/-2700, Gesicht + Ruecken), Leon posiert "
              "wie der Zeichner: Hand im Wurzel- UND Kopf-Rahmen, Blick-Akku Part 8 und Blickziel Bild fuer Bild, Zahl der "
              "Ueberschneidungsbilder wie in der Original-RAM", ok);
    }
    /* (6b) Nutzerfall Port-Hoehe -2500 gegen die Nachbarhoehen des Originals. */
    {   int og = 0, orr = 0, mx = 0;
        const char *g[3] = { "g5_r5_gesicht", "g9_r8_gesicht", "g7_r0_gesicht" };
        for (int k = 0; k < 3; k++) { const r2o_lauf_t *l = orig_lauf(g[k]); int z = l ? orig_zyklus(l, &mx) : 0; if (z > og) og = z; }
        const char *r[2] = { "g6_r5_ruecken", "g8_r0_ruecken" };
        for (int k = 0; k < 2; k++) { const r2o_lauf_t *l = orig_lauf(r[k]); int z = l ? orig_zyklus(l, &mx) : 0; if (z > orr) orr = z; }
        printf("  (6b) Port y -2500: Gesicht %d/19, Ruecken %d/19 | Original Nachbarhoehen (-2480/-2540/-2580): Gesicht "
               "hoechstens %d/19, Ruecken hoechstens %d/19\n", s_zyk[0], s_zyk[1], og, orr);
        CHECK("(6b) Nutzerfall (Arm 5 ROOM1210, Port-Hoehe -2500 = PORT-BRUECKE aus der Fensterbank): nicht mehr "
              "Ueberschneidungsbilder als das RE2-Original an den Nachbarhoehen (Satz 5/8/0)", s_zyk[0] <= og && s_zyk[1] <= orr);
    }

    /* (4a-c) Konstruktion im Port (Gesicht-/Ruecken-Lauf derselben Lage). */
    {
        int vergl = 0, wurzel = 1, hand = 1, maxdh = 0;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[1][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if (g->lx != r->lx || g->lz != r->lz || g->pfr != r->pfr || g->afr != r->afr) wurzel = 0;
            int dh = abs(g->h[0] - r->h[0]) + abs(g->h[2] - r->h[2]);
            if (dh > maxdh) maxdh = dh;
            if (dh > 2) hand = 0;
        }
        printf("  (4) Ruecken vs. Gesicht: %d Bilder verglichen, Blick %d / %d, Hand-Abweichung max %d\n",
               vergl, s_auf_yaw[1], s_auf_yaw[0], maxdh);
        CHECK("(4a) Ruecken-Griff: Leon-Blick = Gesicht-Blick + 2048 (Flip @0x8010130C-18; Original-RAM (4d))",
              s_auf_yaw[0] >= 0 && s_auf_yaw[1] == ((s_auf_yaw[0] + 2048) & 0xfff));
        CHECK("(4b) Wurzel, Bildtakt und Hand-Bahn haengen nicht an Leons Blick (Arm liest PL+0x76 nie; Original-RAM (4d))",
              vergl >= 30 && wurzel && hand);
    }

    /* (5) RE1.5-KI: Typ 0x1A laeuft in JEDEM Flavor auf dem RE2-EM2D-Gehirn (re15_ai_re2_for_type). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK("(5a) RE1.5-KI: Typ 0x1A gehoert dem RE2-EM2D-Gehirn (re15_ai_re2_for_type(0x1A) = 1)",
          re15_ai_re2_for_type(0x1Au) == 1);
    griff(2048, "RE1.5-KI Gesicht", 2, 0, -2);
    {   int gleich = 1, vergl = 0;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[2][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if (g->lx != r->lx || g->lz != r->lz || g->pfr != r->pfr || g->afr != r->afr || g->yaw != r->yaw || g->vi != r->vi) gleich = 0;
        }
        CHECK("(5b) RE1.5-KI: derselbe Griff Bild fuer Bild wie unter RE2-KI (Wurzel, Blick, Leon-/Arm-Bild, Volumenmass)",
              vergl >= 30 && gleich);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    printf(g_fail ? "test_r35_raeume_arme: FAIL\n" : "test_r35_raeume_arme: OK\n");
    return g_fail ? 1 : 0;
}
