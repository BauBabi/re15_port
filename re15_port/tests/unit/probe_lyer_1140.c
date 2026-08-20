/* probe_lyer_1140.c — MESSSONDE (Nutzer-Report v0.3.4+):
 *
 *   "Im Dining Room muss der Zombie, der gefressen wird, am Boden auch bei RE2 AI und
 *    RE2 AI + Model am Boden liegen bleiben wie bei RE1.5 AI, und nicht aufstehen."
 *
 * Faehrt den ECHTEN ROOM1140 (RDT + raum-eigenes sub00 -> Sce_em_set-Spawns), laedt die
 * ECHTEN Baenke (RE1.5 EMD/CDEMD0.EMS bzw. RE2 shared_assets/RE2/CDEMD0.EMS — ohne Bank ist
 * re15_actor_clip_len()==0 und JEDE clip-getriebene Phase endet im Setz-Tick) und protokolliert
 * ueber viele hundert Frames fuer JEDEN Zombie:
 *   state / +0x5 / +0x6 / +0x7 / grid_id(+0x9) / motion(+0x94) / anim_frame(+0x95) / clip_len /
 *   ai_dist(+0x1d0) / re2z_f10e(+0x10E) / actor-y / HOECHSTER PUNKT der posierten Skelett-Pose
 * Der hoechste Punkt (min. Welt-Y ueber alle Bones; PSX-Y zeigt nach UNTEN) ist die
 * rig-unabhaengige LAGE-Messung: liegend ~ -400, stehend ~ -1400.
 *
 * Aufruf:  probe_lyer_1140 [re15|re2] [frames]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_actor_clip_len(const re15_actor_t *a);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Baenke ------------------------------------------------------------------------------ */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2  = NULL; static size_t s_ems2_n  = 0;
static uint8_t  s_scratch[3][0x80000];

static void load_bank15(uint8_t type, int idxslot)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return;
    if (len > sizeof s_scratch[0]) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type); if (!eb) return;
    memcpy(s_scratch[idxslot], s_ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_scratch[idxslot], len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_scratch[idxslot], len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_scratch[idxslot], len, &eb->skel_own,  &eb->anim_own)  == 0);
    }
}

