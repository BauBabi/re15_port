/* Eine Maske hat EINE Tiefe, eine Figur nicht — wo schneidet das durch?
 *
 * ⛔ NUTZER-BEFUND 2026-09-06, am laufenden Spiel gemessen (befund.log, ROOM1140
 * Cut 0, drei F9-Marken):
 *
 *     MARKE 1  vz Fuss/Huefte/Kopf = 8090 / 7856 / 7621   Stuhl-Maske z = 7488
 *     MARKE 2                        7764 / 7530 / 7295
 *     MARKE 3                        7582 / 7347 / 7113
 *
 * Der Zeichner verdeckt ein Dreieck, solange dessen Kamera-Tiefe GROESSER als
 * `depth*64` der Maske ist (re15_pri.h). Weil die Kamera von oben schaut, liegt der
 * Kopf naeher als die Fuesse — gemessen 469 Einheiten Unterschied. Faellt die
 * Maskentiefe DAZWISCHEN, wird die Figur waagerecht durchgeschnitten: Beine verdeckt,
 * Kopf nicht. Beim Naeherkommen schaelt die Maske sie von oben herab frei.
 *
 * Ueber alle 597 aufgezeichneten Bilder des Nutzers in diesem Cut:
 *     505 Bilder in der Stuhl-Spalte
 *       0 davon mit dem FUSS vor der Maske (er stand also NIE davor)
 *     485 davon durchgeschnitten
 * Er steht nie vor dem Stuhl und wird trotzdem in 485 von 505 Bildern nur halb
 * verdeckt. Das ist der Fehler — nicht eine Frage des Geschmacks.
 *
 * ⛔ WOZU DIESE SCHRANKE STATT EINER EINZELKORREKTUR: der Nutzer hat zu Recht gesagt,
 * er will nicht "fuer jedes einzelne PRI 400 Runden drehen". Der Fall ist ohne ihn
 * pruefbar — die begehbaren Punkte kennt die Engine (re15_collision_on_floor), die
 * Kamera steht im RDT, die Maskentiefen in der Datei. Diese Schranke geht deshalb ALLE
 * Raeume und Winkel durch und meldet jede Maske, die diesen Schnitt erzeugt.
 *
 * GEPRUEFT WIRD NUR, WAS WIR SELBST GEBAUT HABEN. Wo das Original eine eigene
 * Maskensektion fuehrt, sind die Tiefen die der Kuenstler — die stehen hier nicht zur
 * Debatte und werden getrennt ausgewiesen (sie sind ausserdem der Massstab, an dem sich
 * die Regel fuer die nachgezeichneten ablesen laesst).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_pri.h"
#include "re15_tim.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Koerpermasse als MESSGROESSEN (kein Spielwert): Fusspunkt bis Kopf 1500 Einheiten
 * — dieselbe Zahl, die die Sonde RE15_POCC benutzt und mit der die drei Marken oben
 * gemessen wurden; halbe Schulterbreite 450. */
#define KOPF_HOCH   1500
#define HALB_BREIT   450

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

/* Kamera-Tiefe eines Weltpunktes — dieselbe Rechnung wie im Zeichner (main.c
 * PROJECT_VERT: vz = rot[6..8]·p / 4096 + trans[2]). */
static long vz_von(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z)
{
    return ((long) x * v->rot[6] + (long) y * v->rot[7] + (long) z * v->rot[8]) / 4096
           + v->trans[2];
}

static void bild_von(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z,
                     int *sx, int *sy, long *vz)
{
    long vx = ((long) x * v->rot[0] + (long) y * v->rot[1] + (long) z * v->rot[2]) / 4096
              + v->trans[0];
    long vy = ((long) x * v->rot[3] + (long) y * v->rot[4] + (long) z * v->rot[5]) / 4096
              + v->trans[1];
    *vz = vz_von(v, x, y, z);
    if (*vz > 64) {
        *sx = 160 + (int) (vx * v->fov_screen_dist / *vz);
        *sy = 120 + (int) (vy * v->fov_screen_dist / *vz);
    } else { *sx = 160; *sy = 120; }
}

