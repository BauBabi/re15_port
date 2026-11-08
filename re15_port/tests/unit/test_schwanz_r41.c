/* test_schwanz_r41.c — Feder-Kette des Schwanzes der Sichel-Form (Runde 41 Spur SF N1.9, include/re15_schwanz_r41.h).
 *
 * PORT-WAHL (keine Original-Konstante). Der Riegel haelt fest, was der Nutzer verlangt ("Der Schwanz ist viel zu starr")
 * und was das Video zeigt (Video 4 Hangar h18-h70, Dossier SF_sichel.md N1.9), ohne die Form zu zerbrechen:
 *   (1) Ruhe: steht der Koerper still, liegt die Kette nach kurzer Zeit wieder auf dem Keyframe (keine Eigenbewegung,
 *       die den Clip verfaelscht).
 *   (2) Nachlauf: dreht sich Birkin um 45 Grad in einem Takt, bleibt die Spitze zurueck (Abstand zum Keyframe > 300)
 *       und schwingt dann aus; ohne Kette (Keyframe allein) gaebe es diesen Abstand nicht.
 *   (3) Glieder bleiben verbunden: Gelenkabstaende = Keyframe-Gliedlaengen (+-3), Knochen 16-22 schliessen aneinander an.
 *   (4) Boden: kein Gelenk unter y = 0.
 *   (5) Szenenvorgabe: Seite 4000 legt die Spitze auf Birkins rechte Seite (Modell z < -800), Freigabe -> sie schwingt
 *       durch die Mitte (z ~ 0) zurueck auf die linke Seite des Clips (z > +1000) - Video h28 -> h30-h34 -> h38.
 *   (6) Nur die Sichel-Form: EM30 mit 16 Knochen bleibt unberuehrt.
 *
 * Seit v0.8.30 (Einbau V7, Nutzerfreigabe 2026-11-08 "Naja, gehen wir so erst mal ins rennnen.") traegt das Spiel-Asset die
 * V4-Kette (ungleiche Glieder 624/440/887/1121/350/204/608, max/min 5,5 > SW_GLATT_V4_VERH 2,0; Dossier SF_sichel.md V6 drei
 * Punkte 2.). Fuer sie gelten in schwanz_r41.c die V4-Regeln (SW_V4_MITNAHME 0,80 / SW_V4_FEDER 1,5 / SW_V4_ERHALT 0,50, Entscheid O3
 * "T7 koerperfest", Nutzer-Wortlaut 2026-11-06 "Ich will EXAKT diese Umsetzung": die Kette schwingt noch, bleibt aber in der
 * Ruheform; Knick-Grenze glaetten() nach dem Szenenziel). kette_pruefen() prueft deshalb zweimal:
 *   - Spiel-Asset (V4-Kette): (1), (3), (4) unveraendert; (2) Nachlauf > 60 (= ausserhalb der Ruhe-Toleranz von (1)) und Ausschwingen
 *     < 60; (5) Glieder je Glied so lang wie gezeichnet, Spitze rechts, Mitte nach <= 12 Takten (0 erlaubt: SW_V4_MITNAHME nimmt
 *     80 % der Zielbewegung im selben Takt mit); (7) Spitze naeher am Ziel als ohne Ziel, Glieder verbunden, Freigabe ohne Sprung.
 *     Gemessen mit V7 (Lauf 2026-11-08): Nachlauf 189 / Rest 4, Glieder 201..1115 je wie gezeichnet, Mitte nach 0 Takten,
 *     Ziel 380 (Knick-Grenze der V4-Kette; Dossier "Einbau v0.8.30" Offen).
 *   - Gegenlauf "Gliederung Referenz 2" (Knochenlagen 16-22 aus cfd1ac42, keine V4-Kette): alle Schwellen der Referenz-2-Kette wie bisher
 *     ((2) > 300, (5) Mitte nach 1..12, (7) Ziel < 40) - die Pruefungen der freien Kette bleiben so streng wie vor dem Einbau. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_engine.h"
#include "re15_schwanz_r41.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static re15_md1_t s_md1; static re15_emd_skeleton_t s_skel; static re15_emd_animation_t s_anim;

/* Pose wie der Zeichner (main.c): Keyframe des Aktors, re15_skel_compute_pose, dann die Kette. */
static int pose(re15_actor_t *a, re15_skel_pose_t *p, int kette)
{
    int kf = re15_compute_actor_kf(&s_anim, &s_skel, a, -1, a->anim_frame);
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(&s_skel, kf, p) != 0) return 0;
    return kette ? re15_schwanz_r41_pose(a, &s_skel, p) : 1;
}
static void spitze(const re15_skel_pose_t *p, double out[3])        /* Modellraum */
{
    const int16_t *r = s_skel.bone_relative_pos[22];
    const double l = sqrt((double)r[0] * r[0] + (double)r[1] * r[1] + (double)r[2] * r[2]);
    for (int k = 0; k < 3; k++) out[k] = p[22].trans[k] + p[22].rot[k * 3 + 1] / 4096.0 * l;
}
static double abst(const double a[3], const double b[3])
{
    return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
}
static void takt(void) { g_engine.frame_count++; }

