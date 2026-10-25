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
 * des RDT (gb_zone des Boss-Moduls) statt x<-17877 / z>=-7322; Putz-Bursts (ROOM40A0 sub14) um Leon
 * beim Dash-Start statt am Korridorende; Kanister-Aufnahme laengs der Blickachse statt laengs +Z.
 * (Nachbesserung 1: der Knick `yaw += 1024` nach Clip 3 ist RE2 - Dash @0x80100eb0-c0, Intro @0x80100938-48 -
 * und Dash/Intro haben kein Bahn-Gate mehr; die frueheren PORT-WAHLEN "kein yaw += 0x400" / "nur bei freier
 * Bahn" sind ueberholt, Abnahme 1 M4. Nachbesserung 2: Intro-Startlage = Pos_set-Zwilling, s. Intro.)
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
#include "re15_scd.h"          /* Nachtrag 4: g_scd (cam_id / cut_auto_enabled / work_vars) = Cut_chg-Zwilling */
#include "re15_room.h"         /* Nachtrag 4: g_room_rdt (RID-Saetze + RVD-Gruppen) */
#include "re15_camera.h"       /* Nachtrag 4: re15_camera_build_view (FUN_80053ca4-Zwilling) */
#include "re15_math.h"         /* Nachtrag 4: re15_squareroot0 */
#include "re15_engine.h"       /* Nachtrag 4: g_engine.frame_count (Bildnummer im Log) */
#include "re15_md1.h"          /* Nachbesserung 6 (M3): Huelle der Raum-Objekte (Obj_model_set-MD1 aus dem RDT) */
#include "re15_aot.h"          /* Nachbesserung 6 (M3): re15_prop_culled / re15_prop_render_y (Objekt-Zeichner FUN_8002c18c) */
#include "re15_collision.h"    /* Runde 41 Spur G, G-E1: Wandklemme in der Vorausschau des Todes-Clips (wie re15_gator_boss_tick) */

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
    uint8_t  dash_endbild;    /* voriger Tick zeigte das letzte Clip-Bild (FUN_8001a330 lieferte 1) */
    uint8_t  dash_vor;        /* Nachbesserung 2 (M2): Vordrehung auf dash_gier laeuft (PORT-WAHL, s. Dash-Planung) */
    int32_t  wand_max;        /* Nachbesserung 2 (M2, Messzeile): groesster Aussenabstand Rumpf/Hals/Kopf/Kiefer */
    int32_t  kopf_x_min;      /*   ... und kleinstes Kopf-x (Bone 6) waehrend Intro bzw. Dash */
    int16_t  dash_gier;       /* geplante Start-Gier des Dashes (-1 = keine) */
    uint8_t  intro_endbild;   /* dito fuer den Intro */
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
    uint8_t  ziel_bit;        /* Nachbesserung 3 (M3): +0x1C0 & 4 = Zielmodus "Kanister" (Anschwimm-Sperre), s. gr2_zielmodus_takt */
    /* Putz */
    int      putz_t;          /* 0 = aus, sonst Bild der Sequenz (1..90) */
    int32_t  putz_x, putz_y, putz_z;
    /* Nachbesserung 5 (Abnahme 4, M1): Finisher-Buehne der FRESSEN-P0 (re15_gr2_finisher_buehne) */
    uint8_t  fin_geplant;     /* Buehne gewaehlt (toedlicher Biss): fin_x/z/gier/cut */
    uint8_t  fin_aktiv;       /* P0 gelaufen: Gator steht auf fin_x/z/gier (Bezug der Messzeile Maul-Sitz) */
    int8_t   fin_cut;         /* Kamera der Buehne (-1 = keine Buehne gefunden, Wahl wie bisher) */
    int16_t  fin_gier;
    int32_t  fin_x, fin_y, fin_z;
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
    uint8_t  anhaengen;       /* Nachbesserung 4 (M1): Stufe 2 hat t == 13 erreicht (@0x801018c4-cc) - die Lokallage zur
                               * Part-6-Matrix rechnet re15_gr2_nachtakt aus der GEZEICHNETEN Pose dieses Bildes */
    int32_t  liege_z;         /* Nachbesserung 2 (M3): z beim Setzen = Lage, an der das Raumflag Bank 5 Bit 0
                               * ("liegt im Wasser", ROOM40A0 sub10 `Set 22 05 00 01` @SCD 0x0712, nie geloescht)
                               * gesetzt wird; ihre Beckenzone ist das Gegenstueck der RE2-Zone 1 (s. chase_clip) */
} s_kan;

static int s_env_geprueft;
/* Nachbesserung 2 (M3): Beckenzone des Boss-Moduls (gb_zone: 0 Sued, 1 Nord, 2 Westkanal-Streifen, PORT-WAHL der
 * RE2-Zonen wie beim Dash). Der Kanister-Block des RE2-Sub-1/2-Pre laeuft nur in RE2-Zone 1 (`lbu v1,538 / bne
 * v1,1,0x80100b20` @0x801009c8-d0) = dem Korridorbein, in dem der Kanister liegt. */
static int (*s_zone_fn)(int32_t z);
void re15_gr2_zonen_quelle(int (*zone_fn)(int32_t z)) { s_zone_fn = zone_fn; }
/* RE2-Bildtakt DAT_800dfc1a = VSync-Wert der Hauptschleife (FUN_8002b968: `lui a0,0x800e / lbu a0,-0x3e6(a0) /
 * jal VSync` @0x8002b990-98); im Spiel 2 (`li v0,0x2 / sb v0,0x800dfc1a` @0x8003b9a8-b0 = 30 Bilder/s), R1 P0 setzt 4
 * (`addiu v0,zero,4 / lui at,0x800e / sb v0,-998(at)` @0x8010235c-64 = 15 Bilder/s, Zeitlupe der Explosion), R1 P3
 * wieder 2 (`addiu v0,zero,2` @0x80102668, `sb v0,-998(at)` @0x8010267c). */
static uint8_t s_vsync = 2;
/* Nachtrag 4: Kamera-Programme (Definitionen vor dem Takt) */
static void gr2_kam_reset(void);
static int  gr2_kam_schnitt(int programm, int blick, const re15_actor_t *e, const re15_actor_t *opfer);
static void gr2_kam_r1_start(const re15_actor_t *e);
static void gr2_kam_zurueck(const re15_actor_t *e);
static void gr2_kam_frei(const re15_actor_t *e);
static int  gr2_kam_auto(void);

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

/* Nachbesserung 4 (Nebenbefund zu M1, Punkt 2 "gleiches Verhalten bei Animation"): RE2-UEBERBLENDUNG eines Clipwechsels
 * FUN_8001a330(em, clip, n, 0) (EXE, selbst gelesen): Clipwechsel -> +0x14E = n (`sb s1,334(s0)` @0x8001a364), Gewichtsschritt
 * = 4096 / (n + 1) im unteren Halbwort des 959c-Arguments (`addiu v1,s1,1 / addiu v0,zero,4096 / divu v0,v1 / mflo v0 / ...
 * or a3,v0,a3` @0x8001a37c-ac); 0x80029614 mischt mit Vorbild-Gewicht = Schritt x Zaehler (`lbu t3,334(s2) / andi v0,a3,0xffff
 * / mult v0,t3` @0x800296a8-bc, `subu v0,v0,t1` @0x800296ec) und zaehlt danach -1 (`lbu v0,334(s2) / addiu v0,v0,-1 /
 * sb v0,334(s2)` @0x800299c0-cc). Port: anim_frac = n, anim_blend_rate = 4096 / (n + 1) (skeleton_common.c: wp = anim_frac x
 * anim_blend_rate), der Abbau je Takt im Boss-Takt (re15_gator_boss_tick). Vorher stand nur anim_frac (Rate Default 0x200,
 * kein Abbau, Typ 0x23 ist selbst-vorschiebend): frac 4 = dauerhaft 50 % Vorbild, frac 16/32 = Pose eingefroren (gemessen
 * exe C3: Dash F645-F794 150 Bilder bitgleiche Pose). */
static void gr2_blend(re15_actor_t *e, int n)
{
    e->anim_frac = (uint8_t)n;
    e->anim_blend_rate = (uint16_t)(n > 0 ? 4096 / (n + 1) : 0);
}
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
/* QUELL-MATRIZEN der Effekte (Nachbesserung 1 M2, Fortsetzung 3 - gemessen: die Putz-Teilchen lagen bei
 * (67,-2000,2) .. (-634,-4000,-722) statt um Leon (-8438,0,-25538), quads 85/Bild, gezeichnet 0):
 * FUN_8001bf10 merkt sich den Matrix-ZEIGER a2 (`sw a2,108(t0)` @0x8001cd6c = re2_fx.c spawn_kern mtx_src) und
 * FUN_8001d894 kopiert ihn bei Status-Bit 0x800 JE BILD nach +0x4C (@0x8001d8ac-fc). RE2 uebergibt darum nur
 * DAUERHAFTE Matrizen: Sce_espr_on die Einheitsmatrix 0x8009db44 (FUN_80056a38(0) -> Tafel @0x80011470[0] ->
 * `lui v0,0x800a / addiu v0,v0,-9404` @0x80056a78-7c) und die Lage als Versatz a3 (`addiu a3,sp,16` @0x80056618,
 * SCD-Bytes 8..13 @0x800565ec-0c), die Fontaene die Part-Matrix des Modellblocks (`addiu a2,s2,1104` @0x80103f58),
 * der Spritzring die Kanister-Matrix. Der Port reichte einen STACK-Puffer (gr2_fx: lokales m[32]) -> haengender
 * Zeiger. Jetzt: je Quelle ein statischer Puffer, den der Takt laufend nachfuehrt (wie RE2 den Modellblock). */
enum { GR2_FXM_TEIL6, GR2_FXM_TEIL7, GR2_FXM_KANISTER, GR2_FXM_N };
static uint8_t s_fx_mtx[GR2_FXM_N][32];
static int     s_fx_ok;          /* Nachbesserung 4: Part-6/7-Puffer stammen aus einer gezeichneten Pose (re15_gr2_nachtakt) */
static int32_t s_p6_rot[9], s_p6_pos[3];   /* Messhaken: Part-6-Weltmatrix der letzten gezeichneten Pose */
static int gr2_fx(uint32_t a0, int16_t a1, const uint8_t mtx[32], const int16_t ofs[4])
{
    return re2fx_spawn_sofort(a0, a1, mtx, ofs);             /* FUN_8001bf10 = sofort lebendig */
}

/* ------------------------------------------------------------------------------------------ */
void re15_gr2_reset(int slot, re15_gr2_se_fn se)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    memset(&s_g[slot], 0, sizeof s_g[slot]);
    s_g[slot].se = se;
    s_g[slot].zone_alt = -1;
    s_g[slot].dash_gier = -1;
    s_g[slot].letzter_clip = 0;
    s_g[slot].fin_cut = -1;                                 /* Nachbesserung 5: keine Finisher-Buehne */
    s_cfbd8 = 0;                                            /* Raumstart: keine Schuettel-Bits */
    s_vsync = 2;                                            /* Raumstart: Spieltakt VSync(2) @0x8003b9a8-b0 */
    gr2_kam_reset();                                        /* Nachtrag 4: keine offene Kamera-Sperre (Raum neu geladen) */
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
        /* Sce_espr_on: FUN_8001bf10(a0, Richtung, 0x8009db44, &{x,y,z}) @0x800565a4-0x80056618 */
        int16_t ofs[4] = { (int16_t)pos[0], (int16_t)pos[1], (int16_t)pos[2], 0 };
        if (gr2_fx(a0, (int16_t)k_putz[i].dir, re2fx_einheitsmatrix, ofs) >= 0) {
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

/* Angehaengt: Welt = Part-6-Matrix x (rel, s*lokal). Nachbesserung 4 (M1): die Part-6-Matrix ist die der GEZEICHNETEN Pose
 * dieses Bildes (re15_gr2_nachtakt, nach Bildvorschub, Wende, Vortrieb und Wand-Klemme) - RE2 haengt den Kanister als Kind an
 * die Part-6-Weltmatrix (`addiu a0,s0,1104` @0x801018d0, `sw a0,128(s2)` @0x801018e0). Vorher rechnete der Takt-Anfang die
 * Lage aus der Pose des VORBILDS (Abnahme 3: bis 637 Einheiten je Bild hinter dem Kopf). */
static void gr2_kan_folgen(const re15_actor_t *e, const int32_t r6[9], const int32_t p6[3])
{
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

/* Nachbesserung 3 (Abnahme 2 M3): RE2-ANSCHWIMM-SPERRE = Zielmodus +0x1C0 & 4 (EM23_OVL_0000.BIN, Sub-1/2-Pre, selbst gelesen):
 * nur in Zone 1 (`lbu v1,538 / bne v1,1,0x80100b20` @0x801009c8-d0, vor der Zone-2-Linie @0x801009d8-e4, bei +0x22C == 0
 * @0x80100a0c-14):
 *   (a) Bank 5 Bit 1 (`jal 0x80077360` a0 = 0x800d4874, a1 = 1 @0x80100a1c-2c; ROOM40A0 sub10 `Set 22 05 01 01` @SCD 0x0659 =
 *       Kanister faellt/liegt, nie geloescht) und Stufe < 2 (`lbu v0,550 / sltiu v0,v0,0x2` @0x80100a34-3c) -> `+0x1C0 |= 4`
 *       (`ori v0,v0,0x4 / sb v0,448(s0)` @0x80100a54-5c), Hals-Ziel (x, y - 800, obj[1].z) @0x80100a4c-70;
 *   (b) danach, auch ohne (a): obj[1].z - z < 8880 (`addiu v0,v0,-9000 / slti v0,v0,-120` @0x80100a88-8c) UND Stufe 0
 *       (`lbu v0,550 / bne v0,zero` @0x80100a98-a0) -> `+0x1C0 &= 0xfb` (@0x80100aa8-b4);
 *   (c) beim Anhaengen geloescht (@0x801018e4-f4, gr2_kan_takt Stufe 2).
 * Solange das Bit steht, schaerft FUN_801012fc die Biss-Maschine T1 nicht (`lbu v0,448(a0) / andi v0,v0,0x4 / bne v0,zero,
 * 0x80101380` @0x8010134c-58) -> re15_gr2_biss_erlaubt. Port: Zone 1 = Beckenzone der Liegestelle (PORT-WAHL wie die
 * Hysterese-Ruhe, gb_zone), obj[1].z - z = Lage laengs der Blickachse (gr2_kan_vor), 8880 x Zeichen-Skala wie das
 * Fenster 8601/0x18f; Bank 5 Bit 1 = Kanister im Raum (re15_gr2_kanister_setzen legt ihn liegend hin). PORT-WAHL: ausserhalb
 * der Kanister-Zone ist das Bit aus - RE2 verlaesst Zone 1 nur ueber die Zone-2-Linie z >= -7322 (@0x801009e0), dort liegt
 * der Kanister (z -9500, sub05 Pos_set @SCD 0x0484) laengst HINTER der 8880-Grenze, d.h. RE2 traegt das Bit nie aus Zone 1
 * hinaus; im Becken fuehrt die Verfolgung den Gator zurueck in andere Zonen. Der Hals-Zielmodus selbst (Root @0x80100344-60
 * -> FUN_800177c0 statt FUN_80017d98) ist OFFEN (Dossier §11.3): der Port fuehrt die RE2-Halsfuehrung des Gators nicht. */
static void gr2_zielmodus_takt(gr2_t *g, const re15_actor_t *e, int im_chase)
{
    if (!s_kan.aktiv || s_kan.stufe == 13) { g->ziel_bit = 0; return; }
    if (!im_chase) { g->ziel_bit = 0; return; }              /* PORT-WAHL: nur in der Schwimm-Verfolgung (RE2 Sub 1/2) - die
                                                               * Nutzer-Design-Phasen GUARD/CROSS haben kein RE2-Gegenstueck */
    if (!s_zone_fn || s_zone_fn(e->z) != s_zone_fn(s_kan.liege_z)) { g->ziel_bit = 0; return; }
    int32_t laengs, quer; gr2_kan_vor(e, &laengs, &quer);
    int alt = g->ziel_bit;
    if (s_kan.stufe < 2) g->ziel_bit = 1;                                              /* (a) @0x80100a34-5c */
    if (s_kan.stufe == 0 && laengs < q12mul(9000 - 120, gr2_scale(e))) g->ziel_bit = 0;   /* (b) @0x80100a74-b4 */
    if (g->ziel_bit != alt)
        re15_gr2_log("KANISTER Zielmodus %d (Anschwimm-Sperre) laengs=%d Stufe %d F%u", (int)g->ziel_bit, (int)laengs,
                     (int)s_kan.stufe, (unsigned)g_engine.frame_count);
}

static void gr2_kan_takt(gr2_t *g, re15_actor_t *e, int im_chase)
{
    gr2_zielmodus_takt(g, e, im_chase);
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
            /* Nachbesserung 2 (Abnahme 1, M3): RE2 schaltet im selben Pre PADDELN (Sub 2) auf SCHWIMMEN (Sub 1):
             * `lbu v1,5(s0) / sb v0(1),550(s0) / addiu v0,zero,2 / bne v1,v0,0x80100c08 / addiu v0,zero,257 /
             * j 0x80100c04` @0x80100b04-18 -> `sw v0(0x101),4(s0)` @0x80100c04. Der Dispatcher liest +5 nach dem
             * Pre neu (`lbu v0,5(a0)` @0x801007b8) und ruft im SELBEN Tick Sub-1-Main @0x80100c20 = FUN_8001a330(em,
             * Clip 0, 32, 0) @0x80100c34-40 -> der Clipwechsel mit Bild 0 / Blend 32 passiert im naechsten
             * re15_gr2_chase_clip dieses Ticks (letzter_clip 0 != motion 1). */
            int sub_vor = g->letzter_clip ? 2 : 1;
            if (g->letzter_clip == 1) g->letzter_clip = 0;
            re15_gr2_log("KANISTER Stufe 1 laengs=%d quer=%d sub %d -> 1 F%u", laengs, quer, sub_vor,
                         (unsigned)g_engine.frame_count);
            /* Nachbesserung 3 (M2): KEIN return - RE2 ruft T2 im selben Takt nach dem Pre (Root: Routine @0x80100200,
             * dann `jal 0x801017c4` @0x80100320), Stufe 1 laeuft dort gleich mit (unten). */
        } else return;
    }
    int32_t s = gr2_scale(e);
    if (s_kan.stufe == 1) {
        /* Stufe 1 @0x8010183c-5c: Raumflag 29 (`jal 0x8007730c` a1 = 29), t = 0 (`sh zero,346(s1)`), Stufe 2 (`lbu/addiu/sb
         * v0,550(s1)`) und OHNE Sprung weiter in den Stufe-2-Code @0x80101860 (Tafel @0x80100024: [1] 0x8010183c, [2]
         * 0x80101860) = der erste Stufe-2-Schritt (t = 0) im selben Takt. Nachbesserung 3 (M2): vorher ein eigener Takt
         * (break) - die Aufnahme dauerte zwei Bilder laenger als RE2. */
        g->ease_t = 0; s_kan.stufe = 2;
    }
    switch (s_kan.stufe) {
    case 2: {                                               /* @0x80101860 */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(13, g->ease_t, 240));   /* @0x80101860-6c */
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(13, g->ease_t, -320));  /* @0x80101870-88 */
        g->ease_t++;
        /* Kanister bleibt >= 8100 (x Scale) vor der Wurzel (@0x801018a4-c0: `lw v0,64(s2) / lw v1,64(s1) / subu / slti v0,v0,
         * 8100 / beq v0,zero,+ / addiu v0,v1,8100 / sw v0,64(s2)` = obj[1].z = z + 8100, NUR die Korridor-Achse, x
         * unberuehrt). Nachbesserung 3 (M2): der Schub laeuft laengs der Gier, die seit dem Fenstertreffer GESPERRT ist
         * (re15_gr2_aufnahme_lenksperre: RE2 hat in Sub 1/2 und T2 keinen Gier-Schreiber) - also geradeaus. */
        int32_t laengs, quer; gr2_kan_vor(e, &laengs, &quer);
        int32_t vh = q12mul(RE15_GR2_KAN_VORHALT, s);
        if (laengs < vh) {
            int32_t c = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
            s_kan.pos[0] += q12mul(c, vh - laengs);
            s_kan.pos[2] -= q12mul(sn, vh - laengs);
        }
        if (g->ease_t == 13) {                              /* @0x801018c4-cc */
            /* Nachbesserung 4 (M1): FUN_8002d030(Part-6-Matrix, Kanister-Matrix) @0x801018f0 rechnet RE2 gegen die Part-6-
             * WELTmatrix (+0x48, `addiu a0,s0,1104` @0x801018d0) - im Port die Matrix der gezeichneten Pose DIESES Bildes, also
             * erst am Takt-Ende (re15_gr2_nachtakt). Der Kanister liegt bis dahin unbewegt (s_kan.pos), im selben Bild
             * gezeichnet = Part6(Bild) x lokal = s_kan.pos: kein Sprung beim Anhaengen. */
            s_kan.anhaengen = 1;
            g->ziel_bit = 0;                        /* NB3 (M3): `lbu v0,448(s1) / andi v0,v0,0xfb / sb v0,448(s1)` @0x801018e4-f4 */
            g->ease_t = 0; s_kan.stufe = 3;                 /* @0x801018dc, 0x80101964-6c */
        }
        break; }
    /* Nachbesserung 3 - QUER-AUSGLEICH (PORT-WAHL): RE2 fuehrt nach dem Anhaengen nur die Lokallage x
     * (+0x38 -> 1750) und y (+0x3C -> -210) heran (Stufen 3-7 @0x801019b4-0x80101b8c, kein Schreiber auf +0x40) - die
     * Querlage z bleibt, wie sie beim Anhaengen war. Im RE2-Korridor ist sie klein: der Kanister liegt nach dem Fall bei
     * x -22816 (sub05 Pos_set @SCD 0x0484), der Gator schwimmt Bein 2 auf x -23078 (@0x80101048) = 262 neben der Achse, und
     * das Fenster fragt nur laengs ab (@0x80100ae4-ec). Das Port-Fenster laesst den Kanister bis Koerperradius + 267 seitlich
     * zu (s. Fenster oben, "dran vorbei kommt") - ohne Ausgleich hing er dann bis ~1600 neben dem Kopf (gemessen exe C3 NB3:
     * lokal z 2372 = 1581 Welt). Port: z wird mit DENSELBEN Schritten wie x (70/145/100/100) auf die Symmetrieebene des
     * Kopfes (z 0) gefuehrt, so endet der Kanister wie in RE2 zwischen den Kiefern. */
    case 3:                                                 /* @0x80101970: ease-in 6 (-140, -60) */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(6, g->ease_t, -140));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(6, g->ease_t, -60));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 70);             /* @0x801019c4-cc */
        gr2_annaehern(&s_kan.lokal[2], 0, 70);                                /* NB3 Quer-Ausgleich (PORT-WAHL, s. Quer-Ausgleich vor case 3) */
        if (g->ease_t == 6) { g->ease_t = 0; s_kan.stufe = 4; }               /* @0x801019d0-d8, 0x80101b3c-54 */
        break;
    case 4:                                                 /* @0x801019dc: ease-out 6 */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_out(6, g->ease_t, -140));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_out(6, g->ease_t, -60));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 145);            /* @0x80101a30-38 */
        gr2_annaehern(&s_kan.lokal[2], 0, 145);                               /* NB3 Quer-Ausgleich, Schritt wie x */
        gr2_annaehern(&s_kan.lokal[1], RE15_GR2_KAN_LOKAL_Y, 25);             /* @0x80101a4c-54 */
        if (g->ease_t == 6) { g->ease_t = 0; s_kan.stufe = 5; }
        break;
    case 5:                                                 /* @0x80101a64: ease-in 7 (+75, +100) */
        g->kopf_az   = (int16_t)(g->kopf_az   + gr2_ease_in(7, g->ease_t, 75));
        g->kiefer_az = (int16_t)(g->kiefer_az + gr2_ease_in(7, g->ease_t, 100));
        g->ease_t++;
        gr2_annaehern(&s_kan.lokal[0], RE15_GR2_KAN_LOKAL_X, 100);            /* @0x80101ab8-c0 */
        gr2_annaehern(&s_kan.lokal[2], 0, 100);                               /* NB3 Quer-Ausgleich, Schritt wie x */
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
        gr2_annaehern(&s_kan.lokal[2], 0, 100);                               /* NB3 Quer-Ausgleich, Schritt wie x */
        gr2_annaehern(&s_kan.lokal[1], RE15_GR2_KAN_LOKAL_Y, 30);             /* @0x80101b84-8c */
        break;
    default: break;
    }
    /* Nachbesserung 4 (M1): die Weltlage folgt erst am Takt-Ende der gezeichneten Part-6-Matrix (re15_gr2_nachtakt). */
}

