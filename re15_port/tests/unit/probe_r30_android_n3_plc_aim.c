/* probe_r30_android_n3_plc_aim.c - RIEGEL N3 (Runde 30, Thema C, Nebenbefund N3).
 *
 * Befund (Dossier analysis/befunde_runde30/android-r1-toggle.md, Abschnitt 6, N3): beginnt eine
 * Skript-Szene, waehrend der Spieler zielt, bleibt die Zielphase im Port eingefroren stehen
 * (Sonde probe_r30_android_r1_toggle Fall F: Zielphase aktiv 40 von 40 Bildern). Mit der
 * R1-Raste haeufiger sichtbar, mit gehaltenem R1 heute schon erreichbar.
 *
 * Original (selbst disassembliert, info/Re1.5/PSX.EXE): die Skript-Befehle an den Spieler
 * schreiben das KOMMANDOWORT (Spieler+0x4 = 0x800aca58) auf 4 und ersetzen damit den ganzen
 * cmd-1-Zustand samt Zielaktion (Zieleintritt = sw 0x701,0x800aca58 @0x80032020 = cmd 1,
 * Substate 7):
 *   Plc_motion (0x3F) Handler 0x80041b90:
 *     80041ba0  lw   v0,340(a0)      Work-Entity (Thread+0x154)
 *     80041ba4  ori  v1,zero,0x4
 *     80041bb0  sb   v1,4(v0)        +0x4 = 4  (Dispatcher-Tabelle 0x80073f90[4] = 0x80030660)
 *     80041bb4  sb   zero,6(v0)      +0x6 = 0
 *     80041bb8  sb   zero,7(v0)      +0x7 = 0
 *     80041bc4  sb   a2,5(v0)        +0x5 = pc[1]  (Substate 7 = Zielen ueberschrieben)
 *   Plc_dest (0x40) Handler 0x80041be4:
 *     80041c14  sb   v0(=4),4(a1)    +0x4 = 4, dazu +0x5 = mode @0x80041c18, +0x6/+0x7 = 0
 *   Plc_ret (0x42) Handler 0x80041f88: sb 1,4 @0x80041f90, sb zero,5/6/7 @0x80041f94-9c
 *     -> zurueck auf cmd 1 / Substate 0 (DECIDE): ein weiter gehaltenes R1 hebt frisch
 *        (andi 0x100 @0x80031ffc).
 * Der Port fuehrt fuer Treffer (cmd 2) und Griff (cmd 5) bereits dieselbe Regel
 * (re15_player_aim_interrupt, game_step_common.c re15_player_stagger_cmd2).
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
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_active(void);
extern void re15_fade_tick(void);
extern void re15_fade_kill(int ch);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint16_t           s_prev_pad;
static int                s_fehler;

static const uint16_t FC_W03[14] = {22,16,52,1,50,30,10,23,1,24,1,24,1,32};

/* Skript-Schnipsel (je eigener Thread, Work-Entity ungesetzt -> Spieler, scd_vm.c):
 * Plc_motion(entity 0, Clip 3, Flags 0) / Plc_dest(0, Mode 9, Bit 0x20, x, z) / Plc_ret,
 * jeweils mit Evt_end (0x01) dahinter. */
static const uint8_t SK_MOTION[] = { SCD_OP_PLC_MOTION, 0x00, 0x03, 0x00, SCD_OP_EVT_END, 0x00 };
static const uint8_t SK_DEST[]   = { SCD_OP_PLC_DEST, 0x00, 0x09, 0x20, 0x30, 0x75, 0x30, 0x75,
                                     SCD_OP_EVT_END, 0x00 };
static const uint8_t SK_RET[]    = { SCD_OP_PLC_RET, SCD_OP_EVT_END, 0x00 };

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

static void bild(uint16_t cur)
{
    uint16_t edge = (uint16_t)(cur & ~s_prev_pad);
    s_prev_pad = cur;
    { const unsigned char *raw; int len, id; re15_msg_tick(&raw, &len, &id); }
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_fade_tick();
}

