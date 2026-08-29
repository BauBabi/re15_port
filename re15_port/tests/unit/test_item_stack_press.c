/* test_item_stack_press.c — PIN (Nutzer-Report 2026-08-29 "Item-Stapel"):
 * Liegen mehrere Item-AOTs uebereinander (Original-Muster, 38 Paare game-weit, z.B.
 * ROOM1020 main00 @0x1d6a/@0x1d80 = 2x H.GUN BULLETS x15 auf identischem Rect,
 * ebenso ROOM1210 @0x1e70/@0x1e86), dann gibt das Original das ZWEITE Item erst nach
 * einem NEUEN SQUARE-Druck:
 *   - Frame-Ordnung des Hauptloops FUN_8001c6e8: Spieler-Dispatcher @0x8001ce0c
 *     (jal 0x80031c44, einziger Weg zum ACTION-Scan @0x80031fe4) laeuft VOR der
 *     Item-Modal-FSM @0x8001ce34 (jal 0x8001db28) — der Modal-Abschluss 6->7->0
 *     liegt also NACH jedem moeglichen Scan desselben Frames.
 *   - Der Confirm VERBRAUCHT die Flanke: `ori v0,zero,0xffff` @0x80028578 +
 *     `sw v0,DAT_800ac768` @0x80028588 -> edge=(prev^held)&held des Folgeframes = 0.
 *   - Re-Arm-Guard des sce-9-Handlers: `lbu DAT_80072d3b` @0x8004332c +
 *     `bne v0,zero,ret` @0x80043334 (Modal-State != 0 -> kein Arm).
 * Der Port bildet das ueber die Frame-Ordnung ab (main.c: re15_item_modal_tick NACH
 * re15_game_step, seit 2026-08-29; vorher tickte das Modal VOR dem Step und der
 * Bestaetigungs-SQUARE feuerte im selben Host-Frame das zweite AOT).
 *
 * Dieser Test faehrt den ECHTEN Bild-Ablauf des Ports in ROOM1020 (echte RDT):
 *   [Modal aktiv? kein scd_vm_tick] -> re15_game_step -> [Modal aktiv? modal_tick]
 * und pinnt den ZUSTAND: 1 Druck = 1 Grant; ohne neuen Druck bleibt es bei 1;
 * der 2. Druck holt das 2. Item.
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
#include "re15_item_modal.h"
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

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                g_fail = 0;

#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* raw SQUARE -> virtuelles Confirm-Bit 0x4000 (Preset-0-Tabelle @0x80073dbc[14],
 * wave-6 finding 4) — mehr braucht der Yes/No-Prompt hier nicht. */
static uint16_t virt(uint16_t raw)
{
    return (raw & RE15_PAD_BIT_SQUARE) ? 0x4000u : 0u;
}

/* Der Bild-Ablauf der Plattform-Schleife (main.c) in der SEIT 2026-08-29 gueltigen
 * Ordnung. Genau diese Reihenfolge ist der gepinnte Mechanismus. */
static int s_cine_was_active = 0;

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    if (!re15_item_modal_active())
        scd_vm_tick();                    /* main.c: SCD-Tick unterbleibt bei aktivem Modal */
    re15_actor_step_all_walkers();
    {   /* Cine/Letterbox-Handling wie main.c / probe_1090_gate_selfdoor.c — ohne das bleibt
         * player_mode nach dem Raum-Eintritt auf 2 und der Normalpfad (ACTION-Scan) laeuft nie. */
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
    if (re15_item_modal_active())         /* NACH dem Step — Original @0x8001ce34 nach @0x8001ce0c */
        re15_item_modal_tick(virt(edge), virt(held));
}

static int qty_sum(uint8_t id)
{
    int q = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == id) q += g_inv.slots[i].qty;
    return q;
}

/* Bis der Yes/No-Prompt Eingabe annimmt (Typewriter fertig), ohne Input ticken. */
static int run_to_prompt_ready(void)
{
    int guard = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 400)
        frame(0, 0);
    return re15_item_modal_active() && re15_item_modal_prompt_ready();
}

