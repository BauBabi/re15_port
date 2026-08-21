/* test_rvd_scan_ungated_by_climb.c — NEGATIV-TEST zu Kandidat 2 aus Commit 3b141d9d:
 * "startet in einem Bild ein Klettern oder eine Treppe, wird re15_aot_scan GAR NICHT gerufen".
 *
 * === ORIGINAL-BELEG (ghidra1_V2.txt, selbst nachdisassembliert) =========================
 * Der RVD-/KAMERA-Zonen-Scan ist ein EIGENER Top-Level-Aufruf im Haupt-Loop und haengt an
 * KEINEM Spieler-Substate. FUN_80014230 hat game-weit GENAU EINEN Aufrufer (XREF[1]:
 * 8001ccec):
 *   8001cc98  lw   v0,-0x35c0(v0)=>DAT_800aca40   \ TOT: v0 wird @0x8001ccb4 sofort
 *   8001cca0  lui  v1,0x80                        | ueberschrieben, kein Branch dazwischen
 *   8001cca4  and  v0,v0,v1                       /
 *   8001ccac  lui  v1,0x2
 *   8001ccb4  lw   v0,-0x38a0(v0)=>DAT_800ac760   ; (PAD_EDGE<<16)|PAD_HELD
 *   8001ccb8  ori  v1,v1,0x4                      ; v1 = 0x00020004
 *   8001ccbc  and  v0,v0,v1
 *   8001ccc0  bne  v0,v1,LAB_8001cccc
 *   8001ccc8  xori s2,s2,0x1                      ; DEV-Hotkey: R3 gehalten + L3-Flanke
 *   8001cccc  bne  s2,zero,LAB_8001ccf4
 *   8001ccd4  lui  v0,0x800b
 *   8001ccd8  lw   v0,-0x35c4(v0)=>DAT_800aca3c
 *   8001cce0  andi v0,v0,0x100                    ; Cut_auto AUS -> Scan ueberspringen
 *   8001cce4  bne  v0,zero,LAB_8001ccf4
 *   8001ccec  jal  FUN_80014230                   ; DER RVD-ZONEN-SCAN
 *   8001ccf0  _clear a0
 * Die einzigen Schreiber von DAT_800ac760/762 sind `sh r4,0xc760(r1)` @0x80030564 (Pad
 * GEHALTEN, aus `lhu DAT_800ac758`) und `sh r4,0xc762(r1)` @0x800305a0 (Pad PRESS-EDGE, aus
 * `lhu DAT_800ac75c`) — eigener wortweiser Store-Scan ueber die ausgelieferte PSX.EXE ueber
 * alle sb/sh/sw mit Immediate 0xc75c/60/62/68/6c. Der s2-Hotkey (R3+L3) ist mit dem
 * Digital-Pad unerreichbar; s2 bleibt 0, der Scan laeuft JEDES Bild.
 *
 * Was Klettern/Treppe im Original unterdruecken, ist AUSSCHLIESSLICH die ACTION-Klasse
 * (@0x80031fc8-fec, identisch in allen vier Sub-ENTRY-Handlern):
 *   80031fc8  beq v0,zero,LAB_80031ff4          ; keine ACTION-Flanke
 *   80031fd0  jal FUN_8002d474                  ; Kletter-Sonde ZUERST
 *   80031fe4  jal FUN_80042bac(player,1,0x10)   ; AOT-ACTION-Scan nur bei Rueckgabe 0
 *
 * === WAS HIER GEPRUEFT WIRD ============================================================
 * Im Port steckt der RVD-Scan IN re15_aot_scan (aot_common.c:829ff). Der Zweig in
 * game_step_common.c uebersprang den ganzen Aufruf, sobald re15_climb_try_start /
 * re15_stair_try_start in diesem Bild zugeschlagen haben. Ein verpasster Zonenuebergang ist
 * ENDGUELTIG (Memory reai-v2-camera-zone-statemachine) — also genau eine Klasse
 * "die Kamera wechselt nicht mehr".
 *
 * Aufbau: ROOM1090 (der Kletter-Raum), Spieler vor Kiste 0, ACTION-Flanke (QUADRAT). In
 * DEMSELBEN Bild liegt er in einer CAM_SWITCH-Zone, die von cam_from auf einen anderen Cut
 * schalten muss. Vor dem Fix: Klettern startet, Cut bleibt stehen. Nach dem Fix: beides.
 *
 * G1 Kontrolle: ohne ACTION (kein Kletterstart) schaltet die Zone.
 * G2 Der Fall: MIT ACTION startet das Klettern UND die Zone schaltet trotzdem.
 * G3 Waehrend das Klettern LAEUFT (Folgebild) schaltet die Zone weiterhin (war schon ok).
 * G4 Gegenprobe: der ACTION-Druck darf trotzdem KEINEN AOT-Event ausloesen
 *    (@0x80031fe4 wird im Original uebersprungen) -> fired_event_id_this_frame == 0.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_game_step.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
static void chk(const char *what, long got, long want)
{
    if (got != want) { printf("FAIL %-46s got=%ld want=%ld\n", what, got, want); g_fail++; }
    else               printf("ok   %-46s %ld\n", what, got);
}

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

static re15_rdt_t            s_rdt;
static re15_emd_animation_t  s_pl00, s_w01;
static re15_actor_t         *s_pl;

#define CAM_FROM  3
#define CAM_TO    5
#define ZONE_SLOT (RE15_AOT_MAX - 1)   /* oberster Slot = im Port die RVD-Region */

