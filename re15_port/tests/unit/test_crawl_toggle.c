/* test_crawl_toggle.c — ctest-Gate `unit_crawl_toggle`: die ROOM1030-Kriechtor-ZUSTANDSMASCHINE
 * (Sub-Modus-0x10-Toggle FUN_80104f80 + Grid-Wurzel 1 FUN_80101708 + Kriech-Erstframe
 * FUN_801036dc), byte-true nach analysis/room1030_crawl_mechanism.md Glieder 9/10 und dem
 * B3-Report. Die Bank ist GEMOCKT (Cliplaengen 98/99 wie CDEMD0.EMS Typ 0x16, B3 §6; Frame-
 * Flags synthetisch, Keyframes leer -> der Hand-Lock liefert 0-Delta) — geprueft wird die
 * ZUSTANDSMASCHINE, nicht die Root-Motion (die misst probe_crawl_commit mit der echten Bank).
 *
 * HINWEG:  state=1/+0x5=0x10/grid=0  ->  Phase 0 (@0x80104fcc: +0x93|=1, +0x94=0x12,
 *          +0x95=rng&3, +0x8F=0x0F, +0x9F=1 weil Bit 0x80 clear) FAELLT DURCH in Phase 1
 *          (Dossier-Falle 1) -> Clip-Ende -> Phase 2 Kriech-Commit (@0x801050d0-f4:
 *          grid=0x81, Wort=1, sca_mask=8) -> Grid-1-Tick: Kriech-Erstframe (@0x801036fc-4c:
 *          motion=0x1A, ai_timer=0x1E).
 * RUECKWEG: grid=0x81 + anim_flags-Bit 0x2000 -> Grid-1-DECIDE[0] (FUN_801035f8: 0x2000-Gate
 *          @0x80103690-c4 inkl. +0x1D8-Bit-0x80-Sperre @0x801036b8) -> Wort 0x601 -> f920[6]
 *          = derselbe Toggle, xfer_dir bleibt 0 (Bit 0x80 gesetzt) -> Phase 2 Aufstehen
 *          (@0x80105094-b4: grid=0, Wort=0x201, sca_mask=4). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"

static int s_fail = 0;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  OK   " __VA_ARGS__); printf("\n"); } \
    else      { printf("  FAIL " __VA_ARGS__); printf("\n"); s_fail++; } \
} while (0)

/* Mock der Typ-0x16-BANK-1 (dir[3]/dir[4]): Cliplaengen 98 (0x12) / 99 (0x1A) = die echten
 * CDEMD0.EMS-Werte (B3 §6, selbst geparst); Frame-Flags des Kriech-Clips synthetisch nach dem
 * B3-Muster (Bit 0x2000: Frames 0-21 + 63-98; Bit 0x10000: 0, 24-45, 74-98). Keyframe-Pool
 * leer -> re15_emd_get_keyframe_angles/position liefern 0 -> Hand-Lock-Delta 0. */
static void install_mock_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x16);
    if (!eb) { printf("FAIL Bank-Slot\n"); s_fail++; return; }
    memset(&eb->anim_own, 0, sizeof eb->anim_own);
    memset(&eb->skel_own, 0, sizeof eb->skel_own);
    eb->anim_own.clip_count = 0x2B;             /* 43 wie die echte Bank 1 (B3 §6) */
    eb->anim_own.clips[0x12].first_frame = 0;   eb->anim_own.clips[0x12].frame_count = 98;
    eb->anim_own.clips[0x1A].first_frame = 98;  eb->anim_own.clips[0x1A].frame_count = 99;
    /* Kriech-Grab-/Devour-Clips (echte Laengen aus CDEMD0.EMS dir[3]: 0x1B=19, 0x1C=15,
     * 0x1D=29, 9=65, 0xA=65); Frame-Eintraege bleiben 0 (kf 0, keine Flags). */
    eb->anim_own.clips[0x1B].first_frame = 197; eb->anim_own.clips[0x1B].frame_count = 19;
    eb->anim_own.clips[0x1C].first_frame = 216; eb->anim_own.clips[0x1C].frame_count = 15;
    eb->anim_own.clips[0x1D].first_frame = 231; eb->anim_own.clips[0x1D].frame_count = 29;
    eb->anim_own.clips[0x09].first_frame = 260; eb->anim_own.clips[0x09].frame_count = 65;
    eb->anim_own.clips[0x0A].first_frame = 325; eb->anim_own.clips[0x0A].frame_count = 65;
    eb->anim_own.frame_count = 390;
    for (int f = 0; f < 98; f++) eb->anim_own.frames[f] = 0;          /* Clip 0x12: alle Flags 0 */
    for (int f = 0; f < 99; f++) {
        uint32_t w = 0;
        if (f <= 21 || f >= 63)                       w |= 0x2000u;   /* Hand B (synthetisch) */
        if (f == 0 || (f >= 24 && f <= 45) || f >= 74) w |= 0x10000u; /* Steer-Fenster        */
        eb->anim_own.frames[98 + f] = w;
    }
    eb->skel_own.bone_count = 15;                 /* B3 §6 Armature: 0->{1,4,7}, 7->{8,11,14} */
    { static const int8_t par[15] = { -1,0,1,2, 0,4,5, 0, 7,8,9, 7,11,12, 7 };
      for (int b = 0; b < 15; b++) eb->skel_own.bone_parent[b] = par[b]; }
    eb->own_ok = 1;
    eb->ok = 1;                                   /* re15_enemy_find liefert nur ok-Banken;
                                                   * Container-anim bleibt leer (clip_count 0),
                                                   * re15_zcrawl_anim nimmt own_ok zuerst */
}

