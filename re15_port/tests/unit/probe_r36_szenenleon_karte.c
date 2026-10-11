/* probe_r36_szenenleon_karte.c — MESS-WERKZEUG (kein add_test) fuer integration_r36_szenenleon Lauf D
 * (Runde 36 Spur J, Nachbesserung 1, Dossier analysis/befunde_runde36/J_szenenleon.md N1.5).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in ROOM11B0 NACH Marvins Erweiterung (Muster
 * probe_r30_granate_karte.c), damit die echte exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg (Boot-/CONTINUE-Pfad in main.c, NICHT scd_room_reenter) in ROOM11B0 startet.
 * Flags wie nach der Szene: (4,0xF3) Strom, (3,0x43) 11C0 erledigt, (3,0x82) sub04 gesehen, (3,0x83) sub06 gesehen
 * (@0x01478), (4,0xCD) Container verschoben (@0x0147C), (9,84) Werkstatt-Tuer verschlossen (VERTRAG §1.1 Spur J).
 *
 * Aufruf: probe_r36_szenenleon_karte <kartendatei> <ost|west> [offen] [cut=N]
 *   ost   Leon (-20500,0,-16300) Blick West (2048) — oestlich der Trennwand, Zone 0->2 (RDT @0x0280)
 *   west  Leon (-23300,0,-16300) Blick Ost (0)     — westlich der Trennwand
 *   offen Gegenprobe ohne (9,84): Original, die Tuer steht offen (Bild 8/7)
 *   cut=N gespeicherter Kamera-Cut (re15_savedata camera_cut). Der CONTINUE-Weg setzt ihn NACH main00/sub00 wieder
 *         (main.c "FE-4 CONTINUE", s_resume_cut) — er ist der Cut, in dem der Spieler beim Speichern stand: nach der
 *         Szene an der Tuer Ost 2 (Tuer zu), vor der Szene / ohne (9,84) Ost 8 (Tuer offen, Leon sub00 @0x01188).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_szenenleon_r36.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    const char *seite = (argc > 2) ? argv[2] : "ost";
    int offen = 0, cut = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "offen") == 0) offen = 1;
        else if (strncmp(argv[a], "cut=", 4) == 0) cut = atoi(argv[a] + 4);
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    int west = (strcmp(seite, "west") == 0);
    if (!west && strcmp(seite, "ost") != 0) { printf("FAIL: Seite '%s' (ost|west)\n", seite); return 2; }

    scd_vm_init();
    g_scd.cam_id = (uint8_t)cut;          /* -> re15_savedata camera_cut (re15_savedata.c: out->camera_cut = cam_id) */
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)RE15_SZL_RAUM;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = west ? -23300 : -20500; pl->y = 0; pl->z = -16300; pl->rot_y = west ? 0 : 2048;

    re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(3, 0x43, 1); re15_game_flag_set(3, 0x82, 1);
    re15_game_flag_set(3, 0x83, 1); re15_game_flag_set(4, 0xCD, 1);
    if (!offen) re15_game_flag_set(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  11B0") != 0) { printf("FAIL: Karte %s nicht schreibbar\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, 0);
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int zu = re15_game_flag_get(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU) ? 1 : 0;
    printf("Karte %s: Raum 0x%04X, Leon %s (%ld,%ld), Flag(9,%d)=%d, Cut %u\n", path, (unsigned)rr, west ? "West" : "Ost",
           (long)pl->x, (long)pl->z, RE15_SZL_BIT_ZU, zu, (unsigned)back.camera_cut);
    if (rr != (uint16_t)RE15_SZL_RAUM || zu != !offen || back.camera_cut != (uint8_t)cut) { printf("FAIL: Ruecklesen\n"); return 1; }
    return 0;
}
