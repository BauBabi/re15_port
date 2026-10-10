/*
 * Runde 36 Spur F "zombieki", Punkt 1, Nachbesserung 1 (Abnahme 0, Maengel 1/3) — "sie bleiben hier ewig
 * Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen bin, fast vor ihrer Nase. Auch wenn sie
 * innerhalb eines Raumes sind." (AUFTRAG.md Z. 59). Dossier: analysis/befunde_runde36/F_zombieki.md.
 *
 * EINTRITTS-ZENSUS NACH LAGE. Die beiden Vorgaenger-Zensen (wecker/sicht) stellten Leon auf konstruierte Plaetze
 * und zaehlten eine REAKTION (Sub-Wechsel) — ein Zombie, der reagiert und dann auf der Stelle tritt, galt als
 * "reagiert". Hier zaehlt die LAGE:
 *   - Eintritte: jede Door_aot_set-Tuer jedes Leon-Raums der Stage 1, deren Ziel ein Raum mit Zombies ist
 *     (Ziel-Id ((pc22+1)<<12)|(pc23<<4) wie aot_common.c), Leon am Spawnpunkt der Tuer mit Spawn-Yaw, Ziel-Cut.
 *   - Szenario "steht": Leon bleibt am Eintritt (keine Eingabe).
 *   - Szenario "hin":   Leon geht per Pad (UP, Blick auf den Zombie) auf JEDEN Zombie zu, bis 900 vor ihm.
 *   - je Zombie: erste Reaktion (Sub/State-Wechsel), kleinster Abstand, laengste Strecke OHNE Bewegung (< 40
 *     Einheiten je 30 Bilder) bei Leon < 5000, nicht im Griff.
 *   Klassen: PASSIV = nie reagiert, obwohl Leon >= 120 Bilder < 5000 im Kegel +-1024 war; KLEMME = reagiert,
 *   danach >= 300 Bilder ohne Bewegung mit Abstand > 1500 (Griff-Bilder zaehlen nicht als Stillstand); sonst ok.
 * Ohne Argument: Riegel. "mess [raum-hex] [bilder]": Tabelle.
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
static uint16_t           s_pad = 0, s_pad_alt = 0;
static int                s_spur_slot = 0;   /* > 0: Zeitlinie dieses Slots (Sonde `spur`) */
static int                s_spur_takt = 30;  /* Zeilen je n Bilder (RE15-Sonde: Umgebungsvariable nicht noetig, 8. Argument) */

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
    s_ctx.pad_current = s_pad;
    s_ctx.pad_pressed = (uint16_t)(s_pad & (uint16_t)~s_pad_alt);
    s_pad_alt = s_pad;
    re15_game_step(&s_ctx);
}

static int zombie_typ(unsigned t) { return t == 0x10 || t == 0x11 || t == 0x12 || t == 0x16 || t == 0x18; }

static uint8_t *s_raw; static size_t s_n;
static int raum_laden(uint16_t raum)
{
    if (s_raw) { free(s_raw); s_raw = NULL; }
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", raum >> 12, raum);
    s_raw = slurp(p, &s_n);
    return s_raw != NULL;
}

static int eintreten_an(uint16_t raum, int cut, int32_t px, int32_t py, int32_t pz, int16_t rot)
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
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = rot;
    int band = re15_collision_band_from_y(py);
    pl->floor = (uint8_t)band;
    re15_collision_set_band(band);
    s_shown = cut; s_pad = 0; s_pad_alt = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    g_scd.cut_auto_enabled = 1;
    load_bank_re2(0x10); load_bank_re2(0x11); load_bank_re2(0x12); load_bank_re2(0x16); load_bank_re2(0x18);
    return 1;
}

static int32_t isqrt64(long long v) { if (v <= 0) return 0; long long r = 0; long long b = 1LL << 31;
    while (b) { long long t = r + b; if (t * t <= v) r = t; b >>= 1; } return (int32_t)r; }
static int32_t abst(const re15_actor_t *a, const re15_actor_t *b)
{ long long dx = a->x - b->x, dz = a->z - b->z; return isqrt64(dx * dx + dz * dz); }

/* ---------------- Eintritte sammeln ---------------- */
typedef struct { uint16_t von, ziel; int cut; int32_t x, y, z; int16_t rot; } eintritt_t;
static eintritt_t s_ein[400]; static int s_nein = 0;