/* =====================================================================================
 * PHASE 2 (2026-09-19, pri-masken-audit.md §4.6/§4.8): VOR/HINTER-Standplaetze je Cut.
 *
 * Fuer jeden STAGE1-Cut, fuer den der Bau eine Ziel-Silhouette MASKS/ROOM####_PRI##.PBM und
 * die Standlinie MASKS/ROOM####_PRI##.STAND (Kamera-z je Bildspalte) hinterlegt hat:
 *   - BEGEHBARE Standplaetze ueber den SPIELER-Klemmpfad (re15_collision_constrain, Radius
 *     PR=450, Band gesetzt): ein Punkt ist begehbar, wenn der Klemmpfad ihn nicht verschiebt.
 *     ⛔ NICHT re15_collision_on_floor: das ist der Containment-Scan FUN_8003b7f0 und liefert
 *     die Zell-INNENRAEUME (= Hindernisse; gemessen 2026-09-19: 3350/3350 "Bodenpunkte" der
 *     alten Sonde in ROOM1010 lagen IN soliden Zellen, waehrend 1201 von 1416 echten
 *     Nutzer-Standorten aus befund.log auf dem Klemmpfad-Komplement liegen, 46 im alten Satz).
 *     Die Phase 1 dieses Tests oben rechnet noch mit on_floor — ihre Zahlen sind ein
 *     Verhaeltnis-Pin und bleiben unveraendert stehen.
 *   - Koerperkasten Fuss..Kopf (1500) und +-450, je Bildzeile die Kamera-z der SENKRECHTEN
 *     durch den Standplatz (dieselbe Formel wie tools/maske/geometrie.profil_spalte), das
 *     Urteil je Maskenpunkt nach der ORIGINAL-Regel re15_pri_mask_occludes.
 *   - VOR = Spalten, in denen der Fuss NAEHER liegt als die Standlinie der Maske; ein
 *     Standplatz, der dort zu >= 95 % verdeckt wird, ist ein FEHLER (VORverd). HINTER =
 *     Fuss FERNER als die Standlinie; dort soll er zu >= 95 % verdeckt sein (HINTfrei wird
 *     gezaehlt und ausgegeben, ist aber kein Riegel: eine Maske deckt nur ihre eigene
 *     Silhouette, und ein Kopf, der ueber den Tisch ragt, ist richtig frei).
 *   ⛔ Die 95 % sind eine Heuristik (Gegenpruefung #12), kein belegter Wert.
 * Riegel: VORverd == 0 ueber ALLE Cuts mit PBM. */
static uint8_t  p2_deck[240][320];
static uint16_t p2_tief[240][320];
static int      p2_col_lo[320], p2_col_hi[320];

static int p2_pbm(const char *pfad, uint8_t soll[240][320])
{
    const char *kopf = "P4\n320 240\n"; size_t k = strlen(kopf), n = 0, y, x;
    uint8_t *b = slurp(pfad, &n);
    if (!b || n != k + 240 * 40 || memcmp(b, kopf, k) != 0) { free(b); return 0; }
    for (y = 0; y < 240; y++) for (x = 0; x < 320; x++)
        soll[y][x] = (b[k + y * 40 + x / 8] >> (7 - (x & 7))) & 1;
    free(b); return 1;
}

/* Zeile 1 = Standlinie (Kamera-z je Spalte, -1 = keine), Zeile 2 = Bodenebene y0 je Spalte,
 * auf der die Standlinie gilt — VOR/HINTER wird auf DIESER Ebene verglichen (der Standplatz
 * wird mit seiner (x,z) auf y0 projiziert), sonst laege ein Standplatz auf einem hoeheren Band
 * allein wegen seiner Hoehe "vor" der Standlinie (ROOM10C0 C2, Band 1, 2026-09-19). */
