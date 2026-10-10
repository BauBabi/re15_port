/*
 * Runde 36 Spur F "zombieki", Punkt 1 — "sie bleiben hier ewig Beteiligungslos stehen, obwohl ich ziemlich in
 * Ihrer Naehe vor ihnen bin, fast vor ihrer Nase." Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 1.
 *
 * SICHT-ZENSUS: der Weck-Zensus (test_r36_zombieki_wecker) stellte Leon nur auf Plaetze, die der Zell-Strahl
 * (re15_re2_los_cells_blocked) FREI meldet — er konnte den Nutzer-Fall deshalb nicht finden, wenn die Ursache der
 * Zell-Strahl selbst ist. Hier wird fuer jeden stehenden Zombie jedes Leon-Raums der Stage 1 ein Raster von
 * Leon-Plaetzen im Vorderkegel (+-1024, Abstand bis 4000, Schritt 400) abgetastet, auf dem Leon stehen kann
 * (re15_collision_box_blocked r 450 Maske 1 frei), und je Platz gemessen:
 *   zell  = Zell-Strahl re15_re2_los_cells_blocked (Slab-Test gegen JEDE Zelle mit u0&4, Port-Mapping seit
 *           2026-09-08 fuer die Kraehe, auch vom Zombie gelesen)
 *   ray   = RE1.5-Sichtstrahl FUN_8003dcc4 (nur Zellen mit Boden-Nibble 3, re15_los_ray_blocked ueber 4 Regionen)
 *   steht = der Zombie reagiert in 480 Bildern NICHT (echte Spielschleife, keine Eingabe)
 * Der Nutzer-Befund = Plaetze mit steht=1. Ohne Argument: Riegel (siehe Ende). "mess [raum-hex]": nur Tabelle.
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
#include "re15_zombieki_r36.h"
#include "re15_md1.h"
#include "test_r36_zombieki_kopf.h"   /* NB2 Mangel 5: Leons Kopf aus PL00 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    kopf_setzen(&g_actors[RE15_ACTOR_SLOT_PLAYER]);   /* NB2 Mangel 5: head_world wie der Renderer (PL00-Daten) */
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

/* erste Zelle, die der Slab-Test trifft (fuer die Tabelle) */
static int zell_treffer(const re15_actor_t *e, int32_t px, int32_t pz, char *out, size_t n)
{
    out[0] = 0;
    int band = re15_collision_band_from_y(e->y);
    unsigned mask = e->sca_mask ? e->sca_mask : 4u;
    const double p0[2] = { (double)e->x, (double)e->z };
    const double d[2]  = { (double)px - (double)e->x, (double)pz - (double)e->z };
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
            if (len + 40 < n)
                snprintf(out + len, n - len, "%s#%d t%d nib%x u0=%02x", k ? " " : "", i, c->type, c->floor & 0xf, c->u0);
            k++;
        }
    }
    return k;
}

typedef struct { uint16_t raum; int cut, slot; unsigned typ; int32_t zx, zz; int16_t rot; int anz, zell, ray, zell_nur, steht, steht_zell_nur, steht_frei, rmax, spaet, spaet_zell; } zomb_t;
typedef struct { uint16_t raum; int cut, slot; int32_t px, pz, dist; int zell, ray, reagiert, sub; char zelle[120]; } platz_t;

static const uint16_t k_raeume[] = { 0x1010, 0x1020, 0x1030, 0x1040, 0x1070, 0x1090, 0x10B0, 0x10D0, 0x10E0,
                                     0x1100, 0x1110, 0x1140, 0x1180, 0x11E0, 0x11F0, 0x1200, 0x1220, 0x1250 };
#define N_RAEUME ((int)(sizeof k_raeume / sizeof k_raeume[0]))

typedef struct { int nz; long plaetze, zell_blk, ray_blk, zell_nur, steht, steht_zell_nur, steht_frei, steht_ray, steht_wand, steht_moebel, ausserhalb; } summe_t;

/* NB1 (Abnahme 0, Mangel 3): Klasse der Nibble-3-Zellen, die der RE1.5-Strahl (FUN_8003dcc4 je Zelle) von e nach (px,pz)
 * kreuzt: *wand = Zellen OHNE belegte Oberkante (volle Hoehe), *moebel = Zellen MIT belegter Oberkante
 * (re15_r36_zelle_oberkante: Obj_model_set darauf oder Messtabelle). */
