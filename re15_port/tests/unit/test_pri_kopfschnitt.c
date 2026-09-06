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
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_pri.h"

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
                                long schwelle = (long) pri.masks[i].depth * 64;
                                int mx = (int16_t) pri.masks[i].dstX;
                                int my = (int16_t) pri.masks[i].dstY;
                                int mw = pri.masks[i].width, mh = pri.masks[i].height;
                                if (mx + mw <= fsx - hw || mx >= fsx + hw) continue;
                                if (my + mh <= oben     || my >= unten)    continue;
                                if (kvz > schwelle)       voll[i]++;
                                else if (fvz > schwelle) { schnitt[i]++;
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
            printf("     ROOM%04X Cut %-2d  Tiefe %3d (Kamera-z %5d)  %3d %%%% Schnitt-Anteil\n",
                   (unsigned) schlimmste[k][0], schlimmste[k][1], schlimmste[k][2],
                   schlimmste[k][2] * 64, schlimmste[k][3]);
    }

    CHECK("es wurden ueberhaupt Standplaetze und Masken gefunden",
          ges_orte > 0 && ges_masken > 0);
    /* ⛔ ABDECKUNG ZUERST: ohne diese Zahlen sieht "0 Schnitte" aus wie ein Erfolg.
     * Genau daran ist der Haken beim ersten Lauf gescheitert (103 Raeume geladen, 0
     * Kamerawinkel - re15_camera_build_view liefert 0 bei ERFOLG, ich hatte es als
     * Wahrheitswert gelesen). */
    CHECK("alle Stages erreicht (>= 100 Raeume, >= 300 Winkel mit Masken)",
          n_raeume >= 100 && n_mitmaske >= 300);
    /* SCHRANKE GEGEN RUECKSCHRITT. Gemessen am Stand 2026-09-06: 7223 Standplaetze
     * werden von einer NACHGEZEICHNETEN Maske durchschnitten. Der Schnitt an sich ist
     * NICHT der Fehler - die Original-Masken erzeugen 12761, also fast doppelt so
     * viele. Die Zahl haelt nur fest, dass kuenftige Maskenarbeit ihn nicht vermehrt. */
    CHECK("Schnitte durch nachgezeichnete Masken nicht mehr als am Stand 2026-09-06 (7223)",
          ges_geschnitten <= 7223);
    return g_fail;
}
