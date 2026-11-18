/* test_r42_lageplaene4.c — Strang lageplaene_nach2: der Revierplan in der Lobby 1030/1031 auf AUGENHOEHE.
 * Kopf mit allen Belegen: include/re15_kartenitems_r36.h (Abschnitt STRANG LAGEPLAENE_NACH2).
 *
 * NUTZERWUNSCH 18.11.2026 woertlich: "Bei 1030 und 1031 passt das nicht. Das der Revierplan so weit über den Spieler
 * hängt, das geht so nicht." (Stand 10287b077: Mitte y -4150, Unterkante -3430 ueber beiden Scheiteln.)
 *
 * Teile (je eigener ctest-Eintrag). Nur API/Zeilenfelder, die schon auf aaea5f8c2 bestanden (raum, aot_slot, x/y/z,
 * rect_x/rect_z, md1_bytes), damit die Gegenprobe dort und auf 10287b077 BAUT und rot wird:
 *   hoehe   Kopfband Leon (PLD/PL00.PLD) und Elza (PLD/PL04.PLD) aus den Knochenlagen GEMESSEN: Ursprung Knochen 8 (Plc_neck,
 *           neck_bone 8 @0x80031938, Formel skeleton_common.c) bis hoechste Ecke des Kopf-Meshs, Clip 0 Bild 0 (wie
 *           RE15_R36_KOPF_CLIP). Plan in 1030 UND 1031 an der Ostwand x 2398 (SCA-Zelle 1), seine Mitte liegt in BEIDEN
 *           Kopfbaendern, und die Plakatflaeche deckt beide Kopfbaender ganz (Augenhoehe beider Figuren).
 *   ort     Plan links der Ausgangstuer auf dem freien Wandstueck zwischen Door_aot_set Slot 0 (@0x01C6A / @0x01CD8) und dem
 *           Ende der SCA-Zelle 1 (@0x01A1C / @0x01A80), ganz im Bild der Tuerkamera 2 und mind. 40 Bildpunkte breit (320er
 *           Bild); Aufhebe-Rechteck beruehrt weder Notfall-Tafel Slot 9 (@0x01D42 / @0x01DB0) noch Tresen-Satz Slot 10
 *           (@0x01D56 / @0x01DC4); nach dem Raumstart sind Slot 9/10 weiter Tafel (msg 4) und Tresen (msg 6).
 *   druck   ECHTER Aktionsdruck am gemessenen Halt vor dem Plan (1930,-11036; der Tresen haelt beide Figuren dort an) und bei
 *           (1500,-11300), Blick Ostwand: keine Klettersonde, Ereignis 28. Blick auf den Tresen bei (1000,-11036): weiter
 *           dessen Text (msg 6), nicht der Plan.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_climb.h"
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_kartenitems_r36.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern unsigned         g_current_room_id;

static int g_fail = 0;
#define PRUEFE(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                               else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)
#define MIN_BREITE 40    /* Bildpunkte im 320er Bild (wie unit_r42_lageplaene3_revier; Kanalisationsplakat Kamera 11 = 37) */
#define WAND_X     2398  /* ROOM1030/1031 SCA-Zelle 1 Kante x (@0x01A1C / @0x01A80) */

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    memset(r, 0, sizeof *r);
    r->id = id;
    r->buf = slurp(p, &r->n);
    if (!r->buf) { printf("  FAIL: %s fehlt\n", p); g_fail = 1; return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) < 0) { printf("  FAIL: parse %s\n", p); g_fail = 1; return -1; }
    return 0;
}

