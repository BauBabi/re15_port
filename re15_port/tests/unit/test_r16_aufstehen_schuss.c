/* test_r16_aufstehen_schuss.c — PIN (Phase 2, 2026-09-19, Thema liegende-und-aufstehen)
 *
 * Dossier: analysis/befunde_2026-09-19/aufstehen-schuss.md, Fix-Plan Schritt 8.
 * Nutzer: "Wenn die Zombies im Prozess sind wieder aufzustehen, laesst man sie mit Schuss
 *          wieder hin fallen, und dann probieren sie wieder aufzustehen usw. Das ist im
 *          Original Resident Evil 2 anders — dort reagieren sie zwar auf Schuesse, stehen
 *          aber weiter auf."
 *
 * Original (EMZ0.BIN, RAW @0x80100000): die HURT-Wurzel FUN_80104F40 prueft VOR Flinch-Tor
 * und Liege-Route `+0x21A & 0x10` (@0x80105014-1C) und ruft FUN_80107A78 (@0x8010502C), danach
 * `j 0x80105418` = Epilog. Das Bit setzen EXEC[5]-P6 (@0x80103578-8C, Boden-Aufsteher Clip 8/9)
 * und EXEC[8]-P3 (@0x80103D00-10, Fresser-Aufsteher Clip 0x15). FUN_80107A78 wechselt den Clip
 * NICHT (nur `sb 15,334` @0x80107B3C), sichert das Tick-Ende-Zustandswort +0x228
 * (@0x8010061C-28) nach +0x22C (@0x80107B4C, nur wenn `+0x228 & 0xff == 1` @0x80107B40-44),
 * laesst den Clip in P1..P3 zu Ende laufen (Advance a3=256 @0x80107D0C-10) und stellt am Exit
 * +0x22C mit `+0x6 += 1` wieder her (@0x80107EB8-CC) -> EXEC[5] P8 = `0x101`, er STEHT.
 *
 * Echter Weg wie die Messsonde probe_r16_aufstehen_schuss (ROOM1140.RDT, RE2-Flavor, RE2-
 * Baenke, Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3, Pad R1/SQUARE durch re15_game_step):
 *   [B] Zombie per 0x501 (Flinch-Commit @0x801050A4-AC) in EXEC[5]; in P7 (Clip 8 ab Bild K)
 *       drei Schuesse im Abstand 24 -> KEIN Neustart P7->P6 (0x60501), KEIN Neusturz ->P0,
 *       Clip 8 laeuft ohne Ruecksprung zu Ende, jeder Treffer nimmt FUN_80107A78, +0x22C bleibt
 *       0x00070501, Ende: aufgestanden (1/5 -> 1/1), +0x21A & 0x12 == 0, +0x10E & 0x2000 == 0.
 *   [C] Treffer im LIEGEN (P3, Zielen TIEF) -> Liege-Route 0x60501 -> P6 (byte-true, bleibt);
 *       der Folgetreffer im Aufsteher startet ihn NICHT neu.
 *   [E] Fresser-Aufsteher EXEC[8] P4 (Clip 0x15): Treffer nimmt FUN_80107A78 statt des Haupt-
 *       Treffers, Clip 0x15 laeuft zu Ende, Bit 0x10 faellt, Exit ueber +0x22C = 0x00040801 -> P5.
 */
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

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_aim_elevation(void);
extern int  re15_re2z_last_hit_handler(void);

#define HND_MAIN   1   /* RE2ZH_MAIN  = 0x80105438 */
#define HND_GETUP  7   /* Port-Diagnose: FUN_80107A78 (re2z_getup_hurt) */

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
    case 7: return "GETUP7A78";
    default: return "?";
    }
}

