/*
 * Runde 36 Spur F "zombieki", Punkt 1 — "Ansonsten Verhalten sich die Zombies anders als in Resident
 * Evil 2 - sie bleiben hier ewig Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen
 * bin, fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind." (AUFTRAG.md Z. 59)
 * Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 1.
 *
 * WECK-ZENSUS mit der echten Spielschleife (scd_room_reenter + scd_vm_tick + re15_game_step, RE2-KI =
 * Auslieferungs-Vorgabe, RE2-Baenke aus shared_assets/RE2/CDEMD0.EMS): jeder Leon-Raum der Stage 1 mit
 * Zombie-Spawns, jeder Eintritts-Cut (Switch work_vars[0x0A] in sub00), jeder STEHENDE Zombie (Deskriptor-
 * Nibble nicht 5..8 = kein Fresser/Liegender, die wecken im Original NUR per Skript):
 *   Leon wird 800 (sonst 1200/1600) VOR DIE NASE gestellt (Blickrichtung des Zombies = (cos, -sin) wie
 *   FUN_800152C8/re15_re2z_move_root; freie Grundflaeche re15_collision_box_blocked r 450 Maske 1), Blick
 *   auf den Zombie, KEINE Eingabe, 480 Bilder = mehr als der laengste RE2-Selbstwecker (+0x158 =
 *   rand+150 <= 405 @0x80101478-88, Wecker @0x80101544-7C).
 * RE2-Regel (DECISION[0] @0x80101294): dist < 0x1388 && Spieler im +-1024-Kegel && Sicht -> 0x101 SOFORT
 * (@0x80101308-1C). Ein Zombie, der mit Sicht und Leon im Kegel in 480 Bildern nicht reagiert, ist der
 * Befund des Nutzers.
 *
 * Ohne Argument: Riegel. "mess": nur Tabelle. "mess <raum-hex>": nur dieser Raum.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

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

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static int zombie_typ(unsigned t) { return t == 0x10 || t == 0x11 || t == 0x12 || t == 0x16 || t == 0x18; }

/* Raum laden und sub00 mit Eintritts-Cut fahren (Spieler vorerst am Raumursprung). */
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
    load_bank_re2(0x10); load_bank_re2(0x11); load_bank_re2(0x12); load_bank_re2(0x16); load_bank_re2(0x18);
    return 1;
}

static int32_t isqrt64(long long v) { long long r = 0; while ((r + 1) * (r + 1) <= v) r++; return (int32_t)r; }

/* Welche Zelle blockt die Sicht Zombie -> Spieler (Zell-Strahl von re15_re2_los_cells_blocked, Slab-Test)? */
static int sicht_blocker(const re15_actor_t *e, const re15_actor_t *pl, char *out, size_t n)
{
    out[0] = 0;
    int band = re15_collision_band_from_y(e->y);
    unsigned mask = e->sca_mask ? e->sca_mask : 4u;
    const double p0[2] = { (double)e->x, (double)e->z };
    const double d[2]  = { (double)pl->x - (double)e->x, (double)pl->z - (double)e->z };
    int k = 0;
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        if (band != (c->floor >> 4)) continue;
        if ((mask & c->u0) == 0) continue;
        const double r0[2] = { (double)(int32_t)c->x, (double)(int32_t)c->z };
        const double r1[2] = { r0[0] + (double)c->width, r0[1] + (double)c->density };
        double lo = 0.0, hi = 1.0; int drin = 1;
        for (int a = 0; a < 2 && drin; a++) {
            if (d[a] == 0.0) { if (p0[a] < r0[a] || p0[a] > r1[a]) drin = 0; continue; }
            double t0 = (r0[a] - p0[a]) / d[a], t1 = (r1[a] - p0[a]) / d[a];
            if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
            if (t0 > lo) lo = t0;
            if (t1 < hi) hi = t1;
            if (lo > hi) drin = 0;
        }
        if (drin) {
            size_t len = strlen(out);
            if (len + 48 < n)
                snprintf(out + len, n - len, "%s#%d typ%d nib%x (%d,%d %dx%d)", k ? " " : "", i, c->type,
                         c->floor & 0xf, (int)c->x, (int)c->z, (int)c->width, (int)c->density);
            k++;
        }
    }
    return k;
}