static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Muster test_r42_lageplaene3.c. */
static void raum_start(raum_t *r, int32_t px, int32_t py, int32_t pz)
{
    re15_game_state_t flags = g_game;
    scd_vm_init();
    g_game = flags;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = r->id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(py));
    pl->floor = (uint8_t)re15_collision_band_from_y(py);
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* Plakatbreite/-hoehe aus den 5 MD1-Ecken @0x44 (Modell-z = Breite, Modell-x = Hoehe; karten_export.py). */
static int md1_masse(uint16_t raum, int *breite, int *hoehe)
{
    int msz = 0;
    const uint8_t *md1 = re15_kartenitems_r36_md1_bytes(raum, &msz);
    int16_t ex0 = 0, ex1 = 0, ez0 = 0, ez1 = 0;
    if (!md1) return -1;
    for (int k = 0; k < 5 && 0x44 + 8 * k + 6 <= msz; k++) {
        int16_t mx = (int16_t)le16(md1 + 0x44 + 8 * k), mz = (int16_t)le16(md1 + 0x48 + 8 * k);
        if (mx < ex0) ex0 = mx; if (mx > ex1) ex1 = mx; if (mz < ez0) ez0 = mz; if (mz > ez1) ez1 = mz;
    }
    *breite = ez1 - ez0; *hoehe = ex1 - ex0;
    return 0;
}

/* Bildrechteck des Plakats (Mitte M, Raumnormale n, Groesse b x h) in Kamera cut; 1 = ganz im Bild. */
static int im_bild(const raum_t *r, int cut, const double M[3], const double n[3], double b, double h, double box[4])
{
    re15_camera_view_t v;
    if (cut >= r->rdt.cut_count || re15_camera_build_view(&r->rdt.cuts[cut], &v) != 0) return 0;
    double rx[3] = { n[2], 0, -n[0] };
    int drin = 1;
    box[0] = 1e9; box[1] = -1e9; box[2] = 1e9; box[3] = -1e9;
    for (int a = -1; a <= 1; a += 2)
        for (int c = -1; c <= 1; c += 2) {
            double p[3] = { M[0] + a * b / 2 * rx[0], M[1] + c * h / 2, M[2] + a * b / 2 * rx[2] };
            double vx = (v.rot[0] * p[0] + v.rot[1] * p[1] + v.rot[2] * p[2]) / 4096.0 + v.trans[0];
            double vy = (v.rot[3] * p[0] + v.rot[4] * p[1] + v.rot[5] * p[2]) / 4096.0 + v.trans[1];
            double vz = (v.rot[6] * p[0] + v.rot[7] * p[1] + v.rot[8] * p[2]) / 4096.0 + v.trans[2];
            if (vz <= 0) return 0;
            double sx = 160.0 + v.fov_screen_dist * vx / vz, sy = 120.0 + v.fov_screen_dist * vy / vz;
            if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) drin = 0;
            if (sx < box[0]) box[0] = sx; if (sx > box[1]) box[1] = sx;
            if (sy < box[2]) box[2] = sy; if (sy > box[3]) box[3] = sy;
        }
    return drin;
}

/* ------------------------------------------------------------------------------------------------ Kopfband */
typedef struct { int ok; int32_t hals, scheitel; } kopfband_t;

/* Kopfband einer Figur aus ihrer PLD (EDD/EMR/MD1 = Teile 0/1/2, re15_pld_part): Clip 0 Bild 0, Pose wie der Zeichner
 * (re15_compute_actor_kf + re15_skel_compute_pose). hals = Modell-y des Plc_neck-Ursprungs (skeleton_common.c: Wurzel-Rot *
 * relpos[8] + Wurzel-Trans, neck_bone 8 @0x80031938); scheitel = kleinstes Modell-y der Kopf-Mesh-Ecken (Knochen 8). */
