/* RUNDE 19, MARKE 2 — ROOM10E0 Cut 7: die Maske darf die Figur nicht ZERREISSEN.
 *
 * ⛔ NUTZER-BEFUND 2026-09-21 (befund.log Zeile 13196, Abzug befund_10E0_F185_marke2.bmp):
 * "es gibt ein paar PRIs die sind zu falsch". Am Bild gemessen: der Spieler steht bei
 * Welt(-2360, 0, -164) hinter der Liege, sein Oberkoerper ist ueber der Matratze zu
 * sehen, seine Schuhe unter dem Gestell — und die Bildzeilen dazwischen sind geloescht.
 * Die Figur erscheint als ZWEI zusammenhanglose Stuecke. Gemessen (Dossier
 * analysis/befunde_2026-09-21/pri-runde19/marke2-10E0-C7.md): 1140 Figurpunkte im
 * Messkasten verdeckt, in 24 von 29 Spalten zerrissen.
 *
 * URSACHE, belegt: die Tiefe der Liege kam aus dem UMSCHLIESSENDEN RECHTECK ihrer
 * SCA-Zelle. Die Zelle ist SCA-Eintrag 21 @ROOM10E0.RDT Datei-Offset 0x758,
 * 12 Bytes  22 0b 74 0e 8c f1 88 fa 05 ff 00 03  (width 2850, density 3700,
 * x -3700, z -1400, type 0x05) — Typ 5 ist eine "/"-DIAGONALE, solide nur wo
 * pz > Z0 + (D/W)*(px-X0) (re15_collision.c push_diag5, @LAB_8003c734,
 * ghidra1_V2.txt:144830-145138). An der Spielerspalte px=-2360 beginnt die solide
 * Flaeche bei z=+340, die Nahkante des Huellrechtecks liegt bei z=-1400: 1739
 * Welteinheiten ZU NAH. geom.sca_sperrzellen() warnt im eigenen Docstring genau davor
 * ("eine Diagonale als volles Rechteck geraycastet waere ZU NAH").
 *
 * DIE SCHRANKE hier ist kein Vergleich mit einem Wunschbild, sondern eine
 * GEOMETRISCHE UNMOEGLICHKEIT: die Kamera schaut von oben, die Fuesse der Figur sind
 * FERNER als ihr Kopf (hier 6361 gegen 5970). Eine Maske verdeckt alles, was FERNER
 * ist als sie (re15_pri_mask_occludes). Sie kann deshalb immer nur ein UNTERES
 * Endstueck der Figur nehmen und niemals ein Mittelstueck: eine freie Zeile ZWISCHEN
 * Fuss und der ersten Verdeckung heisst, dieselbe Maske behauptet oben "naeher" und
 * unten "ferner" als die Figur — das kann kein Gegenstand, der auf dem Boden steht.
 * Kein freier Parameter, keine Schwelle nach Gefuehl.
 *
 * Geprueft werden die AKTENKUNDIGEN Standorte des Nutzers in diesem Winkel; die
 * ABDECKUNG (wieviele Figurspalten die Schiene je Standort sieht) wird ausgegeben.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"
#include "re15_tim.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Messgroessen (kein Spielwert): Fusspunkt bis Kopf 1500, halbe Schulterbreite 450 —
 * dieselben Zahlen wie der Messkasten in main.c (befund.log) und test_pri_kopfschnitt. */
#define KOPF_HOCH  1500
#define HALB_BREIT  450

static uint8_t  s_deck[240][320];
static uint16_t s_tief[240][320];

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *p; long len;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

/* Blit atlas[src] -> screen[dst], Palettenindex != 0 = deckend (bg_pc.c
 * pri_publish_tim: Index 0 ist transparent; render_pc.c blittet mit SDL_RenderCopy). */
static void rastern(const re15_pri_cut_t *pri, const re15_tim_t *tim)
{
    const uint8_t *idx = (const uint8_t *) tim->pixels; int i, x, y;
    memset(s_deck, 0, sizeof s_deck); memset(s_tief, 0, sizeof s_tief);
    for (i = 0; i < pri->draw_count; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
        for (y = 0; y < m->height; y++) {
            int sy = m->srcY + y, ty = dy + y;
            if (sy < 0 || sy >= tim->height || ty < 0 || ty >= 240) continue;
            for (x = 0; x < m->width; x++) {
                int sx = m->srcX + x, tx = dx + x;
                if (sx < 0 || sx >= tim->width || tx < 0 || tx >= 320) continue;
                if (!idx[sy * tim->width + sx]) continue;
                s_deck[ty][tx] = 1;
                if (!s_tief[ty][tx] || m->depth < s_tief[ty][tx]) s_tief[ty][tx] = m->depth;
            }
        }
    }
}

static void proj(const re15_camera_view_t *v, double x, double y, double z,
                 double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    *sx = 160.0 + vx * (double) v->fov_screen_dist / *vz;
    *sy = 120.0 + vy * (double) v->fov_screen_dist / *vz;
}

/* Verdeckt die Maske die Figurachse in Spalte x, Bildzeile y? Die Kamera-z der
 * Senkrechten durch den Standplatz wird aus der Bildzeile zurueckgerechnet: der
 * Anteil t zwischen Fuss- und Kopfzeile ist die Welt-Hoehe -t*1500. */
static int verdeckt_zeile(const re15_camera_view_t *v, int32_t wx, int32_t wz,
                          double fsy, double hsy, int x, int y)
{
    double t, sx, sy, vz;
    if (x < 0 || x >= 320 || y < 0 || y >= 240) return -1;      /* -1 = nicht messbar */
    t = (fsy - (double) y) / (fsy - hsy);
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    proj(v, wx, -(int32_t) (KOPF_HOCH * t), wz, &sx, &sy, &vz);
    if (vz <= 64) return -1;
    return (s_deck[y][x] && re15_pri_mask_occludes(s_tief[y][x], (long) vz)) ? 1 : 0;
}

