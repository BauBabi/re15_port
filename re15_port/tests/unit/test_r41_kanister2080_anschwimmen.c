/*
 * Runde 41 Spur G "kanister2080", Nachbesserung 2 (Abnahme 1, Mangel N3 / Nachtrag 23): nimmt DERSELBE Alligator den
 * geworfenen Kanister im weiteren Kampf auf - unabhaengig davon, wo der Kampf beim Ausloesen stand?
 * Treibt den ECHTEN Boss-Takt (re15_gator_boss_tick + Dispatcher-Klemme, Aufbau wie tools/r41_kanister2080/probe_liegestelle.c
 * und tests/unit/test_r41_re2gator.c setup(0): KEIN RE15_GB_TEST, kein RE15_GATOR_KANISTER). Je Fall:
 *   1. Alligator in die Verfolgung bringen (Leon als Statue am Lauerplatz -> Auftauchen -> Intro -> CHASE),
 *   2. Alligator und Leon auf die Lagen eines Abnahme-Laufs beim Ausloesen setzen (Dossier 9.3: kampf, v1, v3/v5, v2/v4, dazu ein
 *      Kampf im Suedwestbecken),
 *   3. den Kanister wie nach der Szene legen: Bits 110/115/116, Bit 117 nach re15_k2080_wurf_gier_wahl, Raumaufbau
 *      re15_kanister2080_r41_install (Kanister an der Aufschlagstelle der Nennbahn = Spur-B-API re15_gr2_kanister_setzen),
 *   4. bis 1500 Bilder takten: Bild der Aufnahme (re15_gr2_kanister_stufe >= 1) und kein Biss (Phase LUNGE) davor.
 * Gegenprobe: derselbe Kanister OHNE die Szenen-Bits (Spur-B-Testhaken-Weg) -> kein Anschwimmen (Haken nur fuer die Szene).
 * Herleitung: include/re15_kanister2080_r41.h RE15_K2080_ANSCHWIMM_AB (RE2-Zielmodus @0x80100a34-5c / @0x80100a88-8c, Biss-Sperre
 * @0x8010134c-58, RE2-Bahn x -23078 @0x80101048-4c, Kanister auf der Bahn ROOM40A0 sub10/sub05 @0x0672/@0x06A2/@0x0484).
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
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_gator_re2_r41.h"
#include "re15_kanister2080_r41.h"
#include "re15_gameflow.h"
#include "re2_ems.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern unsigned g_current_room_id;

static int fails = 0;
#define PRUEFE(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static uint8_t *s_rdt; static size_t s_rdt_n;
static uint8_t *s_ems; static size_t s_ems_n;
static int slot; static re15_actor_t *e, *pl;
static void se_nop(int id, int f) { (void)id; (void)f; }
static void bank_nop(int b) { (void)b; }
static int setup(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST="); _putenv("RE15_GB_STUMM=1"); _putenv("RE15_GATOR_KANISTER="); _putenv("RE15_K2080_WURF_GIER=");
#else
    unsetenv("RE15_GB_TEST"); setenv("RE15_GB_STUMM", "1", 1); unsetenv("RE15_GATOR_KANISTER"); unsetenv("RE15_K2080_WURF_GIER");
#endif
    if (re15_rdt_parse(s_rdt, s_rdt_n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
    if (!eb || !eb->ok) {
        if (!eb) eb = re15_enemy_alloc(0x23u);
        if (!eb || re2_ems_load_bank(s_ems, s_ems_n, 0x23, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1;
    e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -5300; e->y = 0; e->z = -16300;            /* Port-Spawn (main.c, dritte Wasserzone), vor dem Boss-INIT */
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    re15_gator_audio_hook(se_nop, bank_nop);
    re15_gr2_raum_se_hook = NULL;
    re15_gr2_kanister_entfernen();
    re15_k2080_reset();
    for (int b = 110; b <= 119; b++) re15_game_flag_set(9, (uint8_t)b, 0);
    g_gameflow.character = 0;
    return 1;
}
static void tick(void)
{
    int32_t ox = e->x, oz = e->z;
    re15_gator_boss_tick(slot);
    if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
        int32_t nx = e->x, nz = e->z;
        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
        e->x = nx; e->z = nz;
    }
}
static void statue(int32_t x, int32_t z)
{
    pl->x = x; pl->z = z; pl->y = 0; pl->floor = 0; pl->state = 0; pl->motion = 0; pl->hit_react = 0; pl->hp = 100;
}
/* Rueckgabe: Bild der Aufnahme (>= 0), -1 keine, -2 Aufbau gescheitert; *biss = Bild des ersten Bisses (LUNGE) vor der Aufnahme. */
static int fall(const char *was, int32_t gx, int32_t gz, int32_t px, int32_t pz, int szene, int *biss, int *gier_out)
{
    *biss = -1;
    if (!setup()) { printf("FAIL: %s: Aufbau\n", was); fails++; return -2; }
    statue(-6000, -12000);                             /* am Lauerplatz (-7200,-9200): Auftauchen < 9000, Intro, Verfolgung */
    int f = 0;
    for (; f < 3000 && !re15_gator_boss_verfolgt(e); f++) { statue(-6000, -12000); tick(); }
    if (!re15_gator_boss_verfolgt(e)) { printf("FAIL: %s: keine Verfolgung nach %d Bildern\n", was, f); fails++; return -2; }
    for (int k = 0; k < 30; k++) { statue(-6000, -12000); tick(); }
    /* Lage beim Ausloesen (FG-E3: derselbe Alligator, Lage/Zustand bleiben) */
    e->x = gx; e->z = gz;
    statue(px, pz);
    int g = RE15_K2080_WURF_GIER_NORD;
    if (szene) {
        g = re15_k2080_wurf_gier_wahl(slot, gx, gz);
        re15_game_flag_set(9, RE15_K2080_BIT_F_SZENE, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_SZENE, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_KANISTER, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_SUEDWURF, g == RE15_K2080_WURF_GIER_SUED);
        re15_kanister2080_r41_install(0x2090);
    } else {
        re15_gr2_kanister_setzen(-6821, -1200, -22914);   /* derselbe Ort wie der Suedwurf, aber ohne die Szene (Spur-B-Weg) */
    }
    if (gier_out) *gier_out = g;
    int32_t kp[3] = { 0, 0, 0 }, kr[9];
    if (!re15_gr2_kanister_welt(kp, kr)) { printf("FAIL: %s: kein Kanister\n", was); fails++; return -2; }
    int aufnahme = -1;
    for (f = 0; f < 1500; f++) {
        statue(px, pz);
        tick();
        if (*biss < 0 && re15_gator_boss_phase(e) == 3 /* GBP_LUNGE */) *biss = f;
        if (re15_gr2_kanister_stufe() >= 1) { aufnahme = f; break; }
    }
    printf("  %-36s Alligator (%6d,%6d) Leon (%6d,%6d) Wurf-Gier %4d Kanister (%6d,%6d): ", was, (int)gx, (int)gz, (int)px,
           (int)pz, g, (int)kp[0], (int)kp[2]);
    if (aufnahme >= 0) printf("Aufnahme Bild %d (%.1f s)", aufnahme, aufnahme / 30.0);
    else printf("KEINE Aufnahme in 1500 Bildern");
    printf(", Biss davor %s\n", *biss >= 0 ? "JA" : "nein");
    return aufnahme;
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    s_rdt = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &s_rdt_n);
    s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_rdt || !s_ems) { printf("FAIL: ROOM2090.RDT oder CDEMD0.EMS fehlt\n"); return 1; }
    /* Lagen beim Ausloesen aus den Laeufen der Abnahme 1 (state.log, Dossier 9.3) */
    const struct { const char *was; int32_t gx, gz, px, pz; } k_f[] = {
        { "kampf (Riegel-Startlage)",      -4889,  -7442,  -5790,  -6245 },
        { "v1 natuerlich (Suedost-Ecke)",   5251, -24990,   6732, -26335 },
        { "v3/v5 Start West",              -4940,  -9716,  -8432, -10447 },
        { "v2/v4 Start (0,-7000)",          5716,  -7090,   6670,  -5935 },
        { "Kampf im Suedwestbecken",       -6000, -23500,  -7000, -25000 },
    };
    for (unsigned i = 0; i < sizeof k_f / sizeof k_f[0]; i++) {
        int biss = -1, g = 0;
        const int a = fall(k_f[i].was, k_f[i].gx, k_f[i].gz, k_f[i].px, k_f[i].pz, 1, &biss, &g);
        if (a == -2) continue;
        PRUEFE(a >= 0, "%s: keine Aufnahme in 1500 Bildern", k_f[i].was);
        PRUEFE(biss < 0, "%s: Biss auf Leon (Bild %d) vor der Aufnahme - RE2 schaerft T1 im Zielmodus nicht (@0x8010134c-58)",
               k_f[i].was, biss);
    }
    {   /* Gegenprobe: Kanister ohne die Szene -> kein Anschwimmen (Spur B unveraendert) */
        int biss = -1;
        (void)fall("Gegenprobe ohne Szene (Spur-B-Weg)", 5251, -24990, 6732, -26335, 0, &biss, NULL);
        PRUEFE(re15_k2080_r41_anschwimmen_aktiv(e) == 0, "Gegenprobe: Anschwimmen aktiv ohne die Szenen-Bits");
    }
    printf(fails ? "unit_r41_kanister2080_anschwimmen: %d Fehler\n" : "unit_r41_kanister2080_anschwimmen: gruen (%d)\n", fails);
    return fails != 0;
}
