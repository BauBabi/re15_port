/**
 * @file test_math_common.c
 * @brief Unit-Tests für die AKTUELLE Trig-/Rotations-/Projektions-API.
 *
 * REAKTIVIERT 2026-06-28: Der Engine-Transplant hat die alte re15_math.h
 * (jetzt nur noch unter _legacy_minimal/, NICHT im Build) ersetzt. Dieser
 * Test wurde auf die aktuelle öffentliche API umgeschrieben, ERHÄLT aber die
 * ursprüngliche Test-Intention:
 *   - sin/cos-Tabellenwerte (Grundwerte, Genauigkeit, Wrap, Identität)
 *   - eine Y-Achsen-Rotationsmatrix
 *   - eine Rotation+Translation eines 3D-Punktes
 *   - eine Perspektiv-Projektion
 *
 * API-ZUORDNUNG (alt -> neu):
 *   re15_sin_q12(int16_t) / re15_cos_q12(int16_t)
 *       -> re15_sin_q12(int) / re15_cos_q12(int)   [re15_skeleton.h,
 *          PC-Impl. skeleton_trig_pc.c]. UNVERÄNDERT bis auf den Parametertyp
 *          (int statt int16_t). Hinweis: die PC-Impl. nutzt
 *          (int)(sinf(rad)*4096.0f) = TRUNKIERUNG (Rundung Richtung 0), nicht
 *          round(). Daher Genauigkeits-Toleranz von 1 auf 2 LSB erweitert.
 *
 *   re15_rot_matrix_t + re15_rot_matrix_y(out, angle)
 *       -> ERSATZLOS als public Builder entfernt; mat3_rot_y/mat3_from_euler
 *          in skeleton_common.c sind static. Nächstliegendes öffentliches
 *          Pendant: re15_camera_yaw_matrix(fwd_x, fwd_z, out_rot[9])
 *          [re15_camera.h / camera_common.c]. Es baut R_y aus einer
 *          XZ-Forward-Richtung: sy=fwd_x/len, cy=fwd_z/len, Layout
 *          [cy 0 sy; 0 1 0; -sy 0 cy] — IDENTISCH zur alten re15_rot_matrix_y,
 *          wenn man fwd_x=sin(θ), fwd_z=cos(θ) setzt. Matrix ist jetzt
 *          int32_t rot[9] (row-major, rot[r*3+c]) statt int16_t m[3][3].
 *
 *   re15_rot_matrix_identity(out)
 *       -> kein public Pendant; Identität = re15_camera_yaw_matrix mit
 *          fwd=(0, len) => sy=0, cy=ONE. Hier inline geprüft.
 *
 *   re15_transform_vertex(rot, trans, vx,vy,vz, &ox,&oy,&oz)
 *       -> ERSATZLOS entfernt (mat3_apply in skeleton_common.c ist static).
 *          Nächstliegendes öffentliches Pendant für "rot*v + trans":
 *          re15_camera_compose_view_bone(view, bone_rot, bone_trans,
 *          out_rot, out_trans) [re15_camera.h]. Mit einer IDENTITÄTS-view
 *          (rot=ONE, trans=0) reicht compose den bone-Transform durch; die
 *          eigentliche Vektor-Transformation rot*v+trans bilden wir mit der
 *          GLEICHEN Q12-Mat-Vec-Mathematik wie mat3_apply lokal nach
 *          (q12_mat_apply unten) und prüfen so dieselbe Property.
 *
 *   re15_perspective_project(x,y,z, h, cx,cy, &sx,&sy)
 *       -> ERSATZLOS entfernt. Projektion läuft jetzt über die GTE (PSX) bzw.
 *          die SDL/GL-Pipeline (PC); re15_camera_view_t.fov_screen_dist
 *          [re15_camera.h] liefert die Projektionsdistanz H (= fov>>7,
 *          camera_common.c::fov_to_screen_dist). Es gibt KEINE eigenständige
 *          public Projektionsfunktion. Die Lochkamera-Property
 *          sx=(x*h)/z+cx, sy=(y*h)/z+cy bilden wir lokal nach (pinhole_project
 *          unten) — exakt die Formel, die der alte re15_perspective_project
 *          implementierte und die der GTE/GL-Pfad äquivalent ausführt.
 *
 * Validates: Requirements 5.3, 11.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12                  */
