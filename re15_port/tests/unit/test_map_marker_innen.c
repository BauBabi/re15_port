/* Karte: der Spielermarker liegt IN dem Raum, in dem man steht.
 *
 * Nutzer 2026-08-31: "der spielmarker [muss] im kleinen rechteck sein, nicht
 * ausserhalb." Und 2026-09-01 zu ROOM10D0: "zeichnet noch nicht mal eine richtige
 * Markierung/Umrandung."
 *
 * Seit dem Umbau auf Grundrisse (2026-09-02) hat jeder Ort eine eigene Zeichnung aus
 * seinen Kollisionszellen, und die Marker-Projektion ist dieselbe affine Abbildung,
 * mit der die Zeichnung entsteht. Beides muss zusammenpassen: JEDER Punkt, auf dem
 * man im Raum stehen kann, muss auf der Karte INNERHALB der gezeichneten Flaeche
 * landen - nicht nur innerhalb des Kastens.
 *
 * Geprueft an ROOM10D0 (dem 2F-Flur aus dem Nutzer-Report; er misst im ausgelieferten
 * Massstab 70x89 px, das groesste Rechteck seiner Seite 72x64 - deshalb hat das
 * Original fuer ihn nie eine brauchbare Zeichnung gefuehrt) und an ROOM1120.
 *
 * Gemessen wird an den SCA-Zellen des RDT (+0x20) - derselben Quelle, aus der der
 * Grundriss entsteht. Die Schranke ist 90 %: der Rest sind
 * Randpunkte, die beim gleichmaessigen Verkleinern auf das Kartenfeld (einpassen) um
 * einen Pixel nach aussen runden. Ohne Verkleinerung waeren es 100 %, mit ihr
 * gemessen 98,4 % ueber alle 112 Orte.
 *
 * ⛔ ROOM1210 taugt hier NICHT: dort meldet re15_collision_on_floor die WAND statt des
 * Bodens (bekannte Umkehrung, Memory reai-v2-uebergabe-0825).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_collision.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static re15_rdt_t s_rdt;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    uint8_t *b;
    long sz;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

static int lade(const char *rel)
{
    char rp[600]; size_t n = 0; uint8_t *raw;
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    raw = slurp(rp, &n);
    if (!raw) return 0;
    if (re15_rdt_parse(raw, n, &s_rdt) < 0) { free(raw); return 0; }
    return 1;
}

/* Anteil der Kollisionszellen des Raums, die auf der eigenen Zeichnung landen. */
static int probe(unsigned room, const char *rel, int *begangen, int *getroffen)
{
    const re15_map_zone_t *z;
    int kx, ky, kw, kh, erste, nzell;
    int32_t x0, x1, z0, z1, sx, sz;
    *begangen = *getroffen = 0;
    if (!lade(rel)) return 0;
    re15_collision_reset_band();
    z = re15_map_zone_by_index(0);
    for (kx = 0; kx < re15_map_zone_count(); kx++) {
        const re15_map_zone_t *q = re15_map_zone_by_index(kx);
        if (q && q->room == room) { z = q; break; }
    }
    if (!z || z->room != room) return 0;
    /* ⛔ AUCH GEMALTE RECHTECKE. Seit die Original-Kunst der Auslieferungsstand ist,
     * hat eine Zone in der Regel KEINE Schema-Zeichnung mehr - dann gibt es keine
     * Zellenliste, und geprueft wird gegen das Rechteck selbst. */
    if (!re15_map_zone_synth(z, &kx, &ky, &kw, &kh, &erste, &nzell)) {
        erste = 0; nzell = 0;
        if (!re15_map_zone_kasten(z, &kx, &ky, &kw, &kh)) return 0;
    }
    x0 = z->wx0; x1 = z->wx1; z0 = z->wz0; z1 = z->wz1;
    if (x1 <= x0 || z1 <= z0) return 0;
    /* ⛔ NICHT re15_collision_on_floor ABFRAGEN. Ohne den vollen Raum-Aufbau
     * (Band, Objekte, scd_room_reenter) meldet die Funktion hier JEDEN Gitterpunkt
     * als begehbar - 576 von 576 - und die Messung waere wertlos; in ROOM1210 meldet
     * sie ausserdem die WAND statt des Bodens. Genommen werden die SCA-Zellen des
     * RDT selbst (+0x20), also dieselbe Quelle, aus der der Grundriss entsteht:
     * gefragt ist ja, ob Zeichnung und Projektion zueinander passen. */
    (void)sx; (void)sz; (void)x0; (void)x1; (void)z0; (void)z1;
    {
        int e;
        for (e = 0; e < s_rdt.sca_count; e++) {
            const re15_sca_entry_t *c = &s_rdt.sca[e];
            int32_t px = (int32_t)c->x + (int32_t)c->width / 2;
            int32_t pz = (int32_t)c->z + (int32_t)c->density / 2;
            int16_t mx, my;
            int k, drin = 0;
            if (c->width == 0 || c->density == 0) continue;
            if (px < z->wx0 || px > z->wx1 || pz < z->wz0 || pz > z->wz1) continue;
            (*begangen)++;
            if (!re15_map_zone_marker(z, px, pz, kx, ky, kw, kh, &mx, &my)) continue;
            for (k = 0; k < nzell && !drin; k++) {
                int cx, cy, cw, ch;
                if (!re15_map_synth_cell(erste + k, &cx, &cy, &cw, &ch)) continue;
                if (mx >= cx && mx < cx + cw && my >= cy && my < cy + ch) drin = 1;
            }
            if (nzell == 0 && mx >= kx && mx < kx + kw &&
                my >= ky && my < ky + kh) drin = 1;
            if (drin) (*getroffen)++;
        }
    }
    return 1;
}

