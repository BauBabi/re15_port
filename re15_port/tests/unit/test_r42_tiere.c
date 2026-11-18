/*
 * test_r42_tiere.c — Runde 42 Strang "gore", Stufe 2 "Tiere und Bosse" (BAUPLAN gore_matrix/BAUPLAN.md U5, U7, U16, U4,
 * U3, U9, U6, U8; Nutzerregel 16.11.2026 "80% build ist ausschlaggebend, dann 40% build ... dann Resident Evil 2").
 *
 *   [u4]  Birkin in Menschform (0x30): Start-HP 300 (`ori v0,zero,0x12c` @0x8011690c) statt 600; 20 Pistolentreffer zu 15
 *         (Disc-Zeile 0x30 Spalte 3 @0x8006F15C) lassen ihn leben, der 21. toetet (Todesregel hp < 0 @0x80013020).
 *   [u3]  Gorilla (0x27): INIT-HP 180 (@0x8011f514 x 1) statt 360; 15 Pistolentreffer zu 12 (@0x8006ee44) lassen ihn leben,
 *         der 16. toetet. Die Zuck-Schwelle (3 Pistolentreffer = 36) bleibt (F3 a).
 *   [u5]  Kraehe: RE2-Gehirn - Browning w3 / SIG w4 (80 %) und Schrot w8 / SPAS w13 (Disc @0x801211cc[8]/[13]) zerlegen in
 *         13 Teile (GIB 0x80102CA0); Messer w1, Beretta w5, Ingram w12 nicht. Messung der Sichtbarkeit der Teile.
 *         RE1.5-Gehirn: Pistole -> GIB-Spur (+0x5 = 7).
 *   [u7]  Spinne: Platzen (RE2-Tod w7 Krit, RE1.5-Tod w9) -> Teile 0/19 platzen, die Beine 3..18 bleiben dran
 *         ((w & 0x41) == 1, kein Flugbit 0x20). Stufe 2 Runde 2 (145-149): der Kadaver liegt im Disc-Platzweg-Clip 1 und
 *         haelt dessen letztes Bild (RE2- und RE1.5-Gehirn), statt auf dem Ruecken mit Clip 12/13 (5) zu zappeln.
 *   [u16] Hund: RE2-Gehirn, Handgranate (Art 2) -> RE2-Teile-Wurf 0x80104440 (Flagwort 0x4A), nach 29 Bildern weg
 *         (@0x80028DCC/DC); Werfer-Explosivrunde (0x10020009, +0x1D2 >= 3) -> kein Teile-Wurf (@0x801046A8-D4).
 *   [u9]  Riesenform (0x30 in ROOM5080): Schaden je Waffe = RE2-G4-Zeile Klammer 0 (@0x800A5BE4 + (Zeile-1)*20); dieselbe
 *         Form in ROOM5090 behaelt die alte Zeile (Strang-Grenze r42/finale); Handgranate D_G 251 (NUTZERWUNSCH).
 *   [u6]  Beretta-Feuerstoss: Gorilla/Kakerlake/Tyrant/Birkin = EIN Treffer mit Spalte 5/6 (@0x8003396c), drei Patronen;
 *         der Zombie dahinter nimmt Kugel 2/3; Zombie allein drei Treffer; Alligator unveraendert (drei Treffer).
 *   [u8]  Gitterhaende ROOM1210: VORPRUEFUNG (Flur ohne Schuss, RE2-Arm-Gehirn, Wand- und Mittelweg, kein Tastendruck) und
 *         Riegel "Schuesse/Messer gehen durch" (re15_player_weapon_fire liefert keinen Arm), Explosion trifft weiter.
 *   [aus] eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: Kraehe faellt bei der Pistole ganz, Schrot zerlegt (Disc);
 *         RE1.5-Spinne wirft die Beine (Disc).
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 * Aufruf: test_r42_tiere [u4|u3|u5|u7|u16|u9|u6|u8|alle|aus]
 *
 * GEGENPROBE: der Test liest nur Felder und Funktionen, die es schon auf aaea5f8c2 gibt (kein Aufruf der neuen
 * re15_feuerstoss_ein_treffer), damit er gegen die Engine VOR dieser Stufe baut und dort rot ist.
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_player.h"
#include "re15_inventory.h"
#include "re15_beretta_r37.h"
#include "re15_teile_r15.h"
#include "re15_schuss_r36.h"
#include "re15_granate_r35.h"
#include "re15_gegner_r39.h"
#include "re15_esp.h"
#include "re15_math.h"
#include "re15_skeleton.h"
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

extern int  re15_re2crow_hurt_test(re15_actor_t *e, re15_actor_t *pl);
extern int  re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame, int32_t rot[9], int32_t trans[3]);
extern void re15_re2z_rng_reset(void);
extern int  re15_re2spider_tick(int slot);
extern void re15_aspider_beine_reset(void);
extern void re15_spinnengore_test_schalter(int an);
extern unsigned re15_re2d_granate_zahl(void);
extern int  re15_re2_gl_apply(const int32_t p[3], int16_t gier, const int16_t box_in[4], uint32_t hitcode);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_re2z_hit_filter_apply(int slot);
extern void re15_re2_pause_filter_apply(int slot);
extern void re15_beretta_r37_reset(void);

static int s_erste = 0, s_fehler = 0;
#define PRUEF(nr, cond, ...) do { if (!(cond)) { \
        printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
        if (!s_erste) s_erste = (nr); s_fehler++; } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Raum / Spielschritt ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_current_room_id = (uint16_t)room_id;
    return 0;
}
static void frame(int hp_halten)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    if (hp_halten) g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static const uint8_t *s_re2_ems = NULL; static size_t s_re2_ems_n = 0;
static const uint8_t *re2_ems(size_t *n)
{
    static int tried = 0;
    if (!tried) { tried = 1; s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_ems_n); }
    *n = s_re2_ems_n; return s_re2_ems;
}
/* RE2-Bank `re2_kind` unter dem Port-Typ `type` (wie re2_ems_load_bank in den Sonden). */
static int load_re2_bank(uint8_t type, uint8_t re2_kind)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    size_t n = 0; const uint8_t *ems = re2_ems(&n);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (!ems || re2_ems_load_bank(ems, n, re2_kind, eb, &tim) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

/* ---- Schuetze vor einem Ziel (Muster test_r37_beretta: freie Bahn ohne Raum-Kollision) ------------------------------ */
static void spieler(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 200; pl->y = 0; pl->z = -18200; pl->rot_y = 1024;
    pl->floor = 0;
    re15_player_apply_hitbox(pl);
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
}
static re15_actor_t *ziel(int slot, uint8_t type, int32_t z, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->x = 200; e->y = 0; e->z = z; e->hp = hp;
    e->state = 1; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    e->grid_id = 0; e->aim_band = 2;
    return e;
}
static int schuss(int w)
{
    re15_inv_init(); re15_inv_grant((uint8_t)w, 15); re15_player_set_equipped_weapon(w);
    return re15_player_weapon_fire(w);
}

/* ================================================================================================================== */
/* [u4] Birkin in Menschform                                                                                           */
/* ================================================================================================================== */
static void abschnitt_u4(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    g_current_room_id = 0x3070;
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->z = 4000; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x30; e->state = 0; e->grid_id = 0x33; e->x = 0; e->z = 0;
    re15_enemy_apply_hitbox(e, 0x30);
    re15_enemy_spawn_root(1);
    PRUEF(100, e->hp == 300, "Birkin 0x30 (ROOM3070) INIT-HP %d, Soll 300 = `ori v0,zero,0x12c` @0x8011690c (Disc, Faktor 1)", e->hp);
    /* Pistole am echten Resolver: ROOM-unabhaengig, freie Bahn. Stufe 2 Runde 2 (Pruefbericht Gelb 5): die HP kommen aus dem
     * INIT oben, nicht aus einer gesetzten Zahl. */
    const int16_t hp_init = e->hp;
    spieler();
    g_current_room_id = 0x3070;
    re15_actor_t *b = ziel(1, 0x30, -19600, hp_init);
    int lebt20 = 1, tot21 = 0;
    for (int k = 1; k <= 21; k++) {
        b->hit_react = 0; b->state = 1;
        (void)schuss(3);
        if (k <= 20 && b->hp < 0) lebt20 = 0;
        if (k == 21 && b->hp < 0) tot21 = 1;
    }
    PRUEF(101, lebt20 && tot21, "Birkin INIT-HP %d: 20 Pistolentreffer zu 15 (@0x8006F15C) leben, der 21. toetet (hp %d)",
          hp_init, b->hp);
}

/* ================================================================================================================== */
/* [u3] Gorilla                                                                                                        */
/* ================================================================================================================== */
static void abschnitt_u3(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_r39_gegner_enable(1);
    g_current_room_id = 0x11C0;
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x27;
    int16_t hp = re15_r39_affen_hp_init(e, 180);
    PRUEF(110, hp == 180, "Gorilla 0x27 INIT-HP %d aus Tabelle 180 (@0x8011f514), Soll 180 (Faktor 1)", hp);
    PRUEF(111, RE15_R39_ZUCK_SCHWELLE == 36, "Zuck-Schwelle bleibt 3 x 12 = 36 (F3 a), ist %d", RE15_R39_ZUCK_SCHWELLE);
    spieler();
    g_current_room_id = 0x11C0;
    re15_actor_t *g = ziel(1, 0x27, -19600, hp);   /* Stufe 2 Runde 2 (Gelb 5): HP aus dem INIT-Leser oben */
    int lebt15 = 1, tot16 = 0;
    for (int k = 1; k <= 16; k++) {
        g->hit_react = 0; g->state = 1;
        (void)schuss(3);
        if (k <= 15 && g->hp < 0) lebt15 = 0;
        if (k == 16 && g->hp < 0) tot16 = 1;
    }
    PRUEF(112, lebt15 && tot16, "Gorilla INIT-HP %d: 15 Pistolentreffer zu 12 (@0x8006ee44) leben, der 16. toetet (hp %d)",
          hp, g->hp);
}

/* ================================================================================================================== */
/* [u5] Kraehe                                                                                                         */
/* ================================================================================================================== */
static re15_actor_t *kraehe(int32_t y)
{
    re15_actor_t *e = &g_actors[RE15_ACTOR_SLOT_PLAYER + 1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x21; e->state = 1; e->hp = 10;
    e->x = 1000; e->y = y; e->z = 2000; e->rot_y = 0;
    e->dog_floor_y = 0;
    re15_schuss_r36_kraehe_parts_seed(e);
    return e;
}
static int kraehe_zerlegt(const re15_actor_t *e)
{
    int n = 0; for (int i = 0; i < 13; i++) n += (e->re2z_part_flags[i] & 0x40u) ? 1 : 0;
    return n;
}
/* wie der Renderer: je Bild jede Zeile durch die Part-Bruecke; Rueckgabe = erstes Bild, in dem kein Teil mehr Bit 0 traegt */
static int kraehe_weg_bild(re15_actor_t *e, int max)
{
    for (uint32_t f = 1; f <= (uint32_t)max; f++) {
        for (int i = 0; i < 13; i++) {
            int32_t rot[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
            int32_t t[3] = { e->x, e->y, e->z };
            (void)re15_re2z_gore_part_matrix(e, i, f, rot, t);
        }
        int da = 0; for (int i = 0; i < 13; i++) da += (e->re2z_part_flags[i] & 1u) ? 1 : 0;
        if (da == 0) return (int)f;
    }
    return -1;
}
static void abschnitt_u5(int aus)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 0; pl->z = 2000;
    static const struct { uint8_t w; int gib; int gib_aus; const char *n; } F[] = {
        { 3, 1, 0, "Browning w3 (80 %)" }, { 4, 1, 0, "SIG P228 w4 (80 %)" },
        { 8, 1, 1, "Schrot w8 (Disc @0x801211cc[8])" }, { 13, 1, 1, "SPAS w13 (Disc @0x801211cc[13])" },
        { 7, 1, 1, "Redhawk w7 (RE2 Zeile 5, wie vorher)" },
        { 1, 0, 0, "Messer w1" }, { 5, 0, 0, "Beretta w5" }, { 12, 0, 0, "Ingram w12" },
    };
    const int nr0 = aus ? 230 : 120;
    for (unsigned k = 0; k < sizeof F / sizeof F[0]; k++) {
        re15_actor_t *e = kraehe(-1500);
        e->state = 2; e->sub_state_1 = F[k].w; e->sub_state_2 = 0;
        /* Stufe 2 Runde 2 (Pruefbericht Gelb 5: bei 125-127 starb die Kraehe gar nicht, HP 10): jeder Fall ist ein TOEDLICHER
         * Treffer - die HP stehen nach dem Abzug unter 0 (der Applier zieht vor dem HURT-Handler ab, Todesregel hp < 0). */
        e->hp = -1;
        re15_re2z_rng_reset();
        re15_re2crow_hurt_test(e, pl);
        int soll = aus ? F[k].gib_aus : F[k].gib;
        /* Ohne GIB stirbt die RE2-Kraehe im HURT-Zustand selbst (hp < 0 -> Phase 5 @0x80102B6C-98, danach Leiche 7/0, 7/2
         * oder 7/3); den Ablauf bis zur Leiche weiterlaufen lassen (hoechstens 400 Takte). */
        for (int t = 0; t < 400 && e->state == 2; t++) re15_re2crow_hurt_test(e, pl);
        int z = kraehe_zerlegt(e);
        PRUEF(nr0 + (int)k, soll ? (z == 13 && e->state == 7 && e->hp < 0)
                                 : (z == 0 && e->hp < 0 && e->state == 7 && e->sub_state_1 != 1),
              "RE2-Kraehe %s, toedlich: %s (13 Teile mit eigener Matrix: %d, Zustand %d, hp %d)", F[k].n,
              soll ? "zerfaellt in Fetzen (GIB 0x80102CA0)" : "stirbt und faellt ganz (kein GIB, Leiche nicht 7/1)", z, e->state,
              e->hp);
    }
    if (aus) return;
    /* Messung: wie lange sind die Fetzen sichtbar? Maske 0x1F (alle Wurf) und 0 (Wirbel), Flughoehe -1500 und Boden -250 */
    {
        static const struct { uint32_t m; int32_t y; const char *n; } M[] = {
            { 0x1Fu, -1500, "Maske 0x1F (alle WURF), Flughoehe 1500" }, { 0x00u, -1500, "Maske 0 (WIRBEL), Flughoehe 1500" },
            { 0x1Fu, -250,  "Maske 0x1F, am Boden (y -250)" },         { 0x00u, -250,  "Maske 0, am Boden (y -250)" },
        };
        int wurf_max = 0, alle_max = 0;
        for (unsigned k = 0; k < 4; k++) {
            re15_actor_t *e = kraehe(M[k].y);
            re15_re2z_rng_reset();
            re15_schuss_r36_kraehe_gib(e, 0, M[k].m);
            int f = kraehe_weg_bild(e, 200);
            printf("   MESS u5: %s -> alle Fetzen weg ab Bild %d (%.2f s bei 30 Bildern/s)\n", M[k].n, f, f / 30.0);
            if (M[k].m == 0x1Fu && f > wurf_max) wurf_max = f;
            if (f > alle_max || f < 0) alle_max = (f < 0) ? 9999 : f;
        }
        PRUEF(128, wurf_max == 30, "WURF-Fetzen sind nach 29 Bildern weg (`sltiu v0,v0,0x1d` @0x80028DCC, `sw zero,0` @0x80028DDC): "
              "Bild %d = 0,97 s, im 80 %%-Fenster 0,6-1,0 s (F07 F1 0:54,63-0:55,6)", wurf_max);
        printf("   MESS u5: laengste Sichtbarkeit (auch WIRBEL, 2. Bodenkontakt @0x80028CA0) %d Bilder\n", alle_max);
    }
    /* RE1.5-Gehirn: Pistole -> GIB-Spur (re15_crow_death mappt auf +0x5 = 7) */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_actor_init();
        re15_actor_t *p2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p2->active = 1; p2->hp = 100; p2->x = 0; p2->z = 2000;
        re15_actor_t *e = &g_actors[1];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x21; e->state = 3; e->sub_state_1 = 3; e->sub_state_3 = 0; e->hp = -5;
        e->x = 1000; e->y = -1500; e->z = 2000; e->em_flag_id = 0xFF;
        re15_enemy_ai_run_all(1);
        PRUEF(129, e->sub_state_1 == 7 && e->crow_hide == 1,
              "RE1.5-Kraehe Pistole w3: Todes-Spur GIB (+0x5 = %d, Soll 7; crow_hide %d)", e->sub_state_1, e->crow_hide);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    }
}

/* ================================================================================================================== */
/* [u7] Spinne                                                                                                         */
/* ================================================================================================================== */
#define SPS 1
static void spinne_frisch(re15_ai_flavor_t fl)
{
    re15_actor_init();
    re15_ai_flavor_set(fl);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *sp = &g_actors[SPS];
    memset(pl, 0, sizeof *pl); memset(sp, 0, sizeof *sp);
    pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = -3000;
    sp->active = 1; sp->type = 0x25; sp->em_flag_id = 0xFF;
    re15_teile_reset(SPS);
    re15_aspider_beine_reset();
    re15_esp_fx_reset();
}
static void spinne_takt(void)
{
    re15_actor_t *sp = &g_actors[SPS];
    sp->ai_dist = (uint32_t)re15_enemy_player_dist(sp, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
    re15_re2spider_tick(SPS);
}
/* 1 = Teile 0/19 platzen; *beine_dran = Zahl der Beinteile 3..18 mit (w & 0x41) == 1 ohne Flugbit 0x20; *flug = mit 0x20 */
static int spinne_platzt(int *beine_dran, int *flug)
{
    re15_teil_t *t = re15_teile(SPS);
    *beine_dran = 0; *flug = 0;
    if (!t || !re15_teile_an(SPS)) return 0;
    for (int k = 3; k <= 18; k++) {
        if (t[k].w & 0x20u) (*flug)++;
        else if ((t[k].w & 0x41u) == 1u) (*beine_dran)++;
    }
    return (t[0].w & 0x4a) == 0x4a && (t[19].w & 0x4a) == 0x4a;
}
/* ---- Stufe 2 Runde 2 (Pruefbericht s2 r1 Rot 1): die LIEGE-POSE des geplatzten Kadavers ------------------------------
 * Disc STAGE2: Platzweg Phase 0 Clip 1 (`sb v1(1),0x94` @0x80114058), Phase 1 spielt ihn (`jal 0x8001f314` a3 0x200
 * @0x80114548-4c), Leiche Platzweg ohne Clip und ohne Vorschub (`j 0x8011559c` @0x801154b4); leichter Tod Clip 5
 * (@0x80115520). RE2: Clip 12 (@0x80103DC0 / @0x80104578), Leiche Clip 13 (@0x80104D5C). Bild: 80 % F2 0:15,0-0:15,5. */
static int bank_re15_spinne(void)
{
    re15_enemy_reset();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x25);
    if (!eb) return 0;
    static uint8_t *s_ems = NULL; static size_t s_n = 0;
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_n);
    size_t off = 0, len = 0;
    if (!s_ems || re15_ems_get_entry(s_ems, s_n, re15_ems_index_for_type(0x25), &off, &len) != 0) return 0;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_ems + off, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->buf = NULL; eb->ok = 1;
    return 1;
}
static int clip_len(int clip)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x25);
    return (eb && eb->ok && clip < eb->anim.clip_count) ? eb->anim.clips[clip].frame_count : -1;
}
/* RE2-Gehirn, Tod mit Waffe w / Treffer-Bits h93: Takte bis zur Leiche, dann 300 Leichen-Takte. Rueckgabe 1 = jeder
 * Leichen-Takt zeigt Clip c_soll; *letztes = letztes Bild; *mo_tod = Clip nach dem ersten Todestakt; *bis = Takte bis zur
 * Leiche. */
