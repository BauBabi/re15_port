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
 *   schluessel            FG-E7 Option (a): Schluessel-Lagewahl sucht weiter, Scan ueber das ganze Becken mindestens 10,4 Bildpunkte
 *   tragen                Abnahme G 4 O1: mit dem Kanister im Maul kein Lagesprung (Bildschritt <= 340), Gegenprobe harte Klemme
 * Nachbesserung 1 (Abnahme 0, AUFTRAG.md Nachtrag 35):
 *   erreichbar            Schluessel an jeder Todeslage auf einem erreichbaren Punkt, Klettersonde greift ihn nie, echte Aktionstaste aus schraeger Stellung nimmt ihn auf
 *   weg                   echter Lauf ueber die Eingabe bis zum Aufheben, auch quer durch den Westkanal
 *   koerper               R1 + Leiche schieben die Figur nicht (RE2 +0 |= 0x80a), Gegenprobe lebend, keine Zielhilfe auf die Leiche
 *   finisher              Leon in Clip 4/5 sichtbar, ab Clip 11 Modell aus (RE2 |= 0x80000 @0x80101190-ac)
 *   leiche / leiche_elza  Rumpfmitte der Leiche sitzt im Maul und folgt Bild fuer Bild, mit dem Clip-5-Ende weg
 *   kanister_sitz         Tank-Achse = Quer-Achse des Kopfes bei jeder Gier wie RE2, Lokallage (1750,-210,-262)
 * Werkzeug (kein Riegel):
 *   lagesuche2 <datei>    wie lagesuche von test_r41_gatorszene, zusaetzlich Kampfbeginn-Lage und Kamera-Mass
 *   erreichbar_scan       teil erreichbar mit Raster 1000 (ohne Taste)
 *   wegplan ...           Eingabe-Skript des exe-Riegels integration_r41_alligator2_schluessel*
 *   fxspur ...            Spur eines RE2-Effekts der Raum-ESP von ROOM40A0
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

/* ---- Teil: schluessel (FG-E7 Option (a)) ----------------------------------------------------------------------------------
 * Leiche im Zustand 7 an einer Lage, Leon an (px,pz): die Lagewahl des Schluessels wie im Spiel (Szenen-Takt). Rueckgabe Bild x 10. */
static int tod_lage(int32_t gx, int32_t gz, int16_t gier, int32_t px, int32_t pz, int cam, int *cut, int *stufe, int32_t *d_min)
{
    *cut = -1; *stufe = -1; *d_min = -1;
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(0x2090, px, 0, pz, 0, (uint8_t)cam) != 0 || !gator_bank()) return -1;
    re15_enemy_ai_set_paused(1);
    re15_actor_t *g = gator_setzen();
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    if (!g || !bk || !bk->ok) return -1;
    g->x = gx; g->z = gz; g->y = -1200; g->rot_y = gier; g->state = 7;
    g->render_scale_q12 = 2731;                                /* GB_SCALE_Q12 = 2/3 */
    g->motion = 0; g->anim_frame = 0; g->anim_frac = 0; g->anim_prev_valid = 0;
    g_scd.cam_id = (uint8_t)cam; g_scd.work_vars[0x0A] = (int16_t)cam; s_shown = cam;
    static int32_t kn[64][3]; int nb = 0;
    for (int b = 0; b < bk->skel.bone_count && nb < 64; b++) if (re15_gr2_bone_welt_pub(g, b, kn[nb])) nb++;
    frame(0, 0);
    *cut = re15_gatorszene_r41_key_kamera(); *stufe = re15_gatorszene_r41_key_stufe();
    const int ks = prop_von(RE15_GSZ_KEY_OBJ);
    if (ks < 0 || *cut < 0) return 0;
    int32_t dm = 0x7fffffff;
    for (int b = 0; b < nb; b++) {
        const int64_t dx = (int64_t)kn[b][0] - g_scd.props[ks].x, dz = (int64_t)kn[b][2] - g_scd.props[ks].z;
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        if (d < dm) dm = d;
    }
    *d_min = dm;
    int kb = 0, kh = 0; int32_t tiefe = 0;
    if (!re15_gatorszene_r41_key_kasten(*cut, &kb, &kh, &tiefe) || tiefe <= 0) return 0;
    return (int)(1800 * 208 / tiefe);                          /* Quadrat 180 x H / Tiefe, x 10 (H = 208, RID @0x060) */
}
/* FG-E7 Option (a): ueber das ganze Becken (Raster schritt, vier Gieren, Leon an der Leiter und mitten im Suedbecken) zeigt die Lagewahl den Schluessel
 * mit mindestens 10,4 Bildpunkten; die Stufen zaehlen, wie oft sie weitersuchen musste. Voller Lauf (Raster 1000, Werkzeug schluessel_scan): 1688 Lagen,
 * Stufe 0: 1595, 1: 48, 2: 33, 3: 12, 4: 0, unter 10,4: 0. */
static void schluessel_scan(int schritt, int zeigen, int *lagen, int *klein, int st[5], int32_t *d_max)
{
    static const int32_t sp[2][2] = { { -8200, -25550 }, { 1325, -25550 } };
    *lagen = 0; *klein = 0; *d_max = 0;
    for (int i = 0; i < 5; i++) st[i] = 0;
    for (int p = 0; p < 2; p++)
        for (int32_t z = -26000; z <= -6500; z += schritt)
            for (int32_t x = -8000; x <= 6000; x += schritt) {
                if (x > -1700 - 1466 && x < 1850 + 1466 && z > -20000 - 1466 && z < -12450 + 1466) continue;   /* Plattform-Block: keine Leichenlage */
                if (x > 1850 && z > -18100 && z < -14500) continue;                                            /* Rampe */
                for (int gi = 0; gi < 4; gi++) {
                    int c, stf; int32_t dm;
                    const int px = tod_lage(x, z, (int16_t)(gi * 1024), sp[p][0], sp[p][1], 0, &c, &stf, &dm);
                    if (c < 0) continue;
                    (*lagen)++;
                    if (stf >= 0 && stf < 5) st[stf]++;
                    if (px < 104) (*klein)++;
                    if (dm > *d_max) *d_max = dm;
                    if (zeigen && (stf > 0 || px < 104))
                        printf("SCAN Leon (%d,%d) Leiche (%d,%d) Gier %d: Cut %d Stufe %d Bild %d.%d, %d von der Leiche\n", (int)sp[p][0], (int)sp[p][1],
                               (int)x, (int)z, gi * 1024, c, stf, px / 10, px % 10, (int)dm);
                }
            }
    printf("    Scan Raster %d: %d Lagen, Stufe 0: %d, 1: %d, 2: %d, 3: %d, 4: %d, unter 10,4: %d, groesster Abstand zur Leiche %d\n", schritt, *lagen, st[0], st[1],
           st[2], st[3], st[4], *klein, (int)*d_max);
}
static void teil_schluessel_p(void)
{
    printf("== schluessel (FG-E7 Option (a)) ==\n");
    int c, stf; int32_t dm;
    int px = tod_lage(-4000, -17000, 0, -8200, -25550, 0, &c, &stf, &dm);
    PRUEF(px >= 104 && stf >= 1, "Westkanal (-4000,-17000): die Lagewahl sucht weiter (Stufe %d), Cut %d, Bild %d.%d >= 10,4, %d von der Leiche", stf, c, px / 10,
          px % 10, (int)dm);
    px = tod_lage(3413, -25001, 2048, -8200, -25550, 1, &c, &stf, &dm);
    PRUEF(px >= 104 && stf == 0, "Fall r1 (3413,-25001): Naehe wie bisher reicht (Stufe %d), Cut %d, Bild %d.%d", stf, c, px / 10, px % 10);
    int n, kl, st[5]; int32_t d_max;
    schluessel_scan(3000, 0, &n, &kl, st, &d_max);
    PRUEF(n > 100 && kl == 0, "Becken-Scan: %d Lagen, keine unter 10,4 Bildpunkten (%d)", n, kl);
    PRUEF(st[1] + st[2] + st[3] + st[4] > 0 && d_max <= 2 * 3100, "Scan: die Stufen 1..4 kommen vor (%d), Schluessel hoechstens %d von der Leiche (doppelte Naehe)",
          st[1] + st[2] + st[3] + st[4], (int)d_max);
}

