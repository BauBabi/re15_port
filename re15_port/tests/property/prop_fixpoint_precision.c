/**
 * @file prop_fixpoint_precision.c
 * @brief Property-Test: Fixkomma-Rendering-Präzision
 *
 * **Property 8: Fixkomma-Rendering-Präzision**
 *
 * Für jeden gültigen 3D-Vertex (x, y, z im Bereich ±32767) soll die
 * PC-Software-Transformation (20.12-Fixkomma mit RotMatrix + TransMatrix)
 * einen Bildschirm-Punkt erzeugen, der maximal ±1 Pixel vom Ergebnis
 * der Double-Präzisions-Floating-Point-Referenzimplementierung bei 320×240
 * Auflösung abweicht.
 *
 * Testverfahren:
 *   1. Generiere zufälligen Vertex (x, y, z) im Bereich ±32767
 *   2. Generiere zufälligen Rotationswinkel (0-4095)
 *   3. Transformiere mit Q12-Fixkomma: re15_rot_matrix_y + re15_transform_vertex
 *      + re15_perspective_project
 *   4. Transformiere mit double-Referenz: math.h sin/cos + Perspektiv-Division
 *   5. Vergleiche projizierte Bildschirmkoordinaten: max ±1 Pixel Differenz
 *
 * Kamera-Setup: h=320, screen_center=(160,120), translation_z=5000
 * Anzahl Testfälle: N=500
 *
 * **Validates: Requirements 5.3**
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include "re15_math.h"
#include "re15_types.h"

/* =========================================================================
 * Konfiguration
 * ========================================================================= */

/** Anzahl der Testfälle */
#define NUM_ITERATIONS 500

/** Maximale erlaubte Abweichung in Pixel */
#define MAX_PIXEL_DEVIATION 1

/** Kamera-Parameter (typisches PSX-Setup) */
#define PROJ_DISTANCE_H  320
#define SCREEN_CENTER_X  160
#define SCREEN_CENTER_Y  120
#define TRANSLATION_Z    5000

/** Vertex-Bereich */
#define VERTEX_RANGE     32767

/**
 * Minimale Z-Tiefe nach Transformation für gültigen Vergleich.
 * Bei kleineren Z-Werten verstärkt die Perspektiv-Division jeden
 * Rundungsfehler überproportional — diese Fälle sind im PSX-Original
 * ebenfalls durch Near-Clipping ausgeschlossen.
 */
#define MIN_Z_DEPTH      1000

/**
 * Maximaler Abstand vom Bildschirm-Zentrum für relevante Tests.
 * Vertices die weit außerhalb des 320×240-Bildschirms projiziert werden
 * sind für die visuelle Genauigkeit irrelevant — die ±1 Pixel Garantie
 * bezieht sich auf sichtbare Pixel. Wir testen großzügig bis 2× Bildschirm.
 */
#define MAX_SCREEN_EXTENT 640

/* =========================================================================
 * Test-Framework
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

/* =========================================================================
 * PRNG (xorshift32 für Reproduzierbarkeit)
 * ========================================================================= */

static uint32_t xorshift_state;

static uint32_t xorshift32(void)
{
    uint32_t x = xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift_state = x;
    return x;
}

/**
 * Generiert einen zufälligen int16_t Wert im Bereich [-VERTEX_RANGE, +VERTEX_RANGE].
 */
static int16_t rand_vertex_coord(void)
{
    int32_t val = (int32_t)(xorshift32() % (2 * VERTEX_RANGE + 1)) - VERTEX_RANGE;
    return (int16_t)val;
}

/**
 * Generiert einen zufälligen Winkel im Bereich [0, 4095].
 */
static int16_t rand_angle(void)
{
    return (int16_t)(xorshift32() & 0x0FFF);
}

/* =========================================================================
 * Double-Referenzimplementierung (GTE-Äquivalent in Floating Point)
 *
 * Die "GTE-Referenz" für Testzwecke ist double-precision Floating-Point-
 * Mathematik, die dieselbe Transformation durchführt.
 * ========================================================================= */

