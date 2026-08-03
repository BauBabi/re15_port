/* probe_door_1170.c — REGRESSION (ctest): Nutzer-Report 2026-08-03:
 * "ROOM1170: die Tuer am Treppenende rechts ist im Port begehbar; im Original ist sie
 *  verschlossen und sagt 'It's locked from the other side.'"
 *
 * Original-Mechanik (ROOM1170.RDT main00 @scd 0x0124, Datei 0x1318):
 *   Ifel_ck            06 00 1c 00
 *   Ck   (4,0xc3)==0   21 04 c3 00
 *   Aot_set slot5 sce1 2c 05 01 31 00 00 [rect -22740,-27610,2280,1210] 0c 00 ff ff 00 00
 *                       -> MESSAGE-AOT, msg-Index 0x0c = "It's locked from the other side."
 *   Else_ck            07 00 26 00
 *   Door_aot_set slot5 3b 05 02 31 ... dest stage0 room0x14 cut6  (-> ROOM1140)
 *
 * MESSUNG hier: welcher Typ landet im Port in AOT-Slot 5 (frisches Spiel, Flag 4/0xc3=0)?
 * Erwartet byte-true: TYPE_MESSAGE(5) mit event_id=0x0c. Report sagt: Tuer geht auf ->
 * vermutlich TYPE_DOOR(1). Zweite Variante: Flag gesetzt -> DOOR nach ROOM1140 erwartet. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

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

static const char *type_name(uint8_t t)
{
    switch (t) {
    case RE15_AOT_TYPE_GENERIC:    return "GENERIC";
    case RE15_AOT_TYPE_DOOR:       return "DOOR";
    case RE15_AOT_TYPE_ITEM:       return "ITEM";
    case RE15_AOT_TYPE_CAM_SWITCH: return "CAM_SWITCH";
    case RE15_AOT_TYPE_STAIR:      return "STAIR";
    case RE15_AOT_TYPE_MESSAGE:    return "MESSAGE";
    case RE15_AOT_TYPE_AUTO_EVENT: return "AUTO_EVENT";
    case RE15_AOT_TYPE_EXAMINE_WORKVAR: return "EXAMINE_WV";
    case RE15_AOT_TYPE_FLAG_CHG:   return "FLAG_CHG";
    case RE15_AOT_TYPE_WATER:      return "WATER";
    case RE15_AOT_TYPE_RAMP:       return "RAMP";
    case RE15_AOT_TYPE_NONE:       return "NONE";
    default:                       return "?";
    }
}

static void run_variant(const re15_rdt_t *rdt, int unlock_flag)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    if (unlock_flag) re15_game_flag_set(4, 0xc3, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 2664; pl->y = -7200; pl->z = -7336; pl->hp = 100;
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);
    for (int f = 0; f < 8; f++) scd_vm_tick();

    printf("== Variante Flag(4,0xc3)=%d ==\n", unlock_flag);
    printf("   flag_get(4,0xc3) = %d\n", re15_game_flag_get(4, 0xc3));
    for (int i = 0; i < 16; i++) {
        const re15_aot_t *s = &g_aot.slots[i];
        if (!s->active && s->type == 0) continue;
        printf("   slot %2d: active=%d type=%-10s ev=0x%02x band=%d rect c=(%ld,%ld) h=(%ld,%ld)",
               i, s->active, type_name(s->type), s->event_id, s->band,
               (long)s->x, (long)s->z, (long)s->half_w, (long)s->half_h);
        if (s->type == RE15_AOT_TYPE_DOOR) {
            const re15_aot_door_params_t *d = &g_aot.door_params[i];
            printf("  DOOR dest stage=%d room=0x%02x cut=%d spawn=(%ld,%ld,%ld)",
                   d->dest_stage, d->dest_room, d->target_cut,
                   (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z);
        }
        printf("\n");
    }

    /* byte-true Erwartungen (ROOM1170.RDT main00 @scd 0x0124-0x0164, Datei-verifiziert;
     * analysis/door_lock_1170.md §1/§3): Ifel_ck `Ck(4,0xc3)==0` -> Aot_set slot5 sce=1
     * msg 0x0c ("It's locked from the other side."), Else -> Door_aot_set slot5 sce=2
     * -> ROOM1140 cut 6 spawn(-7250,0,-1200). */
    const re15_aot_t *s5 = &g_aot.slots[5];
    if (!unlock_flag) {
        CHECK(s5->type == RE15_AOT_TYPE_MESSAGE && s5->event_id == 0x0c,
              "(locked) slot 5 = MESSAGE msg 0x0c (Aot_set @scd 0x012c `2c 05 01 31`, "
              "Payload `0c 00 ff ff`), got type=%s ev=0x%02x",
              type_name(s5->type), s5->event_id);
    } else {
        CHECK(s5->type == RE15_AOT_TYPE_DOOR,
              "(unlocked) slot 5 = DOOR (Door_aot_set @scd 0x0144 `3b 05 02 31`), got %s",
              type_name(s5->type));
        if (s5->type == RE15_AOT_TYPE_DOOR) {
            const re15_aot_door_params_t *d5 = &g_aot.door_params[5];
            CHECK(d5->dest_room == 0x14 && d5->target_cut == 6,
                  "(unlocked) Ziel ROOM1140 cut 6 (`00 14 06` @scd 0x0159-5b), got room=0x%02x cut=%d",
                  d5->dest_room, d5->target_cut);
            CHECK(d5->spawn_x == -7250 && d5->spawn_y == 0 && d5->spawn_z == -1200,
                  "(unlocked) Spawn (-7250,0,-1200) (`ae e3 00 00 50 fb` @scd 0x0152-57), "
                  "got (%ld,%ld,%ld)", (long)d5->spawn_x, (long)d5->spawn_y, (long)d5->spawn_z);
        }
    }
    /* Blockstruktur-Beweis: Tuer 4 (->ROOM1130), Tuer 6 (Rueckweg) und die 4 Treppen
     * liegen AUSSERHALB des Ifel/Else (unconditional) — sie muessen in BEIDEN Varianten
     * installiert sein (die alte "Dead-End"-Lesung war falsch, §8). */
    CHECK(g_aot.slots[4].type == RE15_AOT_TYPE_DOOR,
          "slot 4 (->ROOM1130) unconditional (Door_aot_set @scd 0x0104), got %s",
          type_name(g_aot.slots[4].type));
    CHECK(g_aot.slots[6].type == RE15_AOT_TYPE_DOOR,
          "slot 6 (Rueckweg) unconditional (`3b 06` @scd 0x0166 HINTER dem Else), got %s",
          type_name(g_aot.slots[6].type));
    for (int i = 7; i <= 10; i++)
        CHECK(g_aot.slots[i].type == RE15_AOT_TYPE_STAIR,
              "slot %d Treppe unconditional (@scd 0x0186ff), got %s",
              i, type_name(g_aot.slots[i].type));
}

int main(void)
{
    char path[512];
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    run_variant(&rdt, 0);   /* frisches Spiel: Original = MESSAGE 0x0c ("locked from the other side") */
    run_variant(&rdt, 1);   /* Flag gesetzt: Original = Tuer nach ROOM1140 cut 6 */
    free(data);
    if (fails) { printf("DOOR 1170: %d FAIL\n", fails); return 1; }
    printf("DOOR 1170: all checks passed\n");
    return 0;
}
