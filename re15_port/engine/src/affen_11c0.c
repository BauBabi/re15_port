/*
 * RE1.5 Rebuilt — Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (Typ 0x27) + Ada-Szene.
 * Belege und Messungen: include/re15_affen.h (Kopf) und analysis/befunde_runde35/J_affen.md.
 */
#include "re15_affen.h"
#include "re15_collision.h"     /* re15_collision_floor_typeword = FUN_8003b7f0 */
#include "re15_enemy.h"         /* re15_enemy_find (Bank: re2_rig, Skelett/Clips des Gorillas) */
#include "re15_enemy_ai.h"      /* re15_clip_anchor_set_pub */
#include "re15_math.h"          /* re15_squareroot0 = BIOS SquareRoot0 0x80065f60 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_victim_anchor_calibrate(int32_t stand_x, int32_t stand_z);   /* enemy_ai_common.c */

/* (1) NPC-Wandklemme: FUN_8003b0a4 vergleicht `entity+0x82` mit `floor >> 4` der Zelle
 *     (`lbu v1,130(a3)` @0x8003b228-3c; Zonen-Zwilling `sra v0,v0,28` @0x8003ba04). Das Byte wird
 *     byte-true aus Sce_em_set pc[4] geseedet (@0x800421c8-d0) und per Member_set 0x12 (FUN_8004116c
 *     Fall 0x12 -> +0x82) vom Skript gesetzt — ROOM11C0 sub07 @0x1C66 `Member_set 12 = 2`. */
int re15_affen_npc_band(const re15_actor_t *e)
{
    return (int)e->floor;                               /* +0x82 */
}

/* (2) Parts ohne Knochen: FUN_8001e5b0 (EXE) — Schleife ueber +0x83 = Mesh-Zahl Parts; je Part
 *     rel = EMR[8 + 6*i] (`local_38`/`puVar14` laufen 6 Byte je Part weiter, OHNE Obergrenze
 *     Knochenzahl); ab Part >= EMR+4 (`uVar21 == 0`) Elternmatrix = &DAT_80072d4c (Identitaet,
 *     t = 0, re15_disasm read 0x80072d4c) und Eltern-Record = 0. FUN_8001f3bc posiert nur
 *     EMR+4 Knochen (`uVar7 = *(byte *)(param_1 + 4)`), die lokale Rotation bleibt die
 *     Binder-Identitaet. FUN_8001e9ec: m1 = Eltern * Lokal, Vertex = View * m1 -> weltfest. */
int re15_affen_surplus_part_world(const re15_emd_skeleton_t *sk, int part,
                                  int32_t rot[9], int32_t trans[3])
{
    if (!sk || part < sk->bone_count) return 0;
    if (!sk->emr_raw || (size_t)(8 + 6 * part + 6) > sk->emr_raw_size) return 0;
    const uint8_t *p = sk->emr_raw + 8 + 6 * part;      /* EMR[8+6i] wie @FUN_8001e5b0 */
    memset(rot, 0, 9 * sizeof(int32_t));
    rot[0] = rot[4] = rot[8] = 0x1000;                  /* DAT_80072d4c = Identitaet (Q12) */
    for (int k = 0; k < 3; k++)
        trans[k] = (int32_t)(int16_t)((uint16_t)p[2*k] | ((uint16_t)p[2*k + 1] << 8));
    return 1;
}

/* (2c) Zeichner-Haken (main.c): Regel (2b) ausser fuer G5-Kinder, RE2-Banken und den Gorilla-Part 18. */
int re15_affen_teil_weltfest(uint8_t type, const re15_emd_skeleton_t *sk, int part,
                             int32_t rot[9], int32_t trans[3])
{
    if (type == 0x36u || type == 0x37u) return 0;                     /* G5-Kinder: eigene Regel in main.c */
    if (type == 0x27u && part == RE15_AFFEN_BRUST_PART) return 0;     /* (2a) am Rumpf (@0x80117200-3c) */
    const re15_enemy_bank_t *b = re15_enemy_find(type);
    if (b && b->re2_rig) return 0;                                    /* RE2-Banken: eigene Regel */
    return re15_affen_surplus_part_world(sk, part, rot, trans);
}

/* (4d) Fuss-Sperre: Pose als ABFRAGE (ohne Pose-Aktor/Tween), Zustand danach zurueck. */
int re15_affen_pose_abfrage(const re15_emd_skeleton_t *sk, int kf, re15_skel_pose_t *pose)
{
    void *pa = g_anim_pose_actor; re15_kf_tween_t tw = g_anim_kf_tween;
    g_anim_pose_actor = NULL; g_anim_kf_tween.active = 0;
    int rc = re15_skel_compute_pose(sk, kf, pose);
    g_anim_pose_actor = pa; g_anim_kf_tween = tw;
    return rc;
}

