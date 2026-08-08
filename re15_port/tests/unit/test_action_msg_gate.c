/* test_action_msg_gate.c — byte-true ACTION-Zustands-Gate + Live-FF-Kadenz (Nutzer-Reports A+B,
 * 2026-08-08).
 *
 * (A) Der ACTION-Scan FUN_80042bac(player, 1, 0x10) hat im Original GENAU 8 Caller (XREF-komplett,
 *     ghidra1_V2.txt): die DECIDE-Handler der cmd-1-Substates 0-4 der Tabelle @0x80073fb0
 *     (@0x80031fe4 / @0x80032400 / @0x800326d0 / @0x80032a50 / @0x80032c84, jeweils gated auf
 *     `lw DAT_800ac76c; andi 0x80` = virtueller ACTION-Edge) + die 3 AUTO-Pool-Sites in
 *     FUN_800436a8 (a2=0). Der Substate-Dispatcher @0x80031ecc-1ef4 laeuft NUR im cmd-1-Handler
 *     LAB_80031de8 (@0x80073f90[1]); waehrend cmd 2 (Hit/Knockdown, LAB_80035af0), cmd 3/6/7
 *     (Tod, LAB_800366bc/LAB_800368c0/LAB_8003694c) und cmd 5 (Grab, LAB_80036834) laeuft KEIN
 *     DECIDE -> kein Examine/Tuer/Message-Fire. Der Aim-Substate 7 hat einen NOP-DECIDE
 *     (@0x80032e3c `jr ra`) -> auch waehrend des Zielens (inkl. LOWER nach R1-Release, Exit erst
 *     `sb zero,DAT_800aca59` @0x80033d4c/@0x80034d38/@0x80035500) feuert nichts.
 *     PORT-PIN: waehrend Knockdown/Flinch/Tod/Aim darf ein SQUARE-Druck KEINEN MESSAGE-AOT feuern;
 *     im Normal-Zustand MUSS er feuern.
 *
 * (B) Typewriter-Fast-Forward (FUN_80028134 state 1): FF = virtuelles 0x4000 GEHALTEN
 *     (`lw DAT_800ac768` @0x8002820c; `andi 0x4000` @0x80028214), Wirkung Timer -= 4 statt -1
 *     (`addiu v1,v1,-0x4` @0x80028228 vs `addiu a0,v1,-0x1` @0x800281f0) + Budget 2 Glyphen/Frame
 *     (`ori s2,zero,0x2` @0x80028238), gated auf Periode < 4 (`sltiu v0,v0,4` @0x8002822c).
 *     Kein Instant-Complete: state 1 liest NUR das Held-Wort, keinen Edge.
 *     PIN hier auf der LIVE-VERKABELUNG (main-Loop-Reihenfolge msg_tick -> game_step; Pad fliesst
 *     ctx.pad_current -> re15_pad_virtual_word -> g_scd_pad_held): SQUARE gehalten = 2 Glyphen/
 *     Frame, neutral = 1 Glyphe je 2 Frames (Periode 2). Die reine FSM-Kadenz pint test_msg_dialog.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_enemy_ai.h"     /* re15_player_knockdown_begin */
#include "re15_damage.h"       /* re15_player_is_dead */

/* Aim-FSM-Hooks (player_common.c — wie in test_aim_* per extern, kein Header). */
extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

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

static re15_rdt_t        s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t   s_ctx;

/* Ein Frame in der main.c-Reihenfolge: Subtitle-Tick (liest die Pad-Woerter des VORHERIGEN
 * game_step, main.c:3202 < :4197), dann der geteilte Interpreter-Step. */
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void dismiss_msg(void)
{
    g_scd.message_active         = 0;
    g_scd.message_fsm_active     = 0;
    g_scd.message_display_frames = 0;
    g_scd.message_query          = 0;
    g_scd.message_select         = 0;
}

/* MESSAGE-AOT (sce=1) synthetisch am Spieler installieren — Geometrie/Band wie eine
 * Examine-Zone (band 0xFF = Wildcard-Bit 0x80, sce_flags 0 = Legacy-Install -> Centre+Forward).
 * Halb-Extents 3000: die Raumkollision (Normal-Branch) und der Flinch-/Knockdown-Rueckstoss
 * verschieben den Spieler um einige hundert Einheiten — das Rechteck muss ihn dabei halten,
 * sonst misst der Test die Geometrie statt des Zustands-Gates. */
