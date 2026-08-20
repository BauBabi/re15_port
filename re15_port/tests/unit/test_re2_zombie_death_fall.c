/**
 * @file test_re2_zombie_death_fall.c
 * @brief PIN — die STURZ-KETTE des RE2-Zombie-Todes (DEATH @0x80108250 -> FUN_80108530).
 *
 * ⛔ NUTZER-REPORT 2026-08-20 (v0.3.4, beide RE2-Modi): "Die Zombies fallen manchmal ganz komisch
 * nach vorne, nachdem sie frontal mit Kugel getroffen wurden, und liegen merkwuerdig abrupt am
 * Boden auf dem Bauch mit Sterbe-Animation, ohne richtigen Uebergang."
 *
 * GEMESSEN VORHER (probe_re2_zfall 4 3, ROOM1140, echter Weg game_step + R1/SQUARE, GELADENE
 * RE2-Bank EM010; Brustknochen 8 relativ zum Boden):
 *   f160 st=1 clip=0  af= 0  b8dy=-2766   <- STEHT
 *   f165 st=3 clip=7  af= 0  b8dy= -548   <- EIN Frame spaeter LIEGT er: Sprung von 2218 Einheiten
 *   f234 st=7 clip=7  af=69  b8dy= -165
 *   f235      clip=22 af= 0  b8dy= -129   <- Leiche schnappt in die BAUCH-Pose
 * NACHHER (derselbe Lauf):
 *   f165 st=3 clip=1  af= 0  b8dy=-2759   <- der Sturz BEGINNT im Stand (Sprung 7 statt 2218)
 *   f175 af=10 -2329 / f180 af=15 -1690 / f201 af=36 -1283 / f211 af=46 -87
 *   f224 s2=2 / f225 st=7 / f226 clip=23 b8dy=-150  <- Leiche auf DERSELBEN Hoehe, kein Sprung
 * VORWAERTS-Fall (probe_re2_zfall 4 1, +0x16A = 1):
 *   f823 st=3 clip=2 af=10 b8dy=-2736 -> f828 af=15 -2660 -> f836 af=23 -2176 -> f838 af=25 -1882
 *   f872 s2=2 / f873 st=7 / f874 clip=22 (Bauch)  <- Pose passt zur gewaehlten Richtung
 *
 * URSACHE (selbst disassembliert, EMOVL10_S0.BIN): die DEATH-Wurzel FUN_80108250 hat VIER Zweige,
 * der Port kannte nur EINEN — und zwar den falschen:
 *   @0x801083B0-E0  `lhu 270 / andi 0x1 / beq` -> 1D-Tabelle @0x8010CECC[+0x5] = FUN_80108A14.
 *                   DAS ist der Clip-7-Handler (`lui v0,0xf / ori 0x7 / sw v0,332` @0x80108A60-88),
 *                   und er gilt NUR fuer den wiederbelebten Kriecher (+0x10E Bit 0 wird
 *                   ausschliesslich in der Wiederbelebung gesetzt, `sh 0x2001,270` @0x801089B0).
 *   @0x801083E4-404 `andi 0x10` -> FUN_801099E4 (Kriecher)
 *   @0x80108408-DC  `andi 0x2`  -> Liegend-Leiter + `sw 0x907,4` @0x801084D8-DC
 *   @0x801084E0-518 SONST 2D-Dispatch tbl[0x8010CC24][+0x5 *36][+0x1D2 *4], `jalr v0` @0x80108514
 * Der normale Stand-Tod landet in FUN_80108530, und dessen Phase 0 zieht die RICHTUNG:
 *   `rand & 1 -> +0x16A`                       @0x801085A4/B0/BC
 *   `+0x21A & 0x2000 -> +0x16A = 0`            @0x801085B4-C0
 *   `+0x21A &= ~0x4; +0x16A ? |= 0x4`          @0x801085CC-DC
 *   Clip-Wort ((+0x16A*5)<<9) + 0xF0000 + {1,2}[+0x16A]   @0x8010861C-30
 *        dir 0 -> Clip 1 ab Frame  0 (Sturz nach HINTEN)
 *        dir 1 -> Clip 2 ab Frame 10 (Sturz nach VORNE)
 *   Phase 1 `+0x6 += 959c(...,256)`            @0x801088E4-FC
 *   Phase 2 `sw 7,4(s1)` = CORPSE (Sub 0)      @0x8010891C (Delay-Slot, v0 = 7 @0x80108598)
 * und genau `+0x21A & 0x4` waehlt spaeter die LEICHEN-POSE (`? 22 : 23` @0x8010A490-BC). Der Port
 * setzte das Bit im Tod nie, sondern liess den Rest eines frueheren Knockdowns stehen — deshalb
 * die Bauch-Leiche nach einer Ruecken-Animation.
 *
 * GEPINNT WIRD DER VERLAUF, NICHT DER ENDZUSTAND: jeder Frame der Sturzsequenz wird
 * mitgeschrieben (Clip, anim_frame, Phase +0x6, Weltposition des Brustknochens) und geprueft auf
 * (1) kein Positions-SPRUNG beim Eintritt in DEATH, (2) streng monoton wachsender Keyframe ueber
 * die ganze Sequenz, (3) ein echter Abstieg von Stand- auf Bodenhoehe in kleinen Schritten,
 * (4) kein Positions-SPRUNG an der Naht DEATH->CORPSE, (5) Leichen-Pose == gewaehlte Richtung.
 * Ein Pin, der nur den Endzustand prueft, haette genau diesen Fehler wieder nicht gefangen.
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
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
extern int  re15_re2z_last_death_handler(void);
extern int  re15_re2z_death_cell(unsigned row, unsigned col);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

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

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

/* ---- EIN Todesverlauf, Frame fuer Frame mitgeschrieben ------------------------------------- */
#define TRACE_MAX 400
typedef struct {
    int  n;
    int  state[TRACE_MAX];
    int  sub2 [TRACE_MAX];
    int  clip [TRACE_MAX];
    int  frame[TRACE_MAX];
    int32_t dy[TRACE_MAX];        /* Brustknochen 8 relativ zum Boden (negativ = ueber Boden) */
    int32_t dy_before;            /* Hoehe im Frame VOR dem Todes-Tick */
    int  dir;                     /* +0x16A der Sturz-Phase */
    int  flags21a;
    int  handler;                 /* re15_re2z_last_death_handler() im ersten DEATH-Frame */
    int  corpse_clip;
    int32_t corpse_dy;
    int  ok;                      /* Lauf ueberhaupt zustande gekommen? */
} death_trace_t;

