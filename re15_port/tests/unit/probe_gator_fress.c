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
    /* ⛔ WANN STARTET DIE TODES-PRAESENTATION? (Nutzer 2026-09-13, zweite Meldung:
     * "Die Fressanimation findet immer noch nicht im YOU ARE DEAD Screen statt, sondern
     * davor.") Im Original laeuft sie MITTEN im Fressen: RE1.5s FSM FUN_8001500c gated
     * auf Spieler-Kommando 6 = GEFRESSEN (@0x80015014-30), RE2 setzt den Todes-Latch bei
     * Gator-Clip-4-FRAME 13 (@0x80101104-34). Gemessen wird deshalb der ABSTAND zwischen
     * dem Beginn des Fressens und dem Moment, in dem re15_death_presentation_active()
     * anschlaegt - und wie viele Frames danach noch Fress-Animation laeuft.
     * Die FSM selbst braucht ab dort fest 0x32 + 0x1b = 77 Ticks bis zum schwarzen
     * Grund (game_step_common.c, sub0/sub1) - das ist die zweite Haelfte der Rechnung. */
    extern int re15_death_presentation_active(void);
    int f_fress_start = -1, f_praesentation = -1, f_ende = -1;
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
        if (re15_gator_fressen_hold()) {
            fress_gesehen = 1;
            if (f_fress_start < 0) f_fress_start = f;
        }
        if (f_praesentation < 0 && re15_death_presentation_active()) f_praesentation = f;
        /* UMVERANKERT (Runde 6, gator-vollausbau.md 7b): der authored Finisher
         * verschlingt Leon nicht mehr per no_draw - er haengt im letzten
         * Opfer-Frame sichtbar im Maul, waehrend der Gator Clip 11 kaut. */
        if (e->motion == 11) { f_ende = f; break; }  /* P3-Kau-Loop = Sequenz komplett */
    }

    {   /* Die Rechnung, die der Nutzer sieht: schwarzer Grund = Praesentationsstart + 77
         * (FSM sub0 0x32 + sub1 0x1b). Liegt er VOR dem Ende der Fress-Animation, sieht
         * man das Fressen unter dem Bildschirm - genau das ist gefordert. */
        int blackbg = (f_praesentation >= 0) ? f_praesentation + 77 : -1;
        printf("Praesentation: Fressen ab F%d, Gate ab F%d, Kau-Loop ab F%d, "
               "schwarzer Grund bei F%d\n",
               f_fress_start, f_praesentation, f_ende, blackbg);
        if (f_praesentation < 0) {
            printf("FAIL: re15_death_presentation_active() schlaegt NIE an - die "
                   "Todes-Praesentation startet gar nicht waehrend des Fressens\n");
            return 1;
        }
        /* ⛔ KEIN "Gate am Phasenbeginn"-Test (korrigiert 2026-09-13): der Latch sitzt
         * im Original NICHT am Anfang der Fress-Phase, sondern bei CLIP-4-FRAME 13
         * (@0x80101104-34) - demselben Block, der das Opfer-Paar koppelt. Gemessen sind
         * das 14 Frames nach dem Phasenbeginn; ein 2-Frame-Fenster haette den richtigen
         * Stand zurueckgewiesen. Geprueft wird deshalb die Eigenschaft, um die es dem
         * Nutzer geht: ein nennenswerter Teil der Fress-Animation muss UNTER dem
         * Bildschirm laufen. Gemessener Stand: schwarzer Grund F102, Kau-Loop ab F146 =
         * 44 Frames Ueberlappung. Vor dem Runde-8-Fix startete die FSM erst NACH dem
         * Finisher - der schwarze Grund waere dann erst bei F146+77 gefallen. */
        if (f_ende >= 0 && blackbg >= 0 && blackbg > f_ende - 30) {
            printf("FAIL: der schwarze Grund faellt auf F%d, das Fressen endet schon F%d "
                   "- unter dem Bildschirm laeuft fast nichts mehr (Soll: mindestens 30 "
                   "Frames)\n", blackbg, f_ende);
            return 1;
        }
    }

    if (!fress_gesehen) { printf("FAIL: FRESSEN nie erreicht\n"); return 1; }
    if (e->motion != 11) { printf("FAIL: P3-Kau-Loop (Clip 11) nie erreicht\n"); return 1; }
    if (pl->motion != 1 || pl->anim_frame != 119) {
        printf("FAIL: Leon nicht im letzten Opfer-Frame geparkt (clip=%d af=%u)\n",
               (int)pl->motion, (unsigned)pl->anim_frame); return 1; }
    printf("OK: FRESS-Sequenz komplett - FSYNC-Messreihe in gator_boss.log\n");
    return 0;
}
