/**
 * @file test_r38_szenenschutz.c
 * @brief Runde 38 Spur B "szenenschutz" — Mechanik-Riegel ohne exe (Skill re15-room-probe).
 *
 * Nutzer (AUFTRAG.md 2026-10-16): "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal
 * bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei
 * den es ist Teil der Cutscene wie bei Elza Room 1170." Dossier analysis/befunde_runde38/B_szenenschutz.md.
 *
 *   Teil 1  Praedikat re15_szenenschutz_gegen/_haelt: freies Gehirn (+0x4 == 1) ja; Skript-Zustand (+0x4 != 1),
 *           NPC-Typ, Kraehe mit Skript-Marke grid&0x40 (INIT-Weiche @0x80112408) nein; Halten = player_mode 2.
 *   Teil 2  Haken-Folge direkt: Sicht nur waehrend des geschuetzten Ticks (+0x93 Bit 0 / PL+0x1D3&0x80), danach der
 *           echte Wert; Sicherheitsnetz gibt einen HP-Abzug zurueck; ein Szenen-Gegner (+0x4 = 4) trifft echt.
 *   Teil 3  ROOM1190, RE2-Hund: der Sprung-Ausgang schreibt +0x1d6 = 0x78 (@0x801115e8-ec), der erste Angriffs-
 *           Commit (Sub 3) kommt >= 120 Bilder nach der Landung (Abzug @0x8010dd5c-70, Tor @0x8010e578-80).
 *   Teil 4  ROOM1190, RE2-Hund, Sperrzeit genullt: player_mode 2 -> 400 Bilder kein HP-Verlust, kein Griff;
 *           player_mode 0 -> Biss (20 HP, FUN_800401D4 @0x80104EB8-C0).
 *   Teil 5  dasselbe mit dem RE1.5-Gehirn (Tor Spieler+0x93 @0x8010e5c4-cc / FUN_8010f15c).
 *   Teil 6  Gegenprobe ROOM1171 (Elza-Raum): die Szenen-Kraehe (grid 0x40 -> FLIGHT-2) greift den GEHALTENEN
 *           Spieler weiterhin (HOVER-Griff @0x80115254-0x801152c4, ohne +0x93-Tor) — "Teil der Cutscene".
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_szenenschutz_r38.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static void re2_hundebank(void)
{
    static uint8_t *ems = NULL; static long ems_sz = 0;
    if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (eb && re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}

/* ROOM1190 hochfahren wie test_re2_room1190_ab (main + sub00 + sub13 = Hunde-Spawns grid 0x40), dann NUR den ersten
 * Hund per grid 0x43 freigeben (Member_set(12,0x43) @0x027E6) und bis zur Landung (Ausgang 0x201 @0x8011162c)
 * ticken. Rueckgabe: Slot des Hundes, *t_land = Bild der Landung, *cd_land = +0x1d6 direkt nach der Landung. */
static int hund_1190(const re15_rdt_t *rdt, int *t_land, int *cd_land)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_scd.player_mode = 0;
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    if (rdt->sub_scd_count > 13 && rdt->sub_scd[13]) scd_thread_start(2, rdt->sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) return -1;
    frame();                                            /* INIT: grid 0x40 -> Zustand 4/Sub 0 (@0x8010db88-98) */
    re15_actor_t *d = &g_actors[slot];
    if (d->state != 4) return -1;
    d->grid_id = 0x43;                                  /* Skript-Freigabe wie sub10 @0x027E6 */
    for (int f = 0; f < 400; f++) {
        frame();
        if (d->state == 1) { *t_land = f; *cd_land = d->dog_atk_cd; return slot; }
    }
    return -1;
}

/* Spieler 1200 vor den Hund stellen und ihn anschauen (Biss-Lage wie test_re2_room1190_ab (4)). */
static void spieler_vor_hund(re15_actor_t *pl, const re15_actor_t *d, int f)
{
    pl->motion = 100;
    pl->floor = d->floor;
    if (f % 120 == 0) { pl->x = d->x + 1200; pl->z = d->z; }
    pl->rot_y = (int16_t)((re15_atan2_q12(d->z - pl->z, d->x - pl->x) - 0x400) & 0xfff);
}