static void load_bank2(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

/* Hoechster Punkt der GERADE posierten Skelett-Pose (min. Welt-Y). Rig-unabhaengig. */
static int32_t pose_top_y(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count <= 0) return e->y;
    int32_t best = 0x7fffffff;
    for (int i = 0; i < b->skel.bone_count; i++) {
        int32_t w[3]; re15_enemy_bone_world_pos(e, i, w);
        if (w[1] < best) best = w[1];
    }
    return (best == 0x7fffffff) ? e->y : best;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

int main(int argc, char **argv)
{
    const char *fl = (argc > 1) ? argv[1] : "re2";
    int budget     = (argc > 2) ? atoi(argv[2]) : 900;
    const char *room = (argc > 3) ? argv[3] : "1140";
    int re2 = (fl[0] == 'r' && fl[1] == 'e' && fl[2] == '2');

    size_t sz = 0;
    char rpath[512];
    snprintf(rpath, sizeof rpath, "%s/STAGE1/ROOM%s.RDT", RE15_ASSET_PSX_DIR, room);
    uint8_t *buf = slurp(rpath, &sz);
    if (!buf) { printf("FAIL: ROOM1140.RDT nicht lesbar\n"); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0 || !rdt.sub_scd[0]) { printf("FAIL: RDT-Parse\n"); return 1; }

    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    /* --- BLOCK 1: Wer ist der Liegende? (VOR jedem AI-Tick, also Roh-Spawn) --- */
    printf("=== ROOM1140 Roh-Spawn (vor jedem AI-Tick), Flavor=%s ===\n", re2 ? "RE2" : "RE1.5");
    int lyer = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("  slot %d: typ=0x%02X grid=0x%02X (nibble %u, bit0x80=%d) pos=(%d,%d,%d) yaw=%d "
               "state=%u +0x5=%u motion=%d\n",
               s, e->type, e->grid_id, e->grid_id & 0xf, (e->grid_id & 0x80) ? 1 : 0,
               e->x, e->y, e->z, e->rot_y, e->state, e->sub_state_1, e->motion);
        if ((e->grid_id & 0x0f) == 8) lyer = s;
    }
    if (lyer < 0) { printf("FAIL: kein Liege-Spawn (Nibble 8) gefunden\n"); return 1; }
    printf("  -> LIEGENDER = slot %d (Deskriptor 0x%02X)\n\n", lyer, g_actors[lyer].grid_id);

    /* Baenke */
    if (re2) { load_bank2(0x10); load_bank2(0x11); load_bank2(0x16); }
    else     { load_bank15(0x10, 0); load_bank15(0x11, 1); load_bank15(0x16, 2); }
    for (int t = 0x10; t <= 0x16; t++) {
        if (t != 0x10 && t != 0x11 && t != 0x16) continue;
        re15_enemy_bank_t *b = re15_enemy_find((uint8_t)t);
        printf("  Bank 0x%02X: %s (%d Bones, %d Clips, loco=%d)\n", t,
               (b && b->ok) ? "geladen" : "FEHLT", b ? b->skel.bone_count : 0,
               b ? b->anim.clip_count : 0, b ? b->loco_ok : 0);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    /* Spieler faehrt zum Liegenden: Raumeingang -> daneben. Der Weg IST das Reiz-Signal. */
    re15_actor_t *L = &g_actors[lyer];
    pl->x = L->x; pl->z = L->z + 12000;

    printf("\n=== Verlauf (nur Aenderungen + alle 100 Frames), %d Frames ===\n", budget);
    printf("f     slot typ  st +0x5 +0x6 +0x7 grid  mot len fr   dist  f10e   y      topY\n");
    uint64_t last[RE15_ACTOR_MAX]; memset(last, 0xff, sizeof last);
    int standup_frame = -1;
    for (int f = 0; f < budget; f++) {
        /* langsam heranfahren (200/Frame), dann daneben stehen bleiben */
        if (pl->z > L->z + 500) pl->z -= 200;
        pl->motion = 100;
        if (pl->hp < 30) { pl->hp = 100; pl->state = 0; pl->hit_react = 0; }
        frame();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active) continue;
            uint64_t sig = ((uint64_t)e->state << 40) | ((uint64_t)e->sub_state_1 << 32) |
                           ((uint64_t)e->sub_state_2 << 24) | ((uint64_t)e->sub_state_3 << 16) |
                           ((uint64_t)e->grid_id << 8) | (uint64_t)e->motion;
            int show = (sig != last[s]) || (f % 100 == 0);
            if (s != lyer && f % 100 != 0 && sig == last[s]) show = 0;
            if (show) {
                printf("%-5d %-4d 0x%02X %-2u %-4u %-4u %-4u 0x%02X  %-3d %-3d %-4u %-5u 0x%04x %-6d %-6d%s\n",
                       f, s, e->type, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                       e->grid_id, (int)e->motion, re15_actor_clip_len(e), e->anim_frame,
                       e->ai_dist, e->re2z_f10e, e->y, pose_top_y(e), (s == lyer) ? "  <== LIEGENDER" : "");
            }
            last[s] = sig;
        }
        if (standup_frame < 0 && lyer >= 0) {
            re15_actor_t *e = &g_actors[lyer];
            /* "aufgestanden" = raus aus dem Liege-Executor */
            int lying_re2  = (e->state == 1 && e->sub_state_1 == 7);
            int lying_re15 = (e->state == 1 && (e->grid_id & 0x0f) == 8);
            if (re2 ? !lying_re2 : !lying_re15) standup_frame = f;
        }
    }

    re15_actor_t *e = &g_actors[lyer];
    printf("\n=== ERGEBNIS ===\n");
    printf("  Liegender slot %d: erste Abweichung vom Liege-Zustand bei Frame %d\n", lyer, standup_frame);
    printf("  Endzustand: state=%u +0x5=%u +0x6=%u grid=0x%02X motion=%d topY=%d ai_dist=%u f10e=0x%04x\n",
           e->state, e->sub_state_1, e->sub_state_2, e->grid_id, (int)e->motion,
           pose_top_y(e), e->ai_dist, e->re2z_f10e);
    free(buf);
    return 0;
}
