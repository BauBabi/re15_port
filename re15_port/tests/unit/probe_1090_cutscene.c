/* probe_1090_cutscene.c — MESSSONDE zum Nutzer-Befund (C)
 * "In der Cutscene laeuft/orientiert sich Leon falsch" (ROOM1090).
 *
 * Die Cutscene ist sub02 (ROOM1090.RDT sub-Tabelle @0x21B4: sub02 @0x2414). Ausgeloest wird
 * sie von sub01 @0x23F4 ueber `Cmp(work_vars[0x0A] == 13)` + `18 02` (GOSUB sub02) — der
 * Per-Frame-Kamera-Spiegel work_vars[0x0A] (FUN_80021bc0 @0x80021bfc) liefert die 13.
 *
 * Die Wegpunkte stehen roh in der RDT (Plc_dest = [0x40][reg][mode][flag][s16 x][s16 z]):
 *   @0x242C  40 00 06 3f 00 00 00 00   Plc_dest(0, mode 6 = EVENT-REACH, kein Schritt)
 *   @0x244A  40 00 05 20 3b f9 ad 06   Plc_dest(0, mode 5 RUN, dest = (-1733, 1709))
 *   @0x2454  40 00 05 20 5c ff da 00   Plc_dest(0, mode 5 RUN, dest = (-164, 218))
 *   @0x245E  40 00 05 20 92 02 2a f7   Plc_dest(0, mode 5 RUN, dest = (658, -2262))
 *   @0x247C  40 00 08 20 7e ff 3c f8   Plc_dest(0, mode 8,      dest = (-130, -1988))
 *
 * Gemessen wird: WO steht Leon waehrend sub02 wirklich, wohin laeuft er, und liegen die
 * Ziel-Wegpunkte ueberhaupt im Raum (Vergleich gegen die Kamera-/RVD-Geometrie des Raums)?
 * Ohne diesen Vergleich ist "laeuft falsch" nicht messbar.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);

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
    const char *base = RE15_ASSET_PSX_DIR;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id); }

    /* --- M0: die Raum-Geometrie (RVD-Zonen + Tuer-AOTs) als Massstab fuer "im Raum" --- */
    int32_t minx = 0x7fffffff, maxx = -0x7fffffff, minz = 0x7fffffff, maxz = -0x7fffffff;
    int nz = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        nz++;
        if (a->x - a->half_w < minx) minx = a->x - a->half_w;
        if (a->x + a->half_w > maxx) maxx = a->x + a->half_w;
        if (a->z - a->half_h < minz) minz = a->z - a->half_h;
        if (a->z + a->half_h > maxz) maxz = a->z + a->half_h;
    }
    printf("[M0] ROOM1090: %d aktive AOTs, Ausdehnung x[%ld..%ld] z[%ld..%ld]\n",
           nz, (long)minx, (long)maxx, (long)minz, (long)maxz);
    printf("[M0] Spieler-Startpunkt der Sonde: (%ld,%ld)\n", (long)pl->x, (long)pl->z);

    /* --- M1: die Plc_dest-Wegpunkte aus den RDT-Rohbytes --- */
    static const unsigned wp[] = { 0x242C, 0x244A, 0x2454, 0x245E, 0x247C };
    printf("[M1] Plc_dest-Wegpunkte von sub02 (roh aus der RDT):\n");
    for (unsigned k = 0; k < sizeof wp / sizeof wp[0]; k++) {
        unsigned o = wp[k];
        int16_t x = (int16_t)(raw[o+4] | (raw[o+5] << 8));
        int16_t z = (int16_t)(raw[o+6] | (raw[o+7] << 8));
        printf("      @0x%04X op=%02x reg=%u mode=%u flag=0x%02x dest=(%6d,%6d) %s\n",
               o, raw[o], raw[o+1], raw[o+2], raw[o+3], x, z,
               (x >= minx && x <= maxx && z >= minz && z <= maxz) ? "IM Raum" : "AUSSERHALB");
    }

    /* --- M2: sub02 als echten SCD-Thread laufen lassen und Leon protokollieren --- */
    scd_thread_start(3, rdt.sub_scd[2]);
    printf("[M2] Bild  x       z      yaw  motion state sub1 walk mode dest=(%s)\n", "x,z");
    int32_t lx = pl->x, lz = pl->z;
    int32_t travelled = 0;
    for (int f = 0; f < 400; f++) {
        scd_vm_tick();
        re15_actors_anim_advance();
        re15_actor_step_all_walkers();
        re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
        travelled += (pl->x > lx ? pl->x - lx : lx - pl->x) + (pl->z > lz ? pl->z - lz : lz - pl->z);
        lx = pl->x; lz = pl->z;
        if (f < 12 || (f % 25) == 0)
            printf("     %4d %7ld %7ld %5d %6d %5d %4d %4d %4d  (%ld,%ld)\n",
                   f, (long)pl->x, (long)pl->z, (int)pl->rot_y, (int)pl->motion,
                   (int)pl->state, (int)pl->sub_state_1, (int)pl->walk_active,
                   (int)pl->walk_mode, (long)pl->walk_dest_x, (long)pl->walk_dest_z);
    }
    printf("[M2] Endstand (%ld,%ld) yaw=%d, Gesamtweg %ld\n",
           (long)pl->x, (long)pl->z, (int)pl->rot_y, (long)travelled);

    free(raw);
    return 0;
}
