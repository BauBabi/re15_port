/* test_re2_dog_grab_anchor.c — PINs fuer die Nutzer-Befunde vom 2026-08-21 im RE2-KI-Modus.
 *
 * (A) "Wenn der Hund mich toetet, verschwinde ich irgendwie komplett."
 *     ZWEI Fehler, eine Wurzel = der Griff-ANKER (enemy_ai_re2_dog.c, re2d_grab_anchor):
 *       A1  Der Kontakt-Latch startete die Opfer-FSM OHNE Anker -> pl->anchor blieb (0,0) und
 *           re15_clip_root_motion_abs setzte Leon in den WELTURSPRUNG.
 *           GEMESSEN vorher: Latch-Frame Sprung 27479 auf PL(0,0), Hund bei (-2917,-24429).
 *       A2  Der Anker wurde an Frame 0 gezogen, platziert wurde am AKTUELLEN Frame
 *           -> pro Tick pos += rot(off(f) - off(0)).
 *           GEMESSEN vorher: Endstand PL(-298658,-3600,225648) = 389271 Einheiten vom Start.
 *     ORIGINAL (info/re2leon/PSX.EXE + COMMON/BIN/EMD0G_MOD0.BIN, selbst disassembliert):
 *       80101ed0 jal 0x80015b94   Anker   (a0 = 0x800CFBF8 = Spieler-Struktur, a3 = 0)
 *       80101ee4 jal 0x80015cb8   Pose
 *       80015db0 holt den Offset fuer BEIDE:
 *         80015db4 lw  t1,-7376(t1)  ; t1 = *(0x800ce330) = die getickte Entity (der Hund)
 *         80015dbc lbu v0,332(t1)    ; +0x14C = Clip
 *         80015df4 lbu v0,333(t1)    ; +0x14D = **AKTUELLER FRAME** (a1 == 0-Zweig)
 *       Anker und Pose heben sich damit exakt auf -> der Hund steht beim Biss STILL, und der
 *       eigentliche Zweck ist die Anker-KOPIE in den Spieler (@0x80015c7c-94).
 *       Der Kontakt-Latch @0x80104F20-74 schreibt KEIN Spieler-Kommando und KEINEN Anker; das
 *       Kommando 0x800CFBFC = 6 faellt erst @0x80101DC4, zusammen mit dem Anker @0x80101DA0.
 *
 * (B) "Ich kann teilweise immer noch nicht die Zombies treffen mit der Handfeuerwaffe."
 *     Der RE2-Vier-Gate-Trefferfilter (FUN_800470C0 @0x80047124-30/38-40/48-50/58-64) steht im
 *     Original IN DER KANDIDATENSCHLEIFE DES AUFLOESERS und haengt an keinem Gegner-Tick. Der
 *     Port bildet ihn ueber den Latch +0x93 Bit 0 ab, den bisher NUR das Ende von
 *     re15_re2z_tick schrieb. Der Zombie-Root ueberspringt seinen Tick aber bei
 *       8010042c-3c  globaler Freeze (g_pauseflags & 0x20000000)
 *       80100450-5c  Per-Entity-Bit (+0x9 & 0x20)
 *     Dann friert der Latch ein und der Gegner ist nach EINEM Treffer unverwundbar.
 *     game_step ruft den Filter deshalb zusaetzlich fuer jeden RE2-eigenen Zombie auf.
 *
 * Braucht die echte RE2-Bank (CDEMD0.EMS). Fehlt sie -> SKIP (77).
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_sin_q12(int), re15_cos_q12(int);
extern int  re2_ems_load_bank(const uint8_t *ems, size_t n, int type,
                              re15_enemy_bank_t *out, void *unused);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static int s_fail = 0, s_checks = 0;

#define CHECK(cond, ...) do { s_checks++; if (!(cond)) { s_fail++; \
    printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1;
                                                                       return 1; }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int load_room(const char *sub, int room_id)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) return 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_current_room_id = (uint16_t)room_id;
    return 1;
}

static void bringup(int room_id, int fire_sub)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (fire_sub >= 0 && s_rdt.sub_scd_count > fire_sub && s_rdt.sub_scd[fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int find_type(unsigned lo, unsigned hi)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= lo && g_actors[s].type <= hi) return s;
    return -1;
}

/* ================= TEIL A — der Hunde-Toetungsanker ==================================== */
static void part_a(void)
{
    printf("TEIL A — Hunde-Toetung: Leon bleibt am Hund, statt zu verschwinden\n");
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190)) { printf("  ROOM1190 fehlt\n"); s_fail++; return; }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup(0x1190, 13);
    if (!load_bank2(0x20)) { printf("  RE2-Bank 0x20 fehlt\n"); s_fail++; return; }
    for (int f = 0; f < 60; f++) { g_actors[0].hp = 100; frame(0, 0); }
    int slot = find_type(0x20, 0x20);
    if (slot < 0) { printf("  kein Hund in ROOM1190\n"); s_fail++; return; }
    for (int t = 1; t < RE15_ACTOR_MAX; t++) if (t != slot) g_actors[t].active = 0;

    re15_actor_t *pl = &g_actors[0], *e = &g_actors[slot];
    pl->hp = 18;                                   /* schwach -> der toedliche Latch kann greifen */
    pl->floor = e->floor;
    re15_player_cmd_reset(); re15_player_aim_reset();
    pl->x = e->x; pl->z = e->z + 3000; pl->y = e->y; pl->rot_y = 2048;

    int32_t px0 = pl->x, pz0 = pl->z;
    int latch_f = -1, maxjump = 0, anchor_zero_frames = 0;
    double maxpairdist = 0;
    int dog_moved_in_latch = 0;
    int32_t lx = pl->x, lz = pl->z;
    int32_t dogx = 0, dogz = 0; int dog_seen = 0;
    for (int f = 0; f < 1400; f++) {
        frame(0, 0);
        int mag = (int)(labs((long)(pl->x - lx)) + labs((long)(pl->z - lz)));
        if (mag > maxjump) maxjump = mag;
        lx = pl->x; lz = pl->z;
        if (re15_player_victim_state() != 0) {
            if (pl->anchor_x == 0 && pl->anchor_z == 0) anchor_zero_frames++;
            double d = sqrt((double)(pl->x - e->x) * (pl->x - e->x)
                          + (double)(pl->z - e->z) * (pl->z - e->z));
            if (d > maxpairdist) maxpairdist = d;
        }
        if (e->sub_state_1 == 7) {
            if (latch_f < 0) latch_f = f;
            if (e->sub_state_2 == 0 && e->sub_state_3 == 1) {   /* P1: Anker+Pose je Tick */
                if (dog_seen && (e->x != dogx || e->z != dogz)) dog_moved_in_latch++;
                dogx = e->x; dogz = e->z; dog_seen = 1;
            }
        }
        if (latch_f >= 0 && f - latch_f > 300) break;
    }
    double total = sqrt((double)(pl->x - px0) * (pl->x - px0)
                      + (double)(pl->z - pz0) * (pl->z - pz0));
    printf("  Latch@%d | Gesamtversatz %.0f | groesster 1-Frame-Sprung %d | max Paar-Abstand %.0f"
           " | Anker-(0,0)-Frames %d | Hund bewegt in P1: %d\n",
           latch_f, total, maxjump, maxpairdist, anchor_zero_frames, dog_moved_in_latch);

    CHECK(latch_f >= 0, "kein toedlicher Latch erreicht — Testaufbau greift nicht");
    /* A1: der Spieler darf im Opfer-Zustand NIE aus einem Null-Anker platziert werden. */
    CHECK(anchor_zero_frames == 0,
          "Spieler-Anker war in %d Opfer-Frames (0,0) -> Platzierung im Welt-Ursprung (A1)",
          anchor_zero_frames);
    /* A2: kein Weglaufen. Vorher waren es 389271 Einheiten und Einzelspruenge bis 27479. */
    CHECK(total < 20000.0, "Gesamtversatz %.0f — der Anker-Restbetrag treibt das Paar weg (A2)",
          total);
    CHECK(maxjump < 8000, "groesster 1-Frame-Sprung %d — Teleport (A1/A2)", maxjump);
    /* Der Hund steht beim Latch-Biss still: Anker und Pose ziehen denselben Frame. */
    CHECK(dog_moved_in_latch == 0,
          "Hund hat sich in %d P1-Ticks bewegt — Anker/Pose ziehen verschiedene Frames (A2)",
          dog_moved_in_latch);
}