/* ==== Nachtrag 4: Kamera-Programme = das RE2-Kameraskript des Alligator-Kampfes (ROOM40A0) auf ROOM2090 ====
 * FORM = RE2 (Dossier §9.1, alles selbst gelesen):
 *   - Schnitt FUN_8002c7ac(cut) (`DAT_800cfbf2 = cut`), Sperre der Zonenkamera 0x800cfbd8 |= 0x100 (die
 *     Hauptschleife ruft FUN_8002c6b4 nur bei Bit 0x100 = 0: `andi v0,v0,0x100 / bne` @0x8002639c-a0).
 *   - INTRO: Clip 6 laeuft in Cut 10 an (ROOM40A0 sub02 `29 0a` @SCD 0x00E0, 78 Bilder vor Evt_exec sub11),
 *     Bild 70 -> Cut 0 (`addiu v0,zero,70 / bne v1,v0` @0x80100894-98, jal @0x801008a0), Clip-6-Ende -> Cut 1
 *     (@0x801008bc-c8), Clip-3-Ende -> Bits 0x110 frei (`and v0,v0,-17 / and v0,v0,-257` @0x801008f8-918).
 *   - R1: Sperre (`ori v0,v0,0x100` @0x8010220c); Cut 5 nur, wenn der aktuelle Cut 6/13 ist (@0x80102218-24) und
 *     z < -13300 (@0x8010222c-34) = dort, wo der Kopf NICHT im Bild ist (Probe: Kopf in Cut 6 bei Bild-x -2);
 *     dann +0x218 |= 0x40 (@0x80102240-4c), am Clip-9-Ende zurueck auf 6/13 (@0x801024a4-ec); P3 Sperre frei
 *     (`addiu v1,zero,-257 / and / sw` @0x80102688-94).
 *   - FRESSEN P0: Sperre (`ori v0,v0,0x100` @0x80100fcc) + Cut 7 bzw. 14 (@0x80100fd4-ff4), kein Ende.
 *   - Kein Schnitt in Dash, R4, R0, Kanister-Aufnahme (T2 0x801017c4-0x80101d40) - Zensus der 9 Rufe.
 * ZUORDNUNG = PORT-WAHL (Nutzer-Nachtrag 4: "je RE2-Schnitt die 2090-Kamera mit gleicher Blickbeziehung"):
 *   Kandidaten = die ZONENKAMERAS des Raums (Cuts mit RVD-Gruppe; ROOM2090: 0..7, die einzigen mit Masken -
 *   die Sonderkameras 8..12 sind Nahaufnahmen ohne Zonen); Bedingung = Kopf (Bone 6) im Bild (Projektion wie
 *   der Zeichner, re15_camera_build_view, sx = 160 + H*x/z in 0..319, sy in 0..239, Tiefe > 0) UND Bildmassstab
 *   m = H * Skala / Tiefe des Kopfes nicht groesser als der groesste RE2-Schnittmassstab (Cut 0 bei Clip-6-Bild 70:
 *   H 257 / Tiefe 8898; die sechs RE2-Schnittbilder liegen bei 927..1892 in m * 65536) - RE2 schneidet an keiner
 *   Stelle auf eine naehere Aufnahme (Nachbesserung 1 Fortsetzung 3, gemessen: ohne die Grenze schnitt die
 *   EXPLOSION im Spiel auf Cut 2 mit dem Kopf 3201 vor der Linse, m 2839 = 2,5-facher RE2-Cut-5-Massstab, der Leib
 *   fuellte das Bild); Wahl = kleinster Abstand |frontal - soll|, frontal = cos(Blickachse Bone 1 -> Bone 6,
 *   Kopf -> Kamera) in Q12. Findet sich unter der Grenze keine Kamera, entfaellt sie (dann erst das Opfer). Das Soll je
 *   RE2-Schnitt ist an der RE2-Lage des Schnittbildes gemessen (tools/r41_re2gator/probe_kamera_re2.c; Lagen:
 *   Pos_set sub11 @SCD 0x0736 + R*off6[f] (FUN_80015e7c/152c8), Clip-3-Anker z -21212 @0x801008d0, R1 Bein 2 mit
 *   Kanister-Fenster @0x80100ae4-ec, FRESSEN x -23078 @0x80101048 / Leon x+10643, -915 @0x8010106c-84).
 *   Rueckkehr am Programm-Ende = Cut_old-Zwilling (FUN_8004032c) auf den Zonen-Cut VOR dem Programm (RE2 kehrt
 *   in R1 genauso auf 6/13 zurueck; im Intro war Cut 1 dort selbst die Zonenkamera fuer Leon). */
/* soll = frontal (Q12) des RE2-Schnittbildes; re2_h/re2_tiefe = Projektionsabstand H (RID fov >> 7) und Kopf-Tiefe
 * des RE2-Schnittbildes -> RE2-Bildmassstab m = H * 1 / Tiefe (RE2 zeichnet den EM23 1x); alles gemessen mit
 * tools/r41_re2gator/probe_kamera_re2.c (gcc -std=c11 -DRE15_PLATFORM_PC=1 gegen libre15_engine.a, Dossier §9.2/§9.3). */
static const struct { int16_t re2_cut; int16_t soll; int16_t re2_h; int16_t re2_tiefe; uint8_t opfer; const char *name; } k_blick[6] = {
    { -1,    0,   0,     1, 0, "-" },
    { 10, 4092, 208, 11734, 0, "DURCHBRUCH" },   /* Clip 6 f0, Cut 10: frontal +1.00 (3 Grad), Kopf bei Bild (140,126), RID 10 fov 26684 */
    {  0, 1445, 257,  8898, 0, "SEITE" },        /* Clip 6 f70, Cut 0: +0.35 (69 Grad), Bild (145,116), RID 0 fov 32946 */
    {  1, 3386, 231, 16318, 0, "ANGRIFF" },      /* Clip 6 f124, Cut 1: +0.83 (34 Grad), Bild (183,109), RID 1 fov 29623; in Clip 3 bis +0.99 */
    {  5, 4046, 231, 13113, 0, "EXPLOSION" },    /* R1 Clip 9 f0, Cut 5: +0.99 (9 Grad), Bild (170,111), RID 5 fov 29623 */
    {  7, 4094, 231, 11175, 1, "FINISHER" },     /* FRESSEN P0 Clip 4 f13, Cut 7: +1.00 (2 Grad), Leon im Bild (186,186), RID 7 fov 29623 */
};
/* FINISHER: das Opfer haengt ab P1 ueber das Anker-Paar im Maul (@0x80101110-34, Port gb FRESSEN P1/P2), also
 * muss neben dem Kopf der KIEFER (Part 7) im Bild liegen. Leons P0-Lage (Gator + R*(10643 x s, -915) @0x8010106c-84)
 * taugt dafuer nicht: aus der Tuernische liegt sie hinter der Suedwand (gemessen unit_r41_re2gator_kamera:
 * z -29941 bei Wand -27000), gezeichnet wird Leon dort erst ab P1 im Maul. */
enum { GR2_KAM_INTRO = 1, GR2_KAM_R1 = 2, GR2_KAM_FRESSEN = 3, GR2_KAM_SZENE = 4 };
static const char *const k_kam_prog[5] = { "-", "INTRO", "R1", "FINISHER", "SZENE" };
#define GR2_KAM_FOLGE 24
static struct {
    uint8_t  aktiv;              /* Sperre gesetzt (RE2 0x800cfbd8 |= 0x100) */
    uint8_t  programm;           /* GR2_KAM_* */
    uint8_t  r1_geschnitten;     /* RE2 +0x218 |= 0x40 @0x80102248 */
    int16_t  vorher;             /* Zonen-Cut vor dem Programm (-1 = keiner) */
    int      n;                  /* Messhaken: Schnittfolge */
    int8_t   cut[GR2_KAM_FOLGE], blick[GR2_KAM_FOLGE];
    int16_t  clip[GR2_KAM_FOLGE], bild[GR2_KAM_FOLGE];
} s_kam;
static uint8_t s_kam_aus;        /* re15_gr2_kamera_auto(0): Szene haelt die Kamera selbst */

/* Kopf (Bone 6) und Blickachse (Bone 1 -> Bone 6, xz) der aktuellen Pose (Kette 1->5->6, Memory gator-bosskampf). */
static int gr2_kopf_achse(const re15_actor_t *e, int32_t kopf[3], int32_t achse[2])
{
    int32_t r[9], p1[3];
    if (!gr2_bone_welt(e, 6, r, kopf) || !gr2_bone_welt(e, 1, r, p1)) return 0;
    achse[0] = kopf[0] - p1[0]; achse[1] = kopf[2] - p1[2];
    return 1;
}
/* Punkt im Bild von Kamera c? Projektion wie der Zeichner (re15_camera_build_view = FUN_80053ca4-Zwilling,
 * sx = 160 + H*x/z, sy = 120 + H*y/z, H = fov >> 7). */
static int gr2_im_bild(const re15_camera_cut_t *c, const int32_t p[3], int *sx, int *sy)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(c, &v) != 0) return 0;
    int64_t px = p[0], py = p[1], pz = p[2];
    int32_t vx = (int32_t)(((int64_t)v.rot[0]*px + (int64_t)v.rot[1]*py + (int64_t)v.rot[2]*pz) >> 12) + v.trans[0];
    int32_t vy = (int32_t)(((int64_t)v.rot[3]*px + (int64_t)v.rot[4]*py + (int64_t)v.rot[5]*pz) >> 12) + v.trans[1];
    int32_t vz = (int32_t)(((int64_t)v.rot[6]*px + (int64_t)v.rot[7]*py + (int64_t)v.rot[8]*pz) >> 12) + v.trans[2];
    if (vz <= 0) return 0;
    int x = 160 + (int)(((int64_t)vx * v.fov_screen_dist) / vz);
    int y = 120 + (int)(((int64_t)vy * v.fov_screen_dist) / vz);
    if (sx) *sx = x;
    if (sy) *sy = y;
    return x >= 0 && x < 320 && y >= 0 && y < 240;
}
/* frontal (Q12) = cos(Blickachse, Kopf -> Kamera) in xz. */
static int gr2_frontal(const int32_t kopf[3], const int32_t achse[2], const re15_camera_cut_t *c)
{
    int64_t cx = (int64_t)c->pos_x - kopf[0], cz = (int64_t)c->pos_z - kopf[2];
    uint32_t lc = re15_squareroot0((uint32_t)(cx * cx + cz * cz));
    uint32_t la = re15_squareroot0((uint32_t)((int64_t)achse[0] * achse[0] + (int64_t)achse[1] * achse[1]));
    if (lc == 0 || la == 0) return 0;
    return (int)((((int64_t)achse[0] * cx + (int64_t)achse[1] * cz) * 4096) / ((int64_t)la * lc));
}
/* Zonenkamera = Cut, dessen RVD-Gruppe ausser dem Such-Anker (erster Satz je cam_from, rdt_common.c) mindestens
 * einen Wechselsatz hat. ROOM2090: Cuts 0..7 je 3 Saetze; die Sonderkameras 8..12 tragen nur den Anker (RVD
 * @0x400-0x450, Ziel 0) und haben keine Masken (MASKS/ROOM2090_PRI00..07). */
static int gr2_zonenkamera(int cut)
{
    int n = 0;
    for (int i = 0; i < g_room_rdt.zone_count; i++)
        if ((int)g_room_rdt.zones[i].cam_from == cut) n++;
    return n >= 2;
}

/* Bildmassstab m des Punkts p in Kamera c (Bildpunkte je Modelleinheit, hier als m * 65536):
 * H * Skala / Tiefe, H = fov_screen_dist (FUN_80053ca4-Zwilling), Skala = Zeichen-Skala des Gators (Q12). 0 = hinter
 * der Kamera. RE2: H * 1 / Tiefe (EM23 1x). */
static int32_t gr2_massstab(const re15_camera_cut_t *c, const int32_t p[3], int32_t skala_q12)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(c, &v) != 0) return 0;
    int32_t vz = (int32_t)(((int64_t)v.rot[6]*p[0] + (int64_t)v.rot[7]*p[1] + (int64_t)v.rot[8]*p[2]) >> 12) + v.trans[2];
    if (vz <= 0) return 0;
    return (int32_t)(((int64_t)v.fov_screen_dist * skala_q12 * 16) / vz);   /* (H * s/4096) / vz * 65536 */
}
static int32_t gr2_massstab_soll(int blick)
{
    return (int32_t)(((int64_t)k_blick[blick].re2_h * 65536) / k_blick[blick].re2_tiefe);
}
/* Groesster RE2-Schnittmassstab (m * 65536) = Cut 0 bei Clip-6-Bild 70: H 257 (RID 0 fov 32946 >> 7) / Kopf-Tiefe
 * 8898 (probe_kamera_re2.c) -> 1892; die uebrigen RE2-Schnittbilder: Cut 10 1161, Cut 1 927, Cut 5 1154, Cut 7 1354. */
static int32_t gr2_massstab_grenze(void)
{
    int32_t g = 0;
    for (int b = RE15_GR2_BLICK_DURCHBRUCH; b <= RE15_GR2_BLICK_FINISHER; b++)
        if (gr2_massstab_soll(b) > g) g = gr2_massstab_soll(b);
    return g;
}
static uint8_t s_kam_log;        /* gr2_kam_schnitt: Kandidatenzeilen in gator_boss.log (Messzeilen, Dossier §9.4) */
static uint8_t s_wahl_ohne_laufend;   /* Nachbesserung 6 (M2): Rueckfall-Wahl des Finishers ohne Buehne nimmt die laufende Kamera nicht */

int re15_gr2_kamera_wahl(int blick, const re15_actor_t *gator, const re15_actor_t *opfer, int *frontal)
{
    if (frontal) *frontal = 0;
    if (blick < RE15_GR2_BLICK_DURCHBRUCH || blick > RE15_GR2_BLICK_FINISHER || !gator) return -1;
    if (!g_room_rdt_ok || !g_room_rdt.cuts || g_room_rdt.cut_count <= 0) return -1;
    int32_t kopf[3], achse[2];
    if (!gr2_kopf_achse(gator, kopf, achse)) return -1;
    int32_t kiefer[3] = { 0, 0, 0 }, op[3] = { 0, 0, 0 }, r[9];
    int mit_opfer = k_blick[blick].opfer && gr2_bone_welt(gator, 7, r, kiefer);
    if (opfer) { op[0] = opfer->x; op[1] = opfer->y; op[2] = opfer->z; }   /* Szene: Opfer-Fusspunkt zusaetzlich */
    int32_t skala = gr2_scale(gator), grenze = gr2_massstab_grenze();
    /* Stufen: 0 = Kopf (+ Kiefer/Opfer) im Bild + Massstab-Grenze, 1 = ohne Grenze, 2 = nur Kopf + Grenze, 3 = nur Kopf */
    for (int stufe = (mit_opfer || opfer) ? 0 : 2; stufe < 4; stufe++) {
        int mit_o = stufe < 2, mit_g = !(stufe & 1);
        int best = -1, best_d = 0x7fffffff, best_f = 0;
        for (int k = 0; k < g_room_rdt.cut_count; k++) {
            if (!gr2_zonenkamera(k)) continue;
            if (s_wahl_ohne_laufend && (k == (int)g_scd.work_vars[0x0A] || k == (int)g_scd.cam_id)) continue;   /* NB6 (M2) */
            const re15_camera_cut_t *c = &g_room_rdt.cuts[k];
            int sx = -9999, sy = -9999, im = gr2_im_bild(c, kopf, &sx, &sy);
            int kim = (mit_o && mit_opfer) ? gr2_im_bild(c, kiefer, NULL, NULL) : 1;
            int oim = (mit_o && opfer) ? gr2_im_bild(c, op, NULL, NULL) : 1;
            int32_t m = gr2_massstab(c, kopf, skala);
            int gok = !mit_g || m <= grenze;
            int f = gr2_frontal(kopf, achse, c);
            int d = f - k_blick[blick].soll; if (d < 0) d = -d;
            if (s_kam_log)
                re15_gr2_log("KAMERA-KANDIDAT %s stufe=%d Cut %d kopf_bild=(%d,%d)%s%s%s massstab=%d%s frontal=%d |f-soll|=%d",
                             k_blick[blick].name, stufe, k, sx, sy, im ? "" : " draussen", kim ? "" : " -Kiefer",
                             oim ? "" : " -Opfer", (int)m, gok ? "" : " >grenze", f, d);
            if (!im || !kim || !oim || !gok) continue;
            if (d < best_d) { best = k; best_d = d; best_f = f; }   /* Gleichstand: kleinerer Cut */
        }
        if (best >= 0) { if (frontal) *frontal = best_f; return best; }
    }
    return -1;
}

static void gr2_kam_merken(int cut, int blick, const re15_actor_t *e)
{
    if (s_kam.n >= GR2_KAM_FOLGE) return;
    s_kam.cut[s_kam.n] = (int8_t)cut; s_kam.blick[s_kam.n] = (int8_t)blick;
    s_kam.clip[s_kam.n] = e ? (int16_t)e->motion : -1; s_kam.bild[s_kam.n] = e ? (int16_t)e->anim_frame : -1;
    s_kam.n++;
}
/* RE1.5 Cut_chg-Zwilling (scd_vm.c op_cut_chg, LAB_800402a0): angeforderter Cut + Work-Vars + Bit 0x100. */
static void gr2_kam_cut_chg(int cut)
{
    g_scd.cam_id_prev        = (uint8_t)g_scd.work_vars[0x0A];   /* @0x800402c0 / @0x800402e4 */
    g_scd.cam_id             = (uint8_t)cut;
    g_scd.cam_arg2           = 0;
    g_scd.cam_arg3           = 0;
    g_scd.cam_change_pending = 1;
    g_scd.work_vars[0x0C]    = g_scd.work_vars[0x0A];          /* @0x800402ec */
    g_scd.work_vars[0x0A]    = (int16_t)cut;                   /* @0x800402fc */
    g_scd.cut_auto_enabled   = 0;                              /* @0x800402d4 `ori ...,0x100` */
}
static void gr2_kam_sperre(int programm)
{
    if (!s_kam.aktiv) { s_kam.vorher = (int16_t)g_scd.cam_id; s_kam.aktiv = 1; }
    s_kam.programm = (uint8_t)programm;
    g_scd.cut_auto_enabled = 0;                                /* RE2 0x800cfbd8 |= 0x100 */
}
static int gr2_kam_schnitt(int programm, int blick, const re15_actor_t *e, const re15_actor_t *opfer)
{
    gr2_kam_sperre(programm);
    s_kam_log = 1;                                             /* Kandidatenzeilen (Messung §9.4) */
    int f = 0, cut = re15_gr2_kamera_wahl(blick, e, opfer, &f);
    s_kam_log = 0;
    if (cut >= 0) gr2_kam_cut_chg(cut);
    gr2_kam_merken(cut, blick, e);
    int32_t kopf[3], achse[2], m = 0;
    if (cut >= 0 && e && gr2_kopf_achse(e, kopf, achse)) m = gr2_massstab(&g_room_rdt.cuts[cut], kopf, gr2_scale(e));
    re15_gr2_log("KAMERA %s %s RE2-Cut %d -> Cut %d F%u clip=%d bild=%d frontal=%d soll=%d massstab=%d re2_m=%d grenze=%d",
                 k_kam_prog[programm], k_blick[blick].name, (int)k_blick[blick].re2_cut, cut,
                 (unsigned)g_engine.frame_count, e ? (int)e->motion : -1, e ? (int)e->anim_frame : -1,
                 f, (int)k_blick[blick].soll, (int)m, (int)gr2_massstab_soll(blick), (int)gr2_massstab_grenze());
    return cut;
}
/* Cut_old-Zwilling (FUN_8004032c: cam_id = gemerkter Cut, Work-Vars) auf den Zonen-Cut VOR dem Programm. */
static void gr2_kam_zurueck(const re15_actor_t *e)
{
    if (s_kam.vorher < 0 || (int)g_scd.cam_id == s_kam.vorher) return;
    gr2_kam_cut_chg(s_kam.vorher);
    gr2_kam_merken(s_kam.vorher, 0, e);
    re15_gr2_log("KAMERA %s zurueck -> Cut %d F%u clip=%d bild=%d", k_kam_prog[s_kam.programm], (int)s_kam.vorher,
                 (unsigned)g_engine.frame_count, e ? (int)e->motion : -1, e ? (int)e->anim_frame : -1);
}
static void gr2_kam_frei(const re15_actor_t *e)
{
    if (!s_kam.aktiv) return;
    gr2_kam_zurueck(e);
    g_scd.cut_auto_enabled = 1;                                /* FUN_8004032c @0x8004032c / RE2 &= ~0x100 */
    gr2_kam_merken(-2, 0, e);
    re15_gr2_log("KAMERA %s frei (Zonenkamera an) Cut %d F%u clip=%d bild=%d", k_kam_prog[s_kam.programm],
                 (int)g_scd.cam_id, (unsigned)g_engine.frame_count, e ? (int)e->motion : -1, e ? (int)e->anim_frame : -1);
    s_kam.aktiv = 0; s_kam.programm = 0; s_kam.r1_geschnitten = 0; s_kam.vorher = -1;
}

int re15_gr2_kamera_schnitt(int blick, const re15_actor_t *gator, const re15_actor_t *opfer, const char *programm)
{
    if (programm) re15_gr2_log("KAMERA SZENE \"%s\"", programm);
    return gr2_kam_schnitt(GR2_KAM_SZENE, blick, gator, opfer);
}
void re15_gr2_kamera_frei(const char *programm)
{
    if (programm) re15_gr2_log("KAMERA SZENE \"%s\" Ende", programm);
    gr2_kam_frei(NULL);
}
int  re15_gr2_kamera_gesperrt(void) { return s_kam.aktiv; }
static int gr2_kam_auto(void) { return !s_kam_aus; }
static void gr2_kam_reset(void)
{
    /* Boss-INIT = Raum frisch geladen: g_scd (cut_auto_enabled = 1) setzt der Raumlader selbst zurueck. */
    memset(&s_kam, 0, sizeof s_kam);
    s_kam.vorher = -1;
}
/* ==== Runde 41 Spur G, G-E1 (Nachbesserung 3, Abnahme G 2 M1 "der R1-Tod ist in der Riegel-Startlage nicht im Bild") ==========
 * Orchestrator-Entscheid G-E1: fuer den R1-Explosionstod die 2090/2091-Zonenkamera, in der Kopf und Kiefer mit Rand im Bild liegen
 * und von keiner Maske verdeckt sind, am naechsten an der RE2-Lage (RE2-Cut 5: Kopf bei Bild (170,111) im ersten Bild von Clip 9,
 * k_blick EXPLOSION, gemessen mit tools/r41_re2gator/probe_kamera_re2.c der Spur B).
 * START (Bild R1 P0): Kopf (Bone 6), Kiefer (Bone 7) und die sechs Punkte, an denen RE2 die Explosion zeichnet (Spritzring
 * FUN_8010338c = Kanister-Matrix x SVECTOR-Tafel @0x801000a8, `jal 0x8001bf10` @0x8010344c-d0, k_ring) muessen im Bild liegen, die
 * Spritzring-Punkte samt dem groessten Bild ihres Sprites ("mit Rand", GR2_R1_RAND_*), keiner von einer Maske verdeckt (Masken-
 * Rechteck des Cuts mit re15_pri_mask_occludes(depth, vz), STRIKT depth < Bucket(vz) @0x8001ce54, Plattform-Haken
 * re15_gr2_maske_hook = dieselbe Masken-Kette wie der Zeichner; Rechteck statt Texel = strenger als das Bild), und alle im
 * Regions-Viereck des Cuts: der Zeichner verwirft Effekt-Plaetze (re2fx_quads `jal 0x8002c820` @0x80077a30) und den Gegner (NPC-
 * Pass npc_region_culled, Ursprung = Wurzel) ausserhalb des Vierecks.
 * TODES-CLIP: der Kopf wandert im Clip 9 weit (R1 P1: Rueckstoss +0x144 = -500, Schritt 40 je Bild - 2 @0x8010246c-a0, Ziel-Gier
 * +160 mit 75/Bild @0x8010239c-a4/@0x801025f8-608, Clip 9 absolut vom Anker; gemessen bis 6100 Einheiten). Je zweites Clip-Bild wird
 * mit derselben Rechnung wie re15_gr2_r1_tick auf einer Kopie des Aktors vorausgerechnet (samt Wandklemme und Anker-Nachfuehrung
 * wie re15_gator_boss_tick) und
 * gezaehlt, in wie vielen Proben Kopf und Kiefer im Bild und unverdeckt sind und Kopf und Wurzel im Viereck liegen.
 * WAHL (PORT-WAHL der Abbildung, Mass = G-E1), Rangfolge ohne Gewichte: (1) gueltiger START, (2) die meisten sichtbaren Proben des
 * Todes-Clips, (3) die meisten sichtbaren START-Punkte, (4) Kopf-Bildlage naechst (170,111); Stufe 0 mit Spur B's Massstab-Grenze
 * fuer jeden Schnitt (der laufende Cut ist kein Schnitt und faellt nicht darunter), Stufe 1 ohne. Ist die Wahl der laufende Cut,
 * bleibt er (RE2 R1 P0 schneidet nur, wenn der laufende Cut den Kopf nicht zeigt, @0x80102218-34), sonst Schnitt (+0x218 |= 0x40
 * @0x80102248, zurueck am Clip-Ende @0x801024a4-ec); zeigt keine Zonenkamera einen Punkt, die alte Wahl.
 * Messzeilen "KAMERA R1 G-E1 ..." in gator_boss.log. */
