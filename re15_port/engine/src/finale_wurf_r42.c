/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 3 (Plan S5, K-13): Wurfszene. Nutzerwortlaut, Ablauf und alle Konstanten mit
 * Adressen: include/re15_finale_wurf_r42.h. Adressen @0x0xxxx = Datei-Offset in RE2 ROOM7020.RDT, @0x800xxxxx = RE2-Leon-EXE.
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

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;   /* scd_vm.c (= RE2 0x800cfbd8 & 0x20) */
/* finale_r42.c */
void    re15_finale_r42_go_spieler(int32_t *x, int32_t *z);
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
#define OP_END             0x01, 0x00

static struct {
    int      aktiv;
    int      form, john;
    int      w;                       /* RE15_FW_W_* */
    int32_t  pos[3], v[3], a[3];
    int      flug_n;
    int16_t  john_gier;
    /* gemerkt @0x0182A-0x0184A */
    int      cut;
    int32_t  lx, lz; int16_t lg;
    int32_t  fx, fz; int16_t fg;
    int16_t  f_motion; uint32_t f_frame; uint8_t f_frac;
    int      form_versteckt;
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

/* Teil 10 + Versatz (0,600,-50) im Bild `bild` des laufenden Clips (Paar 1 bei Sub 1, sonst Paar 2): Weltlage + Weltdrehung
 * (Super_set @0x01854: Teil-Matrix x Objekt-Drehung (-896,0,1024), Lage = Teil-Matrix * Versatz). 0 = keine Bank. */
static int hand_welt(const re15_actor_t *n, int bild_plus, int32_t pos[3], int32_t rot[9])
{
    const re15_enemy_bank_t *bk = re15_enemy_find(n->type);
    if (!bk || !bk->ok) return 0;
    const re15_emd_skeleton_t  *sk = (n->state == 4 && n->sub_state_1 == 1 && bk->loco_ok) ? &bk->skel_loco : &bk->skel;
    const re15_emd_animation_t *an = (n->state == 4 && n->sub_state_1 == 1 && bk->loco_ok) ? &bk->anim_loco : &bk->anim;
    const int c = (int)n->motion;
    if (c < 0 || c >= an->clip_count || an->clips[c].frame_count <= 0 || sk->bone_count <= RE15_FW_TEIL) return 0;
    int f = (int)n->anim_frame + bild_plus;
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
    const int32_t o[3] = {
        t->trans[0] + (int32_t)(((int64_t)t->rot[1] * RE15_FW_VERSATZ_Y + (int64_t)t->rot[2] * RE15_FW_VERSATZ_Z) >> 12),
        t->trans[1] + (int32_t)(((int64_t)t->rot[4] * RE15_FW_VERSATZ_Y + (int64_t)t->rot[5] * RE15_FW_VERSATZ_Z) >> 12),
        t->trans[2] + (int32_t)(((int64_t)t->rot[7] * RE15_FW_VERSATZ_Y + (int64_t)t->rot[8] * RE15_FW_VERSATZ_Z) >> 12) };
    re15_skel_bone_to_world(o, n->rot_y, n->x, n->y, n->z, pos);
    if (rot) {
        int32_t my[9], mo[9], tmp[9];
        mat_y(n->rot_y, my);
        re15_skel_euler_matrix(RE15_FW_HAND_RX, 0, RE15_FW_HAND_RZ, mo);
        mat_mul(my, t->rot, tmp);
        mat_mul(tmp, mo, rot);
    }
    return 1;
}

/* RE2-Drehung (rx, ry, rz) fuer Adas Gier 3700, im Port um (Gier John - 3700) um die Welt-Hochachse gedreht (Kopf). */
static void dreh_aus_re2(int rx, int ry, int rz, int32_t rot[9])
{
    int32_t e[9], my[9];
    re15_skel_euler_matrix(rx, ry, rz, e);
    mat_y((int16_t)((s_w.john_gier - RE15_FW_ADA_GIER) & 0xfff), my);
    mat_mul(my, e, rot);
}

int re15_finale_wurf_werfer_lage(int32_t pos[3], int32_t rot[9])
{
    if (!raum()) return 0;
    switch (s_w.w) {
    case RE15_FW_W_HAND: {
        const re15_actor_t *n = john();
        return n ? hand_welt(n, 0, pos, rot) : 0;
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
static void flug_a(int16_t gier, int32_t a[3])
{
    dreh_y(gier, RE15_FW_FLUG_AX, RE15_FW_FLUG_AZ, &a[0], &a[2]);
    a[1] = RE15_FW_FLUG_AY;
}
void re15_finale_wurf_bahn(const int32_t start[3], const int32_t v0[3], int16_t gier, int32_t ende[3], int *bilder)
{
    int32_t p[3], v[3], a[3];
    memcpy(p, start, sizeof p); memcpy(v, v0, sizeof v); flug_a(gier, a);
    int n = 0;
    while (n < RE15_FW_FLUG_MAX) { n++; if (!flug_schritt(p, v, a)) break; }
    if (ende) memcpy(ende, p, sizeof p);
    if (bilder) *bilder = n;
}

/* ---- Programm (RE2 sub12 in RE1.5-Opcodes) ------------------------------------------------------------------------------ */
#define PUT(...) do { const uint8_t _b[] = { __VA_ARGS__ }; \
    if (n + (int)sizeof _b <= (int)sizeof s_w.prog) { memcpy(s_w.prog + n, _b, sizeof _b); n += (int)sizeof _b; } } while (0)
static void programm_bauen(void)
{
    int n = 0;
    PUT(OP_SET(2, 7, 1), OP_SET(1, 27, 1));                          /* @0x0181A / @0x0181E */
    PUT(OP_CUT(RE15_FW_CUT_WURF));                                   /* Cut_chg 7 @0x0184E -> Ankunfts-Schnitt 14 */
    PUT(0x44, RE15_FW_JOHN_SLOT, RE15_FW_JOHN_TYP, RE15_FW_JOHN_GRID, 0x00, 0x00, 0x00, 0xff,
        LE16(RE15_FW_JOHN_X), LE16(0), LE16(RE15_FW_JOHN_Z), 0x00, 0x00, LE16(RE15_FW_JOHN_GIER), 0x00, 0x00);
                                                                     /* Sce_em_set (Form ROOM2001 @0x0179E) an die Ankunft @0x0108E */
    PUT(OP_WORK(2, RE15_FW_JOHN_SLOT));                              /* Work_set(2 = Ada) @0x01864 */
    PUT(OP_MOTION(1, RE15_FW_CLIP_WURF));                            /* Plc_motion `3f 0a 12 40` @0x01878 */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_1));                                 /* @0x0187C */
    PUT(OP_MSG(RE15_FW_MSG_ZURUF));                                  /* Xa_on 0 @0x01888 + Message_on 5 @0x0188C (K-11-Weg) */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_30));                                /* @0x01892 */
    PUT(OP_MOTION(1, RE15_FW_CLIP_WURF));                            /* @0x018AA */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_20));                                /* @0x018AE */
    PUT(OP_SIG(RE15_FW_SIG_WURF));                                   /* Super_reset @0x018B6 + Evt_exec sub13 @0x018BE */
    PUT(OP_SET(5, RE15_FW_SIG_WSLEEP, 0), OP_POLL(RE15_FW_SIG_WSLEEP));   /* Wsleep/Wsleeping @0x018C2/@0x018C3 */
    PUT(OP_POLL(RE15_FW_SIG_GELANDET));                              /* Do .. Edwhile Ck(5,0x0E,0) @0x018C4-0x018CC */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_15));                                /* @0x018D0 */
    PUT(OP_SIG(RE15_FW_SIG_REAKTION));                               /* Lagen @0x018D4-0x018FA */
    PUT(OP_CUT(RE15_FW_CUT_REAKTION));                               /* Cut_chg 11 @0x018FC */
    PUT(OP_SLEEP(RE15_FW_SCHLAF_1));                                 /* @0x018FE */
    PUT(OP_SLEEP(RE15_FW_REAKTION_BILDER));                          /* Wsleep @0x01942 auf Xa 1 (Lesefenster, Kopf) */
    PUT(OP_CUT((uint8_t)s_w.cut));                                   /* Work_copy + Cut_chg @0x01944/@0x01948: gemerkte Kamera */
    s_w.item_at = n;                                                 /* Aot_reset 5 @0x0194A: Werfer aufhebbar (Item_aot_set, Kopf) */
    PUT(0x50, RE15_FW_AOT_SLOT, 0x09, 0x31, 0x00, 0x00, LE16(0), LE16(0), LE16(RE15_FW_AOT_W), LE16(RE15_FW_AOT_D),
        LE16(RE15_FW_ITEM), LE16(RE15_FW_ANZAHL), LE16(RE15_R42_BIT_WERFER_FINALE), 0xff, 0x00);
    PUT(OP_SIG(RE15_FW_SIG_ENDE));                                   /* @0x01954-0x019A6 (Takt) */
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
    if (!s_w.prog_n) { s_w.cut = (int)g_scd.work_vars[0x0A]; programm_bauen(); }   /* Riegel: gemerkte Kamera wie der Start */
    if (out_len) *out_len = s_w.prog_n;
    return s_w.prog;
}

