/* probe_r41_kollision_abstand.c — Auswerter fuer integration_r41_kollision (Runde 41 Spur H, kein eigener Test).
 *
 * Liest ein RE15_STATE_LOG (platform/pc/main.c: je Bild "F<n> ... [slot t=<typ> st=.. ... @(x,z,r..)] hp=..") und
 * misst den Abstand der beiden NPC (Typ 0x40..0x4d, nicht geparkt (-30000,-30000)) ab Bild <ab>:
 *     MIN <d> F<bild>     kleinster Abstand
 *     KONTAKT <n>         Bilder mit Abstand < <summe> + 100 (sie kommen sich nahe = Messung nicht vakuant)
 *     UNTER <n>           Bilder mit Abstand < <summe> - 50 (ineinander)
 *     ENDE <d>            Abstand im letzten Bild
 *     PAARBILDER <n>      Bilder mit zwei NPC
 * Aufruf: probe_r41_kollision_abstand <state.txt> <ab> <summe>
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 4) { fprintf(stderr, "Aufruf: %s <state.txt> <ab> <summe>\n", argv[0]); return 2; }
    FILE *f = fopen(argv[1], "r");
    if (!f) { printf("FEHLER Datei %s\n", argv[1]); return 2; }
    const long ab = atol(argv[2]);
    const double summe = atof(argv[3]);
    static char zeile[16384];
    double dmin = 1e18, dend = -1;
    long fmin = -1, kontakt = 0, unter = 0, paar = 0;
    while (fgets(zeile, sizeof zeile, f)) {
        long bild;
        if (zeile[0] != 'F' || sscanf(zeile + 1, "%ld", &bild) != 1) continue;
        double x[2], z[2];
        int n = 0;
        const char *p = zeile;
        while ((p = strstr(p, " t=")) != NULL && n < 2) {
            unsigned typ = 0;
            if (sscanf(p, " t=%x", &typ) != 1) { p += 3; continue; }
            const char *at = strstr(p, "@(");
            const char *zu = strchr(p, ']');
            if (!at || (zu && at > zu)) { p += 3; continue; }
            long ex, ez;
            if (typ >= 0x40 && typ <= 0x4d && sscanf(at, "@(%ld,%ld", &ex, &ez) == 2 && !(ex == -30000 && ez == -30000)) {
                x[n] = (double)ex; z[n] = (double)ez; n++;
            }
            p += 3;
        }
        if (n < 2 || bild < ab) continue;
        paar++;
        double d = sqrt((x[0] - x[1]) * (x[0] - x[1]) + (z[0] - z[1]) * (z[0] - z[1]));
        if (d < dmin) { dmin = d; fmin = bild; }
        if (d < summe + 100) kontakt++;
        if (d < summe - 50) unter++;
        dend = d;
    }
    fclose(f);
    printf("PAARBILDER %ld\nMIN %.0f F%ld\nKONTAKT %ld\nUNTER %ld\nENDE %.0f\n", paar, paar ? dmin : -1.0, fmin,
           kontakt, unter, dend);
    return 0;
}
