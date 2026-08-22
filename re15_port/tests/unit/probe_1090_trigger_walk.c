/* probe_1090_trigger_walk.c — MESSSONDE (kein ctest, reine Diagnose)
 *
 * NUTZER-BEFUND (2026-08-22): "Jetzt laeuft Leon in der ROOM1090-Cutscene nicht mehr in die
 * falsche Richtung, aber VERSCHWINDET VOELLIG."
 *
 * Die alte Sonde probe_1090_cutscene.c misst NICHT, was das Spiel tut: sie ruft
 *   scd_vm_tick(); re15_actors_anim_advance(); re15_actor_step_all_walkers();
 * also den Walker DIREKT und OHNE re15_game_step — damit ohne Kollision
 * (re15_collision_constrain), ohne Spieler-cmd-FSM und ohne den echten Ausloeser.
 * Sie startet sub02 ausserdem am TUERSPAWN (-10100,4200), waehrend das Skript die
 * Cutscene erst bei Kamera 13 (work_vars[0x0A]==13, sub01 @0x23F4) zuendet — dort steht
 * Leon ~8000 Einheiten weiter.
 *
 * Diese Sonde faehrt stattdessen den ECHTEN Bild-Ablauf des Ports
 *   scd_vm_tick -> re15_msg_tick -> re15_cam_present_tick -> re15_game_step
 * (main.c 3626 / 3751 / 3951 / 4810) mit Tank-Pad vom echten Tuerspawn los, laeuft bis der
 * RVD-Zonen-Scan Cut 13 liefert, und protokolliert dann JEDES Bild der Cutscene:
 * Position, Yaw, Kamera, walk_active/mode/fsm, Plc_dest-Ziel und g_re15_pauseflags.
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
#include "re15_fade.h"      /* re15_letterbox_tick — main.c:3688 */

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

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Ein Bild in der EXAKTEN Reihenfolge des PC-Ports (platform/pc/main.c).
 * ⚠️ Die beiden mittleren Bloecke (Walker-Schritt + cine_active/player_mode) liegen im
 * Original-Port in main.c und NICHT in re15_game_step. Ohne sie ist die Sonde unehrlich:
 * player_mode bleibt 0, und player_common.c:865 loescht dann JEDEN Frame `walk_active`
 * — die skript-gesteuerte Cutscene-Bewegung findet gar nicht erst statt. */
static int s_cine_was_active = 0;
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();                                             /* main.c:3626 */
    re15_actor_step_all_walkers();                             /* main.c:3667 */
    {                                                          /* main.c:3682-3714 */
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
    re15_msg_tick(&raw, &len, &id);                            /* main.c:3751 */
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;   /* main.c:3951 */
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);                                    /* main.c:4810 */
}

/* Der EXAKTE Sichtbarkeits-Test des Renderers (platform/pc/main.c:5457):
 *   cam_has_region = re15_rdt_get_region_quad(rdt, ANGEZEIGTER Cut, xs, zs)
 *   player_visible = !(cam_has_region && !point_in_quad(pl.x, pl.z, xs, zs))
 * s_shown ist der ANGEZEIGTE Cut (main.c s_last_cut_idx), nicht g_scd.cam_id. */
static int leon_visible(int *has_region_out)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int16_t xs[4] = {0}, zs[4] = {0};
    int has = re15_rdt_get_region_quad(&s_rdt, s_shown, xs, zs);
    if (has_region_out) *has_region_out = has;
    if (!has) return 1;
    return re15_aot_point_in_quad(pl->x, pl->z, xs, zs) ? 1 : 0;
}

static void pline(const char *tag, int f)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int has = 0; int vis = leon_visible(&has);
    printf("%-6s f%-4d pos=(%7ld,%7ld,%7ld) yaw=%5d mo=%3d | wact=%d wmode=%d wfsm=%d "
           "dest=(%6d,%6d) fbit=%2d | cam=%2u shown=%2d VIS=%d(rgn=%d) pause=%08lX pmode=%d\n",
           tag, f, (long)pl->x, (long)pl->y, (long)pl->z, (int)pl->rot_y, (int)pl->motion,
           (int)pl->walk_active, (int)pl->walk_mode, (int)pl->walk_fsm,
           (int)pl->walk_dest_x, (int)pl->walk_dest_z, (int)pl->walk_flag_bit,
           (unsigned)g_scd.cam_id, s_shown, vis, has,
           (unsigned long)g_re15_pauseflags,
           (int)g_scd.player_mode);
}

static void room_boot(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was_active = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    /* Tuerspawn wie im echten Spiel (re15_room_spawns[ROOM1090] + debug.log-Messung). */
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    /* ⚠️ EHRLICHKEIT: exakt die Schritte von re15_room_apply_pending (room_common.c:343/391).
     * (7) scd_room_reenter bekommt den EINTRITTS-CUT der Tuer (re15_room_spawns[ROOM1090].cut = 3)
     *     als work_vars[0x0A]; nur damit waehlt sub00s Switch den Cut_chg 0x0B = Kamera 11.
     *     Mit der 0 der ersten Fassung landete die Sonde auf Kamera 8 — ein anderer Raumzustand.
     * (13) Der Boden-Band kommt aus dem Spawn-Y (-(Y/0x708)), NICHT hart 0; mit 0 schob die
     *     Kollision Leon sofort von z=4200 auf z=8926. */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, /*entry cut*/ 3);
    g_scd.cut_auto_enabled = 1;                       /* (12) */
    for (int f = 0; f < 30; f++) frame(0, 0);
}