static int p2_stand(const char *pfad, double stand[320], double ebene[320])
{
    FILE *f = fopen(pfad, "r"); char z[8192]; int i = 0, j = 0, zeile = 0;
    if (!f) return 0;
    while (fgets(z, sizeof z, f)) {
        char *p = z; if (z[0] == '#') continue;
        zeile++;
        if (zeile == 1) { while (i < 320) { char *e; long v = strtol(p, &e, 10); if (e == p) break; stand[i++] = (double) v; p = e; } }
        else if (zeile == 2) { while (j < 320) { char *e; long v = strtol(p, &e, 10); if (e == p) break; ebene[j++] = (double) v; p = e; } }
    }
    fclose(f);
    return i == 320 && j == 320;
}

static void p2_rastern(const re15_pri_cut_t *pri, const re15_tim_t *tim)
{
    const uint8_t *idx = (const uint8_t *) tim->pixels; int i, x, y;
    memset(p2_deck, 0, sizeof p2_deck); memset(p2_tief, 0, sizeof p2_tief);
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
                p2_deck[ty][tx] = 1;
                if (!p2_tief[ty][tx] || m->depth < p2_tief[ty][tx]) p2_tief[ty][tx] = m->depth;
            }
        }
    }
    for (x = 0; x < 320; x++) {
        p2_col_lo[x] = 240; p2_col_hi[x] = -1;
        for (y = 0; y < 240; y++) if (p2_deck[y][x]) { if (y < p2_col_lo[x]) p2_col_lo[x] = y; p2_col_hi[x] = y; }
    }
}

/* Projektion mit Gleitkomma wie tools/maske/abnahme.proj (dieselbe Zaehlung wie der Bau). */
static int p2_proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

static double p2_vz_senkrechte(const re15_camera_view_t *v, double wx, double wz, int y, double fallback)
{
    double H = v->fov_screen_dist;
    double a = v->rot[4] / 4096.0, b = (v->rot[3] * wx + v->rot[5] * wz) / 4096.0 + v->trans[1];
    double c = v->rot[7] / 4096.0, d = (v->rot[6] * wx + v->rot[8] * wz) / 4096.0 + v->trans[2];
    double sy = y + 0.5 - 120.0, n = sy * c - H * a, Y, vz;
    if (fabs(n) < 1e-9) return fallback;
    Y = (H * b - sy * d) / n; vz = c * Y + d;
    return (vz > 1) ? vz : fallback;
}

