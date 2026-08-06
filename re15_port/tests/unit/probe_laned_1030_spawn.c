/* probe_laned_1030_spawn.c — LANE D MESSUNG (kein Fix): Nutzer-Report 2026-08-06 zu ROOM1030
 *   (1) "Die Zombies kriechen im ORIGINAL unter dem Tor durch. Bei uns nicht."
 *   (2) "Im ORIGINAL sind nur SECHS Zombies sichtbar. Bei uns sind es Dutzende."
 *
 * Zweck: den ECHTEN Live-Zustand des Ports protokollieren — RDT laden, EM016-Bank laden,
 * main00 + sub00 fahren, dann N Ticks MIT der echten AI (re15_enemy_ai_run_all) laufen
 * lassen. Gemessen wird pro Tick:
 *   - wie viele Aktoren existieren / aktiv sind, mit Typ/grid/state/pos
 *   - ob NEUE Aktoren waehrend des Laufs entstehen (per-Frame-sub01-Reseed-Verdacht)
 *   - wie viele Aktoren ein Modell/eine Bank haetten (Render-Kandidaten)
 *   - member_0b (+0x0b, Member id 15) — der AOT-Slot-Stempel, den die ROOM1030-Stream-
 *     Maschine (sub03/04/06/09) abfragt
 *   - anim_flags (+0x1c4, Member id 16) Bits 0x1000/0x2000 — die Kriech-/Aufsteh-Bits
 *
 * ORIGINAL-Referenz (belegt, Datei-Offsets ROOM1030.RDT / STAGE1.BIN / PSX.EXE):
 *   main00 f0x1de6..0x1f62: 20x Sce_em_set, Typ 0x16, behavior 0x0d, kill-Flag = pc[7] = slot
 *   INIT-Decoder @0x80100f14-54: (behavior & 0x1f) == 0x0d -> +0x94=0x27, +0x4-Wort=0x201
 *     (state 1 / +0x5=2 = ENGAGE), +0x9 = 0
 *   Spawn-Handler FUN_800420a0 @0x8004254c-60: g_active_count (DAT_800aca4e) += 1 pro Spawn
 *   AOT-Pool-Scan FUN_800436a8 @0x8004371c: entity+0x0b JEDEN FRAME = 0, dann
 *     FUN_80042bac @0x80042f5c/0x80042fc4 `sb (aot_slot),11(entity)` = AOT-Stempel
 *   sub09 f0x27e0 / sub07 f0x2754: Member 0x10 (+0x1c4) |= 0x1000  -> Kriech-Trigger
 *   Zombie-Animate @0x80101ec4 / 0x801021cc / 0x80105790: `lhu +0x1c4; andi 0x1000;
 *     bne -> sw 0x1001,4` = state 1 / +0x5 = 0x10 -> animate[16] @0x80104f80 (Clip 0x12,
 *     danach +0x9 = 0x81 = kriechender/liegender Zombie)
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

/* EM016-Bank laden (Muster load_em10_bank aus probe_zreentry_10d0) */
static int load_em_bank(const char *base, uint8_t type)
{
    static uint8_t s_em[0x80000];
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "WARN: CDEMD0.EMS nicht lesbar\n"); return 0; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
        len <= sizeof s_em) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_em, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_em, len, &eb->md1, &eb->skel,
                                         &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL; ok = 1;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_em, len,
                                  &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok = (re15_emd_parse_own_bank(s_em, len,
                                  &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
    return ok;
}

static int count_active(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) n++;
    return n;
}

