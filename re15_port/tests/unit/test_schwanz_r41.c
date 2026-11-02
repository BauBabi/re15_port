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
 *   (6) Nur die Sichel-Form: EM30 mit 16 Knochen bleibt unberuehrt. */
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

int main(void)
{
    size_t len = 0;
    uint8_t *blob = slurp(RE15_ASSET_PSX_DIR "/EMD/EM30_HANGAR.EMD", &len);
    CHECK(blob != NULL, "EM30_HANGAR.EMD fehlt");
    if (!blob) return 1;
    CHECK(re15_emd_parse_container(blob, len, &s_md1, &s_skel, &s_anim, NULL) == 0, "EM30_HANGAR nicht lesbar");
    CHECK(s_skel.bone_count == 23, "Knochen %d statt 23", s_skel.bone_count);

    re15_schwanz_r41_reset();
    re15_actor_t *a = &g_actors[20];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0x30u; a->x = -8488; a->z = -21651; a->rot_y = 1300;
    a->motion = 0x0B; a->anim_frame = 20;
    re15_skel_pose_t p[RE15_EMD_MAX_BONES], k[RE15_EMD_MAX_BONES];
    double sp[3], sk[3];

    /* (1) Ruhe */
    for (int i = 0; i < 90; i++) { takt(); CHECK(pose(a, p, 1), "Kette nicht angewandt (Ruhe)"); }
    pose(a, k, 0); spitze(p, sp); spitze(k, sk);
    CHECK(abst(sp, sk) < 60.0, "Ruhe: Spitze %.0f vom Keyframe (Soll < 60)", abst(sp, sk));
    verbunden(p, k, "Ruhe");
    for (int b = 0; b < 16; b++)
        CHECK(!memcmp(&p[b], &k[b], sizeof p[b]), "Knochen %d von der Kette veraendert", b);

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
            if (re15_schwanz_r41_gelenk(20, j, g)) CHECK(g[1] <= 0, "Gelenk %d unter dem Boden (y %d)", j, g[1]);
        }
    }
    end_ab = abst(sp, sk);
    CHECK(max_ab > 300.0, "Drehung: Spitze laeuft nicht nach (max %.0f vom Keyframe)", max_ab);
    CHECK(end_ab < 60.0, "Drehung: Kette schwingt nicht aus (%.0f nach 120 Takten)", end_ab);

    /* (5) Szenenvorgabe Seite 4000 (wie k_schwanz_seite) -> rechte Seite, Freigabe -> zurueck durch die Mitte */
    re15_schwanz_r41_seite(20, 4000);
    for (int i = 0; i < 25; i++) { takt(); pose(a, p, 1); }
    spitze(p, sp);
    CHECK(sp[2] < -800.0, "Vorgabe rechts: Spitze z %.0f (Soll < -800)", sp[2]);
    re15_schwanz_r41_seite(20, 0);
    int mitte = -1, links = -1;
    for (int i = 0; i < 60; i++) {
        takt(); pose(a, p, 1); spitze(p, sp);
        if (mitte < 0 && sp[2] > -200.0) mitte = i;
        if (links < 0 && sp[2] > 1000.0) links = i;
    }
    CHECK(mitte >= 1 && mitte <= 12, "Freigabe: Mitte nach %d Takten (Video h28 -> h30..h34)", mitte);
    CHECK(links > mitte && links <= 20, "Freigabe: linke Seite nach %d Takten (Video bis h38)", links);
    fprintf(stderr, "Schwanz: Nachlauf max %.0f, Rest %.0f; Freigabe Mitte nach %d, links nach %d Takten\n",
            max_ab, end_ab, mitte, links);

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
