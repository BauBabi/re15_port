/**
 * @file probe_re2z_stomp.c
 * @brief REGRESSION — FINDING 4 (Nutzer-Report 2026-08-24, RE2-KI, ROOM1030):
 *        "fehlt der 'smash' sound, wenn ich einen am boden herankriechenden Zombie der mich
 *        beisst der Kopf zertrete. Ausserdem wird der Kopf nicht zertreten und der Zombie
 *        kriecht weiter."
 *
 * Es gibt in RE1.5 KEIN Spieler-Kommando "treten": die vollstaendige Aufzaehlung der
 * DAT_800ACA58-Schreiber ergibt {0,1,2,3,5,6,7} x Sub {0..13}, und Sub 7 (Zielen/Angriff)
 * ist ohne Waffe explizit gesperrt (`lbu v0,-13731(v0)` / `beq v0,zero,0x80032024`
 * @0x80032008-14). Was der Nutzer ausloest, ist die BEFREIUNG aus dem Bein-Biss des
 * Kriechers — eine reine GEGNER-Choreografie im RE2-Grab-EXEC[1] @0x801025EC, Phase 5:
 *
 *   80102a6c-80  +0x6 += Advance (Store im DELAY-SLOT der Weiche, laeuft in BEIDEN Zweigen)
 *   80102a78-7c  andi v0,s5,0x1 / beq v0,zero,0x80102bd0     ; s5&1 = KRIECHER
 *   80102ac0-c4  Zweig A: +0x14D == 22
 *   80102ad8-e4    lw v0,1376(v1) / ori v0,v0,0x4a / sw      ; PART 8 = KOPF ab
 *   80102b04       sw a1,1488(v1)                            ; Tint 0x00101040
 *   80102b08       sh v0,1528(v1)                            ; Kurs = rot_y
 *   80102b24-28    jal 0x8001bf10 (Id 0x00801000, Anker Part 0 +72, ofs (0,-600,0))
 *   80102b2c/58    addiu a0,zero,2 / jal 0x8005bd6c          ; >>> DER "SMASH"-SE <<<
 *   80102b40-58  Zweig B: +0x14D == 27 -> +0x21A |= 0x80, SE 8
 *   80102b70-c0  Frames 23..41, nur UNGERADE -> Hals-Stumpf (Id 4096, Anker Part 8 +72)
 *   80102bc4-c8  andi v0,s5,0x1 / bne v0,zero,0x80102ebc     ; KRIECHER: RAUS
 *   80102bd0-e4  (nur AUFRECHTE) +0x14D == 7 -> +0x6 = 7
 *   80102be8+    P6: sw 7,4(s1) / sh -1,342(s1) / +0x21A &= ~4 / word0-Gruppe / +0x1D3 |= 0x80
 *
 * IST-MESSUNG VOR DEM FIX (dieselbe Sonde, 1800 Frames): der Frame-7-Schnitt lief OHNE das
 * Tor @0x80102BC4-C8 auch auf dem Kriecher —
 *   f353 +0x6=5 frame=6 | f354 +0x6=7 frame=7 | f355..f368 +0x6=8 (Aufrecht-Erholpfad)
 *   +0x6 == 6: 0x ; state 7: 0x ; hp blieb 97 ; f554 greift wieder an
 * Alle drei Nutzer-Symptome haben damit EINE Ursache.
 *
 * Aufruf: probe_re2z_stomp [frames=1800] [nkriecher=2]
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

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static int                s_fail = 0;

#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); s_fail++; } } while (0)

/* SE-SPION: die Engine ruft den RE2-Enemy-SE-Hook; der Test haengt sich davor, damit die
 * Id 2 (@0x80102B2C) als ZAHL sichtbar ist und nicht nur als "klingt anders". */
static int s_se_seen[32];
static void spy_se(int se_id, int flag2000)
{
    (void)flag2000;
    if (se_id >= 0 && se_id < 32) s_se_seen[se_id]++;
}

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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

