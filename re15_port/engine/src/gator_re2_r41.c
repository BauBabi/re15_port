/*
 * gator_re2_r41.c — Runde 41 Spur B "re2gator": RE2-Alligator (EM23_OVL_0000.BIN @0x80100000)
 * als Bausteine fuer den ROOM2090-Boss. Belege: analysis/befunde_runde41/B_re2gator.md §2.
 *
 * Was aus RE2 kommt (byte-belegt, Adresse je Konstante):
 *   - HP-Sockel 25300 / Schwaeche < 25000 (INIT @0x80100518-580, Selektor @0x80101ebc-dc)
 *   - Hysterese Schwimmen (Clip 0, Blend 32 @0x80100c38) <-> Paddeln (Clip 1, Blend 4 @0x80100cfc)
 *     ueber +0x1F0 >= 0x65 / < 0x5a und die Biss-Sperre +0x238 (Sub-1-Pre @0x80100b20-c08)
 *   - Wurzelgeschwindigkeit je Bild = Differenz der kumulierten Versatz-Bytes 6..11 zweier
 *     Keyframes (FUN_80015e7c @0x80015e7c; bei Bild 0 gegen 0, `sw zero,0x20c` ebd.)
 *   - LUNGE = Zonen-Dash Clip 2 (16) -> Clip 3 (0) mit SE 4 am Start (@0x80100d64/84, @0x80100dac-b4,
 *     @0x80100e20-28); INTRO Clip 6 -> Clip 3 (@0x801008ac-f0); Biss-Sperre 110 danach (@0x8010091c)
 *   - Stroke-Ereignisse (Frame-Wort Top-5-Bits 0x28 @0x801003f0-40c) -> Ereignis-Maschine
 *     @0x80101d44 (Tabelle @0x80100044): Rumble-Rampen FUN_800395b8(10,120,50) / (8,85,40), Bits 4/8
 *     in 0x800cfbd8 fuer 5 / 10 Bilder
 *   - Bildschirm-Schuetteln FUN_8002aa28/FUN_8002aa6c: Tabellen @0x8009daf4 (Modus 1) / @0x8009db14
 *     (Modus 2), Zaehler 0x800dfd6b, DR_MOVE des Hintergrunds um (-dx, -dy) (FUN_8002ac94)
 *   - HURT-Selektor @0x80101eac: R1 Kanister (+0x226 == 7), R4 Grossreaktion (+0x225 == 0 &&
 *     +0x22E == 0 && (rnd & 3) == 0, Sperre (rnd & 0x3f) + 120 @0x80101fa0-d4), sonst R0
 *   - Kanister T2 @0x801017c4 (Tabelle @0x80100024): Fenster @0x80100ae4-ec, 13er-Ease (+240/-320)
 *     @0x80101860-8c, Anhaengen an die Part-6-Weltmatrix @0x801018d0-60, Lokallage -> (1750,-210)
 *     @0x801019c4-0x80101b8c, Ease-Helfer 0x80101bac / 0x80101c74
 *   - R1 Explosionstod @0x80102158: SE 4, Raum-SE 15, Rumble (8,3)/(50,255->0), Kiefer FUN_80104318,
 *     Fontaenen FUN_80103eb4 (64 Ticks), Spritzring @0x8010338c, Clip 9, Vortrieb -500 (+40, -2/F),
 *     Ziel-Yaw +160 mit 75/F, Endzustand @0x80102654
 *
 * PORT-WAHL (keine RE2-Adresse, Pool-Geometrie ROOM2090): Zonenlinien = die drei sce-8-Wasserzonen
 * des RDT (gb_zone des Boss-Moduls) statt x<-17877 / z>=-7322; kein `yaw += 0x400` nach Clip 3
 * (Korridor-Ecke von ROOM40A0, Root-Yaw der Clips 2/3 endet bei 0 - EMR selbst geparst);
 * Putz-Bursts (ROOM40A0 sub14) um Leon beim Dash-Start statt am Korridorende; Kanister-Aufnahme
 * laengs der Blickachse statt laengs +Z.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "re15_gator_re2_r41.h"
#include "re15_enemy.h"        /* re15_enemy_find */
#include "re15_enemy_ai.h"     /* re15_clip_anchor_set_pub / re15_clip_root_motion_abs_pub */
#include "re15_emd.h"
#include "re15_skeleton.h"     /* re15_skel_compute_pose / bone_to_world / euler_matrix / sin,cos */
#include "re15_anim_select.h"  /* re15_compute_actor_kf */
#include "re15_ai_flavor.h"    /* re15_re2_rand = FUN_80015fe8 */
#include "re15_rumble.h"       /* FUN_8003947c / FUN_800395b8-Zwillinge */
#include "re2_fx.h"            /* FUN_8001bf10-Zwilling re2fx_spawn_sofort */
#include "re15_damage.h"       /* re15_enemy_bone_world_pos (Blut) */
#include "re15_esp.h"          /* re15_esp_fx_spawn_ex (Blut) */

/* ------------------------------------------------------------------------------------------ */
typedef struct {
    re15_gr2_se_fn se;
    uint8_t  ereignis;        /* +0x227 */
    uint8_t  ereignis_ctr;    /* +0x16A */
    int16_t  biss_cd;         /* +0x238 */
    uint8_t  r4_cd;           /* +0x22E */
    uint8_t  geschwaecht;     /* +0x218 & 0x20 */
    uint8_t  im_dash;         /* +0x225 */
    uint8_t  dash_phase;      /* 0 aus, 1 Clip 2, 2 Clip 3 */
    int8_t   zone_alt;        /* zuletzt gesehene Beckenzone (PORT) */
    uint8_t  intro_phase;     /* 0 aus, 1 Clip 6, 2 Clip 3 */
    int16_t  kopf_az;         /* +0x22A (Part 6 az) */
    int16_t  kiefer_az;       /* +0x228 (Part 7 az) */
    int16_t  ease_t;          /* +0x15A */
    uint8_t  t1_stufe;        /* +0x224 (Port: nicht genutzt, Biss = Boss-Modul) */
    /* R1 */
    uint8_t  r1_phase;        /* +0x6 der HURT-Routine: 0 Start, 1 Clip 9, 2, 3 Ende */
    int16_t  r1_speed;        /* +0x144 */
    int16_t  r1_schritt;      /* [0x801048c0] */
    uint16_t r1_zielyaw;      /* [0x801048bc] */
    int16_t  r1_176;          /* +0x176 (3678 -> 3478, Zweck im Overlay nicht gelesen) */
    uint8_t  fontaene;        /* +0x231 (0..64) */
    uint8_t  kiefer_rot;      /* FUN_80104318 ausgefuehrt */
    uint8_t  tot;
    int      treffer;
    int      letzter_clip;    /* Hysterese-Latch: 0 = Sub 1, 1 = Sub 2 */
    /* Putz */
    int      putz_t;          /* 0 = aus, sonst Bild der Sequenz (1..90) */
    int32_t  putz_x, putz_y, putz_z;
} gr2_t;

static gr2_t s_g[RE15_ACTOR_MAX];

/* global (EXE-Zustand): 0x800cfbd8 Bits 4/8, 0x800dfd6b */
static uint32_t s_cfbd8;
static uint8_t  s_schuettel_idx;
void (*re15_gr2_raum_se_hook)(int satz) = NULL;

/* Kanister (ein Objekt je Raum, obj[1] @0x800d051c) */
static struct {
    uint8_t  aktiv;
    uint8_t  stufe;           /* +0x226 */
    uint8_t  entfern_ctr;     /* +0x16B */
    uint8_t  angehaengt;
    int32_t  pos[3];          /* Welt (frei) */
    int32_t  rot[9];          /* Welt-Rotation Q12 (frei) */
    int32_t  lokal[3];        /* 1x-Rig-Lokallage relativ zur Part-6-Matrix (angehaengt) */
    int32_t  rel[9];          /* relative Rotation zur Part-6-Matrix (angehaengt) */
    int32_t  welt[3];         /* zuletzt berechnete Weltlage (angehaengt) */
    int32_t  welt_rot[9];
} s_kan;

