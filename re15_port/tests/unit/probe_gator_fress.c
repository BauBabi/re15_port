/* probe_gator_fress.c — MESS-SCHIENE des ROOM2090-Fress-Finishers (Befund B,
 * analysis/befunde_runde4_2026-09-12/fress-sync.md).
 *
 * ANLASS (Nutzer 2026-09-12): "leon steckt bei der Todesanimation immer noch
 * nicht dann im Aligator Maul und wird rumgeschleudert, das ist noch sehr
 * asynchron."
 *
 * Der Probe-Lauf treibt den ECHTEN Boss-Tick (re15_gator_boss_tick + die
 * Dispatcher-SCA-Klemme wie probe_gator_sweep.c, KEIN Nachbau) mit GELADENER
 * RE2-EM23-Bank (22 Bones — ohne Bank faellt re15_enemy_bone_world_pos auf
 * die Wurzel zurueck und taeuscht, Lehre reai-v2-re2-trace-datei) bis in die
 * FRESS-Sequenz: Leon steht mit hp=30 im Wasser, der erste Biss (50 Schaden,
 * dmg_table[5] @DAT_8006f418) macht hp<0 -> gb_biss_abschluss -> GBP_FRESSEN.
 * Die Messreihe selbst schreibt der Engine-Tick als FSYNC-Zeilen nach
 * gator_boss.log (pro Frame: analytischer Bahn-Anker kx/kz, Leon-Wurzel,
 * gerenderte Kiefer-Bones 6/7 inkl. pitch/jaw-vz-Hook + Render-Scale,
 * 3D-Abstand Leon<->Maulmitte).
 *
 * Bestehen = die Sequenz laeuft bis zum Verschlingen (pl->no_draw == 1).
 * Die eigentliche Auswertung (Asynchronitaet beziffern) liest gator_boss.log.
 */
#include "re15_rdt.h"
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

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

/* RE2-Bank aus CDEMD0.EMS laden — Muster test_re2_zombie_teardeath.c:139-152. */
static int load_re2_bank(uint8_t type)
{
    static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
        s_ems_sz = (long)n; }
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

int main(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");     /* aggro sofort */
#else
    setenv("RE15_GB_TEST", "1", 1);
#endif

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) {
        printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    if (!load_re2_bank(0x23u)) {
        printf("FAIL: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); return 1; }
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
        printf("EM23-Bank: bones=%d clips=%d\n",
               eb ? eb->skel.bone_count : -1, eb ? eb->anim.clip_count : -1);
        if (!eb || eb->skel.bone_count != 22) {
            printf("FAIL: erwartet 22 Bones (EM23-Rig)\n"); return 1; }
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Gator-Neuspawn wie main.c / probe_gator_sweep.c. */
    int slot = RE15_ACTOR_MAX - 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);

    /* Leon im Wasser, hp=30: der erste Biss (50) toetet -> FRESSEN. */
    pl->active = 1; pl->type = 0; pl->hp = 30;
    pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;

    re15_gator_boss_tick(slot);           /* Frame 0 = Boss-INIT (setzt Lauerplatz) */
    e->x = -6000; e->z = -22000; e->y = -1200;

    int fress_gesehen = 0;
    for (int f = 0; f < 3000; f++) {
        if (pl->hp >= 0) {                /* Statue nur solange er lebt — danach
                                           * fuehrt die FRESS-Sequenz Leon selbst */
            pl->x = -4000; pl->z = -22000; pl->y = 0;
            pl->state = 0; pl->motion = 0; pl->hit_react = 0;
        }
        int32_t ox = e->x, oz = e->z;
        re15_gator_boss_tick(slot);
        if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
            int32_t nx = e->x, nz = e->z;   /* Dispatcher-Klemme 1:1 (sweep) */
            re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                           e->hit_radius_min, e->y, 4u);
            e->x = nx; e->z = nz;
        }
        if (re15_gator_fressen_hold()) fress_gesehen = 1;
        if (pl->no_draw) break;           /* Verschlungen = Sequenz komplett */
    }

    if (!fress_gesehen) { printf("FAIL: FRESSEN nie erreicht\n"); return 1; }
    if (!pl->no_draw)   { printf("FAIL: Verschlingen (no_draw) nie erreicht\n"); return 1; }
    printf("OK: FRESS-Sequenz komplett - FSYNC-Messreihe in gator_boss.log\n");
    return 0;
}
