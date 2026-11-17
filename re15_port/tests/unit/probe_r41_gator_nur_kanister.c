/* probe_r41_gator_nur_kanister.c — Riegel K-12: der ROOM2090/2091-Alligator stirbt NUR durch den Gaskanister.
 *
 * NUTZERWUNSCH 2026-11-17, woertlich: "überprüfe noch mal das der alligator wirklich nur mit den gascontainer getötet werden
 * kann. Auf keinen Fall ander."
 *
 * Treibt den ECHTEN Boss-Takt (re15_gator_boss_tick + Dispatcher-Klemme wie probe_gator_fress.c / test_r41_re2gator.c, kein
 * Nachbau) mit geladener RE2-EM23-Bank (22 Bones) in ROOM2090. Der Schaden kommt ueber die echten Eingaenge von re15_damage.c:
 *   Rakete       re15_re2_gl_apply(P, Gier, {-800,0,400,200}, 0x30011) = RE2 Op 24 Gegnerkontakt (`lui a3,0x3 / ori a3,a3,0x11`
 *                @0x8001f7b0/c0, Box @0x80010908, re2_fx.c op_24) -> Zeile 17 (re15_rakete_schaden, Typ 0x23: 110 @0x800A4B54)
 *   Handgranate  re15_granate_r35_explosion(P, Gier, 2) = RE2 Op 47 (Box @0x80010918 an P und P+900) -> RE1.5-Gegnerzweig Art 2
 *                (re15_granate_schaden = D_G 251)
 *   dieselben Arten direkt im RE1.5-Gegnerzweig FUN_80012d60 (re15_enemy_take_damage Art 9 / Art 2)
 *   kleine Treffer re15_enemy_take_damage Art 0 (DAT_8006f418[0] = 10 @0x8006f418)
 * P = Kastenmitte des Gators: Lage + gedrehter Kastenversatz wie re15_hitbox_versatz (FUN_8002b498), Hoehe + hit_offset_y.
 *
 *   Teil A  nach dem Intro mehr als 30000 Schadenspunkte (Start 25240 = RE15_GR2_HP_INIT), je Treffer vier Boss-Takte. Leon steht
 *           als Statue (HP je Takt 100) bis zum ersten abgefangenen Tod im Nordbecken, danach wechselt er alle 100 Treffer die
 *           Lage (Nordbecken, Plattform oben, Suedbecken, Westkanal; mindestens 400 Treffer nach dem ersten Tod): der Gator jagt
 *           ihn, dasht, beisst, belagert, flincht - die Phasen laufen natuerlich.
 *           Nach JEDEM Takt: keine Todesrolle (Phase GBP_DIE/GBP_DEAD, Clip 7 - einziger Schreiber `e->motion = 7` in GBP_DIE),
 *           kein Zustand 3/7, HP >= 0; nach jedem toedlichen Treffer ausserhalb der Unverwundbarkeit (FLINCH/RE2_TOD/Tauchgang
 *           nehmen den Schaden selbst zurueck) HP genau 1; kein R1 und kein R1-Tod (kein Kanister im Becken).
 *           Am Ende: re15_gator_boss_nur_kanister_zaehler() == Zahl der toedlichen Treffer > 0, mindestens 20 davon ausserhalb
 *           der Unverwundbarkeit, toedliche Treffer in mindestens 3 Boss-Phasen, und Rakete/Granate ueber Applier bzw. Explosion
 *           haben je mindestens einmal getroffen.
 *           HP 0 ist kein Tod (Todesregel `bgez` @0x80013014: tot erst bei HP < 0) - darum HP >= 0 je Takt und HP == 1 nach dem
 *           Abfangen, nicht HP >= 1 vor dem ersten toedlichen Treffer.
 *   Teil B  mit HP 1 nimmt er den Kanister ins Maul (Stufe 7, Weg wie test_r41_re2gator Teil kanister: Statue, Kanister 8800 x
 *           Skala voraus im Fenster 8601..8999 @0x80100ad8-ec); der naechste Treffer ist selbst toedlich (1 - 10 < 0, Zustand 3)
 *           und startet trotzdem R1 (HURT-Selektor +0x226 == 7 @0x80101ef4-fc): Clip 9, Kanister Stufe 13, Ende in der Leiche
 *           (re15_gr2_tot, P3 @0x80102654) - ohne Todesrolle.
 *
 * Aufruf ohne Argument: A, dann B im selben Kampf (ctest unit_r41_gator_nur_kanister).
 * Argument "b": nur B, HP 1 als Vorbedingung direkt gesetzt (Muster Messhaken RE15_K2080_GATOR_HP) - fuer die Gegenprobe am
 * Stand vor K-12, an dem schon Teil A abbricht.
 * R41_NK_LOG=1: gator_boss.log schreiben (sonst RE15_GB_STUMM=1).
 */
