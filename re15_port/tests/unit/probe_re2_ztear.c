/**
 * @file probe_re2_ztear.c
 * @brief MESSUNG der ZERREISS-TODE des RE2-Zombies (DEATH-Tabelle 0x8010CC24, die Zellen, die
 *        NICHT FUN_80108530 sind).
 *
 * Faehrt den ECHTEN Weg (re15_game_step + Pad R1/SQUARE, echte ROOM1140-Spawns, GELADENE
 * RE2-Bank EM010 — ohne Bank ist re15_actor_clip_len() == 0 und jede clip-getriebene Phase
 * endet im Setz-Tick, die Messung waere wertlos).
 *
 * Fuer jede Waffe wird bis zum Tod gefeuert und danach die volle Zustandsfolge protokolliert:
 *   Zustand/Sub/Clip/Frame, die dispatchte DEATH-Zelle (re15_re2z_last_death_handler),
 *   die Flagworte der 16 Modellblock-Records (welcher Part traegt 0x4A / 0x1062 / 0x80 /
 *   Bit 0), der Geometrie-Zeiger von Part 0 (Rumpf-Stumpf = 16) und die Flug-Translation
 *   des Kopfes (Part 8) ueber re15_re2z_gore_part_matrix.
 *
 * argv[1] = RE1.5-Waffen-Id (7 Magnum -> Zeile 5, 8 Schrotflinte -> Zeile 7,
 *           13 SPAS-12 -> Zeile 8, 9 HE-Granate -> Zeile 9, 18 Rakete -> Zeile 17,
 *           3 Pistole -> Zeile 3 = Regressions-Referenz)
 * argv[2] = Frame-Budget (Default 600)
 * argv[3] = 1 -> zusaetzlich die Part-Flagworte JEDES Frames dumpen
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_death_handler(void);
extern int  re15_re2z_death_cell(unsigned row, unsigned col);
extern int  re15_re2z_last_hit_handler(void);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
extern uint32_t re15_re2_rand_draws(void);
extern int  re15_inv_find_item(uint8_t id);
extern int  re15_inv_equipped_slot(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

static const char *hname(int h)
{
    switch (h) {
    case  0: return "NULL";
    case  1: return "MAIN(80108530)";
    case  2: return "7438";
    case  3: return "92C4";
    case  4: return "66FC";
    case  5: return "8BEC";
    case  6: return "9610";
    case -1: return "crawler(10E&1)";
    case -2: return "801099E4-stub";
    case -3: return "lying(21A&2)";
    default: return "?";
    }
}

static void dump_parts(const re15_actor_t *e, const char *tag)
{
    printf("      %s parts:", tag);
    for (int p = 0; p < 16; p++) {
        uint16_t f = e->re2z_part_flags[p];
        if (f == 1u) continue;                       /* unveraendert (nur Bit 0 = zeichnen) */
        printf(" [%d]=%04X", p, f);
        if (e->re2z_part_mesh[p] != (uint8_t)p) printf("(mesh%u)", e->re2z_part_mesh[p]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int weapon  = (argc > 1) ? atoi(argv[1]) : 8;
    int budget  = (argc > 2) ? atoi(argv[2]) : 600;
    int verbose = (argc > 3) ? atoi(argv[3]) : 0;

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    /* Die Briefing-Ladung fuehrt nur Messer + Browning HP. Die schweren Waffen muessen erst in
     * einen Slot, sonst liefert re15_inv_find_item() -1 und der Equip-Slot wird 0x80 (= nichts
     * ausgeruestet) -> jeder SQUARE-Druck verpufft. */
    if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 200; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    {   re15_enemy_bank_t *b = load_re2_bank(0x10);
        printf("RE2-Bank EM010: %s (%d Knochen, %d Clips, %d Meshes)\n",
               (b && b->ok) ? "geladen" : "FEHLT",
               b ? b->skel.bone_count : 0, b ? b->anim.clip_count : 0,
               b ? b->md1.mesh_count : 0); }
    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: kein stehender Zombie\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    printf("=== RE2-Zombie ROOM1140 ZERREISS-TOD — Waffe %d ===\n", weapon);
    printf("Zombie Slot %d Typ 0x%02X hp=%d\n", slot, e->type, e->hp);

    /* Die DEATH-Wurzel FUN_80108250 prueft VOR dem 2D-Dispatch drei Zweige (@0x801083B0 /
     * @0x801083E4 / @0x80108408): +0x10E Bit 0 (Kriecher), +0x21A Bit 0x10 (Kriech-Marker,
     * gesetzt vom Knockdown-P6 @0x8010358C und vom Fress-P3 @0x80103D00) und +0x21A Bit 0x2
     * (liegend). Der ROOM1140-Spawn startet FRESSEND, also mit Bit 0x10 — dann gewinnt Zweig 2
     * und die ZERREISS-Zelle wird gar nicht erreicht. Also erst warten, bis der Zombie
     * AUFRECHT ist, dann feuern. */
    {   int w = 0;
        while (w < 400 && ((e->re2z_flags21a & 0x12u) != 0u || (e->re2z_f10e & 1u) != 0u)) {
            pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); w++;
        }
        printf("Aufrecht ab Warmlauf-Frame %d: 21A=%04X 10E=%04X st=%d s1=%u\n",
               w, e->re2z_flags21a, e->re2z_f10e, e->state, e->sub_state_1);
    }
    /* argv[4] = zusaetzliche Leerframes VOR dem ersten Schuss. Der Port-RNG ist
     * deterministisch (re2z_rand, Zwilling von FUN_80015FE8 mit EINEM Zustandswort), also
     * verschiebt jeder Leerframe die Wurf-Folge — so werden BEIDE Zweige der 3/4-Weichen
     * (@0x80106800 / @0x80108C8C) messbar, ohne am Original zu drehen. */
    if (argc > 4) { int extra = atoi(argv[4]);
        for (int w = 0; w < extra; w++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
        printf("Zusatz-Warmlauf %d Frames: 21A=%04X st=%d s1=%u\n",
               extra, e->re2z_flags21a, e->state, e->sub_state_1); }

    int hp_last = e->hp, hits = 0, shots = 0, dead_f = -1;
    int st_last = -1, s1_last = -1, s2_last = -1, h_last = -99;
    int32_t head_t_first[3] = { 0, 0, 0 }, head_t_last[3] = { 0, 0, 0 };
    int head_flew = 0;
    uint32_t draws_at_death = 0;

    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        if (dead_f < 0) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; shots++; }
        frame(cur, edge);

        int hit = (e->hp < hp_last);
        if (hit) hits++;
        if (dead_f < 0 && e->state == 3) { dead_f = f; draws_at_death = re15_re2_rand_draws();
            printf("--- TOD bei f%d: Zeile +0x5 = %u, Spalte +0x1D2 = %u -> Zelle %s\n",
                   f, e->sub_state_1, e->re2z_hits1d2,
                   hname(re15_re2z_death_cell(e->sub_state_1, e->re2z_hits1d2))); }

        int h = re15_re2z_last_death_handler();
        int changed = (e->state != st_last || e->sub_state_1 != s1_last
                       || e->sub_state_2 != s2_last || h != h_last);
        if (verbose || changed || (dead_f >= 0 && f - dead_f < 40)) {
            int32_t b0[3], b8[3];
            re15_enemy_bone_world_pos(e, 0, b0);
            re15_enemy_bone_world_pos(e, 8, b8);
            printf("f%-4d %s st=%d s1=%2u s2=%2u clip=%2u af=%3d hp=%4d | dcell=%-14s "
                   "| 21A=%04X 10E=%04X 16A=%u 16B=%u 158=%5d 15A=%3d spd=%4d 231=%u "
                   "| b0dy=%-6d b8dy=%-6d\n",
                   f, hit ? "HIT" : "   ", e->state, e->sub_state_1, e->sub_state_2,
                   (unsigned)e->motion, (int)e->anim_frame, e->hp, hname(h),
                   e->re2z_flags21a, e->re2z_f10e, e->re2z_dir16a, e->re2z_gaitrow,
                   (int)e->re2z_t158, (int)e->re2z_t15a, (int)e->speed_h, e->re2z_rag231,
                   b0[1] - e->y, b8[1] - e->y);
            if (changed && dead_f >= 0) dump_parts(e, "");
        }

        /* Der KOPF (Part 8) traegt nach dem Abriss 0x4A -> eigene Matrix + Drift-Physik.
         * Der Renderer ruft re15_re2z_gore_part_matrix je Frame; hier derselbe Aufruf. */
        if (e->re2z_part_flags[8] & 0x40u) {
            int32_t rot[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
            int32_t tr[3];
            re15_enemy_bone_world_pos(e, 8, tr);
            if (re15_re2z_gore_part_matrix(e, 8, (uint32_t)(10000 + f), rot, tr)) {
                if (!head_flew) { head_flew = 1;
                    head_t_first[0] = tr[0]; head_t_first[1] = tr[1]; head_t_first[2] = tr[2]; }
                head_t_last[0] = tr[0]; head_t_last[1] = tr[1]; head_t_last[2] = tr[2];
            }
        }

        st_last = e->state; s1_last = e->sub_state_1; s2_last = e->sub_state_2; h_last = h;
        hp_last = e->hp;
        if (dead_f >= 0 && f - dead_f > 260) break;
    }

    printf("--- Waffe %d: %d Schuesse, %d Treffer, Tod bei f%d (RNG-Wuerfe bis dahin %u)\n",
           weapon, shots, hits, dead_f, draws_at_death);
    printf("--- Endstand: st=%d s1=%u s2=%u clip=%u hp=%d\n",
           e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, e->hp);
    dump_parts(e, "ENDE");
    if (head_flew)
        printf("--- KOPF (Part 8) geflogen: erste (%d,%d,%d) -> letzte (%d,%d,%d)\n",
               head_t_first[0], head_t_first[1], head_t_first[2],
               head_t_last[0], head_t_last[1], head_t_last[2]);
    else
        printf("--- KOPF (Part 8): kein Abriss\n");
    free(buf);
    return 0;
}