static void install_msg_aot(int32_t x, int32_t z)
{
    re15_aot_set_message(0, 7);
    re15_aot_t *a = &g_aot.slots[0];
    a->x = x; a->z = z; a->half_w = 3000; a->half_h = 3000;
    a->band = 0xFF;
    a->sce_flags = 0;
    a->has_quad = 0;
}

static void fresh_state(void)
{
    re15_actor_init();
    memset(&g_aot, 0, sizeof g_aot);
    scd_vm_init();
    re15_player_cmd_reset();                 /* Knockdown/Flinch/Aim-Statics wischen */
    re15_player_aim_reset();
    g_current_room_id = 0x1140;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    pl->hp = 100; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0;

    re15_collision_set_band(0);

    /* Message 7: 24 druckbare Glyphen + `01 00` (Ende = Warte-auf-Taste, state 5). */
    {
        uint8_t blob[26];
        for (int i = 0; i < 24; i++) blob[i] = 0x30;
        blob[24] = 0x01; blob[25] = 0x00;
        re15_msg_install_text(7, blob, sizeof blob);
    }

    /* SETTLE: die Raumkollision schiebt einen frei gesetzten Punkt zunaechst in die begehbare
     * Zelle (gemessen: (0,0) -> (0,2218) im ersten Normal-Frame). Erst settlen lassen, DANN
     * das AOT-Rechteck am gesettelten Spielerpunkt verankern — sonst steht der Spieler im
     * Normal-Branch ausserhalb der Zone und der Test misst Geometrie statt Zustands-Gate. */
    for (int f = 0; f < 10; f++) frame(0, 0);
    install_msg_aot(pl->x, pl->z);
    pl->hp = 100;                            /* Baseline nach dem Settle re-setzen */
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(data, size, &s_rdt) != 0) { fprintf(stderr, "FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== (A) ACTION-Klasse nur im Normal-Zustand (DECIDE-Tabelle @0x80073fb0[0..4]) ===\n");

    /* --- 1: Normal-Zustand — Examine MUSS feuern (Substate-0-DECIDE @0x80031fe4) --- */
    fresh_state();
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK("(A1) Normal: SQUARE-Edge im MESSAGE-AOT feuert den Dialog", g_scd.message_active == 1);

    /* --- 2 (B): Live-FF-Kadenz auf dem in A1 geoeffneten Dialog --- */
    {
        /* 1 Halte-Frame, damit g_scd_pad_held sicher das Held-Wort des Vorframes traegt. */
        frame(RE15_PAD_BIT_SQUARE, 0);
        int p0 = (int)g_scd.message_parse;
        for (int f = 0; f < 6; f++) frame(RE15_PAD_BIT_SQUARE, 0);
        int d_ff = (int)g_scd.message_parse - p0;
        printf("  [Messung] FF-Kadenz (SQUARE gehalten): %d Glyphen in 6 Frames\n", d_ff);
        CHECK("(B1) FF gehalten = 2 Glyphen/Frame (12 in 6 Frames; @0x80028228/@0x80028238)",
              d_ff == 12);
    }
    dismiss_msg();
    {   /* Kontrolle: neutrale Kadenz 1 Glyphe je 2 Frames (Periode 2 = 2<<0, @0x80028440). */
        re15_dialog_open(7, 0);
        frame(0, 0);                          /* pad-neutral settle (held-Wort des Vorframes) */
        int p0 = (int)g_scd.message_parse;
        for (int f = 0; f < 8; f++) frame(0, 0);
        int d_n = (int)g_scd.message_parse - p0;
        printf("  [Messung] Normal-Kadenz (neutral): %d Glyphen in 8 Frames\n", d_n);
        CHECK("(B2) neutral = 1 Glyphe je 2 Frames (4 in 8 Frames)", d_n == 4);
        dismiss_msg();
    }

    /* --- 3: KNOCKDOWN (cmd-2 [4]/[5], Handler 0x800360e8/0x8003644c — kein DECIDE) --- */
    fresh_state();
    re15_player_knockdown_begin(0);
    {
        int fired = 0, kd_frames = 0;
        for (int f = 0; f < 40 && re15_player_knockdown_active(); f++) {
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            kd_frames++;
            if (g_scd.message_active) fired = 1;
        }
        printf("  [Messung] Knockdown: %d Frames, Examine gefeuert=%d\n", kd_frames, fired);
        CHECK("(A2) Knockdown: SQUARE feuert KEIN Examine (cmd!=1 -> kein Dispatcher @0x80031ecc)",
              kd_frames > 0 && fired == 0);
    }

    /* --- 4: HIT-FLINCH (cmd 2, LAB_80035af0 — kein DECIDE) --- */
    fresh_state();
    frame(0, 0);                                        /* HP-Baseline setzen (s_prev_hp) */
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 90;           /* Drop -> Flinch-Detector */
    {
        int fired = 0, flinch_frames = 0;
        for (int f = 0; f < 12; f++) {
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            if (g_actors[RE15_ACTOR_SLOT_PLAYER].motion == 0x0a) flinch_frames++;
            if (g_scd.message_active) fired = 1;
        }
        printf("  [Messung] Flinch: %d Clip-0xa-Frames, Examine gefeuert=%d\n", flinch_frames, fired);
        CHECK("(A3) Flinch: SQUARE feuert KEIN Examine (cmd 2 -> LAB_80035af0)",
              flinch_frames > 0 && fired == 0);
    }

    /* --- 5: TOD (cmd 3/6/7 — kein DECIDE) --- */
    fresh_state();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = -1;
    {
        int fired = 0;
        for (int f = 0; f < 5; f++) {
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            if (g_scd.message_active) fired = 1;
        }
        printf("  [Messung] Tod: Examine gefeuert=%d\n", fired);
        CHECK("(A4) Tod: SQUARE feuert KEIN Examine (LAB_800366bc/LAB_8003694c)", fired == 0);
    }

    /* --- 6: AIM (cmd-1-Substate 7 — DECIDE = `jr ra` @0x80032e3c) --- */
    fresh_state();
    re15_player_set_aim_clip_len(12);                   /* Mock-W-Bank: alle Clips 12 Frames */
    for (int f = 0; f < 20 && !re15_player_aim_active(); f++) frame(RE15_PAD_BIT_R1, 0);
    CHECK("(A5a) Aim-FSM aktiv nach R1-Halten", re15_player_aim_active() == 1);
    {
        int fired = 0;
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* SQUARE = FEUER */
        if (g_scd.message_active) fired = 1;
        printf("  [Messung] Aim+R1: Examine gefeuert=%d\n", fired);
        CHECK("(A5b) Aim (R1 gehalten): SQUARE feuert KEIN Examine", fired == 0);
    }
    {
        /* R1 loslassen -> LOWER-Phase: Substate bleibt 7 bis `sb zero,DAT_800aca59`
         * (@0x80033d4c/@0x80034d38/@0x80035500) -> auch hier KEIN Examine. */
        int fired = 0, lower_frames = 0;
        for (int f = 0; f < 6 && re15_player_aim_active(); f++) {
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            lower_frames++;
            if (g_scd.message_active) fired = 1;
        }
        printf("  [Messung] Aim-LOWER (R1 los): %d Frames, Examine gefeuert=%d\n",
               lower_frames, fired);
        CHECK("(A5c) Aim-LOWER: SQUARE feuert KEIN Examine (Substate 7 bis zum aca59-Clear)",
              lower_frames > 0 && fired == 0);
    }

    /* --- 7: Rueckkehr in Normal — Examine feuert wieder (Gate darf nicht kleben) --- */
    {
        for (int f = 0; f < 30 && re15_player_aim_active(); f++) frame(0, 0);
        frame(0, 0);
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        CHECK("(A6) zurueck in Normal: SQUARE-Edge feuert wieder", g_scd.message_active == 1);
    }

    free(data);
    if (g_fail) { printf("ACTION-MSG-GATE: FAIL\n"); return 1; }
    printf("ACTION-MSG-GATE: alle Checks bestanden\n");
    return 0;
}
