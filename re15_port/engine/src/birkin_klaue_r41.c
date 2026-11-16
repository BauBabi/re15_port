/* birkin_klaue_r41.c — die GEZEICHNETE Klaue der Schwanz-Gestalt gegen SICHTBARE Waende (re15_birkin_klaue_r41.h).
 *
 * NUTZERENTSCHEID 16.11.2026: Herausziehen nur, wenn die Klaue sichtbar gegen eine Wand schlaegt, und synchron dazu.
 * Kein Capcom-Code: die Schwanz-Gestalt (EM30_HANGAR.EMD) gibt es auf der Disc nicht, der Laborkampf ist ein Fan-Mod-Kampf,
 * der Hangar-Kampf eine Port-Erfindung (Bericht Runde 3). Alles hier ist PORT-WAHL nach diesem Entscheid; die Teile, die dem
 * Zeichner folgen, sind dessen Rechnung (platform/pc/main.c, Gegner-Zeichner) und tragen dort ihre Belege.
 */
#include "re15_birkin_klaue_r41.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_engine.h"
#include "re15_room.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Wurzel nur fuer Abstands-MESSWERTE (Kontakt ja/nein entscheidet allein der Innen-Test ohne Wurzel). */
static int64_t bkk_wurzel(int64_t q) { return q > 0 ? (int64_t)sqrt((double)q) : 0; }

extern void *g_anim_pose_actor;

#define BKK_TEIL 15   /* Teil/Mesh 15 = die Klaue (FUN_8001a5e0 a0 = Teil 15, +0x188 + 0xa14 @0x80117f28 STAGE5; in
                       * EM30_HANGAR traegt Mesh 15 die Klinge, birkin_formen.c BF_KLAUE) */

const re15_md1_mesh_t *re15_bkk_klinge(void)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x30);
    if (!b || !b->ok || b->md1.mesh_count <= BKK_TEIL) return NULL;
    return &b->md1.meshes[BKK_TEIL];
}

/* Lage wie der Zeichner (main.c): kf = re15_compute_actor_kf(anim, skel, npc, -1, anim_frame) bei 30 Bildern/s,
 * g_anim_pose_actor = npc (Ueberblendung +0x8f aus prev_root/prev_angles), nyaw = {c,0,s, 0,1,0, -s,0,c} (Gier rot_y),
 * mit render_scale_q12 skaliert, nyawed_rot = nyaw * pose.rot >> 12, Welt-T = nyaw * pose.trans >> 12 + Aktorlage.
 * Die Ueberblendung schreibt in den Aktor (Schnappschuss) - deshalb rechnet diese Abfrage auf einer Kopie. */
int re15_bkk_lage_logik(const re15_actor_t *e, re15_bkk_lage_t *out)
{
    if (!e || !out || e->type != 0x30) return 0;
    re15_enemy_bank_t *b = re15_enemy_find(0x30);
    if (!b || !b->ok || b->skel.bone_count <= BKK_TEIL || b->anim.clip_count <= 0) return 0;
    static re15_actor_t kopie;                 /* statisch: re15_actor_t ist gross */
    kopie = *e;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    const int kf = re15_compute_actor_kf(&b->anim, &b->skel, &kopie, -1, (uint32_t)kopie.anim_frame);
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = &kopie;
    const int rc = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rc != 0) return 0;
    const int32_t c = re15_cos_q12((int)e->rot_y), s = re15_sin_q12((int)e->rot_y);
    int32_t ny[9] = { c, 0, s, 0, 0x1000, 0, -s, 0, c };
    if (e->render_scale_q12)
        for (int k = 0; k < 9; k++) ny[k] = (ny[k] * (int32_t)e->render_scale_q12) >> 12;
    const re15_skel_pose_t *p = &poses[BKK_TEIL];
    for (int r = 0; r < 3; r++) {
        for (int cc = 0; cc < 3; cc++) {
            int64_t acc = 0;
            for (int k = 0; k < 3; k++) acc += (int64_t)ny[r * 3 + k] * (int64_t)p->rot[k * 3 + cc];
            out->R[r * 3 + cc] = (int32_t)(acc >> 12);
        }
        int64_t t = 0;
        for (int k = 0; k < 3; k++) t += (int64_t)ny[r * 3 + k] * (int64_t)p->trans[k];
        out->T[r] = (int32_t)(t >> 12);
    }
    out->T[0] += e->x; out->T[1] += e->y; out->T[2] += e->z;
    return 1;
}

