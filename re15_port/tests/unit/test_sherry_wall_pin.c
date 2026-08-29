/* test_sherry_wall_pin.c — PIN (Nutzer-Report 2026-08-29 "Sherry laeuft nach der Cutscene
 * wo sie verschwindet immer noch rum"):
 *
 * Sherry = Typ 0x4B (Leon-Route), spawnt in ROOM11B0 GEPARKT bei (-30000,0,-30000)
 * (main00 Sce_em_set @RDT 0x10A8); die Verschwinde-Sequenz parkt sie erneut dorthin
 * (Pos_set @0x13AC). Trifft der Park sie mitten in einem Plc_dest-Walk (sub05: mode 4
 * -> (-5913,-23975)), laeuft der Executor weiter — was sie im ORIGINAL draussen haelt,
 * ist die SCA-Wand-Klemme im UNKONDITIONALEN Root-Tail NACH dem State-Dispatch:
 *   0x4B-Root 0x8011e22c: `jalr` @0x8011e2d0 (Tabelle @0x801218d8[+0x4]), dann Tail
 *   `lw v0,120(a0); ori a2,4; lhu a1,6(v0); jal 0x8003b0a4; addiu a0,a0,52`
 *   @0x8011e30c-1c — in ALLEN sechs NPC-Roots (@0x8011c694/@0x8011cc64/@0x8011d270/
 *   @0x8011d804/@0x8011dd50/@0x8011e318, Callsite-Scan).
 * Sie bleibt an ROOM11B0s Suedwand-Band (SCA @0xDE4: x=-28800 z=-28720 w=32500 d=2000)
 * ausserhalb haengen — unsichtbar. Der Port klemmte bis 2026-08-29 nur das
 * Eskorte-Blatt; im state-4-Executor lief sie ungeklemmt durch die Wand zurueck in
 * den Raum. Dieser Pin faehrt genau die Park-mitten-im-Walk-Lage ueber die ECHTE
 * SCD-Kette (Work_set(2,idx) + Plc_dest(mode 4)) und verlangt: nie noerdlich der
 * Suedwand. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

int main(void)
{
    char rp[600];
    size_t rawsz = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11B0.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *rawbuf = slurp(rp, &rawsz);
    if (!rawbuf) { printf("SKIP: %s fehlt\n", rp); return 77; }
    /* direkt in g_room_rdt parsen — der Klemm-Helfer re15_npc_wall_clamp liest die
     * Raum-Kollision aus dieser Globalen (Muster: probe_1010_kriecher.c:201-203). */
    extern re15_rdt_t g_room_rdt; extern int g_room_rdt_ok;
    if (re15_rdt_parse(rawbuf, rawsz, &g_room_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_rdt_t rdt = g_room_rdt;

    printf("=== Sherry-Wand-Pin ROOM11B0 (0x4B, Park -30000/-30000, Walk-Ziel -5913/-23975) ===\n");

    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_pauseflags_clear();
    g_current_room_id = 0x11B0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10000; pl->y = 0; pl->z = -20000; pl->rot_y = 0; pl->floor = 0;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_msg_load_room_block(rdt.messages, rdt.messages_size);
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); }

    /* Sherry finden (Typ 0x4B, geparkt) */
    int ss = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x4B) { ss = s; break; }
    if (ss < 0) { printf("FAIL: kein 0x4B in ROOM11B0 gespawnt\n"); return 1; }
    re15_actor_t *e = &g_actors[ss];
    printf("  [spawn] slot=%d pos=(%ld,%ld) state=%d\n", ss, (long)e->x, (long)e->z, (int)e->state);
    CHECK("Sherry spawnt geparkt suedlich der Wand (z <= -28720)", e->z <= -28720);

    /* Die Park-mitten-im-Walk-Lage ueber die echte SCD-Kette: Work_set(2, idx) +
     * Plc_dest(reg 0, mode 4 = WALK, flag 0, Ziel -5913/-23975) + Evt_end.
     * (Encodings: Work_set 0x2E [op,kind,idx] -> slot=idx+1; Plc_dest 0x40 8 Byte
     * [op,reg,mode,flag,x LE,z LE] — scd_vm.c op_work_set/op_plc_dest.) */
    {
        static uint8_t prog[16];
        int i = 0;
        prog[i++] = 0x2E; prog[i++] = 0x02; prog[i++] = (uint8_t)(ss - 1);
        prog[i++] = 0x40; prog[i++] = 0x00; prog[i++] = 0x04; prog[i++] = 0x00;
        int16_t tx = -5913, tz = -23975;
        prog[i++] = (uint8_t)(tx & 0xff); prog[i++] = (uint8_t)((tx >> 8) & 0xff);
        prog[i++] = (uint8_t)(tz & 0xff); prog[i++] = (uint8_t)((tz >> 8) & 0xff);
        prog[i++] = 0x01; prog[i++] = 0x00;                   /* Evt_end */
        scd_thread_start(5, prog);
    }

    int32_t max_z = e->z;
    int moved = 0;
    int32_t start_x = e->x, start_z = e->z;
    for (int f = 0; f < 600; f++) {
        scd_vm_tick();
        re15_enemy_ai_run_all(1);
        if (e->z > max_z) max_z = e->z;
        if (e->x != start_x || e->z != start_z) moved = 1;
    }
    printf("  [walk] end=(%ld,%ld) max_z=%ld state=%d sub=%d\n",
           (long)e->x, (long)e->z, (long)max_z, (int)e->state, (int)e->sub_state_1);
    CHECK("der Walk ist scharf (sie bewegt sich ueberhaupt)", moved);
    CHECK("Root-Tail-Klemme haelt sie SUEDLICH der Wand (max_z < -26720, Original: haengt an "
          "SCA @0xDE4)", max_z < -26720);
    CHECK("sie erreicht das Raum-Innere NICHT (z-Ziel -23975 nie erreicht)", e->z < -26720);

    free(rawbuf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
