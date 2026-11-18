/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 3 (Plan S5, K-13): Wurfszene. Nutzerwortlaut, Ablauf und alle Konstanten mit
 * Adressen: include/re15_finale_wurf_r42.h. Adressen @0x0xxxx = Datei-Offset in RE2 ROOM7020.RDT, @0x800xxxxx = RE2-Leon-EXE bzw.
 * (Wurf-Clip, Loslassbild) RE1.5-PSX.EXE, wie im Kopf angegeben.
 *
 * ⛔ PORT-ERGAENZUNG nach Nutzerentscheid, KEIN Auslieferungscode (der Mod-Baum hat kein Finale mit Raketenwerfer).
 */
#include "re15_finale_wurf_r42.h"
#include "re15_finale_r42.h"
#include "re15_story_r42.h"       /* RE15_R42_BIT_WERFER_FINALE */

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

#include "re15_actor.h"
#include "re15_scd.h"             /* g_scd, scd_thread_start, re15_game_flag_*, SCD_EVENT_SLOT_* */
#include "re15_enemy.h"           /* re15_enemy_find */
#include "re15_skeleton.h"        /* re15_skel_compute_pose, re15_skel_euler_matrix, re15_sin_q12/cos */
#include "re15_anim_select.h"     /* re15_actor_playback_slot */
#include "re15_emd.h"
#include "re15_engine.h"
#include "re15_inventory.h"       /* g_inv, re15_inv_compact */
#include "re15_aot.h"             /* g_aot (Messzeile Aufnahme-AOT) */
#include "re15_math.h"            /* re15_squareroot0 */
#include "re15_room.h"            /* g_room_rdt (Kameras der Schnitte) */
#include "re15_camera.h"          /* re15_camera_build_view */
#include "re15_esp.h"             /* re15_esp_fx_get (Messzeile Landestelle) */
#include "re15_door_seq.h"        /* re15_door_seq_zuordnen_flaeche (Tuerton beim Abgang) */

/* PC: Tuerton der Tuer, durch die John geht (platform/pc/src/door_scene_pc.c re15_pc_tuer_ton_vorspielen); NULL = stumm (PSX, Riegel). */
static int (*s_tuer_ton)(const re15_door_seq_anfrage_t *q);
void re15_finale_wurf_tuer_ton_hook(int (*fn)(const re15_door_seq_anfrage_t *q)) { s_tuer_ton = fn; }

extern unsigned g_current_room_id;
/* finale_r42.c */
int32_t re15_finale_r42_peilung(int32_t x0, int32_t z0, int32_t x1, int32_t z1);
void    re15_finale_r42_wurf_protokoll(const char *fmt, int a, int b, int c);
void    re15_finale_r42_se(int satz);

#define LOG(f, a, b, c) re15_finale_r42_wurf_protokoll("Wurf: " f, (int)(a), (int)(b), (int)(c))

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c; Vorbild je Zeile re15_kanister2080_r41.c Kopf) -------------- */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_MOTION(s,c)     0x3f, (s), (c), 0x00
#define OP_MSG(id)         0x2b, (id), 0x00, 0x00
#define OP_POLL(bit)       0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SIG(bit)        OP_SET(5, (bit), 1)
#define OP_DEST(m,bit,x,z) 0x40, 0x00, (m), (bit), LE16(x), LE16(z)
#define OP_END             0x01, 0x00

static struct {
    int      aktiv;
    int      form, john;
    int      w;                       /* RE15_FW_W_* */
    int32_t  pos[3], v[3], a[3];
    int      flug_n;
    int16_t  john_gier, flug_gier;
    int32_t  ziel_x, ziel_z;          /* Landestelle */
    /* gemerkt @0x0182A-0x0184A */
    int      cut;
    int32_t  lx, lz; int16_t lg;
    int32_t  fx, fz; int16_t fg;
    int16_t  f_motion; uint32_t f_frame; uint8_t f_frac;
    int      form_versteckt;
    int      bild0;                   /* 1 = erstes Programmbild gesehen (W) */
    int      flug_schnitt;            /* 1 = waehrend des Flugs auf die Kamera des Spielers geschnitten */
    int      abgang, abgang_t;        /* John geht ab (Takt des Reaktions-Signals) */
    int32_t  jx_alt, jz_alt; int gegangen;
    int      john_weg;
    int      an_tuer;                 /* John ist an der Tuer angekommen (Signal an das Programm, noch NICHT geparkt) */
    int      parken_offen;            /* RE2 @0x01954: parken, sobald ihn das aktuelle Bild nicht zeigt */
    int32_t  start[3];                /* Loslasslage (Schnitt im Flug nach einer Werferlaenge) */
    int      prog_n, item_at;
    unsigned takt, t0;
    int      z_an, z_aus;             /* Messzeile: Zuruf stand von/bis (Takt) */
    int      log_genommen;
    uint8_t  prog[192];
} s_w = { .form = -1, .john = -1 };

void re15_finale_wurf_reset(void)
{
    memset(&s_w, 0, sizeof s_w);
    s_w.form = -1; s_w.john = -1; s_w.z_an = s_w.z_aus = -1;
}

int re15_finale_wurf_aktiv(void) { return s_w.aktiv; }
int re15_finale_wurf_besitzt(int slot) { return s_w.aktiv && slot == s_w.form; }
int re15_finale_wurf_werfer_zustand(void) { return s_w.w; }

static int raum(void) { const unsigned r = g_current_room_id & 0xFFFFu; return r == 0x5090u || r == 0x5091u; }

const char *re15_finale_wurf_emd_datei(uint8_t type)
{
    return (type == RE15_FW_JOHN_TYP && raum()) ? RE15_FW_EMD_DATEI : NULL;
}

static int sig(uint8_t b) { return re15_game_flag_get(5, b) != 0; }
static void sig_setzen(uint8_t b, int v) { re15_game_flag_set(5, b, v); }

