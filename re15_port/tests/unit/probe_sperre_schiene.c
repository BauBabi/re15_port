/* probe_sperre_schiene.c — VERIFIKATION des Befundes "Messschiene RE15_SPERRE_LOG".
 *
 * Diese Sonde prueft NICHT die Engine, sondern den VORGESCHLAGENEN Schienen-Code:
 *   (1) uebersetzt der vorgeschlagene Block ueberhaupt gegen die echten Header?
 *   (2) trifft die Bedingung `sperr = lebt && (hit_react & 1)` den Zustand, den
 *       re15_damage.c:1621 tatsaechlich abweist?
 *   (3) stimmt die GRUND-Deutung (1D3 / 10E&C000 / haengt)?
 *   (4) ZUSATZ-Behauptung: meldet `(hit_react & 0x3) == 0x3` (main.c:5150) einen
 *       gesperrten RE2-Zombie faelschlich als treffbar?
 *
 * Der echte Filter ist re15_re2z_hit_filter_apply (enemy_ai_re2_zombie.c:8184),
 * die vier Gates spiegeln FUN_800470C0 @0x80047124-30 / 38-40 / 48-50 / 58-64
 * (selbst disassembliert aus info/re2leon/PSX.EXE, t_addr 0x80010000).
 * NUR LESEN — keine Engine-Datei wird veraendert. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_engine.h"

extern void re15_re2z_hit_filter_apply(int slot);
extern int  re15_re2z_owns_type(unsigned type);
extern int  re15_ai_re2_for_type(unsigned type);

static int fails = 0;
#define CHECK(c, msg, ...) do { if (!(c)) { printf("  FAIL: " msg "\n", ##__VA_ARGS__); fails++; } \
                                else       { printf("  ok  : " msg "\n", ##__VA_ARGS__); } } while (0)

/* --- der vorgeschlagene Block, GRUND-Teil woertlich uebernommen ------------------ */
static const char *grund_des_befundes(const re15_actor_t *e)
{
    return e->re2z_self1d3            ? "1D3"
         : (e->re2z_f10e & 0xC000u)   ? "10E&C000"
                                      : "haengt (kein Gate)";
}

/* Der Port-Filter, wie enemy_ai_re2_zombie.c:8211-8214 ihn rechnet — zur Gegenprobe
 * der GRUND-Deutung (inkl. der spawn_pose-Ausnahme, die der Befund NICHT erwaehnt). */
static int port_hittable(const re15_actor_t *e)
{
    int spawn_pose = (e->state == 1) && (e->sub_state_1 == 7 || e->sub_state_1 == 8);
    return (e->active != 0)
        && (spawn_pose || e->re2z_self1d3 == 0u)
        && (e->hp >= 0)
        && (spawn_pose || !(e->re2z_f10e & 0xC000u));
}

static void reset_slot(int s)
{
    memset(&g_actors[s], 0, sizeof(g_actors[s]));
    g_actors[s].active = 1;
    g_actors[s].type   = 0x10;   /* RE2-eigener Zombie (re15_re2z_owns_type) */
    g_actors[s].hp     = 50;
    g_actors[s].state  = 2;      /* KEIN spawn_pose */
    g_actors[s].sub_state_1 = 1;
}

