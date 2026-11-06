/* schwanz_r41.c — Feder-Kette fuer den Schwanz der Sichel-Form (Runde 41 Spur SF N1.9, re15_schwanz_r41.h).
 *
 * PORT-WAHL, keine Original-Konstante: die Form (EM30_HANGAR.EMD) existiert auf der Disc nicht. Alle Zahlen hier sind
 * Gestaltung, abgestimmt am 80%-Video (Video 4 Hangar: Schwanz schwingt h28 -> h38 in ~10 Bildern von Birkins rechter
 * Seite durch die Knie auf die linke; Dossier SF_sichel.md N1.9). */
#include "re15_schwanz_r41.h"
#include "re15_engine.h"     /* g_engine.frame_count: ein Kettenschritt je Spieltakt */
#include <math.h>
#include <string.h>

#define SW_K0   16            /* erster Schwanzknochen (Kette 16-22 an Knochen 8, tools/birkin_formen.py schwanz_kette) */
#define SW_N    7             /* Glieder */
#define SW_J    (SW_N + 1)    /* Gelenke: Wurzel (Knochen 16) .. Spitze */

/* Feder je Gelenk 1..7 zum Keyframe-Ziel (Anteil des Abstands je Takt): Wurzel steifer, Spitze weich -> Peitsche.
 * Mit k ~0,1 schwingt die Kette mit ~20 Takten Periode: eine freigegebene Seitenlage kreuzt die Mitte nach wenigen
 * Takten und liegt nach ~10 auf der anderen Seite (Video: Schwanz h28 links oben, h30-h34 zwischen den Knien, ab h38
 * Bild-rechts). Erster Versuch k 0,34..0,11 / 0,85: Seitenwechsel
 * in 3 Takten (Unit-Test), zu hart. Keine Schwerkraft: die Keyframes haengen den Schwanz schon (Ruhe = Keyframe). */
static const float k_feder[SW_J] = { 1.0f, 0.22f, 0.18f, 0.15f, 0.12f, 0.10f, 0.08f, 0.07f };
#define SW_ERHALT   0.80f     /* Geschwindigkeit, die je Takt erhalten bleibt (0,88: Lauf n8/n9 schwang die Spitze nach
                                 * der Freigabe bis Bild-x 260-276 statt ~205-214 wie das Video h38-h48 -> 0,80) */
#define SW_SPRUNG   3000.0f   /* Wurzel springt weiter (Teleport, Raumwechsel): Kette neu auf das Ziel */

/* Runde 41 SF NB3 (Abnahme 1 Mangel 12, Labor-Pult: Schwanz als langer waagerechter Dorn quer durchs Bild, Video n430-n465
 * kompakte Masse): ohne Szenenvorgabe haengt jedes Glied hoechstens SW_HANG_GRAD von der Senkrechten ab. GESTALTUNG. */
#define SW_HANG_COS 0.4695f   /* cos 62 Grad */
/* Runde 41 SF "Schwanz V2" (Formblatt V2 S6: Ruheverlauf als Haken - erst fast waagerecht nach hinten, dann senkrecht, dazu
 * die seitliche Welle der V2-Ruhelinie): ein Glied darf so waagerecht haengen wie sein Keyframe-Ziel (Ruhe + Nachschwingen
 * des Modells), hoechstens SW_HANG_COS_MIN von der Senkrechten. Mit der festen 62-Grad-Grenze drueckte haengen() die
 * V2-Glieder 4-5 (Keyframe 70-71 Grad, Gang Clip 1) in die Senkrechte, der Haken fiel zusammen. Der waagerechte Dorn am
 * Labor-Pult (NB3 Mangel 12, 90 Grad) bleibt ausgeschlossen. Referenz 2: Gang/Stand-Keyframes 10-68 Grad -> Glied 1 darf 65-68
 * statt 62 Grad, sonst gleich. GESTALTUNG, keine Original-Konstante. */
#define SW_HANG_COS_MIN 0.1736f   /* cos 80 Grad */
#define SW_HANG_RAND    0.10f     /* Spielraum zum Keyframe-Winkel (Kosinus; ~6-8 Grad bei 60-70 Grad). Mit 0,03 kippte die
                                   * Kette V2 im Gang (laengere Beine, mehr Seitenschwung) an der Grenze in die Senkrechte:
                                   * Spitze h108-h132 375-995 E. ueber dem Boden statt der Keyframes ~1000 (Lauf sfv2/V2s). */

