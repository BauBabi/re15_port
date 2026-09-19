/* test_p2_kriecher1010.c — PINS (Phase 2, 2026-09-19), Thema kriecher-1010, RE2-Flavor.
 * Dossier: analysis/befunde_2026-09-19/kriecher-1010.md (§2 Original, §4 F1/F2).
 *
 *   hurt_reentry  (unit_re2z_crawl_hurt_reentry)
 *     ROOM1010 Cut 4 (Suedtuer), Kriecher A in der Lokomotion (RE2-Deskriptor-3-Zweig
 *     `sw a1(=1),4` @0x80100B8C, hier explizit ueber re15_re2z_enter_crawler(sub 0), damit der
 *     Pin nicht am Totstellen haengt), Ein-Angreifer-Riegel Spieler+0x1D3|=0x80 gesetzt
 *     (DECIDE[0] kehrt @0x80102F4C-58 um), Pistole mit D-Pad DOWN, EIN Treffer.
 *     Gepinnt: im Wiedereintritts-Tick der Lokomotion (HURT-P2 @0x80107A54-58 -> P0
 *     @0x80103064) ist der Versatz 0 (P1-e7c @0x8010312C gegen die in P0 neu verankerte
 *     Momentaufnahme, @0x80103094/@0x801030B8), und in den 5 Bildern danach ist jeder Versatz
 *     <= max_f |root(f+1) - root(f)| des Kriech-Clips 5 — die Schranke wird AUS DER BANK
 *     berechnet, nicht geraten. Gegenprobe im Test: die ALTE Rechnung (Momentaufnahme vor dem
 *     Treffer gegen sx(r)) haette die Schranke verletzt, sonst prueft das Szenario nichts.
 *
 *   spawn_wait  (unit_re2z_crawler_spawn_wait)
 *     ROOM1010 Cut 4, Spieler am Tuer-Ziel, 300 Bilder: beide 0x81-Kriecher liegen still —
 *     Wort 0x201 (`sw 513,4` @0x80100B74), Clip 0x17 (EXEC[2] @0x80103B5C-60), Position
 *     unveraendert. Dann Spieler in den 512er-Sektor vor Kriecher A (dist < 0x514):
 *     DECIDE[2] @0x80103A70 -> 0x101 (@0x80103B10-14) im naechsten Tick, Riegel gesetzt
 *     (@0x80103B18-2C). Der GRIFF endet fuer den Kriecher im TOD (Risiko 1 des Dossiers,
 *     geklaert): FUN_801025EC P5 advanct +0x6 in beiden Zweigen (@0x80102A6C-80), der
 *     Kriecher-Zweig (s5&1, @0x80102A78-7C) ueberspringt den Bild-7-Schnitt (@0x80102BC4-C8)
 *     -> P6 @0x80102BE8 `sw 7,4(s1)` @0x80102BF4, `sh -1,342(s1)` @0x80102BFC. P7/P8 und
 *     der 0x501-Wurf (@0x80102D2C) sind fuer den Kriecher unerreichbar. Gepinnt: Zustand 7,
 *     hp < 0, Phase 6 gesehen, Phasen 7/8 nie, Sub 5 nie. */
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
extern int  re15_sin_q12(int), re15_cos_q12(int);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int s_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { s_fail++; printf("FAIL: " __VA_ARGS__); printf("\n"); } \
                              else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

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

static int32_t s_px, s_pz;
static int s_latch = 0;
static void pin_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = s_px; pl->z = s_pz; pl->y = 0;
    if (s_latch) pl->re2z_self1d3 |= 0x80u;
}

static int bringup(uint8_t cut, int32_t px, int32_t pz)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1010;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; s_px = px; s_pz = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    if (!load_bank_re2(0x10) || !load_bank_re2(0x11)) return 0;
    re15_player_set_aim_clip_len(12);
    return 1;
}

/* Kriecher A = der Spawn bei (950,-1700) (Sce_em_set @Datei 0x0A12), B wird isoliert. */
static int find_crawlers(int *a, int *b)
{
    *a = -1; *b = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (g_actors[s].x == 950 && *a < 0) *a = s; else if (*b < 0) *b = s;
    }
    return *a >= 0;
}