/* Wie probe_r30_android_r1_toggle.c, aber OHNE die Raum-Threads: der VM-Tick soll hier nur das
 * eine Skript-Schnipsel ausfuehren. */
static void bringup(void)
{
    re15_fade_kill(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 30000; pl->z = 30000;
    re15_collision_set_band(0);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.message_display_frames = 0; g_scd.message_query = 0;
    g_re15_pauseflags = 0;
    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;
    re15_inv_set_prev_equip_slot(0x80);
    re15_player_set_aim_clip_lens(FC_W03, 14);
    re15_player_set_equipped_weapon(3);
    re15_inv_set_equipped_slot(1);
    s_prev_pad = 0;
    for (int i = 0; i < 3; i++) bild(0);
}

static void skript(const uint8_t *code)
{
    int slot = 2;
    g_scd.threads[slot].active = 0;
    scd_thread_start(slot, code);
    scd_vm_tick();
}

static void fall(const char *name, const uint8_t *befehl, const char *adr)
{
    const uint16_t R1 = RE15_PAD_BIT_R1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    bringup();
    printf("\n--- %s ---\n", name);
    int n = 0;
    for (; n < 90 && !re15_player_aim_ready(); n++) bild(R1);
    printf("  R1 gehalten: zielbereit nach %d Bildern (Phase %d)\n", n, phase());
    SOLL(re15_player_aim_ready(), "%s: Waffe kommt nicht hoch", name);

    /* Szene beginnt: Skript-Modus wie im Spiel (Cutscene-Latch) + der Befehl an den Spieler */
    g_scd.player_mode = 2;
    skript(befehl);
    int ph_befehl = phase(), st_befehl = (int)pl->state;
    printf("  nach dem Befehl: Kommandowort(state)=%d Zielphase=%d\n", st_befehl, ph_befehl);
    SOLL(st_befehl == 4, "%s: Kommandowort %d statt 4 (%s)", name, st_befehl, adr);
    SOLL(ph_befehl == 0, "%s: Zielphase %d ueberlebt den Skript-Befehl (%s)", name, ph_befehl, adr);
    int aktiv = 0;
    for (int i = 0; i < 40; i++) {
        g_scd.player_mode = 2;
        bild(R1);
        if (re15_player_aim_active()) aktiv++;
    }
    printf("  40 Bilder Szene (R1 weiter gehalten): Zielphase aktiv in %d\n", aktiv);
    SOLL(aktiv == 0, "%s: Zielphase aktiv in %d von 40 Szenenbildern", name, aktiv);

    /* Szenenende: Plc_ret -> cmd 1 / Substate 0; R1 weiter gehalten hebt frisch */
    skript(SK_RET);
    printf("  Plc_ret: Kommandowort(state)=%d player_mode=%d\n", (int)pl->state, (int)g_scd.player_mode);
    int wieder = -1;
    for (int i = 0; i < 60; i++) {
        bild(R1);
        if (re15_player_aim_ready()) { wieder = i + 1; break; }
    }
    printf("  danach: wieder zielbereit nach %d Bildern\n", wieder);
    SOLL(wieder > 0, "%s: nach Plc_ret hebt gehaltenes R1 die Waffe nicht wieder", name);
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

    printf("===== N3: Skript-Befehl an den Spieler beendet die Zielaktion (cmd 4) =====\n");
    fall("N3-a Plc_motion waehrend des Zielens", SK_MOTION, "sb v1(=4),4(v0) @0x80041bb0");
    fall("N3-b Plc_dest waehrend des Zielens",   SK_DEST,   "sb v0(=4),4(a1) @0x80041c14");

    if (s_fehler) { printf("\nRIEGEL-ROT: %d Abweichung(en)\n", s_fehler); return 1; }
    printf("\nRIEGEL-GRUEN: cmd 4 ersetzt die Zielaktion wie @0x80041bb0 / @0x80041c14\n");
    return 0;
}
