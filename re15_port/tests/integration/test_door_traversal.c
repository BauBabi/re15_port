/* ============================================================================
 *  Door-traversal integration — roadmap S1-1 / RL-6 (the payoff).
 *
 *  Walks ROOM1140 -> >=3 connected rooms via REAL doors, confirming each
 *  crossing's destination + spawn + cut against the room's own Door_aot_set data
 *  (the same bytes the RL-2 door graph is built from). For each room it:
 *    1. loads the real RDT (re15_rdt_parse) and runs the room script
 *       (scd_vm_init + scd_register_room_events + scd_room_reenter = main00+sub00),
 *       which installs that room's Door_aot_set AOTs;
 *    2. picks a cross-room DOOR AOT (preferring an unvisited destination);
 *    3. fires it the byte-true way — places the player 620 units before the door
 *       facing +X so the forward-620 action point (FUN_80042bac: fx = x+620*cos,
 *       fz = z-620*sin) lands in the door's rect, sets the player floor-band to the
 *       door band, raises g_aot_action_pressed (SQUARE press-edge), and runs
 *       re15_aot_scan until it queues g_room_change;
 *    4. verifies g_room_change (room_id/spawn/cut) == the door's door_params, then
 *       hands the player to that spawn and repeats.
 *
 *  Engine-only (re15_room_load is platform code): RDTs are read straight from
 *  RE15_ASSETS_PATH like the RL-3 room-probe sweep. ✔ >=3 real door crossings,
 *  each arrival = the door-graph destination.
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"        /* g_room_change, g_current_room_id */
#include "re15_collision.h"   /* re15_collision_set_band — the door band gate */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static const char *ASSETS;

static uint8_t *read_room(unsigned rid, size_t *out_size)
{
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", ASSETS, rid);
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz < 0x60) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f); *out_size = (size_t)sz; return buf;
}

/* Load + script a room so its Door_aot_set AOTs are installed. Keeps `buf` alive
 * (the rdt aliases it); caller frees the returned buffer after the fire. */
static uint8_t *enter_room(unsigned rid, re15_rdt_t *rdt,
                           int32_t px, int32_t py, int32_t pz, int16_t yaw)
{
    size_t sz = 0;
    uint8_t *buf = read_room(rid, &sz);
    if (!buf) return NULL;
    if (re15_rdt_parse(buf, sz, rdt) != 0) { free(buf); return NULL; }
    scd_vm_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = yaw; pl->hp = 100;
    g_current_room_id = rid;                       /* dest-id resolution uses the low nibble */
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, px, pz, /*entry_scenario=*/0);   /* runs main00 -> installs doors */
    for (int f = 0; f < 10; f++) scd_vm_tick();     /* let the install settle */
    return buf;
}

static unsigned dest_id_of(int slot, unsigned cur)
{
    return (((unsigned)g_aot.door_params[slot].dest_stage + 1u) << 12)
         | ((unsigned)g_aot.door_params[slot].dest_room << 4)
         | (cur & 0x000Fu);
}

int main(void)
{
    ASSETS = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== RL-6 door traversal — ROOM1140 -> connected rooms via real doors ===\n");

    unsigned visited[16]; int nvis = 0;
    unsigned cur = 0x1140;
    int32_t px = 0, py = 0, pz = 0; int16_t yaw = 0;   /* ROOM1140: origin is fine, the SCD spawns the roster */
    int crossings = 0, fail = 0;

    re15_rdt_t rdt;
    uint8_t *buf = enter_room(cur, &rdt, px, py, pz, yaw);
    if (!buf) { fprintf(stderr, "FAIL: cannot enter ROOM%04X\n", cur); return 1; }
    visited[nvis++] = cur;

    for (int step = 0; step < 10 && crossings < 5; step++) {
        /* pick a cross-room DOOR AOT: prefer one whose dest is unvisited, else any cross-room door. */
        int pick = -1, fallback = -1;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            if (!g_aot.slots[i].active || g_aot.slots[i].type != RE15_AOT_TYPE_DOOR) continue;
            unsigned d = dest_id_of(i, cur);
            if (d == cur) continue;                    /* same-room teleport, not a boundary */
            int seen = 0; for (int v = 0; v < nvis; v++) if (visited[v] == d) seen = 1;
            if (!seen) { pick = i; break; }
            if (fallback < 0) fallback = i;
        }
        if (pick < 0) pick = fallback;
        if (pick < 0) { printf("  ROOM%04X: no further cross-room door -> stop\n", cur); break; }

        unsigned want = dest_id_of(pick, cur);
        /* fire: forward-620 point must land in the door rect; band must match the door. */
        re15_collision_set_band((int)g_aot.door_params[pick].band);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->rot_y = 0;                                 /* facing +X -> forward = (+620, 0) */
        pl->x = g_aot.slots[pick].x - 620;
        pl->z = g_aot.slots[pick].z;
        g_room_change.pending = 0;
        for (int f = 0; f < 12 && !g_room_change.pending; f++) {
            g_aot_action_pressed = 1;                  /* SQUARE press-edge, re-armed each frame */
            re15_aot_scan(pl->x, pl->z, /*active_cut=*/0);
        }
        g_aot_action_pressed = 0;

        if (!g_room_change.pending) {
            fprintf(stderr, "FAIL: ROOM%04X door slot %d (-> ROOM%04X) did not fire\n", cur, pick, want);
            fail = 1; break;
        }
        /* verify the queued transition matches the door's own params (= the RL-2 graph). */
        if (g_room_change.room_id != want) {
            fprintf(stderr, "FAIL: ROOM%04X door queued ROOM%04X, door_params say ROOM%04X\n",
                    cur, g_room_change.room_id, want);
            fail = 1; break;
        }
        if (g_room_change.x != g_aot.door_params[pick].spawn_x ||
            g_room_change.z != g_aot.door_params[pick].spawn_z ||
            g_room_change.target_cut != (int)g_aot.door_params[pick].target_cut) {
            fprintf(stderr, "FAIL: ROOM%04X arrival spawn/cut != door_params\n", cur);
            fail = 1; break;
        }

        unsigned dest = g_room_change.room_id;
        int32_t sx = g_room_change.x, sy = g_room_change.y, sz = g_room_change.z;
        int16_t syaw = g_room_change.yaw_4096; int scut = g_room_change.target_cut;
        printf("  ROOM%04X --door%d--> ROOM%04X  spawn=(%d,%d,%d) yaw=%d cut=%d\n",
               cur, pick, dest, sx, sy, sz, syaw, scut);
        crossings++;

        /* hand the player to the destination room and continue the walk. */
        free(buf);
        buf = enter_room(dest, &rdt, sx, sy, sz, syaw);
        if (!buf) { fprintf(stderr, "FAIL: dest ROOM%04X failed to load\n", dest); fail = 1; break; }
        int seen = 0; for (int v = 0; v < nvis; v++) if (visited[v] == dest) seen = 1;
        if (!seen && nvis < 16) visited[nvis++] = dest;
        cur = dest;
    }
    if (buf) free(buf);

    printf("\n%d door crossings, %d distinct rooms visited: ", crossings, nvis);
    for (int v = 0; v < nvis; v++) printf("%04X ", visited[v]);
    printf("\n");
    if (fail || crossings < 3 || nvis < 4) { printf("DOOR TRAVERSAL: FAIL\n"); return 1; }
    printf("DOOR TRAVERSAL: crossed >=3 real doors, each arrival = door-graph destination\n");
    return 0;
}