static void eintritte_sammeln(void)
{
    for (int r = 0; r < 0x30; r++) {
        uint16_t raum = (uint16_t)(0x1000 | (r << 4));
        if (!raum_laden(raum)) continue;
        if (!eintreten_an(raum, 0, 0, 0, 0, 0)) continue;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
            const re15_aot_door_params_t *d = &g_aot.door_params[i];
            uint16_t ziel = (uint16_t)((((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4));
            if (ziel == raum || (ziel >> 12) != 1) continue;
            int dup = 0;
            for (int k = 0; k < s_nein; k++)
                if (s_ein[k].ziel == ziel && s_ein[k].x == d->spawn_x && s_ein[k].z == d->spawn_z) dup = 1;
            if (dup || s_nein >= 400) continue;
            eintritt_t *e = &s_ein[s_nein++];
            e->von = raum; e->ziel = ziel; e->cut = d->target_cut;
            e->x = d->spawn_x; e->y = d->spawn_y; e->z = d->spawn_z; e->rot = d->spawn_yaw_4096;
        }
    }
}

/* ---------------- ein Lauf ---------------- */
typedef struct {
    int slot; unsigned typ; uint8_t grid; int st0, sub0;
    int32_t x0, z0;
    int reakt;              /* Bild der ersten Reaktion, 0 = keine */
    int32_t dmin;           /* kleinster Abstand zu Leon */
    int kegel_nah;          /* Bilder mit Leon < 5000 im Kegel +-1024 */
    int still_max;          /* laengste Strecke ohne Bewegung (Bilder), Leon < 5000, kein Griff */
    int32_t still_x, still_z, still_d; int still_ab;   /* wo/wann die laengste Strecke begann */
    int griff;              /* Bild des ersten Griffs */
    int los_end, sub_end, st_end;
    int32_t x_end, z_end;
    int klasse;             /* 0 ok, 1 PASSIV, 2 KLEMME, 3 liegt (Skript-Wecker) */
} zstat_t;

static const char *k_klasse[] = { "ok", "PASSIV", "KLEMME", "liegt" };

/* modus 0 = steht, 1 = hin zu Slot ziel_slot (bis 900) */
static int lauf(const eintritt_t *ein, int modus, int ziel_slot, int bilder, zstat_t *zs, int *nz)
{
    if (!raum_laden(ein->ziel)) return 0;
    if (!eintreten_an(ein->ziel, ein->cut, ein->x, ein->y, ein->z, ein->rot)) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    *nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || !zombie_typ(e->type)) continue;
        zstat_t *z = &zs[(*nz)++]; memset(z, 0, sizeof *z);
        z->slot = s; z->typ = e->type; z->grid = e->grid_id; z->st0 = e->state; z->sub0 = e->sub_state_1;
        z->x0 = e->x; z->z0 = e->z; z->dmin = 1 << 30;
    }
    if (modus == 1) {
        int ok = 0; for (int k = 0; k < *nz; k++) if (zs[k].slot == ziel_slot) ok = 1;
        if (!ok) return 0;
    }
    int32_t ax[RE15_ACTOR_MAX], az[RE15_ACTOR_MAX]; int still[RE15_ACTOR_MAX];
    for (int k = 0; k < *nz; k++) { ax[k] = zs[k].x0; az[k] = zs[k].z0; still[k] = 0; }
    for (int fr = 1; fr <= bilder; fr++) {
        pl->hp = 100;
        s_pad = 0;
        if (modus == 1) {
            re15_actor_t *t = &g_actors[ziel_slot];
            if (t->active && t->state == 1 && !re15_player_is_grabbed()) {
                int32_t d = abst(pl, t);
                pl->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - pl->z, t->x - pl->x) - 0x400) & 0xfff);
                if (d > 900) s_pad = RE15_PAD_BIT_UP;
            }
        }
        frame();
        int grab = re15_player_is_grabbed();
        if (s_spur_slot > 0 && (fr % s_spur_takt) == 0) {
            re15_actor_t *e = &g_actors[s_spur_slot];
            printf("  F%-4d Z(%6d,%6d) st%d/%d/%d clip%-3d af%-3d rot%4d | L(%6d,%6d) d%5d grab%d | los%d steer(%6d,%6d) wand%02x hp%d\n",
                   fr, (int)e->x, (int)e->z, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame, (int)e->rot_y,
                   (int)pl->x, (int)pl->z, (int)abst(pl, e), grab, e->re2z_los154, (int)e->steer_x, (int)e->steer_z, e->sca_wall_hit, (int)e->hp);
        }
        for (int k = 0; k < *nz; k++) {
            zstat_t *z = &zs[k]; re15_actor_t *e = &g_actors[z->slot];
            if (!e->active) continue;
            int32_t d = abst(pl, e);
            if (d < z->dmin) z->dmin = d;
            if (fr == 1) { z->st0 = e->state; z->sub0 = e->sub_state_1; }   /* Basis = Zustand NACH dem INIT (Bild 1) */
            else if (!z->reakt && (e->state != z->st0 || e->sub_state_1 != z->sub0)) z->reakt = fr;
            if (!z->griff && grab && d < 1500) z->griff = fr;
            if (!z->reakt && d < 5000 && re15_ai_arc_test(e, pl->x, pl->z, 1024) == 0) z->kegel_nah++;
            if ((fr % 30) == 0) {
                long long dx = e->x - ax[k], dz = e->z - az[k];
                if (dx * dx + dz * dz < 40LL * 40LL && d < 5000 && !grab && e->state == 1) {
                    if (still[k] == 0) { z->still_x = e->x; z->still_z = e->z; z->still_d = d; z->still_ab = fr - 30; }
                    still[k] += 30;
                    if (still[k] > z->still_max) z->still_max = still[k];
                } else still[k] = 0;
                ax[k] = e->x; az[k] = e->z;
            }
        }
    }
    for (int k = 0; k < *nz; k++) {
        zstat_t *z = &zs[k]; re15_actor_t *e = &g_actors[z->slot];
        z->los_end = e->re2z_los154; z->sub_end = e->sub_state_1; z->st_end = e->state; z->x_end = e->x; z->z_end = e->z;
        if (z->st0 == 1 && (z->sub0 == 7 || z->sub0 == 8) && !z->reakt) z->klasse = 3;   /* Liegender: weckt nur das Skript (re2z_exec_lying) */
        else if (!z->reakt && z->kegel_nah >= 120) z->klasse = 1;
        else if (z->reakt && z->still_max >= 300 && z->still_d > 1500) z->klasse = 2;   /* auch nach einem frueheren Griff (Taschen-Fall) */
    }
    return 1;
}