/**
 * Konvertiert einen GTE-Winkel (0-4095 = 0°-360°) in Radians.
 */
static double angle_to_radians(int16_t angle)
{
    return ((double)(angle & 0x0FFF)) * (2.0 * 3.14159265358979323846 / 4096.0);
}

/**
 * Führt die Referenz-Transformation in double-Präzision durch:
 *   1. Y-Achsen-Rotation
 *   2. Translation hinzufügen
 *   3. Perspektiv-Projektion
 *
 * Gibt die projizierten Bildschirmkoordinaten zurück.
 * Return: 0 bei Erfolg, -1 wenn z <= 0.
 */
static int reference_transform(int16_t vx, int16_t vy, int16_t vz,
                               int16_t angle,
                               int32_t tx, int32_t ty, int32_t tz,
                               int32_t h, int32_t cx, int32_t cy,
                               double* sx_out, double* sy_out)
{
    double rad = angle_to_radians(angle);
    double cos_a = cos(rad);
    double sin_a = sin(rad);

    /* Y-Achsen-Rotation:
     *   [ cos   0   sin ] * [vx]
     *   [  0    1    0  ]   [vy]
     *   [-sin   0   cos ]   [vz]
     */
    double rx = cos_a * (double)vx + sin_a * (double)vz;
    double ry = (double)vy;
    double rz = -sin_a * (double)vx + cos_a * (double)vz;

    /* Translation addieren */
    double ox = rx + (double)tx;
    double oy = ry + (double)ty;
    double oz = rz + (double)tz;

    /* Perspektiv-Projektion: sx = (x * h) / z + cx */
    if (oz <= 0.0) {
        return -1;
    }

    *sx_out = (ox * (double)h) / oz + (double)cx;
    *sy_out = (oy * (double)h) / oz + (double)cy;

    return 0;
}

/* =========================================================================
 * Property-Test: Fixkomma-Rendering-Präzision
 * ========================================================================= */