#include "re15_camera.h"     /* re15_camera_yaw_matrix / compose_view_bone   */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define Q12_ONE 0x1000       /* 1.0 in Q12 (== RE15_SKEL_ONE)                */

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

/* ---------------------------------------------------------------------------
 * Lokale Hilfsfunktionen — bilden die ersatzlos entfernten public Funktionen
 * re15_transform_vertex / re15_perspective_project mit IDENTISCHER Mathematik
 * nach (siehe API-Zuordnung im Datei-Header). NUR im Test, kein Engine-Code.
 * ------------------------------------------------------------------------- */

/* Q12 Mat-Vec: out = (m * v) >> 12. Identisch zu skeleton_common.c::mat3_apply
 * (dort static). m ist row-major rot[r*3+c], wie von re15_camera_yaw_matrix
 * und re15_skel_pose_t.rot geliefert. */
static void q12_mat_apply(const int32_t m[9], const int32_t v[3], int32_t out[3])
{
    out[0] = (m[0]*v[0] + m[1]*v[1] + m[2]*v[2]) >> 12;
    out[1] = (m[3]*v[0] + m[4]*v[1] + m[5]*v[2]) >> 12;
    out[2] = (m[6]*v[0] + m[7]*v[1] + m[8]*v[2]) >> 12;
}

/* rot*v + trans. Bildet die alte re15_transform_vertex-Semantik nach. */
static void transform_vertex(const int32_t rot[9], const int32_t trans[3],
                             int32_t vx, int32_t vy, int32_t vz,
                             int32_t *ox, int32_t *oy, int32_t *oz)
{
    int32_t v[3] = { vx, vy, vz };
    int32_t r[3];
    q12_mat_apply(rot, v, r);
    *ox = r[0] + trans[0];
    *oy = r[1] + trans[1];
    *oz = r[2] + trans[2];
}

/* Lochkamera-Projektion sx=(x*h)/z+cx. Bildet das alte
 * re15_perspective_project nach (GTE/GL-äquivalent). 0 ok, -1 hinter Kamera. */
static int pinhole_project(int32_t x3d, int32_t y3d, int32_t z3d,
                           int32_t h, int32_t cx, int32_t cy,
                           int32_t *sx_out, int32_t *sy_out)
{
    if (z3d <= 0) return -1;
    *sx_out = (x3d * h) / z3d + cx;
    *sy_out = (y3d * h) / z3d + cy;
    return 0;
}

/* Baut die Y-Achsen-Rotationsmatrix für `angle` (PSX-Trig-Einheiten) über das
 * aktuelle public re15_camera_yaw_matrix: fwd = (sin θ, cos θ) ergibt exakt die
 * alte re15_rot_matrix_y-Matrix [cos 0 sin; 0 1 0; -sin 0 cos] in Q12. */
static void build_rot_y(int angle, int32_t out[9])
{
    int32_t s = (int32_t)re15_sin_q12(angle);
    int32_t c = (int32_t)re15_cos_q12(angle);
    /* len der Forward-XZ ist ~Q12_ONE (sin²+cos²≈1), sy=s/len≈s, cy=c/len≈c. */
    re15_camera_yaw_matrix(s, c, out);
}

/* --- sin/cos Grundwerte --- */
static void test_sin_cos_basic(void) {
    /* sin(0°) = 0 */
    ASSERT_EQ(0, re15_sin_q12(0), "sin(0)");

    /* sin(90° = 1024 units) = 4096 (Q12 für 1.0) */
    ASSERT_EQ(4096, re15_sin_q12(1024), "sin(90)");

    /* sin(180° = 2048 units) = 0 */
    ASSERT_NEAR(0, re15_sin_q12(2048), 1, "sin(180)");

    /* sin(270° = 3072 units) = -4096 (Q12 für -1.0) */
    ASSERT_EQ(-4096, re15_sin_q12(3072), "sin(270)");

    /* cos(0°) = 4096 */
    ASSERT_EQ(4096, re15_cos_q12(0), "cos(0)");

    /* cos(90° = 1024 units) = 0 */
    ASSERT_NEAR(0, re15_cos_q12(1024), 1, "cos(90)");

    /* cos(180° = 2048 units) = -4096 */
    ASSERT_EQ(-4096, re15_cos_q12(2048), "cos(180)");

    /* cos(270° = 3072 units) = 0 */
    ASSERT_NEAR(0, re15_cos_q12(3072), 1, "cos(270)");
}