/* RotMatrixY wie re15_skel_bone_to_world: x' = c x + s z, z' = -s x + c z (Q12). */
static void dreh_y(int16_t gier, int32_t x, int32_t z, int32_t *ox, int32_t *oz)
{
    const int32_t c = re15_cos_q12(gier & 0xfff), s = re15_sin_q12(gier & 0xfff);
    *ox = (int32_t)(((int64_t)x * c + (int64_t)z * s) >> 12);
    *oz = (int32_t)((-(int64_t)x * s + (int64_t)z * c) >> 12);
}
static void mat_mul(const int32_t a[9], const int32_t b[9], int32_t o[9])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            o[r * 3 + c] = (int32_t)(((int64_t)a[r * 3] * b[c] + (int64_t)a[r * 3 + 1] * b[3 + c] + (int64_t)a[r * 3 + 2] * b[6 + c]) >> 12);
}
static void mat_y(int16_t gier, int32_t m[9])
{
    const int32_t c = re15_cos_q12(gier & 0xfff), s = re15_sin_q12(gier & 0xfff);
    m[0] = c; m[1] = 0; m[2] = s; m[3] = 0; m[4] = 4096; m[5] = 0; m[6] = -s; m[7] = 0; m[8] = c;
}

static re15_actor_t *john(void)
{
    if (s_w.john > 0 && s_w.john < RE15_ACTOR_MAX && g_actors[s_w.john].active && g_actors[s_w.john].type == RE15_FW_JOHN_TYP)
        return &g_actors[s_w.john];
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_FW_JOHN_TYP) { s_w.john = s; return &g_actors[s]; }
    return NULL;
}

/* Weltpunkt im Bild des Schnitts `cut` (RID-Satz wie der Renderer, re15_camera_build_view = FUN_80076cb0-Nachbau, Bild 320x240,
 * u = 160 + H x / z, v = 120 + H y / z). Liefert 1 = im Bild; u/v (relativ, +-1 = Bildrand) fuer die Messzeile. */
static int im_bild(int cut, int32_t x, int32_t y, int32_t z, int *u_rel, int *v_rel)
{
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[cut], &v) != 0) return 0;
    const int64_t vx = (((int64_t)v.rot[0] * x + (int64_t)v.rot[1] * y + (int64_t)v.rot[2] * z) >> 12) + v.trans[0];
    const int64_t vy = (((int64_t)v.rot[3] * x + (int64_t)v.rot[4] * y + (int64_t)v.rot[5] * z) >> 12) + v.trans[1];
    const int64_t vz = (((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) >> 12) + v.trans[2];
    if (vz <= 50) return 0;
    const int64_t u = (int64_t)v.fov_screen_dist * vx / vz, w = (int64_t)v.fov_screen_dist * vy / vz;
    if (u_rel) *u_rel = (int)(u * 1000 / 160);
    if (v_rel) *v_rel = (int)(w * 1000 / 120);
    return u >= -160 && u <= 160 && w >= -120 && w <= 120;
}

/* Teil 11 (rechte Hand) im Bild `bild` des laufenden Clips (Paar 1 bei Sub 1, sonst Paar 2): Weltlage + Weltdrehung der Teil-Matrix =
 * der Rahmen, in dem das PL00W12-Waffenband liegt (Kopf RE15_FW_TEIL: Leons Griff). 0 = keine Bank. */
static int hand_welt(const re15_actor_t *n, int bild, int32_t pos[3], int32_t rot[9])
{
    const re15_enemy_bank_t *bk = re15_enemy_find(n->type);
    if (!bk || !bk->ok) return 0;
    const re15_emd_skeleton_t  *sk = (n->state == 4 && n->sub_state_1 == 1 && bk->loco_ok) ? &bk->skel_loco : &bk->skel;
    const re15_emd_animation_t *an = (n->state == 4 && n->sub_state_1 == 1 && bk->loco_ok) ? &bk->anim_loco : &bk->anim;
    const int c = (int)n->motion;
    if (c < 0 || c >= an->clip_count || an->clips[c].frame_count <= 0 || sk->bone_count <= RE15_FW_TEIL) return 0;
    int f = bild;
    if (f >= an->clips[c].frame_count) f = an->clips[c].frame_count - 1;
    if (f < 0) f = 0;
    f = re15_actor_playback_slot(n, (uint32_t)f, an->clips[c].frame_count);
    const int kf = (int)(an->frames[an->clips[c].first_frame + f] & 0xfffu);
    re15_skel_pose_t p[RE15_EMD_MAX_BONES];
    void *pa = g_anim_pose_actor; g_anim_pose_actor = NULL;      /* Abfrage ohne Ueberblend-Zustand (re15_skeleton.h) */
    const int rv = re15_skel_compute_pose(sk, kf, p);
    g_anim_pose_actor = pa;
    if (rv != 0) return 0;
    const re15_skel_pose_t *t = &p[RE15_FW_TEIL];
    re15_skel_bone_to_world(t->trans, n->rot_y, n->x, n->y, n->z, pos);
    if (rot) {
        int32_t my[9];
        mat_y(n->rot_y, my);
        mat_mul(my, t->rot, rot);                                      /* wie main.c WEAPON-IN-HAND: Teil-11-Matrix, keine Zusatzdrehung */
    }
    return 1;
}

/* RE2-Drehung (rx, ry, rz) fuer Adas Gier 3700 und das R702-Modell, im Port um (Gier John - 3700) um die Welt-Hochachse gedreht und
 * ueber die Achsenzuordnung W12 -> R702 (Kopf RE15_FW_TEIL: W12-y -> R702-x, W12-x -> R702-y, W12-z -> -R702-z) auf das W12-Modell. */
static const int32_t s_w12_nach_r702[9] = { 0, 4096, 0,   4096, 0, 0,   0, 0, -4096 };
static void dreh_aus_re2(int rx, int ry, int rz, int32_t rot[9])
{
    int32_t e[9], my[9], t[9];
    re15_skel_euler_matrix(rx, ry, rz, e);
    mat_y((int16_t)((s_w.john_gier - RE15_FW_ADA_GIER) & 0xfff), my);
    mat_mul(my, e, t);
    mat_mul(t, s_w12_nach_r702, rot);
}

