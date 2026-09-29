/* probe_r33_speichern_karte.c — MESS-WERKZEUG fuer integration_r33_speichern (kein eigener Test).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand (Platz 0) in Irons' Buero ROOM1150, Leon
 * VOR DEM TELEFON: Lage (-22689,0,-19693), Blick 1864 — die Lage aus dem Spielstand des Nutzers
 * (analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr, Platz 0), an der ein
 * VIERECK im echten Spiel den Telefon-AOT (Slot 3, Event 6 -> sub06 -> Message_on(1)) ausloest
 * (gemessen, Dossier analysis/befunde_runde33/speichern_memory_card.md §5).
 *
 * Aufruf: probe_r33_speichern_karte <kartendatei> [karte]
 *   karte   legt die Memory Card (Item 0x21, 1 Stueck) in Inventarplatz 0.
 * Ausgabe: die zurueckgelesenen Werte (Raum, Platz 0).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_savepoint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    int karte = (argc > 2 && strcmp(argv[2], "karte") == 0);

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();

    g_current_room_id = 0x1150;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22689; pl->y = 0; pl->z = -19693; pl->rot_y = 1864;
    if (karte) { g_inv.slots[0].id = RE15_SAVEPOINT_CARD_ITEM; g_inv.slots[0].qty = 1; }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    printf("Karte %s: Platz 0, Raum 0x%04X, Inventarplatz 0 = Item 0x%02X x%u\n",
           path, (unsigned)back.room, (unsigned)back.inv[0].id, (unsigned)back.inv[0].qty);
    if (back.room != 0x1150) return 1;
    if (karte && back.inv[0].id != RE15_SAVEPOINT_CARD_ITEM) return 1;
    return 0;
}
