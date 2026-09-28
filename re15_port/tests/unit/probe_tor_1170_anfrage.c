/* probe_tor_1170_anfrage.c — Riegel: das Tor bekommt seine Tuersequenz in BEIDEN Varianten.
 *
 * Befund der Pruefung (2026-09-28): die Anfrage war an den Szenario-Wiedereintritt gebunden,
 * und dessen Schranke in aot_fire_door ist variantenblind (0x1000|dest<<4 = 0x1170 gegen
 * ROOM1171) - Elzas Tor haette nie gespielt. Jetzt setzt der Selbst-Tuer-Zweig die Anfrage.
 *
 * Gemessen an den echten RDT (main00 installiert die Tore unbedingt):
 *   ROOM1170 Slot 0 @Datei 0x1206 -> Variante 0, Slot 6 @0x135A -> Variante 1,
 *   ROOM1171 Slot 0 @0x11DC       -> Variante 0, Slot 5 @0x12F0 -> Variante 1,
 *   ROOM1170 Slot 3 (Intro-Uebergabe, Rechteck 0, Band 0) -> keine Anfrage.
 * Ohne Plattform-Laeufer muss re15_door_seq_ausfuehren die Anfrage verfallen lassen (0). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_door_seq.h"

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

static void raum(unsigned room_id, int slot_v0, int slot_v1)
{
    char path[512];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, room_id);
    size_t n = 0;
    uint8_t *buf = read_file(path, &n);
    CHECK(buf != NULL, "%s nicht lesbar", path);
    if (!buf) return;
    re15_rdt_t rdt;
    CHECK(re15_rdt_parse(buf, (int)n, &rdt) == 0, "%s: RDT-Parse", path);

    const int slots[2] = { slot_v0, slot_v1 };
    for (int k = 0; k < 2; k++) {
        scd_vm_init();
        g_current_room_id = room_id;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 2550; pl->y = -7200; pl->z = 15250; pl->hp = 100;
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) scd_vm_tick();

        const re15_aot_t *a = &g_aot.slots[slots[k]];
        int var = -1;
        int archiv = re15_door_seq_zuordnen(room_id, a->x, a->z, a->half_w, a->half_h,
                                            g_aot.door_params[slots[k]].band, &var);
        printf("ROOM%04X Slot %d: Typ %d Mitte (%ld,%ld) halb (%ld,%ld) Band %d -> Archiv %d Variante %d\n",
               room_id, slots[k], a->type, (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h,
               g_aot.door_params[slots[k]].band, archiv, var);
        CHECK(a->type == RE15_AOT_TYPE_DOOR, "ROOM%04X Slot %d ist keine Tuer", room_id, slots[k]);
        CHECK(archiv == RE15_DOOR_ARCHIV_TOR1170 && var == k,
              "ROOM%04X Slot %d: Zuordnung Archiv %d Variante %d, erwartet Tor Variante %d",
              room_id, slots[k], archiv, var, k);

        memset(&g_door_seq_anfrage, 0, sizeof g_door_seq_anfrage);
        re15_aot_fire_slot(slots[k]);   /* derselbe aot_fire_door wie Scan und Aot_on */
        CHECK(g_door_seq_anfrage.aktiv == 1 && g_door_seq_anfrage.archiv == RE15_DOOR_ARCHIV_TOR1170
              && g_door_seq_anfrage.variante == k,
              "ROOM%04X Slot %d: Anfrage aktiv=%d archiv=%d variante=%d, erwartet 1/%d/%d",
              room_id, slots[k], g_door_seq_anfrage.aktiv, g_door_seq_anfrage.archiv,
              g_door_seq_anfrage.variante, RE15_DOOR_ARCHIV_TOR1170, k);
        /* ohne Laeufer: verfaellt, gibt 0 */
        CHECK(re15_door_seq_ausfuehren() == 0 && g_door_seq_anfrage.aktiv == 0,
              "ROOM%04X Slot %d: Anfrage verfaellt ohne Laeufer nicht", room_id, slots[k]);
    }

    if (room_id == 0x1170) {
        /* Intro-Uebergabe Slot 3: Rechteck 0, Band 0 -> keine Sequenz */
        scd_vm_init();
        g_current_room_id = room_id;
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) scd_vm_tick();
        memset(&g_door_seq_anfrage, 0, sizeof g_door_seq_anfrage);
        if (g_aot.slots[3].type == RE15_AOT_TYPE_DOOR) re15_aot_fire_slot(3);
        CHECK(g_door_seq_anfrage.aktiv == 0, "ROOM1170 Slot 3 (Intro) darf keine Sequenz anfragen");
    }
    free(buf);
}

int main(void)
{
    raum(0x1170, 0, 6);
    raum(0x1171, 0, 5);
    printf(fails ? "probe_tor_1170_anfrage: %d FEHLER\n" : "probe_tor_1170_anfrage: OK\n", fails);
    return fails ? 1 : 0;
}
