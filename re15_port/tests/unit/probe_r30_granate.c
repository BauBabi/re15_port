/* probe_r30_granate.c — RIEGEL (Runde 30, Nachtrag K: Handgranate im Hebetisch).
 *
 * Auftrag (AUFTRAG.md K): "im hochfahrenden model in irons office eine granate mit hochfahren
 * haben". Dossier analysis/befunde_runde30/nachtrag-granate.md, Konstanten include/re15_granate.h.
 *
 * PRUEFUNGEN
 *  Modell (gen/granate_prop.inc gegen die ausgelieferte PLD/PL00W09.PLW):
 *    1  MD1: ein Mesh, 24 Punkte, 5 Dreiecke, 14 Vierecke
 *    2  jede Flaeche = eine Waffen-Flaeche (alle UV-v >= 200) aus dir[2], in Dateireihenfolge,
 *       Punkte/Normalen gedreht (x,y,z) -> (x, -(z-32), y-190), UV-Werte gleich,
 *       clut 0x7840 -> 0x7800, page 0x0081 -> 0x0080
 *    3  TIM 128x256 8bpp, CLUT = dir[3]-CLUT (256 Eintraege), dir[3]-Bild 56x32 bei (72,224)
 *  Sitz (je ROOM1150.RDT und ROOM1151.RDT, Plattform-Koordinaten, Kanten in 8 Stuecke geteilt).
 *  Runde 32: im LINKEN UNTEREN Fach von Prop 0 (Runde 30/31: oben in der Kuppel, Boden -1036):
 *    4  tiefster Punkt genau AUF dem Boden des Fachs (Viereck 89, y=-90)
 *    5  Abstand zur Sicherung (Zylinder r=26 um deren GEDREHTE Laengsachse, +-203) > 0
 *    6  unter der Fachdecke (Viereck 87, y=-810), vor der Rueckwand x=-1258, hinter der offenen
 *       Vorderkante x=2
 *    7  "Granate links": ganz im linken Fach zwischen den Waenden z=96/861 (Viereck 86/88),
 *       links der Sicherung (+z = Schirm rechts in Cut 4). Links/rechts GEMESSEN: Dossier
 *       runde32 hebetisch_faecher.md §4
 *  Fahrten (je Raum, Bildschleife in der Reihenfolge des Spiels):
 *    8  Fall A Fahrt 1: genau EIN Sicherungs- und EIN Granaten-Modal, Granate NACH Sicherung,
 *       beide in der RUHE OBEN (y = -1205, Runde 31; vorher Fenster (-5000,-1100] mitten im
 *       Hub); No/No -> keine Flags, beide Props sichtbar,
 *       nichts im Inventar
 *    9  Fall A Fahrt 2: wieder 1+1 in derselben Folge; Yes/Yes -> Flags (9,53)/(9,56), beide
 *       Props weg, Item 0x40 x1 und 0x09 x RE15_GRANATE_MENGE im Inventar
 *   10  Fall A Fahrt 3: kein Modal
 *   11  Fall B: Sicherung Yes / Granate No -> nur (9,53); Fahrt 2 NUR das Granaten-Modal
 *   12  Fall C: Sicherung No / Granate Yes -> nur (9,56), Granate weg; Fahrt 2 NUR Sicherung
 *   13  Fall D: Flag (9,56) VOR dem Raumstart -> kein Granaten-Prop im Pool, Fahrt: nur das
 *       Sicherungs-Modal (NEGATIV-KONTROLLE des Anlegens)
 *   14  Fall E: dieselbe Fahrt mit VERTAUSCHTER Tick-Reihenfolge (Granate vor Sicherung) -
 *       die Sicherung kommt trotzdem zuerst. Das haelt re15_sicherung_fahrt_offen() fest: mit
 *       der Spiel-Reihenfolge (game_step_common.c) allein erzwingt schon re15_item_modal_active()
 *       die Folge, der Riegel waere dann blind fuer das Gate.
 *
 * Rueckgabe 0 = alles bestanden, sonst die Nummer der ersten gerissenen Pruefung.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_skeleton.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fehler = 0, erste = 0;
static void pruefe(int nr, const char *was, int ok_)
{
    printf("   [%s] %d. %s\n", ok_ ? "OK " : "FEHL", nr, was);
    if (!ok_) { fehler++; if (!erste) erste = nr; }
}

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

static uint32_t u32(const uint8_t *b) { return b[0] | (b[1] << 8) | (b[2] << 16) | ((uint32_t)b[3] << 24); }

/* ---------------------------------------------------------------- Modell --------------- */
static int waffe_tri(const re15_md1_tri_uv_t *u) { return u->v0 >= 200 && u->v1 >= 200 && u->v2 >= 200; }
static int waffe_quad(const re15_md1_quad_uv_t *u)
{ return u->v0 >= 200 && u->v1 >= 200 && u->v2 >= 200 && u->v3 >= 200; }

