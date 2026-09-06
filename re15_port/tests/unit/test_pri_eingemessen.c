/* Vom Nutzer EINGEMESSENE Verdeckungspunkte — die Maske muss dort wirklich verdecken.
 *
 * ⛔ WOZU: eine nachgezeichnete Vordergrundmaske ist erst dann richtig, wenn sie die
 * Figur an der Stelle, an der der Nutzer sie gemeldet hat, auch WIRKLICH verdeckt.
 * "Die Flaeche ist jetzt breiter" reicht nicht — das war am 2026-09-04 schon einmal
 * die Antwort auf denselben Befund, und sie hat nichts gebracht, weil die TIEFE zu
 * fern blieb (damals an grow=3 gemessen: "deckt 37/38 ab, verdeckt aber 0/38").
 *
 * Die Punkte kommen aus befund.log: der Nutzer stellt sich im Spiel an die Stelle und
 * drueckt F9; das Log schreibt Raum, Kamerawinkel, Weltlage und die Kamera-Tiefen von
 * Fuss/Huefte/Kopf. Sie sind damit Messwerte wie eine Disassembly-Adresse und gehoeren
 * genauso gegen stilles Verrutschen geschuetzt.
 *
 * Geprueft wird auf dem ECHTEN Ladeweg der Engine (re15_rdt_parse,
 * re15_camera_build_view, re15_pri_msk_section_offset, re15_pri_parse_section) — nicht
 * an einem Nachbau, der schon einmal vier Fehler nicht gesehen hat.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Koerpermasse wie in der Sonde RE15_POCC und im Befund-Log: Fusspunkt bis Kopf 1500,
 * halbe Schulterbreite 450. */
#define KOPF_HOCH   1500
#define HALB_BREIT   450

typedef struct {
    unsigned room;
    int      cut;
    int32_t  wx, wz;      /* Weltlage aus befund.log */
    int      x0, x1;      /* Bildspalten, in denen verdeckt werden MUSS */
    const char *woher;
} messpunkt_t;

/* ---- ROOM1130 Cut 3, eingemessen 2026-09-06 --------------------------------------
 * befund.log, MARKE 1, Bild F506:
 *   F506  R1130 C3  pos=(-3818, 0, 3732) rot=-264  vz= 6398/6166/5935
 *         scr=(119,169) kasten=x102..136,y111..172 masken=80
 * Nutzer: "wenn Leon rennt, sieht man noch seinen Arm durchblitzen, weil nicht genug
 * Flaeche abgedeckt war". Am Abzug nachgemessen (Schwelle 40 gegen den Hintergrund):
 * 820 Figurpunkte, davon 687 ungedeckt, darunter die Spalten 76..81 — das Messer, das
 * ueber die Marmorwand gezeichnet wurde.
 *
 * Ursache war NICHT die Flaeche, sondern die TIEFE: die Spaltenregel las die seitliche
 * Kante des Pfeilers als Bodenkontakt und bekam fuer die Spalten 67..89 Tiefen von 94
 * bis 1023 (67..73 trafen den Boden gar nicht mehr). Verdeckt wird nur mit einer Tiefe
 * kleiner als Kopf-vz/64 = 92,7. Behoben mit "bodenkante": [94,115] in auswahl.json.
 *
 * Geprueft wird die ganze Breite der Maske (67..115), nicht nur die gemeldeten
 * Spalten: der Befund gilt fuer jede Spalte, die der Koerper beruehrt. */
static const messpunkt_t MESS[] = {
    { 0x1130, 3, -3818, 3732, 67, 115,
      "ROOM1130 C3 F9-Marke 1 (F506): Wandpfeiler muss Leon verdecken" },
};

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

static long vz_von(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z)
{
    return ((long) x * v->rot[6] + (long) y * v->rot[7] + (long) z * v->rot[8]) / 4096
           + v->trans[2];
}

