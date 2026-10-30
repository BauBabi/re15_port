/*
 * test_r41_alligator2.c - Runde 41 Spur P "alligator2" (Nachtrag 31 Punkt 2, FG-E7 Option (a), Lagespruenge, Riegel F -> G).
 * Dossier analysis/befunde_runde41/P_alligator2.md.
 *
 * Nutzt den Harness von test_r41_gatorszene.c (echte Engine-Wege: Raumaufbau, SCD-VM, Boss-Takt, RE2-EM23-Bank): die Datei wird
 * eingebunden, ihr main heisst hier gsz_haupt - kein doppelter Harness.
 *
 * Teile (Aufruf: test_r41_alligator2 <teil>):
 *   kamera / kamera_elza  Nachtrag 31 Punkt 2: Abstand der Gator-Teile zur laufenden Kamera und Bildanteil ueber die ersten 150
 *                         Bilder ab dem Kampfbeginn (s. kam_messen)
 * Werkzeug (kein Riegel):
 *   lagesuche2 <datei>    wie lagesuche von test_r41_gatorszene, zusaetzlich Kampfbeginn-Lage und Kamera-Mass
 */
#define main gsz_haupt
#include "test_r41_gatorszene.c"
#undef main

/* ---- Kamera-Mass ueber die ersten Bilder ab dem Kampfbeginn -------------------------------------------------------------- */
#define P_MASS_BILDER 150                       /* Nachtrag 31 Punkt 2: "ueber die ersten ~150 Bilder" */
#define P_RASTER_W    64                        /* Raster 5 x 5 Bildpunkte */
#define P_RASTER_H    48
#define P_BILD_B      320
#define P_BILD_H      240
#define P_KOERPER_R   700                       /* Radius des Koerper-Schlauchs (nur Schaetzmass des Bildanteils; der Riegel an der exe zaehlt Pixel) */

typedef struct {
    int     bilder;
    int32_t wurzel_min;       /* kleinster Abstand Gator-Wurzel <-> Kamera (xz) */
    int32_t teil_min;         /* kleinster Abstand eines Gator-Knochens <-> Kamera (3D) */
    int32_t tiefe_min;        /* kleinste Tiefe eines Knochens VOR der Kamera, der im Bild liegt */
    int     rechts_max, ganz_max;   /* groesster Bildanteil in % (Raster) */
    int     rechts25;         /* Bilder mit > 25 % der rechten Bildhaelfte */
    int     rechts50;
    int     erstes_im_bild;   /* erstes Bild mit einem Knochen im Bild (-1 = keins) */
    int     wurzel_im_bild;   /* erstes Bild mit der Wurzel im Bild */
    int     biss, treffer;    /* erstes Bild Clip 4 / HP-Abfall (-1 = keins) */
} kam_mass_t;