/* MESS-SCHIENE (kein Spielverhalten): RE15_AFFEN_FUSS=1 -> affen_fuss.log, je Fuss-Sperren-Schritt. */
void re15_affen_fuss_log(int slot, int clip, int bild, int bone, int kf_n, int kf_p,
                         int32_t dx, int32_t dz, int16_t rot_y)
{
    static int an = -1; static FILE *lf = NULL;
    if (an < 0) { an = (getenv("RE15_AFFEN_FUSS") != NULL); if (an) lf = fopen("affen_fuss.log", "w"); }
    if (!lf) return;
    const re15_actor_t *pa = (const re15_actor_t *)g_anim_pose_actor;   /* unveraendert durch die Abfrage */
    fprintf(lf, "slot=%d clip=%d bild=%d bone=%d kf=%d/%d d=(%d,%d) rot=%d poseaktor=%d frac=%d tween=%d\n",
            slot, clip, bild, bone, kf_n, kf_p, (int)dx, (int)dz, (int)rot_y,
            pa ? (int)(pa - g_actors) : -1, pa ? (int)pa->anim_frac : -1, (int)g_anim_kf_tween.active);
    fflush(lf);
}

/* (5) Pin-Latch: FUN_8001ac38(a0 = Spieler) @0x8011ac18 — Anker des Greifers, Kopie an den Spieler. */
void re15_affen_pin_anker(re15_actor_t *e, re15_actor_t *pl)
{
    re15_enemy_bank_t *gb = re15_enemy_find(0x27);
    if (gb && gb->ok) re15_clip_anchor_set_pub(e, &gb->skel, &gb->anim, (int)e->motion, (int)e->anim_frame);
    else { e->anchor_x = e->x; e->anchor_z = e->z; }
    pl->anchor_x = e->anchor_x; pl->anchor_z = e->anchor_z;       /* @0x8001ad30 / @0x8001ad48 */
    re15_victim_anchor_calibrate(pl->x, pl->z);                   /* Port-Wandklemme: Bezug = Standpunkt */
}

/* (8) Ritt-Platzierung des Greifers (re15_affen.h (8)): Phase 2 faellt in Phase 3; beide platzieren den Gorilla
 *     mit dem +0x95 VOR anim_set. */
void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* = FUN_8001a8f8-Kern */
void re15_affen_ritt_platz(re15_actor_t *e, const re15_actor_t *pl, int latch)
{
    if (latch) re15_enemy_steer_point(e, pl->x, pl->z, 0x800);    /* a8f8(&Spieler+0x34, 0x800) @0x8011ac60/@0x8011acac-b0 */
    re15_enemy_bank_t *gb = re15_enemy_find(0x27);
    if (gb && gb->ok)                                             /* ad68(g_entity, +0x84, +0x16c) @0x8011acc0-cc */
        re15_clip_root_motion_abs_pub(e, &gb->skel, &gb->anim, (int)e->motion, (int)e->anim_frame);
}

/* (11)/(12) POOL DES GORILLAS + ZEICHENSTAND (re15_affen.h (11)/(12)/(13)).
 *   jetzt   = Pool nach dem letzten anim_set: Winkel (Record+0x60) und Wurzel (Record0+0x2c..+0x34) wie FUN_8001f3bc
 *             (Ueberblendung mit +0x8f VOR dem Abbau, Rate 0x200 = a3 der Gorilla-Aufrufe, z.B. @0x80118320);
 *   zeichen = der Pool, den der Zeichner am Ende des Vorticks in Record+0x40 komponierte (@0x8001d108), mit Lage und
 *             +0x68 von damals (RotMatrix(+0x68 -> +0x20) nur in FUN_8001e8c8).
 * (13) Nachbesserung 5: die Ketten werden wie im Original auf der GTE gerechnet (affen_kette), Entity-Matrix zuerst. */
typedef struct {
    int16_t ang[RE15_EMD_MAX_BONES][3];               /* Record+0x60 (RotMatrix-Eingang, FUN_8001f3bc Z. 66-87) */
    int32_t root[3];                                  /* Record0+0x2c..+0x34 (Wurzel-t, Z. 28-61) */
    int32_t x, y, z;                                  /* +0x34..+0x3c = t-Spalte von +0x20 */
    int16_t rx, ry, rz;                               /* +0x68..+0x6c (RotMatrix in FUN_8001e8c8) */
    uint8_t ok;
} affen_stand_t;
typedef struct { affen_stand_t jetzt, zeichen; } re15_affen_pool_t;
static re15_affen_pool_t s_pool[RE15_ACTOR_MAX];

static re15_affen_pool_t *affen_pool(const re15_actor_t *e)
{
    int s = (int)(e - g_actors);
    return (s >= 0 && s < RE15_ACTOR_MAX) ? &s_pool[s] : NULL;
}

void re15_skel_euler_matrix_for_test(int ax, int ay, int az, int32_t m[9]);   /* skeleton_common.c: RotMatrix @0x80068130 byte-true */

static int32_t affen_ir(int64_t mac)                  /* IR1..3 bei lm = 0: Saettigung auf 16 Bit */
{
    return (int32_t)(mac > 0x7fff ? 0x7fff : (mac < -0x8000 ? -0x8000 : mac));
}

