/* test_r42_kellerplan.c — Strang kellerplan: der Kellerplan in der Leichenhalle 1200/1201 doppelt so gross und auf AUGENHOEHE.
 * Kopf mit allen Belegen: include/re15_kartenitems_r36.h (Abschnitt STRANG KELLERPLAN), Generator
 * tools/r36_kartenitems/karten_export.py (Zeile KELLERPLAN in PLAENE2).
 *
 * Entscheid des Orchestrators (Strang kellerplan, 19.11.2026): "doppelt so gross wie bisher und auf Augenhoehe der Figuren wie
 * der Revierplan". Bis 364bf10a9 hing der Plan an der Nutzer-Marke mapB2.bmp neben der Vitrine (x -26330, Mitte y -2573,
 * 744 x 496 = MD1-Ecken z +-372 / x +-248, im Bild der Kamera 2 nur 17 x 14 Bildpunkte).
 *
 * Teile (je eigener ctest-Eintrag). Nur API/Zeilenfelder, die schon auf 364bf10a9 bestanden (des_raums, x/y/z, rect_x/rect_z,
 * aot_slot, md1_bytes; RE15_R42L_* aus re15_leichenhalle_r42.h), damit die Gegenprobe dort BAUT und rot wird:
 *   hoehe   Kopfband Leon (PLD/PL00.PLD) und Elza (PLD/PL04.PLD) aus den Knochenlagen GEMESSEN (wie unit_r42_lageplaene4_hoehe:
 *           Ursprung Knochen 8, neck_bone 8 @0x80031938, bis hoechste Ecke des Kopf-Meshs, Clip 0 Bild 0). Plakat in 1200 UND
 *           1201 an der Nordwand (SCA-Zelle 4 @0x006D4: z -28771 + 2000 = -26771), Mitte in BEIDEN Kopfbaendern, Flaeche deckt
 *           beide Kopfbaender ganz (Boden 0: ROOM11E0/11E1 Slot 1 next_y `00 00` @0x01542). Groesse = 2 x bisher (744 x 496).
 *   ort     Plakat auf dem freien Wandstueck zwischen der Eingangstuer (Door_aot_set Slot 0 @0x007BA, x -20890..-19200) und dem
 *           Schrank SCA-Zelle 16 (@0x00764, x bis -26210); ganz im Bild der Ankunftskamera 0 (ROOM11E0/11E1 Slot 1 next_cut
 *           @0x0154A = 0) mit mind. 34 Bildpunkten Breite (2 x 17) und ganz im Bild der Kamera 3 (Aufnahme-Stelle); weit weg von
 *           den Kuehlfaechern (obj 0/1 @0x007DA/@0x00804, z -14382, Modell 1440 lang). Aufhebe-Rechteck beruehrt weder die Tuer
 *           Slot 0, noch Aot 1 (@0x00908), noch Aot 2 = Minidisc/Vitrine (@0x0091E), noch die Klapper-Flaeche (Disc main00
 *           @0x007D2, RE15_R42L_X/Z/B/T). Nach dem Raumstart sind Slot 0/1/2 unveraendert (Tuer / Gegenstand / Ereignis 3).
 *   druck   ECHTER Aktionsdruck am gemessenen Halt vor dem Plan (-23262,-26177; echter Weg von der Tuer, Bild F210) und bei
 *           (-23550,-26177), Blick Nordwand (rot 1024): keine Klettersonde (FUN_8002d2c0 @0x8002d364-70), Ereignis 28. An der
 *           Tuer (-20045,-26177) Blick Nord: nicht der Plan.
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
#include "re15_leichenhalle_r42.h"

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

#define ALT_B       744     /* bis 364bf10a9: MD1-Ecken z +-372 (`74 01`) -> 744 breit */
#define ALT_H       496     /*                MD1-Ecken x +-248 (`f8 00`) -> 496 hoch  */
#define ALT_BILD_B  17      /* bis 364bf10a9: Kamera 2 x57..73 (Dossier N_kartenitems.md 5.2) */
#define OFF_SCA4    0x006D4u   /* SCA-Zelle 4 Nordwand: w, d, x, z (je 2 B) */
#define OFF_SCA16   0x00764u   /* SCA-Zelle 16 Schrank (Aot 1) */
#define OFF_TUER    0x007BAu   /* main00 Door_aot_set Slot 0 -> ROOM11E0 */
#define OFF_AOT1    0x00908u   /* sub00 Item_aot_set Slot 1 */
#define OFF_AOT2    0x0091Eu   /* sub00 Aot_set Slot 2 (Minidisc-Player an der Vitrine, Ereignis 3) */
#define OFF_OBJ0    0x007DAu   /* main00 Obj_model_set obj 0 (Kuehlfach) */
#define OFF_OBJ1    0x00804u   /* main00 Obj_model_set obj 1 (Kuehlfach-Tuer, die klappert) */
#define KUEHL_LEN   1440       /* Modell obj 0/1: bbox z -3..1440 (probe_r34n_e_dokumente modelle 1200) */

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
static int32_t  s16(const uint8_t *p) { return (int16_t)le16(p); }

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Muster test_r42_lageplaene4.c. */
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

