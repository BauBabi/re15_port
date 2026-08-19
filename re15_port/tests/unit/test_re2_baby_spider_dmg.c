/**
 * @file test_re2_baby_spider_dmg.c
 * @brief PIN — Waffenschaden auf die BABY-SPINNE (Typ 0x26), beide KI-Modi.
 *
 * BEFUND 2026-08-19 (Nebenbefund aus eb841053/9178ddba, hier zu Ende untersucht):
 *
 * RE1.5 = KORREKT, wird gepinnt, nicht geaendert. Die Waffen-Schadenszeile fuer Typ 0x26 liegt
 * @0x8006EDE0 (= 0x8006e0d0 + 0x26*0x58) und ist 22x0 (aus info/Re1.5/PSX.EXE gedumpt). Die
 * Tabelle hat GENAU EINEN Xref — `addiu a0,a0,-7984` @0x800124B8 (Voll-Scan aller
 * PSX.EXE-Instruktionen), es gibt also keinen zweiten Schadensweg. Der Crit-Instakill greift
 * nicht (`sltiu v0,v0,0x20` @0x80012510, 0x26 >= 0x20). Sie ist in RE1.5 unzerstoerbar und
 * reagiert nur mit HURT (`+0x4 = 2` @0x80012520-2C -> RE1.5-Handler 0x80116A04, 3 Frames,
 * dann zurueck auf state 1).
 *
 * RE2 = WICH AB, gefixt. Der RE2-Applier FUN_800470C0 liest den Schaden ueber eine PER-TYP-
 * ZEIGER-Tabelle: `lbu v0,8(s1)` (Typ) @0x80047218 -> `lw a1,27272(at)` = 0x800A6A88 + typ*4
 * @0x8004722C; Zeile = (w-1)*20 @0x80047230-40; `lw v1,0(a1)` @0x8004724C;
 * `srlv v1,v1,v0` (v0 = Bracket*10) @0x80047254; `andi v1,v1,0x3ff` @0x8004725C;
 * `sh v0,342(s1)` @0x80047268. 0x800A6A88[0x25] == 0x800A6A88[0x26] == 0x800A4B90 — Adult und
 * Baby teilen dieselbe Zeile, und die ist voll besetzt (Zone 0: 15/17/.../200). Baby-HP = 1
 * (`sh v0,342` EMS26.BIN @0x801000F8) -> in RE2 toetet JEDER Treffer sofort.
 *
 * GEMESSEN VOR DEM FIX (probe_re2_baby_spider_hit, ROOM1090, echter game_step-Schussweg):
 *   - Baby mit HP 1: erster Schuss -> `+0x4 = 2`, danach GENAU EIN Zustandswechsel in 300
 *     Frames, +0x93 blieb 0x03 = PERMANENT eingefroren. Grund: das RE2-Baby-Modul hat keinen
 *     HURT-Handler (Wurzeltabelle @0x80101084 Eintrag [2] = 0x80100BB8 = `jr ra`).
 *   - Baby mit Spawn-Deskriptor 0 (HP = -1 = UNVERWUNDBAR, `sh -1,342` @0x80100204, Zweig
 *     `+0x10E & 0xFF == 0` @0x801001F8): starb beim ERSTEN Schuss, obwohl RE2 solche Ziele
 *     komplett verwirft (`lh v0,342(s0)` @0x80047148 / `bltz v0,0x8004740C` @0x80047150).
 *
 * ERREICHBARKEIT: Typ 0x26 steht 7x in STAGE1/ROOM1090 (eigener Sce_em_set-Zensus ueber alle
 * 240 RDTs, Deskriptoren 0x00/0x01/0x02/0x03x2/0x04x2, RDT-Offsets 0x2214..0x228C).
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* Zone 0 von 0x800A4B90 + (RE2-Zeile-1)*20, RE2-Zeile = s_re2s_row_from_weapon
 * (enemy_ai_re2_spider.c:1525). Adressen je Waffe stehen in re15_damage.c. */