/* FUN_80022da0 (PSX.EXE, selbst disassembliert): R' = M.R * L.R spaltenweise ueber `lhu`/`mtc2` IR1..3 (16 Bit mit
 * Vorzeichen) + MVMVA 0x4a49e012 (sf=1, RT*IR, kein Zusatz, lm=0) @0x80022df0/@0x80022e38/@0x80022e84, `sh` der IR;
 * t' = (TR<<12 + M.R*V0) >> 12 ueber `lw`/`ctc2` TR (32 Bit) @0x80022eb0-c4, V0 = 16-Bit-Haelften von L.t (`lhu` /
 * `lwc2` @0x80022ecc-e0), MVMVA 0x4a480012 (sf=1, RT*V0+TR) @0x80022eec, `swc2` MAC1..3 (32 Bit) @0x80022ef8-f00. */
static void affen_comp(int32_t mr[9], int32_t mt[3], const int32_t lr[9], const int32_t lt[3])
{
    int32_t r[9], t[3];
    for (int col = 0; col < 3; col++)
        for (int row = 0; row < 3; row++)
            r[row * 3 + col] = affen_ir(((int64_t)(int16_t)mr[row * 3 + 0] * (int16_t)lr[0 * 3 + col] +
                                         (int64_t)(int16_t)mr[row * 3 + 1] * (int16_t)lr[1 * 3 + col] +
                                         (int64_t)(int16_t)mr[row * 3 + 2] * (int16_t)lr[2 * 3 + col]) >> 12);
    for (int row = 0; row < 3; row++)
        t[row] = (int32_t)((((int64_t)mt[row] << 12) + (int64_t)(int16_t)mr[row * 3 + 0] * (int16_t)lt[0] +
                            (int64_t)(int16_t)mr[row * 3 + 1] * (int16_t)lt[1] +
                            (int64_t)(int16_t)mr[row * 3 + 2] * (int16_t)lt[2]) >> 12);
    memcpy(mr, r, sizeof r); memcpy(mt, t, sizeof t);
}

/* Welt-t des Knochens `bone` wie Zeichner/Fuss-Sperre: m = +0x20 = RotMatrix(+0x68) * ScaleMatrix(+0x166) (FUN_8001e8c8;
 * ScaleMatrix je Element `(short)m * s >> 12`, ScaleMatrix-Decompilat), t = Lage; dann CompMatrix mit Record+0x18 von der
 * Wurzel abwaerts (FUN_8001ef54/FUN_8001e9ec `FUN_80022da0(rec[0x1b], rec+0x18, rec+0x40)`; FUN_8011bf50 `jal 0x80022da0`
 * @0x8011bf80/a4/b4/c4). Record+0x18 = RotMatrix(Record+0x60) (FUN_8001f3bc), t = Wurzel bzw. EMR-Versatz.
 * a1 != NULL: zusaetzlich FUN_8001bff8 = CompMatrix(Record+0x40, (I | a1)) (@0x8001c058-78). */
static int affen_kette(const re15_emd_skeleton_t *sk, const affen_stand_t *S, int bone, int16_t scale,
                       const int32_t *a1, int32_t out[3])
{
    int ch[RE15_EMD_MAX_BONES], n = 0;
    for (int b = bone; n < RE15_EMD_MAX_BONES; ) {
        ch[n++] = b;
        int p = (int)sk->bone_parent[b];
        if (p < 0 || p >= b) break;
        b = p;
    }
    int32_t mr[9], mt[3] = { S->x, S->y, S->z };
    re15_skel_euler_matrix_for_test(S->rx, S->ry, S->rz, mr);
    if (scale)
        for (int k = 0; k < 9; k++) mr[k] = (int16_t)(((int32_t)(int16_t)mr[k] * (int32_t)scale) >> 12);
    for (int i = n - 1; i >= 0; i--) {
        int b = ch[i];
        int32_t lr[9], lt[3];
        re15_skel_euler_matrix_for_test(S->ang[b][0], S->ang[b][1], S->ang[b][2], lr);
        if (i == n - 1) { lt[0] = S->root[0]; lt[1] = S->root[1]; lt[2] = S->root[2]; }
        else for (int k = 0; k < 3; k++) lt[k] = (int32_t)sk->bone_relative_pos[b][k];
        affen_comp(mr, mt, lr, lt);
    }
    if (a1) {
        static const int32_t id[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 };   /* 0x80072d4c */
        affen_comp(mr, mt, id, a1);
    }
    out[0] = mt[0]; out[1] = mt[1]; out[2] = mt[2];
    return n;
}

/* FUN_80020510 + LoadAverageShort12: prev auf 12 Bit maskiert, kf auf +-0x800 um prev gefaltet, dann `gpf12_b`
 * (IR0 = 0x200*frac, prev) und `gpl12_b` (IR0 = 0x1000 - 0x200*frac, kf) = zwei getrennte >> 12. */
static int16_t affen_winkel(int prev, int kf, int wp)
{
    prev &= 0xfff;
    unsigned d = (unsigned)(uint16_t)((kf - prev) + 0x800);
    if (d > 0x1000u) kf += (d & 0x8000u) ? 0x1000 : -0x1000;
    return (int16_t)affen_ir((int64_t)((prev * wp) >> 12) + ((kf * (0x1000 - wp)) >> 12));
}