/* Faehrt den ECHTEN Weg: R1 halten, SQUARE druecken, bis der Zombie tot ist, und danach noch
 * bis in den CORPSE-Zustand hinein. Der Zombie wird VORHER auf "steht, volle HP" zurueckgesetzt,
 * damit mehrere Laeufe im selben Raum moeglich sind (nur die RNG-Folge unterscheidet sie —
 * die Richtung ist `rand & 1` @0x801085B0, also braucht der Pin mehrere Laeufe fuer beide Seiten). */
static void run_death(int slot, int budget, death_trace_t *t, int force_crawler_branch)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    memset(t, 0, sizeof *t);

    e->hp = 79; e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->re2z_flags21a = 0; e->re2z_f10e = force_crawler_branch ? 1u : 0u;
    e->re2z_self1d3 = 0; e->re2z_flag222 = 0; e->re2z_hits1d2 = 1;
    e->re2z_dir16a = 0; e->re2z_gaitrow = 0; e->hit_react = 0;
    e->grid_id = (uint8_t)(e->grid_id & ~0x80u);
    e->motion = 0; e->anim_frame = 0; e->anim_freeze = 0;

    int32_t prev[3] = { 0, 0, 0 };
    re15_enemy_bone_world_pos(e, 8, prev);
    t->dy_before = prev[1] - e->y;

    int dead_seen = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE;
        if (!dead_seen) { t->dy_before = prev[1] - e->y; }
        frame(cur, RE15_PAD_BIT_SQUARE);
        int32_t bp[3]; re15_enemy_bone_world_pos(e, 8, bp);

        if (e->state == 3) {
            if (!dead_seen) {
                dead_seen = 1;
                t->handler  = re15_re2z_last_death_handler();
                t->dir      = (int)e->re2z_dir16a;
                t->flags21a = (int)e->re2z_flags21a;
            }
            if (t->n < TRACE_MAX) {
                t->state[t->n] = e->state; t->sub2[t->n] = e->sub_state_2;
                t->clip[t->n]  = (int)e->motion; t->frame[t->n] = (int)e->anim_frame;
                t->dy[t->n]    = bp[1] - e->y; t->n++;
            }
        } else if (dead_seen && e->state == 7) {
            /* erster CORPSE-Frame MIT gesetzter Pose (der Sub-0-Init laeuft einen Tick) */
            if (e->sub_state_1 == 1) {
                t->corpse_clip = (int)e->motion; t->corpse_dy = bp[1] - e->y; t->ok = 1;
                return;
            }
        }
        prev[0] = bp[0]; prev[1] = bp[1]; prev[2] = bp[2];
    }
}

