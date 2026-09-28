/* probe_r31_hebetisch.c — RIEGEL Runde 31 / H: Hebetisch in Irons' Buero (ROOM1150/1151).
 *
 * Auftrag (analysis/befunde_runde31/AUFTRAG.md): "packe mir die Granate links in die hochfahrende
 * Box und die Sicherung rechts. Ausserdem starte mit dem Aufnahme Dialog der items erst wenn das
 * Modell wirklich komplett hochgefahren ist." Dossier analysis/befunde_runde31/hebetisch.md.
 *
 * PRUEFUNGEN (je ROOM1150.RDT und ROOM1151.RDT):
 *  Ruhe-Fenster (include/re15_hebetisch.h)
 *    1  das Fenster liegt bei [0x101A,0x1042) (1150) bzw. [0x0FF8,0x1020) (1151); an seinem Beginn
 *       steht Sleep 30 `09 0a 1e 00`, 10 Byte davor der Setzen-For `0d 00 04 00 0a 00`, an seinem
 *       Ende der Abfahrt-For `0d 00 04 00 5a 00`
 *  Sitz (Plattform-Koordinaten, Engine-Trig re15_sin_q12/re15_cos_q12, Kanten in 8 Stuecke):
 *    2  beide liegen AUF dem Fachboden: tiefster Punkt genau y = -1036 (Prop-0-Achteck)
 *    3  Luft unter der GESCHLOSSENEN Kuppel > 0 (Prop 1/2 wie ausgeliefert) — beide
 *    4  Luft unter den OFFENEN Deckeln > 0 (Deckelweg aus For @0x0FC0 Zaehler x Speed_set) — beide
 *    5  Grundriss im Achteck des Fachbodens — beide
 *    6  kein Durchdringen: jede Granaten-Probe ausserhalb des Sicherungs-Zylinders (r 26 um die
 *       GEDREHTE Laengsachse, +-203)
 *    7  Cut 4 (re15_camera_build_view, Plattform y=-305 und y=-1205): Schirm-Schwerpunkt der
 *       Granate mindestens 15 px LINKS von dem der Sicherung, und die Granate liegt ganz links
 *       vom Sicherungs-Schwerpunkt (Rechnung; GEMESSEN im Framedump: Dossier §1.3)
 *  Zeitpunkt (Bildschleife in der Reihenfolge des Spiels: SCD-Tick nur ohne offene Aufnahme,
 *  dann Sicherung-, dann Granaten-Tick, dann Aufnahme-Tick):
 *    8  die Sicherungs-Aufnahme geht im ERSTEN Ruhebild auf: y = -1205, im Bild davor -1205,
 *       zwei Bilder davor -1206 (letztes Add_speed des Setzens), sub04-PC = Fensterbeginn + 1
 *    9  die Granaten-Aufnahme geht danach auf, ebenfalls in der Ruhe (y = -1205, PC im Fenster)
 *   10  in JEDEM Bild mit offener Aufnahme steht die Plattform auf -1205
 *   11  Ruhebilder ohne offene Aufnahme = 40 = Sleep 30 @0x101A + Sleep 10 @0x102E: die Dialoge
 *       verbrauchen keinen Skript-Takt, die Abfahrt kommt erst danach
 *   12  Yes/Yes: Flags (9,53)/(9,56), beide Props weg, beide im Inventar
 *  Negativ-Kontrolle
 *   13  ein anderer Raum (ROOM1140) traegt kein Ruhe-Fenster, re15_hebetisch_ruht_oben() = 0
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
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_hebetisch.h"
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

/* ------------------------------------------------------------ Raum ------------------------ */
static void raum_frisch(re15_rdt_t *rdt, uint16_t rid)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
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