void re15_affen_pool_anim(re15_actor_t *e)
{
    re15_affen_pool_t *P = affen_pool(e);
    re15_enemy_bank_t *b = re15_enemy_find(0x27);
    if (!P) return;
    affen_stand_t *J = &P->jetzt;
    if (!b || !b->ok || (int)e->motion >= b->anim.clip_count) { J->ok = 0; return; }
    const re15_emd_clip_t *c = &b->anim.clips[e->motion];
    if (c->frame_count <= 0) { J->ok = 0; return; }
    int kf = (int)(b->anim.frames[c->first_frame + (int)e->anim_frame % c->frame_count] & 0xFFFu);   /* +0x95 VOR dem Vorschub @0x8001f344 */
    int frac = (int)e->anim_frac;                     /* +0x8f vor dem Abbau (Decompilat Z. 23 / Z. 78) */
    int wp = frac * 0x200;                            /* FUN_8001f314 a3 = 0x200 (@0x80118320), 5. Argument @0x8001f380-88 */
    int misch = (frac != 0 && J->ok);
    int16_t px = 0, py = 0, pz = 0;
    re15_emd_get_keyframe_position(&b->skel, kf, &px, &py, &pz);
    int32_t kr[3] = { px, py, pz };
    for (int k = 0; k < 3; k++)                       /* frac 0: Wurzel := kf (Z. 28-37); sonst gpf12 + gpl12 (Z. 40-61) */
        J->root[k] = misch ? affen_ir((int64_t)((kr[k] * (0x1000 - wp)) >> 12) + (((int32_t)(int16_t)J->root[k] * wp) >> 12))
                           : kr[k];
    for (int bn = 0; bn < b->skel.bone_count && bn < RE15_EMD_MAX_BONES; bn++) {
        int16_t ax = 0, ay = 0, az = 0;
        re15_emd_get_keyframe_angles(&b->skel, kf, bn, &ax, &ay, &az);
        if (misch) {                                  /* FUN_80020510(r, kf, r, 0x1000 - 0x200*frac) (Z. 77-87) */
            J->ang[bn][0] = affen_winkel(J->ang[bn][0], ax, wp);
            J->ang[bn][1] = affen_winkel(J->ang[bn][1], ay, wp);
            J->ang[bn][2] = affen_winkel(J->ang[bn][2], az, wp);
        } else {                                      /* Record+0x60 := Record+0x78 (Z. 66-72) */
            J->ang[bn][0] = ax; J->ang[bn][1] = ay; J->ang[bn][2] = az;
        }
    }
    J->ok = 1;
}

void re15_affen_zeichen_merk(const re15_actor_t *e)
{
    re15_affen_pool_t *P = affen_pool(e);
    if (!P) return;
    P->zeichen = P->jetzt;
    P->zeichen.x = e->x; P->zeichen.y = e->y; P->zeichen.z = e->z;
    P->zeichen.rx = e->rot_x; P->zeichen.ry = e->rot_y; P->zeichen.rz = e->rot_z;
}

/* FUN_8011bf50 / FUN_8011c024: m = +0x20 * Kette(jetzt) (@0x8011bf80-c4), rec[84]/[92] = Welt-t des Zeichners;
 * +0x34 -= m.tx - rec[84] (@0x8011bfd4-e8), +0x3c -= m.tz - rec[92] (@0x8011bfec-c008). +0x20 traegt +0x68 des
 * Zeichners und die LAUFENDE Lage (+0x34/+0x3c = Matrix-t). */
int re15_affen_fusssperre(re15_actor_t *e, int bone)
{
    re15_affen_pool_t *P = affen_pool(e);
    re15_enemy_bank_t *b = re15_enemy_find(0x27);
    if (!P || !b || !b->ok) return 0;
    if (bone < 0 || bone >= b->skel.bone_count) return 1;
    if (!P->jetzt.ok || !P->zeichen.ok) return 1;
    affen_stand_t S = P->jetzt;                       /* Pool jetzt, Matrix +0x20 des Zeichners, t = laufende Lage */
    S.rx = P->zeichen.rx; S.ry = P->zeichen.ry; S.rz = P->zeichen.rz;
    S.x = e->x; S.y = e->y; S.z = e->z;
    int32_t m[3], rec[3];
    affen_kette(&b->skel, &S, bone, e->render_scale_q12, NULL, m);
    affen_kette(&b->skel, &P->zeichen, bone, e->render_scale_q12, NULL, rec);
    int32_t dx = m[0] - rec[0], dz = m[2] - rec[2];   /* m.t - rec[84/92] */
    e->x -= dx;                                       /* @0x8011bfe4-e8 */
    e->z -= dz;                                       /* @0x8011c004-08 */
    re15_affen_fuss_log((int)(e - g_actors), (int)e->motion, (int)e->anim_frame, bone, 0, 0, dx, dz, e->rot_y);
    return 1;
}

/* FUN_8001bff8(Record+0x40, a1, r, &Spieler): Welt-t von Record * (I | a1) (@0x8001c078). */
int re15_affen_trefferpunkt(const re15_actor_t *e, int bone, int32_t out[3])
{
    re15_affen_pool_t *P = affen_pool(e);
    re15_enemy_bank_t *b = re15_enemy_find(0x27);
    if (!P || !b || !b->ok || !P->zeichen.ok || bone < 0 || bone >= b->skel.bone_count) return 0;
    const int32_t a1[3] = { (bone == 9) ? 0x64 : 0, 0, 0 };   /* B[5] Knochen 9 @0x80118380-84, sonst (0,0,0) aus 0x80072d60 */
    affen_kette(&b->skel, &P->zeichen, bone, e->render_scale_q12, a1, out);
    return 1;
}

