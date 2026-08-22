/* probe_1090_gate_selfdoor.c — MESSSONDE (kein ctest, reine Diagnose)
 *
 * FRAGE: ROOM1090 hat GENAU ZWEI Cut_chg, die den Spieler NICHT mitversetzen —
 *   sub03 @0x24E6  Cut_chg 5   (Anker = RVD-Eintrag camFrom=5 @Datei 0x03AC, x[-5600..4300])
 *   sub03 @0x2510  Cut_chg 6   (Anker = RVD-Eintrag camFrom=6 @Datei 0x03E8, x[-1500..4200]
 *                               z[-5300..2200])
 * Beide sind nur dann konsistent, wenn der Spieler beim START von sub03 am
 * SELBST-TUER-Spawn (1252,-1800,-2529) steht. Genau den liefert AOT-Slot 3, den sub00
 * @0x2352 als `Door_aot_set 3` (Ziel ROOM1090 selbst, Eintritts-Cut 6) installiert und den
 * sub06 @0x2726 per `Aot_on 3` (Opcode 0x47 = FEUER-JETZT, LAB_800407bc @0x8004082c
 * `jalr PTR_8007469c[rec[0]]`) ausloest.
 *
 * Betritt der Port ROOM1090 dagegen mit flag(3,0x84)==1 durch die ROOM1050-TUER
 * (Spawn -10100,-1800,4200), dann feuert sub00 @0x22E0 `Evt_exec sub03` und `Cut_chg 5`
 * laeuft, waehrend der Spieler bei x=-10100 steht — ausserhalb des Cut-5-Ankers, also
 * (byte-true FUN_8001e8c8 @0x8001e97c `beq v0,zero,0x8001e9c0` -> FUN_8001ef54 OHNE
 * Geometrie-Emitter) GAR NICHT GEZEICHNET. Das ist exakt der in df61447a gemessene Zustand.
 *
 * Diese Sonde prueft die VORBEDINGUNG dafuer: fuehrt der Port die Original-Kette
 *   flag(3,0x85)=1 (aus ROOM1000 @0x0D0C)  ->  AOT 2 = sce-3 Event sub06 (@0x2336)
 *   + Door_aot_set 3 (@0x2352)  ->  sub06 setzt flag(3,0x81)/flag(3,0x84) + Aot_on 3
 *   ->  Selbst-Tuer nach (1252,-1800,-2529)/Cut 6  ->  sub00 -> sub03
 * ueberhaupt aus? Wenn ja, ist flag(3,0x84) beim naechsten ROOM1050-Tuereintritt bereits
 * von sub03 @0x24CE geloescht und der Fehlzustand unerreichbar. Wenn nein, bleibt
 * flag(3,0x84) gesetzt und JEDER spaetere Tuereintritt zeigt die Cutscene ohne Leon.
 *
 * Gemessen wird ausschliesslich mit dem ECHTEN Bild-Ablauf des Ports
 *   scd_vm_tick -> Walker -> cine/pmode -> re15_msg_tick -> re15_cam_present_tick
 *   -> re15_game_step
 * (identisch zu probe_1090_trigger_walk.c, main.c 3626/3667/3682/3751/3951/4810).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static int                s_cine_was_active = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
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
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Der EXAKTE Renderer-Test: FUN_8001e8c8 @0x8001e974 jal FUN_80014368(entity+0x34,
 * DAT_800ac790) / @0x8001e97c beq -> Zweig OHNE Emitter. */
static int leon_drawn(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int16_t xs[4] = {0}, zs[4] = {0};
    if (!re15_rdt_get_region_quad(&s_rdt, s_shown, xs, zs)) return 1;
    return re15_aot_point_in_quad(pl->x, pl->z, xs, zs) ? 1 : 0;
}

static void pline(const char *tag, int f)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("%-8s f%-4d pos=(%7ld,%7ld,%7ld) yaw=%5d | cam=%2u shown=%2d DRAWN=%d "
           "| f3.81=%d f3.84=%d f3.85=%d f2.07=%d pmode=%d\n",
           tag, f, (long)pl->x, (long)pl->y, (long)pl->z, (int)pl->rot_y,
           (unsigned)g_scd.cam_id, s_shown, leon_drawn(),
           re15_game_flag_get(3, 0x81), re15_game_flag_get(3, 0x84),
           re15_game_flag_get(3, 0x85), re15_game_flag_get(2, 0x07),
           (int)g_scd.player_mode);
}

