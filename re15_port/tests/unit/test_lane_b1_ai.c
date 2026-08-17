/**
 * @file test_lane_b1_ai.c
 * @brief Batch B1 (Gegner-/NPC-KI-Cluster) — Regressions-Pins fuer die 5 Nutzer-Reports.
 *
 * Alle Erwartungen sind gegen SELBST disassemblierte Original-Bytes gepinnt (Adressen im
 * jeweiligen Abschnitt). Die Sonden laden IMMER die EMD-Baenke — ohne sie liefert
 * re15_actor_clip_len()==0 und jedes Playout ist "instant" (dokumentierte Sonden-Falle).
 *
 * T1  ALIGN-TICK: der Phase-1-Body laeuft NOCH auf dem Align-Tick (Sub 4 @0x80051230 ->
 *     Body @0x80051234, Exit `j 0x80051470` @0x800512cc; Sub 5 @0x8005156c -> @0x800515a0,
 *     Exit @0x80051638). Messbar: Slew 96 (Tabelle 0x80076c41 @0x80051260/@0x800515cc) und
 *     KEINE Translation auf dem Umschalt-Tick; Phase 2 (Slew 48 @0x80076c01 + pos_advance
 *     @0x80051344) erst im Folgetick ueber den Dispatch @0x80051184-88.
 * T2  LOOP-IDLE: FUN_80101d08 ruft f314 unbedingt @0x80101da8, die Advance-Haelfte wrappt
 *     @0x8001f63c (`sb zero,149`) -> Clip 0 LOOPT. Kein fr-Pin ueber 840 Ticks.
 * T3  RE2-SFX-GATE: `jal 0x801016c8` existiert in EMOVL10_S0.BIN an GENAU zwei Stellen —
 *     @0x80101d34 (EXEC[1] Walk) und @0x80102454 (EXEC[2] Bump). Gemessene Abdeckung: der
 *     WALK-Positiv-Pin (EXEC[1]) und das KNOCKDOWN-Fenster (EXEC[5]) — mehr erreicht dieses
 *     Fenster nicht, der Knockdown committet direkt 0x101 (`sw 0x101` @0x801036F4).
 * T3b LIEGEN/AUFSTEHEN: derselbe Gate-Nachweis fuer EXEC[7] (Liegen) und EXEC[9] (Get-up,
 *     0x901 @0x80103E48) — das ist der WOERTLICH gemeldete Teil ("Sound beim Aufstehen"),
 *     den T3 nachweislich NICHT durchlaeuft. Eigener Abdeckungs-Waechter.
 * T4  ROOM1100 RE2 sel-7: das RE1.5-Posenpaket behandelt {4,7,9} (Clip 0x12 FINAL
 *     @0x80100dc0-e8) und {5,8,0xa} (0x13 @0x80100e04-e2c) als die zwei LIEGE-Familien,
 *     beide unter dem 0x80-Gate @0x80100ca4-b0 -> beh 0x87 MUSS liegen.
 * T5  ROOM1100 RE2 Skript-Wecker: Member_set(12,0x89/0x8A) ist `sb a2,9(a0)` @0x800411f8;
 *     RE1.5 haengt daran den Sofort-Wecker (Decide @0x8011f9dc[0]=0x801039fc), der RE2-Brain
 *     bekommt die Flavor-Abbildung auf EXEC[9] Get-up (0x901 @0x80103E48).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"     /* re15_actors_anim_advance */
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re2_ems.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

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

/* Eine EM-Bank aus CDEMD0.EMS in die Registry (Muster probe_zreentry_10d0/probe_marvin_10d0):
 * ALLE vier Kanal-Baenke, sonst messen clip_len/channel_anim die falsche Laenge. */