static void fenster_pruefen(const re15_rdt_t *rdt, uint16_t rid)
{
    long von = re15_hebetisch_fenster_von(), bis = re15_hebetisch_fenster_bis();
    long soll_von = rid == 0x1150 ? 0x101A : 0x0FF8, soll_bis = rid == 0x1150 ? 0x1042 : 0x1020;
    static const uint8_t sleep30[4] = { 0x09, 0x0a, 0x1e, 0x00 };
    static const uint8_t setzen[6]  = { 0x0d, 0x00, 0x04, 0x00, 0x0a, 0x00 };
    static const uint8_t abfahrt[6] = { 0x0d, 0x00, 0x04, 0x00, 0x5a, 0x00 };
    int ok = von == soll_von && bis == soll_bis && bis + 6 <= rdt->raw_size &&
             memcmp(rdt->raw + von, sleep30, 4) == 0 && memcmp(rdt->raw + von - 10, setzen, 6) == 0 &&
             memcmp(rdt->raw + bis, abfahrt, 6) == 0;
    printf("   Ruhe-Fenster [0x%04lX, 0x%04lX)\n", von, bis);
    pruefe(1, "Ruhe-Fenster = [Sleep 30, For-Abfahrt), 10 Byte davor der Setzen-For", ok);
}

/* ------------------------------------------------------------ Sitz ------------------------ */
typedef struct { float a[3], b[3], c[3]; } dreieck_t;

static int kuppel_lesen(const re15_rdt_t *rdt, dreieck_t *d, int max, float dz1, float dz2)
{
    int n = 0;
    for (int op = 1; op <= 2; op++) {
        static re15_md1_t m;
        float dz = op == 1 ? dz1 : dz2;
        if (re15_md1_parse(rdt->prop_md1[op], rdt->prop_md1_size[op], &m) != 0) return -1;
        const re15_md1_mesh_t *s = &m.meshes[0];
        for (int i = 0; i < s->triangle_count && n < max; i++) {
            const re15_md1_triangle_t *t = &s->triangles[i];
            const re15_md1_vertex_t *v[3] = { &s->tri_vertices[t->v0], &s->tri_vertices[t->v1], &s->tri_vertices[t->v2] };
            for (int k = 0; k < 3; k++) { float *p = k == 0 ? d[n].a : k == 1 ? d[n].b : d[n].c;
                p[0] = v[k]->x; p[1] = v[k]->y; p[2] = v[k]->z + dz; }
            n++;
        }
        for (int i = 0; i < s->quad_count && n + 1 < max; i++) {
            const re15_md1_quad_t *q = &s->quads[i];
            const re15_md1_vertex_t *v[4] = { &s->quad_vertices[q->v0], &s->quad_vertices[q->v1],
                                              &s->quad_vertices[q->v2], &s->quad_vertices[q->v3] };
            static const int idx[2][3] = { {0, 1, 2}, {1, 3, 2} };
            for (int h = 0; h < 2; h++) {
                for (int k = 0; k < 3; k++) { float *p = k == 0 ? d[n].a : k == 1 ? d[n].b : d[n].c;
                    p[0] = v[idx[h][k]]->x; p[1] = v[idx[h][k]]->y; p[2] = v[idx[h][k]]->z + dz; }
                n++;
            }
        }
    }
    return n;
}

/* kleinste Hoehe (ueber dem Boden -1036) der Kuppel-Unterseite ueber (x,z); 1e9 = keine Flaeche */
static float kuppel_hoehe(const dreieck_t *d, int n, float x, float z)
{
    float best = 1e9f;
    for (int i = 0; i < n; i++) {
        const float *a = d[i].a, *b = d[i].b, *c = d[i].c;
        float den = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2]);
        if (den == 0) continue;
        float l1 = ((b[2] - c[2]) * (x - c[0]) + (c[0] - b[0]) * (z - c[2])) / den;
        float l2 = ((c[2] - a[2]) * (x - c[0]) + (a[0] - c[0]) * (z - c[2])) / den;
        float l3 = 1 - l1 - l2;
        if (l1 < -1e-6f || l2 < -1e-6f || l3 < -1e-6f) continue;
        float h = -1036.0f - (l1 * a[1] + l2 * b[1] + l3 * c[1]);
        if (h > 0.5f && h < best) best = h;
    }
    return best;
}

