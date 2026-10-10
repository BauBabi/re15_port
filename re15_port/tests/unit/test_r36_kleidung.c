/* test_r36_kleidung.c — Runde 36 Spur B "kleidung": Leons zerrissene/blutige Kleidung
 * (Blut-Decal-Wundsystem, analysis/blood_decals.md; Dossier analysis/befunde_runde36/B_kleidung.md).
 *
 * Nutzer-Wortlaut: "Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr,
 * wenn er auf Warning/Danger runter kommt."
 *
 * Original-Mechanik (RE1.5 PSX.EXE, blood_decals.md §2/§3, selbst gegengelesen):
 *   FUN_80037edc(panel, amount): Akku += amount (@0x80037f04-08); ab 0x78=120 (@0x80037f14 sltiu)
 *   Stempel: Level++ (@0x80037f2c), Clamp 2 (@0x80037f40/50), DR_MOVE-Blit in die OT (@0x80037f7c-fc4).
 *   Trigger beim Zombie-Griff = Phase 4 der SPIELER-Opfer-FSM (cmd 5, 0x8010a2cc): Dispatcher
 *   @0x8010a580-5b0 `lbu aca59 / jalr 0x801201b8[dir]` -> Helper 0x8010a1cc (dir 0: (0,10),(5,50),(7,50))
 *   / 0x8010a208 (dir 1: (0,10),(4,50),(7,50)) / 0x8010a244 (dir 2: (1,50)) / 0x8010a268 (dir 3: (2,50)).
 *
 * Teile:
 *   griff <re2|re15> [budget]  — ROOM1140 (Briefing-Raum) auf dem ECHTEN game_step-Weg: Leon steht
 *                                neben einem Fresser, wird gegriffen, ueberlebt (HP wird aufgefuellt),
 *                                der Griff wird abgeschuettelt. Gemessen: Griffe, Abwuerfe, Wund-
 *                                Tabelle (Level/Akku) nach jedem Abwurf.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "re15_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_victim_state(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static int                s_fail = 0;

#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

/* RE1.5-Bank (EMD/CDEMD0.EMS des CD-Baums) wie probe_dogwound_zknock.c */
static uint8_t *s_blob15[64];
static void load_bank15(uint8_t type)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) { free(ems); return; }
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { free(ems); return; }
    uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, ems + off, len); free(ems);
    if (type < 64) { free(s_blob15[type]); s_blob15[type] = blob; }
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    /* Opfer-Bank (Bank 2 = Leons Griff-Clips) wie der Plattform-Lader (main.c:1278) */
    eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int is_zombie(uint8_t t) { return t == 0x10 || t == 0x11 || t == 0x16; }

