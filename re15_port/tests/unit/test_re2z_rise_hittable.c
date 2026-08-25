/**
 * @file test_re2z_rise_hittable.c
 * @brief PIN — der RE2-Zombie ist waehrend des BODEN-AUFSTEHERS treffbar.
 *
 * ⛔ NUTZER-BEFUND 2026-08-27: "bei der Aufsteh-Animation vom Zombie ist er unverwundbar.
 * Im Original kann man da bereits anschiessen. Auch bei RE2."
 *
 * URSACHE (alles selbst disassembliert):
 *   - RE2s EINZIGER Kandidatenfilter ist FUN_800470C0 (info/re2leon/PSX.EXE). Er hat genau
 *     VIER Gates und KEIN Hoehen-/Elevations-Band:
 *         8004712c: andi v0,v0,0x1        ; aktiv?
 *         80047138: lbu  v0,467(s0)       ; +0x1D3 (Anspruch)
 *         80047148: lh   v0,342(s0)       ; HP
 *         80047160: andi v0,v0,0xc000     ; +0x10E
 *         8004716c: lhu  v1,464(s0)       ; -> direkt zur Trefferpruefung
 *   - +0x1D3 wird in EXEC[5] P2 freigegeben (`andi v1,v1,0x7f` @0x80103484 /
 *     `sb v1,467(s2)` @0x80103490) und im ganzen Fenster bis zum P7-Ende NICHT neu belegt
 *     (eigener Scan aller `sb rt,467(rs)` in 0x80103484..0x801036f0: genau EIN Store, und
 *     das ist die Freigabe selbst). Der Aufsteher ist im Original also Kandidat.
 *   - Der Port sperrte ihn trotzdem, weil `grid_id |= 0x80` in EXEC[5] P0
 *     (enemy_ai_re2_zombie.c, "PORT-MAPPING Review #18" — RE2 kennt kein +0x9-Downed-Bit)
 *     zusammen mit +0x21A & 0x2 als "liegend" galt und das LEVEL-Band aus dem RE1.5-Stempel
 *     strich (re15_damage.c).
 *
 * DER MARKER, an dem der Fix haengt: RE2 setzt +0x21A |= 0x10 exakt zu Beginn beider
 * Boden-Aufsteher und loescht es an deren Ende —
 *     EXEC[5] P6  @0x80103588 `ori v0,v0,0x10`  / @0x8010358c `sh v0,538(s2)`
 *             P7  @0x801036b8 `andi v0,v0,0xffef` / @0x801036bc `sh v0,538(s2)`
 *     EXEC[8] P3  @0x80103d08 `ori v0,v0,0x10`  / @0x80103d10 `sh v0,538(s1)`
 *             P4  @0x80103d84 `andi v0,v0,0xffef` / @0x80103d8c `sh v0,538(s1)`
 * Der LIEGE-SPAWN EXEC[7] setzt es NIE: in dessen ganzer Region 0x80103780..0x80103c00 steht
 * kein einziger Store auf +0x21A (eigener Vollscan) — er bleibt byte-true gesperrt.
 *
 * DIE WACHE IST NICHT VAKUANT: sie schiesst den Zombie im ECHTEN game_step-Weg nieder, wartet
 * die Aufsteh-Phase ab und verlangt, dass DORT Treffer landen. Findet sie gar keine
 * Aufsteh-Frames, meldet sie FAIL (statt gruen bei 0 Faellen). Vor dem Fix: 0 Treffer.
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_inv_equipped_slot(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static long s_ems_sz = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void bringup(unsigned seed)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(seed);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

/* Ein Lauf: niederschiessen, Aufstehen abwarten, dort weiterschiessen.
 * Liefert (rise_frames, hits_while_rising) fuer diesen Seed. */