/* Riegel-Zugang (nur Tests): Elternkette eines Knochens wie affen_kette (Wurzel zuletzt). */
int re15_affen_kette_test(int bone, int out[RE15_EMD_MAX_BONES])
{
    re15_enemy_bank_t *b = re15_enemy_find(0x27);
    if (!b || !b->ok || bone < 0 || bone >= b->skel.bone_count) return 0;
    int n = 0;
    for (int k = bone; n < RE15_EMD_MAX_BONES; ) {
        out[n++] = k;
        int p = (int)b->skel.bone_parent[k];
        if (p < 0 || p >= k) break;
        k = p;
    }
    return n;
}

/* (2a) Gorilla-Part 18 (Brust-/Halsschale) haengt am Rumpf: INIT-Schwanz FUN_80116f50
 *      `lw v0,392(v0)` @0x80117200; rec18.Elternmatrix = &rec1.Matrix (`sw v1,3204(v0)`
 *      @0x80117214, v1 = v0+236), rec18.Eltern-Record = rec1 (`sw v1,3240(v0)` @0x8011721c),
 *      rel = (0x66,-810,0) (@0x80117220-30), lokale Rotation = Identitaet (@0x80117234-3c).
 *      FUN_8001e9ec: Welt = Eltern * Lokal -> R = R1, t = T1 + R1 * rel. */
int re15_affen_part_attach(uint8_t type, int part, const re15_skel_pose_t *poses,
                           int bone_count, re15_skel_pose_t *out)
{
    static const int32_t rel[3] = { RE15_AFFEN_BRUST_REL_X, RE15_AFFEN_BRUST_REL_Y, RE15_AFFEN_BRUST_REL_Z };
    if (type != 0x27u || part != RE15_AFFEN_BRUST_PART) return 0;
    if (!poses || !out || bone_count <= RE15_AFFEN_BRUST_ELTERN || part < bone_count) return 0;
    const re15_skel_pose_t *r = &poses[RE15_AFFEN_BRUST_ELTERN];
    *out = *r;
    for (int k = 0; k < 3; k++)
        out->trans[k] = r->trans[k] +
            (int32_t)(((int64_t)r->rot[k*3+0] * rel[0] + (int64_t)r->rot[k*3+1] * rel[1] +
                       (int64_t)r->rot[k*3+2] * rel[2]) >> 12);
    return 1;
}

/* (4a) FUN_8001c2dc — nur das Stopp-Flag (*param_3); die Rueckgabe (Bodenhoehe) braucht der
 *      Knockdown nicht. Schleife @0x8001c330-f8: Band s0 = -(y/1800); Zellwort w = FUN_8003b7f0
 *      (pos, r, s0 & 0xff); w == 0 -> gibt es IRGENDEINE Zelle dieses Bandes (FUN_8003bc2c
 *      @0x8001c358), ist Schluss mit Flag 0 (@0x8001c368), sonst ein Band tiefer bis 0
 *      (@0x8001c3e8-f4). w != 0: Bit 0x1 -> Flag 1 (@0x8001c37c-8c / @0x8001c3c0); Bit 0x2 ->
 *      Flag 0 (@0x8001c390-ac); Bit 0x600 -> Flag 1 (@0x8001c3b0-c0); sonst Flag 0. */
static int affen_band_hat_zelle(const re15_rdt_t *rdt, int band)
{
    for (int i = 0; i < rdt->sca_count; i++) {          /* FUN_8003bc2c: alle vier Quadranten */
        const re15_sca_entry_t *e = &rdt->sca[i];
        uint16_t w = (uint16_t)((uint16_t)e->u1 | ((uint16_t)e->floor << 8));
        if ((int)(((int32_t)((uint32_t)w << 16)) >> 28) == (band & 0xff)) return 1;
    }
    return 0;
}

int re15_affen_kd_sonde(const re15_rdt_t *rdt, int32_t x, int32_t y, int32_t z, int32_t r)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    uint32_t s0 = (uint32_t)(-(y / 0x708));             /* @0x8001c304-2c */
    for (;;) {
        int s1 = (int)(s0 & 0xffu);                     /* @0x8001c338 */
        uint16_t w = re15_collision_floor_typeword(rdt, x, z, s1, r);   /* @0x8001c340 */
        if (w != 0) {
            if (w & 0x1u) return 1;                     /* @0x8001c37c-8c -> sb s4 @0x8001c3c0 */
            if (w & 0x2u) return 0;                     /* @0x8001c390-ac sb zero */
            return (w & 0x600u) != 0;                   /* @0x8001c3b0-c0 */
        }
        if (affen_band_hat_zelle(rdt, s1)) return 0;    /* @0x8001c358-68 */
        if ((s0 & 0xffu) == 0) return 0;                /* @0x8001c3e8-f8 */
        s0--;                                           /* Delay-Slot @0x8001c3f4 */
    }
}

