/*
 * schuss_r36.c — Runde 36 Spur C: Sichtlinie des Spieler-Schusses (RE1.5 FUN_8001b9b4).
 * Belege und Einordnung: include/re15_schuss_r36.h, Dossier analysis/befunde_runde36/C_schuss.md §1.
 */
#include "re15_schuss_r36.h"
#include "re15_room.h"       /* g_room_rdt_ok */

static int s_verworfen = 0;

int  re15_schuss_r36_sichtlinie_verworfen(void) { return s_verworfen; }
void re15_schuss_r36_sichtlinie_reset(void)     { s_verworfen = 0; }

int re15_schuss_r36_sichtlinie_blockiert(const re15_actor_t *pl, const re15_actor_t *ziel)
{
    int region;
    if (!pl || !ziel) return 0;
    if (!g_room_rdt_ok) return 0;                  /* keine SCA -> leere Regionen -> 0 (@0x8003dd94) */
    /* FUN_8001b9b4: `ori s0,zero,0x3` @0x8001ba08, Schleife `addiu s0,s0,-1` / `bne` @0x8001ba34-38,
     * je Region FUN_8003dcc4(delta, region, 0xf00, 0x300) @0x8001ba1c-24; das Ergebnis wird
     * ODER-verknuepft (`or s1,s1,v0` @0x8001ba28) — der erste Treffer entscheidet. Ursprung und
     * Band (+0x82) sind die des Spielers (DAT_800ac784, @0x8003dd00/@0x8003de60). */
    for (region = 3; region >= 0; region--) {
        if (re15_los_ray_blocked(pl, ziel, region, 0u)) {
            s_verworfen++;
            return 1;
        }
    }
    return 0;
}

/* ---- Punkt 2: Blut-Anker = Anker.R * a3 + Anker.T (Belege im Header) ------------------------- */
#include "re15_damage.h"       /* re15_enemy_bone_world_pos */
#include "re15_enemy.h"        /* re15_enemy_find, re15_actor_rbj_skel/_anim, re15_re2z_re15_pose_skel/_anim */
#include "re15_enemy_ai.h"     /* re15_actor_uses_loco_bank, re15_actor_uses_own_bank */
#include "re15_skeleton.h"     /* re15_skel_compute_pose / g_anim_pose_actor / re15_sin_q12 */
#include "re15_anim_select.h"  /* re15_compute_actor_kf */
#include "re15_esp.h"          /* re15_esp_fx_get / RE15_ESP_FX_MAX (Nachfuehrung, Nachbesserung 3) */
#include "re15_engine.h"       /* g_engine.frame_count */
#include "re15_wasserzombie_r38.h" /* Runde 38 Spur U: Bank des RE2-KI-Hochziehens */

/* Nachbesserung 2: die BANK, aus der der Port-Renderer den Aktor zeichnet — Zwilling von platform/pc/main.c
 * 9798-9925 fuer einen Gegner mit eigener Bank (re15_actor_anim_select-Gegnerzweig: Hauptbank, clip_override = motion
 * nur bei +0x1C4 & 4; dann Loco-Bank / Kanaele / eigene Bank / RE1.5-Sitz-Import in DERSELBEN Reihenfolge wie dort).
 * Im Original ist das das (EMR,EDD)-Paar, das der Handler dem Anim-Durchlauf reicht (`sw a2,376(a0)` / `jal 0x80029614`
 * @0x800295F8-FC) — Paar 1 = Loco-Bank (re15_re2z_poses_loco_bank). */