/* Der PIN-Praedikat-Kern, damit der Negativ-Test EXAKT dasselbe misst wie der Positiv-Test.
 * Rueckgabe 0 = alles gut, sonst eine Bitmaske der verletzten Bedingungen. */
enum {
    P_ENTRY_SNAP  = 1,   /* Positions-Sprung beim Eintritt in DEATH                */
    P_NOT_FALLCLIP= 2,   /* Sturz-Clip ist nicht 1/2 (bzw. Startframe falsch)      */
    P_FRAME_STUCK = 4,   /* Keyframe waechst nicht monoton ueber die Sequenz       */
    P_TOO_SHORT   = 8,   /* Sequenz zu kurz fuer einen echten Sturz                */
    P_NO_DESCENT  = 16,  /* kein Abstieg von Stand- auf Bodenhoehe                 */
    P_BIG_STEP    = 32,  /* Teleport-Schritt innerhalb der Sequenz                 */
    P_HANDOFF     = 64,  /* Positions-Sprung an der Naht DEATH -> CORPSE           */
    P_CORPSE_POSE = 128  /* Leichen-Pose passt nicht zur gewaehlten Richtung       */
};
#define ENTRY_TOL 400    /* gemessen: 7 Einheiten (neu) gegen 2218 (alt)            */
#define STEP_TOL  700    /* groesster gemessener Einzelschritt im Sturz: ~362       */

static int pin_mask(const death_trace_t *t)
{
    int m = 0;
    if (!t->ok || t->n < 2) return 0xFFFF;

    int32_t d0 = t->dy[0];
    int32_t j  = d0 - t->dy_before; if (j < 0) j = -j;
    if (j > ENTRY_TOL) m |= P_ENTRY_SNAP;

    if (!(t->clip[0] == 1 || t->clip[0] == 2)) m |= P_NOT_FALLCLIP;
    else if (t->frame[0] != ((t->clip[0] == 2) ? 10 : 0)) m |= P_NOT_FALLCLIP;

    int fall = 0;
    for (int i = 1; i < t->n; i++) {
        if (t->clip[i] != t->clip[0]) break;
        if (t->frame[i] <= t->frame[i - 1]) { m |= P_FRAME_STUCK; break; }
        int32_t s = t->dy[i] - t->dy[i - 1]; if (s < 0) s = -s;
        if (s > STEP_TOL) m |= P_BIG_STEP;
        fall = i;
    }
    if (fall < 40) m |= P_TOO_SHORT;

    int32_t hi = t->dy[0];  if (hi < 0) hi = -hi;      /* Stand-Hoehe */
    int32_t lo = t->dy[fall]; if (lo < 0) lo = -lo;    /* Boden-Hoehe */
    if (!(hi > 2000 && lo < 600)) m |= P_NO_DESCENT;

    int32_t hd = t->corpse_dy - t->dy[t->n - 1]; if (hd < 0) hd = -hd;
    if (hd > ENTRY_TOL) m |= P_HANDOFF;

    if (t->corpse_clip != ((t->flags21a & 0x4) ? 22 : 23)) m |= P_CORPSE_POSE;
    return m;
}

