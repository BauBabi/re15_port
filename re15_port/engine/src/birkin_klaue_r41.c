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
#include "re15_camera.h"
#include "re15_scd.h"
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
static int bkk_lagen(const re15_actor_t *e, re15_bkk_lage_t *alle, int max, int *n_out);

int re15_bkk_lage_logik(const re15_actor_t *e, re15_bkk_lage_t *out)
{
    static re15_bkk_lage_t alle[RE15_EMD_MAX_BONES];
    int n = 0;
    if (!out || !bkk_lagen(e, alle, RE15_EMD_MAX_BONES, &n) || n <= BKK_TEIL) return 0;
    *out = alle[BKK_TEIL];
    return 1;
}

/* Welt-Lage JEDES Teils der gezeichneten 0x30-Bank (gleiche Rechnung wie fuer Teil 15). */
static int bkk_lagen(const re15_actor_t *e, re15_bkk_lage_t *alle, int max, int *n_out)
{
    if (!e || !alle || e->type != 0x30) return 0;
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
    const int nb = b->skel.bone_count < max ? b->skel.bone_count : max;
    for (int bi = 0; bi < nb; bi++) {
        const re15_skel_pose_t *p = &poses[bi];
        re15_bkk_lage_t *out = &alle[bi];
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
    }
    *n_out = nb;
    return 1;
}

/* ---------------------------------------------------------------------------------------------------------------------
 * SICHTBARE WAENDE. Grundlage: die SCA-Zellen (RDT +0x20, 12 B, re15_sca_entry_t) sind die Waende und Hindernisse des
 * Raums (Memory reai-v2-kollisionszellen-sind-waende). In 50E0/50F1/50A0/50A1 tragen alle Zellen Band 0 und das Wort 0x0300
 * (gemessen, Bericht Runde 3). Ob eine Zelle im BILD eine Wand ist und wie hoch sie reicht, steht nicht in den Daten; das
 * entscheidet die Tabelle unten nach den Messbildern (Zellen auf dem Hintergrund der Kampf-Cuts).
 * Runde 2 (Pruefbericht r1 rot 2: 50E0 #16 galt ungeprueft als Wand bis zur Decke): JEDE Zelle der drei Raeume steht jetzt in der
 * Tabelle, je mit Messbild-Beleg (Runde 2 r2/mb/c_*.png: Zelle als Quader mit Hoehen 1000/1900/3000/4000 auf dem Hintergrund
 * der Cuts, in denen sie am groessten liegt). Der Riegel unit_r41_birkin_kralle_schwanz_abdeckung prueft, dass keine Zelle
 * dieser Raeume fehlt. Nur Zellen anderer Raeume (3070 im Test hangar_wand) gelten weiter als Wand.
 * ------------------------------------------------------------------------------------------------------------------- */
enum { BKK_KEINE = 0, BKK_WAND = 1, BKK_HINDERNIS = 2 };
typedef struct {
    uint16_t raum;            /* 4-stellige Raum-Id, Spieler-Ziffer ignoriert (50A0 == 50A1, 50F0 == 50F1) */
    int16_t  x, z;            /* Zell-Ecke */
    uint16_t w, d;            /* Zell-Ausdehnung */
    uint8_t  art;
    int16_t  oben;            /* BKK_HINDERNIS: Oberkante (Welt-y, negativ = ueber dem Boden y 0) */
} bkk_wand_t;

