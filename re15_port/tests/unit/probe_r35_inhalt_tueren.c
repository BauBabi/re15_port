/* probe_r35_inhalt_tueren.c — MESS-SONDE (Runde 35 Spur F, Punkt 1): "Einige Doppeltueren der
 * Tuersequenzen haben unsymmetrischen Aufbau, z.B. unsymmetrische Tuergriffe."
 *
 * Fuer JEDE Wahl der Tuer-Tabelle (Archiv, Variante, Port-Archiv, Spender) laeuft die Tuer-Maschine
 * (re15_door_seq_start/_bild = Door_init FUN_80013c1c / Door_move FUN_80013eb4) bis Bild B, und je
 * GRIFF-OBJEKT (Objekt mit Eltern-Fluegel, kein Fluegel selbst) wird die Weltmatrix gebildet, mit der
 * der Laeufer zeichnet: ohne Griff-Tausch o->welt, mit Griff-Tausch dieselbe Formel wie
 * door_scene_pc.c griff_zeichnen (Grund-Drehung vorn/hinten nach pos[0] < 0, Ausschlag, Versatz).
 * Die Mesh-Ecken (Archiv- bzw. Spender-Mesh) gehen durch die Matrix; gemessen wird je Griff die
 * SPITZE (Ecke mit dem groessten Abstand zum Anhaengepunkt) relativ zum Anhaengepunkt.
 * Doppeltuer = zwei Fluegel (Objekte ohne Eltern, die je mindestens einen Griff tragen). Symmetrie:
 * Spiegelebene = Mittelebene zwischen den beiden Fluegel-Ursprungen (Angeln), Normale = Verbindung
 * der Angeln. Fehler = Winkel zwischen Spitze(B) und Spiegel(Spitze(A)) und Abstand zwischen
 * Ursprung(B) und Spiegel(Ursprung(A)), je Seite (vorn/hinten getrennt).
 *
 * Aufruf: probe_r35_inhalt_tueren [bild]   (kein add_test)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_door_seq.h"
#include "re15_md1.h"
#include "re15_tuer_spiegel.h"

#ifndef RE15_ASSET_SHARED_DIR
#error RE15_ASSET_SHARED_DIR fehlt
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

typedef struct { double x, y, z; } v3;
static v3 mat_pt(const re15_door_mat_t *m, double x, double y, double z)
{
    v3 r = { (m->m[0]*x + m->m[1]*y + m->m[2]*z) / 4096.0 + m->t[0],
             (m->m[3]*x + m->m[4]*y + m->m[5]*z) / 4096.0 + m->t[1],
             (m->m[6]*x + m->m[7]*y + m->m[8]*z) / 4096.0 + m->t[2] };
    return r;
}
static v3 sub(v3 a, v3 b) { v3 r = { a.x-b.x, a.y-b.y, a.z-b.z }; return r; }
static double dot(v3 a, v3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static double len(v3 a) { return sqrt(dot(a, a)); }
static v3 scale(v3 a, double s) { v3 r = { a.x*s, a.y*s, a.z*s }; return r; }

typedef struct { int oi, fluegel, hinten, mesh_id, lokal_x; v3 ursprung, spitze, huelle_welt, blatt_x;
                 double z_lo, z_hi, sx0, sx1, sy0, sy1; } griff_t;
typedef struct { uint8_t re2, var, eigen; int seite; double winkel, hoehe; uint16_t seite_id; } ergebnis_t;
static ergebnis_t s_erg[512]; static int s_nerg;

/* Mess-Art: 0 = Spitze (Ecke mit groesstem Abstand vom Ursprung welt.t), 1 = Mitte der Huelle
 * (min/max je Achse) relativ zum Ursprung — robust fuer Stangen, die um den Anhaengepunkt liegen.
 * s_fix = 1: Griff-Tausch MIT der Spiegel-Drehung des Archiv-Objekts (rot[2] += rot0[2]). */
