/* test_zombie_bank_sfx.c — REGRESSIONS-PINS fuer die BANK-REGEL des STAGE1-Zombies.
 *
 * Nutzer-Reports 2026-08-17 (beide ZWEITMELDUNGEN):
 *   A) "ROOM1140: nach dem Reinlaufen in die Zombie-Menge, raus und wieder rein sind die
 *       Zombies broken / verhalten sich komisch."
 *   B) "ROOM10D0: Zombie-Sound beim Hinfallen nach mehreren Schuessen und beim Aufstehen
 *       nicht korrekt."
 *
 * GEMEINSAME URSACHE (selbst disassembliert, STAGE1.BIN roh @0x80100000):
 * anim_set FUN_8001f314 bekommt die BANK als a0/a1 und legt in `+0x168` den Zeiger auf das
 * Frame-Wort DER GERADE POSIERTEN BANK ab (@0x8001f36c `sw a2,360(t0)`); der SE-Dekoder
 * FUN_8001b38c liest genau diesen Zeiger (@0x8001b3a4 `lw v0,360(v0)` / @0x8001b3ac
 * `lw v0,0(v0)` / @0x8001b3b4 `srl s0,v0,22` = Bit N -> SE N).
 *
 * Eigener jal-Scan ueber GANZ STAGE1.BIN nach `jal 0x8001f314` mit Rueckwaerts-Aufloesung
 * von a0/a1 — BANK 0 (`lw a0,132` / `lw a1,364` = +0x84/+0x16c, Port b->anim_loco, 6 Clips):
 *   @0x80100c90 INIT · @0x80101da8 f890[0x00] SEARCH-STAND · @0x80102000 f890[0x01] WANDER
 *   @0x80102420 f890[0x02] ENGAGE · @0x80102efc f890[0x07] TURN
 *   @0x80103128/@0x80103144 f890[0x08] CHARGE · @0x8010590c f890[0x13] APPROACH
 *   @0x80105d44/@0x80105de8 FUN_80105b7c und @0x80105fd0/@0x80106024 FUN_80106048 (State-2-Stagger)
 * BANK 1 (`lw a0,368` / `lw a1,372` = +0x170/+0x174, Port b->anim, 43 Clips): alles andere,
 * u.a. Knockdown @0x80105248/@0x801053d8, Downed-Flinch @0x801069b8/@0x80106b28,
 * Grab @0x801026c8, Devour @0x80102cec, Feeding @0x801049a0, Standup @0x80104afc.
 *
 * Die f890-Kaskade haengt an GENAU ZWEI Grid-Nibbles: @0x8011f80c[0]=FUN_8010168c
 * (`addiu at,at,-1904` = 0x8011f890 @0x801016e0) und @0x8011f80c[2]=FUN_80101784
 * (dieselbe Tabelle @0x801017d8) — Nibble 2 ist der STEHENDE Wiedereintritts-Schlaefer.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_player.h"   /* re15_actors_anim_advance */
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
extern int g_test_room_se_tickof[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    if (s <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

static uint8_t s_blob[0x80000];
static int load_em10(void)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_XSTR(RE15_ASSETS_PATH));
    uint8_t *ems = slurp(p, &n); if (!ems) return -1;
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0, ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_blob, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own) == 0);
                ok = eb->loco_ok;
            }
        }
    }
    free(ems);
    return ok ? 0 : -1;
}