typedef struct {
    uint16_t raum; int cut; int slot; unsigned typ; uint8_t grid;
    int32_t zx, zz; int16_t rot;
    int32_t px, pz; int32_t dist;                 /* Leons Platz vor der Nase, Abstand */
    int platz;                                    /* 0 = kein freier Platz gefunden */
    int reagiert;                                 /* Bild der ersten Reaktion (Sub != 0), 0 = keine in 480 */
    int sub_erst;                                 /* Sub-Zustand der ersten Reaktion */
    int los;                                      /* Sicht-Bit im letzten Bild */
    int a1024;                                    /* Kegeltest 1024 (0 = im Kegel) */
    int griff;                                    /* Bild des Griffs (re15_player_is_grabbed), 0 = keiner */
    char blocker[200];
} fall_t;

static int lauf(uint16_t raum, int cut, int slot, fall_t *f)
{
    if (!eintreten(raum, cut)) return 0;
    re15_actor_t *e = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active || !zombie_typ(e->type)) return 0;
    memset(f, 0, sizeof *f);
    f->raum = raum; f->cut = cut; f->slot = slot; f->typ = e->type; f->grid = e->grid_id;
    f->zx = e->x; f->zz = e->z; f->rot = e->rot_y;
    /* Leon vor die Nase: Blickrichtung (cos, -sin) in Port-Konvention, Abstand 800 / 1200 / 1600,
     * Winkel 0 / +-30 Grad (341) / +-60 Grad (683) — der erste freie Platz gewinnt. */
    static const int k_dist[3] = { 800, 1200, 1600 };
    static const int k_ang[5]  = { 0, 341, -341, 683, -683 };
    for (int di = 0; di < 3 && !f->platz; di++)
        for (int ai = 0; ai < 5 && !f->platz; ai++) {
            int a = ((int)e->rot_y + k_ang[ai]) & 0xfff;
            int32_t px = e->x + (int32_t)(((int64_t)re15_cos_q12(a) * k_dist[di]) >> 12);
            int32_t pz = e->z - (int32_t)(((int64_t)re15_sin_q12(a) * k_dist[di]) >> 12);
            int band = re15_collision_band_from_y(e->y);
            if (re15_collision_box_blocked(&s_rdt, px, pz, band, 450, 1u)) continue;
            if (re15_re2_los_cells_blocked(&s_rdt, e->x, e->z, px, pz, band, 1u)) continue;  /* Platz hinter einer Wand = kein "vor der Nase" */
            f->platz = 1; f->px = px; f->pz = pz;
        }
    if (!f->platz) return 1;
    pl->x = f->px; pl->z = f->pz; pl->y = e->y; pl->floor = e->floor;
    pl->rot_y = (int16_t)(((int)e->rot_y + 2048) & 0xfff);
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    {   long long dx = e->x - pl->x, dz = e->z - pl->z; f->dist = isqrt64(dx * dx + dz * dz); }
    for (int fr = 1; fr <= 480; fr++) {
        frame();
        if (!f->reagiert && e->state == 1 && e->sub_state_1 != 0) { f->reagiert = fr; f->sub_erst = e->sub_state_1; }
        if (!f->griff && re15_player_is_grabbed()) f->griff = fr;
        if (f->reagiert && (f->griff || fr >= f->reagiert + 120)) break;
    }
    f->los = e->re2z_los154;
    f->a1024 = re15_ai_arc_test(e, pl->x, pl->z, 1024);
    sicht_blocker(e, pl, f->blocker, sizeof f->blocker);
    return 1;
}

static const uint16_t k_raeume[] = { 0x1010, 0x1020, 0x1030, 0x1040, 0x1070, 0x1090, 0x10B0, 0x10D0, 0x10E0,
                                     0x1100, 0x1110, 0x1140, 0x1180, 0x11E0, 0x11F0, 0x1200, 0x1220, 0x1250 };