int main(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* ================================ Teil 1: Praedikat ===================================== */
    {
        re15_actor_init();
        g_scd.player_mode = 2;
        CHECK(re15_szenenschutz_haelt() == 1, "T1 player_mode 2 = Szene haelt den Spieler");
        g_scd.player_mode = 1;
        CHECK(re15_szenenschutz_haelt() == 0, "T1 player_mode 1 (Plc_ret) = frei");
        g_scd.player_mode = 0;
        CHECK(re15_szenenschutz_haelt() == 0, "T1 player_mode 0 = frei");
        re15_actor_t a; memset(&a, 0, sizeof a);
        a.active = 1; a.type = 0x20; a.state = 1;
        CHECK(re15_szenenschutz_gegen(&a) == 1, "T1 Hund im freien Gehirn (+0x4 = 1) -> geschuetzt");
        a.state = 4;
        CHECK(re15_szenenschutz_gegen(&a) == 0, "T1 Hund im Skript-Zustand 4 (Sprung/Kaefig) -> Szenen-Rolle");
        a.type = 0x10; a.state = 1;
        CHECK(re15_szenenschutz_gegen(&a) == 1, "T1 Zombie 0x10 frei -> geschuetzt");
        a.type = 0x21; a.state = 1; a.grid_id = 0x40;
        CHECK(re15_szenenschutz_gegen(&a) == 0, "T1 Kraehe mit Skript-Marke grid 0x40 (@0x80112408) -> Szenen-Rolle");
        a.grid_id = 0x00;
        CHECK(re15_szenenschutz_gegen(&a) == 1, "T1 freie Kraehe grid 0 -> geschuetzt");
        a.type = 0x49; a.state = 1; a.grid_id = 0;
        CHECK(re15_szenenschutz_gegen(&a) == 0, "T1 NPC-Typ 0x49 (John) -> nie");
        a.type = 0x20; a.active = 0;
        CHECK(re15_szenenschutz_gegen(&a) == 0, "T1 inaktiver Akteur -> nie");
    }

    /* ======================= Teil 2: Haken-Folge + Sicherheitsnetz ============================ */
    {
        re15_actor_init();
        re15_szenenschutz_zaehler_reset();
        pl->active = 1; pl->hp = 100; pl->hit_react = 0;
        re15_actor_t *h = &g_actors[3]; h->active = 1; h->type = 0x20; h->state = 1;
        re15_actor_t *s = &g_actors[4]; s->active = 1; s->type = 0x21; s->state = 4; s->grid_id = 0x40;
        g_scd.player_mode = 2;
        re15_szenenschutz_durchlauf_beginn();
        re15_szenenschutz_gegner(3);
        CHECK((pl->hit_react & 1u) && re15_player_is_grabbed(),
              "T2 geschuetzter Tick sieht +0x93 Bit 0 und PL+0x1D3&0x80 (hit_react=%u)", pl->hit_react);
        pl->hp = (int16_t)(pl->hp - 20);                /* ungatterter Treffer eines freien Gegners */
        re15_szenenschutz_gegner(4);
        CHECK(pl->hp == 100, "T2 Sicherheitsnetz gibt den Abzug zurueck (hp=%d)", pl->hp);
        CHECK(pl->hit_react == 0 && !re15_player_is_grabbed(),
              "T2 Szenen-Rolle tickt mit dem echten Wert (hit_react=%u)", pl->hit_react);
        pl->hp = (int16_t)(pl->hp - 8); pl->hit_react |= 1u;   /* Szenen-Griff trifft echt */
        re15_szenenschutz_durchlauf_ende();
        CHECK(pl->hp == 92 && (pl->hit_react & 1u),
              "T2 Treffer der Szenen-Rolle bleibt (hp=%d hit_react=%u)", pl->hp, pl->hit_react);
        uint32_t ticks = 0, ab = 0; re15_szenenschutz_zaehler(&ticks, &ab);
        CHECK(ticks == 1 && ab == 1, "T2 Zaehler: 1 geschuetzter Tick, 1 abgefangener Abzug (%u/%u)", ticks, ab);
        g_scd.player_mode = 0;
        pl->hp = 100; pl->hit_react = 0;
        re15_szenenschutz_durchlauf_beginn();
        re15_szenenschutz_gegner(3);
        CHECK(pl->hit_react == 0 && !re15_player_is_grabbed(), "T2 ohne Halten keine Sicht");
        re15_szenenschutz_durchlauf_ende();
    }

    /* ================================= ROOM1190 laden ========================================= */
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz);
    re15_rdt_t rdt;
    if (!buf || re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        printf("FAIL: ROOM1190.RDT nicht lesbar\n");
        return 1;
    }

    /* ============== Teil 3: Freigabe-Sperrzeit 0x78 im RE2-Hund (Szenenschutz aus) ============== */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re2_hundebank();
        int t_land = -1, cd_land = -1;
        int slot = hund_1190(&rdt, &t_land, &cd_land);
        CHECK(slot > 0, "T3 RE2: Hund freigegeben und gelandet (Ausgang 0x201)");
        if (slot > 0) {
            re15_actor_t *d = &g_actors[slot];
            CHECK(cd_land == 0x78, "T3 Landung schreibt +0x1d6 = 0x78 (@0x801115e8-ec), war %d", cd_land);
            int t_commit = -1;
            /* Lage des Messlaufs m1190a: Hund 3 landet bei (690,-22992), Plc_ret-Teleport setzt Leon auf
             * (900,-19800) (sub10 @0x02834/0x02838) = 210/3192 VOR dem gelandeten Hund; dort committete der
             * RE2-Hund ohne Sperrzeit 2 Bilder nach der Landung (F741) und biss bei F752. */
            pl->x = d->x + 210; pl->z = d->z + 3192; pl->floor = d->floor; pl->motion = 0;
            for (int f = 0; f < 600 && t_commit < 0; f++) {
                frame();
                if (d->state == 1 && d->sub_state_1 == 3) t_commit = f + 1;   /* Bilder seit der Landung */
            }
            printf("  [T3] Landung=%d Angriffs-Commit %d Bilder danach, Spieler hp=%d\n", t_land, t_commit, pl->hp);
            CHECK(t_commit >= 120, "T3 erster Angriffs-Commit erst nach der Sperrzeit (>= 120 Bilder), war %d", t_commit);
            CHECK(t_commit > 0 && t_commit < 600, "T3 danach greift der Hund normal an (Commit gesehen)");
        }
    }

    /* ======= Teil 4/5: Szenenschutz an RE2- und RE1.5-Gehirn (Sperrzeit genullt) ============== */
    for (int flavor = 0; flavor < 2; flavor++) {
        re15_ai_flavor_set(flavor == 0 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        const char *nm = flavor == 0 ? "T4 RE2" : "T5 RE1.5";
        int t_land = -1, cd_land = -1;
        int slot = hund_1190(&rdt, &t_land, &cd_land);
        CHECK(slot > 0, "%s: Hund freigegeben und gelandet", nm);
        if (slot <= 0) continue;
        re15_actor_t *d = &g_actors[slot];
        d->dog_atk_cd = 0;                              /* (B) aus dem Weg: nur der Szenenschutz wirkt */
        re15_szenenschutz_zaehler_reset();
        g_scd.player_mode = 2;                          /* die Szene haelt den Spieler */
        int angriff = 0;
        for (int f = 0; f < 400; f++) {
            spieler_vor_hund(pl, d, f);
            frame();
            if (d->state == 1 && (flavor == 0 ? d->sub_state_1 == 3 : d->sub_state_1 == 8)) angriff++;
        }
        uint32_t ticks = 0, ab = 0; re15_szenenschutz_zaehler(&ticks, &ab);
        printf("  [%s] gehalten: hp=%d Griff=%d Angriffs-Bilder=%d geschuetzte Ticks=%u abgefangen=%u\n",
               nm, pl->hp, re15_player_victim_state(), angriff, ticks, ab);
        CHECK(pl->hp == 100, "%s: 400 Bilder gehalten -> KEIN Treffer (hp=%d)", nm, pl->hp);
        CHECK(angriff == 0, "%s: gehalten -> kein Angriffs-Zustand (RE2 Sub 3 Lunge / RE1.5 Sub 8 Biss), %d Bilder "
                            "(Negativkontrolle ohne Schutz: RE2 64 Bilder + 2 Bisse, RE1.5 49 Bilder)", nm, angriff);
        CHECK(re15_player_victim_state() == 0 && !re15_player_is_grabbed(), "%s: gehalten -> kein Griff", nm);
        CHECK(ticks >= 400, "%s: der Hund tickte geschuetzt (%u Ticks)", nm, ticks);
        CHECK(ab == 0, "%s: das Original-Tor haelt den Biss ab, das Sicherheitsnetz musste nicht eingreifen (%u)", nm, ab);
        g_scd.player_mode = 0;                          /* Szene vorbei: Angriff normal */
        int t_biss = -1;
        for (int f = 0; f < 900 && t_biss < 0; f++) {
            spieler_vor_hund(pl, d, f);
            int16_t vor = pl->hp;
            frame();
            if (pl->hp < vor) t_biss = f;
        }
        printf("  [%s] frei: erster Biss nach %d Bildern, hp=%d\n", nm, t_biss, pl->hp);
        CHECK(t_biss >= 0, "%s: nach der Szene beisst der Hund (hp=%d)", nm, pl->hp);
        pl->hp = 100; pl->state = 0; pl->hit_react = 0; pl->motion = 0;
        re15_player_clear_hit_guard(pl);
    }

    /* ============== Teil 6: Gegenprobe ROOM1171 — Szenen-Kraehe greift den gehaltenen Spieler ===== */
    {
        long sz2 = 0;
        uint8_t *b2 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1171.RDT", &sz2);
        re15_rdt_t r2;
        CHECK(b2 && re15_rdt_parse(b2, (size_t)sz2, &r2) == 0 && r2.sub_scd_count > 7,
              "T6 ROOM1171.RDT lesbar");
        if (b2 && r2.sub_scd_count > 7) {
            re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);    /* die Szenen-Kraehe lebt im RE1.5-FLIGHT-2 */
            re15_actor_init();
            re15_aot_init();
            scd_vm_init();
            re15_enemy_ai_set_paused(0);
            re15_damage_seed_rng(0x0badf00du);
            re15_game_flag_set(3, 194, 1);              /* sub07 @0x016C4 Ck(3,194)==1 -> Szene */
            scd_thread_start(2, r2.sub_scd[7]);         /* sub07: John 0x49 + 2 Kraehen 0x21 grid 0x40 (@0x016C8..) */
            for (int i = 0; i < 2; i++) scd_vm_tick();
            /* sub02 (Spieler-Fuehrung) nicht weiterlaufen lassen; scd_vm_init wuerde Akteure + Flags loeschen */
            for (int t = 0; t < SCD_THREAD_COUNT; t++) g_scd.threads[t].active = 0;
            pl->active = 1; pl->x = 1164; pl->y = -7200; pl->z = 7435;   /* Spielerlage der Szene (Messlauf m1171a) */
            pl->hp = 100; pl->hit_react = 0; pl->state = 4; pl->motion = 0; pl->floor = 4;
            int nkr = 0, kr = -1;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x21) { nkr++; if (kr < 0) kr = s; }
            CHECK(nkr == 2, "T6 zwei Szenen-Kraehen gespawnt (%d)", nkr);
            g_scd.player_mode = 2;                      /* Set(2,7)=1 @0x013A8: Elza wird gehalten */
            int t_griff = -1, gegen = -1;
            for (int f = 0; f < 400 && t_griff < 0; f++) {
                frame();
                if (kr > 0 && f == 5) gegen = re15_szenenschutz_gegen(&g_actors[kr]);
                if (pl->hp < 100 || re15_player_is_grabbed()) t_griff = f;
            }
            printf("  [T6] Kraehe state=%d grid=0x%02X, Griff bei Bild %d, hp=%d\n",
                   kr > 0 ? g_actors[kr].state : -1, kr > 0 ? g_actors[kr].grid_id : 0, t_griff, pl->hp);
            CHECK(gegen == 0, "T6 Szenen-Kraehe (FLIGHT-2) ist vom Schutz ausgenommen");
            CHECK(t_griff >= 0, "T6 die Szenen-Kraehe greift den gehaltenen Spieler (Teil der Cutscene)");
            g_scd.player_mode = 0;
            free(b2);
        }
    }

    free(buf);
    printf("%s (%d Fehler)\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