int re15_finale_wurf_start(int form_slot)
{
    if (s_w.aktiv || s_w.w != RE15_FW_W_AUS || form_slot < 0 || form_slot >= RE15_ACTOR_MAX) return -1;
    re15_actor_t *e = &g_actors[form_slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_w.form = form_slot; s_w.john = -1;
    s_w.john_gier = RE15_FW_JOHN_GIER;
    /* Merken @0x0182A-0x0184A: Kamera (Var 4 <- Var 0x1A), Leon x/z/Richtung (Var 5/6/7), Tyrant (Var 8/9/10) */
    s_w.cut = (int)g_scd.work_vars[0x0A];
    s_w.lx = pl->x; s_w.lz = pl->z; s_w.lg = pl->rot_y;
    s_w.fx = e->x;  s_w.fz = e->z;  s_w.fg = e->rot_y;
    s_w.f_motion = e->motion; s_w.f_frame = e->anim_frame; s_w.f_frac = e->anim_frac;
    s_w.z_an = s_w.z_aus = -1;
    programm_bauen();
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, s_w.prog) == 0) {
            s_w.aktiv = 1; s_w.w = RE15_FW_W_HAND; s_w.t0 = s_w.takt;
            LOG("Szene startet (Programm Slot %d, %d Bytes), Kamera vorher %d", slot, s_w.prog_n, s_w.cut);
            LOG("Leon (%d,%d) Gier %d gemerkt", s_w.lx, s_w.lz, s_w.lg);
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

static void wsleep_takt(void)
{
    const int zeile = g_scd.message_active != 0;
    const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
    if (zeile && g_scd.message_id == RE15_FW_MSG_ZURUF && s_w.z_an < 0) { s_w.z_an = (int)s_w.takt; LOG("Zuruf an (Id %d)", RE15_FW_MSG_ZURUF, 0, 0); }
    if (!zeile && s_w.z_an >= 0 && s_w.z_aus < 0) {
        s_w.z_aus = (int)s_w.takt;
        LOG("Zuruf aus nach %d Bildern, Stimme %d", s_w.z_aus - s_w.z_an, stimme, 0);
    }
    /* Notbremse wie kanister2080_r41.c (RE15_VOICE_NOTBREMSE 300) */
    const int notbremse = stimme && s_w.z_aus >= 0 && (int)s_w.takt - s_w.z_aus >= 300;
    if (s_w.z_aus >= 0 && (!stimme || notbremse) && !sig(RE15_FW_SIG_WSLEEP)) sig_setzen(RE15_FW_SIG_WSLEEP, 1);
}

static void loslassen(void)
{
    re15_actor_t *n = john();
    int32_t p0[3], p1[3];
    if (!n || !hand_welt(n, 0, p0, NULL) || !hand_welt(n, 1, p1, NULL)) {
        /* ohne Bank (sollte nicht vorkommen: John ist seit W im Bild): an Johns Lage fallen lassen */
        p0[0] = RE15_FW_JOHN_X; p0[1] = -RE15_FW_FLUG_STOP; p0[2] = RE15_FW_JOHN_Z;
        memcpy(p1, p0, sizeof p1);
        LOG("Loslassen OHNE Bank - Werfer faellt an Johns Lage", 0, 0, 0);
    }
    memcpy(s_w.pos, p0, sizeof s_w.pos);
    for (int i = 0; i < 3; i++) s_w.v[i] = p1[i] - p0[i];            /* U7: Weg der Hand Bild -> Bild + 1 */
    flug_a(s_w.john_gier, s_w.a);
    s_w.flug_n = 0;
    s_w.w = RE15_FW_W_FLUG;
    LOG("Loslassen im Clip-Bild %d (Takt W+%d)", n ? (int)n->anim_frame : -1, (int)(s_w.takt - s_w.t0), 0);
    LOG("Start (%d,%d,%d)", s_w.pos[0], s_w.pos[1], s_w.pos[2]);
    LOG("v (%d,%d,%d)", s_w.v[0], s_w.v[1], s_w.v[2]);
}

static void flug_takt(void)
{
    s_w.flug_n++;
    const int weiter = flug_schritt(s_w.pos, s_w.v, s_w.a);
    if (weiter && s_w.flug_n < RE15_FW_FLUG_MAX) return;
    /* Landung @0x019DE-0x01A02: Set(5,0x0E,1), Se_on Raumbank 0x0E, Pos_set, Dir_set */
    s_w.pos[1] = 0;
    s_w.w = RE15_FW_W_LIEGT;
    item_lage_setzen(s_w.pos[0], s_w.pos[2]);
    re15_finale_r42_se(RE15_FW_SE_LANDUNG);
    sig_setzen(RE15_FW_SIG_GELANDET, 1);
    LOG("Landung nach %d Flugbildern bei (%d,%d)", s_w.flug_n, s_w.pos[0], s_w.pos[2]);
}

static void reaktion(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t x = 0, z = 0;
    re15_finale_r42_go_spieler(&x, &z);
    pl->x = (int16_t)x; pl->z = (int16_t)z;
    pl->rot_y = (int16_t)((re15_finale_r42_peilung(x, z, RE15_FW_JOHN_X, RE15_FW_JOHN_Z) + RE15_FW_LEON_REL) & 0xfff);
    if (s_w.form >= 0) { g_actors[s_w.form].no_draw = 1; s_w.form_versteckt = 1; }
    LOG("Reaktion: Leon (%d,%d) Gier %d, Form unsichtbar", x, z, pl->rot_y);
}

static void ende(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *n = john();
    if (n) { n->active = 0; n->no_draw = 1; }                          /* Ada parken @0x01954-0x01960 */
    if (s_w.form >= 0) {
        re15_actor_t *e = &g_actors[s_w.form];
        if (s_w.form_versteckt) e->no_draw = 0;
        e->x = (int16_t)s_w.fx; e->z = (int16_t)s_w.fz; e->rot_y = s_w.fg;   /* Member_set2 aus Var 8/9/10 @0x01992-0x019A6 */
    }
    pl->x = (int16_t)s_w.lx; pl->z = (int16_t)s_w.lz; pl->rot_y = s_w.lg;     /* Var 5/6/7 @0x0197A-0x0198E */
    s_w.aktiv = 0;                                                     /* +0x10E &= 0x7FFF @0x01968-0x01976: Form-KI an */
#ifdef RE15_PLATFORM_PC
    {   /* PRUEFHAKEN RE15_FINALE_WURF_HIN=1 (kein Spielverhalten): Leon fuer die Aufnahme-Abnahme 600 oestlich neben den liegenden
         * Werfer, Blick zu ihm (Integrationslauf integration_r42_finale_wurf: Aufheben ueber den echten Item-Weg). */
        const char *eh = getenv("RE15_FINALE_WURF_HIN");
        if (eh && *eh && *eh != '0' && s_w.w == RE15_FW_W_LIEGT) {
            pl->x = (int16_t)(s_w.pos[0] + 600); pl->z = (int16_t)s_w.pos[2];
            pl->rot_y = (int16_t)re15_finale_r42_peilung(pl->x, pl->z, s_w.pos[0], s_w.pos[2]);
            g_scd.cam_id = RE15_FW_CUT_WURF; g_scd.cam_change_pending = 1;   /* dort sichtbar (Kamera der Ankunft) */
            LOG("PRUEFHAKEN: Leon neben den Werfer (%d,%d) Gier %d", pl->x, pl->z, pl->rot_y);
        }
    }
#endif
    LOG("Ende: John geparkt, Form-KI an, Leon zurueck (%d,%d)", s_w.lx, s_w.lz, 0);
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
    if (!s_w.aktiv) return;
    if (s_w.form >= 0 && s_w.form < RE15_ACTOR_MAX) form_ruht(&g_actors[s_w.form]);
    if (s_w.john < 0 && john()) LOG("John erscheint (Aktor %d) an (%d,%d)", s_w.john, g_actors[s_w.john].x, g_actors[s_w.john].z);
    wsleep_takt();
    if (sig(RE15_FW_SIG_WURF)) { sig_setzen(RE15_FW_SIG_WURF, 0); loslassen(); }
    else if (s_w.w == RE15_FW_W_FLUG) flug_takt();
    if (sig(RE15_FW_SIG_REAKTION)) { sig_setzen(RE15_FW_SIG_REAKTION, 0); reaktion(); }
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