/* (4b) Biss: aca59 = a780(Beisser) + 2 (@0x80118488-9c); a780 @0x8001a788-a4 mit a0 = Spieler. */
uint8_t re15_affen_biss_clip(const re15_actor_t *e, const re15_actor_t *pl)
{
    int a780 = ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800;
    return (uint8_t)(a780 ? 0x09 : 0x08);               /* [3] Clip 9 @0x80035fbc-c4 / [2] Clip 8 @0x80035e38-40 */
}

/* (3) Trefferzaehler fuer den Vergeltungs-Sprung (NUTZER-VORGABE 3). */
void re15_affen_treffer_zaehlen(re15_actor_t *e)
{
    if (e->mag_hit_ctr < 255u) e->mag_hit_ctr++;        /* ein Flinch-Eintritt = ein Treffer (+0x93-Latch) */
}

uint8_t re15_affen_sprung_oder_jagd(re15_actor_t *e)
{
    if (e->mag_hit_ctr >= RE15_AFFEN_TREFFER_BIS_SPRUNG) {
        e->mag_hit_ctr = 0;
        return 7;                                       /* Vergeltungs-Sprung: Spur 0 @0x8011b194-98, Spur 1 @0x8011b3c8-cc, Spur 2 @0x8011b6c4-c8 */
    }
    return 3;                                           /* PORT-WAHL: zurueck in die Jagd (A[3]/B[3]) */
}

uint8_t re15_affen_flinch_exit_sub(re15_actor_t *e)
{
    if (e->mag_1e3 != 0) return 9;                      /* byte-true Variante @0x8011b1c8-d8 (nie gesetzt) */
    return re15_affen_sprung_oder_jagd(e);
}

/* (6e) Griff-Paar vom Koerper-Schub ausgenommen: FUN_8002aec4 prueft `and v0,a0,v1; andi 0x1000` (@0x8002af14) — beide
 *      Worte muessen Bit 0x1000 tragen. Der Gorilla setzt seins beim Pin-Latch (`ori v0,v0,0x1000` / `sw v0,0(v1)`
 *      @0x8011ac34-38, g_entity(cur)) und loescht es beim Loslassen (`addiu v1,zero,-4097` / `and` / `sw` @0x8011ad8c-94,
 *      Phase 4); Leons Bit (@0x8011ac4c) faellt erst am Ende von P2 (@0x8011c2c0-dc) — das UND ist also genau die
 *      Gorilla-Spanne Phase 3/4 von sub 15. GDB (jnb1/g_griff.txt Wort 0): e1 0x60001811 ab T254, 0x60000811 ab T302. */
int re15_affen_griff_paar(const re15_actor_t *e)
{
    return e && e->type == 0x27 && e->sub_state_1 == 15 && (e->sub_state_2 == 3 || e->sub_state_2 == 4);
}

/* (7) FUN_8001af20 = Hash des a0-Registers des AUFRUFERS (der State @0x800ac774 ist ein toter Store):
 *     `srl v1,a0,7` / `andi v1,v1,0xff` @0x8001af30-34, `addu a0,a0,v1` / `andi a0,a0,0xff` @0x8001af38-3c,
 *     `sll v1,v1,8` / `or a0,a0,v1` @0x8001af40-44 (a0 bleibt so fuer die naechste Ziehung), Rueckgabe
 *     `andi v0,a0,0xff` @0x8001af4c. */
uint8_t re15_affen_rng_a0(uint32_t *a0)
{
    uint32_t v1 = (*a0 >> 7) & 0xffu;
    uint32_t a  = (*a0 + v1) & 0xffu;
    *a0 = a | (v1 << 8);
    return (uint8_t)a;
}

/* PSX-Adresse des Entity-Records zu einem Port-Slot: Array 0x800acc2c, Stride 0x1f4 (Schleife FUN_8001a50c),
 * Port-Slot = Skript-Slot + 1 (scd_vm.c SCRIPT_SLOT_TO_ACTOR). */
uint32_t re15_affen_psx_entity(const re15_actor_t *e)
{
    return 0x800acc2cu + (uint32_t)((int)(e - g_actors) - 1) * 0x1f4u;
}

/* (7b) a0 nach FUN_8001a6d4 (atan2 von e nach p): dx = lh p.x - lh e.x, dz = lh p.z - lh e.z (@0x8001a6dc-704);
 *      dx == 0 -> Rueckkehr mit a0 = dz << 12 (`sll a0,a0,12` im Delay-Slot von `beq s0,zero` @0x8001a714-18);
 *      sonst a0 = (dz << 12) / dx (`div` / `mflo a0` @0x8001a71c/44) -> catan @0x800658fc, das in seiner 12. Iteration
 *      `lw a0,56(a1)` @0x80065928 = den CORDIC-Rest y_11 laedt und so zurueckkehrt. */