/* ---- Teil: tragen (Lagespruenge mit dem Kanister im Maul, Abnahme G 4 O1) ----------------------------------------------------
 * Echter Boss-Takt: Szene bis zum Kampf, Alligator an (x,z) Gier gier, Kanister vor dem Maul (Form teil_kopfklemme test_r41_re2gator.c: Wurzel fest,
 * 8800 x Skala voraus @Fenster), Aufnahme, dann frei: bilder Takte mit stehendem Spieler an (lx,lz). Mass je Bildpaar = Wurzelschritt (Sicht-Messzeile der
 * Spur B, Schranke 340 = groesster authored Wurzelschritt, integration_r41_re2gator). */
typedef struct {
    int     bilder, s300, s340, s400, s600, s1000;
    int32_t max_d; int max_f; int32_t max_von[2], max_nach[2]; int max_clip;
    int32_t ueber_max;       /* groesster Ueberstand von Kopf/Kanister ueber die Aussenwaende */
    int     ueber_n;         /* Bilder mit Ueberstand > 300 (Rand der Ueberblendung, teil_kopfklemme) */
} trag_mass_t;
static int32_t ueberstand(const int32_t p[3])
{
    int32_t u = 0;
    if (-8900 - p[0] > u) u = -8900 - p[0];
    if (p[0] - 7200 > u) u = p[0] - 7200;
    if (-27000 - p[2] > u) u = -27000 - p[2];
    if (p[2] + 5400 > u) u = p[2] + 5400;
    return u;
}
static int trag_lauf(int32_t x, int32_t z, int gier, int32_t lx, int32_t lz, int bilder, trag_mass_t *m, int zeige)
{
    memset(m, 0, sizeof *m);
    re15_actor_t *g = szene_bis_kampf(0);
    if (!g) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier) * 8800 * sk) >> 24);
    if (!re15_aot_water_at(x + vx, z + vz)) return 0;          /* der geworfene Kanister liegt im Wasser (kanister2080_r41.c Aufschlag) */
    re15_gr2_kanister_setzen(x + vx, -1200, z + vz);
    for (int f = 0; f < 200 && !re15_gr2_traegt(); f++) {
        g->x = x; g->z = z; g->rot_y = (int16_t)gier;
        pl->x = lx; pl->z = lz; pl->hp = 100;
        frame(0, 0);
    }
    if (!re15_gr2_traegt()) return 0;
    int32_t ax = g->x, az = g->z;
    for (int f = 1; f <= bilder && re15_gr2_traegt(); f++) {
        pl->x = lx; pl->z = lz; pl->hp = 100;
        frame(0, 0);
        const int64_t dx = (int64_t)g->x - ax, dz = (int64_t)g->z - az;
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        m->bilder++;
        if (d > 300) m->s300++;
        if (d > 340) m->s340++;
        if (d > 400) m->s400++;
        if (d > 600) m->s600++;
        if (d > 1000) m->s1000++;
        if (d > m->max_d) { m->max_d = d; m->max_f = f; m->max_von[0] = ax; m->max_von[1] = az; m->max_nach[0] = g->x; m->max_nach[1] = g->z; m->max_clip = g->motion; }
        if (zeige > 1) printf("    F+%d d %d (%d,%d) Gier %d Clip %d Bild %d\n", f, (int)d, g->x, g->z, (int)g->rot_y, (int)g->motion, (int)g->anim_frame);
        if (zeige && d > 340) printf("    F+%d Sprung %d (%d,%d) -> (%d,%d) Gier %d Clip %d Bild %d\n", f, (int)d, (int)ax, (int)az, g->x, g->z, (int)g->rot_y, (int)g->motion, (int)g->anim_frame);
        ax = g->x; az = g->z;
        int32_t kp[3], kr[9], u = 0;
        if (re15_gr2_kanister_welt(kp, kr)) {
            const int32_t fp[3] = { kp[0] + (int32_t)(((int64_t)kr[1] * -2784) >> 12), kp[1], kp[2] + (int32_t)(((int64_t)kr[7] * -2784) >> 12) };
            const int32_t u1 = ueberstand(kp), u2 = ueberstand(fp);
            u = u1 > u2 ? u1 : u2;
        }
        int32_t k6[3];
        if (re15_gr2_bone_welt_pub(g, 6, k6)) { const int32_t u3 = ueberstand(k6); if (u3 > u) u = u3; }
        if (u > m->ueber_max) m->ueber_max = u;
        if (u > 300) m->ueber_n++;
    }
    return 1;
}
static void trag_scan(int zeige)
{
    static const int32_t leon[6][2] = { { -8200, -25550 }, { -6000, -17500 }, { 0, -7000 }, { -8000, -16000 }, { 5800, -26500 }, { 6000, -21000 } };
    static const int32_t start[][2] = { { -5173, -18311 }, { -6096, -21600 }, { -4000, -24000 }, { 3000, -23000 }, { -5000, -10000 }, { 2000, -8000 },
                                        { -6500, -15700 }, { -5500, -7000 }, { 5000, -24500 }, { 0, -25000 }, { -7000, -13000 }, { -3000, -22500 } };
    trag_mass_t ges; memset(&ges, 0, sizeof ges);
    int laeufe = 0, ohne = 0;
    for (size_t si = 0; si < sizeof start / sizeof start[0]; si++)
        for (int gi = 0; gi < 8; gi++)
            for (int li = 0; li < 6; li++) {
                trag_mass_t m;
                const int gier = gi * 512;
                if (!trag_lauf(start[si][0], start[si][1], gier, leon[li][0], leon[li][1], 900, &m, 0)) { ohne++; continue; }
                laeufe++;
                ges.bilder += m.bilder; ges.s300 += m.s300; ges.s340 += m.s340; ges.s400 += m.s400; ges.s600 += m.s600; ges.s1000 += m.s1000; ges.ueber_n += m.ueber_n;
                if (m.ueber_max > ges.ueber_max) ges.ueber_max = m.ueber_max;
                if (m.max_d > ges.max_d) ges = (trag_mass_t){ ges.bilder, ges.s300, ges.s340, ges.s400, ges.s600, ges.s1000, m.max_d, m.max_f, { m.max_von[0], m.max_von[1] },
                                                             { m.max_nach[0], m.max_nach[1] }, m.max_clip, ges.ueber_max, ges.ueber_n };
                if (zeige && m.max_d > 340)
                    printf("TRAG Start (%d,%d) Gier %d Leon (%d,%d): %d Bilder, max %d F+%d (%d,%d)->(%d,%d) Clip %d, >340: %d, Ueberstand max %d, Bilder > 300: %d\n", (int)start[si][0],
                           (int)start[si][1], gier, (int)leon[li][0], (int)leon[li][1], m.bilder, (int)m.max_d, m.max_f, (int)m.max_von[0], (int)m.max_von[1], (int)m.max_nach[0],
                           (int)m.max_nach[1], m.max_clip, m.s340, (int)m.ueber_max, m.ueber_n);
            }
    printf("TRAGSCAN: %d Laeufe (%d ohne Aufnahme), %d Bilder, Sprung max %d (F+%d (%d,%d)->(%d,%d) Clip %d), > 300: %d, > 340: %d, > 400: %d, > 600: %d, > 1000: %d, Ueberstand max %d, "
           "Bilder Ueberstand > 300: %d\n", laeufe, ohne, ges.bilder, (int)ges.max_d, ges.max_f, (int)ges.max_von[0], (int)ges.max_von[1], (int)ges.max_nach[0], (int)ges.max_nach[1],
           ges.max_clip, ges.s300, ges.s340, ges.s400, ges.s600, ges.s1000, (int)ges.ueber_max, ges.ueber_n);
}