static void zeichen_bank(const re15_actor_t *e, const re15_enemy_bank_t *b, const re15_emd_skeleton_t **sk,
                         const re15_emd_animation_t **an, int *ov)
{
    const int slot = (int)(e - g_actors), mo = (int)e->motion;
    *sk = &b->skel; *an = &b->anim;
    *ov = (e->anim_flags & 0x04) ? mo : -1;
    if (re15_actor_uses_loco_bank(e) && b->loco_ok && mo < b->anim_loco.clip_count) {           /* main.c:9834-9843 */
        *sk = &b->skel_loco; *an = &b->anim_loco; *ov = mo;
    }
    if (e->state == 4 && e->sub_state_1 == 0 && !e->walk_active) {                              /* main.c:9853-9861 */
        const re15_emd_skeleton_t  *rs = (slot >= 0 && slot < RE15_ACTOR_MAX) ? re15_actor_rbj_skel(slot) : NULL;
        const re15_emd_animation_t *ra = (slot >= 0 && slot < RE15_ACTOR_MAX) ? re15_actor_rbj_anim(slot) : NULL;
        if (rs && ra && mo < ra->clip_count) { *sk = rs; *an = ra; *ov = mo; }
    } else if (e->state == 4 && !e->walk_active &&
               (e->sub_state_1 == 1 || e->sub_state_1 == 7 || e->sub_state_1 == 8)) {         /* main.c:9862-9870 */
        if (b->loco_ok && mo < b->anim_loco.clip_count) { *sk = &b->skel_loco; *an = &b->anim_loco; *ov = mo; }
    } else if (e->state == 4 && !e->walk_active && e->sub_state_1 == 3) {                       /* main.c:9871-9876 */
        if (b->victim_ok && mo < b->anim_victim.clip_count) { *sk = &b->skel_victim; *an = &b->anim_victim; *ov = mo; }
    } else if (re15_actor_uses_own_bank(e)) {                                                    /* main.c:9877-9912 */
        if (b->own_ok && mo < b->anim_own.clip_count) { *sk = &b->skel_own; *an = &b->anim_own; *ov = mo; }
    } else if (e->re2z_re15_pose) {                                                              /* main.c:9913-9925 */
        const re15_emd_skeleton_t  *rs = re15_re2z_re15_pose_skel();
        const re15_emd_animation_t *ra = re15_re2z_re15_pose_anim();
        if (rs && ra && mo < ra->clip_count) { *sk = rs; *an = ra; *ov = mo; }
    }
    { const re15_emd_skeleton_t *wz_sk; const re15_emd_animation_t *wz_an;   /* Runde 38 Spur U: wie main.c (RE2-KI-Hochziehen) */
      if (re15_wasserzombie_bank(e, &wz_sk, &wz_an)) { *sk = wz_sk; *an = wz_an; *ov = mo; } }
}