static uint32_t affen_atan2_a0(int32_t ex, int32_t ez, int32_t px, int32_t pz)
{
    int32_t dx = (int32_t)(int16_t)px - (int32_t)(int16_t)ex;
    int32_t dz = (int32_t)(int16_t)pz - (int32_t)(int16_t)ez;
    if (dx == 0) return (uint32_t)dz << 12;
    int32_t x = 0x1000, y = (dz * 4096) / dx;             /* MIPS div rundet zur Null wie C */
    for (int i = 0; i < 11; i++) {                         /* Iterationen 0..10 -> y_11 (vgl. re15_catan) */
        int32_t xs = x >> i, ys = y >> i;
        if (y >= 0) { x += ys; y -= xs; } else { x -= ys; y += xs; }
    }
    return (uint32_t)y;
}

/* (7b) a0 nach FUN_8001a804(r, tol, &ziel): dx/dz aus `lw` (32 Bit) @0x8001a82c-50, a0 = dx^2+dz^2 (`mflo`,
 *      niedrige 32 Bit) fuer SquareRoot0 @0x8001a85c-60, das a0 nicht schreibt; r < d -> Rueckkehr ohne atan2
 *      (`slt s0,s0,s1` / `bne` @0x8001a870-74), sonst atan2(e -> ziel) @0x8001a894. */
uint32_t re15_affen_a804_a0(const re15_actor_t *e, const re15_actor_t *ziel, int32_t r)
{
    uint32_t dx = (uint32_t)(ziel->x - e->x), dz = (uint32_t)(ziel->z - e->z);
    uint32_t d2 = dx * dx + dz * dz;
    if (r < (int32_t)re15_squareroot0(d2)) return d2;
    return affen_atan2_a0(e->x, e->z, ziel->x, ziel->z);
}

/* (7b) a0 beim Eintritt in B[3] (CHASE 0x80117c90) = was A[3] 0x80117a3c hinterlaesst: Spieler +0x93 != 0 ->
 *      0xbb8 (Delay-Slot @0x80117a60), sonst a0 nach a804(0xbb8, 0x180, Spieler) @0x80117a6c; der Rest von A[3]
 *      (@0x80117b40-c74) ruft nichts mehr. GDB-Modellpruefung: e1 517/517, e2 443/443 (jnb2/a0model.py). */
uint32_t re15_affen_b3_a0(const re15_actor_t *e, const re15_actor_t *pl)
{
    return pl->hit_react ? 0xbb8u : re15_affen_a804_a0(e, pl, 3000);
}

/* (7b) a0 beim Eintritt in B[0] (Leerlauf 0x80117574): der Zustand-1-Handler 0x80117254 rechnet den Abstand mit
 *      a0 = dx^2+dz^2 (Spieler - Entity, `lw`/`mult`/`mflo` @0x801172c4-fc) fuer SquareRoot0 @0x80117300 (a0 bleibt);
 *      A[0] 0x80117484 kehrt bei +0x1dc != 0 sofort zurueck (`bne` @0x80117498), sonst laedt es
 *      `lw a0,-14460(a0)` = g_entity(cur) @0x801174d0-d4 (wenn A[0] nicht nach sub 3/4 umschaltet). */
uint32_t re15_affen_b0_a0(const re15_actor_t *e, const re15_actor_t *pl)
{
    if (e->dog_blocked_ctr != 0) {
        uint32_t dx = (uint32_t)(pl->x - e->x), dz = (uint32_t)(pl->z - e->z);
        return dx * dx + dz * dz;
    }
    return re15_affen_psx_entity(e);
}

/* (15) GORILLA-FINISHER = cmd-6-Hook 0x8011c3d4 -> 0x8011c414 (Nachbesserung 6, M1). Herleitung re15_affen.h (15). */
#include "re15_audio.h"   /* re15_audio_core_se, re15_audio_footstep */
#include "re15_damage.h"  /* re15_wound_add = FUN_80037edc */
#include "re15_esp.h"     /* re15_esp_fx_spawn_ex = FUN_80019700 */
#include "re15_room.h"    /* g_room_rdt, g_room_rdt_ok */
void re15_player_victim_bone_pos_pub(int bone, int32_t out[3]);   /* enemy_ai_common.c: Part-Lage in der Opfer-Pose */
void re15_player_aim_interrupt(void);                              /* player_common.c */

static uint8_t s_fin_ph   = 3;   /* = aca5a des Hooks; 3 = Port: Hook fertig (aca58 = 7), nur noch Endpose halten */
static uint8_t s_fin_f314 = 0;   /* im Vortick lief FUN_8001f314 (+0x8f-Abbau, s. Tick) */
static int     s_fin_log[RE15_AFFEN_FIN_LOG_N];
static int     s_fin_log_n = 0;

static void affen_fin_ereignis(int was) { if (s_fin_log_n < RE15_AFFEN_FIN_LOG_N) s_fin_log[s_fin_log_n++] = was; }

static void affen_fin_blut(const re15_actor_t *pl)
{
    int32_t p[3];
    re15_player_victim_bone_pos_pub(8, p);                       /* a2 = [acbdc]+0x5a0 = Part 8 @0x8011c4a4/@0x8011c51c */
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000,     /* a0 = 0x2000 @0x8011c440/@0x8011c4f8 */
                         p[0], p[1], p[2], (int16_t)pl->rot_y);  /* a1 = Spieler+0x6a @0x8011c49c/@0x8011c514 */
}

