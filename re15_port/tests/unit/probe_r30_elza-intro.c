/* probe_r30_elza-intro.c — MESSSONDE Runde 30, Thema G (Elza-Intro). KEIN ctest, reine Diagnose.
 *
 * Dossier: analysis/befunde_runde30/elza-intro.md
 *
 * NUTZER-BEFUND (woertlich, AUFTRAG.md Abschnitt G): "waehle ich am Anfang Elza aus kommt zum
 * einen im Intro schon kurz ein Bild der Lobby ... breche ich das intro mit square halten ab,
 * spielt es einfach noch einmal ... nach dem Intro stehe ich in der Lobby aber mit Leon statt
 * mit Elza, und ihre Cutscene spielt nicht ab".
 *
 * WAS DIE SONDE MISST (Engine, ohne Fenster — die Bildmessung steht im Dossier):
 *
 *  M1  ROOM1031 MIT flag(3,193)==0  — das ist der Zustand, in dem der Port heute aus der
 *      Vorspann-Montage ROOM1241 ankommt. ROOM1031.RDT main00:
 *          @0x0204A  06 00 2c 00   Ifel_ck
 *          @0x0204E  21 03 c1 00   Ck(3,193,0)
 *          @0x02052  3b 12 02 31 .. 00 24 ..   Door_aot_set Slot 18 -> Stage 0 / Raum 0x24
 *          @0x02072  04 ff 18 0c   Evt_exec sub12
 *      sub12 @0x02976:  22 03 c1 01  Set(3,193,1) / 09 0a 01 00 Sleep 1 / 47 12 Aot_on(18).
 *      Erwartung: nach wenigen Bildern steht ein Raumwechsel nach 0x1241 an = die Montage
 *      laeuft ein ZWEITES Mal, dazwischen ist die Lobby zu sehen.
 *
 *  M2  ROOM1031 MIT flag(3,193)==1 — der Zustand des ORIGINALS nach dem ersten Besuch.
 *          @0x02076  07 00 34 00   Else_ck
 *          @0x0207E  21 03 7d 00   Ck(3,125,0)
 *          @0x02082  3b 13 02 31 .. f8 df 00 00 1c a8 00 04 00 03 06 ..
 *                    Door_aot_set Slot 19 -> Stage 0 / Raum 0x03 (= ROOM1031 SELBST), Cut 6,
 *                    next_pos (-8200,0,-22500)
 *          @0x020A2  04 ff 18 0f   Evt_exec sub15 (Erzaehler: Cut_chg 13, Message 0x14..0x17,
 *                    Set(3,125,1), Set(3,207,1), am Ende @0x02A9E 47 13 Aot_on(19))
 *      Gemessen wird, ob der Port nach Aot_on(19) den Raum NEU EINSTEIGT (main00 + sub00
 *      noch einmal). Im Original tut das JEDE Tuer, auch die Selbst-Tuer: FUN_8001d600
 *      vergleicht nur die STAGE (`beq v1,v0` @0x8001d968) und ruft den Raumlader unbedingt
 *      (`jal 0x800396fc` @0x8001d988), der die SCD-Raum-Init faehrt (`jal 0x8003ef6c`
 *      @0x80039a00: Slot 0 = RDT+0x40 @0x8003efa0, Slot 1 = RDT+0x44 @0x8003efc4).
 *
 *  M3  GEGENPROBE zu M2: dieselbe Fahrt, aber die Sonde stellt g_scd_pending_scenario selbst,
 *      sobald die Selbst-Tuer gefeuert hat und der Port es nicht getan hat. Damit ist
 *      messbar, was NACH dem Neueinstieg passiert: main00 dritter Durchlauf
 *      (Ck(3,125,1) @0x01E54 -> 20x Sce_em_set, Evt_exec sub11; Ck(3,207,1) @0x020AE ->
 *      Evt_exec sub13 = Elzas Lobby-Szene) und ob sub13 bis zum Plc_ret @0x02A56 durchlaeuft.
 *
 *  M4  scd_vm_init() NULLT work_vars[0x10]. Das ist der Port-Zwilling von DAT_800B0FF0, dem
 *      angeforderten PL-Index (`sh a0,4080(at)` @0x8001d558, Vergleich im Raumlader
 *      `lh v1,4080(v1)` @0x80039768 / `beq v0,v1` @0x80039770 / `jal 0x800314b0` @0x80039788).
 *      platform/pc/main.c setzt ihn VOR scd_vm_init() — die Sonde zeigt, dass der Wert das
 *      nicht ueberlebt.
 *
 * Bild-Ablauf wie probe_1090_gate_selfdoor.c (scd_vm_tick -> Walker -> cine/pmode ->
 * re15_msg_tick -> re15_cam_present_tick -> re15_game_step).
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
#include "re15_gameflow.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int event_threads(void)
{
    int n = 0;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++)
        if (g_scd.threads[s].active) n++;
    return n;
}

static int gegner_aktiv(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) n++;
    return n;
}

static void zeile(const char *tag, int f)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("%-7s f%-4d pos=(%6ld,%5ld,%6ld) yaw=%5d cam=%2u | f3.193=%d f3.125=%d f3.207=%d "
           "f2.7=%d f1.27=%d pmode=%d | evt=%d akteure=%d reenter=%d wechsel=%d->%04x\n",
           tag, f, (long)pl->x, (long)pl->y, (long)pl->z, (int)pl->rot_y, (unsigned)g_scd.cam_id,
           re15_game_flag_get(3, 193), re15_game_flag_get(3, 125), re15_game_flag_get(3, 207),
           re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), (int)g_scd.player_mode,
           event_threads(), gegner_aktiv(), g_scd_self_reenter_fired,
           g_room_change.pending, (unsigned)g_room_change.room_id);
}

/* Eintritt wie re15_room_apply_pending: Spawn + Band + Eintritts-Cut. Die Flags bleiben
 * stehen (der Raumlader des Originals fasst die Story-Baenke nicht an — eigener Voll-Scan
 * ueber PSX.EXE + 8 Overlays: 0 fest verdrahtete Zugriffe auf 0x800b0ff8..0x800b1017,
 * analysis/befunde_runde30/r30_elza_flagxref.py). */