int (*re15_gr2_maske_hook)(int cut, int sx, int sy, int32_t vz) = NULL;
#define GR2_R1_SOLL_X  170   /* RE2-Cut-5-Bildlage des Kopfes (k_blick EXPLOSION, probe_kamera_re2.c) */
#define GR2_R1_SOLL_Y  111
#define GR2_R1_START   8     /* Kopf, Kiefer, 6 Spritzring-Punkte */
#define GR2_R1_PROBEN  24    /* Vorausschau: je zweites Bild von Clip 9 */
/* "MIT RAND": um jeden Spritzring-Punkt muss das groesste Bild seines Sprites ins Bild passen. CORE00.ESP Bank 3 (Kopf @0x8) Sub
 * 0x0f, Anim 11..22 (Eintraege @0x68..@0xC0, Zelltafel @0x190): groesstes Bild A17 @0x98 / A18 @0xA0 = Zellen x -36..36, y -32..36
 * Texel (dx/dy + Groesse 16); Bildschirm-Versatz je Texel = Skala * camf * Aspekt / (SZ * 16 * 65536) (FUN_80077ed0: step = Groesse
 * * Skala * camf / (SZ << 4), Breite = step * Aspekt, Texelschritt = Breite / Groesse; re2fx_quads), Aspekt 0x1000 (Schritt 0 +4/+6
 * `00 10 00 10` @0x5BC), Skala = a0 & 0xffff des Rings (0x2000/0x3000, k_ring_a0). */
#define GR2_R1_RAND_X0 (-36)
#define GR2_R1_RAND_X1 36
#define GR2_R1_RAND_Y0 (-32)
#define GR2_R1_RAND_Y1 36
#define GR2_R1_ASPEKT  0x1000
typedef struct {
    int32_t start[GR2_R1_START][3];
    int32_t wurzel0[3];
    int     n;                                   /* Proben des Todes-Clips */
    int32_t kopf[GR2_R1_PROBEN][3], kiefer[GR2_R1_PROBEN][3], wurzel[GR2_R1_PROBEN][3];
} gr2_r1_pfad_t;
typedef struct { int start_ok, im, vd, aus, kx, ky, bilder, kopf_ok, kiefer_ok; int32_t m; } gr2_r1_urteil_t;
static int gr2_r1_pfad(const re15_actor_t *e, gr2_r1_pfad_t *P);                 /* nach k_ring definiert */
static uint16_t gr2_r1_skala(int ring);                                          /* ebenso */
static int gr2_fin_proj(const re15_camera_view_t *v, const int32_t p[3], int *sx, int *sy, int32_t *vz_aus);
/* Ein Punkt in Cut c: 1 = im Bild (+ Rand bei ring >= 0), im Viereck, unverdeckt; *sx/*sy Bildlage; *aus/*vd Gruende. */
static int gr2_r1_punkt(int c, const re15_camera_view_t *v, int region, const int16_t rx[4], const int16_t rz[4], const int32_t p[3],
                        int ring, int *sx, int *sy, int *aus, int *vd)
{
    int32_t vz = 0;
    *sx = *sy = -9999;
    int drin = gr2_fin_proj(v, p, sx, sy, &vz);
    if (drin && ring >= 0) {                                    /* Sprite-Rand (s.o.), 16.16 */
        const int64_t f = ((int64_t)gr2_r1_skala(ring) * v->fov_screen_dist * GR2_R1_ASPEKT) / ((int64_t)vz * 16);
        const int x0 = *sx + (int)((GR2_R1_RAND_X0 * f) >> 16), x1 = *sx + (int)((GR2_R1_RAND_X1 * f) >> 16);
        const int y0 = *sy + (int)((GR2_R1_RAND_Y0 * f) >> 16), y1 = *sy + (int)((GR2_R1_RAND_Y1 * f) >> 16);
        drin = x0 >= 0 && x1 < 320 && y0 >= 0 && y1 < 240;
    }
    if (drin && region && !re15_aot_point_in_quad(p[0], p[2], rx, rz)) { if (aus) (*aus)++; return 0; }   /* FUN_8002c820 */
    if (!drin) return 0;
    if (re15_gr2_maske_hook && re15_gr2_maske_hook(c, *sx, *sy, vz)) { if (vd) (*vd)++; return 0; }
    return 1;
}
static void gr2_r1_beurteilen(int c, const re15_actor_t *e, const gr2_r1_pfad_t *P, gr2_r1_urteil_t *u)
{
    memset(u, 0, sizeof *u); u->kx = u->ky = -9999;
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[c], &v) != 0) return;
    int16_t rx[4], rz[4];
    const int region = re15_rdt_get_region_quad(&g_room_rdt, c, rx, rz);
    for (int j = 0; j < GR2_R1_START; j++) {
        int sx, sy;
        const int ok = gr2_r1_punkt(c, &v, region, rx, rz, P->start[j], j >= 2 ? j - 2 : -1, &sx, &sy, &u->aus, &u->vd);
        if (ok) u->im++;
        if (j == 0) { u->kx = sx; u->ky = sy; u->kopf_ok = ok; }
        if (j == 1) u->kiefer_ok = ok;
    }
    const int wurzel_ok = !region || re15_aot_point_in_quad(P->wurzel0[0], P->wurzel0[2], rx, rz);
    if (!wurzel_ok) u->aus++;
    u->start_ok = (u->im == GR2_R1_START) && wurzel_ok;
    for (int s = 0; s < P->n; s++) {
        int sx, sy;
        if (!gr2_r1_punkt(c, &v, region, rx, rz, P->kopf[s], -1, &sx, &sy, NULL, NULL)) continue;
        if (!gr2_r1_punkt(c, &v, region, rx, rz, P->kiefer[s], -1, &sx, &sy, NULL, NULL)) continue;
        if (region && !re15_aot_point_in_quad(P->wurzel[s][0], P->wurzel[s][2], rx, rz)) continue;
        u->bilder++;
    }
    u->m = gr2_massstab(&g_room_rdt.cuts[c], P->start[0], gr2_scale(e));
}
static int gr2_r1_wahl(const re15_actor_t *e, const gr2_r1_pfad_t *P, int cur)
{
    const int32_t grenze = gr2_massstab_grenze();
    for (int stufe = 0; stufe < 2; stufe++) {
        int best = -1; gr2_r1_urteil_t bu; int64_t best_d = 0;
        memset(&bu, 0, sizeof bu);
        for (int k = 0; k < g_room_rdt.cut_count; k++) {
            if (!gr2_zonenkamera(k)) continue;
            gr2_r1_urteil_t u;
            gr2_r1_beurteilen(k, e, P, &u);
            const int gok = stufe == 1 || k == cur || u.m <= grenze;
            const int64_t d = (int64_t)(u.kx - GR2_R1_SOLL_X) * (u.kx - GR2_R1_SOLL_X) + (int64_t)(u.ky - GR2_R1_SOLL_Y) * (u.ky - GR2_R1_SOLL_Y);
            re15_gr2_log("KAMERA R1 G-E1 Kandidat stufe=%d Cut %d kopf_bild=(%d,%d) Start %d/%d (%d verdeckt, %d ausserhalb)%s, "
                         "Todes-Clip %d/%d Proben, massstab=%d%s, Abstand zu RE2 (%d,%d) %d", stufe, k, u.kx, u.ky, u.im, GR2_R1_START,
                         u.vd, u.aus, u.start_ok ? "" : " ungueltig", u.bilder, P->n, (int)u.m, gok ? "" : " >grenze",
                         GR2_R1_SOLL_X, GR2_R1_SOLL_Y, (int)(d > 0x7fffffff ? 0x7fffffff : d));
            if (!gok || (u.im == 0 && u.bilder == 0)) continue;
            int besser = best < 0;
            if (!besser && u.start_ok != bu.start_ok) besser = u.start_ok > bu.start_ok;               /* (1) */
            else if (!besser && u.bilder != bu.bilder) besser = u.bilder > bu.bilder;                 /* (2) */
            else if (!besser && u.im != bu.im) besser = u.im > bu.im;                                 /* (3) */
            else if (!besser) besser = d < best_d;                                                    /* (4), Gleichstand: kleinerer Cut */
            if (besser) { best = k; bu = u; best_d = d; }
        }
        if (best >= 0) return best;
    }
    return -1;
}

/* R1 P0 (@0x801021ec-6c): Sperre immer; Schnitt nur, wenn der aktuelle Cut den Kopf nicht zeigt (RE2: Cut 6/13 und
 * z < -13300 = genau der Fall "Kopf ausserhalb", Probe: Bild-x -2 in Cut 6), dann +0x218 |= 0x40 (@0x80102240-4c). */
static void gr2_kam_r1_start(const re15_actor_t *e)
{
    if (!gr2_kam_auto()) return;
    int32_t kopf[3], achse[2];
    int cur = (int)g_scd.cam_id, sx = -1, sy = -1, sichtbar = 0;
    if (g_room_rdt_ok && g_room_rdt.cuts && cur < g_room_rdt.cut_count && gr2_kopf_achse(e, kopf, achse))
        sichtbar = gr2_im_bild(&g_room_rdt.cuts[cur], kopf, &sx, &sy);
    {   /* Runde 41 Spur G, G-E1 (s.o.) */
        static gr2_r1_pfad_t P;
        if (gr2_r1_pfad(e, &P) && g_room_rdt_ok && g_room_rdt.cuts && cur >= 0 && cur < g_room_rdt.cut_count) {
            gr2_r1_urteil_t u;
            gr2_r1_beurteilen(cur, e, &P, &u);
            re15_gr2_log("KAMERA R1 G-E1 laufender Cut %d: kopf_bild=(%d,%d) Start %d/%d (%d verdeckt, %d ausserhalb)%s, Todes-Clip %d/%d "
                         "Proben F%u", cur, u.kx, u.ky, u.im, GR2_R1_START, u.vd, u.aus, u.start_ok ? "" : " ungueltig", u.bilder, P.n,
                         (unsigned)g_engine.frame_count);
            const int c = gr2_r1_wahl(e, &P, cur);
            if (c >= 0 && c == cur) {
                gr2_kam_sperre(GR2_KAM_R1);
                gr2_kam_merken(-1, RE15_GR2_BLICK_EXPLOSION, e);
                re15_gr2_log("KAMERA R1 Sperre ohne Schnitt: Kopf im Bild von Cut %d bei (%d,%d) F%u (G-E1: laufender Cut ist die Wahl, "
                             "Start %d/%d, Kopf %s, Kiefer %s, Todes-Clip %d/%d Proben)", cur, u.kx, u.ky, (unsigned)g_engine.frame_count,
                             u.im, GR2_R1_START, u.kopf_ok ? "ok" : "NEIN", u.kiefer_ok ? "ok" : "NEIN", u.bilder, P.n);
                return;
            }
            if (c >= 0) {
                gr2_kam_sperre(GR2_KAM_R1);
                gr2_kam_cut_chg(c);
                gr2_kam_merken(c, RE15_GR2_BLICK_EXPLOSION, e);
                s_kam.r1_geschnitten = 1;                           /* RE2 +0x218 |= 0x40 @0x80102248 */
                gr2_r1_beurteilen(c, e, &P, &u);
                const int f = gr2_kopf_achse(e, kopf, achse) ? gr2_frontal(kopf, achse, &g_room_rdt.cuts[c]) : 0;
                re15_gr2_log("KAMERA R1 EXPLOSION RE2-Cut %d -> Cut %d F%u clip=%d bild=%d frontal=%d soll=%d massstab=%d re2_m=%d grenze=%d "
                             "(G-E1: kopf_bild=(%d,%d), Start %d/%d, Kopf %s, Kiefer %s, Todes-Clip %d/%d Proben)",
                             (int)k_blick[RE15_GR2_BLICK_EXPLOSION].re2_cut, c, (unsigned)g_engine.frame_count, (int)e->motion,
                             (int)e->anim_frame, f, (int)k_blick[RE15_GR2_BLICK_EXPLOSION].soll, (int)u.m,
                             (int)gr2_massstab_soll(RE15_GR2_BLICK_EXPLOSION), (int)gr2_massstab_grenze(), u.kx, u.ky, u.im, GR2_R1_START,
                             u.kopf_ok ? "ok" : "NEIN", u.kiefer_ok ? "ok" : "NEIN", u.bilder, P.n);
                return;
            }
            re15_gr2_log("KAMERA R1 G-E1: keine Zonenkamera zeigt einen Punkt -> alte Wahl F%u", (unsigned)g_engine.frame_count);
        }
    }
    if (!sichtbar) {
        s_kam.r1_geschnitten = (uint8_t)(gr2_kam_schnitt(GR2_KAM_R1, RE15_GR2_BLICK_EXPLOSION, e, NULL) >= 0);
        return;
    }
    gr2_kam_sperre(GR2_KAM_R1);
    gr2_kam_merken(-1, RE15_GR2_BLICK_EXPLOSION, e);
    re15_gr2_log("KAMERA R1 Sperre ohne Schnitt: Kopf im Bild von Cut %d bei (%d,%d) F%u", cur, sx, sy,
                 (unsigned)g_engine.frame_count);
}
void re15_gr2_kamera_auto(int an) { s_kam_aus = (uint8_t)(an ? 0 : 1); }
/* Nachbesserung 2 (Abnahme 1, M1 b) + Orchestrator-Entscheid B6 "Finisher-Kamera = RE2": RE2 FRESSEN P0 sperrt die
 * Zonenkamera (`ori v0,v0,0x100 / sw v0,0(a0)` @0x80100fcc-d0 auf 0x800cfbd8) und setzt Cut 7/14 (`jal 0x8002c7ac`
 * @0x80100ff4) - Sub 4 (@0x80100f58-0x801011fc) gibt Bit 0x100 nie frei; die vier `addiu v1,zero,-257` des Overlays
 * stehen in Intro P2 @0x80100910, R1 P3 @0x80102688, R2 @0x801028ac und R3 @0x80102b1c (R2/R3 = Tod durch Beschuss,
 * nicht gebaut, Entscheid B2): der Schnitt steht bis zum Game Over. Der Port startet ab dem
 * Fress-Latch (Clip-4-Bild 13, RE2 @0x80101104-34) die RE1.5-Todes-Praesentation (game_step_common.c
 * re15_gameover_fsm_tick = FUN_8001500c): Weissblende sub0/1 (FUN_800217b0/80021880), flacher Schwarzgrund
 * (FUN_80021634(2,0)) und die kreisende Todeskamera (FUN_80015850, main.c g_death_cam) - gemessen Abnahme 1 Lauf A:
 * F735-F745 weiss, ab F750 Orbit (F755 Kamera im Gator-Leib). B6: diese drei Teile entfallen fuer den Gator-Tod; die
 * Game-Over-Folge des Ports (YOU-DIED-Buchstaben g_death_flyin, Schluss-Abblende g_death_fade, Titel) bleibt.
 * Wirkung ohne Haken in gemeinsamen Dateien: die FSM laeuft am Anfang von re15_game_step (@0x8001cdfc-Zwilling,
 * game_step_common.c:1137-1138), der Boss-Takt am Ende (Entity-Schleife) - hier zurueckgesetzt sieht der Zeichner
 * die drei Werte nie. Die Sperre selbst (cut_auto_enabled = 0) wird dabei jedes Bild gehalten. */
static unsigned s_fin_halt_log;
static void gr2_kam_finisher_halten(void)
{
    if (!(s_kam.aktiv && s_kam.programm == GR2_KAM_FRESSEN)) { s_fin_halt_log = 0; return; }
    int war = (g_death_cam != 0) | ((g_death_white != 0) << 1) | ((g_death_blackbg != 0) << 2);
    g_death_cam = 0; g_death_white = 0; g_death_blackbg = 0; g_death_glow = 0;
    g_scd.cut_auto_enabled = 0;                             /* RE2: 0x800cfbd8 Bit 0x100 bleibt (kein Freigabe-Pfad) */
    unsigned f = (unsigned)g_engine.frame_count;
    if (war && !(s_fin_halt_log & 0x80000000u)) {
        s_fin_halt_log |= 0x80000000u;
        re15_gr2_log("KAMERA FINISHER haelt Cut %d: RE1.5-Todesbild unterdrueckt (cam=%d weiss=%d schwarz=%d) ab F%u",
                     (int)g_scd.cam_id, war & 1, (war >> 1) & 1, (war >> 2) & 1, f);
    }
    if ((s_fin_halt_log & 0xffffu) == 0 || f >= (s_fin_halt_log & 0xffffu) + 20u) {
        s_fin_halt_log = (s_fin_halt_log & 0x80000000u) | (f & 0xffffu);
        re15_gr2_log("KAMERA FINISHER Halt Cut %d F%u", (int)g_scd.cam_id, f);   /* B6-Messung: Cut je 20 Bilder */
    }
}
void re15_gr2_kamera_finisher(int slot, const re15_actor_t *gator, const re15_actor_t *opfer)
{
    (void)opfer;                                            /* Opfer = Kiefer (s. k_blick), nicht die P0-Lage */
    if (s_kam_aus) return;
    int c = (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].fin_cut : -1;
    if (c >= 0 && g_room_rdt_ok && g_room_rdt.cuts && c < g_room_rdt.cut_count) {
        /* Nachbesserung 5 (Abnahme 4, M1): die Finisher-Buehne (re15_gr2_finisher_buehne) hat Lage, Gier UND Kamera gewaehlt -
         * wie RE2 P0, das den Gator umsetzt (@0x80101048-4c) und fest auf Cut 7/14 schneidet (@0x80100fec-ff4), dessen Bild die
         * Finisherlage frontal zeigt (Dossier §9.2). Sperre + Cut_chg-Zwilling wie gr2_kam_schnitt, gleiche Logzeile. */
        gr2_kam_sperre(GR2_KAM_FRESSEN);
        gr2_kam_cut_chg(c);
        gr2_kam_merken(c, RE15_GR2_BLICK_FINISHER, gator);
        int32_t kopf[3], achse[2], m = 0; int f = 0;
        if (gator) {
            re15_actor_t b = *gator;                        /* Kopf/Blickachse an der geplanten Buehne (Clip 4 Bild 0) */
            b.x = s_g[slot].fin_x; b.z = s_g[slot].fin_z; b.rot_y = s_g[slot].fin_gier;
            if (gr2_kopf_achse(&b, kopf, achse)) {
                m = gr2_massstab(&g_room_rdt.cuts[c], kopf, gr2_scale(gator));
                f = gr2_frontal(kopf, achse, &g_room_rdt.cuts[c]);
            }
        }
        re15_gr2_log("KAMERA %s %s RE2-Cut %d -> Cut %d F%u clip=%d bild=%d frontal=%d soll=%d massstab=%d re2_m=%d grenze=%d (Buehne)",
                     k_kam_prog[GR2_KAM_FRESSEN], k_blick[RE15_GR2_BLICK_FINISHER].name, (int)k_blick[RE15_GR2_BLICK_FINISHER].re2_cut,
                     c, (unsigned)g_engine.frame_count, gator ? (int)gator->motion : -1, gator ? (int)gator->anim_frame : -1,
                     f, (int)k_blick[RE15_GR2_BLICK_FINISHER].soll, (int)m, (int)gr2_massstab_soll(RE15_GR2_BLICK_FINISHER),
                     (int)gr2_massstab_grenze());
        return;
    }
    /* ohne Buehne bleibt der Gator am Ort, Leon kommt trotzdem vor ihn (P0 @0x8010106c-84): auch dann nie die laufende Kamera (NB6 M2) */
    s_wahl_ohne_laufend = 1;
    gr2_kam_schnitt(GR2_KAM_FRESSEN, RE15_GR2_BLICK_FINISHER, gator, NULL);   /* @0x80100fbc-ff4, kein Ende */
    s_wahl_ohne_laufend = 0;
}
int re15_gr2_vsync(void) { return s_vsync; }

int re15_gr2_kamera_folge(int max, int *cut, int *blick, int *clip, int *bild)
{
    int n = s_kam.n < max ? s_kam.n : max;
    for (int i = 0; i < n; i++) {
        if (cut) cut[i] = s_kam.cut[i];
        if (blick) blick[i] = s_kam.blick[i];
        if (clip) clip[i] = s_kam.clip[i];
        if (bild) bild[i] = s_kam.bild[i];
    }
    return n;
}

/* ---- Takt ---------------------------------------------------------------------------------- */
void re15_gr2_takt(int slot, re15_actor_t *e, re15_actor_t *pl, int im_chase)
{
    (void)pl;
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    gr2_t *g = &s_g[slot];
    if (!s_env_geprueft) re15_gr2_kanister_env();
    /* Nachtrag 4: ein Programm, dessen Ablauf nicht mehr laeuft (Intro/R1 abgebrochen), gibt die Kamera frei. */
    if (s_kam.aktiv && ((s_kam.programm == GR2_KAM_INTRO && !g->intro_phase)
                        || (s_kam.programm == GR2_KAM_R1 && !g->r1_phase)))
        gr2_kam_frei(e);
    gr2_kam_finisher_halten();                              /* Nachbesserung 2 (M1 b, B6): Finisher-Schnitt bis Game Over */
    gr2_ereignis_takt(g);                                   /* Root @0x80100410 */
    gr2_schuettel_takt();                                   /* EXE je Bild */
    gr2_putz_takt(g);
    /* Nachbesserung 4 (M1): die Quell-Matrizen (Effekte mit Status 0x800 folgen ihnen je Bild, FUN_8001d894 @0x8001d8ac-fc)
     * fuehrt re15_gr2_nachtakt am Takt-Ende aus der gezeichneten Pose nach - hier, am Takt-Anfang, waeren sie die des Vorbilds. */
    if (g->biss_cd > 0) g->biss_cd--;                       /* +0x238 @0x801004a8-b8 */
    if (g->r4_cd > 0) g->r4_cd--;                           /* +0x22E @0x80100494-a4 */
    gr2_kan_takt(g, e, im_chase && !g->tot);
}

