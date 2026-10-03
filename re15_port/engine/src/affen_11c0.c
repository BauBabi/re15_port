/*
 * RE1.5 Rebuilt — Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (Typ 0x27) + Ada-Szene.
 * Belege und Messungen: include/re15_affen.h (Kopf) und analysis/befunde_runde35/J_affen.md.
 */
#include "re15_affen.h"
#include "re15_collision.h"     /* re15_collision_floor_typeword = FUN_8003b7f0 */
#include "re15_enemy.h"         /* re15_enemy_find (Bank: re2_rig, Skelett/Clips des Gorillas) */
#include "re15_enemy_ai.h"      /* re15_clip_anchor_set_pub */
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