static void dump_slots(const char *tag)
{
    printf("%s AOT2 type=%d ev=%d aktiv=%d | AOT3 type=%d ev=%d aktiv=%d dest=%u cut=%u "
           "spawn=(%d,%d,%d)\n", tag,
           (int)g_aot.slots[2].type, (int)g_aot.slots[2].event_id, (int)g_aot.slots[2].active,
           (int)g_aot.slots[3].type, (int)g_aot.slots[3].event_id, (int)g_aot.slots[3].active,
           (unsigned)g_aot.door_params[3].dest_room, (unsigned)g_aot.door_params[3].target_cut,
           (int)g_aot.door_params[3].spawn_x, (int)g_aot.door_params[3].spawn_y,
           (int)g_aot.door_params[3].spawn_z);
}

/* Eintritt wie re15_room_apply_pending: Spawn + Band + Eintritts-Cut in work_vars[0x0A]. */
static void enter_room(int32_t x, int32_t y, int32_t z, int yaw, int entry_cut)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was_active = 0;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = yaw;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    s_shown = entry_cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, entry_cut);
    g_scd.cut_auto_enabled = 1;
    for (int f = 0; f < 30; f++) frame(0, 0);
}

static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < (long long)tol*tol) return 1;
        if (g_room_change.pending) return 2;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        int want = ((int)(ang + 0.5)) & 0x0FFF;
        int err  = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        uint16_t bits = (err > 60) ? RE15_PAD_BIT_RIGHT
                      : (err < -60) ? RE15_PAD_BIT_LEFT : RE15_PAD_BIT_UP;
        int32_t bx = pl->x, bz = pl->z; int br = pl->rot_y;
        frame(bits, 0);
        if (pl->x == bx && pl->z == bz && pl->rot_y == br) { if (++stuck > 25) return 0; }
        else stuck = 0;
    }
    return 0;
}

int main(void)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", RE15_ASSET_PSX_DIR);
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    scd_vm_init();
    re15_inv_load_briefing();
    /* ROOM1000 @0x0D0C `Set(3,0x85,1)` — der Spieler war schon im Verbindungsgang. */
    re15_game_flag_set(3, 0x85, 1);

    printf("=== M1: Eintritt aus ROOM1050 (Tuer-Slot 5 @Datei 0x0B7A: -10100,-1800,4200 Cut 3) ===\n");
    enter_room(-10100, -1800, 4200, 3072, 3);
    pline("[spawn]", 0);
    dump_slots("[slots]");

    printf("\n=== M2: zum Tor laufen (AOT 2 = Aot_set @0x2336, Mitte ~ (1825,-325)) ===\n");
    static const int32_t wp[][2] = {
        { -10100, 7000 }, { -6500, 7000 }, { -3500, 6500 }, { -3000, 4200 },
        { -1500, 1000 }, { 1000, -400 }, { 1825, -325 },
    };
    for (unsigned k = 0; k < sizeof wp / sizeof wp[0]; k++) {
        int r = drive_to(wp[k][0], wp[k][1], 900, 400);
        printf("   wp%u -> (%6ld,%6ld) : %-9s ", k, (long)wp[k][0], (long)wp[k][1],
               r == 2 ? "ROOMCHG" : r ? "erreicht" : "STUCK");
        pline("", (int)k);
        if (r == 2) break;
    }

    printf("\n=== M3: 400 Bilder stehen bleiben (sub06 -> Aot_on 3 -> Selbst-Tuer?) ===\n");
    for (int f = 0; f < 400; f++) {
        frame(0, 0);
        if ((f % 25) == 0) pline("[wait]", f);
        if (g_room_change.pending) { printf("   ROOM-CHANGE angefordert bei f%d\n", f); break; }
    }
    pline("[m3end]", 0);
    dump_slots("[slots]");
    printf("   g_room_change: pending=%d room=%04X pos=(%d,%d,%d) cut=%d\n",
           (int)g_room_change.pending, (unsigned)g_room_change.room_id,
           (int)g_room_change.x, (int)g_room_change.y, (int)g_room_change.z,
           (int)g_room_change.target_cut);

    printf("\n=== M4: der Zustand, den df61447a gemessen hat — flag(3,0x84)=1 + ROOM1050-Tuer ===\n");
    printf("   (nur ausgefuehrt, wenn flag(3,0x84) nach M3 noch gesetzt ist)\n");
    if (re15_game_flag_get(3, 0x84)) {
        int keep81 = re15_game_flag_get(3, 0x81);
        enter_room(-10100, -1800, 4200, 3072, 3);
        printf("   (flag 3.81=%d beim Wiedereintritt)\n", keep81);
        int notdrawn = 0;
        for (int f = 0; f < 900; f++) {
            frame(0, 0);
            if (!leon_drawn()) notdrawn++;
            if ((f % 50) == 0) pline("[re]", f);
        }
        printf("   NICHT GEZEICHNET in %d von 900 Bildern\n", notdrawn);
    } else {
        printf("   flag(3,0x84) ist 0 -> Fehlzustand nicht erreichbar (Original-Verhalten)\n");
    }

    free(s_raw);
    return 0;
}