static int s_env_geprueft;

/* ------------------------------------------------------------------------------------------ */
void re15_gr2_log(const char *fmt, ...)
{
    static FILE *f = NULL; static int stumm = -1;
    if (stumm < 0) stumm = (getenv("RE15_GB_STUMM") != NULL);
    if (stumm) return;
    if (!f) f = fopen("gator_boss.log", "a");
    if (!f) return;
    va_list ap; va_start(ap, fmt);
    fputs("RE2 ", f); vfprintf(f, fmt, ap); fputc('\n', f);
    va_end(ap); fflush(f);
}

static inline int32_t q12mul(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 12); }
static int32_t gr2_scale(const re15_actor_t *e) { return e->render_scale_q12 ? e->render_scale_q12 : 4096; }

/* Keyframe-Index eines Clip-Bildes (0x8000-Marker = Tween-Wort, wie gb_clip_kf im Boss-Modul). */
static int gr2_kf(const re15_emd_animation_t *an, int clip, int frame)
{
    if (!an || clip < 0 || clip >= an->clip_count) return -1;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (frame < 0 || frame >= c->frame_count) return -1;
    int fi = c->first_frame + frame, fend = c->first_frame + c->frame_count;
    while ((an->frames[fi] & 0x8000u) && fi + 1 < fend) fi++;
    return (int)(an->frames[fi] & 0xfffu);
}

/* Kumulierter Wurzelversatz (Keyframe-Bytes 6..11) eines Clip-Bildes. */
static int gr2_off(const re15_enemy_bank_t *bk, int clip, int frame, int16_t o[3])
{
    o[0] = o[1] = o[2] = 0;
    int kf = gr2_kf(&bk->anim, clip, frame);
    if (kf < 0) return 0;
    return re15_emd_get_keyframe_speed(&bk->skel, kf, &o[0], &o[1], &o[2]);
}

/* Netto-Versatz (letztes - erstes Bild), Rig-Raum. */
static void gr2_netto(const re15_enemy_bank_t *bk, int clip, int32_t out[3])
{
    int16_t a[3], b[3];
    int n = bk->anim.clips[clip].frame_count;
    gr2_off(bk, clip, 0, a); gr2_off(bk, clip, n - 1, b);
    out[0] = b[0] - a[0]; out[1] = b[1] - a[1]; out[2] = b[2] - a[2];
}

/* Rig-Vektor -> Welt (Ryaw wie re15_skel_bone_to_world: x' = c*x + s*z, z' = -s*x + c*z), skaliert. */
static void gr2_rig_welt(const re15_actor_t *e, const int32_t v[3], int32_t out[2])
{
    int32_t s = gr2_scale(e);
    int32_t c = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
    int32_t vx = q12mul(v[0], s), vz = q12mul(v[2], s);
    out[0] = e->x + (int32_t)(((int64_t)c * vx + (int64_t)sn * vz) >> 12);
    out[1] = e->z + (int32_t)((-(int64_t)sn * vx + (int64_t)c * vz) >> 12);
}

/* Ein Bild authored Wurzelbewegung (FUN_8001a330 -> FUN_80015e7c + FUN_800152c8, info/re2leon/PSX.EXE):
 * Delta = off[f] - off[f-1] (`subu` @0x80015fcc/@0x80015fd4), um den Yaw gedreht und auf +0x38/+0x40
 * addiert (@0x80015314-34) - BEIDE Komponenten (laengs und quer), skaliert. Bild 0 = Neu-Anker ohne Schritt
 * (Port-Konvention von re15_clip_root_motion_delta bei fr_prev < 0): RE2 nullt dort die Momentaufnahme
 * (`sw zero,524` @0x80015f14) und springt um off[0] - fuer Clip 6 sind das -7040 roh (Probe nb1/probe_bahn,
 * EMR), das Korridor-Staging des ROOM40A0-Durchbruchs (Pos_set + Rueckversatz hinter die Wand); im Becken
 * IST der Lauerplatz der Startpunkt (PORT-WAHL). Clip 2/3: off[0] = (0,0,0)/(0,0,3), ohne Belang.
 * Nachbesserung 1 (Abnahme 0, M1): vorher lief die Bahn ABSOLUT vom Anker (FUN_8001ad68-Zwilling) und
 * brauchte deshalb eine freie Bahn (gb_bahn_frei, Klemmlinie 1600) - im Westkanal nach Sueden scheiterte
 * die um 9 Einheiten (Ende-x -7309 < -7300), der Dash zuendete im Spiel nie. Jetzt geht jeder Schritt wie
 * der CHASE-Vortrieb durch die Dispatcher-Klemme (enemy_ai_common.c: re15_collision_constrain_enemy nach
 * dem Boss-Tick, hit_radius 1466); die Wand nimmt den RE2-Korridor-Seitversatz von Clip 3 (-4042 roh quer
 * ueber f60..f149 = die Ecke, nach der RE2 `yaw += 1024` dreht, @0x80100eb0-c0). Groesster Einzelschritt
 * skaliert: Clip 2 81, Clip 3 70, Clip 6 320 (f29) - weit unter dem Klemmradius. */
static void gr2_wurzel_schritt(re15_actor_t *e, const re15_enemy_bank_t *bk, int clip, int frame)
{
    if (frame <= 0) return;
    int16_t a[3], b[3];
    gr2_off(bk, clip, frame, b); gr2_off(bk, clip, frame - 1, a);
    int32_t v[3] = { b[0] - a[0], 0, b[2] - a[2] };
    int32_t out[2]; gr2_rig_welt(e, v, out);
    e->x = out[0]; e->z = out[1];
}

static re15_enemy_bank_t *gr2_bank(const re15_actor_t *e)
{
    re15_enemy_bank_t *bk = re15_enemy_find(e->type);
    return (bk && bk->ok) ? bk : NULL;
}

/* Weltmatrix (Rotation Q12 + Lage, Entity-Scale wie der Zeichner) eines Bones der aktuellen Pose. */
static int gr2_bone_welt(const re15_actor_t *e, int bone, int32_t rot[9], int32_t pos[3])
{
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk || bone < 0 || bone >= bk->skel.bone_count) return 0;
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t s = gr2_scale(e);
    int32_t tr[3] = { q12mul(poses[bone].trans[0], s), q12mul(poses[bone].trans[1], s),
                      q12mul(poses[bone].trans[2], s) };
    re15_skel_bone_to_world(tr, e->rot_y, e->x, e->y, e->z, pos);
    int32_t ry[9]; re15_skel_euler_matrix(0, (int)e->rot_y, 0, ry);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rot[r*3 + c] = (int32_t)(((int64_t)ry[r*3+0] * poses[bone].rot[0*3+c]
                                    + (int64_t)ry[r*3+1] * poses[bone].rot[1*3+c]
                                    + (int64_t)ry[r*3+2] * poses[bone].rot[2*3+c]) >> 12);
    return 1;
}

/* Ease-Deltas des Overlays: 0x80101bac (ease-in) / 0x80101c74 (ease-out), a0 = N, a1 = t, a2 = A. */
static int gr2_ease_in(int n, int t, int a)
{
    if (t == 0) return 0;                                   /* `beq a1,zero` @0x80101bac */
    int nn = n * n;
    return (a * t * t) / nn - (a * (t - 1) * (t - 1)) / nn; /* @0x80101bb0-c64 */
}
static int gr2_ease_out(int n, int t, int a)
{
    if (t == 0) return a;                                   /* `beq a1,zero -> addu v0,a2` @0x80101c74/d38 */
    int u = n - t, nn = n * n;                              /* `subu v1,a0,a1` @0x80101c78 */
    return (a * (u + 1) * (u + 1)) / nn - (a * u * u) / nn; /* @0x80101c7c-d34 */
}
/* FUN_8001a1e0(ptr, ziel, schritt): Wert um hoechstens `schritt` an `ziel` heran. */
static void gr2_annaehern(int32_t *v, int32_t ziel, int32_t schritt)
{
    if (*v < ziel) { *v += schritt; if (*v > ziel) *v = ziel; }
    else if (*v > ziel) { *v -= schritt; if (*v < ziel) *v = ziel; }
}