/* Linux (glibc, -std=c11 ohne Erweiterungen): setenv/unsetenv nur mit Feature-Makro deklariert - Muster test_r41_re2gator.c.
 * Bau-Umgebung, kein Spielverhalten. */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
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
#include "re15_gator_re2_r41.h"
#include "re15_granate_r35.h"
#include "re15_door_seq.h"      /* re15_door_rotmatrix: Versatz-Drehung wie re15_hitbox_versatz */
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Boss-Phasen = enum in enemy_ai_boss_gator.c (OFF 0, LURK 1, CHASE 2, LUNGE 3, FLINCH 4, GUARD 5, CROSS_APPR 6, CROSS 7,
 * FRESSEN 8, DIE 9, DEAD 10, RE2_INTRO 11, RE2_DASH 12, RE2_TOD 13); Messhaken re15_gator_boss_phase (Header: 2 CHASE, 5 GUARD,
 * 7 CROSS). */
enum { PH_CHASE = 2, PH_FLINCH = 4, PH_DIE = 9, PH_DEAD = 10, PH_RE2_TOD = 13, PH_N = 14 };
static const char *const k_ph_name[PH_N] = { "OFF", "LURK", "CHASE", "LUNGE", "FLINCH", "GUARD", "CROSS_APPR", "CROSS", "FRESSEN",
                                             "DIE", "DEAD", "RE2_INTRO", "RE2_DASH", "RE2_TOD" };
static const char *ph_name(int ph) { return (ph >= 0 && ph < PH_N) ? k_ph_name[ph] : "?"; }

/* Schadens-Arten der Sonde (Eingaenge s. Kopf) */
enum { A_RAKETE_APPLIER = 0, A_GRANATE_EXPLOSION, A_RAKETE_ART9, A_GRANATE_ART2, A_KLEIN, A_N };
static const char *const k_art_name[A_N] = { "Rakete (RE2-Applier 0x30011)", "Handgranate (Explosion Op 47)",
                                             "Rakete (Gegnerzweig Art 9)", "Handgranate (Gegnerzweig Art 2)",
                                             "kleiner Treffer (Art 0)" };