static void dump_roster(const char *tag)
{
    printf("== Roster %s (RE15_ACTOR_MAX=%d, g_actor_count=%u) ==\n",
           tag, RE15_ACTOR_MAX, (unsigned)g_actor_count);
    int n = 0, banked = 0;
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        if (s != RE15_ACTOR_SLOT_PLAYER) n++;
        re15_enemy_bank_t *eb = (s == RE15_ACTOR_SLOT_PLAYER) ? NULL
                                                              : re15_enemy_find(a->type);
        if (eb && eb->ok) banked++;
        printf("  slot=%2d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u mo=%-3d fr=%-3u "
               "af=0x%04x m0b=%u flags=0x%02x emfl=0x%02x hp=%d floor=%u pos=(%6ld,%6ld,%6ld) rot=%d bank=%s\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2,
               (int)a->motion, a->anim_frame, a->anim_flags, a->member_0b,
               a->flags, a->em_flag_id, (int)a->hp, a->floor,
               (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y,
               (eb && eb->ok) ? "OK" : "-");
    }
    printf("  -> Gegner aktiv = %d, davon mit geladener Bank (= renderbar) = %d\n", n, banked);
}

static void dump_aots(void)
{
    printf("== AOT-Tabelle (ohne CAM_SWITCH) ==\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH) continue;
        printf("  slot=%2d type=%2u ev=%3u sce_flags=0x%02x band=0x%02x "
               "rect c=(%ld,%ld) half=(%ld,%ld)  x:[%ld..%ld] z:[%ld..%ld]\n",
               i, a->type, a->event_id, a->sce_flags, a->band,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h));
    }
}