int re15_finale_wurf_werfer_lage(int32_t pos[3], int32_t rot[9])
{
    if (!raum()) return 0;
    switch (s_w.w) {
    case RE15_FW_W_HAND: {
        const re15_actor_t *n = john();
        return (n && !n->no_draw) ? hand_welt(n, (int)n->anim_frame, pos, rot) : 0;
    }
    case RE15_FW_W_FLUG:
        memcpy(pos, s_w.pos, sizeof s_w.pos);
        dreh_aus_re2(RE15_FW_FLUG_RX, RE15_FW_FLUG_RY, RE15_FW_FLUG_RZ, rot);   /* Super_reset @0x018B6 */
        return 1;
    case RE15_FW_W_LIEGT:
        if (re15_game_flag_get(9, RE15_R42_BIT_WERFER_FINALE)) return 0;
        memcpy(pos, s_w.pos, sizeof s_w.pos);
        dreh_aus_re2(RE15_FW_LIEG_RX, 0, RE15_FW_LIEG_RZ, rot);                  /* Dir_set @0x019FA */
        return 1;
    default: return 0;
    }
}

/* ---- Flug sub13 ---------------------------------------------------------------------------------------------------------- */
static int flug_schritt(int32_t p[3], int32_t v[3], const int32_t a[3])
{
    for (int i = 0; i < 3; i++) p[i] += v[i];                         /* Add_speed @0x80055ab0 (Weltachsen @0x80055abc-e0) */
    for (int i = 0; i < 3; i++) v[i] += a[i];                         /* Add_aspeed @0x80055b2c */
    return p[1] < -RE15_FW_FLUG_STOP;                                 /* Edwhile Member_cmp(y < -800) @0x019D8: 1 = weiter */
}
void re15_finale_wurf_bahn(const int32_t start[3], const int32_t v0[3], const int32_t a[3], int32_t ende[3], int *bilder)
{
    int32_t p[3], v[3];
    memcpy(p, start, sizeof p); memcpy(v, v0, sizeof v);
    int n = 0;
    while (n < RE15_FW_FLUG_MAX) { n++; if (!flug_schritt(p, v, a)) break; }
    if (ende) memcpy(ende, p, sizeof p);
    if (bilder) *bilder = n;
}
/* v_y (Kopf "Flug sub13", PORT-WAHL): ab der RE2-Startgeschwindigkeit +20 abwaerts zaehlen, bis die RE2-Schleife (Lage += v, v += 20,
 * Ende y >= -800 @0x019D8) ab y0 genau RE15_FW_FLUG_BILDER Bilder dauert. */
static int flug_dauer(int32_t y0, int32_t vy)
{
    int32_t p = y0, v = vy;
    int n = 0;
    while (n < RE15_FW_FLUG_MAX) { n++; p += v; v += RE15_FW_FLUG_AY; if (p >= -RE15_FW_FLUG_STOP) break; }
    return n;
}
int32_t re15_finale_wurf_vy(int32_t y0)
{
    for (int32_t vy = RE15_FW_FLUG_VY; vy > -4000; vy--)
        if (flug_dauer(y0, vy) == RE15_FW_FLUG_BILDER) return vy;
    return RE15_FW_FLUG_VY;                                           /* nicht erreichbar - RE2-Wert */
}
static int32_t rdiv(int32_t a, int32_t b) { return (a >= 0) ? (a + b / 2) / b : -((-a + b / 2) / b); }
void re15_finale_wurf_startwerte(const int32_t start[3], int32_t ziel_x, int32_t ziel_z, int32_t v[3], int32_t a[3], int16_t *gier)
{
    const int32_t dx = ziel_x - start[0], dz = ziel_z - start[2];
    const int32_t d = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
    const int32_t n = RE15_FW_FLUG_BILDER, s = n * (n - 1) / 2;      /* 28 Bilder: Weg = n v + a n(n-1)/2, n(n-1)/2 = 378 */
    if (d >= RE15_FW_FLUG_AX * s) { a[0] = d ? RE15_FW_FLUG_AX * dx / d : 0; a[2] = d ? RE15_FW_FLUG_AX * dz / d : 0; }
    else                          { a[0] = dx / s; a[2] = dz / s; }
    a[1] = RE15_FW_FLUG_AY;
    v[0] = rdiv(dx - a[0] * s, n);
    v[2] = rdiv(dz - a[2] * s, n);
    v[1] = re15_finale_wurf_vy(start[1]);
    if (gier) *gier = (int16_t)re15_finale_r42_peilung(start[0], start[2], ziel_x, ziel_z);
}
void re15_finale_wurf_landestelle(int32_t px, int32_t pz, int32_t jx, int32_t jz, int32_t *lx, int32_t *lz)
{
    const int32_t dx = jx - px, dz = jz - pz;
    const int32_t d = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
    /* der Spieler steht im Rechteck (halbe Masse 1700 x 1250) um die Landestelle: |k ux| <= 1700 - Rand, |k uz| <= 1250 - Rand */
    const int32_t adx = dx < 0 ? -dx : dx, adz = dz < 0 ? -dz : dz;
    int32_t k = d / 2;
    if (adx) { const int32_t kx = (int32_t)((int64_t)(RE15_FW_AOT_W / 2 - RE15_FW_LANDUNG_RAND) * d / adx); if (kx < k) k = kx; }
    if (adz) { const int32_t kz = (int32_t)((int64_t)(RE15_FW_AOT_D / 2 - RE15_FW_LANDUNG_RAND) * d / adz); if (kz < k) k = kz; }
    *lx = d ? px + (int32_t)((int64_t)dx * k / d) : px;
    *lz = d ? pz + (int32_t)((int64_t)dz * k / d) : pz;
}

