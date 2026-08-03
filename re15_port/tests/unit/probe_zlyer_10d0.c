/* probe_zlyer_10d0.c — DIAGNOSE (kein ctest-Assert): Nutzer-Report 2026-08-03:
 * "Der Zombie in ROOM10D0 soll am Anfang liegen, sich dann nach vorne beugen und
 *  aufstehen. Bei uns bleibt er nur nach vorne gebeugt liegen."
 *
 * ORIGINAL (RE'ed, siehe analysis/zombie_lyer_10d0.md):
 *  - sub00 spawnt den Zombie als Sce_em_set slot=1 type=0x10 behavior=0x0e
 *    pos=(5878,0,25694) (RDT sub_scd +0x005e, Datei 0x126a).
 *  - INIT FUN_80100688: Spawn-State-Decoder (@0x80100c20, Branch @0x80100904 wenn
 *    DAT_800aca3c&1==0). sel==0x0e (@0x80100f64-fd4):
 *      +0x94=0x2A, +0x95=0, +0x8f=0, anim_set(+0x170/+0x174,0,0x200),
 *      word +0x4 = 0x00001201  -> State 1 / +0x5=0x12 (SLEEPING-LYING) / +0x6=0,
 *      +0x9 = 0                -> Behavior-Byte KOMPLETT geloescht.
 *  - Sleeping-Lying [0x12]: animate FUN_801054f4 case0: +0x1b8=1, +0x6=1; Phase 1
 *    spielt NICHTS (Frame haelt 0 = flach liegend). decide 0x80105470: Wake
 *    +0x6 1->2 wenn (+0x9&0x1f)==0xF (Script) ODER +0x1d0<0xBB8 && player.hp>=0.
 *    Phase 2: Clip 0x2A neu ab Frame 0, +0x8f=0, +0x6=3; Phase 3: f314 rate 0x200
 *    spielt 0x2A durch (= "nach vorne beugen"); done -> Phase 4: word=0xd01,
 *    +0x1b8=0, +0x93&=0xfe.
 *  - Standup [0xd] FUN_80104a50: Clip 0x29 (+0x8f=7, 1/4-RNG SE 5), einmal
 *    durchspielen -> word=0x201 (ENGAGE). Sequenz: 0x2A(hold f0) -> 0x2A(play) ->
 *    0x29 -> engage.
 *
 * PORT-HYPOTHESEN (statisch, hier dynamisch zu belegen):
 *  H1  Spawn-Decoder (enemy_ai_common.c INIT) behandelt sel 0x0e als "pose-only":
 *      sub_state_1 bleibt 0, grid_id bleibt 0x0e.
 *  H2  AI-Dispatch `switch(grid_id&0xf)` hat fuer 0xE nur `default: break` ->
 *      die (vorhandene, byte-true) Sleeping-Maschine re15_enemy_ai_live_sleeping
 *      wird NIE erreicht; der Zombie wacht nie auf, egal wie nah der Spieler ist.
 *  H3  Der globale Advancer (player_common.c) pinnt Clip 0x2A NICHT (Pin-Liste
 *      0x0C/0x0E/0x12/0x13) -> Clip 0x2A spielt ab Spawn einmal durch und haelt
 *      auf dem letzten Frame = die NACH-VORNE-GEBEUGTE Pose statt flach-liegend.
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
    uint8_t st, s1, s2, grid;
    int16_t motion; uint16_t frame; uint16_t aflags;
    uint32_t dist;
} zsnap_t;

static void ztake(const re15_actor_t *a, zsnap_t *s)
{
    s->st = a->state; s->s1 = a->sub_state_1; s->s2 = a->sub_state_2;
    s->grid = a->grid_id; s->motion = a->motion; s->frame = a->anim_frame;
    s->aflags = a->anim_flags; s->dist = a->ai_dist;
}

static int zctrl_diff(const zsnap_t *a, const zsnap_t *b)
{
    return a->st != b->st || a->s1 != b->s1 || a->s2 != b->s2 ||
           a->grid != b->grid || a->motion != b->motion || a->aflags != b->aflags;
}

static void zdump(const char *tag, int tick, const zsnap_t *s)
{
    printf("%5d %-5s st=%u s1=0x%02x s2=%u grid=0x%02x mo=%-3d fr=%-3u af=0x%04x dist=%u\n",
           tick, tag, s->st, s->s1, s->s2, s->grid, (int)s->motion, s->frame,
           s->aflags, (unsigned)s->dist);
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

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Tuer-Spawn (weit weg vom Zombie @ (5878,25694)): dist ~14300 > 0xBB8 */
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    /* Wie live: EM010-Bank laden, damit re15_actor_clip_len/clip_done echte
     * Framezahlen sehen (Muster probe_marvin_10d0). */
    {
        char emsp[600]; size_t ems_size = 0;
        snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
        uint8_t *ems = read_file(emsp, &ems_size);
        if (ems) {
            int idx = re15_ems_index_for_type(0x10);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0) {
                re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
                if (eb) {
                    static uint8_t s_em10[0x80000];
                    if (len <= sizeof s_em10) {
                        memcpy(s_em10, ems + off, len);
                        re15_tim_t tim = (re15_tim_t){0};
                        if (re15_emd_parse_container(s_em10, len, &eb->md1, &eb->skel,
                                                     &eb->anim, &tim) == 0) {
                            eb->ok = 1; eb->buf = NULL;
                            printf("EM010 bank: %d clips, fc[0x29]=%d fc[0x2a]=%d\n",
                                   eb->anim.clip_count,
                                   eb->anim.clip_count > 0x29 ? eb->anim.clips[0x29].frame_count : -1,
                                   eb->anim.clip_count > 0x2a ? eb->anim.clips[0x2a].frame_count : -1);
                        }
                    }
                }
            }
            free(ems);
        }
    }

    for (int f = 0; f < 8; f++) scd_vm_tick();

    printf("== Spawn-Roster nach sub00 ==\n");
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (a->active)
            printf("  slot=%d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u mo=%d fr=%u pos=(%ld,%ld,%ld)\n",
                   s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2,
                   (int)a->motion, a->anim_frame,
                   (long)a->x, (long)a->y, (long)a->z);
    }

    /* Zombie = Script-Slot 1 -> Actor-Slot 2 (Marvin = Slot 1) */
    re15_actor_t *z = &g_actors[2];
    if (!z->active || z->type != 0x10) {
        fprintf(stderr, "FAIL: Zombie nicht in Slot 2 (type=0x%02x active=%u)\n",
                z->type, z->active);
        return 1;
    }

    zsnap_t prev, now;
    memset(&prev, 0xFF, sizeof prev);

    /* Phase A — Spieler bleibt an der Tuer (dist >> 3000): liegt er ruhig auf Frame 0? */
    printf("== Phase A: Spieler fern (soll: liegend, Frame haelt 0) ==\n");
    for (int tick = 0; tick < 240; tick++) {
        scd_vm_tick();
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        ztake(z, &now);
        if (zctrl_diff(&now, &prev)) { zdump("ZOMB", tick, &now); prev = now; }
        if (tick % 60 == 0) zdump("  fr", tick, &now);
    }

    /* Phase B — Spieler dicht an den Zombie (dist ~1700 < 0xBB8): WACHT ER AUF? */
    printf("== Phase B: Spieler nah (dist<3000; soll: +0x6 1->2, 0x2A spielen, 0x29, engage) ==\n");
    pl->x = 5878; pl->z = 24000;   /* dist = 1694 < 0xBB8 */
    int woke = -1, standup = -1, engaged = -1;
    for (int tick = 0; tick < 600; tick++) {
        scd_vm_tick();
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        ztake(z, &now);
        if (zctrl_diff(&now, &prev)) { zdump("ZOMB", tick, &now); prev = now; }
        if (tick % 120 == 0) zdump("  fr", tick, &now);
        if (woke < 0 && now.s1 == 0x12 && now.s2 >= 2) woke = tick;
        if (standup < 0 && now.s1 == 0x0d) standup = tick;
        if (engaged < 0 && now.s1 == 0x02 && now.st == 1) engaged = tick;
    }

    printf("\n== BEFUNDE ==\n");
    printf("Ende: st=%u s1=0x%02x s2=%u grid=0x%02x mo=%d fr=%u\n",
           now.st, now.s1, now.s2, now.grid, (int)now.motion, now.frame);
    printf("H1 grid nach INIT (soll 0x00, +0x5=0x12): grid=0x%02x s1=0x%02x -> %s\n",
           now.grid, now.s1,
           (now.grid == 0x0e && now.s1 != 0x12) ? "DECODER-LUECKE REPRODUZIERT" : "decoder ok?");
    printf("H2 Wake (s2>=2 im Sub 0x12): %s (tick %d); Standup 0x0d: %s (tick %d); Engage: %s (tick %d)\n",
           woke >= 0 ? "JA" : "NIE", woke,
           standup >= 0 ? "JA" : "NIE", standup,
           engaged >= 0 ? "JA" : "NIE", engaged);
    printf("H3 Clip 0x2A Frame (soll im Schlaf 0, ist Hold-Last wenn Advancer frei laeuft): fr=%u\n",
           now.frame);

    free(data);
    return 0;
}
