/* test_r18_ada_eintritt_1050.c — RIEGEL zum Nutzer-Befund (2026-09-21):
 * "nach der feuer cutscene der raum danach [ROOM1050] direkt am Anfang spielt ihre
 *  [Adas] Animation doppelt ab."
 *
 * GEMESSENER IST-STAND VOR DEM FIX (echte exe, echter Uebergang ROOM1090 -> Tuer-Slot 0 ->
 * ROOM1050, Haken RE15_ANIM_TRACE / RE15_STATE_LOG):
 *
 *   Bild  Clip  Bild-im-Clip   Quelle
 *    1-5   0      0            Tuer-Blende: Pausemaske 0xFF000000 friert KI+Skript ein,
 *                              der Renderer laeuft -> Ada steht 5 Bilder in EM42-Clip 0
 *                              Bild 0 = MITTEN IM LAUFSCHRITT (Bein in der Luft)
 *    6-8   5      1..3         Plc_dest(mode 9) -> Sub-9-Turn (+0x94 = 5 @0x80051d3c)
 *    9-24  1      1..15,0      Sub-9 ausgerichtet -> Sub 6 (+0x5 = 6 @0x80051dac,
 *                              +0x6 = 0 @0x80051dbc) -> Phase 0 setzt Clip 1 @0x80051854
 *   25-34  2      1..10        Sub-6-Phase 2 -> Clip 2 = Ruhe-Schleife @0x800518c8
 *   35+    0      1..21        sub04 @0x0E04 Plc_dest(mode 5) -> sie laeuft WIRKLICH weg
 *
 * Bild 1-5 und Bild 35+ sind DERSELBE Clip 0. Der Raum blendet also auf, waehrend Ada
 * bereits im Weglauf-Schritt steht, sie schnappt in den Stand zurueck und spielt die
 * Weglauf-Animation danach noch einmal — die gemeldete Doppelung.
 *
 * ORIGINAL (selbst disassembliert; vollstaendige Belegkette im Kopf von
 * re15_enemy_spawn_action, scd_vm.c):
 *   - `Sce_em_set` FUN_800420a0 schreibt +0x94 NIE (kein Store auf 148(s0) im ganzen
 *     Handler 0x800420a0..0x8004262c; er nullt +0x4..+0x7 als Wort @0x800421e0 und
 *     +0x1c4 @0x8004216c).
 *   - Der Raumwechsel nullt am Entity nur +0x00: FUN_8001a4c0, 20 Slots ab 0x800acc2c,
 *     Stride 0x1f4, `sw zero,0(at)` @0x8001a4e8. +0x94 ueberlebt also.
 *   - Die Pose kommt aus dem State-0-INIT der Typ-Wurzel; Typ 0x42 -> Wurzel 0x8011cb70
 *     (Installer @0x8011e924/@0x8011e92c -> Dispatch 0x80072cb4 = 0x80072bac + 0x42*4),
 *     INIT @0x8011ccac, und dort steht verzweigungsfrei
 *         8011cd8c: ori v0,zero,0x2
 *         8011cd90: sb  v0,148(v1)      ; entity+0x94 = 2
 *     Dieselbe 2 tragen die INITs der uebrigen STAGE1-NPC-Wurzeln:
 *     0x8011c7c0 (0x40), 0x8011d39c (0x45), 0x8011d930 (0x47), 0x8011de7c (0x49),
 *     0x8011e454 (0x4b).
 *
 * DER RIEGEL faehrt genau den Eintritt: ROOM1050.RDT laden, Flag(3,0x6e) setzen (das Tor,
 * das ROOM1090s sub03 @0x24D6 stellt), scd_room_reenter -> main00+sub00 -> Sce_em_set
 * @0x0C8E + Evt_exec sub03, und prueft die Pose des Ada-Aktors VOR dem ersten KI-Tick —
 * also genau den Zustand, den die Tuer-Blende fuenf Bilder lang zeigt.
 *
 * Er DISKRIMINIERT: ohne den Fix liefert re15_enemy_spawn_action fuer Typ 0x42 den
 * Platzhalter 0 ("overlay decoders not yet RE'd"), also den Laufzyklus.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"

extern scd_vm_t g_scd;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

int main(void)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1050.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); free(raw); return 1; }

    scd_vm_init();                 /* setzt Flags/Aktoren/AOTs zurueck */
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = 0x1050;
    g_room_change.pending = 0;

    /* Das Tor, das ROOM1090s sub03 @0x24D6 stellt (`22 03 6e 01`) und ROOM1050s sub00
     * @0x0C8A prueft (`21 03 6e 01`). */
    re15_game_flag_set(3, 0x6e, 1);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Tuer-Zielpunkt aus ROOM1090 main00 Slot 0 (Door_aot_set @0x211A, Ziel-Block
     * (19850,0,-22300) yaw 3072, ROOM 0x05, CUT 0x06). */
    pl->x = 19850; pl->y = 0; pl->z = -22300; pl->rot_y = 3072;

    scd_room_reenter(&rdt, pl->x, pl->z, 6);

    const re15_actor_t *ada = &g_actors[1];
    printf("[M1] Slot 1: active=%d type=0x%02X grid=0x%02X motion=%d anim_frame=%d pos=(%ld,%ld)\n",
           (int)ada->active, (unsigned)ada->type, (unsigned)ada->grid_id,
           (int)ada->motion, (int)ada->anim_frame, (long)ada->x, (long)ada->z);

    int fails = 0;
    if (!ada->active || ada->type != 0x42) {
        printf("FAIL: ROOM1050 sub00 @0x0C8E hat Ada (Typ 0x42) nicht gespawnt "
               "(active=%d type=0x%02X)\n", (int)ada->active, (unsigned)ada->type);
        fails++;
    }
    /* DER KERN: die Pose, die waehrend der Tuer-Blende auf dem Schirm steht. */
    if (ada->motion != 2) {
        printf("FAIL: Ada-Spawn-Pose +0x94 = %d, erwartet 2 "
               "(State-0-INIT der Wurzel 0x8011cb70: @0x8011cd8c `ori v0,zero,0x2` / "
               "@0x8011cd90 `sb v0,148(v1)`). 0 = EM42-Clip 0 = Laufzyklus -> Ada steht "
               "beim Aufblenden mitten im Weglauf-Schritt.\n", (int)ada->motion);
        fails++;
    }
    /* Das Skript hat im SELBEN Tick `Evt_exec sub03` -> `Plc_dest(mode 9)` gefeuert; der
     * State-0-INIT laeuft deshalb NIE (Plc_dest @0x80041c14 `sb v0,4(a1)` mit v0 = 4).
     * Genau darum muss der Saat-Wert stimmen — nichts korrigiert ihn nachtraeglich. */
    if (ada->state != 4 || ada->sub_state_1 != 9) {
        printf("FAIL: erwartet State 4 / Sub 9 aus Plc_dest @RDT 0x0DAA, ist %d/%d\n",
               (int)ada->state, (int)ada->sub_state_1);
        fails++;
    }

    free(raw);
    if (fails) { printf("FAIL: %d Pruefung(en)\n", fails); return 1; }
    printf("OK: Ada betritt ROOM1050 in Clip 2 (Steh-Pose), nicht im Laufzyklus\n");
    return 0;
}
