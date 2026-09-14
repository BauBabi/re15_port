/* probe_gegen_hundsperre2.c - GEGENPRUEFUNG II des Befundes
 *   "Die Sperre nach einem Treffer dauert im Port 120 Bilder (4,00 s) unter RE2-KI"
 *
 * Gegenpruefung I hat gezeigt: 120 Bilder gilt NUR fuer HURT-Zeile 0; die Zeilen 1..20
 * ergeben 72 (bzw. 75) Bilder. Die Zeile ist re2z_prev_sub, und die wird am KOPF des
 * Hunde-Ticks geschnappt (enemy_ai_re2_dog.c:2181, nur wenn state==1).
 *
 * Diese Sonde prueft die daraus folgende Vermutung: Im DAUERFEUER (Schuss in JEDEM Bild)
 * bekommt der Hund nach dem Ende der Sperre NIE einen freien ACTIVE-Tick - der Schuss
 * faellt vor dem naechsten Tick-Kopf. Deshalb bleibt prev_sub auf 0 kleben und JEDER
 * Treffer laeuft ueber Zeile 0 = 120 Bilder. Bei realistischer Schussfolge (>=1 freier
 * Tick zwischen den Schuessen) muesste die Sperre auf 72 Bilder fallen.
 *
 * Gemessen wird: Sperrdauer des ZWEITEN Treffers als Funktion der Anzahl freier
 * ACTIVE-Ticks zwischen Lock-Ende und naechstem Schuss.
 *
 * KEIN add_test (reine Messsonde).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_elevation_for_test(int elev);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;

static void bank_re2(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    memset(eb, 0, sizeof *eb); eb->type = type;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return;
    }
    eb->type = 0; eb->ok = 0;
}

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

static re15_actor_t *g_e, *g_pl;

static void aufbau(int32_t dist)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    bank_re2(0x20);
    g_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(g_pl, 0, sizeof *g_pl);
    g_pl->active = 1; g_pl->type = 0; g_pl->z = dist; g_pl->hp = 100;
    g_e = &g_actors[1];
    memset(g_e, 0, sizeof *g_e);
    g_e->active = 1; g_e->type = 0x20; g_e->state = 0; g_e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(g_e, 0x20);
    face(g_e, g_pl); face(g_pl, g_e);
    for (int i = 0; i < 2; i++) {
        re15_enemy_ai_run_all(1);
        g_e->x = 0; g_e->z = 0; g_e->y = 0; face(g_e, g_pl); face(g_pl, g_e);
    }
    re15_player_set_aim_elevation_for_test(0);
    g_e->hp = 1000;
}

static void tick1(void)
{
    g_e->x = 0; g_e->z = 0; g_e->y = 0;
    face(g_e, g_pl); face(g_pl, g_e);
    re15_enemy_ai_run_all(1);
    g_pl->hit_react = 0; g_pl->hp = 100;
}

/* Schiesst bis es trifft; liefert die Sperrdauer und die benutzte Zeile. */
static int schuss_und_sperre(int weapon, int *row_used, int *sub_at_fire)
{
    if (row_used)    *row_used    = (int)g_e->re2z_prev_sub;
    if (sub_at_fire) *sub_at_fire = (int)g_e->sub_state_1;
    int16_t hp0 = g_e->hp;
    if (re15_player_weapon_fire(weapon) != 2 || g_e->hp == hp0) return -1;
    for (int t = 1; t <= 600; t++) {
        uint8_t hr = g_e->hit_react;
        tick1();
        if ((hr & 1u) && !(g_e->hit_react & 1u)) return t;
    }
    return 0;
}

int main(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== GEGENPRUEFUNG II: haengt die 120 an der Dauerfeuer-Kadenz? ===\n");
    printf("    Hund 0x20, RE2-KI, Pistole (w3), Distanz 2000, Elevation LEVEL.\n");
    printf("    Zeile = re2z_prev_sub im Schuss-Moment (Schnappschuss am Tick-Kopf,\n");
    printf("    enemy_ai_re2_dog.c:2181, nur bei state==1).\n\n");

    printf("  freie ACTIVE-Ticks | Zeile 1.Tr | Sperre 1.Tr | Zeile 2.Tr | sub@2.Tr | Sperre 2.Tr | s @30Hz\n");
    for (int warte = 0; warte <= 6; warte++) {
        aufbau(2000);
        int r1 = -1, s1v = -1;
        int lock1 = schuss_und_sperre(3, &r1, &s1v);
        if (lock1 <= 0) { printf("   %2d  -> erster Treffer misslang (%d)\n", warte, lock1); continue; }
        /* Sperre ist gerade gefallen. Jetzt N freie ACTIVE-Ticks verstreichen lassen. */
        for (int k = 0; k < warte; k++) tick1();
        int r2 = -1, s2v = -1;
        int lock2 = schuss_und_sperre(3, &r2, &s2v);
        printf("         %2d        |     %2d     |     %3d     |     %2d     |    %2d    |     %3d     |  %.2f\n",
               warte, r1, lock1, r2, s2v, lock2, lock2 / 30.0);
    }

    printf("\n--- Kontrolle: Dauerfeuer-Kette (Schuss in JEDEM Bild), Zeile je Treffer ---\n");
    aufbau(2000);
    int frame = 0, letzter = -1;
    for (int t = 0; t < 400; t++, frame++) {
        g_e->x = 0; g_e->z = 0; g_e->y = 0;
        face(g_e, g_pl); face(g_pl, g_e);
        int row = (int)g_e->re2z_prev_sub;
        int sub = (int)g_e->sub_state_1;
        int16_t hp0 = g_e->hp;
        int ret = re15_player_weapon_fire(3);
        if (ret == 2 && g_e->hp != hp0) {
            printf("    Bild %3d TREFFER: Zeile(prev_sub)=%d  sub_state_1=%d  Luecke=%s%d\n",
                   t, row, sub, letzter < 0 ? "-" : "", letzter < 0 ? 0 : t - letzter);
            letzter = t;
        }
        re15_enemy_ai_run_all(1);
        g_pl->hit_react = 0; g_pl->hp = 100;
    }

    printf("\n=== ENDE ===\n");
    return 0;
}
