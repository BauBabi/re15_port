/*
 * panel2030_r41.c — Runde 41 Spur N "nachtrag30": Panel-Raetsel ROOM2030/2031 (AUFTRAG.md Nachtrag 37, Entwurf A "Druckspeicher").
 * Regeln, Vorgaben und Belege: include/re15_panel2030_r41.h, Dossier analysis/befunde_runde41/N_nachtrag30.md §2.
 */
#include "re15_panel2030_r41.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "re15_audio.h"          /* re15_audio_re2_panel_se, RE15_PANEL_SE_* */
#include "re15_ventilkette_r41.h"

extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);

#ifdef RE15_PLATFORM_PC
#include "gen/panel2030_r41.inc"
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

/* Virtuelle Pad-Bits (re15_pad_virtual_word), wie ROOM2050 sub01 @0x02182.. und re15_hebetisch_cursor.h: 1 hoch, 2 rechts, 4 runter, 8 links,
 * 0x40 SQUARE, 0x8000 CROSS. */
#define RE15_HC_ZUG_HOCH    0x0001u
#define RE15_HC_ZUG_RECHTS  0x0002u
#define RE15_HC_ZUG_RUNTER  0x0004u
#define RE15_HC_ZUG_LINKS   0x0008u
#define RE15_HC_ZUG_DRUCK   0x0040u
#define RE15_HC_ZUG_ABBRUCH 0x8000u

static const int k_kap[RE15_R41N_TANKS]   = RE15_R41N_KAPAZITAET;
static const int k_start[RE15_R41N_TANKS] = RE15_R41N_START;
static const int k_x0[RE15_R41N_TANKS]    = RE15_R41N_TANK_X0;
static const int k_x1[RE15_R41N_TANKS]    = RE15_R41N_TANK_X1;
static const int k_exit[4]                = { RE15_R41N_EXIT_RECT };

static int s_offen = 0, s_stand[RE15_R41N_TANKS], s_von[RE15_R41N_TANKS], s_fahrt = 0, s_quelle = -1;
static int s_cx_q8 = RE15_R41N_CURSOR_X0 << 8, s_cy_q8 = RE15_R41N_CURSOR_Y0 << 8;
static int s_ruhe = 0, s_ziel = 0, s_nachricht = 0;

void re15_panel2030_r41_reset(void)
{
    s_offen = 0; s_fahrt = 0; s_quelle = -1; s_ruhe = 0; s_ziel = 0; s_nachricht = 0;
    s_cx_q8 = RE15_R41N_CURSOR_X0 << 8; s_cy_q8 = RE15_R41N_CURSOR_Y0 << 8;
    for (int i = 0; i < RE15_R41N_TANKS; i++) s_stand[i] = s_von[i] = k_start[i];
}

void re15_panel2030_r41_oeffnen(void)
{
    re15_panel2030_r41_reset();
    s_offen = 1;
    LOG("[panel2030] offen: A %d B %d C %d\n", s_stand[0], s_stand[1], s_stand[2]);
}

int re15_panel2030_r41_offen(void) { return s_offen; }

/* Die Regel aus Nachtrag 37. */
int re15_panel2030_r41_aktion(int t)
{
    if (t < 0 || t >= RE15_R41N_TANKS) return 0;
    if (s_quelle < 0) {
        if (s_stand[t] <= 0) return 4;                  /* Quelle leer: Fehlerton, Satz */
        s_quelle = t;
        return 1;
    }
    if (s_quelle == t) { s_quelle = -1; return 2; }     /* dieselbe Taste nimmt die Wahl zurueck */
    if (s_stand[t] >= k_kap[t]) return 5;               /* Ziel voll: Fehlerton, Satz, Quelle bleibt gewaehlt */
    for (int i = 0; i < RE15_R41N_TANKS; i++) s_von[i] = s_stand[i];
    const int frei = k_kap[t] - s_stand[t];
    const int menge = s_stand[s_quelle] < frei ? s_stand[s_quelle] : frei;    /* bis die Quelle leer oder das Ziel voll ist */
    s_stand[s_quelle] -= menge;
    s_stand[t] += menge;
    s_quelle = -1;
    s_fahrt = RE15_R41N_UMFUELL_TAKTE;
    return 3;
}