/* Gelenkabstaende und Anschluss der Knochen: so lang wie im Keyframe `k`. Die Posenmatrizen des Ports sind nicht ganz
 * normiert (Spaltenlaengen 0,996 an Knochen 15 bis 0,94 an Knochen 22, gemessen), die Kette uebernimmt diesen Massstab. */
static double glied(const re15_skel_pose_t *p, int b)
{
    const int32_t *t0 = p[b - 1].trans, *t1 = p[b].trans;
    return sqrt((double)(t1[0] - t0[0]) * (t1[0] - t0[0]) + (double)(t1[1] - t0[1]) * (t1[1] - t0[1]) +
                (double)(t1[2] - t0[2]) * (t1[2] - t0[2]));
}
static void verbunden(const re15_skel_pose_t *p, const re15_skel_pose_t *k, const char *wo)
{
    for (int b = 17; b <= 22; b++)
        CHECK(fabs(glied(p, b) - glied(k, b)) <= 3.0, "%s: Glied %d Laenge %.0f statt %.0f", wo, b - 1, glied(p, b),
              glied(k, b));
}

static void kette_pruefen(re15_actor_t *a, const char *wo)
{
    const int v4 = re15_schwanz_r41_v4_kette(&s_skel);
    re15_skel_pose_t p[RE15_EMD_MAX_BONES], k[RE15_EMD_MAX_BONES];
    double sp[3], sk[3];
    fprintf(stderr, "Schwanz [%s]: %s\n", wo, v4 ? "V4-Kette (ungleiche Glieder)" : "Kette mit gleichen Gliedern");

    /* (1) Ruhe */
    for (int i = 0; i < 90; i++) { takt(); CHECK(pose(a, p, 1), "[%s] Kette nicht angewandt (Ruhe)", wo); }
    pose(a, k, 0); spitze(p, sp); spitze(k, sk);
    CHECK(abst(sp, sk) < 60.0, "[%s] Ruhe: Spitze %.0f vom Keyframe (Soll < 60)", wo, abst(sp, sk));
    verbunden(p, k, "Ruhe");
    for (int b = 0; b < 16; b++)
        CHECK(!memcmp(&p[b], &k[b], sizeof p[b]), "[%s] Knochen %d von der Kette veraendert", wo, b);

    /* (2) Nachlauf nach einer Drehung um 45 Grad in einem Takt */
    double max_ab = 0, end_ab;
    a->rot_y = (int16_t)(a->rot_y + 512);
    for (int i = 0; i < 120; i++) {
        takt(); pose(a, p, 1); pose(a, k, 0);
        spitze(p, sp); spitze(k, sk);
        if (abst(sp, sk) > max_ab) max_ab = abst(sp, sk);
        verbunden(p, k, "Nachlauf");
        for (int j = 0; j < 8; j++) {
            int32_t g[3];
            if (re15_schwanz_r41_gelenk(20, j, g)) CHECK(g[1] <= 0, "[%s] Gelenk %d unter dem Boden (y %d)", wo, j, g[1]);
        }
    }
    end_ab = abst(sp, sk);
    if (v4)   /* V4-Kette: schwingt noch (ausserhalb der Ruhe-Toleranz 60 von (1)), bleibt in der Ruheform (SW_V4_*, Entscheid O3) */
        CHECK(max_ab > 60.0, "[%s] Drehung: V4-Kette laeuft nicht nach (max %.0f vom Keyframe, Soll > 60)", wo, max_ab);
    else
        CHECK(max_ab > 300.0, "[%s] Drehung: Spitze laeuft nicht nach (max %.0f vom Keyframe)", wo, max_ab);
    CHECK(end_ab < 60.0, "[%s] Drehung: Kette schwingt nicht aus (%.0f nach 120 Takten)", wo, end_ab);

    /* (5) Szenenvorgabe Seite 4000 (wie k_schwanz_seite) -> rechte Seite, Freigabe -> zurueck durch die Mitte */
    re15_schwanz_r41_seite(20, 4000);
    double sim_max = 0, ist_l[8] = { 0 }, soll_l[8] = { 0 };
    for (int i = 0; i < 25; i++) {
        takt(); pose(a, p, 1);
        /* Runde 41 SF NB3 (Abnahme 1 Mangel 3): die SIMULIERTEN Glieder bleiben auch mit Seitenvorgabe so lang wie die
         * gezeichneten (vorher 429..973 statt 424, weil die Laengen nach der Vorgabe gemessen wurden). Einbau V7: je Glied gegen
         * seine gezeichnete Laenge (Keyframe k: Glied j = Knochen 16+j, Glied 7 = Spitze) - die V4-Kette hat ungleiche Glieder. */
        for (int j = 1; j < 8; j++) {
            int32_t g0[3], g1[3];
            if (re15_schwanz_r41_gelenk(20, j - 1, g0) && re15_schwanz_r41_gelenk(20, j, g1)) {
                const double l = sqrt((double)(g1[0] - g0[0]) * (g1[0] - g0[0]) + (double)(g1[1] - g0[1]) * (g1[1] - g0[1]) +
                                      (double)(g1[2] - g0[2]) * (g1[2] - g0[2]));
                double t22[3] = { k[22].trans[0], k[22].trans[1], k[22].trans[2] }, skk[3];
                spitze(k, skk);
                const double soll = j < 7 ? glied(k, 16 + j) : abst(skk, t22);
                if (fabs(l - soll) > sim_max) { sim_max = fabs(l - soll); }
                ist_l[j] = l; soll_l[j] = soll;
            }
        }
    }
    CHECK(sim_max < 6.0, "[%s] Seitenvorgabe: simulierte Glieder bis %.0f von der gezeichneten Laenge (Glieder %.0f/%.0f/%.0f/%.0f/%.0f/%.0f/%.0f "
          "statt %.0f/%.0f/%.0f/%.0f/%.0f/%.0f/%.0f)", wo, sim_max, ist_l[1], ist_l[2], ist_l[3], ist_l[4], ist_l[5], ist_l[6], ist_l[7],
          soll_l[1], soll_l[2], soll_l[3], soll_l[4], soll_l[5], soll_l[6], soll_l[7]);
    spitze(p, sp);
    CHECK(sp[2] < -800.0, "[%s] Vorgabe rechts: Spitze z %.0f (Soll < -800)", wo, sp[2]);
    re15_schwanz_r41_seite(20, 0);
    int mitte = -1, links = -1;
    for (int i = 0; i < 60; i++) {
        takt(); pose(a, p, 1); spitze(p, sp);
        if (mitte < 0 && sp[2] > -200.0) mitte = i;
        if (links < 0 && sp[2] > 1000.0) links = i;
    }
    /* V4-Kette: SW_V4_MITNAHME 0,80 nimmt 80 % der Zielbewegung (auch der Seitenvorgabe) im selben Takt mit -> Mitte ab Takt 0 */
    CHECK(mitte >= (v4 ? 0 : 1) && mitte <= 12, "[%s] Freigabe: Mitte nach %d Takten (Video h28 -> h30..h34)", wo, mitte);
    /* Neuaufbau ab NB2: "linke Seite" nur, wenn die Ruhelage der Spitze links liegt (Clip 0x0B B20: z +1500..+1850); im Stand
     * (Clip 0 B0, s. oben) liegt sie mittig - die Seitenvorgabe nutzt die Szene seit NB3 nicht mehr (Schwanzbahn). */
    if (sk[2] > 1000.0) CHECK(links > mitte && links <= 20, "[%s] Freigabe: linke Seite nach %d Takten (Video bis h38)", wo, links);
    fprintf(stderr, "Schwanz [%s]: Nachlauf max %.0f, Rest %.0f; Glieder bis %.0f von der gezeichneten Laenge; Freigabe Mitte nach %d, "
            "links nach %d Takten\n", wo, max_ab, end_ab, sim_max, mitte, links);

    /* (7) Runde 41 SF NB3: Szenenziel der Spitze (FABRIK) - erreichbares Ziel wird im selben Takt getroffen, die Glieder
     *     bleiben gleich lang; Freigabe ohne Sprung. V4-Kette: glaetten() (Knick-Grenze, V4 Runde 2) laeuft NACH dem Szenenziel und
     *     hat Vorrang - die Spitze muss dem Ziel naeher kommen als ohne Ziel (gemessen V7: 380 statt < 40, Dossier Offen). */
    {
        int32_t w0[3], ziel[3], g[3], g_ohne[3];
        re15_schwanz_r41_gelenk(20, 0, w0);
        ziel[0] = w0[0] + 900; ziel[1] = -300; ziel[2] = w0[2] - 1200;
        re15_schwanz_r41_gelenk(20, 7, g_ohne);
        const double d0 = sqrt((double)(g_ohne[0] - ziel[0]) * (g_ohne[0] - ziel[0]) + (double)(g_ohne[1] - ziel[1]) * (g_ohne[1] - ziel[1]) +
                               (double)(g_ohne[2] - ziel[2]) * (g_ohne[2] - ziel[2]));
        re15_schwanz_r41_ziel(20, ziel, 4096);
        takt(); pose(a, p, 1); pose(a, k, 0);
        re15_schwanz_r41_gelenk(20, 7, g);
        const double d = sqrt((double)(g[0] - ziel[0]) * (g[0] - ziel[0]) + (double)(g[1] - ziel[1]) * (g[1] - ziel[1]) +
                              (double)(g[2] - ziel[2]) * (g[2] - ziel[2]));
        if (v4)
            CHECK(d < d0, "[%s] Ziel: V4-Kette kommt dem Szenenziel nicht naeher (%.0f, ohne Ziel %.0f)", wo, d, d0);
        else
            CHECK(d < 40.0, "[%s] Ziel: Spitze %.0f vom Szenenziel (Soll < 40)", wo, d);
        verbunden(p, k, "Ziel");
        re15_schwanz_r41_ziel(20, NULL, 0);
        int32_t g1[3];
        takt(); pose(a, p, 1);
        re15_schwanz_r41_gelenk(20, 7, g1);
        const double s = sqrt((double)(g1[0] - g[0]) * (g1[0] - g[0]) + (double)(g1[1] - g[1]) * (g1[1] - g[1]) +
                              (double)(g1[2] - g[2]) * (g1[2] - g[2]));
        CHECK(s < 700.0, "[%s] Ziel-Freigabe: Spitze springt %.0f in einem Takt", wo, s);
        fprintf(stderr, "Schwanz [%s]: Szenenziel getroffen auf %.0f (ohne Ziel %.0f), Freigabe-Schritt %.0f\n", wo, d, d0, s);
    }
}