/* Runde 41 SF "V3" (analysis/befunde_runde41/SF_MESSBLATT_V3.md, Riegel R25/R26 "auch in der Bewegung"): Glaettung der Kette
 * nach Feder, Haengen und Szenenziel. Jedes Gelenk darf hoechstens um seinen Keyframe-Gelenkwinkel + SW_GLATT_RAND abknicken,
 * mindestens SW_GLATT_MIN, hoechstens SW_GLATT_MAX (Bogenmass). Gemessen (Laeufe sfv3/r, riegel_v3.py --lauf, Gelenkwinkel im Raum
 * je Szenentakt): ohne Glaettung V3 im Gang h84-h132 bis 115 Grad, beim Halten h16-h70 bis 129 Grad (Feder-Nachlauf bzw. FABRIK
 * der Schwanzbahn), Referenz 2 bis 163 / 168 Grad; die Keyframes des V3-Bogens haben 8-24 Grad je Gelenk (kf_gelenkwinkel.py).
 * KI-Vorlage: hoechstens 28-33 Grad je 0,05 H im Bild, 2-4 Abschnitte ueber 20 Grad (Messblatt 4). Nur fuer Ketten mit der hohen
 * Wurzel der Varianten V1/V2/V3 (Bindversatz Gelenk 16 x < SW_GLATT_WURZEL_X wie BF_V2_WURZEL_X in birkin_formen.c); Referenz 2
 * (Spiel-Asset, x -380) bleibt unveraendert. GESTALTUNG (PORT-WAHL), keine Original-Konstante. */
#define SW_GLATT_WURZEL_X  (-700)
#define SW_GLATT_RAND      0.0524f    /* 3 Grad ueber dem Keyframe-Gelenkwinkel */
#define SW_GLATT_MIN       0.2094f    /* 12 Grad */
#define SW_GLATT_MAX       0.2793f    /* 16 Grad. Lauf V3 mit 22 Grad (Raum max 22,2): im Bild der Spielkamera bis 55 Grad je Gelenk,
                                       * 5 Gelenke > 20 Grad (Glieder schraeg zur Kamera verkuerzt, riegel_v3 R25b/R26b rot) */
/* Runde 41 SF "V4" (analysis/befunde_runde41/SF_MESSBLATT_V4.md Abschnitt 4 Punkt 2: die Biegung 70-90 Grad auf <= 0,10 H muss auf
 * ein Gelenk fallen, "SW_GLATT_MAX muss fuer die Biegung angepasst werden"). V4 Runde 2 (Pruefung V4 r1 Zahlen Abschnitt 3: im Lauf
 * sfv4/r/V4 lief glaetten() fuer V4 nie, weil der Bindversatz x -588 nicht unter SW_GLATT_WURZEL_X liegt; 73 von 144 Takten mit einem
 * Gelenk ueber 90 Grad, bis 160,9 Grad; ausserdem hob die Ausnahme bei V3 in 214 von 1358 Keyframes die 16-Grad-Grenze auf):
 * Die V4-Kette wird an ihrer Gliederteilung erkannt - die Gelenke kommen direkt aus den Bildmassen (birkin_formen.py kette_ruhe_v4,
 * keine Neuverteilung), die Glieder sind ungleich lang. Gemessen an den gebauten Formen (Gliedlaengen max/min der Knochen 17-22):
 * Referenz 2 1,00, V3 1,00, V2 1,15, V1 1,49, V4 2,4. Fuer die V4-Kette laeuft glaetten() immer, und nur dort darf ein Gelenk, dessen
 * Keyframe-Gelenkwinkel ueber SW_GLATT_KNICK_AB liegt (die gewollte Biegung), seinen Keyframe-Winkel + SW_GLATT_RAND behalten; alle
 * anderen Gelenke (und alle Gelenke von V1/V2/V3) behalten die Grenze SW_GLATT_MAX. Referenz 2 (Spiel-Asset) wird nicht geglaettet.
 * GESTALTUNG (PORT-WAHL), keine Original-Konstante. */
#define SW_GLATT_V4_VERH   2.0f       /* Gliedlaengen max/min ueber 2,0 = V4-Kette (V1 1,49 < 2,0 < V4 2,4) */
#define SW_GLATT_KNICK_AB  0.5236f    /* 30 Grad (V4: Keyframe-Gelenkwinkel der Biegung ~55-70 Grad, uebrige Gelenke <= 16 Grad) */

static struct {
    int      gueltig;
    uint32_t takt;
    float    p[SW_J][3], q[SW_J][3];          /* Lage jetzt / vorher (Welt) */
    int32_t  seite;                            /* Szenenvorgabe, Birkin-Raum (+ = rechte Seite = -z) */
    int32_t  ziel[3];                          /* NB3: Szenenziel der Spitze (Welt) */
    int32_t  ziel_w;                           /* NB3: Gewicht des Ziels Q12, 0 = aus */
    int32_t  via[3];                           /* NB4: Durchgangspunkt (Welt) fuer Gelenk via_j */
    int      via_j;                            /* NB4: 1..SW_J-2, 0 = kein Durchgangspunkt */
    float    beuge[3];                         /* NB4: Beugerichtung der Startform (Einheitsvektor, Welt) */
    int      beuge_an;                         /* NB4: 1 = Startform gebogen statt Federstand */
    int32_t  dicke;                            /* NB5: Querschnitt der Glieder Q12, 0 = 4096 */
    int      verdeckt;                         /* NB5: so viele Glieder ab der Wurzel nicht zeichnen (Querschnitt 0) */
} s_sw[RE15_ACTOR_MAX];