static int s_mitte = 0, s_fix = 1, s_var_dy = 0, s_var_dz = 0;   /* s_fix 0 = alter Stand ('alt') */
static int s_seite_lokal = 0;   /* 1 = alte Seitenzuordnung nach pos[0] ('lokal'), Nachbesserung 1 */
static v3 spitze(const re15_md1_mesh_t *m, const re15_door_mat_t *w)
{
    v3 o = { w->t[0], w->t[1], w->t[2] }, best = o; double bd = -1;
    v3 lo = { 1e9, 1e9, 1e9 }, hi = { -1e9, -1e9, -1e9 };
    for (int k = 0; k < 2; k++) {
        const re15_md1_vertex_t *vv = k ? m->quad_vertices : m->tri_vertices;
        int nv = k ? m->quad_vertex_count : m->tri_vertex_count;
        for (int i = 0; vv && i < nv; i++) {
            v3 p = mat_pt(w, vv[i].x, vv[i].y, vv[i].z);
            double d = len(sub(p, o));
            if (d > bd) { bd = d; best = p; }
            if (p.x < lo.x) lo.x = p.x; if (p.y < lo.y) lo.y = p.y; if (p.z < lo.z) lo.z = p.z;
            if (p.x > hi.x) hi.x = p.x; if (p.y > hi.y) hi.y = p.y; if (p.z > hi.z) hi.z = p.z;
        }
    }
    if (s_mitte) { v3 c = { (lo.x + hi.x) / 2, (lo.y + hi.y) / 2, (lo.z + hi.z) / 2 }; return sub(c, o); }
    return sub(best, o);
}

/* Mitte der Welt-Huelle eines Griff-Meshes (absolut) — Seitenzuordnung und Hoehenmass
 * (Nachbesserung 1, M3). */
static double s_hz_lo, s_hz_hi;   /* Tiefe (Welt-z) der letzten Huelle */
static v3 huelle_welt(const re15_md1_mesh_t *m, const re15_door_mat_t *w)
{
    v3 lo = { 1e9, 1e9, 1e9 }, hi = { -1e9, -1e9, -1e9 };
    for (int k = 0; k < 2; k++) {
        const re15_md1_vertex_t *vv = k ? m->quad_vertices : m->tri_vertices;
        int nv = k ? m->quad_vertex_count : m->tri_vertex_count;
        for (int i = 0; vv && i < nv; i++) {
            v3 p = mat_pt(w, vv[i].x, vv[i].y, vv[i].z);
            if (p.x < lo.x) lo.x = p.x; if (p.y < lo.y) lo.y = p.y; if (p.z < lo.z) lo.z = p.z;
            if (p.x > hi.x) hi.x = p.x; if (p.y > hi.y) hi.y = p.y; if (p.z > hi.z) hi.z = p.z;
        }
    }
    v3 c = { (lo.x + hi.x) / 2, (lo.y + hi.y) / 2, (lo.z + hi.z) / 2 };
    s_hz_lo = lo.z; s_hz_hi = hi.z;
    return c;
}

/* Bild-Huelle (Pixel) eines Griff-Meshes: "welt" ist Kameraraum der Tuerszene ->
 * sx = OFX + H*x/z, sy = OFY + H*y/z mit H = RE15_DOOR_H 290 (@0x80013e34), OFX/OFY 160/120
 * (@0x80068e80/@0x80068e88) — wie re15_door_rtp (RTPT sf=1). Nachbesserung 1 (M3b). */
static void bild_huelle(const re15_md1_mesh_t *m, const re15_door_mat_t *w,
                        double *x0, double *x1, double *y0, double *y1)
{
    *x0 = *y0 = 1e9; *x1 = *y1 = -1e9;
    for (int k = 0; k < 2; k++) {
        const re15_md1_vertex_t *vv = k ? m->quad_vertices : m->tri_vertices;
        int nv = k ? m->quad_vertex_count : m->tri_vertex_count;
        for (int i = 0; vv && i < nv; i++) {
            v3 p = mat_pt(w, vv[i].x, vv[i].y, vv[i].z);
            if (p.z < 1) continue;
            double sx = RE15_DOOR_OFX + RE15_DOOR_H * p.x / p.z, sy = RE15_DOOR_OFY + RE15_DOOR_H * p.y / p.z;
            if (sx < *x0) *x0 = sx; if (sx > *x1) *x1 = sx;
            if (sy < *y0) *y0 = sy; if (sy > *y1) *y1 = sy;
        }
    }
}