static int p2_phase(void)
{
    char pfad[700]; DIR *d; struct dirent *e;
    int n_cuts = 0, n_fehler = 0, sum_vorverd = 0, sum_hintfrei = 0, sum_vorn = 0, sum_hintn = 0;
    static uint8_t soll[240][320];
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS", RE15_PORT_SRC_DIR);
    d = opendir(pfad);
    printf("\n== Phase 2: VOR/HINTER-Standplaetze je Cut mit Ziel-Silhouette (Begehbarkeit = Klemmpfad) ==\n");
    printf("  %-9s %-3s %7s %7s | %5s %7s %7s | %5s %8s\n", "Raum", "Cut", "Plaetze", "beruehr", "VORn", "VORverd", "VORteil", "HINTn", "HINTfrei");
    if (!d) { printf("  FAIL: %s nicht lesbar\n", pfad); return 1; }
    while ((e = readdir(d)) != NULL) {
        unsigned raum; int cut; size_t sz = 0, msz = 0, tsz = 0;
        uint8_t *roh, *msk, *tb; re15_rdt_t rdt; re15_camera_view_t view; re15_pri_cut_t pri; re15_tim_t tim;
        uint32_t off; int n, lo = 0, hi = 0, b, s;
        int X0 = 1 << 30, X1 = -(1 << 30), Z0 = 1 << 30, Z1 = -(1 << 30);
        int plaetze = 0, beruehrt = 0, vorn = 0, vorverd = 0, vorteil = 0, hintn = 0, hintfrei = 0;
        static double stand[320], ebene[320];
        /* Endung selbst pruefen: sscanf zaehlt nur die Umwandlungen und nimmt auch
         * ROOM1000_PRI00.TIM an (gleich lang, s. test_pri_silhouette). */
        {   size_t L = strlen(e->d_name);
            if (L != 18 || strcmp(e->d_name + L - 4, ".PBM") != 0) continue; }
        if (sscanf(e->d_name, "ROOM%4x_PRI%2d.PBM", &raum, &cut) != 2) continue;
        n_cuts++;
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/%s", RE15_PORT_SRC_DIR, e->d_name);
        if (!p2_pbm(pfad, soll)) { printf("  FEHLER: %s\n", e->d_name); n_fehler++; continue; }
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X_PRI%02d.STAND", RE15_PORT_SRC_DIR, raum, cut);
        if (!p2_stand(pfad, stand, ebene)) { printf("  FEHLER: Standlinie fehlt: %s\n", pfad); n_fehler++; continue; }
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%u/ROOM%04X.RDT", RE15_PORT_SRC_DIR, raum >> 12, raum);
        roh = slurp(pfad, &sz);
        if (!roh || re15_rdt_parse(roh, sz, &rdt) < 0 || cut >= rdt.cut_count
            || re15_camera_build_view(&rdt.cuts[cut], &view) != 0) {
            printf("  FEHLER: ROOM%04X Cut %d: RDT/Kamera\n", raum, cut); n_fehler++; free(roh); continue;
        }
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK", RE15_PORT_SRC_DIR, raum);
        msk = slurp(pfad, &msz);
        memset(&pri, 0, sizeof pri);
        off = msk ? re15_pri_msk_section_offset(msk, msz, cut) : 0;
        n = off ? re15_pri_parse_section(msk, msz, off, &pri) : 0;
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X_PRI%02d.TIM", RE15_PORT_SRC_DIR, raum, cut);
        tb = slurp(pfad, &tsz);
        if (n <= 0 || !tb || re15_tim_parse(tb, (int) tsz, &tim) != 0 || tim.bpp != 8) {
            printf("  FEHLER: ROOM%04X Cut %d: Sektion/Atlas\n", raum, cut); n_fehler++;
            free(roh); free(msk); free(tb); continue;
        }
        p2_rastern(&pri, &tim);
        for (s = 0; s < rdt.sca_count; s++) {
            const re15_sca_entry_t *c = &rdt.sca[s];
            if ((int) c->x < X0) X0 = c->x; if ((int) c->z < Z0) Z0 = c->z;
            if ((int) c->x + (int) c->width > X1) X1 = c->x + c->width;
            if ((int) c->z + (int) c->density > Z1) Z1 = c->z + c->density;
        }
        re15_collision_reset_band();
        re15_collision_band_range(&rdt, &lo, &hi);
        for (b = lo; b <= hi; b++) {
            int gx, gz, hat = 0;
            for (s = 0; s < rdt.sca_count; s++) if ((rdt.sca[s].floor >> 4) == b) { hat = 1; break; }
            if (!hat) continue;
            re15_collision_set_band(b);
            for (gx = X0; gx <= X1; gx += 200)
                for (gz = Z0; gz <= Z1; gz += 200) {
                    int32_t x2 = gx, z2 = gz; double yfoot = -(double) b * 0x708;
                    double fsx, fsy, fvz, ksx, ksy, kvz, hw; int x0, x1, y0, y1, x, y, touch = 0;
                    long bv = 0, bvor = 0, bhin = 0, vv = 0, vh = 0;
                    re15_collision_constrain(&rdt, gx, gz, &x2, &z2);
                    if (x2 != gx || z2 != gz) continue;
                    if (!p2_proj(&view, gx, yfoot, gz, &fsx, &fsy, &fvz)) continue;
                    if (!p2_proj(&view, gx, yfoot - KOPF_HOCH, gz, &ksx, &ksy, &kvz)) continue;
                    plaetze++;
                    hw = HALB_BREIT * (double) view.fov_screen_dist / fvz;
                    x0 = (int) (fsx - hw); if (x0 < 0) x0 = 0;
                    x1 = (int) (fsx + hw); if (x1 > 320) x1 = 320;
                    y0 = (int) (ksy < fsy ? ksy : fsy); if (y0 < 0) y0 = 0;
                    y1 = (int) (ksy < fsy ? fsy : ksy); if (y1 > 240) y1 = 240;
                    if (x1 <= x0 || y1 <= y0) continue;
                    for (x = x0; x < x1 && !touch; x++) if (p2_col_hi[x] >= y0 && p2_col_lo[x] < y1) touch = 1;
                    if (!touch) continue;
                    beruehrt++;
                    (void) bv;
                    for (y = y0; y < y1; y++) {
                        double vz = p2_vz_senkrechte(&view, gx, gz, y, fvz);
                        for (x = x0; x < x1; x++) {
                            int occ; double ref;
                            if (!p2_deck[y][x]) continue;
                            occ = re15_pri_mask_occludes(p2_tief[y][x], (long) vz);
                            if (stand[x] < 0) continue;
                            /* Standplatz auf die Bodenebene der Spalte projiziert (s. p2_stand). */
                            ref = ((double) gx * view.rot[6] + ebene[x] * view.rot[7] + (double) gz * view.rot[8]) / 4096.0 + view.trans[2];
                            if (ref < stand[x] - 1)      { bvor++; vv += occ; }
                            else if (ref > stand[x] + 1) { bhin++; vh += occ; }
                        }
                    }
                    if (bvor) { double q = (double) vv / bvor; vorn++; if (q >= 0.95) vorverd++; else if (q > 0.05) vorteil++; }
                    if (bhin) { double q = (double) vh / bhin; hintn++; if (q < 0.95) hintfrei++; }
                }
        }
        re15_collision_reset_band();
        printf("  ROOM%04X %-3d %7d %7d | %5d %7d %7d | %5d %8d%s\n", raum, cut, plaetze, beruehrt,
               vorn, vorverd, vorteil, hintn, hintfrei, vorverd ? "   <<< VOR VERDECKT" : "");
        sum_vorverd += vorverd; sum_hintfrei += hintfrei; sum_vorn += vorn; sum_hintn += hintn;
        free(roh); free(msk); free(tb);
    }
    closedir(d);
    printf("  %d Cuts mit Ziel-Silhouette, %d Fehler; VOR %d Standplaetze (%d verdeckt), HINTER %d (%d frei)\n",
           n_cuts, n_fehler, sum_vorn, sum_vorverd, sum_hintn, sum_hintfrei);
    CHECK("Phase 2: es gibt Cuts mit Ziel-Silhouette und Standlinie", n_cuts > 0 && n_fehler == 0);
    CHECK("Phase 2: KEIN begehbarer Standplatz VOR der Standlinie wird zu >= 95 % verdeckt (VORverd == 0)",
          sum_vorverd == 0);
    return 0;
}