void re15_schwanz_r41_reset(void) { memset(s_sw, 0, sizeof s_sw); }

void re15_schwanz_r41_seite(int slot, int32_t seite)
{
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_sw[slot].seite = seite;
}

void re15_schwanz_r41_ziel(int slot, const int32_t spitze[3], int32_t gewicht_q12)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    if (!spitze || gewicht_q12 <= 0) { s_sw[slot].ziel_w = 0; return; }
    for (int k = 0; k < 3; k++) s_sw[slot].ziel[k] = spitze[k];
    s_sw[slot].ziel_w = gewicht_q12 > 4096 ? 4096 : gewicht_q12;
}

void re15_schwanz_r41_fuehrung(int slot, const int32_t via[3], int via_j, const int32_t beuge_q12[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    s_sw[slot].via_j = (via && via_j > 0 && via_j < SW_J - 1) ? via_j : 0;
    if (s_sw[slot].via_j) for (int k = 0; k < 3; k++) s_sw[slot].via[k] = via[k];
    s_sw[slot].beuge_an = 0;
    if (beuge_q12) {
        float l = 0.0f;
        for (int k = 0; k < 3; k++) { s_sw[slot].beuge[k] = (float)beuge_q12[k] / 4096.0f; l += s_sw[slot].beuge[k] * s_sw[slot].beuge[k]; }
        if (l > 1e-6f) { l = sqrtf(l); for (int k = 0; k < 3; k++) s_sw[slot].beuge[k] /= l; s_sw[slot].beuge_an = 1; }
    }
}

void re15_schwanz_r41_dicke(int slot, int32_t dicke_q12)
{
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_sw[slot].dicke = dicke_q12;
}

void re15_schwanz_r41_verdeckt(int slot, int glieder)
{
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_sw[slot].verdeckt = glieder < 0 ? 0 : glieder > SW_N ? SW_N : glieder;
}

int re15_schwanz_r41_gelenk(int slot, int j, int32_t out[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || j < 0 || j >= SW_J || !s_sw[slot].gueltig) return 0;
    for (int k = 0; k < 3; k++) out[k] = (int32_t)lrintf(s_sw[slot].p[j][k]);
    return 1;
}

static int kette_ok(const re15_emd_skeleton_t *s)
{
    if (s->bone_count != SW_K0 + SW_N || s->bone_parent[SW_K0] != 8) return 0;
    for (int b = SW_K0 + 1; b < SW_K0 + SW_N; b++)
        if (s->bone_parent[b] != b - 1) return 0;
    return 1;
}

/* Modell -> Welt wie re15_skel_bone_to_world (Gier um y, dann Aktorlage); Welt -> Modell umgekehrt. */
static void zu_welt(const re15_actor_t *a, float cs, float sn, const float m[3], float w[3])
{
    w[0] = (float)a->x + cs * m[0] + sn * m[2];
    w[1] = (float)a->y + m[1];
    w[2] = (float)a->z - sn * m[0] + cs * m[2];
}
static void zu_modell(const re15_actor_t *a, float cs, float sn, const float w[3], float m[3])
{
    const float dx = w[0] - (float)a->x, dz = w[2] - (float)a->z;
    m[0] = cs * dx - sn * dz;
    m[1] = w[1] - (float)a->y;
    m[2] = sn * dx + cs * dz;
}

static float norm(float v[3]);
static void mat_aus(const int32_t r[9], float f[9]) { for (int i = 0; i < 9; i++) f[i] = (float)r[i] / 4096.0f; }
/* Die Posenmatrizen des Ports sind nicht ganz normiert (Q12-Kette wie RotMatrix/MulMatrix: Spaltenlaengen 0,99..0,94
 * entlang der Kette, gemessen test_schwanz_r41). Fuer die Drehrechnung orthonormieren (Gram-Schmidt der Spalten),
 * den Massstab merken und am Ende wieder anbringen, damit Glieder und Netze so lang bleiben wie im Keyframe. */
static float ortho(float m[9])
{
    float c[3][3];
    for (int k = 0; k < 3; k++) for (int r = 0; r < 3; r++) c[k][r] = m[r * 3 + k];
    const float s = (sqrtf(c[0][0]*c[0][0] + c[0][1]*c[0][1] + c[0][2]*c[0][2]) +
                     sqrtf(c[1][0]*c[1][0] + c[1][1]*c[1][1] + c[1][2]*c[1][2]) +
                     sqrtf(c[2][0]*c[2][0] + c[2][1]*c[2][1] + c[2][2]*c[2][2])) / 3.0f;
    norm(c[1]);                                            /* Gliedachse (lokal y) zuerst */
    float d = c[0][0]*c[1][0] + c[0][1]*c[1][1] + c[0][2]*c[1][2];
    for (int r = 0; r < 3; r++) c[0][r] -= d * c[1][r];
    norm(c[0]);
    c[2][0] = c[0][1]*c[1][2] - c[0][2]*c[1][1];
    c[2][1] = c[0][2]*c[1][0] - c[0][0]*c[1][2];
    c[2][2] = c[0][0]*c[1][1] - c[0][1]*c[1][0];
    for (int k = 0; k < 3; k++) for (int r = 0; r < 3; r++) m[r * 3 + k] = c[k][r];
    return s;
}
static void mul(const float a[9], const float b[9], float o[9])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            o[r * 3 + c] = a[r * 3] * b[c] + a[r * 3 + 1] * b[3 + c] + a[r * 3 + 2] * b[6 + c];
}
static void mul_tn(const float a[9], const float b[9], float o[9])   /* a^T * b */
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            o[r * 3 + c] = a[r] * b[c] + a[3 + r] * b[3 + c] + a[6 + r] * b[6 + c];
}
static void mul_v(const float a[9], const float v[3], float o[3])
{
    for (int r = 0; r < 3; r++) o[r] = a[r * 3] * v[0] + a[r * 3 + 1] * v[1] + a[r * 3 + 2] * v[2];
}
static float norm(float v[3])
{
    const float l = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (l > 1e-6f) { v[0] /= l; v[1] /= l; v[2] /= l; }
    return l;
}
/* Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues). */
static void min_dreh(const float a[3], const float b[3], float R[9])
{
    float v[3] = { a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0] };
    const float c = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    static const float I[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
    if (c < -0.9999f) { memcpy(R, I, sizeof I); return; }   /* entgegengesetzt: nicht drehen (kommt durch die Feder nicht vor) */
    const float k = 1.0f / (1.0f + c);
    const float K[9] = { 0, -v[2], v[1], v[2], 0, -v[0], -v[1], v[0], 0 };
    float KK[9];
    mul(K, K, KK);
    for (int i = 0; i < 9; i++) R[i] = I[i] + K[i] + KK[i] * k;
}