/* ---- Verfolgung: Hysterese + authored Vortrieb --------------------------------------------- */
int re15_gr2_chase_clip(int slot, re15_actor_t *e, int32_t dist, int32_t *vortrieb)
{
    gr2_t *g = &s_g[slot];
    int clip;
    if (re15_gr2_hysterese_ruht(slot, e)) {
        clip = g->letzter_clip;                             /* Sub bleibt (Pre springt nach 0x80100c08) */
    } else if (g->letzter_clip == 0) {                      /* Sub 1 */
        clip = (g->biss_cd == 0 && dist >= RE15_GR2_PADDEL_AB) ? 1 : 0;   /* @0x80100b5c-64, @0x80100b88-98 */
    } else {                                                /* Sub 2 */
        clip = (dist < RE15_GR2_SCHWIMM_AB || g->biss_cd != 0) ? 0 : 1;   /* @0x80100be4-c04 */
    }
    if (clip != g->letzter_clip || e->motion != clip) {
        e->motion = (uint8_t)clip; e->anim_frame = 0;       /* FUN_8001a330: Clipwechsel -> Bild 0 @0x8001a34c-64 */
        gr2_blend(e, clip == 0 ? 32 : 4);                   /* Blend 32 @0x80100c38 / 4 @0x80100cfc */
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
/* Nachbesserung 2 (Abnahme 1, M3): wann die Paddel-Hysterese des RE2-Sub-1/2-Pre @0x80100964 NICHT laeuft
 * (selbst disassembliert, EM23_OVL_0000.BIN):
 *  (a) Zone 1 + Kanister liegt im Wasser (Raumflag 0x800d4874 Bank 5 Bit 0, `jal 0x80077360 / addu a1,zero,zero /
 *      beq v0,zero,0x80100b20` @0x80100ab8-c8): Fenster verfehlt -> `beq v0,zero,0x80100c08` @0x80100aec, Fenster
 *      getroffen bei Stufe != 0 -> `bne v0,zero,0x80100c08` @0x80100afc, Stufe 0 -> Stufe 1 (+ Sub 2 -> 1) und
 *      ebenfalls 0x80100c08 - die Hysterese @0x80100b20 wird nie erreicht. Das Bit setzt ROOM40A0 sub10 einmal
 *      (`Set 22 05 00 01` @SCD 0x0712), kein `Set 22 05 00 00` im Raumskript (Walker re2_scd_walk.py) -> es gilt,
 *      solange der Kanister im Raum ist, auch beim Tragen. PORT-WAHL der Zone (wie beim Dash): RE2-Zone 1 = das
 *      Korridorbein, in dem der Kanister liegt -> die Beckenzone (gb_zone) seiner Liegestelle.
 *  (b) Stufe nicht 0 und nicht 7 (`lbu a0,550 / beq a0,zero,0x80100b38 / addiu v0,zero,7 / bne a0,v0,0x80100c08`
 *      @0x80100b20-30), in jeder Zone: waehrend der Aufnahme bleibt der Sub stehen. */
int re15_gr2_hysterese_ruht(int slot, const re15_actor_t *e)
{
    (void)slot;
    if (!s_kan.aktiv || s_kan.stufe == 13) return 0;
    if (s_kan.stufe != 0 && s_kan.stufe != 7) return 1;                              /* (b) */
    if (e && s_zone_fn && s_zone_fn(e->z) == s_zone_fn(s_kan.liege_z)) return 1;     /* (a) */
    return 0;
}
/* Nachbesserung 2 (Abnahme 1, M3): der RE2-Biss T1 wird nur bei Stufe 0 oder 7 scharf gemacht - FUN_801012fc
 * `lbu v1,550(a0) / beq v1,zero,0x80101378 / addiu v0,zero,7 / bne v1,v0,0x80101380` @0x80101360-70 vor `sb v0(1),
 * 548(a0)` @0x8010137c (+0x224 = 1 = Biss-Maschine an). Waehrend der Aufnahme (Stufe 1..6) beisst er nicht. Das
 * Boss-Modul fragt das vor jedem Biss-Start (Schnapp/Hochbiss/Kletterbiss = Nutzer-Design-Biss). */
int re15_gr2_biss_erlaubt(int slot)
{
    /* Nachbesserung 3 (M3): Zielmodus +0x1C0 & 4 sperrt T1 (`lbu v0,448(a0) / andi v0,v0,0x4 / bne` @0x8010134c-58) */
    if (slot >= 0 && slot < RE15_ACTOR_MAX && s_g[slot].ziel_bit) return 0;
    if (!s_kan.aktiv) return 1;
    return s_kan.stufe == 0 || s_kan.stufe == 7 || s_kan.stufe == 13;
}
int re15_gr2_zielmodus(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].ziel_bit : 0; }
/* Nachbesserung 3 (Abnahme 2 M2): vom Fenstertreffer (Stufe 1) bis zum Anhaengen (Stufe 2, t = 13 @0x801018c4-cc) keine
 * Koerperlenkung. RE2 hat in Sub 1/2 und in T2 keinen Gier-Schreiber (`sh ...,118(...)` im Overlay nur @0x80100280/2a0 (Root,
 * Rueckschreiben um FUN_80035408), Intro @0x80100948, Dash @0x80100ec0, R1 @0x80102404/2440, R2/R3 @0x801027c8/2a34) - der
 * Gator schwimmt die Aufnahme geradeaus (Clip 0, Wurzelversatz), Stufe 2 schiebt den Kanister laengs (@0x801018a4-c0).
 * Das Boss-Modul fragt das im CHASE (kein Steuern, keine Wende, kein Dash, kein Wechsel in GUARD) und in der Selbst-
 * Befreiung ab. */
int re15_gr2_aufnahme_lenksperre(int slot)
{
    (void)slot;
    return s_kan.aktiv && (s_kan.stufe == 1 || s_kan.stufe == 2) && !s_kan.angehaengt;
}
void re15_gr2_biss_cd(int slot, int n) { if (slot >= 0 && slot < RE15_ACTOR_MAX) s_g[slot].biss_cd = (int16_t)n; }
int  re15_gr2_biss_cd_rest(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].biss_cd : 0; }

/* ==== Nachbesserung 2 (Abnahme 1, M2) + Orchestrator-Entscheid B7: die RE2-Wende im BECKEN ====================
 * MESSUNG (Abnahme 1 Lauf A/N, selbst nachgerechnet mit tools/r41_re2gator/probe_wende.c gegen libre15_engine.a): Clip 3
 * dreht die Pose per Root-ry 438 -> 1023 und schiebt die Wurzel quer (Netto laengs 8135 / quer -2828 fuer Clip 6 + 3,
 * 4191 / -2861 fuer Clip 2 + 3, skaliert 2731), danach `yaw += 1024` (Intro @0x80100938-48, Dash @0x80100eb0-c0) - eine
 * RECHTS-Wende um 90 Grad. Vom Lauerplatz (-7200,-9200) mit Gier 1089 (Sued) zeigt sie im Westkanal in die Westwand:
 * Probe = Lauf A Bild fuer Bild (Kopf Bone 6 am Clip-6-Ende -10022 bei Wurzel -7882 ungeklemmt; mit der Klemme
 * -7434 also -9574 = gemessen), am Clip-3-Ende 4988 ausserhalb. Der Dash am Suedausgang des Kanals (Lauf A (-7300,
 * -18109) Gier 1024, C2 (-6767,-18110) 695, N (-7247,-18259) 1059) dreht ebenso in die Westwand: Kopf 4340 / 1048 /
 * 4504 draussen. Das Becken ist der RE2-Korridor gespiegelt: RE2 legt die Zonenlinien VOR Rechtskurven des Korridors
 * (Bein 1 entlang -X auf z -21212 @0x801008d0-d4, Bein 2 entlang +Z auf x -23078 @0x80101048), im Becken oeffnet sich der
 * Suedausgang nach OSTEN.
 * RE2-FORM bleibt (B7): Clip 6 -> Clip 3 bzw. Clip 2 -> Clip 3 und `yaw += 1024` unveraendert. PORT-WAHL ist nur die
 * START-LAGE/-GIER:
 *  - INTRO: RE2 stellt den Gator vor dem Intro per Pos_set (ROOM40A0 sub11 @SCD 0x0736: (4557,-1800,-18237)) + Gier
 *    0x400 (Sce_em_set @0x25A0) auf eine feste Lage, die zum Korridor passt. Port: gr2_intro_lage waehlt eine Lage +
 *    Gier, bei der (1) Rumpf (Bones 0/1), Hals 5, Kopf 6 und Kiefer 7 in JEDEM Bild der 275 Intro-Bilder im Wasser
 *    bleiben und die Wurzel ausserhalb der Klemmzone (hit_radius 1466) - damit gilt die gerechnete Bahn exakt,
 *    (2) der Knick auf Leons Angriffslinie endet: End-Gier = Peilung vom Bahnende zu Leon (Toleranz 128, dann 256/512),
 *    (3) kein Bahnpunkt durch Leon laeuft (Rumpf/Kopf-Bones >= 1350 = SEG[0].r 900 + Spielerradius 0x1c2 450 des
 *    Boss-Moduls) und Leon in KEINEM Bild in einem Koerperlaengen-Push-Kreis liegt (gb_koerper_kontakt), (4) unter allen solchen Lagen die mit den kleinsten Kosten
 *    Startabstand + 4 x Gier-Drehung + 16 x Toleranz - also so nah wie moeglich am Lauerplatz. Der Schwanz (Bones 11-15)
 *    darf wie in der ganzen Nutzer-Design-Verfolgung an die Wand reichen (12,2 m Tier im 7,2 m Kanal). Der Sprung auf die
 *    Lage faellt in den DURCHBRUCH-Schnitt (wie RE2 Pos_set + Cut 10) und in den Posenwechsel Clip 6 Bild 0 mit Blend 0
 *    (`FUN_8001a330(em,6,0,0)` @0x801008ac-b8). Findet sich keine Lage: Start am Ort (alter Ablauf) + Logzeile.
 *  - DASH: der Dash beginnt, wo der Zonenwechsel passiert (RE2: an der Zonenlinie, @0x80100990-a8 / @0x801009d8-fc);
 *    gr2_dash_planen waehlt die Start-Gier (Raster 16, hoechstens 0x400 neben der aktuellen) mit denselben Bedingungen
 *    (1)/(3); Kosten 4 x Drehung + Abweichung der End-Gier von der Peilung zu Leon (RE2: die Wende folgt dem naechsten
 *    Korridorbein, nicht Leon - die Ausrichtung entscheidet nur zwischen gleich kleinen Drehungen; Abnahme 1 M2:
 *    "Dash-Yaw so, dass die RE2-Wende vom Rand weg zeigt"). Die Drehung dorthin laeuft VOR Clip 2 mit 0x60
 *    je Bild (die "zackige Wende" der Nutzer-Design-Verfolgung, enemy_ai_boss_gator.c re15_enemy_steer_point(...,0x60)),
 *    dann SE 4 + Clip 2 wie RE2. Keine zulaessige Gier = kein Dash an diesem Zonenwechsel (Logzeile). */
static int32_t (*s_becken_fn)(int32_t x, int32_t z, int32_t m);
void re15_gr2_becken_quelle(int32_t (*draussen)(int32_t x, int32_t z, int32_t m)) { s_becken_fn = draussen; }
static int (*s_kontakt_fn)(int32_t x, int32_t z, int yaw, int32_t px, int32_t pz);
void re15_gr2_koerper_quelle(int (*kontakt)(int32_t x, int32_t z, int yaw, int32_t px, int32_t pz)) { s_kontakt_fn = kontakt; }
#define GR2_BAHN_MAX   300
#define GR2_BAHN_BONES 5
#define GR2_LEON_MIN   1350          /* SEG[0].r 900 + Spielerradius 0x1c2 (450) - Boss-Modul Koerperlaengen-Push */
#define GR2_KLEMM_R    1466          /* hit_radius_min des 0x23 im Boss (2200 x 2731/4096, Boss-INIT) */
static const int k_bahn_bone[GR2_BAHN_BONES] = { 0, 1, 5, 6, 7 };   /* Rumpf 0/1, Hals 5, Kopf 6, Kiefer 7 */
typedef struct {
    int     n;
    int     n_a;                                      /* Bilder der ersten Clip (Clip 6 bzw. 2) */
    int32_t skala;
    int32_t w[GR2_BAHN_MAX][2];                       /* Wurzelschritt DIESES Bildes im Rig (x laengs, z quer), skaliert
                                                       * wie gr2_wurzel_schritt (q12mul je Komponente), 0 ohne Schritt */
    int32_t b[GR2_BAHN_MAX][GR2_BAHN_BONES][2];       /* Bone-Lage im Rig (x, z), skaliert */
    uint8_t knick[GR2_BAHN_MAX];                      /* Bild mit yaw += 1024 */
} gr2_bahn_t;
static gr2_bahn_t s_bahn[2];                          /* 0 = Intro (Clip 6 -> 3), 1 = Dash (Clip 2 -> 3) */
/* Bahn wie gr2_clip_tick: Bild 0 Neu-Anker, Schritt off[f]-off[f-1] fuer f = 1..n-2, Bild n-1 ohne Schritt (FUN_8001a330
 * `bne v0,zero,0x8001a3cc` @0x8001a3b0), Knick im letzten Bild der zweiten Clip. */
static const gr2_bahn_t *gr2_bahn(const re15_actor_t *e, int art)
{
    gr2_bahn_t *b = &s_bahn[art];
    int32_t s = gr2_scale(e);
    if (b->n > 0 && b->skala == s) return b;
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk) return NULL;
    int clips[2] = { art ? 2 : 6, 3 };
    b->n = 0; b->skala = s;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    for (int k = 0; k < 2; k++) {
        int c = clips[k], n = bk->anim.clips[c].frame_count;
        for (int f = 0; f < n && b->n < GR2_BAHN_MAX; f++) {
            int32_t w[2] = { 0, 0 };
            if (f >= 1 && f <= n - 2) {
                int16_t o1[3], o0[3];
                gr2_off(bk, c, f, o1); gr2_off(bk, c, f - 1, o0);
                w[0] = q12mul(o1[0] - o0[0], s); w[1] = q12mul(o1[2] - o0[2], s);
            }
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            int kf = gr2_kf(&bk->anim, c, f);
            if (kf < 0 || re15_skel_compute_pose(&bk->skel, kf, poses) != 0) { g_anim_pose_actor = save; b->n = 0; return NULL; }
            b->w[b->n][0] = w[0]; b->w[b->n][1] = w[1];
            for (int i = 0; i < GR2_BAHN_BONES; i++) {
                int bo = k_bahn_bone[i] < bk->skel.bone_count ? k_bahn_bone[i] : 0;
                b->b[b->n][i][0] = q12mul(poses[bo].trans[0], s);
                b->b[b->n][i][1] = q12mul(poses[bo].trans[2], s);
            }
            b->knick[b->n] = (uint8_t)(k == 1 && f == n - 1);
            b->n++;
        }
        if (k == 0) b->n_a = b->n;
    }
    g_anim_pose_actor = save;
    return b;
}
static void gr2_dreh(int yaw, int32_t lx, int32_t lz, int32_t *wx, int32_t *wz)   /* wie gr2_rig_welt, ohne Skala */
{
    int32_t c = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    *wx = (int32_t)(((int64_t)c * lx + (int64_t)sn * lz) >> 12);
    *wz = (int32_t)((-(int64_t)sn * lx + (int64_t)c * lz) >> 12);
}
static int32_t gr2_abstand(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int64_t dx = ax - bx, dz = az - bz;
    return (int32_t)re15_squareroot0((uint32_t)((dx * dx + dz * dz) > 0xffffffffLL ? 0xffffffffLL : (dx * dx + dz * dz)));
}
/* 1 = Bahn ab (sx,sz) mit Start-Gier y0 frei (Bedingungen (1)/(3) oben); *kopf_ende = Kopf-Leon im letzten Bild,
 * ende[] = Wurzel im letzten Bild. Die Wurzel laeuft Bild fuer Bild GENAU wie gr2_wurzel_schritt (Schritt um die Gier
 * gedreht, `>> 12` je Schritt) - die Planung trifft so dieselbe Lage wie das Spiel (eine einmal gedrehte Summe wich
 * gemessen um ~180 ab, unit wand). Geprueft wird jedes 3. Bild und das letzte. */
static int gr2_bahn_frei(const gr2_bahn_t *b, int32_t sx, int32_t sz, int y0, const re15_actor_t *pl, int32_t *kopf_ende,
                         int32_t ende[2])
{
    if (!s_becken_fn) return 0;
    int32_t c = re15_cos_q12(y0), sn = re15_sin_q12(y0);
    int32_t rx = sx, rz = sz;
    for (int t = 0; t < b->n; t++) {
        rx += (int32_t)(((int64_t)c * b->w[t][0] + (int64_t)sn * b->w[t][1]) >> 12);
        rz += (int32_t)((-(int64_t)sn * b->w[t][0] + (int64_t)c * b->w[t][1]) >> 12);
        if (t % 3 != 0 && t != b->n - 1) continue;
        int yaw = b->knick[t] ? ((y0 + 1024) & 0xfff) : y0;
        if (s_becken_fn(rx, rz, GR2_KLEMM_R)) return 0;
        /* (3b) keine Beruehrung durch die Push-Kreise des Boss-Moduls (laengs der Entity-Gier): sonst setzt der Push
         * g->maul_kontakt und der Nutzer-Design-Biss faellt sofort nach der Wende (gemessen unit intro: Biss im
         * ersten CHASE-Takt). RE2 liest +0x238 = 110 nach dem Intro nur in der Hysterese (@0x80100b5c/bf4/cb4). */
        if (pl && s_kontakt_fn && s_kontakt_fn(rx, rz, yaw, pl->x, pl->z)) return 0;
        for (int i = 0; i < GR2_BAHN_BONES; i++) {
            int32_t bx, bz; gr2_dreh(yaw, b->b[t][i][0], b->b[t][i][1], &bx, &bz);
            bx += rx; bz += rz;
            if (s_becken_fn(bx, bz, 0)) return 0;
            if (pl) { int64_t qx = bx - pl->x, qz = bz - pl->z;
                      if (qx * qx + qz * qz < (int64_t)GR2_LEON_MIN * GR2_LEON_MIN) return 0; }
            if (t == b->n - 1 && i == 3 && kopf_ende) *kopf_ende = pl ? gr2_abstand(bx, bz, pl->x, pl->z) : 0;
        }
    }
    if (ende) { ende[0] = rx; ende[1] = rz; }
    return 1;
}
/* Wurzel nach den ersten n_bis Bildern ab (sx,sz) mit Gier y0, Schritt fuer Schritt wie gr2_bahn_frei. */
static void gr2_bahn_bis(const gr2_bahn_t *b, int n_bis, int32_t sx, int32_t sz, int y0, int32_t ende[2])
{
    int32_t c = re15_cos_q12(y0), sn = re15_sin_q12(y0);
    int32_t rx = sx, rz = sz;
    for (int t = 0; t < n_bis; t++) {
        rx += (int32_t)(((int64_t)c * b->w[t][0] + (int64_t)sn * b->w[t][1]) >> 12);
        rz += (int32_t)((-(int64_t)sn * b->w[t][0] + (int64_t)c * b->w[t][1]) >> 12);
    }
    ende[0] = rx; ende[1] = rz;
}
static void gr2_bahn_ende(const gr2_bahn_t *b, int32_t sx, int32_t sz, int y0, int32_t ende[2]) { gr2_bahn_bis(b, b->n, sx, sz, y0, ende); }
static int gr2_peilung(int32_t fx, int32_t fz, int32_t tx, int32_t tz)
{
    return ((int)re15_atan2_q12(tz - fz, tx - fx) - 0x400) & 0xfff;    /* Boss-Konvention: Ost 0, Sued 1024 */
}
static int gr2_wdiff(int a, int b) { int d = ((a - b + 0x800) & 0xfff) - 0x800; return d < 0 ? -d : d; }

/* Ergebnis-Speicher: die Lage haengt nur von (Gator x/z/Gier, Leon x/z, Skala) ab (Bahn, Becken und Push-Kreise sind
 * feste Daten) - gleiche Eingabe, gleiche Lage. Im Spiel einmal je Raumeintritt; die Sweep-Riegel (probe_gator_sweep:
 * 3342 Paare, 31 Leon-Lagen, Gator immer am Lauerplatz) rechnen so jede Eingabe nur einmal. */
