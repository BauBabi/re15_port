/**
 * @file test_r38_szenenschutz_alle.c
 * @brief Runde 38 Spur B "szenenschutz" — Zensus ALLER Gegnertypen (Skill re15-room-probe, ohne exe).
 *
 * Nutzer (AUFTRAG.md 2026-10-16): "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal
 * bewegen kann ist mist. ... Es sei den es ist Teil der Cutscene wie bei Elza Room 1170." Dossier
 * analysis/befunde_runde38/B_szenenschutz.md Abschnitt 2.5/2.6.
 *
 * Je Gegnertyp aus dem Zensus "Szene haelt den Spieler x Gegner" (Dossier 2.5) und je Gehirn (RE2-Standard, RE1.5):
 * ein frischer Gegner (Zustand 0 = INIT) steht dem Spieler zugewandt gegenueber; zweimal 900 Bilder:
 *   gehalten  g_scd.player_mode = 2 (die Szene haelt den Spieler)  -> KEIN HP-Verlust, KEIN Griff (Opfer-FSM bleibt 0),
 *             und das Sicherheitsnetz musste NICHT eingreifen (abgefangen == 0: das Tor im Gehirn hielt den Angriff ab,
 *             nicht erst die HP-Rueckgabe);
 *   frei      g_scd.player_mode = 0 -> Kontrolle: derselbe Aufbau greift den Spieler an (sonst misst der Fall nichts).
 * Gegner ohne Angriff auf den Spieler (Umgebungs-Effekt 0x24, ...) und Faelle, deren freie Kontrolle im Aufbau ohne Raum
 * nicht angreift, stehen als "Kontrolle ohne Angriff" in der Tabelle; sie werden nicht als bestanden gezaehlt.
 */
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_szenenschutz_r38.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int16_t re15_atan2_q12(int32_t, int32_t);
extern int re15_player_victim_state(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

typedef struct {
    int     erster;      /* erstes Bild mit HP-Verlust oder Griff (-1 = keins) */
    int16_t hp;          /* Spieler-HP am Ende */
    int     griff;       /* Opfer-FSM jemals != 0 */
    uint32_t ticks, ab;  /* geschuetzte Ticks, abgefangene Abzuege */
} lauf_t;

static lauf_t lauf(uint8_t type, int32_t abstand, int32_t ey, int halten, int bilder)
{
    lauf_t r; memset(&r, 0, sizeof r); r.erster = -1;
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_szenenschutz_zaehler_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 1; pl->motion = 0; pl->floor = 0;
    re15_actor_t *e = &g_actors[5];
    memset(e, 0, sizeof(*e));
    e->active = 1; e->type = type; e->hp = 100; e->flags = 1; e->em_flag_id = 0xFF;
    e->state = 0; e->x = 0; e->y = ey; e->z = abstand;
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    re15_enemy_apply_hitbox(e, type);
    g_scd.player_mode = halten ? 2 : 0;
    for (int f = 0; f < bilder; f++) {
        pl->x = 0; pl->z = 0;                          /* Leon steht (Szene / still) */
        int16_t vor = pl->hp;
        frame();
        int vs = re15_player_victim_state();
        if (vs != 0) r.griff = 1;
        if (r.erster < 0 && (pl->hp < vor || vs != 0 || re15_player_is_grabbed())) r.erster = f;
    }
    r.hp = pl->hp;
    re15_szenenschutz_zaehler(&r.ticks, &r.ab);
    g_scd.player_mode = 0;
    return r;
}

typedef struct { const char *name; uint8_t type; int32_t abstand; int32_t ey; } fall_t;

int main(void)
{
    static const fall_t faelle[] = {
        { "Zombie 0x10",        0x10,  900,     0 },
        { "Zombie 0x11",        0x11,  900,     0 },
        { "Zombie 0x12",        0x12,  900,     0 },
        { "Zombie 0x16",        0x16,  900,     0 },
        { "Zombie 0x18",        0x18,  900,     0 },
        { "Zombie-Maedchen 0x13", 0x13, 900,    0 },
        { "Gitterarm 0x1A",     0x1A,  600,     0 },
        { "Hund 0x20",          0x20, 1500,     0 },
        { "Kraehe 0x21",        0x21, 3000, -1800 },
        { "Alligator 0x23",     0x23, 2500,     0 },
        { "Effekt 0x24",        0x24, 1500, -1000 },
        { "Spinne 0x25",        0x25, 1500,     0 },
        { "Feuer 0x26",         0x26,  300,     0 },
        { "Affe 0x27",          0x27, 1500,     0 },
        { "Kakerlake 0x29",     0x29,  900,     0 },
        { "Tyrant 0x2B",        0x2B, 1500,     0 },
        { "Efeu 0x2D",          0x2D,  900,     0 },
        { "Birkin 0x30",        0x30, 1500,     0 },
        { "Birkin 0x36",        0x36, 1500,     0 },
    };
    const int n = (int)(sizeof faelle / sizeof faelle[0]);
    const int BILDER = 900;
    printf("Typ                    Gehirn | gehalten: erster hp griff ab ticks | frei: erster hp griff\n");
    for (int fl = 0; fl < 2; fl++) {
        re15_ai_flavor_set(fl == 0 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        const char *gn = fl == 0 ? "RE2 " : "RE15";
        for (int i = 0; i < n; i++) {
            lauf_t g = lauf(faelle[i].type, faelle[i].abstand, faelle[i].ey, 1, BILDER);
            lauf_t f = lauf(faelle[i].type, faelle[i].abstand, faelle[i].ey, 0, BILDER);
            printf("%-22s %s | %5d %4d %d %3u %5u | %5d %4d %d\n", faelle[i].name, gn,
                   g.erster, g.hp, g.griff, g.ab, g.ticks, f.erster, f.hp, f.griff);
        }
    }
    printf("%s (%d Fehler)\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