/* NB3 Mangel 12: kein Glied steiler als SW_HANG_COS zur Senkrechten (y+ = unten); Laengen bleiben.
 * Schwanz V2: Grenze je Glied = min(SW_HANG_COS, max(Keyframe-Kosinus - SW_HANG_RAND, SW_HANG_COS_MIN)), zw = Keyframe-Gelenke. */
static void haengen(float (*p)[3], const float *laenge, float boden, float (*zw)[3])
{
    for (int j = 1; j < SW_J; j++) {
        float d[3] = { p[j][0] - p[j - 1][0], p[j][1] - p[j - 1][1], p[j][2] - p[j - 1][2] };
        float kf[3] = { zw[j][0] - zw[j - 1][0], zw[j][1] - zw[j - 1][1], zw[j][2] - zw[j - 1][2] };
        if (norm(d) < 1e-3f) continue;
        float lim = SW_HANG_COS;
        if (norm(kf) > 1e-3f) {
            const float k = kf[1] - SW_HANG_RAND;
            lim = k < SW_HANG_COS ? (k > SW_HANG_COS_MIN ? k : SW_HANG_COS_MIN) : SW_HANG_COS;
        }
        if (d[1] < lim) {
            float h = sqrtf(d[0] * d[0] + d[2] * d[2]);
            const float s = sqrtf(1.0f - lim * lim);
            if (h > 1e-4f) { d[0] *= s / h; d[2] *= s / h; } else { d[0] = s; d[2] = 0.0f; }
            d[1] = lim;
        }
        for (int k = 0; k < 3; k++) p[j][k] = p[j - 1][k] + d[k] * laenge[j];
        if (p[j][1] > boden) p[j][1] = boden;
    }
}

/* Runde 41 SF V3: Knick-Grenze je Gelenk (siehe SW_GLATT_*). Von der Wurzel zur Spitze: ist das Glied j -> j+1 staerker gegen
 * das Vorglied abgeknickt als erlaubt, wird es in der Ebene beider Glieder auf die Grenze zurueckgedreht; Laenge bleibt, Boden.
 * Die alte Lage q wird um dieselbe Verschiebung mitgezogen (keine Scheingeschwindigkeit fuer die Feder im naechsten Takt). */