int re15_schuss_r36_gezeichnet(const re15_actor_t *e, int bone, int32_t R[9], int32_t T[3])
{
    static re15_actor_t sh;                                   /* Schatten-Aktor (Muster enemy_ai_re2_zellenarm.c:462-82) */
    static re15_skel_pose_t po[RE15_EMD_MAX_BONES], lo[RE15_EMD_MAX_BONES];
    const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an;
    const re15_skel_pose_t *np;
    int32_t c, s, ny[9];
    int ov, kf, rv, r, q;
    if (!e || !R || !T) return 0;
    c = re15_cos_q12(e->rot_y); s = re15_sin_q12(e->rot_y);
    ny[0] = c; ny[1] = 0; ny[2] = s; ny[3] = 0; ny[4] = 0x1000; ny[5] = 0; ny[6] = -s; ny[7] = 0; ny[8] = c;
    for (r = 0; r < 9; r++) R[r] = ny[r];                    /* Rueckfall: Wurzel, R = Ryaw (Aktor-Rahmen) */
    T[0] = e->x; T[1] = e->y; T[2] = e->z;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || bone < 0 || bone >= RE15_EMD_MAX_BONES) return 0;
    /* (5) Teil mit EIGENER Matrix (Flag 0x40, vom Renderer schon geseedet): der Zeichner komponiert es nicht neu
     * (`andi v0,s3,0x40` / `bne` @0x80027498-9C), die Physik fuehrt dieselbe Matrix (`jal 0x80028ad8`, a1 = Record+72
     * @0x800276A0-A4) — sie IST der Anker (Aufschlag-Blut `addiu a2,s1,72` @0x80028C34). */
    if (re15_re2z_gore_part_matrix_peek(e, bone, R, T)) return 2;
    zeichen_bank(e, b, &sk, &an, &ov);
    if (!sk || !an || an->clip_count <= 0 || bone >= sk->bone_count) return 0;
    /* Crossfade wie der Renderer (g_anim_pose_actor = Aktor, main.c:9960; Original: Blende a3 * +0x14E im
     * Anim-Durchlauf @0x800296A8-BC) — auf einer KOPIE, damit der Blend-Zustand des echten Aktors unberuehrt bleibt. */
    sh = *e;
    kf = re15_compute_actor_kf(an, sk, &sh, ov, (uint32_t)e->anim_frame);
    {   void *save = g_anim_pose_actor; g_anim_pose_actor = &sh;
        rv = re15_skel_compute_pose(sk, kf, po);
        g_anim_pose_actor = save;
    }
    if (rv != 0) return 0;
    np = &po[bone];
    if (e->re2z_rag_anchor_on && bone >= 1 && bone <= 6) {   /* Unterhaelfte: eigener Clip (main.c:10195-10220) */
        const re15_emd_skeleton_t *ls = NULL;
        int lkf = re15_re2z_lower_kf(e, &ls);
        void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
        if (re15_skel_compute_pose(ls ? ls : sk, lkf, lo) == 0) np = &lo[bone];
        g_anim_pose_actor = save;
    }
    for (r = 0; r < 3; r++) {                                 /* R = Ryaw * R_part, T = Ryaw * t_part + Wurzel */
        int64_t a = 0;
        for (q = 0; q < 3; q++) {
            R[r*3+q] = (int32_t)(((int64_t)ny[r*3+0] * np->rot[0*3+q] + (int64_t)ny[r*3+1] * np->rot[1*3+q] +
                                  (int64_t)ny[r*3+2] * np->rot[2*3+q]) >> 12);
            a += (int64_t)ny[r*3+q] * np->trans[q];
        }
        T[r] = (int32_t)(a >> 12);
    }
    T[0] += e->x; T[1] += e->y; T[2] += e->z;
    if (e->re2z_rag_anchor_on) {                              /* Ragdoll-Bodenanker (main.c:10315-10330) */
        int32_t p0w[3];
        for (r = 0; r < 3; r++)
            p0w[r] = (int32_t)(((int64_t)ny[r*3+0] * po[0].trans[0] + (int64_t)ny[r*3+1] * po[0].trans[1] +
                                (int64_t)ny[r*3+2] * po[0].trans[2]) >> 12);
        p0w[0] += e->x; p0w[1] += e->y; p0w[2] += e->z;
        re15_re2z_ragdoll_part_anchor(e, bone, p0w, T);
    }
    return 1;
}

static int s_anker_eigen = 0;      /* letzter Anker = eigene Matrix eines freifliegenden Teils (Nachbesserung 3) */

int re15_schuss_r36_blut_anker(const re15_actor_t *e, int bone, const int32_t a3[3], int32_t out[3])
{
    int32_t R[9];
    int r, rv;
    if (!e || !out) return 0;
    /* Anker = die Matrix, die der Zeichner zuletzt in Record+72 geschrieben hat (FUN_80027434 @0x8002751C-DC), fuer
     * JEDEN Emitter — auch bei a3 = 0 (FUN_8001BF10 nullt dann nur den Versatz, `sw zero,44/48(t0)` @0x8001C000-04).
     * Ohne geladene Bank (nur Unit-Sonden): Anker.T = Aktor-Wurzel, R = Ryaw (a3 im Aktor-Rahmen, unit_r26_mg_blut). */
    rv = re15_schuss_r36_gezeichnet(e, bone, R, out);
    s_anker_eigen = (rv == 2);                                /* Nachfuehrung (Nachbesserung 3): eigene Matrix nicht */
    if (!a3 || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) return rv ? 1 : 0;
    /* R * a3 (Q12; RE2-Lagen-Tick 0x8001D894: RT = Record+76 @0x8001DB74, V0 = a3 @0x8001DBA0, MVMVA 0x4A486012
     * @0x8001DBB4, + Anker.T @0x8001DBD0-DC1C); R ist schon die Welt-Rotation (Ryaw * R_part bzw. die eigene
     * Matrix des Teils). */
    for (r = 0; r < 3; r++)
        out[r] += (int32_t)(((int64_t)R[r*3+0] * a3[0] + (int64_t)R[r*3+1] * a3[1] + (int64_t)R[r*3+2] * a3[2]) >> 12);
    return 2;
}