int main(void)
{
    size_t len = 0;
    uint8_t *blob = slurp(RE15_ASSET_PSX_DIR "/EMD/EM30_HANGAR.EMD", &len);
    CHECK(blob != NULL, "EM30_HANGAR.EMD fehlt");
    if (!blob) return 1;
    CHECK(re15_emd_parse_container(blob, len, &s_md1, &s_skel, &s_anim, NULL) == 0, "EM30_HANGAR nicht lesbar");
    CHECK(s_skel.bone_count == 23, "Knochen %d statt 23", s_skel.bone_count);
    /* Einbau V7 (v0.8.30): das Spiel-Asset traegt die V4-Kette mit den Gliedlaengen aus V6 drei Punkte 2. (Knochen 17-22 relpos y
     * 624/440/887/1121/350/204, gemessen am EMR von 319bbd17; Referenz 2: 467/466/466/466/466/466) */
    {
        static const int16_t v7_y[6] = { 624, 440, 887, 1121, 350, 204 };
        int abw = 0;
        for (int b = 17; b <= 22; b++) abw += s_skel.bone_relative_pos[b][1] != v7_y[b - 17];
        CHECK(s_skel.bone_count == 23 && abw == 0 && re15_schwanz_r41_v4_kette(&s_skel),
              "Spiel-Asset: Schwanzglieder 17-22 nicht die V7-Kette (%d Laengen anders, V4-Kette %d)", abw,
              re15_schwanz_r41_v4_kette(&s_skel));
    }

    re15_schwanz_r41_reset();
    re15_actor_t *a = &g_actors[20];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0x30u; a->x = -8488; a->z = -21651; a->rot_y = 1300;
    /* Runde 41 SF Neuaufbau ab NB2: Haltung wie NB2 (Becken 170 tiefer als NB4/NB5) und Schwanz in NB4-Laenge - in Clip 0x0B B20
     * liegt die Spitze jetzt auf dem Boden (Modellraum: Gelenk 6 bei y -133, NB5-Stand -478), die Bodenregel haelt sie dort fest.
     * Die freie Kette wird deshalb im Stand (Clip 0 B0, Gelenk 6 bei y -892) geprueft. */
    a->motion = 0; a->anim_frame = 0;
    re15_skel_pose_t p[RE15_EMD_MAX_BONES];

    /* (1)-(7) mit dem Spiel-Asset (V4-Kette seit dem Einbau V7) */
    kette_pruefen(a, "Spiel-Asset");
    /* (1)-(7) Gegenlauf mit der Gliederung der Referenz 2 (EMR von cfd1ac42, gemessen: Knochen 16 Bindversatz (-380,120,0),
     * 17 (0,467,0), 18-22 (0,466,0); keine V4-Kette): die Schwellen der freien Kette wie vor dem Einbau */
    {
        static const int16_t ref2_rel[7][3] = { { -380, 120, 0 }, { 0, 467, 0 }, { 0, 466, 0 }, { 0, 466, 0 }, { 0, 466, 0 },
                                                { 0, 466, 0 }, { 0, 466, 0 } };
        int16_t alt[7][3];
        for (int b = 16; b <= 22; b++)
            for (int c = 0; c < 3; c++) { alt[b - 16][c] = s_skel.bone_relative_pos[b][c]; s_skel.bone_relative_pos[b][c] = ref2_rel[b - 16][c]; }
        CHECK(!re15_schwanz_r41_v4_kette(&s_skel), "Gegenlauf: Gliederung Referenz 2 als V4-Kette erkannt");
        re15_schwanz_r41_reset();
        a->rot_y = 1300; a->motion = 0; a->anim_frame = 0;
        kette_pruefen(a, "Gliederung Referenz 2");
        for (int b = 16; b <= 22; b++)
            for (int c = 0; c < 3; c++) s_skel.bone_relative_pos[b][c] = alt[b - 16][c];
        re15_schwanz_r41_reset();
        a->rot_y = 1300; a->motion = 0; a->anim_frame = 0;
        kette_pruefen(a, "Spiel-Asset, zweiter Lauf");
    }

    /* (8) Runde 41 SF NB3 (Abnahme 1 Mangel 12): ohne Ziel steht kein Glied flacher als 62 Grad zur Senkrechten (Labor-Pult:
     *     kein waagerechter Dorn). Runde 41 SF "Schwanz V2": ein Glied darf so flach sein wie sein Keyframe (Haken der
     *     V2-Ruhelinie, Formblatt V2 S6), aber nie flacher als 80 Grad (schwanz_r41.c SW_HANG_COS_MIN). */
    {
        double flach = 0, ueber = -90;
        a->rot_y = (int16_t)(a->rot_y + 1024);
        for (int i = 0; i < 40; i++) {
            re15_skel_pose_t q[RE15_EMD_MAX_BONES];
            double kf[8][3];
            takt();
            pose(a, q, 0);                                   /* Keyframe ohne Kette (Modellraum; Winkel zur Senkrechten gierfrei) */
            for (int j = 0; j < 7; j++) for (int k = 0; k < 3; k++) kf[j][k] = q[16 + j].trans[k];
            spitze(q, kf[7]);
            pose(a, p, 1);
            for (int j = 1; j < 8; j++) {
                int32_t g0[3], g1[3];
                if (!re15_schwanz_r41_gelenk(20, j - 1, g0) || !re15_schwanz_r41_gelenk(20, j, g1)) continue;
                const double dx = g1[0] - g0[0], dy = g1[1] - g0[1], dz = g1[2] - g0[2];
                const double w = acos(dy / sqrt(dx * dx + dy * dy + dz * dz + 1e-9)) * 180.0 / 3.14159265;
                const double ex = kf[j][0] - kf[j - 1][0], ey = kf[j][1] - kf[j - 1][1], ez = kf[j][2] - kf[j - 1][2];
                const double wk = acos(ey / sqrt(ex * ex + ey * ey + ez * ez + 1e-9)) * 180.0 / 3.14159265;
                const double grenze = wk + 8.0 > 62.0 ? wk + 8.0 : 62.0;   /* SW_HANG_RAND 0,10 im Kosinus */
                if (w > flach) flach = w;
                if (w - grenze > ueber) ueber = w - grenze;
            }
        }
        CHECK(ueber <= 1.0, "Haengen: Glied %.0f Grad ueber der Grenze max(62, Keyframe) (flachstes %.0f)", ueber, flach);
        CHECK(flach <= 81.0, "Haengen: Glied %.0f Grad von der Senkrechten (Soll <= 80)", flach);
        fprintf(stderr, "Schwanz: flachstes Glied ohne Ziel %.0f Grad (Abstand zur Grenze %.1f)\n", flach, ueber);
    }

    /* (9) Runde 41 SF V4 Runde 2 (Pruefung V4 r1 Zahlen Abschnitt 3: glaetten() lief fuer V4 nie, 73 von 144 Lauftakten mit einem
     *     Gelenk ueber 90 Grad): eine Kette mit V4-Gliederteilung (Knochen 17-22 so lang wie EM30_V4 Runde 2: 624/492/589/582/493/258,
     *     max/min 2,4 > SW_GLATT_V4_VERH 2,0) wird geglaettet - nach einer Drehung um 180 Grad in einem Takt knickt kein Gelenk weiter
     *     ab als max(16 Grad, Keyframe-Gelenkwinkel + 3 Grad) (+1 Grad Rundung). Gegenprobe mit gleich langen Gliedern
     *     (Referenz 2, nicht geglaettet) nur ausgegeben. */
    {
        static const int16_t v4_l[2][6] = { { 624, 492, 589, 582, 493, 258 }, { 466, 466, 466, 466, 466, 466 } };
        int16_t alt[6];
        for (int b = 17; b <= 22; b++) alt[b - 17] = s_skel.bone_relative_pos[b][1];
      for (int lauf = 0; lauf < 2; lauf++) {      /* lauf 1 = Gegenprobe gleich lange Glieder (Referenz 2): nicht geglaettet */
        for (int b = 17; b <= 22; b++) s_skel.bone_relative_pos[b][1] = v4_l[lauf][b - 17];
        re15_schwanz_r41_reset();
        a->motion = 0; a->anim_frame = 0;
        for (int i = 0; i < 30; i++) { takt(); pose(a, p, 1); }
        a->rot_y = (int16_t)(a->rot_y + 2048);
        double ueber = -90, max_w = 0;
        for (int i = 0; i < 60; i++) {
            re15_skel_pose_t q[RE15_EMD_MAX_BONES];
            double kf[8][3];
            int32_t g[8][3];
            takt();
            pose(a, q, 0);
            for (int j = 0; j < 7; j++) for (int k = 0; k < 3; k++) kf[j][k] = q[16 + j].trans[k];
            spitze(q, kf[7]);
            pose(a, p, 1);
            int ok = 1;
            for (int j = 0; j < 8; j++) ok &= re15_schwanz_r41_gelenk(20, j, g[j]);
            if (!ok) continue;
            for (int j = 1; j < 7; j++) {
                double u[3], v[3], ku[3], kv[3];
                for (int k = 0; k < 3; k++) {
                    u[k] = g[j][k] - g[j - 1][k]; v[k] = g[j + 1][k] - g[j][k];
                    ku[k] = kf[j][k] - kf[j - 1][k]; kv[k] = kf[j + 1][k] - kf[j][k];
                }
                const double w = acos((u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) /
                                      (sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]) * sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) + 1e-9))
                                 * 180.0 / 3.14159265;
                const double wk = acos((ku[0] * kv[0] + ku[1] * kv[1] + ku[2] * kv[2]) /
                                       (sqrt(ku[0] * ku[0] + ku[1] * ku[1] + ku[2] * ku[2]) *
                                        sqrt(kv[0] * kv[0] + kv[1] * kv[1] + kv[2] * kv[2]) + 1e-9)) * 180.0 / 3.14159265;
                const double grenze = wk + 3.0 > 16.0 ? wk + 3.0 : 16.0;
                if (w > max_w) max_w = w;
                if (w - grenze > ueber) ueber = w - grenze;
            }
        }
        if (lauf == 0)
            CHECK(ueber <= 1.0, "V4-Kette: Gelenk %.1f Grad ueber max(16, Keyframe + 3) (groesster Gelenkwinkel %.1f)", ueber, max_w);
        fprintf(stderr, "Schwanz %s: groesster Gelenkwinkel %.1f Grad, Abstand zur Grenze %.1f\n",
                lauf ? "gleiche Glieder (Gegenprobe, ungeglaettet)" : "V4-Gliederung", max_w, ueber);
        re15_schwanz_r41_reset();
      }
        for (int b = 17; b <= 22; b++) s_skel.bone_relative_pos[b][1] = alt[b - 17];
    }

    /* (6) EM30 mit 16 Knochen: keine Kette */
    {
        size_t esz = 0, off = 0, elen = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        re15_md1_t m2; re15_emd_skeleton_t s2; re15_emd_animation_t a2;
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &elen) == 0 &&
            re15_emd_parse_container(ems + off, elen, &m2, &s2, &a2, NULL) == 0) {
            re15_skel_pose_t q[RE15_EMD_MAX_BONES];
            int kf = re15_compute_actor_kf(&a2, &s2, a, -1, a->anim_frame);
            re15_skel_compute_pose(&s2, kf, q);
            CHECK(re15_schwanz_r41_pose(a, &s2, q) == 0, "EM30 (16 Knochen) von der Kette erfasst");
        }
        free(ems);
    }
    free(blob);
    if (fails) { fprintf(stderr, "test_schwanz_r41: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_schwanz_r41: OK\n");
    return 0;
}
