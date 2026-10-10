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
#include "re15_enemy.h"        /* re15_enemy_find */
#include "re15_skeleton.h"     /* re15_skel_compute_pose / g_anim_pose_actor / re15_sin_q12 */
#include "re15_anim_select.h"  /* re15_compute_actor_kf */

int re15_schuss_r36_blut_anker(const re15_actor_t *e, int bone, const int32_t a3[3], int32_t out[3])
{
    static re15_actor_t sh;                                   /* Schatten-Aktor (Muster enemy_ai_re2_zellenarm.c:462-82) */
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t v[3], c, s;
    int kf, rv, r;
    if (!e || !out) return 0;
    re15_enemy_bone_world_pos(e, bone, out);                 /* Anker.T (Welt) aus der ROHEN Pose, inkl. Ragdoll */
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || b->skel.bone_count <= 0 || bone < 0 || bone >= b->skel.bone_count) return 1;
    if (e->re2z_rag_anchor_on) {                             /* abgetrennte Unterhaelfte: Anker wie bisher */
        if (!a3 || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) return 1;
        void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
        rv = re15_skel_compute_pose(&b->skel, re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame), poses);
        g_anim_pose_actor = save;
        if (rv != 0) return 1;
    } else {
        /* Das Original liest die Matrix, die der ZEICHNER geschrieben hat (Record +0x48, FUN_8001e8c8 im
         * Vorbild) — also die CROSSFADE-geblendete Pose (FUN_8001f3bc), nicht das rohe Keyframe. Der Port
         * blendet nur mit gesetztem g_anim_pose_actor und schreibt dabei den Blend-Zustand zurueck; eine
         * KOPIE des Aktors liefert dieselbe Pose, die der Renderer in diesem Bild zeichnet, ohne den
         * Zustand des echten Aktors zu veraendern (gemessen probe_r36_blut: roh vs. gezeichnet bis 98
         * Einheiten im zweiten Treffer-Clip). */
        sh = *e;
        kf = re15_compute_actor_kf(&b->anim, &b->skel, &sh, -1, e->anim_frame);
        {   void *save = g_anim_pose_actor; g_anim_pose_actor = &sh;
            rv = (kf >= 0) ? re15_skel_compute_pose(&b->skel, kf, poses) : -1;
            g_anim_pose_actor = save;
        }
        if (rv != 0) return 1;
        re15_skel_bone_to_world(poses[bone].trans, e->rot_y, e->x, e->y, e->z, out);   /* Anker.T gezeichnet */
        if (!a3 || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) return 1;
    }
    /* R_part * a3 (Q12; RE2-Lagen-Tick 0x8001D894: RT = Record+76 @0x8001DB74, V0 = a3 @0x8001DBA0,
     * MVMVA 0x4A486012 @0x8001DBB4, + Anker.T @0x8001DBD0-DC1C) ... */
    for (r = 0; r < 3; r++)
        v[r] = (int32_t)(((int64_t)poses[bone].rot[r*3+0] * a3[0] + (int64_t)poses[bone].rot[r*3+1] * a3[1] +
                          (int64_t)poses[bone].rot[r*3+2] * a3[2]) >> 12);
    /* ... dann in die Welt des Aktors (Ryaw wie re15_skel_bone_to_world: Netz blickt +X bei Gier 0). */
    c = re15_cos_q12(e->rot_y); s = re15_sin_q12(e->rot_y);
    out[0] += (int32_t)(((int64_t)c * v[0] + (int64_t)s * v[2]) >> 12);
    out[1] += v[1];
    out[2] += (int32_t)(((int64_t)-s * v[0] + (int64_t)c * v[2]) >> 12);
    return 2;
}

/* ---- Mess-Ring der Zombie-Blut-Spawns (nur Sonden, Header) ----------------------------------- */
static struct { int part; int32_t pos[3]; int32_t a3[3]; int mit_a3; } s_fx_ring[RE15_SCHUSS_R36_FX_RING];
static unsigned s_fx_zahl = 0u;

void re15_schuss_r36_fx_merke(int part, const int32_t pos[3], const int32_t *a3)
{
    unsigned i = s_fx_zahl % RE15_SCHUSS_R36_FX_RING;
    s_fx_ring[i].part = part;
    s_fx_ring[i].pos[0] = pos[0]; s_fx_ring[i].pos[1] = pos[1]; s_fx_ring[i].pos[2] = pos[2];
    s_fx_ring[i].mit_a3 = (a3 != NULL);
    s_fx_ring[i].a3[0] = a3 ? a3[0] : 0; s_fx_ring[i].a3[1] = a3 ? a3[1] : 0; s_fx_ring[i].a3[2] = a3 ? a3[2] : 0;
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
