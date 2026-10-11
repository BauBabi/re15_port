/* probe_r36_inventar_karte.c — MESS-WERKZEUG fuer integration_r36_inventar (kein eigener Test).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand (Platz 0) in Irons' Buero ROOM1150, Leon VOR DEM
 * TELEFON — Lage/Blick wie probe_r33_speichern_karte (-22689,0,-19693 / 1864, gemessen Runde 33 §5).
 *
 * Aufruf: probe_r36_inventar_karte <kartendatei> <menge_memory_card>
 *   Inventar: Platz 0 = Memory Card x<menge> (bei 0: kein Kartenplatz), danach Browning HP x15
 *   (AUSGERUESTET, Waffe 3) und H. Gun Bullets x50. Mit der letzten Karte in Platz 0 muss der
 *   geschriebene Stand den Browning nach Platz 0 nachruecken und den Ausruest-Platz mitziehen
 *   (FUN_8004dadc @0x8004dbac-c8 / RE2 FUN_80069714).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_damage.h"
#include "re15_inventar_r36.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    int menge = (argc > 2) ? atoi(argv[2]) : 1;

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();

    g_current_room_id = 0x1150;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22689; pl->y = 0; pl->z = -19693; pl->rot_y = 1864;
    int p = 0;
    if (menge > 0) { g_inv.slots[p].id = RE15_R36_KARTE_ID; g_inv.slots[p].qty = (uint8_t)menge; p++; }
    g_inv.slots[p].id = 0x03; g_inv.slots[p].qty = 15; p++;     /* BROWNING HP */
    g_inv.slots[p].id = 0x15; g_inv.slots[p].qty = 50; p++;     /* H. GUN BULLETS */
    re15_player_set_equipped_weapon(0x03);                       /* 25c8 = Platz des Browning */

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    printf("Karte %s: Platz 0, Raum 0x%04X, Inventar %02x:%u %02x:%u %02x:%u, Ausruest-Platz %u, Waffe %u\n",
           path, (unsigned)back.room, back.inv[0].id, back.inv[0].qty, back.inv[1].id, back.inv[1].qty,
           back.inv[2].id, back.inv[2].qty, (unsigned)back.equipped_slot, (unsigned)back.weapon_id);
    if (back.room != 0x1150) return 1;
    if (menge > 0 && (back.inv[0].id != RE15_R36_KARTE_ID || back.inv[0].qty != (uint8_t)menge)) return 1;
    return 0;
}
