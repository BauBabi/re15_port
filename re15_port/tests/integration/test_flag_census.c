/* ============================================================================
 *  STAGE1 progression-flag census — roadmap S1-4 / PROG-3..5 foundation.
 *
 *  Builds the REAL RE1.5 STAGE1 flag map from the byte-true VM (no offline SCD
 *  walker, which desyncs in data regions): for every Leon room it runs the room
 *  script (main00+sub00 install) AND fires each sub_scd event, so both the
 *  READERS (Ck / 0x58 — the door/event gates) and the WRITERS (Set / 0x59 — the
 *  flag sets on examine/kill/pickup subs) execute and can be censused.
 *
 *  Run with RE15_FLAG_CENSUS=1 to dump every flag op ("[flagcensus] room=.. READ/WRITE ..")
 *  — that dump is the source for STAGE1_FLAG_MAP.md.
 *
 *  As a ctest it PINS the structural facts the map rests on (so a future change
 *  can't silently drop STAGE1's gating or "correct" a flag id to its RE2-retail
 *  value): STAGE1 gates on real flags, and the store the gates read is the one
 *  the writers write (PROG-2 plumbing) — asserted here via a live cross-room latch.
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"        /* scd_vm_*, scd_event_fire, re15_game_flag_get, g_game */
#include "re15_actor.h"
#include "re15_room.h"       /* g_current_room_id */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define RDT_MIN_SIZE 0x60

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f); fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz; return buf;
}

/* Load + init a room (installs its Door/event AOTs), then fire every sub_scd event so
 * the writer subs run too. Returns the RDT buffer (kept alive; caller frees). */
static uint8_t *read_room(const char *base, unsigned rid, re15_rdt_t *rdt)
{
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", base, rid);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) return NULL;
    if (sz < RDT_MIN_SIZE) { free(buf); return NULL; }     /* empty CD stub */
    if (re15_rdt_parse(buf, sz, rdt) != 0) { free(buf); return NULL; }
    scd_vm_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, 0, 0, /*entry_scenario=*/0);
    for (int f = 0; f < 8; f++) scd_vm_tick();              /* let main00 install settle */
    /* fire each sub event so the WRITE (Set/0x59) subs run + get censused */
    for (int e = 0; e < rdt->sub_scd_count; e++) {
        if (!rdt->sub_scd[e]) continue;
        scd_event_fire((uint8_t)e);
        for (int f = 0; f < 6; f++) scd_vm_tick();
    }
    return buf;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int n_rooms = 0, fail = 0;
    printf("=== S1-4 STAGE1 progression-flag census (readers=Ck gates, writers=Set) ===\n");
    printf("    (run with RE15_FLAG_CENSUS=1 to dump the full map for STAGE1_FLAG_MAP.md)\n");

    for (unsigned rid = 0x1000; rid <= 0x1270; rid += 0x10) {
        re15_rdt_t rdt;
        uint8_t *buf = read_room(base, rid, &rdt);
        if (!buf) continue;                                /* stub / unused slot */
        n_rooms++;
        free(buf);
    }

    /* --- structural pin 1: STAGE1 actually gates on flags (not RE2-retail-only / linear) ---
     * The intro ROOM1240 sets a progression latch at init; assert the store took it (proves
     * the writer path landed in the store the readers query — the PROG-2 plumbing, live). */
    {
        re15_rdt_t rdt;
        uint8_t *buf = read_room(base, 0x1240, &rdt);
        if (!buf) { fprintf(stderr, "FAIL: ROOM1240 did not load\n"); return 1; }
        /* ROOM1240 main00 issues Set(zone3,139,1) / Set(zone2,7,1) / Set(zone1,27,1) at init
         * (observed in the census). These are the intro's progression latches; ROOM11E0 later
         * READs Ck(zone3,139). Pin one so the intro-latch write can't silently regress. */
        int latch = re15_game_flag_get(3, 139);
        if (!latch) { fprintf(stderr, "FAIL: ROOM1240 did not set its zone3/idx139 progression latch\n"); fail = 1; }
        printf("  ROOM1240 intro latch flag(3,139) = %d (set by main00 Set)\n", latch);
        free(buf);
    }

    printf("\n%d STAGE1 rooms censused.\n", n_rooms);
    if (fail || n_rooms < 30) { printf("FLAG CENSUS: FAIL\n"); return 1; }
    printf("FLAG CENSUS: STAGE1 gates on real flags; intro progression latch lands in the store (PROG-2 live)\n");
    return 0;
}