/* 32-Byte-MATRIX-Abbild (m[9] s16 @0, t[3] s32 @20) fuer re2fx_spawn. */
static void gr2_mtx(uint8_t m[32], const int32_t rot[9], const int32_t t[3])
{
    memset(m, 0, 32);
    for (int i = 0; i < 9; i++) { int16_t v = (int16_t)rot[i]; m[i*2] = (uint8_t)v; m[i*2+1] = (uint8_t)(v >> 8); }
    for (int i = 0; i < 3; i++) {
        uint32_t v = (uint32_t)t[i];
        m[20 + i*4] = (uint8_t)v; m[21 + i*4] = (uint8_t)(v >> 8);
        m[22 + i*4] = (uint8_t)(v >> 16); m[23 + i*4] = (uint8_t)(v >> 24);
    }
}
static const int32_t k_einheit[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
static int gr2_fx(uint32_t a0, int16_t a1, const int32_t rot[9], const int32_t pos[3], const int16_t ofs[4])
{
    uint8_t m[32]; gr2_mtx(m, rot ? rot : k_einheit, pos);
    return re2fx_spawn_sofort(a0, a1, m, ofs);               /* FUN_8001bf10 = sofort lebendig */
}

/* ------------------------------------------------------------------------------------------ */
void re15_gr2_reset(int slot, re15_gr2_se_fn se)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    memset(&s_g[slot], 0, sizeof s_g[slot]);
    s_g[slot].se = se;
    s_g[slot].zone_alt = -1;
    s_g[slot].letzter_clip = 0;
    s_cfbd8 = 0;                                            /* Raumstart: keine Schuettel-Bits */
    re15_gr2_kanister_env();
}

static void gr2_se(gr2_t *g, int id) { if (g->se) g->se(id); }

/* Frame-Wort: Top-5-Bits 0x28 -> Ereignis 1 (@0x801003f0-40c). */
void re15_gr2_frame_wort(int slot, uint32_t wort)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    if ((wort & 0xf8000000u) == 0x28000000u) s_g[slot].ereignis = 1;
}

/* Ereignis-Maschine @0x80101d44 (Tabelle @0x80100044). */
static void gr2_ereignis_takt(gr2_t *g)
{
    switch (g->ereignis) {
    case 1:                                                 /* @0x80101d80 */
        re15_rumble_ramp(10, 120, 50, 0);                   /* FUN_800395b8(10,120,50,0) @0x80101d8c */
        g->ereignis_ctr = 2; g->ereignis = 2;               /* @0x80101d98-a4 */
        break;
    case 2:                                                 /* @0x80101da8 */
        if (--g->ereignis_ctr == 0) g->ereignis = 3;        /* @0x80101db0-bc, 0x80101e4c */
        break;
    case 3:                                                 /* @0x80101dcc */
        g->ereignis_ctr = 5; s_cfbd8 |= 4u; g->ereignis = 4;/* @0x80101dd8-f8 */
        break;
    case 4:                                                 /* @0x80101dfc */
        if (--g->ereignis_ctr == 0) {                       /* @0x80101e04-10 */
            re15_rumble_ramp(8, 85, 40, 0);                 /* @0x80101e14-28 */
            g->ereignis_ctr = 10;                           /* @0x80101e24/2c */
            s_cfbd8 = (s_cfbd8 & ~4u) | 8u;                 /* @0x80101e3c-48 */
            g->ereignis = 5;
        }
        break;
    case 5:                                                 /* @0x80101e60 */
        if (--g->ereignis_ctr == 0) { s_cfbd8 &= ~8u; g->ereignis = 0; }   /* @0x80101e78-94 */
        break;
    default: break;
    }
}

/* Bildschirm-Schuetteln FUN_8002aa28 / FUN_8002aa6c. */
static const uint8_t k_schuettel1[32] = {   /* @0x8009daf4 */
    0x00,0x01, 0x00,0x00, 0x00,0x00, 0x00,0x01, 0x01,0x01, 0x00,0x00, 0x00,0x01, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x01, 0x00,0x01, 0x00,0x00, 0x00,0x00, 0x01,0x01, 0x00,0x00 };
static const uint8_t k_schuettel2[32] = {   /* @0x8009db14 */
    0x00,0x02, 0x00,0x00, 0x00,0x02, 0x00,0x03, 0x01,0x00, 0x00,0x01, 0x00,0x02, 0x00,0x00,
    0x00,0x02, 0x00,0x00, 0x02,0x01, 0x00,0x02, 0x00,0x02, 0x00,0x00, 0x01,0x01, 0x00,0x02 };
static int s_schuettel_dx, s_schuettel_dy;   /* fuer dieses Bild (im Takt berechnet) */
static int s_schuettel_modus;

static void gr2_schuettel_takt(void)
{
    int modus = (s_cfbd8 & 8u) ? 1 : (s_cfbd8 & 4u) ? 2 : 0;   /* @0x8002aa34-50 */
    s_schuettel_modus = modus;
    if (!modus) { s_schuettel_dx = s_schuettel_dy = 0; return; }
    s_schuettel_idx = (uint8_t)((s_schuettel_idx + 1) & 0xf);  /* @0x8002aa90-a4 */
    const uint8_t *t = (modus == 1) ? k_schuettel1 : k_schuettel2;
    s_schuettel_dx = t[s_schuettel_idx * 2];                   /* bVar1 */
    s_schuettel_dy = t[s_schuettel_idx * 2 + 1];               /* uVar2 */
}
void re15_gr2_schuettel_offset(int *dx, int *dy) { if (dx) *dx = s_schuettel_dx; if (dy) *dy = s_schuettel_dy; }
int  re15_gr2_schuettel_modus(void) { return s_schuettel_modus; }
uint8_t re15_gr2_ereignis(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].ereignis : 0; }

/* ---- Putz (ROOM40A0 sub14, 9x Sce_espr_on id 4, 6 Durchlaeufe) ------------------------------
 * PORT-WAHL: Bursts um Leon beim Dash-Start (RE2: Kamera 9 am Korridorende, sub13). Lage = RE2-Versatz
 * zur Mitte der neun Emitter (-24378, ., 1350) in x/z, Hoehe = RE2-Absolutwert ueber dem Steg y=0
 * (Leons Watt-Ebene ist ebenfalls y=0). */