/* Riegel: Auswahl der Scan-Laeufe, die ohne die weiche Klemme springen (Scan trag_scan, Stand vor Spur P: > 340 in 946 Bildpaaren, > 1000 in 68). */
static void teil_tragen(void)
{
    printf("== tragen (Abnahme G 4 O1) ==\n");
    static const struct { int32_t x, z; int gier; int32_t lx, lz; } k[] = {
        { -6096, -21600, 3072, -8200, -25550 }, { -6096, -21600, 2560, -6000, -17500 }, { -7000, -13000, 3072, -8000, -16000 }, { 5000, -24500, 2560, 5800, -26500 },
        { 3000, -23000, 2560, 0, -7000 },       { 0, -25000, 2560, 0, -7000 },          { 2000, -8000, 1536, 6000, -21000 },    { 2000, -8000, 3584, 5800, -26500 },
        { -5500, -7000, 512, 0, -7000 },        { -3000, -22500, 1024, -8200, -25550 } };
    const int n = (int)(sizeof k / sizeof k[0]);
    for (int hart = 0; hart < 2; hart++) {
        re15_gr2_kk_hart_setzen(hart);
        trag_mass_t ges; memset(&ges, 0, sizeof ges);
        int laeufe = 0;
        for (int i = 0; i < n; i++) {
            trag_mass_t m;
            if (!trag_lauf(k[i].x, k[i].z, k[i].gier, k[i].lx, k[i].lz, 900, &m, 0)) continue;
            laeufe++;
            ges.bilder += m.bilder; ges.s340 += m.s340; ges.s600 += m.s600; ges.ueber_n += m.ueber_n;
            if (m.max_d > ges.max_d) ges.max_d = m.max_d;
            if (m.ueber_max > ges.ueber_max) ges.ueber_max = m.ueber_max;
        }
        printf("    %s: %d Laeufe, %d Bilder, Schritt max %d, > 340: %d, > 600: %d, Ueberstand max %d, Bilder Ueberstand > 300: %d\n", hart ? "Gegenprobe harte Klemme" : "weiche Klemme",
               laeufe, ges.bilder, (int)ges.max_d, ges.s340, ges.s600, (int)ges.ueber_max, ges.ueber_n);
        if (!hart) PRUEF(laeufe >= 7 && ges.max_d <= 340, "mit dem Kanister im Maul kein Lagesprung ueber den groessten authored Wurzelschritt 340 (max %d, %d Bildpaare, %d Laeufe)",
                         (int)ges.max_d, ges.bilder, laeufe);
        else PRUEF(ges.s340 > 0, "Gegenprobe: die harte Klemme springt (max %d, %d Bildpaare > 340) - der Riegel misst", (int)ges.max_d, ges.s340);
    }
    re15_gr2_kk_hart_setzen(0);
}

/* ---- Auswerter tragelog (exe-Riegel integration_r41_alligator2_tragen*): Wurzelschritte des Alligators je Bild aus state.log -----------------------------
 * Aufruf `test_r41_alligator2 tragelog <state.log> <f0> <f1>`: letzter Raum-Abschnitt (der Bildzaehler beginnt je Raum neu), Bildpaare f0..f1. */
static int tragelog(const char *pfad, int f0, int f1)
{
    FILE *fp = fopen(pfad, "r");
    if (!fp) { printf("tragelog: %s fehlt\n", pfad); return 1; }
    static char z[8192];
    int vor = -1, n = 0, s340 = 0, s600 = 0, lf = -1, mf = -1, ma = 0;
    int32_t lx = 0, lz = 0, maxd = 0;
    while (fgets(z, sizeof z, fp)) {
        int f = -1;
        if (z[0] != 'F' || sscanf(z + 1, "%d", &f) != 1) continue;
        if (f < vor) { n = 0; s340 = 0; s600 = 0; maxd = 0; lf = -1; mf = -1; ma = 0; }   /* neuer Raum-Abschnitt */
        vor = f;
        if (f < f0 || f > f1) continue;
        const char *t = strstr(z, " t=23 ");
        const char *a = t ? strchr(t, '@') : NULL;
        int x, zz;
        if (!a || sscanf(a, "@(%d,%d,", &x, &zz) != 2) { lf = -1; continue; }
        if (lf == f - 1) {
            const int64_t dx = (int64_t)x - lx, dz = (int64_t)zz - lz;
            const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
            n++;
            if (d > 340) s340++;
            if (d > 600) s600++;
            if (d > maxd) { maxd = d; mf = f; }
            ma = 1;
        }
        lf = f; lx = x; lz = zz;
    }
    fclose(fp);
    printf("tragelog: %d Bildpaare F%d-F%d, Wurzelschritt max %d (F%d), > 340: %d, > 600: %d\n", n, f0, f1, (int)maxd, mf, s340, s600);
    return (ma && n >= 50 && maxd <= 340) ? 0 : 1;
}

/* ==== Nachbesserung 1 (Abnahme 0, AUFTRAG.md Nachtrag 35) ==================================================================================== */
#include "re15_climb.h"
extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);   /* re15_damage.c */