static void kreuz_klassen(const re15_actor_t *e, int32_t px, int32_t pz, int *wand, int *moebel)
{
    *wand = 0; *moebel = 0;
    unsigned mask = e->sca_mask ? e->sca_mask : 4u;
    int start = 0;
    for (int k = 0; k < 4; k++) {
        int cnt = s_rdt.sca_rgn[k];
        if (start + cnt > s_rdt.sca_count) break;
        for (int i = 0; i < cnt; i++) {
            const re15_sca_entry_t *c = &s_rdt.sca[start + i];
            if ((c->floor >> 4) != e->floor || (c->floor & 0x0f) != 3 || !(mask & c->u0)) continue;
            if (!re15_r36_los_zelle_kreuzt(e->x, e->z, px, pz, c)) continue;
            if (re15_r36_zelle_oberkante(c, NULL) != 0) (*moebel)++; else (*wand)++;
        }
        start += cnt;
    }
}

/* Der Zensus: modus 1 = Weg vor Runde 36 (re15_re2_los_clear mit Slab-Test), 2 = Abbildung vor NB1 (jede Nibble-3-Zelle
 * blockt), 0 = NB1 (Nibble-3-Zelle mit belegter Oberkante blockt nur unter dem Kopf-zu-Kopf-Strahl). */