/* Achteck des Fachbodens (Prop 0 Punkte @0x121AC..@0x1221C), gegen den Uhrzeigersinn in (x,z) */
static int im_achteck(float x, float z)
{
    static const float p[8][2] = { {-485, 1260}, {-432, 958}, {-280, 875}, {-128, 958},
                                   {-74, 1260}, {-128, 1562}, {-280, 1645}, {-432, 1562} };
    int vz = 0;
    for (int i = 0; i < 8; i++) {
        const float *a = p[i], *b = p[(i + 1) % 8];
        float c = (b[0] - a[0]) * (z - a[1]) - (b[1] - a[1]) * (x - a[0]);
        int s = c > 0.01f ? 1 : c < -0.01f ? -1 : 0;
        if (s && vz && s != vz) return 0;
        if (s) vz = s;
    }
    return 1;
}

/* Proben eines Props in Plattform-Koordinaten: Punkte auf allen Kanten (8 Stuecke) + Flaechenmitten.
 * Drehung wie main.c pc_prop_rot_q12 (Ry*Rx*Rz, hier nur Ry, Engine-Trig). */
#define MAX_PROBEN 4000
static int proben(const uint8_t *md1, int sz, int px, int py, int pz, int ry, float (*out)[3])
{
    static re15_md1_t m;
    if (re15_md1_parse(md1, sz, &m) != 0) return 0;
    const re15_md1_mesh_t *s = &m.meshes[0];
    int sn = re15_sin_q12(ry), cs = re15_cos_q12(ry), n = 0;
    for (int art = 0; art < 2; art++) {
        int nf = art ? s->quad_count : s->triangle_count;
        for (int i = 0; i < nf; i++) {
            const re15_md1_vertex_t *e[4]; int ne;
            if (art) { const re15_md1_quad_t *q = &s->quads[i];     /* Z-Ordnung 0-1-3-2 = Umlauf */
                e[0] = &s->quad_vertices[q->v0]; e[1] = &s->quad_vertices[q->v1];
                e[2] = &s->quad_vertices[q->v3]; e[3] = &s->quad_vertices[q->v2]; ne = 4; }
            else { const re15_md1_triangle_t *t = &s->triangles[i];
                e[0] = &s->tri_vertices[t->v0]; e[1] = &s->tri_vertices[t->v1];
                e[2] = &s->tri_vertices[t->v2]; ne = 3; }
            float mitte[3] = {0, 0, 0};
            for (int k = 0; k < ne; k++) {
                mitte[0] += e[k]->x / (float)ne; mitte[1] += e[k]->y / (float)ne; mitte[2] += e[k]->z / (float)ne;
                const re15_md1_vertex_t *a = e[k], *b = e[(k + 1) % ne];
                for (int j = 0; j < 8 && n < MAX_PROBEN - 1; j++) {
                    float t = j / 8.0f, v[3] = { a->x + (b->x - a->x) * t, a->y + (b->y - a->y) * t,
                                                 a->z + (b->z - a->z) * t };
                    out[n][0] = px + (v[0] * cs + v[2] * sn) / 4096.0f;
                    out[n][1] = py + v[1];
                    out[n][2] = pz + (-v[0] * sn + v[2] * cs) / 4096.0f;
                    n++;
                }
            }
            out[n][0] = px + (mitte[0] * cs + mitte[2] * sn) / 4096.0f;
            out[n][1] = py + mitte[1];
            out[n][2] = pz + (-mitte[0] * sn + mitte[2] * cs) / 4096.0f;
            n++;
        }
    }
    return n;
}

static float S_[MAX_PROBEN][3], G_[MAX_PROBEN][3];

static void schirm(const re15_camera_view_t *v, int plat_y, float (*p)[3], int n, float *mx, float *xmin, float *xmax)
{
    double s = 0; int k = 0; *xmin = 1e9f; *xmax = -1e9f;
    for (int i = 0; i < n; i++) {
        /* Plattform rot_y 2048 (main00 Obj_model_set @0x0E00 `00 08`): x' = -x, z' = -z */
        float w[3] = { -20700.0f - p[i][0], (float)plat_y + p[i][1], -17460.0f - p[i][2] };
        float c[3];
        for (int r = 0; r < 3; r++)
            c[r] = (v->rot[r * 3] * w[0] + v->rot[r * 3 + 1] * w[1] + v->rot[r * 3 + 2] * w[2]) / 4096.0f + v->trans[r];
        if (c[2] < 64) continue;
        float sx = 160.0f + c[0] * v->fov_screen_dist / c[2];
        s += sx; k++;
        if (sx < *xmin) *xmin = sx;
        if (sx > *xmax) *xmax = sx;
    }
    *mx = k ? (float)(s / k) : 0;
}