static kopfband_t kopfband(const char *pld)
{
    kopfband_t k = { 0, 0, 0 };
    char p[600];
    snprintf(p, sizeof p, "%s/PLD/%s", RE15_ASSET_PSX_DIR, pld);
    size_t n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b) { printf("  FAIL: %s fehlt\n", p); g_fail = 1; return k; }
    unsigned long o[3], l[3];
    static re15_emd_animation_t an; static re15_emd_skeleton_t sk; static re15_md1_t md;
    memset(&an, 0, sizeof an); memset(&sk, 0, sizeof sk); memset(&md, 0, sizeof md);
    int ok = 1;
    for (int t = 0; t < 3; t++) if (!re15_pld_part(b, (long)n, t, &o[t], &l[t])) ok = 0;
    if (ok && (re15_emd_parse_animation(b + o[0], l[0], &an) != 0 || re15_emd_parse_skeleton(b + o[1], l[1], &sk) != 0 ||
               re15_md1_parse(b + o[2], (int)l[2], &md) != 0 || sk.bone_count <= 8)) ok = 0;
    if (ok) {
        re15_actor_t a; memset(&a, 0, sizeof a);
        a.active = 1; a.motion = 0; a.anim_frame = 0;
        int kf = re15_compute_actor_kf(&an, &sk, &a, 0, 0);
        static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (kf < 0 || re15_skel_compute_pose(&sk, kf, P) != 0) ok = 0;
        if (ok) {
            const int bn = 8;
            int32_t rp[3] = { sk.bone_relative_pos[bn][0], sk.bone_relative_pos[bn][1], sk.bone_relative_pos[bn][2] };
            k.hals = ((P[0].rot[3] * rp[0] + P[0].rot[4] * rp[1] + P[0].rot[5] * rp[2]) >> 12) + P[0].trans[1];
            int mi = sk.bone_mesh_index[bn];
            if (mi < 0 || mi >= md.mesh_count) mi = bn;
            const re15_md1_mesh_t *m = &md.meshes[mi];
            int32_t top = 0; int gesehen = 0;
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *v = pass ? m->quad_vertices : m->tri_vertices;
                int nv = pass ? m->quad_vertex_count : m->tri_vertex_count;
                for (int i = 0; i < nv; i++) {
                    int32_t y = ((P[bn].rot[3] * v[i].x + P[bn].rot[4] * v[i].y + P[bn].rot[5] * v[i].z) >> 12) + P[bn].trans[1];
                    if (!gesehen || y < top) { top = y; gesehen = 1; }
                }
            }
            k.scheitel = top;
            k.ok = gesehen && k.scheitel < k.hals;
        }
    }
    free(b);
    return k;
}

/* Plakatmitte/-groesse einer Zeile mit Ostwand-Normale n = (-1,0,0): Flaeche 85*s vor dem Ursprung (karten_export.py). */
static int plakat_ost(uint16_t raum, double M[3], int *b, int *h)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    if (!z || md1_masse(raum, b, h) != 0) return -1;
    double s = *b / 1080.0;
    M[0] = z->x - 85.0 * s; M[1] = z->y; M[2] = z->z;
    return 0;
}