static void dump(const char *tag, const death_trace_t *t)
{
    printf("  %-10s dir=%d 21A=%04X handler=%d vor=%6d | ", tag, t->dir, t->flags21a,
           t->handler, (int)t->dy_before);
    for (int i = 0; i < t->n && i < 10; i++)
        printf("[c%d f%d %d]", t->clip[i], t->frame[i], (int)t->dy[i]);
    if (t->n > 10) printf(" ... [c%d f%d %d]", t->clip[t->n-1], t->frame[t->n-1],
                          (int)t->dy[t->n-1]);
    printf(" -> Leiche c%d %d\n", t->corpse_clip, (int)t->corpse_dy);
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    /* ---- TABELLEN-PIN: die Zellen gegen den eigenen Dump ------------------------------------
     * `re2_disasm.py table 0x8010cc24 170 --bin EMOVL10_S0.BIN`, Zeile r ab 0x8010CC24 + r*36.
     * 1 = FUN_80108530 (Sturz), 2 = FUN_80107438, 3 = FUN_801092C4, 4 = FUN_801066FC,
     * 5 = FUN_80108BEC, 6 = FUN_80109610, 0 = NULL. */
    {   const struct { unsigned r, c; int want; const char *addr; } cells[] = {
            { 3, 1, 1, "@0x8010CC94 Browning HP / Basis-Zone -> FUN_80108530" },
            { 1, 0, 1, "@0x8010CC48" }, { 4, 7, 1, "@0x8010CCC0" },
            { 1, 2, 0, "@0x8010CC50 NULL" }, { 3, 5, 0, "@0x8010CCA4 NULL" },
            { 5, 0, 2, "@0x8010CCD8 -> FUN_80107438" },
            { 5, 1, 3, "@0x8010CCDC -> FUN_801092C4" },
            { 7, 1, 4, "@0x8010CD24 -> FUN_801066FC" },
            { 8, 1, 5, "@0x8010CD48 -> FUN_80108BEC" },
            { 8, 4, 6, "@0x8010CD54 -> FUN_80109610" },
            { 9, 2, 5, "@0x8010CD70 -> FUN_80108BEC" },
            { 10, 8, 1, "@0x8010CDAC" }, { 12, 1, 6, "@0x8010CDD8 -> FUN_80109610" },
            { 17, 4, 6, "@0x8010CE98 -> FUN_80109610" },
            { 18, 8, 0, "@0x8010CECC = 1D-Tabelle[0] = NULL" },
        };
        for (unsigned i = 0; i < sizeof cells / sizeof cells[0]; i++)
            CHECK(re15_re2z_death_cell(cells[i].r, cells[i].c) == cells[i].want,
                  "DEATH-Tabelle [%u][%u] = %d, erwartet %d (%s)",
                  cells[i].r, cells[i].c, re15_re2z_death_cell(cells[i].r, cells[i].c),
                  cells[i].want, cells[i].addr);
    }

    /* ---- RE1.5-REGRESSIONSWACHE: im RE1.5-Flavor darf die RE2-DEATH-Maschine NIE laufen ------
     * Muss VOR dem RE2-Teil stehen: die Diagnose startet bei 0 und wird nur von re2z_death
     * geschrieben. */
    {   re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        bringup();
        re15_inv_load_briefing();
        re15_player_set_equipped_weapon(3);
        {   int es = re15_inv_equipped_slot();
            if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
        for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
        int slot = standing_zombie();
        CHECK(slot > 0, "RE1.5-WACHE: kein stehender Zombie in ROOM1140");
        if (slot > 0) {
            for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
            pl->rot_y = 0;                       /* Blickrichtung +X, wie in der Schwester-Sonde */
            e->hp = 1; e->hit_react = 0;         /* der naechste Treffer ist der Todesschuss */
            int fired = re15_player_weapon_fire(3);   /* ECHTER Waffenpfad */
            CHECK(fired, "RE1.5-WACHE: der Todesschuss hat nicht getroffen");
            CHECK(e->hp < 0, "RE1.5-WACHE: der RE1.5-Zombie ist nicht gestorben (hp=%d)", e->hp);
            for (int f = 0; f < 300; f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
        }
        CHECK(re15_re2z_last_death_handler() == 0,
              "RE1.5-WACHE: die RE2-DEATH-Wurzel (0x80108250) hat im RE1.5-Flavor dispatcht (%d)",
              re15_re2z_last_death_handler());
    }

    /* ---- der RE2-Teil ---------------------------------------------------------------------- */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);                 /* Browning HP -> Zeile 3 */
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    CHECK(load_re2_bank(0x10), "RE2-Bank EM010 fehlt — ohne Bank ist clip_len 0 und der Pin blind");

    int slot = standing_zombie();
    CHECK(slot > 0, "kein stehender Zombie in ROOM1140");
    if (slot <= 0) { printf("test_re2_zombie_death_fall: %d FAIL\n", ++fails); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    {   re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *e  = &g_actors[slot];
        pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100;
            frame(RE15_PAD_BIT_R1, 0); }
        /* Ohne geladene Bank waere re15_actor_clip_len() == 0 und jede clip-getriebene Phase
         * endete im Setz-Tick — der Pin waere blind. Deshalb hart nachgemessen: */
        {   re15_actor_t probe = *e; probe.motion = 1;
            int l1 = re15_actor_clip_len(&probe);
            probe.motion = 2; int l2 = re15_actor_clip_len(&probe);
            CHECK(l1 > 40 && l2 > 40,
                  "Sturz-Clips 1/2 haben Laenge %d/%d — ohne echte Bank ist der Pin blind",
                  l1, l2);
        }
    }

    /* ---- POSITIV: der VERLAUF, fuer BEIDE Richtungen ---------------------------------------
     * Die Richtung ist `rand & 1` (@0x801085B0). Mehrere Laeufe hintereinander liefern beide
     * Seiten; gefordert ist, dass BEIDE vorkommen UND beide den Pin bestehen. */
    {   int seen[2] = { 0, 0 }; int runs = 0;
        printf("Todesverlaeufe (Brustknochen 8 relativ zum Boden):\n");
        for (int r = 0; r < 10 && (!seen[0] || !seen[1]); r++) {
            death_trace_t t;
            run_death(slot, 600, &t, 0);
            if (!t.ok) continue;
            runs++;
            dump(t.dir ? "VORWAERTS" : "RUECKWAERTS", &t);
            int m = pin_mask(&t);
            CHECK(t.handler == 1,
                  "%s: DEATH-Zelle [3][1] muss FUN_80108530 (=1) sein, war %d "
                  "(@0x8010CC94 / Dispatch @0x801084E0-518)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", t.handler);
            CHECK((m & P_NOT_FALLCLIP) == 0,
                  "%s: Sturz-Clip/Startframe falsch — c%d f%d, erwartet c%d f%d "
                  "(Clip-Wort @0x8010861C-30)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", t.clip[0], t.frame[0],
                  t.dir ? 2 : 1, t.dir ? 10 : 0);
            CHECK((m & P_ENTRY_SNAP) == 0,
                  "%s: SPRUNG beim Eintritt in DEATH — vorher %d, erster Todes-Frame %d "
                  "(genau der gemeldete Fehler: 2218 Einheiten in EINEM Frame)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", (int)t.dy_before, (int)t.dy[0]);
            CHECK((m & P_FRAME_STUCK) == 0,
                  "%s: der Keyframe waechst nicht monoton ueber die Sturzsequenz",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS");
            CHECK((m & P_TOO_SHORT) == 0,
                  "%s: Sturzsequenz zu kurz (%d Frames) — Clip 1/2 haben 60 Frames",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", t.n);
            CHECK((m & P_NO_DESCENT) == 0,
                  "%s: kein Abstieg Stand->Boden (Start %d, Ende %d)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", (int)t.dy[0], (int)t.dy[t.n - 1]);
            CHECK((m & P_BIG_STEP) == 0,
                  "%s: Teleport-Schritt (> %d) innerhalb der Sturzsequenz",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", STEP_TOL);
            CHECK((m & P_HANDOFF) == 0,
                  "%s: SPRUNG an der Naht DEATH->CORPSE — letzter Sturz-Frame %d, Leiche %d",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", (int)t.dy[t.n - 1], (int)t.corpse_dy);
            CHECK((m & P_CORPSE_POSE) == 0,
                  "%s: Leichen-Pose %d passt nicht zu +0x21A&4=%d (erwartet %d, @0x8010A490-BC)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", t.corpse_clip, (t.flags21a & 4) ? 1 : 0,
                  (t.flags21a & 4) ? 22 : 23);
            CHECK(((t.flags21a & 0x4) != 0) == (t.dir != 0),
                  "%s: +0x21A Bit 0x4 (%d) muss der Richtung +0x16A (%d) folgen "
                  "(@0x801085CC-DC)",
                  t.dir ? "VORWAERTS" : "RUECKWAERTS", (t.flags21a & 4) ? 1 : 0, t.dir);
            seen[t.dir ? 1 : 0] = 1;
        }
        CHECK(runs > 0, "kein einziger Todesverlauf zustande gekommen");
        CHECK(seen[0], "der RUECKWAERTS-Fall (Clip 1) kam in 10 Laeufen nicht vor "
                       "— `rand & 1` @0x801085B0 haengt");
        CHECK(seen[1], "der VORWAERTS-Fall (Clip 2 ab Frame 10) kam in 10 Laeufen nicht vor "
                       "— genau der vom Nutzer gemeldete Fall waere ungetestet");
    }

    /* ---- NEGATIV-KONTROLLE: die ALTE Kette muss durchfallen ---------------------------------
     * `+0x10E & 1` schickt die Wurzel in die 1D-Tabelle @0x8010CECC -> FUN_80108A14 = Clip 7,
     * also GENAU in das Verhalten, das der Port bisher fuer JEDEN Tod gefahren hat. Der Pin muss
     * es als Sprung erkennen — sonst wuerde er den gemeldeten Fehler wieder durchlassen. */
    {   death_trace_t t;
        run_death(slot, 600, &t, 1);
        CHECK(t.ok, "NEGATIV-KONTROLLE: der Clip-7-Zweig kam nicht bis CORPSE");
        if (t.ok) {
            dump("NEG(clip7)", &t);
            int m = pin_mask(&t);
            CHECK(t.clip[0] == 7,
                  "NEGATIV-KONTROLLE: `+0x10E & 1` muss FUN_80108A14 (Clip 7 @0x80108A88) fahren, "
                  "war Clip %d", t.clip[0]);
            CHECK(t.handler == -1,
                  "NEGATIV-KONTROLLE: die Diagnose muss den 1D-Zweig (-1) melden, war %d",
                  t.handler);
            CHECK((m & (P_ENTRY_SNAP | P_NOT_FALLCLIP)) == (P_ENTRY_SNAP | P_NOT_FALLCLIP),
                  "NEGATIV-KONTROLLE: der Pin haette die alte Clip-7-Kette NICHT gefangen "
                  "(Maske 0x%02X, erwartet mindestens Sprung+falscher Clip) — vorher %d, "
                  "erster Todes-Frame %d", m, (int)t.dy_before, (int)t.dy[0]);
        }
    }

    free(buf);
    if (fails == 0) printf("test_re2_zombie_death_fall: OK\n");
    else            printf("test_re2_zombie_death_fall: %d FAIL\n", fails);
    return fails ? 1 : 0;
}