static const int s_re2_expect[22] = {
    15, 15, 15, 17, 17, 17, 17, 130, 55, 60, 90, 130, 10, 130, 10, 60, 90, 130, 200, 18, 17, 15
};
/* Waffen, die den unbegrenzten Schuss-Streifen benutzen (Tester-Tabelle @0x8006E548). */
static const uint8_t s_gun_strip[22] = { 1,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,0,1 };

static uint8_t *s_buf = NULL;
static long     s_sz  = 0;

static int load_room(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); s_sz = ftell(f); fseek(f, 0, SEEK_SET);
    s_buf = (uint8_t *)malloc((size_t)s_sz);
    if (!s_buf || fread(s_buf, 1, (size_t)s_sz, f) != (size_t)s_sz) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

/* Raum frisch hochfahren und die Baby-Slots einsammeln. Rueckgabe = Anzahl. */
static int bringup(int flavor, int *slots, int max)
{
    re15_rdt_t rdt;
    if (re15_rdt_parse(s_buf, (size_t)s_sz, &rdt) != 0) return 0;
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->x = 0; pl->y = 0; pl->z = 0;

    for (int i = 0; i < 30; i++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < max; s++)
        if (g_actors[s].active && g_actors[s].type == 0x26u) slots[n++] = s;
    return n;
}

/* EIN Ziel scharfmachen: alle anderen aus, Spieler davor, frischer Zustand. */
static void arm(int slot, uint8_t type, int hp, int dist)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->active = 1; e->type = type;
    re15_enemy_apply_hitbox(e, type);
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = (int16_t)hp; e->hit_react = 0; e->hit_stun = 0;
    e->grid_id = (uint8_t)(e->grid_id & 0x7f);   /* aufrecht -> LEVEL-Band @0x801015F4-FC */
    e->anim_flags = 0;
    pl->x = e->x - dist; pl->z = e->z; pl->y = e->y; pl->rot_y = 0; pl->hit_react = 0;
}