/* ---- Programm (RE2 sub12 in RE1.5-Opcodes) ------------------------------------------------------------------------------ */
#define PUT(...) do { const uint8_t _b[] = { __VA_ARGS__ }; \
    if (n + (int)sizeof _b <= (int)sizeof s_w.prog) { memcpy(s_w.prog + n, _b, sizeof _b); n += (int)sizeof _b; } } while (0)
static void programm_bauen(void)
{
    int n = 0;
    PUT(OP_SET(2, 7, 1), OP_SET(1, 27, 1));                          /* @0x0181A / @0x0181E */
    PUT(OP_CUT(RE15_FW_CUT_WURF));                                   /* Cut_chg 7 @0x0184E -> Schnitt 13 (Kopf) */
    PUT(0x44, RE15_FW_JOHN_SLOT, RE15_FW_JOHN_TYP, RE15_FW_JOHN_GRID, 0x00, 0x00, 0x00, 0xff,
        LE16(RE15_FW_JOHN_X), LE16(0), LE16(RE15_FW_JOHN_Z), 0x00, 0x00, LE16(s_w.john_gier), 0x00, 0x00);
                                                                     /* Sce_em_set (Form ROOM2001 @0x0179E) an die Ankunft @0x0108E */
    PUT(OP_WORK(2, RE15_FW_JOHN_SLOT));                              /* Work_set(2 = Ada) @0x01864 */
    PUT(OP_MOTION(1, RE15_FW_CLIP_HEBEN));                           /* Plc_motion @0x01878 -> HEBEN (W09 Clip 6) */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_1));                                 /* @0x0187C */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_30));                                /* @0x01892 (RE2 ruft vorher, Xa_on 0 @0x01888 - Port: beim Loslassen) */
    PUT(OP_MOTION(1, RE15_FW_CLIP_WURF));                            /* @0x018AA -> WURF (W09 Clip 7) */
    PUT(OP_POLL(RE15_FW_SIG_WURF));                                  /* Loslassen im Clip-Bild 22 (@0x800336fc statt Sleep 20 @0x018AE) */
    PUT(OP_MSG(RE15_FW_MSG_ZURUF));                                  /* PORT-WAHL "Ruf und Untertitel beim Loslassen" (Kopf) */
    PUT(OP_POLL(RE15_FW_SIG_GELANDET));                              /* Do .. Edwhile Ck(5,0x0E,0) @0x018C4-0x018CC */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_15));                                /* @0x018D0 */
    PUT(OP_SIG(RE15_FW_SIG_REAKTION));                               /* Dir_set @0x018E0 (Takt) */
    PUT(OP_CUT(RE15_FW_CUT_REAKTION));                               /* Cut_chg 11 @0x018FC -> 13 */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_1));                                 /* @0x018FE */
    PUT(OP_DEST(RE15_FW_GEHEN_MODUS, RE15_FW_SIG_TUER, RE15_FW_TUER_X, RE15_FW_TUER_Z));   /* Plc_dest @0x0193A: John laeuft zur Tuer */
    PUT(OP_POLL(RE15_FW_SIG_TUER));                                  /* PORT-WAHL statt Wsleep @0x01942: bis er an der Tuer ist */
    PUT(OP_CUT((uint8_t)s_w.cut));                                   /* Work_copy + Cut_chg @0x01944/@0x01948: gemerkte Kamera, VOR dem Parken */
    s_w.item_at = n;                                                 /* Aot_reset 5 @0x0194A: Werfer aufhebbar (Item_aot_set, Kopf) */
    PUT(0x50, RE15_FW_AOT_SLOT, 0x09, 0x31, 0x00, 0x00, LE16(0), LE16(0), LE16(RE15_FW_AOT_W), LE16(RE15_FW_AOT_D),
        LE16(RE15_FW_ITEM), LE16(RE15_FW_ANZAHL), LE16(RE15_R42_BIT_WERFER_FINALE), 0xff, 0x00);
    PUT(OP_SIG(RE15_FW_SIG_ENDE));                                   /* John parken @0x01950-0x01960 + @0x01968-0x019A6 (Takt) */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_1));                                 /* @0x019AA */
    PUT(OP_WORK(1, 0), 0x42, 0x00);                                  /* sub18 Work_set + Plc_ret @0x01B54/@0x01B57 */
    PUT(OP_SET(2, 7, 0), OP_SET(1, 27, 0));                          /* @0x01B58 / @0x01B5C */
    PUT(0x3c, 0x01);                                                 /* Cut_auto 1 @0x01B60 */
    PUT(OP_END);
    s_w.prog_n = n;
}
static void item_lage_setzen(int32_t x, int32_t z)
{
    uint8_t *p = s_w.prog + s_w.item_at;
    const int32_t x0 = x - RE15_FW_AOT_W / 2, z0 = z - RE15_FW_AOT_D / 2;
    p[6] = (uint8_t)(x0 & 0xff); p[7] = (uint8_t)((x0 >> 8) & 0xff);
    p[8] = (uint8_t)(z0 & 0xff); p[9] = (uint8_t)((z0 >> 8) & 0xff);
}

const uint8_t *re15_finale_wurf_programm(int *out_len)
{
    if (!s_w.prog_n) {                                               /* Riegel: gemerkte Kamera und Gier wie der Start */
        s_w.cut = (int)g_scd.work_vars[0x0A];
        if (!s_w.john_gier) s_w.john_gier = RE15_FW_JOHN_GIER;
        programm_bauen();
    }
    if (out_len) *out_len = s_w.prog_n;
    return s_w.prog;
}