/* Schranke aus der Bank: max |root(f+1) - root(f)| ueber den Kriech-Clip (Bild 0 verankert die
 * Momentaufnahme neu, @0x80015F14-1C, deshalb zaehlt am Umbruch |root(0)| statt root(0)-root(49)). */
static int clip_max_step(uint8_t type, int clip, int *fc_out)
{
    re15_enemy_bank_t *b = re15_enemy_find(type);
    if (!b || !b->ok || clip >= b->anim.clip_count) return -1;
    int fc = b->anim.clips[clip].frame_count; *fc_out = fc;
    int best = 0;
    int16_t px = 0, pz = 0;
    for (int f = 0; f < fc; f++) {
        int kf = (int)(re15_emd_get_frame_entry(&b->anim, clip, f) & 0xFFFu);
        int16_t sx = 0, sy = 0, sz = 0;
        re15_emd_get_keyframe_speed(&b->skel, kf, &sx, &sy, &sz);
        int dx = (f == 0) ? sx : sx - px, dz = (f == 0) ? sz : sz - pz;
        int d2 = dx * dx + dz * dz;
        if (d2 > best) best = d2;
        px = sx; pz = sz;
    }
    int r = 0; while ((r + 1) * (r + 1) <= best) r++;
    return r;
}

static int root_sx(uint8_t type, int clip, int f)
{
    re15_enemy_bank_t *b = re15_enemy_find(type);
    int kf = (int)(re15_emd_get_frame_entry(&b->anim, clip, f) & 0xFFFu);
    int16_t sx = 0, sy = 0, sz = 0;
    re15_emd_get_keyframe_speed(&b->skel, kf, &sx, &sy, &sz);
    return sx;
}

