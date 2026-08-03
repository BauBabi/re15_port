/* probe_zreentry_10d0.c — REGRESSION (ctest, Szenario A+B): Nutzer-Report 2026-08-03:
 * "Der Liegend-Zombie in ROOM10D0 verhaelt sich komisch, wenn man den Raum einmal
 *  verlaesst und dann wieder rein kommt, ihn aber vorher noch nicht aktiviert hat."
 *
 * ORIGINAL (ROOM10D0.RDT sub00, byte-exakt, Datei-Offsets):
 *   +0x2e f0x123a  Ifel_ck -> +0x94 (Else)
 *   +0x32 f0x123e  Ck(4,247)==0            ; Cutscene-Gate (Set NUR in sub21 +0x07e0 f0x19ec)
 *   IF-Zweig (Erst-Betretung):
 *     +0x36 AOT18 Evt 0x15 (sub21-Cutscene-Zone)
 *     +0x4a Sce_em_set slot=0 type=0x40 beh=0x40 p7=0xFF  (MARVIN)
 *     +0x5e Sce_em_set slot=1 type=0x10 beh=0x0e p7=0xCD  (LIEGEND-Zombie, kill-flag 205)
 *   ELSE-Zweig (Ck(4,247)==1, nach der Cutscene):
 *     +0x98 Sce_em_set slot=0 type=0x10 beh=0x02 p7=0xCD  (DERSELBE Zombie, STEHEND,
 *            gleiche Pos (5878,0,25694) yaw=1256, gleicher kill-flag 205)
 *     + Sca_id/Sca_floor-Umbau
 *
 * Der Raumwechsel-Pfad des Ports (room_common.c re15_room_apply_pending):
 *   keep hp/status -> re15_actor_init -> player-motion-reset -> re15_enemy_reset ->
 *   player an Tuer-Spawn -> scd_room_reenter(rdt,x,z,0). g_game.flags PERSISTIEREN.
 *
 * SZENARIEN:
 *   A) 10D0 rein (fern bleiben) -> raus nach 10C0 (Tuer 15, next=(-6700,-2100)) ->
 *      wieder rein. Ck(4,247)==0 -> Erst-Zweig MUSS wieder laufen: Zombie liegt,
 *      schlaeft (grid=0 s1=0x12 s2=1 mo=0x2A fr=0), wacht bei dist<0xBB8 auf.
 *   B) wie A, aber vor dem Verlassen Set(4,247)=1 (byte-true sub21 +0x07e0) ->
 *      ELSE-Zweig: STEHENDER Zombie beh=0x02 in Aktor-Slot 1, KEIN Marvin.
 *      Wie verhaelt sich der Port-Zombie mit grid_id=0x02?
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"

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

/* EM010-Bank laden (Muster probe_zlyer_10d0; nach jedem re15_enemy_reset noetig) */
static void load_em10_bank(const char *base)
{
    static uint8_t s_em10[0x80000];
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "WARN: CDEMD0.EMS nicht lesbar\n"); return; }
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
        len <= sizeof s_em10) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_em10, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_em10, len, &eb->md1, &eb->skel,
                                         &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_em10, len,
                                  &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok = (re15_emd_parse_own_bank(s_em10, len,
                                  &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

/* Nachbau des raum-relevanten Kerns von re15_room_apply_pending (room_common.c:100-171):
 * Aktor-Wipe (hp/status behalten), Spieler-Motion-Reset, Enemy-Banks droppen,
 * Spieler an den Tuer-Spawn, scd_room_reenter (Flags persistieren). */
static void door_transition(const re15_rdt_t *rdt, unsigned room_id,
                            int32_t x, int32_t z)
{
    const int16_t  keep_hp     = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    const uint16_t keep_status = g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    re15_actor_init();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp           = keep_hp;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = keep_status;
    {
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0;
        p->motion = 0; p->anim_frame = 0; p->anim_flags = 0;
        p->sub_state_1 = 0; p->sub_state_2 = 0; p->sub_state_3 = 0;
        p->state = 1;
        g_scd.player_mode = 0;
        p->x = x; p->y = 0; p->z = z; p->rot_y = 2048;
    }
    re15_enemy_reset();
    g_current_room_id = room_id;
    scd_room_reenter(rdt, x, z, 0);
}

static void dump_flags(const char *tag)
{
    extern uint8_t re15_em_status_zone(void);
    printf("[flags %-14s] Ck(4,247)=%d Ck(3,50)=%d em-zone%d bit205=%d work[10]=%d\n",
           tag,
           re15_game_flag_get(4, 247), re15_game_flag_get(3, 50),
           re15_em_status_zone(), re15_game_flag_get(re15_em_status_zone(), 205),
           (int)g_scd.work_vars[10]);
}

static void dump_roster(const char *tag)
{
    printf("== Roster %s ==\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (a->active)
            printf("  slot=%d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u mo=%d fr=%u "
                   "emflag=0x%02x pos=(%ld,%ld,%ld) rot=%d\n",
                   s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2,
                   (int)a->motion, a->anim_frame, a->em_flag_id,
                   (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y);
    }
}

typedef struct {
    uint8_t st, s1, s2, s3, grid;
    int16_t motion; uint16_t frame; uint16_t aflags;
    uint32_t dist; int32_t x, z;
} zsnap_t;

static void ztake(const re15_actor_t *a, zsnap_t *s)
{
    s->st = a->state; s->s1 = a->sub_state_1; s->s2 = a->sub_state_2;
    s->s3 = a->sub_state_3;
    s->grid = a->grid_id; s->motion = a->motion; s->frame = a->anim_frame;
    s->aflags = a->anim_flags; s->dist = a->ai_dist; s->x = a->x; s->z = a->z;
}

static int zctrl_diff(const zsnap_t *a, const zsnap_t *b)
{
    return a->st != b->st || a->s1 != b->s1 || a->s2 != b->s2 || a->s3 != b->s3 ||
           a->grid != b->grid || a->motion != b->motion || a->aflags != b->aflags;
}

static void zdump(const char *tag, int tick, const zsnap_t *s)
{
    printf("%5d %-5s st=%u s1=0x%02x s2=%u s3=%u grid=0x%02x mo=%-3d fr=%-3u af=0x%04x "
           "dist=%-6u pos=(%ld,%ld)\n",
           tick, tag, s->st, s->s1, s->s2, s->s3, s->grid, (int)s->motion, s->frame,
           s->aflags, (unsigned)s->dist, (long)s->x, (long)s->z);
}

static re15_actor_t *find_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10)
            return &g_actors[s];
    return NULL;
}

static void tick_once(void)
{
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

static void run_phase(const char *tag, int nticks, int period)
{
    zsnap_t prev, now;
    memset(&prev, 0xFF, sizeof prev);
    re15_actor_t *z = find_zombie();
    for (int tick = 0; tick < nticks; tick++) {
        tick_once();
        if (!z) z = find_zombie();
        if (!z) continue;
        ztake(z, &now);
        if (zctrl_diff(&now, &prev)) { zdump(tag, tick, &now); prev = now; }
        else if (tick % period == 0)  zdump("  fr", tick, &now);
    }
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int scenario_b = (argc > 1 && argv[1][0] == 'B');
    char path[600];
    size_t sz_d0 = 0, sz_c0 = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    uint8_t *dat_d0 = read_file(path, &sz_d0);
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10C0.RDT", base);
    uint8_t *dat_c0 = read_file(path, &sz_c0);
    if (!dat_d0 || !dat_c0) { fprintf(stderr, "FAIL: RDTs nicht lesbar\n"); return 1; }
    static re15_rdt_t rdt_d0, rdt_c0;
    if (re15_rdt_parse(dat_d0, sz_d0, &rdt_d0) != 0 ||
        re15_rdt_parse(dat_c0, sz_c0, &rdt_c0) != 0) {
        fprintf(stderr, "FAIL: parse\n"); return 1;
    }

    printf("##### SZENARIO %s #####\n", scenario_b ? "B (Cutscene-Flag gesetzt)" : "A (ohne Cutscene)");

    /* ===== ERST-BETRETUNG 10D0 (Tuer-15-Spawn, fern vom Zombie) ===== */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt_d0, pl->x, pl->z, 0);
    load_em10_bank(base);
    for (int f = 0; f < 8; f++) scd_vm_tick();

    dump_flags("nach Entry#1");
    dump_roster("Entry#1");
    printf("== Phase 1: Erst-Betretung, Spieler fern (soll: liegend, fr=0) ==\n");
    run_phase("Z1", 240, 120);
    dump_flags("Ende Phase 1");

    if (scenario_b) {
        /* byte-true sub21 +0x07e0 (Datei 0x19ec): Set(4,247)=1 — "Szene gesehen".
         * (Wir simulieren NUR das Flag, nicht die ganze Cutscene.) */
        re15_game_flag_set(4, 247, 1);
        printf("== SZENARIO B: Set(4,247)=1 (sub21 +0x07e0) ==\n");
    }

    /* ===== RAUS: Tuer 15 nach ROOM10C0 (main00 +0x196: next=(-6700,0,-2100)) ===== */
    printf("== Tuer 15 -> ROOM10C0, 120 Ticks dort ==\n");
    door_transition(&rdt_c0, 0x10C0, -6700, -2100);
    for (int t = 0; t < 120; t++) tick_once();
    dump_flags("in 10C0");

    /* ===== WIEDER REIN: 10D0, gleicher Tuer-Spawn ===== */
    printf("== zurueck nach ROOM10D0 ==\n");
    door_transition(&rdt_d0, 0x10D0, 7650, 11400);
    load_em10_bank(base);
    for (int f = 0; f < 8; f++) scd_vm_tick();
    dump_flags("nach Re-Entry");
    dump_roster("Re-Entry");

    printf("== Phase 2: Re-Entry, Spieler fern — 360 Ticks ==\n");
    run_phase("Z2", 360, 120);

    printf("== Phase 3: Spieler nah (dist<0xBB8) — 600 Ticks: wacht er auf? ==\n");
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 5878; pl->z = 24000;   /* dist = 1694 < 0xBB8 zur Spawn-Pos */
    run_phase("Z3", 600, 120);

    re15_actor_t *z = find_zombie();
    printf("\n== BEFUND (%s) ==\n", scenario_b ? "B" : "A");
    if (!z) {
        printf("KEIN Zombie aktiv nach Re-Entry!\n");
    } else {
        printf("Ende: st=%u s1=0x%02x s2=%u grid=0x%02x mo=%d fr=%u pos=(%ld,%ld)\n",
               z->state, z->sub_state_1, z->sub_state_2, z->grid_id,
               (int)z->motion, z->anim_frame, (long)z->x, (long)z->z);
    }

    /* SOLL (zombie_10d0_reentry.md, F1-F4 CONFIRMED) */
    CHECK(z != NULL, "(%s) Zombie nach Re-Entry aktiv (kill-flag 0xCD ungesetzt)",
          scenario_b ? "B" : "A");
    if (z && !scenario_b) {
        /* A: Ck(4,247)==0 -> Erst-Zweig erneut: Liegend-Schlaefer (beh 0x0e), weckt bei
         * dist<0xBB8 (decide 0x80105470), steht auf und engaged — wie die Erstbetretung. */
        CHECK(z->sub_state_1 != 0x12 && z->motion != 0x2a,
              "(A) Zombie ist nach Annaeherung WACH (nicht mehr Schlaf-Sub 0x12/Clip 0x2A), "
              "got s1=0x%02x mo=%d", z->sub_state_1, (int)z->motion);
        CHECK(z->state == 1 || z->state == 2,
              "(A) lebende State-Maschine (1/2), got st=%u", z->state);
    }
    if (z && scenario_b) {
        /* B: ELSE-Zweig (Ck(4,247)==1) -> STEHENDER Schlaefer beh 0x02, KEIN Marvin.
         * Nibble-2-Dispatch (FUN_80101784, decide f960[0]=FUN_80101c7c): bei Naehe
         * (arc 0x5f4 + LOS-Bit + dist<0xFA0) -> 0x201 ENGAGE -> Approach. Vor dem Fix
         * (default:break) blieb er als Statue auf s1=0 stehen. */
        CHECK(z->grid_id == 0x02, "(B) beh-0x02-Spawn (ELSE @Datei 0x12a4), got grid=0x%02x",
              z->grid_id);
        CHECK(z->sub_state_1 != 0,
              "(B) Zombie ENGAGED nach Annaeherung (f960[0] @0x80101c8c-cf4 -> 0x201) — "
              "s1==0 = die alte Statue, got s1=0x%02x mo=%d fr=%u",
              z->sub_state_1, (int)z->motion, z->anim_frame);
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            CHECK(!(g_actors[s].active && g_actors[s].type == 0x40),
                  "(B) KEIN Marvin im ELSE-Zweig, slot %d ist type 0x40", s);
    }
    free(dat_d0); free(dat_c0);
    if (fails) { printf("ZOMBIE 10D0 RE-ENTRY: %d FAIL\n", fails); return 1; }
    printf("ZOMBIE 10D0 RE-ENTRY (%s): all checks passed\n", scenario_b ? "B" : "A");
    return 0;
}
