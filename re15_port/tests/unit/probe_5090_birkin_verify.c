/* probe_5090_birkin.c — MESSUNG (kein Pin): warum steht Birkin (0x36) in ROOM5090
 * auf Zustand 0 und der Nutzer sieht ihn nie?
 *
 * Nutzer-Befund (befund.log 2026-09-14, F9-MARKE, ROOM5090 Bild 1040, Kamera 12):
 *   Spieler pos=(13600,0,-23550) rot=2057
 *   slot2  typ=0x36 st=0 ss=0/0 clip=1 bild=43 hp=600 pos=(-14174,0,-23350) grid=13
 *
 * Diese Sonde faehrt den ECHTEN Ablauf: ROOM5090.RDT -> SCD-VM -> re15_game_step
 * (mit AOT-/RVD-Scan) -> re15_enemy_ai_run_all, Spieler auf der Original-Tuerposition,
 * und laeuft ihn nach WESTEN, bis der RVD-Zonenwechsel 13->12 die Kamera auf 12 stellt
 * (das ist der Kampf-Ausloeser: sub01 @0x1290 `23 00 0a 00 0c 00` = Cmp(work[0x0A]==12)).
 *
 * Bezugsgroessen, alle aus ROOM5090.RDT gelesen (Datei-Offsets, kein Schaetzwert):
 *   @0x122A  44 00 4d 40 ...            Sce_em_set slot0 Typ 0x4D
 *   @0x124A  44 01 30 33 00 00 00 ff 94 c6 00 00 ca a4
 *                                       Sce_em_set slot1 Typ 0x30 grid 0x33 (-14700,0,-23350)
 *   @0x1290  23 00 0a 00 0c 00          Cmp(work[0x0A] == 12)  -> Kampf-Gate
 *   @0x1296  04 ff 18 04                Evt_exec sub04
 *   @0x12FA  2e 02 01                   Work_set(2,1)  = Gegner-Slot 1
 *   @0x12FE  32 00 b0 04 00 00 ca a4    Pos_set(1200, 0, -23350)   <-- der Auftritt
 *   @0x130A  34 0c 13 00                Member_set(0x0c, 0x13) = entity+0x09 = 0x13
 *   @0x1320  40 00 09 20 2c 01 98 a4    Plc_dest(mode 9 = DREHEN, 300, -23400)
 *   @0x1342  29 0c                      Cut_chg(12)
 *   @0x10CE  3b 04 ... 3e 62 2c 9d 84 03 6e 0f   Door_aot 4: x 25150..26050 / z -25300..-21350
 *   RVD @0x04CC  from=12 to=13  x 14000..15400   (Ostgrenze der Kampf-Kamera)
 *   RVD @0x04B8  from=12 to=11  x  1203.. 2703   (Westgrenze der Kampf-Kamera)
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* EM036 aus der RE2-Bank (genau wie probe_g5_boss / der Port zur Laufzeit). */
static int load_em036(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x36);
    if (!eb) eb = re15_enemy_alloc(0x36);
    if (!eb) { free(ems); return 0; }
    if (re2_ems_load_bank(ems, n, 0x36, eb, NULL) != 0) { free(ems); return 0; }
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

/* Der Spieler-Modus-/Letterbox-Zweig des ECHTEN Hauptlaufs (main.c:4340-4360).
 * Ohne ihn bleibt player_mode 0, der Pad-Zweig in player_common.c:1058 setzt
 * walk_active jeden Frame zurueck, der Plc_dest-Mode-9-Dreher kommt nie an und
 * sub04 haengt vor Cut_chg(12) — eine reine SONDEN-Verfaelschung. */
int g_probe_saw_1200=-1; int g_probe_fno=0; int g_probe_minabs=0x7fffffff;
static int s_cine_was_active = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    { int _bs=-1; for (int _s=1;_s<RE15_ACTOR_MAX;_s++) if (g_actors[_s].active && g_actors[_s].type==0x36) _bs=_s;
      if (_bs>=0 && g_probe_saw_1200<0 && g_actors[_bs].x==1200) g_probe_saw_1200=g_probe_fno;
      if (g_probe_fno==181) printf("  [TRACE] nach scd_tick : x=%d\n", (int)g_actors[2].x);
      if (_bs>=0 && g_probe_minabs > (g_actors[_bs].x>1200?g_actors[_bs].x-1200:1200-g_actors[_bs].x))
          g_probe_minabs = (g_actors[_bs].x>1200?g_actors[_bs].x-1200:1200-g_actors[_bs].x); }
    g_probe_fno++;
    re15_actor_step_all_walkers();
    if (g_probe_fno==182) printf("  [TRACE] nach walkers : x=%d\n", (int)g_actors[2].x);
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    if (g_probe_fno==182) printf("  [TRACE] nach game_step: x=%d\n", (int)g_actors[2].x);
}

