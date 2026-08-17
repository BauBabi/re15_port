/* test_text_freeze.c — BATCH C: der GLOBALE TEXT-FREEZE (Nutzer-Report 16, 2026-08-17).
 *
 * "Wenn man im Original etwas anklickt und einen Text liest, friert alles andere inklusive
 *  Gegner ein, bis der Text weg ist. Bei uns nicht."
 *
 * MECHANISMUS (alle Adressen fuer diesen Batch selbst nachdisassembliert, info/Re1.5/PSX.EXE
 * bzw. info/Re1.5/PSX/BIN/STAGE1.BIN):
 *
 *  OPEN — FUN_80027e68(param_1, param_2=0x300, msg_id, param_4 = MASKE):
 *    80027e74  lbu v0,0(v1)        v1 = DAT_800b8520 (Open-Flag)
 *    80027e7c  andi v0,v0,0x80
 *    80027e80  beq v0,zero,+       schon offen -> j 0x800280ac, v0 = -1 (KEIN Re-Save)
 *    80027e98  addiu a0,a0,-13760  a0 = 0x800ACA40 = g_pauseflags
 *    80027eb4  lw   v0,0(a0)
 *    80027ec8  sw   v0,-31428(at)  DAT_800b853c = SNAPSHOT des Vorzustands
 *    80027ecc  or   v0,a3,v0
 *    80027ed0  sw   v0,0(a0)       g_pauseflags |= Maske      <<<< DER FREEZE
 *
 *  MASKE = RAUM-DATEN, nicht hartkodiert:
 *    SCD 0x2B Message_on @0x800404f4:  80040504 lbu a2,1(v0) | 80040508 lhu a3,2(v0)
 *                                      80040518 jal 0x80027e68 | 8004051c sll a3,a3,16
 *    sce-1 Examine-AOT  @0x80043084:  80043098 lhu a3,2(v0) | 8004309c lhu a2,0(v0)
 *                                      800430a0 jal 0x80027e68 | 800430a4 sll a3,a3,16
 *  Eigener Census ueber alle 240 ausgelieferten RDTs (2026-08-17):
 *    Message_on 698x -> 420x 0x0000 | 34x 0xff80 | 244x 0xffff
 *    sce-1 AOT  511x -> 511x 0xffff
 *    ROOM1240 (6) + ROOM1170 (16) Kino-Captions: ALLE 0x0000
 *  => Untertitel frieren NICHT, Examine-Texte schon. Genau diese Unterscheidung pint (3).
 *
 *  LESER (jedes Subsystem gated sich SELBST):
 *    0x80000000 Spieler   FUN_80031c44 @0x80031c54 lw + @0x80031c78 bltz
 *    0x20000000 AI-Roots  FUN_80100424 @0x8010042c lw / @0x80100430 lui 0x2000 /
 *                         @0x80100438 and / @0x8010043c bne  (STAGE1.BIN)
 *    0x10000000 Anim      FUN_80019e20 @0x80019e28 lw / @0x80019e2c lui 0x1000 /
 *                         @0x80019e3c and / @0x80019e40 bne
 *    0x02000000 SCD-VM    FUN_8003f038 @0x8003f040 lw / @0x8003f044 lui 0x200 / @0x8003f04c bne
 *    0x01000000 Pad       FUN_80030444 @0x800304f4 lw / @0x800304f8 lui 0x100 /
 *                         @0x80030500 beq / @0x80030514 andi 0xf000 / @0x8003051c sw
 *
 *  CLOSE = Snapshot-Restore: @0x800285a4 / @0x800286cc / @0x8002871c.
 *  RAUMWECHSEL = Voll-Clear:  @0x8001ca44 / @0x8001caec `sw zero,0x800aca40`.
 *
 * Der Message-FSM-Pump laeuft im Original TOP-LEVEL und UNGEGATET (FUN_800280b4
 * @0x800280c8-dc, Caller @0x80010044) — deshalb ruft frame() re15_msg_tick immer.
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
#include "re15_enemy_ai.h"
#include "re15_skeleton.h"   /* re15_skel_compute_pose — Neck-FSM laeuft im RENDER-Pfad */
#include "re15_emd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

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

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

