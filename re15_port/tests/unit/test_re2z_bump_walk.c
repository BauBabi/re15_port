/**
 * @file test_re2z_bump_walk.c
 * @brief PIN — EXEC[2] (@0x80102260) IST DER ZWEITE GANG DES RE2-ZOMBIES ("Arme oben")
 *        und er BEWEGT sich.
 *
 * ⛔ NUTZER-REPORT 2026-08-22 (fetter Zombie 0x11, ROOM1140, RE2-Modus):
 *   "Es gibt manchmal Momente, wo er kurz zu einer Animation mit GEHOBENEN ARMEN wechselt, und
 *    dann laeuft. Immer wenn diese Animation kommt, LAEUFT ER AUF DER STELLE, anstatt wirklich
 *    Richtung Spieler zu gehen."
 *
 * GEMESSEN VOR DEM FIX (probe_re2_fatz_walkspot, echte ROOM1140.RDT + echte RE2-Bank EM011,
 * game_step-Reihenfolge, +0x4 = 0x201 gesetzt wie es die Original-Leitern tun):
 *   f0100 st=1 s1=2 s2=1 clip=4 af= 1 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   f0125 st=1 s1=2 s2=1 clip=4 af=26 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   f0150 st=1 s1=2 s2=1 clip=4 af=51 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   => 59 Bilder Loco-Clip 4, Summe |dx|+|dz| = 0.
 * Und die "gehobenen Arme" sind gemessen, nicht behauptet — Mittel der Hand-Bone-Y ueber den
 * ganzen Clip (Y nach unten positiv, kleiner = hoeher), EM011-Loco-Bank:
 *   Clip 0/1 (EXEC[1]-Gang): b11 -1609 b14 -1635 | Clip 2/3: b11 -1646 b14 -1526
 *   Clip 4/5 (EXEC[2]-Gang): b11 -2084 b14 -1936 | Clip 6/7 (EXEC[2]): b11 -2053 b14 -2114
 *
 * URSACHE: die Port-Fassung von EXEC[2] war ein 2-Phasen-Stub ("P1+ nicht RE'd; Entry OPEN,
 * toter Code") — Clip setzen, dann auf das Clip-Ende warten. Kein 0x80015558 (Steuerung), kein
 * 0x80015E7C/0x800152C8 (Wurzelbewegung). Beides steht im Original:
 *   @0x8010234C-58 / @0x80102364-70  jal 0x80015558 mit a3 = +16 bzw. -16 (Weben ueber +0x16A)
 *   @0x801023D4-E0 jal 0x80015E7C (Pair-1-Bank a1=+0x108/a2=+0x17C)
 *   @0x801023E4-F4 jal 0x8002959C (a3 = 256)
 *   @0x8010246C-70 jal 0x800152C8 = das ANWENDEN der Bewegung
 * Dazu DECISION[2] @0x80101F7C (Tabelle @0x8010C88C[2]), im Port vorher `default: break` —
 * ohne sie koennte der Zombie im zweiten Gang weder greifen noch beissen.
 * (Die DREI zusaetzlichen Produzenten im Gang-Executor @0x80101DB8 / @0x80101F0C / @0x80101F5C
 *  sind disassembliert, aber noch nicht scharf — Begruendung + Messung im Kopf von
 *  re2z_exec_walk. Erreichbar ist EXEC[2] im Port ueber die Treffer-Erholungs-Leitern.)
 *
 * Der Pin faehrt die ECHTE Kette (re15_enemy_ai_run_all + re15_actors_anim_advance) auf der
 * echten ROOM1140-Besetzung mit geladener RE2-Bank; ohne Assets -> SKIP 77.
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
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern int re15_actor_clip_len(const re15_actor_t *a);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = sz; return b;
}

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static int load_re2_bank(uint8_t type)
{
    if (!s_ems) { long n = 0; s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
                  s_ems_sz = n; }
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

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

int main(void)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf) { printf("SKIP: ROOM1140.RDT fehlt\n"); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        printf("SKIP: RDT-Parse\n"); return 77; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x11) { slot = s; break; }
    if (slot < 0) { printf("SKIP: kein Typ 0x11 in ROOM1140\n"); return 77; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (!load_re2_bank(g_actors[slot].type)) { printf("SKIP: RE2-Bank EM011 fehlt\n"); return 77; }

    re15_actor_t *e = &g_actors[slot];
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    CHECK(b && b->loco_ok && b->anim_loco.clip_count >= 8,
          "RE2-Loco-Bank (Pair 1) mit 8 Clips erwartet");

    /* Der Spieler steht 3000 in -X und GEHT (cfbf6 Bit 1 @0x8003CC80) — der Zombie wacht auf
     * und laeuft an. */
    pl->x = e->x - 3000; pl->z = e->z; pl->y = e->y; pl->motion = 105;

    int saw_walk = 0, saw_bump = 0, bump_first = -1;
    int32_t px = e->x, pz = e->z;
    long long bump_move = 0; int bump_frames = 0;
    uint32_t dist_at_entry = 0, dist_min_in_bump = 0xffffffffu;
    int bump_clip = -1, bump_loco = -1, bump_exit_sub = -1;

    /* EINSTIEG: das Wort 0x201 wird im ausgelieferten Port von den TREFFER-ERHOLUNGS-Leitern
     * geschrieben (@0x801060F4 hit_STAGGER, @0x80105B24 hit_MAIN P3, @0x80107418 hit_SLIDE P6,
     * @0x801081BC DEATH-P2) — genau dort traf der Nutzer den Fehler. Die drei zusaetzlichen
     * Produzenten im GANG-Executor (@0x80101DB8 / @0x80101F0C / @0x80101F5C) sind zwar
     * disassembliert, aber noch nicht scharf (Begruendung + Messung im Kopf von
     * re2z_exec_walk). Der Pin setzt das Wort deshalb selbst — wortgleich zu den Leitern. */
    for (int f = 0; f < 600; f++) {
        pl->hp = 100; pl->motion = 105;
        if (f == 120 && e->state == 1 && e->sub_state_1 == 1) {
            re15_ai_set_state_word(e, 0x201);       /* 513, wie @0x80105B24-2C */
            e->sub_state_2 = 0;
        }
        frame();
        int32_t dx = e->x - px, dz = e->z - pz; px = e->x; pz = e->z;
        if (e->state == 1 && e->sub_state_1 == 1) saw_walk = 1;
        if (e->state == 1 && e->sub_state_1 == 2) {
            if (!saw_bump) { saw_bump = 1; bump_first = f; dist_at_entry = e->ai_dist; }
            /* Der Clip wird erst in P0 gesetzt: das Wort 0x201 faellt am ENDE des Gang-Ticks
             * (@0x80101F10 / @0x80101F60), EXEC[2] P0 laeuft also erst im FOLGENDEN Bild
             * (+0x6 == 0 -> 1 @0x8010229C-A0). Deshalb erst ab sub_state_2 >= 1 messen. */
            if (bump_clip < 0 && e->sub_state_2 >= 1) {
                bump_clip = (int)e->motion; bump_loco = re15_re2z_poses_loco_bank(e); }
            bump_move += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            bump_frames++;
            if (e->ai_dist < dist_min_in_bump) dist_min_in_bump = e->ai_dist;
        } else if (saw_bump && bump_exit_sub < 0 && bump_frames > 0) {
            bump_exit_sub = e->sub_state_1;
        }
    }

    printf("  Gang gesehen=%d | EXEC[2] ab Bild %d, %d Bilder, Clip %d (LOCO=%d), "
           "Summe|d|=%lld, dist %u -> %u, Ausgang sub=%d\n",
           saw_walk, bump_first, bump_frames, bump_clip, bump_loco, bump_move,
           dist_at_entry, dist_min_in_bump, bump_exit_sub);

    /* --- PIN 1: der Gang-Executor ERREICHT EXEC[2] ueberhaupt (die drei Produzenten) ------- */
    CHECK(saw_walk, "PIN1: der Zombie erreicht den Gang (ACTIVE sub 1) nicht");
    CHECK(saw_bump, "PIN1: EXEC[2] wurde nie erreicht (Zustandswort 0x201 nicht angenommen)");

    /* --- PIN 2: der gespielte Clip ist der PAIR-1-Clip 4/6 (`0xF0004 + walkclip`) ---------- */
    CHECK(bump_clip == 4 || bump_clip == 6,
          "PIN2: EXEC[2] spielt Clip %d statt 4/6 (Wort 0xF0004 + +0x218 @0x80102290-AC)",
          bump_clip);
    CHECK(bump_loco == 1,
          "PIN2: EXEC[2] posiert nicht aus der PAIR-1-Bank (a1=+0x108/a2=+0x17C "
          "@0x801022F0-F4 / @0x801023D4-E0)");

    /* --- PIN 3 (DER NUTZER-BEFUND): er LAEUFT NICHT AUF DER STELLE ------------------------- */
    CHECK(bump_frames >= 20, "PIN3: EXEC[2] lief nur %d Bilder — zu kurz zum Messen",
          bump_frames);
    CHECK(bump_move > 0,
          "PIN3: EXEC[2] bewegt sich NICHT (Summe |dx|+|dz| = %lld in %d Bildern) — genau der "
          "gemeldete Fehler; 0x80015E7C@0x801023DC + 0x800152C8@0x8010246C fehlen",
          bump_move, bump_frames);
    if (bump_frames >= 20)
        CHECK(bump_move / bump_frames >= 5,
              "PIN3: EXEC[2] kriecht mit %lld Einheiten/Bild — der Pair-1-Clip 4 traegt 1172 "
              "Einheiten auf 60 Bilder (~19/Bild), Clip 6 sogar 1569 auf 55",
              bump_move / (bump_frames ? bump_frames : 1));

    /* --- PIN 4: er laeuft AUF DEN SPIELER ZU, nicht irgendwohin ---------------------------- */
    CHECK(dist_min_in_bump < dist_at_entry,
          "PIN4: die Distanz sank waehrend EXEC[2] nicht (%u -> %u) — die Steuerung "
          "0x80015558 (a3 = +-16, @0x8010234C-58 / @0x80102364-70) fehlt",
          dist_at_entry, dist_min_in_bump);

    printf("test_re2z_bump_walk: %s\n", fails ? "FAIL" : "OK");
    return fails ? 1 : 0;
}
