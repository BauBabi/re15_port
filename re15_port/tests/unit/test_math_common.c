/**
 * @file test_math_common.c
 * @brief Unit-Tests für re15_math.h — sin/cos Tabellen und 3D-Transformationen
 *
 * Validates: Requirements 5.3, 11.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "re15_math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT_EQ(expected, actual, msg) do { \
    tests_run++; \
    if ((expected) != (actual)) { \
        printf("FAIL [%s]: expected %d, got %d\n", msg, (int)(expected), (int)(actual)); \
        tests_failed++; \
    } \
} while(0)

#define ASSERT_NEAR(expected, actual, tolerance, msg) do { \
    tests_run++; \
    int _diff = abs((int)(expected) - (int)(actual)); \
    if (_diff > (tolerance)) { \
        printf("FAIL [%s]: expected %d, got %d (diff=%d, tol=%d)\n", \
               msg, (int)(expected), (int)(actual), _diff, (int)(tolerance)); \
        tests_failed++; \
    } \
} while(0)

/* --- sin/cos Grundwerte --- */
static void test_sin_cos_basic(void) {
    /* sin(0°) = 0 */
    ASSERT_EQ(0, re15_sin_q12(0), "sin(0)");

    /* sin(90° = 1024 units) = 4096 (Q12 für 1.0) */
    ASSERT_EQ(4096, re15_sin_q12(1024), "sin(90)");

    /* sin(180° = 2048 units) = 0 */
    ASSERT_EQ(0, re15_sin_q12(2048), "sin(180)");

    /* sin(270° = 3072 units) = -4096 (Q12 für -1.0) */
    ASSERT_EQ(-4096, re15_sin_q12(3072), "sin(270)");

    /* cos(0°) = 4096 */
    ASSERT_EQ(4096, re15_cos_q12(0), "cos(0)");

    /* cos(90° = 1024 units) = 0 */
    ASSERT_EQ(0, re15_cos_q12(1024), "cos(90)");

    /* cos(180° = 2048 units) = -4096 */
    ASSERT_EQ(-4096, re15_cos_q12(2048), "cos(180)");

    /* cos(270° = 3072 units) = 0 */
    ASSERT_EQ(0, re15_cos_q12(3072), "cos(270)");
}

/* --- sin/cos Genauigkeit gegen Referenz (float) --- */
static void test_sin_cos_accuracy(void) {
    int max_err = 0;
    int angle;

    for (angle = 0; angle < 4096; angle++) {
        double rad = (double)angle * 2.0 * M_PI / 4096.0;
        int32_t expected_sin = (int32_t)round(sin(rad) * 4096.0);
        int32_t expected_cos = (int32_t)round(cos(rad) * 4096.0);

        int32_t actual_sin = re15_sin_q12((int16_t)angle);
        int32_t actual_cos = re15_cos_q12((int16_t)angle);

        int err_sin = abs(expected_sin - actual_sin);
        int err_cos = abs(expected_cos - actual_cos);

        if (err_sin > max_err) max_err = err_sin;
        if (err_cos > max_err) max_err = err_cos;

        /* Toleranz: max 1 LSB Abweichung (±1 in Q12) */
        ASSERT_NEAR(expected_sin, actual_sin, 1, "sin accuracy");
        ASSERT_NEAR(expected_cos, actual_cos, 1, "cos accuracy");
    }

    printf("  Max sin/cos error across all 4096 angles: %d (Q12 LSBs)\n", max_err);
}

/* --- sin/cos Wrap-Verhalten bei negativen Winkeln --- */
static void test_sin_cos_wrap(void) {
    /* Negative Winkel sollen korrekt gewrappt werden */
    ASSERT_EQ(re15_sin_q12(4095), re15_sin_q12(-1), "sin(-1) == sin(4095)");
    ASSERT_EQ(re15_sin_q12(3072), re15_sin_q12(-1024), "sin(-1024) == sin(3072)");
    ASSERT_EQ(re15_cos_q12(4095), re15_cos_q12(-1), "cos(-1) == cos(4095)");
}