static void sitz_pruefen(const re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Sitz (Sicherung (%d,%d,%d) rot_y %d, Granate (%d,%d,%d) rot_y %d) ==\n", rid,
           RE15_SICHERUNG_POS_X, RE15_SICHERUNG_POS_Y, RE15_SICHERUNG_POS_Z, RE15_SICHERUNG_ROT_Y,
           RE15_GRANATE_POS_X, RE15_GRANATE_POS_Y, RE15_GRANATE_POS_Z, RE15_GRANATE_ROT_Y);
    int ssz = 0, gsz = 0;
    const uint8_t *smd = re15_sicherung_md1_bytes(&ssz), *gmd = re15_granate_md1_bytes(&gsz);
    int ns = proben(smd, ssz, RE15_SICHERUNG_POS_X, RE15_SICHERUNG_POS_Y, RE15_SICHERUNG_POS_Z, RE15_SICHERUNG_ROT_Y, S_);
    int ng = proben(gmd, gsz, RE15_GRANATE_POS_X, RE15_GRANATE_POS_Y, RE15_GRANATE_POS_Z, RE15_GRANATE_ROT_Y, G_);

    /* Deckelweg aus den Skript-Bytes: For @0x0FC0 (1150) / @0x0F9E (1151) `0d 00 18 00 0f 00`,
     * Zaehler = drittes Feld (@0x8003f568 `lhu a1,4(t0)`), Speed_set +10 10 Byte dahinter. */
    long for_off = rid == 0x1150 ? 0x0FC0 : 0x0F9E;
    const uint8_t *fr = rdt->raw + for_off;
    int weg = (fr[0] == 0x0d && fr[2] == 0x18) ? (int)(fr[4] | fr[5] << 8) * (int16_t)(fr[12] | fr[13] << 8) : 0;
    static dreieck_t zu[64], offen[64];
    int nz = kuppel_lesen(rdt, zu, 64, 0, 0), no = kuppel_lesen(rdt, offen, 64, (float)weg, (float)-weg);

    float tief_s = -1e9f, tief_g = -1e9f, lz_s = 1e9f, lz_g = 1e9f, lo_s = 1e9f, lo_g = 1e9f;
    int raus = 0;
    for (int pass = 0; pass < 2; pass++) {
        float (*P)[3] = pass ? G_ : S_; int n = pass ? ng : ns;
        float *tief = pass ? &tief_g : &tief_s, *lz = pass ? &lz_g : &lz_s, *lo = pass ? &lo_g : &lo_s;
        for (int i = 0; i < n; i++) {
            float h = -1036.0f - P[i][1];
            if (P[i][1] > *tief) *tief = P[i][1];
            float a = kuppel_hoehe(zu, nz, P[i][0], P[i][2]) - h;
            if (a < *lz) *lz = a;
            if (!(P[i][2] > 1260 - weg && P[i][2] < 1260 + weg)) {
                float b = kuppel_hoehe(offen, no, P[i][0], P[i][2]) - h;
                if (b < *lo) *lo = b;
            }
            if (!im_achteck(P[i][0], P[i][2])) raus++;
        }
    }
    /* Granate gegen den Sicherungs-Zylinder um die GEDREHTE Achse (Modell-X, Laenge +-203, r 26) */
    int sn = re15_sin_q12(RE15_SICHERUNG_ROT_Y), cs = re15_cos_q12(RE15_SICHERUNG_ROT_Y);
    float ax[3] = { cs / 4096.0f, 0, -sn / 4096.0f }, abst = 1e9f;
    for (int i = 0; i < ng; i++) {
        float d[3] = { G_[i][0] - RE15_SICHERUNG_POS_X, G_[i][1] - RE15_SICHERUNG_POS_Y, G_[i][2] - RE15_SICHERUNG_POS_Z };
        float t = d[0] * ax[0] + d[1] * ax[1] + d[2] * ax[2];
        float r = sqrtf(fmaxf(0.0f, d[0] * d[0] + d[1] * d[1] + d[2] * d[2] - t * t));
        float a = fabsf(t) <= 203.0f ? r - 26.0f : hypotf(fmaxf(0.0f, r - 26.0f), fabsf(t) - 203.0f);
        if (a < abst) abst = a;
    }
    printf("   Proben S %d / G %d, Deckelweg %d | tiefster Punkt S y=%.1f G y=%.1f | Luft zu S %.2f G %.2f | "
           "Luft offen S %.2f G %.2f | ausserhalb Achteck %d | Abstand G->S-Mantel %.2f\n",
           ns, ng, weg, tief_s, tief_g, lz_s, lz_g, lo_s, lo_g, raus, abst);
    pruefe(2, "beide liegen AUF dem Fachboden (tiefster Punkt y = -1036)",
           (int)lroundf(tief_s) == -1036 && (int)lroundf(tief_g) == -1036);
    pruefe(3, "Luft unter der GESCHLOSSENEN Kuppel > 0 (beide)", nz > 0 && lz_s > 0 && lz_g > 0);
    pruefe(4, "Luft unter den OFFENEN Deckeln > 0 (beide, Deckelweg 150)", weg == 150 && lo_s > 0 && lo_g > 0);
    pruefe(5, "Grundriss ganz im Achteck des Fachbodens (beide)", raus == 0);
    pruefe(6, "kein Durchdringen Granate <-> Sicherung (Abstand zum Mantel > 0)", abst > 0.0f);

    re15_camera_view_t v;
    int ok7 = rdt->cut_count > 4 && re15_camera_build_view(&rdt->cuts[4], &v) == 0;
    for (int k = 0; ok7 && k < 2; k++) {
        int py = k ? -1205 : -305;
        float ms, mg, s0, s1, g0, g1;
        schirm(&v, py, S_, ns, &ms, &s0, &s1);
        schirm(&v, py, G_, ng, &mg, &g0, &g1);
        printf("   Cut 4, Plattform y=%d: Schirm-x Granate %.1f (%.0f..%.0f)  Sicherung %.1f (%.0f..%.0f)\n",
               py, mg, g0, g1, ms, s0, s1);
        ok7 = ms - mg >= 15.0f && g1 < ms;
    }
    pruefe(7, "Cut 4: Granate LINKS, Sicherung RECHTS (Schwerpunkte >= 15 px, Granate ganz links vom S-Schwerpunkt)", ok7);
}