int re15_finale_wurf_start(int form_slot)
{
    if (s_w.aktiv || s_w.w != RE15_FW_W_AUS || form_slot < 0 || form_slot >= RE15_ACTOR_MAX) return -1;
    re15_actor_t *e = &g_actors[form_slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_w.form = form_slot; s_w.john = -1;
    /* PORT-WAHL (Kopf): John blickt beim Erscheinen zum Spieler */
    s_w.john_gier = (int16_t)re15_finale_r42_peilung(RE15_FW_JOHN_X, RE15_FW_JOHN_Z, pl->x, pl->z);
    /* Merken @0x0182A-0x0184A: Kamera (Var 4 <- Var 0x1A), Leon x/z/Richtung (Var 5/6/7), Tyrant (Var 8/9/10) */
    s_w.cut = (int)g_scd.work_vars[0x0A];
    s_w.lx = pl->x; s_w.lz = pl->z; s_w.lg = pl->rot_y;
    s_w.fx = e->x;  s_w.fz = e->z;  s_w.fg = e->rot_y;
    s_w.f_motion = e->motion; s_w.f_frame = e->anim_frame; s_w.f_frac = e->anim_frac;
    s_w.z_an = s_w.z_aus = -1;
    s_w.bild0 = 0; s_w.flug_schnitt = 0; s_w.abgang = 0; s_w.john_weg = 0; s_w.an_tuer = 0; s_w.parken_offen = 0;
    static const uint8_t sigs[] = { RE15_FW_SIG_WURF, RE15_FW_SIG_GELANDET, RE15_FW_SIG_REAKTION, RE15_FW_SIG_ENDE, RE15_FW_SIG_TUER };
    for (unsigned i = 0; i < sizeof sigs; i++) sig_setzen(sigs[i], 0);
    programm_bauen();
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, s_w.prog) == 0) {
            s_w.aktiv = 1; s_w.w = RE15_FW_W_HAND; s_w.t0 = s_w.takt;
            /* Form ausser Bild (Kopf: RE2 zeigt den Tyrant in keinem Szenenbild) */
            if (!e->no_draw) { e->no_draw = 1; s_w.form_versteckt = 1; }
            LOG("Szene startet (Programm Slot %d, %d Bytes), Kamera vorher %d", slot, s_w.prog_n, s_w.cut);
            LOG("Leon (%d,%d) Gier %d gemerkt", s_w.lx, s_w.lz, s_w.lg);
            LOG("John-Gier %d (Peilung zum Spieler), Form unsichtbar", s_w.john_gier, 0, 0);
            return 0;
        }
    LOG("kein freier Ereignis-Slot - Szene nicht gestartet", 0, 0, 0);
    return -1;
}

/* Form ruht: RE2 ueberspringt ihre Typroutine (+0x10E = 0x8000 @0x01822-26, @0x80026590-9c) - keine KI, kein Bildvorschub. Der
 * Port-Animator laeuft getrennt von der KI (game_step_common.c), darum haelt der Takt Clip und Bild selbst. */
static void form_ruht(re15_actor_t *e)
{
    e->motion = s_w.f_motion; e->anim_frame = s_w.f_frame; e->anim_frac = s_w.f_frac;
}

static void zuruf_messen(void)
{
    const int zeile = g_scd.message_active != 0;
    if (zeile && g_scd.message_id == RE15_FW_MSG_ZURUF && s_w.z_an < 0) {
        s_w.z_an = (int)s_w.takt;
        LOG("Zuruf an (Id %d) bei W+%d", RE15_FW_MSG_ZURUF, (int)(s_w.takt - s_w.t0), 0);
    }
    if (!zeile && s_w.z_an >= 0 && s_w.z_aus < 0) {
        s_w.z_aus = (int)s_w.takt;
        LOG("Zuruf aus nach %d Bildern", s_w.z_aus - s_w.z_an, 0, 0);
    }
}

/* Loslassen im Clip-Bild 22 (Kopf RE15_FW_LOS_BILD, RE1.5 @0x800336fc): der Spielzug zeigt dieses Bild als naechstes (der Animator
 * schaltet nach dem Takt weiter), darum gilt die Handlage des Bilds anim_frame + 1. */
static int loslassen_faellig(const re15_actor_t *n)
{
    return n && n->motion == RE15_FW_CLIP_WURF && (int)n->anim_frame + 1 >= RE15_FW_LOS_BILD;
}
static void loslassen(void)
{
    re15_actor_t *n = john();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t p0[3];
    const int bild = n ? (int)n->anim_frame + 1 : -1;
    if (!n || !hand_welt(n, bild, p0, NULL)) {
        p0[0] = RE15_FW_JOHN_X; p0[1] = -1500; p0[2] = RE15_FW_JOHN_Z;  /* ohne Bank (sollte nicht vorkommen): Johns Lage */
        LOG("Loslassen OHNE Bank - Werfer startet an Johns Lage", 0, 0, 0);
    }
    re15_finale_wurf_landestelle(pl->x, pl->z, RE15_FW_JOHN_X, RE15_FW_JOHN_Z, &s_w.ziel_x, &s_w.ziel_z);
    memcpy(s_w.pos, p0, sizeof s_w.pos);                             /* Super_reset @0x018B6: Weltlage der Hand */
    memcpy(s_w.start, p0, sizeof s_w.start);
    re15_finale_wurf_startwerte(s_w.pos, s_w.ziel_x, s_w.ziel_z, s_w.v, s_w.a, &s_w.flug_gier);
    s_w.flug_n = 0;
    s_w.w = RE15_FW_W_FLUG;
    sig_setzen(RE15_FW_SIG_WURF, 1);
    LOG("Loslassen im Clip-Bild %d bei W+%d", bild, (int)(s_w.takt - s_w.t0), 0);
    LOG("Start (%d,%d,%d)", s_w.pos[0], s_w.pos[1], s_w.pos[2]);
    LOG("Ziel neben dem Spieler (%d,%d), Spieler x %d", s_w.ziel_x, s_w.ziel_z, pl->x);
    LOG("v (%d,%d,%d)", s_w.v[0], s_w.v[1], s_w.v[2]);
    LOG("a (%d,%d,%d)", s_w.a[0], s_w.a[1], s_w.a[2]);
}

/* Kamera der Szene: wie Cut_chg (Zonen aus; Muster finale_r42.c kamera). */
static void kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