/* --- sin²+cos²=1 Identität --- */
static void test_sin_cos_identity(void) {
    int angle;
    int max_err = 0;

    for (angle = 0; angle < 4096; angle++) {
        int32_t s = re15_sin_q12((int16_t)angle);
        int32_t c = re15_cos_q12((int16_t)angle);

        /* sin²+cos² in Q12: (s*s + c*c) >> 12 should equal 4096 (=1.0 in Q12) */
        int64_t sum_sq = (int64_t)s * s + (int64_t)c * c;
        int32_t result = (int32_t)(sum_sq >> 12);
        int err = abs(result - 4096);

        if (err > max_err) max_err = err;

        /* Toleranz: ±2 LSB wegen doppelter Rundung */
        ASSERT_NEAR(4096, result, 2, "sin^2+cos^2=1");
    }

    printf("  Max sin^2+cos^2 deviation from 4096: %d\n", max_err);
}

/* --- Rotationsmatrix Y-Achse --- */
static void test_rot_matrix_y(void) {
    re15_rot_matrix_t rot;

    /* 0° Rotation = Identität */
    re15_rot_matrix_y(&rot, 0);
    ASSERT_EQ(4096, rot.m[0][0], "rotY(0) m00");
    ASSERT_EQ(0,    rot.m[0][1], "rotY(0) m01");
    ASSERT_EQ(0,    rot.m[0][2], "rotY(0) m02");
    ASSERT_EQ(0,    rot.m[1][0], "rotY(0) m10");
    ASSERT_EQ(4096, rot.m[1][1], "rotY(0) m11");
    ASSERT_EQ(0,    rot.m[1][2], "rotY(0) m12");
    ASSERT_EQ(0,    rot.m[2][0], "rotY(0) m20");
    ASSERT_EQ(0,    rot.m[2][1], "rotY(0) m21");
    ASSERT_EQ(4096, rot.m[2][2], "rotY(0) m22");

    /* 90° Rotation (1024 units) */
    re15_rot_matrix_y(&rot, 1024);
    ASSERT_EQ(0,     rot.m[0][0], "rotY(90) m00");
    ASSERT_EQ(4096,  rot.m[0][2], "rotY(90) m02");
    ASSERT_EQ(4096,  rot.m[1][1], "rotY(90) m11");
    ASSERT_EQ(-4096, rot.m[2][0], "rotY(90) m20");
    ASSERT_EQ(0,     rot.m[2][2], "rotY(90) m22");
}

/* --- Vertex-Transformation --- */
static void test_transform_vertex(void) {
    re15_rot_matrix_t rot;
    re15_trans_matrix_t trans;
    int32_t ox, oy, oz;

    /* Identität + Translation */
    re15_rot_matrix_identity(&rot);
    trans.tx = 100;
    trans.ty = 200;
    trans.tz = 300;

    re15_transform_vertex(&rot, &trans, 10, 20, 30, &ox, &oy, &oz);
    ASSERT_EQ(110, ox, "identity+trans X");
    ASSERT_EQ(220, oy, "identity+trans Y");
    ASSERT_EQ(330, oz, "identity+trans Z");

    /* 90° Y-Rotation: (1,0,0) → (0,0,-1) */
    re15_rot_matrix_y(&rot, 1024);
    trans.tx = 0;
    trans.ty = 0;
    trans.tz = 0;

    re15_transform_vertex(&rot, &trans, 4096, 0, 0, &ox, &oy, &oz);
    ASSERT_NEAR(0, ox, 1, "rotY90 (1,0,0) → X");
    ASSERT_EQ(0, oy, "rotY90 (1,0,0) → Y");
    ASSERT_NEAR(-4096, oz, 1, "rotY90 (1,0,0) → Z");
}

/* --- Perspektiv-Projektion --- */
static void test_perspective_project(void) {
    int32_t sx, sy;
    int result;

    /* Punkt direkt vor der Kamera bei z=320 → Mitte */
    result = re15_perspective_project(0, 0, 320, 320, 160, 120, &sx, &sy);
    ASSERT_EQ(0, result, "project OK");
    ASSERT_EQ(160, sx, "project center X");
    ASSERT_EQ(120, sy, "project center Y");

    /* Punkt z hinter Kamera → Fehler */
    result = re15_perspective_project(100, 100, -10, 320, 160, 120, &sx, &sy);
    ASSERT_EQ(-1, result, "project behind camera");

    /* Punkt bei (160, 0, 320) → sx = (160*320)/320 + 160 = 320 */
    result = re15_perspective_project(160, 0, 320, 320, 160, 120, &sx, &sy);
    ASSERT_EQ(0, result, "project right edge OK");
    ASSERT_EQ(320, sx, "project right X");
    ASSERT_EQ(120, sy, "project right Y");
}

