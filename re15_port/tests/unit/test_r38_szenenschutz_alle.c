/**
 * @file test_r38_szenenschutz_alle.c
 * @brief Runde 38 Spur B "szenenschutz" — Zensus ALLER Gegnertypen (Skill re15-room-probe, ohne exe).
 *
 * Nutzer (AUFTRAG.md 2026-10-16): "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal
 * bewegen kann ist mist. ... Es sei den es ist Teil der Cutscene wie bei Elza Room 1170." Dossier
 * analysis/befunde_runde38/B_szenenschutz.md Abschnitt 2.5/2.6.
 *
 * Je Gegnertyp aus dem Zensus "Szene haelt den Spieler x Gegner" (Dossier 2.5: 68 Raeume, Typen 0x10 0x11 0x16 0x18
 * 0x1A 0x20 0x21 0x24 0x25 0x26 0x27 0x29 0x2B 0x2D 0x30 0x36) und je Gehirn (RE2-Standard, RE1.5) zwei gleiche
 * Laeufe ueber N Bilder, Spieler steht:
 *   frei      g_scd.player_mode = 0 -> KONTROLLE: derselbe Aufbau greift den Spieler an (HP-Verlust, Treffer-
 *             Riegel +0x93 oder Griff). Greift er nicht an, misst der Fall nichts und gilt als nicht bestanden.
 *   gehalten  g_scd.player_mode = 2 (die Szene haelt den Spieler) -> KEIN HP-Verlust, KEIN Treffer (Riegel +0x93 bleibt
 *             0), KEIN Griff (Opfer-FSM 0, PL+0x1D3-Sicht frei) — und das Sicherheitsnetz griff NICHT ein (abgefangen
 *             == 0): das Tor im Gehirn hielt den Angriff ab, nicht erst die HP-Rueckgabe.
 * Ausgenommen (kein Angriff im Gehirn, im Zensus belegt): Effekt-Schwarm 0x24 (harmlos), Efeu 0x2D (ruhendes
 * Skript-Requisit). Feuer 0x26 ist ein Umgebungsschaden ohne Tor (-2 je Kontaktbild): dort zaehlt der Fall nur,
 * dass das Sicherheitsnetz den Abzug zurueckgibt.
 */
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_szenenschutz_r38.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int16_t re15_atan2_q12(int32_t, int32_t);
extern int re15_player_victim_state(void);
extern int re15_player_is_grabbed(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static void face(re15_actor_t *e, const re15_actor_t *pl)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
}

typedef struct {
    int      erster;     /* erstes Bild mit Treffer (HP-Verlust / +0x93) oder Griff (-1 = keins) */
    int16_t  hp;         /* Spieler-HP am Ende */
    int      treffer;    /* Bilder mit gesetztem Treffer-Riegel +0x93 Bit 0 nach dem Bild */
    int      griff;      /* Opfer-FSM jemals != 0 oder PL+0x1D3-Sicht nach dem Bild */
    int      angriff;    /* Bilder im Angriffs-Unterzustand (nur Typen mit Praedikat, s. angriffs_zustand) */
    uint32_t ticks, ab;  /* geschuetzte Ticks, abgefangene Abzuege */
} lauf_t;

/* Aufbau: Gegner `type` in Slot 5 frisch (Zustand 0 = INIT), dem Spieler zugewandt, `abstand` vor ihm; nach dem
 * INIT-Bild optional `nach_init` (fuehrt den Gegner in sein freies Jagd-Unterprogramm, wie die Typ-Tests). */
typedef void (*nach_init_fn)(re15_actor_t *e, re15_actor_t *pl);

/* Angriffs-Unterzustaende der Gehirne OHNE Spieler-Tor am Angriff (die Kraehen), aus dem Code gelesen:
 *   RE2-Kraehe (EMOVL21): Sub 11 Anlauf (EXEC 0x80101D98), 12 STRIKE (0x80101FB0), 13 Hover-Harass (0x80102260),
 *                         14 GRAB (0x8010249C) — Eintritt nur ueber den Arbiter 0x801041F0 bzw. 11/13;
 *   RE1.5-Kraehe:         Sub 11 Dive (0x8011376c-Zweig), 12 Grapple (move[12] 0x80113c7c), 13 Griff,
 *                         15 Homing-Descent, 16 STRIKE (@0x801144f0) — Commit @0x8011330c-70.
 * Andere Typen: 0 (ihr Angriff haengt am Spieler-Tor, gemessen ueber Treffer/Griff/Netz). */
