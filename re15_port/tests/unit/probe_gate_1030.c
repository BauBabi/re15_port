/* probe_gate_1030.c — LANE C MESSUNG (kein Fix): ROOM1030 "Tor + Einlauf-Choreografie".
 *
 * Nutzer-Report: (1) "Zombies kriechen im ORIGINAL unter dem Tor durch, bei uns nicht."
 *                (2) "Im ORIGINAL sind nur SECHS Zombies sichtbar, bei uns Dutzende."
 *
 * Gemessen wird NUR der Ist-Zustand des Ports (keine Engine-Aenderung):
 *   M1  Wieviele Aktoren aktiviert main00 wirklich? (20 Sce_em_set, Datei 0x01de6..0x01f62)
 *   M2  work_vars[17] / work_vars[18] nach main00 — im ORIGINAL sind das
 *       DAT_800b0ff2 (Spawn-Zaehler) und DAT_800b0ff4 (Spawn-CAP), und main00
 *       schreibt @Datei 0x01de2 `24 12 06 00` = Save(work_var[18]=6).
 *       Sce_em_set prueft @0x80042214-38:  if (count >= cap) { entity+0 = 0x8000; return; }
 *   M3  Spawn-Zustand je Gegner (grid/s1/motion) — INIT-Decoder @0x80100f14-54:
 *       behavior 0x0d -> +0x94=0x27, +0x4=0x201 (s1=2), +0x9=0
 *   M4  member_0b (+0x0b) — im ORIGINAL schreibt der AOT-Scan FUN_80042bac
 *       `*(char*)(entity+0xb) = slot` (@0x80042f5c / @0x80042fc4). sub04/sub06 von
 *       ROOM1030 vergleichen Member 15 (== +0x0b) gegen 4 bzw. 5.
 *   M5  anim_flags (+0x1c4) Bits 0x1000 / 0x2000 — sub07 setzt |0x1000, sub05 setzt
 *       (x&0x0fff)|0x2000. Konsument: FUN_80102058 (s1=2 decide) -> state 0x1001 (s1=16).
 *   M6  Laeuft sub02 (der Einlauf-Governor, Datei 0x021a6) ueberhaupt?
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
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

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

static uint8_t s_scratch[0x80000];

static void load_bank(const char *base, uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("  (CDEMD0.EMS nicht lesbar)\n"); return; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                printf("  EM%03X Bank geladen: %d Clips (clip 0x12 fc=%d, clip 0x27 fc=%d)\n",
                       type, eb->anim.clip_count,
                       eb->anim.clip_count > 0x12 ? eb->anim.clips[0x12].frame_count : -1,
                       eb->anim.clip_count > 0x27 ? eb->anim.clips[0x27].frame_count : -1);
            } else printf("  EM%03X parse FAIL\n", type);
        }
    } else printf("  EM%03X kein EMS-Eintrag (idx=%d)\n", type, idx);
    free(ems);
}

static void roster(const char *tag)
{
    int n = 0;
    printf("-- %s --\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        if (s == RE15_ACTOR_SLOT_PLAYER) continue;
        n++;
        printf("   slot=%2d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u mo=%-3d "
               "animflg=0x%04x m0b=%u floor=%u pos=(%6ld,%6ld)\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2,
               (int)a->motion, a->anim_flags, a->member_0b, a->floor,
               (long)a->x, (long)a->z);
    }
    printf("   >>> aktive GEGNER = %d\n", n);
}

static void dump_workvars(const char *tag)
{
    printf("[workvars %-22s] wv[4]=%d wv[5]=%d wv[7]=%d | wv[17](Spawn-Zaehler)=%d "
           "wv[18](Spawn-CAP)=%d\n", tag,
           g_scd.work_vars[4], g_scd.work_vars[5], g_scd.work_vars[7],
           g_scd.work_vars[17], g_scd.work_vars[18]);
}

static void dump_threads(const char *tag)
{
    printf("[threads %-22s]", tag);
    for (int i = 0; i < SCD_THREAD_COUNT; i++)
        if (g_scd.threads[i].active) printf(" %d", i);
    printf("\n");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("ROOM1030: nOmodel=%u nDoor=%u cuts=%d zones=%d subs=%d sca=%d rgn=[%d %d %d %d %d]\n",
           rdt.nOmodel, rdt.nDoor, rdt.cut_count, rdt.zone_count, rdt.sub_scd_count,
           rdt.sca_count, rdt.sca_rgn[0], rdt.sca_rgn[1], rdt.sca_rgn[2],
           rdt.sca_rgn[3], rdt.sca_rgn[4]);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Spieler in die NORD-Haelfte (Tuer 1 Bereich), weit weg von den Gegner-Zonen. */
    pl->x = -16900; pl->y = 0; pl->z = -4900; pl->rot_y = 0;

    printf("== EM-Bank ==\n");
    load_bank(base, 0x16);

    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    printf("\n===== M1/M2/M3: direkt nach main00 + sub00 =====\n");
    roster("Spawn-Roster");
    dump_workvars("nach main00/sub00");
    dump_threads("nach main00/sub00");

    printf("\n-- SCA-Zellen der Gruppen 2 und 3, Eintrag 6 (Sca_id_set-Ziele aus sub00/sub08) --\n");
    {
        int b2 = rdt.sca_rgn[0] + rdt.sca_rgn[1];
        int b3 = b2 + rdt.sca_rgn[2];
        int idx[2] = { b2 + 6, b3 + 6 };
        for (int k = 0; k < 2; k++) {
            if (idx[k] < rdt.sca_count && rdt.sca) {
                const re15_sca_entry_t *e = &rdt.sca[idx[k]];
                printf("   g%d[6] (global #%d): type=%u u0=0x%02x u1=0x%02x floor=%u "
                       "x=%d z=%d w=%u d=%u -> x[%d..%d] z[%d..%d]\n",
                       k + 2, idx[k], e->type, e->u0, e->u1, e->floor,
                       e->x, e->z, e->width, e->density,
                       e->x, e->x + e->width, e->z, e->z + e->density);
            }
        }
    }

    printf("\n-- AOT-Slots (nur nicht-CAM_SWITCH) --\n");
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active) continue;
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH) continue;
        printf("   aot=%2d type=%u ev=%u band=0x%02x sceflg=0x%02x c=(%6ld,%6ld) half=(%5ld,%5ld)\n",
               s, a->type, a->event_id, a->band, a->sce_flags,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }

    /* ---- Phase A: 300 Ticks freie Simulation mit AI ---- */
    printf("\n===== Phase A: 300 Ticks (SCD + AOT + AI) =====\n");
    for (int t = 0; t < 300; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        re15_aot_stamp_entities();      /* LETZTER Zustands-Tick des Frames (FUN_800436a8
                                         * @0x8001ce1c nach AI/Spieler) — wie re15_game_step */
        re15_object_notch_update();
    }
    roster("nach 300 Ticks");
    dump_workvars("nach 300 Ticks");
    dump_threads("nach 300 Ticks");

    /* ---- Phase B: einen Gegner GENAU in das AOT-5-Rect setzen (Tor-Suedstreifen) ----
     * Original: AOT slot 5 rect x[-12900..-3700] z[-25300..-24200], Pool=Gegner (0x42).
     * Erwartung im ORIGINAL: +0x0b := 5, sub06 -> sub07 -> anim_flags |= 0x1000,
     * FUN_80102058 -> +0x4 = 0x1001 (s1=16), Clip 0x12 einmal, danach grid=0x81/s1=0. */
    printf("\n===== Phase B: Gegner in das AOT-5-Rect (-8000,-24700) setzen =====\n");
    int victim = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) { victim = s; break; }
    if (victim < 0) { printf("  KEIN aktiver Gegner — Phase B entfaellt\n"); }
    else {
        printf("  Gegner slot=%d -> (-8000,-24700), floor=0\n", victim);
        for (int t = 0; t < 240; t++) {
            g_actors[victim].x = -8000; g_actors[victim].z = -24700; g_actors[victim].floor = 0;
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            re15_aot_stamp_entities();  /* Frame-Ende wie re15_game_step (@0x8001ce1c) */
            re15_object_notch_update();
            if (t == 0 || t == 1 || t == 5 || t == 30 || t == 120 || t == 239)
                printf("   t=%3d: slot%d grid=0x%02X s1=0x%02x mo=%-3d animflg=0x%04x m0b=%u "
                       "| flag(5,%d)=%d flag(5,34)=%d\n",
                       t, victim, g_actors[victim].grid_id, g_actors[victim].sub_state_1,
                       (int)g_actors[victim].motion, g_actors[victim].anim_flags,
                       g_actors[victim].member_0b,
                       victim - 1, re15_game_flag_get(5, (uint8_t)(victim - 1)),
                       re15_game_flag_get(5, 34));
        }
        roster("nach Phase B");
    }

    /* ---- Phase C: denselben Gegner in das AOT-4-Rect (Tor-Nordstreifen) ---- */
    printf("\n===== Phase C: Gegner in das AOT-4-Rect (-8000,-21400) setzen =====\n");
    if (victim >= 0) {
        for (int t = 0; t < 240; t++) {
            g_actors[victim].x = -8000; g_actors[victim].z = -21400; g_actors[victim].floor = 0;
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            re15_aot_stamp_entities();  /* Frame-Ende wie re15_game_step (@0x8001ce1c) */
            re15_object_notch_update();
            if (t == 0 || t == 1 || t == 5 || t == 30 || t == 239)
                printf("   t=%3d: slot%d grid=0x%02X s1=0x%02x mo=%-3d animflg=0x%04x m0b=%u "
                       "| flag(5,33)=%d\n",
                       t, victim, g_actors[victim].grid_id, g_actors[victim].sub_state_1,
                       (int)g_actors[victim].motion, g_actors[victim].anim_flags,
                       g_actors[victim].member_0b, re15_game_flag_get(5, 33));
        }
        roster("nach Phase C");
    }

    /* ---- Phase D (P2d): END-TO-END — die ECHTE Story-Kette, nur echter SCD-Bytecode. ----
     * Vorbedingungs-Kette (Dossier §12/§14, Bytes selbst gedumpt):
     *   flag(4,0x0f)  — Original-Setzer sub01 @0x2198 `22 04 0f 01` ("Cutscene ist gelaufen").
     *                   Hier direkt gesetzt (der Setzer haengt an flag(3,0x74)+flag(5,0x20)+
     *                   flag(5,0x22) = Spieler-Choreografie); Bank 4 ueberlebt den Re-Init
     *                   (scd_room_reenter wischt nur Bank-1[16..31], flag(2,7), Bank-5-W0).
     *   Raum-Re-Init  — sub00 @0x1fe8 laeuft ERNEUT mit den ECHTEN Bytes: `06 00 1e 00 If /
     *                   21 04 0f 01 Ck / 4b 00 09 .. 4b 06 0c / 37 02 06 f7 / 37 03 06 f7 /
     *                   22 05 14 01` (= flag(5,0x14) @0x2008 + Tor-Zellen 0xF7) und der
     *                   sub00-Tail spawnt `04 ff 18 0b` (sub11) + `04 ff 18 02` (sub02 =
     *                   der Kriech-Zyklus, Glied 6) — KEIN Nachbau in C.
     *   Dann Gegner in Zone 5 parken (wie Phase B) und die Kette beobachten:
     *   Stempel 5 -> sub06 -> Gosub 7 = sub07 @0x2754 `3d 04 10 / 26 op5 OR 0x1000 / 35 10 04`
     *   (jetzt LIVE: 0x3D ist registriert) -> anim_flags|0x1000 -> Steer-Decide 0x1001 ->
     *   Toggle Clip 0x12 -> grid 0x81/sca 8 -> Kriechen Clip 0x1A mit Hand-Lock-Vortrieb.
     *   Parken NUR solange der Gegner im Normalzustand ist — sobald Toggle/Grid-1 uebernimmt,
     *   wird die Position NICHT mehr angefasst (kein Hand-Weiterschieben von Zustand ODER Ort). */
    printf("\n===== Phase D (P2d): END-TO-END Story-Kette (echter Bytecode) =====\n");
    re15_game_flag_set(4, 0x0f, 1);                 /* sub01-Setzer @0x2198 (Vorbedingung) */
    scd_room_reenter(&rdt, pl->x, pl->z, 0);        /* echter Raum-Re-Init: main00 + sub00 */
    printf("  nach Re-Init: flag(3,0x74)=%d flag(4,0x0f)=%d flag(5,0x14)=%d\n",
           re15_game_flag_get(3, 0x74), re15_game_flag_get(4, 0x0f), re15_game_flag_get(5, 0x14));
    dump_threads("nach Re-Init");
    roster("Phase-D-Spawn");
    victim = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) { victim = s; break; }
    if (victim < 0 || !re15_game_flag_get(5, 0x14)) {
        printf("  HAENGER VOR DEM ZYKLUS: victim=%d flag(5,0x14)=%d — sub00-Gate nicht gefeuert\n",
               victim, re15_game_flag_get(5, 0x14));
    } else {
        re15_actor_t *v = &g_actors[victim];
        int t_stamp = -1, t_bit1000 = -1, t_s110 = -1, t_grid81 = -1, t_mo1a = -1, t_lock0 = -1;
        int t_rueck = -1, t_standup = -1;
        int32_t cx0 = 0, cz0 = 0, rx = 0, rz = 0;
        printf("  Gegner slot=%d (Enemy-Index %d, Bank-5-Latch Bit %d) -> Zone 5 (-8000,-24700)\n",
               victim, victim - 1, victim - 1);
        /* WARTENDER Zombie (Diagnose-Task 4): einen ZWEITEN Gegner ebenfalls in Zone 5 parken.
         * Sobald die wv7-Drossel (>=4 Zombies in Zone 6) zu ist, bekommt er den Stempel 5, aber
         * sub06/Gosub-7 feuert nie -> er bleibt in der normalen AI vor dem GESCHLOSSENEN Tor
         * (Maske 4 vs. Zellen-0xF7). Geloggt: s1/motion/rot_y + Zustandswechsel-Zaehler. */
        int wait_slot = -1;
        for (int s = victim + 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active) { wait_slot = s; break; }
        int wait_s1_changes = 0, wait_mo_changes = 0; int wait_last_s1 = -1, wait_last_mo = -1;
        if (wait_slot > 0)
            printf("  WARTENDER slot=%d -> Zone 5 (-6000,-24700)\n", wait_slot);
        for (int t = 0; t < 600; t++) {
            if (wait_slot > 0) {
                re15_actor_t *w = &g_actors[wait_slot];
                int wtaken = (w->grid_id & 0x80) ||
                             (w->state == 1 && (w->sub_state_1 == 0x10 || (w->grid_id & 0x0f) == 1));
                if (!wtaken) { w->x = -6000; w->z = -24700; w->floor = 0; }
                if (wait_last_s1 >= 0 && w->sub_state_1 != wait_last_s1) wait_s1_changes++;
                if (wait_last_mo >= 0 && (int)w->motion != wait_last_mo) wait_mo_changes++;
                wait_last_s1 = w->sub_state_1; wait_last_mo = (int)w->motion;
                if ((t % 120) == 0)
                    printf("   [warte t=%3d] slot%d grid=0x%02x s1=0x%02x mo=0x%02x af=0x%04x m0b=%u rot=%d\n",
                           t, wait_slot, w->grid_id, w->sub_state_1, (unsigned)w->motion,
                           w->anim_flags, w->member_0b, (int)w->rot_y);
            }
            /* Parken nur bis der Toggle/Grid-1 uebernimmt (danach bewegt der Hand-Lock). */
            int taken = (v->grid_id & 0x80) ||
                        (v->state == 1 && (v->sub_state_1 == 0x10 || (v->grid_id & 0x0f) == 1));
            if (!taken) { v->x = -8000; v->z = -24700; v->floor = 0; }
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            re15_aot_stamp_entities();
            re15_object_notch_update();
            if (t_stamp   < 0 && v->member_0b == 5)          { t_stamp = t;   printf("   t=%3d KETTE: m0b=5 (Glied 1, Zone-5-Stempel)\n", t); }
            if (t_bit1000 < 0 && (v->anim_flags & 0x1000))   { t_bit1000 = t; printf("   t=%3d KETTE: anim_flags|=0x1000 (sub07 @0x2754 via 0x3D/0x26/0x35; af=0x%04x wv4=%d)\n", t, v->anim_flags, g_scd.work_vars[4]); }
            if (t_s110    < 0 && v->state == 1 && v->sub_state_1 == 0x10)
                                                             { t_s110 = t;    printf("   t=%3d KETTE: Wort 0x1001 -> Sub-Modus 0x10 (Steer-Decide @0x80101ecc-Klasse)\n", t); }
            if (t_grid81  < 0 && v->grid_id == 0x81)         { t_grid81 = t;  cx0 = v->x; cz0 = v->z;
                                                               printf("   t=%3d KETTE: Kriech-Commit grid=0x81 sca=%u (Toggle Phase 2)\n", t, v->sca_mask); }
            if (t_mo1a    < 0 && v->motion == 0x1A)          { t_mo1a = t;    printf("   t=%3d KETTE: Clip 0x1A (Kriech-Lokomotion, ai_timer=%d)\n", t, (int)v->ai_timer); }
            if (t_lock0   < 0 && t_grid81 >= 0 && (v->x != -8000 || v->z != -24700) && (v->x != cx0 || v->z != cz0))
                                                             { t_lock0 = t;   printf("   t=%3d KETTE: erster Hand-Lock-Schritt pos=(%ld,%ld)\n", t, (long)v->x, (long)v->z); }
            if (t_rueck   < 0 && t_grid81 >= 0 && (v->grid_id & 0x80) && v->sub_state_1 == 6)
                                                             { t_rueck = t;   rx = v->x; rz = v->z;
                                                               printf("   t=%3d KETTE: RUECKWEG — Zone-4-Stempel -> sub05 (af=0x%04x) -> Wort 0x601 -> Toggle, Kriech-Strecke dx=%ld dz=%ld\n",
                                                                      t, v->anim_flags, (long)(rx - cx0), (long)(rz - cz0)); }
            if (t_standup < 0 && t_rueck >= 0 && !(v->grid_id & 0x80))
                                                             { t_standup = t; printf("   t=%3d KETTE: AUFGESTANDEN — grid=0x%02x sca=%u Wort=%u/0x%02x\n",
                                                                      t, v->grid_id, v->sca_mask, v->state, v->sub_state_1); }
            if ((t % 60) == 59 || t == 0)
                printf("   t=%3d: grid=0x%02X s1=0x%02x mo=0x%02x af=0x%04x m0b=%u sca=%u pos=(%6ld,%6ld) "
                       "wv4=%d wv7=%d f(5,0x14)=%d latch(5,%d)=%d aot4/5/6-sce=%u/%u/%u\n",
                       t, v->grid_id, v->sub_state_1, (unsigned)v->motion, v->anim_flags,
                       v->member_0b, v->sca_mask, (long)v->x, (long)v->z,
                       g_scd.work_vars[4], g_scd.work_vars[7],
                       re15_game_flag_get(5, 0x14), victim - 1,
                       re15_game_flag_get(5, (uint8_t)(victim - 1)),
                       g_aot.slots[4].sce_flags ? g_aot.slots[4].type : 0,
                       g_aot.slots[5].sce_flags ? g_aot.slots[5].type : 0,
                       g_aot.slots[6].sce_flags ? g_aot.slots[6].type : 0);
        }
        if (wait_slot > 0) {
            re15_actor_t *w = &g_actors[wait_slot];
            printf("  WARTENDER-Bilanz: s1-Wechsel=%d motion-Wechsel=%d Endzustand s1=0x%02x mo=0x%02x af=0x%04x grid=0x%02x\n",
                   wait_s1_changes, wait_mo_changes, w->sub_state_1, (unsigned)w->motion,
                   w->anim_flags, w->grid_id);
        }
        printf("  -- Phase-D-Bilanz --\n");
        printf("   Stempel=%d  0x1000=%d  0x1001=%d  grid81=%d  clip1A=%d  ersterLock=%d  Rueckweg=%d  aufgestanden=%d (Tick-Nummern; -1 = nie)\n",
               t_stamp, t_bit1000, t_s110, t_grid81, t_mo1a, t_lock0, t_rueck, t_standup);
        if (t_rueck >= 0)
            printf("   Kriech-Strecke Commit->Rueckweg: dx=%ld dz=%ld (von (%ld,%ld) nach (%ld,%ld) in %d Ticks)\n",
                   (long)(rx - cx0), (long)(rz - cz0), (long)cx0, (long)cz0, (long)rx, (long)rz, t_rueck - t_grid81);
        else if (t_grid81 >= 0)
            printf("   Kriech-Vortrieb seit Commit: dx=%ld dz=%ld (von (%ld,%ld) nach (%ld,%ld))\n",
                   (long)(v->x - cx0), (long)(v->z - cz0), (long)cx0, (long)cz0, (long)v->x, (long)v->z);
        /* Zweitzyklus-Beobachtung: nach dem Aufstehen bleibt der HINWEG-Poller (Gosub 6) hinter
         * der Drossel `Cmp(wv7 < 4)` (sub02 @0x21cc `23 00 07 03 04 00`) gesperrt, solange >= 4
         * Zombies in Zone 6 stehen — sub03 zaehlt member15==6 ueber die 20 Slots (Save(wv7,0) +
         * For/Work_set(2,wv4)/Member_cmp(15==6)/wv7++, Bytes @0x21f8-2224). Nach dem Re-Entry-
         * Repositioning (sub00-For @0x2028+) stehen 5 der 6 Zombies IN Zone 6 -> wv7=5..6 >= 4 =
         * byte-true GESCHLOSSEN, kein Haenger. */
        if (t_bit1000 < 0) {
            printf("   HAENGER-DIAGNOSE: sub07 hat nie gefeuert.\n");
            dump_threads("Haenger");
            dump_workvars("Haenger");
            printf("   flag(5,0x14)=%d flag(5,latch %d)=%d m0b=%u — Kettenglied davor pruefen\n",
                   re15_game_flag_get(5, 0x14), victim - 1,
                   re15_game_flag_get(5, (uint8_t)(victim - 1)), v->member_0b);
        }
        roster("nach Phase D");
    }

    printf("\n===== BEFUND =====\n");
    {
        int n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) n++;
        printf("  aktive Gegner am Ende      = %d   (ORIGINAL-Erwartung: 6, Cap work_var[18]=6)\n", n);
        printf("  work_vars[17] (Zaehler)    = %d   (ORIGINAL: 6 nach main00)\n", g_scd.work_vars[17]);
        printf("  work_vars[18] (Cap)        = %d   (ORIGINAL: 6, gesetzt @Datei 0x01de2)\n",
               g_scd.work_vars[18]);
    }
    free(dat);
    return 0;
}