static int proj_cut(const re15_camera_view_t *v, const int32_t p[3], int *sx, int *sy, int32_t *vz_out)
{
    const int64_t px = p[0], py = p[1], pz = p[2];
    const int32_t vx = (int32_t)(((int64_t)v->rot[0]*px + (int64_t)v->rot[1]*py + (int64_t)v->rot[2]*pz) >> 12) + v->trans[0];
    const int32_t vy = (int32_t)(((int64_t)v->rot[3]*px + (int64_t)v->rot[4]*py + (int64_t)v->rot[5]*pz) >> 12) + v->trans[1];
    const int32_t vz = (int32_t)(((int64_t)v->rot[6]*px + (int64_t)v->rot[7]*py + (int64_t)v->rot[8]*pz) >> 12) + v->trans[2];
    *vz_out = vz;
    if (vz <= 0) return 0;
    *sx = 160 + (int)(((int64_t)vx * v->fov_screen_dist) / vz);
    *sy = 120 + (int)(((int64_t)vy * v->fov_screen_dist) / vz);
    return 1;
}
static void raster_punkt(uint8_t *r, const re15_camera_view_t *v, const int32_t p[3])
{
    int sx = 0, sy = 0; int32_t vz = 0;
    if (!proj_cut(v, p, &sx, &sy, &vz)) return;
    const int rad = (int)(((int64_t)P_KOERPER_R * v->fov_screen_dist) / vz);
    for (int cy = 0; cy < P_RASTER_H; cy++)
        for (int cx = 0; cx < P_RASTER_W; cx++) {
            const int dx = cx * 5 + 2 - sx, dy = cy * 5 + 2 - sy;
            if (dx * dx + dy * dy <= rad * rad) r[cy * P_RASTER_W + cx] = 1;
        }
}
/* Ein Bild: Knochen + Zwischenpunkte der Knochen-Segmente. */
static void kam_bild(const re15_actor_t *g, const re15_camera_view_t *v, int cut, int f, kam_mass_t *m)
{
    static uint8_t ras[P_RASTER_W * P_RASTER_H];
    memset(ras, 0, sizeof ras);
    const re15_enemy_bank_t *bk = re15_enemy_find(g->type);
    if (!bk || !bk->ok) return;
    const re15_camera_cut_t *c = &g_room_rdt.cuts[cut];
    int32_t p[RE15_EMD_MAX_BONES][3];
    int ok[RE15_EMD_MAX_BONES];
    const int nb = bk->skel.bone_count;
    for (int b = 0; b < nb; b++) {
        ok[b] = re15_gr2_bone_welt_pub(g, b, p[b]);
        if (!ok[b]) continue;
        const int64_t dx = (int64_t)p[b][0] - c->pos_x, dy = (int64_t)p[b][1] - c->pos_y, dz = (int64_t)p[b][2] - c->pos_z;
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dy * dy + dz * dz));
        if (m->teil_min < 0 || d < m->teil_min) m->teil_min = d;
        int sx = 0, sy = 0; int32_t vz = 0;
        if (proj_cut(v, p[b], &sx, &sy, &vz) && sx >= 0 && sx < P_BILD_B && sy >= 0 && sy < P_BILD_H) {
            if (m->tiefe_min < 0 || vz < m->tiefe_min) m->tiefe_min = vz;
            if (m->erstes_im_bild < 0) m->erstes_im_bild = f;
            if (b == 0 && m->wurzel_im_bild < 0) m->wurzel_im_bild = f;
        }
        raster_punkt(ras, v, p[b]);
        const int q = bk->skel.bone_parent[b];
        if (q >= 0 && q < b && ok[q])
            for (int s = 1; s < 4; s++) {
                const int32_t z[3] = { p[q][0] + (p[b][0] - p[q][0]) * s / 4, p[q][1] + (p[b][1] - p[q][1]) * s / 4,
                                       p[q][2] + (p[b][2] - p[q][2]) * s / 4 };
                raster_punkt(ras, v, z);
            }
    }
    const int64_t wx = (int64_t)g->x - c->pos_x, wz = (int64_t)g->z - c->pos_z;
    const int32_t wd = (int32_t)re15_squareroot0((uint32_t)(wx * wx + wz * wz));
    if (m->wurzel_min < 0 || wd < m->wurzel_min) m->wurzel_min = wd;
    int n = 0, nr = 0;
    for (int cy = 0; cy < P_RASTER_H; cy++)
        for (int cx = 0; cx < P_RASTER_W; cx++)
            if (ras[cy * P_RASTER_W + cx]) { n++; if (cx >= P_RASTER_W / 2) nr++; }
    const int ganz = n * 100 / (P_RASTER_W * P_RASTER_H), rechts = nr * 200 / (P_RASTER_W * P_RASTER_H);
    if (ganz > m->ganz_max) m->ganz_max = ganz;
    if (rechts > m->rechts_max) m->rechts_max = rechts;
    if (rechts > 25) m->rechts25++;
    if (rechts > 50) m->rechts50++;
    m->bilder++;
}
static void kam_messen(const re15_actor_t *g, int cut, int bilder, kam_mass_t *m, void (*je_bild)(int f, const kam_mass_t *m))
{
    memset(m, 0, sizeof *m);
    m->wurzel_min = m->teil_min = m->tiefe_min = -1; m->erstes_im_bild = m->wurzel_im_bild = -1; m->biss = m->treffer = -1;
    re15_camera_view_t v;
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut >= g_room_rdt.cut_count || re15_camera_build_view(&g_room_rdt.cuts[cut], &v) != 0) return;
    kam_bild(g, &v, cut, 0, m);
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int hp0 = pl->hp;
    for (int f = 1; f <= bilder; f++) {
        frame(0, 0);
        kam_bild(g, &v, cut, f, m);
        if (m->biss < 0 && g->motion == 4) m->biss = f;
        if (m->treffer < 0 && pl->hp < hp0) m->treffer = f;
        if (je_bild) je_bild(f, m);
    }
}