/* ---- Reichweite der Figur: Flutfuellung mit der Spieler-Klemme auf Band 0 (Form Abnahme 0 p0_key.c), die Vorgaenger zeigen zum Ziel --------------- */
#define P_RX0 (-9800)
#define P_RX1 8000
#define P_RZ0 (-27800)
#define P_RZ1 (-4600)
#define P_RS  100
#define P_NX  ((P_RX1 - P_RX0) / P_RS + 1)
#define P_NZ  ((P_RZ1 - P_RZ0) / P_RS + 1)
static uint8_t s_r[P_NZ][P_NX];                 /* 0 = nicht erreicht, sonst 1 + Richtung zum Vorgaenger */
static int     s_q[P_NX * P_NZ][2];
static const int k_r8[8][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };
static int r_ix(int32_t x) { return (x - P_RX0 + P_RS / 2) / P_RS; }
static int r_iz(int32_t z) { return (z - P_RZ0 + P_RS / 2) / P_RS; }
static int r_innen(int ix, int iz) { return ix >= 0 && iz >= 0 && ix < P_NX && iz < P_NZ; }
static int fuellen(int32_t sx, int32_t sz)
{
    memset(s_r, 0, sizeof s_r);
    const int ix = r_ix(sx), iz = r_iz(sz);
    if (!r_innen(ix, iz)) return 0;
    re15_collision_set_band(0);
    int k = 0, e = 0, n = 1;
    s_r[iz][ix] = 1; s_q[e][0] = ix; s_q[e][1] = iz; e++;
    while (k < e) {
        const int cx = s_q[k][0], cz = s_q[k][1]; k++;
        const int32_t ox = P_RX0 + cx * P_RS, oz = P_RZ0 + cz * P_RS;
        for (int i = 0; i < 8; i++) {
            const int tx = cx + k_r8[i][0], tz = cz + k_r8[i][1];
            if (!r_innen(tx, tz) || s_r[tz][tx]) continue;
            int32_t nx = P_RX0 + tx * P_RS, nz = P_RZ0 + tz * P_RS;
            const int32_t wx = nx, wz = nz;
            re15_collision_constrain(&g_room_rdt, ox, oz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (nx != wx || nz != wz) continue;
            s_r[tz][tx] = (uint8_t)(1 + i); s_q[e][0] = tx; s_q[e][1] = tz; e++; n++;
        }
    }
    return n;
}
static int erreicht(int32_t x, int32_t z)
{
    const int ix = r_ix(x), iz = r_iz(z);
    return r_innen(ix, iz) && s_r[iz][ix] != 0;
}

/* ---- Laufen ueber die echte Eingabe: Lenken je Bild auf einen Punkt der Fuellung, Tasten wie im Spiel; skript != NULL haengt RE15_INPUT_SCRIPT-Tokens an --- */
typedef struct { uint16_t tasten; int bilder; } p_lauf_t;
static p_lauf_t s_lauf_tok;
static void lauf_tok_raus(char *skript, size_t cap)
{
    if (!skript || s_lauf_tok.bilder <= 0) return;
    char b[32]; int n = 0;
    if (s_lauf_tok.tasten & RE15_PAD_BIT_UP)    b[n++] = 'U';
    if (s_lauf_tok.tasten & RE15_PAD_BIT_CROSS) b[n++] = 'X';
    if (s_lauf_tok.tasten & RE15_PAD_BIT_LEFT)  b[n++] = 'L';
    if (s_lauf_tok.tasten & RE15_PAD_BIT_RIGHT) b[n++] = 'R';
    if (n == 0) b[n++] = 'W';
    b[n] = 0;
    const size_t l = strlen(skript);
    snprintf(skript + l, cap > l ? cap - l : 0, ",%s%.3f", b, s_lauf_tok.bilder / 30.0);
}
static void lauf_tok(char *skript, size_t cap, uint16_t tasten)
{
    if (tasten != s_lauf_tok.tasten) { lauf_tok_raus(skript, cap); s_lauf_tok.tasten = tasten; s_lauf_tok.bilder = 0; }
    s_lauf_tok.bilder++;
}
/* Laeuft zur Mitte (tx,tz) bis auf rest heran; Rueckgabe Bilder, -1 = nicht angekommen. */
static int laufen(int32_t tx, int32_t tz, int32_t rest, int max_bilder, char *skript, size_t cap)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (fuellen(tx, tz) <= 1) return -1;
    uint16_t alt = 0;
    memset(&s_lauf_tok, 0, sizeof s_lauf_tok);
    for (int f = 0; f < max_bilder; f++) {
        const int64_t dtx = (int64_t)tx - pl->x, dtz = (int64_t)tz - pl->z;
        if (dtx * dtx + dtz * dtz <= (int64_t)rest * rest) { lauf_tok_raus(skript, cap); s_lauf_tok.bilder = 0; frame(0, 0); return f; }
        int ix = r_ix(pl->x), iz = r_iz(pl->z);
        if (!r_innen(ix, iz) || !s_r[iz][ix]) {             /* an einer Kante: naechste Zelle der Fuellung */
            int bx = -1, bz = -1, bd = 1 << 30;
            for (int dz = -6; dz <= 6; dz++)
                for (int dx = -6; dx <= 6; dx++)
                    if (r_innen(ix + dx, iz + dz) && s_r[iz + dz][ix + dx] && dx * dx + dz * dz < bd) { bd = dx * dx + dz * dz; bx = ix + dx; bz = iz + dz; }
            if (bx < 0) { lauf_tok_raus(skript, cap); return -1; }
            ix = bx; iz = bz;
        }
        for (int i = 0; i < 8 && s_r[iz][ix] > 1; i++) {      /* acht Zellen voraus entlang der Vorgaenger */
            const int d = s_r[iz][ix] - 1;
            ix -= k_r8[d][0]; iz -= k_r8[d][1];
        }
        const int32_t wx = P_RX0 + ix * P_RS, wz = P_RZ0 + iz * P_RS;
        const int ziel = ((int)re15_atan2_q12(wz - pl->z, wx - pl->x) - 1024) & 0xfff;
        const int d = ((ziel - ((int)pl->rot_y & 0xfff) + 0x800) & 0xfff) - 0x800;
        uint16_t t = 0;
        if (d > 48) t |= RE15_PAD_BIT_RIGHT; else if (d < -48) t |= RE15_PAD_BIT_LEFT;
        if (d > -240 && d < 240) t |= RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS;
        lauf_tok(skript, cap, t);
        frame(t, (uint16_t)(t & ~alt));
        alt = t;
    }
    lauf_tok_raus(skript, cap);
    return -1;
}

/* Aufnahme ueber die echte Aktionstaste an der Lage (sx,sz) mit Blick rot: 1 = Water Key im Inventar, 2 = geklettert, 0 = nichts. */
static int aufheben(int32_t sx, int32_t sz, int16_t rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = rot; pl->y = 0;
    re15_collision_set_band(0);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    if (re15_climb_dbg_sub() == 9) return 2;
    if (!re15_item_modal_active()) return 0;
    int w = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
    w = 0;
    while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
    return re15_inv_find_item(RE15_GSZ_KEY_ITEM) >= 0 ? 1 : 0;
}

/* Blick von (x,z) auf (tx,tz) */
static int16_t blick(int32_t x, int32_t z, int32_t tx, int32_t tz) { return (int16_t)(((int)re15_atan2_q12(tz - z, tx - x) - 1024) & 0xfff); }

/* ---- Teil erreichbar (M1, M2, M3; Nachtrag 35 Punkte 1 + 3) -----------------------------------------------------------------------------------------
 * Je Todeslage (Raster, vier Gieren, Leon an der Leiter und im Suedbecken, dazu Wand- und natuerliche Lagen): Schluessel an einem Punkt, den die Fuellung ab der Leiter erreicht;
 * ueber alle Stellungen der Aufheb-Reichweite (Raster 200, 16 Blicke) startet die Klettersonde re15_climb_try_start nie auf den Schluessel (vorher 56 % der Stellungen,
 * Abnahme 0 kind 0x81 obj 4); die echte Aktionstaste an einer schraegen Stellung nimmt ihn auf (Stellung, an der Band 0 der Klettersonde vorher griff). */