static void glaetten(float (*p)[3], float (*q)[3], const float *laenge, float boden, float (*zw)[3], int biegung)
{
    for (int j = 1; j < SW_J - 1; j++) {
        float a[3] = { p[j][0] - p[j - 1][0], p[j][1] - p[j - 1][1], p[j][2] - p[j - 1][2] };
        float d[3] = { p[j + 1][0] - p[j][0], p[j + 1][1] - p[j][1], p[j + 1][2] - p[j][2] };
        float ka[3] = { zw[j][0] - zw[j - 1][0], zw[j][1] - zw[j - 1][1], zw[j][2] - zw[j - 1][2] };
        float kd[3] = { zw[j + 1][0] - zw[j][0], zw[j + 1][1] - zw[j][1], zw[j + 1][2] - zw[j][2] };
        if (norm(a) < 1e-3f || norm(d) < 1e-3f) continue;
        float lim = SW_GLATT_MIN, kw = 0.0f;
        if (norm(ka) > 1e-3f && norm(kd) > 1e-3f) {
            float c = ka[0] * kd[0] + ka[1] * kd[1] + ka[2] * kd[2];
            c = c > 1.0f ? 1.0f : (c < -1.0f ? -1.0f : c);
            kw = acosf(c);
            lim = kw + SW_GLATT_RAND;
        }
        if (lim < SW_GLATT_MIN) lim = SW_GLATT_MIN;
        if (lim > SW_GLATT_MAX && !(biegung && kw > SW_GLATT_KNICK_AB)) lim = SW_GLATT_MAX;   /* nur V4: Biegung behaelt ihren Winkel */
        const float c = a[0] * d[0] + a[1] * d[1] + a[2] * d[2];
        if (c < cosf(lim)) {
            float s[3] = { d[0] - a[0] * c, d[1] - a[1] * c, d[2] - a[2] * c };
            if (norm(s) < 1e-6f) { s[0] = -a[1]; s[1] = a[0]; s[2] = 0.0f; if (norm(s) < 1e-6f) { s[0] = 1.0f; s[1] = 0.0f; s[2] = 0.0f; } }
            for (int k = 0; k < 3; k++) d[k] = a[k] * cosf(lim) + s[k] * sinf(lim);
        }
        float alt[3] = { p[j + 1][0], p[j + 1][1], p[j + 1][2] };
        for (int k = 0; k < 3; k++) p[j + 1][k] = p[j][k] + d[k] * laenge[j + 1];
        if (p[j + 1][1] > boden) {                       /* Boden: auf Bodenhoehe ziehen, aber nur ohne neuen Knick (Lauf V3 mit
                                                          * hartem Boden: Spitze h33-h35 / h69-h71 bis 72 Grad abgeknickt) */
            float e[3] = { p[j + 1][0] - p[j][0], boden - p[j][1], p[j + 1][2] - p[j][2] };
            if (norm(e) > 1e-3f && a[0] * e[0] + a[1] * e[1] + a[2] * e[2] >= cosf(lim))
                for (int k = 0; k < 3; k++) p[j + 1][k] = p[j][k] + e[k] * laenge[j + 1];
        }
        for (int k = 0; k < 3; k++) q[j + 1][k] += p[j + 1][k] - alt[k];
    }
    /* Runde 41 SF V3 Runde 2 (Zahlen-Pruefer V3 r1 Abschnitt 6.1: Lauf sfv3/r/V3, Halten h32-h36, h60, h65-h71, h84: Gelenke 6/7 bis
     * 612 E. unter dem Boden, weil die Bodenregel oben keinen neuen Knick zulaesst; Referenz 2 / V1 dort >= 100 E. darueber): liegt
     * danach noch ein Gelenk unter dem Boden, die ganze Kette starr um das Gelenk 0 hochdrehen - alle Gelenkwinkel bleiben, es
     * entsteht kein Knick. Achse waagerecht quer zur Richtung Gelenk 0 -> tiefstes Gelenk, Schritt 1 Grad (Aufloesung der Messung
     * Gelenkwinkel), hoechstens 90 Grad (dann liegt die Strecke Gelenk 0 -> tiefstes Gelenk waagerecht oder hoeher). */
    int tief = -1;
    float ymax = boden;
    for (int j = 1; j < SW_J; j++)
        if (p[j][1] > ymax) { ymax = p[j][1]; tief = j; }
    if (tief > 0) {
        float v[3] = { p[tief][0] - p[0][0], 0.0f, p[tief][2] - p[0][2] };
        float ax[3] = { -v[2], 0.0f, v[0] };                /* waagerecht, quer zu v */
        if (norm(ax) < 1e-3f) { ax[0] = 1.0f; ax[1] = 0.0f; ax[2] = 0.0f; }
        float r0[SW_J][3];
        for (int j = 0; j < SW_J; j++)
            for (int k = 0; k < 3; k++) r0[j][k] = p[j][k] - p[0][k];
        const float schritt = 0.0174533f;                   /* 1 Grad */
        float best = 0.0f;
        for (int sg = -1; sg <= 1 && best == 0.0f; sg += 2) {
            for (int n = 1; n <= 90; n++) {
                const float w = sg * n * schritt, c = cosf(w), sn = sinf(w);
                int ok = 1;
                for (int j = 1; j < SW_J && ok; j++) {
                    const float *r = r0[j];
                    const float kd = ax[0] * r[0] + ax[1] * r[1] + ax[2] * r[2];
                    const float y = r[1] * c + (ax[2] * r[0] - ax[0] * r[2]) * sn + ax[1] * kd * (1.0f - c);
                    if (p[0][1] + y > boden) ok = 0;
                }
                if (ok) { best = w; break; }
            }
        }
        if (best != 0.0f) {
            const float c = cosf(best), sn = sinf(best);
            for (int j = 1; j < SW_J; j++) {
                const float *r = r0[j];
                const float kd = ax[0] * r[0] + ax[1] * r[1] + ax[2] * r[2];
                const float kr[3] = { ax[1] * r[2] - ax[2] * r[1], ax[2] * r[0] - ax[0] * r[2], ax[0] * r[1] - ax[1] * r[0] };
                float neu[3];
                for (int k = 0; k < 3; k++) neu[k] = p[0][k] + r[k] * c + kr[k] * sn + ax[k] * kd * (1.0f - c);
                for (int k = 0; k < 3; k++) { q[j][k] += neu[k] - p[j][k]; p[j][k] = neu[k]; }
            }
        }
    }
}