/* --- Fixkomma-Rendering-Präzision: ±1 Pixel bei 320×240 --- */
static void test_rendering_precision(void) {
    /*
     * Simuliere eine Vertex-Transformation + Projektion und vergleiche
     * das Ergebnis gegen eine Floating-Point-Referenz.
     *
     * Anforderung: Max ±1 Pixel Abweichung bei 320×240.
     */
    int max_pixel_err_x = 0;
    int max_pixel_err_y = 0;
    int test_count = 0;
    int angle;

    const int32_t h = 320;    /* Projektionsdistanz */
    const int32_t cx = 160;   /* Bildschirm-Mitte X */
    const int32_t cy = 120;   /* Bildschirm-Mitte Y */

    /* Teste verschiedene Rotationswinkel mit einem Vertex */
    for (angle = 0; angle < 4096; angle += 64) {
        re15_rot_matrix_t rot;
        re15_trans_matrix_t trans = {0, 0, 5000};  /* z-Offset damit sichtbar */
        int32_t ox, oy, oz;
        int32_t sx_q12, sy_q12;
        int result;

        /* Test-Vertex: (1000, 500, 0) — typisch für Spielermodell */
        int32_t vx = 1000, vy = 500, vz = 0;

        /* Q12-Transformation */
        re15_rot_matrix_y(&rot, (int16_t)angle);
        re15_transform_vertex(&rot, &trans, vx, vy, vz, &ox, &oy, &oz);

        if (oz <= 0) continue;

        result = re15_perspective_project(ox, oy, oz, h, cx, cy,
                                          &sx_q12, &sy_q12);
        if (result != 0) continue;

        /* Float-Referenz */
        double rad = (double)angle * 2.0 * M_PI / 4096.0;
        double fcos = cos(rad);
        double fsin = sin(rad);

        /* Rotation um Y: x' = x*cos + z*sin, z' = -x*sin + z*cos */
        double fx = (double)vx * fcos + (double)vz * fsin;
        double fy = (double)vy;
        double fz = -(double)vx * fsin + (double)vz * fcos;

        /* Translation */
        fx += 0.0;
        fy += 0.0;
        fz += 5000.0;

        if (fz <= 0.0) continue;

        /* Projektion */
        double ref_sx = (fx * (double)h) / fz + (double)cx;
        double ref_sy = (fy * (double)h) / fz + (double)cy;

        int pixel_err_x = abs(sx_q12 - (int32_t)round(ref_sx));
        int pixel_err_y = abs(sy_q12 - (int32_t)round(ref_sy));

        if (pixel_err_x > max_pixel_err_x) max_pixel_err_x = pixel_err_x;
        if (pixel_err_y > max_pixel_err_y) max_pixel_err_y = pixel_err_y;

        test_count++;

        /* Max ±1 Pixel Toleranz */
        ASSERT_NEAR((int32_t)round(ref_sx), sx_q12, 1, "precision X");
        ASSERT_NEAR((int32_t)round(ref_sy), sy_q12, 1, "precision Y");
    }

    printf("  Rendering precision test: %d vertices, max err X=%d Y=%d pixels\n",
           test_count, max_pixel_err_x, max_pixel_err_y);
}

/* --- Hauptprogramm --- */
int main(void) {
    printf("=== test_math_common ===\n");

    printf("Test: sin/cos basic values\n");
    test_sin_cos_basic();

    printf("Test: sin/cos accuracy vs float reference\n");
    test_sin_cos_accuracy();

    printf("Test: sin/cos wrap (negative angles)\n");
    test_sin_cos_wrap();

    printf("Test: sin^2 + cos^2 = 1 identity\n");
    test_sin_cos_identity();

    printf("Test: rotation matrix Y-axis\n");
    test_rot_matrix_y();

    printf("Test: vertex transformation\n");
    test_transform_vertex();

    printf("Test: perspective projection\n");
    test_perspective_project();

    printf("Test: rendering precision (max +/-1 pixel at 320x240)\n");
    test_rendering_precision();

    printf("\n=== Results: %d tests, %d passed, %d failed ===\n",
           tests_run, tests_run - tests_failed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