typedef struct { int lagen, wand, kletter, stellungen, kein_aufheben; int32_t wx, wz; } err_mass_t;
static void err_eine(int32_t x, int32_t z, int gier, int32_t lx, int32_t lz, err_mass_t *m, int mit_taste, int zeigen)
{
    int c, stf; int32_t dm;
    (void)tod_lage(x, z, (int16_t)gier, lx, lz, 0, &c, &stf, &dm);
    if (c < 0) return;
    const int ks = prop_von(RE15_GSZ_KEY_OBJ);
    if (ks < 0) return;
    m->lagen++;
    const int32_t kx = g_scd.props[ks].x, kz = g_scd.props[ks].z;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!erreicht(kx, kz)) { m->wand++; m->wx = kx; m->wz = kz; if (zeigen) printf("    NICHT ERREICHT: Leiche (%d,%d) Gier %d, Schluessel (%d,%d)\n", (int)x, (int)z, gier, (int)kx, (int)kz); }
    re15_climb_reset();
    for (int32_t dz = -1400; dz <= 1400; dz += 200)
        for (int32_t dx = -1400; dx <= 1400; dx += 200) {
            const int32_t sx = kx + dx, sz = kz + dz;
            if (!erreicht(sx, sz)) continue;
            for (int a = 0; a < 4096; a += 256) {
                pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)a; pl->y = 0;
                re15_collision_set_band(0);
                re15_climb_standing_tick();
                m->stellungen++;
                (void)re15_climb_try_start(&g_room_rdt, 1);
                if (re15_climb_dbg_sub() == 9 && re15_climb_dbg_kind() == 0x81 && re15_climb_dbg_obj() == ks) {
                    if (m->kletter == 0 && zeigen) printf("    KLETTERN auf den Schluessel: Stellung (%d,%d) Blick %d\n", (int)sx, (int)sz, a);
                    m->kletter++;
                }
                re15_climb_reset();
            }
        }
    pl->x = lx; pl->z = lz; pl->y = 0;
    if (!mit_taste) return;
    /* schraege Stellung: 300 neben dem Schluessel in beiden Achsen (Sonde 800 voraus, auf 90 Grad gerastet: |dx|, |dz| >= 112 = der Fall der Null-Box) */
    int f = 0;
    while (f < 200 && re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL)) { frame(0, 0); f++; }
    for (f = 0; f < 20; f++) frame(0, 0);
    static const int k_d[4][2] = { { 300, 300 }, { -300, 300 }, { 300, -300 }, { -300, -300 } };
    int r = 0;
    for (int i = 0; i < 4 && r != 1; i++) {
        const int32_t sx = kx + k_d[i][0], sz = kz + k_d[i][1];
        if (!erreicht(sx, sz)) continue;
        r = aufheben(sx, sz, blick(sx, sz, kx, kz));
        if (r == 2) re15_climb_reset();
    }
    if (r != 1) { m->kein_aufheben++; if (zeigen) printf("    KEIN AUFHEBEN: Leiche (%d,%d) Gier %d, Schluessel (%d,%d): %d\n", (int)x, (int)z, gier, (int)kx, (int)kz, r); }
}
static void err_scan(int schritt, int mit_taste, int zeigen, err_mass_t *m)
{
    static const int32_t sp[2][2] = { { -8200, -25550 }, { 1325, -25550 } };
    memset(m, 0, sizeof *m);
    int c, stf; int32_t dm;
    (void)tod_lage(0, -20000, 0, -8200, -25550, 0, &c, &stf, &dm);
    const int nf = fuellen(-8200, -25500);
    printf("    Fuellung ab der Leiter: %d Zellen Raster %d\n", nf, P_RS);
    for (int p = 0; p < 2; p++)
        for (int32_t z = -26000; z <= -6500; z += schritt)
            for (int32_t x = -8000; x <= 6000; x += schritt) {
                if (x > -1700 - 1466 && x < 1850 + 1466 && z > -20000 - 1466 && z < -12450 + 1466) continue;   /* Plattform-Block, wie schluessel_scan */
                if (x > 1850 && z > -18100 && z < -14500) continue;
                for (int gi = 0; gi < 4; gi++) err_eine(x, z, gi * 1024, sp[p][0], sp[p][1], m, mit_taste, zeigen);
            }
}
static void teil_erreichbar(void)
{
    printf("== erreichbar (M1, M2, M3) ==\n");
    err_mass_t m;
    err_scan(3000, 1, 1, &m);
    /* Wand- und natuerliche Lagen: K3 (Rezept FG-E7), Abnahme 0 ken2/kln, Laeufe der Nachbesserung 1, Maschinenblock Nord, Westkanal */
    static const int32_t k_l[][3] = { { 3413, -23942, 2048 }, { 3413, -25001, 2048 }, { -5676, -19980, 2095 }, { -5835, -19103, 2000 }, { -6963, -9962, 2800 }, { -5943, -12800, 2800 },
                                      { 1500, -9500, 0 }, { 1500, -6500, 1024 }, { -8000, -22000, 0 }, { -3000, -8000, 2048 }, { 6400, -7000, 3072 }, { -2500, -17000, 1024 }, { 6400, -25500, 1024 } };
    for (size_t i = 0; i < sizeof k_l / sizeof k_l[0]; i++) err_eine(k_l[i][0], k_l[i][1], k_l[i][2], -8200, -25550, &m, 1, 1);
    printf("    %d Todeslagen, %d Stellungen, Schluessel nicht erreicht: %d, Klettern auf den Schluessel: %d, Aufnahme fehlgeschlagen: %d\n", m.lagen, m.stellungen, m.wand, m.kletter, m.kein_aufheben);
    PRUEF(m.lagen > 150 && m.wand == 0, "Schluessel an jeder der %d Todeslagen auf einem Punkt, den die Figur erreicht (%d nicht)", m.lagen, m.wand);
    PRUEF(m.stellungen > 10000 && m.kletter == 0, "Klettersonde greift in %d Stellungen nie den Schluessel (%d)", m.stellungen, m.kletter);
    PRUEF(m.kein_aufheben == 0, "Aktionstaste aus schraeger Stellung nimmt ihn an jeder Lage auf (%d nicht)", m.kein_aufheben);
    {   /* Gegenprobe: mit Band 0 (vorher) greift die Sonde den Schluessel - der Riegel misst */
        int c, stf; int32_t dm;
        (void)tod_lage(0, -20000, 0, -8200, -25550, 0, &c, &stf, &dm);
        const int ks = prop_von(RE15_GSZ_KEY_OBJ);
        err_mass_t g; memset(&g, 0, sizeof g);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (ks >= 0) {
            const int32_t kx = g_scd.props[ks].x, kz = g_scd.props[ks].z;
            g_scd.props[ks].band = 0;
            for (int32_t dz = -1400; dz <= 1400; dz += 200)
                for (int32_t dx = -1400; dx <= 1400; dx += 200)
                    for (int a = 0; a < 4096; a += 256) {
                        if (!erreicht(kx + dx, kz + dz)) continue;
                        pl->x = kx + dx; pl->z = kz + dz; pl->rot_y = (int16_t)a; pl->y = 0;
                        re15_collision_set_band(0);
                        re15_climb_standing_tick();
                        g.stellungen++;
                        (void)re15_climb_try_start(&g_room_rdt, 1);
                        if (re15_climb_dbg_sub() == 9 && re15_climb_dbg_kind() == 0x81 && re15_climb_dbg_obj() == ks) g.kletter++;
                        re15_climb_reset();
                    }
        }
        PRUEF(g.kletter > 0, "Gegenprobe Band 0: die Sonde greift den Schluessel in %d von %d Stellungen", g.kletter, g.stellungen);
    }
}

/* ---- Teil weg (M3): echter Lauf bis zum Aufheben ueber die Eingabe, ab der Leiter und ab dem Nordbecken, Leon und Elza, auch quer durch den Westkanal -------------------------- */
static void weg_eine(int32_t x, int32_t z, int gier, int32_t lx, int32_t lz, int *ok, int *n)
{
    int c, stf; int32_t dm;
    (void)tod_lage(x, z, (int16_t)gier, lx, lz, 0, &c, &stf, &dm);
    const int ks = prop_von(RE15_GSZ_KEY_OBJ);
    if (ks < 0 || c < 0) return;
    (*n)++;
    const int32_t kx = g_scd.props[ks].x, kz = g_scd.props[ks].z;
    int f = 0;
    while (f < 200 && re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL)) { frame(0, 0); f++; }
    for (f = 0; f < 20; f++) frame(0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int b = laufen(kx, kz, 450, 1800, NULL, 0);
    int r = 0;
    if (b >= 0) r = aufheben(pl->x, pl->z, blick(pl->x, pl->z, kx, kz));
    printf("    Leiche (%d,%d) Gier %d, Start (%d,%d), Schluessel (%d,%d): %d Bilder, Aufnahme %d\n", (int)x, (int)z, gier, (int)lx, (int)lz, (int)kx, (int)kz, b, r);
    if (r == 1) (*ok)++;
}
static void teil_weg(void)
{
    printf("== weg (M3) ==\n");
    int ok = 0, n = 0;
    weg_eine(-5676, -19980, 2095, -8200, -25550, &ok, &n);   /* Westkanal, Abnahme 0 ken2: Figur im Suedbecken */
    weg_eine(-5835, -19103, 2000, 2000, -8500, &ok, &n);     /* Westkanal, Figur im Nordbecken */
    weg_eine(3413, -23942, 2048, -8200, -25550, &ok, &n);    /* K3-Wandlage */
    weg_eine(-3000, -8000, 2048, -8200, -25550, &ok, &n);    /* Nordbecken am Maschinenblock */
    weg_eine(6400, -7000, 3072, 1325, -25550, &ok, &n);      /* Nordostecke */
    PRUEF(n == 5 && ok == n, "echter Lauf bis zum Aufheben in %d von %d Lagen", ok, n);
}

