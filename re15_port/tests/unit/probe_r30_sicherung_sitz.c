/* probe_r30_sicherung_sitz.c — RIEGEL fuer Sitz und Drehung der Sicherung im Hebetisch
 * (Runde 30, Thema H). Dossier: analysis/befunde_runde30/sicherung.md §3.6, §5.3.
 *
 * ⛔ Sitz und Drehung sind PORT-WAHL, KEINE ORIGINAL-ADRESSE — das Original hat im
 * Hebetisch keinen Gegenstand. Die vier Konstanten in include/re15_sicherung.h sind aus
 * der AUSGELIEFERTEN Geometrie des Tisches abgeleitet. Dieser Riegel liest genau diese
 * Geometrie aus ROOM1150.RDT UND ROOM1151.RDT und faellt, wenn jemand den Sitz
 * verschiebt, ohne die Geometrie anzusehen:
 *
 * Runde 32 (Nutzer: "sie sollen unten, in den hochfahrenden Fach liegen - ein item links ein
 * item rechts"): die Sicherung liegt im RECHTEN UNTEREN Fach von Prop 0, nicht mehr in der Kuppel.
 *   1. Der Boden des rechten Fachs = Viereck 93 von Prop 0 (Face-Record @Datei 0x12F20 in
 *      ROOM1150.RDT): vier Punkte auf EINER Hoehe, von der offenen Vorderkante x=2 bis zur
 *      Rueckwand x=-1258, zwischen den Waenden Viereck 90 (z=950) und 92 (z=1715).
 *   2. POS_X / POS_Z = Mitte dieses Bodens (+-1);  POS_Y = Bodenhoehe - Rohrradius, der
 *      Rohrradius = groesstes y des Sicherungs-MD1.
 *   3. Das Fach liegt RECHTS der Trennwand zum linken Fach (dessen Wand Viereck 88, z=861;
 *      Plattform +z = Schirm rechts in Cut 4): z0 des Fachs > 861.
 *   4. ROT_Y legt die Laengsachse X des Modells auf z = parallel zur offenen Vorderseite (quer im
 *      Bild).
 *   5. Die Sicherung passt in das Fach (Laenge <= Fachbreite in z, Dicke <= Fachtiefe in x).
 *      Die volle Fach-Pruefung (Durchstoss, Sichtlinien, links/rechts) haelt
 *      unit_r32_hebetisch_faecher fest.
 *   6. Der Deckelweg ist 15 x 10 = 150 (For-Record `0d 00 18 00 0f 00`: Zaehler = drittes
 *      Feld, For-Handler @0x8003f540 `lhu a1,4(t0)` @0x8003f568) — NICHT 240. (Seit Runde 32
 *      nur noch Byte-Beleg der Fahrt; die Sicherung liegt nicht mehr unter den Deckeln.)
 *   7. Im Spiel (Raumstart ueber scd_room_reenter) traegt das Prop genau diese Werte.
 *
 * Rueckgabe 0 = alles bestanden.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_sicherung.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fehler = 0;

static void pruefe(const char *was, int ok)
{
    printf("   [%s] %s\n", ok ? "OK  " : "FEHL", was);
    if (!ok) fehler++;
}

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static void bbox(const re15_md1_mesh_t *m, int lo[3], int hi[3])
{
    for (int k = 0; k < 3; k++) { lo[k] = 0x7fffffff; hi[k] = -0x7fffffff; }
    for (int i = 0; i < (int)m->quad_vertex_count; i++) {
        int p[3] = { m->quad_vertices[i].x, m->quad_vertices[i].y, m->quad_vertices[i].z };
        for (int k = 0; k < 3; k++) { if (p[k] < lo[k]) lo[k] = p[k]; if (p[k] > hi[k]) hi[k] = p[k]; }
    }
}

static int slot_von_obj(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

static void raum(unsigned room_id, const char *datei, long for_offset)
{
    char path[700], was[200];
    snprintf(path, sizeof path, "%s/STAGE1/%s", RE15_XSTR(RE15_ASSETS_PATH), datei);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    printf("\n== %s ==\n", datei);
    if (!buf) { printf("   RDT nicht lesbar: %s\n", path); fehler++; return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { printf("   parse fail\n"); fehler++; free(buf); return; }
    pruefe("der Raum bringt vier Props mit (nOmodel = 4), Slot 4 ist frei", rdt.prop_count == 4);
    if (rdt.prop_count < 3) { free(buf); return; }

    /* ---- 1. Boden des rechten unteren Fachs (Viereck 93) ---- */
    static re15_md1_t tisch, d1, d2, sich;
    if (re15_md1_parse(rdt.prop_md1[0], rdt.prop_md1_size[0], &tisch) != 0 ||
        re15_md1_parse(rdt.prop_md1[1], rdt.prop_md1_size[1], &d1) != 0 ||
        re15_md1_parse(rdt.prop_md1[2], rdt.prop_md1_size[2], &d2) != 0) {
        printf("   MD1 der Props nicht lesbar\n"); fehler++; free(buf); return;
    }
    const re15_md1_mesh_t *m = &tisch.meshes[0];
    long md1_off = (long)(rdt.prop_md1[0] - buf);
    printf("   Prop 0: MD1 @Datei 0x%05lX, %d Punkte, %d Vierecke\n",
           md1_off, (int)m->quad_vertex_count, (int)m->quad_count);
    pruefe("Prop 0 traegt 163 Punkte und 120 Vierecke", m->quad_vertex_count == 163 && m->quad_count == 120);
    if (m->quad_count < 94) { free(buf); return; }

    int lo[3] = { 0x7fffffff, 0x7fffffff, 0x7fffffff }, hi[3] = { -0x7fffffff, -0x7fffffff, -0x7fffffff };
    {
        const re15_md1_quad_t *q = &m->quads[93];
        uint16_t vi[4] = { q->v0, q->v1, q->v2, q->v3 };
        for (int k = 0; k < 4; k++) {
            int p[3] = { m->quad_vertices[vi[k]].x, m->quad_vertices[vi[k]].y, m->quad_vertices[vi[k]].z };
            for (int a = 0; a < 3; a++) { if (p[a] < lo[a]) lo[a] = p[a]; if (p[a] > hi[a]) hi[a] = p[a]; }
        }
        printf("   Fachboden (Viereck 93, Face-Record @Datei 0x%05lX): x[%d..%d] y[%d..%d] z[%d..%d]\n",
               (long)((const uint8_t *)q - buf), lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]);
    }
    int w0 = 0x7fffffff, w1 = 0x7fffffff;
    for (int qi = 90; qi <= 92; qi += 2) {            /* Waende Viereck 90 / 92: je ein z */
        const re15_md1_quad_t *q = &m->quads[qi];
        uint16_t vi[4] = { q->v0, q->v1, q->v2, q->v3 };
        int z = m->quad_vertices[vi[0]].z, eins = 1;
        for (int k = 1; k < 4; k++) if (m->quad_vertices[vi[k]].z != z) eins = 0;
        if (eins) { if (qi == 90) w0 = z; else w1 = z; }
    }
    printf("   Waende: Viereck 90 z=%d, Viereck 92 z=%d\n", w0, w1);
    pruefe("alle vier Bodenpunkte liegen auf EINER Hoehe", lo[1] == hi[1]);
    pruefe("der Boden reicht von der Vorderkante x=2 bis zur Rueckwand x=-1258", lo[0] == -1258 && hi[0] == 2);
    pruefe("die Waende (Viereck 90/92) begrenzen den Boden in z", w0 == lo[2] && w1 == hi[2]);

    /* ---- 2. Mitte und Hoehe ---- */
    int n_s = 0;
    const uint8_t *sb = re15_sicherung_md1_bytes(&n_s);
    if (re15_md1_parse(sb, n_s, &sich) != 0) { printf("   Sicherungs-MD1 nicht lesbar\n"); fehler++; free(buf); return; }
    int slo[3], shi[3];
    bbox(&sich.meshes[0], slo, shi);
    int radius = shi[1];
    printf("   Sicherung: x[%d..%d] y[%d..%d] z[%d..%d] -> Laenge %d, Rohrradius %d\n",
           slo[0], shi[0], slo[1], shi[1], slo[2], shi[2], shi[0] - slo[0], radius);
    /* Mitte mal zwei, damit die halbe Einheit bei ungerader Spanne nicht wegrundet */
    int mx2 = lo[0] + hi[0], mz2 = lo[2] + hi[2];
    printf("   Mitte des Fachbodens: x = %.1f, z = %.1f ; Boden y = %d\n", mx2 / 2.0, mz2 / 2.0, lo[1]);
    snprintf(was, sizeof was, "POS_X %d = Mitte des Fachbodens +-1", RE15_SICHERUNG_POS_X);
    pruefe(was, abs(2 * RE15_SICHERUNG_POS_X - mx2) <= 2);
    snprintf(was, sizeof was, "POS_Z %d = Mitte des Fachbodens +-1", RE15_SICHERUNG_POS_Z);
    pruefe(was, abs(2 * RE15_SICHERUNG_POS_Z - mz2) <= 2);
    snprintf(was, sizeof was, "POS_Y %d = Boden %d - Rohrradius %d", RE15_SICHERUNG_POS_Y, lo[1], radius);
    pruefe(was, RE15_SICHERUNG_POS_Y == lo[1] - radius);

    /* ---- 3. rechts der Trennwand ---- */
    int l1[3], h1[3], l2[3], h2[3];
    bbox(&d1.meshes[0], l1, h1);
    bbox(&d2.meshes[0], l2, h2);
    printf("   Deckel Prop 1 z[%d..%d], Prop 2 z[%d..%d]\n", l1[2], h1[2], l2[2], h2[2]);
    pruefe("die Deckelhaelften stossen aneinander (z min Prop 1 = z max Prop 2)", l1[2] == h2[2]);
    {   /* Trennwand zum linken Fach: dessen rechte Wand = Viereck 88 (ein z) */
        const re15_md1_quad_t *q = &m->quads[88];
        uint16_t vi[4] = { q->v0, q->v1, q->v2, q->v3 };
        int zt = m->quad_vertices[vi[0]].z, eins = 1;
        for (int k = 1; k < 4; k++) if (m->quad_vertices[vi[k]].z != zt) eins = 0;
        printf("   Trennwand: rechte Wand des linken Fachs Viereck 88 z=%d, dieses Fach ab z=%d\n", zt, lo[2]);
        pruefe("das Fach liegt RECHTS der Trennwand (Plattform +z = Schirm rechts): z0 > Viereck 88",
               eins && lo[2] > zt);
    }

    /* ---- 4./5. Drehung und Passung ---- */
    int ex = hi[0] - lo[0], ez = hi[2] - lo[2];
    int32_t s = (int32_t)re15_sin_q12(RE15_SICHERUNG_ROT_Y);
    int32_t c = (int32_t)re15_cos_q12(RE15_SICHERUNG_ROT_Y);
    /* Prop-Drehmatrix Ry*Rx*Rz mit rx = rz = 0 (pc_prop_rot_q12, main.c): die Modellachse X
     * geht auf (cos ry, 0, -sin ry). */
    printf("   Fach: %d in x (Tiefe), %d in z (Breite) ; ROT_Y %d -> Modell-X auf (%d, 0, %d) / 4096\n",
           ex, ez, RE15_SICHERUNG_ROT_Y, (int)c, (int)-s);
    pruefe("ROT_Y legt die Laengsachse auf z, parallel zur offenen Vorderseite (cos = 0, |sin| = 4096)",
           c == 0 && abs((int)s) == 4096);
    pruefe("die Sicherung passt der Laenge nach ins Fach (Laenge <= Breite in z)", (shi[0] - slo[0]) <= ez);
    pruefe("die Sicherung passt der Dicke nach ins Fach (Dicke <= Tiefe in x)", (shi[2] - slo[2]) <= ex);

    /* ---- 6. Deckelweg ---- */
    static const uint8_t FOR15[6] = { 0x0d, 0x00, 0x18, 0x00, 0x0f, 0x00 };
    int for_ok = ((size_t)for_offset + 6 <= sz) && memcmp(buf + for_offset, FOR15, 6) == 0;
    snprintf(was, sizeof was, "For-Record @Datei 0x%04lX = 0d 00 18 00 0f 00 (Zaehler 15)", for_offset);
    pruefe(was, for_ok);
    int zaehler = for_ok ? (buf[for_offset + 4] | (buf[for_offset + 5] << 8)) : 0;
    int weg = zaehler * 10;                 /* Speed_set +-10 je Durchlauf (2f 02 0a 00 / 2f 02 f6 ff) */
    pruefe("Speed_set +10 folgt 10 Byte hinter dem For (2f 02 0a 00)",
           for_ok && memcmp(buf + for_offset + 10, "\x2f\x02\x0a\x00", 4) == 0);
    pruefe("Speed_set -10 folgt 20 Byte hinter dem For (2f 02 f6 ff)",
           for_ok && memcmp(buf + for_offset + 20, "\x2f\x02\xf6\xff", 4) == 0);
    printf("   Deckelweg %d -> Oeffnung z[%d..%d]\n", weg, l1[2] - weg, l1[2] + weg);
    pruefe("Deckelweg 150", weg == 150);

    /* ---- 7. im Spiel ---- */
    scd_vm_init(); re15_actor_init();
    re15_game_state_init();
    g_current_room_id = (int)room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22250; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, -22250, -18500, 0);
    int si = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
    pruefe("das Prop ist nach dem Raumstart im Pool", si >= 0);
    if (si >= 0) {
        printf("   Pool[%d]: pos=(%ld,%ld,%ld) rot=(%d,%d,%d) parent=%d\n", si,
               (long)g_scd.props[si].x, (long)g_scd.props[si].y, (long)g_scd.props[si].z,
               g_scd.props[si].rot_x, g_scd.props[si].rot_y, g_scd.props[si].rot_z,
               g_scd.props[si].parent_obj);
        pruefe("Sitz im Pool = (POS_X, POS_Y, POS_Z)",
               g_scd.props[si].x == RE15_SICHERUNG_POS_X &&
               g_scd.props[si].y == RE15_SICHERUNG_POS_Y &&
               g_scd.props[si].z == RE15_SICHERUNG_POS_Z);
        pruefe("Drehung im Pool = (0, ROT_Y, 0)",
               g_scd.props[si].rot_x == 0 && g_scd.props[si].rot_y == RE15_SICHERUNG_ROT_Y &&
               g_scd.props[si].rot_z == 0);
        pruefe("haengt an der Elternmatrix der Plattform (obj 0)", g_scd.props[si].parent_obj == 0);
    }
    free(buf);
}

int main(void)
{
    raum(0x1150, "ROOM1150.RDT", 0x0FC0);
    raum(0x1151, "ROOM1151.RDT", 0x0F9E);
    printf("\n%s - %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? 1 : 0;
}