static int angriffs_zustand(const re15_actor_t *e)
{
    if (e->type != 0x21 || e->state != 1) return 0;
    if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2)
        return e->sub_state_1 >= 11 && e->sub_state_1 <= 14;
    return e->sub_state_1 == 11 || e->sub_state_1 == 12 || e->sub_state_1 == 13 ||
           e->sub_state_1 == 15 || e->sub_state_1 == 16;
}
typedef void (*je_bild_fn)(re15_actor_t *e, re15_actor_t *pl);

static lauf_t lauf(uint8_t type, int32_t abstand, int32_t ey, int32_t py, int halten, int bilder,
                   nach_init_fn nach_init, je_bild_fn je_bild)
{
    lauf_t r; memset(&r, 0, sizeof r); r.erster = -1;
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_szenenschutz_zaehler_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = py; pl->z = 0; pl->rot_y = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 1; pl->motion = 0; pl->floor = 0;
    re15_actor_t *e = &g_actors[5];
    memset(e, 0, sizeof(*e));
    e->active = 1; e->type = type; e->hp = 100; e->flags = 1; e->em_flag_id = 0xFF;
    e->state = 0; e->x = 0; e->y = ey; e->z = abstand;
    face(e, pl);
    re15_enemy_apply_hitbox(e, type);
    g_scd.player_mode = halten ? 2 : 0;
    for (int f = 0; f < bilder; f++) {
        pl->x = 0; pl->z = 0; pl->y = py;              /* Leon steht (Szene / still) */
        if (f == 1 && nach_init) nach_init(e, pl);
        if (je_bild) je_bild(e, pl);
        int16_t vor = pl->hp;
        frame();
        int vs = re15_player_victim_state();
        int gr = vs != 0 || re15_player_is_grabbed();
        if (gr) r.griff = 1;
        if (pl->hit_react & 1u) r.treffer++;
        if (angriffs_zustand(e)) r.angriff++;
        if (r.erster < 0 && (pl->hp < vor || gr || (pl->hit_react & 1u) || angriffs_zustand(e))) r.erster = f;
        pl->hit_react = 0;                             /* der Spieler-Handler loescht +0x93 je Bild (@0x80031964) */
    }
    r.hp = pl->hp;
    re15_szenenschutz_zaehler(&r.ticks, &r.ab);
    g_scd.player_mode = 0;
    return r;
}

/* ---- Jagd-Aufbauten aus den Typ-Tests ---------------------------------------------------------------------- */
/* RE2-Kraehe (test_re2_crow_ai.c test_arbitration_single_claim): fliegend im Kreisen (Sub 4), Hoehe -3000. */
static void kraehe_re2_flug(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    e->state = 1; e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; e->y = -3000;
}
static void kraehe_re2_blick(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->state == 1 && e->sub_state_1 == 4) face(e, pl);   /* wie face_player je Bild im Typ-Test */
}
/* Gorilla/Affe 0x27 (test_maggot_ai.c (2b)): Jagd Sub 3 Phase 1, im Sub 3/4 dem Spieler zugewandt. */
static void affe_jagd(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 1; e->sub_state_3 = 0;
}
static void affe_blick(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->state == 1 && (e->sub_state_1 == 3 || e->sub_state_1 == 4)) face(e, pl);
}
/* RE1.5-Spinne 0x25 (test_adult_spider_ai.c (2c)): grid 1 = APPROACH. */
static void spinne15_naehe(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    e->grid_id = 1;
}

/* EM027-Bank (CDEMD0.EMS Eintrag 12) wie test_maggot_ai.c load_maggot_bank: Fussabdruck-Wurzelbewegung + Biss-Bones. */
static int affe_bank(void)
{
    if (re15_enemy_find(0x27)) return 1;
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return 0; }
    fclose(f);
    size_t off, len;
    if (re15_ems_get_entry(buf, (size_t)n, 12, &off, &len)) { free(buf); return 0; }
    re15_enemy_bank_t *bank = re15_enemy_alloc(0x27);
    if (!bank) { free(buf); return 0; }
    if (re15_emd_parse_loco_bank(buf + off, len, &bank->skel, &bank->anim)) { free(buf); return 0; }
    bank->ok = 1;
    bank->victim_ok = (re15_emd_parse_victim_bank(buf + off, len, &bank->skel_victim, &bank->anim_victim) == 0);
    bank->buf = buf;
    return 1;
}
static void affe_blick_pin(re15_actor_t *e, re15_actor_t *pl)
{
    affe_blick(e, pl);
    e->mag_pin_cd = 200;                               /* Aufbaeumen geparkt wie test_maggot_ai.c (2b) */
}
/* RE1.5-Spinne: GRAB-A (Sub2 6) zwischen den Zyklen erneut erzwungen wie test_adult_spider_ai.c (2c) —
 * misst das Kontakt-Tor @0x80112514-40 (pl->hit_react == 0). */