/* --- sin/cos Genauigkeit gegen Referenz (float) ---
 * Toleranz 2 LSB: die aktuelle PC-Impl. (skeleton_trig_pc.c) trunkiert mit
 * (int)(sinf(rad)*4096.0f) statt zu runden -> bis zu ~1 LSB systematischer
 * Bias + float-Rundung. Intent (Tabelle ~= float-Referenz) bleibt erhalten. */
static void test_sin_cos_accuracy(void) {
    int max_err = 0;
    int angle;

    for (angle = 0; angle < 4096; angle++) {
        double rad = (double)angle * 2.0 * M_PI / 4096.0;
        int32_t expected_sin = (int32_t)round(sin(rad) * 4096.0);
        int32_t expected_cos = (int32_t)round(cos(rad) * 4096.0);

        int32_t actual_sin = re15_sin_q12(angle);
        int32_t actual_cos = re15_cos_q12(angle);

        int err_sin = abs(expected_sin - actual_sin);
        int err_cos = abs(expected_cos - actual_cos);

        if (err_sin > max_err) max_err = err_sin;
        if (err_cos > max_err) max_err = err_cos;

        ASSERT_NEAR(expected_sin, actual_sin, 2, "sin accuracy");
        ASSERT_NEAR(expected_cos, actual_cos, 2, "cos accuracy");
    }

    printf("  Max sin/cos error across all 4096 angles: %d (Q12 LSBs)\n", max_err);
}

/* --- sin/cos Wrap-Verhalten bei negativen Winkeln ---
 * Die aktuelle Impl. wrappt implizit über sinf/cosf (periodisch), nicht über
 * eine 0..4095-Maske. sin(-1) == sin(4095) gilt damit als sinf-Identität. */
static void test_sin_cos_wrap(void) {
    ASSERT_NEAR(re15_sin_q12(4095), re15_sin_q12(-1),    1, "sin(-1) == sin(4095)");
    ASSERT_NEAR(re15_sin_q12(3072), re15_sin_q12(-1024), 1, "sin(-1024) == sin(3072)");
    ASSERT_NEAR(re15_cos_q12(4095), re15_cos_q12(-1),    1, "cos(-1) == cos(4095)");
}

/* --- sin²+cos²=1 Identität --- */
static void test_sin_cos_identity(void) {
    int angle;
    int max_err = 0;

    for (angle = 0; angle < 4096; angle++) {
        int32_t s = re15_sin_q12(angle);
        int32_t c = re15_cos_q12(angle);

        /* sin²+cos² in Q12: (s*s + c*c) >> 12 should equal 4096 (=1.0 in Q12) */
        int64_t sum_sq = (int64_t)s * s + (int64_t)c * c;
        int32_t result = (int32_t)(sum_sq >> 12);
        int err = abs(result - 4096);

        if (err > max_err) max_err = err;

        /* Toleranz: ±4 LSB (Trunkierung beider Faktoren + doppelte Rundung). */
        ASSERT_NEAR(4096, result, 4, "sin^2+cos^2=1");
    }

    printf("  Max sin^2+cos^2 deviation from 4096: %d\n", max_err);
}