static int gleich_gedreht(const re15_md1_vertex_t *roh, const re15_md1_vertex_t *neu, int punkt)
{
    int x = roh->x, y = roh->y, z = roh->z;
    if (punkt) { y -= 190; z -= 32; }
    return neu->x == x && neu->y == -z && neu->z == y;
}

static void modell_pruefen(void)
{
    printf("\n== Modell: gen/granate_prop.inc gegen PLD/PL00W09.PLW ==\n");
    size_t n = 0;
    uint8_t *plw = datei("PLD/PL00W09.PLW", &n);
    if (!plw) { pruefe(1, "PL00W09.PLW lesbar", 0); return; }
    uint32_t dir = u32(plw), d2 = u32(plw + dir + 8), d3 = u32(plw + dir + 12);
    static re15_md1_t roh, neu;
    int msz = 0, tsz = 0;
    const uint8_t *mb = re15_granate_md1_bytes(&msz);
    const uint8_t *tb = re15_granate_tim_bytes(&tsz);
    int ok_roh = re15_md1_parse(plw + d2, (int)(d3 - d2), &roh) == 0 && roh.mesh_count == 1;
    int ok_neu = re15_md1_parse(mb, msz, &neu) == 0 && neu.mesh_count == 1;
    const re15_md1_mesh_t *r = &roh.meshes[0], *g = &neu.meshes[0];
    printf("   dir[2] @0x%X: %d Punkte, %d Dreiecke, %d Vierecke | Granate: %d Punkte, %d Dreiecke, %d Vierecke\n",
           d2, r->tri_vertex_count, r->triangle_count, r->quad_count,
           g->tri_vertex_count, g->triangle_count, g->quad_count);
    pruefe(1, "MD1: ein Mesh, 24 Punkte, 5 Dreiecke, 14 Vierecke",
           ok_roh && ok_neu && g->tri_vertex_count == 24 && g->quad_vertex_count == 24 &&
           g->triangle_count == 5 && g->quad_count == 14);

    int ok2 = ok_roh && ok_neu, k = 0;
    for (int i = 0; ok2 && i < r->triangle_count; i++) {
        if (!waffe_tri(&r->triangle_uvs[i])) continue;
        if (k >= g->triangle_count) { ok2 = 0; break; }
        const re15_md1_triangle_t *a = &r->triangles[i], *b = &g->triangles[k];
        const re15_md1_tri_uv_t *ua = &r->triangle_uvs[i], *ub = &g->triangle_uvs[k];
        ok2 &= gleich_gedreht(&r->tri_vertices[a->v0], &g->tri_vertices[b->v0], 1)
            && gleich_gedreht(&r->tri_vertices[a->v1], &g->tri_vertices[b->v1], 1)
            && gleich_gedreht(&r->tri_vertices[a->v2], &g->tri_vertices[b->v2], 1)
            && gleich_gedreht(&r->tri_normals[a->n0], &g->tri_normals[b->n0], 0)
            && gleich_gedreht(&r->tri_normals[a->n1], &g->tri_normals[b->n1], 0)
            && gleich_gedreht(&r->tri_normals[a->n2], &g->tri_normals[b->n2], 0)
            && ua->u0 == ub->u0 && ua->v0 == ub->v0 && ua->u1 == ub->u1 && ua->v1 == ub->v1
            && ua->u2 == ub->u2 && ua->v2 == ub->v2 && ua->clut == 0x7840 && ub->clut == 0x7800
            && ua->page == 0x0081 && ub->page == 0x0080;
        k++;
    }
    ok2 &= (k == g->triangle_count);
    k = 0;
    for (int i = 0; ok2 && i < r->quad_count; i++) {
        if (!waffe_quad(&r->quad_uvs[i])) continue;
        if (k >= g->quad_count) { ok2 = 0; break; }
        const re15_md1_quad_t *a = &r->quads[i], *b = &g->quads[k];
        const re15_md1_quad_uv_t *ua = &r->quad_uvs[i], *ub = &g->quad_uvs[k];
        ok2 &= gleich_gedreht(&r->quad_vertices[a->v0], &g->quad_vertices[b->v0], 1)
            && gleich_gedreht(&r->quad_vertices[a->v1], &g->quad_vertices[b->v1], 1)
            && gleich_gedreht(&r->quad_vertices[a->v2], &g->quad_vertices[b->v2], 1)
            && gleich_gedreht(&r->quad_vertices[a->v3], &g->quad_vertices[b->v3], 1)
            && gleich_gedreht(&r->quad_normals[a->n0], &g->quad_normals[b->n0], 0)
            && gleich_gedreht(&r->quad_normals[a->n3], &g->quad_normals[b->n3], 0)
            && ua->u0 == ub->u0 && ua->v0 == ub->v0 && ua->u3 == ub->u3 && ua->v3 == ub->v3
            && ua->clut == 0x7840 && ub->clut == 0x7800 && ua->page == 0x0081 && ub->page == 0x0080;
        k++;
    }
    ok2 &= (k == g->quad_count);
    pruefe(2, "jede Flaeche = Waffen-Flaeche aus dir[2] (gedreht, UV gleich, clut/page umgesetzt)", ok2);

    re15_tim_t tr, tn;
    int ok3 = re15_tim_parse(plw + d3, (int)(dir - d3), &tr) == 0 && re15_tim_parse(tb, tsz, &tn) == 0
           && tn.bpp == 8 && tn.width == 128 && tn.height == 256 && tr.width == 56 && tr.height == 32
           && tn.clut_entries >= 256 && tr.clut_entries >= 256
           && memcmp(tn.clut, tr.clut, 512) == 0 && tn.clut_x == 0 && tn.clut_y == 480;
    if (ok3) {
        const uint8_t *pr = (const uint8_t *)tr.pixels, *pn = (const uint8_t *)tn.pixels;
        for (int y = 0; y < 32 && ok3; y++)
            ok3 = memcmp(pn + (224 + y) * 128 + 72, pr + y * 56, 56) == 0;
    }
    pruefe(3, "TIM 128x256 8bpp, CLUT = dir[3]-CLUT, dir[3]-Bild bei (72,224)", ok3);
    free(plw);
}