/* --- Tank-Token-Mitschrift, damit derselbe Weg im ECHTEN Spiel per RE15_INPUT_SCRIPT
 *     wiederholbar ist (Muster aus probe_11f0_puzzle_pos.c). --- */
static char     s_tok[16384];
static int      s_toklen  = 0;
static uint16_t s_lastbits = 0xFFFF;
static int      s_runlen   = 0;
static void tok_flush(void)
{
    if (s_lastbits == 0xFFFF || s_runlen == 0) return;
    char L[8]; int n = 0;
    if (s_lastbits & RE15_PAD_BIT_CROSS) L[n++] = 'X';
    if (s_lastbits & RE15_PAD_BIT_UP)    L[n++] = 'U';
    if (s_lastbits & RE15_PAD_BIT_DOWN)  L[n++] = 'D';
    if (s_lastbits & RE15_PAD_BIT_LEFT)  L[n++] = 'L';
    if (s_lastbits & RE15_PAD_BIT_RIGHT) L[n++] = 'R';
    if (n == 0) L[n++] = 'W';
    L[n] = 0;
    s_toklen += snprintf(s_tok + s_toklen, sizeof s_tok - (size_t)s_toklen,
                         "%s%s%.4f", s_toklen ? "," : "", L, s_runlen / 30.0);
    s_runlen = 0;
}
static void tok_add(uint16_t bits)
{
    if (bits != s_lastbits) { tok_flush(); s_lastbits = bits; }
    s_runlen++;
}

/* Ziel anlaufen: erst drehen (in-place, 96/Frame), dann GEHEN (kein Rennen). */
static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol, int verbose)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < (long long)tol*tol) return 1;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        int want = ((int)(ang + 0.5)) & 0x0FFF;
        int err  = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        uint16_t bits = (err > 60) ? RE15_PAD_BIT_RIGHT
                      : (err < -60) ? RE15_PAD_BIT_LEFT
                                    : RE15_PAD_BIT_UP;
        int32_t bx = pl->x, bz = pl->z; int br = pl->rot_y;
        tok_add(bits);
        frame(bits, 0);
        if (verbose && (i % 20) == 0) pline("[nav]", i);
        if (pl->x == bx && pl->z == bz && pl->rot_y == br) { if (++stuck > 25) return 0; }
        else stuck = 0;
        if (g_scd.cam_id == 13) return 2;      /* Ausloeser erreicht */
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

    room_boot();
    printf("=== M1: Spawn wie im echten Spiel? (Erwartung aus debug.log: pos(-10100,4268) cam=11) ===\n");
    pline("[spawn]", 0);

    /* --- M2: zur Ausloeser-Kamera 13 laufen (Wegpunkte umgehen die Wand bei x=-9397) --- */
    printf("\n=== M2: Anmarsch bis Cut 13 (RVD 11 -> 12 -> 13) ===\n");
    static const int32_t wp[][2] = {
        { -10100, 7000 }, { -6500, 7000 }, { -3500, 6500 }, { -3000, 4200 }, { -2000, 3000 },
    };
    int reached = 0;
    for (unsigned k = 0; k < sizeof wp / sizeof wp[0] && !reached; k++) {
        int r = drive_to(wp[k][0], wp[k][1], 700, 400, 0);
        printf("   wp%u -> (%ld,%ld) : %s ", k, (long)wp[k][0], (long)wp[k][1],
               r == 2 ? "CUT13!" : r ? "erreicht" : "STUCK");
        pline("", (int)k);
        if (r == 2) reached = 1;
    }
    if (!reached) {
        /* Fallback-Fegen: in 16 Richtungen je 200 Bilder gehen, bis Cut 13 kippt. */
        printf("   Fallback-Fegen ...\n");
        for (int d = 0; d < 16 && !reached; d++) {
            int32_t tx = (int32_t)(-3000 + 4000.0 * cos(d * 6.2831853 / 16.0));
            int32_t tz = (int32_t)( 4000 + 4000.0 * sin(d * 6.2831853 / 16.0));
            if (drive_to(tx, tz, 300, 300, 0) == 2) reached = 1;
        }
    }
    printf("   -> Cut 13 %s\n", reached ? "ERREICHT" : "NICHT erreicht");
    pline("[trig]", 0);
    tok_flush();
    printf("\n[TANKPFAD] RE15_INPUT_SCRIPT=\"%s\"\n\n", s_tok);

    /* --- M3: die Cutscene Bild fuer Bild --- */
    printf("\n=== M3: Cutscene-Verlauf (jedes Bild, Pad losgelassen) ===\n");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t minx = pl->x, maxx = pl->x, minz = pl->z, maxz = pl->z;
    int walk_seen = 0;
    for (int f = 0; f < 900; f++) {
        frame(0, 0);
        if (pl->x < minx) minx = pl->x;   if (pl->x > maxx) maxx = pl->x;
        if (pl->z < minz) minz = pl->z;   if (pl->z > maxz) maxz = pl->z;
        if (pl->walk_active) walk_seen = 1;
        if (f < 400 || (f % 10) == 0) pline("[cs]", f);
    }
    printf("\n=== ERGEBNIS ===\n");
    printf("   walk_active jemals gesehen: %d\n", walk_seen);
    printf("   (VIS=0 = Leon wird NICHT gezeichnet — Renderer-Gate main.c:5457)\n");
    printf("   Positions-Huelle waehrend der Cutscene: x[%ld..%ld] z[%ld..%ld]\n",
           (long)minx, (long)maxx, (long)minz, (long)maxz);
    pline("[end]", 0);
    free(s_raw);
    return 0;
}