/* FABRIK auf den Gelenken a..e (a fest, e auf das Ziel t), feste Gliedlaengen, Boden. NB4 (Abnahme 2 A4: h18 Schlinge,
 * h60-h72 J-Bogen statt gerader Diagonale): mit `beuge` beginnt die Rechnung nicht am Federstand, sondern an einer Startform,
 * die von der Geraden a -> t in Richtung `beuge` ausbaucht (Sinusbogen, Hoehe aus Kettenlaenge und Abstand). FABRIK behaelt
 * die Seite der Startform bei; liegt `beuge` in der Blickrichtung der Kamera, verschwindet die ueberschuessige Laenge in der
 * Tiefe und das Bild zeigt eine gerade Linie von a nach t (GESTALTUNG nach Video 4 h18-h22 / h60-h70). */
static void fabrik(float (*ik)[3], int a, int e, const float t[3], const float *laenge, float boden, const float *beuge)
{
    if (beuge) {
        float D[3] = { t[0] - ik[a][0], t[1] - ik[a][1], t[2] - ik[a][2] }, L = 0.0f, acc = 0.0f;
        const float dl = sqrtf(D[0] * D[0] + D[1] * D[1] + D[2] * D[2]);
        for (int j = a + 1; j <= e; j++) L += laenge[j];
        const float hg = (dl < L) ? 0.5f * sqrtf(L * L - dl * dl) : 0.0f;
        for (int j = a + 1; j <= e; j++) {
            acc += laenge[j];
            const float s = (L > 1e-3f) ? acc / L : 1.0f, b = hg * sinf(3.14159265f * s);
            for (int k = 0; k < 3; k++) ik[j][k] = ik[a][k] + D[k] * s + beuge[k] * b;
        }
    }
    const float a0[3] = { ik[a][0], ik[a][1], ik[a][2] };
    for (int it = 0; it < 16; it++) {
        for (int k = 0; k < 3; k++) ik[e][k] = t[k];
        for (int j = e - 1; j > a; j--) {
            float d[3] = { ik[j][0] - ik[j + 1][0], ik[j][1] - ik[j + 1][1], ik[j][2] - ik[j + 1][2] };
            if (norm(d) < 1e-3f) { d[0] = 0; d[1] = -1; d[2] = 0; }
            for (int k = 0; k < 3; k++) ik[j][k] = ik[j + 1][k] + d[k] * laenge[j + 1];
        }
        for (int k = 0; k < 3; k++) ik[a][k] = a0[k];
        for (int j = a + 1; j <= e; j++) {
            float d[3] = { ik[j][0] - ik[j - 1][0], ik[j][1] - ik[j - 1][1], ik[j][2] - ik[j - 1][2] };
            if (norm(d) < 1e-3f) { d[0] = 0; d[1] = 1; d[2] = 0; }
            for (int k = 0; k < 3; k++) ik[j][k] = ik[j - 1][k] + d[k] * laenge[j];
            if (ik[j][1] > boden) ik[j][1] = boden;
        }
    }
}

/* NB3 Mangel 3 ("Schwanzbahn direkt aus der Videotabelle nachfahren"): Szenenziel der Spitze per FABRIK (Wurzel fest,
 * feste Gliedlaengen, Boden), Startform = Kette des Federschritts; Ergebnis mit dem Gewicht ueberblendet, die Geschwindigkeit
 * der Feder wird mitgeschoben (beim Freigeben laeuft sie ohne Sprung weiter).
 * NB4: optional ein Durchgangspunkt fuer Gelenk via_j (zwei FABRIK-Abschnitte Wurzel -> via, via -> Spitze) und eine
 * gebogene Startform (re15_schwanz_r41_fuehrung). */