/* ---------------------------------------------------------------- Sitz ----------------- */
/* Runde 32: die Granate liegt im LINKEN UNTEREN Fach von Prop 0 (vorher oben in der Kuppel).
 * Fach A aus den Bytes (ROOM1150.RDT Prop 0 MD1 @0x11E40; ROOM1151 dieselben Indizes):
 *   Boden Viereck 89 (y=-90), Decke Viereck 87 (y=-810), Waende Viereck 86 (z=96) / 88 (z=861);
 *   vorn offen x=2, Rueckwand x=-1258. Herleitung: include/re15_granate.h, Dossier
 *   analysis/befunde_runde32/hebetisch_faecher.md; volle Fach-Pruefung unit_r32_hebetisch_faecher. */
#define Q_BODEN_A 89
#define Q_DECKE_A 87
#define Q_WAND0_A 86
#define Q_WAND1_A 88

/* konstante Koordinate (achse 1 = y, 2 = z) eines Vierecks von Prop 0; 0x7fffffff = nicht konstant */
static int viereck_fest(const re15_md1_mesh_t *m, int qi, int achse)
{
    if (qi >= m->quad_count) return 0x7fffffff;
    const re15_md1_quad_t *q = &m->quads[qi];
    const uint16_t vi[4] = { q->v0, q->v1, q->v2, q->v3 };
    int w0 = achse == 1 ? m->quad_vertices[vi[0]].y : m->quad_vertices[vi[0]].z;
    for (int k = 1; k < 4; k++) {
        int w = achse == 1 ? m->quad_vertices[vi[k]].y : m->quad_vertices[vi[k]].z;
        if (w != w0) return 0x7fffffff;
    }
    return w0;
}