struct marke { const char *name; int32_t x, z; };

/* Die aktenkundigen Standorte des Nutzers in ROOM10E0 Cut 7 (auswahl.json _warum +
 * befund.log). Alle vier liegen im Huellrechteck der Typ-5-Zelle und alle VOR ihrer
 * echten Diagonalflaeche — deshalb hat er in diesem Winkel dreimal dasselbe gemeldet. */
static const struct marke MARKEN[] = {
    { "F185  2026-09-21 (Marke 2)",         -2360,  -164 },
    { "F4141 2026-09-07 'mittendrin'",      -2360,  -164 },
    { "F523  2026-09-07 'muss davor sein'", -1137,  1375 },
    { "F367  2026-09-08 'durch die Wand'",  -1422,   485 },
};

int main(void)
{
    char pfad[512];
    size_t sz = 0, msz = 0, tsz = 0;
    uint8_t *roh, *msk, *tb;
    re15_rdt_t rdt; re15_camera_view_t view; re15_pri_cut_t pri; re15_tim_t tim;
    uint32_t off; int n, mi;
    int summe_zerrissen = 0, summe_spalten = 0;
    const int CUT = 7;

    printf("=== Runde 19 / Marke 2: ROOM10E0 Cut 7 darf die Figur nicht zerreissen ===\n");

    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE1/ROOM10E0.RDT", RE15_PORT_SRC_DIR);
    roh = slurp(pfad, &sz);
    if (!roh || re15_rdt_parse(roh, sz, &rdt) < 0 || CUT >= rdt.cut_count
        || re15_camera_build_view(&rdt.cuts[CUT], &view) != 0) {
        printf("  FAIL: RDT/Kamera nicht lesbar: %s\n", pfad);
        free(roh); return 1;
    }
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM10E0.MSK", RE15_PORT_SRC_DIR);
    msk = slurp(pfad, &msz);
    memset(&pri, 0, sizeof pri);
    off = msk ? re15_pri_msk_section_offset(msk, msz, CUT) : 0;
    n = off ? re15_pri_parse_section(msk, msz, off, &pri) : 0;
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM10E0_PRI07.TIM", RE15_PORT_SRC_DIR);
    tb = slurp(pfad, &tsz);
    if (n <= 0 || !tb || re15_tim_parse(tb, (int) tsz, &tim) != 0 || tim.bpp != 8) {
        printf("  FAIL: Maskensektion oder Atlas fehlt (Sektion %d Masken)\n", n);
        free(roh); free(msk); free(tb); return 1;
    }
    rastern(&pri, &tim);
    printf("  Maskensatz: %d Masken gebaut, %d gezeichnet\n", pri.mask_count, pri.draw_count);

    for (mi = 0; mi < (int) (sizeof MARKEN / sizeof MARKEN[0]); mi++) {
        const struct marke *mk = &MARKEN[mi];
        double fsx, fsy, fvz, hsx, hsy, hvz;
        int hw, x, spalten = 0, mit = 0, zerrissen = 0, beispiel = -1;

        proj(&view, mk->x, 0, mk->z, &fsx, &fsy, &fvz);
        proj(&view, mk->x, -KOPF_HOCH, mk->z, &hsx, &hsy, &hvz);
        (void) hsx;
        if (fvz <= 64 || hvz <= 64 || fsy <= hsy) {
            printf("  %-34s nicht messbar (Figur nicht im Bild)\n", mk->name);
            continue;
        }
        hw = (int) ((double) HALB_BREIT * view.fov_screen_dist / fvz);
        if (hw < 1) hw = 1;

        for (x = (int) fsx - hw; x <= (int) fsx + hw; x++) {
            int y, gesehen = 0, frei_unter = 0, erste_verd = -1;
            /* Von FUSS (unten, y gross) nach KOPF (oben, y klein). */
            for (y = (int) fsy; y >= (int) hsy; y--) {
                int v = verdeckt_zeile(&view, mk->x, mk->z, fsy, hsy, x, y);
                if (v < 0) continue;
                gesehen++;
                if (v) { erste_verd = y; break; }
                frei_unter = 1;
            }
            if (!gesehen) continue;
            spalten++;
            if (erste_verd < 0) continue;                  /* gar keine Verdeckung */
            mit++;
            if (frei_unter) {                              /* frei UNTER der Verdeckung */
                zerrissen++;
                if (beispiel < 0) beispiel = x;
            }
        }
        summe_spalten += spalten;
        summe_zerrissen += zerrissen;
        printf("  %-34s Welt(%6d,%6d) vz Fuss %5.0f Kopf %5.0f | Spalten %2d (Abdeckung)"
               ", mit Verdeckung %2d, ZERRISSEN %2d\n",
               mk->name, mk->x, mk->z, fvz, hvz, spalten, mit, zerrissen);
        if (beispiel >= 0) printf("       erste zerrissene Spalte x=%d\n", beispiel);
    }

    printf("  Abdeckung der Schiene: %d Figurspalten ueber %d Standorte geprueft\n",
           summe_spalten, (int) (sizeof MARKEN / sizeof MARKEN[0]));
    CHECK("ROOM10E0 Cut 7: kein Standort des Nutzers wird zerrissen", summe_zerrissen == 0);

    free(roh); free(msk); free(tb);
    printf(g_fail ? "FEHLGESCHLAGEN\n" : "OK\n");
    return g_fail;
}