int main(void)
{
    re15_actor_init();
    install_mock_bank();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[1];
    pl->active = 1; pl->hp = 100; pl->hit_react = 1;   /* blockt den Grab-Write in DECIDE[0]
                                                        * (@0x8010360c-14: player+0x93 != 0) */
    e->active = 1; e->type = 0x16; e->hp = 100;
    e->state = 1; e->sub_state_1 = 0x10; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->grid_id = 0; e->ai_flags = 0; e->anim_flags = 0; e->hit_react = 0;
    e->sca_mask = 4; e->floor = 0; e->x = 0; e->z = -24000;

    printf("== HINWEG: f890[0x10]-Toggle ==\n");
    re15_enemy_ai_live_active(1);                       /* Tick 1: Phase 0 + Phase 1 im selben Tick */
    CHECK(e->motion == 0x12, "Phase 0: motion==0x12 (@0x80104fec), ist 0x%02x", e->motion);
    CHECK((e->hit_react & 1) != 0, "Phase 0: hit_react|=1 (@0x80104fd4)");
    /* Phase 0 seedet +0x8F=0x0F (@0x8010501c), FAELLT aber in Phase 1 durch (Dossier-Falle 1),
     * deren f314/f3bc den Blend-Zaehler im SELBEN Tick einmal dekrementiert (@0x8001f5b4). */
    CHECK(e->anim_frac == 0x0E, "Phase 0->1: anim_frac==0x0E (0x0F @0x8010501c - same-tick decay), ist %u", e->anim_frac);
    CHECK(e->sub_state_2 == 1, "Phase 0: sub_state_2==1 (@0x8010500c), ist %u", e->sub_state_2);
    CHECK(e->xfer_dir == 1, "HINWEG-Latch: xfer_dir==1 (Bit 0x80 clear, @0x80105050), ist %u", e->xfer_dir);
    CHECK(e->anim_frame >= 1 && e->anim_frame <= 4, "Startframe rng&3 (+1 Advance) in [1..4], ist %u", (unsigned)e->anim_frame);
    CHECK(re15_actor_toggle_reverse(e) == 1, "HINWEG posiert GESPIEGELT (a2=1: Slot fc-+0x95-1 @0x8001f344-54; kf-Daten: frame0 py=-175 LIEGEND -> gespiegelt = Hinlegen)");

    int ticks = 1;
    while (e->grid_id != 0x81 && ticks < 200) { re15_enemy_ai_live_active(1); ticks++; }
    printf("  (Commit nach %d Ticks; Clip 0x12 = 98f, Start rng&3)\n", ticks);
    CHECK(e->grid_id == 0x81, "Kriech-Commit: grid_id==0x81 (@0x801050d0-d4), ist 0x%02x", e->grid_id);
    CHECK(e->sca_mask == 8, "Kriech-Commit: sca_mask==8 (@0x801050f4), ist %u", e->sca_mask);
    CHECK(e->state == 1 && e->sub_state_1 == 0 && e->sub_state_2 == 0,
          "Kriech-Commit: Wort 1 -> state=1/+0x5=0/+0x6=0 (@0x801050e4), ist %u/%u/%u",
          e->state, e->sub_state_1, e->sub_state_2);
    CHECK((e->hit_react & 1) == 0, "Kriech-Commit: hit_react-Bit 0 geloescht (@0x80105100-10)");
    CHECK(ticks <= 100, "Commit binnen ~98 Ticks (fc=98), ist %d", ticks);

    printf("== GRID-1-Tick: Kriech-Erstframe (FUN_801036dc) ==\n");
    re15_enemy_ai_live_active(1);
    CHECK(e->motion == 0x1A, "Erstframe: motion==0x1A (@0x8010371c), ist 0x%02x", e->motion);
    CHECK(e->ai_timer == 0x1E, "Erstframe: ai_timer==0x1E (+0x8C Halbwort @0x801036fc), ist %d", (int)e->ai_timer);
    CHECK(e->sub_state_2 == 1, "Erstframe: sub_state_2==1 (@0x8010370c), ist %u", e->sub_state_2);
    CHECK(e->sca_mask == 8, "Erstframe: sca_mask==8 (@0x8010374c)");
    CHECK(e->anim_frac == 0x0E, "Erstframe: +0x8F=0x0F (@0x8010373c) - same-tick decay = 0x0E, ist %u", e->anim_frac);
    CHECK(e->grid_id == 0x81 && e->state == 1 && e->sub_state_1 == 0, "Kriecher bleibt in Grid 1 / Zeile 0");

    printf("== +0x1D8-Bit-0x80-Sperre (permanenter Kriecher, @0x801036b8) ==\n");
    e->anim_flags |= 0x2000;                       /* sub05-Wirkung: (x & 0x0fff) | 0x2000 */
    e->ai_flags |= 0x80;
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 0 && e->motion == 0x1A,
          "Sperre haelt: 0x2000 gesetzt, aber ai_flags&0x80 -> KEIN 0x601, ist +0x5=%u", e->sub_state_1);
    e->ai_flags &= (uint16_t)~0x80u;

    printf("== RUECKWEG: 0x2000-Gate -> f920[6]-Toggle -> Aufstehen ==\n");
    re15_enemy_ai_live_active(1);                  /* DECIDE[0] -> 0x601; ANIMATE[6] Phase 0+1 */
    CHECK(e->sub_state_1 == 6, "DECIDE[0]: Wort 0x601 (@0x801036c0-c4) -> +0x5==6, ist %u", e->sub_state_1);
    CHECK(e->motion == 0x12, "Toggle Phase 0: motion==0x12, ist 0x%02x", e->motion);
    CHECK(e->xfer_dir == 0, "RUECKWEG-Latch: xfer_dir bleibt 0 (Bit 0x80 gesetzt, @0x80105044-48)");
    CHECK((e->hit_react & 1) != 0, "Toggle Phase 0: hit_react|=1");
    CHECK(re15_actor_toggle_reverse(e) == 0, "RUECKWEG posiert VORWAERTS-wie-gespeichert (a2=0: Slot=+0x95 @0x8001f35c; frame0 py=-175 -> 97 py=-1744 = Aufstehen)");

    ticks = 1;
    while ((e->grid_id & 0x80) != 0 && ticks < 200) { re15_enemy_ai_live_active(1); ticks++; }
    printf("  (Aufstehen nach %d Ticks)\n", ticks);
    CHECK(e->grid_id == 0, "Aufstehen: grid_id==0 (kein 0x80; @0x80105094), ist 0x%02x", e->grid_id);
    CHECK(e->sca_mask == 4, "Aufstehen: sca_mask==4 (@0x801050b4), ist %u", e->sca_mask);
    CHECK(e->state == 1 && e->sub_state_1 == 2 && e->sub_state_2 == 0,
          "Aufstehen: Wort 0x201 (@0x801050a4) -> state=1/+0x5=2/+0x6=0, ist %u/%u/%u",
          e->state, e->sub_state_1, e->sub_state_2);
    CHECK((e->hit_react & 1) == 0, "Aufstehen: hit_react-Bit 0 geloescht (@0x80105100-10)");

    printf("== KRIECH-GRAB (Grid-1 Zeilen 1/2 = FUN_80103b94) + DEVOUR (3/4 = FUN_80104548) ==\n");
    /* Kriecher zuruecksetzen + Spieler NAH und FRONTAL: DECIDE[0]-Nah-Zweig
     * (player+0x93==0 @0x8010360c-14, +0x1d0<0x4b0 @0x80103628-34, arc 0x200 @0x80103640-48,
     * gleiche Etage @0x80103650-68) -> Wort 0x101/0x201 @0x80103670-8c. */
    pl->hit_react = 0; pl->hp = 100; pl->x = 0; pl->z = 800; pl->floor = 0;
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 1; e->sub_state_3 = 0;
    e->grid_id = 0x81; e->sca_mask = 8; e->motion = 0x1A; e->anim_frame = 20;
    e->anim_flags = 0; e->ai_flags = 0; e->hit_react = 0; e->anim_frac = 0;
    e->x = 0; e->z = 0; e->floor = 0;
    e->ai_dist = 0x400;                                 /* +0x1d0 < 0x4b0 (Sonde setzt direkt;
                                                         * live_tick wuerde ihn berechnen) */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff);
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 1 || e->sub_state_1 == 2,
          "Nah-Zweig: Wort (aligned+1)<<8|1 -> +0x5==1/2 (@0x80103670-8c), ist %u", e->sub_state_1);
    CHECK(e->motion == 0x1B, "Grab [0]: motion==0x1B (@0x80103c1c-20), ist 0x%02x", e->motion);
    CHECK((e->hit_react & 1) && (pl->hit_react & 1),
          "Grab [0]: self+0x93|=1 (@0x80103c68-74) + player+0x93|=1 (@0x80103cc4-e0)");
    CHECK((e->ai_flags & 1) != 0, "Grab [0]: +0x1D8|=1 (@0x80103d00-10)");
    CHECK(re15_player_is_grabbed() == 1, "Grab [0]: cmd-5-Pin (aca58 @0x80103cbc-c0)");
    CHECK(e->sub_state_2 == 1, "Grab [0] faellt in [1] (Sprungtabelle @0x8010007c), s2==%u", e->sub_state_2);
    {
        int t = 0;
        while (e->sub_state_2 != 3 && t < 40) { re15_enemy_ai_live_active(1); t++; }
        printf("  (Biss-Loop nach %d Ticks; Clip 0x1B = 19f)\n", t);
    }
    CHECK(e->sub_state_2 == 3 && e->motion == 0x1C,
          "Grab [2]: motion==0x1C (@0x80103d78-7c) -> Loop [3], ist s2=%u mo=0x%02x",
          e->sub_state_2, e->motion);
    CHECK(e->ai_timer == 100 || e->ai_timer == 99, "Grab [2]: ESCAPE +0x9C=100 (@0x80103d88-8c), ist %d", (int)e->ai_timer);
    CHECK(e->grab_kill_ctr <= 0x5a && e->grab_kill_ctr >= 0x58,
          "Grab [2]: KILL-Budget +0x9E=0x5A (@0x80103dd0-d4), ist %d", (int)e->grab_kill_ctr);
    CHECK(pl->hp == 95, "Grab [2]: player.hp -= 5 (@0x80103ddc-ec), ist %d", (int)pl->hp);
    e->anim_flags |= 0x2000;                            /* sub05-Wirkung waehrend des Grabs */
    for (int t = 0; t < 3; t++) re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 <= 2 && e->sub_state_1 >= 1,
          "0x2000 in Sub 1/2 IGNORIERT (DECIDE[1,2]=jr ra @0x80103b8c-90), +0x5 ist %u", e->sub_state_1);
    pl->hp = -1;                                        /* Kill-Bedingung (@0x80103ef8-f04) */
    re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 3 || e->sub_state_1 == 4,
          "DEVOUR-Handoff: +0x5 += 2 (@0x80103f18-24), +0x6=0 (@0x80103f34), ist %u/%u",
          e->sub_state_1, e->sub_state_2);
    re15_enemy_ai_live_active(1);
    CHECK(e->motion == (uint16_t)(e->sub_state_1 + 6),
          "Devour [0]: motion==(+0x5)+6 (@0x80104588-94; Sub 3 -> Clip 0x09), ist 0x%02x", e->motion);
    e->anim_flags |= 0x2000;
    for (int t = 0; t < 3; t++) re15_enemy_ai_live_active(1);
    CHECK(e->sub_state_1 == 3 || e->sub_state_1 == 4,
          "0x2000 in Sub 3/4 IGNORIERT (DECIDE[3,4]=jr ra @0x80104540-44) — Aufstehen NUR aus Sub 0, ist %u",
          e->sub_state_1);

    printf("\n%s (%d Fehler)\n", s_fail ? "FAILED" : "PASSED", s_fail);
    return s_fail ? 1 : 0;
}