static void flug_takt(void)
{
    s_w.flug_n++;
    const int weiter = flug_schritt(s_w.pos, s_w.v, s_w.a);
    {   /* Messzeile: Bildlage im Wurf-Schnitt und in der Kamera des Spielers */
        int u = 0, v = 0, u2 = 0, v2 = 0;
        const int i13 = im_bild(RE15_FW_CUT_WURF, s_w.pos[0], s_w.pos[1], s_w.pos[2], &u, &v);
        const int ic = im_bild(s_w.cut, s_w.pos[0], s_w.pos[1], s_w.pos[2], &u2, &v2);
        LOG("Flug %d: Lage (%d,%d)", s_w.flug_n, s_w.pos[0], s_w.pos[2]);
        LOG("Flug %d: y %d, Schnitt 13 Bild-y/1000 %d", s_w.flug_n, s_w.pos[1], i13 ? v : -9999);
        LOG("Flug %d: im Bild Schnitt 13 %d, Kamera des Spielers %d", s_w.flug_n, i13, ic);
        (void)u; (void)u2; (void)v2;
        /* PORT-WAHL (Kopf "Ablauf", Runde 3): Schnitt auf die Kamera des Spielers, sobald der Werfer eine Werferlaenge (PL00W12-Rohr,
         * RE15_FW_W12_LAENGE) von der Loslasslage weg ist - er hat Johns Hand sichtbar verlassen. Runde 2 schnitt erst, als er im Bild
         * des Spielers war (Flugbild 14): 13 Bilder + Standbild des Schnitts hing er im Schnitt 13 ueber Johns Kopf (Pruefbericht s3 r2,
         * Rot 2). Der Schnitt kostet das gehaltene Bild + 6 Standbilder (kamerahalt_r41.c KH_STANDBILDER, @0x80021d2c), die Logik steht
         * dabei mit. */
        const int64_t ddx = s_w.pos[0] - s_w.start[0], ddy = s_w.pos[1] - s_w.start[1], ddz = s_w.pos[2] - s_w.start[2];
        const int32_t weg = (int32_t)re15_squareroot0((uint32_t)(ddx * ddx + ddy * ddy + ddz * ddz));
        if (!s_w.flug_schnitt && s_w.cut != RE15_FW_CUT_WURF && weg >= RE15_FW_W12_LAENGE) {
            s_w.flug_schnitt = 1;
            kamera(s_w.cut);
            LOG("Schnitt auf die Kamera des Spielers %d im Flugbild %d", s_w.cut, s_w.flug_n, 0);
            LOG("Schnitt: Werfer %d von der Loslasslage (Werferlaenge %d), im Bild des Spielers %d", weg, RE15_FW_W12_LAENGE, ic);
        }
    }
    if (weiter && s_w.flug_n < RE15_FW_FLUG_MAX) return;
    /* Landung @0x019DE-0x01A02: Set(5,0x0E,1), Se_on Raumbank 0x0E, Pos_set, Dir_set */
    s_w.pos[1] = 0;
    s_w.w = RE15_FW_W_LIEGT;
    item_lage_setzen(s_w.pos[0], s_w.pos[2]);
    re15_finale_r42_se(RE15_FW_SE_LANDUNG);
    sig_setzen(RE15_FW_SIG_GELANDET, 1);
    if (!s_w.flug_schnitt && s_w.cut != RE15_FW_CUT_WURF) {
        s_w.flug_schnitt = 1; kamera(s_w.cut);
        LOG("Schnitt auf die Kamera des Spielers %d bei der Landung", s_w.cut, 0, 0);
    }
    {
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int32_t dx = s_w.pos[0] - pl->x, dz = s_w.pos[2] - pl->z;
        LOG("Landung nach %d Flugbildern bei (%d,%d)", s_w.flug_n, s_w.pos[0], s_w.pos[2]);
        LOG("Landung: Abstand zum Spieler %d", (int)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz)), 0, 0);
        int u = 0, v = 0;
        const int ib = im_bild(s_w.cut, s_w.pos[0], -100, s_w.pos[2], &u, &v);
        LOG("Landung im Bild der Kamera %d: %d, Bild-x/1000 %d", s_w.cut, ib, u);
        LOG("Landung: Bild-y/1000 %d; Spieler Bild-x/1000 folgt", v, 0, 0);
        if (im_bild(s_w.cut, pl->x, -100, pl->z, &u, &v)) LOG("Landung: Spieler Fuesse Bild-x/1000 %d Bild-y/1000 %d", u, v, 0);
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {                   /* Messzeile: Effekte (Flammen) um die Landestelle */
            const re15_esp_fx_t *fx = re15_esp_fx_get(i);
            if (!fx) continue;
            const int32_t ex = fx->x - s_w.pos[0], ez = fx->z - s_w.pos[2];
            if (ex > -3000 && ex < 3000 && ez > -3000 && ez < 3000)
                LOG("Landung: Effekt-Platz %d bei (%d,%d) relativ", i, ex, ez);
        }
    }
}

static void reaktion(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)((re15_finale_r42_peilung(pl->x, pl->z, RE15_FW_JOHN_X, RE15_FW_JOHN_Z) + RE15_FW_LEON_REL) & 0xfff);
    s_w.abgang = 1; s_w.abgang_t = (int)s_w.takt; s_w.gegangen = 0;
    { const re15_actor_t *n = john(); if (n) { s_w.jx_alt = n->x; s_w.jz_alt = n->z; } }
    LOG("Reaktion: Spieler (%d,%d) blickt zu John, Gier %d", pl->x, pl->z, pl->rot_y);
}

/* John sichtbar im Schnitt `cut`? Quader RE15_FW_JOHN_HUELLE_* um seine Lage, 8 Ecken; eine Ecke im Bild = sichtbar. */
static int john_sichtbar(const re15_actor_t *n, int cut)
{
    for (int i = 0; i < 8; i++) {
        const int32_t x = n->x + ((i & 1) ? RE15_FW_JOHN_HUELLE_XZ : -RE15_FW_JOHN_HUELLE_XZ);
        const int32_t z = n->z + ((i & 2) ? RE15_FW_JOHN_HUELLE_XZ : -RE15_FW_JOHN_HUELLE_XZ);
        const int32_t y = (i & 4) ? -RE15_FW_JOHN_HUELLE_Y : 0;
        if (im_bild(cut, x, y, z, NULL, NULL)) return 1;
    }
    return 0;
}