static void ziel_anwenden(int slot, const float *laenge, float boden)
{
    float (*p)[3] = s_sw[slot].p, (*q)[3] = s_sw[slot].q;
    float ik[SW_J][3], t[3];
    const float *bg = s_sw[slot].beuge_an ? s_sw[slot].beuge : NULL;
    memcpy(ik, p, sizeof ik);
    for (int k = 0; k < 3; k++) t[k] = (float)s_sw[slot].ziel[k];
    if (s_sw[slot].via_j > 0) {
        float v[3];
        for (int k = 0; k < 3; k++) v[k] = (float)s_sw[slot].via[k];
        fabrik(ik, 0, s_sw[slot].via_j, v, laenge, boden, bg);
        fabrik(ik, s_sw[slot].via_j, SW_J - 1, t, laenge, boden, bg);
    } else fabrik(ik, 0, SW_J - 1, t, laenge, boden, bg);
    const float w = (float)s_sw[slot].ziel_w / 4096.0f;
    for (int j = 1; j < SW_J; j++)
        for (int k = 0; k < 3; k++) {
            const float dlt = (ik[j][k] - p[j][k]) * w;
            p[j][k] += dlt; q[j][k] += dlt;
        }
    for (int j = 1; j < SW_J; j++) {                                          /* Laengen nach der Ueberblendung */
        float d[3] = { p[j][0] - p[j - 1][0], p[j][1] - p[j - 1][1], p[j][2] - p[j - 1][2] };
        if (norm(d) < 1e-3f) continue;
        for (int k = 0; k < 3; k++) p[j][k] = p[j - 1][k] + d[k] * laenge[j];
    }
}