int main(void)
{
    if (!load_room()) { printf("SKIP: ROOM1090.RDT fehlt\n"); return 0; }

    int slots[8];
    printf("=== 1) ERREICHBARKEIT: Typ 0x26 in ROOM1090 ===\n");
    int n = bringup(RE15_AI_FLAVOR_RE15, slots, 8);
    printf("  Baby-Spinnen aufgestellt: %d\n", n);
    CHECK(n == 7, "ROOM1090 muss 7 Sce_em_set-Records vom Typ 0x26 aufstellen (Zensus), sind %d", n);
    if (n < 1) { printf("\n%s\n", fails ? "FAILED" : "OK"); return fails ? 1 : 0; }

    /* ---- 2) RE1.5: byte-true Nullzeile @0x8006EDE0 — JEDE Waffe macht 0 Schaden ---- */
    printf("\n=== 2) RE1.5: Zeile 0x26 @0x8006EDE0 ist 22x0 (Port bleibt unveraendert) ===\n");
    for (int w = 0; w < 22; w++) {
        bringup(RE15_AI_FLAVOR_RE15, slots, 8);
        arm(slots[1], 0x26, 100, s_gun_strip[w] ? 3000 : 700);
        re15_player_set_equipped_weapon(w);
        int fired = re15_player_weapon_fire(w);
        re15_actor_t *e = &g_actors[slots[1]];
        CHECK(fired != 0, "RE1.5 w%d: Schuss kam nicht an (Reichweite/Kegel)", w);
        if (!fired) continue;
        CHECK(e->hp == 100, "RE1.5 w%d: hp %d != 100 — die Nullzeile @0x8006EDE0 wurde verletzt",
              w, e->hp);
        CHECK(e->state == 2, "RE1.5 w%d: state %d != 2 (HURT, @0x80012520)", w, e->state);
    }
    printf("  22/22 Waffen: hp unveraendert, state = 2 (HURT)\n");

    /* ---- 3) RE1.5: die Baby-Spinne erholt sich aus HURT (kein Einfrieren) ---- */
    printf("\n=== 3) RE1.5: HURT ist ein 3-Frame-One-Shot (0x80116A04 -> state 1) ===\n");
    bringup(RE15_AI_FLAVOR_RE15, slots, 8);
    arm(slots[1], 0x26, 100, 3000);
    re15_player_set_equipped_weapon(3);
    (void)re15_player_weapon_fire(3);
    {
        re15_actor_t *e = &g_actors[slots[1]];
        int back = 0;
        for (int f = 0; f < 30 && !back; f++) {
            re15_enemy_ai_run_all(1); re15_actors_anim_advance();
            if (e->state == 1) back = f + 1;
        }
        printf("  zurueck in ACTIVE nach %d Frames, hp=%d\n", back, e->hp);
        CHECK(back > 0 && back <= 5, "RE1.5: kein Ruecksprung nach ACTIVE (%d Frames)", back);
        CHECK(e->hp == 100, "RE1.5: hp %d != 100", e->hp);
    }

    /* ---- 4) RE2: jede Waffe toetet sofort (Zeile 0x800A4B90, HP 1 @0x801000F8) ---- */
    printf("\n=== 4) RE2: Zeile 0x800A4B90 Zone 0 — jeder Treffer toetet (HP 1) ===\n");
    for (int w = 0; w < 22; w++) {
        bringup(RE15_AI_FLAVOR_RE2, slots, 8);
        arm(slots[1], 0x26, 1, s_gun_strip[w] ? 3000 : 700);
        re15_player_set_equipped_weapon(w);
        int fired = re15_player_weapon_fire(w);
        re15_actor_t *e = &g_actors[slots[1]];
        CHECK(fired != 0, "RE2 w%d: Schuss kam nicht an", w);
        if (!fired) continue;
        CHECK(e->hp == (int16_t)(1 - s_re2_expect[w]),
              "RE2 w%d: hp %d, erwartet %d (Zone 0 = %d)", w, e->hp,
              1 - s_re2_expect[w], s_re2_expect[w]);
        CHECK(e->state == 3, "RE2 w%d: state %d != 3 (DEATH, @0x8004728C-90) — die Spinne friert "
              "sonst im nicht existierenden HURT ein (@0x80101084[2] = `jr ra`)", w, e->state);
    }
    printf("  22/22 Waffen: hp = 1 - dmg < 0, state = 3 (DEATH)\n");

    /* ---- 5) RE2: HP < 0 = kein gueltiges Ziel (@0x80047148-50) ---- */
    printf("\n=== 5) RE2: Deskriptor-0-Baby (HP -1 @0x80100204) ist UNVERWUNDBAR ===\n");
    {
        bringup(RE15_AI_FLAVOR_RE2, slots, 8);
        arm(slots[1], 0x26, -1, 3000);
        re15_actor_t *e = &g_actors[slots[1]];
        int st0 = e->state;
        re15_player_set_equipped_weapon(3);
        int fired = re15_player_weapon_fire(3);
        printf("  fired=%d hp=%d state=%d (vorher %d)\n", fired, e->hp, e->state, st0);
        CHECK(fired == 0, "RE2: HP<0-Ziel wurde getroffen (RE2 verwirft es, `bltz` @0x80047150)");
        CHECK(e->hp == -1 && e->state == st0, "RE2: HP<0-Ziel wurde veraendert (hp=%d state=%d)",
              e->hp, e->state);
        /* Gegenprobe: derselbe Aktor mit HP >= 0 ist sehr wohl ein Ziel. */
        arm(slots[1], 0x26, 1, 3000);
        CHECK(re15_player_weapon_fire(3) != 0, "RE2: HP>=0-Ziel wurde NICHT getroffen "
              "(dann misst Punkt 5 nichts)");
    }

    /* ---- 6) GELTUNGSBEREICH: RE1.5 unveraendert, RE2 traegt jetzt die RE2-Zeilen ---------
     * ⚠ HIER STAND FRUEHER: "der Fix darf NUR Typ 0x26 betreffen" — 0x25 und 0x10 mussten auch
     * im RE2-Modus die RE1.5-Zeile behalten. Diese Erwartung ist per NUTZER-ENTSCHEIDUNG
     * 2026-08-19 aufgehoben ("die Zombies stecken viel zu viel ein"): der RE2-Modus faehrt jetzt
     * das VOLLSTAENDIGE RE2-Schadens-/HP-Modell (re15_damage.c, Applier FUN_800470C0
     * @0x80047218-68 ueber die Per-Typ-Zeiger 0x800A6A88[typ]). Der Test bleibt eine Wache —
     * aber fuer die richtige Grenze: RE1.5 unveraendert, RE2 mit der RE2-Zeile.
     *   RE1.5: 0x25 @0x8006ED88[3] = 10 ; 0x10 @0x8006E650[3] = 5     (aus PSX.EXE gedumpt)
     *   RE2:   0x25 -> 0x800A6A88[0x25] = 0x800A4B90, Waffe 3 -> Zeile 3 @0x800A4BB8 Zone 0 = 17
     *          0x10 -> 0x800A6A88[0x10] = 0x800A412C, Waffe 3 -> Zeile 3 @0x800A4154 Zone 0 = 16
     * Der Modell-Schalter muss beides umschalten koennen (Negativ-Test). */
    printf("\n=== 6) GELTUNGSBEREICH: RE1.5-Zeile vs. RE2-Zeile bei 0x25 / 0x10 ===\n");
    {
        const struct { uint8_t type; int dmg15; int dmg2; const char *a15; const char *a2; } guard[2] = {
            { 0x25u, 10, 17, "0x8006ED88[3]", "0x800A4BB8" },
            { 0x10u,  5, 16, "0x8006E650[3]", "0x800A4154" }
        };
        for (int g = 0; g < 2; g++) {
            /* (a) RE1.5-Pfad — MUSS byte-identisch bleiben */
            bringup(RE15_AI_FLAVOR_RE15, slots, 8);
            arm(slots[1], guard[g].type, 300, 3000);
            re15_player_set_equipped_weapon(3);
            if (re15_player_weapon_fire(3)) {
                re15_actor_t *e = &g_actors[slots[1]];
                CHECK(e->hp == (int16_t)(300 - guard[g].dmg15),
                      "Guard 0x%02X (RE1.5): hp %d, erwartet %d aus %s", guard[g].type,
                      e->hp, 300 - guard[g].dmg15, guard[g].a15);
            } else CHECK(0, "Guard 0x%02X (RE1.5): Schuss kam nicht an", guard[g].type);

            /* (b) RE2 + Modell AN -> die RE2-Zeile */
            re15_re2_damage_model_set(1);
            bringup(RE15_AI_FLAVOR_RE2, slots, 8);
            arm(slots[1], guard[g].type, 300, 3000);
            g_actors[slots[1]].re2_hp_stamped = 1;   /* Test-HP behalten (HP-Seite hat eigenen Pin) */
            re15_player_set_equipped_weapon(3);
            if (re15_player_weapon_fire(3)) {
                re15_actor_t *e = &g_actors[slots[1]];
                CHECK(e->hp == (int16_t)(300 - guard[g].dmg2),
                      "Guard 0x%02X (RE2 AN): hp %d, erwartet %d aus %s", guard[g].type,
                      e->hp, 300 - guard[g].dmg2, guard[g].a2);
            } else CHECK(0, "Guard 0x%02X (RE2 AN): Schuss kam nicht an", guard[g].type);

            /* (c) RE2 + Modell AUS -> wieder die RE1.5-Zeile (Negativ-Test) */
            re15_re2_damage_model_set(0);
            bringup(RE15_AI_FLAVOR_RE2, slots, 8);
            arm(slots[1], guard[g].type, 300, 3000);
            re15_player_set_equipped_weapon(3);
            if (re15_player_weapon_fire(3)) {
                re15_actor_t *e = &g_actors[slots[1]];
                CHECK(e->hp == (int16_t)(300 - guard[g].dmg15),
                      "Guard 0x%02X (RE2 AUS): hp %d, erwartet %d aus %s", guard[g].type,
                      e->hp, 300 - guard[g].dmg15, guard[g].a15);
            } else CHECK(0, "Guard 0x%02X (RE2 AUS): Schuss kam nicht an", guard[g].type);
            re15_re2_damage_model_set(1);
        }
        printf("  0x25: RE1.5 10 / RE2 17 ; 0x10: RE1.5 5 / RE2 16 ; Modell AUS = RE1.5\n");
    }

    printf("\n%s\n", fails ? "FAILED" : "OK");
    return fails ? 1 : 0;
}