static void dump(int f)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("F%-5d cam=%-2d work0A=%-3d spieler=(%6d,%6d) rot=%-5d st=%d/%d walk=%d "
           "f1:1b=%d f2:07=%d f5:20=%d f3:2a=%d | ",
           f, s_shown, (int)g_scd.work_vars[0x0A], (int)pl->x, (int)pl->z,
           (int)pl->rot_y, pl->state, pl->sub_state_1, pl->walk_active,
           re15_game_flag_get(1, 0x1b), re15_game_flag_get(2, 0x07),
           re15_game_flag_get(5, 0x20), re15_game_flag_get(3, 0x2a));
    { int b, erste = 1;
      printf("bank5:{");
      for (b = 0; b < 256; b++) if (re15_game_flag_get(5, (unsigned char)b)) {
          printf("%s%02X", erste ? "" : ",", b); erste = 0; }
      printf("} wfb=%02X wact=%d wfsm=%d | ",
             (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].walk_flag_bit,
             g_actors[RE15_ACTOR_SLOT_PLAYER].walk_active,
             g_actors[RE15_ACTOR_SLOT_PLAYER].walk_fsm); }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type == 0) continue;
        long dx = (long)pl->x - (long)e->x;
        printf("[%d t=%02X st=%d ss=%d/%d clip=%d bild=%d grid=%02X hp=%d pos=(%d,%d,%d) dx=%ld] ",
               s, (unsigned)e->type, e->state, e->sub_state_1, e->sub_state_2,
               (int)e->motion, (int)e->anim_frame, (unsigned)e->grid_id, (int)e->hp,
               (int)e->x, (int)e->y, (int)e->z, dx);
    }
    printf("\n");
}

static int s_posset = 0, s_posset_done = 0;