static const struct { int8_t bild; uint8_t typ; uint16_t param; int16_t dx, y, dz, dir; } k_putz[9] = {
    { 0, 0x10, 0x0e00,   178, -3500,  -50,  824 },   /* @0x086E */
    { 0, 0x08, 0x1000,   578, -2000,   50,  474 },   /* @0x087E */
    { 2, 0x08, 0x1000, -1122, -3800,  -50, 1024 },   /* @0x0892 (Sleep 2) */
    { 3, 0x08, 0x1200,   578, -2000,   50,  474 },   /* @0x08A4 (Evt_next) */
    { 4, 0x10, 0x0800,   178, -4400,    0, 1124 },   /* @0x08B6 */
    { 5, 0x10, 0x2000, -1022, -4000,  -50,  974 },   /* @0x08C8 */
    { 7, 0x08, 0x1400,   578, -3500,  -50, 1074 },   /* @0x08DC (Sleep 2) */
    { 9, 0x00, 0x2200,   178, -3000,  150, 1524 },   /* @0x08F0 (Sleep 2) */
    { 9, 0x10, 0x1e00,  -622, -2400,  -50, 1024 },   /* @0x0900 */
};
#define GR2_PUTZ_DAUER 15     /* letzter Spawn Bild 9 + Sleep 5 @0x0910 -> Evt_end */
#define GR2_PUTZ_RUNDEN 6     /* sub13 `For 6: Gosub sub14` @0x0862-6A */
static int s_putz_spawns, s_putz_runde_start;
static int32_t s_putz_letzt[3];                              /* Messhaken: Welt-Lage des letzten Spawns */
static void gr2_putz_takt(gr2_t *g)
{
    if (g->putz_t <= 0) return;
    int t = g->putz_t - 1;                                  /* 0.. */
    int bild = t % GR2_PUTZ_DAUER, runde = t / GR2_PUTZ_DAUER;
    if (bild == 0) s_putz_runde_start = s_putz_spawns;
    for (int i = 0; i < 9; i++) {
        if (k_putz[i].bild != bild) continue;
        int32_t pos[3] = { g->putz_x + k_putz[i].dx, g->putz_y + k_putz[i].y, g->putz_z + k_putz[i].dz };
        uint32_t a0 = 0x04000000u | ((uint32_t)k_putz[i].typ << 16) | k_putz[i].param;
        if (gr2_fx(a0, (int16_t)k_putz[i].dir, NULL, pos, NULL) >= 0) {
            s_putz_spawns++;
            memcpy(s_putz_letzt, pos, sizeof s_putz_letzt);
        }
    }
    /* Messzeile je Durchlauf (Abnahme 0, M2: die exe hatte keine Zeile fuer die Putz-Spawns) - nach dem
     * letzten Spawn-Bild 9 (@0x08F0/@0x0900) des Durchlaufs. */
    if (bild == 9)
        re15_gr2_log("PUTZ runde %d/%d spawns=%d gesamt=%d um=(%d,%d,%d) letzter=(%d,%d,%d)",
                     runde + 1, GR2_PUTZ_RUNDEN, s_putz_spawns - s_putz_runde_start, s_putz_spawns,
                     g->putz_x, g->putz_y, g->putz_z, s_putz_letzt[0], s_putz_letzt[1], s_putz_letzt[2]);
    g->putz_t++;
    if (g->putz_t > GR2_PUTZ_DAUER * GR2_PUTZ_RUNDEN) g->putz_t = 0;
}
int  re15_gr2_putz_spawns(void) { return s_putz_spawns; }
void re15_gr2_putz_letzte(int32_t pos[3]) { memcpy(pos, s_putz_letzt, sizeof s_putz_letzt); }

/* ---- Kanister: Vorpass (Sub-1-Pre @0x80100a0c-b18) + T2-Maschine @0x801017c4 ---------------- */
static int gr2_kan_vor(const re15_actor_t *e, int32_t *laengs, int32_t *quer)
{
    /* Lage des Kanisters im Rig-Rahmen des Gators (laengs = Blickachse, quer = rechtwinklig). */
    int32_t c = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
    int64_t dx = (int64_t)s_kan.pos[0] - e->x, dz = (int64_t)s_kan.pos[2] - e->z;
    *laengs = (int32_t)((c * dx - sn * dz) >> 12);
    *quer   = (int32_t)((sn * dx + c * dz) >> 12);
    return 1;
}

static void gr2_kan_welt_setzen(const int32_t rot[9], const int32_t pos[3])
{
    memcpy(s_kan.welt_rot, rot, sizeof s_kan.welt_rot);
    memcpy(s_kan.welt, pos, sizeof s_kan.welt);
}

/* Angehaengt: Welt = Part-6-Matrix x (rel, s*lokal). */
static void gr2_kan_folgen(const re15_actor_t *e)
{
    int32_t r6[9], p6[3];
    if (!gr2_bone_welt(e, 6, r6, p6)) return;
    int32_t s = gr2_scale(e);
    int32_t l[3] = { q12mul(s_kan.lokal[0], s), q12mul(s_kan.lokal[1], s), q12mul(s_kan.lokal[2], s) };
    int32_t pos[3], rot[9];
    for (int r = 0; r < 3; r++) {
        pos[r] = p6[r] + (int32_t)(((int64_t)r6[r*3] * l[0] + (int64_t)r6[r*3+1] * l[1] + (int64_t)r6[r*3+2] * l[2]) >> 12);
        for (int c = 0; c < 3; c++)
            rot[r*3+c] = (int32_t)(((int64_t)r6[r*3] * s_kan.rel[c] + (int64_t)r6[r*3+1] * s_kan.rel[3+c]
                                  + (int64_t)r6[r*3+2] * s_kan.rel[6+c]) >> 12);
    }
    gr2_kan_welt_setzen(rot, pos);
}

static void gr2_kan_takt(gr2_t *g, re15_actor_t *e, int im_chase)
{
    if (!s_kan.aktiv) return;
    if (s_kan.stufe == 13) {                                /* R1 P1 @0x8010255c-78: `lb v0,363; blez -> word0 = 0; sonst -1` */
        if (s_kan.entfern_ctr <= 0) s_kan.aktiv = 0; else s_kan.entfern_ctr--;
        return;
    }
    if (s_kan.stufe == 0) {
        if (!im_chase) return;
        /* Fenster: Kanister 8601..8999 (x Scale) VOR der Wurzel laengs der Blickachse (RE2: obj[1].z - z
         * @0x80100ad8-ec), seitlich innerhalb des Koerpers (PORT-WAHL, RE2-Korridor ist 1 Spur breit). */
        int32_t laengs, quer, s = gr2_scale(e);
        gr2_kan_vor(e, &laengs, &quer);
        int32_t ab = q12mul(RE15_GR2_KAN_FENSTER_AB, s), len = q12mul(RE15_GR2_KAN_FENSTER_LEN, s);
        int32_t qmax = (int32_t)e->hit_radius_min + 267;    /* Koerperradius (skaliert) + Kanisterradius (MD1) */
        if (laengs >= ab && laengs < ab + len && quer > -qmax && quer < qmax) {
            s_kan.stufe = 1;                                /* +0x226 = 1 @0x80100b08 */
            re15_gr2_log("KANISTER Stufe 1 laengs=%d quer=%d", laengs, quer);
        }
        return;
    }
    int32_t s = gr2_scale(e);
    switch (s_kan.stufe) {
    case 1:                                                 /* @0x8010183c: Raumflag 29, t = 0 */
        g->ease_t = 0; s_kan.stufe = 2;
        break;
    case 2: {                                               /* @0x80101860 */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(13, g->ease_t, 240));   /* @0x80101860-6c */
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(13, g->ease_t, -320));  /* @0x80101870-88 */
        g->ease_t++;
        /* Kanister bleibt >= 8100 (x Scale) vor der Wurzel (@0x801018a4-c0: obj[1].z = z + 8100). */
        int32_t laengs, quer; gr2_kan_vor(e, &laengs, &quer);
        int32_t vh = q12mul(RE15_GR2_KAN_VORHALT, s);
        if (laengs < vh) {
            int32_t c = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
            s_kan.pos[0] += q12mul(c, vh - laengs);
            s_kan.pos[2] -= q12mul(sn, vh - laengs);
        }
        if (g->ease_t == 13) {                              /* @0x801018c4-cc */
            int32_t r6[9], p6[3];
            if (gr2_bone_welt(e, 6, r6, p6)) {
                /* FUN_8002d030(Part-6-Matrix, Kanister-Matrix): rel = R6^T * Rk, lokal = R6^T * (p - p6) / s */
                int32_t d[3] = { s_kan.pos[0] - p6[0], s_kan.pos[1] - p6[1], s_kan.pos[2] - p6[2] };
                for (int r = 0; r < 3; r++) {
                    int32_t v = (int32_t)(((int64_t)r6[0*3+r] * d[0] + (int64_t)r6[1*3+r] * d[1] + (int64_t)r6[2*3+r] * d[2]) >> 12);
                    s_kan.lokal[r] = (int32_t)(((int64_t)v << 12) / s);
                    for (int c = 0; c < 3; c++)
                        s_kan.rel[r*3+c] = (int32_t)(((int64_t)r6[0*3+r] * s_kan.rot[0*3+c] + (int64_t)r6[1*3+r] * s_kan.rot[1*3+c]
                                                    + (int64_t)r6[2*3+r] * s_kan.rot[2*3+c]) >> 12);
                }
                s_kan.angehaengt = 1;
                re15_gr2_log("KANISTER angehaengt lokal=(%d,%d,%d)", s_kan.lokal[0], s_kan.lokal[1], s_kan.lokal[2]);
            }
            g->ease_t = 0; s_kan.stufe = 3;                 /* @0x801018dc, 0x80101964-6c */
        }
        break; }
    case 3:                                                 /* @0x80101970: ease-in 6 (-140, -60) */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(6, g->ease_t, -140));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(6, g->ease_t, -60));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 70);             /* @0x801019c4-cc */
        if (g->ease_t == 6) { g->ease_t = 0; s_kan.stufe = 4; }               /* @0x801019d0-d8, 0x80101b3c-54 */
        break;
    case 4:                                                 /* @0x801019dc: ease-out 6 */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_out(6, g->ease_t, -140));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_out(6, g->ease_t, -60));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 145);            /* @0x80101a30-38 */
        gr2_annaehern(&s_kan.lokal[1], RE15_GR2_KAN_LOKAL_Y, 25);             /* @0x80101a4c-54 */
        if (g->ease_t == 6) { g->ease_t = 0; s_kan.stufe = 5; }
        break;
    case 5:                                                 /* @0x80101a64: ease-in 7 (+75, +100) */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(7, g->ease_t, 75));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(7, g->ease_t, 100));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 100);            /* @0x80101ab8-c0 */
        gr2_annaehern(&s_kan.lokal[1], RE15_GR2_KAN_LOKAL_Y, 55);             /* @0x80101ad4-dc */
        if (g->ease_t == 7) { g->ease_t = 0; s_kan.stufe = 6; }
        break;
    case 6:                                                 /* @0x80101aec: ease-out 7 */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_out(7, g->ease_t, 75));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_out(7, g->ease_t, 100));
        g->ease_t++;
        if (g->ease_t == 7) { g->ease_t = 0; s_kan.stufe = 7; re15_gr2_log("KANISTER getragen (Stufe 7)"); }
        break;
    case 7:                                                 /* @0x80101b58: halten */
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 100);            /* @0x80101b68-70 */
        gr2_annaehern(&s_kan.lokal[1], RE15_GR2_KAN_LOKAL_Y, 30);             /* @0x80101b84-8c */
        break;
    default: break;
    }
    if (s_kan.angehaengt) gr2_kan_folgen(e);
}