int main(void)
{
    static const unsigned STAGE_VON[7] = { 0, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000 };
    int  ges_geschnitten = 0, ges_orte = 0, ges_masken = 0;
    int  orig_geschnitten = 0;
    int  schlimmste[16][4];      /* raum, cut, maskentiefe, zahl */
    int  n_schlimm = 0;
    int  n_raeume = 0, n_cuts = 0, n_mitmaske = 0;
    int  o_masken = 0, o_nievoll = 0, u_masken = 0, u_nievoll = 0;
    unsigned raum;
    int st;

    printf("== Masken, die eine Figur waagerecht durchschneiden ==\n");
    /* ⛔ WO HABE ICH GESUCHT. Ein Haken, der 0 meldet, muss sagen koennen, ob er
     * nichts GEFUNDEN oder nichts GESUCHT hat - sonst sieht "0 Schnitte" aus wie ein
     * Erfolg. Genau diese Blindheit hat hier schon eine Messung wertlos gemacht. */
    {   char _pf[600]; size_t _p = 0; uint8_t *_t;
        snprintf(_pf, sizeof _pf, "%s/shared_assets/PSX/STAGE1/ROOM1000.RDT", RE15_PORT_SRC_DIR);
        printf("  Suchpfad-Probe: %s\n", _pf);
        _t = slurp(_pf, &_p);
        printf("  -> %s (%d Bytes)\n", _t ? "lesbar" : "NICHT LESBAR", (int) _p);
        if (_t) { re15_rdt_t _r; int _rc = re15_rdt_parse(_t, _p, &_r);
                  printf("  -> geparst rc=%d cuts=%d sca=%d\n", _rc,
                         _rc < 0 ? -1 : (int) _r.cut_count, _rc < 0 ? -1 : (int) _r.sca_count);
                  free(_t); } }

    for (st = 1; st <= 6; st++) {
        for (raum = STAGE_VON[st]; raum < STAGE_VON[st] + 0x1000; raum += 0x10) {
            char pfad[600];
            size_t sz = 0;
            uint8_t *roh;
            re15_rdt_t rdt;
            uint8_t *msk = NULL;  int msk_sz = 0;
            int ci;

            snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%d/ROOM%04X.RDT",
                     RE15_PORT_SRC_DIR, st, raum);
            roh = slurp(pfad, &sz);
            if (!roh) continue;
            if (re15_rdt_parse(roh, sz, &rdt) < 0) { free(roh); continue; }
            n_raeume++;

            snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK",
                     RE15_PORT_SRC_DIR, raum);
            { size_t ms = 0; msk = slurp(pfad, &ms); msk_sz = (int) ms; }

            for (ci = 0; ci < (int) rdt.cut_count; ci++) {
                re15_camera_view_t view;
                re15_pri_cut_t pri;
                int aus_original = 1, n = 0, i, gx, gz;
                int X0, X1, Z0, Z1, s;

                /* ⛔ 0 = ERFOLG. Als Wahrheitswert gelesen verwarf die Schleife jeden
                 * gebauten Blick - 103 Raeume geladen, 0 Kamerawinkel. Aufgefallen ist
                 * das nur, weil der Haken seine ABDECKUNG ausgibt. */
                if (re15_camera_build_view(&rdt.cuts[ci], &view) != 0) continue;
                n_cuts++;

                n = re15_pri_parse_section(roh, sz, rdt.cuts[ci].pri_offset, &pri);
                if (n == 0 && msk) {
                    uint32_t off = re15_pri_msk_section_offset(msk, (size_t) msk_sz, ci);
                    if (off) { n = re15_pri_parse_section(msk, (size_t) msk_sz, off, &pri);
                               aus_original = 0; }
                }
                if (n <= 0) continue;
                n_mitmaske++;
                ges_masken += n;

                /* Suchgitter = Aussenmasse der Kollisionsgeometrie (wie [poccscan]). */
                X0 = 1 << 30; X1 = -(1 << 30); Z0 = 1 << 30; Z1 = -(1 << 30);
                for (s = 0; s < (int) rdt.sca_count; s++) {
                    const re15_sca_entry_t *e = &rdt.sca[s];
                    if ((int) e->x < X0) X0 = e->x;
                    if ((int) e->z < Z0) Z0 = e->z;
                    if ((int) e->x + (int) e->width   > X1) X1 = e->x + e->width;
                    if ((int) e->z + (int) e->density > Z1) Z1 = e->z + e->density;
                }
                if (X1 <= X0 || Z1 <= Z0) continue;

                /* Je MASKE zaehlen, wie sie den Spieler an allen Standplaetzen
                 * trifft, an denen sie seinen Bildkasten beruehrt:
                 *   voll   = auch der Kopf liegt dahinter -> saubere Verdeckung
                 *   schnitt= Fuss dahinter, Kopf davor    -> waagerechter Schnitt
                 *   frei   = auch der Fuss liegt davor    -> zu Recht unverdeckt
                 * Der SCHNITT allein ist kein Fehler (die Original-Masken tun es
                 * oefter als unsere). Verdaechtig ist eine Maske, die NIE voll deckt:
                 * dann liegt sie zu FERN, und wer hinter ihr steht, sieht sie nie
                 * als Vordergrund - genau der Stuhl in ROOM1140. */
                {
                    static int voll[128], schnitt[128], frei[128];
                    memset(voll, 0, sizeof voll);
                    memset(schnitt, 0, sizeof schnitt);
                    memset(frei, 0, sizeof frei);
                    for (gx = X0; gx <= X1; gx += 400)
                        for (gz = Z0; gz <= Z1; gz += 400) {
                            int fsx, fsy, ksx, ksy, hw, oben, unten;
                            long fvz, kvz;
                            if (!re15_collision_on_floor(&rdt, gx, gz)) continue;
                            bild_von(&view, gx, 0, gz, &fsx, &fsy, &fvz);
                            if (fvz <= 64) continue;
                            bild_von(&view, gx, -KOPF_HOCH, gz, &ksx, &ksy, &kvz);
                            ges_orte++;
                            hw = (int) (HALB_BREIT * view.fov_screen_dist / fvz);
                            oben  = ksy < fsy ? ksy : fsy;
                            unten = ksy < fsy ? fsy : ksy;
                            for (i = 0; i < n && i < 128; i++) {
                                int dep = pri.masks[i].depth;
                                int mx = (int16_t) pri.masks[i].dstX;
                                int my = (int16_t) pri.masks[i].dstY;
                                int mw = pri.masks[i].width, mh = pri.masks[i].height;
                                if (mx + mw <= fsx - hw || mx >= fsx + hw) continue;
                                if (my + mh <= oben     || my >= unten)    continue;
                                /* Original-Regel (re15_pri.h, 2026-09-19): verdeckt gdw.
                                 * depth < (1023*vz)>>16 — strikt, gleicher Bucket = Figur obenauf. */
                                if (re15_pri_mask_occludes(dep, kvz))       voll[i]++;
                                else if (re15_pri_mask_occludes(dep, fvz)) { schnitt[i]++;
                                    if (aus_original) orig_geschnitten++; else ges_geschnitten++; }
                                else                      frei[i]++;
                            }
                        }
                    for (i = 0; i < n && i < 128; i++) {
                        int beruehrt = voll[i] + schnitt[i] + frei[i];
                        int quote;
                        if (beruehrt < 20) continue;      /* zu wenige Standplaetze */
                        /* ⛔ NICHT "schneidet ueberhaupt" und auch nicht "deckt nie
                         * voll" - beide Kriterien trennen NICHT (gemessen: die
                         * Original-Masken schneiden oefter als unsere, und "deckt nie
                         * voll" trifft 9,6 % der Originale gegen 2,8 % der unseren).
                         * Die Signatur des Stuhls ist der ANTEIL: er schneidet an
                         * fast jedem Standplatz, an dem er die Figur beruehrt, und
                         * deckt sie fast nie ganz. */
                        quote = 100 * schnitt[i] / beruehrt;
                        if (aus_original) { o_masken++; if (quote >= 80) o_nievoll++; }
                        else {
                            u_masken++;
                            if (quote >= 80) {
                                u_nievoll++;
                                if (n_schlimm < 12) {
                                    schlimmste[n_schlimm][0] = (int) raum;
                                    schlimmste[n_schlimm][1] = ci;
                                    schlimmste[n_schlimm][2] = pri.masks[i].depth;
                                    schlimmste[n_schlimm][3] = quote;
                                    n_schlimm++;
                                }
                            }
                        }
                    }
                }
            }
            free(msk);
            free(roh);
        }
    }

    printf("  Raeume geladen: %d, Kamerawinkel: %d, davon mit Masken: %d\n",
           n_raeume, n_cuts, n_mitmaske);
    printf("  Begehbare Standplaetze geprueft: %d\n", ges_orte);
    printf("  Masken insgesamt:                %d\n", ges_masken);
    printf("  Standplaetze mit SCHNITT durch eine NACHGEZEICHNETE Maske: %d\n",
           ges_geschnitten);
    printf("  ... durch eine ORIGINAL-Maske (Massstab, nicht unser Fehler): %d\n",
           orig_geschnitten);

    printf("\n  --- Masken, die an >= 80 %% der Standplaetze SCHNEIDEN ---\n");
    printf("  ORIGINAL      : %4d von %4d Masken (%.1f %%)\n",
           o_nievoll, o_masken, o_masken ? 100.0 * o_nievoll / o_masken : 0.0);
    printf("  NACHGEZEICHNET: %4d von %4d Masken (%.1f %%)\n",
           u_nievoll, u_masken, u_masken ? 100.0 * u_nievoll / u_masken : 0.0);
    {   int k;
        printf("\n  Nachgezeichnete Masken mit hohem Schnitt-Anteil:\n");
        for (k = 0; k < n_schlimm; k++)
            printf("     ROOM%04X Cut %-2d  Tiefe %3d (verdeckt ab Kamera-z %5d)  %3d %%%% Schnitt-Anteil\n",
                   (unsigned) schlimmste[k][0], schlimmste[k][1], schlimmste[k][2],
                   (int) re15_pri_mask_camera_z(schlimmste[k][2]), schlimmste[k][3]);
    }

    CHECK("es wurden ueberhaupt Standplaetze und Masken gefunden",
          ges_orte > 0 && ges_masken > 0);
    /* ⛔ ABDECKUNG ZUERST: ohne diese Zahlen sieht "0 Schnitte" aus wie ein Erfolg.
     * Genau daran ist der Haken beim ersten Lauf gescheitert (103 Raeume geladen, 0
     * Kamerawinkel - re15_camera_build_view liefert 0 bei ERFOLG, ich hatte es als
     * Wahrheitswert gelesen). */
    CHECK("alle Stages erreicht (>= 100 Raeume, >= 300 Winkel mit Masken)",
          n_raeume >= 100 && n_mitmaske >= 300);
    /* ⛔ SCHRANKE ALS RATE, NICHT ALS SUMME (berichtigt 2026-09-07).
     * Die alte Schranke war die ABSOLUTE Zahl (6518 am Stand 2026-09-06). Sie fiel,
     * sobald der Nutzer neue Freistellungen lieferte: mit 27 Masken mehr (2043 ->
     * 2780) stieg die Summe auf 8913, obwohl JE MASKE nichts schlechter wurde
     * (3,190 -> 3,206 Standplaetze je Maske). Eine Schranke, die beim blossen
     * Hinzufuegen guter Masken bricht, misst die falsche Groesse - sie haette mich
     * dazu gebracht, entweder die Zahl blind hochzusetzen oder Masken wegzulassen.
     *
     * Der Massstab ist das ORIGINAL: die Kuenstler erzeugen 12761 Schnitte mit 5931
     * Masken = 2,152 je Maske. Wir liegen bei 3,206 je Maske, also beim 1,49-fachen.
     * Gehalten wird dieses Verhaeltnis (mit Luft bis 1,70) - so darf die Maskenarbeit
     * beliebig wachsen, aber nicht schlechter werden.
     * Die absolute Zahl wird weiter AUSGEGEBEN, damit ein Sprung sichtbar bleibt. */
    {
        double r_uns  = u_masken ? (double) ges_geschnitten / u_masken : 0.0;
        double r_orig = o_masken ? (double) orig_geschnitten / o_masken : 0.0;
        char t[220];
        printf("\n  Schnitte JE MASKE: nachgezeichnet %.3f (%d/%d), "
               "Original %.3f (%d/%d) -> Verhaeltnis %.2f\n",
               r_uns, ges_geschnitten, u_masken, r_orig, orig_geschnitten, o_masken,
               r_orig > 0 ? r_uns / r_orig : 0.0);
        snprintf(t, sizeof t,
                 "Schnitte je nachgezeichneter Maske hoechstens das 1,70-fache des "
                 "Originals (gemessen %.2f)", r_orig > 0 ? r_uns / r_orig : 0.0);
        CHECK(t, r_orig > 0.0 && r_uns <= 1.70 * r_orig);
    }
    p2_phase();
    return g_fail;
}