/* ---------------------------------------------------------------------------------------------------------------------
 * SICHTBARE WAENDE. Grundlage: die SCA-Zellen (RDT +0x20, 12 B, re15_sca_entry_t) sind die Waende und Hindernisse des
 * Raums (Memory reai-v2-kollisionszellen-sind-waende). In 50E0/50F1/50A0/50A1 tragen alle Zellen Band 0 und das Wort 0x0300
 * (gemessen, Bericht Runde 3). Ob eine Zelle im BILD eine Wand ist und wie hoch sie reicht, steht nicht in den Daten; das
 * entscheidet die Tabelle unten nach den Messbildern (Zellen auf dem Hintergrund der Kampf-Cuts). Zellen, die nicht in der
 * Tabelle stehen, gelten als Wand bis zur Decke.
 * ------------------------------------------------------------------------------------------------------------------- */
enum { BKK_KEINE = 0, BKK_WAND = 1, BKK_HINDERNIS = 2 };
typedef struct {
    uint16_t raum;            /* 4-stellige Raum-Id, Spieler-Ziffer ignoriert (50A0 == 50A1) */
    int16_t  x, z;            /* Zell-Ecke */
    uint16_t w, d;            /* Zell-Ausdehnung */
    uint8_t  art;
    int16_t  oben;            /* BKK_HINDERNIS: Oberkante (Welt-y, negativ = ueber dem Boden y 0) */
} bkk_wand_t;

/* Gelaender-Oberkante der Labor-Laufstege (50E0/50F1), GEMESSEN (Messbilder Runde 1 "synchron", tools/kandidaten.py: waagerechte
 * Kandidatenlinien 800..2000 auf der Gangkante der Zelle, in ZWEI Cuts gegen das obere Rohr des Gelaenders gelesen):
 *   50E0 #17 (x -16750..-15470), Gangkante x -15470: Cut 4 oberes Rohr auf 1800, Cut 5 auf 1900 (Zellmitte x -16110: 1700/1900);
 *   50F1 #5 (z -11435..-10415), Gangkante z -10415: Cut 7 1800..2100, Cut 6 1500..1800.
 * Genommen wird der obere Wert der Zwei-Cut-Messung an #17 (1900): darunter steckt die Klinge sichtbar im Gelaender, darueber
 * geht sie ueber das Gelaender hinweg (Pit dahinter). Dieselbe Gelaender-Bauart (Bild) traegt #2 und #12 in 50F1. */
#define BKK_GELAENDER_OBEN (-1900)

/* Zellen nach den Messbildern (Pfosten je 250 auf den Zellkanten, Klinge auf Bildabzuegen der Kampf-Cuts, Runde 1 "synchron"):
 *   50E0 #17: Gelaender des Laufstegs (Cut 5 hinter Leon, Cut 4 links vom Steg), dahinter offener Schacht.
 *   50E0 #3 : Suedwand, volle Hoehe (Cut 6 rechts, Cut 5 links) -> nicht eingetragen = Wand.
 *   50F1 #2 (x -14850..-14245), #12 (x -9725..-7725), #5 (z -11435..-10415): Gelaender der Laufstege (Cut 5/6/7).
 *   50A0/50A1 #2/#3/#4: Hangarwaende bis zur Decke (Cut 3/4/9, Tor "C-2 HANGAR") -> nicht eingetragen = Wand. */
static const bkk_wand_t k_waende[] = {
    { 0x50E0, -16750, -28895, 1280, 11100, BKK_HINDERNIS, BKK_GELAENDER_OBEN },   /* 50E0 #17 */
    { 0x50F0, -14850, -23990,  605, 19280, BKK_HINDERNIS, BKK_GELAENDER_OBEN },   /* 50F1 #2  */
    { 0x50F0,  -9725, -23000, 2000, 12575, BKK_HINDERNIS, BKK_GELAENDER_OBEN },   /* 50F1 #12 */
    { 0x50F0,  -9240, -11435, 16770, 1020, BKK_HINDERNIS, BKK_GELAENDER_OBEN },   /* 50F1 #5  */
};

static const bkk_wand_t *bkk_eintrag(uint16_t raum, const re15_sca_entry_t *c)
{
    for (size_t i = 0; i < sizeof k_waende / sizeof k_waende[0]; i++) {
        const bkk_wand_t *w = &k_waende[i];
        if (w->raum == (raum & 0xFFF0u) && w->x == c->x && w->z == c->z && w->w == c->width && w->d == c->density)
            return w;
    }
    return NULL;
}