static int tank_unter(int x, int y)
{
    if (y < RE15_R41N_TANK_Y0 || y >= RE15_R41N_TANK_Y1) return -1;
    for (int i = 0; i < RE15_R41N_TANKS; i++) if (x >= k_x0[i] && x < k_x1[i]) return i;
    return -1;
}

static int anzeige_q8(int t)    /* Fuellstand in 24.8, laeuft waehrend der 16 Bilder von s_von nach s_stand */
{
    const int ziel = s_stand[t] << 8;
    if (s_fahrt <= 0) return ziel;
    const int von = s_von[t] << 8;
    return von + (ziel - von) * (RE15_R41N_UMFUELL_TAKTE - s_fahrt) / RE15_R41N_UMFUELL_TAKTE;
}

int re15_panel2030_r41_takt(uint16_t edge, uint16_t held, int nachricht)
{
    if (!s_offen) return RE15_R41N_ERG_NICHTS;
    s_nachricht = nachricht;
    if (s_fahrt > 0) s_fahrt--;
    if (s_ziel) {
        if (s_fahrt == 0 && !nachricht && ++s_ruhe >= RE15_R41N_RUHE) {   /* RE2 @0x0171C sleep 30, dann @0x01752 cmp == 80 */
            s_offen = 0;
            re15_audio_re2_panel_se(RE15_PANEL_SE_BESTAET);              /* RE2 @0x01762 se 0x0C */
            LOG("[panel2030] GELOEST: A %d B %d C %d -> Anzeige 80\n", s_stand[0], s_stand[1], s_stand[2]);
            return RE15_R41N_ERG_GELOEST;
        }
        return RE15_R41N_ERG_NICHTS;
    }
    if (nachricht) return RE15_R41N_ERG_NICHTS;
    /* Cursor: Steuerkreuz je Bild wie ROOM2050 sub01 @0x02182..@0x021E0 -> sub05..08 (Schritt im Kopf). */
    if (held & RE15_HC_ZUG_HOCH)   s_cy_q8 -= RE15_R41N_SCHRITT_Q8;
    if (held & RE15_HC_ZUG_RUNTER) s_cy_q8 += RE15_R41N_SCHRITT_Q8;
    if (held & RE15_HC_ZUG_RECHTS) s_cx_q8 += RE15_R41N_SCHRITT_Q8;
    if (held & RE15_HC_ZUG_LINKS)  s_cx_q8 -= RE15_R41N_SCHRITT_Q8;
    if (s_cx_q8 < RE15_R41N_X_MIN << 8) s_cx_q8 = RE15_R41N_X_MIN << 8;
    if (s_cx_q8 > RE15_R41N_X_MAX << 8) s_cx_q8 = RE15_R41N_X_MAX << 8;
    if (s_cy_q8 < RE15_R41N_Y_MIN << 8) s_cy_q8 = RE15_R41N_Y_MIN << 8;
    if (s_cy_q8 > RE15_R41N_Y_MAX << 8) s_cy_q8 = RE15_R41N_Y_MAX << 8;
    const int cx = s_cx_q8 >> 8, cy = s_cy_q8 >> 8;
    if (edge & RE15_HC_ZUG_ABBRUCH) {                                     /* CROSS wie das Pult 2050 */
        s_offen = 0;
        LOG("[panel2030] Abbruch\n");
        return RE15_R41N_ERG_ZU;
    }
    if (edge & RE15_HC_ZUG_DRUCK) {                                       /* SQUARE-Flanke (Sce_key_ck 0x40, 2060 @0x011D2) */
        if (cx >= k_exit[0] && cy >= k_exit[1] && cx < k_exit[2] && cy < k_exit[3]) {
            s_offen = 0;
            re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);                 /* RE2 ROOM2130 sub04 @0x01192 */
            LOG("[panel2030] EXIT\n");
            return RE15_R41N_ERG_ZU;
        }
        const int t = tank_unter(cx, cy);
        if (t >= 0) {
            const int erg = re15_panel2030_r41_aktion(t);
            if (erg >= 1 && erg <= 3) re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);
            if (erg == 4 || erg == 5) re15_audio_re2_panel_se(RE15_R41C_SE_FEHLER);   /* RE2 ROOM2130 sub04 @0x01800 */
            if (erg == 4) re15_scd_show_message(RE15_R41C_MSG_PANEL_LEER, (uint32_t)RE15_R41C_PAUSE << 16);
            if (erg == 5) re15_scd_show_message(RE15_R41C_MSG_PANEL_VOLL, (uint32_t)RE15_R41C_PAUSE << 16);
            LOG("[panel2030] Tank %c: %s -> A %d B %d C %d\n", 'A' + t, erg == 1 ? "Quelle" : erg == 2 ? "Quelle zurueck" : erg == 3 ? "umgefuellt" :
                erg == 4 ? "leer" : "voll", s_stand[0], s_stand[1], s_stand[2]);
            if (erg == 3 && s_stand[0] == RE15_R41N_ZIEL) { s_ziel = 1; s_ruhe = 0; LOG("[panel2030] Anzeige 80 erreicht\n"); }
        }
    }
    return RE15_R41N_ERG_NICHTS;
}