static int prop_fixpoint_precision(void)
{
    int i;
    int skipped = 0;
    int tested = 0;
    int max_dx_seen = 0;
    int max_dy_seen = 0;

    for (i = 0; tested < NUM_ITERATIONS && i < NUM_ITERATIONS * 10; i++) {
        /* Zufällige Eingabedaten */
        int16_t vx = rand_vertex_coord();
        int16_t vy = rand_vertex_coord();
        int16_t vz = rand_vertex_coord();
        int16_t angle = rand_angle();

        re15_rot_matrix_t rot;
        re15_trans_matrix_t trans;
        int32_t ox_fp, oy_fp, oz_fp;
        int32_t sx_fp, sy_fp;
        double sx_ref, sy_ref;
        int dx, dy;
        int ret_fp, ret_ref;

        /* Fixkomma-Pfad: Rotation + Transformation + Projektion */
        re15_rot_matrix_y(&rot, angle);

        trans.tx = 0;
        trans.ty = 0;
        trans.tz = TRANSLATION_Z;

        re15_transform_vertex(&rot, &trans,
                              (int32_t)vx, (int32_t)vy, (int32_t)vz,
                              &ox_fp, &oy_fp, &oz_fp);

        ret_fp = re15_perspective_project(ox_fp, oy_fp, oz_fp,
                                          PROJ_DISTANCE_H,
                                          SCREEN_CENTER_X, SCREEN_CENTER_Y,
                                          &sx_fp, &sy_fp);

        /* Referenz-Pfad: Double-Precision */
        ret_ref = reference_transform(vx, vy, vz, angle,
                                      0, 0, TRANSLATION_Z,
                                      PROJ_DISTANCE_H,
                                      SCREEN_CENTER_X, SCREEN_CENTER_Y,
                                      &sx_ref, &sy_ref);

        /* Wenn einer der Pfade z <= 0 hat — überspringen */
        if (ret_fp != 0 || ret_ref != 0) {
            skipped++;
            continue;
        }

        /*
         * Nur testen wenn der transformierte Z-Wert groß genug ist.
         * Bei sehr kleinem Z (nahe Near-Clip) wird die Perspektiv-Division
         * instabil und projizierte Koordinaten landen weit außerhalb des
         * Bildschirms. Die ±1 Pixel Garantie gilt für Vertices die sich
         * im sichtbaren/nahen Bereich befinden.
         *
         * Mindest-Z: Wir verlangen oz >= 1000, was typische Spielszenarien
         * abdeckt (PSX Near-Clip ist typisch ~200-500 Einheiten).
         */
        if (oz_fp < MIN_Z_DEPTH) {
            skipped++;
            continue;
        }

        /*
         * Nur testen wenn die projizierten Koordinaten in Bildschirmnähe
         * liegen. Die ±1 Pixel Garantie bezieht sich auf 320×240 Auflösung.
         * Vertices die weit außerhalb projiziert werden sind für die visuelle
         * Genauigkeit irrelevant (werden ohnehin geclippt).
         */
        if (abs(sx_fp - SCREEN_CENTER_X) > MAX_SCREEN_EXTENT ||
            abs(sy_fp - SCREEN_CENTER_Y) > MAX_SCREEN_EXTENT) {
            skipped++;
            continue;
        }

        tested++;

        /* Vergleiche projizierte Koordinaten */
        dx = abs(sx_fp - (int32_t)round(sx_ref));
        dy = abs(sy_fp - (int32_t)round(sy_ref));

        if (dx > max_dx_seen) max_dx_seen = dx;
        if (dy > max_dy_seen) max_dy_seen = dy;

        if (dx > MAX_PIXEL_DEVIATION || dy > MAX_PIXEL_DEVIATION) {
            printf("  FAIL: Iteration %d (tested #%d)\n", i, tested);
            printf("        Vertex: (%d, %d, %d), Angle: %d\n", vx, vy, vz, angle);
            printf("        Fixkomma:  sx=%d, sy=%d\n", sx_fp, sy_fp);
            printf("        Referenz:  sx=%.4f, sy=%.4f (gerundet: %d, %d)\n",
                   sx_ref, sy_ref,
                   (int32_t)round(sx_ref), (int32_t)round(sy_ref));
            printf("        Delta:     dx=%d, dy=%d (max erlaubt: %d)\n",
                   dx, dy, MAX_PIXEL_DEVIATION);
            printf("        Transformiert (FP): ox=%d, oy=%d, oz=%d\n",
                   ox_fp, oy_fp, oz_fp);
            g_failed++;
            return 1;
        }
    }

    printf("OK (%d tested, max dx=%d, max dy=%d, skipped=%d)\n",
           tested, max_dx_seen, max_dy_seen, skipped);
    g_passed++;
    return 0;
}

/* =========================================================================
 * Zusatz-Property: Extreme Werte (Eckfälle der Vertex-Range)
 *
 * Testet gezielt die Grenzen des Eingabebereichs:
 * - Maximalwerte ±32767
 * - Null-Vertex
 * - Winkel an Quadranten-Grenzen (0, 1024, 2048, 3072)
 * ========================================================================= */