/* Gelaender-Oberkante der Labor-Laufstege (50E0/50F1), GEMESSEN (Messbilder Runde 1 "synchron", tools/kandidaten.py: waagerechte
 * Kandidatenlinien 800..2000 auf der Gangkante der Zelle, in ZWEI Cuts gegen das obere Rohr des Gelaenders gelesen):
 *   50E0 #17 (x -16750..-15470), Gangkante x -15470: Cut 4 oberes Rohr auf 1800, Cut 5 auf 1900 (Zellmitte x -16110: 1700/1900);
 *   50F1 #5 (z -11435..-10415), Gangkante z -10415: Cut 7 1800..2100, Cut 6 1500..1800.
 *   Runde 2 dazu 50E0 #10, Gangkante z -22315: in Cut 5 und Cut 6 liegt das obere Rohr zwischen den Kandidaten 1600 und 1900
 *   (r2/mb/t_k10_16.png).
 * Genommen wird der obere Wert der Zwei-Cut-Messung an #17 (1900): darunter steckt die Klinge sichtbar im Gelaender, darueber
 * geht sie ueber das Gelaender hinweg (Schacht dahinter). Die Gelaender unten haben im Bild dieselbe Bauart (zwei waagerechte
 * Rohre auf Pfosten). */
#define BKK_GELAENDER_OBEN (-1900)
#define BKK_G(raum, x, z, w, d) { raum, x, z, w, d, BKK_HINDERNIS, BKK_GELAENDER_OBEN }
#define BKK_W(raum, x, z, w, d) { raum, x, z, w, d, BKK_WAND, 0 }

/* Jede Zelle (Index wie im RDT; die Eintraege ab #20 / #15 / #8 wiederholen dieselben Rechtecke) nach den Messbildern
 * r2/mb/c_*.png: Gelaender = im Bild Rohr-Gelaender, dahinter Schacht oder offener Raum; Wand = im Bild bis ueber 4000
 * geschlossen (Raumwand, Pfeiler, Turm, Maschine, Gangwand). */