/* ---- Takt ---------------------------------------------------------------------------------- */
void re15_gr2_takt(int slot, re15_actor_t *e, re15_actor_t *pl, int im_chase)
{
    (void)pl;
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    gr2_t *g = &s_g[slot];
    if (!s_env_geprueft) re15_gr2_kanister_env();
    gr2_ereignis_takt(g);                                   /* Root @0x80100410 */
    gr2_schuettel_takt();                                   /* EXE je Bild */
    gr2_putz_takt(g);
    if (g->biss_cd > 0) g->biss_cd--;                       /* +0x238 @0x801004a8-b8 */
    if (g->r4_cd > 0) g->r4_cd--;                           /* +0x22E @0x80100494-a4 */
    gr2_kan_takt(g, e, im_chase && !g->tot);
}

/* ---- Verfolgung: Hysterese + authored Vortrieb --------------------------------------------- */
int re15_gr2_chase_clip(int slot, re15_actor_t *e, int32_t dist, int32_t *vortrieb)
{
    gr2_t *g = &s_g[slot];
    int clip;
    if (g->letzter_clip == 0) {                             /* Sub 1 */
        clip = (g->biss_cd == 0 && dist >= RE15_GR2_PADDEL_AB) ? 1 : 0;   /* @0x80100b5c-64, @0x80100b88-98 */
    } else {                                                /* Sub 2 */
        clip = (dist < RE15_GR2_SCHWIMM_AB || g->biss_cd != 0) ? 0 : 1;   /* @0x80100be4-c04 */
    }
    if (clip != g->letzter_clip || e->motion != clip) {
        e->motion = (uint8_t)clip; e->anim_frame = 0;       /* FUN_8001a330: Clipwechsel -> Bild 0 @0x8001a34c-64 */
        e->anim_frac = (uint8_t)(clip == 0 ? 32 : 4);      /* Blend 32 @0x80100c38 / 4 @0x80100cfc */
        g->letzter_clip = clip;
    } else {
        re15_enemy_bank_t *bk = gr2_bank(e);
        int n = bk ? bk->anim.clips[clip].frame_count : 1;
        if (++e->anim_frame >= n) e->anim_frame = 0;        /* Clip-Ende: Sub 2 Selbst-Reset @0x80100d08-10 */
    }
    if (vortrieb) {
        re15_enemy_bank_t *bk = gr2_bank(e);
        int16_t a[3] = {0,0,0}, b[3] = {0,0,0};
        if (bk) {
            gr2_off(bk, clip, e->anim_frame, b);
            if (e->anim_frame > 0) gr2_off(bk, clip, e->anim_frame - 1, a);   /* Bild 0: Latch 0 (`sw zero,0x20c`) */
        }
        *vortrieb = q12mul((int32_t)b[0] - a[0], gr2_scale(e));                /* +0x144 = dx, skaliert */
    }
    return clip;
}
void re15_gr2_biss_cd(int slot, int n) { if (slot >= 0 && slot < RE15_ACTOR_MAX) s_g[slot].biss_cd = (int16_t)n; }
int  re15_gr2_biss_cd_rest(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].biss_cd : 0; }

/* ---- Dash ---------------------------------------------------------------------------------- */
int re15_gr2_dash_zonenwechsel(int slot, const re15_actor_t *e, int zone, int zone_leon, int32_t ziel_vor[2])
{
    gr2_t *g = &s_g[slot];
    int neu = (g->zone_alt >= 0 && zone != g->zone_alt);
    g->zone_alt = (int8_t)zone;
    if (!neu || zone != zone_leon || g->im_dash) return 0;
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk) return 0;
    /* Ende der Bahn in Blickrichtung = nur die LAENGS-Komponente (x) der Netto-Versaetze Clip 2 + 3
     * (2307 + 1881 skaliert); die Quer-Komponente ist die RE2-Korridor-Ecke und geht im Becken in die
     * Wand-Klemme (s. gr2_wurzel_schritt). Nachbesserung 1 (M1). */
    int32_t n2[3], n3[3];
    gr2_netto(bk, 2, n2); gr2_netto(bk, 3, n3);
    int32_t v[3] = { n2[0] + n3[0], 0, 0 };
    gr2_rig_welt(e, v, ziel_vor);
    return 1;
}
void re15_gr2_dash_start(int slot, re15_actor_t *e, const re15_actor_t *pl)
{
    gr2_t *g = &s_g[slot];
    gr2_se(g, 4);                                           /* P0: `addiu a0,zero,4` @0x80100d64, jal @0x80100d84 */
    e->motion = 2; e->anim_frame = 0; e->anim_frac = 16;    /* Clip 2 Blend 16 @0x80100dac-b4 */
    g->im_dash = 1; g->dash_phase = 1;                      /* +0x225 = 1 @0x80100e04-0c */
    /* PORT-WAHL: Putz-Sequenz um Leon (s. k_putz) - Lage = Leons Stand beim Dash-Start (RE2: Kamera 9 am
     * Korridorende, dort laeuft Leon, sub13 @0x0778/0x0786). Nachbesserung 1 (M2): vorher blieb putz_x/y/z
     * ungesetzt (0,0,0) = noerdlich ausserhalb des Beckens (z > -5400). */
    g->putz_t = 1;
    g->putz_x = pl ? pl->x : e->x; g->putz_y = pl ? pl->y : 0; g->putz_z = pl ? pl->z : e->z;
    re15_gr2_log("DASH start pos=(%d,%d) yaw=%d putz_um=(%d,%d,%d)", e->x, e->z, (int)e->rot_y,
                 g->putz_x, g->putz_y, g->putz_z);
}
int re15_gr2_dash_tick(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!g->dash_phase || !bk) { g->im_dash = 0; g->dash_phase = 0; return 0; }
    int clip = (g->dash_phase == 1) ? 2 : 3;
    int n = bk->anim.clips[clip].frame_count;
    e->motion = (uint8_t)clip;
    gr2_wurzel_schritt(e, bk, clip, (int)e->anim_frame);   /* bildweise, durch die Dispatcher-Klemme */
    if (e->anim_frame + 1 >= n) {                           /* FUN_8001a330 liefert 1 am Clip-Ende */
        if (g->dash_phase == 1) {
            g->dash_phase = 2; e->motion = 3; e->anim_frame = 0; e->anim_frac = 0;   /* Clip 3 (0) @0x80100e20-28 */
            return 1;
        }
        g->im_dash = 0; g->dash_phase = 0;                  /* +0x225 = 0 `sb zero,549` @0x80100eb8, -> 0x101 @0x80100ee4 */
        /* `lhu v0,118 / addiu v0,v0,1024 / sh v0,118` @0x80100eb0-c0: Clip 3 dreht den Root-Bone von 438 auf
         * 1023 (f70..f148, EMR, Probe nb1/probe_bahn) und springt im letzten Bild auf 0 - die Vierteldrehung
         * wandert in den Entity-Yaw. Nachbesserung 1: vorher weggelassen (als "Korridor-Ecke" gelesen), dann
         * poppte der Koerper am Dash-Ende um 90 Grad zurueck; die Verfolgung dreht danach auf Leon (Wende). */
        e->rot_y = (int16_t)((e->rot_y + 1024) & 0x0fff);
        re15_gr2_log("DASH ende pos=(%d,%d) yaw=%d", e->x, e->z, (int)e->rot_y);
        return 0;
    }
    e->anim_frame++;
    return 1;
}
int re15_gr2_im_dash(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].im_dash : 0; }