static void betrete_1031(int32_t x, int32_t y, int32_t z, int yaw, int entry_cut)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was_active = 0;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1031; g_room_change.pending = 0; g_room_change.room_id = 0;
    g_scd_pending_scenario = -1; g_scd_self_reenter_fired = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = (int16_t)yaw;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    s_shown = entry_cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, (uint8_t)entry_cut);
    g_scd.cut_auto_enabled = 1;
}

static void story_flags_leeren(void)
{
    static const uint8_t b[] = { 193, 125, 207, 111, 112 };
    for (unsigned i = 0; i < sizeof b; i++) re15_game_flag_set(3, b[i], 0);
    re15_game_flag_set(2, 7, 0); re15_game_flag_set(1, 27, 0);
    re15_game_flag_set(4, 15, 0);
}

/* Faehrt bis zu `max` Bilder; liefert das Bild, in dem die Selbst-Tuer (Slot 19) gefeuert
 * hat — erkennbar daran, dass der Spieler auf next_pos X = -8200 steht. -1 = nie. */
static int fahre_bis_selbsttuer(int max, int hilf_dem_port)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 1; f <= max; f++) {
        frame(0, 0);
        if (g_room_change.pending) { zeile("[WECHS]", f); return -2; }
        /* Die Tuer versetzt den Spieler vom Montage-Payload (-26214,0,-3861) auf next_pos
         * (-8200,0,-22500). Gemessen steht er im selben Bild schon auf z=-22303 (die
         * Kollision schiebt ihn 197 Einheiten) — deshalb am X-Wert erkennen, nicht an Z. */
        /* Bau Runde 30 (P3): steigt der Port im Tuer-Bild neu ein, hat sub13 den Spieler im
         * selben Bild schon umgesetzt (Member_set @0x02994) — ohne diese Zusatzbedingung meldete
         * die Sonde nach dem Bau "Selbst-Tuer NICHT erreicht" (gemessen). */
        if (pl->x == -8200 || g_scd_self_reenter_fired) {
            zeile("[TUER]", f);
            if (hilf_dem_port && !g_scd_self_reenter_fired) {
                /* GEGENPROBE M3: was die Engine an dieser Stelle tun MUESSTE
                 * (@0x8001d988 `jal 0x800396fc`, unbedingt). */
                g_scd_pending_scenario = (int)g_aot.door_params[19].target_cut;
                printf("        (Sonde stellt g_scd_pending_scenario=%d selbst)\n",
                       g_scd_pending_scenario);
            }
            return f;
        }
        if ((f % 100) == 0) zeile("[..]", f);
    }
    return -1;
}