static void run(unsigned seed, int budget, int *out_rise, int *out_hits, int *out_ok)
{
    *out_rise = 0; *out_hits = 0; *out_ok = 0;
    bringup(seed);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);                   /* Pistole = Tabellenzeile 3 */
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 900; }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    if (!load_re2_bank(0x10)) return;
    int slot = standing_zombie();
    if (slot < 0) return;
    /* Nur EIN Ziel — sonst waehlt der Aufloeser einen Nachbarn und die Messung meint
     * einen anderen Aktor (genau die Falle, die test_re2_livepath 2026-08-27 gerissen hat). */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - 1500; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int hp_last = e->hp;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        e->hp  = (e->hp > 0 && e->hp < 400) ? 400 : e->hp;  /* am Leben halten: gemessen wird
                                                             * die TREFFBARKEIT, nicht der Tod */
        hp_last = e->hp;
        /* LEBENDE Aufsteh-Phase, nicht das klebrige Bit +0x21A & 0x10 (siehe re15_damage.c):
         *   EXEC[5] P6 @0x80103568 .. P7 @0x80103628 | EXEC[8] P3 @0x80103d00 .. P4 @0x80103d60 */
        int rising = (e->state == 1) &&
                     ((e->sub_state_1 == 5 && (e->sub_state_2 == 6 || e->sub_state_2 == 7)) ||
                      (e->sub_state_1 == 8 && (e->sub_state_2 == 3 || e->sub_state_2 == 4)));
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if (rising) {
            (*out_rise)++;
            if (e->hp < hp_last) (*out_hits)++;
        }
        if (getenv("RISE_DBG") && (f % 25) == 0)
            fprintf(stderr, "   f%-4d st=%u.%u 21A=%04X 10E=%04X grid=%02X hp=%d rising=%d\n",
                    f, e->state, e->sub_state_1, e->re2z_flags21a, e->re2z_f10e,
                    e->grid_id, e->hp, rising);
        *out_ok = 1;
    }
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== RE2-Boden-Aufsteher muss treffbar sein (FUN_800470C0 @0x80047124-64: kein Band) ===\n");

    int total_rise = 0, total_hits = 0, seeds_with_rise = 0, runs = 0;
    static const unsigned seeds[] = { 0x0badf00du, 0x12345678u, 0xdeadbeefu, 0x0000c0deu,
                                      0xa5a5a5a5u, 0x5eed0001u, 0x13579bdfu, 0x2468ace0u };
    for (unsigned i = 0; i < sizeof seeds / sizeof seeds[0]; i++) {
        int rise = 0, hits = 0, ok = 0;
        run(seeds[i], 900, &rise, &hits, &ok);
        if (!ok) continue;
        runs++;
        if (rise) seeds_with_rise++;
        total_rise += rise; total_hits += hits;
        printf("  Seed %08X: Aufsteh-Frames %3d, Treffer darin %2d\n", seeds[i], rise, hits);
    }

    printf("\n%d Laeufe, %d Aufsteh-Frames gesamt (%d Seeds), %d Treffer waehrend des Aufstehens.\n",
           runs, total_rise, seeds_with_rise, total_hits);

    /* --- Nicht-vakuant: es MUSS Aufsteh-Frames gegeben haben ------------------------------ */
    CHECK(runs >= 4, "%d Laeufe zustande gekommen (< 4 hiesse: RE2-Bank/Raum fehlt, Wache blind)",
          runs);
    CHECK(total_rise >= 500,
          "%d Aufsteh-Frames beobachtet (EXEC[5] P6/P7 @0x80103568/@0x80103628, EXEC[8] P3/P4 "
          "@0x80103d00/@0x80103d60; gemessen 2026-08-27: 4344). Zu wenige hiesse: die Wache "
          "misst nichts — dann Basis neu herleiten, nicht die Schranke senken",
          total_rise);

    /* --- Die eigentliche Zusicherung ------------------------------------------------------ */
    /* GEMESSENE TRENNUNG (2026-08-27, einvariabler A/B-Lauf, nur der Aufsteh-Zweig in
     * re15_damage.c an/aus):
     *     MIT Fix : 4344 Aufsteh-Frames, 400 Treffer
     *     OHNE Fix:  648 Aufsteh-Frames,   8 Treffer
     * ">0" waere hier KEINE Wache gewesen — auch ohne Fix kommen vereinzelt Treffer durch
     * (Zielhoehe zufaellig unten). Die Schranke liegt deshalb zwischen den beiden Messungen. */
    CHECK(total_hits >= 100,
          "%d Treffer waehrend des Aufstehens (mit Fix gemessen 400, ohne Fix 8). RE2s "
          "Kandidatenfilter FUN_800470C0 (@0x80047124-64) hat KEIN Band-Gate und +0x1D3 ist "
          "seit EXEC[5] P2 (`andi v1,v1,0x7f` @0x80103484 / `sb v1,467(s2)` @0x80103490) frei",
          total_hits);

    free(buf);
    if (fails) { printf("\nRE2Z RISE HITTABLE: FAIL (%d)\n", fails); return 1; }
    printf("\nRE2Z RISE HITTABLE: der Aufsteher nimmt Schaden\n");
    return 0;
}