void re15_affen_finisher_start(void)
{
    /* B[8] schreibt das ganze Wort aca58 := 6 (@0x801191c4-cc) -> aca59 = aca5a = 0: der Hook beginnt mit dem
     * Eintritt. Das Wort ersetzt das Kommando, die Verteilung @0x80031c8c liest nur aca58 -> eine laufende
     * Zielphase (cmd 1) laeuft nicht weiter (wie beim cmd-5-Latch, enemy_ai_common.c). */
    s_fin_ph = 0; s_fin_f314 = 0; s_fin_log_n = 0;
    re15_player_aim_interrupt();
}

int re15_affen_finisher_aktiv(void) { return s_fin_ph < 3; }

int re15_affen_finisher_ereignisse(int *out, int max)
{
    int n = (s_fin_log_n < max) ? s_fin_log_n : max;
    for (int i = 0; i < n; i++) out[i] = s_fin_log[i];
    return n;
}

void re15_affen_finisher_tick(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    if (s_fin_ph >= 3) return;                                   /* Leiche: Endpose (Bild fc-1) bleibt stehen */
    int fc = (vb && vb->anim_victim.clip_count > 0) ? vb->anim_victim.clips[0].frame_count : 1;
    if (fc < 1) fc = 1;
    /* +0x8f: FUN_8001f3bc mischt mit dem Wert und zieht danach 1 ab (Decompilat Z. 78). Der Port zeigt den Stand
     * nach dem Tick -> Abbau am Anfang des Ticks, der auf einen f314-Aufruf folgt (Eintritt ohne Abbau). */
    if (s_fin_f314 && pl->anim_frac > 0) pl->anim_frac--;
    s_fin_f314 = 0;
    if (s_fin_ph == 2) {                                         /* aca5a = 2 @0x8011c55c-84 */
        re15_wound_add(0, 0x0a);                                 /* jal 0x80037edc (0,0xa)  @0x8011c55c */
        re15_wound_add(5, 0x32);                                 /*               (5,0x32) @0x8011c568 */
        re15_wound_add(7, 0x32);                                 /*               (7,0x32) @0x8011c574 */
        if (pl->hp >= 0) pl->hp = -1;                            /* PORT-PLUMBING (die Death-FSM keyt auf hp < 0); B[8] zog schon 600 ab */
        pl->state = 7;                                           /* Wort aca58 := 7 @0x8011c57c-84 = Leiche */
        affen_fin_ereignis(RE15_AFFEN_FIN_TOD | (int)pl->anim_frame);
        s_fin_ph = 3;
        return;
    }
    int k;
    if (s_fin_ph == 0) {                                         /* aca5a = 0 @0x8011c460-d4 */
        s_fin_ph = 1;                                            /* sb 1 -> aca5a @0x8011c464 */
        pl->hit_react = 7;                                       /* +0x93 := 7 @0x8011c468-70 */
        pl->motion = 0;                                          /* Clip acae8 := 0 @0x8011c490 (Opfer-Bank acbcc/acbd0) */
        pl->anim_frame = 0;                                      /* Bild acae9 := 0 @0x8011c498 */
        affen_fin_blut(pl);                                      /* jal 0x80019700 @0x8011c4a0 */
        re15_audio_core_se(3);                                   /* Se_on(0x04030001) @0x8011c4a8-b8 = CORE 3 */
        affen_fin_ereignis(RE15_AFFEN_FIN_EINTRITT);
        /* aca3c |= 0xc0 @0x8011c4c0-d4: im Port ohne Gegenstueck (nur PSX-Anzeige, s. Hunde-Kommentar). Kein
         * `bne` zwischen @0x8011c4d4 und @0x8011c4d8: dasselbe Bild laeuft in den Zweig aca5a = 1. */
        k = 0;
    } else {
        k = (int)pl->anim_frame + 1;                             /* acae9 = vom f314 des Vorticks hochgezaehlt */
    }
    if (k == 0x3c) {                                             /* `ori v0,zero,0x3c` / `bne` @0x8011c4e0-e4 */
        if (g_room_rdt_ok)                                       /* FUN_80045630(2,0,0) @0x8011c4f0 */
            re15_audio_footstep(2, re15_rdt_floor_sound(&g_room_rdt, pl->x, pl->z));
        affen_fin_blut(pl);                                      /* zweites Blut @0x8011c518 */
        affen_fin_ereignis(RE15_AFFEN_FIN_FALL | k);
    }
    pl->anim_frame = (uint8_t)k;                                 /* f314(acbcc, acbd0, 0, 0x200) @0x8011c534 posiert k */
    pl->anim_flags &= (uint16_t)~0x80u;                          /* a2 = 0 = vorwaerts */
    pl->anim_blend_rate = 0x200;                                 /* a3 = 0x200 @0x8011c538 */
    s_fin_f314 = 1;
    if (k + 1 >= fc) s_fin_ph = 2;                               /* f3bc: +0x95+1 >= Bildzahl -> 1; aca5a += 1 @0x8011c548-50 */
    /* KEINE Platzierung: der Hook ruft kein 0x8001ad68 und schreibt weder +0x34/+0x3c noch +0x6a. */
}
