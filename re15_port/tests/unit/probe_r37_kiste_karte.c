/* probe_r37_kiste_karte.c — MESS-WERKZEUG (kein add_test), Runde 37 Spur M "kiste".
 * Dossier: analysis/befunde_runde37/M_kiste.md.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand (Platz 0) in einem Kisten-Raum, die Figur vor der
 * Item-Kiste, und fuellt die KISTE mit den Gegenstaenden, deren Namen im Port-Zeichensatz am weitesten nach
 * rechts reichen (Zensus im Dossier), dazu eine 2-Feld-Waffe und Mengen mit 1..3 Ziffern. Damit zeigt jeder
 * Gleit-Schritt der Liste (RE2 Zustaende 2/3) eine volle Zeile an Ober- und Unterkante des Fensters.
 *
 * Aufruf: probe_r37_kiste_karte <kartendatei> <raum-hex> <x> <z> <gier>
 *   Figur aus der Raum-Variante wie probe_r36_elza_karte: xxx1 = Elza (Zeichen 4), sonst Leon (0).
 *   Ring-Belegung (Platz: Id Menge):
 *     63: 0x15 H. Gun Bullets x100  (laeuft beim Hochblaettern von OBEN ein, drei Ziffern)
 *      0: 0x44 Minidisc Player w/ Disc (laengster Name der Namensbank)
 *      1: 0x0F Grenade Launcher x6, 2-Feld (Size 3 = RE15_BOX_KIND_WIDE)
 *      2: 0x43 Communications Card
 *      3: 0x1D Empty Grenade Shells x12 (Ziffern)
 *      4: 0x47 Blue Master Keycard
 *      5: 0x46 Red Master Keycard   (laeuft beim Runterblaettern von UNTEN ein)
 *      6: 0x0B Incendiary Grenade x3
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"
#include "re15_inventory.h"
#include "re15_itembox.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void box_set(int slot, uint8_t id, uint8_t qty, uint8_t kind)
{
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].id = id;
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].qty = qty;
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].flags = kind;
}

int main(int argc, char **argv)
{
    if (argc < 6) { printf("Aufruf: karte raum-hex x z gier\n"); return 2; }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    re15_inv_init();
    re15_itembox_init();
    g_current_room_id = (int)room;
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[3]); pl->z = atoi(argv[4]); pl->rot_y = (int16_t)atoi(argv[5]); pl->y = 0;

    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;           /* BROWNING HP          */
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 50;           /* H. GUN BULLETS       */
    box_set(63, 0x15, 100, 0);
    box_set(0, 0x44, 1, 0);
    box_set(1, 0x0F, 6, RE15_BOX_KIND_WIDE);
    box_set(2, 0x43, 1, 0);
    box_set(3, 0x1D, 12, 0);
    box_set(4, 0x47, 1, 0);
    box_set(5, 0x46, 1, 0);
    box_set(6, 0x0B, 3, 0);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "KISTE R37   ") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d (Elza=4), Lage (%d,%d), Kiste 0:%02x 1:%02x/%u 5:%02x 63:%02x\n",
           path, (unsigned)rr, (int)g_gameflow.character, (int)pl->x, (int)pl->z,
           g_itembox.slots[0].id, g_itembox.slots[1].id, g_itembox.slots[1].flags, g_itembox.slots[5].id,
           g_itembox.slots[63].id);
    return (rr == (uint16_t)room && g_itembox.slots[0].id == 0x44 && g_itembox.slots[63].id == 0x15) ? 0 : 1;
}