static void line(const char *tag, int f, const re15_actor_t *e)
{
    printf("%s f%-4d st=%u/%2u/%u mo=0x%02X fr=%3u | 21A=0x%04X 1D3=0x%02X 223=%3d 222=%u 16B=%u "
           "1D2=%u 10E=0x%04X 22C=0x%08X | hp=%4d react=0x%02X hnd=%s\n",
           tag, f, e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion,
           (unsigned)e->anim_frame, (unsigned)e->re2z_flags21a, (unsigned)e->re2z_self1d3,
           (int)e->re2z_res223, (unsigned)e->re2z_flag222, (unsigned)e->re2z_gaitrow,
           (unsigned)e->re2z_hits1d2, (unsigned)e->re2z_f10e, (unsigned)e->re2z_word22c,
           (int)e->hp, e->hit_react, hname(re15_re2z_last_hit_handler()));
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
    int ok = load_bank_re2(0x10) & load_bank_re2(0x11) & load_bank_re2(0x16);
    CHECK(ok, "RE2-Baenke 0x10/0x11/0x16 laden nicht — ohne Bank ist clip_len 0");
    re15_player_set_aim_clip_len(12);
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (slot < 0 && (e->type == 0x10 || e->type == 0x11)) slot = s;   /* 1140: alle Fresser 0x86 */
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    CHECK(slot >= 0, "kein aufrechter Zombie in ROOM1140");
    if (slot < 0) return -1;
    re15_actor_t *e = &g_actors[slot];
    s_px = e->x - 1500; s_pz = e->z;
    s_pry = (int16_t)(((int)re15_atan2_q12(e->z - s_pz, e->x - s_px) - 0x400) & 0x0fff);
    pin_player();
    re15_player_set_equipped_weapon(3);                       /* Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3 */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    return slot;
}

/* Den Zombie WACH machen und mit dem Flinch-Wort 0x501 (@0x801050A4-AC) in EXEC[5] schicken. */
static void wake_and_knock(re15_actor_t *e)
{
    for (int k = 0; k < 3; k++) { pin_player(); frame(0, 0); }
    e->state = 1; e->grid_id = 0; e->sub_state_1 = 1; e->sub_state_2 = 0; e->hp = 200;
    e->re2z_f10e &= (uint16_t)~0x4001u;
    re15_ai_set_state_word(e, 0x501);
}

static int tick_until_phase(re15_actor_t *e, int *f, int want, int max)
{
    for (int k = 0; k < max; k++, (*f)++) {
        pin_player(); frame(0, 0);
        if (e->state == 1 && e->sub_state_1 == 5 && (int)e->sub_state_2 == want) return 1;
    }
    return 0;
}

static void aim(int *f, int down)
{
    uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, (*f)++) { pin_player(); frame(cur, 0); }
    CHECK(re15_player_aim_ready() && re15_player_aim_elevation() == (down ? -1 : 0),
          "Zielen nicht bereit (ready=%d elev=%d)", re15_player_aim_ready(), re15_player_aim_elevation());
}

/* Messwerte eines Feuer-Laufs */
typedef struct {
    int hits, restarts, refalls, standups;
    int hits_getup;        /* Treffer, deren Handler FUN_80107A78 war */
    int hits_main;         /* Treffer ueber den Haupt-Treffer 0x80105438 */
    int clip_changed;      /* Bild, an dem der Clip vor dem Aufstehen gewechselt hat (-1 = nie) */
    int frame_backstep;    /* Bild, an dem die Bildnummer zurueck sprang (-1 = nie) */
    int stood_frame;       /* Bild des Aufstehens (-1 = nie) */
    uint32_t word22c_last;
    uint16_t stood_21a, stood_10e;   /* +0x21A / +0x10E im Aufsteh-Bild */
    unsigned stood_s1, stood_s2;     /* Zustandswort im Aufsteh-Bild */
} run_t;

/* `shots` Schuesse im Abstand `gap` (nur solange er noch nicht steht), dann `tail` Bilder,
 * hoechstens aber 30 Bilder nach dem Aufstehen (danach greift er den Spieler, wird
 * abgeschuettelt und faellt regulaer ueber 0x501 — das ist nicht Gegenstand des Pins).
 * Invarianten bis zum Aufstehen: Clip == `keep_clip`, Bildnummer monoton.
 * `exec_sub` = 5 (Knockdown) oder 8 (Fresser). */
