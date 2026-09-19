/* probe_r16_aufstehen_schuss.c — MESSUNG (Runde 16, 2026-09-19), keine Behauptung.
 *
 * NUTZER: "Wenn die Zombies im Prozess sind wieder aufzustehen, laesst man sie mit Schuss
 *          wieder hin fallen, und dann probieren sie wieder aufzustehen usw. Das ist im
 *          Original Resident Evil 2 anders - dort reagieren sie zwar auf Schuesse, stehen
 *          aber weiter auf."
 *
 * Gemessen wird am ECHTEN Pfad (ROOM1140.RDT, RE2-Flavor = Nutzer-Default, geladene RE2-Bank
 * EM010/EM011, Pistolenschuss ueber das Pad R1/SQUARE durch re15_game_step):
 *   A) Ein aufrechter Zombie wird per Zustandswort 0x501 (= der Flinch-Commit @0x801050A4)
 *      in die Sturzkette EXEC[5] geschickt; PRO BILD: Zustandswort, Clip, Bildnummer,
 *      +0x21A, +0x1D3, +0x223, +0x222, +0x16B, HP, y. Bis er in P7 (Boden-Aufsteher Clip 8/9)
 *      angekommen ist.
 *   B) WAEHREND des Aufstehens (P7, Bildnummer >= K) faellt ein Pistolenschuss; danach im
 *      Abstand von 24 Bildern zwei weitere. Je Bild wird geloggt, welchen Weg der Treffer
 *      nimmt (Handler-Zelle, +0x1D2-Spalte, Zustandsfolge). Zaehler: wie oft faengt die
 *      Aufsteh-Kette von vorn an (P7 -> P6 = 0x60501-Route) bzw. wie oft faellt er neu
 *      (-> P0 = 0x501-Flinch).
 *   C) Dasselbe im LIEGEN (P3..P5), zum Vergleich.
 *   Zonen: Zielen eben (D-Pad frei) und Zielen tief (D-Pad unten).
 * KEIN add_test — reine Messsonde. */
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_aim_elevation(void);
extern int  re15_re2z_last_hit_handler(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int32_t s_px, s_pz; static int16_t s_pry;
static void pin_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = s_px; pl->z = s_pz; pl->y = 0; pl->rot_y = s_pry;
}

static const char *hname(int h)
{
    switch (h) {
    case 0: return "-";
    case 1: return "MAIN5438";
    case 2: return "STAG5BC0";
    case 3: return "RAGD66FC";
    case 4: return "SLID703C";
    case 5: return "KNCK7438";
    case 6: return "LGHT7EF0";
    default: return "?";
    }
}

static void line(const char *tag, int f, const re15_actor_t *e)
{
    printf("%s f%-4d st=%u/%2u/%u mo=0x%02X fr=%3u | 21A=0x%04X 1D3=0x%02X 223=%3d 222=%u 16B=%u "
           "1D2=%u 10E=0x%04X grid=0x%02X | hp=%4d y=%6ld react=0x%02X hnd=%s\n",
           tag, f, e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion,
           (unsigned)e->anim_frame, (unsigned)e->re2z_flags21a, (unsigned)e->re2z_self1d3,
           (int)e->re2z_res223, (unsigned)e->re2z_flag222, (unsigned)e->re2z_gaitrow,
           (unsigned)e->re2z_hits1d2, (unsigned)e->re2z_f10e, e->grid_id, (int)e->hp,
           (long)e->y, e->hit_react, hname(re15_re2z_last_hit_handler()));
}

static int bringup(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    re15_player_set_aim_clip_len(12);
    /* Ziel: der erste AUFRECHTE Zombie (0x10/0x11), alle anderen weg */
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("  spawn slot%d typ=0x%02X grid=0x%02X pos=(%ld,%ld) st=%u/%u/%u mo=0x%02X 10E=0x%04X\n",
               s, e->type, e->grid_id, (long)e->x, (long)e->z, e->state, e->sub_state_1,
               e->sub_state_2, (unsigned)e->motion, (unsigned)e->re2z_f10e);
        if (slot < 0 && (e->type == 0x10 || e->type == 0x11)) slot = s;   /* 1140: alle Fresser 0x86 */
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (slot < 0) { printf("FAIL: kein aufrechter Zombie in ROOM1140\n"); return -1; }
    re15_actor_t *e = &g_actors[slot];
    /* Spieler 1500 vor dem Zombie, Blick auf ihn (Tank-Ausrichtung wie probe_1140_feeder_shot) */
    s_px = e->x - 1500; s_pz = e->z;
    s_pry = (int16_t)(((int)re15_atan2_q12(e->z - s_pz, e->x - s_px) - 0x400) & 0x0fff);
    pin_player();
    re15_player_set_equipped_weapon(3);                       /* Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3 */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    printf("  Ziel slot%d typ=0x%02X pos=(%ld,%ld) grid=0x%02X st=%u/%u/%u; Spieler (%ld,%ld) ry=%d\n",
           slot, e->type, (long)e->x, (long)e->z, e->grid_id, e->state, e->sub_state_1,
           e->sub_state_2, (long)s_px, (long)s_pz, (int)s_pry);
    return slot;
}