/* Abstand eines Punkts zur Zelle (waagerecht, mit Oberkante): < 0 = Eindringtiefe, sonst Abstand. */
static int32_t bkk_abstand(const re15_sca_entry_t *c, int art, int32_t oben, const int32_t p[3])
{
    const int32_t x0 = c->x, z0 = c->z;
    int64_t aussen2 = 0; int32_t innen = 0x7fffffff;
    if ((c->type & 0x0f) == 3) {                        /* Kreis: Radius width/2, Mitte (x+r, z+r) (push_circle FUN_8003d6a8) */
        const int32_t r = (int32_t)c->width >> 1;
        const int64_t dx = p[0] - (x0 + r), dz = p[2] - (z0 + r);
        const int64_t d2 = dx * dx + dz * dz;
        if (d2 > (int64_t)r * r) { const int64_t dd = bkk_wurzel(d2) - r; aussen2 = dd > 0 ? dd * dd : 1; }
        else innen = (int32_t)(r - bkk_wurzel(d2));
    } else {
        const int32_t x1 = x0 + c->width, z1 = z0 + c->density;
        const int32_t ax = p[0] < x0 ? x0 - p[0] : (p[0] > x1 ? p[0] - x1 : 0);
        const int32_t az = p[2] < z0 ? z0 - p[2] : (p[2] > z1 ? p[2] - z1 : 0);
        if (ax || az) aussen2 = (int64_t)ax * ax + (int64_t)az * az;
        else {
            int32_t m = p[0] - x0;
            if (x1 - p[0] < m) m = x1 - p[0];
            if (p[2] - z0 < m) m = p[2] - z0;
            if (z1 - p[2] < m) m = z1 - p[2];
            innen = m;
        }
    }
    if (art == BKK_HINDERNIS && p[1] < oben) {           /* ueber der Oberkante: kein Kontakt */
        const int64_t dy = (int64_t)oben - p[1];
        return (int32_t)bkk_wurzel(aussen2 + dy * dy);
    }
    if (aussen2 == 0) {
        if (art == BKK_HINDERNIS && p[1] - oben < innen) innen = p[1] - oben;
        return -innen;
    }
    return (int32_t)bkk_wurzel(aussen2);
}

static void bkk_welt(const re15_bkk_lage_t *l, const re15_md1_vertex_t *v, int32_t w[3])
{
    for (int r = 0; r < 3; r++)
        w[r] = l->T[r] + (int32_t)(((int64_t)l->R[r * 3] * v->x + (int64_t)l->R[r * 3 + 1] * v->y + (int64_t)l->R[r * 3 + 2] * v->z) >> 12);
}

