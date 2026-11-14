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
    const int v4 = re15_schwanz_r41_v4_kette(&s_skel) || re15_schwanz_r41_v8_kette(&s_skel);   /* V8: Regeln der V4-Kette (SW_V4_*) */
    re15_skel_pose_t p[RE15_EMD_MAX_BONES], k[RE15_EMD_MAX_BONES];
    double sp[3], sk[3];
    fprintf(stderr, "Schwanz [%s]: %s\n", wo, re15_schwanz_r41_v8_kette(&s_skel) ? "V8-Kette (gleiche Glieder, Wurzel oben)" :
            v4 ? "V4-Kette (ungleiche Glieder)" : "Kette mit gleichen Gliedern");

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
    /* Zweig r41/sichel_v8: das Spiel-Asset traegt die V8-Kette (Messblatt V8 6a; tools/r41_sichel/v8/kette_v8.json, Runde 3): Knochen 16
     * relpos (-722,-145,108), 17-22 (0,541,0), gemessen am EMR von 66806bc4 (Runde 2 d0ef852c: (-714,-214,130) / 526, Runde 1 fa31156c:
     * (-667,-342,102) / 538 - Gegenprobe rot).
     * Gegenprobe V7 (319bbd17: 16 (-378,-602,-410), 17-22 y 624/440/887/1121/350/204): keine V8-Kette, V4-Kette (= hier rot fuer V7). */
    {
        static const int16_t v8_rel[7][3] = { { -722, -145, 108 }, { 0, 541, 0 }, { 0, 541, 0 }, { 0, 541, 0 }, { 0, 541, 0 },
                                              { 0, 541, 0 }, { 0, 541, 0 } };
        static const int16_t v7_rel[7][3] = { { -378, -602, -410 }, { 0, 624, 0 }, { 0, 440, 0 }, { 0, 887, 0 }, { 0, 1121, 0 },
                                              { 0, 350, 0 }, { 0, 204, 0 } };
        int abw = 0;
        for (int b = 16; b <= 22; b++) for (int c = 0; c < 3; c++) abw += s_skel.bone_relative_pos[b][c] != v8_rel[b - 16][c];
        CHECK(s_skel.bone_count == 23 && abw == 0 && re15_schwanz_r41_v8_kette(&s_skel) && !re15_schwanz_r41_v4_kette(&s_skel),
              "Spiel-Asset: Schwanzkette 16-22 nicht die V8-Kette (%d Werte anders, V8-Kette %d, V4-Kette %d)", abw,
              re15_schwanz_r41_v8_kette(&s_skel), re15_schwanz_r41_v4_kette(&s_skel));
        int16_t alt[7][3];
        for (int b = 16; b <= 22; b++)
            for (int c = 0; c < 3; c++) { alt[b - 16][c] = s_skel.bone_relative_pos[b][c]; s_skel.bone_relative_pos[b][c] = v7_rel[b - 16][c]; }
        CHECK(!re15_schwanz_r41_v8_kette(&s_skel) && re15_schwanz_r41_v4_kette(&s_skel),
              "Gegenprobe V7-Kette: als V8-Kette erkannt (%d) oder keine V4-Kette (%d)",
              re15_schwanz_r41_v8_kette(&s_skel), re15_schwanz_r41_v4_kette(&s_skel));
        for (int b = 16; b <= 22; b++) for (int c = 0; c < 3; c++) s_skel.bone_relative_pos[b][c] = alt[b - 16][c];
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

    /* (10) V8 (Messblatt V8 6a, schwanz_r41.c SW_V8_GLATT_MAX): mit der V8-Kette (Spiel-Asset) knickt nach einer Drehung um 180 Grad in
     *      einem Takt kein Gelenk weiter ab als min(25 Grad, max(12 Grad, Keyframe-Gelenkwinkel + 3 Grad)) (+1 Grad Rundung), es gibt
     *      keine Knick-Ausnahme (V4: Biegung behaelt bis 73 Grad). Gegenprobe ohne V8-Erkennung (Wurzel x -800 < SW_V8_WURZEL_X -760):
     *      nur ausgegeben. */
    if (re15_schwanz_r41_v8_kette(&s_skel)) {
        const int16_t wx = s_skel.bone_relative_pos[16][0];
        for (int lauf = 0; lauf < 2; lauf++) {
            if (lauf) s_skel.bone_relative_pos[16][0] = -800;
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
                    double grenze = wk + 3.0 > 12.0 ? wk + 3.0 : 12.0;
                    if (grenze > 25.0) grenze = 25.0;
                    if (w > max_w) max_w = w;
                    if (w - grenze > ueber) ueber = w - grenze;
                }
            }
            if (lauf == 0)
                CHECK(ueber <= 1.0, "V8-Kette: Gelenk %.1f Grad ueber min(25, max(12, Keyframe + 3)) (groesster Gelenkwinkel %.1f)", ueber, max_w);
            fprintf(stderr, "Schwanz %s: groesster Gelenkwinkel %.1f Grad, Abstand zur Grenze %.1f\n",
                    lauf ? "ohne V8-Erkennung (Gegenprobe)" : "V8-Kette", max_w, ueber);
        }
        s_skel.bone_relative_pos[16][0] = wx;
        re15_schwanz_r41_reset();
    }

    /* (11) V8 Runde 2 (schwanz_r41.c SW_V8_STARR, Sicht-Pruefer V8 r1 Punkt 5 "graue Dreiecke des Wurzelrohrs stechen durch die
     *      Rueckenhaut"): ohne Szenenziel liegt das erste Glied (Gelenk 0 -> 1) nach einer Drehung um 180 Grad und waehrend des Gangs in
     *      jedem Takt auf seinem Keyframe (Abweichung <= 1 Grad, gemessen 0); nach einer Szenenfuehrung kehrt es zurueck.
     *      Gegenprobe ohne V8-Erkennung (Wurzel x -800): das erste Glied weicht ab (nur ausgegeben). */
    if (re15_schwanz_r41_v8_kette(&s_skel)) {
        const int16_t wx = s_skel.bone_relative_pos[16][0];
        for (int lauf = 0; lauf < 2; lauf++) {
            if (lauf) s_skel.bone_relative_pos[16][0] = -800;
            re15_schwanz_r41_reset();
            a->motion = 0; a->anim_frame = 0;
            for (int i = 0; i < 20; i++) { takt(); pose(a, p, 1); }
            a->rot_y = (int16_t)(a->rot_y + 2048);
            a->motion = 1;
            double max_w = 0;
            for (int i = 0; i < 60; i++) {
                re15_skel_pose_t q[RE15_EMD_MAX_BONES];
                int32_t g0[3], g1[3];
                takt();
                a->anim_frame = i % 20;                      /* Gang (Clip 1), Bild 0-19 */
                pose(a, q, 0);
                pose(a, p, 1);
                if (!re15_schwanz_r41_gelenk(20, 0, g0) || !re15_schwanz_r41_gelenk(20, 1, g1)) continue;
                /* Keyframe-Gelenke 0/1 (Modellraum) -> Welt wie die Kette: gleiche Gier */
                const double cs = re15_cos_q12(a->rot_y) / 4096.0, sn = re15_sin_q12(a->rot_y) / 4096.0;
                const double kx = q[17].trans[0] - q[16].trans[0], ky = q[17].trans[1] - q[16].trans[1], kz = q[17].trans[2] - q[16].trans[2];
                const double wx_ = cs * kx + sn * kz, wz_ = -sn * kx + cs * kz;
                const double dx = g1[0] - g0[0], dy = g1[1] - g0[1], dz = g1[2] - g0[2];
                const double c = (dx * wx_ + dy * ky + dz * wz_) /
                                 (sqrt(dx * dx + dy * dy + dz * dz) * sqrt(wx_ * wx_ + ky * ky + wz_ * wz_) + 1e-9);
                const double w = acos(c > 1 ? 1 : (c < -1 ? -1 : c)) * 180.0 / 3.14159265;
                if (w > max_w) max_w = w;
            }
            if (lauf == 0) CHECK(max_w <= 1.0, "V8-Kette: erstes Glied %.1f Grad neben dem Keyframe ohne Szenenziel (Soll <= 1)", max_w);
            fprintf(stderr, "Schwanz %s: erstes Glied hoechstens %.1f Grad neben dem Keyframe (Drehung 180 Grad, Gang)\n",
                    lauf ? "ohne V8-Erkennung (Gegenprobe)" : "V8-Kette", max_w);
        }
        s_skel.bone_relative_pos[16][0] = wx;
        a->motion = 0;
        re15_schwanz_r41_reset();
    }

    /* (12) Runde 41 SF V8 Schwanz-Bewegung (Bauplan 3, re15_schwanz_r41_rolle): die Rolle dreht jedes Glied NUR um seine eigene Achse -
     *      Gelenklagen (trans) und Gliedachsen (lokale y) bleiben gleich, die Bauchseite (+e_h, beim Laden aus der Flachachse der Meshes
     *      gemessen; Spiel-Asset (0,8956, 0, 0,4449), werkzeug quer.py) zeigt zur Kamera (Modus 1, Rolle <= 10 Grad an den Gliedern 18-22,
     *      Glied 16/17 laufen mit je hoechstens 30 Grad ein) bzw. die Flanke zur Kamera (Modus 2, 90 +- 10 Grad); Verdrehung zwischen
     *      Nachbargliedern hoechstens 30 Grad (+1 Rundung) bzw. die des Keyframes. Gegenprobe: ohne Rolle liegt der Bauch nicht ueberall
     *      unter 10 Grad. Dazu wird die Bank 0x30 mit dem Spiel-Asset angelegt (die Bauchachse liest die Meshes der Bank). */
    {
        re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
        if (!eb) eb = re15_enemy_alloc(0x30u);
        CHECK(eb != NULL, "Rolle: Bank 0x30 nicht anlegbar");
        if (eb) {
            eb->md1 = s_md1; eb->skel = s_skel; eb->anim = s_anim; eb->ok = 1;
            static const double eh[3] = { 0.8956, 0.0, 0.4449 };
            const re15_emd_skeleton_t *sk = &eb->skel;
            re15_schwanz_r41_reset();
            a->motion = 0; a->anim_frame = 0; a->rot_y = 1300;
            re15_skel_pose_t p0[RE15_EMD_MAX_BONES], p1[RE15_EMD_MAX_BONES];
            const double cs = re15_cos_q12(a->rot_y) / 4096.0, sn = re15_sin_q12(a->rot_y) / 4096.0;
            /* Kamera vor Birkin (Modellraum +x = vorn), 8000 E. entfernt, 1500 E. hoch - in Welt umgerechnet */
            const double km[3] = { 8000.0, -1500.0, 0.0 };
            const int32_t kam[3] = { (int32_t)(a->x + cs * km[0] + sn * km[2]), (int32_t)(a->y + km[1]), (int32_t)(a->z - sn * km[0] + cs * km[2]) };
            for (int modus = 0; modus <= 2; modus++) {
                for (int i = 0; i < 30; i++) {
                    takt();
                    int kf = re15_compute_actor_kf(&s_anim, sk, a, -1, a->anim_frame);
                    re15_skel_compute_pose(sk, kf, p0);
                    re15_schwanz_r41_rolle(20, 0, NULL, 0);
                    re15_schwanz_r41_pose(a, sk, p0);              /* Kettenschritt dieses Takts, ohne Rolle */
                }
                int kf = re15_compute_actor_kf(&s_anim, sk, a, -1, a->anim_frame);
                re15_skel_compute_pose(sk, kf, p1);
                re15_schwanz_r41_rolle(20, modus, modus ? kam : NULL, modus ? 4096 : 0);
                re15_schwanz_r41_pose(a, sk, p1);                  /* gleicher Takt: nur die Knochen neu, Kette unveraendert */
                double d_lage = 0, d_achse = 0, phi_max = 0, phi_min = 180, tw_max = 0;
                double e_vor[3] = { 0, 0, 0 }, a_vor[3] = { 0, 0, 0 };
                for (int b = 16; b <= 22; b++) {
                    double ax0[3], ax1[3], bauch[3], mid[3], v[3], n0 = 0, n1 = 0;
                    for (int k = 0; k < 3; k++) {
                        d_lage = fmax(d_lage, fabs((double)p1[b].trans[k] - p0[b].trans[k]));
                        ax0[k] = p0[b].rot[k * 3 + 1]; ax1[k] = p1[b].rot[k * 3 + 1];
                        n0 += ax0[k] * ax0[k]; n1 += ax1[k] * ax1[k];
                    }
                    n0 = sqrt(n0); n1 = sqrt(n1);
                    double dot = 0, nb = 0;
                    for (int k = 0; k < 3; k++) { ax0[k] /= n0; ax1[k] /= n1; dot += ax0[k] * ax1[k]; }
                    d_achse = fmax(d_achse, acos(dot > 1 ? 1 : dot) * 180.0 / 3.14159265);
                    for (int k = 0; k < 3; k++) bauch[k] = p1[b].rot[k * 3] * eh[0] + p1[b].rot[k * 3 + 2] * eh[2];
                    double db = 0; for (int k = 0; k < 3; k++) db += bauch[k] * ax1[k];
                    for (int k = 0; k < 3; k++) { bauch[k] -= db * ax1[k]; nb += bauch[k] * bauch[k]; }
                    nb = sqrt(nb); for (int k = 0; k < 3; k++) bauch[k] /= nb;
                    /* Kamera im Modellraum wie die Kette (zu_modell) */
                    const double dx = kam[0] - a->x, dz = kam[2] - a->z;
                    const double kmod[3] = { cs * dx - sn * dz, (double)kam[1] - a->y, sn * dx + cs * dz };
                    const int16_t *r = s_skel.bone_relative_pos[b < 22 ? b + 1 : 22];
                    const double L = sqrt((double)r[0] * r[0] + (double)r[1] * r[1] + (double)r[2] * r[2]);
                    double dv = 0, nv = 0, c = 0;
                    for (int k = 0; k < 3; k++) { mid[k] = p1[b].trans[k] + ax1[k] * L * 0.5; v[k] = kmod[k] - mid[k]; dv += v[k] * ax1[k]; }
                    for (int k = 0; k < 3; k++) { v[k] -= dv * ax1[k]; nv += v[k] * v[k]; }
                    nv = sqrt(nv);
                    for (int k = 0; k < 3; k++) c += bauch[k] * v[k] / nv;
                    const double phi = acos(c > 1 ? 1 : (c < -1 ? -1 : c)) * 180.0 / 3.14159265;
                    if (b >= 18 && phi > phi_max) phi_max = phi;
                    if (b >= 18 && phi < phi_min) phi_min = phi;
                    if (b > 16) {                                 /* Verdrehung gegen das Vorglied: Bauch des Vorglieds parallel verschoben */
                        double cr[3] = { a_vor[1] * ax1[2] - a_vor[2] * ax1[1], a_vor[2] * ax1[0] - a_vor[0] * ax1[2],
                                         a_vor[0] * ax1[1] - a_vor[1] * ax1[0] };
                        const double cc = a_vor[0] * ax1[0] + a_vor[1] * ax1[1] + a_vor[2] * ax1[2];
                        double et[3];
                        for (int k = 0; k < 3; k++) {
                            const double kx = cr[(k + 1) % 3] * e_vor[(k + 2) % 3] - cr[(k + 2) % 3] * e_vor[(k + 1) % 3];
                            const double kd = cr[0] * e_vor[0] + cr[1] * e_vor[1] + cr[2] * e_vor[2];
                            et[k] = e_vor[k] * cc + kx + cr[k] * kd / (1.0 + cc);
                        }
                        double de = 0, ne = 0, ct = 0;
                        for (int k = 0; k < 3; k++) de += et[k] * ax1[k];
                        for (int k = 0; k < 3; k++) { et[k] -= de * ax1[k]; ne += et[k] * et[k]; }
                        ne = sqrt(ne);
                        for (int k = 0; k < 3; k++) ct += et[k] / ne * bauch[k];
                        tw_max = fmax(tw_max, acos(ct > 1 ? 1 : (ct < -1 ? -1 : ct)) * 180.0 / 3.14159265);
                    }
                    for (int k = 0; k < 3; k++) { e_vor[k] = bauch[k]; a_vor[k] = ax1[k]; }
                }
                fprintf(stderr, "Schwanz Rolle Modus %d: Lage %.1f E., Achse %.2f Grad, Bauch-Kamera Glied 18-22 %.1f..%.1f Grad, Verdrehung max %.1f\n",
                        modus, d_lage, d_achse, phi_min, phi_max, tw_max);
                CHECK(d_lage <= 1.0 && d_achse <= 0.5, "Rolle Modus %d: Gelenklage %.1f E. / Gliedachse %.2f Grad veraendert", modus, d_lage, d_achse);
                if (modus == 1) CHECK(phi_max <= 10.0, "Rolle Modus 1: Bauch %.1f Grad neben der Kamera (Soll <= 10)", phi_max);
                if (modus == 2) CHECK(phi_min >= 80.0 && phi_max <= 100.0, "Rolle Modus 2: Flanke %.1f..%.1f Grad (Soll 90 +- 10)", phi_min, phi_max);
                if (modus) CHECK(tw_max <= 31.0, "Rolle Modus %d: Verdrehung %.1f Grad zwischen Nachbargliedern (Soll <= 30)", modus, tw_max);
                if (modus == 0) CHECK(phi_max > 10.0, "Gegenprobe ohne Rolle: Bauch ueberall unter 10 Grad (%.1f) - Pruefung waere leer", phi_max);
            }
            re15_schwanz_r41_rolle(20, 0, NULL, 0);
            eb->ok = 0; eb->type = 0;
            re15_schwanz_r41_reset();
        }
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