static int test_hurt_reentry(void)
{
    printf("== unit_re2z_crawl_hurt_reentry ==\n");
    if (!bringup(4, 2450, -1700)) { printf("SKIP: RE2-Bank CDEMD0.EMS fehlt\n"); return 77; }
    int a, bslot;
    if (!find_crawlers(&a, &bslot)) { printf("FAIL: Kriecher A fehlt\n"); return 1; }
    if (bslot >= 0) { g_actors[bslot].x = 30000; g_actors[bslot].z = 30000; }
    re15_actor_t *e = &g_actors[a];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_re2z_enter_crawler(e, NULL, 0u);                      /* Deskriptor-3-Zweig @0x80100B8C */
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    s_latch = 1;
    int fc = 0;
    int bound = clip_max_step(0x10, 5, &fc);
    printf("Schranke aus der Bank: Clip 5, %d Bilder, max Schritt = %d\n", fc, bound);
    CHECK(bound > 0 && bound < 200, "Schranke aus der Bank plausibel gelesen (%d)", bound);

    int f = 0;
    for (; f < 200; f++) {
        pin_player(); frame(0, 0);
        if (e->ai_dist < 1200u && f > 5) { f++; break; }
    }
    CHECK(e->state == 1 && e->sub_state_1 == 0 && e->motion == 5, "Kriecher kriecht vor dem Schuss (st=%u/%u mo=%d)",
          e->state, e->sub_state_1, (int)e->motion);
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, f++) {
        pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN), 0);
    }
    CHECK(re15_player_aim_ready(), "Zielen bereit (f%d, dist %u)", f, (unsigned)e->ai_dist);

    int hp_last = e->hp, hit = 0, reentry = 0, checked = 0, bad = 0;
    int32_t lx = e->x, lz = e->z;
    int rp_x_before = 0, reentry_r = -1;
    uint8_t st_prev = e->state, s1_prev = e->sub_state_1, s2_prev = e->sub_state_2;
    int16_t mo_prev = e->motion;
    for (int k = 0; k < 120; k++, f++) {
        pin_player();
        uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN | (k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        frame(cur, (uint16_t)(k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        int32_t dx = e->x - lx, dz = e->z - lz;
        int d2 = (int)(dx * dx + dz * dz);
        if (!hit && e->hp < hp_last) { hit = 1; rp_x_before = e->root_prev_x; printf("Treffer f%d: hp %d->%d, mo=%d fr=%u rp_x=%d\n", f, hp_last, (int)e->hp, (int)e->motion, (unsigned)e->anim_frame, rp_x_before); }
        hp_last = e->hp;
        /* Wiedereintritt: voriger Tick Zustand 1/0 mit HURT-Clip 6 (P2 hat Wort 1 geschrieben),
         * dieser Tick Lokomotion P1 (1/0/1) auf Clip 5 */
        if (hit && !reentry && st_prev == 1 && s1_prev == 0 && mo_prev == 6
            && e->state == 1 && e->sub_state_1 == 0 && e->sub_state_2 == 1 && e->motion == 5) {
            reentry = 1; reentry_r = (int)e->anim_frame;
            printf("Wiedereintritt f%d: Bild r=%d, dx=%ld dz=%ld, r144=%d rp_x=%d\n", f, reentry_r,
                   (long)dx, (long)dz, (int)e->re2z_root144, (int)e->root_prev_x);
            CHECK(dx == 0 && dz == 0, "Wiedereintritts-Tick: Versatz 0 (dx=%ld dz=%ld) — P1-e7c @0x8010312C gegen die P0-Momentaufnahme", (long)dx, (long)dz);
            /* Gegenprobe: die alte Rechnung sx(r) - Momentaufnahme(vor Treffer) haette die
             * Schranke gerissen — sonst pruefte das Szenario nichts */
            int old_dx = root_sx(0x10, 5, reentry_r % (fc > 0 ? fc : 1)) - rp_x_before;
            printf("Gegenprobe alte Rechnung: sx(%d) - %d = %d\n", reentry_r, rp_x_before, old_dx);
            CHECK(old_dx < -bound || old_dx > bound, "Szenario traegt: alte Rechnung %d liegt ausserhalb der Schranke %d", old_dx, bound);
        } else if (reentry && checked < 5) {
            checked++;
            if (d2 > bound * bound) { bad++; printf("  f%d: Versatz^2 %d > Schranke^2 %d (dx=%ld dz=%ld)\n", f, d2, bound * bound, (long)dx, (long)dz); }
            else printf("  f%d: dx=%ld dz=%ld ok (Schranke %d)\n", f, (long)dx, (long)dz, bound);
        }
        lx = e->x; lz = e->z;
        st_prev = e->state; s1_prev = e->sub_state_1; s2_prev = e->sub_state_2; mo_prev = e->motion;
        (void)s2_prev;
        if (reentry && checked >= 5) break;
    }
    CHECK(hit, "Treffer registriert");
    CHECK(reentry, "Wiedereintritt in die Lokomotion beobachtet");
    CHECK(checked == 5 && bad == 0, "5 Bilder nach dem Wiedereintritt innerhalb der Bank-Schranke (%d verletzt)", bad);
    return s_fail ? 1 : 0;
}

static int test_spawn_wait(void)
{
    printf("== unit_re2z_crawler_spawn_wait ==\n");
    if (!bringup(4, 3650, -3950)) { printf("SKIP: RE2-Bank CDEMD0.EMS fehlt\n"); return 77; }
    int a, bslot;
    if (!find_crawlers(&a, &bslot)) { printf("FAIL: Kriecher A fehlt\n"); return 1; }
    re15_actor_t *e = &g_actors[a];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t ax = e->x, az = e->z, bx = 0, bz = 0;
    if (bslot >= 0) { bx = g_actors[bslot].x; bz = g_actors[bslot].z; }
    CHECK(bslot >= 0, "zwei Kriecher gespawnt (A slot %d, B slot %d)", a, bslot);
    s_latch = 0;
    int f = 0;
    for (; f < 300; f++) { pin_player(); frame(0, 0); }
    CHECK(e->state == 1 && e->sub_state_1 == 2, "A nach 300 Bildern im Kriecher-WAIT 0x201 (st=%u/%u/%u)", e->state, e->sub_state_1, e->sub_state_2);
    CHECK(e->motion == 0x17, "A liegt in Clip 0x17 (mo=0x%02X)", (unsigned)e->motion);
    CHECK(e->x == ax && e->z == az, "A unbewegt (%ld,%ld)", (long)e->x, (long)e->z);
    if (bslot >= 0) {
        re15_actor_t *b = &g_actors[bslot];
        CHECK(b->state == 1 && b->sub_state_1 == 2 && b->motion == 0x17 && b->x == bx && b->z == bz,
              "B ebenfalls im WAIT, unbewegt (st=%u/%u mo=0x%02X)", b->state, b->sub_state_1, (unsigned)b->motion);
        b->x = 30000; b->z = 30000;                                /* isolieren */
    }
    /* Spieler in den 512er-Sektor vor A (dist 600 < 0x514) */
    {   int ang = ((int)e->rot_y + 1024) & 0xfff;
        s_px = e->x + ((600 * re15_sin_q12(ang)) >> 12);    /* atan2-Konvention 0 = +Z, 1024 = +X
                                                              * (re15_actor.h: rot_y 0 = +X) */
        s_pz = e->z + ((600 * re15_cos_q12(ang)) >> 12);
        pin_player();
        CHECK(re15_ai_arc_test(e, pl->x, pl->z, 512) == 0, "Spieler im 512er-Sektor vor A (%ld,%ld)", (long)s_px, (long)s_pz);
    }
    int woke_f = -1;
    for (int k = 0; k < 3; k++, f++) {
        pin_player(); frame(0, 0);
        if (e->state == 1 && e->sub_state_1 == 1) { woke_f = k; break; }
    }
    CHECK(woke_f >= 0, "DECIDE[2] -> 0x101 GRIFF (nach %d Tick(s), st=%u/%u/%u dist=%u los=%d)",
          woke_f, e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->ai_dist, (int)e->re2z_los154);
    CHECK(pl->re2z_self1d3 & 0x80u, "Riegel Spieler+0x1D3 |= 0x80 gesetzt (0x%02X)", (unsigned)pl->re2z_self1d3);
    /* Griff bis zum Ausgang: Zustand 7 (TOD), Phase 6 gesehen, 7/8 nie, Sub 5 nie */
    int seen6 = 0, seen78 = 0, sub5 = 0, exit_state = -1;
    for (int k = 0; k < 900; k++, f++) {
        pin_player(); frame(0, 0);
        if (e->state == 1 && e->sub_state_1 == 1) {
            if (e->sub_state_2 == 6) seen6 = 1;
            if (e->sub_state_2 == 7 || e->sub_state_2 == 8) seen78 = 1;
        }
        if (e->state == 1 && e->sub_state_1 == 5) sub5 = 1;
        if (e->state != 1) { exit_state = e->state; break; }
    }
    printf("Griff-Ausgang: Zustand %d nach f%d, hp=%d, seen6=%d seen78=%d sub5=%d\n", exit_state, f, (int)e->hp, seen6, seen78, sub5);
    CHECK(exit_state == 7, "Kriecher-Griff endet im TOD (sw 7,4 @0x80102BF4): Zustand %d", exit_state);
    CHECK(e->hp < 0, "hp = -1 (@0x80102BFC): %d", (int)e->hp);
    CHECK(seen6, "P6 @0x80102BE8 wurde durchlaufen");
    CHECK(!seen78, "P7/P8 nie (Bild-7-Schnitt nur aufrecht, @0x80102BC4-C8)");
    CHECK(!sub5, "0x501-Wurf (@0x80102D2C) nie");
    return s_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1010.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    const char *mode = (argc > 1) ? argv[1] : "hurt_reentry";
    int rc;
    if (!strcmp(mode, "hurt_reentry")) rc = test_hurt_reentry();
    else if (!strcmp(mode, "spawn_wait")) rc = test_spawn_wait();
    else { printf("unbekannter Modus %s\n", mode); return 2; }
    printf("%s: %s\n", mode, rc == 0 ? "PASS" : (rc == 77 ? "SKIP" : "FAIL"));
    return rc;
}