/* ---- Werkzeug lagesuche2 ------------------------------------------------------------------------------------------------- */
static void lage2_eine(int32_t x, int32_t z, int ry, int gier, int bis, int rx, int rz)
{
    re15_gsz_lage_t l;
    re15_gatorszene_r41_lage_setzen(NULL);
    re15_gatorszene_r41_lage(&l);
    l.x = x; l.z = z; l.rot_y = (int16_t)ry; l.gier = (int16_t)gier; l.durchbruch_bis = (int16_t)bis;
    if (rx != 99999) { l.key_rx = (int16_t)rx; l.key_rz = (int16_t)rz; }
    re15_gatorszene_r41_lage_setzen(&l);
    int ruhe = -1, spitze = -1, best_r = -1, best_s = -1, best_rx = 0, best_rz = 0, best_b = 0, best_h = 0;
    int32_t key_tiefe = 0;
    re15_game_state_init();
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) == 0) {
        re15_gatorszene_r41_key_prim_jetzt(0, &ruhe, &spitze);
        const int ks = prop_von(RE15_GSZ_KEY_OBJ);
        if (ks >= 0) {
            const int16_t ax = g_scd.props[ks].rot_x, az = g_scd.props[ks].rot_z;
            int best_w = -1;
            for (int a = -640; a <= 640; a += 32)
                for (int b = -640; b <= 640; b += 32) {
                    int r2 = -1, s2 = -1, kb = 0, kh = 0;
                    g_scd.props[ks].rot_x = (int16_t)a; g_scd.props[ks].rot_z = (int16_t)b;
                    if (!re15_gatorszene_r41_key_prim_jetzt(0, &r2, &s2) || s2 > 0x80 || r2 < 84 || s2 < 125) continue;
                    if (!re15_gatorszene_r41_key_kasten(0, &kb, &kh, &key_tiefe)) continue;
                    const int w = kb * 9 < kh * 10 ? kb * 9 : kh * 10;       /* groesstes min(b/10, h/9), Massstab F-E2 Punkt 2 */
                    if (w > best_w) { best_w = w; best_r = r2; best_s = s2; best_rx = a; best_rz = b; best_b = kb; best_h = kh; }
                }
            g_scd.props[ks].rot_x = ax; g_scd.props[ks].rot_z = az;
        }
    }
    re15_actor_t *g = szene_bis_kampf(0);
    int fr = 0, fc = re15_gatorszene_r41_fress_kamera(&fr);
    int32_t ab = -1; int zg = -1, zs = -1;
    const int kb = re15_gatorszene_r41_kampfbeginn(&ab, &zg, &zs);
    int kontakt = -1, biss = -1, treffer = -1;
    kam_mass_t m; memset(&m, 0, sizeof m);
    int32_t wx = 0, wz = 0, wg = 0;
    if (g) {
        wx = g->x; wz = g->z; wg = g->rot_y;
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int hp0 = pl->hp;
        kam_messen(g, 0, P_MASS_BILDER, &m, NULL);
        biss = m.biss; treffer = m.treffer;
        for (int f = P_MASS_BILDER + 1; f <= 400 && (kontakt < 0 || biss < 0 || treffer < 0); f++) {
            frame(0, 0);
            if (kontakt < 0) kontakt = re15_gatorszene_r41_kontakt();
            if (biss < 0 && g->motion == 4) biss = f;
            if (treffer < 0 && pl->hp < hp0) treffer = f;
        }
        if (kontakt < 0) kontakt = re15_gatorszene_r41_kontakt();
    }
    printf("LAGE2 %d %d %d %d %d %d %d | prim %d/%d best %d/%d rx %d rz %d kasten %d %d tiefe %d | fress Cut %d frontal %d | kampf %s ab %d zone %d/%d wurzel %d %d gier %d | "
           "kontakt %d biss %d treffer %d | kam w %d t %d tiefe %d rechts %d ganz %d r25 %d im %d wim %d\n",
           (int)x, (int)z, ry, gier, bis, (int)l.key_rx, (int)l.key_rz, ruhe, spitze, best_r, best_s, best_rx, best_rz, best_b, best_h, (int)key_tiefe, fc, fr,
           g && kb ? "ok" : "NEIN", (int)ab, zg, zs, (int)wx, (int)wz, (int)wg, kontakt, biss, treffer,
           (int)m.wurzel_min, (int)m.teil_min, (int)m.tiefe_min, m.rechts_max, m.ganz_max, m.rechts25, m.erstes_im_bild, m.wurzel_im_bild);
    re15_gatorszene_r41_lage_setzen(NULL);
}
static int lagesuche2(const char *datei)
{
    FILE *f = fopen(datei, "r");
    if (!f) { printf("lagesuche2: %s fehlt\n", datei); return 1; }
    re15_door_seq_setze_laeufer(fang);
    char zeile[256];
    while (fgets(zeile, sizeof zeile, f)) {
        int x, z, ry, gier, bis, rx = 99999, rz = 0;
        const int n = sscanf(zeile, "%d %d %d %d %d %d %d", &x, &z, &ry, &gier, &bis, &rx, &rz);
        if (n < 5) continue;
        if (n < 7) rx = 99999;
        lage2_eine(x, z, ry, gier, bis, rx, rz);
        fflush(stdout);
    }
    fclose(f);
    re15_door_seq_setze_laeufer(NULL);
    return 0;
}

