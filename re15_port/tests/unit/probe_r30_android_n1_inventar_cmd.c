/* probe_r30_android_n1_inventar_cmd.c - RIEGEL N1 (Runde 30, Thema C, Nebenbefund N1).
 *
 * Befund (Dossier analysis/befunde_runde30/android-r1-toggle.md, Abschnitt 6, N1): nach dem
 * Schliessen des Inventars setzt der Port das Spieler-Kommandoregister nicht zurueck. Eine
 * gehobene Waffe bleibt in der Zielphase stehen, und der Klassen-Latch des Messers klebt an
 * einer im Inventar angelegten Schusswaffe (Sonde probe_r30_android_r1_toggle Fall D HALTEN:
 * Phase 2, melee_latch=1, Waffe 3). Mit gehaltenem R1 (Tastatur, Gamepad) heute erreichbar.
 *
 * Original (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Transitions-FSM FUN_8001c958, Tabelle @0x8001069c ([1] = 0x8001ca98 Stufe 2):
 *     8001cb3c  addiu a1,a1,24636        a1 = 0x8004603c (Status-Task)
 *     8001cb40  jal   0x80029a98         Task 1 := Status-Task (sw a1 @0x80029aac, sh 2 @0x80029abc)
 *     8001cb48  jal   0x80029ac8         Task 0 gibt ab (ChangeTh @0x80029ae4); die Status-Task
 *                                        suspendiert Task 0 sofort: FUN_80029bf8(0) @0x800460c4
 *                                        (ori 0x40 @0x80029c10) - Task 0 laeuft erst nach dem
 *                                        Menue weiter (FUN_80029c2c, andi 0xffbf @0x80029c44)
 *     8001cb50..cb74                      aca3c &= ~(0x40|0x8000), j 0x8001cbac
 *     8001cbac  ori v0,zero,0x3 / sb v0,0x800b5359 @0x8001cbb4   Zustand 3, faellt durch nach
 *     8001cbb8..cc28 (Zustand-3-Rumpf, ohne Verzweigung):
 *     8001cbdc  sb zero,0x800aca58       SPIELER-KOMMANDOWORT := 0
 *   Der cmd-0-Handler 0x800318f8 (Tabelle 0x80073f90[0]) raeumt danach Aim/Turn und stellt das
 *   Wort per sw 1,0x800aca58 @0x8003192c auf cmd 1 / Substate 0 - die Zielaktion 7
 *   (sw 0x701,0x800aca58 @0x80032020) ist damit weg.
 *
 * Riegel: nach dem Schliessen steht die Zielphase auf 0, das Kommandowort auf 1, die
 * Eintritts-Pose ist scharf; ein weiter gehaltenes R1 hebt die Waffe FRISCH (Zieleintritt
 * andi 0x100 @0x80031ffc) - mit der neu angelegten Browning ohne Messer-Latch.
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
#include "re15_itembox.h"
#include "re15_msg.h"
#include "re15_menu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_phase_debug(void);
extern int  re15_player_aim_melee_dbg(void);
extern int  re15_player_aim_ready(void);
extern int  re15_player_entry_pose_latch(void);
extern void re15_fade_tick(void);
extern void re15_fade_kill(int ch);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint16_t           s_prev_pad;
static int                s_fehler;

/* PL00W03 (Browning) Cliplaengen wie probe_r30_android_r1_toggle.c */
static const uint16_t FC_W03[14] = {22,16,52,1,50,30,10,23,1,24,1,24,1,32};

#define SOLL(cond, ...) do { if (!(cond)) { s_fehler++; printf("  RIEGEL-ROT: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int phase(void) { return re15_player_aim_phase_debug() & 0x0f; }
static int mag_qty(void)
{
    int s = re15_inv_equipped_slot();
    if (s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    return (int)g_inv.slots[s].qty;
}

/* EIN Bild mit gehaltenen Pegeln (R1 = Halte-Taste wie Tastatur/Gamepad). */
static void bild(uint16_t cur)
{
    uint16_t edge = (uint16_t)(cur & ~s_prev_pad);
    s_prev_pad = cur;
    { const unsigned char *raw; int len, id; re15_msg_tick(&raw, &len, &id); }
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_fade_tick();
}

static void bringup(int waffe, int mag)
{
    for (int i = 0; i < 300 && re15_menu_gameplay_frozen() && !re15_menu_is_open(); i++) {
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        re15_game_step(&s_ctx); re15_fade_tick();
    }
    if (re15_menu_is_open()) re15_menu_toggle();
    re15_fade_kill(0);
    re15_itembox_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    g_re15_pauseflags = 0;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 60; i++) scd_vm_tick();
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 30000; pl->z = 30000;
    re15_collision_set_band(0);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.message_display_frames = 0; g_scd.message_query = 0;
    g_re15_pauseflags = 0;

    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;             /* KNIFE    */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = (uint8_t)mag;  /* BROWNING */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;            /* H.GUN BULLETS */
    re15_inv_set_prev_equip_slot(0x80);
    re15_player_set_aim_clip_lens(FC_W03, 14);
    re15_player_set_equipped_weapon(waffe);
    re15_inv_set_equipped_slot(waffe == 3 ? 1 : 0);
    s_prev_pad = 0;
    for (int i = 0; i < 3; i++) bild(0);   /* Eintritts-Pose (zwei Bilder) abarbeiten */
}