/* ---- Teil koerper (M2, Nachtrag 35 Punkt 3): RE2 R1 P0 +0 |= 0x80a @0x801022f8-fc -> Gate FUN_80034d0c; P3 +0x10E |= 0x4000 @0x8010265c-64 nimmt die Leiche aus der Zielhilfe --------- */
static void koerper_punkt(const re15_actor_t *g, int32_t ofs, int32_t *x, int32_t *z)
{
    *x = g->x + (int32_t)(((int64_t)re15_cos_q12(g->rot_y) * ofs) >> 12);
    *z = g->z - (int32_t)(((int64_t)re15_sin_q12(g->rot_y) * ofs) >> 12);
}
static void teil_koerper(void)
{
    printf("== koerper (M2) ==\n");
    static const int32_t k_ofs[5] = { 2600, 4600, -2600, -4800, 0 };             /* k_gb_seg */
    re15_actor_t *g = szene_bis_kampf(0);
    if (!g) { PRUEF(0, "Szene bis zum Kampf"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int32_t x0 = -5173, z0 = -18311; const int gier = 0;
    const int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier) * 8800 * sk) >> 24);
    re15_gr2_kanister_setzen(x0 + vx, -1200, z0 + vz);
    for (int f = 0; f < 200 && !re15_gr2_traegt(); f++) { g->x = x0; g->z = z0; g->rot_y = (int16_t)gier; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
    PRUEF(re15_gr2_traegt(), "Kanister aufgenommen");
    /* Gegenprobe lebend: der Koerperlaengen-Push schiebt */
    int schub = 0;
    for (int i = 0; i < 5; i++) {
        int32_t x, z; koerper_punkt(g, k_ofs[i], &x, &z);
        pl->x = x; pl->z = z; pl->y = 0; pl->hp = 100;
        frame(0, 0);
        const int64_t dx = (int64_t)pl->x - x, dz = (int64_t)pl->z - z;
        if (dx * dx + dz * dz > 10000) schub++;
    }
    PRUEF(schub >= 3, "Gegenprobe lebend: %d von 5 Koerperpunkten schieben die Figur (Mass misst)", schub);
    pl->x = -8200; pl->z = -25550;
    g->state = 2; g->hp = (int16_t)(g->hp - 1);                                  /* Treffer: HURT-Selektor -> R1 (Stufe 7) */
    frame(0, 0);
    PRUEF(re15_gr2_r1_laeuft((int)(g - g_actors)), "R1 laeuft");
    int geschoben = 0, bilder = 0;
    for (int f = 0; f < 400 && !re15_gr2_tot(); f++) {
        int32_t x, z; koerper_punkt(g, k_ofs[f % 5], &x, &z);
        pl->x = x; pl->z = z; pl->y = 0; pl->hp = 100;
        frame(0, 0);
        bilder++;
        if (pl->x != x || pl->z != z) geschoben++;
    }
    PRUEF(re15_gr2_tot() && bilder > 20, "R1 zu Ende nach %d Bildern", bilder);
    for (int i = 0; i < 25; i++) {
        int32_t x, z; koerper_punkt(g, k_ofs[i % 5], &x, &z);
        pl->x = x; pl->z = z; pl->y = 0; pl->hp = 100;
        frame(0, 0);
        bilder++;
        if (pl->x != x || pl->z != z) geschoben++;
    }
    PRUEF(geschoben == 0, "R1 und Leiche schieben die Figur in %d Bildern nie (%d)", bilder, geschoben);
    PRUEF(re15_gator_boss_ohne_koerper(g), "Boss meldet: ohne Koerper");
    PRUEF(g->hit_radius_min == 0, "Leiche ohne Trefferbox (P3 +0x10E |= 0x4000 @0x8010265c-64: keine Zielhilfe, Schussprobe nimmt sie nicht)");
    int32_t tx, tz; pl->x = g->x + 1500; pl->z = g->z; pl->rot_y = 2048;
    PRUEF(!re15_player_aim_target(6000, &tx, &tz), "Zielhilfe nimmt die Leiche nicht");
}