int main(int argc, char **argv)
{
    if (argc > 6 && strcmp(argv[1], "spur") == 0) {   /* spur <ziel-hex> <von-hex> <cut> <slot> <0 steht|1 hin> [bilder] */
        unsigned ziel = (unsigned)strtoul(argv[2], NULL, 16), von = (unsigned)strtoul(argv[3], NULL, 16);
        int cut = atoi(argv[4]), slot = atoi(argv[5]), modus = atoi(argv[6]), b = (argc > 7) ? atoi(argv[7]) : 900;
        if (argc > 8) s_spur_takt = atoi(argv[8]) > 0 ? atoi(argv[8]) : 30;
        eintritte_sammeln();
        static zstat_t zz[RE15_ACTOR_MAX]; int nzz = 0;
        for (int i = 0; i < s_nein; i++) if (s_ein[i].ziel == ziel && s_ein[i].von == von && s_ein[i].cut == cut) {
            s_spur_slot = slot;
            printf("Eintritt %04X<-%04X cut%d Leon(%d,%d) rot%d\n", ziel, von, s_ein[i].cut, (int)s_ein[i].x, (int)s_ein[i].z, (int)s_ein[i].rot);
            lauf(&s_ein[i], modus, slot, b, zz, &nzz);
            break;
        }
        return 0;
    }
    if (argc > 2 && strcmp(argv[1], "aot") == 0) {   /* aot <raum-hex>: AOT-Saetze des Raums (Typ, Aot_set pc[3] = Pool-Maske & 7) */
        uint16_t raum = (uint16_t)strtoul(argv[2], NULL, 16);
        if (!raum_laden(raum) || !eintreten_an(raum, 0, 0, 0, 0, 0)) return 1;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active) continue;
            printf("aot%-2d typ%d sce_flags=%02x (Pool-Maske %d) band=%d x=%d z=%d hw=%d hh=%d\n", i, a->type, a->sce_flags,
                   a->sce_flags & 7, a->band, (int)a->x, (int)a->z, (int)a->half_w, (int)a->half_h);
        }
        return 0;
    }
    if (argc > 7 && strcmp(argv[1], "platz") == 0) {   /* platz <raum-hex> <cut> <x> <z> <slot> <bilder> [takt] [0=Haken aus] */
        eintritt_t ein; memset(&ein, 0, sizeof ein);
        ein.ziel = (uint16_t)strtoul(argv[2], NULL, 16); ein.cut = atoi(argv[3]);
        ein.x = atoi(argv[4]); ein.z = atoi(argv[5]); ein.y = 0; ein.rot = 0;
        int slot = atoi(argv[6]), b = atoi(argv[7]);
        if (argc > 8) s_spur_takt = atoi(argv[8]) > 0 ? atoi(argv[8]) : 30;
        if (argc > 9 && atoi(argv[9]) == 0) re15_r36_nav_zonenlos_enable(0);
        s_spur_slot = slot;
        static zstat_t zz[RE15_ACTOR_MAX]; int nzz = 0;
        lauf(&ein, 0, slot, b, zz, &nzz);
        for (int k = 0; k < nzz; k++)
            printf("slot%d reakt %d dmin %d still %d ab F%d d%d Ende st%d/%d (%d,%d) los%d\n", zz[k].slot, zz[k].reakt, (int)zz[k].dmin,
                   zz[k].still_max, zz[k].still_ab, (int)zz[k].still_d, zz[k].st_end, zz[k].sub_end, (int)zz[k].x_end, (int)zz[k].z_end, zz[k].los_end);
        return 0;
    }
    const int mess = (argc > 1 && strcmp(argv[1], "mess") == 0);
    unsigned nur = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0u;
    const int bilder = (argc > 3) ? atoi(argv[3]) : 900;
    eintritte_sammeln();
    printf("%d Eintritte (Door_aot_set, Ziel Stage 1)\n", s_nein);
    int passiv = 0, klemme = 0, n_faelle = 0;
    static zstat_t zs[RE15_ACTOR_MAX];
    for (int i = 0; i < s_nein; i++) {
        const eintritt_t *ein = &s_ein[i];
        if (nur && ein->ziel != nur) continue;
        int nz = 0;
        if (!lauf(ein, 0, 0, bilder, zs, &nz) || nz == 0) continue;
        int slots[RE15_ACTOR_MAX]; int nslots = nz;
        for (int k = 0; k < nz; k++) slots[k] = zs[k].slot;
        for (int m = 0; m <= nslots; m++) {
            int modus = (m == 0) ? 0 : 1;
            int ziel = (m == 0) ? 0 : slots[m - 1];
            if (modus == 1 && !lauf(ein, 1, ziel, bilder, zs, &nz)) continue;
            if (modus == 0) { /* schon gelaufen */ }
            for (int k = 0; k < nz; k++) {
                zstat_t *z = &zs[k];
                if (modus == 1 && z->slot != ziel) continue;        /* "hin": nur der angesteuerte Zombie zaehlt */
                n_faelle++;
                if (z->klasse == 1) passiv++;
                if (z->klasse == 2) klemme++;
                if ((z->klasse == 1 || z->klasse == 2) || mess)
                    printf("%04X<-%04X cut%d Leon(%6d,%6d) %s slot%-2d typ%02X grid%02X st%d/%d (%6d,%6d) | reakt %4d dmin %5d kegel %4d griff %4d "
                           "still %4d ab F%-4d @(%6d,%6d) d%5d | Ende st%d/%d (%6d,%6d) los%d  %s\n",
                           ein->ziel, ein->von, ein->cut, (int)ein->x, (int)ein->z, modus ? "hin  " : "steht", z->slot, z->typ, z->grid,
                           z->st0, z->sub0, (int)z->x0, (int)z->z0, z->reakt, (int)z->dmin, z->kegel_nah, z->griff,
                           z->still_max, z->still_ab, (int)z->still_x, (int)z->still_z, (int)z->still_d,
                           z->st_end, z->sub_end, (int)z->x_end, (int)z->z_end, z->los_end, k_klasse[z->klasse]);
            }
        }
    }
    printf("\n%d Faelle (Eintritt x Zombie x Szenario): PASSIV %d, KLEMME %d\n", n_faelle, passiv, klemme);
    if (mess) return 0;
    CHECK(n_faelle > 40, "Zensus deckt %d Faelle", n_faelle);
    CHECK(passiv == 0, "kein Zombie bleibt PASSIV (nie reagiert, Leon >= 120 Bilder < 5000 im Kegel): %d", passiv);
    CHECK(klemme == 0, "kein Zombie tritt nach der Reaktion >= 300 Bilder auf der Stelle (Abstand > 1500, kein Griff): %d", klemme);
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