static summe_t zensus(int vorher, unsigned nur, int lauf_bilder)
{
    re15_r36_zombie_sicht_enable(vorher == 1 ? 0 : 1);
    re15_r36_zombie_hoehe_enable(vorher == 2 ? 0 : 1);
    printf("Sicht-Bit: %s\n", vorher == 1 ? "vor Runde 36 (re15_re2_los_clear, Slab-Test + Nibble-3-Strahl)" :
                              vorher == 2 ? "vor NB1 (jede Nibble-3-Zelle blockt)" : "NB1 (Nibble 3, belegte Moebel-Oberkante gegen den Kopfstrahl)");
    static zomb_t zs[200]; int nz = 0;
    static platz_t steher[2000]; int nsteh = 0;
    long plaetze = 0, zell_blk = 0, ray_blk = 0, zell_nur = 0, steht = 0, steht_zell_nur = 0, steht_frei = 0, steht_ray = 0;
    long steht_wand = 0, steht_moebel = 0, ausserhalb = 0;
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
                    if (nib >= 5 && nib <= 8) continue;
                    slots[ns++] = s;
                }
            for (int i = 0; i < ns && nz < 200; i++) {
                int slot = slots[i];
                if (!eintreten(raum, cut)) continue;
                re15_actor_t *e = &g_actors[slot];
                int dup = 0;
                for (int k = 0; k < nz; k++)
                    if (zs[k].raum == raum && zs[k].zx == e->x && zs[k].zz == e->z && zs[k].typ == e->type) dup = 1;
                if (dup) continue;
                zomb_t *z = &zs[nz++]; memset(z, 0, sizeof *z);
                z->raum = raum; z->cut = cut; z->slot = slot; z->typ = e->type; z->zx = e->x; z->zz = e->z; z->rot = e->rot_y;
                int band = re15_collision_band_from_y(e->y);
                unsigned mask = e->sca_mask ? e->sca_mask : 4u;
                for (int32_t dx = -5000; dx <= 5000; dx += 400)
                    for (int32_t dz = -5000; dz <= 5000; dz += 400) {
                        int32_t px = e->x + dx, pz = e->z + dz;
                        int32_t dist = isqrt64((long long)dx * dx + (long long)dz * dz);
                        if (dist < 600 || dist >= 0x1388) continue;          /* DECISION[0] dist < 0x1388 @0x80101308 */
                        if (re15_collision_box_blocked(&s_rdt, px, pz, band, 450, 1u)) continue;
                        if (re15_nav_zone_count() > 0 && re15_nav_zone_from_pos((int16_t)px, (int16_t)pz) == 0xff) { ausserhalb++; continue; }
                                                                             /* NB1: nur Plaetze in einer BLK-Zone (Capcoms begehbare
                                                                              * Flaeche) — vorher zaehlten Plaetze hinter Aussenwaenden */
                        /* Vorderkegel +-1024 wie DECISION[0] (re15_ai_arc_test == 0) */
                        if (!eintreten(raum, cut)) continue;
                        e = &g_actors[slot];
                        if (re15_ai_arc_test(e, px, pz, 1024) != 0) continue;
                        int zell = re15_re2_los_cells_blocked(&s_rdt, e->x, e->z, px, pz, band, mask) ? 1 : 0;
                        int ray = re15_r36_los_ray_nib3(e, px, pz) ? 1 : 0;
                        plaetze++; z->anz++;
                        if (zell) { zell_blk++; z->zell++; }
                        if (ray) { ray_blk++; z->ray++; }
                        if (zell && !ray) { zell_nur++; z->zell_nur++; }
                        /* Reaktion: Leon dorthin, Blick auf den Zombie, keine Eingabe */
                        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                        pl->x = px; pl->z = pz; pl->y = e->y; pl->floor = e->floor;
                        pl->rot_y = (int16_t)((((int)re15_atan2_q12(e->z - pz, e->x - px) - 0x400)) & 0xfff);
                        re15_collision_set_band(band);
                        int reagiert = 0, sub = 0;
                        for (int fr = 1; fr <= lauf_bilder; fr++) {
                            frame();
                            if (e->state == 1 && e->sub_state_1 != 0) { reagiert = fr; sub = e->sub_state_1; break; }
                        }
                        if (reagiert > z->rmax) z->rmax = reagiert;
                        if (reagiert > 60) { z->spaet++; if (zell && !ray) z->spaet_zell++; }
                        if (!reagiert) {
                            steht++; z->steht++;
                            { int w = 0, m = 0; kreuz_klassen(e, px, pz, &w, &m); if (w) steht_wand++; else if (m) steht_moebel++; }
                            if (zell && !ray) { steht_zell_nur++; z->steht_zell_nur++; }
                            if (!zell && !ray) { steht_frei++; z->steht_frei++; }
                            if (ray) steht_ray++;
                            if (nsteh < 2000) {
                                platz_t *s = &steher[nsteh++]; memset(s, 0, sizeof *s);
                                s->raum = raum; s->cut = cut; s->slot = slot; s->px = px; s->pz = pz; s->dist = dist;
                                s->zell = zell; s->ray = ray; s->reagiert = reagiert; s->sub = sub;
                                zell_treffer(e, px, pz, s->zelle, sizeof s->zelle);
                            }
                        }
                    }
            }
        }
        free(s_raw); s_raw = NULL;
    }
    printf("Raum Cut Slot Typ  Zombie@(x,z) rot | Plaetze  zell  ray  zellNUR | steht  davon zellNUR  davon frei | Reakt.max  >60 (davon zellNUR)\n");
    for (int k = 0; k < nz; k++) {
        zomb_t *z = &zs[k];
        printf("%04X %3d %4d  %02X  (%6d,%6d) %4d | %7d %5d %4d %7d | %5d %14d %11d | %8d %5d %5d\n", z->raum, z->cut, z->slot, z->typ,
               (int)z->zx, (int)z->zz, (int)z->rot, z->anz, z->zell, z->ray, z->zell_nur, z->steht, z->steht_zell_nur, z->steht_frei, z->rmax, z->spaet, z->spaet_zell);
    }
    printf("\nStehende Plaetze (Zombie reagiert in %d Bildern nicht):\n", lauf_bilder);
    for (int k = 0; k < nsteh; k++) {
        platz_t *s = &steher[k];
        printf("  %04X cut%d slot%d Leon@(%d,%d) d=%d zell=%d ray=%d  %s\n", s->raum, s->cut, s->slot, (int)s->px, (int)s->pz,
               (int)s->dist, s->zell, s->ray, s->zelle);
    }
    printf("\n%d Zombies, %ld Plaetze im Vorderkegel < 0x1388 in einer BLK-Zone (%ld ausserhalb verworfen): Zell-Strahl blockt %ld (%.1f%%), "
           "RE1.5-Strahl (Nibble 3) blockt %ld (%.1f%%), nur Zell-Strahl %ld (%.1f%%); STEHT %ld (%.1f%%): davon nur-Zell-Strahl %ld, "
           "beide frei %ld, Nibble-3 %ld; Klassen: hinter WAND %ld, nur hinter belegten MOEBELN %ld\n",
           nz, plaetze, ausserhalb, zell_blk, plaetze ? 100.0 * zell_blk / plaetze : 0.0, ray_blk, plaetze ? 100.0 * ray_blk / plaetze : 0.0,
           zell_nur, plaetze ? 100.0 * zell_nur / plaetze : 0.0, steht, plaetze ? 100.0 * steht / plaetze : 0.0,
           steht_zell_nur, steht_frei, steht_ray, steht_wand, steht_moebel);
    summe_t su = { nz, plaetze, zell_blk, ray_blk, zell_nur, steht, steht_zell_nur, steht_frei, steht_ray, steht_wand, steht_moebel, ausserhalb };
    return su;
}