/* Schwerpunkt der Ecken des Meshes von Objekt oi unter seiner Weltmatrix. */
static v3 schwerpunkt(const re15_door_seq_t *s, int oi)
{
    const re15_door_obj_t *o = &s->obj[oi];
    v3 sum = { 0, 0, 0 }; int n = 0;
    if (s->md1_ok && o->mesh < (unsigned)s->md1.mesh_count) {
        const re15_md1_mesh_t *m = &s->md1.meshes[o->mesh];
        for (int k = 0; k < 2; k++) {
            const re15_md1_vertex_t *vv = k ? m->quad_vertices : m->tri_vertices;
            int nv = k ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; vv && i < nv; i++) {
                v3 p = mat_pt(&o->welt, vv[i].x, vv[i].y, vv[i].z);
                sum.x += p.x; sum.y += p.y; sum.z += p.z; n++;
            }
        }
    }
    if (!n) { v3 t = { o->welt.t[0], o->welt.t[1], o->welt.t[2] }; return t; }
    return scale(sum, 1.0 / n);
}

static void messen(const re15_tuer_zeile_t *z, int bild_ziel)
{
    const re15_tuer_eigen_t *e = z->eigen ? re15_door_seq_eigen(z->eigen) : NULL;
    char pfad[600];
    if (e) snprintf(pfad, sizeof pfad, "%s/RE15DOOR/%s.DO2", RE15_ASSET_SHARED_DIR, e->kennung);
    else   snprintf(pfad, sizeof pfad, "%s/RE2/DOOR/DOOR%02X.DO2", RE15_ASSET_SHARED_DIR, z->re2_nr);
    size_t n = 0; uint8_t *d = slurp(pfad, &n);
    int ton = 0, modell = 0, sektor = 0, datei = 0;
    if (!d || re15_door_seq_re2_archiv(z->re2_nr, &ton, &modell, &sektor, &datei) != 0) { free(d); return; }
    static re15_door_seq_t s;
    if (re15_door_seq_start(&s, d + sektor * 0x800, (int)n - sektor * 0x800, z->variante, 0, z->re2_nr) != 0) { free(d); return; }
    for (int b = 0; b < bild_ziel && re15_door_seq_bild(&s, 1); b++) { }
    /* Griff-Tausch wie der Laeufer (door_scene_pc.c) */
    const re15_griff_tausch_t *gt = z->spender ? re15_door_seq_griff_tausch_fuer(z->re2_nr, z->spender, z->eigen) : NULL;
    static re15_md1_t smd1; int s_ok = 0; uint8_t *sd = NULL;
    if (gt) {
        char sp[600]; size_t sn = 0;
        const re15_tuer_eigen_t *se = gt->spender_eigen ? re15_door_seq_eigen(gt->spender_eigen) : NULL;
        if (se) snprintf(sp, sizeof sp, "%s/RE15DOOR/%s.DO2", RE15_ASSET_SHARED_DIR, se->kennung);
        else    snprintf(sp, sizeof sp, "%s/RE2/DOOR/DOOR%02X.DO2", RE15_ASSET_SHARED_DIR, gt->spender);
        sd = slurp(sp, &sn);
        int st = 0, sm = 0, ss = 0, sdat = 0;
        int spender_basis = se ? se->basis : gt->spender;
        if (sd && re15_door_seq_re2_archiv(spender_basis, &st, &sm, &ss, &sdat) == 0) {
            const uint8_t *tm = sd + ss * 0x800;
            uint32_t md1_rel = (uint32_t)tm[0] | ((uint32_t)tm[1] << 8) | ((uint32_t)tm[2] << 16) | ((uint32_t)tm[3] << 24);
            uint32_t tim_rel = (uint32_t)tm[4] | ((uint32_t)tm[5] << 8) | ((uint32_t)tm[6] << 16) | ((uint32_t)tm[7] << 24);
            s_ok = re15_md1_parse(tm + md1_rel, (int)(tim_rel - md1_rel), &smd1) == 0;
        }
    }
    griff_t g[RE15_DOOR_OBJEKTE]; int ng = 0;
    for (int oi = 0; oi < RE15_DOOR_OBJEKTE; oi++) {
        const re15_door_obj_t *o = &s.obj[oi];
        if (!o->on || o->eltern < 0 || !s.md1_ok) continue;
        if (s.obj[o->eltern].eltern >= 0) continue;            /* nur direkte Kinder eines Fluegels */
        int getauscht = gt && s_ok && o->mesh == gt->mesh_archiv;
        re15_door_mat_t welt = o->welt;
        const re15_md1_mesh_t *m = NULL;
        int hinten = o->pos[0] < 0;
        if (getauscht) {
            const uint16_t *basis = hinten ? gt->rot_hinten : gt->rot_vorn;
            int32_t delta = (int16_t)(uint16_t)(o->rot[0] - o->rot0[0]);
            int32_t d2 = gt->aus_archiv ? delta * gt->aus_spender / gt->aus_archiv : 0;
            uint16_t rot[3];
            if (s_fix) re15_tuer_griff_tausch_rot(basis, d2, o->rot0[2], gt->spender, rot);   /* = door_scene_pc.c */
            else { rot[0] = (uint16_t)(basis[0] + d2); rot[1] = basis[1]; rot[2] = basis[2]; }   /* alt */
            const int16_t *vs = hinten ? gt->versatz_hinten : gt->versatz_vorn;
            int32_t pos[3] = { o->pos[0] + vs[0], o->pos[1] + vs[1], o->pos[2] + vs[2] };
            if (s_fix) pos[0] += re15_tuer_griff_tausch_dx(gt->spender, o->eltern, o->pos[0], o->rot0[2]);
            re15_door_seq_objmatrix(&s.obj[o->eltern].welt, rot, pos, &welt);
            if (gt->mesh_spender < (unsigned)smd1.mesh_count) m = &smd1.meshes[gt->mesh_spender];
        } else if (o->mesh < (unsigned)s.md1.mesh_count) {
            m = &s.md1.meshes[o->mesh];
            uint16_t dz = s_fix ? re15_tuer_spiegel_dz(e ? e->kennung : NULL, o->eltern) : 0;
            uint16_t dy = s_fix ? re15_tuer_spiegel_dy(e ? e->kennung : NULL, o->eltern) : 0;
            if ((dz || dy) && s.obj[o->eltern].eltern < 0) {     /* = door_scene_pc.c objekt_zeichnen */
                uint16_t rot[3] = { o->rot[0], (uint16_t)(o->rot[1] + dy), (uint16_t)(o->rot[2] + dz) };
                int32_t pos[3] = { o->pos[0], o->pos[1], o->pos[2] };
                re15_door_seq_objmatrix(&s.obj[o->eltern].welt, rot, pos, &welt);
            }
            if ((s_var_dy || s_var_dz) && o->eltern == 1) {      /* Versuch: Drehung am 2. Fluegel */
                uint16_t rot[3] = { o->rot[0], (uint16_t)(o->rot[1] + s_var_dy), (uint16_t)(o->rot[2] + s_var_dz) };
                int32_t pos[3] = { o->pos[0], o->pos[1], o->pos[2] };
                re15_door_seq_objmatrix(&s.obj[o->eltern].welt, rot, pos, &welt);
            }
        }
        if (!m) continue;
        g[ng].oi = oi; g[ng].fluegel = o->eltern; g[ng].hinten = hinten; g[ng].lokal_x = o->pos[0];
        { const re15_door_mat_t *bw = &s.obj[o->eltern].welt;   /* lokale x-Achse des Blatts in Welt */
          v3 bx = { bw->m[0] / 4096.0, bw->m[3] / 4096.0, bw->m[6] / 4096.0 }; g[ng].blatt_x = bx; }
        g[ng].mesh_id = getauscht ? 1000 + (int)gt->mesh_spender : (int)o->mesh;
        v3 u = { welt.t[0], welt.t[1], welt.t[2] };
        g[ng].ursprung = u; g[ng].spitze = spitze(m, &welt); g[ng].huelle_welt = huelle_welt(m, &welt);
        g[ng].z_lo = s_hz_lo; g[ng].z_hi = s_hz_hi;
        bild_huelle(m, &welt, &g[ng].sx0, &g[ng].sx1, &g[ng].sy0, &g[ng].sy1);
        printf("   obj %2d mesh %u eltern %d pos (%d,%d,%d) rot0 (%u,%u,%u) %s%s spitze (%.0f,%.0f,%.0f)"
               " huelle_welt (%.1f,%.1f,%.1f) z %.0f..%.0f\n",
               oi, o->mesh, o->eltern, o->pos[0], o->pos[1], o->pos[2], o->rot0[0], o->rot0[1], o->rot0[2],
               hinten ? "hinten" : "vorn", getauscht ? " GETAUSCHT" : "",
               g[ng].spitze.x, g[ng].spitze.y, g[ng].spitze.z,
               g[ng].huelle_welt.x, g[ng].huelle_welt.y, g[ng].huelle_welt.z, s_hz_lo, s_hz_hi);
        ng++;
    }
    /* Fluegel mit Griffen */
    int fl[4] = { -1, -1, -1, -1 }, nf = 0;
    for (int i = 0; i < ng; i++) {
        int neu = 1;
        for (int k = 0; k < nf; k++) if (fl[k] == g[i].fluegel) neu = 0;
        if (neu && nf < 4) fl[nf++] = g[i].fluegel;
    }
    printf("  => DOOR%02X V%u eigen %s spender %02X: %d Griffe an %d Fluegeln%s\n", z->re2_nr, z->variante,
           e ? e->kennung : "-", z->spender, ng, nf, nf >= 2 ? "  (DOPPELTUER)" : "");
    for (int k = 0; k < nf; k++) {
        const re15_door_mat_t *bw = &s.obj[fl[k]].welt;
        double zlo = 1e9, zhi = -1e9;
        if (s.md1_ok && s.obj[fl[k]].mesh < (unsigned)s.md1.mesh_count) {
            const re15_md1_mesh_t *bm = &s.md1.meshes[s.obj[fl[k]].mesh];
            for (int q = 0; q < 2; q++) {
                const re15_md1_vertex_t *vv = q ? bm->quad_vertices : bm->tri_vertices;
                int nv = q ? bm->quad_vertex_count : bm->tri_vertex_count;
                for (int i = 0; vv && i < nv; i++) {
                    v3 p = mat_pt(bw, vv[i].x, vv[i].y, vv[i].z);
                    if (p.z < zlo) zlo = p.z; if (p.z > zhi) zhi = p.z;
                }
            }
        }
        printf("     Fluegel obj %d: x-Achse (%d,%d,%d) y-Achse (%d,%d,%d) z-Achse (%d,%d,%d) t (%d,%d,%d)"
               " Blatt-Tiefe z %.0f..%.0f\n", fl[k],
               bw->m[0], bw->m[3], bw->m[6], bw->m[1], bw->m[4], bw->m[7], bw->m[2], bw->m[5], bw->m[8],
               bw->t[0], bw->t[1], bw->t[2], zlo, zhi);
    }
    for (int i = 0; i < ng; i++)
        printf("     Griff obj %d: Anhaengepunkt Welt (%.0f,%.0f,%.0f)\n", g[i].oi, g[i].ursprung.x, g[i].ursprung.y,
               g[i].ursprung.z);
    if (nf >= 2) {
        const re15_door_obj_t *A = &s.obj[fl[0]], *B = &s.obj[fl[1]];
        /* Breitenachse der Tuer = Verbindung der SCHWERPUNKTE der beiden Fluegel-Meshes (die
         * Blaetter sind spiegelgleiche Platten); die Angel-Ursprunge taugen nicht (DOOR04: andere
         * Tiefe), die Anhaengepunkte der Griffe auch nicht (anderer Fluegel-Rahmen). */
        v3 a = schwerpunkt(&s, fl[0]), b = schwerpunkt(&s, fl[1]);
        v3 w = sub(b, a); double wl = len(w); w = scale(w, 1.0 / wl);
        v3 mitte = scale(sub(scale(sub(a, scale(a, 0)), 1), scale(sub(a, b), 0.5)), 1);   /* (a+b)/2 */
        mitte.x = (a.x + b.x) / 2; mitte.y = (a.y + b.y) / 2; mitte.z = (a.z + b.z) / 2;
        /* Nachbesserung 1 (M3a): Seite nach der WELT bestimmen, nicht nach dem Vorzeichen der lokalen
         * pos[0] — der zweite Fluegel ist gespiegelt, dort zeigt lokales +x auf die andere Seite
         * (S022/S041/S045/S080/S136/S157: je Fluegel ein Griff mit pos[0] +130 / -130 auf DERSELBEN
         * Seite). Tuernormale = Breitenachse w x senkrecht (0,1,0). Der Anhaengepunkt sitzt bei
         * lokal pos[0] = +-130 vor/hinter der Blattebene; in WELT zeigt dieses lokale +x entlang
         * der lokalen x-Achse des Blatts (Spalte 0 seiner Weltmatrix) -> Seite = Vorzeichen von
         * pos[0] * (Blatt-x . Normale). Robust gegen das beginnende Oeffnen (nur die Achse, nicht
         * der 3000 lange Hebel zur Angel geht ein). Gemessen und verworfen: absolute Huellen-Mitte
         * gegen die Tuermitte (V3-Paare falsch) und Huellen-Mitte gegen den Anhaengepunkt (Stangen
         * ohne Abstand entlang der Normale: S041/S136/S157 fielen heraus). Mit "lokal" alter Stand. */
        /* ENDGUELTIG (gemessen, s. Dossier Nachbesserung 1): "welt" ist bereits Kameraraum der
         * Tuerszene (Kamera im Ursprung, z = Tiefe; Projektion H 290 @0x80013e34, 160/120
         * @0x80068e80/88 reproduziert die Abnahme-Messung 157,0/156,0 px). Seite 0 = KAMERASEITE =
         * Griffe, deren Huelle vor die Blattmitte (z der Tuermitte) ragt; Seite 1 = abgewandt = Huelle
         * ragt hinter die Blattmitte. Je Fluegel zaehlt der am weitesten zur jeweiligen Seite
         * ragende Griff. (Ein Griff kann beide Seiten erreichen: DOOR1D V3 obj 3 sitzt an der
         * Rueckseite und ragt durch das Blatt nach vorn — RE2-Daten, auch im Original S192.) */
        for (int seite = 0; seite < 2; seite++) {
            int ia = -1, ib = -1;
            for (int i = 0; i < ng; i++) {
                int dabei = s_seite_lokal ? (g[i].hinten == seite)
                          : seite == 0 ? (g[i].z_lo < mitte.z) : (g[i].z_hi > mitte.z);
                if (!dabei) continue;
                if (g[i].fluegel == fl[0] && (ia < 0 || (seite == 0 ? g[i].z_lo < g[ia].z_lo : g[i].z_hi > g[ia].z_hi)))
                    ia = i;
                if (g[i].fluegel == fl[1] && (ib < 0 || (seite == 0 ? g[i].z_lo < g[ib].z_lo : g[i].z_hi > g[ib].z_hi)))
                    ib = i;
            }
            if (ia < 0 || ib < 0 || g[ia].mesh_id != g[ib].mesh_id) continue;   /* nur gleiche Griffe */
            v3 ta = g[ia].spitze, tb = g[ib].spitze;
            /* Spiegelnormale aus den beiden ANHAENGEPUNKTEN des Paars (liegen ueber die Fuge
             * gespiegelt; die Angel-Ursprunge der Fluegel liegen bei DOOR04 nicht in einer Tiefe) */
            v3 tam = sub(ta, scale(w, 2 * dot(ta, w)));                    /* Spiegel der Spitze A */
            v3 ua = sub(g[ia].ursprung, mitte);
            v3 uam = sub(ua, scale(w, 2 * dot(ua, w)));
            v3 ub = sub(g[ib].ursprung, mitte);
            double c = dot(tam, tb) / (len(tam) * len(tb) + 1e-9);
            if (c > 1) c = 1; if (c < -1) c = -1;
            double winkel = acos(c) * 180.0 / 3.14159265358979;
            double lage = len(sub(uam, ub));
            /* Bildmass (M3b): Bild-Huellen beider Griffe (H 290): Unterschied der Oberkante, Unterkante,
             * Hoehe und Breite in Pixeln — der groesste Wert ist der Bild-Fehler. Spiegelachse x wird
             * nicht gebraucht (Hoehen und Groessen haengen nicht von der Fugenlage ab). */
            double bf = 0, dd[4] = { fabs(g[ia].sy0 - g[ib].sy0), fabs(g[ia].sy1 - g[ib].sy1),
                                     fabs((g[ia].sy1 - g[ia].sy0) - (g[ib].sy1 - g[ib].sy0)),
                                     fabs((g[ia].sx1 - g[ia].sx0) - (g[ib].sx1 - g[ib].sx0)) };
            for (int q = 0; q < 4; q++) if (dd[q] > bf) bf = dd[q];
            double hoehe = bf;
            printf("  SYMMETRIE DOOR%02X V%u %s %s: Spitzen-Winkel %.1f Grad, Lage-Fehler %.0f (Angelabstand %.0f),"
                   " Bild-Fehler %.2f px (oben %.2f unten %.2f Hoehe %.2f Breite %.2f; obj %d/%d) (Seite S%03u)%s\n",
                   z->re2_nr, z->variante, e ? e->kennung : "RE2", seite ? "abgewandt" : "KAMERA", winkel, lage, wl,
                   bf, dd[0], dd[1], dd[2], dd[3], g[ia].oi, g[ib].oi, z->seite,
                   winkel > 10.0 ? "  <-- UNSYMMETRISCH" : "");
            if (s_nerg < 512) { ergebnis_t *r = &s_erg[s_nerg++]; r->re2 = z->re2_nr; r->var = z->variante;
                                r->eigen = z->eigen; r->seite = seite; r->winkel = winkel; r->hoehe = hoehe;
                                r->seite_id = z->seite; }
        }
    }
    re15_door_seq_ende(&s);
    free(d); free(sd);
}