/* ------------------------------------------------------------ Zeitpunkt ------------------- */
static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}
static int menge(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

static void zeit_pruefen(re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Zeitpunkt der Aufnahmen (Yes/Yes) ==\n", rid);
    raum_frisch(rdt, rid);
    long von = re15_hebetisch_fenster_von();
    long y[1400]; memset(y, 0, sizeof y);
    int f_si = -1, f_gr = -1, pc_si = -1, pc_gr = -1, modal_falsch = 0, modal_bilder = 0;
    int ruhe_ohne = 0, abfahrt = -1, aktuell = 0;
    scd_event_fire(4);
    for (int f = 0; f < 1400; f++) {
        int skript = !re15_item_modal_active();
        if (skript) scd_vm_tick();                                   /* main.c Freeze-Zweig */
        int p = slot_von_obj(0);
        y[f] = p >= 0 ? (long)g_scd.props[p].y : 0;
        if (skript && re15_hebetisch_ruht_oben()) ruhe_ohne++;
        if (re15_sicherung_tick()) { f_si = f; pc_si = (int)re15_hebetisch_ruhe_pc_off(); aktuell = 1; }
        if (re15_granate_tick())   { f_gr = f; pc_gr = (int)re15_hebetisch_ruhe_pc_off(); aktuell = 2; }
        if (re15_item_modal_active()) {
            modal_bilder++;
            if (y[f] != -1205) modal_falsch++;
            uint16_t edge = 0; uint8_t typ = 0; int wahl = 0;
            int art = re15_item_modal_prompt(&typ, &wahl);
            if (re15_item_modal_prompt_ready() && art >= 1) edge = 0x4000;   /* Yes */
            re15_item_modal_tick(edge, edge);
        }
        if (f_gr >= 0 && abfahrt < 0 && !re15_item_modal_active() && y[f] > -1205 && y[f] < -300) abfahrt = f;
        if (abfahrt >= 0 && f > abfahrt + 5) break;
    }
    (void)aktuell;
    printf("   MESS Sicherung Bild %d (y %ld, davor %ld, %ld) PC 0x%04X | Granate Bild %d (y %ld) PC 0x%04X | "
           "Bilder mit Aufnahme %d, davon y != -1205: %d | Ruhebilder ohne Aufnahme %d | Abfahrt Bild %d\n",
           f_si, f_si >= 0 ? y[f_si] : 0, f_si >= 1 ? y[f_si - 1] : 0, f_si >= 2 ? y[f_si - 2] : 0, pc_si,
           f_gr, f_gr >= 0 ? y[f_gr] : 0, pc_gr, modal_bilder, modal_falsch, ruhe_ohne, abfahrt);
    pruefe(8, "Sicherungs-Aufnahme im ERSTEN Ruhebild (-1206 -> -1205 -> -1205, PC = Sleep 30 + 1)",
           f_si >= 2 && y[f_si] == -1205 && y[f_si - 1] == -1205 && y[f_si - 2] == -1206 && pc_si == von + 1);
    pruefe(9, "Granaten-Aufnahme danach, ebenfalls in der Ruhe (y -1205, PC im Fenster)",
           f_gr > f_si && y[f_gr] == -1205 && pc_gr >= von && pc_gr < re15_hebetisch_fenster_bis());
    pruefe(10, "in JEDEM Bild mit offener Aufnahme steht die Plattform auf -1205",
           modal_bilder > 0 && modal_falsch == 0);
    pruefe(11, "40 Ruhebilder ohne Aufnahme (Sleep 30 @0x101A + Sleep 10 @0x102E), erst dann die Abfahrt",
           ruhe_ohne == 40 && abfahrt > f_gr);
    pruefe(12, "Yes/Yes: beide Flags, beide Props weg, beide im Inventar",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) && re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT) &&
           !g_scd.props[slot_von_obj(RE15_SICHERUNG_OBJ_ID)].active &&
           !g_scd.props[slot_von_obj(RE15_GRANATE_OBJ_ID)].active &&
           menge(RE15_SICHERUNG_ITEM) == 1 && menge(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);
}