static void spinne15_grab(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    if (e->state == 1 && e->sub_state_2 != 6) { e->sub_state_1 = 1; e->sub_state_2 = 6; e->sub_state_3 = 0; }
}

typedef struct {
    const char *name; uint8_t type; int32_t abstand[4]; int32_t ey; int32_t py; int bilder;
    nach_init_fn nach_init; je_bild_fn je_bild; int flavor_maske; /* 1 = RE2, 2 = RE1.5 */
    int ohne_angriff;                                              /* Gehirn ohne Spielerangriff (belegt) */
    int netz;                                                      /* Umgebungsschaden ohne Tor: Netz zaehlt */
} fall_t;


/* ===================== Teil 2: die Szene beginnt MITTEN im Kraehen-Anflug ===================================== */
/* Frischer Aufbau wie lauf(), aber Bild fuer Bild von aussen gefuehrt. */
static re15_actor_t *aufbau(uint8_t type, int32_t abstand, int32_t ey, int32_t py)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_szenenschutz_zaehler_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = py; pl->z = 0; pl->rot_y = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 1; pl->motion = 0; pl->floor = 0;
    re15_actor_t *e = &g_actors[5];
    memset(e, 0, sizeof(*e));
    e->active = 1; e->type = type; e->hp = 100; e->flags = 1; e->em_flag_id = 0xFF;
    e->state = 0; e->x = 0; e->y = ey; e->z = abstand;
    face(e, pl);
    re15_enemy_apply_hitbox(e, type);
    return e;
}
/* n Bilder gehalten: Treffer/Griff/Angriffs-Bilder zaehlen (Spieler steht, +0x93 je Bild geloescht). */
static void gehalten(re15_actor_t *e, int32_t py, int n, int *hp_verlust, int *griff, int *angriff, int *treffer)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    *hp_verlust = 0; *griff = 0; *angriff = 0; *treffer = 0;
    for (int f = 0; f < n; f++) {
        pl->x = 0; pl->z = 0; pl->y = py;
        int16_t vor = pl->hp;
        frame();
        if (pl->hp < vor) *hp_verlust += vor - pl->hp;
        if (re15_player_victim_state() != 0 || re15_player_is_grabbed()) *griff = 1;
        if (angriffs_zustand(e)) (*angriff)++;
        if (pl->hit_react & 1u) (*treffer)++;
        pl->hit_react = 0;
    }
}