/* ---- Teil: kamera (Nachtrag 31 Punkt 2) ---------------------------------------------------------------------------------- */
static void kamera_zeile(int f, const kam_mass_t *m)
{
    if (f % 30 == 0)
        printf("    +%d: Wurzel-min %d, Teil-min %d, Tiefe-min %d, rechts-max %d %%, ganz-max %d %%\n", f, (int)m->wurzel_min, (int)m->teil_min,
               (int)m->tiefe_min, m->rechts_max, m->ganz_max);
}
/* Schranken (Riegel-Form, Nutzer-Mass "Abstand der Gator-Teile zur Kamera", Nachtrag 31 Punkt 2): vorher 1755 / 610 (Lage der Nachbesserung 5),
 * Lagewahl 4007 / 2684. Die Gegenprobe faehrt die Lage der Nachbesserung 5 und verlangt das Scheitern beider Schranken. */
#define P_WURZEL_MIN 3800
#define P_TEIL_MIN   2500
static void teil_kamera(int elza)
{
    printf("== kamera%s ==\n", elza ? "_elza" : "");
    re15_actor_t *g = szene_bis_kampf(elza);
    PRUEF(g != NULL, "Szene ueber die Leiter bis zum Kampfbeginn (ROOM%04X)", 0x2090 + elza);
    if (!g) return;
    kam_mass_t m;
    printf("    Kampfbeginn: Gator-Wurzel (%d,%d) Gier %d\n", g->x, g->z, (int)g->rot_y);
    kam_messen(g, 0, P_MASS_BILDER, &m, kamera_zeile);
    printf("    Mass: %d Bilder, Wurzel-min %d, Teil-min %d, Tiefe-min %d, erstes Bild im Bild %d, Wurzel im Bild ab %d\n", m.bilder,
           (int)m.wurzel_min, (int)m.teil_min, (int)m.tiefe_min, m.erstes_im_bild, m.wurzel_im_bild);
    PRUEF(m.bilder == P_MASS_BILDER + 1 && (int)g_scd.cam_id == 0, "Cut 0 (laufende Kamera) ueber %d Bilder", m.bilder);
    PRUEF(m.wurzel_min >= P_WURZEL_MIN, "Gator-Wurzel <-> Kamera Cut 0 mindestens %d (gemessen %d, vorher 1755)", P_WURZEL_MIN, (int)m.wurzel_min);
    PRUEF(m.teil_min >= P_TEIL_MIN, "jeder Gator-Teil <-> Kamera Cut 0 mindestens %d (gemessen %d, vorher 610)", P_TEIL_MIN, (int)m.teil_min);
    PRUEF(m.erstes_im_bild == 0, "Gator ab dem ersten Bild zu sehen (Knochen im Bild ab +%d): im Becken, nicht ausserhalb", m.erstes_im_bild);
    PRUEF(m.wurzel_im_bild >= 0 && m.wurzel_im_bild <= P_MASS_BILDER, "Rumpf im Bild ab +%d (bis +%d: VOR Leon im Becken)", m.wurzel_im_bild, P_MASS_BILDER);
    /* Gegenprobe: Lage der Nachbesserung 5 */
    re15_gsz_lage_t l;
    re15_gatorszene_r41_lage(&l);
    const re15_gsz_lage_t neu = l;
    l.x = 0; l.z = -26000; l.rot_y = 2944; l.gier = 1952;
    re15_gatorszene_r41_lage_setzen(&l);
    g = szene_bis_kampf(elza);
    PRUEF(g != NULL, "Gegenprobe: Szene mit der Lage der Nachbesserung 5");
    if (g) {
        kam_mass_t v;
        kam_messen(g, 0, P_MASS_BILDER, &v, NULL);
        PRUEF(v.wurzel_min < P_WURZEL_MIN && v.teil_min < P_TEIL_MIN, "Gegenprobe (0,-26000) / Anlauf 1952: Wurzel %d < %d, Teil %d < %d (der Riegel "
              "faengt den Befund der Abnahme F 5)", (int)v.wurzel_min, P_WURZEL_MIN, (int)v.teil_min, P_TEIL_MIN);
    }
    re15_gatorszene_r41_lage_setzen(&neu);
}

