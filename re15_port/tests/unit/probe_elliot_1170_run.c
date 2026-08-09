/* probe_elliot_1170_run.c — DIAGNOSE (kein ctest-Assert): Nutzer-Report 2026-08-09 (nach v0.1):
 * ROOM1170-Intro — "Elliots RENN-Animation ganz am Anfang ist kaputt: ein Bein hinten,
 * er rennt nicht, er GLEITET."
 *
 * ORIGINAL-ABLAUF (aus der RDT-SCD selbst extrahiert, build/extracted .../room1170/scd/):
 *   sub15: Ck(4,242,1) -> Sce_em_set(0, 0x47, 64, 4, ..., 1261, -7200, 10091, 1199) = Elliot,
 *          behavior 0x40 -> grid_id, dann Evt_exec(0x1802) = sub02.
 *   sub02: Set(4,242,0) ... Work_set(2,0) ; Plc_dest(0, 5=RUN, 33, -2261, 5791) = der
 *          gemeldete Renn-Abschnitt. Danach TURN (mode 9), Cut_chg(2), Gesten 15/20/16/17.
 *
 * MESSUNG: Intro ab dem Selbst-Reenter (Flags 3/125 + 4/242 = Zustand nach sub11) ticken,
 * pro Tick Elliots Steuer-/Anim-Zustand loggen. GLIDE-Detektor: walk_active=1 UND
 * |pos-Delta| > 0 UND anim_frame unveraendert ueber N Ticks.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1170;
    /* Zustand nach sub11-Narrator (Set(3,125)=1, Set(4,242)=1) + Tuer-3-Selbst-Reenter. */
    re15_game_flag_set(3, 125, 1);
    re15_game_flag_set(4, 242, 1);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 2664; pl->y = -7200; pl->z = -7336;   /* Helipad (HANDOVER-Intro-Endpos) */

    /* EM047 wie die Live-Plattform laden (pc_enemy_load-Muster; Marvin-Probe): CDEMD0.EMS
     * Blob-Index 21 (s_ems_order, re15_ems.c) -> Container + alle Kanal-Baenke. MESSUNG der
     * eigenen Bank 1 (dir[4]/dir[3] = Kanal +0x170/+0x174, FUN_80022300 @0x800224b8/c8). */
    {
        char emsp[600]; size_t ems_size = 0;
        snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
        uint8_t *ems = read_file(emsp, &ems_size);
        if (ems) {
            int idx = re15_ems_index_for_type(0x47);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0) {
                printf("EM047: EMS-Blob %d @Datei-Offset 0x%zx, len 0x%zx\n", idx, off, len);
                re15_enemy_bank_t *eb = re15_enemy_alloc(0x47);
                static uint8_t s_em47[0x60000];
                if (eb && len <= sizeof s_em47) {
                    memcpy(s_em47, ems + off, len);
                    re15_tim_t tim = (re15_tim_t){0};
                    if (re15_emd_parse_container(s_em47, len, &eb->md1, &eb->skel,
                                                 &eb->anim, &tim) == 0) {
                        eb->ok = 1; eb->buf = NULL;
                        eb->loco_ok = (re15_emd_parse_loco_bank(s_em47, len,
                                           &eb->skel_loco, &eb->anim_loco) == 0);
                        eb->victim_ok = (re15_emd_parse_victim_bank(s_em47, len,
                                           &eb->skel_victim, &eb->anim_victim) == 0);
                        eb->own_ok = (re15_emd_parse_own_bank(s_em47, len,
                                           &eb->skel_own, &eb->anim_own) == 0);
                        printf("EM047 Container-Bank: %d clips, bones=%d; fc[0..9]=",
                               eb->anim.clip_count, eb->skel.bone_count);
                        for (int c = 0; c < 10 && c < eb->anim.clip_count; c++)
                            printf("%d ", eb->anim.clips[c].frame_count);
                        printf("\nEM047 own(Bank1) ok=%d: %d clips, bones=%d; fc[0..13]=",
                               eb->own_ok, eb->own_ok ? eb->anim_own.clip_count : 0,
                               eb->own_ok ? eb->skel_own.bone_count : 0);
                        for (int c = 0; eb->own_ok && c < 14 && c < eb->anim_own.clip_count; c++)
                            printf("%d ", eb->anim_own.clips[c].frame_count);
                        printf("\nEM047 loco ok=%d victim ok=%d\n", eb->loco_ok, eb->victim_ok);
                    } else printf("EM047 Container-Parse FAIL\n");
                }
            } else printf("EM047: kein EMS-Eintrag\n");
            free(ems);
        }
    }

    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);

    /* ECHTER FRAME-RAND (game_step_common.c): der Selbst-Reenter (PATH A) wird IM game_step
     * dispatcht — re15_actors_anim_advance (:910) und re15_enemy_ai_run_all (:918) laufen
     * danach NOCH IM SELBEN Frame. Der NPC-INIT tickt also EINMAL, BEVOR sub02 nach Sleep(1)
     * im Folgeframe Work_set(2,0)+Plc_dest ausfuehrt. Ohne diesen Schwanz misst die Probe
     * eine andere Maschine (Elliot bliebe state 0). */
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);

    /* Elliot-Slot finden */
    int es = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x47) { es = s; break; }
    if (es >= 0)
        printf("nach Reenter-Frame: slot=%d grid=0x%02x st=%u s1=%u mo=%d af=%u\n",
               es, g_actors[es].grid_id, g_actors[es].state, g_actors[es].sub_state_1,
               (int)g_actors[es].motion, g_actors[es].anim_frame);

    extern uint16_t g_scd_pad_edge, g_scd_pad_held;

    int prev_mo = -999, prev_af = -1, prev_st = -1, prev_s1 = -1, prev_wact = -1, prev_frz = -1;
    long prev_x = 0, prev_z = 0;
    int glide_ticks = 0, run_ticks = 0, run_af_advances = 0, run_clip0_ticks = 0;
    int glide_first = -1, run_fc = -1;
    long run_dist = 0;
    long step2_min = 0x7fffffffL, step2_max = 0;         /* pro-Tick-Weg^2 im RUN (Soll ~210^2) */
    int saw_arrival_gesture = 0;                          /* Sub-6-Ankunfts-Clip 1 nach dem Lauf */
    int gesture_idx = 0;                                  /* SCD-Gesten 15,20,16,17 in Reihenfolge */
    static const int k_gestures[4] = { 15, 20, 16, 17 };

    for (int tick = 0; tick < 900; tick++) {
        g_scd_pad_edge = 0x8000; g_scd_pad_held = 0x8000;   /* Message-Confirm wie Marvin-Probe */
        scd_vm_tick();
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);

        if (es < 0) {
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x47) { es = s; break; }
            if (es < 0) continue;
            printf("Elliot gespawnt: slot=%d grid=0x%02x pos=(%ld,%ld,%ld) rot=%d\n",
                   es, g_actors[es].grid_id, (long)g_actors[es].x, (long)g_actors[es].y,
                   (long)g_actors[es].z, (int)g_actors[es].rot_y);
        }
        re15_actor_t *e = &g_actors[es];

        int ctrl_changed = (e->motion != prev_mo || e->state != prev_st ||
                            e->sub_state_1 != prev_s1 || e->walk_active != prev_wact ||
                            e->anim_freeze != prev_frz);
        if (ctrl_changed) {
            printf("%5d st=%u s1=%-2u s2=%u mo=%-3d af=%-3u frac=%u frz=%u aflags=0x%04x "
                   "walk=%u/%u fsm=%u pos=(%ld,%ld)\n",
                   tick, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                   e->anim_frame, e->anim_frac, e->anim_freeze, e->anim_flags,
                   e->walk_active, e->walk_mode, e->walk_fsm, (long)e->x, (long)e->z);
        }
        /* RUN-Phase seit 7b = NPC-Sub-VM: st=4, s1=5 (Sub-5-RUN @0x80051484), s2=2 (Phase 2
         * WALK/Translation @0x80051640ff). Der Clip ist 0 = EM047-Own-Bank-Run-Gait (22f,
         * `sb zero,148` @0x8005157c; Bank CDEMD0.EMS Blob 21 @0x336000). */
        int in_run = (e->state == 4 && e->sub_state_1 == 5 && e->sub_state_2 == 2);
        if (in_run && (tick % 4) == 0)
            printf("%5d RUN* mo=%d af=%u frz=%u st=%u pos=(%ld,%ld)\n",
                   tick, (int)e->motion, e->anim_frame, e->anim_freeze, e->state,
                   (long)e->x, (long)e->z);

        /* GLIDE-Detektor: Position bewegt sich, af steht */
        if (in_run) {
            long dx = (long)e->x - prev_x, dz = (long)e->z - prev_z;
            long d2 = dx*dx + dz*dz;
            run_ticks++; run_dist += d2 > 0;
            if (e->motion == 0) run_clip0_ticks++;
            if (d2 > 0 && (int)e->anim_frame == prev_af) {
                glide_ticks++;
                if (glide_first < 0) glide_first = tick;
            }
            if ((int)e->anim_frame != prev_af) run_af_advances++;
            /* Schrittweite nur zwischen ZWEI aufeinanderfolgenden RUN-Ticks messen
             * (der erste Tick vergliche sonst gegen die prev-Initialisierung (0,0)). */
            static int s_prev_was_run = 0;
            if (d2 > 0 && s_prev_was_run) {
                if (d2 < step2_min) step2_min = d2;
                if (d2 > step2_max) step2_max = d2;
            }
            s_prev_was_run = 1;
            if (run_fc < 0) {                       /* Render-/Laengen-Bank des Runs festhalten */
                extern int re15_actor_clip_len(const re15_actor_t *a);
                run_fc = re15_actor_clip_len(e);
            }
        }
        /* Sub-6-HANDOFF nach Run/Turn (Walk-Arrival @0x80051794/a4, Turn-Alignment @0x80051dac/
         * dbc setzen +0x5=6). Der Ankunfts-Clip 1 SPIELT in sub02 byte-true NIE: das Turn-Flag
         * (@0x80051dd8) wird im Folgetick vom Sub10-Poll konsumiert und Plc_motion(0,15,0)
         * ueberschreibt +0x5, bevor Sub 6 dispatcht wird. */
        if (run_ticks > 0 && e->state == 4 && e->sub_state_1 == 6)
            saw_arrival_gesture = 1;
        if (gesture_idx < 4 && e->motion == k_gestures[gesture_idx]) gesture_idx++;

        prev_mo = e->motion; prev_af = e->anim_frame; prev_st = e->state;
        prev_s1 = e->sub_state_1; prev_wact = e->walk_active; prev_frz = e->anim_freeze;
        prev_x = e->x; prev_z = e->z;
    }

    printf("\n== BEFUND ==\n");
    printf("RUN-Ticks (st4/s1=5/s2=2): %d, davon af-Advances: %d, Clip-0-Ticks: %d, fc(Run)=%d\n",
           run_ticks, run_af_advances, run_clip0_ticks, run_fc);
    printf("GLIDE-Ticks (pos bewegt + af eingefroren): %d (erster: tick %d)\n",
           glide_ticks, glide_first);
    printf("RUN-Schritt^2 min/max: %ld/%ld (Soll ~210^2=44100, Q12-trunkiert)\n",
           step2_min == 0x7fffffffL ? -1L : step2_min, step2_max);
    printf("Sub-6-Handoff nach Run/Turn: %d   Gesten 15/20/16/17 in Reihenfolge: %d/4\n",
           saw_arrival_gesture, gesture_idx);
    printf("=> %s\n", (glide_ticks > 10) ? "GLEITEN REPRODUZIERT" :
                       (run_ticks > 0 ? "Run animiert (kein Glide)" : "Run nie erreicht"));

    /* ===== ctest-PIN (Elliot-Intro-Trace, 7b: NPC-Sub-VM + EM047-Own-Bank) =====
     * (1) Elliot gespawnt; (2) RUN-Phase erreicht (>=20 Ticks st=4/s1=5/s2=2 —
     *     Plc_dest setzt IMMER state 4 + Sub = mode @0x80041c14-18);
     * (3) Frame advanct in JEDEM Run-Tick (f314 jeden Tick im Walk-Sub:
     *     Phase 1 @0x80051630, Phase 2 @0x8005171c, Kanal +0x170/+0x174 @0x80051714/18);
     * (4) kein Glide-Tick; (5) Positions-Delta pro Tick == Gait-Speed 210
     *     (Sub-5-INIT lbu 0x80076c80[(0x47-0x40)*2] @0x800514e4-f0 -> sh +0x8c @0x800514f8;
     *     Savestate orig_1170_gp.sav enemy[0]+0x8c = 210; Q12-Rotation trunkiert ->
     *     Betrag in [205,211]);
     * (6) RUN-Clip = 0 aus der EM047-OWN-Bank (`sb zero,148` @0x8005157c; Bank =
     *     CDEMD0.EMS Blob 21 @Datei 0x336000, dir[4]/dir[3], 6 Clips {22,16,52,1,50,30}
     *     -> Clip-0-Laenge 22, hier ueber re15_actor_clip_len gemessen);
     * (7) Ankunft -> Sub-6-HANDOFF (+0x5=6 @0x80051794/a4 Walk bzw. @0x80051dac Turn;
     *     Turn setzt zusaetzlich das SCD-Flag @0x80051dd8 — der Port-Turn hatte das nicht);
     * (8) SCD-Gesten 15/20/16/17 folgen in Reihenfolge (RBJ-REC-Kanal; REC0 ⊃ REC1
     *     emd_common.c:380ff; Clip 1 spielt in sub02 nie — Flag wird sofort konsumiert). */
    int fail = 0;
    if (es < 0)                      { printf("PIN-FAIL: Elliot nie gespawnt\n"); fail = 1; }
    if (run_ticks < 20)              { printf("PIN-FAIL: RUN-Phase zu kurz (%d)\n", run_ticks); fail = 1; }
    if (run_af_advances != run_ticks){ printf("PIN-FAIL: af-Advances %d != RUN-Ticks %d\n",
                                              run_af_advances, run_ticks); fail = 1; }
    if (run_clip0_ticks != run_ticks){ printf("PIN-FAIL: RUN-Clip nicht durchgehend 0 (%d/%d)\n",
                                              run_clip0_ticks, run_ticks); fail = 1; }
    if (run_fc != 22)                { printf("PIN-FAIL: RUN-Clip-Laenge %d != 22 (EM047-Own-Bank"
                                              " Clip 0 @CDEMD0.EMS 0x336000)\n", run_fc); fail = 1; }
    if (glide_ticks != 0)            { printf("PIN-FAIL: %d Glide-Ticks\n", glide_ticks); fail = 1; }
    if (step2_min < 205L*205L || step2_max > 211L*211L) {
        printf("PIN-FAIL: RUN-Schritt ausserhalb [205,211] (min2=%ld max2=%ld)\n",
               step2_min, step2_max); fail = 1;
    }
    if (!saw_arrival_gesture)        { printf("PIN-FAIL: Sub-6-Handoff nach Run/Turn fehlt\n"); fail = 1; }
    if (gesture_idx != 4)            { printf("PIN-FAIL: Gesten-Sequenz unvollstaendig (%d/4)\n",
                                              gesture_idx); fail = 1; }
    printf("PIN: %s\n", fail ? "FAIL" : "OK");

    free(data);
    return fail;
}
