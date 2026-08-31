/* Spielt die ROOM1240-Bildfolge durch und protokolliert, WAS in jedem Abschnitt
 * sichtbar waere: Helligkeit der aktuellen und der ausblendenden Ebene.
 * Standzeiten aus dem Inventar-Dossier (Sleep-Werte des Treibers sub02). */
#include <stdio.h>
#include "re15_montage_fx.h"

/* Cut -> Standzeit in Frames (grob nach dem SCD: 160 fuer das Schwarz, dann
 * 180/90/80 je Bildgruppe). Fuer die Frage "wird Bild 7 sichtbar?" reicht das. */
static const int hold[9] = { 160, 200, 200, 110, 110, 200, 110, 200, 200 };

int main(void)
{
    re15_montage_fx_stock_set(0);
    re15_montage_fx_set_active(1);
    printf("Cut | Frame |  neu  alt | Modus  | sichtbar?\n");
    for (int cut = 0; cut < 9; cut++) {
        re15_montage_fx_on_cut(cut, cut > 0);
        int peak_new = 0, peak_prev = 0;
        for (int f = 0; f < hold[cut]; f++) {
            re15_montage_fx_tick();
            int ln = re15_montage_fx_level_new(), lp = re15_montage_fx_level_prev();
            if (ln > peak_new)  peak_new  = ln;
            if (lp > peak_prev) peak_prev = lp;
        }
        printf(" %d  | %5d | %4d %4d | %-6s | %s\n", cut, hold[cut],
               peak_new, peak_prev,
               re15_montage_fx_mode() == 0 ? "steht" :
               re15_montage_fx_mode() == 1 ? "wandert" :
               re15_montage_fx_mode() == 2 ? "zoom+" : "zoom-",
               peak_new > 0 ? "JA — dieses Bild wird gezeigt"
                            : "nein (bleibt dunkel)");
    }
    return 0;
}