int main(int argc, char **argv)
{
    size_t rsz = 0;
    for (int i = 1; i < argc; i++)
        if (strcmp(argv[i], "possetzt") == 0) s_posset = 1;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== ROOM5090 — warum steht Birkin (0x36) auf Zustand 0? ===\n");
    printf("  RDT-Bytes (Datei-Offsets):\n");
    printf("    @0x124A Sce_em_set :");
    for (int i = 0; i < 20; i++) printf(" %02x", raw[0x124A + i]);
    printf("\n");
    printf("    @0x12FA Work_set   : %02x %02x %02x\n", raw[0x12FA], raw[0x12FB], raw[0x12FC]);
    printf("    @0x12FE Pos_set    :");
    for (int i = 0; i < 8; i++) printf(" %02x", raw[0x12FE + i]);
    printf("   -> x=%d y=%d z=%d\n",
           (int)(int16_t)(raw[0x1300] | (raw[0x1301] << 8)),
           (int)(int16_t)(raw[0x1302] | (raw[0x1303] << 8)),
           (int)(int16_t)(raw[0x1304] | (raw[0x1305] << 8)));
    printf("    @0x130A Member_set : %02x %02x %02x %02x  -> member %d = %d (entity+0x09)\n",
           raw[0x130A], raw[0x130B], raw[0x130C], raw[0x130D], raw[0x130B],
           (int)(int16_t)(raw[0x130C] | (raw[0x130D] << 8)));
    printf("    @0x1320 Plc_dest   :");
    for (int i = 0; i < 8; i++) printf(" %02x", raw[0x1320 + i]);
    printf("   -> mode=%d (9 = DREHEN, kein Laufen) dest=(%d,%d)\n", raw[0x1322],
           (int)(int16_t)(raw[0x1324] | (raw[0x1325] << 8)),
           (int)(int16_t)(raw[0x1326] | (raw[0x1327] << 8)));

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    /* Tuerspawn: Mitte des Door_aot-4-Rechtecks @0x10CE. */
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_em036()) {
        printf("  (EM036-Bank fehlt — Clips/Root-Motion sind dann leer)\n");
    } else {
        re15_enemy_bank_t *eb = re15_enemy_find(0x36);
        printf("  EM036 geladen: %d Bones, %d Clips\n",
               eb->skel.bone_count, eb->anim.clip_count);
    }

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, /*entry cut*/ 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    printf("\n-- A) 30 Bilder stehen bleiben (Raum-Init, Spawn) --\n");
    for (int f = 0; f < 30; f++) { frame(); if (f < 4 || f == 29) dump(f); }

    printf("\n-- B) nach WESTEN laufen (75/Bild) bis Kamera 12 --\n");
    int trigger_f = -1;
    int32_t trigger_px = 0;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        frame();
        if (trigger_f < 0 && s_shown == 12) { trigger_f = f; trigger_px = pl->x; dump(f); }
        else if ((f % 40) == 0) dump(f);
        if (trigger_f >= 0 && f > trigger_f + 3) break;
    }
    printf("  KAMPF-GATE: Kamera 12 erreicht bei Bild %d, Spieler x=%d\n",
           trigger_f, (int)trigger_px);

    printf("\n-- C) 3000 Bilder Kampf (Spieler steht) — NACHMESSUNG --\n");
    int32_t bx_min = 0x7fffffff, bx_max = -0x7fffffff;
    int bslot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) bslot = s;
    printf("  Boss-Slot = %d\n", bslot);
    int f_first_move = -1, f_cross = -1, f_ever_1200 = -1, f_bild43 = -1;
    int32_t x_bild43 = 0;
    int cam12_frames = 0, in_quad_frames = 0;
    int16_t qx[4], qz[4];
    for (int f = 0; f < 3000; f++) {
        frame();
        if (bslot < 0) continue;
        re15_actor_t *b = &g_actors[bslot];
        if (f_first_move < 0 && b->x != -14700) f_first_move = f;
        if (f_ever_1200 < 0 && b->x == 1200) f_ever_1200 = f;
        if (f_cross < 0 && b->x > -5600) f_cross = f;
        if (f_bild43 < 0 && (int)b->motion == 1 && (int)b->anim_frame == 43) {
            f_bild43 = f; x_bild43 = b->x;
        }
        if (b->x < bx_min) bx_min = b->x;
        if (b->x > bx_max) bx_max = b->x;
        if (s_shown == 12) cam12_frames++;
        if (re15_rdt_get_region_quad(&s_rdt, s_shown, qx, qz)) {
            if (re15_aot_point_in_quad(b->x, b->z, qx, qz)) in_quad_frames++;
        }
    }
    if (bslot >= 0) {
        re15_actor_t *b = &g_actors[bslot];
        printf("\n  === NACHMESSUNG ===\n");
        printf("  Birkin-X ueber 3000 Bilder: %d .. %d  (Spieler x=%d)\n",
               (int)bx_min, (int)bx_max, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x);
        printf("  kleinste |dx| = %ld\n",
               (long)((long)g_actors[RE15_ACTOR_SLOT_PLAYER].x - (long)bx_max));
        printf("  erste Bewegung weg von -14700 : Bild %d\n", f_first_move);
        printf("  x==1200 DIREKT NACH scd_vm_tick (vor KI): %s (Bild %d)\n",
               g_probe_saw_1200>=0?"JA":"NEIN", g_probe_saw_1200);
        printf("  kleinster Abstand zu x=1200 nach scd_vm_tick: %d\n", g_probe_minabs);
        printf("  jemals exakt x==1200 (Pos_set): %s (Bild %d)\n",
               f_ever_1200 >= 0 ? "JA" : "NEIN", f_ever_1200);
        printf("  erstes Bild mit x > -5600     : %d\n", f_cross);
        printf("  clip=1 bild=43 bei Bild %d, x=%d\n", f_bild43, (int)x_bild43);
        printf("  Bilder mit Kamera 12          : %d / 3000\n", cam12_frames);
        printf("  Bilder INNERHALB der Anker-Zone des aktiven Cuts: %d / 3000\n",
               in_quad_frames);
        printf("  Endposition: (%d,%d,%d) clip=%d bild=%d st=%d grid=%02X\n",
               (int)b->x,(int)b->y,(int)b->z,(int)b->motion,(int)b->anim_frame,
               b->state,(unsigned)b->grid_id);
    }
    return 0;
}
