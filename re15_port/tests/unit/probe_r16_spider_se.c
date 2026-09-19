/* probe_r16_spider_se.c - MESS-SONDE Runde 16 (2026-09-19), kein add_test.
 *
 * Nutzer: "Und auch die Spinne hat noch nicht ihren Angriffssound."
 *
 * Erzwingt die Angriffsphase der RE2-Spinne (enemy_ai_re2_spider.c, Boden-Modus
 * sub_state_1 = 7 = re2s_m0_attack @0x80100D48-F4C) in ROOM2090 mit dem Spieler in
 * Reichweite und protokolliert JEDEN SE-Ruf des Spinnen-Hooks (id, flag2000, Frame,
 * Zustand) sowie den Bank-Latch. Szenario A: natuerlicher Ablauf ab ACTIVE/sub 0 mit
 * Spieler nah (1500 Frames). Szenario B: sub_state_1 = 7 direkt (Sprung -> Biss ->
 * re2s_attack -> SE 1 @0x80105B34-38). Szenario C: Decken-/Faden-Modus (mode 2, Abseilen ->
 * SE 5/6 @0x801032F0-304 sind Landungs-SEs, kein Angriff).
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
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_re2spider_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int), int baby);
extern int  re15_re2spider_tick(int slot);
extern int  re15_re2spider_owns(const re15_actor_t *e);

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

static int s_bank_sel = -1, s_frame = 0, s_calls = 0, s_mismatch = 0, s_bites_se1 = 0;
static re15_actor_t *s_e = NULL, *s_pl = NULL;
static void spy_bank(int bank)
{
    if (bank != s_bank_sel) printf("   [bank_fn] Latch %d -> %d\n", s_bank_sel, bank);
    s_bank_sel = bank;
}
static void spy_se(int id, int flag2000)
{
    int ok = (s_bank_sel == 11);         /* Phase 2: zustaendige Bank der Spinne = 11 */
    s_calls++;
    if (!ok) s_mismatch++;
    if (id == 1 && !flag2000) s_bites_se1++;
    printf("   SE F%-5d id=%d flag=%d Latch=Bank %d %s | spinne state=%d sub=%d/%d/%d mode=%d motion=%d af=%u pos=(%d,%d,%d) | pl hp=%d\n",
           s_frame, id, flag2000, s_bank_sel, ok ? "(zustaendig)" : "MISMATCH",
           s_e->state, s_e->sub_state_1, s_e->sub_state_2, s_e->sub_state_3,
           s_e->re2s_mode222, (int)s_e->motion, (unsigned)s_e->anim_frame, s_e->x, s_e->y, s_e->z, s_pl->hp);
}
/* Phase 2: der GATOR-Hook wird wie im Spiel NACH der Spinne registriert (main.c laedt den
 * Boss im letzten Slot, Roster aufsteigend) - vor dem Fix stand der Latch dadurch auf 17. */
extern void re15_gator_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int));
static void spy_gator_se(int id, int flag2000) { (void)id; (void)flag2000; }

