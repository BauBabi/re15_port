/* probe_r37_kiste2feld_karte.c — MESS-WERKZEUG (kein add_test), Runde 37 Spur R "kiste2feld".
 * Dossier: analysis/befunde_runde37/R_kiste2feld.md.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand (Platz 0) in einem Kisten-Raum, die Figur vor der
 * Item-Kiste (Lage wie probe_r37_kiste_karte der Spur M), und belegt
 *   die KISTE mit ALLEN 2-Feld-Waffen (Ids 0x0e..0x13 — RE1.5 `addiu -14; sltiu v0,v0,0x6` @0x8004dc78,
 *   Size 3 = RE15_BOX_KIND_WIDE, RE2 `sb 3 -> box.size` @0x80070ad0-dc), Ring-Platz 0..5, einstellige Mengen;
 *   Platz 6 = 0x0B x3 (schmal, Gegenprobe), Platz 63 = 0x15 x100 (schmal, laeuft von oben ein);
 *   das INVENTAR mit vier 2-Feld-Waffen, deren Kopfzelle auf JEDER Spalte des Symbol-Speichers liegt
 *   (RE1.5 Zellen-uv-Tabelle @0x80076244: Zelle k bei u (k%3)*40, v (k/3)*30):
 *     0/1 = 0x0F x6   (Kopf Zelle 0 = Speicher-Spalte 0, Schwanz Spalte 1 — nebeneinander)
 *     2/3 = 0x12 x4   (Kopf Zelle 2 = Spalte 2, Schwanz Zelle 3 = Spalte 0 der NAECHSTEN Speicherzeile)
 *     4   = 0x03 x15  (Browning, ausgeruestet)
 *     5/6 = 0x13 x30  (Kopf Zelle 5 = Spalte 2, Schwanz Zelle 6 = Spalte 0 der naechsten Zeile)
 *     7   = 0x15 x50
 *     8/9 = 0x0E x100 (Kopf Zelle 8 = Spalte 2, Schwanz Zelle 9 = Spalte 0 der naechsten Zeile)
 *
 * Aufruf: probe_r37_kiste2feld_karte <kartendatei> <raum-hex> <x> <z> <gier>
 *   Figur aus der Raum-Variante wie probe_r37_kiste_karte: xxx1 = Elza (Zeichen 4), sonst Leon (0).
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
#include "re15_damage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void box_set(int slot, uint8_t id, uint8_t qty, uint8_t kind)
{
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].id = id;
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].qty = qty;
    g_itembox.slots[slot & (RE15_BOX_SLOTS - 1)].flags = kind;
}

static void inv_set(int slot, uint8_t id, uint8_t qty, uint8_t kind)
{
    g_inv.slots[slot].id = id;
    g_inv.slots[slot].qty = qty;
    g_inv.slots[slot].flags = kind;
}

static void inv_breit(int kopf, uint8_t id, uint8_t qty)
{
    inv_set(kopf, id, qty, 1);       /* Kopf  (RE1.5 Anzeige-Flag 1, FUN_8004dc4c) */
    inv_set(kopf + 1, id, qty, 2);   /* Schwanz (Flag 2)                            */
}

int main(int argc, char **argv)
{
    int i;
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

    for (i = 0; i < RE15_INV_MAX_SLOTS; i++) inv_set(i, 0, 0, 0);
    inv_breit(0, 0x0F, 6);
    inv_breit(2, 0x12, 4);
    inv_set(4, 0x03, 15, 0);
    inv_breit(5, 0x13, 30);
    inv_set(7, 0x15, 50, 0);
    inv_breit(8, 0x0E, 100);
    re15_inv_set_equipped_slot(4);
    re15_player_set_equipped_weapon(0x03);   /* DAT_800aca5d = Browning, passend zu Platz 4 */

    for (i = 0; i < 6; i++) box_set(i, (uint8_t)(0x0e + i), (uint8_t)(i + 1), RE15_BOX_KIND_WIDE);
    box_set(6, 0x0B, 3, 0);
    box_set(63, 0x15, 100, 0);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "KISTE2F R37 ") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    re15_inv_init();
    re15_itembox_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d (Elza=4), Lage (%d,%d), Kiste", path, (unsigned)rr,
           (int)g_gameflow.character, (int)pl->x, (int)pl->z);
    for (i = 0; i < 7; i++) printf(" %d:%02x/%u", i, g_itembox.slots[i].id, g_itembox.slots[i].flags);
    printf(", Inventar");
    for (i = 0; i < 10; i++) printf(" %d:%02x/%u", i, g_inv.slots[i].id, g_inv.slots[i].flags);
    printf(", ausgeruestet %d\n", re15_inv_equipped_slot());
    {
        int ok = (rr == (uint16_t)room);
        for (i = 0; i < 6; i++)
            ok = ok && g_itembox.slots[i].id == 0x0e + i && g_itembox.slots[i].flags == RE15_BOX_KIND_WIDE;
        ok = ok && g_inv.slots[2].id == 0x12 && g_inv.slots[2].flags == 1 && g_inv.slots[3].flags == 2;
        ok = ok && g_inv.slots[8].id == 0x0E && g_inv.slots[8].flags == 1 && g_inv.slots[9].flags == 2;
        return ok ? 0 : 1;
    }
}
