/* probe_room1070_wake.c — DIAGNOSE (kein ctest-Assert): Nutzer-Report 2026-08-05:
 * "Die Zombies in ROOM1070 sollten ab einem gewissen Punkt zum Aufwachen triggern,
 *  tun sie aber nicht."
 *
 * ORIGINAL-MECHANISMUS (RE'd, siehe analysis/bug_room1070_wake.md):
 *  - ROOM1070 sub_scd[0] @Datei 0x15AE:
 *      If / Ck(zone=4, bit=0xC5, ==0)              (Datei 0x15AE/0x15B2)
 *        Aot_set  slot 14, sce=3, flags=0x41, band=0, rect x=10000 z=-1000 w=1400 d=21000,
 *                 eventId = pc[17] = 0x02          (Datei 0x15B6)  -> AUTO (Bit 0x10 CLEAR)
 *        Sce_em_set slot0 type0x10 beh=0x88        (Datei 0x15CA)
 *        Sce_em_set slot1 type0x10 beh=0x88        (Datei 0x15DE)
 *        Sce_em_set slot2 type0x10 beh=0x87        (Datei 0x15F2)
 *        Sce_em_set slot3 type0x11 beh=0x88        (Datei 0x1606)
 *        Sce_em_set slot4 type0x11 beh=0x87        (Datei 0x161A)
 *      Else -> dieselben 5 Spawns mit beh=0x00 (schon wach)
 *  - sub_scd[2] @Datei 0x169C (= das AOT-Event):
 *      Set(4,0xC5,1); Aot_reset(14);
 *      Work_set(2,0) Member_set(12,0x8A) | (2,1) 0x8A | (2,2) 0x89 | (2,3) 0x8A | (2,4) 0x89
 *      -> Member 12 = entity +0x9 (grid/behavior). 0x88 -> 0x8A, 0x87 -> 0x89 (Nibble +2).
 *  - Nibble-Dispatch @0x8011f80c: [7]/[8] = 0x80101974, [9]/[10] = 0x801019f0.
 *      0x80101974 ruft decide-Row @0x8011f9d8[+0x5] = 0x801039f4 = `jr ra` (NICHTS)
 *      0x801019f0 ruft decide-Row @0x8011f9dc[+0x5] = 0x801039fc = DER WAKE:
 *          lbu +0x6; bne !=0 -> ret; sb 2,+0x6; sb 0xF,+0x8f; rng&3==0 -> SE 5
 *      beide danach animate-Row @0x8011f9d4[+0x5] = FUN_80103a58 (+0x6-Phasenmaschine).
 *  - FUN_80103a58: Phase0 +0x93|=1 / +0x1b8=1 (passiv); Phase1 +0x9c-- -> Phase2;
 *      Phase2 anim_set(+0x170,+0x174,0,0x100), +0x6 += done  (Aufsteh-Clip 0x12/0x13!);
 *      Phase3 +0x9=0, word +0x4=0x201 (ENGAGE), +0x93&=0xfe, +0x1b8=0.
 *
 * PORT-HYPOTHESEN (statisch gefunden, hier dynamisch zu belegen):
 *  H1  sub00 spawnt 5 Zombies mit grid 0x88/0x88/0x87/0x88/0x87 (Decoder: pose-only,
 *      +0x5 bleibt 0, motion 0x13/0x13/0x12/0x13/0x12).
 *  H2  Das AUTO-AOT 14 feuert beim Betreten des Streifens sub_scd[2] und die grids
 *      werden 0x8A/0x8A/0x89/0x8A/0x89.
 *  H3  `switch (e->grid_id & 0xf)` in enemy_ai_common.c kennt 7/8, aber NICHT 9/10 ->
 *      `default: break` -> nach dem Wake-Rewrite passiert GAR NICHTS mehr (tote Statue).
 *  H4  Selbst mit Nibble 9/10 wuerde der Aufsteh-Clip nicht laufen: der globale Advancer
 *      (player_common.c) pinnt motion 0x12/0x13 hart auf Frame 0 (nur `getup`
 *      sub_state_1==0x11 && sub_state_2>=4 loest) -> clip_done nie -> Phase 2 haengt.
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
#include "re15_emd.h"
#include "re15_aot.h"
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

static void load_bank(const char *base, uint8_t type, uint8_t *scratch, size_t scratch_sz)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= scratch_sz) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                printf("EM%03X bank: %d clips  fc[0x12]=%d fc[0x13]=%d\n", type,
                       eb->anim.clip_count,
                       eb->anim.clip_count > 0x12 ? eb->anim.clips[0x12].frame_count : -1,
                       eb->anim.clip_count > 0x13 ? eb->anim.clips[0x13].frame_count : -1);
            }
        }
    }
    free(ems);
}

static void roster(const char *tag)
{
    printf("-- %s --\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        printf("   slot=%d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u s3=%u mo=%-3d fr=%-3u "
               "hp=%d hitr=0x%02x pos=(%ld,%ld)\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
               (int)a->motion, a->anim_frame, (int)a->hp, a->hit_react, (long)a->x, (long)a->z);
    }
}

static uint8_t s_scratch[0x80000];

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1070.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1070;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Startposition SUEDLICH/WESTLICH des Trigger-Streifens (x 10000..11400) */
    pl->x = 2000; pl->y = 0; pl->z = 5000; pl->rot_y = 1024;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    load_bank(base, 0x10, s_scratch, sizeof s_scratch);
    load_bank(base, 0x11, s_scratch, sizeof s_scratch);

    for (int f = 0; f < 8; f++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }
    roster("H1: Spawn-Roster nach sub00 (erwartet grid 88/88/87/88/87)");

    printf("-- AOT-Slots --\n");
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active) continue;
        printf("   aot=%2d type=%u ev=%u band=0x%02x flags=0x%02x c=(%ld,%ld) half=(%ld,%ld)\n",
               s, a->type, a->event_id, a->band, a->sce_flags,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }

    {
        extern int re15_collision_debug_band(void);
        printf("-- Scan-Gates -- player_mode=%d letterbox=%d msg_q=%d msg_frames=%d band=%d "
               "aot14.active=%d\n",
               (int)g_scd.player_mode, (int)g_scd.letterbox_countdown,
               (int)g_scd.message_query, (int)g_scd.message_display_frames,
               re15_collision_debug_band(), (int)g_aot.slots[14].active);
    }

    /* Phase A — Spieler fern vom Trigger: laufen die Lyer passiv? */
    printf("== Phase A: Spieler fern vom Trigger-Streifen (200 Ticks) ==\n");
    for (int t = 0; t < 200; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
    }
    roster("Nach Phase A");

    /* Phase B — Spieler IN den AOT-14-Streifen (x 10000..11400, z -1000..20000) */
    printf("== Phase B: Spieler betritt AOT 14 (x=10700, z=5000) ==\n");
    pl->x = 10700; pl->z = 5000;
    uint8_t grid_before[8];
    for (int s = 1; s <= 5; s++) grid_before[s] = g_actors[s].grid_id;
    int rewrote = -1;
    for (int t = 0; t < 5; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        int changed = 0;
        for (int s = 1; s <= 5; s++) if (g_actors[s].grid_id != grid_before[s]) changed = 1;
        if (changed && rewrote < 0) rewrote = t;
    }
    printf("H2 grid-Rewrite durch sub_scd[2]: %s (Tick %d), Flag(4,197)=%d\n",
           rewrote >= 0 ? "JA" : "NIE", rewrote, re15_game_flag_get(4, 197));
    printf("   aot14: active=%d type=%u ev=%u cam_from=0x%02x was_inside=%u\n",
           g_aot.slots[14].active, g_aot.slots[14].type, g_aot.slots[14].event_id,
           g_aot.slots[14].cam_from_filter, g_aot.slots[14].was_inside);
    if (rewrote < 0) {
        printf("   -> Isolation: re15_aot_fire_slot(14) direkt\n");
        re15_aot_fire_slot(14);
        for (int t = 0; t < 4; t++) { scd_vm_tick(); }
        printf("   nach Direkt-Fire: Flag(4,197)=%d grid=%02X/%02X/%02X/%02X/%02X\n",
               re15_game_flag_get(4, 197),
               g_actors[1].grid_id, g_actors[2].grid_id, g_actors[3].grid_id,
               g_actors[4].grid_id, g_actors[5].grid_id);
    }
    roster("Direkt nach dem AOT-Fire (erwartet grid 8A/8A/89/8A/89)");

    /* Phase C — 600 Ticks nach dem Wake-Rewrite: stehen sie auf? */
    printf("== Phase C: 600 Ticks nach dem Rewrite ==\n");
    int woke[8], rose[8], eng[8];
    for (int s = 0; s < 8; s++) { woke[s] = rose[s] = eng[s] = -1; }
    for (int t = 0; t < 600; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        for (int s = 1; s <= 5; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (woke[s] < 0 && a->sub_state_2 >= 2) woke[s] = t;
            if (rose[s] < 0 && a->anim_frame > 0)   rose[s] = t;
            if (eng[s]  < 0 && a->grid_id == 0 && a->state == 1 && a->sub_state_1 == 2) eng[s] = t;
        }
        if (t == 0 || t == 59 || t == 299) {
            char tag[64]; snprintf(tag, sizeof tag, "Tick %d", t);
            roster(tag);
        }
    }
    roster("Ende Phase C");

    printf("\n== BEFUNDE ==\n");
    for (int s = 1; s <= 5; s++) {
        const re15_actor_t *a = &g_actors[s];
        printf("  slot%d grid 0x%02X->0x%02X  Wake(+0x6>=2): %s(%d)  Clip laeuft(fr>0): %s(%d)  "
               "ENGAGE: %s(%d)\n",
               s, grid_before[s], a->grid_id,
               woke[s] >= 0 ? "JA" : "NIE", woke[s],
               rose[s] >= 0 ? "JA" : "NIE", rose[s],
               eng[s]  >= 0 ? "JA" : "NIE", eng[s]);
    }
    /* REGRESSIONS-GATE (Nutzer-Report BUG 5). Nach dem Skript-Bump auf Nibble 9/0xA muss JEDER
     * der fuenf Zombies (a) sofort aufwachen (Decide FUN_801039fc @0x801039fc: +0x6 = 2), (b) den
     * Aufsteh-Clip WIRKLICH abspielen (Phase 2 = der einzige f314-Call @0x80103b14 — der globale
     * Anim-Advancer pinnte 0x12/0x13 vorher hart auf Frame 0) und (c) Phase 3 erreichen
     * (@0x80103b3c-78: +0x9 = 0, Zustandswort 0x201, +0x93 &= 0xfe = wieder beschiessbar).
     * Vorher: 600 Ticks lang KEINE Reaktion, weil `switch (grid_id & 0xf)` die Faelle 9/10 gar
     * nicht kannte (default:break) — und die Zombies blieben dauerhaft UNVERWUNDBAR. */
    {
        int bad = 0;
        for (int s = 1; s <= 5; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (woke[s] < 0) { printf("FAIL: slot%d ist nie aufgewacht\n", s); bad = 1; }
            if (rose[s] < 0) { printf("FAIL: slot%d hat den Aufsteh-Clip nie abgespielt\n", s); bad = 1; }
            if (eng[s]  < 0) { printf("FAIL: slot%d hat ENGAGE nie erreicht\n", s); bad = 1; }
            if ((a->grid_id & 0x0f) != 0) {
                printf("FAIL: slot%d grid=0x%02X (Phase 3 nicht erreicht)\n", s, a->grid_id); bad = 1;
            }
            if (a->hit_react & 1) {
                printf("FAIL: slot%d bleibt unverwundbar (hit_react=0x%02X, +0x93 Bit 0 gesetzt)\n",
                       s, a->hit_react); bad = 1;
            }
        }
        if (bad) { printf("\nERGEBNIS: ROOM1070-Wake DEFEKT\n"); return 1; }
        printf("  alle 5 wach, aufgestanden, engaged und wieder beschiessbar\n");
    }

    /* Phase D — ISOLATIONS-TEST fuer H4: Nibble kuenstlich auf 8 (dispatchbar) setzen und
     * die Wake-Phase 2 direkt stellen. Laeuft der Aufsteh-Clip dann? */
    printf("== Phase D: H4-Isolation — grid=0x88, +0x6=2 (Phase 2) von Hand ==\n");
    {
        re15_actor_t *a = &g_actors[1];
        a->grid_id = 0x88; a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 2;
        a->anim_frame = 0;
        for (int t = 0; t < 120; t++) {
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            if (t < 6 || t % 30 == 0)
                printf("   t=%3d grid=0x%02X s2=%u mo=%d fr=%u clip_len=%d\n",
                       t, a->grid_id, a->sub_state_2, (int)a->motion, a->anim_frame,
                       re15_actor_clip_len(a));
        }
        printf("H4: nach 120 Ticks s2=%u fr=%u grid=0x%02X -> %s\n",
               a->sub_state_2, a->anim_frame, a->grid_id,
               (a->anim_frame == 0) ? "CLIP-PIN BESTAETIGT (Frame haengt auf 0)" : "Clip laeuft");
    }

    /* Phase E — FIX-SIMULATION (kein Engine-Code geaendert): die beiden fehlenden Teile
     * werden hier von Hand nachgebildet, um zu belegen, dass der Plan zum Ziel fuehrt.
     *   (1) Nibble-9/10-Dispatch: decide 0x801039fc + animate FUN_80103a58 (Phasen 0..3)
     *   (2) Anim-Pin-Freigabe fuer motion 0x12/0x13 waehrend Phase >= 2 */
    printf("== Phase E: FIX-SIMULATION (Nibble 9/10 + Pin-Freigabe) ==\n");
    {
        /* Raum frisch aufsetzen, damit die Zombies wieder als 0x87/0x88 liegen */
        re15_actor_init();
        scd_vm_init();
        g_current_room_id = 0x1070;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = 2000; pl->y = 0; pl->z = 5000; pl->rot_y = 1024;
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        for (int f = 0; f < 8; f++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        }
        pl->x = 10700; pl->z = 5000;
        int engaged_at[8]; int shadow_fr[8];
        for (int s = 0; s < 8; s++) { engaged_at[s] = -1; shadow_fr[s] = 0; }
        for (int t = 0; t < 400; t++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            /* (2) Pin-Freigabe emuliert: der globale Advancer pinnt mo 0x12/0x13 auf 0 —
             *     hier NACH ihm die Wake-Phase selbst steppen (= was der Fix im Advancer tut) */
            for (int s = 1; s <= 5; s++) {
                re15_actor_t *a = &g_actors[s];
                int nb = a->grid_id & 0xf;
                if (a->state == 1 && a->sub_state_1 == 0 && (nb == 9 || nb == 10) &&
                    a->sub_state_2 >= 2 && (a->motion == 0x12 || a->motion == 0x13)) {
                    int fc = re15_actor_clip_len(a);
                    if (fc > 0 && shadow_fr[s] < fc - 1) shadow_fr[s]++;
                    a->anim_frame = (uint16_t)shadow_fr[s];   /* = Pin freigegeben */
                }
            }
            re15_enemy_ai_run_all(0);
            /* (1) Nibble-9/10-Maschine */
            for (int s = 1; s <= 5; s++) {
                re15_actor_t *a = &g_actors[s];
                int nb = a->grid_id & 0xf;
                if (a->state != 1 || a->sub_state_1 != 0 || (nb != 9 && nb != 10)) continue;
                if (a->sub_state_2 == 0) { a->sub_state_2 = 2; a->anim_frac = 0xf; }  /* 0x801039fc */
                switch (a->sub_state_2) {                                             /* 0x80103a58 */
                    case 0: a->hit_react |= 1; break;
                    case 1: if (a->ai_timer != 0) a->ai_timer--; else { a->sub_state_2 = 2; a->anim_frac = 0xf; } break;
                    case 2: {
                        int fc = re15_actor_clip_len(a);
                        if (fc > 0 && (int)a->anim_frame >= fc - 1) a->sub_state_2 = 3;
                        break;
                    }
                    default:
                        a->grid_id = 0; a->state = 1; a->sub_state_1 = 2;
                        a->sub_state_2 = 0; a->sub_state_3 = 0;
                        a->hit_react = (uint8_t)(a->hit_react & ~1u);
                        break;
                }
                if (engaged_at[s] < 0 && a->grid_id == 0 && a->sub_state_1 == 2) engaged_at[s] = t;
            }
            if (t == 5 || t == 60 || t == 130) {
                char tag[64]; snprintf(tag, sizeof tag, "SIM Tick %d", t);
                roster(tag);
            }
        }
        roster("SIM Ende");
        for (int s = 1; s <= 5; s++)
            printf("  SIM slot%d ENGAGE nach %d Ticks\n", s, engaged_at[s]);
    }

    free(data);
    return 0;
}