static void tick_once(int32_t px, int32_t pz)
{
    scd_vm_tick();
    re15_aot_scan(px, pz, 0);
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;
    int ticks = (argc > 1) ? atoi(argv[1]) : 600;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("ROOM1030: cuts=%d zones=%d sub_scd_count=%d\n",
           rdt.cut_count, rdt.zone_count, rdt.sub_scd_count);

    /* ⚠ AUFBAU-PFLICHT (Lehre Treppen-Runde): der LIVE-Lauf publiziert das RDT in
     * g_room_rdt/g_room_rdt_ok (platform/pc/main.c:2029-2031, room_pc.c:77). OHNE das
     * laufen weder der Gegner-SCA-Wand-Clamp (enemy_ai_common.c:8975 `if (g_room_rdt_ok…)`)
     * noch der LOS-Ray (enemy_ai_common.c:1565) — die Zahlen waeren wertlos. */
    g_room_rdt    = rdt;
    g_room_rdt_ok = 1;

    re15_actor_init();
    scd_vm_init();
    re15_enemy_reset();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Tuer 0 (main00 +0x000e Door_aot_set slot=0): Spieler in den Raum-Nordteil.
     * argv[2]=="far": Spieler AUSSERHALB von AOT3 (x:[-19900..-16200] z:[-9200..-7400]),
     * damit Flag(5,32) NICHT gesetzt wird -> die sub01-Cutscene (und damit Sca_id_set =
     * Tor-Oeffnung) feuert NICHT. Isoliert: haelt die Kollision die Zombies hinten? */
    if (argc > 2 && argv[2][0] == 'f') { pl->x = 2250;   pl->z = -12700; }
    else                               { pl->x = -19900; pl->z = -9200;  }
    pl->y = 0; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    /* AUFBAU-VERIFIKATION (Lehre aus der Treppen-Runde): Bank wirklich geladen? */
    int bank_ok = load_em_bank(base, 0x16);
    printf("[setup] EM016-Bank geladen = %d ; re15_enemy_find(0x16)=%p\n",
           bank_ok, (void *)re15_enemy_find(0x16));

    for (int f = 0; f < 8; f++) scd_vm_tick();

    dump_aots();
    /* SCA-Dump: Sca_id_set(2,6,0xf7)/(3,6,0xf7) in sub08 (f0x278e/0x2792) und sub00 (f0x2000/
     * 0x2004) schreiben Byte +0x9 (=u0) einer 12-B-Kollisionszelle. ORIGINAL-Adressierung
     * @0x8004175c: `v1 = table[pc[1] + 1]` (addiu v1,v1,1 VOR sll 2) — der Port rechnet
     * sca_entry_at(region) OHNE +1. Beide Kandidaten hier ausgeben. */
    printf("== SCA (count=%d, rgn=%d/%d/%d/%d/%d, ceiling=(%u,%u)) ==\n",
           rdt.sca_count, rdt.sca_rgn[0], rdt.sca_rgn[1], rdt.sca_rgn[2],
           rdt.sca_rgn[3], rdt.sca_rgn[4], rdt.ceiling_x, rdt.ceiling_z);
    {
        int base = 0;
        for (int g2 = 0; g2 < 5; g2++) {
            for (int k = 0; k < rdt.sca_rgn[g2] && base + k < rdt.sca_count; k++) {
                const re15_sca_entry_t *c = &rdt.sca[base + k];
                /* nur Zellen nahe der Tor-Linie z in [-27000..-23000] zeigen + die Indizes 6 */
                if (k == 6 || ((int)c->z >= -27000 && (int)c->z <= -23000))
                    printf("  rgn=%d idx=%2d flat=%3d typ=%u u0=0x%02x u1=0x%02x floor=0x%02x "
                           "rect x:[%d..%d] z:[%d..%d]%s\n",
                           g2, k, base + k, c->type, c->u0, c->u1, c->floor,
                           (int)c->x, (int)c->x + (int)c->width,
                           (int)c->z, (int)c->z + (int)c->density,
                           (k == 6) ? "   <== Sca_id_set-Index 6" : "");
            }
            base += rdt.sca_rgn[g2];
        }
    }
    dump_roster("nach main00/sub00 + 8 VM-Ticks");

    /* Spawn-Zaehlung pro Tick: entsteht pro Tick ein NEUER Aktor (sub01-Reseed-Verdacht)? */
    printf("\n== %d Ticks mit ECHTER AI (re15_enemy_ai_run_all) ==\n", ticks);
    int prev_n = count_active();
    printf("[t=%4d] aktiv=%d\n", -1, prev_n);
    uint8_t prev_state[RE15_ACTOR_MAX], prev_s1[RE15_ACTOR_MAX], prev_grid[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        prev_state[s] = g_actors[s].state; prev_s1[s] = g_actors[s].sub_state_1;
        prev_grid[s]  = g_actors[s].grid_id;
    }
    int ai_moved = 0;
    for (int t = 0; t < ticks; t++) {
        tick_once(pl->x, pl->z);
        int n = count_active();
        if (n != prev_n) { printf("[t=%4d] AKTIV-ZAHL AENDERT SICH: %d -> %d\n", t, prev_n, n); prev_n = n; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            if (a->state != prev_state[s] || a->sub_state_1 != prev_s1[s] ||
                a->grid_id != prev_grid[s]) {
                printf("[t=%4d] slot=%2d st %u->%u  s1 0x%02x->0x%02x  grid 0x%02x->0x%02x  "
                       "mo=%d af=0x%04x m0b=%u pos=(%ld,%ld)\n",
                       t, s, prev_state[s], a->state, prev_s1[s], a->sub_state_1,
                       prev_grid[s], a->grid_id, (int)a->motion, a->anim_flags,
                       a->member_0b, (long)a->x, (long)a->z);
                prev_state[s] = a->state; prev_s1[s] = a->sub_state_1; prev_grid[s] = a->grid_id;
                ai_moved = 1;
            }
        }
        if (t == 60 || t == 300 || t == ticks - 1) {
            char tag[48]; snprintf(tag, sizeof tag, "t=%d", t);
            dump_roster(tag);
        }
    }
    printf("[setup-check] AI hat State-Transitionen erzeugt = %d (0 = Aufbau tickt NICHT!)\n",
           ai_moved);

    /* Wie viele Gegner sind im grossen Raum-AOT 6 (x:[-27300..3300] z:[-24500..-700])? */
    {
        int in6 = 0, in5 = 0, in4 = 0, staging = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            if (a->x >= -27300 && a->x <= 3300 && a->z >= -24500 && a->z <= -700) in6++;
            if (a->x >= -12900 && a->x <= -3700 && a->z >= -25300 && a->z <= -24200) in5++;
            if (a->x >= -14800 && a->x <= -2800 && a->z >= -22500 && a->z <= -20300) in4++;
            if (a->z <= -25400) staging++;
        }
        printf("\n== Geometrie-Bilanz am Ende ==\n"
               "  im Raum-AOT6 = %d   im Tor-Band AOT5 = %d   im AOT4 = %d   hinter dem Tor (z<=-25400) = %d\n",
               in6, in5, in4, staging);
    }

    /* Kriech-Bits: hat IRGENDEIN Aktor je +0x1c4 & 0x1000 / 0x2000 gesehen? */
    {
        int c1000 = 0, c2000 = 0, prone = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            if (a->anim_flags & 0x1000) c1000++;
            if (a->anim_flags & 0x2000) c2000++;
            if (a->grid_id & 0x80)      prone++;
        }
        printf("== Kriech-Bilanz: anim_flags&0x1000 = %d   &0x2000 = %d   grid&0x80 (prone) = %d\n",
               c1000, c2000, prone);
    }

    /* member_0b-Bilanz: stempelt der Port den AOT-Slot ueberhaupt? */
    {
        int stamped = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].member_0b != 0) stamped++;
        printf("== member_0b (Member id 15, +0x0b) != 0 bei %d Gegnern "
               "(Original: JEDEN Frame geklaert + gestempelt @0x8004371c/0x80042f5c)\n", stamped);
    }

    /* Flag-Zone-5-Bilanz: die Stream-Maschine markiert freigegebene Gegner mit Set(5,i,1). */
    {
        printf("== Flags(5,0..19) = ");
        for (int i = 0; i < 20; i++) printf("%d", re15_game_flag_get(5, i));
        printf("   (5,20)=%d (5,32)=%d (5,33)=%d (5,34)=%d (4,15)=%d (3,116)=%d\n",
               re15_game_flag_get(5, 20), re15_game_flag_get(5, 32),
               re15_game_flag_get(5, 33), re15_game_flag_get(5, 34),
               re15_game_flag_get(4, 15), re15_game_flag_get(3, 116));
    }

    /* SCA-Tor-Zellen NACH dem Lauf: hat Sca_id_set(2,6,0xf7)/(3,6,0xf7) gegriffen?
     * ORIGINAL: u0 0xff -> 0xf7 loescht NUR Bit 0x08. Aufrechter Zombie hat Maske
     * +0x1d7 = 4 (@0x801050b4) -> 4 & 0xf7 = 4 = WEITER SOLIDE; kriechender Zombie hat
     * Maske 8 (@0x801050f4 / @0x8010374c) -> 8 & 0xf7 = 0 = KOMMT DURCH. Spieler-Maske 1
     * -> 1 & 0xf7 = 1 = bleibt geblockt. (Test @0x8003b254 `and v1,mask,u0`.) */
    {
        int base = 0;
        for (int g2 = 0; g2 < 5; g2++) {
            if (rdt.sca_rgn[g2] > 6) {
                const re15_sca_entry_t *c = &rdt.sca[base + 6];
                printf("== SCA-Tor rgn=%d idx=6 (flat %d): u0=0x%02x  -> solide fuer Maske4=%d "
                       "Maske8=%d Maske1(Spieler)=%d\n",
                       g2, base + 6, c->u0, (4 & c->u0) != 0, (8 & c->u0) != 0, (1 & c->u0) != 0);
            }
            base += rdt.sca_rgn[g2];
        }
    }

    /* work_vars, die die Stream-Maschine benutzt (var4/5/6/7/0x12). */
    printf("== work_vars: v4=%d v5=%d v6=%d v7=%d v18=%d\n",
           (int)g_scd.work_vars[4], (int)g_scd.work_vars[5], (int)g_scd.work_vars[6],
           (int)g_scd.work_vars[7], (int)g_scd.work_vars[18]);

    free(dat);
    return 0;
}