#define GR2_LAGE_SPEICHER 32
static struct { uint8_t ok, gefunden; int16_t yaw_in, yaw_aus; int32_t gx, gz, lx, lz, s, ax, az; } s_lage[GR2_LAGE_SPEICHER];
static void gr2_intro_lage_rechnen(re15_actor_t *e, const re15_actor_t *pl);
void re15_gr2_intro_lage(int slot, re15_actor_t *e, const re15_actor_t *pl)
{
    (void)slot;
    if (!pl) { re15_gr2_log("INTRO Lage: kein Leon - Start am Ort"); return; }
    int32_t s = gr2_scale(e);
    unsigned h = ((unsigned)(e->x ^ (e->z << 3) ^ (pl->x << 7) ^ (pl->z << 11) ^ ((int)e->rot_y << 5))) % GR2_LAGE_SPEICHER;
    if (s_lage[h].ok && s_lage[h].gx == e->x && s_lage[h].gz == e->z && s_lage[h].yaw_in == (int16_t)e->rot_y
        && s_lage[h].lx == pl->x && s_lage[h].lz == pl->z && s_lage[h].s == s) {
        if (s_lage[h].gefunden) {
            re15_gr2_log("INTRO Lage (Speicher): von (%d,%d) yaw %d -> (%d,%d) yaw %d", e->x, e->z, (int)e->rot_y & 0xfff,
                         s_lage[h].ax, s_lage[h].az, (int)s_lage[h].yaw_aus & 0xfff);
            e->x = s_lage[h].ax; e->z = s_lage[h].az; e->rot_y = s_lage[h].yaw_aus;
        }
        return;
    }
    s_lage[h].ok = 1; s_lage[h].gx = e->x; s_lage[h].gz = e->z; s_lage[h].yaw_in = (int16_t)e->rot_y;
    s_lage[h].lx = pl->x; s_lage[h].lz = pl->z; s_lage[h].s = s;
    int32_t x0 = e->x, z0 = e->z; int16_t y0 = e->rot_y;
    gr2_intro_lage_rechnen(e, pl);
    s_lage[h].gefunden = (uint8_t)(e->x != x0 || e->z != z0 || e->rot_y != y0);
    s_lage[h].ax = e->x; s_lage[h].az = e->z; s_lage[h].yaw_aus = e->rot_y;
}
static void gr2_intro_lage_rechnen(re15_actor_t *e, const re15_actor_t *pl)
{
    const gr2_bahn_t *b = gr2_bahn(e, 0);
    if (!b || !s_becken_fn) { re15_gr2_log("INTRO Lage: keine Bahn/Becken - Start am Ort"); return; }
    static const int k_tol[3] = { 128, 256, 512 };
    int yaw_jetzt = (int)e->rot_y & 0xfff;
    for (int st = 0; st < 3; st++) {
        int64_t best = -1; int32_t bx = 0, bz = 0, bk = 0; int by = 0, bdl = 0;
        int y_basis = yaw_jetzt & ~31;                      /* Raster 32, von der aktuellen Gier nach aussen */
        for (int yi = 0; yi < 128; yi++) {
            int y0 = (y_basis + ((yi & 1) ? (yi + 1) / 2 : -(yi / 2)) * 32) & 0xfff;
            if (best >= 0 && 4 * (int64_t)gr2_wdiff(y0, yaw_jetzt) >= best) break;   /* jede weitere Gier dreht mehr */
            int ye = (y0 + 1024) & 0xfff;
            int32_t e0[2]; gr2_bahn_ende(b, 0, 0, y0, e0);
            int32_t ex = e0[0], ez = e0[1];
            int32_t m0[2]; gr2_bahn_bis(b, b->n_a, 0, 0, y0, m0);     /* Wurzel am Ende von Clip 6 (relativ) */
            for (int dl = -k_tol[st]; dl <= k_tol[st]; dl += 32) {
                int32_t ux, uz; gr2_dreh((ye + dl) & 0xfff, 4096, 0, &ux, &uz);   /* Richtung Ende -> Leon, Q12 */
                int64_t rest = 4 * (int64_t)gr2_wdiff(y0, yaw_jetzt) + 16 * (int64_t)(dl < 0 ? -dl : dl);
                if (best >= 0 && rest >= best) continue;
                /* S(d) = Leon - d*u - e0; |S(d) - G| ist am kleinsten bei d* = (Leon - e0 - G).u und waechst monoton
                 * mit |d - d*| -> die Abstaende (Raster 250 in [1000, 24000]) von d* nach aussen ablaufen */
                int64_t axv = (int64_t)pl->x - ex - e->x, azv = (int64_t)pl->z - ez - e->z;
                int64_t dstern = (axv * ux + azv * uz) >> 12;
                int i0 = (int)((dstern - 1000 + 125) / 250);
                if (i0 < 0) i0 = 0; else if (i0 > 92) i0 = 92;
                int lo = i0, hi = i0 + 1;
                while (lo >= 0 || hi <= 92) {
                    int i;
                    if (lo < 0) i = hi++;
                    else if (hi > 92) i = lo--;
                    else {
                        int64_t dlo = dstern - (1000 + 250 * lo), dhi = (1000 + 250 * hi) - dstern;
                        if (dlo < 0) dlo = -dlo;
                        if (dhi < 0) dhi = -dhi;
                        i = (dlo <= dhi) ? lo-- : hi++;
                    }
                    int32_t d = 1000 + 250 * i;
                    int32_t enx = pl->x - (int32_t)(((int64_t)ux * d) >> 12);    /* Bahnende (Wurzel) */
                    int32_t enz = pl->z - (int32_t)(((int64_t)uz * d) >> 12);
                    int32_t sx = enx - ex, sz = enz - ez;
                    int64_t qx = (int64_t)sx - e->x, qz = (int64_t)sz - e->z, q2 = qx * qx + qz * qz;
                    if (best >= 0) { int64_t lim = best - rest; if (lim <= 0 || q2 >= lim * lim) break; }   /* weitere d weiter weg */
                    /* billige Vorpruefung: Wurzel am Bahnende, am Start und am Clip-6-Ende im Wasser ausserhalb der Klemmzone */
                    if (s_becken_fn(enx, enz, GR2_KLEMM_R) || s_becken_fn(sx, sz, GR2_KLEMM_R)
                        || s_becken_fn(sx + m0[0], sz + m0[1], GR2_KLEMM_R)) continue;
                    int32_t kopf = 0, ende[2];
                    if (!gr2_bahn_frei(b, sx, sz, y0, pl, &kopf, ende)) continue;
                    int abw = gr2_wdiff(ye, gr2_peilung(ende[0], ende[1], pl->x, pl->z));
                    if (abw > k_tol[st]) continue;                 /* am echten Bahnende nachgeprueft */
                    best = (int64_t)gr2_abstand(sx, sz, e->x, e->z) + rest; bx = sx; bz = sz; by = y0; bk = kopf; bdl = abw;
                    break;                                          /* weitere d dieser Gier/Toleranz nur teurer */
                }
            }
        }
        if (best >= 0) {
            re15_gr2_log("INTRO Lage (Pos_set-Zwilling, Nachbesserung 2): von (%d,%d) yaw %d -> (%d,%d) yaw %d, End-Gier %d, "
                         "Peilung zu Leon (%d,%d) Abw. |%d| (Toleranz %d), Kopf-Leon am Ende %d",
                         e->x, e->z, yaw_jetzt, bx, bz, by, (by + 1024) & 0xfff, pl->x, pl->z, bdl, k_tol[st], bk);
            e->x = bx; e->z = bz; e->rot_y = (int16_t)by;
            return;
        }
    }
    re15_gr2_log("INTRO Lage: keine freie Bahn zu Leon (%d,%d) - Start am Ort (%d,%d) yaw %d", pl->x, pl->z, e->x, e->z, yaw_jetzt);
}
int re15_gr2_dash_planen(int slot, const re15_actor_t *e, const re15_actor_t *pl, int32_t ziel_vor[2])
{
    gr2_t *g = &s_g[slot];
    g->dash_gier = -1;
    const gr2_bahn_t *b = gr2_bahn(e, 1);
    if (!b || !pl || !s_becken_fn) return 0;
    int yaw_jetzt = (int)e->rot_y & 0xfff, best_y = -1, best_a = 0, best_k = 0;
    int64_t best = -1;
    int y_basis = yaw_jetzt & ~15;                          /* Raster 16, von der aktuellen Gier nach aussen */
    for (int yi = 0; yi < 256; yi++) {
        int y0 = (y_basis + ((yi & 1) ? (yi + 1) / 2 : -(yi / 2)) * 16) & 0xfff;
        int dreh = gr2_wdiff(y0, yaw_jetzt);
        if (dreh > 0x400) continue;                         /* hoechstens eine Vierteldrehung vor dem Dash */
        if (best >= 0 && 4 * (int64_t)dreh >= best) break;  /* jede weitere Gier dreht mehr */
        int32_t en[2]; gr2_bahn_ende(b, e->x, e->z, y0, en);
        int a = gr2_wdiff((y0 + 1024) & 0xfff, gr2_peilung(en[0], en[1], pl->x, pl->z));
        int64_t k = 4 * (int64_t)dreh + (int64_t)a;         /* kleinste Drehung, dann Ausrichtung auf Leon */
        if (best >= 0 && k >= best) continue;
        int32_t kopf = 0;
        /* ohne Leon-Bedingungen: der Dash IST der Angriff (RE2 oeffnet das Maul in Clip 3 ab Bild 114 in Zone 2,
         * `+0x218 |= 0x10` @0x80100e64-a4) - geprueft werden nur Wasser und Klemmzone */
        if (!gr2_bahn_frei(b, e->x, e->z, y0, NULL, &kopf, NULL)) continue;
        best = k; best_y = y0; best_a = a; best_k = kopf;
    }
    if (best_y < 0) {
        re15_gr2_log("DASH entfaellt (Nachbesserung 2): keine Start-Gier (+-0x400) mit freier Wende an (%d,%d) yaw %d, Leon (%d,%d)",
                     e->x, e->z, yaw_jetzt, pl->x, pl->z);
        return 0;
    }
    g->dash_gier = (int16_t)best_y;
    if (ziel_vor) gr2_bahn_ende(b, e->x, e->z, best_y, ziel_vor);
    re15_gr2_log("DASH Planung: an (%d,%d) yaw %d -> Start-Gier %d (Drehung %d), End-Gier %d, Abw. zu Leon %d",
                 e->x, e->z, yaw_jetzt, best_y, gr2_wdiff(best_y, yaw_jetzt), (best_y + 1024) & 0xfff, best_a);
    (void)best_k;
    return 1;
}

/* ---- Dash ---------------------------------------------------------------------------------- */
static void gr2_wand_messen(gr2_t *g, const re15_actor_t *e);   /* Messzeile (Nachbesserung 2, M2), s.u. */
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
static void gr2_dash_los(gr2_t *g, re15_actor_t *e)
{
    gr2_se(g, 4);                                           /* P0: `addiu a0,zero,4` @0x80100d64, jal @0x80100d84 */
    e->motion = 2; e->anim_frame = 0; gr2_blend(e, 16);     /* Clip 2 Blend 16 @0x80100dac-b4 */
    g->im_dash = 1; g->dash_phase = 1;                      /* +0x225 = 1 @0x80100e04-0c */
    g->dash_vor = 0;
    g->putz_t = 1;
    re15_gr2_log("DASH start pos=(%d,%d) yaw=%d putz_um=(%d,%d,%d)", e->x, e->z, (int)e->rot_y,
                 g->putz_x, g->putz_y, g->putz_z);
}
void re15_gr2_dash_start(int slot, re15_actor_t *e, const re15_actor_t *pl)
{
    gr2_t *g = &s_g[slot];
    g->dash_vor = 0;                                        /* kein Rest einer abgebrochenen Vordrehung (R4/Treffer) */
    /* PORT-WAHL: Putz-Sequenz um Leon (s. k_putz) - Lage = Leons Stand beim Dash-Start (RE2: Kamera 9 am
     * Korridorende, dort laeuft Leon, sub13 @0x0778/0x0786). Nachbesserung 1 (M2): vorher blieb putz_x/y/z
     * ungesetzt (0,0,0) = noerdlich ausserhalb des Beckens (z > -5400). */
    g->putz_x = pl ? pl->x : e->x; g->putz_y = pl ? pl->y : 0; g->putz_z = pl ? pl->z : e->z;
    /* Nachbesserung 2 (M2): erst auf die geplante Start-Gier drehen (gr2_dash_planen), dann SE 4 + Clip 2 */
    if (g->dash_gier >= 0 && gr2_wdiff(g->dash_gier, (int)e->rot_y & 0xfff) != 0) {
        g->dash_vor = 1; g->dash_phase = 0;
        re15_gr2_log("DASH Vordrehung yaw %d -> %d F%u", (int)e->rot_y & 0xfff, (int)g->dash_gier, (unsigned)g_engine.frame_count);
        return;
    }
    gr2_dash_los(g, e);
}
/* Ein Tick FUN_8001a330(em, clip, blend, 0) im Port-Modell (Bild = e->anim_frame NACH dem Tick):
 *   FUN_8002959c/80029614 rechnet die Pose fuer +0x14D, erhoeht dann +0x14D (`lbu v0,333 / addiu v0,v0,1 /
 *   sb v0,333` @0x80029b28-34) und meldet 1, sobald der neue Wert die Bildzahl erreicht (`sltu v0,v0,s3`
 *   @0x80029b3c, `sb zero,333 / addiu v0,zero,1` @0x80029b48-4c); FUN_8001a330 laesst dann FUN_800152c8 AUS
 *   (`bne v0,zero,0x8001a3cc` @0x8001a3b0): das LETZTE Bild wird gezeigt, aber sein Wurzelschritt nicht mehr
 *   angewandt, und der Rufer fuehrt seine Ende-Aktionen im selben Tick aus (Dash: +0x225 = 0, yaw += 1024).
 *   Port: Rueckgabe 2 = letztes Bild liegt an (Ende-Aktionen jetzt), der folgende Tick schaltet um. */
static int gr2_clip_tick(re15_actor_t *e, const re15_enemy_bank_t *bk, int clip)
{
    int n = bk->anim.clips[clip].frame_count;
    int f = (int)e->anim_frame + 1;                         /* Bildvorschub @0x80029b28-34 */
    if (f >= n - 1) { e->anim_frame = (uint16_t)(n - 1); return 2; }   /* letztes Bild, kein 152c8 @0x8001a3b0 */
    gr2_wurzel_schritt(e, bk, clip, f);                     /* FUN_80015e7c + FUN_800152c8 fuer Bild f */
    e->anim_frame = (uint16_t)f;
    return 1;
}
int re15_gr2_dash_tick(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (g->dash_vor && bk) {                                /* Nachbesserung 2 (M2): Vordrehung 0x60/Bild (PORT-WAHL) */
        int d = (((int)g->dash_gier - (int)e->rot_y + 0x800) & 0xfff) - 0x800;
        if (d > 0x60) d = 0x60; else if (d < -0x60) d = -0x60;
        e->rot_y = (int16_t)((e->rot_y + d) & 0xfff);
        int n = bk->anim.clips[e->motion < bk->anim.clip_count ? e->motion : 0].frame_count;
        if (++e->anim_frame >= n) e->anim_frame = 0;        /* laufender Verfolgungs-Clip weiter, ohne Vortrieb */
        if (((int)e->rot_y & 0xfff) == (int)g->dash_gier) { g->dash_vor = 0; gr2_dash_los(g, e); }
        return 1;
    }
    if (!g->dash_phase || !bk) { g->im_dash = 0; g->dash_phase = 0; return 0; }
    if (g->dash_endbild) {                                  /* voriger Tick: FUN_8001a330 == 1 */
        g->dash_endbild = 0;
        if (g->dash_phase == 1) {                           /* P2: Clip 3 (0) @0x80100e20-28 */
            g->dash_phase = 2; e->motion = 3; e->anim_frame = 0; e->anim_frac = 0;
            return 1;
        }
        g->im_dash = 0; g->dash_phase = 0;                  /* Routine 0x101 @0x80100ee4 */
        return 0;
    }
    int clip = (g->dash_phase == 1) ? 2 : 3;
    e->motion = (uint8_t)clip;
    if (g->dash_phase == 1 && e->anim_frame == 0) { g->wand_max = 0; g->kopf_x_min = 0x7fffffff; }
    gr2_wand_messen(g, e);                                  /* Messzeile "DASH ende ... wand_draussen_max" */
    if (gr2_clip_tick(e, bk, clip) == 2) {                  /* letztes Bild liegt an */
        g->dash_endbild = 1;
        if (g->dash_phase == 2) {
            /* Ende-Aktionen P2 @0x80100eb0-c0: `lhu v0,118(s1) / sb zero,549(s1) (+0x225 = 0) / addiu v0,v0,1024 /
             * sh v0,118(s1)` - im Tick des letzten Bildes (Root-ry von Clip 3: 438 -> 1023 ueber f70..f148, f149 = 0,
             * EMR/Probe nb1/probe_bahn): die Vierteldrehung wandert OHNE Pop in den Entity-Yaw. Nachbesserung 1:
             * vorher weggelassen (als "Korridor-Ecke" gelesen) und erst einen Tick spaeter -> 90-Grad-Pop. */
            e->rot_y = (int16_t)((e->rot_y + 1024) & 0x0fff);
            gr2_wand_messen(g, e);
            re15_gr2_log("DASH ende pos=(%d,%d) yaw=%d wand_draussen_max=%d kopf_x_min=%d", e->x, e->z, (int)e->rot_y,
                         (int)g->wand_max, (int)g->kopf_x_min);
        }
    }
    return 1;
}
int re15_gr2_im_dash(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].im_dash : 0; }
/* Messhaken (Nachbesserung 2): Bone-Weltlage MIT der Zeichen-Skala +0x166 (ScaleMatrix um den Ursprung @0x8001e904-40) -
 * re15_enemy_bone_world_pos (re15_damage.c) rechnet in voller Groesse, der Boss wird mit 2731/4096 gezeichnet. */
int re15_gr2_bone_welt_pub(const re15_actor_t *e, int bone, int32_t pos[3]) { int32_t r[9]; return gr2_bone_welt(e, bone, r, pos); }
/* Messzeile (Nachbesserung 2, M2): Aussenabstand der gezeichneten Bones 0/1/5/6/7 zum Wasser dieses Bildes. */
static void gr2_wand_messen(gr2_t *g, const re15_actor_t *e)
{
    if (!s_becken_fn) return;
    for (int i = 0; i < GR2_BAHN_BONES; i++) {
        int32_t r[9], p[3];
        if (!gr2_bone_welt(e, k_bahn_bone[i], r, p)) continue;
        int32_t d = s_becken_fn(p[0], p[2], 0);
        if (d > g->wand_max) g->wand_max = d;
        if (k_bahn_bone[i] == 6 && p[0] < g->kopf_x_min) g->kopf_x_min = p[0];
    }
}
int re15_gr2_dash_vordrehung(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].dash_vor : 0; }

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
    /* Nachtrag 4: RE2 laesst Clip 6 im Szenen-Cut 10 anlaufen (ROOM40A0 sub02 `29 0a` @SCD 0x00E0, 78 Bilder vor
     * `Evt_exec 0x0b` @0x00EE -> sub11 Member_set2 +0x1D4 -> Sub 0 P1) = Blick DURCHBRUCH, Sperre wie 0x110 @0x8010087c. */
    if (gr2_kam_auto()) gr2_kam_schnitt(GR2_KAM_INTRO, RE15_GR2_BLICK_DURCHBRUCH, e, NULL);
}
int re15_gr2_intro_tick(int slot, re15_actor_t *e)
{
    gr2_t *g = &s_g[slot];
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!g->intro_phase || !bk) { g->intro_phase = 0; return 0; }
    if (g->intro_endbild) {                                 /* voriger Tick: FUN_8001a330 == 1 */
        g->intro_endbild = 0;
        if (g->intro_phase == 1) {
            /* P2 Clip 3 @0x801008e4-f0. RE2 setzt davor Cut 1 + z = -21212 (@0x801008c4-e0, Korridor-Ecke);
             * PORT-WAHL: am Ort weiter (kein Kameraskript, kein Teleport). */
            g->intro_phase = 2; e->motion = 3; e->anim_frame = 0; e->anim_frac = 0;
            return 1;
        }
        g->intro_phase = 0;                                 /* Routine 0x101 @0x80100928-2c -> CHASE */
        return 0;
    }
    int clip = (g->intro_phase == 1) ? 6 : 3;
    e->motion = (uint8_t)clip;
    if (g->intro_phase == 1 && e->anim_frame == 0) { g->wand_max = 0; g->kopf_x_min = 0x7fffffff; }
    gr2_wand_messen(g, e);                                  /* Messzeile "INTRO ende ... wand_draussen_max" */
    int rc = gr2_clip_tick(e, bk, clip);
    int kam = (s_kam.aktiv && s_kam.programm == GR2_KAM_INTRO);   /* Nachtrag 4: Intro-Programm laeuft */
    /* Nachtrag 4: Bild 70 -> Cut 0 (`addiu v0,zero,70 / bne v1(+0x14D),v0` @0x80100894-98 VOR dem Clip-Tick = das
     * Bild 70 wird in diesem Tick gezeigt; port: anim_frame nach gr2_clip_tick). */
    if (kam && g->intro_phase == 1 && rc == 1 && e->anim_frame == 70)
        gr2_kam_schnitt(GR2_KAM_INTRO, RE15_GR2_BLICK_SEITE, e, NULL);
    if (rc == 2) {                                          /* letztes Bild liegt an (s. gr2_clip_tick) */
        g->intro_endbild = 1;
        if (kam && g->intro_phase == 1)                     /* Clip-6-Ende -> Cut 1 (@0x801008bc-c8) */
            gr2_kam_schnitt(GR2_KAM_INTRO, RE15_GR2_BLICK_ANGRIFF, e, NULL);
        if (g->intro_phase == 2) {                          /* Ende-Aktionen P2 @0x801008f8-948 */
            g->biss_cd = RE15_GR2_BISS_CD_INTRO;            /* +0x238 = 110 @0x8010091c-20 */
            g->letzter_clip = 0;                            /* Routine 0x101 @0x80100928-2c */
            e->rot_y = (int16_t)((e->rot_y + 1024) & 0x0fff);   /* `lhu v0,118 / addiu v0,v0,1024 / sh v0,118` @0x80100938-48 */
            gr2_wand_messen(g, e);
            re15_gr2_log("INTRO ende pos=(%d,%d) yaw=%d wand_draussen_max=%d kopf_x_min=%d", e->x, e->z, (int)e->rot_y,
                         (int)g->wand_max, (int)g->kopf_x_min);
            if (kam) gr2_kam_frei(e);                       /* Bits 0x110 frei @0x801008f8-918 (Nachtrag 4) */
        }
    }
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
/* Runde 41 Spur G, G-E1: die Pruefpunkte der R1-Kamerawahl - Kopf (Bone 6), Kiefer (Bone 7) und die sechs Spritzring-Punkte
 * (Kanister-Weltlage x k_ring, wie FUN_8001d894 sie fuer die Spritzring-Plaetze rechnet: t + R * ofs). 0 = keine Pose/kein Kanister. */
static uint16_t gr2_r1_skala(int ring) { return (uint16_t)(k_ring_a0[ring] & 0xffffu); }
/* Runde 41 Spur G, G-E1: Pruefpunkte der R1-Kamerawahl (Herleitung am Kopf von gr2_r1_beurteilen). 0 = keine Pose / kein Kanister. */
static int gr2_r1_pfad(const re15_actor_t *e, gr2_r1_pfad_t *P)
{
    int32_t r[9], kp[3], kr[9];
    memset(P, 0, sizeof *P);
    if (!gr2_bone_welt(e, 6, r, P->start[0]) || !gr2_bone_welt(e, 7, r, P->start[1])) return 0;
    if (!re15_gr2_kanister_welt(kp, kr)) return 0;
    for (int k = 0; k < 6; k++)                                 /* Spritzring: t + R * ofs wie FUN_8001d894 im Spawnbild */
        for (int i = 0; i < 3; i++)
            P->start[2 + k][i] = kp[i] + (int32_t)(((int64_t)kr[i * 3 + 0] * k_ring[k][0] + (int64_t)kr[i * 3 + 1] * k_ring[k][1] +
                                                    (int64_t)kr[i * 3 + 2] * k_ring[k][2]) >> 12);
    P->wurzel0[0] = e->x; P->wurzel0[1] = e->y; P->wurzel0[2] = e->z;
    /* Vorausschau des Todes-Clips: P1 von re15_gr2_r1_tick auf einer Kopie (Rueckstoss, Ziel-Gier, Clip 9 absolut vom Anker). */
    re15_enemy_bank_t *bk = gr2_bank(e);
    const int slot = (int)(e - g_actors);
    if (!bk || slot <= 0 || slot >= RE15_ACTOR_MAX || bk->anim.clip_count <= 9) return 1;
    const gr2_t *g = &s_g[slot];
    re15_actor_t t = *e;
    int16_t speed = g->r1_speed, schritt = g->r1_schritt;
    const int n = bk->anim.clips[9].frame_count;
    for (int k = 0; k < n; k++) {
        if (schritt > 0) { speed = (int16_t)(speed + schritt); schritt = (int16_t)(schritt - 2); }   /* @0x8010257c-a0 */
        {
            int32_t v = q12mul(-(int32_t)speed, gr2_scale(&t));
            int32_t c = re15_cos_q12((int)t.rot_y), sn = re15_sin_q12((int)t.rot_y);
            t.anchor_x += q12mul(c, v); t.anchor_z -= q12mul(sn, v);
        }
        {
            int d = (((int)g->r1_zielyaw - (int)t.rot_y + 0x800) & 0xfff) - 0x800;   /* FUN_8001569c @0x801025f8-608 */
            if (d > 75) d = 75; else if (d < -75) d = -75;
            t.rot_y = (int16_t)((t.rot_y + d) & 0xfff);
        }
        t.motion = 9; t.anim_frame = (uint32_t)k;
        {   const int32_t ox = t.x, oz = t.z;
            re15_clip_root_motion_abs_pub(&t, &bk->skel, &bk->anim, 9, k);
            if (g_room_rdt_ok && (t.x != ox || t.z != oz)) {      /* Wandklemme + Anker wie re15_gator_boss_tick (2) */
                int32_t nx = t.x, nz = t.z;
                re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, t.hit_radius_min, t.y, 4u);
                t.anchor_x += nx - t.x; t.anchor_z += nz - t.z; t.x = nx; t.z = nz;
            } }
        if ((k & 1) == 0 && P->n < GR2_R1_PROBEN) {
            if (!gr2_bone_welt(&t, 6, r, P->kopf[P->n]) || !gr2_bone_welt(&t, 7, r, P->kiefer[P->n])) break;
            P->wurzel[P->n][0] = t.x; P->wurzel[P->n][1] = t.y; P->wurzel[P->n][2] = t.z;
            P->n++;
        }
    }
    return 1;
}

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
    /* Kanister loesen: Weltlage aus der Matrix (@0x80102300-24), 9 Bilder sichtbar (@0x80102394). Nachbesserung 4 (M1): die
     * Matrix ist die zuletzt GEZEICHNETE (re15_gr2_nachtakt des Vorbilds) - die, auf die geschossen wurde; kein Nachrechnen
     * aus der Pose am Takt-Anfang mehr. Ein noch ausstehendes Anhaengen (Treffer im selben Takt wie t == 13) faellt aus:
     * der Kanister liegt dann noch, s_kan.pos gilt. */
    if (s_kan.aktiv) {
        s_kan.anhaengen = 0;
        if (!s_kan.angehaengt) gr2_kan_welt_setzen(s_kan.rot, s_kan.pos);
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
    s_vsync = 4;                                            /* `addiu v0,zero,4 / sb v0,0x800dfc1a` @0x8010235c-64 */
    re15_gr2_log("R1 VSync 4 (Zeitlupe) F%u", (unsigned)g_engine.frame_count);
    g->fontaene = 1;                                        /* Callback +0x234 = FUN_80103eb4 @0x80102368 */
    /* Spritzring @0x80102374 -> 0x8010338c: 6 Effekte an der Kanister-Matrix */
    if (s_kan.aktiv)
    {   /* Kanister-Matrix = aktuelle Weltlage (im Maul: Part 6 x Lokallage), dauerhafter Puffer (s.o.) */
        int32_t kp[3], kr[9];
        if (re15_gr2_kanister_welt(kp, kr)) gr2_mtx(s_fx_mtx[GR2_FXM_KANISTER], kr, kp);
        for (int k = 0; k < 6; k++) gr2_fx(k_ring_a0[k], 0, s_fx_mtx[GR2_FXM_KANISTER], k_ring[k]);
    }
    if (re15_gr2_raum_se_hook) re15_gr2_raum_se_hook(15);   /* Se_on(0x020f0001) @0x8010237c-88 */
    g->r1_zielyaw = (uint16_t)((e->rot_y + 160) & 0xfff);  /* @0x8010239c-a4 */
    g->r1_speed = -500; g->r1_schritt = 40;                 /* @0x8010246c-70, @0x8010247c-84 */
    g->r1_phase = 1;                                        /* +0x6 = 1 @0x80102468-78 */
    e->motion = 9; e->anim_frame = 0; e->anim_frac = 0;     /* Clip 9 @0x80102488-98 */
    if (bk) re15_clip_anchor_set_pub(e, &bk->skel, &bk->anim, 9, 0);
    re15_gr2_log("R1 start pos=(%d,%d) hp=%d", e->x, e->z, (int)e->hp);
    gr2_kam_r1_start(e);                                    /* Nachtrag 4: Sperre @0x8010220c, Cut 5 @0x80102218-6c */
}