static const bkk_wand_t k_waende[] = {
    /* ROOM50E0 (Labor, Leon) */
    BKK_G(0x50E0, -10490, -21655, 11565, 11815),   /* #0  Schacht Ost mit Gelaender an der Gangkante x -10490 (Cut 5/6) */
    BKK_W(0x50E0, -30600, -18595,  8810,  1070),   /* #1  Maschinenwand mit Warnstreifen (Cut 1), Pfeiler bis ueber 4000 (Cut 8) */
    BKK_W(0x50E0, -32000, -31975,  6030, 34890),   /* #2  Westwand (Cut 3/5) */
    BKK_W(0x50E0, -32000, -32000, 44860,  5175),   /* #3  Suedwand, volle Hoehe (Cut 6 rechts, Cut 5 links) */
    BKK_W(0x50E0,   6620, -31975,  6260, 34860),   /* #4  Ostwand (Cut 6/7) */
    BKK_G(0x50E0,   -180, -15165,  9140,  1080),   /* #5  Gelaender mit Warnstreifen-Sockel (Cut 7) */
    BKK_G(0x50E0,  -7880, -10435,  1050,  6020),   /* #6  Gelaender am Schacht (Cut 3 vorn links) */
    BKK_W(0x50E0, -31960,  -4795, 44830,  7750),   /* #7  Nordwand (Cut 4/5) */
    BKK_W(0x50E0, -18475, -12845,  2390,  1150),   /* #8  Pfeiler am Turmfuss (Cut 0/3) */
    BKK_W(0x50E0, -18475, -19285,  2390,  1150),   /* #9  Maschinenpfeiler bis ueber 4000 (Cut 4) */
    BKK_G(0x50E0, -10485, -22315, 12330,  1110),   /* #10 Gelaender an der Gangkante z -22315 (Cut 5/6, Hoehe s. oben) */
    BKK_G(0x50E0, -10490, -10435,  3660,  1540),   /* #11 Gelaender am Schacht (Cut 3 vorn links, Cut 4) */
    BKK_G(0x50E0, -28800,  -5435, 13330,  1060),   /* #12 Gelaender vor der Nordwand (Cut 4 hinter dem Pult) */
    BKK_G(0x50E0, -23470,  -9005,  8000,  1060),   /* #13 Pult und Gelaender (Cut 4) */
    BKK_W(0x50E0, -26600,  -9935,  1110,  5560),   /* #14 Maschinenwand mit Warnstreifen (Cut 2/3) */
    BKK_G(0x50E0, -23470, -19525,  1230, 11580),   /* #15 Pult und Gelaender am Steg (Cut 8), Gitterlaufsteg darunter (Cut 0) */
    BKK_G(0x50E0, -16750, -13125,  1280,  5180),   /* #16 Gelaender, setzt #17 auf derselben Linie fort (Cut 4, Pruefer r1) */
    BKK_G(0x50E0, -16750, -28895,  1280, 11100),   /* #17 Gelaender des Laufstegs, dahinter offener Schacht (Cut 4/5) */
    BKK_G(0x50E0,    245, -22315,  1600,  8190),   /* #18 Gelaender mit Warnstreifen-Sockel (Cut 7) */
    BKK_W(0x50E0, -23275, -19305,  7790,  7790),   /* #19 Kreis = Turm (Cut 0: Turm bis zur Decke; Cut 3/5) */
    /* ROOM50F1 (Labor, Elza) */
    BKK_G(0x50F0, -26925, -19020,  4660,   350),   /* #0  Gelaender neben dem Tor TYPE-P (Cut 0/1) */
    BKK_W(0x50F0, -31200, -29405,  6030, 26460),   /* #1  Westwand mit Tor TYPE-P (Cut 0/2) */
    BKK_G(0x50F0, -14850, -23990,   605, 19280),   /* #2  Gelaender Laufsteg (Cut 5/6) */
    BKK_W(0x50F0, -31100, -29325, 38610,  2600),   /* #3  Suedwand (Cut 3/9) */
    BKK_W(0x50F0,   5195, -29325,  2340, 26310),   /* #4  Ostwand (Cut 4/8) */
    BKK_G(0x50F0,  -9240, -11435, 16770,  1020),   /* #5  Gelaender Laufsteg (Cut 4/7) */
    BKK_W(0x50F0, -31110,  -6370, 38630,  3380),   /* #6  Nordwand (Cut 5/7) */
    BKK_W(0x50F0, -18475, -12845,  2390,  1150),   /* #7  Saeule bis ueber 4000 (Cut 6) */
    BKK_W(0x50F0, -18475, -19285,  2390,  1150),   /* #8  Pfeiler am Turm (Cut 0/2) */
    BKK_G(0x50F0, -22595, -23990,   405,  6770),   /* #9  Gelaender (Cut 1) */
    BKK_G(0x50F0, -22595, -23990,  8195,   415),   /* #10 Gelaender (Cut 0/2) */
    BKK_W(0x50F0,  -9300, -23000,  7725,  1050),   /* #11 grosse Maschine mit schraeger Oberseite (Cut 4/9) */
    BKK_G(0x50F0,  -9725, -23000,  2000, 12575),   /* #12 Gelaender Laufsteg (Cut 5); das Suedende ueberlappt #11 = Maschine */
    BKK_W(0x50F0,  -3625, -29325,  2000,  7375),   /* #13 Maschine hinter #11 (Cut 4/9) */
    BKK_W(0x50F0, -23275, -19305,  7790,  7790),   /* #14 Kreis = Turm (Cut 0: Turmfuss; Cut 5) */
    /* ROOM50A0/50A1 (Hangar-Gaenge): alle Zellen Gangwaende bis zur Decke (r2/mb/c_a_0.png, t_a_rest.png) */
    BKK_W(0x50A0, -15300, -23500,  2900, 11000),   /* #0  Gangwand (Cut 2/6) */
    BKK_W(0x50A0, -16300,   4940,  6800,  2000),   /* #1  Stirnwand mit Tuer (Cut 0/1) */
    BKK_W(0x50A0, -11060, -10090,  2000, 17100),   /* #2  Wand am Tor (Cut 4/12) */
    BKK_W(0x50A0, -10900, -10090, 10100,  2000),   /* #3  Wand mit Tor "C-2 HANGAR" (Cut 4/12) */
    BKK_W(0x50A0,  -2700, -28700,  2000, 20500),   /* #4  Ostwand (Cut 3/4) */
    BKK_W(0x50A0, -28300, -15800,  6400,  1500),   /* #5  Gangwand (Cut 3/7) */
    BKK_W(0x50A0, -28500, -27800,  1999, 13300),   /* #6  Gangwand (Cut 7/10) */
    BKK_W(0x50A0, -23400, -23000,  2000,  8500),   /* #7  Gangwand (Cut 7/8) */
    BKK_W(0x50A0, -15580, -13400,  2000, 19600),   /* #9  Gangwand (Cut 4/8) */
    BKK_W(0x50A0, -28300, -28600, 27000,  2000),   /* #12 Suedwand (Cut 6/7) */
    BKK_W(0x50A0, -23500, -23400, 10900,  2000),   /* #13 Gangwand (Cut 3/7) */
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

int re15_bkk_zelle_art(uint16_t raum_id, const re15_sca_entry_t *c)
{
    const bkk_wand_t *w = c ? bkk_eintrag(raum_id, c) : NULL;
    return w ? (int)w->art : 0;
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

#define BKK_ZELLEN_MAX 256   /* SCA-Eintraege je Raum: 50E0 100, 50F1 75, 50A0 21 (Zensus Runde 2); mehr wird nicht geprueft */
typedef struct {
    const re15_rdt_t *rdt; uint16_t raum; re15_bkk_befund_t *b; const re15_bkk_sicht_t *sicht;
    int32_t tief[BKK_ZELLEN_MAX];       /* je Zelle: tiefster Pruefpunkt (kleinster Abstand), 0x7fffffff = keiner */
    int32_t tp[BKK_ZELLEN_MAX][3];      /* dieser Punkt */
    uint8_t tsicht[BKK_ZELLEN_MAX];     /* liegt er im Bild */
    int32_t naechst; int nzelle; int32_t np[3];   /* kleinster Abstand >= 0 (nur fuer die Messschiene, wenn kein Kontakt) */
} bkk_ctx_t;

/* Runde 2 (Pruefbericht r1 rot 1: 5 von 53 Kontakten lagen ausserhalb des Kamerabilds - man sah den Rueckzug, aber keinen
 * Treffer). NUTZERENTSCHEID "nur, wenn er gegen eine Wand schlaegt": sichtbar ist ein Treffer nur, wenn die EINSCHLAGSTELLE im
 * Bild liegt. Einschlagstelle je Zelle = der tiefste Pruefpunkt der Klinge in dieser Zelle (dasselbe Mass, mit dem der Pruefer
 * r1 die 5 Faelle fand: sicht.py projiziert den tiefsten Punkt pt=). Ein Kontakt zaehlt, wenn fuer mindestens eine Zelle ihr
 * tiefster Punkt im Bild liegt. Gemessen (Runde 2, sichtmass.py) reicht "irgendein eindringender Punkt im Bild" NICHT: bei F1723
 * lagen 140 Bildpunkte des Eindringens bis 15 Bildpunkte vom linken Rand im Bild, die Einschlagstelle aber links ausserhalb,
 * und im Video ist kein Treffer zu sehen (Pruefer-Messbild eig_50E0_z3).
 * Projektion wie der Zeichner und wie bf_punkt_im_bild (birkin_formen.c): Kamera des laufenden Cuts (re15_camera_build_view =
 * FUN_80053ca4), sx = 160 + H*vx/vz, sy = 120 + H*vy/vz, H = fov_screen_dist; im Bild = vz > 0 und 0 <= sx < 320, 0 <= sy < 240
 * (Bildgroesse 320x240 des Spiels). PORT-WAHL nach dem Entscheid; kein Capcom-Code. */
static int bkk_im_bild(const re15_camera_view_t *v, const int32_t p[3])
{
    const int64_t vx = (((int64_t)v->rot[0] * p[0] + (int64_t)v->rot[1] * p[1] + (int64_t)v->rot[2] * p[2]) >> 12) + v->trans[0];
    const int64_t vy = (((int64_t)v->rot[3] * p[0] + (int64_t)v->rot[4] * p[1] + (int64_t)v->rot[5] * p[2]) >> 12) + v->trans[1];
    const int64_t vz = (((int64_t)v->rot[6] * p[0] + (int64_t)v->rot[7] * p[1] + (int64_t)v->rot[8] * p[2]) >> 12) + v->trans[2];
    if (vz <= 0) return 0;
    const int64_t sx = 160 + vx * v->fov_screen_dist / vz, sy = 120 + vy * v->fov_screen_dist / vz;
    return sx >= 0 && sx < 320 && sy >= 0 && sy < 240;
}

/* Runde 2: Verdeckung der Einschlagstelle durch Birkins EIGENEN gezeichneten Koerper (alle Teile der gezeichneten Bank ausser
 * der Klinge, Teil 15, in der Pose dieses Bilds). Gemessen (Messbild r2/mb/kontakt_50E0.png): bei F1992 (Cut 6, Kamera hinter
 * Birkin) lag die Einschlagstelle im Bild, aber mitten auf Birkins Ruecken - der Treffer war nicht zu sehen. Test: Strahl von der
 * Kamera (Cut-Position RID +0x04) zur Einschlagstelle gegen jedes Dreieck/Viereck (Moeller-Trumbore); getroffen vor dem Punkt
 * (mehr als 1 Einheit davor) = verdeckt. */
static int bkk_dreieck(const double o[3], const double d[3], const double a[3], const double b[3], const double c[3], double tmax)
{
    const double e1[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] }, e2[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
    const double pv[3] = { d[1] * e2[2] - d[2] * e2[1], d[2] * e2[0] - d[0] * e2[2], d[0] * e2[1] - d[1] * e2[0] };
    const double det = e1[0] * pv[0] + e1[1] * pv[1] + e1[2] * pv[2];
    if (det > -1e-9 && det < 1e-9) return 0;
    const double inv = 1.0 / det, tv[3] = { o[0] - a[0], o[1] - a[1], o[2] - a[2] };
    const double u = (tv[0] * pv[0] + tv[1] * pv[1] + tv[2] * pv[2]) * inv;
    if (u < 0.0 || u > 1.0) return 0;
    const double qv[3] = { tv[1] * e1[2] - tv[2] * e1[1], tv[2] * e1[0] - tv[0] * e1[2], tv[0] * e1[1] - tv[1] * e1[0] };
    const double v = (d[0] * qv[0] + d[1] * qv[1] + d[2] * qv[2]) * inv;
    if (v < 0.0 || u + v > 1.0) return 0;
    const double t = (e2[0] * qv[0] + e2[1] * qv[1] + e2[2] * qv[2]) * inv;
    return t > 0.0 && t < tmax;
}
/* Verdeckung durch die SPIELFIGUR (Messbild r2/mb/kontakt2_50E0.png: F1477 und F1730 lag die Einschlagstelle an der Suedwand
 * hinter Leon). Das Spielermodell liegt nur im PC-Zeichner; die Engine kennt den Koerper der Figur als Trefferzylinder des
 * Originals: EXE-Struktur @0x80073e94 (PSX.EXE Datei-Offset 0x64694) = Mittelpunkt-Versatz (0,-1530,0), Radius 450, halbe
 * Hoehe 1530 (re15_player_apply_hitbox, re15_damage.c; Felder +0x78/+0x7c). Strahl Kamera -> Einschlagstelle schneidet diesen
 * Zylinder vor der Stelle = verdeckt. */
static int bkk_zylinder(const double o[3], const double d[3], double tmax, const re15_actor_t *a)
{
    if (!a || !a->active || !a->hit_radius_max || !a->hit_height) return 0;
    const double cx = a->x + a->hit_offset_x, cy = a->y + a->hit_offset_y, cz = a->z + a->hit_offset_z;
    const double r = a->hit_radius_max, h = a->hit_height;
    double t0 = 0.0, t1 = tmax;
    /* waagerecht: |(o + t d) - c|_xz <= r */
    const double fx = o[0] - cx, fz = o[2] - cz;
    const double A = d[0] * d[0] + d[2] * d[2], B = 2.0 * (fx * d[0] + fz * d[2]), C = fx * fx + fz * fz - r * r;
    if (A < 1e-9) { if (C > 0.0) return 0; }
    else {
        const double disk = B * B - 4.0 * A * C;
        if (disk < 0.0) return 0;
        const double q = sqrt(disk), a1 = (-B - q) / (2.0 * A), a2 = (-B + q) / (2.0 * A);
        if (a1 > t0) t0 = a1;
        if (a2 < t1) t1 = a2;
    }
    /* senkrecht: cy - h <= o.y + t d.y <= cy + h */
    if (d[1] > -1e-9 && d[1] < 1e-9) { if (o[1] < cy - h || o[1] > cy + h) return 0; }
    else {
        double b1 = (cy - h - o[1]) / d[1], b2 = (cy + h - o[1]) / d[1];
        if (b1 > b2) { const double tt = b1; b1 = b2; b2 = tt; }
        if (b1 > t0) t0 = b1;
        if (b2 < t1) t1 = b2;
    }
    return t0 < t1 && t1 > 0.0;
}

static void bkk_welt(const re15_bkk_lage_t *l, const re15_md1_vertex_t *v, int32_t w[3]);
static int bkk_verdeckt(const re15_bkk_sicht_t *si, const int32_t p[3])
{
    static re15_bkk_lage_t alle[RE15_EMD_MAX_BONES];
    int n = 0;
    if (!si || !si->koerper || !bkk_lagen(si->koerper, alle, RE15_EMD_MAX_BONES, &n)) return 0;
    const re15_enemy_bank_t *bk = re15_enemy_find(0x30);
    if (!bk || !bk->ok) return 0;
    const double o[3] = { si->cam[0], si->cam[1], si->cam[2] };
    const double d[3] = { p[0] - o[0], p[1] - o[1], p[2] - o[2] };
    const double len = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
    if (len < 2.0) return 0;
    const double tmax = 1.0 - 1.0 / len;                     /* mehr als 1 Einheit vor der Einschlagstelle */
    if (bkk_zylinder(o, d, tmax, &g_actors[RE15_ACTOR_SLOT_PLAYER])) return 1;
    for (int k = 0; k < n && k < bk->md1.mesh_count; k++) {
        if (k == BKK_TEIL) continue;
        const re15_md1_mesh_t *m = &bk->md1.meshes[k];
        int32_t w[4][3]; double a[4][3];
        for (int i = 0; i < m->triangle_count; i++) {
            const re15_md1_triangle_t *t = &m->triangles[i];
            if (t->v0 >= m->tri_vertex_count || t->v1 >= m->tri_vertex_count || t->v2 >= m->tri_vertex_count) continue;
            bkk_welt(&alle[k], &m->tri_vertices[t->v0], w[0]); bkk_welt(&alle[k], &m->tri_vertices[t->v1], w[1]);
            bkk_welt(&alle[k], &m->tri_vertices[t->v2], w[2]);
            for (int j = 0; j < 3; j++) for (int r = 0; r < 3; r++) a[j][r] = w[j][r];
            if (bkk_dreieck(o, d, a[0], a[1], a[2], tmax)) return 1;
        }
        for (int i = 0; i < m->quad_count; i++) {
            const re15_md1_quad_t *q = &m->quads[i];
            if (q->v0 >= m->quad_vertex_count || q->v1 >= m->quad_vertex_count || q->v2 >= m->quad_vertex_count ||
                q->v3 >= m->quad_vertex_count) continue;
            bkk_welt(&alle[k], &m->quad_vertices[q->v0], w[0]); bkk_welt(&alle[k], &m->quad_vertices[q->v1], w[1]);
            bkk_welt(&alle[k], &m->quad_vertices[q->v2], w[2]); bkk_welt(&alle[k], &m->quad_vertices[q->v3], w[3]);
            for (int j = 0; j < 4; j++) for (int r = 0; r < 3; r++) a[j][r] = w[j][r];
            /* PSX-Viereck: Ecken 0-1-3-2 im Umlauf = Dreiecke 0-1-2 und 1-3-2 */
            if (bkk_dreieck(o, d, a[0], a[1], a[2], tmax) || bkk_dreieck(o, d, a[1], a[3], a[2], tmax)) return 1;
        }
    }
    return 0;
}

static void bkk_punkt(bkk_ctx_t *cx, const int32_t p[3])
{
    const re15_rdt_t *rdt = cx->rdt;
    cx->b->punkte++;
    int sichtbar = -1;                                       /* erst bei Bedarf projizieren */
    for (int i = 0; i < rdt->sca_count && i < BKK_ZELLEN_MAX; i++) {
        const re15_sca_entry_t *c = &rdt->sca[i];
        const int typ = c->type & 0x0f;
        if ((typ != 1 && typ != 3) || !(c->u0 & 1) || (c->floor >> 4) != 0 || !c->width || !c->density) continue;
        const bkk_wand_t *w = bkk_eintrag(cx->raum, c);
        const int art = w ? w->art : BKK_WAND;
        if (art == BKK_KEINE) continue;
        const int32_t a = bkk_abstand(c, art, w ? w->oben : 0, p);
        if (a >= 0) {
            if (cx->nzelle < 0 || a < cx->naechst) { cx->nzelle = i; cx->naechst = a; cx->np[0] = p[0]; cx->np[1] = p[1]; cx->np[2] = p[2]; }
            continue;
        }
        if (a < cx->tief[i]) {                               /* tiefster Punkt dieser Zelle */
            if (sichtbar < 0) sichtbar = !cx->sicht || bkk_im_bild(&cx->sicht->v, p);
            cx->tief[i] = a; cx->tsicht[i] = (uint8_t)sichtbar;
            cx->tp[i][0] = p[0]; cx->tp[i][1] = p[1]; cx->tp[i][2] = p[2];
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
                     const re15_bkk_sicht_t *sicht, re15_bkk_befund_t *b)
{
    if (!b) return 0;
    memset(b, 0, sizeof *b);
    b->zelle = -1; b->abstand = 0x7fffffff;
    if (!m || !l || !rdt || !rdt->sca || rdt->sca_count <= 0) return 0;
    static bkk_ctx_t cx;                                     /* statisch: ~4 KB; die Kralle prueft einen Aktor je Aufruf */
    memset(&cx, 0, sizeof cx);
    cx.rdt = rdt; cx.raum = raum_id; cx.b = b; cx.sicht = sicht; cx.nzelle = -1; cx.naechst = 0x7fffffff;
    for (int i = 0; i < BKK_ZELLEN_MAX; i++) cx.tief[i] = 0x7fffffff;
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
    /* Kontakt = tiefste Einschlagstelle unter den Zellen, deren Einschlagstelle im Bild liegt */
    for (int i = 0; i < rdt->sca_count && i < BKK_ZELLEN_MAX; i++) {
        if (cx.tief[i] >= 0) continue;
        if (!cx.tsicht[i]) { b->ausser_bild++; continue; }
        if (sicht && bkk_verdeckt(sicht, cx.tp[i])) { b->verdeckt++; continue; }   /* hinter Birkins eigenem Koerper */
        if (!b->kontakt || cx.tief[i] < b->abstand) {
            b->kontakt = 1; b->zelle = i; b->abstand = cx.tief[i];
            b->punkt[0] = cx.tp[i][0]; b->punkt[1] = cx.tp[i][1]; b->punkt[2] = cx.tp[i][2];
        }
    }
    if (!b->kontakt && cx.nzelle >= 0) {                     /* Messschiene: naechste Zelle ohne Eindringen */
        b->zelle = cx.nzelle; b->abstand = cx.naechst; b->punkt[0] = cx.np[0]; b->punkt[1] = cx.np[1]; b->punkt[2] = cx.np[2];
    }
    return b->kontakt;
}

/* Kamera des laufenden Cuts (g_scd.cam_id, derselbe Wert, den state.log als cam= fuehrt und mit dem der Zeichner den Hintergrund
 * waehlt). 0 = keine Kamera (kein Raum / Cut ausserhalb der RDT-Liste). */
int re15_bkk_sicht(re15_bkk_sicht_t *si, const re15_actor_t *koerper)
{
    if (!si || !g_room_rdt_ok || !g_room_rdt.cuts || (int)g_scd.cam_id >= g_room_rdt.cut_count) return 0;
    const re15_camera_cut_t *c = &g_room_rdt.cuts[g_scd.cam_id];
    si->cam[0] = c->pos_x; si->cam[1] = c->pos_y; si->cam[2] = c->pos_z;
    si->koerper = koerper;
    return re15_camera_build_view(c, &si->v) == 0;
}

int re15_bkk_kontakt(const re15_actor_t *e)
{
    re15_bkk_lage_t l; re15_bkk_befund_t b; re15_bkk_sicht_t v;
    if (!g_room_rdt_ok || !re15_bkk_lage_logik(e, &l)) return 0;
    if (!re15_bkk_sicht(&v, e)) return 0;                    /* ohne Kamera kein Bild, also kein sichtbarer Treffer */
    return re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, &v, &b);
}

/* MESS-SCHIENE (RE15_BIRKIN_DBG, kein Spielverhalten). Zeilen in birkin_dbg.log:
 *   "zeichner F.. ..." je gezeichnetem Bild des Typs 0x30: tatsaechlich gezeichnete Lage von Teil 15 + Befund der Klinge;
 *   "klinge F.. ..."   je Takt der Kralle ACT[3] (Logik-Seite, re15_bkk_lage_logik) + Befund. */
static void bkk_zeile(const char *wer, const re15_actor_t *e, const re15_bkk_lage_t *l)
{
    static int an = -1;
    if (an < 0) an = (getenv("RE15_BIRKIN_DBG") != NULL);
    if (!an || !e || e->type != 0x30) return;
    re15_bkk_befund_t b; re15_bkk_sicht_t v;
    const int sicht = re15_bkk_sicht(&v, e);
    re15_bkk_pruefen(re15_bkk_klinge(), l, g_room_rdt_ok ? &g_room_rdt : NULL, (uint16_t)g_current_room_id, sicht ? &v : NULL, &b);
    FILE *f = fopen("birkin_dbg.log", "a");
    if (!f) return;
    fprintf(f, "%s F%u ss1=%d ph2=%d mo=%d af=%d frac=%d pos=(%d,%d,%d) gier=%d R=(%d,%d,%d,%d,%d,%d,%d,%d,%d) T=(%d,%d,%d) "
            "kontakt=%d zelle=%d abstand=%d pt=(%d,%d,%d) punkte=%d cam=%d ausser_bild=%d verdeckt=%d\n", wer, (unsigned)g_engine.frame_count,
            (int)e->sub_state_1, (int)e->sub_state_2, (int)e->motion, (int)e->anim_frame, (int)e->anim_frac,
            (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y,
            (int)l->R[0], (int)l->R[1], (int)l->R[2], (int)l->R[3], (int)l->R[4], (int)l->R[5], (int)l->R[6], (int)l->R[7], (int)l->R[8],
            (int)l->T[0], (int)l->T[1], (int)l->T[2], b.kontakt, b.zelle, (int)b.abstand,
            (int)b.punkt[0], (int)b.punkt[1], (int)b.punkt[2], b.punkte, sicht ? (int)g_scd.cam_id : -1, b.ausser_bild, b.verdeckt);
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