/* RE2 @0x01954-0x01960: Ada parken (hier: John aus, nicht gezeichnet). */
static void john_parken(re15_actor_t *n, const char *warum)
{
    LOG("John geparkt bei (%d,%d)", n->x, n->z, 0);
    re15_finale_r42_wurf_protokoll(warum, (int)g_scd.cam_id, 0, 0);
    n->active = 0; n->no_draw = 1;
    s_w.john_weg = 1; s_w.parken_offen = 0;
}

/* John laeuft zur Tuer (Kopf "Abgang"): Ankunft = Plc_dest-Bit (Lauf-Sub 5 < 300 @0x80051764), die Wand vor der Tuer haelt ihn (kein
 * Fortschritt mehr in Blickrichtung, Wandklemme wie der Spieler) oder der Punkt 620 voraus (`ori 0x26c` @0x80042bd0, FUN_80042bac) liegt
 * im Tuer-AOT (FUN_80042b64). Dann NUR das Signal an das Programm - geparkt wird erst nach dem Schnitt zurueck (RE2 @0x01948 vor
 * @0x01954), in ende(). */
static void abgang_takt(void)
{
    re15_actor_t *n = john();
    if (!n || s_w.john_weg || s_w.an_tuer) return;
    const int32_t c = re15_cos_q12(n->rot_y & 0xfff), sn = re15_sin_q12(n->rot_y & 0xfff);
    const int32_t vx = n->x + ((620 * c) >> 12), vz = n->z - ((620 * sn) >> 12);
    const int vorn_in_tuer = vx >= RE15_FW_TUER_X0 && vx <= RE15_FW_TUER_X0 + RE15_FW_TUER_W &&
                             vz >= RE15_FW_TUER_Z0 && vz <= RE15_FW_TUER_Z0 + RE15_FW_TUER_D;
    const int im_aot = n->x >= RE15_FW_TUER_X0 && n->x <= RE15_FW_TUER_X0 + RE15_FW_TUER_W &&
                       n->z >= RE15_FW_TUER_Z0 && n->z <= RE15_FW_TUER_Z0 + RE15_FW_TUER_D;
    /* Fortschritt in Blickrichtung (vorwaerts(g) = (cos, -sin)): > 0 = er laeuft, <= 0 nach dem Laufen = die Wand haelt ihn */
    const int32_t vor = (int32_t)((((int64_t)(n->x - s_w.jx_alt) * c) - ((int64_t)(n->z - s_w.jz_alt) * sn)) >> 12);
    if (vor > 0 && (int)s_w.takt > s_w.abgang_t + 1) s_w.gegangen = 1;
    const int an_der_wand = s_w.gegangen && vor <= 0 && vorn_in_tuer;
    const int notbremse = (int)s_w.takt - s_w.abgang_t >= RE15_FW_ABGANG_MAX;
    s_w.jx_alt = n->x; s_w.jz_alt = n->z;
    const int bit = sig(RE15_FW_SIG_TUER);
    if (im_aot || an_der_wand || bit || notbremse) {
        LOG("John an der Tuer bei (%d,%d) nach %d Bildern", n->x, n->z, (int)s_w.takt - s_w.abgang_t);
        LOG("John Gier %d, Punkt voraus (%d,%d)", n->rot_y, vx, vz);
        LOG("Ankunft: Plc_dest-Bit %d, Wand %d, im AOT %d", bit, an_der_wand, im_aot);
        if (notbremse) LOG("Abgang per Notbremse", 0, 0, 0);
        s_w.an_tuer = 1;
        sig_setzen(RE15_FW_SIG_TUER, 1);
        /* PORT-WAHL "Tuer hoerbar" (Pruefbericht s3 r2: "die Tuer oeffnet sich sichtbar bzw. hoerbar (Tuergeraeusch ... aus dem
         * Original belegen)"): der erste Ton der Tuersequenz, die der Port an GENAU dieser Tuer spielt (Tuer-AOT Slot 4 @0x010CE ->
         * gen/tuer_zuordnung.inc Zeile S283/T144 bzw. 5091 -> RE2-Archiv, Se_on der Tuerbank). RE2 hat fuer Adas Abgang keinen Ton
         * (sub12 @0x0193A-0x01960 ohne Se_on) - sie verlaesst einen Steg, keine Tuer. */
        re15_door_seq_anfrage_t q;
        memset(&q, 0, sizeof q);
        const int arch = re15_door_seq_zuordnen_flaeche(g_current_room_id & 0xFFFFu, 0, RE15_FW_TUER_X0 + RE15_FW_TUER_W / 2,
                                                        RE15_FW_TUER_Z0 + RE15_FW_TUER_D / 2, RE15_FW_TUER_W / 2,
                                                        RE15_FW_TUER_D / 2, NULL, NULL, 0, &q);
        const int se = (arch != RE15_DOOR_ARCHIV_KEINS && s_tuer_ton) ? s_tuer_ton(&q) : -1;
        LOG("Tuerton: Archiv %d DOOR%02X Variante %d", arch, q.re2_nr, q.variante);
        LOG("Tuerton: Seite S%03d, Se_on Tuerbank Eintrag %d (-1 = stumm)", (int)q.seite, se, 0);
    }
}