static void gr2_fontaene_takt(gr2_t *g, re15_actor_t *e)
{
    if (g->fontaene == 0) return;
    /* Part-Matrizen 6/7 (RE2 `addiu a2,s2,1104` @0x80103f58): Nachbesserung 4 (M1) - die Puffer fuehrt re15_gr2_nachtakt je Takt
     * aus der gezeichneten Pose nach (beim Spawn hier = das zuletzt gezeichnete Bild, am Takt-Ende das neue); kein eigenes
     * Nachrechnen aus der Pose am Takt-Anfang mehr. */
    (void)e;
    int ok6 = s_fx_ok, ok7 = s_fx_ok;
    int16_t ofs[4];
    /* Runde 41 Spur G (Nachbesserung 3, Nachtrag 28 "Explosionseffekt"): Folge 1:1 nach FUN_80103eb4 (Dossier G 10.1).
     * Tick 1 (`beq v1,v0,0x80103f20` @0x80103ed8) spawnt 0x26032000 und laeuft OHNE Sprung in den Block der Ticks 1..8
     * (@0x80103f74, vorher `else if`); 0x00064000 UND 0x00034000 an Part 7 (s2 + 1276, s0 erst im Verzoegerungsplatz
     * @0x80104030 auf + 1104), beide mit Versatz (rnd*4+300, rnd*2, rnd*2+200) (`sra 22 / sra 23 / sra 23` @0x80103f80-a4,
     * @0x80103fdc-0x80104000); 0x08024000 an Part 6 mit (rnd*2+300, rnd*2, rnd*4+200) (`sra v0,v0,22` @0x8010405c). */
    if (g->fontaene >= 1 && g->fontaene < 9) {              /* `slti v0,v1,9` @0x80103ef8 */
        if (g->fontaene == 1 && ok6) {                      /* @0x80103f20-70: 0x26032000 an Part 6 */
            ofs[0] = (int16_t)(rnd8() * 2 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 4 + 200); ofs[3] = 0;
            gr2_fx(0x26032000u, 0, s_fx_mtx[GR2_FXM_TEIL6], ofs);
        }
        if (ok7) {                                          /* @0x80103f74-0x80104028: Part 7 */
            ofs[0] = (int16_t)(rnd8() * 4 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 2 + 200); ofs[3] = 0;
            gr2_fx(0x00064000u, (int16_t)(rnd8() * 16), s_fx_mtx[GR2_FXM_TEIL7], ofs);
            ofs[0] = (int16_t)(rnd8() * 4 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 2 + 200); ofs[3] = 0;
            gr2_fx(0x00034000u, (int16_t)(rnd8() * 16), s_fx_mtx[GR2_FXM_TEIL7], ofs);
        }
        if (ok6) {                                          /* @0x8010402c-0x801040a8: Part 6 */
            ofs[0] = (int16_t)(rnd8() * 2 + 300); ofs[1] = (int16_t)(rnd8() * 2); ofs[2] = (int16_t)(rnd8() * 4 + 200); ofs[3] = 0;
            gr2_fx(0x08024000u, (int16_t)(rnd8() * 16), s_fx_mtx[GR2_FXM_TEIL6], ofs);
            gr2_fx(0x08024000u, (int16_t)(rnd8() * 16), s_fx_mtx[GR2_FXM_TEIL6], ofs);
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
        if (e->anim_frame + 1 >= n) {                       /* FUN_8001a330 == 1 @0x8010249c */
            g->r1_phase = 2;
            /* Nachtrag 4: +0x218 & 0x40 -> zurueck auf den Ausgangs-Cut (6/13 @0x801024a4-ec), Sperre bleibt bis P3 */
            if (s_kam.aktiv && s_kam.programm == GR2_KAM_R1 && s_kam.r1_geschnitten) gr2_kam_zurueck(e);
            return 1;
        }
        e->anim_frame++;
        return 1;
    }
    if (g->r1_phase == 2) { g->r1_phase = 3; return 1; }   /* @0x80102638-50 */
    /* P3 @0x80102654: +0x10E |= 0x4000, Raumflag 30 — Endzustand, Pose bleibt (Clip 9 letztes Bild) */
    if (!g->tot) {
        g->tot = 1; re15_gr2_log("R1 ende (Leiche) pos=(%d,%d)", e->x, e->z);
        s_vsync = 2;                                        /* `addiu v0,zero,2` @0x80102668, `sb` @0x8010267c */
        re15_gr2_log("R1 VSync 2 F%u", (unsigned)g_engine.frame_count);
        if (s_kam.aktiv && s_kam.programm == GR2_KAM_R1) gr2_kam_frei(e);   /* P3 `&= ~0x100` @0x80102688-94 */
    }
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
    s_kan.liege_z = z;                                      /* Bank 5 Bit 0 an dieser Stelle (sub10 @SCD 0x0712) */
    /* liegend wie nach dem Fall: Dir_set (0x690, 0, 0xfc00) ROOM40A0 sub05 @0x048C */
    re15_skel_euler_matrix(0x690, 0, (int)(int16_t)0xfc00, s_kan.rot);
    gr2_kan_welt_setzen(s_kan.rot, s_kan.pos);
    re15_gr2_log("KANISTER gesetzt (%d,%d,%d)", x, y, z);
}
void re15_gr2_kanister_entfernen(void) { memset(&s_kan, 0, sizeof s_kan); }
int  re15_gr2_kanister_stufe(void) { return s_kan.aktiv ? (int)s_kan.stufe : -1; }
int  re15_gr2_kanister_lokal(int32_t l[3])
{
    if (!s_kan.aktiv || !s_kan.angehaengt) return 0;
    l[0] = s_kan.lokal[0]; l[1] = s_kan.lokal[1]; l[2] = s_kan.lokal[2];
    return 1;
}
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
/* ==== Nachbesserung 4 (Abnahme 3, M1): Kanister und Effekt-Quellen aus der GEZEICHNETEN Pose ===================================
 * RE2: der Kanister haengt als Kind an der Part-6-WELTmatrix (+0x48 des Part-Records 6: `addiu a0,s0,1104` @0x801018d0, `sw
 * a0,128(s2)` @0x801018e0), die Fontaene an Part 6/7 (`addiu a2,s2,1104` @0x80103f58), der Spritzring an der Kanister-Matrix;
 * Effekte mit Status 0x800 kopieren die Matrix je Bild (FUN_8001d894 @0x8001d8ac-fc). Diese Weltmatrix traegt die Pose des
 * gezeichneten Bildes: Bildvorschub in der Routine (Root @0x80100200 `jalr v0`), Kopf-az +0x22A in die lokale Part-6-Matrix
 * (@0x8010037c-90 `lhu v0,1140(s2) / lhu v1,554(s0) / addu / jal 0x8008e1f4`).
 * Port: dieselbe Rechnung wie der NPC-Zeichner (main.c NPC-Pass: re15_actor_anim_select -> clip_override = (anim_flags & 4) ?
 * motion : -1 (anim_select_common.c, Gegner-Zweig), re15_compute_actor_kf(.., anim_frame), re15_skel_compute_pose mit
 * g_anim_pose_actor = Aktor = Ueberblendung + re15_gator_spine_arc_vz (+0x22A/+0x228, Bogen); Wurzel = Yaw x render_scale_q12
 * + Lage) - aber mit gesichertem und zurueckgeschriebenem Ueberblend-Schnappschuss (prev_root/prev_angles/anim_prev_valid):
 * der Zeichner rechnet im selben Bild aus demselben Schnappschuss genau diese Pose. Aufruf: re15_gator_boss_tick am Takt-Ende
 * NACH der Wand-Klemme (Endlage des Bildes), vor der Effekt-Pumpe (re15_pc_fx_takt) und vor dem Zeichnen. */
static int gr2_pose_gezeichnet(re15_actor_t *e, re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk || bk->skel.bone_count <= 7 || bk->anim.clip_count <= 0) return 0;
    int ov = (e->anim_flags & 0x04u) ? (int)e->motion : -1;
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
    int32_t root[3]; int16_t ang[32][3]; uint8_t gueltig = e->anim_prev_valid;
    memcpy(root, e->prev_root, sizeof root); memcpy(ang, e->prev_angles, sizeof ang);
    void *save = g_anim_pose_actor; g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    memcpy(e->prev_root, root, sizeof root); memcpy(e->prev_angles, ang, sizeof ang); e->anim_prev_valid = gueltig;
    return rv == 0;
}
/* Weltmatrix eines Parts der Pose wie gr2_bone_welt (Rotation = Yaw x Part, ohne Skala; Lage = Wurzel + Yaw x Skala x Part). */
static void gr2_part_welt(const re15_actor_t *e, const re15_skel_pose_t *p, int32_t rot[9], int32_t pos[3])
{
    int32_t s = gr2_scale(e);
    int32_t tr[3] = { q12mul(p->trans[0], s), q12mul(p->trans[1], s), q12mul(p->trans[2], s) };
    re15_skel_bone_to_world(tr, e->rot_y, e->x, e->y, e->z, pos);
    int32_t ry[9]; re15_skel_euler_matrix(0, (int)e->rot_y, 0, ry);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rot[r*3 + c] = (int32_t)(((int64_t)ry[r*3+0] * p->rot[0*3+c] + (int64_t)ry[r*3+1] * p->rot[1*3+c]
                                    + (int64_t)ry[r*3+2] * p->rot[2*3+c]) >> 12);
}
void re15_gr2_nachtakt(int slot, re15_actor_t *e)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !e) return;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (!gr2_pose_gezeichnet(e, poses)) return;
    int32_t r6[9], p6[3], r7[9], p7[3];
    gr2_part_welt(e, &poses[6], r6, p6);
    gr2_part_welt(e, &poses[7], r7, p7);
    memcpy(s_p6_rot, r6, sizeof s_p6_rot); memcpy(s_p6_pos, p6, sizeof s_p6_pos);
    if (s_kan.aktiv && s_kan.anhaengen) {
        /* FUN_8002d030(Part-6-Matrix, Kanister-Matrix) @0x801018f0: rel = R6^T * Rk, lokal = R6^T * (p - p6) / s
         * (Lokallage `sw v0,56(s2) / sw v1,60(s2) / sw a3,64(s2)` @0x80101910-18) */
        int32_t s = gr2_scale(e);
        int32_t d[3] = { s_kan.pos[0] - p6[0], s_kan.pos[1] - p6[1], s_kan.pos[2] - p6[2] };
        for (int r = 0; r < 3; r++) {
            int32_t v = (int32_t)(((int64_t)r6[0*3+r] * d[0] + (int64_t)r6[1*3+r] * d[1] + (int64_t)r6[2*3+r] * d[2]) >> 12);
            s_kan.lokal[r] = (int32_t)(((int64_t)v << 12) / s);
            for (int c = 0; c < 3; c++)
                s_kan.rel[r*3+c] = (int32_t)(((int64_t)r6[0*3+r] * s_kan.rot[0*3+c] + (int64_t)r6[1*3+r] * s_kan.rot[1*3+c]
                                            + (int64_t)r6[2*3+r] * s_kan.rot[2*3+c]) >> 12);
        }
        s_kan.angehaengt = 1; s_kan.anhaengen = 0;
        re15_gr2_log("KANISTER angehaengt lokal=(%d,%d,%d) F%u", s_kan.lokal[0], s_kan.lokal[1], s_kan.lokal[2],
                     (unsigned)g_engine.frame_count);
    }
    if (s_kan.aktiv && s_kan.angehaengt) gr2_kan_folgen(e, r6, p6);
    gr2_mtx(s_fx_mtx[GR2_FXM_TEIL6], r6, p6);
    gr2_mtx(s_fx_mtx[GR2_FXM_TEIL7], r7, p7);
    int32_t kp[3], kr[9];
    if (re15_gr2_kanister_welt(kp, kr)) gr2_mtx(s_fx_mtx[GR2_FXM_KANISTER], kr, kp);
    s_fx_ok = 1;
}
/* Messhaken (Nachbesserung 4, M1; PC-Zeichner nach dem NPC-Pass): Kanister-Sitz gegen die WIRKLICH gezeichnete Pose. Nach dem
 * NPC-Pass haelt der Ueberblend-Schnappschuss genau die gezeichneten Winkel/Wurzel dieses Bildes (skeleton_common.c schreibt
 * das Mischergebnis vor den Zusaetzen zurueck); mit Vorbild-Gewicht 0x1000 (anim_frac 8 x Rate 0x200) liefert
 * re15_skel_compute_pose sie samt Zusaetzen (+0x22A/+0x228/Bogen, unveraendert seit dem Zeichnen) wieder. Daraus Part 6 x
 * Lokallage gegen die Weltlage, die der Kanister-Zeichner bekommt. 1 = gemessen (angehaengt), *abw = Betrag der Differenz.
 * Zaehler, Rate und Schnappschuss werden zurueckgeschrieben (keine Wirkung). */
int re15_gr2_kanister_sitz(re15_actor_t *e, int32_t *abw)
{
    if (!e || !s_kan.aktiv || !s_kan.angehaengt || !e->anim_prev_valid) return 0;
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk || bk->skel.bone_count <= 7) return 0;
    uint8_t frac = e->anim_frac; uint16_t rate = e->anim_blend_rate; uint8_t gueltig = e->anim_prev_valid;
    int32_t root[3]; int16_t ang[32][3];
    memcpy(root, e->prev_root, sizeof root); memcpy(ang, e->prev_angles, sizeof ang);
    e->anim_frac = 8; e->anim_blend_rate = 0x200;            /* wp = 8 x 0x200 = 0x1000 = nur der Schnappschuss */
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    e->anim_frac = frac; e->anim_blend_rate = rate; e->anim_prev_valid = gueltig;
    memcpy(e->prev_root, root, sizeof root); memcpy(e->prev_angles, ang, sizeof ang);
    if (rv != 0) return 0;
    int32_t r6[9], p6[3];
    gr2_part_welt(e, &poses[6], r6, p6);
    int32_t s = gr2_scale(e);
    int32_t l[3] = { q12mul(s_kan.lokal[0], s), q12mul(s_kan.lokal[1], s), q12mul(s_kan.lokal[2], s) };
    int64_t q = 0;
    for (int r = 0; r < 3; r++) {
        int32_t soll = p6[r] + (int32_t)(((int64_t)r6[r*3] * l[0] + (int64_t)r6[r*3+1] * l[1] + (int64_t)r6[r*3+2] * l[2]) >> 12);
        int64_t d = (int64_t)soll - s_kan.welt[r];
        q += d * d;
    }
    if (abw) *abw = (int32_t)re15_squareroot0((uint32_t)(q > 0xffffffffLL ? 0xffffffffLL : q));
    return 1;
}
/* ==== Nachbesserung 5 (Abnahme 4, M1): FINISHER-BUEHNE (FRESSEN P0) + Messzeile Maul-Sitz ======================================
 * Befund (Dossier §13.1, Sonde probe_maul): nach einem toedlichen GUARD-Hochbiss lag Leon nicht im Maul - (1) die Port-Neigung
 * pitch_vz des GUARD-Aufrichtens blieb durch P0-P3 stehen, (2) die Wand-Klemme der Takt-Huelle hielt den Gator, Leon folgte dem
 * ungeklemmten Anker, (3) die authored Konfiguration (Leon 7096 voraus, P2/P3 3800-5400) lag hinter der Ostwand.
 * RE2 (selbst gelesen, §13.0): FRESSEN P0 @0x80100fbc-0x801010e4 INSZENIERT den Finisher - Gator-x auf die Korridor-Achse
 * (`addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c), Leon 10643 davor auf z -915 (@0x8010106c-84), Zusatzwinkel genullt
 * (+0x22A `sh zero,554(s0)` @0x80101050, +0x228 `sh zero,552(s0)` @0x80101054), Kanister-Objekt aus (`sw zero,1308(at)`
 * @0x80101008-0c), Schnitt auf Cut 7/14 (@0x80100fec-ff4), der diese Lage FRONTAL zeigt (Gator frontal +1,00, Leon vor der
 * Kamera, Dossier §9.2); P1-P3 setzen Gator und Leon absolut vom gemeinsamen Anker (FUN_80015cb8 @0x80101174/@0x801011cc, Lage
 * `sw v0,56(s1)` @0x80015d68 / `sw v0,64(s1)` @0x80015d90) ohne Wandkollision.
 * PORT-WAHL (Nachtrag 6 "Betreff Aligator darfst du alles machen was du willst"; Form = RE2-P0): das Becken hat keine feste
 * Korridor-Achse - die BUEHNE ist die naechste Lage + Gier, bei der (a) die ganze authored Finisher-Bahn P1-P3 im Wasser liegt
 * (Wurzel ausserhalb der Klemmzone GR2_KLEMM_R, Hals/Kopf/Kiefer Bones 1/5/6/7 im Wasser, Leons Wurzel mit dem Spielerradius 0x1c2
 * im Wasser - dieselben Massstaebe wie die Intro-Lage gr2_bahn_frei), (b) der Gator wie in RE2 Cut 7 FRONTAL zu einer
 * Zonenkamera steht (Gier = Peilung Wurzel -> Kamera) und (c) Kopf (Bone 6) und Leons Wurzel in P1-P3 im Bild dieser Kamera
 * liegen, unter der RE2-Massstabsgrenze (gr2_massstab_grenze). Kosten wie die Intro-Lage: Versatz + 4 x Gier-Drehung. Das
 * Umsetzen faellt wie in RE2 in den Finisher-Schnitt (P0: Umsetzen @0x80101048-4c und Schnitt @0x80100ff4 im selben Bild).
 * Nachbesserung 6 (Abnahme 5 M2/M3, Dossier §14): (d) nie die laufende Kamera des Biss-Bildes (gr2_fin_laufend), (e) kein gezeichnetes
 * Raum-Objekt im Sichtstrahl zu Leon/Kopf/Kiefer (gr2_fin_unverdeckt), Ordnung erst nach dem RE2-Finisher-Massstab 1354, dann
 * Versatz + 4 x Drehung (gr2_fin_fenster, Kopf der Suche). */
#define GR2_FIN_N     193     /* P1 14 Bilder (Clip 4 Bild 1..14) + P2 120 (sf 1..120) + P3 59 (Clip 11 Bild 0..58), s. gb FRESSEN */
#define GR2_FIN_P2    14
#define GR2_FIN_P3    134
#define GR2_SPIELER_R 450     /* Spielerradius 0x1c2 (Boss-Modul gb_koerper_kontakt, GR2_LEON_MIN) */
#define GR2_FIN_RING  250     /* Suchraster der Buehne = Raster der Intro-Lage (d in 250er-Schritten) */
#define GR2_BECKEN_X0 (-8900) /* Nachbesserung 6: Suchfeld der Buehne = Aussenwaende der Beckenquelle (gb_becken_draussen, SCA ROOM2090 */
#define GR2_BECKEN_X1 7200    /* Band 0 [0]-[3] x[-8900..7200] z[-27000..-5400], Dossier §13.3); was davon im Wasser liegt, entscheidet */
#define GR2_BECKEN_Z0 (-27000)/* s_becken_fn mit der Klemmzone GR2_KLEMM_R wie bisher                                                    */
#define GR2_BECKEN_Z1 (-5400)
#define RE15_GR2_KAM_MAX 16   /* Kamera-Sichten je Buehnen-Suche (ROOM2090: 13 Cuts, Zonenkameras 0..7) */
#define GR2_FIN_WEIT  27000   /* groesster Versatz = Becken-Diagonale (Aussenwaende gb_becken_draussen x[-8900..7200] = 16100,
                               * z[-27000..-5400] = 21600 -> 26940, aufs Raster 250 aufgerundet): die Suche bricht ab, sobald
                               * r >= beste Kosten, eine weitere Grenze waere nur ein Rueckfall-Risiko (Fortsetzung NB5: mit 9000
                               * brauchte die Rampenmitte bis 8997, Sonde B41nb5c/probe_bsweep) */
#define GR2_FIN_LK    15      /* Nachbesserung 6 (M3): Leons Stichpunkte je Bild = Ursprung der PL00-Knochen 0..14 - an diesen Knochen
                               * zeichnet der Spieler-Zeichner die Meshes 0..14 (main.c Spieler-Zweig; derselbe Koerper wie die
                               * Maulraum-Messung von unit_gator_fress / unit_r41_re2gator_maul) */
typedef struct {
    int     n;
    int32_t skala;
    const void *pl00;               /* Nachbesserung 6: Leons Skelett (re15_player_pl00_skel), mit dem lk[] gebaut ist (Cache-Schluessel) */
    int     nlk;                    /* Leon-Stichpunkte je Bild (0 = kein PL00-Skelett geladen: nur die Wurzel l[]) */
    int32_t w[GR2_FIN_N][2];        /* Gator-Wurzel relativ zur P0-Lage (Rig: x laengs, z quer) */
    int32_t b[GR2_FIN_N][4][3];     /* Bones 1/5/6/7 relativ zur P0-Lage (Rig x, y, Rig z), Zeichen-Skala */
    int32_t l[GR2_FIN_N][3];        /* Leons gezeichnete Wurzel relativ zur P0-Lage (Rig x, POS-y x Skala, Rig z) */
    int32_t lk[GR2_FIN_N][GR2_FIN_LK][3];   /* Nachbesserung 6 (M3): Leons gezeichnete Knochen-Urspruenge, Bezug wie l[] */
} gr2_fin_t;
static gr2_fin_t s_fin;
static const int k_fin_bone[4] = { 1, 5, 6, 7 };
static int gr2_voff(const re15_enemy_bank_t *bk, int clip, int frame, int16_t o[3], int16_t p[3])
{
    o[0] = o[1] = o[2] = 0; p[0] = p[1] = p[2] = 0;
    int kf = gr2_kf(&bk->anim_victim, clip, frame);
    if (kf < 0) return 0;
    re15_emd_get_keyframe_position(&bk->skel_victim, kf, &p[0], &p[1], &p[2]);
    return re15_emd_get_keyframe_speed(&bk->skel_victim, kf, &o[0], &o[1], &o[2]);
}
/* Die authored Finisher-Bahn im Rig der P0-Lage (Gier 0, Lage 0), Bild fuer Bild wie gb FRESSEN sie setzt:
 *  P1 (k 0..13): Gator steht (Clip 4 Bild k+1), Leon auf (10643 x s, -915) (@0x8010106c-84, Port-Form mit Zeichen-Skala) mit
 *     Opfer-Clip 0 Bild k+1, im Koppelbild (Clip-4-Bild 13 @0x801010fc-104) schon Opfer-Clip 1 Bild 0;
 *  P2 (sf 1..120): Gator = Anker + Clip-5-Versatz (Anker = P0-Lage - Clip-4-Bild-13-Versatz, FUN_80015b94 @0x80101130), Leon =
 *     Gator + s x (Opfer-Clip-1-Versatz - Clip-5-Versatz) (FUN_80015cb8 @0x80101174, gb_opfer_skalieren); sf = 120 setzt nur den
 *     Gator (Bild 120 % 120 = 0), Leon bleibt auf sf 119;
 *  P3 (Clip 11 Bild 0..58): Gator = Anker + Clip-11-Versatz (@0x801011cc), Leon geparkt auf Opfer-Clip 1 Bild 119.
 * Leons gezeichnete Wurzel = Lage + s x POS (Spieler-Zeichner mit render_scale_q12 = Gator-Skala, P0). */
