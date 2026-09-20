/* probe_r17_weste_karte.c — MESS-WERKZEUG (kein Pin).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand, in dem die R.P.D.-Weste
 * ANGELEGT ist: Flag(3,0x75) = 1 (der save-persistente Traeger, scd_room_setup.c:131)
 * und Raum ROOM1190. Damit laesst sich die echte exe ueber
 *   RE15_CARD_PATH=<datei> RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * in genau den Zustand laden, den der Nutzer beschreibt ("Weste einsammeln,
 * speichern, laden -> Weste weg"), OHNE den Raum vorher von Hand zu spielen.
 *
 * Aufruf: probe_r17_weste_karte <kartendatei> [<raum-hex>] [<waffen-id>] [<wund-level>]
 *
 * Die beiden letzten Argumente dienen dem ZENSUS der uebrigen Ausruestungs-/
 * Modellzustaende (Waffen-Equip, Blut-Decals): sie legen eine Waffe in die Hand und
 * stempeln alle 8 Wund-Panels auf <wund-level>, damit ein CONTINUE-Lauf zeigt, ob
 * auch diese Zustaende nach dem Laden am Modell ankommen.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_damage.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern scd_vm_t g_scd;

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1190u;

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();

    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 105;
    pl->x = 19100; pl->y = 0; pl->z = -21250; pl->rot_y = 0;

    /* Weste AN: das save-persistente Flag + der (nicht serialisierte) Modell-Index. */
    re15_game_flag_set(3, 0x75, 1);
    g_scd.work_vars[0x10] = 1;

    /* ZENSUS-Zusatz: Waffe + Wunden (optional). */
    int wpn = (argc > 3) ? atoi(argv[3]) : 0;
    int wlv = (argc > 4) ? atoi(argv[4]) : 0;
    if (wpn > 0) {
        extern void re15_player_set_equipped_weapon(int id);
        re15_player_set_equipped_weapon(wpn);
    }
    if (wlv > 0) {
        /* je Aufruf ein Level-Schritt (acc-Schwelle 0x78 @0x80037f14), Clamp bei 2. */
        for (int p = 0; p < 8; p++)
            for (int k = 0; k < wlv; k++) re15_wound_add(p, 200);
    }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  WESTE") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, Flag(3,0x75)=%d, work_vars[0x10]=%d, "
           "waffe=%d, wund_level[0]=%d\n",
           path, (unsigned)sd.room, re15_game_flag_get(3, 0x75) ? 1 : 0,
           (int)g_scd.work_vars[0x10], (int)sd.weapon_id, re15_wound_level(0));

    /* Gegenprobe: zurueckgelesen muss das Flag wieder da sein. */
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(3, 0x75, 0);
    g_scd.work_vars[0x10] = 0;
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Rueckgelesen: Raum 0x%04X, Flag(3,0x75)=%d, work_vars[0x10]=%d\n",
           (unsigned)rr, re15_game_flag_get(3, 0x75) ? 1 : 0, (int)g_scd.work_vars[0x10]);
    return 0;
}
