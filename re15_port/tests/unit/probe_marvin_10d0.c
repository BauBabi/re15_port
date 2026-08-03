/* probe_marvin_10d0.c — DIAGNOSE (kein ctest-Assert): Nutzer-Report 2026-08-03:
 * ROOM10D0 Marvin-Cutscene — "Leons Kopf ist komisch verdreht, Marvins Animationen
 * sind falsch".
 *
 * ORIGINAL (RE'ed, siehe analysis/marvin_10d0.md):
 *  - sub00 spawnt Marvin als Sce_em_set slot=0 type=0x40 behavior=0x40 pos=(5775,0,23625)
 *    rot=1142 (RDT sub_scd sub00 @0x004a) + Zombie 0x10 (slot 1, behavior 0x0e).
 *  - FUN_8001b3f8 (Aufruf: Raum-Setup FUN_800396fc @0x80039a08) bindet die RBJ-Records
 *    der animation-Sektion per MARKER-BITMASKE an Entities: 10D0 REC0 marker=1 -> Player,
 *    REC1 marker=2 -> enemy[0]=Marvin (Stores +0x170/+0x174, Stride 0x1F4 @0x8001b47c-bc).
 *  - Cutscene = sub_scd sub21 (0x15), getriggert durch AOT slot 0x12 (Event ff 18 15).
 *  - Plc_neck @0x80041e98 schreibt auf die WORK-ENTITY (lw v1,0x154(a0) @0x80041e9c) —
 *    Marvin bekommt 4 eigene Plc_necks (0x0820/0x08c0/0x09e2/0x0b66).
 *  - Plc_motion @0x80041b90: pc[1]->+0x5 (Executor-Sub), pc[2]->+0x94, pc[3]->+0x1c4.
 *  - Plc_dest @0x80041be4: state=4, +0x5=MODE (6 = Executor-Sub 6 EVENT-REACH
 *    @0x800517f0 = Clip 1 einmal, dann Idle-Clip-2-Loop; KEIN Walk).
 *
 * PORT-HYPOTHESEN (staticher Befund, hier dynamisch belegt):
 *  H1  op_plc_neck schreibt IMMER auf g_actors[PLAYER] — Marvins Neck-Ziele landen auf
 *      Leon; bei 0x08c0 ist das Ziel (5975,0,19825) = Leons eigene Position -> atan2
 *      ueber ~0-Distanz -> Kopf verdreht.
 *  H2  ROOM10D0 fehlt in s_room_rbj_enemy (enemy_common.c) -> Marvin spielt seine
 *      eigene EM40-Bank statt RBJ RECORD 1; Executor-Cliplaengen aus s_irons_clip_len
 *      (50/26/...) statt RBJ-EDD (35/20/...).
 *  H3  Plc_motion(2,6,0): sub_state_1=2 -> Port-Dispatch re15_npc_sub_idle (statt
 *      Original-Sub-2 0x80050f00 = play+hold/loop auf +0x170/+0x174).
 *  H4  Plc_dest(0,6,63,x=0,z=0): Port behandelt Mode 6 als Walk nach (0,0) ->
 *      walk_active haengt fuer immer, Actor dreht Richtung Weltursprung, NPC-Tick
 *      yieldet ab da jede Folge-Plc_motion (Marvin friert ein).
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
#include "re15_aot.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

typedef struct {
    int16_t motion; uint8_t frame; int16_t rot; int32_t x, z;
    uint8_t st, s1, s2; uint16_t aflags;
    uint8_t wact; uint8_t wmode;
    uint8_t nflags; int16_t ntx, nty, ntz, nspd;
} snap_t;

static void take(const re15_actor_t *a, snap_t *s)
{
    s->motion = a->motion; s->frame = a->anim_frame; s->rot = a->rot_y;
    s->x = a->x; s->z = a->z;
    s->st = a->state; s->s1 = a->sub_state_1; s->s2 = a->sub_state_2;
    s->aflags = a->anim_flags;
    s->wact = a->walk_active; s->wmode = a->walk_mode;
    s->nflags = a->neck_flags; s->ntx = a->neck_tx; s->nty = a->neck_ty;
    s->ntz = a->neck_tz; s->nspd = a->neck_speed;
}

/* Nur STEUER-Felder vergleichen (frame/x/z/rot laufen dauernd) */
static int ctrl_diff(const snap_t *a, const snap_t *b)
{
    return a->motion != b->motion || a->st != b->st || a->s1 != b->s1 ||
           a->s2 != b->s2 || a->aflags != b->aflags || a->wact != b->wact ||
           a->wmode != b->wmode || a->nflags != b->nflags || a->ntx != b->ntx ||
           a->nty != b->nty || a->ntz != b->ntz || a->nspd != b->nspd;
}