static void alle_wahlen(int bild);
static int pruefen(void);

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "test")) return pruefen();
    int bild = argc > 1 ? atoi(argv[1]) : 2;
    for (int k = 2; k < argc; k++) { if (!strcmp(argv[k], "mitte")) s_mitte = 1; if (!strcmp(argv[k], "alt")) s_fix = 0; if (!strcmp(argv[k], "lokal")) s_seite_lokal = 1;if (!strncmp(argv[k], "dy:", 3)) s_var_dy = atoi(argv[k] + 3); if (!strncmp(argv[k], "dz:", 3)) s_var_dz = atoi(argv[k] + 3); }
    alle_wahlen(bild);
    return 0;
}

static void alle_wahlen(int bild)
{
    s_nerg = 0;
    int n = re15_door_seq_zeilen();
    /* jede Wahl nur einmal */
    static uint32_t gesehen[1024]; int ns = 0;
    for (int i = 0; i < n; i++) {
        const re15_tuer_zeile_t *z = re15_door_seq_zeile(i);
        if (!z || !z->re2_nr) continue;
        uint32_t key = ((uint32_t)z->re2_nr << 24) | ((uint32_t)z->variante << 16) | ((uint32_t)z->eigen << 8) | z->spender;
        int dup = 0;
        for (int k = 0; k < ns; k++) if (gesehen[k] == key) dup = 1;
        if (dup || ns >= 1024) continue;
        gesehen[ns++] = key;
        printf("\nWahl %d: Seite %u Tuer T%03u Raum %04X\n", ns, z->seite, z->tuer, z->raum);
        messen(z, bild);
    }
}