int main(void)
{
    unsigned i;
    int geprueft = 0;

    printf("== Vom Nutzer eingemessene Verdeckungspunkte ==\n");

    for (i = 0; i < sizeof MESS / sizeof MESS[0]; i++) {
        const messpunkt_t *m = &MESS[i];
        char pfad[600], t[320];
        size_t sz = 0, ms = 0;
        uint8_t *roh, *msk;
        re15_rdt_t rdt;
        re15_camera_view_t view;
        re15_pri_cut_t pri;
        long vz_kopf, vz_fuss;
        int n, j, sx, hw, x0, x1, sy_fuss = 0, sy_kopf = 0;
        int gedeckt[320], beruehrt[320], sp, offen = 0, erste = -1, letzte = -1;
        int n_beruehrt = 0;

        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%u/ROOM%04X.RDT",
                 RE15_PORT_SRC_DIR, m->room >> 12, m->room);
        roh = slurp(pfad, &sz);
        if (!roh) { printf("  FAIL: %s nicht lesbar\n", pfad); g_fail = 1; continue; }
        if (re15_rdt_parse(roh, sz, &rdt) < 0 || m->cut >= (int) rdt.cut_count) {
            printf("  FAIL: ROOM%04X Cut %d nicht vorhanden\n", m->room, m->cut);
            g_fail = 1; free(roh); continue;
        }
        /* ⛔ 0 = ERFOLG. Als Wahrheitswert gelesen verwirft das jeden Blick. */
        if (re15_camera_build_view(&rdt.cuts[m->cut], &view) != 0) {
            printf("  FAIL: ROOM%04X Cut %d: kein Kamerasatz\n", m->room, m->cut);
            g_fail = 1; free(roh); continue;
        }

        n = re15_pri_parse_section(roh, sz, rdt.cuts[m->cut].pri_offset, &pri);
        msk = NULL;
        if (n == 0) {
            snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK",
                     RE15_PORT_SRC_DIR, m->room);
            msk = slurp(pfad, &ms);
            if (msk) {
                uint32_t off = re15_pri_msk_section_offset(msk, ms, m->cut);
                if (off) n = re15_pri_parse_section(msk, ms, off, &pri);
            }
        }
        if (n <= 0) {
            printf("  FAIL: ROOM%04X Cut %d hat keine Masken\n", m->room, m->cut);
            g_fail = 1; free(roh); free(msk); continue;
        }

        vz_fuss = vz_von(&view, m->wx, 0, m->wz);
        vz_kopf = vz_von(&view, m->wx, -KOPF_HOCH, m->wz);
        if (vz_kopf <= 64 || vz_fuss <= 64) {
            printf("  FAIL: ROOM%04X Cut %d: Figur hinter der Kamera\n", m->room, m->cut);
            g_fail = 1; free(roh); free(msk); continue;
        }
        {   long vx = ((long) m->wx * view.rot[0] + (long) m->wz * view.rot[2]) / 4096
                      + view.trans[0];
            long vy_f = ((long) m->wx * view.rot[3] + (long) m->wz * view.rot[5]) / 4096
                        + view.trans[1];
            long vy_k = ((long) m->wx * view.rot[3] + (long) (-KOPF_HOCH) * view.rot[4]
                         + (long) m->wz * view.rot[5]) / 4096 + view.trans[1];
            sx = 160 + (int) (vx * view.fov_screen_dist / vz_fuss);
            hw = (int) ((long) HALB_BREIT * view.fov_screen_dist / vz_fuss);
            sy_fuss = 120 + (int) (vy_f * view.fov_screen_dist / vz_fuss);
            sy_kopf = 120 + (int) (vy_k * view.fov_screen_dist / vz_kopf);
        }

        /* Welche Spalten deckt eine WIRKSAME Maske ab? Wirksam heisst: ihre Tiefe*64
         * liegt vor dem KOPF — dem naechsten Punkt der Figur, weil die Kamera von oben
         * schaut (gemessen 469 Einheiten Unterschied Fuss/Kopf). */
        memset(gedeckt, 0, sizeof gedeckt);
        memset(beruehrt, 0, sizeof beruehrt);
        for (j = 0; j < pri.mask_count && j < pri.draw_count; j++) {
            const re15_pri_mask_t *mk = &pri.masks[j];
            int mx0 = (int16_t) mk->dstX, mx1 = mx0 + (int) mk->width;
            int my0 = (int16_t) mk->dstY, my1 = my0 + (int) mk->height, k;
            if (mx0 < 0) mx0 = 0;
            if (mx1 > 320) mx1 = 320;
            /* ⛔ NUR Rechtecke, die die Figur auch auf der HOEHE beruehren. Spalte 67
             * der Freistellung hat genau EINEN Bildpunkt, und der liegt in Zeile 0 —
             * ueber Leons Kopf. Ohne diese Bedingung meldete der Haken sie als Luecke
             * und haette mich dazu gebracht, an einer Stelle nachzubessern, an der
             * nichts falsch ist. */
            if (my1 <= sy_kopf || my0 > sy_fuss) continue;
            for (k = mx0; k < mx1; k++) {
                beruehrt[k] = 1;
                if (re15_pri_mask_camera_z(mk->depth) < vz_kopf) gedeckt[k] = 1;
            }
        }

        /* ⛔ NICHT auf den Koerperkasten einschraenken. Der Kasten ist sx +/- 450
         * um die Huefte; das MESSER, das der Nutzer gemeldet hat, lag bei x 76..81 und
         * damit weit ausserhalb (Kasten hier: x105..133). Haette der Haken nur den
         * Kasten geprueft, waere er auch VOR dem Fix gruen gewesen und haette den
         * gemeldeten Fall nie gesehen. Geprueft wird die angegebene Spannweite —
         * die ganze Breite der Maske. */
        x0 = m->x0;
        x1 = m->x1;
        (void) hw;
        for (sp = x0; sp <= x1; sp++) if (beruehrt[sp]) n_beruehrt++;
        for (sp = x0; sp <= x1; sp++) {
            if (!beruehrt[sp] || gedeckt[sp]) continue;
            offen++;
            if (erste < 0) erste = sp;
            letzte = sp;
        }
        snprintf(t, sizeof t,
                 "%s: Welt(%d,%d), Bild (%d,%d..%d), geprueft x%d..%d, davon %d mit "
                 "Maske auf Koerperhoehe, vz Fuss %ld Kopf %ld (Maske wirkt bis Tiefe "
                 "%ld); %d Spalten ohne wirksame Maske",
                 m->woher, (int) m->wx, (int) m->wz, sx, sy_kopf, sy_fuss, x0, x1,
                 n_beruehrt, vz_fuss, vz_kopf, (vz_kopf - 1) / 64, offen);
        if (offen) {
            char z[80];
            snprintf(z, sizeof z, " (x %d..%d)", erste, letzte);
            strncat(t, z, sizeof t - strlen(t) - 1);
        }
        CHECK(t, offen == 0);
        geprueft++;
        free(roh); free(msk);
    }

    CHECK("alle eingemessenen Punkte wurden wirklich geprueft",
          geprueft == (int) (sizeof MESS / sizeof MESS[0]));
    return g_fail;
}