/* Wie test_r42_lageplaene4.c kopfband(): Clip 0 Bild 0, hals = Modell-y des Plc_neck-Ursprungs (neck_bone 8 @0x80031938),
 * scheitel = kleinstes Modell-y der Kopf-Mesh-Ecken (Knochen 8). */
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

/* Plakatmitte/-groesse mit Nordwand-Normale n = (0,0,+1): Flaeche 85*s vor dem Ursprung (karten_export.py). */
static int plakat_nord(uint16_t raum, double M[3], int *b, int *h)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    if (!z || md1_masse(raum, b, h) != 0) return -1;
    double s = *b / 1080.0;
    M[0] = z->x; M[1] = z->y; M[2] = z->z + 85.0 * s;
    return 0;
}

static int32_t nordwand_z(const raum_t *r) { const uint8_t *e = r->buf + OFF_SCA4; return s16(e + 6) + (int32_t)le16(e + 2); }

static const uint16_t RAEUME[2] = { 0x1200, 0x1201 };

/* ------------------------------------------------------------------------------------------------ hoehe */
static void teil_hoehe(void)
{
    printf("=== hoehe: Kellerplan auf Augenhoehe von Leon und Elza, doppelt so gross ===\n");
    kopfband_t L = kopfband("PL00.PLD"), E = kopfband("PL04.PLD");
    PRUEFE(L.ok && E.ok, "Kopfband gemessen: Leon PL00 Hals %d .. Scheitel %d, Elza PL04 Hals %d .. Scheitel %d (Clip 0 Bild 0)",
           (int)L.hals, (int)L.scheitel, (int)E.hals, (int)E.scheitel);
    if (!L.ok || !E.ok) return;
    int32_t unten = L.hals > E.hals ? L.hals : E.hals;
    int32_t oben  = L.scheitel < E.scheitel ? L.scheitel : E.scheitel;
    int32_t gem_u = L.hals < E.hals ? L.hals : E.hals;
    int32_t gem_o = L.scheitel > E.scheitel ? L.scheitel : E.scheitel;
    /* Boden 0: einziger Eingang ROOM11E0/11E1 Door_aot_set Slot 1 @0x01532, next_y @0x01542 */
    static const uint16_t vorraum[2] = { 0x11E0, 0x11E1 };
    for (int i = 0; i < 2; i++) {
        raum_t v;
        if (raum_laden(&v, vorraum[i]) != 0) continue;
        const uint8_t *t = v.buf + 0x01532;
        PRUEFE(t[0] == 0x3B && t[1] == 1 && t[0x17] == 0x20 && s16(t + 16) == 0,
               "ROOM%04X Slot 1 @0x01532 -> Raum 0x%02X, next_y %d (Boden der Leichenhalle)", vorraum[i], t[0x17], (int)s16(t + 16));
        free(v.buf);
    }
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, RAEUME[i]) != 0) continue;
        double M[3]; int b = 0, h = 0;
        if (plakat_nord(RAEUME[i], M, &b, &h) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile/kein Modell", RAEUME[i]); free(r.buf); continue; }
        int32_t wz = nordwand_z(&r);
        PRUEFE(wz == -26771 && fabs(M[2] - wz) < 2.0, "ROOM%04X: Plakatflaeche an der Nordwand z %.0f (SCA-Zelle 4 @0x%05X, Kante %d)",
               RAEUME[i], M[2], (unsigned)OFF_SCA4, (int)wz);
        PRUEFE(abs(b - 2 * ALT_B) <= 4 && abs(h - 2 * ALT_H) <= 4, "ROOM%04X: Plakat %d x %d = doppelt so gross wie bisher (%d x %d)",
               RAEUME[i], b, h, ALT_B, ALT_H);
        PRUEFE(M[1] <= gem_u && M[1] >= gem_o, "ROOM%04X: Plakatmitte y %.0f liegt in BEIDEN Kopfbaendern (%d .. %d)",
               RAEUME[i], M[1], (int)gem_u, (int)gem_o);
        PRUEFE(M[1] - h / 2.0 <= oben && M[1] + h / 2.0 >= unten,
               "ROOM%04X: Plakat y %.0f..%.0f deckt beide Kopfbaender ganz (%d .. %d)", RAEUME[i], M[1] + h / 2.0,
               M[1] - h / 2.0, (int)unten, (int)oben);
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ ort */
static int rechteck_beruehrt(int32_t ax, int32_t az, int32_t aw, int32_t ad, int32_t bx, int32_t bz, int32_t bw, int32_t bd)
{
    return ax <= bx + bw && bx <= ax + aw && az <= bz + bd && bz <= az + ad;
}

static void teil_ort(void)
{
    printf("=== ort: Nordwand neben der Eingangstuer, gross in der Ankunftskamera 0, Zone ohne Ueberschneidung ===\n");
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, RAEUME[i]) != 0) continue;
        const uint8_t *t = r.buf + OFF_TUER, *s16z = r.buf + OFF_SCA16, *a1 = r.buf + OFF_AOT1, *a2 = r.buf + OFF_AOT2;
        const uint8_t *o0 = r.buf + OFF_OBJ0, *o1 = r.buf + OFF_OBJ1;
        PRUEFE(t[0] == 0x3B && t[1] == 0 && a1[0] == 0x50 && a1[1] == 1 && a2[0] == 0x2C && a2[1] == 2 && a2[2] == 3 &&
               o0[0] == 0x2D && o0[1] == 0 && o1[0] == 0x2D && o1[1] == 1 && s16(s16z + 4) == -28210,
               "ROOM%04X RDT: Door_aot_set 0 @0x%05X, Item_aot_set 1 @0x%05X, Aot_set 2 @0x%05X, obj 0/1 @0x%05X/@0x%05X, "
               "SCA 16 @0x%05X", RAEUME[i], (unsigned)OFF_TUER, (unsigned)OFF_AOT1, (unsigned)OFF_AOT2, (unsigned)OFF_OBJ0,
               (unsigned)OFF_OBJ1, (unsigned)OFF_SCA16);
        int32_t tuer_x0 = s16(t + 6);                                   /* -20890 */
        int32_t schrank_x1 = s16(s16z + 4) + (int32_t)le16(s16z + 0);   /* -28210 + 2000 = -26210 */
        double M[3]; int b = 0, h = 0;
        if (plakat_nord(RAEUME[i], M, &b, &h) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile", RAEUME[i]); free(r.buf); continue; }
        PRUEFE(M[0] + b / 2.0 < tuer_x0 && M[0] - b / 2.0 > schrank_x1,
               "ROOM%04X: Plakat x %.0f..%.0f zwischen Schrank (x %d) und Eingangstuer (x %d)", RAEUME[i], M[0] - b / 2.0,
               M[0] + b / 2.0, (int)schrank_x1, (int)tuer_x0);
        int32_t k0 = s16(o0 + 14), k1 = s16(o1 + 14);
        PRUEFE(fabs(M[2] - k0) > KUEHL_LEN && fabs(M[2] - k1) > KUEHL_LEN,
               "ROOM%04X: Plakat z %.0f weit weg von den Kuehlfaechern (obj 0 z %d, obj 1 z %d, Modell %d lang)", RAEUME[i], M[2],
               (int)k0, (int)k1, KUEHL_LEN);
        double box[4], n[3] = { 0, 0, 1 };
        int drin = im_bild(&r, 0, M, n, b, h, box);
        PRUEFE(drin && box[1] - box[0] >= 2 * ALT_BILD_B,
               "ROOM%04X Ankunftskamera 0: Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f = %.0f x %.0f Bildpunkte (mind. %d breit)",
               RAEUME[i], box[0], box[1], box[2], box[3], box[1] - box[0], box[3] - box[2], 2 * ALT_BILD_B);
        drin = im_bild(&r, 3, M, n, b, h, box);
        PRUEFE(drin, "ROOM%04X Kamera 3 (an der Aufnahme-Stelle): Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f", RAEUME[i],
               box[0], box[1], box[2], box[3]);
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(RAEUME[i]);
        int32_t zx = z->rect_x, zz = z->rect_z, R = RE15_R36K_RECT;
        int bt  = rechteck_beruehrt(zx, zz, R, R, s16(t + 6), s16(t + 8), le16(t + 10), le16(t + 12));
        int ba1 = rechteck_beruehrt(zx, zz, R, R, s16(a1 + 6), s16(a1 + 8), le16(a1 + 10), le16(a1 + 12));
        int ba2 = rechteck_beruehrt(zx, zz, R, R, s16(a2 + 6), s16(a2 + 8), le16(a2 + 10), le16(a2 + 12));
        int bk  = rechteck_beruehrt(zx, zz, R, R, RE15_R42L_X, RE15_R42L_Z, RE15_R42L_B, RE15_R42L_T);
        PRUEFE(!bt && !ba1 && !ba2 && !bk,
               "ROOM%04X: Aufhebe-Rechteck x %ld..%ld z %ld..%ld beruehrt Tuer %s, Aot 1 %s, Minidisc Aot 2 %s, Klapper-Flaeche %s",
               RAEUME[i], (long)zx, (long)zx + R, (long)zz, (long)zz + R, bt ? "JA" : "nein", ba1 ? "JA" : "nein",
               ba2 ? "JA" : "nein", bk ? "JA" : "nein");
        re15_game_state_init();
        raum_start(&r, -20154, 0, -25245);
        PRUEFE(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR &&
               g_aot.slots[1].active && g_aot.slots[1].type == RE15_AOT_TYPE_ITEM &&
               g_aot.slots[2].active && g_aot.slots[2].event_id == 3 && z->aot_slot > 3 &&
               g_aot.slots[z->aot_slot].active && g_aot.slots[z->aot_slot].event_id == RE15_R36K_EREIGNIS,
               "ROOM%04X nach dem Raumstart: Slot 0 Tuer, Slot 1 Gegenstand, Slot 2 Ereignis 3 unveraendert, Plan in Slot %d",
               RAEUME[i], z->aot_slot);
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
        pl->x = sx + 120 * k; pl->z = sz; pl->rot_y = (int16_t)rot;
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
    printf("=== druck: echter Aktionsdruck vor dem Plan an der Nordwand ===\n");
    /* (-23262,-26177) = gemessener Halt am echten Weg von der Tuer (Lauf t4, Bild F210, Kamera 3); (-23550,-26177) = vor der
     * Plakatmitte. rot 1024 = Blick -z = Nordwand (Vorwaertspunkt 620 voraus @0x80042bd0). */
    static const int32_t stand[2][2] = { { -23262, -26177 }, { -23550, -26177 } };
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, RAEUME[i]) != 0) continue;
        for (int s = 0; s < 2; s++) {
            int kl = 0;
            int ev = druck(&r, stand[s][0], stand[s][1], 1024, &kl);
            PRUEFE(kl == 0, "ROOM%04X (%ld,%ld): Klettersonde an 5 Standorten -> %d (0 = Druck bleibt fuer den Scan)",
                   RAEUME[i], (long)stand[s][0], (long)stand[s][1], kl);
            PRUEFE(ev == RE15_R36K_EREIGNIS, "ROOM%04X (%ld,%ld) Blick Nordwand -> Ereignis %d (soll %d)", RAEUME[i],
                   (long)stand[s][0], (long)stand[s][1], ev, RE15_R36K_EREIGNIS);
        }
        int kl = 0;
        int ev = druck(&r, -20045, -26177, 1024, &kl);   /* vor der Eingangstuer (Slot 0 x -20890..-19200) */
        PRUEFE(ev != RE15_R36K_EREIGNIS, "ROOM%04X (-20045,-26177) vor der Tuer -> Ereignis %d (nicht der Plan)", RAEUME[i], ev);
        free(r.buf);
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("=== Strang kellerplan: Kellerplan 1200/1201 doppelt so gross, Augenhoehe (Teil %s) ===\n", teil);
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "hoehe")) teil_hoehe();
    if (alle || !strcmp(teil, "ort"))   teil_ort();
    if (alle || !strcmp(teil, "druck")) teil_druck();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
