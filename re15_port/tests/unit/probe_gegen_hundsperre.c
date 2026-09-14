/* probe_gegen_hundsperre.c - GEGENPRUEFUNG des Befundes
 *   "Die Sperre nach einem Treffer dauert im Port 120 Bilder (4,00 s) unter RE2-KI"
 *
 * Unabhaengig von probe_hund_trefferluecke nachgebaut. Geprueft werden die
 * ZWISCHENSCHRITTE, die der Befund nur BEHAUPTET:
 *   (1) "die Waffe aendert im Port NICHTS"      -> Sweep ueber ALLE 22 Waffen-Ids.
 *   (2) "die Sperre dauert 120 Bilder"          -> Sweep ueber die HURT-Zeile
 *                                                  (re2z_prev_sub = ACTIVE-Substate).
 *   (3) Ist 120 ein Artefakt des Festnagelns?   -> derselbe Lauf OHNE Positions-Pin.
 *   (4) Kontrolle am RE2-Zombie (dort fuettert die Waffe die Zeile wirklich).
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

/* Setzt Spieler + RE2-Hund auf, tickt zweimal ACTIVE. Gibt den Gegner zurueck. */
static re15_actor_t *aufbau(uint8_t type, int32_t dist, int elev)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    bank_re2(type);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = dist; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->state = 0; e->x = 0; e->y = 0; e->z = 0;
    e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, type);
    face(e, pl); face(pl, e);
    for (int i = 0; i < 2; i++) {
        re15_enemy_ai_run_all(1);
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    }
    re15_player_set_aim_elevation_for_test(elev);
    e->hp = 1000;
    return e;
}

/* Ein Schuss, dann ticken bis +0x93 Bit0 faellt. Liefert die Sperrdauer in Bildern
 * (-1 = kein Treffer, 0 = faellt nicht binnen 600). pin = Position festnageln? */
static int sperre(uint8_t type, int weapon, int32_t dist, int pin, int forced_row,
                  int *row_out, int *dmg_out)
{
    re15_actor_t *e = aufbau(type, dist, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (forced_row >= 0) e->re2z_prev_sub = (uint8_t)forced_row;
    if (row_out) *row_out = (int)e->re2z_prev_sub;
    int16_t hp0 = e->hp;
    int ret = re15_player_weapon_fire(weapon);
    if (ret != 2 || e->hp == hp0) return -1;
    if (dmg_out) *dmg_out = (int)(hp0 - e->hp);
    for (int t = 1; t <= 600; t++) {
        uint8_t hr = e->hit_react;
        if (pin) { e->x = 0; e->z = 0; e->y = 0; }
        face(e, pl); face(pl, e);
        re15_enemy_ai_run_all(1);
        pl->hit_react = 0; pl->hp = 100;
        if ((hr & 1u) && !(e->hit_react & 1u)) return t;
    }
    return 0;
}

int main(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== GEGENPRUEFUNG Hunde-Sperre (RE2-KI, Typ 0x20) ===\n");

    /* (1) WAFFEN-SWEEP -------------------------------------------------------------- */
    printf("\n--- (1) Waffen-Sweep (Distanz 900, Position festgenagelt) ---\n");
    printf("    Waffe | Zeile(prev_sub) | Schaden | Sperre (Bilder) | s @30Hz\n");
    int gleich = 1, erste = -999;
    for (int w = 0; w < 22; w++) {
        int row = -1, dmg = -1;
        int s = sperre(0x20, w, 900, 1, -1, &row, &dmg);
        if (s < 0) { printf("      %2d  |        -        |    -    |   kein Treffer  |   -\n", w); continue; }
        if (erste == -999) erste = s; else if (s != erste) gleich = 0;
        printf("      %2d  |       %2d        |   %3d   |       %3d       |  %.2f\n",
               w, row, dmg, s, s / 30.0);
    }
    printf("    -> Sperrdauer ueber alle treffenden Waffen identisch? %s\n", gleich ? "JA" : "NEIN");

    /* (2) ZEILEN-SWEEP -------------------------------------------------------------- */
    printf("\n--- (2) HURT-Zeilen-Sweep (re2z_prev_sub erzwungen, Pistole w3, Distanz 2000) ---\n");
    printf("    Zeile | Sperre (Bilder) | s @30Hz\n");
    int n120 = 0, nrows = 0, mn = 99999, mx = -1;
    for (int r = 0; r <= 20; r++) {
        int row = -1, dmg = -1;
        int s = sperre(0x20, 3, 2000, 1, r, &row, &dmg);
        if (s < 0) { printf("      %2d  |   kein Treffer  |   -\n", r); continue; }
        nrows++;
        if (s == 120) n120++;
        if (s < mn) mn = s;
        if (s > mx) mx = s;
        printf("      %2d  |       %3d       |  %.2f\n", r, s, s / 30.0);
    }
    printf("    -> %d/%d Zeilen ergeben genau 120 Bilder; Spanne %d..%d Bilder (%.2f..%.2f s)\n",
           n120, nrows, mn, mx, mn / 30.0, mx / 30.0);

    /* (3) IST DAS FESTNAGELN SCHULD? ------------------------------------------------ */
    printf("\n--- (3) Derselbe Lauf OHNE Positions-Pin ---\n");
    for (int pin = 1; pin >= 0; pin--) {
        int row = -1, dmg = -1;
        int s = sperre(0x20, 3, 2000, pin, -1, &row, &dmg);
        printf("    Pin=%d -> Zeile %d, Schaden %d, Sperre %d Bilder (%.2f s)\n",
               pin, row, dmg, s, s / 30.0);
    }

    /* (4) KONTROLLE AM RE2-ZOMBIE --------------------------------------------------- */
    printf("\n--- (4) Kontrolle: Zombie 0x10 unter RE2-KI (dort fuettert die Waffe die Zeile) ---\n");
    for (int w = 3; w <= 6; w++) {
        int row = -1, dmg = -1;
        int s = sperre(0x10, w, 2000, 1, -1, &row, &dmg);
        printf("    Zombie, Waffe %d -> Schaden %d, Sperre %d Bilder\n", w, dmg, s);
    }

    printf("\n=== ENDE ===\n");
    return 0;
}