/* ---- Teil finisher (M5, Nachtrag 35 Punkt 4): Modell aus mit dem Clip-5-Ende, RE2 P2-Ende Spieler-Wort |= 0x80000 @0x80101190-ac; P1/P2 sichtbar -------------------------- */
static void teil_finisher(void)
{
    printf("== finisher (M5) ==\n");
    re15_actor_t *g = szene_bis_kampf(0);
    if (!g) { PRUEF(0, "Szene bis zum Kampf"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int f = 0;
    for (; f < 4000 && !re15_gator_boss_im_fressen(g); f++) { pl->hp = 1; frame(0, 0); }
    PRUEF(re15_gator_boss_im_fressen(g), "toedlicher Biss nach %d Bildern", f);
    int sicht4 = 0, sicht5 = 0, aus5 = 0, aus11 = 0, sicht11 = 0, clip11 = 0;
    for (int i = 0; i < 400 && clip11 < 80; i++) {
        frame(0, 0);
        if (!re15_gator_boss_im_fressen(g)) break;
        if (g->motion == 4)  sicht4 += !pl->no_draw;
        if (g->motion == 5)  { if (pl->no_draw) aus5++; else sicht5++; }
        if (g->motion == 11) { clip11++; if (pl->no_draw) aus11++; else sicht11++; }
    }
    printf("    Clip 4 sichtbar %d, Clip 5 sichtbar %d / aus %d, Clip 11 sichtbar %d / aus %d\n", sicht4, sicht5, aus5, sicht11, aus11);
    PRUEF(sicht4 > 0 && sicht5 > 100 && aus5 == 0, "Leon in Clip 4 und Clip 5 sichtbar (P1/P2)");
    PRUEF(clip11 > 0 && sicht11 == 0 && aus11 == clip11, "Leon ab dem ersten Bild von Clip 11 aus (%d Bilder)", clip11);
}

/* ---- Teil leiche (M6, Nachtrag 35 Punkt 5): Rumpfmitte im Maul (1750,-210 im Part-6-Rahmen x Skala) in jedem Bild, sobald sie angekommen ist, bis zum Clip-5-Ende; danach weg ------- */
static void teil_leiche_p(int elza)
{
    printf("== leiche%s (M6) ==\n", elza ? "_elza" : "");
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    if (room_boot(r2080, -9950, -14400, -25700, 2048, 2) != 0) return;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    const uint8_t cut = g_room_change.target_cut;
    if (room_boot(r2090, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, cut) != 0) return;
    if (!gator_bank()) return;
    re15_actor_t *g = gator_setzen();
    int gemessen = 0, angekommen = -1; int32_t a_start = -1, a_max = 0, a = 0; int weg = -1, clip5_ende = -1;
    for (int f = 0; f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        const int clip_vor = g ? g->motion : -1;
        frame(0, 0);
        if (g && clip_vor == 5 && g->motion == 11 && clip5_ende < 0) clip5_ende = f;
        if (re15_gatorszene_r41_leiche_sitz(&a)) {
            if (gemessen == 0) a_start = a;
            gemessen++;
            if (angekommen < 0 && a <= 40) angekommen = gemessen;
            if (angekommen >= 0 && a > a_max) a_max = a;
        } else if (gemessen && weg < 0) weg = f;
    }
    printf("    %d Bilder angehaengt, Abstand zum Maul beim Anhaengen %d, angekommen nach %d Bildern, danach hoechstens %d; Clip-5-Ende Bild %d, weg ab Bild %d\n", gemessen, (int)a_start, angekommen,
           (int)a_max, clip5_ende, weg);
    PRUEF(gemessen > 100 && angekommen > 0 && angekommen <= 40, "Rumpfmitte kommt im Maul an (nach %d Bildern, Start %d)", angekommen, (int)a_start);
    PRUEF(a_max <= 40, "Leiche folgt dem Kopf Bild fuer Bild (Abstand hoechstens %d)", (int)a_max);
    PRUEF(clip5_ende >= 0 && weg == clip5_ende, "Leiche mit dem Clip-5-Ende weg (RE2 P2-Ende @0x80101190-ac), Bild %d / %d", weg, clip5_ende);
}

/* ---- Teil kanister_sitz (M7, Nachtrag 35 Punkt 6): Tank-Achse = Quer-Achse des Kopfes wie RE2, bei jeder Gier; Lokallage (1750,-210,-262) ------------------------------------------- */
static void teil_kanister_sitz(void)
{
    printf("== kanister_sitz (M7) ==\n");
    static const int32_t st[][2] = { { -5173, -18311 }, { -4000, -24000 }, { 3000, -23000 }, { -5000, -10000 }, { 2000, -8000 }, { 0, -25000 } };
    int n = 0, schief = 0, lage = 0; int32_t dot_min = 4096;
    for (int gi = 0; gi < 8; gi++) {
        const int gier = gi * 512;
        re15_actor_t *g = szene_bis_kampf(0);
        if (!g) continue;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
        const int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier) * 8800 * sk) >> 24);
        int si = -1;
        for (size_t i = 0; i < sizeof st / sizeof st[0] && si < 0; i++)
            if (re15_aot_water_at(st[i][0] + vx, st[i][1] + vz)) si = (int)i;
        if (si < 0) continue;
        const int32_t x = st[si][0], z = st[si][1];
        re15_gr2_kanister_setzen(x + vx, -1200, z + vz);
        for (int f = 0; f < 200 && !re15_gr2_traegt(); f++) { g->x = x; g->z = z; g->rot_y = (int16_t)gier; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
        if (!re15_gr2_traegt()) continue;
        for (int f = 0; f < 60; f++) { g->x = x; g->z = z; g->rot_y = (int16_t)gier; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
        int32_t kp[3], kr[9], r6[9], p6[3], l[3];
        if (!re15_gr2_kanister_welt(kp, kr) || !re15_gr2_part6_gezeichnet(r6, p6) || !re15_gr2_kanister_lokal(l)) continue;
        const int32_t dot = (int32_t)(((int64_t)kr[1] * r6[2] + (int64_t)kr[4] * r6[5] + (int64_t)kr[7] * r6[8]) >> 12);
        n++;
        if (dot > -4030) schief++;                                               /* RE2: Modell-y = -Quer-Achse (Dir (0x690,0,0xfc00) bei Gier 0xC00), 10 Grad = cos 0,985 */
        if (-dot < dot_min) dot_min = -dot;
        if (l[0] != RE15_GR2_KAN_LOKAL_X || l[1] != RE15_GR2_KAN_LOKAL_Y || l[2] != -262) lage++;
        printf("    Gier %4d Start (%d,%d): Achse * Quer %d/4096, Lokallage (%d,%d,%d)\n", gier, (int)x, (int)z, (int)dot, (int)l[0], (int)l[1], (int)l[2]);
    }
    PRUEF(n >= 6 && schief == 0, "Tank-Achse liegt in %d Laeufen auf der Quer-Achse des Kopfes (schief %d, kleinster Wert %d/4096)", n, schief, (int)dot_min);
    PRUEF(lage == 0, "Lokallage (1750,-210,-262) in allen Laeufen (%d nicht)", lage);
}

/* ---- Werkzeug wegplan (exe-Riegel integration_r41_alligator2_schluessel*): liest Lauf 1 (debug.log, state.log) und schreibt das Eingabe-Skript von Lauf 2 --------------------------------
 * Aufruf `test_r41_alligator2 wegplan <lauf1-ordner> <raum hex> <w-sekunden> <block-bilder> <prefix-datei> <aus-datei>`: k = erster Schussblock-Rand nach dem Ende der Einstellung (Folge O); dort steht die
 * Figur laut state.log; ab dieser Lage faehrt der Lauf im Harness (dieselbe Spielschritt-Funktion) bis in die Zone und schreibt die Tasten als Skript-Tokens. */
static int wegplan(const char *dir, unsigned raum, double w_s, int block_bilder, const char *block_txt, const char *aus)
{
    char p[1024], z[8192];
    snprintf(p, sizeof p, "%s/debug.log", dir);
    FILE *fp = fopen(p, "r");
    if (!fp) { printf("wegplan: %s fehlt\n", p); return 1; }
    int fo = -1, kx = 0, ky = 0, kz = 0, hat_key = 0;
    while (fgets(z, sizeof z, fp)) {
        const char *a = strstr(z, "bei der Leiche (");
        if (a && strstr(z, "Water Key") && sscanf(a, "bei der Leiche (%d,%d,%d)", &kx, &ky, &kz) == 3) hat_key = 1;
        a = strstr(z, "Folge O ROOM");
        if (a) { const char *q = strchr(a + 12, 'F'); if (q) fo = atoi(q + 1); }
    }
    fclose(fp);
    if (!hat_key || fo < 0) { printf("wegplan: kein Schluessel (%d) oder keine Folge O (F%d) in Lauf 1\n", hat_key, fo); return 1; }
    const int f0 = (int)(w_s * 30.0 + 0.5);
    int k = (fo + 10 - f0 + block_bilder - 1) / block_bilder;
    if (k < 0) k = 0;
    const int fb = f0 + k * block_bilder + 1;                 /* RE15_INPUT_SCRIPT_START=1 */
    snprintf(p, sizeof p, "%s/state.log", dir);
    fp = fopen(p, "r");
    if (!fp) { printf("wegplan: %s fehlt\n", p); return 1; }
    int px = 0, pz = 0, rot = 0, da = 0, vor = -1;
    while (fgets(z, sizeof z, fp)) {
        int f = -1;
        if (z[0] != 'F' || sscanf(z + 1, "%d", &f) != 1) continue;
        if (f < vor) da = 0;                                  /* neuer Raum-Abschnitt */
        vor = f;
        if (f != fb) continue;
        const char *a = strstr(z, "PL(");
        if (a && sscanf(a, "PL(%d,%d,rot=%d", &px, &pz, &rot) == 3) da = 1;
    }
    fclose(fp);
    if (!da) { printf("wegplan: state.log hat kein Bild F%d\n", fb); return 1; }
    printf("wegplan: Folge O F%d, Block %d, Lauf ab F%d, Figur (%d,%d) Blick %d, Schluessel (%d,%d)\n", fo, k, fb, px, pz, rot, kx, kz);
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot((uint16_t)raum, px, 0, pz, (int16_t)rot, 0) != 0) return 1;
    re15_enemy_ai_set_paused(1);
    static char skript[1 << 16];
    skript[0] = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int b = laufen(kx, kz, 450, 1800, skript, sizeof skript);
    if (b < 0) { printf("wegplan: Lauf nicht angekommen, Figur (%d,%d)\n", pl->x, pl->z); return 1; }
    printf("wegplan: %d Bilder, Ende (%d,%d) Blick %d\n", b, pl->x, pl->z, (int)pl->rot_y);
    /* Blick zum Schluessel: letzte Drehung im Stand, dann Aktionstaste und die Abfrage der Aufnahme */
    const int16_t ziel = blick(pl->x, pl->z, kx, kz);
    int d = (((int)ziel - ((int)pl->rot_y & 0xfff) + 0x800) & 0xfff) - 0x800;
    const int n_dreh = (d < 0 ? -d : d) / 96;
    fp = fopen(aus, "w");
    if (!fp) { printf("wegplan: %s nicht schreibbar\n", aus); return 1; }
    fprintf(fp, "W%.3f", w_s);
    for (int i = 0; i < k; i++) fprintf(fp, ",%s", block_txt);
    fprintf(fp, "%s", skript);
    if (n_dreh > 0) fprintf(fp, ",%c%.3f", d > 0 ? 'R' : 'L', n_dreh / 30.0);
    fprintf(fp, ",W0.300,A0.200,W3.000,A0.200,W1.000,A0.200,W1.000,A0.200,W1.000,A0.200,W1.000,A0.200,W1.000,A0.200,W2.000\n");
    fclose(fp);
    return 0;
}

