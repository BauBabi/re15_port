/* probe_marke_treffbar.c — GEGENPRUEFUNG des Befundes "F9-Marke deutet den RE2-Fall falsch".
 *
 * Gemessen wird NICHT die Absicht, sondern das Artefakt:
 *   (A) Was traegt +0x93 nach dem RE2-Filter (re15_re2z_hit_filter_apply), wenn eines der
 *       vier Original-Gates (@0x80047124-30 / 38-40 / 48-50 / 58-64) sperrt?
 *   (B) Was sagt das Praedikat der Marke (platform/pc/main.c:5148-5151) zu genau diesem Wert?
 *   (C) Was macht der Aufloeser (re15_player_weapon_fire -> FUN_80011F50-Port) bei GLEICHER
 *       Geometrie mit und ohne Sperre?
 * Kontrolle: derselbe Zombie ohne Sperre MUSS Schaden nehmen, sonst misst die Sonde nichts.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern void re15_re2z_hit_filter_apply(int slot);

#define SLOT 2

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* EXAKTE Kopie der Marken-Deutung aus platform/pc/main.c:5147-5151 (nur lesen). */
static const char *marke_urteil(const re15_actor_t *_en)
{
    const char *_ziel = "ja";
    if (_en->hit_radius_min <= 0)           _ziel = "NEIN: keine Trefferbox";
    else if (_en->state == 7)               _ziel = "NEIN: Leiche (st 7)";
    else if ((_en->hit_react & 0x3) == 0x3) _ziel = "NEIN: Ein-Treffer-Riegel +0x93 (Bits 0+1)";
    else if (_en->hp < 0)                   _ziel = "NEIN: hp < 0";
    return _ziel;
}
/* Das im Befund vorgeschlagene Praedikat. */
static const char *marke_urteil_alt(const re15_actor_t *_en)
{
    if (_en->hit_radius_min <= 0)     return "NEIN: keine Trefferbox";
    if (_en->state == 7)              return "NEIN: Leiche (st 7)";
    if ((_en->hit_react & 1u) != 0)   return "NEIN: +0x93 Bit 0";
    if (_en->hp < 0)                  return "NEIN: hp < 0";
    return "ja";
}

static re15_actor_t *mk_zombie(int32_t x, int32_t z)
{
    re15_actor_t *e = &g_actors[SLOT];
    memset(e, 0, sizeof *e);
    e->active = 1;
    e->type   = 0x10;
    e->hp     = 79;
    e->state  = 1;
    e->sub_state_1 = 1;
    e->hit_radius_min = 300;
    e->x = x; e->y = 0; e->z = z;
    e->rot_y = 0;
    return e;
}

static void mk_player(int16_t rot)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(p, 0, sizeof *p);
    p->active = 1;
    p->hp = 100;
    p->x = 0; p->y = 0; p->z = 0;
    p->rot_y = rot;
    p->hit_radius_min = 300;
}