/* ---- Mess-Ring der Zombie-Blut-Spawns (nur Sonden, Header) ----------------------------------- */
static struct { int part; int32_t pos[3]; int32_t a3[3]; int mit_a3; uint32_t id; } s_fx_ring[RE15_SCHUSS_R36_FX_RING];
static unsigned s_fx_zahl = 0u;
static uint8_t  s_esp_vor[RE15_ESP_FX_MAX];   /* Nachbesserung 3: ESP-Plaetze vor dem Spawn (aktiv ja/nein) */
static int      s_esp_vor_ok = 0;

void re15_schuss_r36_fx_merke(int part, const int32_t pos[3], const int32_t *a3)
{
    unsigned i = s_fx_zahl % RE15_SCHUSS_R36_FX_RING;
    for (int k = 0; k < RE15_ESP_FX_MAX; k++) s_esp_vor[k] = (uint8_t)(re15_esp_fx_get(k) != NULL);
    s_esp_vor_ok = 1;                                  /* merke steht unmittelbar VOR dem ESP-Spawn (beide Emitter) */
    s_fx_ring[i].part = part;
    s_fx_ring[i].pos[0] = pos[0]; s_fx_ring[i].pos[1] = pos[1]; s_fx_ring[i].pos[2] = pos[2];
    s_fx_ring[i].mit_a3 = (a3 != NULL);
    s_fx_ring[i].a3[0] = a3 ? a3[0] : 0; s_fx_ring[i].a3[1] = a3 ? a3[1] : 0; s_fx_ring[i].a3[2] = a3 ? a3[2] : 0;
    s_fx_ring[i].id = 0xFFFFFFFFu;                     /* gepackte RE2-Id: nur re2z_gore_fx_core setzt sie (fx_id) */
    s_fx_zahl++;
}
unsigned re15_schuss_r36_fx_zahl(void) { return s_fx_zahl; }
int re15_schuss_r36_fx_holen(unsigned nr, int *part, int32_t pos[3], int32_t a3[3], int *mit_a3)
{
    if (nr >= s_fx_zahl || s_fx_zahl - nr > RE15_SCHUSS_R36_FX_RING) return 0;
    unsigned i = nr % RE15_SCHUSS_R36_FX_RING;
    if (part) *part = s_fx_ring[i].part;
    if (pos) { pos[0] = s_fx_ring[i].pos[0]; pos[1] = s_fx_ring[i].pos[1]; pos[2] = s_fx_ring[i].pos[2]; }
    if (a3)  { a3[0] = s_fx_ring[i].a3[0]; a3[1] = s_fx_ring[i].a3[1]; a3[2] = s_fx_ring[i].a3[2]; }
    if (mit_a3) *mit_a3 = s_fx_ring[i].mit_a3;
    return 1;
}

void re15_schuss_r36_fx_id(uint32_t packed)
{
    if (s_fx_zahl) s_fx_ring[(s_fx_zahl - 1u) % RE15_SCHUSS_R36_FX_RING].id = packed;
}
int re15_schuss_r36_fx_holen_id(unsigned nr, uint32_t *packed)
{
    if (nr >= s_fx_zahl || s_fx_zahl - nr > RE15_SCHUSS_R36_FX_RING) return 0;
    if (packed) *packed = s_fx_ring[nr % RE15_SCHUSS_R36_FX_RING].id;
    return 1;
}

/* ---- Nachbesserung 3: Nachfuehrung der Spawns eines Gegner-Ticks auf die gezeichnete Lage (Belege im Header) ---- */
#define NF_MAX RE15_ESP_FX_MAX   /* PORT-PUFFER (kein Original-Wert): so viele Eintraege wie der ESP-Pool Plaetze hat
                                  * (96, `addiu t2,zero,96` @0x8001BF10-Schleife) — die Eintraege eines Bilds werden erst am
                                  * Ende von run_all abgearbeitet, kein Spawn darf vorher verdraengt werden */
static struct {
    const re15_actor_t *e; uint32_t bild; int32_t x, z; unsigned ring; int eigen; int n;
    uint8_t slot[RE15_ESP_FX_MAX];
} s_nf[NF_MAX];
static unsigned s_nf_zahl = 0u;