static void dump(const char *who, int tick, const snap_t *s)
{
    printf("%5d %-6s mo=%-3d fr=%-3u st=%u s1=%-2u s2=%u af=0x%04x rot=%-5d "
           "pos=(%ld,%ld) walk=%u/%u neck=0x%02x tgt=(%d,%d,%d) spd=%d\n",
           tick, who, (int)s->motion, s->frame, s->st, s->s1, s->s2, s->aflags,
           (int)s->rot, (long)s->x, (long)s->z, s->wact, s->wmode,
           s->nflags, (int)s->ntx, (int)s->nty, (int)s->ntz, (int)s->nspd);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("== ROOM10D0 geladen: sub_scd count? main=%p sub0=%p sub21=%p\n",
           (void *)rdt.main_scd, (void *)rdt.sub_scd[0], (void *)rdt.sub_scd[0x15]);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Tuer-Spawn laut re15_room_spawns.h: (7650, 0, 11400) rot 2048 */
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    /* Wie live: EM040-Bank laden (Plattform pc_enemy_load) + Raum-RBJ registrieren
     * (Marker-Binder FUN_8001b3f8 — Bit 2 -> Aktor-Slot 1 = Marvin, Kanal +0x180). Ohne
     * beides fallen die Sub-Kanal-Laengen auf die s_irons-Tabelle zurueck und der
     * RBJ-Kanal bleibt ungebunden. */
    {
        char emsp[600]; size_t ems_size = 0;
        snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
        uint8_t *ems = read_file(emsp, &ems_size);
        if (ems) {
            int idx = re15_ems_index_for_type(0x40);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0) {
                re15_enemy_bank_t *eb = re15_enemy_alloc(0x40);
                if (eb) {
                    static uint8_t s_em40[0x60000];
                    if (len <= sizeof s_em40) {
                        memcpy(s_em40, ems + off, len);
                        re15_tim_t tim = (re15_tim_t){0};
                        if (re15_emd_parse_container(s_em40, len, &eb->md1, &eb->skel,
                                                     &eb->anim, &tim) == 0) {
                            eb->ok = 1; eb->buf = NULL;
                            printf("EM040 bank: %d clips, fc[0..9]=", eb->anim.clip_count);
                            for (int c = 0; c < 10 && c < eb->anim.clip_count; c++)
                                printf("%d ", eb->anim.clips[c].frame_count);
                            printf("\n");
                        }
                    }
                }
            }
            free(ems);
        }
        if (rdt.animation && rdt.animation_size > 0)
            re15_rbj_bind_room(rdt.animation, rdt.animation_size);
    }
    for (int f = 0; f < 8; f++) scd_vm_tick();
    {   /* nach den Spawns: der lazy Marker-Binder muss REC1 an Slot 1 (Marvin) binden */
        const re15_emd_animation_t *ra = re15_actor_rbj_anim(1);
        if (ra) {
            printf("RBJ-Kanal Marvin (slot 1): %d clips, fc[0..9]=", ra->clip_count);
            for (int c = 0; c < 10 && c < ra->clip_count; c++)
                printf("%d ", ra->clips[c].frame_count);
            printf(" (soll REC1: 20 30 30 20 30 25 35 50 24 20)\n");
        } else printf("RBJ-Kanal Marvin: NICHT gebunden\n");
    }

    printf("== Spawn-Roster nach sub00 ==\n");
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (a->active)
            printf("  slot=%d type=0x%02X grid=0x%02X pos=(%ld,%ld,%ld) rot=%d st=%u mo=%d\n",
                   s, a->type, a->grid_id, (long)a->x, (long)a->y, (long)a->z,
                   (int)a->rot_y, a->state, (int)a->motion);
    }

    /* Cutscene ausloesen: AOT slot 0x12 (Event ff 18 15) wuerde beim Betreten der Zone
     * (4700..5500, 19400..20200) feuern; deterministisch: sub21 (0x15) direkt starten. */
    if (!rdt.sub_scd[0x15]) { fprintf(stderr, "FAIL: sub21 fehlt\n"); return 1; }
    scd_thread_start(3, rdt.sub_scd[0x15]);
    printf("== sub21 (Marvin-Cutscene) gestartet ==\n");

    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    snap_t pl_prev, mv_prev, pl_now, mv_now;
    memset(&pl_prev, 0xFF, sizeof pl_prev);
    memset(&mv_prev, 0xFF, sizeof mv_prev);

    int neck_on_player_while_marvin_scene = 0;
    int leon_selftarget_tick = -1;
    int marvin_walk_stuck_from = -1;

    for (int tick = 0; tick < 4200; tick++) {
        /* Dialog-Confirm: virtueller CROSS (0x8000) als Edge, damit Message_on nicht haengt */
        g_scd_pad_edge = 0x8000; g_scd_pad_held = 0x8000;
        scd_vm_tick();
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);

        take(&g_actors[0], &pl_now);
        take(&g_actors[1], &mv_now);
        if (ctrl_diff(&pl_now, &pl_prev)) { dump("LEON", tick, &pl_now); pl_prev = pl_now; }
        if (ctrl_diff(&mv_now, &mv_prev)) { dump("MARVIN", tick, &mv_now); mv_prev = mv_now; }

        /* H1-Detektor: Neck-Ziel == Leons eigene Position (±600) -> Selbst-Blick */
        if ((pl_now.nflags & 0x04) && leon_selftarget_tick < 0) {
            long dx = (long)pl_now.ntx - pl_now.x, dz = (long)pl_now.ntz - pl_now.z;
            if (dx*dx + dz*dz < 600L*600L) leon_selftarget_tick = tick;
        }
        if ((pl_now.nflags & 0x8C) != 0) neck_on_player_while_marvin_scene = 1;
        if (mv_now.wact && mv_now.wmode == 6 && marvin_walk_stuck_from < 0)
            marvin_walk_stuck_from = tick;
    }

    printf("\n== BEFUNDE ==\n");
    printf("H1 Leon-Neck-Selbstziel (Ziel==eigene Pos, Marvin-Kommando fehlgeleitet): %s (tick %d)\n",
           leon_selftarget_tick >= 0 ? "REPRODUZIERT" : "nicht beobachtet", leon_selftarget_tick);
    printf("H4 Marvin walk_active haengt mit Mode 6 (dest 0,0):                      %s (ab tick %d), Ende: walk=%u mode=%u rot=%d\n",
           marvin_walk_stuck_from >= 0 ? "REPRODUZIERT" : "nicht beobachtet",
           marvin_walk_stuck_from, mv_now.wact, mv_now.wmode, (int)mv_now.rot);
    printf("Marvin Ende: st=%u s1=%u s2=%u mo=%d fr=%u pos=(%ld,%ld) rot=%d\n",
           mv_now.st, mv_now.s1, mv_now.s2, (int)mv_now.motion, mv_now.frame,
           (long)mv_now.x, (long)mv_now.z, (int)mv_now.rot);
    printf("Leon Ende: mo=%d fr=%u pos=(%ld,%ld) rot=%d neck=0x%02x tgt=(%d,%d,%d)\n",
           (int)pl_now.motion, pl_now.frame, (long)pl_now.x, (long)pl_now.z,
           (int)pl_now.rot, pl_now.nflags, (int)pl_now.ntx, (int)pl_now.nty, (int)pl_now.ntz);

    free(data);
    return 0;
}
