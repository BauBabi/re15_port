/*
 * RE1.5 Rebuilt — Runde 41 Spur J: SCD 0x48 Super_set / 0x49 Super_reset (Belege: include/re15_superobj_r41.h,
 * Dossier analysis/befunde_runde41/J_stage3szenen_a.md §2).
 */
#include <stdio.h>
#include <string.h>
#include "re15_superobj_r41.h"
#include "re15_fussanker_r41.h"   /* Kanal + Part-Weltmatrix (gleiche Rechnung wie das Zusammensetzen) */
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_skeleton.h"        /* re15_skel_euler_matrix = RotMatrix */
#include "re15_engine.h"
#include "re15_room.h"

typedef struct {
    int      aktiv;
    int      slot;        /* Aktor-Slot = Gegner-Index + 1 */
    int      teil;        /* Part-Index oder -1 = Entity-Matrix (+0x20) */
    uint8_t  type;
    unsigned room;
    int      welt_ok;
    int32_t  r[9], t[3];  /* letzte Weltmatrix (fuer Super_reset und Ausfall der Pose) */
} haken_t;
static haken_t s_h[RE15_SCD_MAX_PROPS];

void re15_superobj_r41_reset(void) { memset(s_h, 0, sizeof s_h); }

static int16_t s16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

/* Vater-Matrix: Gegner-Teil (Part-Welt, Part+64) bzw. Entity-Matrix +0x20. 1 = ok. */
static int vater(const haken_t *h, int32_t R[9], int32_t t[3])
{
    if (h->slot <= 0 || h->slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *e = &g_actors[h->slot];
    if (!e->active || e->type != h->type) return 0;
    if (h->teil < 0) {
        re15_skel_euler_matrix((int)e->rot_x, (int)e->rot_y, (int)e->rot_z, R);   /* RotMatrix(+0x68) */
        t[0] = e->x; t[1] = e->y; t[2] = e->z;
        return 1;
    }
    const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an;
    if (!re15_fussanker_r41_kanal(e, &sk, &an)) return 0;
    if (h->teil >= sk->bone_count) return 0;
    return re15_fussanker_r41_teil_matrix(e, sk, an, h->teil, R, t);
}

/* Objekt-Welt = Vater x [RotMatrix(+0x68) | +0x34] (0x80022da0). */
static int welt(int pi, int32_t r[9], int32_t t[3])
{
    haken_t *h = &s_h[pi];
    int32_t R[9], T[3], L[9];
    if (!vater(h, R, T)) {
        if (!h->welt_ok) return 0;
        memcpy(r, h->r, sizeof h->r); memcpy(t, h->t, sizeof h->t);   /* letzte Lage (Pose gerade nicht berechenbar) */
        return 1;
    }
    re15_skel_euler_matrix((int)g_scd.props[pi].rot_x, (int)g_scd.props[pi].rot_y, (int)g_scd.props[pi].rot_z, L);
    const int32_t u[3] = { g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z };
    for (int a = 0; a < 3; a++)
        for (int b = 0; b < 3; b++)
            r[a * 3 + b] = (int32_t)(((int64_t)R[a * 3 + 0] * L[0 * 3 + b] + (int64_t)R[a * 3 + 1] * L[1 * 3 + b] +
                                      (int64_t)R[a * 3 + 2] * L[2 * 3 + b]) >> 12);
    for (int a = 0; a < 3; a++)
        t[a] = T[a] + (int32_t)(((int64_t)R[a * 3 + 0] * u[0] + (int64_t)R[a * 3 + 1] * u[1] +
                                 (int64_t)R[a * 3 + 2] * u[2]) >> 12);
    memcpy(h->r, r, sizeof h->r); memcpy(h->t, t, sizeof h->t); h->welt_ok = 1;
    return 1;
}

int re15_superobj_r41_welt(int prop_slot, int32_t r[9], int32_t t[3])
{
    if (prop_slot < 0 || prop_slot >= RE15_SCD_MAX_PROPS) return 0;
    haken_t *h = &s_h[prop_slot];
    if (!h->aktiv) return 0;
    if (h->room != g_current_room_id || !g_scd.props[prop_slot].active) { h->aktiv = 0; return 0; }
    return welt(prop_slot, r, t);
}

int re15_superobj_r41_op_set(scd_thread_t *t)
{
    const uint8_t *pc = t->pc;
    int pi = (t->work_slot < 0) ? (int)t->work_prop_idx : -1;
    uint8_t art = (uint8_t)(pc[2] & 0xc0), b3 = pc[3];
    int ok = 0;
    if (pi >= 0 && pi < RE15_SCD_MAX_PROPS && g_scd.props[pi].active) {
        haken_t h; memset(&h, 0, sizeof h);
        if (art == 0x00) {
            ok = 1;                                                     /* Vater 0x80072d4c = Welt (@0x80040b78) */
        } else if (art == 0x80) {
            h.slot = (int)(pc[2] & 0x3f) + 1;                           /* Gegner pc[2]-0x80 -> Port-Slot +1 */
            h.teil = (b3 & 0x80) ? (int)(b3 & 0x7f) : -1;               /* @0x80040bdc `andi v0,a0,0x80` */
            if (h.slot < RE15_ACTOR_MAX && g_actors[h.slot].active) {
                h.type = g_actors[h.slot].type; h.room = g_current_room_id; h.aktiv = 1;
                int32_t R[9], T[3];
                ok = vater(&h, R, T);                                   /* Pose jetzt berechenbar? sonst OFFEN */
            }
        }
        if (ok) {
            g_scd.props[pi].x = s16(pc + 4);  g_scd.props[pi].y = s16(pc + 6);  g_scd.props[pi].z = s16(pc + 8);   /* @0x80040c78..98 */
            g_scd.props[pi].rot_x = s16(pc + 10); g_scd.props[pi].rot_y = s16(pc + 12); g_scd.props[pi].rot_z = s16(pc + 14); /* @0x80040c9c..bc */
            g_scd.props[pi].parent_obj = -1;
            s_h[pi] = h;                                                /* art 0x00: aktiv = 0 = Welt */
        }
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[superobj41] F%u Super_set Objekt %d Art 0x%02X Teil-Byte 0x%02X lokal (%d,%d,%d) Rot (%d,%d,%d) -> %s\n",
            (unsigned)g_engine.frame_count, pi, art, b3, s16(pc + 4), s16(pc + 6), s16(pc + 8), s16(pc + 10), s16(pc + 12),
            s16(pc + 14), ok ? (art ? "angehaengt" : "Welt") : "OFFEN (Vater nicht berechenbar, uebersprungen)");
#endif
    t->pc += 16;
    return 1;
}

int re15_superobj_r41_op_reset(scd_thread_t *t)
{
    const uint8_t *pc = t->pc;
    int pi = (t->work_slot < 0) ? (int)t->work_prop_idx : -1;
    if (pi >= 0 && pi < RE15_SCD_MAX_PROPS && g_scd.props[pi].active) {
        int32_t r[9], w[3];
        int hing = s_h[pi].aktiv && re15_superobj_r41_welt(pi, r, w);
        if (hing) { g_scd.props[pi].x = w[0]; g_scd.props[pi].y = w[1]; g_scd.props[pi].z = w[2]; }   /* @0x80040cd8..cf8 */
        s_h[pi].aktiv = 0;                                                                             /* @0x80040cec Welt */
        g_scd.props[pi].rot_x = s16(pc + 2); g_scd.props[pi].rot_y = s16(pc + 4); g_scd.props[pi].rot_z = s16(pc + 6);  /* @0x80040cfc..d1c */
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[superobj41] F%u Super_reset Objekt %d %s -> lokal (%d,%d,%d) Rot (%d,%d,%d)\n",
                (unsigned)g_engine.frame_count, pi, hing ? "Welt-Lage uebernommen" : "nicht angehaengt",
                g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z,
                g_scd.props[pi].rot_x, g_scd.props[pi].rot_y, g_scd.props[pi].rot_z);
#endif
    }
    t->pc += 8;
    return 1;
}
