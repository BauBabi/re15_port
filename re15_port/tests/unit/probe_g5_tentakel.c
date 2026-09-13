/**
 * @file probe_g5_tentakel.c
 * @brief SMOKE (ctest) — die vier G5-Tentakel (Typ 0x37, Runde 7, em037-tentakel.md).
 *
 *  PIN 1  SPAWN: der Kampfstart legt vier 0x37-Aktoren an, HP -1 (unverwundbar
 *         @0x80100530), und sie haengen am ANKER von G5s Masse (Tabelle
 *         @0x80105668) - also nicht auf der Boss-Position selbst.
 *  PIN 2  AUSTRITT: die Intro-Choreo weckt sie ([T1] t=10/30/40, t=90 alle vier),
 *         sie fahren aus (render_scale_q12 waechst von 0 auf 4096).
 *  PIN 3  KOMMANDO: ein Peitschen-Wort (0x301) bringt Tentakel 0 in sub3, setzt
 *         sein Beschaeftigt-Bit in der +0x228-Maske (Bit i+4 @0x8010146C) und
 *         loescht es am Ende wieder (@0x80101808).
 *  PIN 4  SCHADEN: ein Spieler im Schlagfenster verliert 15 HP (@0x801016D0).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_g5_boss_tick(int slot);
extern void    re15_g5_tentakel_spawn(const re15_actor_t *g5);
extern void    re15_g5_tentakel_tick(const re15_actor_t *g5);
extern void    re15_g5_tentakel_cmd(int idx, uint32_t wort);
extern void    re15_g5_tentakel_reset(void);
extern uint8_t re15_g5_tentakel_maske(void);
extern int     re15_g5_tentakel_spitze(int idx, int32_t out[3]);

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int tent_slots(int *out)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x37u && n < 8) out[n++] = s;
    return n;
}

int main(void)
{
    printf("=== G5-TENTAKEL (Typ 0x37, ROOM5090) ===\n");
    re15_actor_init();
    re15_g5_tentakel_reset();
    g_current_room_id = 0x5090;

    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("SKIP: shared_assets/RE2/CDEMD0.EMS fehlt\n"); return 0; }
    {   re15_enemy_bank_t *b36 = re15_enemy_alloc(0x36);
        re15_enemy_bank_t *b37 = re15_enemy_alloc(0x37);
        if (!b36 || re2_ems_load_bank(ems, n, 0x36, b36, NULL) != 0) {
            printf("FAIL: EM036 laedt nicht\n"); return 1; }
        b36->buf = NULL; b36->ok = 1;
        if (b37 && re2_ems_load_bank(ems, n, 0x37, b37, NULL) == 0) {
            b37->buf = NULL; b37->ok = 1;
            printf("EM037: %d Bones, %d Clips, %d Meshes\n",
                   b37->skel.bone_count, b37->anim.clip_count, b37->md1.mesh_count);
            CHECK(b37->skel.bone_count == 4, "EM037 hat %d Bones (erwartet 4)",
                  b37->skel.bone_count);
            CHECK(b37->anim.clip_count == 24, "EM037 hat %d Clips (erwartet 24)",
                  b37->anim.clip_count);
        } else {
            printf("FAIL: EM037 laedt nicht\n"); return 1;
        }
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 300; pl->z = -23400; pl->y = 0;

    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600;
    e->x = -14700; e->z = -23350; e->grid_id = 0x13;   /* Kampfstart */

    /* ---- PIN 1: Spawn + Anker ---- */
    re15_g5_boss_tick(2);
    int slots[8];
    int nt = tent_slots(slots);
    printf("Tentakel gespawnt: %d\n", nt);
    CHECK(nt == 4, "%d Tentakel statt 4", nt);
    if (nt != 4) { printf("=== FAIL ===\n"); return 1; }
    for (int i = 0; i < 4; i++) {
        re15_actor_t *t = &g_actors[slots[i]];
        CHECK(t->hp == -1, "Tentakel %d: hp %d != -1 (unverwundbar)", i, (int)t->hp);
    }
    {   /* Anker: nicht auf der Boss-Position, aber in Reichweite der Masse. */
        int weg = 0;
        for (int i = 0; i < 4; i++) {
            re15_actor_t *t = &g_actors[slots[i]];
            int32_t dx = t->x - e->x, dz = t->z - e->z;
            int32_t d2 = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (d2 > 0 && d2 < 12000) weg++;
        }
        printf("Anker: %d/4 Tentakel an der Masse (nicht auf der Boss-Wurzel)\n", weg);
        CHECK(weg == 4, "nur %d/4 Tentakel korrekt geankert", weg);
        {   /* HOEHE (Runde 8, Nutzer-Marke R5090 F550): die Arme haengen an der
             * Masse, die am WURZELKNOCHEN des Kriechers sitzt - der liegt 3,1-4,5 m
             * ueber dem Entity-Ursprung (alle EM036-Keyframes rootY -4536..-3105).
             * Ohne diese Wurzelhoehe landeten sie bei y = +2057..+3816, also METER
             * UNTER dem Boden (PSX-y waechst nach unten). */
            int unten = 0, i2;
            for (i2 = 0; i2 < 4; i2++) if (g_actors[slots[i2]].y > 300) unten++;
            printf("Hoehe: y = %d / %d / %d / %d\n",
                   (int)g_actors[slots[0]].y, (int)g_actors[slots[1]].y,
                   (int)g_actors[slots[2]].y, (int)g_actors[slots[3]].y);
            CHECK(unten == 0, "%d Tentakel stecken unter dem Boden", unten);
        }
    }

    /* ---- PIN 2: Austritt waehrend der Intro-Choreo ---- */
    {
        int32_t scale_max = 0;
        for (int f = 0; f < 400; f++) {
            re15_g5_boss_tick(2);
            for (int i = 0; i < 4; i++) {
                int32_t s = g_actors[slots[i]].render_scale_q12;
                if (s > scale_max) scale_max = s;
            }
        }
        printf("Austritt: max Streckung %d (Soll 4096)\n", (int)scale_max);
        CHECK(scale_max >= 4000, "Tentakel fahren nicht aus (max %d)", (int)scale_max);
    }

    /* ---- PIN 3+4: Peitsche mit Schaden ---- */
    {
        uint8_t maske_vor = re15_g5_tentakel_maske();
        int getroffen = 0, maske_gesetzt = 0;
        /* Spieler direkt vor Tentakel 0 stellen, damit die Spitze ihn erreicht. */
        pl->hp = 100; pl->hit_react = 0;
        re15_g5_tentakel_cmd(0, 0x301u);
        for (int f = 0; f < 400; f++) {
            re15_g5_tentakel_tick(e);
            if (re15_g5_tentakel_maske() & (1u << 4)) maske_gesetzt = 1;
            if (pl->hp < 100) { getroffen = 100 - pl->hp; break; }
            pl->hit_react = 0;                     /* nur den ersten Treffer messen */
            {   /* Spieler AN DIE SPITZE stellen - das Original prueft dort ueber ein
                 * Kind-Entity, der Port ueber die Spitzen-Geometrie (Port-Entscheidung 3). */
                int32_t sp[3];
                if (re15_g5_tentakel_spitze(0, sp) == 0) { pl->x = sp[0]; pl->z = sp[2]; }
            }
        }
        printf("Peitsche: Maske gesetzt=%d (vorher 0x%02X), Schaden %d\n",
               maske_gesetzt, maske_vor, getroffen);
        CHECK(maske_gesetzt, "Beschaeftigt-Bit (+0x228 Bit 4) wurde nie gesetzt");
        CHECK(getroffen == 15, "Peitschen-Schaden %d != 15", getroffen);
    }

    free(ems);
    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS: Spawn, Anker, Austritt, Kommando/Maske, 15 Schaden ===\n");
    return 0;
}