static void reset_room(void)
{
    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    re15_climb_reset();
    re15_collision_reset_band();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = -10100; s_pl->y = -1800; s_pl->z = 4200; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(re15_collision_band_from_y(s_pl->y));
}

/* Genau dorthin stellen, wo der Objekt-Push-Out den Anlauf stoppt (wie
 * test_1090_climb_pin.c: x = kiste.x - box_hx - Spielerradius 450). */
static void stand_at_crate_front(void)
{
    s_pl->x = (int32_t)g_scd.props[0].x - (int32_t)g_scd.props[0].box_hx - 450;
    s_pl->z = (int32_t)g_scd.props[0].z;
    s_pl->y = -1800;
    s_pl->rot_y = 0;
    re15_collision_set_band(1);
    re15_climb_standing_tick();
}

/* Eine CAM_SWITCH-Zone um den Spieler legen und die Auto-Kamera scharf machen.
 * cam_from_filter = CAM_FROM (der ANGEFORDERTE Cut, @0x8001423c lbu DAT_800afbb5),
 * event_id = Ziel-Cut (rec+3, @0x800142a8). band 0xFF = jede Etage (rec+1 == 0xff
 * @0x80014274). Alle uebrigen Slots werden entwaffnet, damit nur diese Zone zaehlt. */
static void arm_cam_zone(void)
{
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].active = 0;
    re15_aot_t *a = &g_aot.slots[ZONE_SLOT];
    memset(a, 0, sizeof *a);
    a->active          = 1;
    a->type            = RE15_AOT_TYPE_CAM_SWITCH;
    a->event_id        = CAM_TO;
    a->cam_from_filter = CAM_FROM;
    a->band            = 0xFF;
    a->has_quad        = 0;
    a->x = s_pl->x; a->z = s_pl->z;
    a->half_w = 4000; a->half_h = 4000;
    g_scd.cam_id             = CAM_FROM;
    g_scd.work_vars[0x0A]    = CAM_FROM;
    g_scd.cam_change_pending = 0;
    g_scd.cut_auto_enabled   = 1;    /* = DAT_800aca3c & 0x100 GELOESCHT (@0x8001cce0) */
    g_scd.player_mode        = 0;
    g_scd.letterbox_countdown = 0;
}

static void step_once(int action_press)
{
    re15_game_ctx_t g;
    memset(&g, 0, sizeof g);
    g.rdt         = &s_rdt;
    g.rdt_ok      = 1;
    g.pl00_skel   = NULL;
    g.pl00_anim   = &s_pl00;
    g.w01_anim    = &s_w01;
    g.cam_view    = NULL;
    g.active_cut  = (int)g_scd.work_vars[0x0A];
    g.pad_current = 0;
    g.pad_pressed = action_press ? (uint16_t)RE15_PAD_BIT_SQUARE : 0u;
    re15_game_step(&g);
}

int main(void)
{
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!raw) { printf("SKIP: ROOM1090.RDT fehlt\n"); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse\n"); return 77; }
    {
        char p[600]; size_t n = 0;
        snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
        uint8_t *a = read_file(p, &n);
        if (!a || re15_emd_parse_animation(a, n, &s_pl00) != 0) { printf("SKIP: PL00.EDD\n"); return 77; }
        snprintf(p, sizeof p, "%s/PLD/PL00W01.EDD", RE15_ASSET_PSX_DIR);
        uint8_t *b = read_file(p, &n);
        if (!b || re15_emd_parse_animation(b, n, &s_w01) != 0) { printf("SKIP: PL00W01.EDD\n"); return 77; }
    }

    /* ---- G1 KONTROLLE: ohne ACTION schaltet die Zone -------------------------------- */
    reset_room(); stand_at_crate_front(); arm_cam_zone();
    step_once(0);
    chk("G1 ohne ACTION: kein Klettern", re15_climb_active() ? 1 : 0, 0);
    chk("G1 ohne ACTION: Zone schaltet",  (long)g_scd.cam_id, CAM_TO);

    /* ---- G2 DER FALL: ACTION startet das Klettern -> Zone MUSS trotzdem schalten ---- */
    reset_room(); stand_at_crate_front(); arm_cam_zone();
    step_once(1);
    chk("G2 ACTION: Klettern gestartet",  re15_climb_active() ? 1 : 0, 1);
    chk("G2 ACTION: Zone schaltet TROTZDEM", (long)g_scd.cam_id, CAM_TO);
    /* G4: der Druck darf keinen AOT-Event feuern (@0x80031fe4 wird uebersprungen). */
    chk("G4 kein AOT-Event im Kletterbild",
        (long)g_aot.fired_event_id_this_frame, 0);

    /* ---- G3 waehrend das Klettern LAEUFT (Folgebild) -------------------------------- */
    reset_room(); stand_at_crate_front(); arm_cam_zone();
    step_once(1);                       /* Startbild */
    g_scd.cam_id = CAM_FROM;            /* Zone neu scharf: Gruppe wieder auf CAM_FROM */
    g_scd.work_vars[0x0A] = CAM_FROM;
    g_aot.slots[ZONE_SLOT].was_inside = 0;
    step_once(0);                       /* Folgebild: Substate 9 laeuft */
    chk("G3 Klettern laeuft noch",      re15_climb_active() ? 1 : 0, 1);
    chk("G3 Zone schaltet im Kletterbild", (long)g_scd.cam_id, CAM_TO);

    printf(g_fail ? "\n%d FAIL\n" : "\nALLE OK\n", g_fail);
    return g_fail ? 1 : 0;
}