int main(void)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1031.RDT", RE15_ASSET_PSX_DIR);
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* Belegbytes aus der ausgelieferten Datei, damit die Sonde nicht auf einem Walker steht. */
    printf("ROOM1031.RDT %lu B\n", (unsigned long)s_rawsz);
    printf("  @0x0204E Ck        : %02x %02x %02x %02x   (erwartet 21 03 c1 00)\n",
           s_raw[0x204E], s_raw[0x204F], s_raw[0x2050], s_raw[0x2051]);
    printf("  @0x02052 Door 18   : op=%02x slot=%02x ... Stage=%02x Raum=%02x Cut=%02x\n",
           s_raw[0x2052], s_raw[0x2053], s_raw[0x2052 + 22], s_raw[0x2052 + 23], s_raw[0x2052 + 24]);
    printf("  @0x02082 Door 19   : op=%02x slot=%02x ... Stage=%02x Raum=%02x Cut=%02x\n",
           s_raw[0x2082], s_raw[0x2083], s_raw[0x2082 + 22], s_raw[0x2082 + 23], s_raw[0x2082 + 24]);
    printf("  @0x02976 sub12     : %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x\n",
           s_raw[0x2976], s_raw[0x2977], s_raw[0x2978], s_raw[0x2979],
           s_raw[0x297A], s_raw[0x297B], s_raw[0x297C], s_raw[0x297D],
           s_raw[0x297E], s_raw[0x297F]);
    printf("  @0x02A9E sub15 Ende: %02x %02x   (erwartet 47 13 = Aot_on 19)\n",
           s_raw[0x2A9E], s_raw[0x2A9F]);

    /* ---------------------------------------------------------------- M4 */
    printf("\n=== M4: ueberlebt work_vars[0x10] ein scd_vm_init()? ===\n");
    scd_vm_init();
    g_gameflow.character = 4;                       /* Elza, @0x801024cc/@0x801024d4 */
    g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);
    printf("  vor  scd_vm_init: work_vars[0x10] = %d\n", (int)g_scd.work_vars[0x10]);
    scd_vm_init();
    printf("  nach scd_vm_init: work_vars[0x10] = %d   (character & 0x0F = %d)\n",
           (int)g_scd.work_vars[0x10], g_gameflow.character & 0x0F);

    re15_inv_load_briefing();

    /* ---------------------------------------------------------------- M1 */
    printf("\n=== M1: ROOM1031, flag(3,193)=0 (Port-Zustand nach ROOM1241) ===\n");
    story_flags_leeren();
    re15_game_flag_set(3, 111, 1);                  /* ROOM1241 sub02 @0x055A */
    re15_game_flag_set(3, 112, 1);                  /* ROOM1241 sub02 @0x055E */
    betrete_1031(-26214, 0, -3861, 0, 0);           /* Payload ROOM1241 Slot 0 @0x051A+14 */
    zeile("[ein]", 0);
    {
        int f, hit = -1;
        for (f = 1; f <= 40; f++) {
            frame(0, 0);
            if (g_room_change.pending) { hit = f; break; }
        }
        zeile("[M1]", f);
        if (hit > 0)
            printf("  ERGEBNIS M1: Raumwechsel nach %04x angefordert in Bild %d "
                   "(flag(3,193) jetzt %d)\n",
                   (unsigned)g_room_change.room_id, hit, re15_game_flag_get(3, 193));
        else
            printf("  ERGEBNIS M1: kein Raumwechsel in 40 Bildern\n");
    }

    /* ---------------------------------------------------------------- M2 */
    printf("\n=== M2: ROOM1031, flag(3,193)=1 (Original-Zustand) — Erzaehler + Selbst-Tuer ===\n");
    story_flags_leeren();
    re15_game_flag_set(3, 111, 1); re15_game_flag_set(3, 112, 1);
    re15_game_flag_set(3, 193, 1);                  /* ROOM1031 sub12 @0x02976 */
    betrete_1031(-26214, 0, -3861, 0, 0);
    zeile("[ein]", 0);
    {
        int ft = fahre_bis_selbsttuer(700, 0);
        if (ft > 0) {
            for (int f = ft + 1; f <= ft + 300; f++) {
                frame(0, 0);
                if (((f - ft) % 100) == 0) zeile("[nach]", f);
            }
            printf("  ERGEBNIS M2: Selbst-Tuer in Bild %d, Neueinstieg=%d, Akteure=%d, "
                   "flag(3,207)=%d, flag(2,7)=%d, pmode=%d\n",
                   ft, g_scd_self_reenter_fired, gegner_aktiv(),
                   re15_game_flag_get(3, 207), re15_game_flag_get(2, 7), (int)g_scd.player_mode);
        } else {
            printf("  ERGEBNIS M2: Selbst-Tuer NICHT erreicht (Rueckgabe %d)\n", ft);
        }
    }

    /* ---------------------------------------------------------------- M3 */
    printf("\n=== M3: GEGENPROBE — wie M2, Sonde erzwingt den Neueinstieg ===\n");
    story_flags_leeren();
    re15_game_flag_set(3, 111, 1); re15_game_flag_set(3, 112, 1);
    re15_game_flag_set(3, 193, 1);
    betrete_1031(-26214, 0, -3861, 0, 0);
    {
        int ft = fahre_bis_selbsttuer(700, 1);
        if (ft > 0) {
            int ende = -1;
            for (int f = ft + 1; f <= ft + 1500; f++) {
                frame(0, 0);
                if (f == ft + 1 || f == ft + 2) zeile("[neu]", f);
                if (((f - ft) % 100) == 0) zeile("[szene]", f);
                /* sub13 ist fertig, wenn flag(3,207) wieder 0 UND das Fenster (2,7)/(1,27)
                 * wieder zu ist (Set @0x02A4A / @0x02A4E, Plc_ret @0x02A56). */
                if (ende < 0 && re15_game_flag_get(3, 207) == 0 &&
                    !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27) && f > ft + 5) {
                    ende = f; zeile("[ENDE]", f);
                    break;
                }
            }
            printf("  ERGEBNIS M3: Neueinstieg=%d, Akteure=%d, Szene zu Ende in Bild %d "
                   "(%d Bilder nach der Tuer)\n",
                   g_scd_self_reenter_fired, gegner_aktiv(), ende, ende > 0 ? ende - ft : -1);
        } else {
            printf("  ERGEBNIS M3: Selbst-Tuer NICHT erreicht (Rueckgabe %d)\n", ft);
        }
    }

    free(s_raw);
    return 0;
}