/* NB2 (Abnahme 1, Mangel 2): alle RDTs aller Stages — welche Nibble-3-Zellen bekommen eine Oberkante (Grund, Objekt,
 * Lage zu den BLK-Zonen)? `oberkante [nur-hex]`. */
static void oberkante_liste(unsigned nur)
{
    int n_zellen = 0, n_ok = 0;
    for (unsigned st = 1; st <= 7; st++)
        for (unsigned r = 0; r < 0x100; r++)
            for (unsigned v = 0; v < 2; v++) {
                uint16_t raum = (uint16_t)((st << 12) | (r << 4) | v);
                if (nur && raum != nur) continue;
                char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", st, raum);
                s_raw = slurp(p, &s_n);
                if (!s_raw) continue;
                if (!eintreten(raum, 0)) { free(s_raw); s_raw = NULL; continue; }
                for (int i = 0; i < s_rdt.sca_count; i++) {
                    const re15_sca_entry_t *c = &s_rdt.sca[i];
                    if ((c->floor & 0x0f) != 3) continue;
                    n_zellen++;
                    int g = 0; int32_t top = re15_r36_zelle_oberkante(c, &g);
                    if (!top) continue;
                    n_ok++;
                    int inzone = -1;
                    for (int b = 0; b < s_rdt.block_count; b++) {
                        const uint8_t *nd = s_rdt.blocks + b * 12;
                        int16_t x1 = (int16_t)(nd[0] | nd[1] << 8), z1 = (int16_t)(nd[2] | nd[3] << 8);
                        int16_t x2 = (int16_t)(nd[4] | nd[5] << 8), z2 = (int16_t)(nd[6] | nd[7] << 8);
                        if ((int32_t)c->x >= x1 && (int32_t)c->x + (int32_t)c->width <= x2 && (int32_t)c->z >= z1 && (int32_t)c->z + (int32_t)c->density <= z2) { inzone = b; break; }
                    }
                    printf("%04X #%-3d x %6d..%6d z %6d..%6d (%5dx%5d) t%d u0=%02x u1=%02x fl=%02x top %6d grund %d zone %d |", raum, i,
                           (int)c->x, (int)c->x + (int)c->width, (int)c->z, (int)c->z + (int)c->density, (int)c->width, (int)c->density,
                           c->type, c->u0, c->u1, c->floor, (int)top, g, inzone);
                    for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++) {
                        const int32_t px = g_scd.props[k].x, pz = g_scd.props[k].z;
                        if (g_scd.props[k].parent_obj >= 0 || g_scd.props[k].y >= 0) continue;
                        if ((uint32_t)(px - (int32_t)c->x) >= (uint32_t)c->width || (uint32_t)(pz - (int32_t)c->z) >= (uint32_t)c->density) continue;
                        printf(" obj%d(%d,%d,%d r%d box %d,%d,%d/%d,%d,%d fl%04x)", k, (int)px, (int)g_scd.props[k].y, (int)pz, (int)g_scd.props[k].rot_y,
                               g_scd.props[k].box_cx, g_scd.props[k].box_cy, g_scd.props[k].box_cz, g_scd.props[k].box_hx, g_scd.props[k].box_hy,
                               g_scd.props[k].box_hz, g_scd.props[k].flags);
                        int oid = g_scd.props[k].obj_id; re15_md1_t md;
                        if (oid < RE15_RDT_MAX_PROPS && s_rdt.prop_md1[oid] && re15_md1_parse(s_rdt.prop_md1[oid], s_rdt.prop_md1_size[oid], &md) == 0) {
                            int mn[3] = { 99999, 99999, 99999 }, mx[3] = { -99999, -99999, -99999 }, nv = 0;
                            for (int pass = 0; pass < 2 * md.mesh_count; pass++) {
                                const re15_md1_mesh_t *ms = &md.meshes[pass / 2];
                                const re15_md1_vertex_t *vv = (pass & 1) ? ms->quad_vertices : ms->tri_vertices; int cnt = (pass & 1) ? ms->quad_vertex_count : ms->tri_vertex_count;
                                for (int q = 0; q < cnt && vv; q++) {
                                    int a[3] = { vv[q].x, vv[q].y, vv[q].z }; nv++;
                                    for (int ax = 0; ax < 3; ax++) { if (a[ax] < mn[ax]) mn[ax] = a[ax]; if (a[ax] > mx[ax]) mx[ax] = a[ax]; }
                                }
                            }
                            printf("[md1 id%d nv%d x%d..%d y%d..%d z%d..%d]", oid, nv, mn[0], mx[0], mn[1], mx[1], mn[2], mx[2]);
                        }
                    }
                    printf("\n");
                }
                free(s_raw); s_raw = NULL;
            }
    printf("\n%d Nibble-3-Zellen, %d mit Oberkante\n", n_zellen, n_ok);
}

