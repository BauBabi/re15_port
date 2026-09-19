/**
 * @file probe_p3_restposten.c
 * @brief MESSSONDE + PIN (Phase 3, 2026-09-19, Thema re-restposten).
 *
 * Vier kleine Restposten aus den Dossiers vom 19.09., je ein Abschnitt:
 *
 *  [A] MESSER durch den RE2-Applier  (trefferhoehe.md §5, letzter Punkt)
 *      Das Messer (RE2-Item 1) hat als EINZIGE Waffe eine Record-REIHE: das Pattern
 *      @0x800A6434 / @0x800A6608 / @0x800A656C ist `ff/6 00/1 01/1 02/1 03/1 04/1 00/255`
 *      (eigener Dump, re15_port/tools/re2_gun_tables_dump.py). Gemessen wird, dass der Port
 *      die Teile-Maske jetzt auch fuer den Schlag fuehrt: EBEN waehlt Zeile 3 Satz 2
 *      `02 00 00` = nur Rumpf (Kriecher mit Maske 1 = NUR BEINE faellt heraus), TIEF Zeile 6
 *      `01 02 04` = Beine zuerst.
 *
 *  [B] HP-HALBIERUNG des Kriecher-INIT (kriecher-1010.md §6.6 "F3")
 *      `lhu 342; sra 1; sh 342` @0x80100B3C-5C im `+0x10E & 1`-Zweig des RE2-INIT.
 *      Gemessen ueber zwei INIT-Laeufe mit demselben RNG-Seed: Deskriptor 0x81 gegen 0x85.
 *
 *  [C] AUFSTEH-SPERRE nach dem Skript-Bump (liegende-zombies.md §5 Punkt 2)
 *      RE1.5 loescht seinen Riegel erst in Liege-Phase 3 (`andi 0xfe / sb 147` @0x80103B64-68),
 *      RE2 erst am Pose-Ausgang (EXEC[7] P4 `andi 0x7f` @0x80103914-18 / EXEC[8] P3
 *      @0x80103CE4-FC). Der Port gab den Gebumpten frueher im Bump-Bild frei.
 *
 *  [D] NIBBLE 4 / 5  (liegende-zombies.md §5 Punkt 1)
 *      In ALLEN FUENF STAGE-Overlays laden die Dispatcher fuer Nibble 4 und fuer Nibble 5/6
 *      dieselbe DECIDE- und dieselbe ANIMATE-Zeile (eigener Dump der Tabellenpaare:
 *      STAGE1 0x8011F9C8/CC == 0x8011F9D0/D4 -> 0x80103980 / 0x80103A58; STAGE2
 *      0x80117ACC/D0 == 0x80117AD4/D8; STAGE3 0x8011DB18/1C == 0x8011DB20/24; STAGE4
 *      0x80118EF0/F4 == 0x80118EF8/FC; STAGE5 0x8011EA58/5C == 0x8011EA60/64).
 *      0x84 und 0x85 gehoeren also in dieselbe Familie wie 0x86 — der Fresser.
 *
 * Aufruf: probe_p3_restposten [pin]
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

extern void re15_player_set_aim_elevation_for_test(int elev);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_pin  = 0;
static int                s_fail = 0;

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

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
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

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static void expect(int cond, const char *what)
{
    if (!cond) { printf("  FAIL: %s\n", what); s_fail++; }
    else         printf("  ok:   %s\n", what);
}

static int find_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type >= 0x10 && e->type <= 0x18 && !(e->grid_id & 0x80)) return s;
    }
    return -1;
}

/* ============================== [A] MESSER ============================================== */
/* Ein Schlag-Bild: Spieler in Distanz d vor dem Ziel, Zielhoehe elev, Spieler-Bild `af`
 * (= Pattern-Schritt af-6). Danach wird der Aktor byte-genau zurueckgesetzt. */
static int knife_shot(int slot, int elev, int dist, int af, int *zone, int *dhp)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t esave = *e, psave = *pl;
    pl->x = e->x + dist; pl->z = e->z; pl->y = e->y; pl->floor = e->floor;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    pl->hp = 100; pl->anim_frame = (uint8_t)af;
    re15_player_set_aim_elevation_for_test(elev);
    int16_t hp0 = e->hp;
    int ret = re15_player_weapon_fire(1);          /* w1 = Combat Knife -> RE2-Id 1 */
    *zone = e->re2z_hits1d2 % 3;
    *dhp  = (int)(hp0 - e->hp);
    *e = esave; *pl = psave;
    return ret != 0;
}