/* Weltpunkt eines Modellpunkts: Drehung wie main.c pc_prop_rot_q12 (Ry*Rx*Rz, hier nur Ry). */
static void welt(const float m[3], float w[3])
{
    int s = re15_sin_q12(RE15_GRANATE_ROT_Y), c = re15_cos_q12(RE15_GRANATE_ROT_Y);
    w[0] = RE15_GRANATE_POS_X + (m[0] * c + m[2] * s) / 4096.0f;
    w[1] = RE15_GRANATE_POS_Y + m[1];
    w[2] = RE15_GRANATE_POS_Z + (-m[0] * s + m[2] * c) / 4096.0f;
}

static void sitz_pruefen(const re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Sitz im linken unteren Fach ==\n", rid);
    static re15_md1_t g, p0;
    int msz = 0; const uint8_t *mb = re15_granate_md1_bytes(&msz);
    if (re15_md1_parse(mb, msz, &g) != 0 ||
        re15_md1_parse(rdt->prop_md1[0], rdt->prop_md1_size[0], &p0) != 0) {
        pruefe(4, "MD1 lesbar", 0); return;
    }
    const re15_md1_mesh_t *pm = &p0.meshes[0];
    int boden = viereck_fest(pm, Q_BODEN_A, 1), decke = viereck_fest(pm, Q_DECKE_A, 1);
    int wand0 = viereck_fest(pm, Q_WAND0_A, 2), wand1 = viereck_fest(pm, Q_WAND1_A, 2);
    const re15_md1_mesh_t *s = &g.meshes[0];
    float tief = -1e9f, hoch = 1e9f, abst = 1e9f, zmin = 1e9f, zmax = -1e9f, xmin = 1e9f, xmax = -1e9f;
    for (int art = 0; art < 2; art++) {
        int nf = art ? s->quad_count : s->triangle_count;
        for (int i = 0; i < nf; i++) {
            const re15_md1_vertex_t *e[4]; int ne;
            if (art) { const re15_md1_quad_t *q = &s->quads[i];   /* Z-Ordnung 0-1-3-2 = Umlauf */
                e[0] = &s->quad_vertices[q->v0]; e[1] = &s->quad_vertices[q->v1];
                e[2] = &s->quad_vertices[q->v3]; e[3] = &s->quad_vertices[q->v2]; ne = 4; }
            else { const re15_md1_triangle_t *t = &s->triangles[i];
                e[0] = &s->tri_vertices[t->v0]; e[1] = &s->tri_vertices[t->v1];
                e[2] = &s->tri_vertices[t->v2]; ne = 3; }
            for (int k = 0; k < ne; k++) {
                const re15_md1_vertex_t *a = e[k], *b = e[(k + 1) % ne];
                for (int j = 0; j <= 8; j++) {
                    float t = j / 8.0f, m[3] = { a->x + (b->x - a->x) * t, a->y + (b->y - a->y) * t,
                                                 a->z + (b->z - a->z) * t }, w[3];
                    welt(m, w);
                    if (w[1] > tief) tief = w[1];
                    if (w[1] < hoch) hoch = w[1];
                    if (w[2] < zmin) zmin = w[2];
                    if (w[2] > zmax) zmax = w[2];
                    if (w[0] < xmin) xmin = w[0];
                    if (w[0] > xmax) xmax = w[0];
                    {   /* Sicherungs-Zylinder um die gedrehte Modell-X-Achse */
                        int ss = re15_sin_q12(RE15_SICHERUNG_ROT_Y), sc = re15_cos_q12(RE15_SICHERUNG_ROT_Y);
                        float ax[3] = { sc / 4096.0f, 0.0f, -ss / 4096.0f };
                        float d3[3] = { w[0] - RE15_SICHERUNG_POS_X, w[1] - RE15_SICHERUNG_POS_Y,
                                        w[2] - RE15_SICHERUNG_POS_Z };
                        float t = d3[0] * ax[0] + d3[1] * ax[1] + d3[2] * ax[2];
                        float r = sqrtf(fmaxf(0.0f, d3[0] * d3[0] + d3[1] * d3[1] + d3[2] * d3[2] - t * t));
                        float d = fabsf(t) <= 203.0f ? r - 26.0f
                                                     : hypotf(fmaxf(0.0f, r - 26.0f), fabsf(t) - 203.0f);
                        if (d < abst) abst = d;
                    }
                }
            }
        }
    }
    printf("   Fach A: Boden y=%d (Viereck %d), Decke y=%d (Viereck %d), Waende z=%d/%d (Viereck %d/%d) | "
           "Granate y %.0f..%.0f x %.0f..%.0f z %.0f..%.0f, Abstand zur Sicherung %.2f\n",
           boden, Q_BODEN_A, decke, Q_DECKE_A, wand0, wand1, Q_WAND0_A, Q_WAND1_A,
           hoch, tief, xmin, xmax, zmin, zmax, abst);
    pruefe(4, "tiefster Punkt genau auf dem Boden des linken unteren Fachs (Viereck 89, y=-90)",
           boden == -90 && (int)lroundf(tief) == boden);
    pruefe(5, "Abstand zur Sicherung > 0 (kein Durchdringen)", abst > 0.0f);
    pruefe(6, "unter der Fachdecke (Viereck 87, y=-810) und zwischen Vorderkante x=2 und Rueckwand x=-1258",
           decke == -810 && hoch > (float)decke && xmax < 2.0f && xmin > -1258.0f);
    pruefe(7, "LINKS: ganz im linken Fach zwischen den Waenden z=96 / z=861 (Viereck 86/88), links der Sicherung",
           wand0 == 96 && wand1 == 861 && zmin > (float)wand0 && zmax < (float)wand1 &&
           zmax < (float)RE15_SICHERUNG_POS_Z);
}