static run_t shoot(re15_actor_t *e, int *f, int down, int shots, int gap, int tail,
                   int keep_clip, unsigned exec_sub)
{
    run_t r; memset(&r, 0, sizeof r);
    r.clip_changed = -1; r.frame_backstep = -1; r.stood_frame = -1;
    uint16_t hold = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    int hp_last = e->hp;
    unsigned prev_s1 = e->sub_state_1, prev_s2 = e->sub_state_2, prev_st = e->state;
    unsigned prev_fr = e->anim_frame;
    int total = shots * gap + tail;
    for (int k = 0; k < total; k++, (*f)++) {
        pin_player();
        int fire = (k % gap == 0) && (k / gap) < shots && r.stood_frame < 0;
        frame((uint16_t)(hold | (fire ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(fire ? RE15_PAD_BIT_SQUARE : 0));
        const char *tag = fire ? "FIRE" : "    ";
        int hit = (e->hp < hp_last);
        if (hit) {
            r.hits++; tag = "HIT!";
            int h = re15_re2z_last_hit_handler();
            if (h == HND_GETUP) r.hits_getup++;
            if (h == HND_MAIN)  r.hits_main++;
        }
        hp_last = e->hp;
        if (e->state == 1 && e->sub_state_1 == 5) {
            if (prev_st == 1 && prev_s1 == 5 && prev_s2 == 7 && e->sub_state_2 == 6) { r.restarts++; tag = "RST6"; }
            if (!(prev_st == 1 && prev_s1 == 5 && prev_s2 == 0) && e->sub_state_2 == 0 &&
                !(prev_st == 1 && prev_s1 == 5 && prev_s2 == 8)) { r.refalls++; tag = "FALL"; }
        }
        int stood_now = (prev_st == 1 && prev_s1 == exec_sub && e->state == 1 &&
                         e->sub_state_1 != exec_sub);
        if (stood_now) {
            r.standups++; tag = "UP!!";
            if (r.stood_frame < 0) {
                r.stood_frame = *f;
                r.stood_21a = e->re2z_flags21a; r.stood_10e = e->re2z_f10e;
                r.stood_s1 = e->sub_state_1; r.stood_s2 = e->sub_state_2;
            }
        }
        if (r.stood_frame < 0 && !stood_now) {
            if ((int)e->motion != keep_clip && r.clip_changed < 0) r.clip_changed = *f;
            if (e->anim_frame < prev_fr && r.frame_backstep < 0) r.frame_backstep = *f;
        }
        if (hit || stood_now || tag[0] != ' ' || (e->state != prev_st) ||
            (e->sub_state_2 != prev_s2) || (k % 40 == 0))
            line(tag, *f, e);
        prev_s1 = e->sub_state_1; prev_s2 = e->sub_state_2; prev_st = e->state;
        prev_fr = e->anim_frame;
        if (e->hp < 0) { printf("  (tot)\n"); break; }
        if (r.stood_frame >= 0 && *f - r.stood_frame >= 30) { (*f)++; break; }
    }
    r.word22c_last = e->re2z_word22c;
    printf("  SUMME: %d Treffer (getup=%d main=%d), Neustarts P7->P6 = %d, Neu-Stuerze ->P0 = %d, "
           "aufgestanden = %d (Bild %d), Clipwechsel=%d, Bild-Ruecksprung=%d, 22C=0x%08X, "
           "Ende st=%u/%u/%u mo=0x%02X 21A=0x%04X 10E=0x%04X\n",
           r.hits, r.hits_getup, r.hits_main, r.restarts, r.refalls, r.standups, r.stood_frame,
           r.clip_changed, r.frame_backstep, r.word22c_last, e->state, e->sub_state_1,
           e->sub_state_2, (unsigned)e->motion, (unsigned)e->re2z_flags21a, (unsigned)e->re2z_f10e);
    return r;
}

/* [B] Treffer WAEHREND des Aufstehens (P7, Clip 8/9 ab Bild K) */
static void part_b(int down, int K)
{
    printf("\n===== B) Schuss WAEHREND des Aufstehens (P7 Bild>=%d), Zielen %s =====\n",
           K, down ? "TIEF" : "EBEN");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    wake_and_knock(e);
    CHECK(tick_until_phase(e, &f, 6, 3000), "B(%d,%d): P6 nicht erreicht", down, K);
    if (!(e->state == 1 && e->sub_state_1 == 5)) return;
    CHECK(tick_until_phase(e, &f, 7, 5), "B(%d,%d): P7 nicht erreicht", down, K);
    aim(&f, down);
    while ((int)e->anim_frame < K && e->sub_state_2 == 7 && f < 6000) {
        pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0)), 0); f++;
    }
    line("PRE ", f, e);
    CHECK(e->state == 1 && e->sub_state_1 == 5 && e->sub_state_2 == 7 && (e->motion == 8 || e->motion == 9)
          && (e->re2z_flags21a & 0x10u),
          "B(%d,%d): Vorbedingung P7 mit Clip 8/9 und +0x21A&0x10 nicht erfuellt (st=%u/%u/%u mo=%u 21A=0x%04X)",
          down, K, e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, (unsigned)e->re2z_flags21a);
    int keep = (int)e->motion;
    run_t r = shoot(e, &f, down, 3, 24, 260, keep, 5u);
    /* EBEN erreicht den Aufsteher nur in seinen tiefen Bildern (RE1.5-Hitscan-Geometrie, nicht
     * Gegenstand hier); die +0x22C-Erhaltung beim ZWEITEN Treffer prueft der TIEF-Lauf. */
    CHECK(r.hits >= (down ? 2 : 1), "B(%d,%d): nur %d Treffer (soll >= %d)", down, K, r.hits, down ? 2 : 1);
    CHECK(r.restarts == 0, "B(%d,%d): %d Aufsteher-Neustarts P7->P6 (Liege-Route 0x60501 @0x8010517C-84 "
          "darf bei +0x21A&0x10 nicht erreicht werden, Weiche @0x80105014-38)", down, K, r.restarts);
    CHECK(r.refalls == 0, "B(%d,%d): %d Neu-Stuerze ->P0 (Flinch-Tor @0x8010503C liegt HINTER der Weiche)",
          down, K, r.refalls);
    CHECK(r.hits_getup == r.hits, "B(%d,%d): %d von %d Treffern liefen NICHT ueber FUN_80107A78",
          down, K, r.hits - r.hits_getup, r.hits);
    CHECK(r.clip_changed < 0, "B(%d,%d): Clip wechselte bei Bild %d vor dem Aufstehen (FUN_80107A78 "
          "schreibt +0x14C/+0x14D nicht, nur +0x14E=15 @0x80107B3C)", down, K, r.clip_changed);
    CHECK(r.frame_backstep < 0, "B(%d,%d): Bildnummer sprang bei Bild %d zurueck (Clip-Neustart)",
          down, K, r.frame_backstep);
    CHECK(r.standups == 1, "B(%d,%d): aufgestanden = %d (soll 1)", down, K, r.standups);
    CHECK(r.word22c_last == 0x00070501u, "B(%d,%d): +0x22C = 0x%08X, soll 0x00070501 (Schnappschuss "
          "@0x8010061C-28 -> @0x80107B4C; zweiter Treffer darf ihn nicht ueberschreiben @0x80107B40-44)",
          down, K, r.word22c_last);
    CHECK((r.stood_21a & 0x12u) == 0, "B(%d,%d): +0x21A & 0x12 = 0x%04X im Aufsteh-Bild (Exit `andi 0xffed` "
          "@0x80107EA0-A4)", down, K, (unsigned)r.stood_21a);
    CHECK((r.stood_10e & 0x2000u) == 0, "B(%d,%d): +0x10E & 0x2000 steht im Aufsteh-Bild noch (Exit `andi 0xdfff` "
          "@0x80107EC0-C8 / P8 @0x8010373C-4C)", down, K);
    CHECK(r.stood_frame >= 0 && r.stood_s1 == 1u,
          "B(%d,%d): Aufsteh-Bild st=1/%u/%u — P8 committet 0x101 @0x801036F4-F8", down, K, r.stood_s1, r.stood_s2);
}