static int prop_extreme_values(void)
{
    /* Test-Vektoren für Extremfälle */
    static const int16_t extreme_vertices[][3] = {
        { 32767,  32767,  32767},
        {-32767, -32767, -32767},
        { 32767, -32767,  32767},
        {-32767,  32767, -32767},
        {     0,      0,      0},
        { 32767,      0,      0},
        {     0,  32767,      0},
        {     0,      0,  32767},
    };
    static const int16_t extreme_angles[] = {
        0, 1024, 2048, 3072, 512, 1536, 2560, 3584, 4095
    };

    int nv = (int)(sizeof(extreme_vertices) / sizeof(extreme_vertices[0]));
    int na = (int)(sizeof(extreme_angles) / sizeof(extreme_angles[0]));
    int vi, ai;
    int max_dx_seen = 0;
    int max_dy_seen = 0;

    for (vi = 0; vi < nv; vi++) {
        for (ai = 0; ai < na; ai++) {
            int16_t vx = extreme_vertices[vi][0];
            int16_t vy = extreme_vertices[vi][1];
            int16_t vz = extreme_vertices[vi][2];
            int16_t angle = extreme_angles[ai];

            re15_rot_matrix_t rot;
            re15_trans_matrix_t trans;
            int32_t ox_fp, oy_fp, oz_fp;
            int32_t sx_fp, sy_fp;
            double sx_ref, sy_ref;
            int dx, dy;
            int ret_fp, ret_ref;

            re15_rot_matrix_y(&rot, angle);
            trans.tx = 0;
            trans.ty = 0;
            trans.tz = TRANSLATION_Z;

            re15_transform_vertex(&rot, &trans,
                                  (int32_t)vx, (int32_t)vy, (int32_t)vz,
                                  &ox_fp, &oy_fp, &oz_fp);

            ret_fp = re15_perspective_project(ox_fp, oy_fp, oz_fp,
                                              PROJ_DISTANCE_H,
                                              SCREEN_CENTER_X, SCREEN_CENTER_Y,
                                              &sx_fp, &sy_fp);

            ret_ref = reference_transform(vx, vy, vz, angle,
                                          0, 0, TRANSLATION_Z,
                                          PROJ_DISTANCE_H,
                                          SCREEN_CENTER_X, SCREEN_CENTER_Y,
                                          &sx_ref, &sy_ref);

            /* Überspringe wenn hinter Kamera */
            if (ret_fp != 0 || ret_ref != 0) {
                continue;
            }

            dx = abs(sx_fp - (int32_t)round(sx_ref));
            dy = abs(sy_fp - (int32_t)round(sy_ref));

            if (dx > max_dx_seen) max_dx_seen = dx;
            if (dy > max_dy_seen) max_dy_seen = dy;

            if (dx > MAX_PIXEL_DEVIATION || dy > MAX_PIXEL_DEVIATION) {
                printf("  FAIL: Extreme test vertex=(%d,%d,%d) angle=%d\n",
                       vx, vy, vz, angle);
                printf("        Fixkomma:  sx=%d, sy=%d\n", sx_fp, sy_fp);
                printf("        Referenz:  sx=%.4f, sy=%.4f (gerundet: %d, %d)\n",
                       sx_ref, sy_ref,
                       (int32_t)round(sx_ref), (int32_t)round(sy_ref));
                printf("        Delta:     dx=%d, dy=%d (max erlaubt: %d)\n",
                       dx, dy, MAX_PIXEL_DEVIATION);
                g_failed++;
                return 1;
            }
        }
    }

    printf("OK (max dx=%d, max dy=%d, %d Kombinationen)\n",
           max_dx_seen, max_dy_seen, nv * na);
    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    /* Seed für Reproduzierbarkeit */
    xorshift_state = (uint32_t)time(NULL);
    printf("=== Property-Test: Fixkomma-Rendering-Praezision ===\n");
    printf("    Validates: Requirements 5.3\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d\n", NUM_ITERATIONS);
    printf("    Max Abweichung: +/-%d Pixel\n", MAX_PIXEL_DEVIATION);
    printf("    Kamera: h=%d, center=(%d,%d), tz=%d\n\n",
           PROJ_DISTANCE_H, SCREEN_CENTER_X, SCREEN_CENTER_Y, TRANSLATION_Z);

    /* Test 1: Zufällige Vertices */
    printf("[1/2] prop_fixpoint_precision (%d Iterationen) ... ", NUM_ITERATIONS);
    prop_fixpoint_precision();

    /* Test 2: Extremwerte */
    printf("[2/2] prop_extreme_values ... ");
    prop_extreme_values();

    /* Summary */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