static uint8_t s_embuf[4][0x80000];
static int     s_embuf_used = 0;
static void load_em_bank(const char *base, uint8_t type)
{
    if (s_embuf_used >= 4) return;
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "WARN: CDEMD0.EMS nicht lesbar\n"); return; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
        len <= sizeof s_embuf[0]) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            uint8_t *b = s_embuf[s_embuf_used++];
            memcpy(b, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(b, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(b, len,
                                    &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(b, len,
                                    &eb->skel_victim, &eb->anim_victim) == 0);
                eb->own_ok    = (re15_emd_parse_own_bank(b, len,
                                    &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}
static void reset_bank_pool(void) { re15_enemy_reset(); s_embuf_used = 0; }

/* Die ECHTE RE2-Bank (Welle-A-Splitter, Muster test_re2_room1190_ab.c): unter RE2-Flavor MUESSEN
 * Clip-Laengen UND Frame-Woerter aus shared_assets/RE2/CDEMD0.EMS kommen — die RE1.5-EM01x-Woerter
 * tragen die RE2-SFX-Bits (0x08000000, @0x801016e4) gar nicht, und eine Messung ohne sie kann den
 * Phantom-SE-Defekt nicht sehen (Sonden-Falle der Recherche-Lane). */
static void load_re2_bank(const char *base, uint8_t type)
{
    static uint8_t *ems = NULL; static size_t ems_sz = 0;
    if (!ems) {
        char p[600];
        snprintf(p, sizeof p, "%s/../RE2/CDEMD0.EMS", base);
        ems = read_file(p, &ems_sz);
    }
    if (!ems) { fprintf(stderr, "WARN: shared_assets/RE2/CDEMD0.EMS nicht lesbar\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(ems, ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}

/* Virtueller Pad-Zustand fuer den Dialog-Confirm (siehe t1_align_tick). Der Port fuehrt in
 * g_scd_pad_edge/held bereits die VIRTUELLEN Worte (re15_pad_virtual_word, game_step_common.c),
 * Confirm = Bit 0x4000 (@0x80028570 Edge DAT_800ac76c / @0x80028214 Held DAT_800ac768;
 * physisch SQUARE ueber Preset 0 @0x80073dbc[14]). */
static uint16_t s_pad_edge = 0, s_pad_held = 0;

/* Ein Spielframe in main.c-Reihenfolge.
 *
 * Der Message-FSM-Pump laeuft im Original TOP-LEVEL und liest g_pauseflags NIRGENDS:
 *   FUN_800280b4  800280c8 lbu v0,DAT_800b8520 | 800280d0 andi v0,v0,0x80 |
 *                 800280dc jal 0x80028134
 * Aufrufer @0x80010044 (in FUN_80010000, dessen einzige Eintrittsbedingung der Frame-Flip-
 * Vergleich @0x80010028 `beq v1,v0` DAT_800a73a4/DAT_800aca34 ist — ein Per-Frame-Gate, KEIN
 * Pause-Gate). Ohne diesen Pump kann eine Message NIE quittiert werden und der von ihr gesetzte
 * Freeze (Message_on -> FUN_80027e68 @0x80027ed0 `g_pauseflags |= maske`) haelt fuer immer —
 * inklusive des SCD-Gates 0x02000000 (FUN_8003f038 @0x8003f040-4c). Genau das liess die
 * ROOM10D0-Sonde (T1) 600 Ticks lang stehen: sub21 beginnt mit
 * `2b 0b 80 ff` = Message_on id=11 mask=0xff80 (ROOM10D0.RDT Datei-Offset 0x19DE).
 * main.c ruft re15_msg_tick ebenfalls VOR dem Step (platform/pc/main.c). */
static void frame(void)
{
    extern uint16_t g_scd_pad_edge;
    extern uint16_t g_scd_pad_held;
    g_scd_pad_edge = s_pad_edge;
    g_scd_pad_held = s_pad_held;
    re15_msg_tick(0, 0, 0);
    re15_enemy_ai_run_all(1);
    re15_actors_anim_advance();
    g_scd_pad_edge = 0;    /* Edge ist ein Ein-Frame-Ereignis (`(prev^cur)&cur`) */
}

/* ===================================================================================== */
/* T1 — ROOM10D0 Marvin: Phase-1-Body auf dem Align-Tick (Report 1b / Report 5 D5-1)      */
/* ===================================================================================== */
static void t1_align_tick(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { CHECK(0, "T1: %s nicht lesbar", path); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { CHECK(0, "T1: RDT-Parse"); free(data); return; }

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    reset_bank_pool();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_em_bank(base, 0x40);
    /* Der .msg-Block des Raums MUSS geladen sein: ohne ihn liefert re15_msg_get_raw NULL und
     * re15_dialog_step verlaesst ueber den Port-Sonderpfad (msg_common.c, `!raw || rlen <= 0`)
     * schon im ersten Pump-Tick — der Text laeuft dann NIE wirklich durch, und jeder daraus
     * abgeleitete absolute Tick-Wert waere harness-relativ statt spielnah. Mit Block +
     * echtem Confirm-Edge (unten) durchlaeuft msg 11 seine Typewriter-FSM wie im Spiel. */
    if (rdt.messages && rdt.messages_size > 0)
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
    if (rdt.animation && rdt.animation_size > 0)
        re15_rbj_bind_room(rdt.animation, rdt.animation_size);
    for (int f = 0; f < 8; f++) scd_vm_tick();

    /* Marvin = Slot 1 (Sce_em_set slot=0 -> Aktor-Slot 1), Cutscene sub21 anstossen */
    re15_actor_t *mv = &g_actors[1];
    CHECK(mv->active && mv->type == 0x40, "T1: Marvin (0x40) in Slot 1 erwartet (type=0x%02X)",
          mv->type);
    {   int rl = 0; const unsigned char *r11 = re15_msg_get_raw(11, &rl);
        CHECK(r11 && rl > 0, "T1: msg 11 (Message_on des sub21) muss aus dem RDT-Block kommen"); }
    scd_event_fire(0x15);

    int align_tick = -1;
    int16_t rot_prev = mv->rot_y;
    int32_t x_prev = mv->x, z_prev = mv->z;
    int     align_rot_delta = 0, align_moved = 0, next_moved = 0;
    int     sub4_ticks = 0, msg_ticks = 0, press = 0;
    for (int t = 0; t < 1200; t++) {
        uint8_t s2_before = mv->sub_state_2;
        uint8_t s1_before = mv->sub_state_1;
        rot_prev = mv->rot_y; x_prev = mv->x; z_prev = mv->z;
        /* Confirm wie ein echter Tastendruck: Druck-Frame, dann Loslass-Frame. Ein Dauer-Edge
         * waere unmoeglich (`(prev^cur)&cur`), und g_scd_pad_held bleibt 0, damit der
         * Typewriter NICHT im Fast-Forward laeuft (act_held @0x80028214). */
        if (g_scd.message_active) { msg_ticks++; press = !press; s_pad_edge = press ? 0x4000u : 0u; }
        else                      { press = 0; s_pad_edge = 0; }
        scd_vm_tick();
        frame();
        if (mv->state == 4 && mv->sub_state_1 == 4) sub4_ticks++;
        if (align_tick < 0 && s1_before == 4 && s2_before == 1 && mv->sub_state_2 == 2) {
            align_tick = t;
            int d = (int)mv->rot_y - (int)rot_prev;
            while (d < -2048) d += 4096;
            while (d >  2048) d -= 4096;
            align_rot_delta = d < 0 ? -d : d;
            align_moved = (mv->x != x_prev || mv->z != z_prev);
        } else if (align_tick >= 0 && t == align_tick + 1) {
            next_moved = (mv->x != x_prev || mv->z != z_prev);
        }
    }
    s_pad_edge = 0;
    printf("  [T1] msg_ticks=%d  sub4_ticks=%d  align_tick=%d  |rot-delta|=%d  "
           "moved_on_align=%d  moved_next=%d\n",
           msg_ticks, sub4_ticks, align_tick, align_rot_delta, align_moved, next_moved);
    /* ⛔ ABDECKUNGS-WAECHTER ZUERST (Review-Fund test-validity, 2026-08-17): vorher blieb
     * align_tick auf -1 stehen, `align_moved` behielt seinen Init-Wert 0, und der Pin
     * "darf NICHT translatieren" bestand ALLEIN deshalb, weil der Zustand nie erreicht wurde
     * (vacuous pass — mit zurueckgebautem Fix byte-identische Ausgabe). Deshalb: harte
     * Erreichbarkeits-Pins, und KEINE Detail-Pins auf Muellwerten. */
    CHECK(msg_ticks > 0, "T1: sub21 startet mit Message_on id=11 mask=0xff80 (ROOM10D0.RDT "
                         "@0x19DE) — der Dialog MUSS im Fenster offen gewesen sein");
    CHECK(sub4_ticks > 0, "T1: Marvin muss den WALK-Sub 4 durchlaufen (Plc_dest mode=4 -> "
                          "`sb 4,0x4(a1); sb mode,0x5(a1)` @0x80041c14-18)");
    CHECK(align_tick >= 0, "T1: der Walk-Sub 4 muss im sub21-Fenster ausrichten (Phase 1 -> 2)");
    if (align_tick < 0) { free(data); return; }   /* keine Folge-Pins auf Init-Werten */
    /* Slew der CONE-Tabelle 0x80076c41[(0x40-0x40)*2] = 0x60 = 96 (@0x80051258-64) —
     * NICHT die Phase-2-Tabelle 0x80076c01 (=48, @0x8005132c-3c). */
    CHECK(align_rot_delta == 96,
          "T1: Align-Tick muss mit CONE-Slew 96 drehen (@0x80051260), gemessen %d",
          align_rot_delta);
    /* `j 0x80051470` @0x800512cc = Exit VOR pos_advance @0x80051344. */
    CHECK(!align_moved, "T1: Align-Tick darf NICHT translatieren (Exit @0x800512cc)");
    CHECK(next_moved, "T1: die erste Translation kommt im FOLGE-Tick (Dispatch @0x80051188)");
    free(data);
}

/* ===================================================================================== */
/* T2 — ROOM1140 Re-Entry: Idle-Clip 0 LOOPT (Report 12)                                 */
/* ===================================================================================== */
static void t2_loop_idle(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { CHECK(0, "T2: %s nicht lesbar", path); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { CHECK(0, "T2: RDT-Parse"); free(data); return; }

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    reset_bank_pool();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    /* Set(3,210) = das Tableau-Flag aus sub04 (RDT-Datei 0xd52) -> sub00 nimmt den ELSE-Zweig
     * mit den 5 stehenden Schlaefern (behavior 0x02). */
    re15_game_flag_set(3, 210, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_em_bank(base, 0x10);
    load_em_bank(base, 0x11);
    load_em_bank(base, 0x16);
    for (int f = 0; f < 120; f++) scd_vm_tick();

    int zs[RE15_ACTOR_MAX], nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        uint8_t t = g_actors[s].type;
        if (g_actors[s].active && (t == 0x10 || t == 0x11 || t == 0x16)) zs[nz++] = s;
    }
    CHECK(nz == 5, "T2: 5 Re-Entry-Schlaefer erwartet, %d gespawnt", nz);
    for (int i = 0; i < nz; i++)
        CHECK((g_actors[zs[i]].grid_id & 0x0f) == 2,
              "T2: ELSE-Zweig => behavior 0x02 (slot %d grid=0x%02X)", zs[i],
              g_actors[zs[i]].grid_id);

    /* Spieler weit weg = FERN (kein Wake): 840 Ticks. */
    pl->x = 30000; pl->z = 30000;
    int seen[RE15_ACTOR_MAX][64], tail[RE15_ACTOR_MAX][64];
    memset(seen, 0, sizeof seen);
    memset(tail, 0, sizeof tail);
    int wrapped[RE15_ACTOR_MAX];
    memset(wrapped, 0, sizeof wrapped);
    uint16_t prev[RE15_ACTOR_MAX];
    for (int i = 0; i < nz; i++) prev[zs[i]] = g_actors[zs[i]].anim_frame;
    for (int t = 0; t < 840; t++) {
        frame();
        for (int i = 0; i < nz; i++) {
            re15_actor_t *e = &g_actors[zs[i]];
            if (e->motion == 0 && e->anim_frame < 64) {
                seen[zs[i]][e->anim_frame] = 1;
                if (t >= 740) tail[zs[i]][e->anim_frame] = 1;   /* die LETZTEN 100 Ticks */
            }
            if (e->motion == 0 && e->anim_frame == 0 && prev[zs[i]] > 0) wrapped[zs[i]] = 1;
            prev[zs[i]] = e->anim_frame;
        }
    }
    for (int i = 0; i < nz; i++) {
        int distinct = 0, distinct_tail = 0;
        for (int f = 0; f < 64; f++) { if (seen[zs[i]][f]) distinct++;
                                       if (tail[zs[i]][f]) distinct_tail++; }
        printf("  [T2] slot %d: distinct idle frames=%d (letzte 100 Ticks: %d)  wrapped=%d  "
               "end fr=%d s1=%d s2=%d\n",
               zs[i], distinct, distinct_tail, wrapped[zs[i]], (int)g_actors[zs[i]].anim_frame,
               g_actors[zs[i]].sub_state_1, g_actors[zs[i]].sub_state_2);
        /* Clip 0 der EM01x-Loco-Bank ist 14 Frames; der Wrap @0x8001f63c fuehrt sie alle vor. */
        CHECK(distinct >= 10,
              "T2: Idle-Clip 0 muss LOOPEN (f314 jeden Tick @0x80101da8, Wrap @0x8001f63c) — "
              "slot %d sah nur %d verschiedene Frames (Statue-Pin)", zs[i], distinct);
        CHECK(wrapped[zs[i]],
              "T2: slot %d muss mindestens einmal auf Frame 0 wrappen (`sb zero,149` @0x8001f63c)",
              zs[i]);
        /* DAS ist der eigentliche Statue-Pin: der alte Port lief den Clip EINMAL durch und
         * pinnte danach fr=13 — hier waeren es dann 1 Frame in den letzten 100 Ticks. */
        CHECK(distinct_tail >= 10,
              "T2: slot %d ist nach 740 Ticks eingefroren (nur %d Frame(s) in den letzten "
              "100 Ticks) — Statue statt Atem-Idle", zs[i], distinct_tail);
    }
    free(data);
}

/* ===================================================================================== */
/* T3 — RE2-Flavor: Frame-Wort-SFX nur in EXEC[1]/EXEC[2] (Report 13)                     */
/* ===================================================================================== */
static int  s_se_n;
static int  s_se_ids[512];
static void t3_se_cap(int se_id, int flag2000) { (void)flag2000;
    if (s_se_n < 512) s_se_ids[s_se_n] = se_id; s_se_n++; }
static void t3_bank_cap(int bank) { (void)bank; }

static void t3_sfx_gate(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { CHECK(0, "T3: %s nicht lesbar", path); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { CHECK(0, "T3: RDT-Parse"); free(data); return; }

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    reset_bank_pool();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_re2_bank(base, 0x10);
    load_re2_bank(base, 0x11);
    load_re2_bank(base, 0x16);
    for (int f = 0; f < 120; f++) scd_vm_tick();
    /* Der RE2-INIT ist der ZUSTAND-0-TICK der Entity (Zustandstabelle @0x8010C830,
     * Handler @0x8010065C), NICHT der Sce_em_set-Spawn: er laeuft erst im ersten
     * Entity-Tick. Gemessen: nach 120x scd_vm_tick steht state=0 sub1=0 +0x21A=0x0000,
     * nach EINEM re15_enemy_ai_run_all state=1 sub1=8 +0x21A=0x8000. Ohne diesen Tick
     * liest der Pin unten zwangslaeufig 0 und meldet einen Engine-Defekt, der keiner ist. */
    re15_enemy_ai_run_all(1);

    /* ⛔ TYP 0x11 (nicht "der erste beste"): das RE2-Sub-Gate vor beiden Call-Sites verlangt
     * `+0x10E & 0x80` ODER `+0x21A & 0x8000` (@0x80101ce0-f8 / @0x80102400-18). Das einzige
     * PORTIERTE Bit davon ist +0x21A|=0x8000 aus dem INIT fuer Typ 0x11 (`lbu v1,8(s2)` /
     * `bne v1,17` @0x80100894/@0x801008BC -> `ori 0x8000` @0x801008C4-D4); +0x10E&0x80 kommt
     * nur aus dem nicht portierten Gore-Zweig (0x80106128). Ein 0x10er kann also gar keinen
     * Frame-Wort-SE spielen — mit ihm waere der Positiv-Pin unten unerfuellbar und der
     * Soll-0-Pin in (b) trivial gruen. */
    int zslot = -1, zslot_any = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        uint8_t t = g_actors[s].type;
        if (!g_actors[s].active || !(t == 0x10 || t == 0x11 || t == 0x16)) continue;
        if (zslot_any < 0) zslot_any = s;
        if (t == 0x11 && zslot < 0) zslot = s;
    }
    if (zslot < 0) zslot = zslot_any;
    CHECK(zslot > 0, "T3: kein RE2-Zombie gespawnt");
    if (zslot < 0) { free(data); return; }
    re15_actor_t *e = &g_actors[zslot];
    printf("  [T3] Messobjekt slot %d type=0x%02X  +0x21A=0x%04X +0x10E=0x%04X\n",
           zslot, e->type, (unsigned)e->re2z_flags21a, (unsigned)e->re2z_f10e);
    CHECK(e->type == 0x11, "T3: ROOM1140 muss einen Typ-0x11-Zombie liefern (der einzige mit "
                           "portiertem Sub-Gate-Bit +0x21A|0x8000 @0x801008C4-D4); type=0x%02X",
          e->type);
    /* Sub-Gate-Bit aus dem RE2-INIT, selbst nachdisassembliert (EMOVL10_S0.BIN, RAW @0x80100000):
     *   80100894: lbu v1,8(s2)              ; Entity+0x8 = Typ
     *   801008a0: addiu v0,zero,17          ; 0x11
     *   801008bc: bne v1,v0,0x801008d8      ; Typ != 0x11 -> ueberspringen
     *   801008c4: lhu v0,538(s2)            ; +0x21A
     *   801008d0: ori v0,v0,0x8000
     *   801008d4: sh  v0,538(s2)            ; +0x21A |= 0x8000
     * Portiert (re2z_init). Das benachbarte `sh v1,342(s2)` (+0x156 = 250, @0x801008C8-CC)
     * ist BEWUSST nicht portiert: +0x156 ist das HP-Halbwort und 250 waere eine einzelne
     * Zeile des RE2-HP-Modells, das der Port durch die RE1.5-HP-Zeile @0x8011f034 ersetzt
     * (Typ 0x11 haette sonst 250 statt 71 HP). Begruendung steht in enemy_ai_re2_zombie.c.
     * Das zweite Gate-Bit +0x10E&0x80 stammt allein aus dem nicht portierten Gore-Zweig
     * (`ori 0x80` @0x80106144) — deshalb spielen unter RE2-Flavor derzeit nur Typ-0x11-
     * Zombies Frame-Wort-SEs; im Original ebenso, solange sie nicht zerstueckelt wurden. */
    CHECK((e->re2z_flags21a & 0x8000u) != 0,
          "T3: das Sub-Gate-Bit +0x21A|0x8000 muss nach dem ersten Entity-Tick stehen "
          "(@0x801008C4-D4) — ohne es kann kein einziger RE2-Frame-Wort-SE feuern");
    re15_re2z_audio_hook(t3_se_cap, t3_bank_cap);

    /* (a) WALK: EXEC[1] darf Schritt-SEs spielen (@0x80101d34).
     * ⛔ Der Zaehler haengt am GEMEINSAMEN re2z-SE-Sink, es landen also auch die
     * Executor-eigenen SEs (Walk-Moan 10/11 @0x80101C7C-88) darin. Frame-Wort-SEs sind
     * laut EM010-Dump ausschliesslich die IDs 0 und 1 — nur die duerfen gezaehlt werden. */
    pl->x = 30000; pl->z = 30000;
    re15_ai_set_state_word(e, 0x101);
    e->motion = 0; e->anim_frame = 0;
    s_se_n = 0;
    for (int t = 0; t < 300; t++) frame();
    int walk_fw = 0, walk_exec = 0;
    for (int i = 0; i < s_se_n && i < 512; i++) { if (s_se_ids[i] < 2) walk_fw++; else walk_exec++; }
    printf("  [T3] WALK-Fenster: %d SEs gesamt = %d Frame-Wort (ID 0/1) + %d Executor\n",
           s_se_n, walk_fw, walk_exec);
    /* POSITIV-Pin (die Gegenrichtung zu (b)): der Frame-Wort-Pfad MUSS in EXEC[1] ueberhaupt
     * feuern. Ohne ihn waere ein Fix, der die RE2-Schritt-SEs komplett stummschaltet, von
     * (b) allein NICHT unterscheidbar (bad==0 gilt dann trivial). */
    CHECK(walk_fw > 0,
          "T3: EXEC[1] MUSS Frame-Wort-SEs spielen (`jal 0x801016c8` @0x80101d34) — gemessen 0");
    /* ⛔ SE-ZAHL-PIN (Regressionsschutz fuer Report 13, Haelfte 2) — EHRLICHE REICHWEITE:
     * Das Original gated beide Call-Sites zusaetzlich auf einen DRITTEL-TAKT ueber +0x14D (=333),
     * selbst nachdisassembliert (EMOVL10_S0.BIN):
     *   80101d00 lbu v1,333(s1) | 80101d04-d20 lui 0xaaaa/ori 0xaaab/multu/mfhi/srl 1/sll/addu/subu
     *                             (= Magic-Division v1 % 3) | 80101d28 addiu v0,zero,2
     *   80101d2c bne v1,v0,0x80101d5c   -> nur Rest 2 erreicht 80101d34 jal 0x801016c8
     *   identisch fuer EXEC[2]: @0x80102420 lbu v1,333(s0), bne @0x8010244c, jal @0x80102454
     * Der Port bildet +0x14D auf den Frame-Slot ab (anim_frame %% clip_len). GEMESSEN (Mutations-
     * Gegentest 2026-08-17: Takt-Zeile entfernt -> unveraenderte 9): der Takt ist auf DIESEM Clip
     * WIRKUNGSLOS, weil die einzigen SE-tragenden Frames des EM010-Walk-Clips 20 und 62 sind und
     * beide %%3==2 erfuellen. Dieser Pin deckt den Takt also NICHT ab — er haelt die SE-ZAHL fest
     * (deterministisch via re15_damage_seed_rng(0x0badf00d)) und faengt Regressionen, die den
     * Pfad verstummen lassen (Gegentest: Emitter stumm -> 0) oder ihn oefter feuern lassen. */
    CHECK(walk_fw == 9,
          "T3: SE-Zahl-Pin — im 300-Frame-Walk-Fenster feuern genau 9 Frame-Wort-SEs "
          "(2 je Clip-Durchlauf, SE-Frames 20/62); gemessen %d", walk_fw);

    /* (b) KNOCKDOWN (EXEC[5]): KEIN Frame-Wort-SE. Die Executor-eigenen SEs
     * (Fall-Commit 12/13 @0x8010333C-58, Lie-Moan 11 @0x801035F0-610, Get-up 12
     * @0x80103F94-A8) sind erlaubt — die Frame-Wort-IDs sind laut EM010-Dump 0 und 1.
     * ⛔ EHRLICHE ABDECKUNG (Review-Fund test-validity 2026-08-17): dieses Fenster durchlaeuft
     * NUR EXEC[5] — der Knockdown committet direkt 0x101 (`sw 0x101` @0x801036F4), EXEC[7]
     * (Liegen) und EXEC[9] (Get-up) sind von hier aus unerreichbar. Frueher behaupteten
     * Kopf-/Abschnitts-/Assertion-Text "Knockdown/Lying/Get-up"; das Liege-/Aufsteh-Fenster
     * steht jetzt getrennt in t3b_getup_sfx (ROOM1100), mit eigenem Abdeckungs-Waechter. */
    re15_ai_set_state_word(e, 0x501);            /* EXEC[5] Knockdown-Fall */
    e->motion = 1; e->anim_frame = 0;
    s_se_n = 0;
    int bad = 0, kd_frames = 0, kd_in5 = 0;
    for (int t = 0; t < 400; t++) {
        frame();
        /* Auswertung VOR dem break (sonst gingen die SEs des Abbruch-Frames verloren). */
        for (int i = 0; i < s_se_n && i < 512; i++)
            if (s_se_ids[i] == 0 || s_se_ids[i] == 1) bad++;
        s_se_n = 0;
        kd_frames++;
        if (e->state == 1 && e->sub_state_1 == 5) kd_in5++;
        if (e->state == 1 && (e->sub_state_1 == 1 || e->sub_state_1 == 2)) break;  /* wieder Walk */
    }
    printf("  [T3] Knockdown-Fenster (EXEC[5]): %d Frames, davon %d in EXEC[5]; "
           "%d Frame-Wort-SEs (0/1) — Soll 0\n", kd_frames, kd_in5, bad);
    CHECK(kd_in5 > 20, "T3: das Knockdown-Fenster muss EXEC[5] wirklich durchlaufen "
                       "(Abdeckungs-Waechter), gemessen %d Frames", kd_in5);
    CHECK(bad == 0,
          "T3: KEIN Frame-Wort-SE in EXEC[5] — jal-Scan EMOVL10_S0.BIN hat "
          "genau 2 Call-Sites (@0x80101d34, @0x80102454); gemessen %d", bad);
    re15_re2z_audio_hook(NULL, NULL);
    free(data);
}

/* ===================================================================================== */
/* T3b — RE2-Flavor: KEIN Frame-Wort-SE im LIEGEN (EXEC[7]) und beim AUFSTEHEN (EXEC[9])  */
/*       Das ist der WÖRTLICH gemeldete Teil von Report 13 ("Sound beim Aufstehen"), den   */
/*       T3 nachweislich NICHT erreicht (dort nur EXEC[5]).                                */
/*       DISKRIMINIERUNG nachgewiesen (Mutations-Gegentest 2026-08-17): mit entferntem      */
/*       Call-Site-Gate UND entferntem Drittel-Takt feuern im Get-up-Fenster 2 Frame-Wort-  */
/*       SEs und dieser Pin wird rot; das Gate allein zu entfernen reicht auf diesem Clip   */
/*       nicht (der Takt streicht die betroffenen Frames) — beide Schutzlagen sind noetig.  */
/* ===================================================================================== */
static void t3b_getup_sfx(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1100.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { CHECK(0, "T3b: %s nicht lesbar", path); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { CHECK(0, "T3b: RDT-Parse"); free(data); return; }

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    reset_bank_pool();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1100;
    re15_game_flag_set(4, 232, 0);                 /* Ck(4,232)==0 -> IF-Zweig (Liege-Spawns) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -20975; pl->y = 0; pl->z = -25000;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_re2_bank(base, 0x10); load_re2_bank(base, 0x11); load_re2_bank(base, 0x16);
    for (int f = 0; f < 120; f++) scd_vm_tick();

    pl->x = 30000; pl->z = 30000;                  /* fern: kein Naehe-Wake */
    for (int f = 0; f < 20; f++) frame();

    /* ⛔ ISOLATION: der SE-Hook ist ein GLOBALER Sink ohne Aktor-Kennung. Mit mehreren aktiven
     * Zombies laesst sich ein Frame-Wort-SE nicht dem Liegenden zuordnen — ein laufender
     * Nachbar (EXEC[1]) feuert voellig legitim. Deshalb bleibt GENAU EIN Liegender aktiv. */
    int zslot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        uint8_t t = g_actors[s].type;
        if (!g_actors[s].active || !(t == 0x10 || t == 0x11 || t == 0x16)) continue;
        if (zslot < 0 && g_actors[s].state == 1 && g_actors[s].sub_state_1 == 7) zslot = s;
        else g_actors[s].active = 0;
    }
    CHECK(zslot > 0, "T3b: ROOM1100 muss unter RE2 mindestens EINEN Liegenden (EXEC[7]) spawnen");
    if (zslot < 0) { free(data); return; }
    re15_actor_t *e = &g_actors[zslot];

    /* ⛔ MAXIMAL DURCHLAESSIGE VORBEDINGUNG, damit dieser Soll-0-Pin nicht vakuum wird:
     * beide Call-Sites stehen hinter `+0x10E&0x80 || +0x21A&0x8000` (@0x80101ce0-f8 /
     * @0x80102400-18). Ist keins der Bits gesetzt, kann der Emitter GAR NICHT feuern und
     * "0 SEs im Liegen" waere trivial wahr. Das Gate ist eine VORBEDINGUNG, kein Produzent —
     * mit gesetztem Bit spielt das Original in EXEC[7]/EXEC[9] trotzdem keinen Frame-Wort-SE
     * (dort steht schlicht kein `jal 0x801016c8`), der Pin kann also nicht falsch-rot werden. */
    e->re2z_flags21a |= 0x8000u;                 /* @0x801008D0-D4 */
    re15_re2z_audio_hook(t3_se_cap, t3_bank_cap);
    int f7 = 0, f9 = 0, bad7 = 0, bad9 = 0;
    /* Fenster A — LIEGEN (EXEC[7]) ohne jeden Reiz. */
    for (int t = 0; t < 120; t++) {
        uint8_t s1_before = (e->state == 1) ? e->sub_state_1 : 0xFF;
        s_se_n = 0;
        frame();
        int fw = 0;
        for (int i = 0; i < s_se_n && i < 512; i++) if (s_se_ids[i] < 2) fw++;
        if (s1_before == 7 && e->state == 1 && e->sub_state_1 == 7) { f7++; bad7 += fw; }
    }
    /* Skript-Wecker wie sub02: Member_set(12, 0x89/0x8A) = `sb a2,9(a0)` @0x800411f8 */
    re15_actor_set_member(zslot, 12, 0x89u);
    /* Fenster B — AUFSTEHEN (EXEC[9]). */
    for (int t = 0; t < 400; t++) {
        uint8_t s1_before = (e->state == 1) ? e->sub_state_1 : 0xFF;
        s_se_n = 0;
        frame();
        int fw = 0;
        for (int i = 0; i < s_se_n && i < 512; i++) if (s_se_ids[i] < 2) fw++;
        if (s1_before == 9 && e->state == 1 && e->sub_state_1 == 9) { f9++; bad9 += fw; }
        else if (s1_before == 7 && e->state == 1 && e->sub_state_1 == 7) { f7++; bad7 += fw; }
    }
    re15_re2z_audio_hook(NULL, NULL);
    printf("  [T3b] slot %d — Abdeckung: %d Frames EXEC[7] (Liegen), %d Frames EXEC[9] (Get-up); "
           "Frame-Wort-SEs: 7er=%d 9er=%d\n", zslot, f7, f9, bad7, bad9);
    /* ⛔ ABDECKUNGS-WAECHTER ZUERST — ohne ihn waere jeder Soll-0-Pin unten ein vacuous pass. */
    CHECK(f7 > 0, "T3b: das Fenster MUSS EXEC[7] (Liegen) durchlaufen — gemessen 0 Frames");
    CHECK(f9 > 0, "T3b: das Fenster MUSS EXEC[9] (Get-up, 0x901 @0x80103E48) durchlaufen — "
                  "gemessen 0 Frames");
    CHECK(bad7 == 0, "T3b: KEIN Frame-Wort-SE im Liegen (EXEC[7]) — die einzigen Call-Sites sind "
                     "@0x80101d34 (EXEC[1]) und @0x80102454 (EXEC[2]); gemessen %d", bad7);
    CHECK(bad9 == 0, "T3b: KEIN Frame-Wort-SE beim AUFSTEHEN (EXEC[9]) — genau der gemeldete "
                     "Phantom-Schritt; gemessen %d", bad9);
    free(data);
}

/* ===================================================================================== */
/* T4/T5 — ROOM1100 unter RE2-Flavor: sel-7 liegt + Skript-Wecker (Report 15)             */
/* ===================================================================================== */
static void t45_room1100(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1100.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { CHECK(0, "T4: %s nicht lesbar", path); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { CHECK(0, "T4: RDT-Parse"); free(data); return; }

    for (int pass = 0; pass < 2; pass++) {         /* pass 0 = RE1.5-Regression, 1 = RE2 */
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        reset_bank_pool();
        re15_ai_flavor_set(pass ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        re15_enemy_ai_set_paused(0);
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1100;
        re15_game_flag_set(4, 232, 0);             /* Ck(4,232)==0 -> IF-Zweig (Liege-Spawns) */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -20975; pl->y = 0; pl->z = -25000;
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        if (pass) { load_re2_bank(base, 0x10); load_re2_bank(base, 0x11); load_re2_bank(base, 0x16); }
        else      { load_em_bank(base, 0x10);  load_em_bank(base, 0x11);  load_em_bank(base, 0x16); }
        for (int f = 0; f < 120; f++) scd_vm_tick();

        int zs[RE15_ACTOR_MAX], nz = 0, n87 = 0, n88 = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            uint8_t t = g_actors[s].type;
            if (g_actors[s].active && (t == 0x10 || t == 0x11 || t == 0x16)) {
                zs[nz++] = s;
                if (g_actors[s].grid_id == 0x87) n87++;
                if (g_actors[s].grid_id == 0x88) n88++;
            }
        }
        printf("  [T%d] ROOM1100 %s: %d Zombies (beh 0x87=%d, 0x88=%d)\n",
               pass ? 4 : 0, pass ? "RE2" : "RE1.5", nz, n87, n88);
        CHECK(nz == 5, "%s: 5 Liege-Spawns erwartet, %d", pass ? "T4" : "T4-Basis", nz);
        CHECK(n87 == 2 && n88 == 3,
              "%s: RDT-Roster 2x beh 0x87 + 3x 0x88 erwartet (0x87=%d 0x88=%d)",
              pass ? "T4" : "T4-Basis", n87, n88);

        pl->x = 30000; pl->z = 30000;               /* fern: kein Naehe-Wake */
        for (int f = 0; f < 20; f++) frame();

        if (!pass) {
            /* RE1.5-REGRESSION: Spawn-Pose 0x12 (beh 0x87) / 0x13 (beh 0x88), byte-true die
             * FINAL-Writes @0x80100de4 / @0x80100e28 (Decoder scd_vm.c re15_enemy_spawn_action). */
            for (int i = 0; i < nz; i++) {
                re15_actor_t *e = &g_actors[zs[i]];
                uint8_t want = (e->grid_id == 0x87) ? 0x12 : 0x13;
                CHECK(e->motion == want,
                      "T4-Basis: RE1.5 beh 0x%02X -> Liege-Clip 0x%02X (FINAL @0x80100dc0-e2c), "
                      "gemessen 0x%02X", e->grid_id, want, e->motion);
            }
            continue;
        }

        /* ---- T4: RE2-Flavor — ALLE 5 liegen (EXEC[7]), KEINER steht ---- */
        int lying = 0, standing = 0;
        for (int i = 0; i < nz; i++) {
            re15_actor_t *e = &g_actors[zs[i]];
            if (e->state == 1 && e->sub_state_1 == 7) lying++;
            else standing++;
        }
        printf("  [T4] RE2 nach Spawn: %d liegend (EXEC[7]) / %d stehend\n", lying, standing);
        CHECK(lying == 5 && standing == 0,
              "T4: alle 5 muessen im RE2-Modus liegen (sel 7 gehoert in die Liege-Familie "
              "{4,7,9} @0x80100dc0-e8), gemessen %d liegend / %d stehend", lying, standing);

        /* ---- T5: Skript-Wecker — sub02s Member_set(12,0x89/0x8A) ---- */
        for (int i = 0; i < nz; i++) {
            uint8_t nib = (i & 1) ? 0x8Au : 0x89u;   /* wie sub02: 7->9 bzw. 8->10 */
            re15_actor_set_member(zs[i], 12, nib);
        }
        int left7[RE15_ACTOR_MAX]; memset(left7, 0, sizeof left7);
        int saw9[RE15_ACTOR_MAX];  memset(saw9,  0, sizeof saw9);
        for (int t = 0; t < 400; t++) {
            frame();
            for (int i = 0; i < nz; i++) {
                re15_actor_t *e = &g_actors[zs[i]];
                if (e->state == 1 && e->sub_state_1 == 9) saw9[zs[i]] = 1;
                if (!(e->state == 1 && e->sub_state_1 == 7)) left7[zs[i]] = 1;
            }
        }
        for (int i = 0; i < nz; i++) {
            printf("  [T5] slot %d: verliess EXEC[7]=%d ueber EXEC[9]=%d, jetzt s1=%d\n",
                   zs[i], left7[zs[i]], saw9[zs[i]], g_actors[zs[i]].sub_state_1);
            CHECK(left7[zs[i]],
                  "T5: der Nibble-Bump 9/10 (`sb a2,9(a0)` @0x800411f8) MUSS den Liegenden "
                  "wecken — slot %d blieb in EXEC[7]", zs[i]);
            CHECK(saw9[zs[i]],
                  "T5: der Weg raus ist EXEC[9] Get-up (0x901 @0x80103E48) — slot %d nie in s1=9",
                  zs[i]);
        }
    }
    free(data);
}

/* ⛔ EIN ctest-EINTRAG PRO PIN (Review-Fund test-validity 2026-08-17, CMakeLists.txt:1084):
 * vorher lief das ganze Binary unter EINEM add_test und aggregierte alles in `fails` -> ein
 * einziger roter Pin (T1) machte das ctest-Verdikt fuer T2/T3/T4/T5 bedeutungslos; die Pins
 * fuer drei Nutzer-Reports konnten unbemerkt rot werden (und WAREN es: T4/T5). Mit dem
 * argv-Selektor bekommt jeder Pin sein eigenes CI-Signal. Ohne Argument laeuft alles. */
int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    const char *sel  = (argc > 1) ? argv[1] : "all";
    int all = (strcmp(sel, "all") == 0);
    printf("== Batch B1 — Gegner-/NPC-KI-Regressionen [%s] ==\n", sel);
    if (all || !strcmp(sel, "t1"))  t1_align_tick(base);
    if (all || !strcmp(sel, "t2"))  t2_loop_idle(base);
    if (all || !strcmp(sel, "t3"))  t3_sfx_gate(base);
    if (all || !strcmp(sel, "t3b")) t3b_getup_sfx(base);
    if (all || !strcmp(sel, "t45")) t45_room1100(base);
    if (!all && strcmp(sel, "t1") && strcmp(sel, "t2") && strcmp(sel, "t3") &&
        strcmp(sel, "t3b") && strcmp(sel, "t45")) {
        fprintf(stderr, "FAIL: unbekannter Pin-Selektor '%s' (t1|t2|t3|t3b|t45|all)\n", sel);
        return 1;
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    if (fails) { fprintf(stderr, "\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }
    printf("\nLANE-B1 AI: all checks passed\n");
    return 0;
}