/* [C] Treffer im LIEGEN (P3, Zielen TIEF): Liege-Route 0x60501 -> P6 bleibt byte-true; der
 *     Folgetreffer im Aufsteher startet nicht neu. */
static void part_c(void)
{
    printf("\n===== C) Schuss im LIEGEN (P3), Zielen TIEF, dann Folgetreffer im Aufsteher =====\n");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    wake_and_knock(e);
    CHECK(tick_until_phase(e, &f, 3, 400), "C: P3 nicht erreicht");
    if (!(e->state == 1 && e->sub_state_1 == 5)) return;
    aim(&f, 1);
    for (int k = 0; k < 10; k++, f++) { pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN), 0); }
    line("PRE ", f, e);
    int hp0 = e->hp;
    /* erster Schuss: Liege-Route -> P6 */
    pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN | RE15_PAD_BIT_SQUARE), RE15_PAD_BIT_SQUARE); f++;
    line("HIT1", f, e);
    CHECK(e->hp < hp0, "C: erster Schuss im Liegen traf nicht (TIEF-Band)");
    CHECK(e->state == 1 && e->sub_state_1 == 5 && e->sub_state_2 == 6 && e->re2z_gaitrow == 1,
          "C: Liege-Treffer muss die Liege-Route 0x60501 (@0x8010517C-84, +0x16B=1 @0x8010518C) "
          "nehmen -> P6; gemessen st=%u/%u/%u 16B=%u", e->state, e->sub_state_1, e->sub_state_2,
          (unsigned)e->re2z_gaitrow);
    /* in P7 warten (Clip 8/9 laeuft), dann ein Treffer im Aufsteher */
    for (int k = 0; k < 12; k++, f++) { pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN), 0); }
    line("PRE2", f, e);
    CHECK(e->sub_state_2 == 7 && (e->re2z_flags21a & 0x10u), "C: P7 nach dem Liege-Treffer nicht erreicht");
    int keep = (int)e->motion;
    run_t r = shoot(e, &f, 1, 1, 24, 200, keep, 5u);
    CHECK(r.hits == 1, "C: Folgetreffer im Aufsteher fehlt (%d)", r.hits);
    CHECK(r.restarts == 0 && r.refalls == 0 && r.clip_changed < 0 && r.frame_backstep < 0,
          "C: Folgetreffer startete den Aufsteher neu (restarts=%d refalls=%d clip=%d back=%d)",
          r.restarts, r.refalls, r.clip_changed, r.frame_backstep);
    CHECK(r.hits_getup == 1, "C: Folgetreffer lief nicht ueber FUN_80107A78");
    CHECK(r.standups == 1, "C: aufgestanden = %d", r.standups);
}

