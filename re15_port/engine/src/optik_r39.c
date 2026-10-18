/*
 * Runde 39 Spur L (optik) — Westwand-Masken ROOM1100/1101 an der GEMALTEN Ecke kappen.
 * Kopf und Belege: include/re15_optik_r39.h, Dossier analysis/befunde_runde39/L_optik.md (Punkt 1).
 *
 * Die Fenster unten sind am Hintergrund GEMESSEN (Engine-Dekoder probe_bg_dump, STAGE1/ROOM110.BSS Scheibe Cut*0x10000,
 * Helligkeit L = 0,299R + 0,587G + 0,114B, Pixelmitte bei x+0,5). Je Zeilenband die Spalten, in denen das Bild die NAHE
 * Westwand zeigt; was jenseits liegt, ist Rueckwand / Boden / Schwelle des hinteren Gangs:
 *
 *  Cut 1 (Kamera ROOM1100.RDT @0x80: fov 26684, Ort (-13122,-3078,-13608), Ziel (-15912,-954,-25398); Wand RECHTS)
 *    Zeilen 0..110   helle Eckleiste der Wand: Spalte 173 L 150..207, 174 L 133..180, 175 L 37..72 (Wand), links die
 *                    Rueckwand 166..171 L 78..118; Halbwert-Kante Rueckwand -> Eckleiste in 71 Zeilen (40..110)
 *                    x = 172,84 (172,24..173,08) -> Wand ab Spalte 173.
 *    Zeilen 111..239 Sockel: dunkle Kantenlinie Spalte 171 (L 5..9 in Zeilen 112..148), Spalte 170 = Sockel der
 *                    Rueckwand (L 13..22, Sockel-Oberkante Zeile 101 wie Spalten 160..169) bzw. ab Zeile 130 Boden
 *                    -> Wand ab Spalte 171. Die Wand-Silhouette endet unterhalb ohnehin an der Wandbasis (>= 172).
 *  Cut 2 (Kamera @0xA0: fov 26684, Ort (-13068,-3078,-21852), Ziel (-14742,-954,-11520); Wand LINKS)
 *    Zeilen 0..106   nahe Wand hell (Sockel-Oberkante der nahen Wand erst Zeile ~106), ferner Sockel ab Zeile 97 dunkel:
 *                    Halbwert-Kante in Zeilen 97..105 x = 133,94 (133,74..134,47) -> Wand bis Spalte 133.
 *    Zeilen 107..239 naher Sockel (L 16..27) gegen fernen Sockel (L 7..12): Halbwert-Kante in Zeilen 108..122
 *                    x = 134,86 (134,46..135,13) -> Wand bis Spalte 134.
 *
 * ELZA: ROOM1101 zeichnet die Masken von ROOM1100 (re15_pri_masken_raum, Runde 37 Spur A; RDT bytegleich) -> dieselben
 * Fenster ueber den Masken-Raum 0x1100.
 */
#include <string.h>
#include "re15_optik_r39.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

typedef struct {
    uint16_t raum;        /* Raum-Id der MSK-Datei (re15_pri_masken_raum) */
    uint8_t  cut;
    uint8_t  y0, y1;      /* Bildzeilen, einschliesslich */
    int16_t  x_min, x_max;/* Spalten der GEMALTEN Wand in diesen Zeilen, einschliesslich */
} re15_optik_r39_fenster_t;

static const re15_optik_r39_fenster_t k_fenster[] = {
    { 0x1100, 1,   0, 110, 173, 319 },   /* Cut 1 Eckleiste: Halbwert-Kante x = 172,84 */
    { 0x1100, 1, 111, 239, 171, 319 },   /* Cut 1 Sockel: Kantenlinie Spalte 171 */
    { 0x1100, 2,   0, 106,   0, 133 },   /* Cut 2 Wand: Halbwert-Kante x = 133,94 */
    { 0x1100, 2, 107, 239,   0, 134 },   /* Cut 2 Sockel: Halbwert-Kante x = 134,86 */
};
#define K_FENSTER_N ((int) (sizeof k_fenster / sizeof k_fenster[0]))