int  re15_panel2030_r41_stand(int t) { return (t >= 0 && t < RE15_R41N_TANKS) ? s_stand[t] : -1; }
int  re15_panel2030_r41_anzeige_zehntel(void) { return (anzeige_q8(0) * 10) >> 8; }
int  re15_panel2030_r41_quelle(void) { return s_quelle; }
int  re15_panel2030_r41_ziel_erreicht(void) { return s_ziel; }
void re15_panel2030_r41_cursor(int *x, int *y) { if (x) *x = s_cx_q8 >> 8; if (y) *y = s_cy_q8 >> 8; }
void re15_panel2030_r41_cursor_setzen(int x, int y) { s_cx_q8 = x << 8; s_cy_q8 = y << 8; }

/* ---- Bild --------------------------------------------------------------------------------------------------------------- */
#ifdef RE15_PLATFORM_PC
static uint32_t s_rgba[RE15_R41N_PANEL_W * RE15_R41N_PANEL_H];

static uint32_t farbe(unsigned v)
{
    const uint8_t *c = &re15_r41n_panel_pal[v * 3];
    return ((uint32_t)c[0] << 24) | ((uint32_t)c[1] << 16) | ((uint32_t)c[2] << 8) | 0xFFu;
}

static void punkt_mischen(int x, int y, int r, int g, int b, int a)    /* a 0..256 */
{
    if ((unsigned)x >= RE15_R41N_PANEL_W || (unsigned)y >= RE15_R41N_PANEL_H) return;
    uint32_t *p = &s_rgba[y * RE15_R41N_PANEL_W + x];
    const int r0 = (int)(*p >> 24), g0 = (int)((*p >> 16) & 255), b0 = (int)((*p >> 8) & 255);
    *p = ((uint32_t)((r0 * (256 - a) + r * a) >> 8) << 24) | ((uint32_t)((g0 * (256 - a) + g * a) >> 8) << 16) |
         ((uint32_t)((b0 * (256 - a) + b * a) >> 8) << 8) | 0xFFu;
}

/* Strecke mit Halbbreite b (Hundertstel Bildpunkte), weich an der Kante: derselbe Zeiger wie im Entwurf (panel_kit.instrument). */
static void strecke(double x0, double y0, double x1, double y1, double b, int r, int g, int bl, int deckung)
{
    const int ax = (int)floor((x0 < x1 ? x0 : x1) - b - 2), ex = (int)ceil((x0 > x1 ? x0 : x1) + b + 2);
    const int ay = (int)floor((y0 < y1 ? y0 : y1) - b - 2), ey = (int)ceil((y0 > y1 ? y0 : y1) + b + 2);
    const double dx = x1 - x0, dy = y1 - y0, l2 = dx * dx + dy * dy;
    for (int y = ay; y <= ey; y++)
        for (int x = ax; x <= ex; x++) {
            const double px = x + .5 - x0, py = y + .5 - y0;
            double h = (px * dx + py * dy) / (l2 > 0 ? l2 : 1);
            if (h < 0) h = 0; if (h > 1) h = 1;
            const double qx = px - dx * h, qy = py - dy * h;
            const double d = sqrt(qx * qx + qy * qy) - b;
            const double a = d <= -.5 ? 1. : d >= .5 ? 0. : .5 - d;
            if (a > 0) punkt_mischen(x, y, r, g, bl, (int)(a * deckung));
        }
}