/* Ein Frame in der main.c-Reihenfolge: Message-Pump (top-level, ungegatet), dann der
 * geteilte Interpreter-Step, dann die SCD-VM (main.c:3313). */
static void frame(uint16_t cur, uint16_t edge)
{
    re15_msg_tick(0, 0, 0);
    s_ctx.pad_current = cur;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_vm_tick();
}

/* Testnachricht: 8 druckbare Glyphen + `01 00` = Ende/Warte-auf-Taste (FSM-Case 4,
 * @0x80028698 `andi 0xc000`) — sie verschwindet also NIE von selbst und haelt das
 * Messfenster offen. */
static void install_probe_msg(unsigned char id)
{
    uint8_t blob[10];
    for (int i = 0; i < 8; i++) blob[i] = 0x30;
    blob[8] = 0x01; blob[9] = 0x00;
    re15_msg_install_text(id, blob, sizeof blob);
}

/* Text wegdruecken (virt. 0x4000-Edge = Confirm, phys. SQUARE @0x80073dbc[14]). */
static void dismiss(void)
{
    /* Getippt/geschlossen wird mit einem echten TASTENDRUCK, nicht mit einem Dauer-Edge:
     * frame() ruft msg_tick VOR game_step, der Edge des Druck-Frames wird also erst im
     * FOLGE-Frame konsumiert. Wuerde man SQUARE in jedem Frame als press-EDGE anlegen,
     * feuerte die Examine-Zone im selben Frame neu, in dem der Text schliesst (im Original
     * unmoeglich: der Edge ist `(prev^cur)&cur`, gedrueckt HALTEN erzeugt keinen). */
    for (int i = 0; i < 400 && g_scd.message_active; i++) {
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* Druck  */
        if (!g_scd.message_active) break;
        frame(0, 0);                                       /* Loslassen: Edge wird konsumiert */
    }
    for (int i = 0; i < 3; i++) frame(0, 0);
}

/*--- Synthetisches 9-Bone-Skelett fuer den Neck-Pin (8) ----------------------
 * Gleiche Bauart wie in test_neck_headlook.c: 9 Bones in einer Kette, Kopf =
 * Bone 8 (Spieler-INIT @0x80031938). Die Neck-FSM sitzt in
 * re15_skel_compute_pose und wird im Port aus dem RENDER-Pfad gerufen — genau
 * deshalb braucht sie ein eigenes Pause-Gate. */
#define TF_KF_BONES 9
#define TF_KF_SIZE  60
static uint8_t s_tf_kf[TF_KF_SIZE];

static void tf_build_skel(re15_emd_skeleton_t *sk)
{
    memset(s_tf_kf, 0, sizeof s_tf_kf);
    memset(sk, 0, sizeof *sk);
    sk->bone_count = TF_KF_BONES;
    for (int b = 0; b < TF_KF_BONES; b++) {
        sk->bone_parent[b]     = (int8_t)(b == 0 ? -1 : b - 1);
        sk->bone_mesh_index[b] = (int8_t)b;
        sk->bone_relative_pos[b][0] = 0;
        sk->bone_relative_pos[b][1] = (int16_t)(b == 0 ? 0 : -100);
        sk->bone_relative_pos[b][2] = 0;
    }
    sk->keyframe_size_bytes = TF_KF_SIZE;
    sk->keyframe_count      = 1;
    sk->keyframe_data       = s_tf_kf;
    sk->keyframe_data_size  = TF_KF_SIZE;
}

static void tf_run_pose(const re15_emd_skeleton_t *sk, re15_actor_t *a, int ticks)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    extern void *g_anim_pose_actor;
    for (int t = 0; t < ticks; t++) {
        g_anim_pose_actor = a;
        re15_skel_compute_pose(sk, 0, poses);
        g_anim_pose_actor = NULL;
    }
}

/* Byte-true Spieler-INIT-Neck-Defaults: Bone 8 @0x80031938, Steps 96/96
 * @0x800319a4-ac, Klemmen +-0x200/+-0x138 @0x800319b0-c4, Akkus 0 @0x800319bc-c0. */
static void tf_neck_defaults(re15_actor_t *a)
{
    a->neck_bone = 8;
    a->neck_step_yaw = 96; a->neck_step_pitch = 96;
    a->neck_clamp_yaw = 0x200; a->neck_clamp_pitch = 0x138;
    a->neck_target_slot = -1;
    a->neck_yaw = 0; a->neck_pitch = 0;
    a->neck_sweep = 0;
    a->head_world_ok = 0;
}