/* Den Zombie WACH machen und mit dem Flinch-Wort 0x501 (@0x801050A4-AC) in EXEC[5] schicken. */
static void wake_and_knock(re15_actor_t *e)
{
    /* erst den RE2-INIT (Zustand 0) laufen lassen: +0x223-Seed, +0x21A-Clear, word0-Bits */
    for (int k = 0; k < 3; k++) { pin_player(); frame(0, 0); }
    line("INIT", 0, e);
    e->state = 1; e->grid_id = 0; e->sub_state_1 = 1; e->sub_state_2 = 0; e->hp = 200;
    e->re2z_f10e &= (uint16_t)~0x4001u;                 /* kein Limpet-Latch, kein Kriecher-Bit */
    re15_ai_set_state_word(e, 0x501);
}

/* Ticken (Spieler zielt nicht), bis Phase `want` von EXEC[5] erreicht ist; Log bei Aenderung */
static int tick_until_phase(re15_actor_t *e, int *f, int want, int max, int verbose)
{
    unsigned last = 0xffffffffu;
    for (int k = 0; k < max; k++, (*f)++) {
        pin_player(); frame(0, 0);
        unsigned w = ((unsigned)e->state << 16) | ((unsigned)e->sub_state_1 << 8) | e->sub_state_2;
        if (w != last || verbose) { line("    ", *f, e); last = w; }
        if (e->state == 1 && e->sub_state_1 == 5 && (int)e->sub_state_2 == want) return 1;
    }
    return 0;
}

/* Zielen: R1 halten bis aim_ready (der Zombie tickt weiter) */
static void aim(re15_actor_t *e, int *f, int down)
{
    uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, (*f)++) { pin_player(); frame(cur, 0); }
    printf("  aim_ready=%d elev=%d nach f%d; Zombie st=%u/%u/%u mo=0x%02X fr=%u 21A=0x%04X\n",
           re15_player_aim_ready(), re15_player_aim_elevation(), *f, e->state, e->sub_state_1,
           e->sub_state_2, (unsigned)e->motion, (unsigned)e->anim_frame, (unsigned)e->re2z_flags21a);
}