#define N_RAEUME ((int)(sizeof k_raeume / sizeof k_raeume[0]))

int main(int argc, char **argv)
{
    const int mess = (argc > 1 && strcmp(argv[1], "mess") == 0);
    unsigned nur = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0u;
    static fall_t faelle[400]; int nf = 0;
    int stehen_mit_sicht = 0, stehen_ohne_sicht = 0, ohne_platz = 0;
    printf("Raum Cut Slot Typ Grid  Zombie@(x,z) rot   Leon@(x,z) dist   Reaktion     LOS Kegel Griff  Blocker\n");
    for (int r = 0; r < N_RAEUME; r++) {
        uint16_t raum = k_raeume[r];
        if (nur && raum != nur) continue;
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", raum >> 12, raum);
        s_raw = slurp(p, &s_n);
        if (!s_raw) { printf("ROOM%04X: nicht lesbar\n", raum); continue; }
        int ncut = s_raw[1];
        for (int cut = 0; cut < ncut && cut < 16; cut++) {
            if (!eintreten(raum, cut)) continue;
            int slots[RE15_ACTOR_MAX]; int ns = 0;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && zombie_typ(g_actors[s].type)) {
                    unsigned nib = g_actors[s].grid_id & 0x0f;
                    if (nib >= 5 && nib <= 8) continue;              /* Fresser/Liegende: nur Skript weckt */
                    slots[ns++] = s;
                }
            for (int i = 0; i < ns && nf < 400; i++) {
                fall_t *f = &faelle[nf];
                if (!lauf(raum, cut, slots[i], f)) continue;
                /* Doppelte Faelle (gleicher Zombie in mehreren Cuts an gleicher Lage) ueberspringen */
                int dup = 0;
                for (int k = 0; k < nf; k++)
                    if (faelle[k].raum == raum && faelle[k].zx == f->zx && faelle[k].zz == f->zz &&
                        faelle[k].typ == f->typ && faelle[k].grid == f->grid) dup = 1;
                if (dup) continue;
                nf++;
                if (!f->platz) { ohne_platz++;
                    printf("%04X %3d %4d  %02X  %02X  (%6d,%6d) %4d   KEIN FREIER PLATZ\n", raum, cut, f->slot,
                           f->typ, f->grid, (int)f->zx, (int)f->zz, (int)f->rot);
                    continue; }
                if (!f->reagiert) { if (f->los) stehen_mit_sicht++; else stehen_ohne_sicht++; }
                printf("%04X %3d %4d  %02X  %02X  (%6d,%6d) %4d  (%6d,%6d) %4d   %s%3d sub%2d   %d   %4d  %4d   %s\n",
                       raum, cut, f->slot, f->typ, f->grid, (int)f->zx, (int)f->zz, (int)f->rot,
                       (int)f->px, (int)f->pz, (int)f->dist, f->reagiert ? "Bild " : "STEHT ", f->reagiert,
                       f->sub_erst, f->los, f->a1024, f->griff, f->blocker);
            }
        }
        free(s_raw); s_raw = NULL;
    }
    printf("\n%d Faelle: %d stehen MIT Sicht (Befund), %d stehen ohne Sicht, %d ohne freien Platz\n",
           nf, stehen_mit_sicht, stehen_ohne_sicht, ohne_platz);
    if (mess) return 0;

    /* Riegel: kein stehender Zombie mit Sicht und Leon vor der Nase bleibt 480 Bilder stehen
     * (RE2 DECISION[0] @0x80101308-1C sofort, Selbstwecker spaetestens Bild 405 @0x80101478-88). */
    CHECK(nf > 20, "Zensus deckt %d stehende Zombies (Stage 1, Leon-Raeume)", nf);
    CHECK(stehen_mit_sicht == 0, "kein stehender Zombie MIT Sicht bleibt 480 Bilder stehen (%d)", stehen_mit_sicht);
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