/*--- Minimal-Kontext fuer den Raumwechsel-Pin (6) ---------------------------*/
static const re15_camera_cut_t *s_cuts      = 0;
static int                      s_cut_count = 0;
static int                      s_cam_cut   = 0;
static int  tf_load_rdt(unsigned room_id)  { (void)room_id; g_room_rdt = s_rdt; g_room_rdt_ok = 1; return 0; }
static void tf_reset_render(void)          { }
static int  tf_load_bg_cut(int cut)        { (void)cut; return 0; }

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

    printf("=== BATCH C: globaler TEXT-FREEZE (DAT_800aca40) ===\n");

    /*--- Raum hochfahren: ROOM1140 main00 + sub00 (die 5 Briefing-Zombies) -----*/
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1140;
    if (s_rdt.messages && s_rdt.messages_size > 0)
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0;
    re15_collision_set_band(0);

    /* Anim-Sonde: ein schlichter Aktor, den re15_actors_anim_advance jeden Frame um einen
     * Frame weiterzaehlt (Typ 0 = kein self-advancer, Motion 0 = keiner der Liege-Halte-Clips
     * 0x0C/0x0E/0x12/0x13/0x2A). Er misst das Bit-28-Gate (@0x80019e40) unabhaengig von der
     * KI-Logik. */
    const int PROBE = RE15_ACTOR_MAX - 1;
    g_actors[PROBE].active = 1; g_actors[PROBE].type = 0;
    g_actors[PROBE].motion = 0; g_actors[PROBE].anim_frame = 0;
    g_actors[PROBE].state = 0; g_actors[PROBE].motion_init_delay = 0;

    install_probe_msg(7);

    /* Settle: die Raumkollision zieht den Spieler zuerst in die begehbare Zelle. */
    for (int f = 0; f < 10; f++) frame(0, 0);

    /*=====================================================================
     * (1) DATEN-PIN: der sce-1-Examine-AOT traegt die Pause-Maske im Payload
     *     u16@+2 (op_aot_set liest pc[16..17] kurz / pc[24..25] lang).
     *===================================================================*/
    {
        int n_msg = 0, all_ffff = 1;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            if (!g_aot.slots[i].active) continue;
            if (g_aot.slots[i].type != RE15_AOT_TYPE_MESSAGE) continue;
            n_msg++;
            if (g_aot.slots[i].pause_mask16 != 0xFFFF) all_ffff = 0;
        }
        printf("  [Messung] ROOM1140 sce-1 MESSAGE-AOTs: %d, alle Maske 0xffff = %d\n",
               n_msg, all_ffff);
        CHECK("(1) Examine-AOT traegt Payload-Maske 0xffff (@0x80043098)", n_msg > 0 && all_ffff);
    }

    /*=====================================================================
     * (7) END-TO-END: ein SQUARE-Druck in einer ECHTEN ROOM1140-Examine-Zone
     *     friert ein. Kein synthetischer Aufruf — der vom Raum-SCD installierte
     *     sce-1-Record wird nur auf den Spieler verschoben, seine Maske bleibt die
     *     der Raum-Daten; gefeuert wird ueber re15_aot_scan/handler[1] @0x80043084.
     *===================================================================*/
    {
        int slot = -1;
        for (int i = 0; i < RE15_AOT_MAX; i++)
            if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_MESSAGE) { slot = i; break; }
        if (slot < 0) { fprintf(stderr, "FAIL: ROOM1140 hat keinen sce-1 MESSAGE-AOT\n"); g_fail = 1; }
        else {
            re15_aot_t *a = &g_aot.slots[slot];
            a->x = pl->x; a->z = pl->z; a->half_w = 3000; a->half_h = 3000;
            a->band = 0xFF; a->sce_flags = 0; a->has_quad = 0; a->was_inside = 0;
            a->event_id = 7;                        /* auf die Sonden-.msg umbiegen */
            install_probe_msg(7);
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            printf("  [Messung] nach SQUARE in der Examine-Zone: msg_active=%d "
                   "g_pauseflags=0x%08x\n", g_scd.message_active, (unsigned)g_re15_pauseflags);
            CHECK("(7) echter Examine-Druck friert mit 0xffff0000 ein "
                  "(@0x80043098 -> @0x800430a4 -> @0x80027ed0)",
                  g_scd.message_active == 1 && g_re15_pauseflags == 0xFFFF0000u);
            a->active = 0;                          /* Zone entschaerfen: der Rest des Tests
                                                     * misst den Freeze, nicht die Zone */
            dismiss();
            CHECK("(7b) Dismiss hebt den Freeze auf", g_re15_pauseflags == 0);
        }
        for (int f = 0; f < 4; f++) frame(0, 0);
    }

    /*=====================================================================
     * (2) EXAMINE-TEXT OFFEN -> Gegner-Ticks 0, Anim steht, SCD steht, Spieler steht.
     *     Maske 0xffff0000 wie JEDER ausgelieferte Examine-Text.
     *===================================================================*/
    {
        uint32_t before_flags = g_re15_pauseflags;
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        CHECK("(2a) Open setzt g_pauseflags = 0xffff0000 (@0x80027ed0)",
              g_re15_pauseflags == 0xFFFF0000u && before_flags == 0);

        re15_actor_t snap[RE15_ACTOR_MAX];
        memcpy(snap, g_actors, sizeof snap);
        uint32_t tick0  = g_scd.tick_count;
        uint16_t aprobe = g_actors[PROBE].anim_frame;

        /* 30 Frames mit Bewegung + Aktion: es darf sich NICHTS an der Welt aendern.
         * Der Pad-Edge traegt bewusst kein 0x4000/0x8000, damit der Text offen bleibt. */
        for (int f = 0; f < 30; f++)
            frame(RE15_PAD_BIT_UP, (f == 0) ? RE15_PAD_BIT_UP : 0);

        int actors_changed = memcmp(snap, g_actors, sizeof snap) != 0;
        int anim_delta     = (int)g_actors[PROBE].anim_frame - (int)aprobe;
        int scd_delta      = (int)(g_scd.tick_count - tick0);
        printf("  [Messung] 30 Frames mit offenem Examine-Text: Aktor-Bytes geaendert=%d, "
               "Anim-Advances=%d, SCD-Ticks=%d\n", actors_changed, anim_delta, scd_delta);
        CHECK("(2b) Anim steht (0 Advances; Bit 0x10000000 @0x80019e40)", anim_delta == 0);
        CHECK("(2c) SCD-VM steht (0 Ticks; Bit 0x02000000 @0x8003f04c)",  scd_delta  == 0);
        CHECK("(2d) KEIN Aktor-Byte aendert sich (Spieler Bit31 @0x80031c78 + KI Bit29 "
              "@0x8010043c)", actors_changed == 0);

        /* Pad-Maske: virt. 0xf000 ueberlebt, alles darunter faellt weg (@0x80030514). */
        {
            extern uint16_t g_scd_pad_held;
            frame((uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_SQUARE), 0);
            printf("  [Messung] Held-Wort im Freeze = 0x%04x (UP+SQUARE roh)\n", g_scd_pad_held);
            CHECK("(2e) Pad maskiert auf 0xf000: Confirm 0x4000 ueberlebt, D-Pad faellt weg",
                  g_scd_pad_held == 0x4000);
        }

        /*=================================================================
         * (5) OPEN-GUARD: ein zweiter Open waehrend offenem Textes darf den
         *     Snapshot NICHT ueberschreiben (@0x80027e74 -> return -1).
         *=================================================================*/
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        CHECK("(5) zweiter Open ueberschreibt den Snapshot nicht (@0x80027e74)",
              g_re15_pauseflags_saved == 0 && g_re15_pauseflags == 0xFFFF0000u);

        /*=================================================================
         * (4) CLOSE = Snapshot-Restore (@0x800285a4/@0x800286cc/@0x8002871c)
         *=================================================================*/
        dismiss();
        CHECK("(4a) Close restauriert den Vorzustand exakt", g_re15_pauseflags == before_flags);
        CHECK("(4b) Text ist zu", g_scd.message_active == 0);
    }

    /*=====================================================================
     * (4c) SNAPSHOT auch ueber einem BESTEHENDEN Freeze (das Item-Modal setzt
     *      0xff000000 @0x8001dbb8/@0x8001dbc8): der Text-Close darf ihn nicht
     *      mitloeschen — deshalb Restore statt `= 0`.
     *===================================================================*/
    {
        g_re15_pauseflags = 0xFF000000u;              /* wie das Item-Modal es setzt */
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        CHECK("(4c1) Open ver-ODERt (0xff000000 | 0xffff0000)", g_re15_pauseflags == 0xFFFF0000u);
        dismiss();
        CHECK("(4c2) Close gibt genau den Item-Modal-Freeze zurueck",
              g_re15_pauseflags == 0xFF000000u);
        re15_pauseflags_clear();
    }

    /*=====================================================================
     * (3) REGRESSIONSSCHUTZ: Maske 0 (Untertitel/Caption) friert NICHTS ein.
     *     420 von 698 ausgelieferten Message_on tragen 0x0000, darunter ALLE
     *     22 Kino-Captions in ROOM1170/ROOM1240 — wuerde hier pauschal
     *     eingefroren, staenden die Cutscenes.
     *===================================================================*/
    {
        for (int f = 0; f < 4; f++) frame(0, 0);      /* settle */
        re15_dialog_open_mask(7, 0, 0x00000000u);
        CHECK("(3a) Caption setzt KEINE Pause-Flags", g_re15_pauseflags == 0);

        uint16_t aprobe = g_actors[PROBE].anim_frame;
        uint32_t tick0  = g_scd.tick_count;
        for (int f = 0; f < 30; f++)
            frame(RE15_PAD_BIT_UP, (f == 0) ? RE15_PAD_BIT_UP : 0);
        int anim_delta = (int)g_actors[PROBE].anim_frame - (int)aprobe;
        int scd_delta  = (int)(g_scd.tick_count - tick0);
        printf("  [Messung] 30 Frames mit offener Caption: Anim-Advances=%d, SCD-Ticks=%d\n",
               anim_delta, scd_delta);
        CHECK("(3b) Caption: Anim laeuft WEITER", anim_delta == 30);
        CHECK("(3c) Caption: SCD-VM laeuft WEITER", scd_delta == 30);
        dismiss();
    }

    /*=====================================================================
     * (6) RAUMWECHSEL cleart die Maske (@0x8001ca44 / @0x8001caec).
     *===================================================================*/
    {
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        CHECK("(6a) Freeze steht vor dem Raumwechsel", g_re15_pauseflags == 0xFFFF0000u);

        re15_room_apply_ctx_t ac;
        memset(&ac, 0, sizeof ac);
        ac.rdt              = &s_rdt;
        ac.rdt_ok           = &s_ctx.rdt_ok;
        ac.active_cuts      = &s_cuts;
        ac.active_cut_count = &s_cut_count;
        ac.cam_active_cut   = &s_cam_cut;
        ac.cam_view         = &s_cam;
        ac.load_rdt         = tf_load_rdt;
        ac.reset_render     = tf_reset_render;
        ac.load_bg_cut      = tf_load_bg_cut;
        re15_room_request_change(0x1140, 0, 0, 0, 0, 0);
        (void)re15_room_apply_pending(&ac);
        printf("  [Messung] g_pauseflags nach Raumwechsel = 0x%08x\n",
               (unsigned)g_re15_pauseflags);
        CHECK("(6b) Raumwechsel nullt g_pauseflags (@0x8001ca44/@0x8001caec)",
              g_re15_pauseflags == 0 && g_re15_pauseflags_saved == 0);
    }

    /*=====================================================================
     * (8) NECK-/HEAD-LOOK-FSM steht im Freeze (Fix-Runde Cluster 1, Fund 1).
     *     Die FSM ist im Original KEIN Top-Level-Treiber; sie haengt an genau
     *     zwei Aufrufer-Klassen, die sich vorher selbst gaten:
     *       SPIELER  `jal 0x80037358` @0x80031d78 — eigener wortweiser EXE-Scan
     *                nach 0x0C00DCD6 liefert GENAU diesen einen Treffer, und er
     *                liegt zwischen @0x80031c78 `bltz a0,0x80031da8`
     *                (a0 = g_pauseflags, @0x80031c54) und dessen Ziel.
     *       NPC      6 Call-Sites in STAGE1.BIN (0x8011c69c/cc6c/d278/d80c/
     *                dd58/e320); JEDE umschliessende Root traegt im Prolog
     *                `lui v1,0x2000` (@0x8011c5b8/cb88/d158/d6ec/dc80/e244) mit
     *                `bne` HINTER den Call.
     *     Der Port ruft sie aus dem RENDER-Pfad — ohne Gate slewten die Koepfe
     *     waehrend jedes eingefrorenen Textes weiter und die Sweep-Zaehler
     *     (+0x160) liefen bis zum Auto-Release `neck_flags = 0x12` herunter.
     *===================================================================*/
    {
        re15_emd_skeleton_t sk;
        re15_pauseflags_clear();
        tf_build_skel(&sk);

        /* --- (8a) Spieler-Slot, Bit 0x80000000 --------------------------- */
        tf_neck_defaults(pl);
        pl->neck_flags = 0x08;          /* Modus 2 = relatives Ziel (@0x80041ef8 `ori 0x8`) */
        pl->neck_ty = 1500; pl->neck_tz = 0;
        tf_run_pose(&sk, pl, 1);
        int16_t acc_free = pl->neck_yaw;
        tf_run_pose(&sk, pl, 1);
        int free_delta = (int)pl->neck_yaw - (int)acc_free;

        int16_t acc_before = pl->neck_yaw;
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        tf_run_pose(&sk, pl, 20);
        int frozen_delta = (int)pl->neck_yaw - (int)acc_before;
        printf("  [Messung] Neck-Akku Spieler: 1 freier Tick = %+d, 20 Frames im Freeze = %+d\n",
               free_delta, frozen_delta);
        CHECK("(8a-vor) ohne Freeze slewt der Kopf ueberhaupt (Pin nicht vakuum)", free_delta != 0);
        CHECK("(8a) Spieler-Kopf steht im Freeze (Bit 0x80000000 @0x80031c78)", frozen_delta == 0);

        /* --- (8b) SWEEP-Zaehler + Auto-Release duerfen im Freeze nicht laufen.
         *     Yaw-Sweep = Modus 4 (@0x80041f10 `ori 0x58`); Ankunft dekrementiert
         *     +0x160 (@0x80037664-c4) und kippt bei 0 auf 0x12 (@0x80037698). --- */
        int8_t  tx_before = 0;
        {
            re15_actor_t *w = &g_actors[PROBE];
            tf_neck_defaults(w);
            w->neck_flags = 0x58;       /* Sweep-Modus 4 */
            w->neck_tx = 2;             /* +0x160 = Sweep-Wiederholungen */
            w->neck_ty = 0; w->neck_tz = 0;
            tx_before = (int8_t)w->neck_tx;
            tf_run_pose(&sk, w, 40);    /* PROBE ist KEIN Spieler -> Bit 0x20000000 */
            printf("  [Messung] NPC-Sweep im Freeze: neck_tx %d -> %d, neck_flags 0x%02x\n",
                   (int)tx_before, (int)w->neck_tx, (unsigned)w->neck_flags);
            CHECK("(8b) NPC-Sweep-Zaehler + Flags stehen im Freeze (Bit 0x20000000 @0x8011c5c0)",
                  (int8_t)w->neck_tx == tx_before && w->neck_flags == 0x58);
            w->neck_bone = 0;           /* Sonde wieder entschaerfen */
        }
        dismiss();
        pl->neck_bone = 0; pl->neck_flags = 0; pl->neck_yaw = 0; pl->neck_pitch = 0;
        re15_pauseflags_clear();
    }

    /*=====================================================================
     * (9) PLC_DEST-WALKER steht im Freeze (Fund 5). Im Original gibt es diesen
     *     Sammel-Treiber nicht: der Spieler-Walk liegt hinter Bit 0x80000000
     *     (@0x80031c78 ueberspringt den cmd-Dispatch @0x80031ca4-cac;
     *     PTR_LAB_80073f90[4] = 0x80030660 -> dispatch @0x8003069c-a4 ueber
     *     PTR_LAB_80073e30, und [4] = 0x80030af0 IST der WALK-Handler), die
     *     NPC-Fortbewegung im AI-Root-Executor hinter Bit 0x20000000.
     *     Ohne Gate rutschte der NPC in eingefrorener Pose durch den Raum und
     *     das Plc_dest-Ankunftsflag konnte mitten im Text umschlagen.
     *===================================================================*/
    {
        re15_pauseflags_clear();
        re15_actor_t *w = &g_actors[PROBE];
        w->active = 1; w->type = 0x40; w->hp = 10;
        w->x = 0; w->z = 0; w->y = 0; w->rot_y = 0;
        w->walk_active = 1; w->walk_fsm = 0; w->walk_mode = 4;
        w->walk_dest_x = 30000; w->walk_dest_z = 0; w->walk_flag_bit = 0;
        int32_t x0 = w->x, z0 = w->z;
        for (int i = 0; i < 60; i++) re15_actor_step_all_walkers();
        int moved_free = (w->x != x0 || w->z != z0);
        int32_t x1 = w->x, z1 = w->z;

        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        for (int i = 0; i < 60; i++) re15_actor_step_all_walkers();
        int moved_frozen = (w->x != x1 || w->z != z1);
        printf("  [Messung] Walker: 60 freie Ticks dx=%d dz=%d | 60 Freeze-Ticks dx=%d dz=%d\n",
               (int)(x1 - x0), (int)(z1 - z0), (int)(w->x - x1), (int)(w->z - z1));
        CHECK("(9-vor) ohne Freeze laeuft der Walker ueberhaupt (Pin nicht vakuum)", moved_free);
        CHECK("(9) NPC-Position steht im Freeze (Bit 0x20000000; Spieler-Walk @0x80030af0 "
              "hinter Bit 0x80000000 @0x80031c78)", !moved_frozen);
        dismiss();
        w->walk_active = 0; w->active = 0;
        re15_pauseflags_clear();
    }

    /*=====================================================================
     * (10) GAME-OVER kommt AUS dem Freeze heraus (Fund 3). Die Game-Over-FSM
     *      ist im Original ein eigener Top-Level-Aufruf @0x8001cdfc
     *      (`jal 0x8001500c`) VOR dem Spieler-Dispatcher @0x8001ce0c, und sie
     *      liest g_pauseflags NIRGENDS: der EXE-weite Scan nach
     *      `lw rX,-13760(rY)` liefert 9 Leser (0x800144a4, 0x80019e28,
     *      0x8001cbcc, 0x8001cc9c, 0x8001cd14, 0x8001cdd8, 0x800304f4,
     *      0x80031c54, 0x8003f040) — keiner in 0x8001500c..0x80015840.
     *      Ihr einziges Gate ist das Kommando-Wort (@0x80015014 `lbu DAT_800aca58`,
     *      @0x8001501c/28/30 == 6/3/7 = die Todes-Handler). Ein Tod, der in einen
     *      Text hineinlaeuft, darf die Todes-Praesentation NICHT einfrieren.
     *===================================================================*/
    {
        re15_pauseflags_clear();
        pl->hp = -1;                                   /* cmd 3/6/7 */
        for (int f = 0; f < 5; f++) frame(0, 0);       /* Kette laeuft an */
        int pool0 = g_death_pool;
        re15_dialog_open_mask(7, 0, 0xFFFF0000u);
        CHECK("(10a) Freeze steht (Spieler-Bit 0x80000000 gesetzt)",
              (g_re15_pauseflags & 0x80000000u) != 0);
        for (int f = 0; f < 40; f++) frame(0, 0);
        printf("  [Messung] Game-Over im Freeze: g_death_pool %d -> %d, white=%d, cam=%d\n",
               pool0, g_death_pool, g_death_white, g_death_cam);
        CHECK("(10b) Game-Over-FSM laeuft im Freeze weiter (@0x8001cdfc, ungegatet)",
              g_death_pool > pool0);
        dismiss();
        pl->hp = 100;
        for (int f = 0; f < 2; f++) frame(0, 0);       /* Reset-Pfad (s_go_on && !dead) */
        re15_pauseflags_clear();
    }

    free(data);
    if (g_fail) { printf("RESULT: FAIL\n"); return 1; }
    printf("RESULT: OK — Examine-Text friert Spieler/KI/Anim/Skript ein, Caption nicht.\n");
    return 0;
}