static int spinne_re2_tod(int w, uint8_t h93, int c_soll, int *mo_tod, int *bis, int *letztes)
{
    spinne_frisch(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset(); load_re2_bank(0x25, 0x25);
    re15_actor_t *sp = &g_actors[SPS];
    sp->state = 0; spinne_takt();
    sp->state = 3; sp->sub_state_1 = (uint8_t)w; sp->sub_state_2 = 0; sp->sub_state_3 = 0; sp->hp = -20; sp->hit_react = h93;
    re15_teile_reset(SPS);
    spinne_takt();
    *mo_tod = sp->motion;
    int t = 1;
    while (t < 400 && sp->state != 7) { spinne_takt(); t++; }
    *bis = t;
    int ok = (sp->state == 7);
    for (int k = 0; k < 300 && ok; k++) {
        spinne_takt();
        if (c_soll >= 0 && sp->motion != c_soll) ok = 0;
    }
    *letztes = (int)sp->anim_frame;
    return ok;
}
static void abschnitt_u7_pose(void)
{
    int mo_tod = -1, bis = 0, letztes = -1, ok;
    /* 145: RE2-Gehirn, Redhawk-Krit (w7, 0x41 = Platzer): Clip 1, nach dessen Ende Leiche, dann 300 Takte Clip 1 Bild 7 */
    ok = spinne_re2_tod(7, 0x41, 1, &mo_tod, &bis, &letztes);
    int l1 = clip_len(1);
    PRUEF(145, ok && mo_tod == 1 && l1 > 0 && letztes == l1 - 1 && bis <= l1 + 1,
          "RE2-Spinne geplatzt (w7 Krit): Todestakt Clip %d (Soll 1, @0x80114058), Leiche nach %d Takten (Clip 1 = %d Bilder, "
          "@0x80114548-4c), 300 Leichen-Takte Clip 1, zuletzt Bild %d (Soll %d, Leiche ohne Vorschub @0x801154b4) -> %s",
          mo_tod, bis, l1, letztes, l1 - 1, ok ? "ja" : "NEIN (Clip 12/13 = auf dem Ruecken)");
    /* 146: der geplatzte Kadaver wird erneut getroffen (RE2-Leiche bleibt beschiessbar, re15_damage.c "RE2-ADULT-SPINNE";
     * Wiedereintritt mit +0x224 = 1 wie nach der allgemeinen Zeile @0x80103EB8): die Pose bleibt Clip 1 Bild 7 */
    {   re15_actor_t *sp = &g_actors[SPS];
        int vorher = re15_teile(SPS) ? (int)re15_teile(SPS)[0].w : -1;
        sp->state = 3; sp->sub_state_1 = 3; sp->sub_state_2 = 0; sp->hp = -1; sp->hit_react = 0x01; sp->re2s_done224 = 1;
        spinne_takt();
        int mo1 = sp->motion, af1 = (int)sp->anim_frame;
        int t = 0; while (t++ < 50 && sp->state != 7) spinne_takt();
        for (int k = 0; k < 60; k++) spinne_takt();
        int nachher = re15_teile(SPS) ? (int)re15_teile(SPS)[0].w : -2;
        PRUEF(146, mo1 == 1 && af1 == l1 - 1 && sp->state == 7 && sp->motion == 1 && (int)sp->anim_frame == l1 - 1 &&
                   vorher == nachher,
              "Kadaver nachgeschossen: Clip %d Bild %d im Treffertakt, danach Zustand %d Clip %d Bild %d, Teil-0-Wort %04x -> %04x "
              "(kein zweites Platzen)", mo1, af1, sp->state, sp->motion, (int)sp->anim_frame, vorher, nachher); }
    /* 147: RE2-Gehirn, Tod ohne Platzen (Pistole w3, kein Krit) -> RE2 wie bisher: Clip 12, Leiche Clip 12/13 */
    {   ok = spinne_re2_tod(3, 0x01, -1, &mo_tod, &bis, &letztes);
        re15_actor_t *sp = &g_actors[SPS];
        PRUEF(147, mo_tod == 12 && sp->state == 7 && (sp->motion == 12 || sp->motion == 13),
              "Gegenpol RE2-Spinne Pistole ohne Krit: Todestakt Clip %d (Soll 12 @0x80103DC0), Leiche Clip %d (12/13 @0x80104D5C)",
              mo_tod, sp->motion); }
    /* 148: RE1.5-Gehirn mit der Disc-Bank, Tod w9 (Platzweg): Leiche haelt Clip 1 Bild 7 */
    {   int bank = bank_re15_spinne();
        spinne_frisch(RE15_AI_FLAVOR_RE15);
        re15_actor_t *sp = &g_actors[SPS];
        sp->state = 3; sp->sub_state_1 = 9; sp->sub_state_3 = 0; sp->hp = -5; sp->hit_react = 0x01;
        int t = 0; while (t++ < 100 && sp->state != 7) re15_enemy_ai_run_all(1);
        int st = sp->state, mo = sp->motion, af = (int)sp->anim_frame, gleich = 1;
        for (int k = 0; k < 200; k++) {
            re15_enemy_ai_run_all(1);
            if (sp->motion != 1 || (int)sp->anim_frame != clip_len(1) - 1) gleich = 0;
        }
        PRUEF(148, bank && st == 7 && mo == 1 && af == clip_len(1) - 1 && gleich,
              "RE1.5-Spinne Tod w9 (Platzweg): Leiche Clip %d Bild %d (Soll 1/%d), 200 Takte unveraendert %d (Leiche ohne Clip "
              "@0x80115414-68, ohne Vorschub @0x801154b4)", mo, af, clip_len(1) - 1, gleich); }
    /* 149: RE1.5-Gehirn, leichter Tod w3 -> Leiche Clip 5 (@0x80115520), unveraendert */
    {   spinne_frisch(RE15_AI_FLAVOR_RE15);
        re15_actor_t *sp = &g_actors[SPS];
        sp->state = 3; sp->sub_state_1 = 3; sp->sub_state_3 = 0; sp->hp = -5; sp->hit_react = 0x01;
        int t = 0; while (t++ < 100 && sp->state != 7) re15_enemy_ai_run_all(1);
        re15_enemy_ai_run_all(1);
        PRUEF(149, sp->state == 7 && sp->motion == 5, "Gegenpol RE1.5-Spinne leichter Tod w3: Leiche Clip %d (Soll 5 @0x80115520)",
              sp->motion); }
    re15_enemy_reset();
}

static void abschnitt_u7(int aus)
{
    int dran, flug, p;
    const int nr0 = aus ? 240 : 140;
    re15_spinnengore_test_schalter(aus ? 0 : 1);
    /* RE2-Gehirn, Tod Redhawk w7 mit Krit (0x41) */
    spinne_frisch(RE15_AI_FLAVOR_RE2);
    { re15_actor_t *sp = &g_actors[SPS]; sp->state = 0; spinne_takt();
      sp->state = 3; sp->sub_state_1 = 7; sp->sub_state_2 = 0; sp->sub_state_3 = 0; sp->hp = -20; sp->hit_react = 0x41; }
    re15_teile_reset(SPS);
    spinne_takt();
    p = spinne_platzt(&dran, &flug);
    if (!aus)
        PRUEF(nr0, p && dran == 16 && flug == 0, "RE2-Spinne Tod w7 Krit: platzt (%d), alle 16 Beinteile bleiben dran (%d), "
              "keines fliegt (%d) - 80 %% F08 F2 0:15,1-0:15,5", p, dran, flug);
    /* RE1.5-Gehirn, Tod w9 (Tabelle @0x80118f38[9] = 0x80113ff8) */
    spinne_frisch(RE15_AI_FLAVOR_RE15);
    { re15_actor_t *sp = &g_actors[SPS];
      sp->state = 3; sp->sub_state_1 = 9; sp->sub_state_3 = 0; sp->hp = -5; sp->hit_react = 0x01; }
    re15_enemy_ai_run_all(1);
    p = spinne_platzt(&dran, &flug);
    if (!aus)
        PRUEF(nr0 + 1, p && dran == 16 && flug == 0, "RE1.5-Spinne Tod w9: platzt (%d), Beine dran %d, fliegend %d", p, dran, flug);
    else
        PRUEF(nr0 + 1, p && flug == 16, "AUS: RE1.5-Spinne Tod w9 wirft wie die Disc alle Beine (@0x8011419c-52c): fliegend %d", flug);
    if (!aus) abschnitt_u7_pose();
    re15_spinnengore_test_schalter(-1);
}

/* ================================================================================================================== */
/* [u16] Hund an der Explosion                                                                                         */
/* ================================================================================================================== */
static re15_actor_t *hund_arena(re15_ai_flavor_t fl)
{
    re15_ai_flavor_set(fl);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    load_re2_bank(0x20, 0x20);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_teile_reset_alle();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    re15_game_flag_set(3, 152, 1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 30; f++) frame(1);
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    if (e->state != 1) re15_ai_set_state_word(e, 0x100);
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0; e->hit_react = 0;
    pl->x = e->x - 8000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 3; f++) frame(1);
    e->re2z_self1d3 = 0; e->hit_react = 0;
    e->hp = 60;
    return e;
}
static const int s_hund_teile[7] = { 2, 3, 4, 7, 8, 9, 10 };   /* @0x80105680 */
static void abschnitt_u16(void)
{
    if (room_load(0x11D0, "STAGE1") != 0) { PRUEF(150, 0, "ROOM11D0 fehlt"); return; }
    /* (a) Handgranate (Art 2) am RE2-Hund */
    {
        re15_actor_t *e = hund_arena(RE15_AI_FLAVOR_RE2);
        if (!e) { PRUEF(150, 0, "kein Hund in ROOM11D0"); return; }
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        frame(1);
        int w4a = 0, w1062 = 0;
        for (int i = 0; i < 7; i++) {
            uint16_t fl = e->re2z_part_flags[s_hund_teile[i]];
            if ((fl & 0x4Au) == 0x4Au) w4a++;
            if ((fl & 0x20u)) w1062++;
        }
        printf("   MESS u16: Handgranate RE2-Hund: Treffer %d Zustand %d hp %d +0x1D2 %u, Teile 0x4A %d, 0x20 %d\n",
               n, e->state, e->hp, (unsigned)e->re2z_hits1d2, w4a, w1062);
        PRUEF(150, n >= 1 && e->hp < 0 && w4a == 7 && w1062 == 0,
              "Handgranate: der RE2-Hund stirbt (hp %d) und wirft die 7 Teile {2,3,4,7,8,9,10} als DRIFT 0x4A "
              "(`ori v0,v0,0x4a` @0x801044a4): %d/7, Wurf-Bit 0x20 an %d Teilen (Soll 0)", e->hp, w4a, w1062);
        int weg = -1;
        for (uint32_t f = 1; f <= 120 && weg < 0; f++) {
            for (int i = 0; i < 16; i++) {
                int32_t rot[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
                int32_t t[3] = { e->x, e->y - 300, e->z };
                (void)re15_re2z_gore_part_matrix(e, i, f, rot, t);
            }
            int da = 0; for (int i = 0; i < 7; i++) da += e->re2z_part_flags[s_hund_teile[i]] != 0u ? 1 : 0;
            if (da == 0) weg = (int)f;
        }
        PRUEF(151, weg == 30, "die Hund-Teile sind nach 29 Bildern weg (`sltiu v0,v0,0x1d` @0x80028DCC, `sw zero,0` "
              "@0x80028DDC): Bild %d (vorher blieben sie liegen)", weg);
    }
    /* (b) Werfer-Explosivrunde (RE2 Op 47, Hitcode 0x10020009, K 1) am RE2-Hund */
    {
        re15_actor_t *e = hund_arena(RE15_AI_FLAVOR_RE2);
        if (!e) { PRUEF(152, 0, "kein Hund in ROOM11D0"); return; }
        unsigned h0 = re15_re2d_granate_zahl();
        e->hp = 20;                                     /* RE2-Hund Zeile 9 Klammer 1 = 50: der Treffer toetet */
        const int16_t box[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_re2_gl_apply(p, 0, box, 0x10020009u);
        frame(1);
        int teile = 0;
        for (int i = 0; i < 7; i++) teile += (e->re2z_part_flags[s_hund_teile[i]] & 0x40u) ? 1 : 0;
        printf("   MESS u16: Werfer RE2-Hund: Treffer %d Zustand %d hp %d +0x1D2 %u, Teile mit 0x40 %d, Zaehler +%u\n",
               n, e->state, e->hp, (unsigned)e->re2z_hits1d2, teile, re15_re2d_granate_zahl() - h0);
        PRUEF(152, n >= 1 && e->hp < 0 && e->re2z_hits1d2 >= 3u && teile == 0,
              "Werfer-Explosivrunde: der RE2-Hund stirbt GANZ (+0x1D2 = %u >= 3 -> nur der Kern @0x801046A8-D4), "
              "Teile mit eigener Matrix %d (Soll 0)", (unsigned)e->re2z_hits1d2, teile);
    }
    /* (c) RE1.5-Hund: Handgranate -> derselbe RE2-Drift-Wurf; Werfer -> keiner */
    {
        re15_actor_t *e = hund_arena(RE15_AI_FLAVOR_RE15);
        if (!e) { PRUEF(153, 0, "kein Hund in ROOM11D0 (RE1.5)"); return; }
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        (void)re15_granate_r35_explosion(p, 0, 2);
        frame(1);
        int w4a = 0; for (int i = 0; i < 7; i++) w4a += ((e->re2z_part_flags[s_hund_teile[i]] & 0x4Au) == 0x4Au) ? 1 : 0;
        PRUEF(153, e->hp < 0 && w4a == 7, "RE1.5-Hund Handgranate: RE2-Drift-Wurf 0x4A %d/7 (hp %d)", w4a, e->hp);
    }
}

/* ================================================================================================================== */
/* [u9] Riesenform                                                                                                     */
/* ================================================================================================================== */
static int riesen_schuss(uint16_t raum, int w)
{
    spieler();
    g_current_room_id = raum;
    re15_actor_t *b = ziel(1, 0x30, -19600, 1000);
    int r = schuss(w);
    return r ? (1000 - b->hp) : -1;
}
static void abschnitt_u9(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_beretta_r37_reset();
    static const struct { int w; int g4; const char *n; } T[] = {
        { 3, 29,  "Browning w3 (Z3 0x01B0701D @0x800A5C0C)" },
        { 4, 29,  "SIG w4 (Z2 0x01B0701D @0x800A5BF8)" },
        { 5, 29,  "Beretta w5 je Kugel (Z4 0x01B0701D @0x800A5C20)" },
        { 7, 101, "Redhawk w7 (Z5 0x05F18865 @0x800A5C34)" },
        { 8, 70,  "Schrot w8 (Z7 0x03C10446 @0x800A5C5C)" },
        { 12, 9,  "Ingram w12 (Z15 0x00902409 @0x800A5CFC)" },
        { 13, 100,"SPAS w13 (Z8 0x05A17C64 @0x800A5C70)" },
        { 19, 20, "MC51 w19 (Z18 0x01405014 @0x800A5D38)" },
    };
    for (unsigned k = 0; k < sizeof T / sizeof T[0]; k++) {
        int d = riesen_schuss(0x5080, T[k].w);
        PRUEF(160 + (int)k, d == T[k].g4, "Riesenform ROOM5080 %s: Schaden %d, Soll %d (RE2-G4 Klammer 0)", T[k].n, d, T[k].g4);
    }
    /* Strang-Grenze: in 5090 bleibt die alte (Disc-Birkin-)Zeile */
    {
        int d = riesen_schuss(0x5090, 3);
        PRUEF(170, d == 15, "Riesenform ROOM5090 Browning: Schaden %d bleibt 15 (Disc-Zeile 0x30 @0x8006F15C; Grenze r42/finale)", d);
    }
    /* Werfer-Explosivrunde (Hitcode 0x10020009 -> Zeile 9 = 90 @0x800A5C84) und Handgranate (Art 2, D_G 251) */
    {
        spieler(); g_current_room_id = 0x5080;
        re15_actor_t *b = ziel(1, 0x30, -19600, 1000);
        const int16_t box[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
        const int32_t p[3] = { b->x + 300, b->y - 500, b->z };
        int n = re15_re2_gl_apply(p, 0, box, 0x10020009u);
        PRUEF(171, n >= 1 && 1000 - b->hp == 90, "Riesenform Werfer-Explosivrunde: Schaden %d, Soll 90 (Z9 0x05A1685A @0x800A5C84)",
              1000 - b->hp);
        spieler(); g_current_room_id = 0x5080;
        b = ziel(1, 0x30, -19600, 1000);
        const int32_t p2[3] = { b->x + 300, b->y - 500, b->z };
        n = re15_granate_r35_explosion(p2, 0, 2);
        PRUEF(172, n >= 1 && 1000 - b->hp == 251, "Riesenform Handgranate: Schaden %d, Soll D_G 251 (NUTZERWUNSCH 16.11.)",
              1000 - b->hp);
    }
}

/* ================================================================================================================== */
/* [u6] Beretta-Feuerstoss                                                                                             */
/* ================================================================================================================== */
static void nachlauf(void)
{
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_ai_re2_for_type(g_actors[s].type)) {
            if (re15_re2z_owns_type(g_actors[s].type)) re15_re2z_hit_filter_apply(s);
            else                                       re15_re2_pause_filter_apply(s);
        }
}
/* Ein Feuerstoss mit drei Patronen wie game_step; zwischen den Bildern loescht ein Gehirn ohne Reaktion den Riegel
 * (wie der Gorilla unter der Zuck-Schwelle, gegner_r39.c "+0x93 = 0 wie der Flinch-Exit") - `brain_loescht`. */
static void feuerstoss(int w, int brain_loescht)
{
    re15_inv_init(); re15_inv_grant((uint8_t)w, 15); re15_player_set_equipped_weapon(w);
    int r1 = re15_player_weapon_fire(w);
    re15_beretta_r37_feuerstoss(w, 3, r1);
    nachlauf();
    for (int t = 1; t <= 8; t++) {
        if (brain_loescht)
            for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && !re15_ai_re2_for_type(g_actors[s].type) && g_actors[s].hp >= 0)
                    g_actors[s].hit_react = 0;
        (void)re15_beretta_r37_tick_ex(1);
        nachlauf();
    }
}
static void abschnitt_u6(void)
{
    static const struct { uint8_t t; int sp5; uint16_t raum; const char *n; } T[] = {
        { 0x27, 28, 0x11C0, "Gorilla (@0x8006EE4C)" }, { 0x29, 25, 0x3000, "kleiner Affe 0x29 (@0x8006EEFC)" },
        { 0x2b, 20, 0x4000, "Tyrant (@0x8006EFAC)" }, { 0x30, 28, 0x3070, "Birkin Menschform (@0x8006F164)" },
    };
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    for (unsigned k = 0; k < sizeof T / sizeof T[0]; k++) {
        for (int br = 0; br < 2; br++) {
            re15_beretta_r37_reset();
            spieler(); g_current_room_id = T[k].raum;
            re15_actor_t *e = ziel(1, T[k].t, -19600, 2000);
            feuerstoss(5, br);
            re15_beretta_r37_mess_t m; re15_beretta_r37_mess(&m);
            PRUEF(180 + 2 * (int)k + br, e->hp == 2000 - T[k].sp5 && m.kugeln == 3 && m.treffer == 1,
                  "%s, Feuerstoss Beretta%s: EIN Treffer mit Spalte 5 = %d (hp-Abzug %d), Kugeln %u, Treffer %u (Disc: ein "
                  "`jal 0x80011f50` @0x8003396c)", T[k].n, br ? " (Gehirn loescht den Riegel)" : "", T[k].sp5, 2000 - e->hp,
                  m.kugeln, m.treffer);
        }
    }
    /* Gorilla vorn, Zombie dahinter: Kugel 2/3 gehen an den Zombie. RE1.5-KI mit RE2-Import-Zeile fuer den Zombie (Vorgabe
     * re15_re15_re2z_import): ohne Gegner-KI-Takte zaehlt hier niemand die RE2-Trefferpause +0x1D3 des RE2-Gehirns herunter,
     * darum laeuft der Zombie-Teil wie test_r37_beretta Teil B unter der RE1.5-KI (dort loest der Feuerstoss den Riegel). Die
     * Standard-KI mit echten Takten prueft integration_r37_beretta. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    {
        re15_beretta_r37_reset();
        spieler(); g_current_room_id = 0x11C0;
        re15_actor_t *g = ziel(1, 0x27, -19600, 2000);
        re15_actor_t *z = ziel(2, 0x10, -21600, 2000);
        feuerstoss(5, 1);
        re15_beretta_r37_mess_t m; re15_beretta_r37_mess(&m);
        PRUEF(190, g->hp == 2000 - 28 && z->hp < 2000 && m.treffer == 3,
              "Gorilla vorn + Zombie dahinter: Gorilla EIN Treffer (-%d), Zombie nimmt Kugel 2/3 (-%d), Treffer %u",
              2000 - g->hp, 2000 - z->hp, m.treffer);
    }
    /* Zombie allein: jede Kugel trifft (RE2 Custom Handgun, Regel 2) */
    {
        re15_beretta_r37_reset();
        spieler(); g_current_room_id = 0x1140;
        re15_actor_t *z = ziel(1, 0x10, -19600, 2000);
        feuerstoss(5, 0);
        re15_beretta_r37_mess_t m; re15_beretta_r37_mess(&m);
        PRUEF(191, m.treffer == 3, "Zombie allein: drei Treffer wie heute (Treffer %u, hp-Abzug %d)", m.treffer, 2000 - z->hp);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    /* Alligator 0x23: nicht angefasst (K-12) - drei Treffer zu 7 wie heute */
    {
        re15_beretta_r37_reset();
        spieler(); g_current_room_id = 0x2080;
        re15_actor_t *a = ziel(1, 0x23, -19600, 2000);
        feuerstoss(5, 0);
        re15_beretta_r37_mess_t m; re15_beretta_r37_mess(&m);
        PRUEF(192, m.treffer == 3 && a->hp == 2000 - 21, "Alligator unveraendert: drei Treffer zu 7 (Treffer %u, hp-Abzug %d)",
              m.treffer, 2000 - a->hp);
    }
}

/* ================================================================================================================== */
/* [u8] Gitterhaende ROOM1210                                                                                          */
/* ================================================================================================================== */
static int arme_aufsetzen(int *slots)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (!load_re2_bank(0x1A, 0x2D)) return 0;
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame(0);
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}
static void abschnitt_u8(void)
{
    if (room_load(0x1210, "STAGE1") != 0) { PRUEF(200, 0, "ROOM1210 fehlt"); return; }
    /* --- VORPRUEFUNG: kommt man ohne Schuss durch? Drei Wege (Ostwand, Westwand, Mitte), Gehtempo 75/Bild (WALK 0x4B,
     *     FUN_80041BE4), KEIN Tastendruck (der Griff haelt dann am laengsten, 151 Bilder @0x80100D5C), alle Arme wach. --- */
    int alle_durch = 1;
    for (int weg = 0; weg < 3; weg++) {
        int slots[RE15_ACTOR_MAX]; int n = arme_aufsetzen(slots);
        if (n < 8) { PRUEF(200, 0, "Arme nicht gespawnt (%d)", n); return; }
        int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
        for (int i = 0; i < n; i++) { if (g_actors[slots[i]].z < zmin) zmin = g_actors[slots[i]].z;
                                      if (g_actors[slots[i]].z > zmax) zmax = g_actors[slots[i]].z; }
        for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;      /* Member_set(12,1) @0x1EDA: alle wach */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int32_t z = zmax + 3000, ziel_z = zmin - 3000;
        int griffe = 0, war = 0, hp_min = pl->hp, bilder = 0, halte_max = 0, halte = 0;
        while (z > ziel_z && bilder < 20000) {
            int gegriffen = re15_player_is_grabbed();
            if (gegriffen && !war) griffe++;
            if (gegriffen) { halte++; if (halte > halte_max) halte_max = halte; } else halte = 0;
            war = gegriffen;
            if (!gegriffen) {
                int32_t px = -19500;
                if (weg < 2) {
                    const int32_t row  = (weg == 0) ? -14000 : -25000;
                    const int32_t goal = (weg == 0) ? (row - 750) : (row + 750);
                    for (int k = 0; k < 200; k++) {
                        if ((weg == 0 && px >= goal) || (weg == 1 && px <= goal)) break;
                        int32_t nx = px + ((weg == 0) ? 25 : -25), nz = z;
                        re15_collision_constrain(&s_rdt, px, z, &nx, &nz);
                        if (nx == px) break;
                        px = nx;
                    }
                }
                pl->x = px; pl->z = z;
                z -= 75;
            } else {
                z = pl->z;                                                /* der Arm haelt ihn an der Hand fest */
            }
            frame(0);
            if (pl->hp < hp_min) hp_min = pl->hp;
            bilder++;
        }
        static const char *namen[3] = { "Ostwand (Armreihe x -14000)", "Westwand (Armreihe x -25000)", "Flurmitte x -19500" };
        printf("   MESS u8 Vorpruefung %s: %d Arme wach, Griffe %d, laengster Griff %d Bilder, HP min %d, Bilder %d, "
               "durch %s\n", namen[weg], n, griffe, halte_max, hp_min, bilder, (z <= ziel_z) ? "ja" : "NEIN");
        if (!(z <= ziel_z && hp_min > 0)) alle_durch = 0;
    }
    PRUEF(200, alle_durch, "VORPRUEFUNG: der Flur ROOM1210 ist auf allen drei Wegen ohne Schuss passierbar (RE2-Griff ohne "
          "HP-Verlust, Hook 0x8010121C ohne FUN_800401D4)");
    /* --- Riegel: Schuesse und Messer gehen durch (Geometrie wie test_r37_beretta Teil J: freie Bahn, Arm 1400 vor Leon) --- */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        g_room_rdt_ok = 0;
        static const int W[6] = { 3, 4, 5, 7, 8, 12 };
        int getroffen = 0;
        for (int k = 0; k < 6; k++) {
            re15_beretta_r37_reset();
            spieler(); g_current_room_id = 0x1210;
            re15_actor_t *a = ziel(1, 0x1A, -19600, 250);
            int r = schuss(W[k]);
            printf("   u8 Waffe %d: Resolver %d, Arm-HP %d\n", W[k], r, a->hp);
            if (r == 2 || a->hp != 250) getroffen++;
        }
        /* Messer: der Klingenpunkt liegt an der Hand (re15_player_weapon_fire, Ursprung bei w < 3) - Arm 600 vor Leon */
        spieler(); g_current_room_id = 0x1210;
        re15_actor_t *a = ziel(1, 0x1A, -18800, 250);
        int rm = schuss(1);
        printf("   u8 Messer: Resolver %d, Arm-HP %d\n", rm, a->hp);
        PRUEF(201, getroffen == 0, "Gitterhaende: Browning/SIG/Beretta/Redhawk/Schrot/Ingram treffen keinen Arm (%d von 6 trafen) - "
              "Disc ohne Band-Stempel im Arm-Code 0x8010c1ec-0x8010d7a0", getroffen);
        PRUEF(202, rm == 0 && a->hp == 250, "Gitterhaende: das Messer trifft keinen Arm (Resolver %d, HP %d)", rm, a->hp);
        /* Explosion wirkt weiter (Bauplan Abschnitt 3): Werfer-Explosivrunde am Arm */
        spieler(); g_current_room_id = 0x1210;
        a = ziel(1, 0x1A, -19600, 250);
        const int16_t box[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
        const int32_t p[3] = { a->x + 300, a->y - 500, a->z };
        int n = re15_re2_gl_apply(p, 0, box, 0x10020009u);
        printf("   u8 Werfer am Arm: Treffer %d, HP %d\n", n, a->hp);
        PRUEF(203, n >= 1 && a->hp < 250, "Gitterhaende: die Werfer-Explosion trifft den Arm weiter (Treffer %d, HP %d)", n, a->hp);
    }
}

/* ================================================================================================================== */
/* Stufe 2 Runde 2 (Pruefbericht s2 r1 Gelb 2: die Vorpruefung 200 setzt die Spielerlage je Bild neu): Auswerter des ECHTEN
 * Durchgangs durch ROOM1210/1211 (integration_r42_gitter, RE15_STATE_LOG der exe, nur Vorwaertslaufen, kein Schuss).
 * Gelesen werden nur die Zeilen mit Arm-Klammern [.. t=1a ..] (der Raum der Arme). Gruen, wenn
 *   - in jeder dieser Zeilen die Spieler-HP 100 sind (ein Griff kostet keine HP),
 *   - mindestens ein Arm wach ist (st=1) - sonst prueft der Lauf nichts,
 *   - der Spieler hinter den letzten Arm kommt: min z(Spieler) < min z(Arm).
 * Messgroesse: Bilder mit Griff (gr != 0). Das ist eine VORBEDINGUNG des Baus (U8: "erst pruefen, ob man ohne Schiessen
 * vorbeikommt"), sie ist auf jedem Stand gruen. */
static int pruef_gitter(const char *pfad)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FAIL: kein Log %s\n", pfad); return 1; }
    static char z[65536];
    int zeilen = 0, hp_falsch = 0, wach = 0, griff = 0, hp_min = 1000;
    int32_t pz_min = 0x7fffffff, az_min = 0x7fffffff;
    while (fgets(z, sizeof z, f)) {
        if (!strstr(z, " t=1a ")) continue;
        int px, pz, rot, hp;
        const char *pl = strstr(z, "PL(");
        if (!pl || sscanf(pl, "PL(%d,%d,rot=%d,hp=%d)", &px, &pz, &rot, &hp) != 4) continue;
        zeilen++;
        if (hp != 100) hp_falsch++;
        if (hp < hp_min) hp_min = hp;
        if (pz < pz_min) pz_min = pz;
        const char *g = strstr(z, " gr=");
        int gr = 0;
        if (g && sscanf(g, " gr=%d", &gr) == 1 && gr != 0) griff++;
        for (const char *q = strchr(z, '['); q; q = strchr(q + 1, '[')) {
            int slot, st; unsigned t;
            if (sscanf(q, "[%d t=%x st=%d", &slot, &t, &st) != 3 || t != 0x1a) continue;
            const char *a = strstr(q, "@(");
            int ax, az;
            if (st == 1) {
                wach++;
                if (a && sscanf(a, "@(%d,%d", &ax, &az) == 2 && az < az_min) az_min = az;
            }
        }
    }
    fclose(f);
    printf("pruef_gitter: Zeilen %d, HP nicht 100 in %d (min %d), wache Arm-Eintraege %d, Bilder mit Griff %d, "
           "Spieler min z %d, letzter Arm z %d\n", zeilen, hp_falsch, hp_min, wach, griff, (int)pz_min, (int)az_min);
    return (zeilen > 0 && hp_falsch == 0 && wach > 0 && pz_min < az_min) ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (argc > 2 && !strcmp(teil, "pruef_gitter")) return pruef_gitter(argv[2]);
    int alle = !strcmp(teil, "alle");
    if (!strcmp(teil, "aus")) {
        if (!getenv("RE15_KEIN_80PROZENT_GORE")) {
#ifdef _WIN32
            _putenv("RE15_KEIN_80PROZENT_GORE=1");
#else
            setenv("RE15_KEIN_80PROZENT_GORE", "1", 1);
#endif
        }
        printf("[aus] RE15_KEIN_80PROZENT_GORE=1\n");
        abschnitt_u5(1);
        abschnitt_u7(1);
    } else {
        if (alle || !strcmp(teil, "u4"))  { printf("[u4] Birkin Menschform\n");  abschnitt_u4(); }
        if (alle || !strcmp(teil, "u3"))  { printf("[u3] Gorilla\n");            abschnitt_u3(); }
        if (alle || !strcmp(teil, "u5"))  { printf("[u5] Kraehe\n");             abschnitt_u5(0); }
        if (alle || !strcmp(teil, "u7"))  { printf("[u7] Spinne\n");             abschnitt_u7(0); }
        if (alle || !strcmp(teil, "u16")) { printf("[u16] Hund\n");              abschnitt_u16(); }
        if (alle || !strcmp(teil, "u9"))  { printf("[u9] Riesenform\n");         abschnitt_u9(); }
        if (alle || !strcmp(teil, "u6"))  { printf("[u6] Feuerstoss\n");         abschnitt_u6(); }
        if (alle || !strcmp(teil, "u8"))  { printf("[u8] Gitterhaende\n");       abschnitt_u8(); }
    }
    printf("test_r42_tiere %s: %d Fehler\n", teil, s_fehler);
    return s_erste;
}