static void teil2(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int hv, gr, an, tr;

    /* (a) RE2-Kraehe: frei bis zum Claim (Sub 11 Anlauf / Sub 13 Hover), dann haelt die Szene den Spieler. Erwartet:
     *     der Original-Abbruch 0x80104400 im DEC desselben Bilds -> Sub 4, Cooldown +0x21C = 30 (@0x8010444c-54). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *e = aufbau(0x21, 2500, -3000, 0);
    g_scd.player_mode = 0;
    int sub_vor = -1;
    for (int f = 0; f < 600 && sub_vor < 0; f++) {
        pl->x = 0; pl->z = 0;
        if (f == 1) kraehe_re2_flug(e, pl);
        kraehe_re2_blick(e, pl);
        frame();
        if (e->state == 1 && (e->sub_state_1 == 11 || e->sub_state_1 == 13)) sub_vor = e->sub_state_1;
    }
    CHECK(sub_vor > 0, "T2a RE2-Kraehe frei im Angriffs-Anflug (Sub %d)", sub_vor);
    g_scd.player_mode = 2;
    pl->x = 0; pl->z = 0;
    frame();
    int sub_nach = e->sub_state_1, cd_nach = e->re2d_abort21c;
    printf("  [T2a] Szene beginnt in Sub %d -> nach 1 Bild Sub %d, Cooldown +0x21C=%d, Flags +0x22A=0x%04X\n",
           sub_vor, sub_nach, cd_nach, (unsigned)e->re2c_flags22a);
    CHECK(sub_nach == 4 && cd_nach == 30 && !(e->re2c_flags22a & 0x10u),
          "T2a Abbruch ueber den Original-Listener: Sub 4, Cooldown 30, Bit 0x10 verbraucht (@0x80104440-64)");
    gehalten(e, 0, 400, &hv, &gr, &an, &tr);
    printf("  [T2a] 400 Bilder gehalten: HP-Verlust %d, Griff %d, Angriffs-Bilder %d, Treffer %d\n", hv, gr, an, tr);
    CHECK(hv == 0 && gr == 0 && an == 0 && tr == 0, "T2a gehalten: kein Griff/Strike/Treffer");
    g_scd.player_mode = 0;
    int wieder = -1;
    for (int f = 0; f < 900 && wieder < 0; f++) {
        pl->x = 0; pl->z = 0;
        kraehe_re2_blick(e, pl);
        frame();
        if (angriffs_zustand(e)) wieder = f;
        pl->hit_react = 0;
    }
    printf("  [T2a] Szene vorbei: Angriffs-Anflug wieder nach %d Bildern\n", wieder);
    CHECK(wieder >= 0, "T2a nach der Szene greift die Kraehe wieder an");

    /* (b) RE1.5-Kraehe: frei bis zum Grapple-Anflug (Sub 12, Commit @0x8011330c-70), dann gehalten: der Kontakt
     *     @0x80113dd4 greift nicht; Sub 12 laeuft ueber seinen Zeitablauf @0x80113e68-78 aus. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    e = aufbau(0x21, 800, 700, 700);
    g_scd.player_mode = 0;
    sub_vor = -1;
    for (int f = 0; f < 900 && sub_vor < 0; f++) {
        pl->x = 0; pl->z = 0; pl->y = 700;
        frame();
        if (e->state == 1 && e->sub_state_1 == 12) sub_vor = 12;
        pl->hit_react = 0;
    }
    CHECK(sub_vor == 12, "T2b RE1.5-Kraehe frei im Grapple-Anflug (Sub 12)");
    g_scd.player_mode = 2;
    gehalten(e, 700, 400, &hv, &gr, &an, &tr);
    printf("  [T2b] 400 Bilder gehalten: HP-Verlust %d, Griff %d, Angriffs-Bilder %d (Sub 12-Rest), Treffer %d, Sub am Ende %d\n",
           hv, gr, an, tr, e->sub_state_1);
    CHECK(hv == 0 && gr == 0 && tr == 0, "T2b gehalten: der Grapple-Kontakt greift nicht (@0x80113dd4)");

    /* (c) RE1.5-Kraehe im Homing-Descent (Sub 15, vom Schwarm-Dispatcher gesetzt) direkt am Spieler: frei -> Strike
     *     (Sub 16 @0x80114464), gehalten -> kein Strike. */
    for (int halten = 0; halten < 2; halten++) {
        e = aufbau(0x21, 300, 700, 700);
        frame();                                       /* INIT */
        e->state = 1; e->sub_state_1 = 15; e->sub_state_2 = 0; e->sub_state_3 = 0;
        g_scd.player_mode = halten ? 2 : 0;
        int strike = -1;
        for (int f = 0; f < 200 && strike < 0; f++) {
            pl->x = 0; pl->z = 0; pl->y = 700;
            if (e->sub_state_1 != 15 && e->sub_state_1 != 16) { e->sub_state_1 = 15; e->sub_state_2 = 0; }
            int16_t vor = pl->hp;
            frame();
            if (e->sub_state_1 == 16 || pl->hp < vor) strike = f;
            pl->hit_react = 0;
        }
        printf("  [T2c] %s: Strike (Sub 16) bei Bild %d, hp %d\n", halten ? "gehalten" : "frei", strike, pl->hp);
        if (halten) CHECK(strike < 0 && pl->hp == 100, "T2c gehalten: kein Strike aus dem Homing-Descent (@0x80114464)");
        else        CHECK(strike >= 0, "T2c frei: Kontrolle — der Homing-Descent schlaegt zu");
    }
    g_scd.player_mode = 0;
}

