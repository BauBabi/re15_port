/* probe_r36_story_karte.c — MESS-WERKZEUG (kein add_test), Runde 36 Spur P "story".
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg (Boot-/CONTINUE-Block main.c) startet — Muster probe_r35_cut10f0_karte.c. Standorte
 * begehbar und Plaetze getroffen: gemessen mit `test_r36_story mess ...` (Dossier P_story.md, Tests).
 *
 * Staende (Gierung: 0 = +X, 1024 = -Z, 2048 = -X):
 *   roy       Leon ROOM1020 vor Roy (-4350,0,-8700) Blick -Z: 620 voraus im Platz Slot 5 @0x01DDA
 *   vor1020   Leon ROOM1010 vor der Tuer Slot 1 (@0x01010-Satz, Rechteck (4150,-4950,1000,2000) -> 1020)
 *             (3700,0,-3950) Blick +X
 *   zelle     Leon ROOM1210 im Gang vor der Zelle (-20600,0,-21650) Blick -X: 620 voraus im Platz Slot 9
 *   vor1210   Leon ROOM11E0 vor der Tuer Slot 3 (Rechteck (9700,4500,1500,2000) -> 1210), (9150,0,5500) Blick +X;
 *             (3,0x8B)=1 "Prisons"-Schloss offen (ROOM11E0 main00 @0x01576 Ck(3,0x8B)==0 legt sonst den
 *             Text-Platz an; Leon setzt es im Vorspann ROOM1240 sub02 @0x0055A `22 03 8b 01`)
 *   schuppen  Elza ROOM1091 vor dem Schuppen (0,-1800,250) Blick +X: 620 voraus im Viereck Slot 5
 *   vor1091   Elza ROOM1051 vor der Tuer Slot 5 (Rechteck (18100,-23900,3500,1000) -> 1091), (19850,0,-22300)
 *             Blick -Z
 *   zug_leon  Leon ROOM6030 (-18930,0,-22420): der Lade-Weg betritt mit Eintritts-Cut 0 -> sub00 @0x00D2C
 *             Gosub 7 -> sub08 (Annette vorne, msg 11..19) — die Reihenfolge "vorne zuerst" (Dossier P4)
 *   marvin    Elza ROOM30C1 (-17000,0,-9000) mit (3,0x39)=1: sub00 @0x00BEA -> sub03 (msg 1..8)
 *   annette   Leon ROOM6000 (2500,-3600,0) Band 2, (3,0x64)=0: sub00 @0x00F40 -> sub06 (msg 0..3)
 *   ada       Leon ROOM50B0 (-16000,0,-12300) im Auto-Platz Slot 3 @0x004FE ((3,0x62)=0) -> sub02 (msg 0..)
 *   vor6030   Leon ROOM6000 (1763,-3600,-9700) Blick -Z vor der Tuer Slot 3 @0x00DDA (Rechteck (693,-11104,2140,
 *             854), Ziel 6030 Cut 6 -> sub06 setzt John/Marvin/Sherry in den Wagen); (3,0x64)=1 Treffen mit
 *             Annette schon gesehen (sub00 @0x00F40 startet es sonst); 3 s vorwaerts -> Marke Slot 5 @0x00CC6
 *             -> sub02 (msg 0..8)
 *   vor6031   Elza ROOM6001 dito (Tuer Slot 3 @0x00DDA -> 6031 Cut 6 -> sub06 @0x00E42) -> sub02 (msg 0..2)
 * Zusatz:
 *   strom     (4,0xF3) = 1 (Generator, ROOM11F0 sub18 @0x016F6)
 *
 * Aufruf: probe_r36_story_karte <kartendatei> <stand> [strom]
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"
#include "re15_story_r36.h"

#include <stdio.h>
#include <string.h>

typedef struct { const char *name; unsigned raum; int elza; int32_t x, y, z; int16_t rot; } stand_t;

static const stand_t k_staende[] = {
    { "roy",      0x1020, 0,  -4350,     0,  -8700, 1024 },
    { "vor1020",  0x1010, 0,   3700,     0,  -3950,    0 },
    { "zelle",    0x1210, 0, -20600,     0, -21650, 2048 },
    { "vor1210",  0x11E0, 0,   9150,     0,   5500,    0 },
    { "schuppen", 0x1091, 1,      0, -1800,    250,    0 },
    { "vor1091",  0x1051, 1,  19850,     0, -22300, 1024 },
    { "zug_leon", 0x6030, 0, -18930,     0, -22420,    0 },
    { "marvin",   0x30C1, 1, -17000,     0,  -9000,    0 },
    { "annette",  0x6000, 0,   2500, -3600,      0,    0 },
    { "ada",      0x50B0, 0, -16000,     0, -12300,    0 },
    { "vor6030",  0x6000, 0,   1763, -3600,  -9700, 1024 },
    { "vor6031",  0x6001, 1,   1763, -3600,  -9700, 1024 },
};

int main(int argc, char **argv)
{
    if (argc < 3) { printf("Aufruf: probe_r36_story_karte <kartendatei> <stand> [strom]\n"); return 2; }
    const char *path = argv[1];
    const stand_t *s = NULL;
    for (size_t i = 0; i < sizeof k_staende / sizeof k_staende[0]; i++)
        if (strcmp(argv[2], k_staende[i].name) == 0) s = &k_staende[i];
    if (!s) { printf("FAIL: unbekannter Stand '%s'\n", argv[2]); return 2; }
    int strom = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "strom") == 0) strom = 1;
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_gameflow.character = (uint8_t)(s->elza ? 4 : 0);    /* DAT_800ACA5C: 0 Leon, 4 Elza (re15_savedata.h) */
    g_current_room_id = s->raum;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = s->x; pl->y = s->y; pl->z = s->z; pl->rot_y = s->rot;
    if (strom) re15_game_flag_set(RE15_S36_STROM_BANK, RE15_S36_STROM_BIT, 1);
    if (s->raum == 0x11E0) re15_game_flag_set(3, 0x8B, 1);
    if (s->raum == 0x30C1) re15_game_flag_set(3, 0x39, 1);
    if (strcmp(s->name, "vor6030") == 0) re15_game_flag_set(3, 0x64, 1);   /* Annette-Treffen 6000 gesehen */

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, s->elza ? "ELZA  STORY36" : "LEON  STORY36") != 0) {
        printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Stand %s Raum 0x%04X Figur %d (%d,%d,%d) Gierung %d, (4,0xF3)=%d\n", path, s->name,
           (unsigned)rr, (int)back.character, (int)back.player_x, (int)back.player_y, (int)back.player_z,
           (int)back.player_rot, re15_game_flag_get(RE15_S36_STROM_BANK, RE15_S36_STROM_BIT));
    return (rr == s->raum && (int)back.character == (s->elza ? 4 : 0) &&
            re15_game_flag_get(RE15_S36_STROM_BANK, RE15_S36_STROM_BIT) == strom) ? 0 : 1;
}