static const gr2_fin_t *gr2_fin_tabelle(const re15_actor_t *e)
{
    int32_t s = gr2_scale(e);
    extern const re15_emd_skeleton_t *re15_player_pl00_skel(void);   /* player_common.c (Leons Skelett, game_step setzt es) */
    const re15_emd_skeleton_t *pl00 = re15_player_pl00_skel();
    if (s_fin.n > 0 && s_fin.skala == s && s_fin.pl00 == (const void *)pl00) return &s_fin;
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk || !bk->victim_ok || bk->skel.bone_count <= 7 || bk->anim.clip_count <= 11 || bk->anim_victim.clip_count <= 1) return NULL;
    int16_t g4[3]; gr2_off(bk, 4, 13, g4);
    int32_t vx = q12mul(10643, s);
    s_fin.n = 0; s_fin.skala = s; s_fin.pl00 = (const void *)pl00; s_fin.nlk = 0;
    /* Nachbesserung 6 (M3): Leons Koerper wie der Spieler-Zeichner im Opfer-Modus - LEONS Knochen (PL00) mit den Posen der Opfer-Bank
     * des Greifers (dieselbe Komposition wie re15_player_victim_bone_pos, enemy_ai_common.c: Original ankert am Spieler-Part-Pool
     * @0x80111dd4-e0), Gier = Gator-Gier (P0 @0x80101154-5c), Massstab = Zeichen-Skala des Gators (P0, render_scale_q12) */
    re15_emd_skeleton_t vs;
    if (pl00 && pl00->bone_count > 0) {
        vs = *pl00;
        vs.keyframe_data = bk->skel_victim.keyframe_data;       vs.keyframe_data_size  = bk->skel_victim.keyframe_data_size;
        vs.keyframe_count = bk->skel_victim.keyframe_count;     vs.keyframe_size_bytes = bk->skel_victim.keyframe_size_bytes;
        s_fin.nlk = pl00->bone_count < GR2_FIN_LK ? pl00->bone_count : GR2_FIN_LK;
    }
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    for (int k = 0; k < GR2_FIN_N; k++) {
        int gc, gf, vc, vf; int32_t wx = 0, wz = 0, lx, lz; int16_t go[3] = {0,0,0}, vo[3], vp[3];
        if (k < GR2_FIN_P2) {
            gc = 4; gf = k + 1;
            vc = (k == GR2_FIN_P2 - 1) ? 1 : 0; vf = (k == GR2_FIN_P2 - 1) ? 0 : k + 1;
            gr2_voff(bk, vc, vf, vo, vp);
            lx = vx; lz = -915;
        } else if (k < GR2_FIN_P3) {
            int sf = k - GR2_FIN_P2 + 1;
            gc = 5; gf = sf < 120 ? sf : 119;                    /* Zeichner: HOLD-LAST jenseits des Clip-Endes */
            gr2_off(bk, 5, sf % 120, go);                        /* Lage: Bild sf % 120 (re15_clip_root_motion_abs) */
            wx = go[0] - g4[0]; wz = go[2] - g4[2];
            vc = 1; vf = sf < 120 ? sf : 119;
            gr2_voff(bk, vc, vf, vo, vp);
            if (sf < 120) { lx = wx + q12mul(vo[0] - go[0], s); lz = wz + q12mul(vo[2] - go[2], s); }
            else          { lx = s_fin.l[k - 1][0] - q12mul(vp[0], s); lz = s_fin.l[k - 1][2] - q12mul(vp[2], s); }
        } else {
            gc = 11; gf = k - GR2_FIN_P3;
            gr2_off(bk, 11, gf, go);
            wx = go[0] - g4[0]; wz = go[2] - g4[2];
            vc = 1; vf = 119;
            gr2_voff(bk, vc, vf, vo, vp);
            lx = wx + q12mul(vo[0] - go[0], s); lz = wz + q12mul(vo[2] - go[2], s);
        }
        s_fin.w[k][0] = wx; s_fin.w[k][1] = wz;
        s_fin.l[k][0] = lx + q12mul(vp[0], s); s_fin.l[k][1] = q12mul(vp[1], s); s_fin.l[k][2] = lz + q12mul(vp[2], s);
        if (s_fin.nlk > 0) {                                     /* Nachbesserung 6 (M3): Leons Knochen dieses Bildes (Lage + s x trans) */
            re15_skel_pose_t vpo[RE15_EMD_MAX_BONES];
            int vkf = gr2_kf(&bk->anim_victim, vc, vf);
            if (vkf < 0 || re15_skel_compute_pose(&vs, vkf, vpo) != 0) s_fin.nlk = 0;
            for (int j = 0; j < s_fin.nlk; j++) {
                s_fin.lk[k][j][0] = lx + q12mul(vpo[j].trans[0], s);
                s_fin.lk[k][j][1] = q12mul(vpo[j].trans[1], s);
                s_fin.lk[k][j][2] = lz + q12mul(vpo[j].trans[2], s);
            }
        }
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        int kf = gr2_kf(&bk->anim, gc, gf);
        if (kf < 0 || re15_skel_compute_pose(&bk->skel, kf, poses) != 0) { g_anim_pose_actor = save; s_fin.n = 0; return NULL; }
        for (int i = 0; i < 4; i++) {
            const re15_skel_pose_t *p = &poses[k_fin_bone[i]];
            s_fin.b[k][i][0] = wx + q12mul(p->trans[0], s);
            s_fin.b[k][i][1] = q12mul(p->trans[1], s);
            s_fin.b[k][i][2] = wz + q12mul(p->trans[2], s);
        }
        s_fin.n++;
    }
    g_anim_pose_actor = save;
    return &s_fin;
}
/* (a) Bahn im Wasser: Wurzel ausserhalb der Klemmzone, Bones 1/5/6/7 im Wasser, Leons Wurzel mit Spielerradius im Wasser - in JEDEM
 * Bild der Bahn (Fortsetzung Nachbesserung 5: vorher P2/P3 nur jedes 2. Bild; der Riegel unit_r41_re2gator_maul prueft jedes Bild). */
static int gr2_fin_frei(const gr2_fin_t *t, int32_t gx, int32_t gz, int yaw, int schritt)
{
    int32_t wx, wz;
    gr2_dreh(yaw, t->l[GR2_FIN_P2 - 2][0], t->l[GR2_FIN_P2 - 2][2], &wx, &wz);     /* Vorpruefung: Leon in P1 */
    if (s_becken_fn(gx + wx, gz + wz, GR2_SPIELER_R)) return 0;
    for (int k = 0; k < t->n; k = (k + schritt < t->n || k == t->n - 1) ? k + schritt : t->n - 1) {
        gr2_dreh(yaw, t->w[k][0], t->w[k][1], &wx, &wz);
        if (s_becken_fn(gx + wx, gz + wz, GR2_KLEMM_R)) return 0;
        for (int i = 0; i < 4; i++) {
            gr2_dreh(yaw, t->b[k][i][0], t->b[k][i][2], &wx, &wz);
            if (s_becken_fn(gx + wx, gz + wz, 0)) return 0;
        }
        gr2_dreh(yaw, t->l[k][0], t->l[k][2], &wx, &wz);
        if (s_becken_fn(gx + wx, gz + wz, GR2_SPIELER_R)) return 0;
    }
    return 1;
}
/* (c) Kopf (Bone 6) und Leons Wurzel im Bild der Kamera c, Kopf-Massstab <= RE2-Grenze - in JEDEM Bild der Bahn (Fortsetzung
 * Nachbesserung 5: vorher nur Koppelbild + jedes 10. Bild; der Riegel unit_r41_re2gator_maul fand im CHASE-Bezug P1 Bild 12 Leons
 * Wurzel bei Bildlage (106,240), eine Zeile unter dem Bild). Projektion wie gr2_im_bild / gr2_massstab (FUN_80053ca4-Zwilling),
 * die Kamera-Sicht einmal je Aufruf gebaut. gy = Gator-y (Wasser); Leon steht in P1-P3 auf Gator-y (Nachbesserung 6: P0 setzt
 * pl->y = Gator-y, ab P2 Anker-y, SPEED-y 0). *kopf_bild / *leon_bild = Bildlage im Koppelbild (Logzeile). */
static int gr2_fin_proj(const re15_camera_view_t *v, const int32_t p[3], int *sx, int *sy, int32_t *vz_aus)
{
    int64_t px = p[0], py = p[1], pz = p[2];
    int32_t vx = (int32_t)(((int64_t)v->rot[0]*px + (int64_t)v->rot[1]*py + (int64_t)v->rot[2]*pz) >> 12) + v->trans[0];
    int32_t vy = (int32_t)(((int64_t)v->rot[3]*px + (int64_t)v->rot[4]*py + (int64_t)v->rot[5]*pz) >> 12) + v->trans[1];
    int32_t vz = (int32_t)(((int64_t)v->rot[6]*px + (int64_t)v->rot[7]*py + (int64_t)v->rot[8]*pz) >> 12) + v->trans[2];
    *vz_aus = vz;
    if (vz <= 0) return 0;
    *sx = 160 + (int)(((int64_t)vx * v->fov_screen_dist) / vz);
    *sy = 120 + (int)(((int64_t)vy * v->fov_screen_dist) / vz);
    return *sx >= 0 && *sx < 320 && *sy >= 0 && *sy < 240;
}
static int gr2_fin_sicht(const gr2_fin_t *t, int c, int32_t gx, int32_t gy, int32_t gz, int yaw, int32_t grenze,
                         int kopf_bild[2], int leon_bild[2], int32_t *m_max, int schritt)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[c], &v) != 0) return 0;
    *m_max = 0;
    for (int k = 0; k < t->n; k = (k + schritt < t->n || k == t->n - 1) ? k + schritt : t->n - 1) {
        int32_t wx, wz, p[3], vz = 0;
        gr2_dreh(yaw, t->b[k][2][0], t->b[k][2][2], &wx, &wz);
        p[0] = gx + wx; p[1] = gy + t->b[k][2][1]; p[2] = gz + wz;
        int ix = -1, iy = -1;
        if (!gr2_fin_proj(&v, p, &ix, &iy, &vz)) return 0;
        int32_t m = (int32_t)(((int64_t)v.fov_screen_dist * t->skala * 16) / vz);   /* = gr2_massstab */
        if (m > grenze) return 0;
        if (m > *m_max) *m_max = m;
        if (k == GR2_FIN_P2 - 1) { kopf_bild[0] = ix; kopf_bild[1] = iy; }
        gr2_dreh(yaw, t->l[k][0], t->l[k][2], &wx, &wz);
        p[0] = gx + wx; p[1] = gy + t->l[k][1]; p[2] = gz + wz;   /* NB6: Leon auch in P1 auf der Gator-Hoehe (gb FRESSEN P0) */
        /* Leon: im Bild - nur oben hinaus darf er (der Wurf in P2 Bild 60..75 hebt ihn aus dem POS-Kanal bis y -9177 roh: in RE2
         * Cut 7 liegt er dort bei Tiefe ~5040 / H 231 rund 420 Bildpunkte ueber der Mitte, also ebenfalls oben ausserhalb). */
        ix = -1; iy = -1;
        int li = gr2_fin_proj(&v, p, &ix, &iy, &vz);
        if (!li && !(vz > 0 && ix >= 0 && ix < 320 && iy < 0)) return 0;
        if (k == GR2_FIN_P2 - 1) { leon_bild[0] = ix; leon_bild[1] = iy; }
    }
    return 1;
}
/* ==== Nachbesserung 6 (Abnahme 5 M2/M3) ========================================================================================
 * M2 - LAUFENDE KAMERA: im Biss-Bild zeigt der Zeichner den Cut, den der Kamera-Apply am Bildanfang uebernommen hat (work_vars[0x0A]
 * = DAT_800b0fe4, Apply-Kopf FUN_80021bbc `sh v1,DAT_800b0fe4` @0x80021bfc), angefordert ist cam_id (= DAT_800afbb5, RVD-Scan
 * FUN_800142f4 `sb a0,-0x44b(at)` @0x80014300 / Cut_chg @0x800402fc). Nimmt die Buehne einen dieser Cuts, gibt es keinen Schnitt,
 * und das Umsetzen von Gator und Opfer ist im Bild zu sehen (Abnahme 5 G1: Cut 0 durchgehend, Gator dreht 101 Grad, Leon springt
 * 8100). RE2 setzt in P0 um (`sw v0,56(s0)` @0x80101048-4c) und schneidet im SELBEN Takt (`jal 0x8002c7ac` @0x80100ff4, Sperre
 * @0x80100fcc-d0); dort ist das Umsetzen klein (Biss nur am Korridorende, Sub 7 @0x801012e0-e4), die Port-Buehne versetzt bis zur
 * Becken-Diagonale. PORT-WAHL (Form RE2-P0 "Umsetzen nur mit Schnitt"): beide Cuts sind als Buehnen-Kamera ausgeschlossen. */
static int gr2_fin_laufend(int c) { return c == (int)g_scd.work_vars[0x0A] || c == (int)g_scd.cam_id; }
/* M3 - RAUM-OBJEKTE IM SICHTSTRAHL: Obj_model_set-Objekte als gedrehte Huellquader (MD1-Huelle aller Meshes im Objektrahmen; Welt =
 * Drehmatrix x v + Lage wie der Objekt-Zeichner FUN_8002c18c: Matrix pool+0x20 = RotMatrix(rot) in der Port-Konvention des PC-
 * Zeichners (main.c pc_prop_rot_q12, Ry*Rx*Rz), Lage x / Zeichen-y re15_prop_render_y (Typ 4 -900 @0x8002c23c-50) / z). Gezeichnet
 * wird ein Objekt in Cut c nur, wenn seine Lage im Regions-Viereck von c liegt (re15_prop_culled = FUN_80014368 ueber DAT_800ac790;
 * Typ 3 ohne Ganzobjekt-Cull) - nur diese Objekte koennen das Opfer in c verdecken. Abnahme 5 E1: Obj_model_set[3] (-25,-1740,-7423)
 * lag in P1 der Cut-4-Buehne zwischen Kamera und Leon (Bild (98,116) gegen Leons Wurzel (138,128)). RE2-Vorbild: P0 nimmt das einzige
 * Objekt, das neben dem Gator im Wasser liegen kann, aus dem Bild (Kanister obj[1] `sw zero,1308(at)` @0x80101008-0c) - der Finisher
 * zeigt nur Gator und Opfer. PORT-WAHL (Raum-Objekte bleiben stehen, die Buehne weicht aus): eine Buehne gilt nur, wenn in JEDEM Bild
 * der Bahn kein gezeichnetes Raum-Objekt den Strahl Kamera -> Leons Knochen (im Bild) bzw. -> Kopf/Kiefer (Bones 6/7) schneidet. */
typedef struct { int32_t lo[3], hi[3], r[9], t[3]; int typ; } gr2_obj_t;
static gr2_obj_t s_obj[RE15_SCD_MAX_PROPS];
static int       s_obj_n;
static re15_md1_t s_obj_md1;
/* Drehmatrix Ry*Rx*Rz (Q12) = main.c pc_prop_rot_q12 (dieselbe Formel, Objekt-Zeichner des Ports). */
static void gr2_obj_rot(int16_t rx, int16_t ry, int16_t rz, int32_t m[9])
{
    int32_t sx = re15_sin_q12((int)rx), cx = re15_cos_q12((int)rx);
    int32_t sy = re15_sin_q12((int)ry), cy = re15_cos_q12((int)ry);
    int32_t sz = re15_sin_q12((int)rz), cz = re15_cos_q12((int)rz);
    m[0] = q12mul(cz, cy);
    m[1] = -q12mul(sz, cy);
    m[2] = sy;
    m[3] = (int32_t)(((((int64_t)sz * cx) << 12) + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)(((((int64_t)cz * cx) << 12) - (int64_t)sz * sy * sx) >> 24);
    m[5] = -q12mul(cy, sx);
    m[6] = (int32_t)(((((int64_t)sz * sx) << 12) - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)(((((int64_t)cz * sx) << 12) + (int64_t)sz * sy * cx) >> 24);
    m[8] = q12mul(cy, cx);
}
/* Huellquader der aktiven Raum-Objekte neu aufbauen (Lage/Drehung koennen Skripte aendern; einmal je Buehnen-Suche/Messbild). Objekte
 * mit Elternteil (Obj_model_set pc[5] != 0, im Bestand nur ROOM1150/1151) haben keinen Weltrahmen ohne die Kette - nicht erfasst. */
static int gr2_raum_objekte(void)
{
    s_obj_n = 0;
    if (!g_room_rdt_ok) return 0;
    for (int pi = 0; pi < (int)g_scd.prop_count && pi < RE15_SCD_MAX_PROPS; pi++) {
        if (!g_scd.props[pi].active || g_scd.props[pi].parent_obj >= 0) continue;
        /* Typ 3 = Raster-Sprite (FUN_8002c18c -> FUN_8003e64c, eigene Zellen-Verdeckung): in ROOM2090 ist das obj 1, die Wasser-
         * oberflaeche (MD1-Huelle 21600 x 16200, flach y 0, Lage y -1620; Sonde B41nb6/probe_aabb) - durchscheinend, kein Verdecker:
         * jeder Strahl zu Leon im Wasser kreuzt sie (erster Lauf E1/G1: 2164 bzw. 1780 Lagen verworfen, keine Buehne). */
        if (g_scd.props[pi].obj_type == 3) continue;
        int oid = (int)g_scd.props[pi].obj_id;
        if (oid >= RE15_RDT_MAX_PROPS || !g_room_rdt.prop_md1[oid]) continue;
        if (re15_md1_parse(g_room_rdt.prop_md1[oid], g_room_rdt.prop_md1_size[oid], &s_obj_md1) != 0) continue;
        gr2_obj_t *o = &s_obj[s_obj_n];
        int nv = 0;
        for (int i = 0; i < 3; i++) { o->lo[i] = 0x7fffffff; o->hi[i] = -0x7fffffff; }
        for (int m = 0; m < s_obj_md1.mesh_count; m++)
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *V = pass ? s_obj_md1.meshes[m].quad_vertices : s_obj_md1.meshes[m].tri_vertices;
                int N = pass ? s_obj_md1.meshes[m].quad_vertex_count : s_obj_md1.meshes[m].tri_vertex_count;
                for (int v = 0; V && v < N; v++) {
                    int32_t q[3] = { V[v].x, V[v].y, V[v].z };
                    for (int i = 0; i < 3; i++) { if (q[i] < o->lo[i]) o->lo[i] = q[i]; if (q[i] > o->hi[i]) o->hi[i] = q[i]; }
                    nv++;
                }
            }
        if (nv == 0) continue;
        gr2_obj_rot(g_scd.props[pi].rot_x, g_scd.props[pi].rot_y, g_scd.props[pi].rot_z, o->r);
        o->t[0] = g_scd.props[pi].x;
        o->t[1] = re15_prop_render_y((int)g_scd.props[pi].obj_type, g_scd.props[pi].y);
        o->t[2] = g_scd.props[pi].z;
        o->typ = (int)g_scd.props[pi].obj_type;
        s_obj_n++;
    }
    return s_obj_n;
}
/* Schneidet die Strecke a -> p den Huellquader von o? (beide Punkte in den Objektrahmen: R^T x (w - t); Strecken-Parameter in Q16) */
static int gr2_obj_im_strahl(const gr2_obj_t *o, const int32_t a[3], const int32_t p[3])
{
    int64_t al[3], pl[3];
    for (int i = 0; i < 3; i++) {
        int64_t sa = 0, sp = 0;
        for (int k = 0; k < 3; k++) {
            sa += (int64_t)o->r[k * 3 + i] * (int64_t)(a[k] - o->t[k]);
            sp += (int64_t)o->r[k * 3 + i] * (int64_t)(p[k] - o->t[k]);
        }
        al[i] = sa >> 12; pl[i] = sp >> 12;
    }
    int64_t t0 = 0, t1 = 65536;
    for (int i = 0; i < 3; i++) {
        int64_t d = pl[i] - al[i];
        if (d == 0) { if (al[i] < o->lo[i] || al[i] > o->hi[i]) return 0; continue; }
        int64_t ta = (((int64_t)o->lo[i] - al[i]) * 65536) / d, tb = (((int64_t)o->hi[i] - al[i]) * 65536) / d;
        if (ta > tb) { int64_t x = ta; ta = tb; tb = x; }
        if (ta > t0) t0 = ta;
        if (tb < t1) t1 = tb;
        if (t0 > t1) return 0;
    }
    return 1;
}
/* Objekte, die in Cut c gezeichnet werden (Regions-Cull wie der Objekt-Zeichner) -> Liste; Rueckgabe Anzahl. */
static int gr2_obj_in_cut(int c, const gr2_obj_t *liste[RE15_SCD_MAX_PROPS])
{
    int16_t xs[4], zs[4];
    int reg = re15_rdt_get_region_quad(&g_room_rdt, c, xs, zs), n = 0;
    for (int i = 0; i < s_obj_n; i++)
        if (!re15_prop_culled(s_obj[i].typ, s_obj[i].t[0], s_obj[i].t[2], reg, xs, zs)) liste[n++] = &s_obj[i];
    return n;
}
/* Wie viele der Weltpunkte p[0..n-1] liegen im Bild von Cut c und sind von einem dort gezeichneten Raum-Objekt verdeckt (Strahl vom
 * Kamera-Ort)? *im_bild = Zahl der Punkte im Bild. Messzeile Maul-Sitz (gator_re2_pc.c). */
int re15_gr2_raum_verdeckt(int c, const int32_t p[][3], int n, int *im_bild)
{
    if (im_bild) *im_bild = 0;
    if (!g_room_rdt_ok || !g_room_rdt.cuts || c < 0 || c >= g_room_rdt.cut_count) return 0;
    gr2_raum_objekte();
    const gr2_obj_t *ob[RE15_SCD_MAX_PROPS];
    int nob = gr2_obj_in_cut(c, ob), verdeckt = 0;
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[c], &v) != 0) return 0;
    const re15_camera_cut_t *cc = &g_room_rdt.cuts[c];
    int32_t a[3] = { cc->pos_x, cc->pos_y, cc->pos_z };
    for (int j = 0; j < n; j++) {
        int sx, sy; int32_t vz = 0;
        if (!gr2_fin_proj(&v, p[j], &sx, &sy, &vz)) continue;
        if (im_bild) (*im_bild)++;
        for (int i = 0; i < nob; i++) if (gr2_obj_im_strahl(ob[i], a, p[j])) { verdeckt++; break; }
    }
    return verdeckt;
}
/* Nachbesserung 6 (Elza-Lauf EZT2, B41nb6/EZT2_gelb_zoom.png): Elzas Zopf (Teile 17..20, eigene Weltmatrix aus zopf_r38.c =
 * FUN_80024c30/FUN_80024e40, gezeichnet ueber Flag 0x40 -> FUN_8001e9ec @0x8001ea10) haengt nicht an der Wurzel-3x3 des Spielers. Der
 * Maul-Massstab des Finishers (render_scale_q12, PORT-WAHL Phase 3: ScaleMatrix-Pfad @0x8001e904-40 auf der Wurzel-3x3, skaliert
 * Knochen und Vertices um pl->(x,y,z)) traf deshalb den Koerper, aber nicht den Zopf: der Zopf flog in P1 als gelber Fetzen bis ~900
 * Einheiten neben Elzas Kopf (TRILOG B41nb6/mess/EZtri: Spieler-Platz 0 im Fenster ueber dem Kopf). PORT-WAHL (Folge derselben
 * Massstab-Entscheidung, im Original wird der Spieler nie skaliert): die Zopf-Weltmatrix um denselben Mittelpunkt mit demselben
 * Massstab skalieren. Ohne render_scale_q12 (jeder andere Spielzustand) wirkungslos. */
void re15_gr2_zopf_massstab(const re15_actor_t *pl, int32_t rot[9], int32_t t[3])
{
    if (!pl || !pl->render_scale_q12) return;
    int32_t s = pl->render_scale_q12;
    for (int i = 0; i < 9; i++) rot[i] = q12mul(rot[i], s);
    t[0] = pl->x + q12mul(t[0] - pl->x, s);
    t[1] = pl->y + q12mul(t[1] - pl->y, s);
    t[2] = pl->z + q12mul(t[2] - pl->z, s);
}
/* Leons gezeichnete Knochen-Ursprunge im Opfer-Modus (PL00 + Posen der Opfer-Bank, Gier, Zeichen-Skala; wie gr2_fin_tabelle) -
 * Messzeile Maul-Sitz. Rueckgabe Anzahl (0 = kein Skelett / keine Opfer-Bank). */