int main(void)
{
    char rp[600];
    size_t rawsz = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1020.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *rawbuf = slurp(rp, &rawsz);
    if (!rawbuf) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(rawbuf, rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    printf("=== Item-Stapel-Pin ROOM1020 (2x 0x15 x15, main00 @0x1d6a slot3 tk2 / @0x1d80 slot4 tk227) ===\n");

    scd_vm_init();
    re15_inv_load_briefing();
    const int q0 = qty_sum(0x15);

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1020; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    /* Mitte des Stapel-Rects [-24900,-10700,2700,1700] -> (-23550,-9850); floor 0 (AOT-Byte +4 = 0). */
    pl->x = -23550; pl->y = 0; pl->z = -9850; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    g_scd.cut_auto_enabled = 1;
    for (int f = 0; f < 30; f++) frame(0, 0);

    CHECK("beide Stapel-AOTs installiert (slot3+slot4 aktiv, sce-9)",
          g_aot.slots[3].active && g_aot.slots[4].active);
    printf("  [pre] pmode=%d cam=%u slot3.type=%d slot3.flags=%02x slot3.band=%d "
           "slot4.type=%d pl=(%ld,%ld) floor=%d band=%d\n",
           (int)g_scd.player_mode, (unsigned)g_scd.cam_id,
           (int)g_aot.slots[3].type, (unsigned)g_aot.slots[3].sce_flags, (int)g_aot.slots[3].band,
           (int)g_aot.slots[4].type, (long)pl->x, (long)pl->z, (int)pl->floor,
           re15_collision_debug_band());
    CHECK("Vorbedingung: player_mode == 0 (Normalpfad erreichbar)", g_scd.player_mode == 0);

    {   /* Die Kollision kann den Spawn aus dem Rect geschoben haben (gemessen: z -9850 ->
         * -10970). Items sind FORWARD-only (flags 0x31, kein 0x40-Centre): der Punkt 620
         * voraus muss im Rect liegen -> auf die AOT-Mitte ausrichten. */
        int32_t dx = g_aot.slots[3].x - pl->x, dz = g_aot.slots[3].z - pl->z;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        pl->rot_y = (int16_t)(((int)(ang + (ang >= 0 ? 0.5 : -0.5))) & 0x0FFF);
        printf("  [aim] pl=(%ld,%ld) -> aot=(%ld,%ld) rot=%d\n",
               (long)pl->x, (long)pl->z, (long)g_aot.slots[3].x, (long)g_aot.slots[3].z,
               (int)pl->rot_y);
    }

    /* --- 1. Druck: ein AOT feuert, Modal laeuft, Confirm=Yes --- */
    frame(0, RE15_PAD_BIT_SQUARE);
    CHECK("1. Druck armt das Modal (Trigger-Frame, INIT tickt im selben Bild)",
          re15_item_modal_active());
    CHECK("Grant ist DEFERRED (noch nichts im Inventar)", qty_sum(0x15) == q0);

    CHECK("Prompt wird eingabebereit", run_to_prompt_ready());
    frame(0, RE15_PAD_BIT_SQUARE);        /* Confirm=Yes — Modal endet 6->7->0 in diesem Bild */
    CHECK("Confirm beendet das Modal im selben Bild", !re15_item_modal_active());
    CHECK("genau EIN Grant (+15)", qty_sum(0x15) == q0 + 15);

    /* --- KERN-PIN: ohne neuen Druck darf NICHTS mehr feuern --- */
    for (int f = 0; f < 20; f++) frame(0, 0);
    CHECK("KEIN Auto-Pickup des 2. Items ohne neuen Druck (20 Bilder)",
          qty_sum(0x15) == q0 + 15 && !re15_item_modal_active());
    CHECK("das 2. Stapel-AOT ist noch scharf", g_aot.slots[3].active || g_aot.slots[4].active);

    /* --- 2. Druck: jetzt (und erst jetzt) kommt das 2. Item --- */
    frame(0, RE15_PAD_BIT_SQUARE);
    CHECK("2. Druck armt das Modal erneut", re15_item_modal_active());
    CHECK("Prompt 2 wird eingabebereit", run_to_prompt_ready());
    frame(0, RE15_PAD_BIT_SQUARE);
    CHECK("2. Grant (+15) nach neuem Druck", qty_sum(0x15) == q0 + 30);
    CHECK("beide Stapel-AOTs verbraucht", !g_aot.slots[3].active && !g_aot.slots[4].active);
    CHECK("beide taken-Bits gesetzt (zone 9: 2 + 227)",
          re15_game_flag_get(9, 2) && re15_game_flag_get(9, 227));

    free(rawbuf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