/* Werkzeug (kein Riegel): Spur eines RE2-Effekts der Raum-ESP von ROOM40A0 (Nachbesserung 1, M4): fxspur <a0 hex> <x> <y> <z> [bilder] */
#include "re2_fx.h"
static int fxspur(uint32_t a0, int x, int y, int z, int bilder)
{
    size_t n = 0, nr = 0;
    uint8_t *esp = slurp(RE15_ASSET_RE2_DIR "/CORE00.ESP", &n), *raum = slurp(RE15_ASSET_RE2_DIR "/GATOR40A0.ESP", &nr);
    if (!esp || !raum) { printf("ESP fehlt\n"); return 1; }
    re2fx_reset();
    if (re2fx_register_core(esp, n) != 0 || re2fx_register_raum(raum, nr) != 0) { printf("Registrierung fehlgeschlagen\n"); return 1; }
    const int16_t ofs[4] = { (int16_t)x, (int16_t)y, (int16_t)z, 0 };
    const int p = re2fx_spawn_sofort(a0, 0, re2fx_einheitsmatrix, ofs);
    printf("Spawn %08X -> Platz %d\n", (unsigned)a0, p);
    for (int f = 0; f < bilder; f++) {
        re2fx_tick();
        int lebend = 0;
        for (int i = 0; i < RE2FX_PLAETZE; i++) {
            const uint8_t *b = re2fx_platz(i);
            if (!b || !(b[0x18] | b[0x19])) continue;
            lebend++;
            if (f < 12 || (f % 10) == 0)
                printf("F%3d Platz %2d Bank %02X Sub %d Op %d/%d St %04X Anim %d/%d Welt (%d,%d,%d) v (%d,%d,%d) a (%d,%d,%d) Skala %u\n", f, i,
                       (unsigned)(b[0x1C] | (b[0x1D] << 8)), b[0x1E], b[0x00], b[0x01], (unsigned)(b[0x18] | (b[0x19] << 8)), b[0x21], b[0x20],
                       (int16_t)(b[0x34] | (b[0x35] << 8)), (int16_t)(b[0x36] | (b[0x37] << 8)), (int16_t)(b[0x38] | (b[0x39] << 8)),
                       (int16_t)(b[0x0C] | (b[0x0D] << 8)), (int16_t)(b[0x0E] | (b[0x0F] << 8)), (int16_t)(b[0x10] | (b[0x11] << 8)),
                       (int8_t)b[0x08], (int8_t)b[0x09], (int8_t)b[0x0A], (unsigned)(b[0x3A] | (b[0x3B] << 8)));
        }
        if (!lebend) { printf("alle Plaetze frei ab F%d\n", f); break; }
    }
    printf("unbekannte Ops %u; Op-Zaehler:", re2fx_op_unbekannt());
    for (int op = 0; op < 96; op++) if (re2fx_op_zaehler(op)) printf(" %d:%u", op, re2fx_op_zaehler(op));
    printf("\n");
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "wegplan") && argc >= 8) { if (!gator_bank()) return 1; re15_door_seq_setze_laeufer(fang); return wegplan(argv[2], (unsigned)strtoul(argv[3], NULL, 16), atof(argv[4]), atoi(argv[5]), argv[6], argv[7]); }
    if (!strcmp(teil, "erreichbar_scan")) { err_mass_t m; if (!gator_bank()) return 1; re15_door_seq_setze_laeufer(fang); err_scan(argc > 2 ? atoi(argv[2]) : 1000, 0, 1, &m);
        printf("%d Todeslagen, %d Stellungen, nicht erreicht %d, Klettern %d\n", m.lagen, m.stellungen, m.wand, m.kletter); return (m.wand || m.kletter) ? 1 : 0; }
    if (!strcmp(teil, "fxspur") && argc >= 6) return fxspur((uint32_t)strtoul(argv[2], NULL, 16), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), argc > 6 ? atoi(argv[6]) : 200);
    if (!strcmp(teil, "lagesuche2") && argc >= 3) { if (!gator_bank()) return 1; return lagesuche2(argv[2]); }
    if (!strcmp(teil, "gsz")) return gsz_haupt(argc - 1, argv + 1);    /* Durchreiche auf die Teile von test_r41_gatorszene */
    if (!strcmp(teil, "trag_scan")) { if (!gator_bank()) return 1; re15_door_seq_setze_laeufer(fang); trag_scan(argc > 2); return 0; }
    if (!strcmp(teil, "trag_eins") && argc >= 7) {
        trag_mass_t m;
        if (!gator_bank()) return 1;
        re15_door_seq_setze_laeufer(fang);
        const int r = trag_lauf(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), argc > 7 ? atoi(argv[7]) : 900, &m, argc > 8 ? 2 : 1);
        printf("trag_eins: rc %d, %d Bilder, Sprung max %d F+%d, > 340: %d, Ueberstand max %d, Bilder > 300: %d\n", r, m.bilder, (int)m.max_d, m.max_f, m.s340, (int)m.ueber_max, m.ueber_n);
        return 0;
    }
    if (!strcmp(teil, "schluessel_scan")) {   /* Werkzeug: voller Scan */
        int n, kl, st[5]; int32_t d_max;
        if (!gator_bank()) return 1;
        re15_door_seq_setze_laeufer(fang);
        schluessel_scan(1000, 1, &n, &kl, st, &d_max);
        return 0;
    }
    if (!strcmp(teil, "tragelog") && argc >= 5) return tragelog(argv[2], atoi(argv[3]), atoi(argv[4]));
    if (!strcmp(teil, "bildanteil") && argc >= 7) return bildanteil(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    if (!gator_bank()) { printf("RE2-EM23-Bank fehlt\n"); return 1; }
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "kamera"))      teil_kamera(0);
    if (alle || !strcmp(teil, "kamera_elza")) teil_kamera(1);
    if (alle || !strcmp(teil, "schluessel"))  teil_schluessel_p();
    if (alle || !strcmp(teil, "tragen"))      teil_tragen();
    if (alle || !strcmp(teil, "erreichbar"))  teil_erreichbar();
    if (alle || !strcmp(teil, "weg"))         teil_weg();
    if (alle || !strcmp(teil, "koerper"))     teil_koerper();
    if (alle || !strcmp(teil, "finisher"))    teil_finisher();
    if (alle || !strcmp(teil, "leiche"))      teil_leiche_p(0);
    if (alle || !strcmp(teil, "leiche_elza")) teil_leiche_p(1);
    if (alle || !strcmp(teil, "kanister_sitz")) teil_kanister_sitz();
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