int re15_schwanz_r41_pose(const re15_actor_t *a, const re15_emd_skeleton_t *skel, re15_skel_pose_t *poses)
{
    if (!a || !skel || !poses || a->type != 0x30u || !kette_ok(skel)) return 0;
    const int slot = (int)(a - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const float cs = (float)re15_cos_q12(a->rot_y) / 4096.0f, sn = (float)re15_sin_q12(a->rot_y) / 4096.0f;

    /* Ziel: Keyframe-Gelenke (Modell), Spitze = Knochen 22 + Achse * Laenge des Vorglieds (gleichlange Glieder). */
    float zm[SW_J][3], zw[SW_J][3], laenge[SW_J];
    for (int j = 0; j < SW_N; j++)
        for (int k = 0; k < 3; k++) zm[j][k] = (float)poses[SW_K0 + j].trans[k];
    {
        const int16_t *r = skel->bone_relative_pos[SW_K0 + SW_N - 1];
        const float lt = sqrtf((float)r[0] * r[0] + (float)r[1] * r[1] + (float)r[2] * r[2]);
        const int32_t *R = poses[SW_K0 + SW_N - 1].rot;
        for (int k = 0; k < 3; k++) zm[SW_N][k] = zm[SW_N - 1][k] + (float)R[k * 3 + 1] / 4096.0f * lt;
    }
    /* Gliedlaengen aus den UNVERSETZTEN Keyframe-Gelenken. Runde 41 SF NB3 (Abnahme 1 Mangel 3): vorher wurden sie nach
     * der Seitenvorgabe gemessen - mit seite 3300 waren die simulierten Glieder 429..973 statt 424 lang (Summe ~4640 statt
     * 2968), gezeichnet blieben 424 -> der Schwanz im Bild endete h20-h24 30-47 Bildpunkte vor der "Schwanzspitze" der
     * Messzeile. */
    laenge[0] = 0.0f;
    for (int j = 1; j < SW_J; j++) {
        float d[3] = { zm[j][0] - zm[j - 1][0], zm[j][1] - zm[j - 1][1], zm[j][2] - zm[j - 1][2] };
        laenge[j] = norm(d);
    }
    /* Seitenvorgabe quadratisch zur Spitze (oberes Stueck haengt weiter, die Spitze schwingt aus): linear gewichtet
     * stand der Schwanz im Lauf n4 h22-h26 waagerecht in Hufthoehe ab, das Video zeigt ihn von der Huefte bis zum Boden. */
    for (int j = 1; j < SW_J; j++)
        zm[j][2] -= (float)s_sw[slot].seite * (float)(j * j) / (float)(SW_N * SW_N);   /* rechte Seite = Birkin-Raum -z */
    for (int j = 0; j < SW_J; j++) zu_welt(a, cs, sn, zm[j], zw[j]);

    /* Ein Kettenschritt je Spieltakt. */
    if (!s_sw[slot].gueltig || s_sw[slot].takt != g_engine.frame_count) {
        float sp[3] = { s_sw[slot].p[0][0] - zw[0][0], s_sw[slot].p[0][1] - zw[0][1], s_sw[slot].p[0][2] - zw[0][2] };
        if (!s_sw[slot].gueltig || norm(sp) > SW_SPRUNG) {
            memcpy(s_sw[slot].p, zw, sizeof zw);
            memcpy(s_sw[slot].q, zw, sizeof zw);
            s_sw[slot].gueltig = 1;
        } else {
            float (*p)[3] = s_sw[slot].p, (*q)[3] = s_sw[slot].q;
            for (int k = 0; k < 3; k++) { q[0][k] = p[0][k]; p[0][k] = zw[0][k]; }     /* Wurzel haengt am Rumpf */
            for (int j = 1; j < SW_J; j++)
                for (int k = 0; k < 3; k++) {
                    const float v = (p[j][k] - q[j][k]) * SW_ERHALT;
                    q[j][k] = p[j][k];
                    p[j][k] += v + (zw[j][k] - p[j][k]) * k_feder[j];
                }
            for (int j = 1; j < SW_J; j++) {                                  /* feste Gliedlaengen, Boden */
                float d[3] = { p[j][0] - p[j - 1][0], p[j][1] - p[j - 1][1], p[j][2] - p[j - 1][2] };
                if (norm(d) < 1e-3f) { d[0] = zw[j][0] - zw[j - 1][0]; d[1] = zw[j][1] - zw[j - 1][1];
                                      d[2] = zw[j][2] - zw[j - 1][2]; norm(d); }
                for (int k = 0; k < 3; k++) p[j][k] = p[j - 1][k] + d[k] * laenge[j];
                if (p[j][1] > (float)a->y) p[j][1] = (float)a->y;            /* nicht unter den Boden (y+ = unten) */
            }
            if (s_sw[slot].ziel_w <= 0) haengen(p, laenge, (float)a->y, zw);
        }
        if (s_sw[slot].ziel_w > 0) ziel_anwenden(slot, laenge, (float)a->y);
        {   /* Runde 41 SF V3: Knick-Grenze (hohe Wurzel V1/V2/V3); V4 Runde 2: auch fuer die V4-Kette (ungleiche Glieder) */
            float lmin = 1e9f, lmax = 0.0f;
            for (int j = 1; j < SW_J; j++) { if (laenge[j] < lmin) lmin = laenge[j]; if (laenge[j] > lmax) lmax = laenge[j]; }
            const int v4 = lmin > 1.0f && lmax > SW_GLATT_V4_VERH * lmin;
            if (skel->bone_relative_pos[SW_K0][0] < SW_GLATT_WURZEL_X || v4)
                glaetten(s_sw[slot].p, s_sw[slot].q, laenge, (float)a->y, zw, v4);
        }
        s_sw[slot].takt = g_engine.frame_count;
    }

    /* Knochen 16-22 neu drehen: Keyframe-Lokaldrehung behalten, dann kleinste Drehung auf das simulierte Glied. */
    float alt[SW_N + 1][9], neu_p[9], t_p[3], s_p;
    mat_aus(poses[8].rot, alt[0]);
    s_p = ortho(alt[0]);
    memcpy(neu_p, alt[0], sizeof neu_p);
    for (int k = 0; k < 3; k++) t_p[k] = (float)poses[8].trans[k];
    for (int j = 0; j < SW_N; j++) {
        const int b = SW_K0 + j;
        float Rb[9], L[9], Rc[9], Rm[9], Rn[9], tb[3], ax[3], ds[3], ziel_m[3], s_b;
        mat_aus(poses[b].rot, Rb);
        s_b = ortho(Rb);
        mul_tn(alt[j], Rb, L);                                /* lokal = Eltern^T * Knochen (Keyframe, normiert) */
        memcpy(alt[j + 1], Rb, sizeof Rb);
        mul(neu_p, L, Rc);
        {   const float rel[3] = { (float)skel->bone_relative_pos[b][0], (float)skel->bone_relative_pos[b][1],
                                   (float)skel->bone_relative_pos[b][2] };
            mul_v(neu_p, rel, tb);                            /* wie re15_skel_compute_pose: Elternmatrix (mit Massstab) * rel */
            for (int k = 0; k < 3; k++) tb[k] = tb[k] * s_p + t_p[k]; }
        ax[0] = Rc[1]; ax[1] = Rc[4]; ax[2] = Rc[7];          /* Gliedachse = lokale y */
        norm(ax);
        zu_modell(a, cs, sn, s_sw[slot].p[j + 1], ziel_m);
        for (int k = 0; k < 3; k++) ds[k] = ziel_m[k] - tb[k];
        if (norm(ds) > 1e-3f) { min_dreh(ax, ds, Rm); mul(Rm, Rc, Rn); }
        else memcpy(Rn, Rc, sizeof Rn);
        {   /* NB5 (Abnahme 3 B1): Querschnitt = lokale x- und z-Spalte, die Gliedachse (y) und damit die Kette bleiben */
            const float dk = (j < s_sw[slot].verdeckt) ? 0.0f : (s_sw[slot].dicke > 0) ? (float)s_sw[slot].dicke / 4096.0f : 1.0f;
            for (int r = 0; r < 3; r++) {
                poses[b].rot[r * 3 + 0] = (int32_t)lrintf(Rn[r * 3 + 0] * s_b * dk * 4096.0f);
                poses[b].rot[r * 3 + 1] = (int32_t)lrintf(Rn[r * 3 + 1] * s_b * 4096.0f);
                poses[b].rot[r * 3 + 2] = (int32_t)lrintf(Rn[r * 3 + 2] * s_b * dk * 4096.0f);
            } }
        for (int k = 0; k < 3; k++) poses[b].trans[k] = (int32_t)lrintf(tb[k]);
        memcpy(neu_p, Rn, sizeof neu_p);
        memcpy(t_p, tb, sizeof t_p);
        s_p = s_b;
    }
    return 1;
}