void re15_schuss_r36_fx_nach(const re15_actor_t *e)
{
    if (!s_esp_vor_ok || !e || s_fx_zahl == 0u) return;
    s_esp_vor_ok = 0;
    unsigned j = s_nf_zahl++ % NF_MAX;
    s_nf[j].e = e; s_nf[j].bild = (uint32_t)g_engine.frame_count; s_nf[j].x = e->x; s_nf[j].z = e->z;
    s_nf[j].ring = s_fx_zahl - 1u; s_nf[j].eigen = s_anker_eigen; s_nf[j].n = 0;
    for (int k = 0; k < RE15_ESP_FX_MAX; k++)
        if (!s_esp_vor[k] && re15_esp_fx_get(k)) s_nf[j].slot[s_nf[j].n++] = (uint8_t)k;
}

void re15_schuss_r36_nachfuehr_marke(const re15_actor_t *e)
{
    for (int j = 0; j < NF_MAX; j++)                   /* Lage am Handler-Ende = Bezug fuer den Schwanz */
        if (s_nf[j].e == e && s_nf[j].bild == (uint32_t)g_engine.frame_count) { s_nf[j].x = e->x; s_nf[j].z = e->z; }
}

void re15_schuss_r36_nachfuehren_alle(void)
{
    for (int j = 0; j < NF_MAX; j++) {
        const re15_actor_t *e = s_nf[j].e;
        if (!e || s_nf[j].bild != (uint32_t)g_engine.frame_count) continue;
        int32_t dx = e->x - s_nf[j].x, dz = e->z - s_nf[j].z;
        if (!s_nf[j].eigen && (dx || dz)) {
            for (int k = 0; k < s_nf[j].n; k++) {
                re15_esp_fx_t *f = (re15_esp_fx_t *)re15_esp_fx_get(s_nf[j].slot[k]);  /* Pool s_esp_fx ist nicht const */
                if (f) { f->x += dx; f->z += dz; }
            }
            if (s_fx_zahl - s_nf[j].ring <= RE15_SCHUSS_R36_FX_RING) {
                unsigned i = s_nf[j].ring % RE15_SCHUSS_R36_FX_RING;
                s_fx_ring[i].pos[0] += dx; s_fx_ring[i].pos[2] += dz;
            }
        }
        s_nf[j].e = NULL;
    }
}

/* ---- Punkt 3: KRAEHE gegen Magnum — RE2 GIB 0x80102CA0 (EMOVL21_S0.BIN), Belege im Header ----- */
#include <string.h>
extern uint32_t re15_re2_rand(void);            /* 0x80015FE8, der geteilte RE2-PRNG */

void re15_schuss_r36_kraehe_parts_seed(re15_actor_t *e)
{
    int i;
    if (!e) return;
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
    e->re2c_gib_parts = 1u;                                  /* Marker: Bruecke aktiv (re15_re2z_gore_active) */
    for (i = 0; i < 16; i++) {
        e->re2z_part_blend[i] = -1;                          /* +0x7A: keine Setz-Blende (`bltz` @0x80028D2C) */
        e->re2z_part_flags[i] = (i < RE15_SCHUSS_R36_KRAEHE_PARTS) ? 1u : 0u;   /* Bit 0 = Part vorhanden */
        e->re2z_part_tint[i]  = 0x00808080u;                 /* Modell-Aufbau FUN_80028368: +0x70 = 0x808080 */
        e->re2z_part_mesh[i]  = (uint8_t)i;
    }
}

/* 0x80102EF4 — WURF (a0 = Zeile, a1 = Peilung): Drift-Physik FUN_80028DAC ueber Flagbit 0x08. */
static void kraehe_wurf(re15_actor_t *e, int i, int16_t a1)
{
    e->re2z_part_tint[i]   = 0x00101040u;                    /* lui a2,0x10 / ori a2,a2,0x1040 @0x80102EF4-F8, sw a2,112(a0) @0x80102F24 */
    e->re2z_part_w9c[i]    = 800;                            /* sh 800,156(a0)  @0x80102F04 (+0x9C Tempo) */
    e->re2z_part_w9e[i]    = 10;                             /* sh 10,158(a0)   @0x80102F0C (+0x9E Fall-Zuwachs) */
    e->re2z_part_yaw98[i]  = a1;                             /* sh a1,152(a0)   @0x80102F14 (+0x98 Gier) */
    e->re2z_part_life[i]   = 0;                              /* sh zero,160(a0) @0x80102F18 (+0xA0 Lebensdauer) */
    e->re2z_part_w9a[i]    = 0;                              /* sh zero,154(a0) @0x80102F1C (+0x9A Fall) */
    e->re2z_part_wa4[i]    = -100;                           /* sh -100,164(a0) @0x80102F20 (+0xA4 Tempo-Abbau) */
    e->re2z_part_flags[i] |= 0x4Au;                          /* ori v1,v1,0x4a / sw v1,0(a0) @0x80102F28-30 */
}

