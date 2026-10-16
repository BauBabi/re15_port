/* kiste2feld_r37.c — Runde 37 Spur R "kiste2feld". Belege: include/re15_kiste2feld_r37.h,
 * Dossier analysis/befunde_runde37/R_kiste2feld.md §2.4.
 *
 * Zuordnung Inventar-Zelle -> Haelfte des 80x30-Breitbilds einer 2-Feld-Waffe. Der Zeichner
 * (platform/pc/src/inv_render_pc.c inv_compose_cells) schreibt damit JEDE Zelle aus ihrer eigenen Haelfte —
 * der Zustand, den das Original mit LoadImage ueber Zelle 0/1 und zellenweisem MoveImage herstellt. */
#include "re15_kiste2feld_r37.h"
#include "re15_inventory.h"

int re15_kiste2feld_zelle(int zelle, uint8_t *id, int *spalte0)
{
    uint8_t art;
    if (zelle < 0 || zelle > 9) return 0;
    art = g_inv.slots[zelle].flags;
    if (g_inv.slots[zelle].id == 0 || (art != 1 && art != 2)) return 0;   /* Flag 1 = Kopf, 2 = Schwanz
                                                                            * (FUN_8004dc4c `ori v0,zero,0x1`
                                                                            * @0x8004de64 / `ori v0,zero,0x2`
                                                                            * @0x8004de70)                   */
    if (id) *id = g_inv.slots[zelle].id;
    if (spalte0) *spalte0 = (art == 2) ? RE15_KISTE2FELD_HAELFTE_W : 0;
    return 1;
}