int main(void)
{
    const int S = 3;
    printf("== probe_sperre_schiene ==\n");
    printf("owns_type(0x10)=%d  re2_for_type(0x10)=%d\n",
           re15_re2z_owns_type(0x10), re15_ai_re2_for_type(0x10));

    /* --- Fall A: 1D3-Latch haengt -> Gate (2) ------------------------------------ */
    printf("\n[A] 1D3-Latch gesetzt, hp>=0, kein spawn_pose\n");
    reset_slot(S);
    g_actors[S].re2z_self1d3 = 0x80;
    re15_re2z_hit_filter_apply(S);
    {
        const re15_actor_t *e = &g_actors[S];
        int lebt  = e->active && e->type != 0 && e->hp >= 0;
        int sperr = lebt && (e->hit_react & 1u);
        printf("      hit_react=%02X 1D3=%02X 10E=%04X hittable=%d\n",
               e->hit_react, e->re2z_self1d3, e->re2z_f10e, port_hittable(e));
        CHECK(sperr, "[A] Schiene erkennt die Sperre (sperr=1)");
        CHECK(strcmp(grund_des_befundes(e), "1D3") == 0,
              "[A] GRUND == \"1D3\" (ist: \"%s\")", grund_des_befundes(e));
        /* ZUSATZ-Behauptung gegen main.c:5150 */
        CHECK((e->hit_react & 0x3) != 0x3,
              "[A] ZUSATZ: (hit_react&3)==3 ist FALSCH -> F9-Marke meldet faelschlich treffbar "
              "(hit_react=%02X)", e->hit_react);
    }

    /* --- Fall B: 10E & 0xC000 -> Gate (4) ---------------------------------------- */
    printf("\n[B] 10E-Bit 0x8000 gesetzt, 1D3 sauber\n");
    reset_slot(S);
    g_actors[S].re2z_f10e = 0x8000;
    re15_re2z_hit_filter_apply(S);
    {
        const re15_actor_t *e = &g_actors[S];
        int sperr = (e->active && e->type && e->hp >= 0) && (e->hit_react & 1u);
        printf("      hit_react=%02X 1D3=%02X 10E=%04X hittable=%d\n",
               e->hit_react, e->re2z_self1d3, e->re2z_f10e, port_hittable(e));
        CHECK(sperr, "[B] Schiene erkennt die Sperre");
        CHECK(strcmp(grund_des_befundes(e), "10E&C000") == 0,
              "[B] GRUND == \"10E&C000\" (ist: \"%s\")", grund_des_befundes(e));
    }

    /* --- Fall C: sauber -> keine Sperre ------------------------------------------- */
    printf("\n[C] alle Gates frei\n");
    reset_slot(S);
    re15_re2z_hit_filter_apply(S);
    {
        const re15_actor_t *e = &g_actors[S];
        int sperr = (e->active && e->type && e->hp >= 0) && (e->hit_react & 1u);
        printf("      hit_react=%02X hittable=%d\n", e->hit_react, port_hittable(e));
        CHECK(!sperr, "[C] keine Sperre gemeldet");
    }

    /* --- Fall D: 1D3 gesetzt, aber SPAWN-POSE (state1/s1=7) ----------------------- *
     * Der Filter umgeht Gate (2)+(4) in der Pose. Traegt die GRUND-Deutung das?      */
    printf("\n[D] 1D3 gesetzt, aber spawn_pose (st=1 s1=7) -> Filter umgeht Gate 2/4\n");
    reset_slot(S);
    g_actors[S].state = 1; g_actors[S].sub_state_1 = 7;
    g_actors[S].re2z_self1d3 = 0x80;
    g_actors[S].re2z_f10e    = 0x8000;
    re15_re2z_hit_filter_apply(S);
    {
        const re15_actor_t *e = &g_actors[S];
        int sperr = (e->active && e->type && e->hp >= 0) && (e->hit_react & 1u);
        printf("      hit_react=%02X 1D3=%02X 10E=%04X hittable=%d GRUND-waere=\"%s\"\n",
               e->hit_react, e->re2z_self1d3, e->re2z_f10e, port_hittable(e),
               grund_des_befundes(e));
        CHECK(!sperr, "[D] keine Sperre gemeldet (Pose ist treffbar) -> GRUND nie ausgegeben");
    }

    /* --- Fall E: Bit 2 ueberlebt keinen Tick -------------------------------------- *
     * Beweis fuer die ZUSATZ-Behauptung: der Filter loescht Bit1 JEDEN Tick
     * (enemy_ai_re2_zombie.c:8217), also kann (hit_react&3)==3 nie beobachtet werden. */
    printf("\n[E] Bit 0x2 kuenstlich gesetzt, dann einen Tick filtern\n");
    reset_slot(S);
    g_actors[S].re2z_self1d3 = 0x80;
    g_actors[S].hit_react = 0x3;                 /* wie direkt nach re15_damage.c:1622 */
    re15_re2z_hit_filter_apply(S);
    {
        const re15_actor_t *e = &g_actors[S];
        printf("      hit_react nach einem Tick = %02X\n", e->hit_react);
        CHECK((e->hit_react & 2u) == 0, "[E] Bit 0x2 ist nach dem Tick geloescht");
        CHECK((e->hit_react & 1u) != 0, "[E] Bit 0x1 (die Sperre) steht weiter");
        CHECK((e->hit_react & 0x3) != 0x3,
              "[E] ZUSATZ bestaetigt: (hit_react&3)==3 unbeobachtbar -> main.c:5150 luegt");
    }

    /* --- Fall F: Schwellen-Arithmetik des Befundes -------------------------------- */
    printf("\n[F] Sekunden-Umrechnung des Befundes (d/30, (d%%30)*10/30)\n");
    {
        unsigned d = 90;
        printf("      d=%u -> %u,%u s\n", d, d / 30u, (d % 30u) * 10u / 30u);
        CHECK(d / 30u == 3u, "[F] 90 Bilder == 3 s bei target_fps=30 (main.c:2911 Default)");
        d = 45;
        printf("      d=%u -> %u,%u s\n", d, d / 30u, (d % 30u) * 10u / 30u);
        CHECK(d / 30u == 1u && (d % 30u) * 10u / 30u == 5u, "[F] 45 Bilder == 1,5 s");
    }

    printf("\n== %s (%d Fehler) ==\n", fails ? "FEHLER" : "ALLE PRUEFUNGEN OK", fails);
    return fails ? 1 : 0;
}