static void print_wounds(const char *tag)
{
    printf("%s wunden:", tag);
    for (int p = 0; p < 8; p++) printf(" p%d=L%d", p, re15_wound_level(p));
    printf("  gen=%d\n", re15_wound_generation());
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_griff(const char *flavor, int budget)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int re2 = (flavor && flavor[0] == 'r' && flavor[2] == '2');
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_wound_reset();
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (re2) {
        load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        if (!eb || !eb->ok) { printf("SKIP: RE2-Bank EM016 fehlt\n"); return 77; }
    } else {
        load_bank15(0x10); load_bank15(0x11); load_bank15(0x16);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();

    int zs[RE15_ACTOR_MAX], nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && is_zombie(g_actors[s].type)) zs[nz++] = s;
    if (nz == 0) { printf("FAIL: keine Zombies in ROOM1140 gespawnt\n"); return 1; }
    /* Greifer: der erste STEHENDE Zombie (Typ 0x10/0x11; Slot 1 ist der Liegende 0x16 des
     * Deskriptors 0x88, der im RE1.5-Flavor vom Boden aus beisst und frisst) */
    int zA = -1;
    for (int i = 0; i < nz; i++) {
        re15_actor_t *e = &g_actors[zs[i]];
        printf("  Zombie Slot %d Typ 0x%02X state %d sub %d pos (%d,%d,%d)\n", zs[i], e->type,
               e->state, e->sub_state_1, (int)e->x, (int)e->y, (int)e->z);
        if (zA < 0 && e->type != 0x16) zA = zs[i];
    }
    if (zA < 0) zA = zs[0];
    /* alle anderen Zombies schlafen legen, damit genau EIN Greifer misst */
    for (int i = 0; i < nz; i++) if (zs[i] != zA) g_actors[zs[i]].active = 0;
    re15_actor_t *z0 = &g_actors[zA];
    printf("ROOM1140 %s: %d Zombies, Greifer Slot %d Typ 0x%02X (state %d sub %d)\n",
           re2 ? "RE2" : "RE1.5", nz, zA, z0->type, z0->state, z0->sub_state_1);

    int grabs = 0, releases = 0, prev_v = 0, prev_s1 = -1, first_grab = -1;
    int min_hp = 100;
    for (int f = 0; f < budget; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) {               /* frei: neben den Greifer stellen */
            pl->x = z0->x + 400; pl->z = z0->z; pl->y = z0->y; pl->floor = z0->floor;
            pl->rot_y = z0->rot_y;                            /* Ruecken zum Zombie -> Griff von hinten/vorn je Bahn */
        }
        if (pl->hp < min_hp) min_hp = pl->hp;
        if (pl->hp < 60) pl->hp = 100;                        /* ueberleben: kein Devour, kein One-Save */
        /* Befreiung wie der Nutzer: Knopf-Hammern im Griff (Kante alle 2 Takte; RE1.5 +0x9c -= 1+5*mash
         * @FUN_80037024, RE2 +0x158 -= 2+5*mash @0x80102868-7C) */
        uint16_t edge = (v == 1 && (f & 1)) ? (uint16_t)RE15_PAD_BIT_CROSS : 0u;
        frame(edge, edge);
        v = re15_player_victim_state();
        if (getenv("R36B_TRACE")) {
            static int lv = -1, ls = -1, lhp = -999, lm = -1;
            if (v != lv || pl->state != ls || pl->hp != lhp || pl->motion != lm) {
                printf("    F%d victim=%d pl.state=%d sub=%d hp=%d motion=%d hit_react=%02x | z0 state=%d sub=%d/%d\n",
                       f, v, pl->state, pl->sub_state_1, pl->hp, pl->motion, pl->hit_react,
                       z0->state, z0->sub_state_1, z0->sub_state_2);
                lv = v; ls = pl->state; lhp = pl->hp; lm = pl->motion;
            }
        }
        if (z0->state == 1 && z0->sub_state_1 == 3 && prev_s1 != 3) { grabs++; if (first_grab < 0) first_grab = f; }
        prev_s1 = (z0->state == 1) ? (int)z0->sub_state_1 : -1;
        if (v == 3 && prev_v != 3) {
            releases++;
            char tag[64]; snprintf(tag, sizeof tag, "  F%d Abwurf #%d (hp=%d)", f, releases, pl->hp);
            print_wounds(tag);
        }
        prev_v = v;
        if (releases >= 4) break;
    }
    printf("ERGEBNIS %s: Griffe=%d Abwuerfe=%d erster Griff F%d min_hp=%d\n",
           re2 ? "RE2" : "RE1.5", grabs, releases, first_grab, min_hp);
    print_wounds("  Ende");
    int any = 0; for (int p = 0; p < 8; p++) if (re15_wound_level(p) > 0) any = 1;
    printf("  Stempel sichtbar (ein Panel Level>0): %s\n", any ? "JA" : "NEIN");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "griff";
    if (strcmp(teil, "griff") == 0) {
        const char *flavor = (argc > 2) ? argv[2] : "re2";
        int budget = (argc > 3) ? atoi(argv[3]) : 3000;
        int rc = teil_griff(flavor, budget);
        if (rc == 77) return 77;
        printf("test_r36_kleidung griff: %s\n", s_fail ? "FAILURES" : "OK");
        return s_fail ? 1 : 0;
    }
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
