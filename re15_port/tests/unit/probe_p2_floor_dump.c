/* Sonde p2 / pri-masken (2026-09-19): BEGEHBARE Standplaetze je Raum und Band — ueber den
 * SPIELER-Klemmpfad der Engine, nicht ueber den Containment-Scan.
 *
 * ⛔ WARUM (gemessen 2026-09-19): die Sonde probe_r16_pri_masken_audit nahm
 * re15_collision_on_floor() als "begehbar". Das ist FUN_8003b7f0, der Containment-Scan
 * ("die Zelle, die (x,z) ENTHAELT"), und die SCA-Zellen sind die HINDERNISSE (Waende,
 * Tische, Stuehle; re15_collision.c Kopf: "the player walks in the band-MATCH-FREE
 * complement"; Memory reai-v2-kollisionszellen-sind-waende: nur 2,9 % von 3727 Nutzer-
 * Standorten liegen in einer Zelle). Nachgezaehlt an ROOM1010/1140/10E0: 3350/12915/6636
 * von 3350/12915/6636 "Bodenpunkten" des alten Dumps liegen IN soliden Band-Zellen — das
 * Standplatz-Audit des Dossiers zaehlte also an den UNBEGEHBAREN Stellen.
 *
 * Hier gilt die Regel des Spielers selbst: ein Punkt ist begehbar, wenn
 * re15_collision_constrain (FUN_8003b0a4, Radius PR=450, Solid-Maske 1, Band gesetzt) ihn
 * NICHT verschiebt. Ausgabe wie die alte Sonde (ROOM / B / F / BN), damit
 * tools/maske/abnahme.floor_aus_dump sie liest.
 *
 * Aufruf: probe_p2_floor_dump [von hex] [bis hex] > floor.txt      (Default 1000..2000) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_collision.h"

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

int main(int argc, char **argv)
{
    unsigned raum, von = 0x1000, bis = 0x2000;
    if (argc > 2) { von = (unsigned) strtoul(argv[1], NULL, 16); bis = (unsigned) strtoul(argv[2], NULL, 16); }
    /* HERKUNFTSMARKE. ⛔ Nachtrag 2026-09-21 (Runde 19, Synthese §3 Schritt 4): diese Sonde
     * ist seit 2026-09-19 formatgleich vorhanden, wurde aber NICHT benutzt — raum.py nahm
     * weiter den Dump der alten Sonde (Containment-Scan = die HINDERNISSE), und weil beide
     * Dateien gleich aussehen, ist das monatelang niemandem aufgefallen. Jede
     * Standplatz-Zahl der Runden davor beschreibt deshalb unerreichbare Orte.
     * tools/maske/abnahme.floor_aus_dump verlangt diese Zeile jetzt und bricht ohne sie ab. */
    printf("# QUELLE klemmpfad re15_collision_constrain PR=450 solid=1\n");
    for (raum = von; raum < bis; raum++) {
        char pfad[600];
        size_t sz = 0;
        uint8_t *roh;
        re15_rdt_t rdt;
        int lo = 0, hi = 0, b, s;
        int X0 = 1 << 30, X1 = -(1 << 30), Z0 = 1 << 30, Z1 = -(1 << 30);
        snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum >> 12, raum);
        roh = slurp(pfad, &sz);
        if (!roh) continue;
        if (sz < 64 || re15_rdt_parse(roh, sz, &rdt) < 0 || rdt.sca_count <= 0) { free(roh); continue; }
        re15_collision_reset_band();
        re15_collision_band_range(&rdt, &lo, &hi);
        printf("ROOM %04X cuts=%d sca=%d bands=%d..%d msk=0\n", raum, rdt.cut_count, rdt.sca_count, lo, hi);
        for (s = 0; s < rdt.sca_count; s++) {
            const re15_sca_entry_t *e = &rdt.sca[s];
            if ((int) e->x < X0) X0 = e->x;
            if ((int) e->z < Z0) Z0 = e->z;
            if ((int) e->x + (int) e->width   > X1) X1 = e->x + e->width;
            if ((int) e->z + (int) e->density > Z1) Z1 = e->z + e->density;
        }
        for (b = lo; b <= hi; b++) {
            int gx, gz, n = 0, hat = 0;
            /* Nur Baender, die ueberhaupt Zellen tragen. */
            for (s = 0; s < rdt.sca_count; s++) if ((rdt.sca[s].floor >> 4) == b) { hat = 1; break; }
            if (!hat) continue;
            re15_collision_set_band(b);
            printf("B %d\n", b);
            for (gx = X0; gx <= X1; gx += 200)
                for (gz = Z0; gz <= Z1; gz += 200) {
                    int32_t x2 = gx, z2 = gz;
                    re15_collision_constrain(&rdt, gx, gz, &x2, &z2);
                    if (x2 == gx && z2 == gz) { printf("F %d %d\n", gx, gz); n++; }
                }
            printf("BN %d %d\n", b, n);
        }
        re15_collision_reset_band();
        free(roh);
    }
    return 0;
}