static void knife_row(const char *lage, int slot, int elev, int af)
{
    static const int D[6] = { 400, 900, 1400, 2000, 2600, 3400 };
    static const char *EL[3] = { "TIEF", "EBEN", "HOCH" };
    char buf[160]; int n = 0;
    for (int i = 0; i < 6; i++) {
        int z = 0, dhp = 0;
        int hit = knife_shot(slot, elev, D[i], af, &z, &dhp);
        n += snprintf(buf + n, sizeof buf - (size_t)n, " d%4d:%s", D[i],
                      hit ? (z == 0 ? "Bein" : z == 1 ? "Rumpf" : "Kopf") : "----");
    }
    printf("  %-8s %-4s Bild %2d (Schritt %d):%s\n", lage, EL[elev + 1], af,
           (af >= 6 && af <= 10) ? af - 6 : 0, buf);
}

static int knife_any(int slot, int elev, int af)
{
    static const int D[6] = { 400, 900, 1400, 2000, 2600, 3400 };
    for (int i = 0; i < 6; i++) { int z, d; if (knife_shot(slot, elev, D[i], af, &z, &d)) return 1; }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "pin") == 0) s_pin = 1;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    if (re15_inv_find_item(1) < 0) (void)re15_inv_grant(1, 1);
    re15_player_set_equipped_weapon(1);
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    {   re15_enemy_bank_t *b = load_re2_bank(0x10);
        printf("RE2-Bank EM010: %s\n", (b && b->ok) ? "geladen" : "FEHLT");
        if (!b || !b->ok) return 77; }

    int slot = find_zombie();
    if (slot < 0) { printf("FAIL: kein stehender Zombie in ROOM1140\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];

    /* ------------------------------------------------------------------ [A] MESSER */
    printf("\n=== [A] MESSER (RE2-Id 1) durch den Applier ===\n");
    /* Spawn-Pose (EXEC[8]) verlassen, sonst misst man die Pose. */
    pl->x = e->x + 1500; pl->z = e->z;
    for (int f = 0; f < 900; f++) {
        pl->hp = 100; frame(0, 0);
        if (e->state == 1 && e->sub_state_1 <= 3 && e->re2z_self1d3 == 0) break;
        if (f > 600) { e->re2z_self1d3 = 0; break; }
    }
    pl->x = e->x + 12000;
    e->x &= ~3; e->z &= ~3;
    e->hp = 4000; e->state = 1; e->sub_state_1 = 1; e->hit_react = 0;
    printf("  Zombie STEHEND: st=%d/%d Maske=%u rad9a=%u\n",
           e->state, e->sub_state_1, (unsigned)e->re2z_parts, (unsigned)e->re2z_rad9a);
    for (int af = 6; af <= 11; af++) knife_row("STEHEND", slot, 0, af);
    for (int af = 6; af <= 11; af++) knife_row("STEHEND", slot, -1, af);
    expect(knife_any(slot, 0, 7), "STEHEND: EBEN trifft (Maske 3 & Zeile 3 Satz2 `02 00 00`)");

    /* Kriecher von Hand aufsetzen (die Maske ist der Gegenstand der Messung, nicht der Weg
     * dorthin — den pinnt probe_r16_trefferhoehe ueber den echten Ragdoll-Pfad). */
    re15_actor_t stand = *e;
    e->re2z_parts = 1u; e->re2z_rad9a = 200u; e->re2z_f10e = 0x2001u;
    printf("  Zombie KRIECHER: Maske=%u rad9a=%u\n",
           (unsigned)e->re2z_parts, (unsigned)e->re2z_rad9a);
    for (int af = 6; af <= 11; af++) knife_row("KRIECHER", slot, 0, af);
    for (int af = 6; af <= 11; af++) knife_row("KRIECHER", slot, -1, af);
    {   int miss_eben = 1;
        for (int af = 6; af <= 11; af++) if (knife_any(slot, 0, af)) miss_eben = 0;
        expect(miss_eben, "KRIECHER: EBEN trifft in KEINEM Schlag-Bild "
                          "(Zeile 3 Satz 2 `02 00 00` & Maske 1 = 0)");
    }
    {   int hit_tief = 0, zone_ok = 1;
        for (int af = 6; af <= 11; af++) {
            int z = 0, d = 0;
            if (knife_shot(slot, -1, 900, af, &z, &d)) { hit_tief = 1; if (z != 0) zone_ok = 0; }
        }
        expect(hit_tief, "KRIECHER: TIEF trifft (Zeile 6 `01 02 04`, Beine zuerst)");
        expect(zone_ok,  "KRIECHER: TIEF stempelt Zone 0 (Beine)");
    }
    *e = stand;

    /* ------------------------------------------------------------------ [B] HP-HALBIERUNG */
    printf("\n=== [B] HP-Halbierung des Kriecher-INIT (@0x80100B3C-5C) ===\n");
    {   re15_actor_t base = *e;
        int hp_norm = 0, hp_crawl = 0;
        for (int pass = 0; pass < 2; pass++) {
            *e = base;
            re15_damage_seed_rng(0x0badf00du);
            e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
            e->grid_id = pass ? 0x81u : 0x85u;      /* 0x81 = Kriecher, 0x85 = Nicht-Kriecher */
            e->re2z_f10e = 0; e->re2z_flags21a = 0; e->re2z_self1d3 = 0;
            re15_re2z_tick(slot);
            if (pass) hp_crawl = e->hp; else hp_norm = e->hp;
        }
        printf("  Deskriptor 0x85 -> hp %d ; Deskriptor 0x81 (Kriecher) -> hp %d\n",
               hp_norm, hp_crawl);
        expect(hp_norm > 0 && hp_crawl == hp_norm / 2,
               "Kriecher-INIT halbiert die HP (`sra v0,v0,1 / sh v0,342` @0x80100B58-5C)");
        *e = base;
    }

    /* ------------------------------------------------------------------ [D] NIBBLE 4/5 */
    printf("\n=== [D] Deskriptoren 0x84 / 0x85 (Fresser-Familie) ===\n");
    {   re15_actor_t base = *e;
        static const uint8_t DESC[3] = { 0x84u, 0x85u, 0x86u };
        int sub[3];
        for (int i = 0; i < 3; i++) {
            *e = base;
            re15_damage_seed_rng(0x0badf00du);
            e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
            e->grid_id = DESC[i];
            e->re2z_f10e = 0; e->re2z_flags21a = 0; e->re2z_self1d3 = 0;
            re15_re2z_tick(slot);
            sub[i] = e->sub_state_1;
            printf("  0x%02X -> st=%d sub=%d/%d f10e=%04x clip=%d\n", DESC[i],
                   e->state, e->sub_state_1, e->sub_state_2, e->re2z_f10e, (int)e->motion);
        }
        expect(sub[0] == sub[2] && sub[1] == sub[2],
               "0x84 und 0x85 landen im selben Executor wie 0x86 (Tabellenpaare identisch "
               "in allen fuenf Overlays, s. Dateikopf)");
        *e = base;
    }

    /* ------------------------------------------------------------------ [C] AUFSTEH-SPERRE */
    printf("\n=== [C] Treffbarkeit erst am Pose-Ausgang (Skript-Bump 0x88 -> 0x89) ===\n");
    {   re15_actor_t base = *e;
        *e = base;
        re15_damage_seed_rng(0x0badf00du);
        e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->grid_id = 0x88u; e->re2z_f10e = 0; e->re2z_flags21a = 0; e->re2z_self1d3 = 0;
        pl->x = e->x + 12000;                                   /* weit weg: kein Naehe-Wecker */
        /* ⛔ UEBER DEN ECHTEN SCHRITT, nicht ueber den nackten KI-Tick: den Clip-Fortschritt
         * (+0x14D) treibt der Advancer im Game-Step VOR re15_enemy_ai_run_all
         * (game_step_common.c) — ein direkter re15_re2z_tick friert EXEC[7] P3 fuer immer ein
         * (gemessen: anim_frame blieb 400 Bilder auf 5). */
        for (int f = 0; f < 10; f++) { pl->hp = 100; frame(0, 0); }
        printf("  liegend: st=%d/%d/%d 1d3=%02x 10e=%04x +93=%02x\n", e->state,
               e->sub_state_1, e->sub_state_2, e->re2z_self1d3, e->re2z_f10e, e->hit_react);
        expect((e->hit_react & 1u) != 0u && (e->re2z_self1d3 & 0x80u) != 0u,
               "0x88 liegend: gesperrt (+0x1D3 |= 0x80 @0x80103804-14)");
        e->grid_id = 0x89u;                                     /* Member_set(12,0x89) @0x800411F8 */
        int frei_bei = -1, p4_bei = -1;
        for (int f = 0; f < 400; f++) {
            pl->hp = 100; frame(0, 0);
            if (f < 6 || (f % 40) == 0)
                printf("    f%-3d st=%d/%d/%d grid=%02x 10e=%04x 1d3=%02x +93=%02x clip=%d fr=%d\n",
                       f, e->state, e->sub_state_1, e->sub_state_2, e->grid_id,
                       e->re2z_f10e, e->re2z_self1d3, e->hit_react, (int)e->motion,
                       (int)e->anim_frame);
            if (frei_bei < 0 && !(e->hit_react & 1u)) frei_bei = f;
            if (p4_bei  < 0 && e->sub_state_1 != 7)   p4_bei  = f;
        }
        printf("  nach dem Bump: frei ab Bild %d ; Pose verlassen ab Bild %d ; Ende st=%d/%d 1d3=%02x\n",
               frei_bei, p4_bei, e->state, e->sub_state_1, e->re2z_self1d3);
        expect(frei_bei > 0, "Der Gebumpte ist NICHT schon im Bump-Bild treffbar");
        expect(p4_bei >= 0 && frei_bei >= p4_bei,
               "Freigabe erst mit dem Pose-Ausgang (EXEC[7] P4 `andi 0x7f` @0x80103914-18)");
        *e = base;
    }

    printf("\n%s: %d FAIL\n", "probe_p3_restposten", s_fail);
    if (s_pin) return s_fail ? 1 : 0;
    return 0;
}