/* [E] Fresser-Aufsteher EXEC[8] P4 (Clip 0x15, +0x21A|=0x10 @0x80103D00-10) */
static void part_e(void)
{
    printf("\n===== E) Schuss in den Fresser-Aufsteher EXEC[8] P4, Zielen EBEN =====\n");
    int slot = bringup(); if (slot < 0) return;
    re15_actor_t *e = &g_actors[slot];
    int f = 0;
    for (int k = 0; k < 3; k++, f++) { pin_player(); frame(0, 0); line("    ", f, e); }
    aim(&f, 0);
    line("PRE ", f, e);
    CHECK(e->state == 1 && e->sub_state_1 == 8 && e->sub_state_2 == 4 && e->motion == 0x15
          && (e->re2z_flags21a & 0x10u),
          "E: Vorbedingung 1/8/4 Clip 0x15 +0x21A&0x10 nicht erfuellt (st=%u/%u/%u mo=0x%02X 21A=0x%04X)",
          e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, (unsigned)e->re2z_flags21a);
    run_t r = shoot(e, &f, 0, 2, 24, 120, 0x15, 8u);
    CHECK(r.hits >= 1, "E: kein Treffer im Fresser-Aufsteher");
    CHECK(r.hits_main == 0, "E: %d Treffer liefen ueber den Haupt-Treffer 0x80105438 — die Weiche "
          "@0x80105014-1C steht davor", r.hits_main);
    CHECK(r.hits_getup == r.hits, "E: %d von %d Treffern nicht ueber FUN_80107A78", r.hits - r.hits_getup, r.hits);
    CHECK(r.clip_changed < 0 && r.frame_backstep < 0, "E: Clip 0x15 wurde abgebrochen (clip=%d back=%d)",
          r.clip_changed, r.frame_backstep);
    CHECK(r.standups == 1, "E: Fresser stand nicht auf (standups=%d)", r.standups);
    CHECK(r.word22c_last == 0x00040801u, "E: +0x22C = 0x%08X, soll 0x00040801 (Exit -> +0x6=5 -> "
          "EXEC[8] P5 `0x101` @0x80103D90-94)", r.word22c_last);
    CHECK((r.stood_21a & 0x10u) == 0, "E: +0x21A & 0x10 klebt im Aufsteh-Bild (0x%04X) — Exit @0x80107EA0-A4 "
          "bzw. P4-Clip-Ende @0x80103D84-8C loeschen es", (unsigned)r.stood_21a);
    CHECK(r.stood_frame >= 0 && r.stood_s1 == 1u, "E: Aufsteh-Bild st=1/%u/%u (P5 committet 0x101 "
          "@0x80103D90-94)", r.stood_s1, r.stood_s2);
}

int main(void)
{
    printf("== test_r16_aufstehen_schuss: Treffer waehrend des Aufstehens (RE2-Flavor) ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1140.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    part_b(0, 10);
    part_b(1, 10);
    part_b(0, 40);
    part_c();
    part_e();

    if (fails) { printf("\n%d FAIL(s)\n", fails); return 1; }
    printf("\nPASS test_r16_aufstehen_schuss\n");
    return 0;
}