/* ------------------------------------------------------------------------------------------------ hoehe */
static void teil_hoehe(void)
{
    printf("=== hoehe: Revierplan auf Augenhoehe von Leon und Elza (Kopfband aus den Knochenlagen) ===\n");
    kopfband_t L = kopfband("PL00.PLD"), E = kopfband("PL04.PLD");
    PRUEFE(L.ok && E.ok, "Kopfband gemessen: Leon PL00 Hals %d .. Scheitel %d, Elza PL04 Hals %d .. Scheitel %d (Clip 0 Bild 0)",
           (int)L.hals, (int)L.scheitel, (int)E.hals, (int)E.scheitel);
    if (!L.ok || !E.ok) return;
    int32_t unten = L.hals > E.hals ? L.hals : E.hals;                 /* tiefster Hals (groesstes y)          */
    int32_t oben  = L.scheitel < E.scheitel ? L.scheitel : E.scheitel; /* hoechster Scheitel (kleinstes y)     */
    int32_t gem_u = L.hals < E.hals ? L.hals : E.hals;                 /* gemeinsames Band: hoeherer Hals ...  */
    int32_t gem_o = L.scheitel > E.scheitel ? L.scheitel : E.scheitel; /* ... bis niedrigerer Scheitel         */
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    for (int i = 0; i < 2; i++) {
        double M[3]; int b = 0, h = 0;
        if (plakat_ost(raeume[i], M, &b, &h) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile/kein Modell", raeume[i]); continue; }
        PRUEFE(fabs(M[0] - WAND_X) < 1.0, "ROOM%04X: Plakatflaeche an der Ostwand x %.0f (SCA-Zelle 1, Kante %d)", raeume[i],
               M[0], WAND_X);
        PRUEFE(M[1] <= gem_u && M[1] >= gem_o, "ROOM%04X: Plakatmitte y %.0f liegt in BEIDEN Kopfbaendern (%d .. %d)",
               raeume[i], M[1], (int)gem_u, (int)gem_o);
        PRUEFE(M[1] - h / 2.0 <= oben && M[1] + h / 2.0 >= unten,
               "ROOM%04X: Plakat y %.0f..%.0f deckt beide Kopfbaender ganz (%d .. %d)", raeume[i], M[1] + h / 2.0,
               M[1] - h / 2.0, (int)unten, (int)oben);
    }
}

/* ------------------------------------------------------------------------------------------------ ort */
static void teil_ort(void)
{
    printf("=== ort: links der Ausgangstuer, gross in der Tuerkamera 2, Zone ohne Ueberschneidung ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    static const uint32_t tuer_off[2]   = { 0x01C6A, 0x01CD8 };   /* main00 Door_aot_set Slot 0 -> 1050/1051       */
    static const uint32_t sca_off[2]    = { 0x01A1C, 0x01A80 };   /* SCA-Zelle 1 Ostwand                            */
    static const uint32_t tafel_off[2]  = { 0x01D42, 0x01DB0 };   /* main00 Aot_set Slot 9 Notfall-Tafel (msg 4)    */
    static const uint32_t tresen_off[2] = { 0x01D56, 0x01DC4 };   /* main00 Aot_set Slot 10 Tresen (msg 6)          */
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        const uint8_t *t = r.buf + tuer_off[i], *e = r.buf + sca_off[i];
        const uint8_t *tf = r.buf + tafel_off[i], *tr = r.buf + tresen_off[i];
        PRUEFE(t[0] == 0x3B && t[1] == 0 && tf[0] == 0x2C && tf[1] == 9 && tr[0] == 0x2C && tr[1] == 10 &&
               (int16_t)le16(e + 4) == WAND_X, "ROOM%04X RDT: Door_aot_set Slot 0 @0x%05X, Aot_set 9 @0x%05X, Aot_set 10 @0x%05X, "
               "SCA @0x%05X", raeume[i], (unsigned)tuer_off[i], (unsigned)tafel_off[i], (unsigned)tresen_off[i],
               (unsigned)sca_off[i]);
        int32_t tuer_z1 = (int16_t)le16(t + 8) + (int32_t)le16(t + 12);            /* -13800 + 2200 = -11600 */
        int32_t wand_z1 = (int16_t)le16(e + 6) + (int32_t)le16(e + 2);             /* -23528 + 13896 = -9632 */
        double M[3]; int b = 0, h = 0;
        if (plakat_ost(raeume[i], M, &b, &h) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile", raeume[i]); free(r.buf); continue; }
        PRUEFE(fabs(M[0] - WAND_X) < 1.0 && M[2] - b / 2.0 > tuer_z1 && M[2] + b / 2.0 < wand_z1,
               "ROOM%04X: Plakat x %.0f z %.0f..%.0f zwischen Tuerkante z %d und Wandende z %d (links der Tuer)", raeume[i],
               M[0], M[2] - b / 2.0, M[2] + b / 2.0, (int)tuer_z1, (int)wand_z1);
        double box[4], n[3] = { -1, 0, 0 };
        int drin = im_bild(&r, 2, M, n, b, h, box);
        PRUEFE(drin && box[1] - box[0] >= MIN_BREITE,
               "ROOM%04X Kamera 2: Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f = %.0f x %.0f Bildpunkte (mind. %d breit)",
               raeume[i], box[0], box[1], box[2], box[3], box[1] - box[0], box[3] - box[2], MIN_BREITE);
        /* Zone gegen die Original-Rechtecke (Aot_set: x s16 @+6, z s16 @+8, w u16 @+10, d u16 @+12) */
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raeume[i]);
        int beruehrt = 0;
        const uint8_t *s2[2] = { tf, tr };
        for (int k = 0; k < 2; k++) {
            int32_t ax = (int16_t)le16(s2[k] + 6), az = (int16_t)le16(s2[k] + 8);
            int32_t aw = le16(s2[k] + 10), ad = le16(s2[k] + 12);
            if (z->rect_x <= ax + aw && ax <= z->rect_x + RE15_R36K_RECT &&
                z->rect_z <= az + ad && az <= z->rect_z + RE15_R36K_RECT) beruehrt |= 1 << k;
        }
        PRUEFE(beruehrt == 0, "ROOM%04X: Aufhebe-Rechteck x %ld..%ld z %ld..%ld beruehrt Tafel (Slot 9) %s, Tresen (Slot 10) %s",
               raeume[i], (long)z->rect_x, (long)z->rect_x + RE15_R36K_RECT, (long)z->rect_z, (long)z->rect_z + RE15_R36K_RECT,
               (beruehrt & 1) ? "JA" : "nein", (beruehrt & 2) ? "JA" : "nein");
        re15_game_state_init();
        raum_start(&r, 1500, 0, -11300);
        PRUEFE(g_aot.slots[9].active && g_aot.slots[9].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[9].event_id == 4 &&
               g_aot.slots[10].active && g_aot.slots[10].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[10].event_id == 6,
               "ROOM%04X nach dem Raumstart: Slot 9 = Notfall-Tafel (msg 4), Slot 10 = Tresen (msg 6), unveraendert", raeume[i]);
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ druck */
static int druck(raum_t *r, int32_t sx, int32_t sz, int rot, int *klettert)
{
    re15_game_state_init();
    raum_start(r, sx, 0, sz);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    *klettert = 0;
    for (int k = -2; k <= 2; k++) {
        pl->x = sx; pl->z = sz + 120 * k; pl->rot_y = (int16_t)rot;
        re15_climb_reset();
        if (re15_climb_try_start(&r->rdt, 1)) (*klettert)++;
        re15_climb_reset();
    }
    pl->x = sx; pl->y = 0; pl->z = sz; pl->rot_y = (int16_t)rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    return g_aot.fired_event_id_this_frame;
}

static void teil_druck(void)
{
    printf("=== druck: echter Aktionsdruck vor dem Plan links der Tuer ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    /* (1930,-11036) = gemessener Halt beider Figuren an der Wand (Tresen SCA-Zelle 0 z -10568 haelt an; Laeufe l3/e2 des
     * Strangs: Leon und Elza enden beide dort); (1500,-11300) = mitten im Streifen vor dem Plan. */
    static const int32_t stand[2][2] = { { 1930, -11036 }, { 1500, -11300 } };
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        for (int s = 0; s < 2; s++) {
            int kl = 0;
            int ev = druck(&r, stand[s][0], stand[s][1], 0, &kl);   /* rot 0 = Blick +x = Ostwand (@0x80042bd0) */
            PRUEFE(kl == 0, "ROOM%04X (%ld,%ld): Klettersonde an 5 Standorten -> %d (0 = Druck bleibt fuer den Scan)",
                   raeume[i], (long)stand[s][0], (long)stand[s][1], kl);
            PRUEFE(ev == RE15_R36K_EREIGNIS, "ROOM%04X (%ld,%ld) Blick Ostwand -> Ereignis %d (soll %d)", raeume[i],
                   (long)stand[s][0], (long)stand[s][1], ev, RE15_R36K_EREIGNIS);
        }
        int kl = 0;
        int ev = druck(&r, 1000, -11036, 3072, &kl);   /* rot 3072: Vorwaertspunkt z + 620 = am Tresen (Slot 10) */
        PRUEFE(ev != RE15_R36K_EREIGNIS && g_scd.message_active && g_scd.message_id == 6,
               "ROOM%04X (1000,-11036) Blick Tresen -> Tresen-Text msg %d aktiv %d (Ereignis %d, nicht der Plan)", raeume[i],
               (int)g_scd.message_id, (int)g_scd.message_active, ev);
        free(r.buf);
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("=== Strang lageplaene_nach2: Revierplan auf Augenhoehe (Teil %s) ===\n", teil);
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "hoehe")) teil_hoehe();
    if (alle || !strcmp(teil, "ort"))   teil_ort();
    if (alle || !strcmp(teil, "druck")) teil_druck();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
