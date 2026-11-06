/* probe_r41_gore_inventur.c — MESSSONDE (kein ctest), Runde 41 Spur S Schritt 3/5: welches Gore-Merkmal ist unter der
 * Standard-KI (RE2) und unter der RE1.5-KI erreichbar? Je Typ und Gehirn: INIT-Takt, dann ein toedlicher Treffer mit
 * der RE1.5-Waffen-Id w und der Hoehe c (wie der Resolver: +0x5 = w @0x800124bc, +0x6 = c, +0x93 = 1 | Krit), fuer die
 * RE2-Zombie-Familie +0x5 = RE2-Zeile (re15_re2z_weapon_id) und +0x1D2 = Spalte (wie RE15_ZGORE_TEST), dann 60 Takte
 * re15_enemy_ai_run_all. Gemessen: Besitz (welches Gehirn), RE1.5-Teilschicht (an, Masken Scherben/Flug/aus),
 * RE2-Part-Bruecke (re15_re2z_gore_active, Parts mit 0x40 / Flagwort 0 gegenueber dem INIT-Stand), ESP-Hoechststand,
 * Endzustand. Ausgabe je Zeile, Auswertung im Dossier S_spinnengore.md (Inventur).
 * Aufruf: probe_r41_gore_inventur   (Saat fest, bankfrei wie die Unit-KI-Pins) */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_teile_r15.h"
#include <stdio.h>
#include <string.h>

int      re15_re2z_gore_active(const re15_actor_t *e);
unsigned re15_re2z_weapon_id(unsigned w);
int      re15_re2spider_owns(const re15_actor_t *e);

#define S 2
typedef struct { uint8_t typ; int w, c, krit; const char *name; } fall_t;

static void lauf(const fall_t *f, re15_ai_flavor_t fl)
{
    re15_actor_init();
    re15_ai_flavor_set(fl);
    re15_damage_seed_rng(7);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[S];
    memset(pl, 0, sizeof *pl); memset(e, 0, sizeof *e);
    pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = -2000;
    e->active = 1; e->type = f->typ; e->em_flag_id = 0xFF; e->x = 0; e->z = 0;
    re15_teile_reset(S); re15_esp_fx_reset();
    for (int i = 0; i < 2; i++) re15_enemy_ai_run_all(1);      /* INIT */
    uint16_t vor[20]; memcpy(vor, e->re2z_part_flags, sizeof vor);
    int re2zom = re15_ai_re2_for_type(e->type) && (f->typ >= 0x10 && f->typ <= 0x18);
    e->hp = -1; e->state = 3; e->sub_state_3 = 0;
    e->sub_state_1 = (uint8_t)(re2zom ? re15_re2z_weapon_id((unsigned)f->w) : (unsigned)f->w);
    /* RE2-Besitz: der Stempel re15_re2_stamp_hit nullt +0x6 (re15_damage.c, Kommentar @Typ 0x26) -> 0. */
    int re2own = re15_ai_re2_for_type(e->type) && (re2zom || f->typ == 0x1a || f->typ == 0x20 || f->typ == 0x21
                 || ((f->typ == 0x25 || f->typ == 0x26) && re15_re2spider_owns(e)));
    e->sub_state_2 = (uint8_t)(re2own ? 0 : f->c);
    if (re2zom) e->re2z_hits1d2 = (uint8_t)f->c;
    e->hit_react = (uint8_t)(1 | (f->krit ? 0x40 : 0));
    int fxmax = 0;
    for (int i = 0; i < 60; i++) { re15_enemy_ai_run_all(1); int n = re15_esp_fx_count(); if (n > fxmax) fxmax = n; }
    const re15_teil_t *t = re15_teile(S);
    unsigned sh = 0, flg = 0, aus = 0;
    for (int k = 0; k < 20; k++) {
        if (t[k].w & 0x08) sh |= 1u << k;
        if (t[k].w & 0x20) flg |= 1u << k;
        if (!(t[k].w & 1)) aus |= 1u << k;
    }
    unsigned r40 = 0, r0 = 0;
    for (int k = 0; k < 20; k++) {
        if ((e->re2z_part_flags[k] & 0x40) && !(vor[k] & 0x40)) r40 |= 1u << k;
        if (e->re2z_part_flags[k] == 0 && vor[k] != 0) r0 |= 1u << k;
    }
    const char *owner = "RE1.5";
    if (re15_ai_re2_for_type(e->type)) {
        if (f->typ == 0x25 || f->typ == 0x26) owner = re15_re2spider_owns(e) ? "RE2" : "RE1.5";
        else if (f->typ == 0x1a || f->typ == 0x20 || f->typ == 0x21 || f->typ == 0x23 || re2zom) owner = "RE2";
    }
    printf("%-34s %-4s owner=%-5s teile=%d sh=%05x fl=%05x aus=%05x | re2bridge=%d p40=%05x p0=%05x | fx=%2d st=%d grid=%02x hp=%d\n",
           f->name, fl == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE15", owner, re15_teile_an(S), sh, flg, aus,
           re15_re2z_gore_active(e), r40, r0, fxmax, e->state, e->grid_id, e->hp);
}

int main(void)
{
    static const fall_t f[] = {
        { 0x10, 7, 1, 1, "Zombie w7 Mitte (Kopf)" },
        { 0x10, 7, 0, 1, "Zombie w7 unten (Beine)" },
        { 0x10, 8, 1, 1, "Zombie w8 Mitte Krit (Zerfetzen)" },
        { 0x10, 8, 0, 1, "Zombie w8 unten (Beine)" },
        { 0x10, 19, 0, 0, "Zombie w19 MC51 unten (Beine)" },
        { 0x10, 9, 1, 0, "Zombie w9 Granate" },
        { 0x10, 3, 1, 0, "Zombie w3 Pistole" },
        { 0x13, 7, 1, 1, "Zombie-Girl w7 Mitte" },
        { 0x1a, 7, 1, 1, "Wuergearm w7 (Hand ab)" },
        { 0x20, 7, 1, 1, "Hund w7" },
        { 0x21, 7, 1, 1, "Kraehe w7" },
        { 0x25, 7, 1, 1, "Spinne w7" },
        { 0x25, 3, 1, 0, "Spinne w3" },
        { 0x27, 7, 1, 1, "Gorilla/Maggot 0x27 w7" },
        { 0x29, 7, 1, 1, "0x29 w7" },
    };
    for (unsigned i = 0; i < sizeof f / sizeof f[0]; i++) {
        lauf(&f[i], RE15_AI_FLAVOR_RE2);
        lauf(&f[i], RE15_AI_FLAVOR_RE15);
    }
    return 0;
}
