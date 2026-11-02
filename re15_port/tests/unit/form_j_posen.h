/* form_j_posen.h - ERZEUGT von analysis/befunde_runde41/form/stufe3/gen/s3_testkopf.py, nicht von Hand aendern.
 * Referenz Formtreue: Variante J mit Kopf K3 (stufe3/basis_n1 = K3_fit mit Gelenkkoerper-UV neu, Nutzer-Entscheide E19-E21): MD1/TIM,
 * Posen Bild 1/Bild 2 von K3, Ruhe = Ruhe von J mit Kiefer zu. Makronamen FORM_J_* wie bisher. Kein RE-Wert. */
#ifndef FORM_J_POSEN_H
#define FORM_J_POSEN_H
#include <stdint.h>

#define FORM_J_MD1_LEN 161228u
#define FORM_J_MD1_CRC 0xe8df6d49u
#define FORM_J_TIM_LEN 133152u
#define FORM_J_TIM_CRC 0x792bced3u

static const int16_t FORM_J_BILD1_WURZEL[3] = { -9, -1887, -22 };
static const int16_t FORM_J_BILD1_WINKEL[32][3] = {
    { 60, -2, 8 }, { -18, -2, -241 }, { -50, -111, -225 }, { -14, -80, -139 },
    { -22, -56, -10 }, { -39, -63, 133 }, { -53, 78, 303 }, { 3, 1, 117 },
    { -51, -238, 482 }, { -344, 12, 490 }, { -21, -73, -365 }, { -222, -203, 540 },
    { 23, -1, 41 }, { 137, -238, -271 }, { 35, -325, 0 }, { -34, 117, 0 },
    { -158, -148, 965 }, { 152, -153, -526 }, { -27, 102, -172 }, { 34, -93, 84 },
    { -36, -344, 65 }, { 37, 314, 432 }, { 141, -141, 171 }, { 0, 0, 0 },
    { 632, 5, -520 }, { -718, -650, 507 }, { -365, 295, 403 }, { -250, -748, -325 },
    { -37, 366, 121 }, { 45, -391, 202 }, { 43, -415, -39 }, { -24, 302, 80 },
};
static const int16_t FORM_J_BILD2_WURZEL[3] = { 112, -1111, -148 };
static const int16_t FORM_J_BILD2_WINKEL[32][3] = {
    { -91, 1, -587 }, { 22, 211, 22 }, { 8, 129, 73 }, { -12, 66, 121 },
    { -10, 50, 146 }, { -4, 37, 150 }, { -3, 24, 120 }, { 35, -153, -14 },
    { -232, -510, -773 }, { 355, 442, -603 }, { 24, 309, 451 }, { -120, -51, 276 },
    { 187, -29, -287 }, { -78, 177, 166 }, { 33, -337, 104 }, { 27, -24, 128 },
    { 419, 1160, -162 }, { -136, 114, -679 }, { 283, -172, 379 }, { -35, -202, -99 },
    { 34, -155, -336 }, { -471, 935, 1116 }, { 128, 54, -561 }, { 0, 40, 279 },
    { -55, -176, 511 }, { 1077, -359, 296 }, { -139, 198, 1323 }, { 548, 782, -1243 },
    { 54, 416, -174 }, { -32, 399, -380 }, { 50, -394, 64 }, { 35, 45, -43 },
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
