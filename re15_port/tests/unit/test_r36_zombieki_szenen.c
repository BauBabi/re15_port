/*
 * Runde 36 Spur F "zombieki", Punkt 1 — SZENEN nach einer Interaktion (Griff/Abwehr/Schuss/Sturz):
 * kommt der Zombie danach wieder auf Leon zu und greift an, wenn Leon still vor ihm steht?
 * Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 1.
 *
 * Echte Spielschleife wie test_r36_zombieki_wecker.c. ROOM1010 Cut 0, Zombie Slot 2 (Typ 0x10, Satz
 * @0x009F6 -> Port-Lage (205,7248) rot 0 = Blick +X), Leon 800 vor der Nase (1005,7248), Blick auf den
 * Zombie. Der Rueckstoss des Griffs (EXEC[3] P8 `re2z_thrust -30` @0x80102CA0-AC, Sturz 0x501
 * @0x80102D24-2C) wirft den Zombie nach -X in die freie Nordhaelfte (Zone 1, keine Zelle bis x -2400).
 *
 *   griff    keine Eingabe: Griff, Bisse, Freigabe, Sturz, Aufstehen, erneuter Angriff?
 *   wehr     im Griff jede 2. Bild eine Tastenflanke (Abwehr), danach keine Eingabe
 *   schussN  Handfeuerwaffe (Waffe 3), N Schuesse ab Bild 10 im 30-Bild-Takt, danach keine Eingabe
 *
 * Ausgabe: Zustands-Zeitlinie des Zombies + Ergebnis "erneuter Angriff in Bild X" bzw. "KEIN Angriff".
 * Ohne Argument: Riegel ueber alle Szenen; mit Argument: nur diese Szene mit Zeitlinie.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re2_ems.h"
#include "test_r36_zombieki_kopf.h"   /* NB2 Mangel 5: Leons Kopf aus PL00 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static void load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    kopf_setzen(&g_actors[RE15_ACTOR_SLOT_PLAYER]);   /* NB2 Mangel 5: head_world wie der Renderer (PL00-Daten) */
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static uint8_t *s_raw; static size_t s_n;
static int eintreten(uint16_t raum, int cut)
{
    if (re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    re15_inv_load_briefing();
    g_current_room_id = raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    s_shown = cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    g_scd.cut_auto_enabled = 1;
    load_bank_re2(0x10); load_bank_re2(0x11); load_bank_re2(0x16);
    return 1;
}

static int32_t isqrt64(long long v) { long long r = 0; while ((r + 1) * (r + 1) <= v) r++; return (int32_t)r; }

typedef struct {
    const char *name;
    int wehr;              /* Tastenflanken im Griff */
    int schuesse;          /* Handfeuerwaffe: Anzahl Schuesse ab Bild 10, Takt 30 */
} szene_t;

typedef struct {
    int griff1, frei1;     /* erster Griff / Freigabe */
    int angriff2;          /* erneuter Angriff (Griff/Sprung/Biss/Schnappbiss) nach der Interaktion, 0 = keiner */
    int sub2;
    int32_t dist_ende;     /* Abstand am Ende */
    int st_ende, s1_ende, s2_ende, los_ende; uint16_t f21a_ende; uint8_t d1d3_ende; int hp_ende;
    int32_t weg;           /* Summe |dx|+|dz| des Zombies nach der Interaktion */
} ergebnis_t;

static void szene(const szene_t *sz, int zeitlinie, ergebnis_t *r)
{
    memset(r, 0, sizeof *r);
    eintreten(0x1010, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[2];
    if (!e->active || e->type != 0x10) { printf("Slot 2 fehlt\n"); return; }
    pl->x = e->x + (sz->schuesse ? 2400 : 800); pl->z = e->z; pl->rot_y = 2048;   /* Schuss-Szenen: ausser Griffweite anfangen */
    g_actors[1].active = 0;                      /* nur Slot 2 im Raum: Slot 1 (3750,3500) stoert die Szene sonst */
    if (sz->schuesse) {
        re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        re15_player_set_equipped_weapon(3);
        int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250;
    }
    int prev_st = -1, prev_s1 = -1, prev_s2 = -1, prev_gr = -1;
    int32_t lx = e->x, lz = e->z;
    int interaktion_ende = 0;      /* Bild, ab dem Leon nur noch steht */
    int schuss_n = 0; int ziel_frei = 0;
    for (int f = 1; f <= 1500; f++) {
        uint16_t held = 0, edge = 0;
        int gr = re15_player_is_grabbed();
        if (sz->schuesse && schuss_n < sz->schuesse) {
            /* R1 halten bis zielbereit, dann SQUARE-Flanke alle 30 Bilder ab Bild 10 */
            held = RE15_PAD_BIT_R1;
            if (re15_player_aim_ready()) ziel_frei = 1;
            if (ziel_frei && f >= 14 && ((f - 14) % 30) == 0) { held |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; schuss_n++;
                if (schuss_n == sz->schuesse) interaktion_ende = f; }
        } else if (sz->wehr && gr) {
            if (f & 1) { held = RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
            else       { held = RE15_PAD_BIT_CROSS;  edge = RE15_PAD_BIT_CROSS; }
        }
        frame(held, edge);
        gr = re15_player_is_grabbed();
        if (gr && !r->griff1) r->griff1 = f;
        if (r->griff1 && !gr && !r->frei1) { r->frei1 = f; if (!sz->schuesse) interaktion_ende = f; }
        r->weg += (interaktion_ende && f > interaktion_ende) ? (labs(e->x - lx) + labs(e->z - lz)) : 0;
        lx = e->x; lz = e->z;
        long long dx = e->x - pl->x, dz = e->z - pl->z; int32_t d = isqrt64(dx * dx + dz * dz);
        if (interaktion_ende && f > interaktion_ende + 1 && !r->angriff2 && e->state == 1 &&
            (e->sub_state_1 == 3 || e->sub_state_1 == 12 || e->sub_state_1 == 14 || e->sub_state_1 == 10 || gr)) {
            r->angriff2 = f; r->sub2 = e->sub_state_1;
        }
        if (zeitlinie && (e->state != prev_st || e->sub_state_1 != prev_s1 || e->sub_state_2 != prev_s2 || gr != prev_gr)) {
            printf("F%4d st=%d/%2d/%2d clip=%2d grid=%02x @(%6d,%6d) rot=%4d dist=%5d los=%u f21a=%04x 1d3=%02x hp=%d | PL(%d,%d) hp=%d gr=%d%s\n",
                   f, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion, e->grid_id, (int)e->x, (int)e->z,
                   (int)e->rot_y, (int)d, e->re2z_los154, e->re2z_flags21a, e->re2z_self1d3, (int)e->hp,
                   (int)pl->x, (int)pl->z, (int)pl->hp, gr, (held || edge) ? " [pad]" : "");
            prev_st = e->state; prev_s1 = e->sub_state_1; prev_s2 = e->sub_state_2; prev_gr = gr;
        }
        if (r->angriff2 && f > r->angriff2 + 5) break;
        if (pl->hp < 0 || e->hp < 0) break;
    }
    long long dx = e->x - pl->x, dz = e->z - pl->z;
    r->dist_ende = isqrt64(dx * dx + dz * dz);
    r->st_ende = e->state; r->s1_ende = e->sub_state_1; r->s2_ende = e->sub_state_2; r->los_ende = e->re2z_los154;
    r->f21a_ende = e->re2z_flags21a; r->d1d3_ende = e->re2z_self1d3; r->hp_ende = e->hp;
    printf("%-8s Griff1 F%d Frei F%d | Interaktion Ende F%d | erneuter Angriff %s%d (sub %d) | Ende st=%d/%d/%d dist=%d los=%d f21a=%04x 1d3=%02x hp=%d weg=%ld\n",
           sz->name, r->griff1, r->frei1, interaktion_ende, r->angriff2 ? "F" : "KEINER ", r->angriff2, r->sub2,
           r->st_ende, r->s1_ende, r->s2_ende, (int)r->dist_ende, r->los_ende, r->f21a_ende, r->d1d3_ende, r->hp_ende, (long)r->weg);
}

static const szene_t k_szenen[] = {
    { "griff",   0, 0 },
    { "wehr",    1, 0 },
    { "schuss1", 0, 1 },
    { "schuss3", 0, 3 },
    { "schuss6", 0, 6 },
};
#define N_SZENEN ((int)(sizeof k_szenen / sizeof k_szenen[0]))

int main(int argc, char **argv)
{
    s_raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &s_n);
    if (!s_raw) { printf("FAIL: ROOM1010.RDT nicht lesbar\n"); return 1; }
    if (argc > 1) {
        for (int i = 0; i < N_SZENEN; i++)
            if (strcmp(argv[1], k_szenen[i].name) == 0) { ergebnis_t r; szene(&k_szenen[i], 1, &r); return 0; }
        printf("unbekannte Szene %s\n", argv[1]); return 1;
    }
    ergebnis_t r[N_SZENEN];
    for (int i = 0; i < N_SZENEN; i++) szene(&k_szenen[i], 0, &r[i]);
    for (int i = 0; i < N_SZENEN; i++)
        CHECK(r[i].angriff2 != 0 || r[i].hp_ende < 0, "%s: nach der Interaktion erneuter Angriff in Bild %d (sub %d)",
              k_szenen[i].name, r[i].angriff2, r[i].sub2);
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