int re15_gr2_leon_knochen(const re15_actor_t *gator, const re15_actor_t *pl, int32_t out[][3], int max)
{
    extern const re15_emd_skeleton_t *re15_player_pl00_skel(void);
    const re15_emd_skeleton_t *pl00 = re15_player_pl00_skel();
    re15_enemy_bank_t *bk = gator ? gr2_bank(gator) : NULL;
    if (!pl || !pl00 || pl00->bone_count <= 0 || !bk || !bk->victim_ok) return 0;
    re15_emd_skeleton_t vs = *pl00;
    vs.keyframe_data = bk->skel_victim.keyframe_data;       vs.keyframe_data_size  = bk->skel_victim.keyframe_data_size;
    vs.keyframe_count = bk->skel_victim.keyframe_count;     vs.keyframe_size_bytes = bk->skel_victim.keyframe_size_bytes;
    int vkf = gr2_kf(&bk->anim_victim, (int)pl->motion, (int)pl->anim_frame);
    re15_skel_pose_t vpo[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = (vkf < 0) ? -1 : re15_skel_compute_pose(&vs, vkf, vpo);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int n = pl00->bone_count < max ? pl00->bone_count : max;
    if (n > GR2_FIN_LK) n = GR2_FIN_LK;
    int32_t sp = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
    for (int j = 0; j < n; j++) {
        int32_t P[3] = { q12mul(vpo[j].trans[0], sp), q12mul(vpo[j].trans[1], sp), q12mul(vpo[j].trans[2], sp) };
        re15_skel_bone_to_world(P, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, out[j]);
    }
    return n;
}
/* (M3) Bahn in Kamera c frei von Raum-Objekten: je Bild Leons Knochen (ohne Skelett: die Wurzel) und Kopf/Kiefer (Bones 6/7) - nur
 * Punkte im Bild zaehlen (Leon "oben hinaus" beim Wurf ist nicht zu verdecken). *k_aus = erstes verdecktes Bild. */
static int gr2_fin_unverdeckt(const gr2_fin_t *t, int c, int32_t gx, int32_t gy, int32_t gz, int yaw, int schritt, int *k_aus)
{
    const gr2_obj_t *ob[RE15_SCD_MAX_PROPS];
    int nob = gr2_obj_in_cut(c, ob);
    if (nob == 0) return 1;
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[c], &v) != 0) return 1;
    const re15_camera_cut_t *cc = &g_room_rdt.cuts[c];
    int32_t a[3] = { cc->pos_x, cc->pos_y, cc->pos_z };
    for (int k = 0; k < t->n; k = (k + schritt < t->n || k == t->n - 1) ? k + schritt : t->n - 1) {
        int32_t basis = gy;                                     /* Leon in P1-P3 auf der Gator-Hoehe (gb FRESSEN P0, NB6) */
        int np = (t->nlk > 0 ? t->nlk : 1) + 2;
        for (int j = 0; j < np; j++) {
            const int32_t *q; int32_t y0;
            if (j < np - 2) { q = t->nlk > 0 ? t->lk[k][j] : t->l[k]; y0 = basis; }
            else            { q = t->b[k][2 + (j - (np - 2))]; y0 = gy; }   /* Kopf (Bone 6) / Kiefer (Bone 7) */
            int32_t wx, wz, p[3], vz = 0; int sx, sy;
            gr2_dreh(yaw, q[0], q[2], &wx, &wz);
            p[0] = gx + wx; p[1] = y0 + q[1]; p[2] = gz + wz;
            if (!gr2_fin_proj(&v, p, &sx, &sy, &vz)) continue;       /* nicht im Bild = nicht zu verdecken */
            for (int i = 0; i < nob; i++)
                if (gr2_obj_im_strahl(ob[i], a, p)) { if (k_aus) *k_aus = k; return 0; }
        }
        if (k == t->n - 1) break;
    }
    return 1;
}
/* Nachbesserung 6: eine Buehnen-Suche (Kontext) und ein Raster-Fenster davon. */
typedef struct {
    const gr2_fin_t *t;
    const re15_actor_t *e;
    int32_t grenze, m_soll;
    int yaw0;
    re15_camera_view_t kv[RE15_GR2_KAM_MAX];
    int kok[RE15_GR2_KAM_MAX];
    int64_t best1, best2;
    int32_t bx, bz, bm, bm13;
    int by, bc, bkb[2], blb[2];
    int n_geprueft, n_verdeckt, k_verdeckt;
} gr2_fin_suche_t;
static void gr2_fin_fenster(gr2_fin_suche_t *S, int32_t x0, int32_t x1, int32_t z0, int32_t z1, int32_t schritt)
{
    const gr2_fin_t *t = S->t;
    const re15_actor_t *e = S->e;
    const int32_t *kopf13 = t->b[GR2_FIN_P2 - 1][2];      /* Kopf (Bone 6) im Koppelbild, Rig relativ zur P0-Lage */
    if (x0 < GR2_BECKEN_X0) x0 = GR2_BECKEN_X0;
    if (x1 > GR2_BECKEN_X1) x1 = GR2_BECKEN_X1;
    if (z0 < GR2_BECKEN_Z0) z0 = GR2_BECKEN_Z0;
    if (z1 > GR2_BECKEN_Z1) z1 = GR2_BECKEN_Z1;
    for (int32_t px = x0; px <= x1; px += schritt)
        for (int32_t pz = z0; pz <= z1; pz += schritt) {
            if (s_becken_fn(px, pz, GR2_KLEMM_R)) continue;
            int32_t versatz = gr2_abstand(px, pz, e->x, e->z);
            if (versatz > GR2_FIN_WEIT) continue;
            for (int c = 0; c < g_room_rdt.cut_count && c < RE15_GR2_KAM_MAX; c++) {
                if (!S->kok[c]) continue;                       /* NB6 (M2): nie die laufende Kamera (gr2_fin_laufend) */
                int y2 = gr2_peilung(px, pz, g_room_rdt.cuts[c].pos_x, g_room_rdt.cuts[c].pos_z);
                int32_t wx, wz, kp[3], vz = 0; int sx, sy;
                gr2_dreh(y2, kopf13[0], kopf13[2], &wx, &wz);
                kp[0] = px + wx; kp[1] = e->y + kopf13[1]; kp[2] = pz + wz;
                if (!gr2_fin_proj(&S->kv[c], kp, &sx, &sy, &vz)) continue;
                int32_t m13 = (int32_t)(((int64_t)S->kv[c].fov_screen_dist * t->skala * 16) / vz);   /* = gr2_massstab */
                int64_t k1 = m13 > S->m_soll ? m13 - S->m_soll : S->m_soll - m13;
                int64_t k2 = versatz + 4 * (int64_t)gr2_wdiff(y2, S->yaw0);
                if (S->best1 >= 0 && (k1 > S->best1 || (k1 == S->best1 && k2 >= S->best2))) continue;
                S->n_geprueft++;
                /* erst Stichproben (jedes 8. Bild + letztes), dann jedes Bild - gleiche Bedingungen, nur schneller verworfen */
                if (!gr2_fin_frei(t, px, pz, y2, 8)) continue;
                int kb[2] = {0, 0}, lb[2] = {0, 0}; int32_t mm = 0;
                if (!gr2_fin_sicht(t, c, px, e->y, pz, y2, S->grenze, kb, lb, &mm, 8)) continue;
                if (!gr2_fin_unverdeckt(t, c, px, e->y, pz, y2, 8, &S->k_verdeckt)) { S->n_verdeckt++; continue; }   /* NB6 (M3) */
                if (!gr2_fin_frei(t, px, pz, y2, 1) || !gr2_fin_sicht(t, c, px, e->y, pz, y2, S->grenze, kb, lb, &mm, 1)) continue;
                if (!gr2_fin_unverdeckt(t, c, px, e->y, pz, y2, 1, &S->k_verdeckt)) { S->n_verdeckt++; continue; }
                S->best1 = k1; S->best2 = k2; S->bx = px; S->bz = pz; S->by = y2; S->bc = c; S->bm = mm; S->bm13 = m13;
                S->bkb[0] = kb[0]; S->bkb[1] = kb[1]; S->blb[0] = lb[0]; S->blb[1] = lb[1];
            }
        }
}
int re15_gr2_finisher_buehne(int slot, re15_actor_t *e)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !e) return 0;
    gr2_t *g = &s_g[slot];
    g->fin_cut = -1;
    const gr2_fin_t *t = gr2_fin_tabelle(e);
    int yaw0 = (int)e->rot_y & 0xfff, gefunden = 0, n_verdeckt = 0, k_verdeckt = -1, n_obj = gr2_raum_objekte();
    if (t && s_becken_fn && g_room_rdt_ok && g_room_rdt.cuts && g_room_rdt.cut_count > 0) {
        int32_t grenze = gr2_massstab_grenze();
        /* Nachbesserung 6 - ORDNUNG DER BUEHNEN (PORT-WAHL, Form = RE2-Finisher-Schnitt): erst der Kopf-Massstab im Koppelbild (Clip 4
         * Bild 13) moeglichst nahe am RE2-Schnitt Cut 7 (gr2_massstab_soll(FINISHER) = H 231 / Kopf-Tiefe 11175 x 65536 = 1354, Dossier
         * §9.2; RE2 P0 stellt Gator und Opfer an einen festen Ort vor einer festen Kamera @0x80101048-4c / @0x80100ff4), dann Versatz +
         * 4 x Drehung (bisherige Ordnung, NB5). Seit M2 faellt jedes Umsetzen in einen Schnitt; die NB5-Ordnung "naechste Lage" nahm
         * deshalb ohne Nutzen ferne Aufnahmen (E1 NB6-Zwischenstand: Cut 7 Massstab 629-681, Leon im Normalbild kaum zu sehen; Abnahme 5
         * H3: 573-1318). Suche: Raster GR2_FIN_RING ueber das ganze Becken (Aussenwaende = Becken-Diagonale GR2_FIN_WEIT) x alle Zonen-
         * kameras ausser der laufenden; der Massstab des Koppelbilds ist billig (eine Projektion) und begrenzt die teuren Pruefungen
         * (Wasser/Bild/Verdeckung je Bild), die nur fuer Lagen laufen, die die bisher beste schlagen. */
        /* Grob -> fein (Laufzeit im Biss-Bild): erst Raster 4 x GR2_FIN_RING ueber das ganze Becken, dann GR2_FIN_RING im Fenster +-4 x
         * GR2_FIN_RING um die beste grobe Lage (alle Kameras, die grobe Lage bleibt Schranke); ohne grobe Lage das volle feine Raster.
         * Gemessen (unit_r41_re2gator_buehne, 2240 Suchen): volles feines Raster 129 s = 58 ms je Suche. */
        static gr2_fin_suche_t S;
        memset(&S, 0, sizeof S);
        S.t = t; S.e = e; S.grenze = grenze; S.yaw0 = yaw0; S.m_soll = gr2_massstab_soll(RE15_GR2_BLICK_FINISHER);
        S.best1 = -1; S.bc = -1; S.k_verdeckt = -1;
        for (int c = 0; c < g_room_rdt.cut_count && c < RE15_GR2_KAM_MAX; c++)
            S.kok[c] = gr2_zonenkamera(c) && !gr2_fin_laufend(c) && re15_camera_build_view(&g_room_rdt.cuts[c], &S.kv[c]) == 0;
        gr2_fin_fenster(&S, GR2_BECKEN_X0, GR2_BECKEN_X1, GR2_BECKEN_Z0, GR2_BECKEN_Z1, 4 * GR2_FIN_RING);
        if (S.best1 >= 0)
            gr2_fin_fenster(&S, S.bx - 4 * GR2_FIN_RING, S.bx + 4 * GR2_FIN_RING, S.bz - 4 * GR2_FIN_RING, S.bz + 4 * GR2_FIN_RING, GR2_FIN_RING);
        else
            gr2_fin_fenster(&S, GR2_BECKEN_X0, GR2_BECKEN_X1, GR2_BECKEN_Z0, GR2_BECKEN_Z1, GR2_FIN_RING);
        int64_t best1 = S.best1; int32_t bx = S.bx, bz = S.bz, bm = S.bm, bm13 = S.bm13; int by = S.by, bc = S.bc;
        int bkb[2] = { S.bkb[0], S.bkb[1] }, blb[2] = { S.blb[0], S.blb[1] }, n_geprueft = S.n_geprueft;
        int32_t m_soll = S.m_soll;
        n_verdeckt = S.n_verdeckt; k_verdeckt = S.k_verdeckt;
        if (best1 >= 0) {
            re15_gr2_log("FINISHER Buehne (Nachbesserung 5, RE2-P0 @0x80101048-4c): von (%d,%d) Gier %d -> (%d,%d) Gier %d Cut %d, "
                         "Versatz %d Drehung %d | Koppelbild Kopf (%d,%d) Leon (%d,%d) Massstab max %d (Grenze %d) F%u | NB6: laufende "
                         "Kamera %d/%d ausgeschlossen, Raum-Objekte %d, Leon-Stichpunkte %d, wegen Verdeckung verworfen %d (zuletzt Bild %d), "
                         "Massstab Koppelbild %d (RE2-Finisher %d), Lagen geprueft %d",
                         e->x, e->z, yaw0, bx, bz, by, bc, gr2_abstand(bx, bz, e->x, e->z), gr2_wdiff(by, yaw0),
                         bkb[0], bkb[1], blb[0], blb[1], (int)bm, (int)grenze, (unsigned)g_engine.frame_count,
                         (int)g_scd.work_vars[0x0A], (int)g_scd.cam_id, n_obj, t->nlk, n_verdeckt, k_verdeckt, (int)bm13, (int)m_soll,
                         n_geprueft);
            g->fin_x = bx; g->fin_z = bz; g->fin_gier = (int16_t)by;
            g->fin_cut = (int8_t)bc;
            gefunden = 1;
        }
    }
    if (!gefunden) {
        re15_gr2_log("FINISHER Buehne: keine Lage im Wasser mit frontaler Zonenkamera bis %d - am Ort (%d,%d) Gier %d F%u (laufende "
                     "Kamera %d/%d, wegen Verdeckung verworfen %d)", GR2_FIN_WEIT, e->x, e->z, yaw0, (unsigned)g_engine.frame_count,
                     (int)g_scd.work_vars[0x0A], (int)g_scd.cam_id, n_verdeckt);
        g->fin_x = e->x; g->fin_z = e->z; g->fin_gier = e->rot_y;
    }
    g->fin_geplant = 1; g->fin_aktiv = 0;
    return gefunden;
}
/* P0 (Zeitpunkt RE2 @0x80101048-4c): den Gator auf die geplante Buehne setzen; ab hier gilt die Messzeile Maul-Sitz. */
int re15_gr2_finisher_setzen(int slot, re15_actor_t *e)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !e || !s_g[slot].fin_geplant) return 0;
    gr2_t *g = &s_g[slot];
    int bewegt = (e->x != g->fin_x || e->z != g->fin_z || e->rot_y != g->fin_gier);
    e->x = g->fin_x; e->z = g->fin_z; e->rot_y = g->fin_gier;
    g->fin_y = e->y; g->fin_aktiv = 1;
    return bewegt;
}
int re15_gr2_finisher_cut(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_g[slot].fin_cut : -1; }
/* RE2 FRESSEN P0: Zusatzwinkel +0x22A/+0x228 = 0 (`sh zero,554(s0)` @0x80101050 / `sh zero,552(s0)` @0x80101054), Kanister-Objekt
 * aus (`lui at,0x800d / sw zero,1308(at)` @0x80101008-0c = obj[1] word0) - der Finisher zeigt nur Gator und Opfer. */
void re15_gr2_fressen_p0(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    s_g[slot].kopf_az = 0; s_g[slot].kiefer_az = 0;
    if (s_kan.aktiv) {
        re15_gr2_log("FINISHER P0: Kanister-Objekt aus (Stufe %d, @0x80101008-0c) F%u", (int)s_kan.stufe, (unsigned)g_engine.frame_count);
        s_kan.aktiv = 0;
    }
}
/* Messhaken Maul-Sitz (PC-Zeichner nach dem NPC-Pass, Riegel): Kopf (Bone 6) der GEZEICHNETEN Pose (Ueberblend-Schnappschuss wie
 * re15_gr2_kanister_sitz, mit allen Zusaetzen pitch/arc/jaw/+0x22A) gegen den Kopf, den die authored Finisher-Regeln verlangen:
 * Gator an der Anker-Lage dieses Bildes (P1: P0-Lage, P2/P3: re15_clip_root_motion_abs = FUN_80015cb8-Zwilling), Keyframe-Pose
 * ohne Zusaetze. *abw = |Kopf gezeichnet - Kopf authored|; kopf[]/leon[] = gezeichnete Weltlagen (Kopf, Leons Wurzel = Lage +
 * Gier x render_scale x POS des Opfer-Bildes) fuer die Bild-/Wasser-Pruefung; *phase = 1/2/3. 0 = kein Finisher-Bild.
 * *kiefer_abw (Abnahme 4 M1: "Abstand Leon <-> gezeichneter Kiefer" je Bild) = |(Leons gezeichnete Wurzel - Part 7 der gezeichneten
 * Pose) - (dieselbe Groesse der authored Finisher-Bahn)|; die authored Bahn kommt aus gr2_fin_tabelle (Bankdaten: Bild k relativ
 * zur Buehne, gedreht um die Buehnen-Gier) - eine von der Platzierung im Boss-Modul (gb FRESSEN / gb_opfer_skalieren) getrennte
 * Rechnung. RE2: P1 Leon fest vor dem Gator (@0x8010106c-84), P2/P3 beide vom gemeinsamen Anker (FUN_80015cb8 @0x80101174 /
 * @0x801011cc) - die Relation Leon/Kiefer ist je Bild fest. Bild k: P1 Clip-4-Bild 1..14 -> k 0..13, P2 sf 1..120 -> 14..133,
 * P3 Clip-11-Bild 0..58 -> 134..192. -1 = nicht bestimmbar. */
int re15_gr2_fress_sitz(re15_actor_t *e, const re15_actor_t *pl, int32_t *abw, int32_t kopf[3], int32_t leon[3],
                        int32_t *kiefer_abw, int *phase)
{
    if (kiefer_abw) *kiefer_abw = -1;
    if (!e || !pl || !e->anim_prev_valid) return 0;
    int slot = (int)(e - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !s_g[slot].fin_aktiv) return 0;
    int ph = e->motion == 4 ? 1 : e->motion == 5 ? 2 : e->motion == 11 ? 3 : 0;
    if (!ph) return 0;
    re15_enemy_bank_t *bk = gr2_bank(e);
    if (!bk || bk->skel.bone_count <= 7 || !bk->victim_ok) return 0;
    /* gezeichnet: Schnappschuss mit Vorbild-Gewicht 0x1000 (wie re15_gr2_kanister_sitz), Zusaetze unveraendert */
    uint8_t frac = e->anim_frac; uint16_t rate = e->anim_blend_rate; uint8_t gueltig = e->anim_prev_valid;
    int32_t root[3]; int16_t ang[32][3];
    memcpy(root, e->prev_root, sizeof root); memcpy(ang, e->prev_angles, sizeof ang);
    e->anim_frac = 8; e->anim_blend_rate = 0x200;
    int ov = (e->anim_flags & 0x04u) ? (int)e->motion : -1;   /* Bildwahl wie der NPC-Pass (gr2_pose_gezeichnet) */
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
    re15_skel_pose_t gp[RE15_EMD_MAX_BONES], ap[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&bk->skel, kf, gp);
    g_anim_pose_actor = save;
    e->anim_frac = frac; e->anim_blend_rate = rate; e->anim_prev_valid = gueltig;
    memcpy(e->prev_root, root, sizeof root); memcpy(e->prev_angles, ang, sizeof ang);
    if (rv != 0) return 0;
    /* authored: dieselbe Bildwahl ohne Zusaetze, Gator an der Anker-Lage */
    int kf2 = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
    save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    rv = re15_skel_compute_pose(&bk->skel, kf2, ap);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_actor_t soll = *e;
    if (ph == 1) { soll.x = s_g[slot].fin_x; soll.z = s_g[slot].fin_z; }
    else re15_clip_root_motion_abs_pub(&soll, &bk->skel, &bk->anim, (int)e->motion, (int)e->anim_frame);
    int32_t r6[9], p6[3], r6a[9], p6a[3];
    gr2_part_welt(e, &gp[6], r6, p6);
    gr2_part_welt(&soll, &ap[6], r6a, p6a);
    int64_t q = 0;
    for (int i = 0; i < 3; i++) { int64_t d = (int64_t)p6[i] - p6a[i]; q += d * d; }
    if (abw) *abw = (int32_t)re15_squareroot0((uint32_t)(q > 0xffffffffLL ? 0xffffffffLL : q));
    if (kopf) memcpy(kopf, p6, sizeof p6);
    int32_t lw[3];
    {
        int vkf = gr2_kf(&bk->anim_victim, (int)pl->motion, (int)pl->anim_frame);
        int16_t px = 0, py = 0, pz = 0;
        if (vkf >= 0) re15_emd_get_keyframe_position(&bk->skel_victim, vkf, &px, &py, &pz);
        int32_t sp = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
        int32_t P[3] = { q12mul(px, sp), q12mul(py, sp), q12mul(pz, sp) };
        re15_skel_bone_to_world(P, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, lw);
    }
    if (leon) memcpy(leon, lw, sizeof lw);
    if (kiefer_abw) {
        const gr2_fin_t *t = gr2_fin_tabelle(e);
        int k = ph == 1 ? (int)e->anim_frame - 1 : ph == 2 ? GR2_FIN_P2 - 1 + (int)e->anim_frame : GR2_FIN_P3 + (int)e->anim_frame;
        if (t && k >= 0 && k < t->n) {
            int32_t r7[9], p7[3], ax = 0, az = 0;
            gr2_part_welt(e, &gp[7], r7, p7);
            gr2_dreh(s_g[slot].fin_gier, t->l[k][0] - t->b[k][3][0], t->l[k][2] - t->b[k][3][2], &ax, &az);
            /* y: Leon steht seit Nachbesserung 6 auch in P1 auf der Gator-Hoehe (gb FRESSEN P0, RE2 P0 schreibt nur x/z
             * @0x8010106c-84); ab P2 stehen beide auf der Anker-y (SPEED-y 0 in allen Bildern, gb FRESSEN P1-Kommentar) */
            int32_t ay = t->l[k][1] - t->b[k][3][1];         /* NB6: Leon auch in P1 auf der Gator-Hoehe (gb FRESSEN P0) */
            int64_t d0 = (int64_t)(lw[0] - p7[0]) - ax, d1 = (int64_t)(lw[1] - p7[1]) - ay, d2 = (int64_t)(lw[2] - p7[2]) - az;
            int64_t q2 = d0 * d0 + d1 * d1 + d2 * d2;
            *kiefer_abw = (int32_t)re15_squareroot0((uint32_t)(q2 > 0xffffffffLL ? 0xffffffffLL : q2));
        }
    }
    if (phase) *phase = ph;
    return 1;
}
/* Wasser-Pruefung fuer die Messzeile: 0 = Punkt im Wasser (Beckenquelle des Boss-Moduls), sonst Abstand nach draussen. */
int32_t re15_gr2_becken_draussen(int32_t x, int32_t z, int32_t m) { return s_becken_fn ? s_becken_fn(x, z, m) : 0; }
/* Projektion wie der Zeichner in die Kamera c (Messzeile/Riegel). */
int re15_gr2_im_bild_cut(int c, const int32_t p[3], int *sx, int *sy)
{
    if (!g_room_rdt_ok || !g_room_rdt.cuts || c < 0 || c >= g_room_rdt.cut_count) return 0;
    return gr2_im_bild(&g_room_rdt.cuts[c], p, sx, sy);
}

int re15_gr2_part6_gezeichnet(int32_t rot[9], int32_t pos[3])
{
    if (!s_fx_ok) return 0;
    memcpy(rot, s_p6_rot, sizeof s_p6_rot); memcpy(pos, s_p6_pos, sizeof s_p6_pos);
    return 1;
}
int re15_gr2_fx_quelle(int i, uint8_t m[32])
{
    if (i < 0 || i >= GR2_FXM_N || !s_fx_ok) return 0;
    memcpy(m, s_fx_mtx[i], 32);
    return 1;
}

int re15_gr2_geschwaecht(void) { for (int i = 0; i < RE15_ACTOR_MAX; i++) if (s_g[i].geschwaecht) return 1; return 0; }
int re15_gr2_treffer(void)     { int n = 0; for (int i = 0; i < RE15_ACTOR_MAX; i++) n += s_g[i].treffer; return n; }
int re15_gr2_tot(void)         { for (int i = 0; i < RE15_ACTOR_MAX; i++) if (s_g[i].tot) return 1; return 0; }
int re15_gr2_traegt(void)      { return s_kan.aktiv && s_kan.stufe == 7; }