/* Schiessen: `shots` Schuesse im Abstand `gap`, dann `tail` Bilder nachlaufen; alles je Bild. */
static void shoot_and_log(re15_actor_t *e, int *f, int down, int shots, int gap, int tail)
{
    uint16_t hold = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    int hp_last = e->hp, hits = 0, restarts = 0, refalls = 0, standups = 0;
    unsigned prev_s1 = e->sub_state_1, prev_s2 = e->sub_state_2, prev_st = e->state;
    int total = shots * gap + tail;
    for (int k = 0; k < total; k++, (*f)++) {
        pin_player();
        int fire = (k % gap == 0) && (k / gap) < shots;
        frame((uint16_t)(hold | (fire ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(fire ? RE15_PAD_BIT_SQUARE : 0));
        const char *tag = "    ";
        if (fire) tag = "FIRE";
        if (e->hp < hp_last) { hits++; tag = "HIT!"; }
        hp_last = e->hp;
        /* Ereignisse an der Zustandsfolge */
        if (e->state == 1 && e->sub_state_1 == 5) {
            if (prev_st == 1 && prev_s1 == 5 && prev_s2 == 7 && e->sub_state_2 == 6) {
                restarts++; tag = "RST6";           /* P7 -> P6: 0x60501-Route, Aufsteher von vorn */
            }
            if (!(prev_st == 1 && prev_s1 == 5 && prev_s2 == 0) && e->sub_state_2 == 0 &&
                !(prev_st == 1 && prev_s1 == 5 && prev_s2 == 8)) {
                refalls++; tag = "FALL";            /* -> P0: 0x501-Flinch, neuer Sturz */
            }
        }
        if (prev_st == 1 && prev_s1 == 5 && e->state == 1 && e->sub_state_1 == 1) { standups++; tag = "UP!!"; }
        line(tag, *f, e);
        prev_s1 = e->sub_state_1; prev_s2 = e->sub_state_2; prev_st = e->state;
        if (e->hp < 0) { printf("  (tot)\n"); break; }
    }
    printf("  SUMME: %d Treffer, Aufsteher-Neustarts P7->P6 = %d, Neu-Stuerze ->P0 = %d, "
           "aufgestanden (5->1) = %d, Ende st=%u/%u/%u mo=0x%02X fr=%u hp=%d\n",
           hits, restarts, refalls, standups, e->state, e->sub_state_1, e->sub_state_2,
           (unsigned)e->motion, (unsigned)e->anim_frame, (int)e->hp);
}

/* B) Treffer WAEHREND des Aufstehens (P7, Clip 8/9 ab Bild K) */
static void part_b(int down, int K)
{
    printf("\n===== B) Schuss WAEHREND des Aufstehens (P7 Bild>=%d), Zielen %s =====\n",
           K, down ? "TIEF (D-Pad unten)" : "EBEN");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    wake_and_knock(e);
    printf("  -- A) Sturzkette bis P6 (Zustandswechsel geloggt) --\n");
    if (!tick_until_phase(e, &f, 6, 3000, 0)) { printf("  FAIL: P6 nicht erreicht\n"); return; }
    /* P7 erreichen */
    if (!tick_until_phase(e, &f, 7, 5, 1)) { printf("  FAIL: P7 nicht erreicht\n"); return; }
    /* bis Bild K des Aufsteh-Clips (dabei schon zielen) */
    aim(e, &f, down);
    while ((int)e->anim_frame < K && e->sub_state_2 == 7 && f < 6000) {
        pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0)), 0); f++;
    }
    line("PRE ", f, e);
    shoot_and_log(e, &f, down, 3, 24, 260);
}

/* C) Treffer im LIEGEN (P3..P5) */
static void part_c(int down)
{
    printf("\n===== C) Schuss im LIEGEN (P3..P5), Zielen %s =====\n", down ? "TIEF" : "EBEN");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    wake_and_knock(e);
    if (!tick_until_phase(e, &f, 3, 400, 0)) { printf("  FAIL: P3 nicht erreicht\n"); return; }
    /* 10 Bilder liegen lassen, dabei zielen */
    aim(e, &f, down);
    for (int k = 0; k < 10; k++, f++) { pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0)), 0); }
    line("PRE ", f, e);
    shoot_and_log(e, &f, down, 2, 24, 300);
}

/* D) Kontrolle: Schuss auf den STEHENDEN Zombie (wie kommt er ueberhaupt in EXEC[5]?) */
static void part_d(void)
{
    printf("\n===== D) Kontrolle: Pistolenschuesse auf den stehenden Zombie, bis er faellt =====\n");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    e->state = 1; e->grid_id = 0; e->sub_state_1 = 1; e->sub_state_2 = 0; e->hp = 200;
    e->re2z_f10e &= (uint16_t)~0x4000u;
    re15_ai_set_state_word(e, 0x101);
    aim(e, &f, 0);
    shoot_and_log(e, &f, 0, 8, 24, 200);
}

/* E) Treffer in den FRESSER-Aufsteher (EXEC[8] P3/P4, Clip 0x15; setzt +0x21A|=0x10 @0x80103D00) */
static void part_e(int down)
{
    printf("\n===== E) Schuss in den Fresser-Aufsteher EXEC[8] P3/P4, Zielen %s =====\n", down ? "TIEF" : "EBEN");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    /* der Fresser wacht von selbst (Spieler 1500 entfernt); INIT -> 1/8/3 (Aufstehen) */
    for (int k = 0; k < 3; k++, f++) { pin_player(); frame(0, 0); line("    ", f, e); }
    aim(e, &f, down);
    line("PRE ", f, e);
    shoot_and_log(e, &f, down, 2, 24, 120);
}

int main(void)
{
    printf("== probe_r16_aufstehen_schuss: Treffer waehrend des Aufstehens (RE2-Flavor) ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1140.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    const char *only = getenv("PROBE_ONLY");
    if (!only || strstr(only, "B")) {
        part_b(0, 10);
        part_b(1, 10);
        part_b(0, 40);
    }
    if (!only || strstr(only, "C")) {
        part_c(0);
        part_c(1);
    }
    if (!only || strstr(only, "D")) part_d();
    if (!only || strstr(only, "E")) { part_e(0); part_e(1); }
    printf("\n(Sonde: reine Messung, kein PASS/FAIL)\n");
    return 0;
}