static int ein_raum(uint16_t rid, const char *rel)
{
    size_t n = 0;
    uint8_t *buf = datei(rel, &n);
    if (!buf) return 77;
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, n, &rdt) != 0) { free(buf); return 1; }
    printf("\n== %04X Ruhe-Fenster ==\n", rid);
    raum_frisch(&rdt, rid);
    fenster_pruefen(&rdt, rid);
    sitz_pruefen(&rdt, rid);
    zeit_pruefen(&rdt, rid);
    free(buf);
    return 0;
}

int main(void)
{
    if (ein_raum(0x1150, "STAGE1/ROOM1150.RDT") == 77) return 77;
    if (ein_raum(0x1151, "STAGE1/ROOM1151.RDT") == 77) return 77;
    {   /* Negativ-Kontrolle: ein anderer Raum traegt kein Ruhe-Fenster */
        size_t n = 0;
        uint8_t *buf = datei("STAGE1/ROOM1140.RDT", &n);
        if (!buf) return 77;
        static re15_rdt_t rdt;
        memset(&rdt, 0, sizeof rdt);
        int ok = re15_rdt_parse(buf, n, &rdt) == 0;
        if (ok) { raum_frisch(&rdt, 0x1140); for (int f = 0; f < 60; f++) scd_vm_tick(); }
        printf("\n== 1140 Negativ-Kontrolle: Fenster %ld..%ld, ruht_oben %d ==\n",
               re15_hebetisch_fenster_von(), re15_hebetisch_fenster_bis(), re15_hebetisch_ruht_oben());
        pruefe(13, "ROOM1140: kein Ruhe-Fenster, ruht_oben = 0",
               ok && re15_hebetisch_fenster_von() < 0 && re15_hebetisch_fenster_bis() < 0 && !re15_hebetisch_ruht_oben());
        free(buf);
    }
    printf("\n%s — %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? erste : 0;
}
