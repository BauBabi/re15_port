/* probe_r34n_e_karte.c — MESS-WERKZEUG (kein add_test) fuer den Bild-Riegel
 * integration_r34n_e_dokumente_bild (Runde 34 Nacht, Spur E "Vier neue Dokumente").
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in einem Dokument-Raum, damit die echte
 * exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 am LADE-Weg dort startet —
 * dem Weg, der nicht durch scd_room_reenter geht (Haken in platform/pc/main.c; Original: EIN
 * Raumlader FUN_800396fc, `jal 0x800396fc` @0x8001d5ac LOAD / @0x8001d988 Tuer).
 * Vorlage: probe_r30_irons_tisch_karte.c.
 *
 * Aufruf: probe_r34n_e_karte <kartendatei> <raum-hex> [genommen] [pos=x,z,rot] [cut=N]
 *   <raum-hex>  1050/1051, 1000/1001, 1020/1021, 1010/1011
 *   genommen    setzt das Genommen-Bit des Raum-Dokuments (9, 56 + Nr) im Spielstand
 *   pos=x,z,rot Spielerlage im Spielstand (Standard 0,0,0)
 *   cut=N       gespeicherter Kamera-Cut (re15_savedata_t.camera_cut, Standard 0)
 *   files=a,b,..  FILE-Liste des Spielstands (Dokument-Nummern, re15_files_add; Abnahme der Liste)
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_dokumente.h"
#include "re15_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3) { printf("FAIL: Aufruf <karte> <raum-hex> [genommen] [pos=x,z,rot] [cut=N]\n"); return 2; }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    int genommen = 0, px = 0, pz = 0, prot = 0, cut = 0;
    int files[24], nfiles = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "genommen") == 0) genommen = 1;
        else if (strncmp(argv[a], "pos=", 4) == 0 &&
                 sscanf(argv[a] + 4, "%d,%d,%d", &px, &pz, &prot) == 3) { }
        else if (strncmp(argv[a], "cut=", 4) == 0 && sscanf(argv[a] + 4, "%d", &cut) == 1) { }
        else if (strncmp(argv[a], "files=", 6) == 0) {
            const char *q = argv[a] + 6;
            while (*q && nfiles < 24) { files[nfiles++] = (int)strtol(q, (char **)&q, 10); if (*q == ',') q++; }
        }
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    int nr = re15_dokumente_nr((uint16_t)room);
    if (nr <= 0) { printf("FAIL: Raum 0x%04X hat kein Dokument\n", room); return 2; }
    int bit = 56 + nr;   /* Bits 57..60 = Dokument 1..4 (VERTRAG 1.1, re15_dokumente.h) */

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    g_scd.cam_id = (uint8_t)cut;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = (int16_t)prot;
    if (genommen) re15_game_flag_set(9, (uint8_t)bit, 1);
    re15_files_reset();
    for (int i = 0; i < nfiles; i++) re15_files_add(files[i]);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  DOKUMENT") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(9, (uint8_t)bit, 0);
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int f = re15_game_flag_get(9, (uint8_t)bit) ? 1 : 0;
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, Dokument %d, Flag(9,%d)=%d, Cut %d, "
           "Spieler (%d,%d) rot %d\n", path, (unsigned)rr, nr, bit, f, (int)back.camera_cut, px, pz, prot);
    if (rr != (uint16_t)room)        { printf("FAIL: Raum 0x%04X\n", rr); return 1; }
    if (f != genommen)               { printf("FAIL: Flag nach dem Ruecklesen\n"); return 1; }
    if ((int)back.camera_cut != cut) { printf("FAIL: Cut nach dem Ruecklesen\n"); return 1; }
    return 0;
}