/* ---- Auswerter bildanteil (exe-Riegel integration_r41_alligator2_kamera*): Vordergrund je Bild gegen den Median-Hintergrund ------------
 * Mass der Abnahme F 5 (Hilfswerkzeug anteil.py): Referenz = Median je Pixel/Kanal ueber jedes 4. Bild f_r0..f_r1 (feste Kamera), Vordergrund =
 * max-Kanal-Differenz > 10. Verlangt: rechte Bildhaelfte in f0..f1 im Median hoechstens P_RECHTS_MEDIAN %, kein Bild > 50 % (vorher Spitze 51,9 %,
 * 23 Bilder > 50 %, Lauf F41a5/L1). Der Median, weil einzelne Bilder durch das Flimmern des Wasserbodens ueber die Schwelle 10 springen (gemessen
 * Lauf D_L1: F394 45 %, Nachbarbilder 27 %, dieselbe Gator-Pose). */
#define P_RECHTS_MEDIAN 35
static int bildanteil(const char *d, int r0, int r1, int f0, int f1)
{
    enum { MAX_REF = 128 };
    unsigned char *ref[MAX_REF]; int nref = 0, w = 0, h = 0;
    for (int f = r0; f <= r1 && nref < MAX_REF; f += 4) {
        char p[700]; int wa, ha; unsigned char *a = NULL;
        snprintf(p, sizeof p, "%s/fd_%06d.ppm", d, f);
        if (!ppm_lesen(p, &wa, &ha, &a)) continue;
        if (nref && (wa != w || ha != h)) { free(a); continue; }
        w = wa; h = ha; ref[nref++] = a;
    }
    if (nref < 10) { printf("bildanteil: nur %d Referenzbilder -> FEHLER\n", nref); return 1; }
    const size_t nb = (size_t)w * (size_t)h * 3u;
    unsigned char *med = (unsigned char *)malloc(nb);
    if (!med) return 1;
    for (size_t i = 0; i < nb; i++) {
        int hist[256]; memset(hist, 0, sizeof hist);
        for (int k = 0; k < nref; k++) hist[ref[k][i]]++;
        int s = 0, v = 0;
        for (; v < 256; v++) { s += hist[v]; if (s * 2 >= nref) break; }
        med[i] = (unsigned char)v;
    }
    for (int k = 0; k < nref; k++) free(ref[k]);
    int bilder = 0, spitze_r = 0, spitze_g = 0, n25 = 0, n50 = 0, f_sp = -1, fehlt = 0, hist_r[101];
    memset(hist_r, 0, sizeof hist_r);
    for (int f = f0; f <= f1; f++) {
        char p[700]; int wa, ha; unsigned char *a = NULL;
        snprintf(p, sizeof p, "%s/fd_%06d.ppm", d, f);
        if (!ppm_lesen(p, &wa, &ha, &a) || wa != w || ha != h) { free(a); fehlt++; continue; }
        long ges = 0, rechts = 0;
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                const unsigned char *q = a + ((size_t)y * w + x) * 3, *m = med + ((size_t)y * w + x) * 3;
                int dm = 0;
                for (int c = 0; c < 3; c++) { const int dv = q[c] > m[c] ? q[c] - m[c] : m[c] - q[c]; if (dv > dm) dm = dv; }
                if (dm > 10) { ges++; if (x >= w / 2) rechts++; }
            }
        free(a);
        bilder++;
        const int pr = (int)(rechts * 200 / ((long)w * h)), pg = (int)(ges * 100 / ((long)w * h));
        if (pr > spitze_r) { spitze_r = pr; f_sp = f; }
        if (pg > spitze_g) spitze_g = pg;
        if (pr > 25) n25++;
        if (pr > 50) n50++;
        hist_r[pr > 100 ? 100 : pr]++;
    }
    free(med);
    int median_r = 0;
    for (int z = 0; median_r < 100; median_r++) { z += hist_r[median_r]; if (z * 2 >= bilder) break; }
    const int ok = bilder >= f1 - f0 - 1 && fehlt <= 2 && median_r <= P_RECHTS_MEDIAN && n50 == 0;
    printf("bildanteil: %d Bilder F%d-F%d (%d Referenzen F%d-F%d): rechte Haelfte Median %d %% (hoechstens %d), Spitze %d %% (F%d), ganzes Bild Spitze %d %%, Bilder > 25 %%: %d, "
           "> 50 %%: %d -> %s\n", bilder, f0, f1, nref, r0, r1, median_r, P_RECHTS_MEDIAN, spitze_r, f_sp, spitze_g, n25, n50, ok ? "OK" : "FEHLER");
    return ok ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "lagesuche2") && argc >= 3) { if (!gator_bank()) return 1; return lagesuche2(argv[2]); }
    if (!strcmp(teil, "gsz")) return gsz_haupt(argc - 1, argv + 1);    /* Durchreiche auf die Teile von test_r41_gatorszene */
    if (!strcmp(teil, "bildanteil") && argc >= 7) return bildanteil(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    if (!gator_bank()) { printf("RE2-EM23-Bank fehlt\n"); return 1; }
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "kamera"))      teil_kamera(0);
    if (alle || !strcmp(teil, "kamera_elza")) teil_kamera(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