/* Inventar mit gehaltenem R1 oeffnen und wieder schliessen; neu_slot >= 0 legt im Menue diese
 * Waffe an (Cursor DAT_800b25c8, Commit beim Schliessen @0x80046654-66c8). Rueckgabe: 0 ok. */
static int inventar_auf_zu(int neu_slot, int *o_phase_im_menue)
{
    const uint16_t R1 = RE15_PAD_BIT_R1;
    bild((uint16_t)(R1 | RE15_PAD_BIT_START));
    int i = 0;
    for (; i < 30 && !re15_menu_is_open(); i++) bild(R1);
    if (!re15_menu_is_open()) return -1;
    for (i = 0; i < 10; i++) bild(R1);
    *o_phase_im_menue = phase();
    if (neu_slot >= 0) re15_inv_set_equipped_slot(neu_slot);
    bild((uint16_t)(R1 | RE15_PAD_BIT_START));
    for (i = 0; i < 200 && re15_menu_gameplay_frozen(); i++) bild(R1);
    return re15_menu_gameplay_frozen() ? -2 : 0;
}

static void fall(const char *name, int waffe, int neu_slot)
{
    const uint16_t R1 = RE15_PAD_BIT_R1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    bringup(waffe, 15);
    printf("\n--- %s ---\n", name);
    int n = 0;
    for (; n < 90 && !re15_player_aim_ready(); n++) bild(R1);
    printf("  R1 gehalten: zielbereit nach %d Bildern (Phase %d, melee_latch=%d)\n",
           n, phase(), re15_player_aim_melee_dbg());
    SOLL(re15_player_aim_ready(), "%s: Waffe kommt vor dem Inventar nicht hoch", name);
    int ph_menue = -1;
    int rc = inventar_auf_zu(neu_slot, &ph_menue);
    SOLL(rc == 0, "%s: Inventar oeffnet/schliesst nicht (rc %d)", name, rc);
    int ph_zu = phase(), st_zu = (int)pl->state, latch_zu = re15_player_entry_pose_latch();
    printf("  im Menue: Zielphase=%d (eingefroren)\n", ph_menue);
    printf("  Inventar zu: Waffe=%d Zielphase=%d Kommandowort(state)=%d Eintritts-Pose=%d\n",
           re15_player_equipped_weapon(), ph_zu, st_zu, latch_zu);
    /* @0x8001cbdc sb zero,0x800aca58 -> cmd-0 -> sw 1 @0x8003192c: Zielaktion weg, Wort = 1 */
    SOLL(ph_zu == 0, "%s: Zielphase ueberlebt das Inventar (Phase %d statt 0; @0x8001cbdc)", name, ph_zu);
    SOLL(st_zu == 1, "%s: Kommandowort nach dem Inventar %d statt 1 (@0x8003192c)", name, st_zu);
    SOLL(latch_zu == 1, "%s: Eintritts-Pose nach dem Inventar nicht scharf (%d; cmd-0 @0x80031c10-c24)", name, latch_zu);
    /* R1 weiter gehalten: frisches Heben (Zieleintritt andi 0x100 @0x80031ffc) */
    int wieder = -1;
    for (int i = 0; i < 90; i++) {
        bild(R1);
        if (re15_player_aim_ready()) { wieder = i + 1; break; }
    }
    printf("  R1 weiter gehalten: wieder zielbereit nach %d Bildern, melee_latch=%d\n",
           wieder, re15_player_aim_melee_dbg());
    SOLL(wieder > 0, "%s: Waffe kommt nach dem Inventar nicht wieder hoch", name);
    SOLL(re15_player_aim_melee_dbg() == (re15_player_equipped_weapon() <= 2 ? 1 : 0),
         "%s: Klassen-Latch passt nicht zur Waffe %d (melee_latch=%d)", name,
         re15_player_equipped_weapon(), re15_player_aim_melee_dbg());
    int m0 = mag_qty();
    for (int i = 0; i < 40; i++) bild((uint16_t)(R1 | ((i < 3) ? RE15_PAD_BIT_SQUARE : 0)));
    printf("  Viereck: Magazin %d -> %d\n", m0, mag_qty());
    if (re15_player_equipped_weapon() == 3)
        SOLL(m0 - mag_qty() == 1, "%s: Browning feuert nach dem Inventar nicht genau einmal (%d -> %d)",
             name, m0, mag_qty());
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("===== N1: Inventar-Ende setzt das Kommandoregister zurueck (@0x8001cbdc) =====\n");
    fall("N1-a Browning gehoben, Inventar auf/zu, R1 gehalten", 3, -1);
    fall("N1-b Messer gehoben, im Inventar Browning angelegt, R1 gehalten", 1, 1);

    if (s_fehler) { printf("\nRIEGEL-ROT: %d Abweichung(en)\n", s_fehler); return 1; }
    printf("\nRIEGEL-GRUEN: das Inventar-Ende raeumt die Zielaktion wie @0x8001cbdc\n");
    return 0;
}