int main(void)
{
    static const fall_t faelle[] = {
        { "Zombie 0x10",          0x10,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Zombie 0x11",          0x11,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Zombie 0x12",          0x12,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Zombie 0x16",          0x16,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Zombie 0x18",          0x18,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Zombie-Maedchen 0x13", 0x13,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Hund 0x20",            0x20, { 1500 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Kraehe 0x21 (Flug)",   0x21, { 2500 }, -3000,   0,  900, kraehe_re2_flug, kraehe_re2_blick, 1, 0, 0 },
        { "Kraehe 0x21 (Sitz)",   0x21,  { 800 },   700, 700,  900, NULL, NULL, 2, 0, 0 },
        { "Spinne 0x25",          0x25, { 1500 },     0,   0,  900, NULL, NULL, 1, 0, 0 },
        { "Spinne 0x25 (grid 1)", 0x25,  { 600 },     0,   0,  600, spinne15_naehe, spinne15_grab, 2, 0, 0 },
        { "Affe 0x27",            0x27, { 1200, 1600, 2000, 2400 }, 0, 0, 600, affe_jagd, affe_blick_pin, 3, 0, 0 },
        { "Kakerlake 0x29",       0x29,  { 900 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Tyrant 0x2B",          0x2B, { 1500 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Birkin 0x30",          0x30, { 1500 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Birkin 0x36",          0x36, { 1500 },     0,   0,  900, NULL, NULL, 3, 0, 0 },
        { "Effekt 0x24",          0x24, { 1500 }, -1000,   0,  600, NULL, NULL, 3, 1, 0 },
        { "Efeu 0x2D",            0x2D,  { 900 },     0,   0,  600, NULL, NULL, 3, 1, 0 },
        { "Feuer 0x26",           0x26,  { 300 },     0,   0,  300, NULL, NULL, 3, 0, 1 },
    };
    const int n = (int)(sizeof faelle / sizeof faelle[0]);
    CHECK(affe_bank(), "EM027-Bank geladen (CDEMD0.EMS Eintrag 12)");
    printf("Typ                    Gehirn | gehalten: erster hp treffer griff angriff netz ticks | frei: erster hp treffer griff angriff\n");
    for (int fl = 0; fl < 2; fl++) {
        re15_ai_flavor_set(fl == 0 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        const char *gn = fl == 0 ? "RE2 " : "RE15";
        for (int i = 0; i < n; i++) {
            const fall_t *c = &faelle[i];
            if (!(c->flavor_maske & (fl == 0 ? 1 : 2))) continue;
            /* Abstands-Sweep wie die Typ-Tests: der erste Abstand, an dem der FREIE Lauf angreift, gilt fuer beide */
            int32_t d = c->abstand[0];
            lauf_t f = lauf(c->type, d, c->ey, c->py, 0, c->bilder, c->nach_init, c->je_bild);
            for (int k = 1; k < 4 && f.erster < 0 && c->abstand[k] > 0; k++) {
                d = c->abstand[k];
                f = lauf(c->type, d, c->ey, c->py, 0, c->bilder, c->nach_init, c->je_bild);
            }
            lauf_t g = lauf(c->type, d, c->ey, c->py, 1, c->bilder, c->nach_init, c->je_bild);
            printf("%-22s %s | %5d %4d %4d %d %4d %3u %5u | %5d %4d %4d %d %4d\n", c->name, gn,
                   g.erster, g.hp, g.treffer, g.griff, g.angriff, g.ab, g.ticks,
                   f.erster, f.hp, f.treffer, f.griff, f.angriff);
            if (c->ohne_angriff) {
                CHECK(f.erster < 0 && g.erster < 0 && f.hp == 100,
                      "%s %s: Gehirn ohne Spielerangriff (frei und gehalten kein Treffer)", c->name, gn);
                continue;
            }
            CHECK(f.erster >= 0, "%s %s frei: Kontrolle — der Aufbau greift an (Bild %d, hp %d)",
                  c->name, gn, f.erster, f.hp);
            if (c->netz) {
                CHECK(g.hp == 100 && g.ab > 0,
                      "%s %s gehalten: Umgebungsschaden ohne Tor, das Sicherheitsnetz gibt ihn zurueck (hp %d, %u)",
                      c->name, gn, g.hp, g.ab);
                continue;
            }
            CHECK(g.erster < 0 && g.hp == 100 && g.treffer == 0 && g.griff == 0 && g.angriff == 0 && g.ab == 0,
                  "%s %s gehalten: kein Treffer/Griff/Angriff, Tor statt Netz (erster %d hp %d treffer %d griff %d "
                  "angriff %d netz %u)", c->name, gn, g.erster, g.hp, g.treffer, g.griff, g.angriff, g.ab);
        }
    }
    teil2();
    printf("%s (%d Fehler)\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
