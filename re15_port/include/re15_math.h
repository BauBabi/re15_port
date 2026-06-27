/**
 * @file re15_math.h
 * @brief Fixkomma-Mathematik (Q12) — sin/cos Lookup-Tabellen und 3D-Transformationen
 *
 * Implementiert die PSX-GTE-äquivalente Mathematik in Software für die PC-Plattform.
 * Verwendet das 20.12 Fixkomma-Format (Q12) mit 4096 Winkeleinheiten = 360°.
 *
 * Die sin/cos-Tabelle nutzt Viertelwellen-Symmetrie (1024 Einträge) und deckt
 * über Spiegelung alle 4096 Winkel ab. Jeder Eintrag ist int16_t im Q12-Format
 * (Bereich -4096 bis +4096 = -1.0 bis +1.0).
 *
 * Validates: Requirements 5.3, 11.2
 */
#ifndef RE15_MATH_H
#define RE15_MATH_H

#include "re15_types.h"

/* ============================================================================
 * Winkel-Konstanten (PSX-GTE-kompatibel: 4096 Einheiten = 360°)
 * ========================================================================= */

/** Volle Umdrehung in GTE-Winkeleinheiten */
#define RE15_ANGLE_FULL     4096

/** Halbe Umdrehung (180°) */
#define RE15_ANGLE_HALF     2048

/** Viertel-Umdrehung (90°) */
#define RE15_ANGLE_QUARTER  1024

/** Winkelmaske für Wrap auf 0-4095 */
#define RE15_ANGLE_MASK     0x0FFF

/* ============================================================================
 * Trigonometrische Funktionen (Q12 Fixkomma)
 * ========================================================================= */

/**
 * Sinus im Q12-Format.
 *
 * @param angle  Winkel in GTE-Einheiten (0-4095 = 0°-360°, automatisch gewrappt)
 * @return       sin(angle) als Q12-Wert (Bereich -4096 bis +4096)
 */
int32_t re15_sin_q12(int16_t angle);

/**
 * Cosinus im Q12-Format.
 *
 * @param angle  Winkel in GTE-Einheiten (0-4095 = 0°-360°, automatisch gewrappt)
 * @return       cos(angle) als Q12-Wert (Bereich -4096 bis +4096)
 */
int32_t re15_cos_q12(int16_t angle);

/* ============================================================================
 * 3×3 Rotationsmatrix (Q12 Fixkomma, PSX-GTE-kompatibel)
 * ========================================================================= */

/**
 * 3×3 Rotationsmatrix im Q12-Format.
 * Entspricht der PSX-GTE RotMatrix-Struktur.
 * Einträge sind int16_t (Q12: 1.0 = 4096, -1.0 = -4096).
 */
typedef struct {
    int16_t m[3][3];    /**< 3×3 Matrixeinträge, Reihenfolge [row][col] */
} re15_rot_matrix_t;

/* ============================================================================
 * Translationsvektor (Q12 Fixkomma)
 * ========================================================================= */

/**
 * Translationsvektor im Q12-Format.
 * Entspricht der PSX-GTE TransMatrix-Struktur.
 * Werte sind int32_t für ausreichende Genauigkeit bei Weltkoordinaten.
 */
typedef struct {
    int32_t tx;         /**< Translation X (Q12) */
    int32_t ty;         /**< Translation Y (Q12) */
    int32_t tz;         /**< Translation Z (Q12) */
} re15_trans_matrix_t;

/* ============================================================================
 * Matrix-Operationen
 * ========================================================================= */

/**
 * Setzt eine Rotationsmatrix für Rotation um die Y-Achse.
 * Äquivalent zur PSX-GTE RotMatrixY-Funktion.
 *
 * Ergebnis:
 *   [ cos(angle)   0   sin(angle) ]
 *   [     0        1       0      ]
 *   [-sin(angle)   0   cos(angle) ]
 *
 * @param out    Zeiger auf die zu setzende Matrix
 * @param angle  Rotationswinkel in GTE-Einheiten (0-4095)
 */
void re15_rot_matrix_y(re15_rot_matrix_t* out, int16_t angle);