/* ---- Intro --------------------------------------------------------------------------------- */
void re15_gr2_intro_bahn(const re15_actor_t *e, int32_t ziel[2])
{
    re15_enemy_bank_t *bk = gr2_bank(e);
    ziel[0] = e->x; ziel[1] = e->z;
    if (!bk) return;
    int16_t a[3], b[3];
    gr2_off(bk, 6, 0, a); gr2_off(bk, 6, bk->anim.clips[6].frame_count - 1, b);
    int32_t v[3] = { b[0] - a[0], 0, b[2] - a[2] };   /* Anker = e - R*off0; Ende = Anker + R*off_n */
    gr2_rig_welt(e, v, ziel);
}
void re15_gr2_intro_start(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    int32_t zi[2]; re15_gr2_intro_bahn(e, zi);              /* nur Telemetrie (Nachbesserung 1: kein Bahn-Gate) */
    e->motion = 6; e->anim_frame = 0; e->anim_frac = 0;     /* Clip 6 (0) @0x801008ac-b8 */
    g->intro_phase = 1;
    re15_gr2_log("INTRO start pos=(%d,%d) yaw=%d bahn_ende=(%d,%d)", e->x, e->z, (int)e->rot_y, zi[0], zi[1]);
}
int re15_gr2_intro_tick(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!g->intro_phase || !bk) { g->intro_phase = 0; return 0; }
    int clip = (g->intro_phase == 1) ? 6 : 3;
    int n = bk->anim.clips[clip].frame_count;
    e->motion = (uint8_t)clip;
    gr2_wurzel_schritt(e, bk, clip, (int)e->anim_frame);   /* bildweise, durch die Dispatcher-Klemme */
    if (e->anim_frame + 1 >= n) {
        if (g->intro_phase == 1) {
            /* Clip 3 @0x801008e4-f0. RE2 setzt davor Cut 1 + z = -21212 (@0x801008c4-e0, Korridor-Ecke);
             * PORT-WAHL: am Ort weiter (kein Kameraskript, kein Teleport). */
            g->intro_phase = 2; e->motion = 3; e->anim_frame = 0; e->anim_frac = 0;
            return 1;
        }
        g->intro_phase = 0;
        g->biss_cd = RE15_GR2_BISS_CD_INTRO;                /* +0x238 = 110 @0x8010091c-20 */
        g->letzter_clip = 0;                                /* Routine 0x101 @0x80100928-2c */
        e->rot_y = (int16_t)((e->rot_y + 1024) & 0x0fff);   /* `lhu v0,118 / addiu v0,v0,1024 / sh v0,118` @0x80100938-48 */
        re15_gr2_log("INTRO ende pos=(%d,%d) yaw=%d", e->x, e->z, (int)e->rot_y);
        return 0;
    }
    e->anim_frame++;
    return 1;
}
int re15_gr2_im_intro(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? (s_g[slot].intro_phase != 0) : 0; }

/* ---- HURT-Selektor @0x80101eac -------------------------------------------------------------- */
int re15_gr2_hurt_reaktion(int slot, re15_actor_t *e, const re15_actor_t *pl, int gesperrt)
{
    gr2_t *g = &s_g[slot];
    g->treffer++;
    if (e->hp < RE15_GR2_HP_SOCKEL) g->geschwaecht = 1;    /* @0x80101ebc-dc */
    if (pl && pl->hp < 0) return 0;                         /* @0x80101ee0-ec */
    if (s_kan.aktiv && s_kan.stufe == 7) {                  /* @0x80101ef4-fc */
        /* RE2 nimmt +0x230 == 1 (Sub SCHWIMMEN vor dem Treffer) aus (@0x80101f04-0c); PORT-WAHL nach
         * Nutzer-Wortlaut "sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen":
         * kein Ausnahme-Sub. */
        return 1;
    }
    if (gesperrt) return 0;
    if (g->im_dash == 0 && g->r4_cd == 0 && (re15_re2_rand() & 3u) == 0u) {   /* @0x80101f8c-b8 */
        g->r4_cd = (uint8_t)((re15_re2_rand() & 0x3fu) + RE15_GR2_R4_CD_BASIS);   /* @0x80101fc0-d4 */
        return 4;
    }
    return 0;
}

/* ---- Geplatzter Kiefer = FUN_80104318 (@0x80104318-58) -------------------------------------
 * Aufruf aus R1 P0: `addu a0,s1,zero / jal 0x80104318 / addiu a1,s3,1204` @0x80102350-58 mit
 * s3 = [em+0x198] (`lw s3,408(s1)` @0x80102174) = der RE2-MODELLBLOCK (Records a 172 Byte,
 * FUN_80028368); 1204 = 7 x 172 = Part-Record 7 = KIEFER. Die Funktion (selbst disassembliert):
 *   lw v0,0(a1) / ori v0,v0,0x4a / sw v0,0(a1)        @0x8010431c/24/28  Flagwort |= 0x4A
 *       (0x40 = eigene Matrix `andi v0,s3,0x40` @0x80027498, 0x08 = Drift-Physik FUN_80028DAC
 *        @0x80027B98 UND Scherben-Zeichnung FUN_8002D3C8/D718 statt intaktem Mesh, FUN_80027434.c:168-170)
 *   lhu v1,118(a0) / addiu v1,v1,2048 / sh v1,152(a1)  @0x8010432c/50/58  +0x98 = Kurs yaw + 0x800
 *   ori v0,zero,0x8000 / sh v0,160(a1)                 @0x80104330/34     +0xA0 = 0x8000: Lebensdauer-Zaehler
 *       (FUN_80028DAC: `andi 0x7fff / sltiu 0x1d` @0x80028DC8-CC zaehlt 0..28, dann `sw zero,0(s0)`
 *        @0x80028DDC = Flagwort 0 = weg; Bit 15 = Zeile 1 der Burst-Skalentabelle @0x8009DC64,
 *        `lhu / srl 0xf` FUN_8002d3c8.c)
 *   addiu v0,zero,-10 / sh v0,154(a1)                  @0x80104338/40     +0x9A = vy -10 (steigt 10/Bild,
 *       `t[1] += +0x9A` @0x80028E04-14) — KEIN Kiefer-az (so stand es in 86661849, korrigiert)
 *   sh zero,156(a1) / sh zero,158(a1) / sh zero,164(a1) @0x8010433c/44/48 +0x9C/+0x9E/+0xA4 = 0 (kein Vortrieb)
 *   lui a2,0x20 / ori a2,a2,0x2038 / sw a2,112(a1)     @0x80104318/20/4c  +0x70 = 0x202038 (R 0x38 G 0x20 B 0x20,
 *       flache Scherbenfarbe `lw t2,112(a2)` @0x8002D508)
 * PORT: dieselbe Part-Bruecke wie Zombie-Arm (Flagwort 0x4A, enemy_ai_re2_zombie.c re2z_arm_throw) und
 * Kraehe (schuss_r36.c): Seed des Blocks wie FUN_80028368 (Bit 0, +0x70 = 0x808080, Mesh = i), dann der
 * Stempel. Den Haken in re15_re2z_gore_active (Typ 0x23: Bruecke an, sobald Record 0 Bit 0 traegt) liest
 * der NPC-Zeichner in main.c (re15_re2z_gore_part_matrix @10512 / re15_re2z_gore_part_burst @10624). */