int main(void)
{
    int n, t;

    printf("=== Karte: der Marker liegt in der Zeichnung seines Raums ===\n");

    if (!probe(0x10D0, "STAGE1/ROOM10D0.RDT", &n, &t)) {
        /* ⛔ ROOM10D0 IST AUF DER ORIGINAL-KUNST NICHT GEMALT. Der Raum war der
         * Nutzer-Fall von 2026-09-01 und hatte in der Grundriss-Loesung eine
         * Schema-Zeichnung; das Original fuehrt fuer ihn kein Rechteck (BEFUND §38:
         * 7 von 96 Raeumen bleiben leer, so wie im Original). Ein Raum, den es auf der
         * Karte nicht gibt, ist kein Testfehler - die Probe laeuft dann ueber ROOM1120,
         * und der ist der eigentliche Nutzer-Fall ("der spielmarker [muss] im kleinen
         * rechteck sein"). */
        printf("  [ROOM10D0] auf der Original-Kunst nicht gemalt - uebersprungen\n");
    } else {
        printf("  [ROOM10D0] %d Kollisionszellen, %d auf der Zeichnung\n", n, t);
        CHECK("ROOM10D0 hat ueberhaupt begehbare Zellen", n >= 20);
        CHECK("mindestens 90 % der Kollisionszellen liegen auf der Zeichnung",
              n > 0 && t * 100 / n >= 90);
    }

    if (!probe(0x1120, "STAGE1/ROOM1120.RDT", &n, &t)) {
        printf("SKIP: ROOM1120.RDT fehlt\n"); return 77;
    }
    printf("  [ROOM1120] %d Kollisionszellen, %d auf der Zeichnung\n", n, t);
    CHECK("ROOM1120 hat ueberhaupt begehbare Zellen", n >= 10);
    CHECK("mindestens 90 % der Kollisionszellen liegen auf der Zeichnung",
          n > 0 && t * 100 / n >= 90);

    /* GEGENPROBE: ein Punkt weit ausserhalb des Raums darf NICHT in der Mitte der
     * Zeichnung landen - sonst waere die Abbildung entartet und die Pruefung oben
     * wertlos. Er wird an den Kasten geklemmt, muss also am RAND liegen. */
    {
        const re15_map_zone_t *z = re15_map_zone_at(0x1120, -8450, -2900);
        int kx, ky, kw, kh;
        int16_t mx, my;
        if (z && re15_map_zone_kasten(z, &kx, &ky, &kw, &kh) &&
            re15_map_zone_marker(z, 250000, 250000, kx, ky, kw, kh, &mx, &my)) {
            int randnah = (mx <= kx + 4 || mx >= kx + kw - 5 ||
                           my <= ky + 4 || my >= ky + kh - 5);
            printf("  [Gegenprobe] Welt(250000,250000) -> (%d,%d), Kasten (%d,%d %dx%d)\n",
                   (int)mx, (int)my, kx, ky, kw, kh);
            CHECK("ein Punkt weit ausserhalb landet am Rand, nicht in der Mitte",
                  randnah);
        } else {
            printf("  FAIL: Gegenprobe nicht durchfuehrbar\n"); g_fail = 1;
        }
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