static void ende(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *n = john();
    if (n && !s_w.john_weg) {                                          /* Ada parken @0x01954-0x01960, NACH dem Schnitt @0x01948 */
        if (john_sichtbar(n, (int)g_scd.cam_id)) {
            s_w.parken_offen = 1;
            LOG("John bleibt an der Tuer: im Bild der Kamera %d nach dem Schnitt", (int)g_scd.cam_id, 0, 0);
        } else {
            john_parken(n, "Wurf: John ausser Bild der Kamera %d nach dem Schnitt zurueck (RE2 @0x01948 vor @0x01954)");
        }
    }
    if (s_w.form >= 0) {
        re15_actor_t *e = &g_actors[s_w.form];
        if (s_w.form_versteckt) e->no_draw = 0;
        e->x = (int16_t)s_w.fx; e->z = (int16_t)s_w.fz; e->rot_y = s_w.fg;   /* Member_set2 aus Var 8/9/10 @0x01992-0x019A6 */
    }
    s_w.aktiv = 0;                                                     /* +0x10E &= 0x7FFF @0x01968-0x01976: Form-KI an */
    LOG("Ende: Form-KI an, Spieler bleibt (%d,%d) Gier %d", pl->x, pl->z, pl->rot_y);
    {   /* Messzeile: der Aufnahme-AOT (Item_aot_set des Programms, op_item_aot_set) */
        const re15_aot_item_params_t *ip = &g_aot.item_params[RE15_FW_AOT_SLOT];
        LOG("Aufnahme-AOT Slot %d: Typ %d, Mitte x %d", RE15_FW_AOT_SLOT, (int)g_aot.slots[RE15_FW_AOT_SLOT].type, (int)g_aot.slots[RE15_FW_AOT_SLOT].x);
        LOG("Aufnahme-AOT: Gegenstand 0x%02x Anzahl %d Taken-Bit %d", ip->item_type, ip->amount, ip->taken_bit);
    }
}

void re15_finale_wurf_takt(void)
{
    if (!raum()) return;
    s_w.takt++;
    if (s_w.w == RE15_FW_W_LIEGT && !s_w.log_genommen && re15_game_flag_get(9, RE15_R42_BIT_WERFER_FINALE)) {
        s_w.log_genommen = 1; s_w.w = RE15_FW_W_GENOMMEN;
        int z0 = -1, z1 = -1;
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) {
            if (g_inv.slots[i].id == RE15_FW_ITEM) { if (z0 < 0) z0 = i; else if (z1 < 0) z1 = i; }
        }
        LOG("aufgehoben: Zellen %d/%d = 0x12", z0, z1, 0);
        LOG("aufgehoben: Anzahl %d/%d", z0 >= 0 ? g_inv.slots[z0].qty : -1, z1 >= 0 ? g_inv.slots[z1].qty : -1, 0);
        for (int i = 0; i < 10; i++)
            LOG("Inventar Zelle %d: Gegenstand 0x%02x Anzahl %d", i, g_inv.slots[i].id, g_inv.slots[i].qty);
    }
    if (s_w.parken_offen && !s_w.aktiv) {
        re15_actor_t *nj = john();
        if (!nj) s_w.parken_offen = 0;
        else if (!john_sichtbar(nj, (int)g_scd.cam_id)) john_parken(nj, "Wurf: John ausser Bild der Kamera %d - jetzt geparkt");
    }
    if (!s_w.aktiv) return;
    if (s_w.form >= 0 && s_w.form < RE15_ACTOR_MAX) form_ruht(&g_actors[s_w.form]);
    re15_actor_t *n = john();
    if (!s_w.bild0 && n) {
        /* W = erstes Programmbild (sub12 Bild 0): Sce_em_set lief in diesem Bild (scd_vm_tick vor dem Spielzug) */
        s_w.bild0 = 1; s_w.t0 = s_w.takt;
        int u = 0, v = 0;
        const int ib = im_bild(RE15_FW_CUT_WURF, n->x, -1200, n->z, &u, &v);
        LOG("Bild 0: John erscheint (Aktor %d) an (%d,%d)", s_w.john, n->x, n->z);
        LOG("Bild 0: John Gier %d, im Schnitt %d Bild-x/1000 %d", n->rot_y, RE15_FW_CUT_WURF, ib ? u : -9999);
    }
    if (s_w.bild0 && n && !s_w.john_weg && (s_w.takt - s_w.t0) % 4u == 0u)
        LOG("John Clip %d Bild %d bei W+%d", n->motion, (int)n->anim_frame, (int)(s_w.takt - s_w.t0));
    if (s_w.abgang && n && !s_w.john_weg)
        LOG("Abgang W+%d: John (%d,%d)", (int)(s_w.takt - s_w.t0), n->x, n->z);
    zuruf_messen();
    if (s_w.w == RE15_FW_W_HAND && loslassen_faellig(n)) loslassen();
    else if (s_w.w == RE15_FW_W_FLUG) flug_takt();
    if (sig(RE15_FW_SIG_REAKTION)) { sig_setzen(RE15_FW_SIG_REAKTION, 0); reaktion(); }
    if (s_w.abgang) abgang_takt();
    if (sig(RE15_FW_SIG_ENDE)) { sig_setzen(RE15_FW_SIG_ENDE, 0); ende(); }
}

/* ---- Aufnahme ohne Verlust der Zellen 8/9 ------------------------------------------------------------------------------- */
int re15_finale_wurf_luecken_schliessen(void)
{
    int n = 0;
    for (int runde = 0; runde < 10; runde++) {
        int luecke = -1, danach = 0;
        for (int i = 0; i < 10; i++) {
            if (g_inv.slots[i].id == 0) { if (luecke < 0) luecke = i; }
            else if (luecke >= 0) { danach = 1; break; }
        }
        if (luecke < 0 || !danach) break;
        re15_inv_compact();                                            /* FUN_8004dadc: schliesst die erste Luecke */
        n++;
    }
    return n;
}
void re15_finale_wurf_vor_aufnahme(uint8_t type, uint8_t taken_bit)
{
    if (type != RE15_FW_ITEM || taken_bit != RE15_R42_BIT_WERFER_FINALE) return;
    const int n = re15_finale_wurf_luecken_schliessen();
    LOG("vor der Aufnahme: %d Luecken geschlossen (Zelle 8 0x%02x, 9 0x%02x)", n, g_inv.slots[8].id, g_inv.slots[9].id);
}