/* ---------------------------------------------------------------- Fahrten -------------- */
static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}
static int sichtbar(uint8_t o) { int s = slot_von_obj(o); return s >= 0 && g_scd.props[s].active; }
static int menge_im_inventar(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

static void raum_frisch(re15_rdt_t *rdt, uint16_t rid, int granate_genommen)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    if (granate_genommen) re15_game_flag_set(9, RE15_GRANATE_TAKEN_BIT, 1);
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, -21000, -18500, 0);
}

enum { JA = 0, NEIN = 1 };
typedef struct { int n_si, n_gr, f_si, f_gr; long y_si, y_gr; int geparkt; } fahrt_t;

static fahrt_t fahrt_ex(int antwort_si, int antwort_gr, int granate_zuerst)
{
    fahrt_t r; memset(&r, 0, sizeof r); r.f_si = r.f_gr = -1;
    int aktuell = 0;     /* 1 = Sicherungs-Modal offen, 2 = Granaten-Modal offen */
    scd_event_fire(4);
    int im_raum = 0, park = -1;
    for (int f = 0; f < 1200; f++) {
        if (!re15_item_modal_active()) scd_vm_tick();              /* Freeze @0x8001cdec */
        int p = slot_von_obj(0);
        long py = p >= 0 ? (long)g_scd.props[p].y : 0;
        for (int w = 0; w < 2; w++) {                               /* game_step_common.c: */
            int gr_jetzt = granate_zuerst ? (w == 0) : (w == 1);    /* Sicherung, dann Granate */
            if (!gr_jetzt && re15_sicherung_tick()) {
                r.n_si++; aktuell = 1; if (r.f_si < 0) { r.f_si = f; r.y_si = py; }
                printf("   MESS Bild %3d: Sicherungs-Modal auf, y=%ld\n", f, py);
            }
            if (gr_jetzt && re15_granate_tick()) {
                r.n_gr++; aktuell = 2; if (r.f_gr < 0) { r.f_gr = f; r.y_gr = py; }
                printf("   MESS Bild %3d: Granaten-Modal auf, y=%ld\n", f, py);
            }
        }
        if (re15_item_modal_active()) {                             /* main.c nach dem Step */
            uint16_t edge = 0; uint8_t typ = 0; int wahl = 0;
            int art = re15_item_modal_prompt(&typ, &wahl);
            int antwort = aktuell == 2 ? antwort_gr : antwort_si;
            if (re15_item_modal_prompt_ready() && art == 1)
                edge = (antwort == NEIN && wahl == 0) ? 0x1000 : 0x4000;
            else if (re15_item_modal_prompt_ready() && art == 2)
                edge = 0x4000;
            re15_item_modal_tick(edge, edge);
        }
        if (py > -5000) im_raum = 1;
        else if (im_raum && park < 0) park = f;
        if (park >= 0 && f > park + 20 && !re15_item_modal_active()) break;
    }
    r.geparkt = park >= 0;
    printf("   MESS Fahrt: Sicherungs-Modale %d (Bild %d y=%ld), Granaten-Modale %d (Bild %d y=%ld), geparkt %d\n",
           r.n_si, r.f_si, r.y_si, r.n_gr, r.f_gr, r.y_gr, r.geparkt);
    return r;
}

