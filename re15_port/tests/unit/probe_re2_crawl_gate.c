/* probe_re2_crawl_gate.c — MESSUNG (kein Fix): der ROOM1030-Kriech-Auslöser unter dem RE2-Flavor.
 *
 * NUTZER-REPORT (2026-08-21): "Bei RE2 AI in der Lobby kriechen die Zombies in der Cutscene nicht
 * unter das Tor (ROOM1030)."
 *
 * DIE KETTE IM ORIGINAL (RE1.5, analysis/room1030_crawl_mechanism.md, alles BYTE_BELEGT):
 *   AOT-Stempel entity+0x0B (@0x80042f5c/@0x80042fc4)
 *     -> ROOM1030 sub07 @Datei 0x2754: `3d 04 10 / 26 00 05 04 00 10 / 35 10 04`
 *        = Member16 (entity+0x1C4, `sh a2,452(a0)` @0x80041218) |= 0x1000
 *     -> KONSUMENT: die drei STAGE1-Steer-/Decide-Funktionen, identische Bytes
 *          80101ec4: lhu  v0,452(v1)      ; +0x1C4 (HALBWORT)
 *          80101ecc: andi v0,v0,0x1000
 *          80101ed0: beq  v0,zero,0x80101edc
 *          80101ed4: ori  v0,zero,0x1001
 *          80101ed8: sw   v0,4(v1)        ; entity+0x04 = 0x1001  -> Sub-Modus 0x10
 *        (identisch @0x801021c0-e0 und @0x80105784-a4)
 *     -> Sub-Modus 0x10 = TOGGLE FUN_80104f80: +0x93|=1, Clip 0x12, +0x9F=1, Commit
 *        +0x09=0x81 (@0x801050d0-d4), +0x1D7=8 (@0x801050f4), Wort 1 (@0x801050e4)
 *     -> Grid-Wurzel 1 (@0x8010164c-60, Tabelle 0x8011f80c[1] = 0x80101708) = die
 *        Kriech-Maschine; Erstframe FUN_801036dc: +0x94=0x1A (@0x8010371c), +0x8C=0x1E
 *        (@0x801036fc), +0x1D7=8 (@0x8010374c).
 *
 * GEMESSEN WIRD: derselbe Aktor, dieselbe Vorbedingung (anim_flags |= 0x1000), einmal unter
 * RE15_AI_FLAVOR_RE15 und einmal unter RE15_AI_FLAVOR_RE2, jeweils über den ECHTEN Tick-Einstieg
 * re15_enemy_ai_live_tick (dort sitzt die Flavor-Weiche, enemy_ai_common.c:4232) und mit GELADENER
 * Bank (ohne Bank ist clip_len==0 und jede clip-getriebene Phase ist wertlos).
 *
 * Ausgabe pro Flavor: +0x4/+0x5/+0x6, grid_id(+0x9), sca_mask(+0x1D7), motion(+0x94), anim_flags.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* RE1.5-Bank (CDEMD0.EMS des PSX-Baums) für den RE1.5-Flavor. */
static uint8_t *s_re15_blob = NULL;
static int load_bank_re15(uint8_t type)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        s_re15_blob = (uint8_t *)malloc(len);
        memcpy(s_re15_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        re15_tim_t tim = (re15_tim_t){0};
        if (eb && re15_emd_parse_container(s_re15_blob, len, &eb->md1, &eb->skel,
                                           &eb->anim, &tim) == 0) {
            eb->ok = 1; eb->buf = NULL; ok = 1;
            /* eigene Bank 1 (dir[3]/dir[4]) = die Kriech-Clips (0x12/0x1A) */
            re15_emd_parse_own_bank(s_re15_blob, len, &eb->skel_own, &eb->anim_own);
            eb->own_ok = (eb->anim_own.clip_count > 0);
        }
    }
    free(ems);
    return ok;
}

/* RE2-Bank (shared_assets/RE2/CDEMD0.EMS) für den RE2-Flavor. */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

static void dump(const char *tag, const re15_actor_t *e)
{
    printf("    %-22s st=%u s1=0x%02x s2=%u  grid=0x%02x sca=%u motion=0x%02x af=0x%04x frame=%u\n",
           tag, e->state, e->sub_state_1, e->sub_state_2, e->grid_id, e->sca_mask,
           e->motion, e->anim_flags, (unsigned)e->anim_frame);
}

/* Ein Lauf: Aktor in den ROOM1030-Ausgangszustand (Sub 2 = ENGAGE, der Zustand, in dem die
 * wartenden Zombies vor dem Tor stehen — Glied 8 nennt FUN_80102058 als Konsument), Flag setzen,
 * N Ticks fahren. Rückgabe: 1 wenn der Kriech-Commit (grid 0x81 + sca 8 + Clip 0x1A) fällt. */