static void gr2_kiefer_platzen(re15_actor_t *e)
{
    memset(e->re2z_part_v,    0, sizeof e->re2z_part_v);
    memset(e->re2z_part_rot,  0, sizeof e->re2z_part_rot);
    memset(e->re2z_part_m,    0, sizeof e->re2z_part_m);
    memset(e->re2z_part_t,    0, sizeof e->re2z_part_t);
    memset(e->re2z_part_grav, 0, sizeof e->re2z_part_grav);
    memset(e->re2z_part_st86, 0, sizeof e->re2z_part_st86);
    memset(e->re2z_part_yaw98,0, sizeof e->re2z_part_yaw98);
    memset(e->re2z_part_w9a,  0, sizeof e->re2z_part_w9a);
    memset(e->re2z_part_w9c,  0, sizeof e->re2z_part_w9c);
    memset(e->re2z_part_w9e,  0, sizeof e->re2z_part_w9e);
    memset(e->re2z_part_life, 0, sizeof e->re2z_part_life);
    memset(e->re2z_part_life_draw, 0, sizeof e->re2z_part_life_draw);
    memset(e->re2z_part_wa4,  0, sizeof e->re2z_part_wa4);
    e->re2z_part_burst_draw = 0u;
    e->re2z_part_seeded = 0u; e->re2z_part_stepped = 0u; e->re2z_part_frame = 0u;
    for (int i = 0; i < 16; i++) {
        e->re2z_part_blend[i] = -1;                          /* +0x7A: keine Setz-Blende (`bltz` @0x80028D2C) */
        e->re2z_part_flags[i] = 1u;                          /* Bit 0 = Part vorhanden (EM23: 22 Parts, Block 16 Records) */
        e->re2z_part_tint[i]  = 0x00808080u;                 /* FUN_80028368: +0x70 = 0x808080 */
        e->re2z_part_mesh[i]  = (uint8_t)i;
    }
    e->re2z_part_flags[7] |= 0x4Au;                          /* @0x8010431c-28 */
    e->re2z_part_tint [7]  = 0x00202038u;                    /* @0x8010434c */
    e->re2z_part_yaw98[7]  = (int16_t)(e->rot_y + 0x800);    /* @0x80104350/58 */
    e->re2z_part_w9a  [7]  = -10;                            /* @0x80104338/40 */
    e->re2z_part_w9c  [7]  = 0;                              /* @0x8010433c */
    e->re2z_part_w9e  [7]  = 0;                              /* @0x80104344 */
    e->re2z_part_wa4  [7]  = 0;                              /* @0x80104348 */
    e->re2z_part_life [7]  = 0x8000u;                        /* @0x80104330/34 */
}

/* ---- R1 Kanister-Explosionstod @0x80102158 ------------------------------------------------- */
static const int16_t k_ring[6][4] = {   /* SVECTOR-Tabelle @0x801000a8 */
    { 0, -2800, 0, 0 }, { 300, -2100, 0, 0 }, { 0, -700, 0, 0 },
    { 0, 0, 300, 0 }, { 0, -1400, -200, 0 }, { -500, -400, 200, 0 } };
static const uint32_t k_ring_a0[6] = { 0x030f2000u, 0x030f3000u, 0x030f3000u, 0x030f2000u, 0x030f2000u, 0x030f3000u };

static inline int16_t rnd8(void) { return (int16_t)(int8_t)(re15_re2_rand() & 0xffu); }

void re15_gr2_r1_start(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    gr2_se(g, 4);                                           /* `addiu a0,zero,4` @0x801021c4, jal @0x801021ec */
    s_cfbd8 |= 4u;                                          /* @0x801022a8-b4 (Bit 4) */
    g->ereignis = 1;                                        /* +0x227 = 1 @0x801022d0-d4 */
    g->kopf_az = 0; g->kiefer_az = 0; g->t1_stufe = 0;      /* @0x801022d8-e0 */
    g->r1_176 = 3678;                                       /* @0x801022c4-c8 */
    /* Kanister loesen: Weltlage aus der Matrix (@0x80102300-24), 9 Bilder sichtbar (@0x80102394) */
    if (s_kan.aktiv) {
        if (s_kan.angehaengt) gr2_kan_folgen(e);
        memcpy(s_kan.pos, s_kan.welt, sizeof s_kan.pos);
        memcpy(s_kan.rot, s_kan.welt_rot, sizeof s_kan.rot);
        s_kan.angehaengt = 0; s_kan.stufe = 13;
        /* +0x16B = 9 (@0x80102394) wird erst in P1 (ab dem NAECHSTEN Bild) gezaehlt; der Port-Takt
         * laeuft im R1-Start-Bild schon einmal -> +1, damit der Kanister wie in RE2 zehn Bilder
         * (P0 + neun P1-Bilder) sichtbar bleibt und im zehnten P1-Bild verschwindet. */
        s_kan.entfern_ctr = RE15_GR2_KAN_ENTFERN_F + 1;
    }
    re15_rumble_small(8, 3);                                /* FUN_8003947c(8,3) @0x80102328-30 */
    re15_rumble_ramp(50, 255, 0, 0);                        /* FUN_800395b8(50,255,0,0) @0x80102334-44 */
    gr2_kiefer_platzen(e); g->kiefer_rot = 1;               /* FUN_80104318(em, Part 7) @0x80102354-58 */
    g->fontaene = 1;                                        /* Callback +0x234 = FUN_80103eb4 @0x80102368 */
    /* Spritzring @0x80102374 -> 0x8010338c: 6 Effekte an der Kanister-Matrix */
    if (s_kan.aktiv)
        for (int k = 0; k < 6; k++) gr2_fx(k_ring_a0[k], 0, s_kan.rot, s_kan.pos, k_ring[k]);
    if (re15_gr2_raum_se_hook) re15_gr2_raum_se_hook(15);   /* Se_on(0x020f0001) @0x8010237c-88 */
    g->r1_zielyaw = (uint16_t)((e->rot_y + 160) & 0xfff);  /* @0x8010239c-a4 */
    g->r1_speed = -500; g->r1_schritt = 40;                 /* @0x8010246c-70, @0x8010247c-84 */
    g->r1_phase = 1;                                        /* +0x6 = 1 @0x80102468-78 */
    e->motion = 9; e->anim_frame = 0; e->anim_frac = 0;     /* Clip 9 @0x80102488-98 */
    if (bk) re15_clip_anchor_set_pub(e, &bk->skel, &bk->anim, 9, 0);
    re15_gr2_log("R1 start pos=(%d,%d) hp=%d", e->x, e->z, (int)e->hp);
}