int main(void)
{
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    /* ---- (A)+(B): der Filter sperrt ueber Gate (2)/(4) -> welcher +0x93-Wert steht? ------- */
    /* ⚠ +0x10E & 0x4000 steht hier NICHT: ausserhalb der Spawn-Pose raeumt der Filter dieses
     * Bit selbst weg (@0x80104F0C / @0x80103914, enemy_ai_re2_zombie.c:8205-8209) - GEMESSEN:
     * +0x93 bleibt 0x00. Es ist also kein Sperr-Fall und taugt nicht als Beleg. */
    static const struct { const char *name; uint8_t d3; uint16_t f10e; int16_t hp; } F[] = {
        { "Gate2 +0x1D3=0x80 (Sturz-Claim @0x80103304)", 0x80, 0x0000,  79 },
        { "Gate2 +0x1D3=0x0F (Griff-Claim @0x8010276C)", 0x0F, 0x0000,  79 },
        { "Gate4 +0x10E&0x8000 (@0x80047164)",           0x00, 0x8000,  79 },
        { "Gate3 HP < 0 (@0x80047150 bltz)",             0x00, 0x0000,  -1 },
    };
    for (unsigned i = 0; i < sizeof F / sizeof F[0]; i++) {
        re15_actor_t *e = mk_zombie(0, 1000);
        e->re2z_self1d3 = F[i].d3;
        e->re2z_f10e    = F[i].f10e;
        e->hp           = F[i].hp;
        e->hit_react    = 0x3;      /* selbst der Extremfall: beide Bits standen vorher */
        re15_re2z_hit_filter_apply(SLOT);
        printf("[A] %-46s -> +0x93=0x%02X  Marke(IST)=\"%s\"  Marke(VORSCHLAG)=\"%s\"\n",
               F[i].name, e->hit_react, marke_urteil(e), marke_urteil_alt(e));
        CHECK((e->hit_react & 1u) != 0, "[A] %s: Gate muss Bit 0 setzen", F[i].name);
        CHECK((e->hit_react & 2u) == 0, "[A] %s: Bit 1 muss geloescht sein", F[i].name);
        CHECK((e->hit_react & 3u) != 3u,
              "[A] %s: (hit_react&3)==3 darf NICHT stehen", F[i].name);
    }

    /* ---- (C): derselbe Schuss, gleiche Geometrie, einmal frei / einmal gesperrt ----------- */
    /* Erst eine Geometrie suchen, in der die KONTROLLE trifft — sonst misst (C) nichts. */
    int gw = -1; int32_t gx = 0, gz = 0; int16_t grot = 0;
    static const int wpn[] = { 3, 4, 5, 0, 21 };
    for (unsigned wi = 0; wi < sizeof wpn / sizeof wpn[0] && gw < 0; wi++) {
        for (int r = 0; r < 4096 && gw < 0; r += 1024) {
            for (int32_t d = 600; d <= 2400 && gw < 0; d += 600) {
                static const int32_t DIR[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
                for (int k = 0; k < 4 && gw < 0; k++) {
                    int32_t x = DIR[k][0] * d, z = DIR[k][1] * d;
                    mk_player((int16_t)r);
                    re15_actor_t *e = mk_zombie(x, z);
                    e->re2z_self1d3 = 0; e->re2z_f10e = 0; e->hit_react = 0;
                    re15_re2z_hit_filter_apply(SLOT);
                    int16_t hp0 = e->hp;
                    re15_player_weapon_fire(wpn[wi]);
                    if (e->hp < hp0) { gw = wpn[wi]; gx = x; gz = z; grot = (int16_t)r;
                        printf("[C] Geometrie gefunden: waffe=%d rot=%d pos=(%d,%d) "
                               "hp %d -> %d\n", gw, r, (int)x, (int)z, (int)hp0, (int)e->hp); }
                }
            }
        }
    }
    CHECK(gw >= 0, "[C] KONTROLLE: keine Geometrie gefunden, in der ein freier RE2-Zombie "
                   "Schaden nimmt - Sonde misst nichts");
    if (gw >= 0) {
        /* KONTROLLE (frei) */
        mk_player(grot);
        re15_actor_t *e = mk_zombie(gx, gz);
        e->re2z_self1d3 = 0; e->re2z_f10e = 0; e->hit_react = 0;
        re15_re2z_hit_filter_apply(SLOT);
        int16_t hp0 = e->hp;
        uint8_t hr_vor = e->hit_react;
        const char *u_ist = marke_urteil(e), *u_alt = marke_urteil_alt(e);
        re15_player_weapon_fire(gw);
        printf("[C-frei]     +0x93 vor Schuss=0x%02X Marke(IST)=\"%s\" Marke(VORSCHLAG)=\"%s\" "
               "hp %d -> %d\n", hr_vor, u_ist, u_alt, (int)hp0, (int)e->hp);
        CHECK(e->hp < hp0, "[C-frei] Kontrolle muss Schaden nehmen");

        /* GESPERRT ueber Gate (2) */
        mk_player(grot);
        e = mk_zombie(gx, gz);
        e->re2z_self1d3 = 0x80; e->re2z_f10e = 0; e->hit_react = 0;
        re15_re2z_hit_filter_apply(SLOT);
        hp0 = e->hp; hr_vor = e->hit_react;
        u_ist = marke_urteil(e); u_alt = marke_urteil_alt(e);
        re15_player_weapon_fire(gw);
        printf("[C-gesperrt] +0x93 vor Schuss=0x%02X Marke(IST)=\"%s\" Marke(VORSCHLAG)=\"%s\" "
               "hp %d -> %d\n", hr_vor, u_ist, u_alt, (int)hp0, (int)e->hp);
        CHECK(e->hp == hp0, "[C-gesperrt] der gesperrte Kandidat darf KEINEN Schaden nehmen "
                            "(hp %d -> %d)", (int)hp0, (int)e->hp);

        /* GESPERRT, aber nur Bit 0 von Hand (ohne Filter) - reiner Latch-Test des Aufloesers */
        mk_player(grot);
        e = mk_zombie(gx, gz);
        e->re2z_self1d3 = 0; e->re2z_f10e = 0; e->hit_react = 0x1;
        hp0 = e->hp; hr_vor = e->hit_react;
        u_ist = marke_urteil(e); u_alt = marke_urteil_alt(e);
        re15_player_weapon_fire(gw);
        printf("[C-bit0]     +0x93 vor Schuss=0x%02X Marke(IST)=\"%s\" Marke(VORSCHLAG)=\"%s\" "
               "hp %d -> %d  (+0x93 nach=0x%02X)\n",
               hr_vor, u_ist, u_alt, (int)hp0, (int)e->hp, e->hit_react);
        CHECK(e->hp == hp0, "[C-bit0] Bit 0 allein muss den Schaden verhindern (hp %d -> %d)",
              (int)hp0, (int)e->hp);

        /* Und der Hitbox-/Messerpfad (re15_enemy_take_damage @0x80012fb4-c0) */
        e = mk_zombie(gx, gz);
        e->hit_react = 0x1;
        hp0 = e->hp;
        int landed = re15_enemy_take_damage(e, 1);
        printf("[C-hitbox]   take_damage bei +0x93=0x01 -> landed=%d hp %d -> %d (+0x93 nach=0x%02X)\n",
               landed, (int)hp0, (int)e->hp, e->hit_react);
        CHECK(landed == 0 && e->hp == hp0, "[C-hitbox] Bit 0 allein blockt auch den Hitbox-Pfad");
    }

    printf(fails ? "PROBE-FAILS %d\n" : "PROBE-OK\n", fails);
    return fails ? 1 : 0;
}