static fahrt_t fahrt(int a, int b) { return fahrt_ex(a, b, 0); }

/* Runde 31: die Aufnahmen gehen erst in der RUHE OBEN auf (sub04 im Sleep 30 @0x101A, Plattform
 * auf -1205, include/re15_hebetisch.h) — nicht mehr im Fenster (-5000,-1100] mitten im Hub. */
static int oben(long y) { return y == -1205; }

static void fahrten_pruefen(re15_rdt_t *rdt, uint16_t rid)
{
    const int G = RE15_GRANATE_TAKEN_BIT, S = RE15_SICHERUNG_TAKEN_BIT;
    printf("\n== %04X Fall A: No/No, dann Yes/Yes ==\n", rid);
    raum_frisch(rdt, rid, 0);
    fahrt_t a1 = fahrt(NEIN, NEIN);
    pruefe(8, "Fahrt 1: 1 Sicherungs- + 1 Granaten-Modal, Granate danach, beide oben; No/No laesst alles liegen",
           a1.n_si == 1 && a1.n_gr == 1 && a1.f_gr > a1.f_si && oben(a1.y_si) && oben(a1.y_gr) &&
           a1.geparkt && !re15_game_flag_get(9, S) && !re15_game_flag_get(9, G) &&
           sichtbar(RE15_SICHERUNG_OBJ_ID) && sichtbar(RE15_GRANATE_OBJ_ID) &&
           menge_im_inventar(RE15_SICHERUNG_ITEM) == 0 && menge_im_inventar(RE15_GRANATE_ITEM) == 0);
    fahrt_t a2 = fahrt(JA, JA);
    printf("   nach Fahrt 2: Flag(9,%d)=%d Flag(9,%d)=%d, Item 0x40 x%d, Item 0x09 x%d\n",
           S, re15_game_flag_get(9, S), G, re15_game_flag_get(9, G),
           menge_im_inventar(RE15_SICHERUNG_ITEM), menge_im_inventar(RE15_GRANATE_ITEM));
    pruefe(9, "Fahrt 2: wieder 1+1 in derselben Folge; Yes/Yes -> beide Flags, Props weg, beide im Inventar",
           a2.n_si == 1 && a2.n_gr == 1 && a2.f_gr > a2.f_si &&
           re15_game_flag_get(9, S) && re15_game_flag_get(9, G) &&
           !sichtbar(RE15_SICHERUNG_OBJ_ID) && !sichtbar(RE15_GRANATE_OBJ_ID) &&
           menge_im_inventar(RE15_SICHERUNG_ITEM) == 1 &&
           menge_im_inventar(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);
    fahrt_t a3 = fahrt(JA, JA);
    pruefe(10, "Fahrt 3: kein Modal, nichts doppelt", a3.n_si == 0 && a3.n_gr == 0 &&
           menge_im_inventar(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);

    printf("\n== %04X Fall B: Sicherung Yes, Granate No ==\n", rid);
    raum_frisch(rdt, rid, 0);
    fahrt_t b1 = fahrt(JA, NEIN);
    fahrt_t b2 = fahrt(JA, JA);
    pruefe(11, "nur (9,53) nach Fahrt 1, Granate liegt; Fahrt 2 NUR das Granaten-Modal, dann genommen",
           b1.n_si == 1 && b1.n_gr == 1 && b2.n_si == 0 && b2.n_gr == 1 &&
           re15_game_flag_get(9, S) && re15_game_flag_get(9, G) && !sichtbar(RE15_GRANATE_OBJ_ID) &&
           menge_im_inventar(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);

    printf("\n== %04X Fall C: Sicherung No, Granate Yes ==\n", rid);
    raum_frisch(rdt, rid, 0);
    fahrt_t c1 = fahrt(NEIN, JA);
    int c_flags = !re15_game_flag_get(9, S) && re15_game_flag_get(9, G) &&
                  sichtbar(RE15_SICHERUNG_OBJ_ID) && !sichtbar(RE15_GRANATE_OBJ_ID);
    fahrt_t c2 = fahrt(JA, JA);
    pruefe(12, "nur (9,56) nach Fahrt 1, Granate weg; Fahrt 2 NUR das Sicherungs-Modal",
           c1.n_si == 1 && c1.n_gr == 1 && c_flags && c2.n_si == 1 && c2.n_gr == 0 &&
           re15_game_flag_get(9, S) && menge_im_inventar(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);

    printf("\n== %04X Fall D: Granate vor dem Raumstart genommen (Negativ-Kontrolle) ==\n", rid);
    raum_frisch(rdt, rid, 1);
    int im_pool = slot_von_obj(RE15_GRANATE_OBJ_ID) >= 0;
    fahrt_t d1 = fahrt(JA, JA);
    pruefe(13, "kein Granaten-Prop im Pool, die Fahrt oeffnet nur das Sicherungs-Modal",
           !im_pool && d1.n_si == 1 && d1.n_gr == 0 && menge_im_inventar(RE15_GRANATE_ITEM) == 0);

    printf("\n== %04X Fall E: Tick-Reihenfolge vertauscht (Granate zuerst) ==\n", rid);
    raum_frisch(rdt, rid, 0);
    fahrt_t e1 = fahrt_ex(JA, JA, 1);
    pruefe(14, "die Sicherung kommt trotzdem zuerst (re15_sicherung_fahrt_offen haelt die Granate)",
           e1.n_si == 1 && e1.n_gr == 1 && e1.f_si >= 0 && e1.f_gr > e1.f_si);
}

static int ein_raum(uint16_t rid, const char *rel)
{
    size_t n = 0;
    uint8_t *buf = datei(rel, &n);
    if (!buf) return 77;
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, n, &rdt) != 0) { free(buf); return 1; }
    sitz_pruefen(&rdt, rid);
    fahrten_pruefen(&rdt, rid);
    free(buf);
    return 0;
}

int main(void)
{
    modell_pruefen();
    if (ein_raum(0x1150, "STAGE1/ROOM1150.RDT") == 77) return 77;
    if (ein_raum(0x1151, "STAGE1/ROOM1151.RDT") == 77) return 77;
    printf("\n%s — %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? erste : 0;
}