static void gr2_fontaene_takt(gr2_t *g, re15_actor_t *e)
{
    if (g->fontaene == 0) return;
    int32_t r6[9], p6[3], r7[9], p7[3];
    int ok6 = gr2_bone_welt(e, 6, r6, p6), ok7 = gr2_bone_welt(e, 7, r7, p7);
    int16_t ofs[4];
    if (g->fontaene == 1 && ok6) {                          /* @0x80103f20-70: 0x26032000 an Part 6 */
        ofs[0] = (int16_t)(rnd8() * 2 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 4 + 200); ofs[3] = 0;
        gr2_fx(0x26032000u, 0, r6, p6, ofs);
    } else if (g->fontaene >= 2 && g->fontaene < 9) {       /* @0x80103f74-40a8 */
        if (ok7) {
            ofs[0] = (int16_t)(rnd8() * 4 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 2 + 200); ofs[3] = 0;
            gr2_fx(0x00064000u, (int16_t)(rnd8() * 16), r7, p7, ofs);
        }
        if (ok6) {
            ofs[0] = (int16_t)(rnd8() * 2 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 4 + 200); ofs[3] = 0;
            gr2_fx(0x00034000u, (int16_t)(rnd8() * 16), r6, p6, ofs);
            ofs[0] = (int16_t)(rnd8() * 2 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 2 + 200); ofs[3] = 0;
            gr2_fx(0x08024000u, (int16_t)(rnd8() * 16), r6, p6, ofs);
            gr2_fx(0x08024000u, (int16_t)(rnd8() * 16), r6, p6, ofs);
        }
    }
    if (g->fontaene == 64) g->fontaene = 0;                 /* @0x801040ac-bc: Callback aus */
    else g->fontaene++;
}

int re15_gr2_r1_tick(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (g->r1_phase == 0 || !bk) return 0;
    gr2_fontaene_takt(g, e);
    if (g->r1_phase == 1) {                                 /* @0x80102488 */
        int n = bk->anim.clips[9].frame_count;
        if (e->anim_frame == 31) { s_cfbd8 |= 4u; g->ereignis = 1; }   /* @0x8010252c-58 */
        /* Rueckstoss: +0x144 += Schritt, Schritt -= 2 (@0x8010257c-a0); FUN_800152c8(em, 0x800) = Vektor um
         * yaw+180 gedreht -> negatives +0x144 schiebt VORWAERTS. Auf den Anker, weil die Clip-Platzierung
         * absolut ist. */
        if (g->r1_schritt > 0) { g->r1_speed = (int16_t)(g->r1_speed + g->r1_schritt); g->r1_schritt = (int16_t)(g->r1_schritt - 2); }
        {
            int32_t v = q12mul(-(int32_t)g->r1_speed, gr2_scale(e));
            int32_t c = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
            e->anchor_x += q12mul(c, v); e->anchor_z -= q12mul(sn, v);
        }
        /* Ziel-Yaw +160 mit 75/Bild (FUN_8001569c @0x801025f8-608) */
        {
            int d = (((int)g->r1_zielyaw - (int)e->rot_y + 0x800) & 0xfff) - 0x800;
            if (d > 75) d = 75; else if (d < -75) d = -75;
            e->rot_y = (int16_t)((e->rot_y + d) & 0xfff);
        }
        if (g->r1_176 > 3478) g->r1_176 = (int16_t)(g->r1_176 - 20);   /* @0x8010260c-34 */
        e->motion = 9;
        re15_clip_root_motion_abs_pub(e, &bk->skel, &bk->anim, 9, (int)e->anim_frame);
        if (e->anim_frame + 1 >= n) { g->r1_phase = 2; return 1; }   /* FUN_8001a330 == 1 @0x8010249c */
        e->anim_frame++;
        return 1;
    }
    if (g->r1_phase == 2) { g->r1_phase = 3; return 1; }   /* @0x80102638-50 */
    /* P3 @0x80102654: +0x10E |= 0x4000, Raumflag 30 — Endzustand, Pose bleibt (Clip 9 letztes Bild) */
    if (!g->tot) { g->tot = 1; re15_gr2_log("R1 ende (Leiche) pos=(%d,%d)", e->x, e->z); }
    e->motion = 9; e->anim_frame = bk->anim.clips[9].frame_count - 1;
    return 0;
}
int re15_gr2_r1_laeuft(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? (s_g[slot].r1_phase != 0 && !s_g[slot].tot) : 0; }

/* ---- Bone-Zusaetze ------------------------------------------------------------------------- */
int re15_gr2_bone_az(int slot, int bone)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const gr2_t *g = &s_g[slot];
    if (bone == 6) return g->kopf_az;                       /* Part 6 az += +0x22A @0x8010037c-94 */
    if (bone == 7) return g->kiefer_az;                     /* Part 7 az += +0x228 @0x801003a8-cc (R1 setzt +0x228 = 0
                                                             * @0x801022dc; der Kiefer fliegt danach als Part-Record, s.o.) */
    return 0;
}
int re15_gr2_kiefer_geplatzt(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].kiefer_rot : 0; }

/* ---- Kanister-API ---------------------------------------------------------------------------- */
void re15_gr2_kanister_setzen(int32_t x, int32_t y, int32_t z)
{
    memset(&s_kan, 0, sizeof s_kan);
    s_kan.aktiv = 1; s_kan.stufe = 0;
    s_kan.pos[0] = x; s_kan.pos[1] = y; s_kan.pos[2] = z;
    /* liegend wie nach dem Fall: Dir_set (0x690, 0, 0xfc00) ROOM40A0 sub05 @0x048C */
    re15_skel_euler_matrix(0x690, 0, (int)(int16_t)0xfc00, s_kan.rot);
    gr2_kan_welt_setzen(s_kan.rot, s_kan.pos);
    re15_gr2_log("KANISTER gesetzt (%d,%d,%d)", x, y, z);
}
void re15_gr2_kanister_entfernen(void) { memset(&s_kan, 0, sizeof s_kan); }
int  re15_gr2_kanister_stufe(void) { return s_kan.aktiv ? (int)s_kan.stufe : -1; }
int  re15_gr2_kanister_welt(int32_t pos[3], int32_t rot[9])
{
    if (!s_kan.aktiv) return 0;
    if (s_kan.angehaengt) { memcpy(pos, s_kan.welt, 12); memcpy(rot, s_kan.welt_rot, 36); }
    else                  { memcpy(pos, s_kan.pos, 12);  memcpy(rot, s_kan.rot, 36); }
    return 1;
}
void re15_gr2_kanister_env(void)
{
    if (s_env_geprueft) return;
    s_env_geprueft = 1;
    const char *v = getenv("RE15_GATOR_KANISTER");
    if (!v || !*v) return;
    /* PORT-WAHL Hoehe: Mitte auf der Wasserkoerper-Hoehe des Beckens (-1200 = Wasser-INIT-Y der 0x23-KI
     * @0x8010c65c, GB_WATER_Y) - auf Leons Watt-Ebene (y=0, Mitte -267) laege der liegende Kanister komplett
     * unter der gemalten Wasseroberflaeche (Sichtlauf c 2026-10-23: Leons Beine werden dort von der Maske
     * geschnitten, der Kanister war unsichtbar). */
    int x = 0, z = 0, y = -1200;
    int n = sscanf(v, "%d,%d,%d", &x, &z, &y);
    if (n >= 2) re15_gr2_kanister_setzen(x, y, z);
}
int re15_gr2_geschwaecht(void) { for (int i = 0; i < RE15_ACTOR_MAX; i++) if (s_g[i].geschwaecht) return 1; return 0; }
int re15_gr2_treffer(void)     { int n = 0; for (int i = 0; i < RE15_ACTOR_MAX; i++) n += s_g[i].treffer; return n; }
int re15_gr2_tot(void)         { for (int i = 0; i < RE15_ACTOR_MAX; i++) if (s_g[i].tot) return 1; return 0; }
int re15_gr2_traegt(void)      { return s_kan.aktiv && s_kan.stufe == 7; }