#define BKK_KANTE_SCHRITT 64   /* Kantenpunkte hoechstens 64 Einheiten auseinander: die duennste Zelle der drei Raeume ist
                                * 350 tief (50F1 #0), ein Klingendreieck kann eine Zelle also nicht zwischen zwei Punkten
                                * ueberspringen */

typedef struct { const re15_rdt_t *rdt; uint16_t raum; re15_bkk_befund_t *b; } bkk_ctx_t;

static void bkk_punkt(bkk_ctx_t *cx, const int32_t p[3])
{
    const re15_rdt_t *rdt = cx->rdt;
    cx->b->punkte++;
    for (int i = 0; i < rdt->sca_count; i++) {
        const re15_sca_entry_t *c = &rdt->sca[i];
        const int typ = c->type & 0x0f;
        if ((typ != 1 && typ != 3) || !(c->u0 & 1) || (c->floor >> 4) != 0 || !c->width || !c->density) continue;
        const bkk_wand_t *w = bkk_eintrag(cx->raum, c);
        const int art = w ? w->art : BKK_WAND;
        if (art == BKK_KEINE) continue;
        const int32_t a = bkk_abstand(c, art, w ? w->oben : 0, p);
        if (cx->b->zelle < 0 || a < cx->b->abstand) {
            cx->b->zelle = i; cx->b->abstand = a;
            cx->b->punkt[0] = p[0]; cx->b->punkt[1] = p[1]; cx->b->punkt[2] = p[2];
        }
    }
}

static void bkk_kante(bkk_ctx_t *cx, const int32_t a[3], const int32_t e[3])
{
    const int64_t dx = e[0] - a[0], dy = e[1] - a[1], dz = e[2] - a[2];
    const int64_t l = bkk_wurzel(dx * dx + dy * dy + dz * dz);
    const int n = (int)(l / BKK_KANTE_SCHRITT);
    for (int k = 1; k <= n; k++) {
        const int32_t p[3] = { a[0] + (int32_t)(dx * k / (n + 1)), a[1] + (int32_t)(dy * k / (n + 1)), a[2] + (int32_t)(dz * k / (n + 1)) };
        bkk_punkt(cx, p);
    }
}

int re15_bkk_pruefen(const re15_md1_mesh_t *m, const re15_bkk_lage_t *l, const re15_rdt_t *rdt, uint16_t raum_id,
                     re15_bkk_befund_t *b)
{
    if (!b) return 0;
    memset(b, 0, sizeof *b);
    b->zelle = -1; b->abstand = 0x7fffffff;
    if (!m || !l || !rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    bkk_ctx_t cx = { rdt, raum_id, b };
    int32_t w[4][3];
    for (int i = 0; i < m->tri_vertex_count; i++) { bkk_welt(l, &m->tri_vertices[i], w[0]); bkk_punkt(&cx, w[0]); }
    for (int i = 0; i < m->quad_vertex_count; i++) { bkk_welt(l, &m->quad_vertices[i], w[0]); bkk_punkt(&cx, w[0]); }
    for (int i = 0; i < m->triangle_count; i++) {
        const re15_md1_triangle_t *t = &m->triangles[i];
        if (t->v0 >= m->tri_vertex_count || t->v1 >= m->tri_vertex_count || t->v2 >= m->tri_vertex_count) continue;
        bkk_welt(l, &m->tri_vertices[t->v0], w[0]); bkk_welt(l, &m->tri_vertices[t->v1], w[1]); bkk_welt(l, &m->tri_vertices[t->v2], w[2]);
        bkk_kante(&cx, w[0], w[1]); bkk_kante(&cx, w[1], w[2]); bkk_kante(&cx, w[2], w[0]);
    }
    for (int i = 0; i < m->quad_count; i++) {
        const re15_md1_quad_t *q = &m->quads[i];
        if (q->v0 >= m->quad_vertex_count || q->v1 >= m->quad_vertex_count || q->v2 >= m->quad_vertex_count ||
            q->v3 >= m->quad_vertex_count) continue;
        bkk_welt(l, &m->quad_vertices[q->v0], w[0]); bkk_welt(l, &m->quad_vertices[q->v1], w[1]);
        bkk_welt(l, &m->quad_vertices[q->v2], w[2]); bkk_welt(l, &m->quad_vertices[q->v3], w[3]);
        /* PSX-Viereck: Ecken 0-1-3-2 im Umlauf */
        bkk_kante(&cx, w[0], w[1]); bkk_kante(&cx, w[1], w[3]); bkk_kante(&cx, w[3], w[2]); bkk_kante(&cx, w[2], w[0]);
    }
    b->kontakt = (b->zelle >= 0 && b->abstand < 0);
    return b->kontakt;
}

int re15_bkk_kontakt(const re15_actor_t *e)
{
    re15_bkk_lage_t l; re15_bkk_befund_t b;
    if (!g_room_rdt_ok || !re15_bkk_lage_logik(e, &l)) return 0;
    return re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, &b);
}

/* MESS-SCHIENE (RE15_BIRKIN_DBG, kein Spielverhalten). Zeilen in birkin_dbg.log:
 *   "zeichner F.. ..." je gezeichnetem Bild des Typs 0x30: tatsaechlich gezeichnete Lage von Teil 15 + Befund der Klinge;
 *   "klinge F.. ..."   je Takt der Kralle ACT[3] (Logik-Seite, re15_bkk_lage_logik) + Befund. */
static void bkk_zeile(const char *wer, const re15_actor_t *e, const re15_bkk_lage_t *l)
{
    static int an = -1;
    if (an < 0) an = (getenv("RE15_BIRKIN_DBG") != NULL);
    if (!an || !e || e->type != 0x30) return;
    re15_bkk_befund_t b;
    re15_bkk_pruefen(re15_bkk_klinge(), l, g_room_rdt_ok ? &g_room_rdt : NULL, (uint16_t)g_current_room_id, &b);
    FILE *f = fopen("birkin_dbg.log", "a");
    if (!f) return;
    fprintf(f, "%s F%u ss1=%d ph2=%d mo=%d af=%d frac=%d pos=(%d,%d,%d) gier=%d R=(%d,%d,%d,%d,%d,%d,%d,%d,%d) T=(%d,%d,%d) "
            "kontakt=%d zelle=%d abstand=%d pt=(%d,%d,%d) punkte=%d\n", wer, (unsigned)g_engine.frame_count,
            (int)e->sub_state_1, (int)e->sub_state_2, (int)e->motion, (int)e->anim_frame, (int)e->anim_frac,
            (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y,
            (int)l->R[0], (int)l->R[1], (int)l->R[2], (int)l->R[3], (int)l->R[4], (int)l->R[5], (int)l->R[6], (int)l->R[7], (int)l->R[8],
            (int)l->T[0], (int)l->T[1], (int)l->T[2], b.kontakt, b.zelle, (int)b.abstand,
            (int)b.punkt[0], (int)b.punkt[1], (int)b.punkt[2], b.punkte);
    fclose(f);
}
void re15_bkk_zeichner_melden(const re15_actor_t *e, const int32_t R[9], const int32_t T[3])
{
    re15_bkk_lage_t l;
    memcpy(l.R, R, sizeof l.R); memcpy(l.T, T, sizeof l.T);
    bkk_zeile("zeichner", e, &l);
}
void re15_bkk_logik_melden(const re15_actor_t *e)
{
    re15_bkk_lage_t l;
    if (re15_bkk_lage_logik(e, &l)) bkk_zeile("klinge", e, &l);
}