static int run(re15_ai_flavor_t fl, const char *name, int ticks)
{
    re15_actor_init();
    re15_ai_flavor_set(fl);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[1];
    pl->active = 1; pl->hp = 100; pl->hit_react = 1;      /* blockt jeden Grab-Write */
    pl->x = 0; pl->z = -18000; pl->floor = 0;

    e->active = 1; e->type = 0x16; e->hp = 100;
    e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->grid_id = 0; e->ai_flags = 0; e->anim_flags = 0; e->hit_react = 0;
    e->sca_mask = 4; e->floor = 0; e->x = 0; e->z = -24838;   /* die Warteposition, §18 */
    e->hit_radius_min = 0;                                   /* kein Body-Push in der Messung */

    printf("  [%s]\n", name);
    dump("vor dem Flag", e);

    /* Glied 7: sub07 setzt Member16 |= 0x1000 (das ist der EINZIGE Skript-Eingriff). */
    e->anim_flags |= 0x1000u;

    int commit_tick = -1, sub10_tick = -1;
    for (int t = 1; t <= ticks; t++) {
        re15_enemy_ai_live_tick(1);
        if (sub10_tick < 0 && e->sub_state_1 == 0x10) sub10_tick = t;
        if (commit_tick < 0 && e->grid_id == 0x81)    commit_tick = t;
        if (t == 1 || t == 2) dump(t == 1 ? "nach Tick 1" : "nach Tick 2", e);
    }
    dump("nach allen Ticks", e);
    printf("    -> Sub-Modus 0x10 erreicht: %s%s | Kriech-Commit (grid 0x81): %s%s\n",
           sub10_tick >= 0 ? "JA @Tick " : "NEIN", sub10_tick >= 0 ? "" : "",
           commit_tick >= 0 ? "JA @Tick " : "NEIN", commit_tick >= 0 ? "" : "");
    if (sub10_tick >= 0)  printf("       (Sub 0x10 @Tick %d)\n", sub10_tick);
    if (commit_tick >= 0) printf("       (Commit  @Tick %d, motion=0x%02x, sca=%u)\n",
                                 commit_tick, e->motion, e->sca_mask);
    return (commit_tick >= 0 && e->sca_mask == 8) ? 1 : 0;
}

int main(void)
{
    printf("== ROOM1030-Kriech-Auslöser: RE1.5 gegen RE2 (gleicher Aktor, gleiche Vorbedingung) ==\n\n");

    /* --- RE1.5 --- */
    re15_actor_init();
    int b15 = load_bank_re15(0x16);
    printf("RE1.5-Bank EM016: %s\n", b15 ? "geladen" : "FEHLT (Messung wertlos!)");
    if (b15) {
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        printf("  Clips Haupt=%d, eigen=%d (Kriech-Clip 0x1A fc=%d, Toggle-Clip 0x12 fc=%d)\n",
               eb->anim.clip_count, eb->anim_own.clip_count,
               eb->anim_own.clip_count > 0x1A ? eb->anim_own.clips[0x1A].frame_count : -1,
               eb->anim_own.clip_count > 0x12 ? eb->anim_own.clips[0x12].frame_count : -1);
    }
    int ok15 = run(RE15_AI_FLAVOR_RE15, "RE15-Flavor", 260);

    /* --- RE2 --- */
    re15_actor_init();
    int b2 = load_bank_re2(0x16);
    printf("\nRE2-Bank EM016: %s\n", b2 ? "geladen" : "FEHLT (Messung wertlos!)");
    if (b2) {
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        printf("  Clips Haupt=%d loco=%d own=%d victim=%d\n", eb->anim.clip_count,
               eb->loco_ok ? eb->anim_loco.clip_count : -1,
               eb->own_ok ? eb->anim_own.clip_count : -1,
               eb->victim_ok ? eb->anim_victim.clip_count : -1);
        printf("  ⛔ CLIP-INDEX-VERGLEICH (die RE1.5-Kriechmaschine adressiert BANK-1-Indizes):\n");
        printf("     RE2 eigen-Bank: Clip 0x12 fc=%d, Clip 0x1A fc=%d, Clip 0x1B fc=%d, 0x1C fc=%d\n",
               eb->own_ok && eb->anim_own.clip_count > 0x12 ? eb->anim_own.clips[0x12].frame_count : -1,
               eb->own_ok && eb->anim_own.clip_count > 0x1A ? eb->anim_own.clips[0x1A].frame_count : -1,
               eb->own_ok && eb->anim_own.clip_count > 0x1B ? eb->anim_own.clips[0x1B].frame_count : -1,
               eb->own_ok && eb->anim_own.clip_count > 0x1C ? eb->anim_own.clips[0x1C].frame_count : -1);
        printf("     (RE1.5-Soll aus CDEMD0.EMS EM016 Bank 1: 0x12=98 Hinlegen, 0x1A=99 Kriechen,\n"
               "      0x1B=19 Kriech-Grab, 0x1C=15 Kriech-Lauern)\n");
    }
    int ok2 = run(RE15_AI_FLAVOR_RE2, "RE2-Flavor", 260);

    printf("\n== ERGEBNIS ==\n");
    printf("  RE1.5: Kriecher %s\n", ok15 ? "JA" : "NEIN");
    printf("  RE2  : Kriecher %s\n", ok2  ? "JA" : "NEIN");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}
