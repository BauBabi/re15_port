/* form_j_posen.h - ERZEUGT von analysis/befunde_runde41/form/stufe3/gen/s3_testkopf.py, nicht von Hand aendern.
 * Referenz Formtreue seit dem R9-Einbau (2026-11-15): Riesenform K5g3 + rechte Hand Variante 1 (varianten/K5/k5g/K5g3 = Spiel-Asset,
 * Generator varianten/K5/generator/kf_k5g3.py; EINBAU_R9.md). Vorher E32 K5e_f1 (einbau_k5e_f1), E30 K5c, davor K4.
 * MD1/TIM aus K5g3; Bild 1 von K5c, Bild 2 Winkel aus dem Asset (Key 542, Posen-Datei R9), Ruhe = Ruhe von J mit Kiefer zu.
 * Makronamen FORM_J_* wie bisher. Kein RE-Wert. */
#ifndef FORM_J_POSEN_H
#define FORM_J_POSEN_H
#include <stdint.h>

#define FORM_J_MD1_LEN 462572u
#define FORM_J_MD1_CRC 0x565363c6u
#define FORM_J_TIM_LEN 138784u
#define FORM_J_TIM_CRC 0xf428fe01u

static const int16_t FORM_J_BILD1_WURZEL[3] = { -9, -1887, -22 };
static const int16_t FORM_J_BILD1_WINKEL[32][3] = {
    { 60, -2, 8 }, { -18, -2, -241 }, { -50, -111, -225 }, { -14, -80, -139 },
    { -22, -56, -10 }, { -39, -63, 133 }, { -53, 78, 303 }, { 3, 1, 117 },
    { -51, -238, 482 }, { -344, 12, 490 }, { -21, -73, -365 }, { -222, -203, 540 },
    { 23, -1, 41 }, { 137, -238, -271 }, { 35, -325, 0 }, { -34, 117, 0 },
    { -158, -148, 965 }, { 152, -153, -526 }, { -27, 102, -172 }, { 34, -93, 84 },
    { -36, -344, 65 }, { 17, 326, 379 }, { 141, -154, 143 }, { 0, 0, 0 },
    { 866, 4, -592 }, { -456, -164, -136 }, { -565, 349, 428 }, { 392, -634, -683 },
    { -37, 366, 121 }, { 45, -391, 202 }, { 43, -415, -39 }, { -24, 302, 80 },
};
static const int16_t FORM_J_BILD2_WURZEL[3] = { 112, -1111, -148 };
static const int16_t FORM_J_BILD2_WINKEL[32][3] = {
    { -91, 1, -587 }, { 22, 211, 22 }, { 8, 129, 73 }, { -12, 66, 121 },
    { -10, 50, 146 }, { -4, 37, 150 }, { -3, 24, 120 }, { 136, -66, 109 },
    { -232, -510, -773 }, { 355, 442, -603 }, { -181, 356, 349 }, { 9, -196, 310 },
    { 263, -64, -49 }, { -171, 218, -52 }, { 33, -337, 104 }, { 27, -24, 128 },
    { -656, 797, 1258 }, { -412, -191, -693 }, { -13, 103, -47 }, { 58, -82, -218 },
    { 52, -190, 161 }, { -518, 1241, 271 }, { 103, -705, 773 }, { 0, -60, 200 },
    { -55, -176, 511 }, { 1077, -359, 296 }, { -139, 198, 1323 }, { 548, 782, -1243 },
    { 54, 416, -174 }, { 25, -358, 308 }, { 50, -394, 64 }, { -35, 252, -97 },
};
static const int16_t FORM_J_RUHE_WURZEL[3] = { 0, -1828, 0 };
static const int16_t FORM_J_RUHE_WINKEL[32][3] = {
    { 0, 0, -104 }, { 0, 0, -186 }, { 0, 0, -137 }, { 0, 0, -97 },
    { 0, 0, -23 }, { 0, 0, 85 }, { 0, 0, 112 }, { 0, 0, -29 },
    { 185, 0, -133 }, { -185, 0, -133 }, { 0, 0, -3 }, { 317, 0, 240 },
    { -124, 0, -312 }, { -216, 0, 263 }, { 0, 0, 85 }, { 0, 0, 47 },
    { -317, 0, 198 }, { 124, 0, -363 }, { 216, 0, 325 }, { 0, 0, 105 },
    { 0, 0, 84 }, { 0, 0, -114 }, { 0, 0, 91 }, { 0, 0, 0 },
    { -75, 0, 1135 }, { 1117, 107, -1153 }, { -868, 0, 1135 }, { 785, -713, -5 },
    { 0, 0, 146 }, { 0, 0, 183 }, { 0, 0, 126 }, { 0, 0, 102 },
};

/* Hoehe der Riesenform im Gehen (Clip 5 des Spiel-Assets, Mittel ueber jedes 4. Bild), Formtreue-Pin. */
#define FORM_HOEHE_GEHEN 3126

#endif