/**
 * Setzt eine Rotationsmatrix für Rotation um die X-Achse.
 *
 * Ergebnis:
 *   [ 1       0           0      ]
 *   [ 0   cos(angle)  -sin(angle)]
 *   [ 0   sin(angle)   cos(angle)]
 *
 * @param out    Zeiger auf die zu setzende Matrix
 * @param angle  Rotationswinkel in GTE-Einheiten (0-4095)
 */
void re15_rot_matrix_x(re15_rot_matrix_t* out, int16_t angle);

/**
 * Setzt eine Rotationsmatrix für Rotation um die Z-Achse.
 *
 * Ergebnis:
 *   [ cos(angle)  -sin(angle)  0 ]
 *   [ sin(angle)   cos(angle)  0 ]
 *   [     0            0       1 ]
 *
 * @param out    Zeiger auf die zu setzende Matrix
 * @param angle  Rotationswinkel in GTE-Einheiten (0-4095)
 */
void re15_rot_matrix_z(re15_rot_matrix_t* out, int16_t angle);

/**
 * Setzt die Rotationsmatrix auf die Identitätsmatrix.
 *
 * @param out  Zeiger auf die zu setzende Matrix
 */
void re15_rot_matrix_identity(re15_rot_matrix_t* out);

/**
 * Multipliziert zwei Rotationsmatrizen: out = a * b.
 * Alle Einträge bleiben im Q12-Format.
 *
 * @param out  Ergebnis-Matrix (darf nicht a oder b sein)
 * @param a    Linke Matrix
 * @param b    Rechte Matrix
 */
void re15_rot_matrix_mul(re15_rot_matrix_t* out,
                         const re15_rot_matrix_t* a,
                         const re15_rot_matrix_t* b);

/* ============================================================================
 * Vertex-Transformation (GTE-RTPT-Äquivalent)
 * ========================================================================= */

/**
 * Transformiert einen 3D-Vertex mit Rotation + Translation.
 * Äquivalent zur PSX-GTE RTPT-Operation (RotTransPers ohne Perspektive).
 *
 * Berechnung:
 *   [ox]     [m[0][0] m[0][1] m[0][2]]   [vx]   [tx]
 *   [oy]  =  [m[1][0] m[1][1] m[1][2]] * [vy] + [ty]
 *   [oz]     [m[2][0] m[2][1] m[2][2]]   [vz]   [tz]
 *
 * Die Multiplikation erfolgt in Q12: (m * v) >> 12 + t
 *
 * @param rot    Rotationsmatrix (Q12)
 * @param trans  Translationsvektor (Q12)
 * @param vx     Eingabe-Vertex X
 * @param vy     Eingabe-Vertex Y
 * @param vz     Eingabe-Vertex Z
 * @param ox     Ausgabe X (nach Transformation)
 * @param oy     Ausgabe Y (nach Transformation)
 * @param oz     Ausgabe Z (nach Transformation)
 */
void re15_transform_vertex(const re15_rot_matrix_t* rot,
                           const re15_trans_matrix_t* trans,
                           int32_t vx, int32_t vy, int32_t vz,
                           int32_t* ox, int32_t* oy, int32_t* oz);

/**
 * Projiziert einen transformierten 3D-Punkt auf 2D-Bildschirmkoordinaten.
 * Äquivalent zur PSX-GTE Perspektiv-Division.
 *
 * Berechnung (mit h = Projektionsdistanz, typisch 320 für PSX):
 *   sx = (x * h) / z + screen_center_x
 *   sy = (y * h) / z + screen_center_y
 *
 * @param x3d      Transformierte X-Koordinate (Q12)
 * @param y3d      Transformierte Y-Koordinate (Q12)
 * @param z3d      Transformierte Z-Koordinate (Q12, muss > 0 sein)
 * @param h        Projektionsdistanz (typisch 320)
 * @param cx       Bildschirm-Mitte X (typisch 160 für 320×240)
 * @param cy       Bildschirm-Mitte Y (typisch 120 für 320×240)
 * @param sx_out   Ausgabe: Bildschirm-X
 * @param sy_out   Ausgabe: Bildschirm-Y
 * @return         0 bei Erfolg, -1 wenn z3d <= 0 (hinter Kamera)
 */
int re15_perspective_project(int32_t x3d, int32_t y3d, int32_t z3d,
                             int32_t h, int32_t cx, int32_t cy,
                             int32_t* sx_out, int32_t* sy_out);

#endif /* RE15_MATH_H */