static void zeiger(int wert_zehntel)
{
    const double R = RE15_R41N_ZEIGER_R / 10.0, cx = RE15_R41N_ZEIGER_X, cy = RE15_R41N_ZEIGER_Y;
    const double ph = (-135.0 + 270.0 * wert_zehntel / 1000.0) * 3.14159265358979 / 180.0, s = sin(ph), k = cos(ph);
    strecke(cx - R * .22 * s + .8, cy + R * .22 * k + 1., cx + R * .86 * s + .8, cy - R * .86 * k + 1., .5, 0, 0, 0, 87);   /* Schatten */
    strecke(cx - R * .24 * s, cy + R * .24 * k, cx + R * .58 * s, cy - R * .58 * k, .62, 16, 15, 15, 256);
    strecke(cx + R * .50 * s, cy - R * .50 * k, cx + R * .88 * s, cy - R * .88 * k, .34, 16, 15, 15, 256);
    strecke(cx, cy, cx, cy, 2.3, 96, 96, 96, 256);                                                                          /* Nabe */
}

static void lampe(int t)    /* Lage wie im Generator: x 84,5 / 161 / 205, y 135,8, 7 x 3 */
{
    static const int k_lx[RE15_R41N_TANKS] = { 85, 161, 205 };
    for (int y = 136; y < 139; y++)
        for (int x = k_lx[t]; x < k_lx[t] + 7; x++) punkt_mischen(x, y, y == 136 ? 190 : 40, y == 136 ? 255 : 212, y == 136 ? 200 : 80, 256);
}

const uint32_t *re15_panel2030_r41_rgba(void)
{
    if (!s_offen) return NULL;
    for (int i = 0; i < RE15_R41N_PANEL_W * RE15_R41N_PANEL_H; i++) s_rgba[i] = farbe(re15_r41n_panel_grund[i]);
    for (int f = 0; f < (int)(sizeof re15_r41n_panel_fenster / sizeof re15_r41n_panel_fenster[0]); f++) {
        const re15_r41n_fenster_t *w = &re15_r41n_panel_fenster[f];
        /* Zeile des Pegels: Wert 100 -> GT, Wert 0 -> GB (Skala je Tank gleich). */
        const int lq8 = anzeige_q8(w->tank);
        const int ly = (RE15_R41N_GB * 256 - (RE15_R41N_GB - RE15_R41N_GT) * lq8 / 100) >> 8;
        for (int y = 0; y < w->h; y++) {
            const int by = w->y + y;
            if (by < ly || lq8 <= 0) continue;
            for (int x = 0; x < w->w; x++) s_rgba[by * RE15_R41N_PANEL_W + w->x + x] = farbe(re15_r41n_panel_voll[w->off + y * w->w + x]);
        }
        if (lq8 > 0)
            for (int x = 2; x < w->w - 2; x++) punkt_mischen(w->x + x, ly, 250, 248, 224, 200);   /* Pegel-Kante wie kit.roehre */
    }
    zeiger((anzeige_q8(0) * 10) >> 8);
    if (s_quelle >= 0) lampe(s_quelle);
    const int cx = s_cx_q8 >> 8, cy = s_cy_q8 >> 8;
    for (int i = 0; i < (int)(sizeof re15_r41n_panel_cursor / sizeof re15_r41n_panel_cursor[0]); i++) {
        const re15_r41n_cursorpx_t *c = &re15_r41n_panel_cursor[i];
        punkt_mischen(cx + c->dx, cy + c->dy, c->r, c->g, c->b, 256);
    }
    if (s_nachricht)                                                        /* Band hinter dem Text (Text = Glyphen ohne Kasten) */
        for (int y = RE15_R41N_NACHRICHT_Y0; y < RE15_R41N_NACHRICHT_Y1; y++)
            for (int x = RE15_R41N_X_MIN; x < RE15_R41N_X_MAX; x++) punkt_mischen(x, y, 0, 0, 0, 170);
    return s_rgba;
}
#else
const uint32_t *re15_panel2030_r41_rgba(void) { return NULL; }
#endif