/* NB2 (Abnahme 1, Mangel 2): DURCHWAND-Zensus — alle RDTs aller Stages (beide Varianten), jeder Zombie (Typ 0x10/11/12/16/18,
 * auch Kriecher), Raster von Leon-Plaetzen (Schritt 250, 600 <= d < 0x1388 @0x80101308, Leon-Klemme frei r 450 Maske 1, in einer
 * BLK-Zone): wo OEFFNET die Hoehenregel die Sicht, die ohne sie (jede Nibble-3-Zelle blockt) zu ist — und ueber WELCHE Zellen
 * (gekreuzte Nibble-3-Zellen mit Oberkante). Ergebnis je (Raum, Zelle). Rueckgabe: Zahl geoeffneter Plaetze; *wand1040 = davon
 * ueber ROOM1040/1041 #4 (die Wand der Abnahme 1). */
typedef struct { uint16_t raum; int zelle; int16_t x, z; uint16_t w, d; int32_t top; long n, kegel; } dw_t;
static dw_t s_dw[400]; static int s_ndw = 0;
static long durchwand(unsigned nur, int ausgabe, long *wand1040, int modus)   /* modus 1 = NB2 (Beleg-Tabelle), 2 = NB1-Abbildung */
{
    long offen = 0, plaetze = 0; int nzomb = 0; s_ndw = 0; *wand1040 = 0;
    re15_r36_zombie_sicht_enable(1);
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned r = 0; r < 0x100; r++)
            for (unsigned v = 0; v < 2; v++) {
                uint16_t raum = (uint16_t)((st << 12) | (r << 4) | v);
                if (nur && raum != nur) continue;
                char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", st, raum);
                s_raw = slurp(p, &s_n);
                if (!s_raw) continue;
                if (!eintreten(raum, 0)) { free(s_raw); s_raw = NULL; continue; }
                for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                    re15_actor_t *e = &g_actors[s];
                    if (!e->active || !zombie_typ(e->type)) continue;
                    nzomb++;
                    int band = re15_collision_band_from_y(e->y);
                    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    for (int32_t dx = -5000; dx <= 5000; dx += 250)
                        for (int32_t dz = -5000; dz <= 5000; dz += 250) {
                            int32_t px = e->x + dx, pz = e->z + dz;
                            int32_t dist = isqrt64((long long)dx * dx + (long long)dz * dz);
                            if (dist < 600 || dist >= 0x1388) continue;
                            if (re15_collision_box_blocked(&s_rdt, px, pz, band, 450, 1u)) continue;
                            if (re15_nav_zone_count() > 0 && re15_nav_zone_from_pos((int16_t)px, (int16_t)pz) == 0xff) continue;
                            plaetze++;
                            pl->x = px; pl->z = pz; pl->y = e->y; pl->floor = e->floor; kopf_setzen(pl);   /* Kopf aus PL00 (NB2 M5) */
                            re15_r36_zombie_hoehe_enable(0); int s0 = re15_r36_zombie_sicht_frei(e, pl);
                            re15_r36_zombie_hoehe_enable(modus); int s1 = re15_r36_zombie_sicht_frei(e, pl);
                            if (!s1 || s0) continue;
                            offen++;
                            int kegel = re15_ai_arc_test(e, px, pz, 1024) == 0;
                            unsigned mask = e->sca_mask ? e->sca_mask : 4u;
                            int gesehen[64]; int ng = 0, w1040 = 0;   /* je Platz jede Zelle EINMAL (SCA-Saetze stehen je Partition mehrfach) */
                            for (int i = 0; i < s_rdt.sca_count; i++) {
                                const re15_sca_entry_t *c = &s_rdt.sca[i];
                                if ((c->floor >> 4) != e->floor || (c->floor & 0x0f) != 3 || !(mask & c->u0)) continue;
                                if (!re15_r36_los_zelle_kreuzt(e->x, e->z, px, pz, c)) continue;
                                int32_t top = re15_r36_zelle_oberkante(c, NULL);
                                if (!top) continue;
                                int k = 0;
                                for (k = 0; k < s_ndw; k++)
                                    if ((s_dw[k].raum & 0xFFF0u) == (raum & 0xFFF0u) && s_dw[k].x == c->x && s_dw[k].z == c->z && s_dw[k].w == c->width && s_dw[k].d == c->density) break;
                                if (k == s_ndw && s_ndw < 400) { s_dw[k].raum = raum; s_dw[k].zelle = i; s_dw[k].x = c->x; s_dw[k].z = c->z; s_dw[k].w = c->width; s_dw[k].d = c->density; s_dw[k].top = top; s_dw[k].n = 0; s_dw[k].kegel = 0; s_ndw++; }
                                int neu = 1; for (int q = 0; q < ng; q++) if (gesehen[q] == k) neu = 0;
                                if (k < s_ndw && neu) { s_dw[k].n++; if (kegel) s_dw[k].kegel++; if (ng < 64) gesehen[ng++] = k; }
                                if ((raum & 0xFFF0u) == 0x1040u && c->x == -28641 && c->width == 2052) w1040 = 1;
                            }
                            if (w1040) (*wand1040)++;
                        }
                }
                free(s_raw); s_raw = NULL;
            }
    re15_r36_zombie_hoehe_enable(1);
    if (ausgabe) {
        for (int k = 0; k < s_ndw; k++)
            printf("  %04X #%-3d x %6d..%6d z %6d..%6d (%5dx%5d) Oberkante %6d: %5ld Plaetze geoeffnet (%ld im Kegel)\n", s_dw[k].raum, s_dw[k].zelle,
                   (int)s_dw[k].x, (int)s_dw[k].x + (int)s_dw[k].w, (int)s_dw[k].z, (int)s_dw[k].z + (int)s_dw[k].d, (int)s_dw[k].w, (int)s_dw[k].d,
                   (int)s_dw[k].top, s_dw[k].n, s_dw[k].kegel);
        printf("DURCHWAND: %d Zombies, %ld Plaetze, Hoehenregel oeffnet %ld (davon ueber ROOM1040 #4: %ld)\n", nzomb, plaetze, offen, *wand1040);
    }
    return offen;
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "oberkante") == 0) { oberkante_liste(argc > 2 ? (unsigned)strtoul(argv[2], NULL, 16) : 0u); return 0; }
    if (argc > 1 && strcmp(argv[1], "kopf") == 0) {   /* NB2 Mangel 5: Leons Kopf (Knochen 8) je Clip, Bild 0 */
        if (!kopf_laden()) { printf("PL00 nicht ladbar\n"); return 1; }
        for (int c = 0; c < s_kopf_anim.clip_count; c++) {
            int32_t hm[3]; int fc = s_kopf_anim.clips[c].frame_count;
            if (kopf_modell(c, 0, hm) == 0) printf("Clip %2d (%3d Bilder): Kopf Bild 0 (%d,%d,%d)\n", c, fc, (int)hm[0], (int)hm[1], (int)hm[2]);
        }
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "durchwand") == 0) { long w = 0; durchwand(argc > 2 ? (unsigned)strtoul(argv[2], NULL, 16) : 0u, 1, &w, argc > 3 ? atoi(argv[3]) : 1); return 0; }
    const int mess = (argc > 1 && strcmp(argv[1], "mess") == 0);
    const int vorher = (argc > 1 && strcmp(argv[1], "vorher") == 0) ? 1 : (argc > 1 && strcmp(argv[1], "vornb1") == 0) ? 2 : 0;   /* Gegenproben */
    unsigned nur = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0u;
    const int lauf_bilder = (argc > 3) ? atoi(argv[3]) : 480;
    if (mess || vorher) { zensus(vorher, nur, lauf_bilder); return 0; }

    /* NB1 Mangel 1 — der Abnahme-Fall wortgleich: ROOM1010 Cut 0, Slot 2 (205,7248) rot 0, Leon (3669,3962): Strahl ueber
     * Tisch #5 (Oberkante -1600, Obj_model_set) und Stuhlkreis #12 (gemessen < -2000) — RE2 (Kopfteil 8, FUN_80050858 a3=1)
     * sieht darueber; die Abbildung vor NB1 (jede Nibble-3-Zelle blockt) nicht. */
    {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT");
        s_raw = slurp(p, &s_n);
        int sv = -1, sn = -1;
        if (s_raw && eintreten(0x1010, 0)) {
            re15_actor_t *e = &g_actors[2], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            if (e->active && e->x == 205 && e->z == 7248) {
                pl->x = 3669; pl->z = 3962; pl->y = 0; kopf_setzen(pl);
                re15_r36_zombie_sicht_enable(1);
                re15_r36_zombie_hoehe_enable(0); sv = re15_r36_zombie_sicht_frei(e, pl);
                re15_r36_zombie_hoehe_enable(1); sn = re15_r36_zombie_sicht_frei(e, pl);
            }
        }
        free(s_raw); s_raw = NULL;
        CHECK(sv == 0, "Abnahme-Fall ROOM1010 Slot 2 -> Leon (3669,3962), Abbildung vor NB1: Sicht verdeckt (%d) = Befund", sv);
        CHECK(sn == 1, "Abnahme-Fall mit NB1: Sicht frei (%d) — Tisch -1600 / Stuhl < -2000 unter dem Kopfstrahl", sn);
    }
    summe_t m2 = zensus(2, 0x1010u, 480);
    CHECK(m2.steht_moebel > 0, "Gegenprobe ROOM1010 vor NB1: %ld stehende Plaetze NUR hinter belegten Moebeln (Befund Mangel 1)", m2.steht_moebel);

    /* Riegel. (1) Gegenprobe = Befund: mit dem alten Weg (Slab-Test) bleiben in ROOM10B0 Zombies 480 Bilder stehen,
     * obwohl der RE1.5-Sichtstrahl FUN_8003dcc4 (Nibble 3) frei ist (Terrassenkante #9, Nibble 1). (2) Mit dem Haken
     * (nur FUN_8003dcc4) steht kein Zombie der Stage 1 mehr, wo nur der Slab-Test die Sicht nahm, und keiner bei
     * freier Sicht (RE2 DECISION[0] @0x80101308-1C: dist < 0x1388 && Kegel && +0x154&0x800 -> 0x101). */
    summe_t v = zensus(1, 0x10B0u, 480);
    CHECK(v.steht_zell_nur > 0, "Gegenprobe ROOM10B0 (alter Weg): %ld Plaetze, an denen der Zombie nur wegen des Slab-Tests steht (Befund)", v.steht_zell_nur);
    summe_t n = zensus(0, 0, 480);
    CHECK(n.nz > 20, "Zensus deckt %d stehende Zombies", n.nz);
    CHECK(n.plaetze > 500, "Raster deckt %ld Leon-Plaetze (in BLK-Zonen, Abstand < 0x1388)", n.plaetze);
    CHECK(n.steht_zell_nur == 0, "kein Zombie steht 480 Bilder, wo nur der Zell-Rechteck-Strahl die Sicht nimmt (%ld)", n.steht_zell_nur);
    CHECK(n.steht_frei == 0, "kein Zombie steht 480 Bilder bei freier Sicht (%ld)", n.steht_frei);
    CHECK(n.steht_moebel == 0, "kein Zombie steht 480 Bilder NUR hinter belegten Moebeln (%ld)", n.steht_moebel);
    CHECK(n.steht == n.steht_wand, "jeder stehende Zombie-Platz (%ld) liegt hinter einer Nibble-3-Zelle ohne belegte Hoehe = Wand (%ld)", n.steht, n.steht_wand);

    /* NB2 Mangel 2 (Abnahme 1): WAENDE sind keine Moebel. (1) Durchwand-Zensus aller Stages (jeder Zombie, Raster 250 bis
     * 0x1388, begehbar + BLK-Zone): Gegenprobe NB1-Abbildung oeffnet Plaetze ueber die Wand ROOM1040 #4 (Befund); NB2 oeffnet
     * dort 0, und JEDE Zelle, ueber die die Hoehenregel ueberhaupt Sicht oeffnet, steht in der Beleg-Tabelle. */
    {
        long w_alt = 0, w_neu = 0;
        long o_alt = durchwand(0, 0, &w_alt, 2);
        CHECK(w_alt > 0, "Gegenprobe NB1-Abbildung: Hoehenregel oeffnet %ld Plaetze, davon %ld ueber die Wand ROOM1040 #4 (Befund Abnahme 1)", o_alt, w_alt);
        long o_neu = durchwand(0, 1, &w_neu, 1);
        CHECK(w_neu == 0, "NB2: 0 Plaetze ueber die Wand ROOM1040 #4 geoeffnet (%ld; gesamt %ld)", w_neu, o_neu);
        static const struct { uint16_t raum; int16_t x, z; uint16_t w, d; } beleg[] = {
            { 0x1010, -50, 1550, 2500, 4350 }, { 0x1010, 1850, 4100, 1200, 1200 }, { 0x1010, 1850, 2200, 1200, 1200 }, { 0x1010, -700, 2800, 1200, 1200 },
            { 0x1220, -27600, -11700, 1950, 3650 }, { 0x1220, -27600, -19750, 1950, 4150 }, { 0x1220, -13475, -23600, 2125, 3800 } };
        int fremd = 0;
        for (int k = 0; k < s_ndw; k++) {
            int ok = 0;
            for (unsigned b = 0; b < sizeof beleg / sizeof beleg[0]; b++)
                if ((s_dw[k].raum & 0xFFF0u) == beleg[b].raum && s_dw[k].x == beleg[b].x && s_dw[k].z == beleg[b].z && s_dw[k].w == beleg[b].w && s_dw[k].d == beleg[b].d) ok = 1;
            if (!ok) { fremd++; printf("  fremde Zelle %04X #%d (%d,%d %ux%u)\n", s_dw[k].raum, s_dw[k].zelle, (int)s_dw[k].x, (int)s_dw[k].z, s_dw[k].w, s_dw[k].d); }
        }
        CHECK(o_neu > 0 && fremd == 0, "NB2: die Hoehenregel oeffnet %ld Plaetze nur ueber belegte Moebel-Zellen (%d Zellen, fremd %d)", o_neu, s_ndw, fremd);
    }
    /* (2) Die exe-Lage der Abnahme 1 (ROOM1040, Leon im Westgang (-29900,6000)): Sicht-Bit der Ostgang-Zombies. */
    {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1040.RDT");
        s_raw = slurp(p, &s_n);
        int sicht[3] = { 0, 0, 0 }, ost = 0;
        for (int m = 1; m <= 2 && s_raw; m++) {
            if (!eintreten(0x1040, 0)) break;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->x = -29900; pl->z = 6000; pl->y = 0; pl->floor = 0; kopf_setzen(pl);
            re15_r36_zombie_hoehe_enable(m);
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active || !zombie_typ(e->type) || e->x <= -26589) continue;   /* oestlich der Wand #4 (x -28641..-26589) */
                if (m == 1) ost++;
                if (re15_r36_zombie_sicht_frei(e, pl)) sicht[m]++;
            }
        }
        re15_r36_zombie_hoehe_enable(1);
        free(s_raw); s_raw = NULL;
        CHECK(ost > 0 && sicht[2] > 0, "Gegenprobe ROOM1040 Leon im Westgang, NB1-Abbildung: %d von %d Ostgang-Zombies 'sehen' ihn durch die Wand", sicht[2], ost);
        CHECK(sicht[1] == 0, "ROOM1040 Leon im Westgang, NB2: %d von %d Ostgang-Zombies sehen ihn (0 = Wand haelt)", sicht[1], ost);
    }
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