/* ---------------- PIN 1: die reine Bank-Regel ---------------------------------------- */
static int pin_bank_rule(void)
{
    if (load_em10() != 0) { printf("FAIL: EM010-Bank/Loco-Bank nicht ladbar\n"); return 1; }
    re15_actor_init();
    re15_actor_t a; memset(&a, 0, sizeof a);
    a.type = 0x10; a.active = 1;

    /* BANK 0 (LOCO): State 1, Grid-Nibble 0 ODER 2, +0x5 in {0,1,2,7,8,0x13}.
     * Motion in Loco-Reichweite (6 Clips) — die Handler schreiben genau solche Clips:
     * search-stand 0 (@0x80101d60), wander 1 (@0x80101f3c-Umfeld), charge/engage = +0x1d4 (2..5). */
    static const struct { uint8_t s1; uint8_t mo; const char *what; } loco[] = {
        { 0x00, 0, "SEARCH-STAND @0x80101da8" }, { 0x01, 1, "WANDER @0x80102000" },
        { 0x02, 2, "ENGAGE @0x80102420" },       { 0x07, 3, "TURN @0x80102efc" },
        { 0x08, 4, "CHARGE @0x80103128" },       { 0x13, 1, "APPROACH @0x8010590c" },
    };
    for (unsigned g = 0; g < 2; g++) {
        uint8_t grid = g ? 0x02 : 0x00;          /* die zwei f890-Nibbles */
        for (unsigned i = 0; i < sizeof loco / sizeof loco[0]; i++) {
            a.state = 1; a.grid_id = grid; a.sub_state_1 = loco[i].s1; a.motion = loco[i].mo;
            CHECK(re15_actor_uses_loco_bank(&a) == 1,
                  "grid 0x%02x +0x5=0x%02x -> LOCO (%s)", grid, loco[i].s1, loco[i].what);
        }
    }
    /* BANK 1 (ACTION): alle uebrigen +0x5 der f890-Kaskade. */
    static const struct { uint8_t s1; uint8_t mo; const char *what; } act[] = {
        { 0x03, 0, "GRAB @0x801026c8" },      { 0x05, 9,  "DEVOUR @0x80102cec" },
        { 0x0b, 16, "PUSHOFF" },              { 0x0c, 0x27, "FEEDING @0x801049a0" },
        { 0x0d, 0x29, "STANDUP @0x80104afc" },{ 0x11, 0x0b, "KNOCKDOWN @0x80105248" },
        { 0x12, 0x2a, "SLEEPING" },           { 0x10, 0x12, "TOGGLE" },
    };
    for (unsigned i = 0; i < sizeof act / sizeof act[0]; i++) {
        a.state = 1; a.grid_id = 0; a.sub_state_1 = act[i].s1; a.motion = act[i].mo;
        CHECK(re15_actor_uses_loco_bank(&a) == 0,
              "+0x5=0x%02x -> ACTION (%s)", act[i].s1, act[i].what);
    }
    /* Grid-Nibble 1 = Kriech-Maschine, eigene Tabelle @0x8011F920 -> NIE Loco. */
    a.state = 1; a.grid_id = 0x01; a.sub_state_1 = 0x02; a.motion = 2;
    CHECK(re15_actor_uses_loco_bank(&a) == 0, "Grid-Nibble 1 (@0x8011F920) bleibt ACTION");
    /* State 2 Steh-Stagger = LOCO (@0x80105d44), Downed-Flinch (+0x9&0x80) = ACTION (@0x801069b8) */
    a.state = 2; a.grid_id = 0x00; a.sub_state_1 = 0x03; a.motion = 2;
    CHECK(re15_actor_uses_loco_bank(&a) == 1, "State-2-Steh-Stagger -> LOCO (@0x80105d44)");
    a.grid_id = 0x80;
    CHECK(re15_actor_uses_loco_bank(&a) == 0, "State-2-Downed-Flinch -> ACTION (@0x801069b8)");
    return fails;
}