static void run(const char *name, re15_actor_t *e, re15_actor_t *pl, int frames, int force_sub1)
{
    int f, c0 = s_calls, hp0 = pl->hp, seen7 = 0, bites = 0, lasthp = pl->hp;
    printf("\n--- %s ---\n", name);
    for (f = 0; f < frames; f++) {
        s_frame = f;
        if (force_sub1 >= 0 && f == 0) { e->state = 1; e->sub_state_1 = (uint8_t)force_sub1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
        e->hit_react = 0; pl->hit_react = 0; pl->state = 0;
        e->re2z_f10e = (uint16_t)e->grid_id;
        re15_re2spider_tick(e - g_actors);
        if (e->sub_state_1 == 7 && e->re2s_mode222 == 0) seen7++;
        if (pl->hp < lasthp) { bites++; printf("   BISS F%d: hp %d -> %d (sub=%d/%d)\n", f, lasthp, pl->hp, e->sub_state_1, e->sub_state_2); lasthp = pl->hp; }
        if (pl->hp < 0) { pl->hp = 200; lasthp = 200; }
        if (f < 3 || (f % 300) == 0)
            printf("   F%-5d state=%d sub=%d/%d/%d mode=%d motion=%d af=%u pos=(%d,%d,%d) dist=%u\n", f, e->state,
                   e->sub_state_1, e->sub_state_2, e->sub_state_3, e->re2s_mode222, (int)e->motion,
                   (unsigned)e->anim_frame, e->x, e->y, e->z, (unsigned)e->ai_dist);
    }
    printf("   => SE-Rufe: %d, Frames in sub7(Angriff): %d, Bisse: %d, hp %d->%d\n", s_calls - c0, seen7, bites, hp0, pl->hp);
}

int main(void)
{
    size_t n = 0, es = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
    re15_enemy_bank_t *eb;
    re15_actor_t *pl, *e; int slot, c, f;
    if (!buf || !ems) { printf("FAIL assets\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    eb = re15_enemy_alloc(0x25u);
    if (!eb || re2_ems_load_bank(ems, es, 0x25, eb, NULL) != 0) { printf("FAIL Bank 0x25\n"); return 1; }
    eb->buf = NULL; eb->ok = 1;
    printf("EMS25-Bank: clips=%d bones=%d\n", eb->anim.clip_count, eb->skel.bone_count);
    for (c = 0; c < eb->anim.clip_count; c++) {
        int nse = 0; printf("   clip %2d: %3d F", c, eb->anim.clips[c].frame_count);
        for (f = 0; f < eb->anim.clips[c].frame_count; f++) {
            uint32_t w = eb->anim.frames[eb->anim.clips[c].first_frame + f];
            if (w & 0x08000000u) { printf(" f%d:SE%u", f, w >> 28); nse++; }
        }
        printf("%s\n", nse ? "" : "  (keine Frame-SEs)");
    }
    re15_re2spider_audio_hook(spy_se, spy_bank, 0);
    re15_gator_audio_hook(spy_gator_se, spy_bank);      /* Spiel-Reihenfolge: Gator zuletzt */
    printf("Latch nach Registrierung (Spinne, dann Gator wie main.c): Bank %d\n", s_bank_sel);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 2; e = &g_actors[slot];
    s_e = e; s_pl = pl;

    /* Spinne auf der Plattform (Memory reai-v2-gator-bosskampf: Sitz (-700,-17800) y=-1800),
     * Spieler 900 Einheiten davor auf derselben Ebene. */
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x25u; e->x = -700; e->y = -1800; e->z = -17800; e->grid_id = 0;
    e->state = 0; e->em_flag_id = 0xFF; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x25u);
    pl->active = 1; pl->type = 0; pl->hp = 200; pl->x = 200; pl->z = -17800; pl->y = -1800; pl->floor = 1;
    printf("owns=%d flavor-re2-for-type=%d\n", re15_re2spider_owns(e), re15_ai_re2_for_type(0x25u));

    run("A: natuerlich ab INIT, Spieler 900 vor der Spinne, 1500 Frames", e, pl, 1500, -1);

    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x25u; e->x = -700; e->y = -1800; e->z = -17800; e->grid_id = 0;
    e->state = 0; e->em_flag_id = 0xFF; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x25u);
    pl->x = 200; pl->z = -17800; pl->y = -1800; pl->hp = 200;
    re15_re2spider_tick(slot);                       /* INIT -> ACTIVE */
    run("B: sub_state_1 = 7 (ANGRIFF) erzwungen, Spieler 900 davor, 600 Frames", e, pl, 600, 7);

    /* B2: Angriff mehrfach, Spieler jeweils neu hinstellen */
    {   int k;
        for (k = 0; k < 3; k++) {
            pl->x = e->x + 900; pl->z = e->z; pl->y = e->y; pl->hp = 200;
            run("B2: erneut sub 7, Spieler 900 in Blickrichtung", e, pl, 300, 7);
        }
    }
    printf("\nGESAMT SE-Rufe: %d (Latch Bank %d) | PHASE 2: Latch==zustaendig(11) bei %d/%d Rufen, "
           "MISMATCH %d, Biss-SE-1-Rufe %d\n", s_calls, s_bank_sel, s_calls - s_mismatch, s_calls,
           s_mismatch, s_bites_se1);
    return 0;
}