/* Fenster des Zeilen-y fuer Raum/Cut, NULL = keins. */
static const re15_optik_r39_fenster_t *fenster_fuer(unsigned raum, int cut, int y)
{
    int i;
    for (i = 0; i < K_FENSTER_N; i++) {
        const re15_optik_r39_fenster_t *f = &k_fenster[i];
        if (f->raum == (uint16_t) raum && (int) f->cut == cut && y >= (int) f->y0 && y <= (int) f->y1) return f;
    }
    return NULL;
}

static int hat_fenster(unsigned raum, int cut)
{
    int i;
    for (i = 0; i < K_FENSTER_N; i++)
        if (k_fenster[i].raum == (uint16_t) raum && (int) k_fenster[i].cut == cut) return 1;
    return 0;
}

int re15_optik_r39_in_wand(unsigned masken_raum, int cut, int x, int y)
{
    const re15_optik_r39_fenster_t *f;
    if (!hat_fenster(masken_raum, cut)) return -1;
    f = fenster_fuer(masken_raum, cut, y);
    if (!f) return 1;                         /* Zeile ohne Fenster: unveraendert */
    return (x >= f->x_min && x <= f->x_max) ? 1 : 0;
}

/* Erlaubte Spalten [*a,*b] des Rechtecks [x0,x1] in Zeile y (a > b = Zeile entfaellt). */
static void spalten(unsigned raum, int cut, int y, int x0, int x1, int *a, int *b)
{
    const re15_optik_r39_fenster_t *f = fenster_fuer(raum, cut, y);
    *a = x0; *b = x1;
    if (!f) return;
    if (*a < f->x_min) *a = f->x_min;
    if (*b > f->x_max) *b = f->x_max;
}

int re15_optik_r39_kappen(unsigned masken_raum, int cut, re15_pri_cut_t *pri)
{
    static re15_pri_mask_t neu[RE15_PRI_MAX_MASKS_PER_CUT];
    int i, n = 0, d = 0, ueberlauf = 0;
    long weg = 0;
    if (!pri) return 0;
    if (!hat_fenster(masken_raum, cut) || pri->mask_count <= 0) return pri->mask_count;
    for (i = 0; i < pri->mask_count && !ueberlauf; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        const int x0 = (int16_t) m->dstX, y0 = (int16_t) m->dstY;
        const int x1 = x0 + (int) m->width - 1, y1 = y0 + (int) m->height - 1;
        int y = y0;
        while (y <= y1) {
            int a, b, ye = y, a2, b2;
            spalten(masken_raum, cut, y, x0, x1, &a, &b);
            /* gleiche Spalten in den Folgezeilen -> ein Teilrechteck */
            while (ye + 1 <= y1) {
                spalten(masken_raum, cut, ye + 1, x0, x1, &a2, &b2);
                if (a2 != a || b2 != b) break;
                ye++;
            }
            if (a <= b) {
                re15_pri_mask_t t = *m;
                if (n >= RE15_PRI_MAX_MASKS_PER_CUT) { ueberlauf = 1; break; }
                t.srcX   = (uint16_t) (m->srcX + (a - x0));
                t.srcY   = (uint16_t) (m->srcY + (y - y0));
                t.dstX   = (uint16_t) (int16_t) a;
                t.dstY   = (uint16_t) (int16_t) y;
                t.width  = (uint16_t) (b - a + 1);
                t.height = (uint16_t) (ye - y + 1);
                neu[n++] = t;
                if (i < pri->draw_count) d++;
                weg += (long) (x1 - x0 + 1 - (b - a + 1)) * (ye - y + 1);
            } else {
                weg += (long) (x1 - x0 + 1) * (ye - y + 1);
            }
            y = ye + 1;
        }
    }
    if (ueberlauf) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[optik_r39] ROOM%04X Cut %d: Kappen braeuchte mehr als %d Rechtecke - unveraendert\n",
                masken_raum, cut, RE15_PRI_MAX_MASKS_PER_CUT);
#endif
        return pri->mask_count;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[optik_r39] ROOM%04X Cut %d: Westwand an der gemalten Ecke gekappt: %d -> %d Rechtecke, "
                    "%ld Rechteckpunkte jenseits der Ecke entfernt (Runde 39 Spur L)\n",
            masken_raum, cut, pri->mask_count, n, weg);
#endif
    memcpy(pri->masks, neu, (size_t) n * sizeof neu[0]);
    pri->mask_count = n;
    pri->draw_count = d;
    return n;
}