/* 0x80102F34 — WIRBEL (a0 = Zeile, a1 = Peilung): Wurf-Physik FUN_80028AD8 ueber Flagbit 0x20, drei Draws. */
static void kraehe_wirbel(re15_actor_t *e, int i, int16_t a1)
{
    e->re2z_part_flags[i] |= 0x1062u;                        /* ori v0,v0,0x1062 / sw v0,0(s0) @0x80102F50-58 */
    e->re2z_part_v[i][0]   = (int16_t)(uint16_t)((uint32_t)a1 + (re15_re2_rand() << 2));   /* jal rand @0x80102F54, sll 2, addu, sh s1,56 @0x80102F5C-64 */
    e->re2z_part_st86[i]   = 0;                              /* sh zero,134(s0) @0x80102F6C (+0x86 -> INIT in FUN_80028AD8) */
    e->re2z_part_v[i][2]   = (int16_t)((re15_re2_rand() & 0x7fu) + 100u);  /* jal rand @0x80102F68, andi 0x7f, addiu 100, sh v0,60 @0x80102F70-78 */
    e->re2z_part_v[i][1]   = 0;                              /* sh zero,58(s0)  @0x80102F80 */
    e->re2z_part_grav[i]   = 20;                             /* addiu v0,zero,20 @0x80102F7C / sb v0,121(s0) @0x80102F84 */
    e->re2z_part_rot[i][0] = 0;                              /* sh zero,62(s0)  @0x80102F8C */
    e->re2z_part_rot[i][1] = (int16_t)(uint16_t)(re15_re2_rand() << 4);   /* jal rand @0x80102F88, sll 4, sh v0,64 @0x80102F90-94 */
    e->re2z_part_rot[i][2] = 0;                              /* sh zero,66(s0)  @0x80102F98 */
}

void re15_schuss_r36_kraehe_gib(re15_actor_t *e, int16_t a1, uint32_t m)
{
    if (!e) return;
    kraehe_wurf(e, 0, a1);                                                   /* @0x80102CE8-F0: Zeile 0 IMMER Wurf */
    if (m & 0x01u) { kraehe_wurf(e, 1, a1);   kraehe_wurf(e, 2, a1); }       /* @0x80102CF4-D18 */
    else           { kraehe_wirbel(e, 1, a1); kraehe_wirbel(e, 2, a1); }     /* @0x80102D24-3C */
    if (m & 0x02u) { kraehe_wurf(e, 3, a1);   kraehe_wurf(e, 4, a1);   kraehe_wurf(e, 5, a1); }      /* @0x80102D40-74 */
    else           { kraehe_wirbel(e, 3, a1); kraehe_wirbel(e, 4, a1); kraehe_wirbel(e, 5, a1); }    /* @0x80102D80-A8 */
    if (m & 0x04u) { kraehe_wurf(e, 6, a1);   kraehe_wurf(e, 7, a1);   kraehe_wurf(e, 8, a1); }      /* @0x80102DAC-E0 */
    else           { kraehe_wirbel(e, 6, a1); kraehe_wirbel(e, 7, a1); kraehe_wirbel(e, 8, a1); }    /* @0x80102DEC-E14 */
    kraehe_wurf(e, 9, a1); kraehe_wurf(e, 10, a1);                           /* @0x80102E20-34: 9/10 IMMER Wurf */
    if (m & 0x08u) kraehe_wurf(e, 11, a1); else kraehe_wirbel(e, 11, a1);   /* @0x80102E38-60 */
    if (m & 0x10u) kraehe_wurf(e, 12, a1); else kraehe_wirbel(e, 12, a1);   /* @0x80102E64-8C */
}