/* RIEGEL unit_r35_inhalt_tueren (Aufruf "test"): beide Masse (Spitze, Huellen-Mitte), Bild 2.
 *   A  ALTER Stand (ohne Spiegel-Drehung beim Tausch, ohne P0CD-Korrektur) zeigt den Befund:
 *      mindestens ein Port-Archiv-Paar > 10 Grad (P1DG/P1DK/P1DL 49,8, P1BD 37,1, P0CD 180).
 *   B  NEUER Stand: JEDES Griff-Paar einer Port-Doppeltuer (eigen != 0) < 10 Grad (Huellen-Mitte:
 *      oder unveraendert gegen den alten Stand, wenn die Geometrie nicht angefasst wird).
 *   C  RE2-Originale (eigen 0) unveraendert: neuer = alter Stand, Wert fuer Wert. */
static const char NL[] = "\n";
static int pruefen(void)
{
    int fails = 0, checks = 0;
    static ergebnis_t alt[2][512], neu[2][512]; int nalt[2], nneu[2];
    for (int m = 0; m < 2; m++) {
        s_mitte = m;
        s_fix = 0; alle_wahlen(2); memcpy(alt[m], s_erg, sizeof s_erg); nalt[m] = s_nerg;
        s_fix = 1; alle_wahlen(2); memcpy(neu[m], s_erg, sizeof s_erg); nneu[m] = s_nerg;
    }
    printf("%s=== Riegel ===%s", NL, NL);
    for (int m = 0; m < 2; m++) {
        const char *mass = m ? "Huellen-Mitte" : "Spitze";
        int befund = 0, port_paare = 0, port_schief = 0, re2_gleich = 1;
        double max_port = 0;
        for (int i = 0; i < nalt[m]; i++) if (alt[m][i].eigen && alt[m][i].winkel > 10.0) befund++;
        for (int i = 0; i < nneu[m]; i++) {
            if (neu[m][i].eigen) {
                port_paare++;
                if (neu[m][i].winkel > max_port) max_port = neu[m][i].winkel;
                /* Huellen-Mitte: ein Paar, dessen Geometrie gar nicht angefasst wird (kein Tausch, keine
                 * Spiegel-Korrektur: P04B = DOOR04 wie RE2), darf seinen RE2-Wert behalten — das Mass
                 * streut bei kurzen Mitte-Vektoren auch an RE2-Originalen (DOOR1B V2 9,0 Grad). */
                int unveraendert = m == 1 && i < nalt[m] && alt[m][i].winkel == neu[m][i].winkel;
                if (neu[m][i].winkel >= 10.0 && !unveraendert) { port_schief++;
                    printf("FAIL: DOOR%02X V%u eigen %u %s: %.1f Grad%s", neu[m][i].re2, neu[m][i].var,
                           neu[m][i].eigen, neu[m][i].seite ? "hinten" : "vorn", neu[m][i].winkel, NL); }
            }
        }
        if (nalt[m] != nneu[m]) re2_gleich = 0;
        for (int i = 0; i < nneu[m] && i < nalt[m]; i++)
            if (!neu[m][i].eigen && (alt[m][i].re2 != neu[m][i].re2 || alt[m][i].winkel != neu[m][i].winkel)) re2_gleich = 0;
        checks++; if (befund == 0) fails++;
        printf("%s A %s: alter Stand %d unsymmetrische Port-Paare (> 10 Grad)%s", befund ? "ok:  " : "FAIL:", mass, befund, NL);
        checks++; if (port_schief || port_paare < 6) fails++;
        printf("%s B %s: neuer Stand %d Port-Paare, alle < 10 Grad (max %.1f)%s",
               (port_schief || port_paare < 6) ? "FAIL:" : "ok:  ", mass, port_paare, max_port, NL);
        checks++; if (!re2_gleich) fails++;
        printf("%s C %s: RE2-Originale unveraendert%s", re2_gleich ? "ok:  " : "FAIL:", mass, NL);
    }
    printf("%s%d Pruefungen, %d Fehler%s", NL, checks, fails, NL);
    return fails ? 1 : 0;
}