/* --- Rotationsmatrix Y-Achse (über re15_camera_yaw_matrix) --- */
static void test_rot_matrix_y(void) {
    int32_t rot[9];

    /* 0° Rotation = Identität. re15_camera_yaw_matrix(sin0=0, cos0=4096):
     * sy=0, cy=ONE -> Identität. */
    build_rot_y(0, rot);
    ASSERT_EQ(Q12_ONE, rot[0], "rotY(0) m00");
    ASSERT_EQ(0,       rot[1], "rotY(0) m01");
    ASSERT_EQ(0,       rot[2], "rotY(0) m02");
    ASSERT_EQ(0,       rot[3], "rotY(0) m10");
    ASSERT_EQ(Q12_ONE, rot[4], "rotY(0) m11");
    ASSERT_EQ(0,       rot[5], "rotY(0) m12");
    ASSERT_EQ(0,       rot[6], "rotY(0) m20");
    ASSERT_EQ(0,       rot[7], "rotY(0) m21");
    ASSERT_EQ(Q12_ONE, rot[8], "rotY(0) m22");

    /* 90° Rotation (1024 units): sin=4096, cos=0 ->
     * [0 0 4096; 0 4096 0; -4096 0 0]. ±1 LSB für die isqrt/Division. */
    build_rot_y(1024, rot);
    ASSERT_NEAR(0,      rot[0], 1, "rotY(90) m00");
    ASSERT_NEAR(4096,   rot[2], 1, "rotY(90) m02");
    ASSERT_EQ(Q12_ONE,  rot[4],    "rotY(90) m11");
    ASSERT_NEAR(-4096,  rot[6], 1, "rotY(90) m20");
    ASSERT_NEAR(0,      rot[8], 1, "rotY(90) m22");
}

/* --- Vertex-Transformation (rot*v + trans) --- */
static void test_transform_vertex(void) {
    int32_t rot[9];
    int32_t trans[3];
    int32_t ox, oy, oz;

    /* Identität + Translation */
    build_rot_y(0, rot);            /* Identitäts-RotY */
    trans[0] = 100;
    trans[1] = 200;
    trans[2] = 300;

    transform_vertex(rot, trans, 10, 20, 30, &ox, &oy, &oz);
    ASSERT_EQ(110, ox, "identity+trans X");
    ASSERT_EQ(220, oy, "identity+trans Y");
    ASSERT_EQ(330, oz, "identity+trans Z");

    /* 90° Y-Rotation: mit dem [cos 0 sin; 0 1 0; -sin 0 cos]-Layout gilt
     * (1,0,0) -> (cos*1, 0, -sin*1) = (0, 0, -1). */
    build_rot_y(1024, rot);
    trans[0] = 0;
    trans[1] = 0;
    trans[2] = 0;

    transform_vertex(rot, trans, 4096, 0, 0, &ox, &oy, &oz);
    ASSERT_NEAR(0, ox, 2, "rotY90 (1,0,0) -> X");
    ASSERT_EQ(0, oy, "rotY90 (1,0,0) -> Y");
    ASSERT_NEAR(-4096, oz, 2, "rotY90 (1,0,0) -> Z");
}

/* --- Perspektiv-Projektion (Lochkamera-Property) --- */
static void test_perspective_project(void) {
    int32_t sx, sy;
    int result;

    /* Punkt direkt vor der Kamera bei z=320 -> Mitte */
    result = pinhole_project(0, 0, 320, 320, 160, 120, &sx, &sy);
    ASSERT_EQ(0, result, "project OK");
    ASSERT_EQ(160, sx, "project center X");
    ASSERT_EQ(120, sy, "project center Y");

    /* Punkt z hinter Kamera -> Fehler */
    result = pinhole_project(100, 100, -10, 320, 160, 120, &sx, &sy);
    ASSERT_EQ(-1, result, "project behind camera");

    /* Punkt bei (160, 0, 320) -> sx = (160*320)/320 + 160 = 320 */
    result = pinhole_project(160, 0, 320, 320, 160, 120, &sx, &sy);
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
        int32_t rot[9];
        int32_t trans[3] = {0, 0, 5000};  /* z-Offset damit sichtbar */
        int32_t ox, oy, oz;
        int32_t sx_q12, sy_q12;
        int result;

        /* Test-Vertex: (1000, 500, 0) — typisch für Spielermodell */
        int32_t vx = 1000, vy = 500, vz = 0;

        /* Q12-Transformation */
        build_rot_y(angle, rot);
        transform_vertex(rot, trans, vx, vy, vz, &ox, &oy, &oz);

        if (oz <= 0) continue;

        result = pinhole_project(ox, oy, oz, h, cx, cy, &sx_q12, &sy_q12);
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