/* ================= TEIL B — Trefferfilter ohne KI-Tick ================================== */
static void part_b(void)
{
    printf("TEIL B — Vier-Gate-Filter laeuft auch ohne KI-Tick (+0x9 & 0x20 / globaler Freeze)\n");
    if (!load_room("STAGE1/ROOM1140.RDT", 0x1140)) { printf("  ROOM1140 fehlt\n"); s_fail++; return; }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup(0x1140, -1);
    if (!load_bank2(0x10)) { printf("  RE2-Bank 0x10 fehlt\n"); s_fail++; return; }
    for (int f = 0; f < 90; f++) { g_actors[0].hp = 100; frame(0, 0); }
    int slot = find_type(0x10, 0x1f);
    if (slot < 0) { printf("  kein RE2-Zombie in ROOM1140\n"); s_fail++; return; }
    re15_actor_t *e = &g_actors[slot];

    /* POSITIV: KI-Tick per +0x9 & 0x20 abgeschaltet, Latch gesetzt -> muss trotzdem fallen.
     * Zustand deterministisch auf "aufrecht laufend" gesetzt, damit die Spawn-Pose-Ausnahme
     * (EXEC[7]/EXEC[8]) NICHT greift und wirklich die vier Gates entscheiden. Da der Tick
     * uebersprungen wird, bleibt der gesetzte Zustand stehen. */
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1;
    e->grid_id     = (uint8_t)((e->grid_id & 0x0fu) | 0x20u);   /* RE15_AI_GRID_SKIP, ohne 0x80 */
    e->hit_react   = (uint8_t)(e->hit_react | 1u);    /* `ori v0,v0,0x1` @0x800124F0 */
    e->re2z_self1d3 = 0; e->re2z_f10e &= (uint16_t)~0xC000u;
    int st = e->state, s1 = e->sub_state_1, hp = e->hp;
    frame(0, 0);
    printf("  grid&0x20-Zombie slot%d: state %d->%d sub %d->%d hp %d->%d hit_react=0x%02X\n",
           slot, st, e->state, s1, e->sub_state_1, hp, e->hp, e->hit_react);
    CHECK((e->hit_react & 1u) == 0u,
          "Latch +0x93 Bit 0 blieb stehen, obwohl alle vier Gates offen sind -> nach EINEM "
          "Treffer unverwundbar (hit_react=0x%02X)", e->hit_react);
    CHECK(e->state == st && e->sub_state_1 == s1,
          "der Nachlauf darf den Zustand NICHT anfassen (state %d->%d sub %d->%d)",
          st, e->state, s1, e->sub_state_1);

    /* NEGATIV-GEGENPROBE: mit gesetztem +0x1D3 (Gate 2) muss der Latch STEHEN bleiben. */
    e->re2z_self1d3 = 0x80u;                          /* `lbu v0,467(s0)` @0x80047138-40 */
    e->hit_react &= (uint8_t)~1u;
    frame(0, 0);
    CHECK((e->hit_react & 1u) != 0u,
          "Gate (2) +0x1D3 != 0 sperrt nicht mehr (hit_react=0x%02X) — der Filter waere blind",
          e->hit_react);

    /* NEGATIV-GEGENPROBE: tot (Gate 3) muss ebenfalls sperren. */
    e->re2z_self1d3 = 0; e->hp = -1;
    e->hit_react &= (uint8_t)~1u;
    frame(0, 0);
    CHECK((e->hit_react & 1u) != 0u,
          "Gate (3) HP < 0 sperrt nicht mehr (hit_react=0x%02X) — Leichen blieben beschiessbar",
          e->hit_react);

    /* GEGENPROBE RE1.5: im RE1.5-Modus darf der Nachlauf gar nicht greifen. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    bringup(0x1140, -1);
    for (int f = 0; f < 90; f++) { g_actors[0].hp = 100; frame(0, 0); }
    int s15 = find_type(0x10, 0x1f);
    if (s15 >= 0) {
        re15_actor_t *z = &g_actors[s15];
        z->grid_id   = (uint8_t)(z->grid_id | 0x20u);
        z->hit_react = (uint8_t)(z->hit_react | 1u);
        frame(0, 0);
        CHECK((z->hit_react & 1u) != 0u,
              "RE1.5-Modus: der RE2-Nachlauf hat den RE1.5-Latch angefasst (hit_react=0x%02X)",
              z->hit_react);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

int main(void)
{
    if (!s_ems) { s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n); }
    if (!s_ems) { printf("SKIP: CDEMD0.EMS (RE2-Baenke) nicht vorhanden\n"); return 77; }
    part_a();
    part_b();
    printf("%s: %d Pruefungen, %d Fehler\n", s_fail ? "ROT" : "GRUEN", s_checks, s_fail);
    return s_fail ? 1 : 0;
}