static int fails = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

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
/* RE2-Bank aus CDEMD0.EMS laden - Muster probe_gator_fress.c. */
static int load_re2_bank(uint8_t type)
{
    static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
    if (!s_ems) { size_t n = 0; s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static int slot;
static re15_actor_t *e, *pl;
static int32_t s_leon_x, s_leon_y, s_leon_z;

/* Leon als Statue (lebt immer: HP je Takt 100, damit der Biss nie in FRESSEN fuehrt und R1 nicht am Spieler-Tor
 * `pl->hp < 0` @0x80101ee0-ec scheitert). */
static void leon_statue(void)
{
    pl->x = s_leon_x; pl->z = s_leon_z; pl->y = s_leon_y; pl->floor = 0;
    pl->state = 0; pl->motion = 0; pl->hit_react = 0; pl->hp = 100;
}
/* Ein Spielbild: Boss-Takt + Dispatcher-Klemme (enemy_ai_common.c Typ 0x23) + Abzug der RE2-GL-Sperre (re15_game_step ruft
 * re15_re2_gl_sperre_tick hinter re15_enemy_ai_run_all, re15_damage.h). */
static void tick(void)
{
    leon_statue();
    int32_t ox = e->x, oz = e->z;
    re15_gator_boss_tick(slot);
    if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
        int32_t nx = e->x, nz = e->z;
        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
        e->x = nx; e->z = nz;
    }
    re15_re2_gl_sperre_tick();
}

/* Kastenmitte des Gators = Lage + Versatz (hit_offset_x/z) gedreht um die Gier, wie re15_hitbox_versatz (FUN_8002b498:
 * RotMatrix (0, Gier, 0), ox = R0*vx + R2*vz, oz = R6*vx + R8*vz), Hoehe = y + hit_offset_y (Bandmitte des Kandidaten-Tors
 * @0x8004716c-a4). */
static void gator_mitte(int32_t P[3])
{
    int16_t R[9];
    const uint16_t rot[3] = { 0u, (uint16_t)e->rot_y, 0u };
    re15_door_rotmatrix(rot, R);
    P[0] = e->x + (int32_t)(((int64_t)R[0] * e->hit_offset_x + (int64_t)R[2] * e->hit_offset_z) >> 12);
    P[1] = e->y + e->hit_offset_y;
    P[2] = e->z + (int32_t)(((int64_t)R[6] * e->hit_offset_x + (int64_t)R[8] * e->hit_offset_z) >> 12);
}
static void treffer(int art)
{
    int32_t P[3];
    gator_mitte(P);
    switch (art) {
    case A_RAKETE_APPLIER: {
        static const int16_t box[4] = { -800, 0, 400, 200 };            /* Op 24 Box @0x80010908 (re2_fx.c op_24) */
        (void)re15_re2_gl_apply(P, (int16_t)e->rot_y, box, 0x30011u);
        break; }
    case A_GRANATE_EXPLOSION: (void)re15_granate_r35_explosion(P, (int16_t)e->rot_y, 2u); break;
    case A_RAKETE_ART9:       (void)re15_enemy_take_damage(e, 9u); break;
    case A_GRANATE_ART2:      (void)re15_enemy_take_damage(e, 2u); break;
    default:                  (void)re15_enemy_take_damage(e, 0u); break;
    }
}

/* Nach jedem Takt: der Gator lebt ohne Kanister. 0 = verletzt (Meldung gedruckt). */
static int lebt(const char *wo, int nr)
{
    const int ph = re15_gator_boss_phase(e);
    if (ph == PH_DIE || ph == PH_DEAD || e->motion == 7 || e->state == 3 || e->state == 7 || e->hp < 0) {
        printf("FAIL %s Nr %d: Todesrolle/Leiche statt Kanister-Explosion (R1) - Phase %s, Clip %d Bild %u, Zustand %d, HP %d\n", wo, nr, ph_name(ph),
               (int)e->motion, (unsigned)e->anim_frame, (int)e->state, (int)e->hp);
        fails++;
        return 0;
    }
    return 1;
}
/* Gegenprobe-Beleg: nach einer Verletzung weiter takten und zeigen, ob die Todesrolle in die Leiche (Zustand 7) laeuft. */
static void todesrolle_zeigen(void)
{
    int leiche = -1;
    for (int f = 0; f < 240 && leiche < 0; f++) { tick(); if (e->state == 7) leiche = f + 1; }
    printf("  weiter getaktet: Phase %s, Clip %d Bild %u, Zustand %d, HP %d - Leiche (Zustand 7) nach %d Takten%s\n",
           ph_name(re15_gator_boss_phase(e)), (int)e->motion, (unsigned)e->anim_frame, (int)e->state, (int)e->hp, leiche,
           leiche < 0 ? " (nicht erreicht)" : "");
}

static int setup(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");                                 /* Kampf sofort (Aggro), wie test_r41_re2gator setup(1) */
    if (!getenv("R41_NK_LOG")) _putenv("RE15_GB_STUMM=1");
    _putenv("RE15_GATOR_KANISTER=");
#else
    setenv("RE15_GB_TEST", "1", 1);
    if (!getenv("R41_NK_LOG")) setenv("RE15_GB_STUMM", "1", 1);
    unsetenv("RE15_GATOR_KANISTER");
#endif
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 0; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 0; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    if (!load_re2_bank(0x23u)) { printf("FAIL: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); return 0; }
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
        if (!eb || eb->skel.bone_count != 22) { printf("FAIL: erwartet 22 Bones (EM23-Rig)\n"); return 0; } }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1;
    e = &g_actors[slot];
    memset(e, 0, sizeof *e);                                   /* Gator-Neuspawn wie main.c / probe_gator_fress.c */
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    s_leon_x = -6000; s_leon_y = 0; s_leon_z = -8000;          /* Nordbecken, wie test_r41_re2gator Teil sockel */
    tick();                                                    /* INIT (Lauerplatz, untergetaucht) + Aggro -> Intro */
    for (int f = 0; f < 400 && (re15_gr2_im_intro(slot) || f < 2); f++) tick();   /* Intro (Clip 6 -> 3) abwarten */
    re15_gr2_biss_cd(slot, 0);
    if (re15_gr2_im_intro(slot)) { printf("FAIL: Intro endet nicht\n"); return 0; }
    printf("setup: nach dem Intro Phase %s, HP %d (RE15_GR2_HP_INIT %d), Lage (%d,%d)\n", ph_name(re15_gator_boss_phase(e)),
           (int)e->hp, RE15_GR2_HP_INIT, (int)e->x, (int)e->z);
    if (e->hp != RE15_GR2_HP_INIT) { printf("FAIL: HP nach dem Intro %d != %d\n", (int)e->hp, RE15_GR2_HP_INIT); return 0; }
    return 1;
}

/* ================================================================================================================================ */
static int teil_a(void)
{
    /* Zyklus aus 32 Treffern (1002 Punkte, wenn alle sitzen): Rakete (Applier), 7 kleine, Rakete (Art 9), 7 kleine, Granate
     * (Explosion), 7 kleine, Granate (Art 2), 7 kleine. Zwischen Applier-Rakete und Explosion liegen 64 Takte - die Sperre nach einem
     * Applier-Treffer (re2_gl_sperre, 15 Bilder, Gate 2 @0x80047138-40) ist dann abgelaufen. */
    static int k_zyklus[32];
    for (int i = 0; i < 32; i++) k_zyklus[i] = A_KLEIN;
    k_zyklus[0] = A_RAKETE_APPLIER; k_zyklus[8] = A_RAKETE_ART9; k_zyklus[16] = A_GRANATE_EXPLOSION; k_zyklus[24] = A_GRANATE_ART2;
    /* Takte je Treffer: 4. Jeder Treffer wirft die laufende Animation zwei Bilder zurueck (`addiu v0,v0,-2` @0x80102030-48,
     * gb_absorb_hit) - bei 2 Takten je Treffer kaeme kein Clip mehr ans Ende (gemessen: der Zonen-Dash lief dann 1862 von 1910
     * Takten). Leons Lage: bis zum ersten abgefangenen Tod im Nordbecken, danach je 100 Treffer die naechste Lage - so faellt
     * der toedliche Schaden in Verfolgung, Dash, Biss, Belagerung (Plattform oben, y -1800 < -900 = player_on_platform) und
     * Grossreaktion. */
    enum { TAKTE_JE_TREFFER = 4, TREFFER_JE_LAGE = 100, NACH_ERSTEM_TOD = 400 };
    static const int32_t k_lagen[4][3] = {
        { -6000,     0,  -8000 },   /* Nordbecken (Wasser, Zone 1) */
        {     0, -1800, -16000 },   /* Plattform oben (Block GB_PLAT_*) */
        {  4000,     0, -25000 },   /* Suedbecken Ost (Zone 0) */
        { -7500,     0, -16300 },   /* Westkanal (Zone 2) */
    };

    long roh = 0;                                   /* Summe der HP-Abzuege im Treffermoment (vor dem Takt) */
    int versuche[A_N] = {0}, sitzt[A_N] = {0}, punkte[A_N] = {0};
    int toed = 0, toed_frei = 0, erster_toed = -1, hp_min = 32767, hp_null = 0;
    int ph_takte[PH_N] = {0}, ph_toed[PH_N] = {0};
    const int zaehler0 = re15_gator_boss_nur_kanister_zaehler();
    int i;
    for (i = 0; i < 8000; i++) {
        if (erster_toed >= 0) {
            const int k = ((i - erster_toed) / TREFFER_JE_LAGE) & 3;
            s_leon_x = k_lagen[k][0]; s_leon_y = k_lagen[k][1]; s_leon_z = k_lagen[k][2];
        }
        const int art = k_zyklus[i & 31];
        const int ph = re15_gator_boss_phase(e);
        const int taucht = re15_gr2_rw_taucht(slot);
        const int hp_vor = e->hp;
        versuche[art]++;
        treffer(art);
        const int hp_nach = e->hp;
        if (hp_nach < hp_vor) { sitzt[art]++; punkte[art] += hp_vor - hp_nach; roh += hp_vor - hp_nach; }
        const int toedlich = hp_nach < 0;
        /* Unverwundbar: Grossreaktion (+0x1D3 |= 0x80 @0x80102c20-28), nach R1 (@0x801022ec-f4) und Tauchgang (Trefferbox 0) -
         * dort nimmt gb_absorb_hit den Schaden auf prev_hp zurueck; ueberall sonst entscheidet allein das K-12-Abfangen. */
        const int frei = toedlich && ph != PH_FLINCH && ph != PH_RE2_TOD && !taucht;
        if (toedlich) {
            toed++; if (ph >= 0 && ph < PH_N) ph_toed[ph]++;
            if (frei) toed_frei++;
            if (erster_toed < 0) {
                erster_toed = i;
                printf("A: erster toedlicher Treffer Nr %d (%s) in Phase %s: HP %d -> %d, Zustand %d, Schaden bis hier %ld\n", i,
                       k_art_name[art], ph_name(ph), hp_vor, hp_nach, (int)e->state, roh);
            }
        }
        for (int t = 0; t < TAKTE_JE_TREFFER; t++) {
            tick();
            const int ph2 = re15_gator_boss_phase(e);
            if (ph2 >= 0 && ph2 < PH_N) ph_takte[ph2]++;
            if (!lebt("A", i)) {
                printf("  ausgeloest von Treffer Nr %d (%s) in Phase %s: HP %d -> %d\n", i, k_art_name[art], ph_name(ph), hp_vor, hp_nach);
                todesrolle_zeigen();
                return 1;
            }
            if (t == 0 && frei && e->hp != 1) {
                printf("FAIL A Nr %d: nach dem abgefangenen Tod HP %d statt 1 (Phase %s)\n", i, (int)e->hp, ph_name(ph2));
                fails++;
                return 1;
            }
            if (re15_gr2_r1_laeuft(slot) || re15_gr2_tot()) {
                printf("FAIL A Nr %d: R1/R1-Tod ohne Kanister (laeuft %d, tot %d)\n", i, re15_gr2_r1_laeuft(slot), re15_gr2_tot());
                fails++;
                return 1;
            }
            if (e->hp < hp_min) hp_min = e->hp;
            if (e->hp == 0) hp_null++;
        }
        if (roh > 30000 && toed_frei >= 20 && erster_toed >= 0 && i - erster_toed >= NACH_ERSTEM_TOD) break;
    }
    const int zaehler = re15_gator_boss_nur_kanister_zaehler() - zaehler0;
    printf("A: %d Treffer, %ld Schadenspunkte (Start %d), %d toedlich (%d ausserhalb der Unverwundbarkeit), K-12-Zaehler %d, "
           "HP am Ende %d, kleinste HP %d (HP 0 in %d Takten)\n", i + 1, roh, RE15_GR2_HP_INIT, toed, toed_frei, zaehler, (int)e->hp,
           hp_min, hp_null);
    for (int a = 0; a < A_N; a++)
        printf("   %-34s %4d Versuche, %4d sitzen, %6d Punkte\n", k_art_name[a], versuche[a], sitzt[a], punkte[a]);
    printf("   Phasen der Takte:");
    for (int p = 0; p < PH_N; p++) if (ph_takte[p]) printf(" %s %d", ph_name(p), ph_takte[p]);
    printf("\n   Phasen der toedlichen Treffer:");
    for (int p = 0; p < PH_N; p++) if (ph_toed[p]) printf(" %s %d", ph_name(p), ph_toed[p]);
    printf("\n");
    {   /* Abdeckung: der toedliche Schaden muss in mehreren Boss-Phasen ankommen (gemessen am Stand K-12: CHASE, LUNGE, FLINCH,
         * GUARD), sonst prueft der Riegel nur einen Zweig. */
        int phasen = 0;
        for (int p = 0; p < PH_N; p++) if (ph_toed[p]) phasen++;
        PRUEF(phasen >= 3, "toedliche Treffer nur in %d Phasen (Soll >= 3)", phasen);
    }
    PRUEF(roh > 30000, "nur %ld Schadenspunkte (Soll > 30000)", roh);
    PRUEF(toed_frei >= 20, "nur %d toedliche Treffer ausserhalb der Unverwundbarkeit (Soll >= 20)", toed_frei);
    PRUEF(zaehler > 0 && zaehler == toed, "K-12-Zaehler %d, toedliche Treffer %d (jeder muss abgefangen sein)", zaehler, toed);
    PRUEF(e->hp == 1, "HP am Ende %d statt 1", (int)e->hp);
    PRUEF(hp_min >= 0, "HP fiel unter 0 (%d)", hp_min);
    PRUEF(sitzt[A_RAKETE_APPLIER] > 0, "die Rakete ueber den RE2-Applier traf nie (P/Box pruefen)");
    PRUEF(sitzt[A_GRANATE_EXPLOSION] > 0, "die Granaten-Explosion traf nie (P/Box pruefen)");
    PRUEF(sitzt[A_RAKETE_ART9] > 0 && sitzt[A_GRANATE_ART2] > 0 && sitzt[A_KLEIN] > 0, "ein Gegnerzweig-Eingang traf nie");
    PRUEF(!re15_gr2_tot(), "R1-Tod ohne Kanister");
    return fails ? 1 : 0;
}

/* ================================================================================================================================ */
static int teil_b(void)
{
    PRUEF(e->hp == 1, "Vorbedingung Teil B: HP %d statt 1", (int)e->hp);
    /* Leon fern im Suedbecken (Zone 0 wie die Statue unten): kein Zonen-Dash, kein Biss; auf die Verfolgung in Zone 0 warten. */
    s_leon_x = 5000; s_leon_y = 0; s_leon_z = -25500;
    int w = 0;
    for (; w < 3000; w++) {
        if (re15_gator_boss_phase(e) == PH_CHASE && e->z <= -19100 && !re15_gr2_im_dash(slot) && !re15_gr2_rw_taucht(slot)) break;
        tick();
        if (!lebt("B-Warten", w)) { todesrolle_zeigen(); return 1; }
    }
    if (w >= 3000) { printf("FAIL B: keine Verfolgung in Zone 0 nach %d Takten (Phase %s, z %d)\n", w, ph_name(re15_gator_boss_phase(e)),
                            (int)e->z); return 1; }
    /* Statue + Kanister wie test_r41_re2gator Teil kanister: Gier 0xC00 (Norden), Kanister 8800 x Skala voraus (Fenster 8601..8999) */
    e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    const int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    const int32_t laengs = (int32_t)(((int64_t)8800 * s) >> 12);
    re15_gr2_kanister_setzen(-6000, -267, -22000 + laengs);
    PRUEF(re15_gr2_kanister_stufe() == 0, "Kanister nicht gesetzt");
    int f7 = -1;
    for (int f = 0; f < 240; f++) {
        e->x = -6000; e->z = -22000; e->rot_y = 0xC00;
        tick();
        if (!lebt("B-Aufnahme", f)) { todesrolle_zeigen(); return 1; }
        if (re15_gr2_kanister_stufe() == 7) { f7 = f; break; }
    }
    printf("B: Verfolgung nach %d Takten, Kanister im Maul (Stufe 7) nach %d Takten, HP %d\n", w, f7, (int)e->hp);
    if (f7 < 0) { printf("FAIL B: Kanister nie im Maul (Stufe %d)\n", re15_gr2_kanister_stufe()); return 1; }
    /* Der Schuss: kleiner Treffer (Art 0, 10) bei HP 1 -> HP -9, Zustand 3 = an sich toedlich. */
    const int ok = re15_enemy_take_damage(e, 0u);
    printf("B: Treffer auf den Kanister-Traeger: sitzt %d, HP %d, Zustand %d\n", ok, (int)e->hp, (int)e->state);
    PRUEF(ok && e->hp < 0 && e->state == 3, "der Treffer war nicht toedlich (sitzt %d, HP %d, Zustand %d)", ok, (int)e->hp, (int)e->state);
    tick();
    if (!lebt("B-Treffer", 0)) { todesrolle_zeigen(); return 1; }
    PRUEF(re15_gr2_r1_laeuft(slot), "der Treffer mit dem Kanister im Maul startet R1 nicht (@0x80101ef4-fc)");
    PRUEF(re15_gator_boss_phase(e) == PH_RE2_TOD, "Phase %s statt RE2_TOD", ph_name(re15_gator_boss_phase(e)));
    PRUEF(e->motion == 9, "R1 spielt nicht Clip 9 (Clip %d)", (int)e->motion);
    PRUEF(re15_gr2_kanister_stufe() == 13, "Kanister nicht explodiert (Stufe %d statt 13)", re15_gr2_kanister_stufe());
    int bilder = 0;
    for (; bilder < 400 && !re15_gr2_tot(); bilder++) {
        tick();
        if (!lebt("B-R1", bilder)) { todesrolle_zeigen(); return 1; }
    }
    printf("B: R1 bis zur Leiche %d Takte, tot %d, Phase %s, Clip %d, Zustand %d, HP %d\n", bilder, re15_gr2_tot(),
           ph_name(re15_gator_boss_phase(e)), (int)e->motion, (int)e->state, (int)e->hp);
    PRUEF(re15_gr2_tot() == 1, "R1 endet nicht in der Leiche (re15_gr2_tot %d)", re15_gr2_tot());
    PRUEF(e->motion == 9, "die Leiche haelt nicht die Clip-9-Pose (Clip %d)", (int)e->motion);
    return fails ? 1 : 0;
}

int main(int argc, char **argv)
{
    const int nur_b = (argc > 1 && (argv[1][0] == 'b' || argv[1][0] == 'B'));
    if (!setup()) return 1;
    if (nur_b) {
        e->hp = 1;                                             /* Vorbedingung (Muster Messhaken RE15_K2080_GATOR_HP) */
        printf("Teil B allein: HP %d als Vorbedingung gesetzt\n", (int)e->hp);
    } else if (teil_a() != 0) {
        printf("ERGEBNIS: FAIL (Teil A - der Alligator stirbt ohne Kanister)\n");
        return 1;
    }
    if (teil_b() != 0 || fails) {
        printf("ERGEBNIS: FAIL (Teil B)\n");
        return 1;
    }
    printf("ERGEBNIS: OK - %sder Alligator stirbt nur durch den Kanister (R1)\n", nur_b ? "Teil B: " : "");
    return 0;
}