/* ---------------- PIN 2: der Steh-Idle spielt den 75-Frame-Loco-Clip ------------------ */
static int pin_idle_clip(void)
{
    if (load_em10() != 0) { printf("FAIL: EM010-Bank nicht ladbar\n"); return 1; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    /* Datenbeleg: die beiden Clip-0 sind UNTERSCHIEDLICH lang — genau daran haengt der Bug. */
    CHECK(b->anim.clip_count == 43, "ACTION-Bank hat 43 Clips (dir[3])");
    CHECK(b->anim_loco.clip_count == 6, "LOCO-Bank hat 6 Clips (dir[1])");
    CHECK(b->anim.clips[0].frame_count == 14, "ACTION-Clip 0 = 14 Frames");
    CHECK(b->anim_loco.clips[0].frame_count == 75, "LOCO-Clip 0 = 75 Frames (der echte Steh-Idle)");

    /* Der stehende Wiedereintritts-Schlaefer (behavior 0x02 -> Grid-Nibble 2, +0x5=0) muss
     * ueber die 75 Frames laufen. re15_actor_clip_len() ist die gemeinsame Uhr. */
    re15_actor_init();
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->grid_id = 0x02;
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 1; e->motion = 0;
    CHECK(re15_actor_uses_loco_bank(e) == 1, "Re-Entry-Schlaefer posiert LOCO (@0x80101da8)");
    CHECK(re15_actor_clip_len(e) == 75, "Clip-Uhr des Steh-Idles = 75 (war 14 = falsche Bank)");
    return fails;
}

/* ---------------- PIN 3: Schritt-SEs aus der Loco-Bank -------------------------------- */
/* LOCO-Clip 1 (Wander, 99 Frames) traegt Frame-Wort-Bit 1 bei f31 und f80; die ACTION-Clips
 * 0..5 tragen KEIN einziges SE-Bit. Ohne die Bank-Regel war der Schlurf-/Stagger-Zombie stumm. */
static int pin_step_sfx(void)
{
    if (load_em10() != 0) { printf("FAIL: EM010-Bank nicht ladbar\n"); return 1; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    const re15_emd_clip_t *lc = &b->anim_loco.clips[1];
    int n31 = (b->anim_loco.frames[lc->first_frame + 31] >> 22) == 0x002;
    int n80 = (b->anim_loco.frames[lc->first_frame + 80] >> 22) == 0x002;
    CHECK(n31 && n80, "LOCO-Clip 1 traegt SE-Bit 1 bei f31 und f80 (Schritte)");
    for (int c = 0; c <= 5; c++) {
        const re15_emd_clip_t *ac = &b->anim.clips[c];
        int any = 0;
        for (int f = 0; f < ac->frame_count; f++)
            if ((b->anim.frames[ac->first_frame + f] >> 22) != 0) any = 1;
        CHECK(any == 0, "ACTION-Clip %d hat KEIN SE-Bit (die falsche Bank war stumm)", c);
    }

    /* LIVE-DISKRIMINATOR: der STEH-STAGGER (State 2). Das Original posiert ihn aus BANK 0
     * (FUN_80105b7c @0x80105d3c `lw a0,132(v0)` / @0x80105d40 `lw a1,364(v0)`) und behaelt den
     * Loco-Walk-Clip OHNE +0x95-Reset — also laufen die Schritt-Frame-Woerter weiter. Die alte
     * SFX-Regel schloss State 2 aus und las die (stumme) Aktions-Bank: kein Ton. */
    re15_actor_init();
    re15_damage_seed_rng(0x0BADC0DEu);
    re15_actor_t *pl = &g_actors[0];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->grid_id = 0x00; e->hit_radius_min = 0;
    e->state = 2; e->sub_state_1 = 0x03; e->sub_state_2 = 0; e->sub_state_3 = 1;  /* Stagger, Phase 1 */
    e->motion = 2; e->anim_frame = 18; e->anim_flags = 0x04; e->hurt_clip = 2;
    e->x = 20000; e->z = 20000;
    g_actor_count = 2;
    g_test_room_se_n = 0;
    int se1 = 0;
    for (int t = 0; t < 6; t++) {
        int b4 = g_test_room_se_n;
        re15_actors_anim_advance(); re15_enemy_ai_run_all(0);
        for (int i = b4; i < g_test_room_se_n; i++) if (g_test_room_se_log[i] == 1) se1++;
    }
    printf("  [diag] Stagger nach 6 Ticks: st=%u s1=0x%02x mo=%d fr=%u loco=%d\n",
           e->state, e->sub_state_1, (int)e->motion, e->anim_frame, re15_actor_uses_loco_bank(e));
    CHECK(se1 >= 1, "Steh-Stagger feuert Schritt-SE 1 aus der Loco-Bank (gezaehlt %d)", se1);
    return fails;
}

/* ---------------- PIN 4: Knockdown-/Getup-SE-Kadenz ---------------------------------- */
/* Original FUN_8010512c: [0] Clip 0x0b + 1/4-Grunzer (rand&1)?5:8 (@0x801051f0);
 * [4] Clip 0x12 + 1/8-Grunzer (@0x801053bc). Die FALL-/AUFPRALL-Geraeusche kommen aus den
 * FRAME-WOERTERN des Clips 0x0b (f14 -> SE 1, f46 -> SE 0), der Getup-Clip 0x12 hat KEINE. */
static int pin_knock_sfx(void)
{
    if (load_em10() != 0) { printf("FAIL: EM010-Bank nicht ladbar\n"); return 1; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    const re15_emd_clip_t *fall = &b->anim.clips[0x0b];
    const re15_emd_clip_t *up   = &b->anim.clips[0x12];
    CHECK(fall->frame_count == 55, "Fall-Clip 0x0b = 55 Frames");
    CHECK(up->frame_count == 98,   "Getup-Clip 0x12 = 98 Frames");
    CHECK((b->anim.frames[fall->first_frame + 14] >> 22) == 0x002, "Fall f14 -> SE 1");
    CHECK((b->anim.frames[fall->first_frame + 46] >> 22) == 0x001, "Fall f46 -> SE 0 (Bodenaufprall)");
    { int any = 0;
      for (int f = 0; f < up->frame_count; f++)
          if ((b->anim.frames[up->first_frame + f] >> 22) != 0) any = 1;
      CHECK(any == 0, "Getup-Clip 0x12 traegt KEIN Frame-Wort-SE (@0x801053bc bleibt der einzige)"); }

    /* LIVE: Knockdown durchfahren und die SE-Folge pruefen. */
    re15_actor_init();
    re15_damage_seed_rng(0x5EED1234u);
    re15_actor_t *pl = &g_actors[0];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->z = 30000;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->grid_id = 0x00; e->hp = 60; e->hit_radius_min = 0;
    e->state = 1; e->sub_state_1 = 0x11; e->sub_state_2 = 0;    /* KNOCKDOWN Phase 0 */
    e->x = 0; e->z = 0;
    g_actor_count = 2;
    g_test_room_se_n = 0;
    int se1_tick = -1, se0_tick = -1, up_tick = -1, se_after_up = 0;
    for (int t = 0; t < 600; t++) {
        g_test_room_se_tick = t;
        int b4 = g_test_room_se_n;
        re15_actors_anim_advance(); re15_enemy_ai_run_all(0);
        if (e->sub_state_2 == 5 && up_tick < 0) up_tick = t;
        for (int i = b4; i < g_test_room_se_n; i++) {
            if (g_test_room_se_log[i] == 1 && se1_tick < 0 && e->motion == 0x0b) se1_tick = t;
            if (g_test_room_se_log[i] == 0 && se0_tick < 0 && e->motion == 0x0b) se0_tick = t;
            if (up_tick >= 0 && t > up_tick) se_after_up++;   /* im Getup-Clip: nichts mehr */
        }
        if (e->sub_state_1 != 0x11 && t > 10) break;
    }
    CHECK(se1_tick >= 0, "Fall: SE 1 gefeuert (Tick %d)", se1_tick);
    CHECK(se0_tick >= 0, "Fall: SE 0 gefeuert (Tick %d)", se0_tick);
    CHECK(se0_tick > se1_tick, "Reihenfolge f14 (SE 1) vor f46 (SE 0)");
    CHECK(se0_tick - se1_tick == 32, "Abstand f46-f14 = 32 Ticks (1 Frame/Tick, rate 0x100)");
    CHECK(se_after_up == 0, "im Getup-Clip 0x12 kein weiterer SE (gezaehlt %d)", se_after_up);
    return fails;
}

int main(int argc, char **argv)
{
    const char *which = (argc > 1) ? argv[1] : "all";
    if (!strcmp(which, "bank")  || !strcmp(which, "all")) pin_bank_rule();
    if (!strcmp(which, "idle")  || !strcmp(which, "all")) pin_idle_clip();
    if (!strcmp(which, "step")  || !strcmp(which, "all")) pin_step_sfx();
    if (!strcmp(which, "knock") || !strcmp(which, "all")) pin_knock_sfx();
    printf(fails ? "FAILED (%d)\n" : "PASSED (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