int main(int argc, char **argv)
{
    int budget = (argc > 1) ? atoi(argv[1]) : 1800;
    int ncrawl = (argc > 2) ? atoi(argv[2]) : 2;

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    memset(s_se_seen, 0, sizeof s_se_seen);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1030;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        if (!eb || !eb->ok) { printf("SKIP: RE2-Bank EM016 fehlt\n"); return 77; }
    }
    re15_re2z_audio_hook(spy_se, NULL);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();

    int cs[8], nc = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && nc < ncrawl; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) cs[nc++] = s;
    if (nc == 0) { printf("FAIL: keine RE2-Zombies in ROOM1030 gespawnt\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        int keep = 0;
        for (int i = 0; i < nc; i++) if (cs[i] == s) keep = 1;
        if (!keep && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
            g_actors[s].active = 0;
    }
    /* Kriecher erzwingen — byte-true der HURT-Eintritt @0x80107A54/@0x80107A58. */
    for (int i = 0; i < nc; i++) re15_re2z_enter_crawler(&g_actors[cs[i]], NULL, 0u);

    re15_actor_t *c0 = &g_actors[cs[0]];
    pl->x = c0->x + 700; pl->z = c0->z;
    pl->y = c0->y; pl->floor = c0->floor;
    printf("Kriecher: %d (Slots", nc);
    for (int i = 0; i < nc; i++) printf(" %d/typ0x%02X", cs[i], g_actors[cs[i]].type);
    printf(")\n");

    int p6_hits = 0, state7 = 0, headoff = 0, grabs = 0;
    int neck_spray = 0;
    int s1prev[8]; for (int i = 0; i < nc; i++) s1prev[i] = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;                                  /* der Spieler soll nicht sterben */
        frame(0, 0);
        for (int i = 0; i < nc; i++) {
            re15_actor_t *e = &g_actors[cs[i]];
            if (!e->active) continue;
            /* NUR im LEBENDEN Zustand zaehlen: die RE2-LEICHE (state 7, Settle-Maschine
             * @0x8010A440) rotiert +0x5 durch 1/2/3 — ein ungegateter Zaehler haelt das
             * faelschlich fuer neue Griffe (gemessen: 43 statt 2). */
            if (e->state == 1 && e->sub_state_1 == 1 && s1prev[i] != 1) grabs++;
            s1prev[i] = (e->state == 1) ? (int)e->sub_state_1 : -1;
            if (e->state == 1 && e->sub_state_1 == 1 && e->sub_state_2 == 6) p6_hits++;
            if (e->state == 7) state7++;
            if (e->re2z_part_flags[8] & 0x40u) headoff++;
        }
    }
    printf("ERGEBNIS: Griff-Eintritte=%d  P6-Ticks(+0x6==6)=%d  CORPSE-Ticks(state 7)=%d  "
           "Kopf-ab-Ticks(Part8 & 0x40)=%d\n", grabs, p6_hits, state7, headoff);
    printf("          SE-Zaehler: id2(smash)=%d id8=%d id3(Biss)=%d\n",
           s_se_seen[2], s_se_seen[8], s_se_seen[3]);
    (void)neck_spray;

    CHECK(grabs > 0, "kein Kriecher-Griff im echten ROOM1030-Durchlauf erreicht");
    CHECK(state7 > 0,
          "der abgeworfene Kriecher stirbt nicht — P6 @0x80102BE8 (`sw 7,4(s1)` / "
          "`sh -1,342(s1)`) wird nie erreicht, weil der Frame-7-Schnitt ohne das Tor "
          "@0x80102BC4-C8 auch auf ihm laeuft (\"der Zombie kriecht weiter\")");
    CHECK(headoff > 0,
          "Part 8 (Kopf) wird nie abgetrennt — @0x80102AD8-E4 `ori v0,v0,0x4a` auf Offset 1376 "
          "(\"der Kopf wird nicht zertreten\")");
    CHECK(s_se_seen[2] > 0,
          "der \"smash\"-SE fehlt — @0x80102B2C `addiu a0,zero,2` + @0x80102B58 "
          "`jal 0x8005bd6c`");

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re2z_audio_hook(NULL, NULL);
    printf("probe_re2z_stomp: %s\n", s_fail ? "FAILURES" : "OK");
    return s_fail ? 1 : 0;
}
