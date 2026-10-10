/*
 * Runde 36 Spur F "zombieki", Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir
 * irgendwie verbessern." Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 2.
 *
 * MESS-SONDE (echte Spielschleife, ROOM11C0, beide Gorillas in der Jagd ab der Original-Lage F195 wie
 * test_r35_affen takt/schrot): Leon laeuft per deterministischem Zufallsgang (alle 90 Bilder neue
 * Blickrichtung, Rennen = UP+CROSS, HP wird je Bild auf 100 gehalten, damit die Jagd nicht endet) N Bilder
 * durch den Parkplatz. Gemessen wird je Gorilla jede Strecke, in der er LEBT, nicht beisst/greift und sich
 * >= 90 Bilder lang um weniger als 40 Einheiten bewegt ("blockiert"): Lage, Abstand zu Leon, Sub-Zustaende,
 * Sicht-Latch (+0x1d0 & 1), Wandklemme (+0x1d6), Zonensprung-Zaehler, naechste SCA-Zellen und ob Leon fuer
 * ihn sichtbar ist (Zellstrahl). Aufruf: test_r36_zombieki_affen_block [bilder] [seed]
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
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_zombieki_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define popen  _popen    /* Riegel startet die Sonde je Lauf als eigenen Prozess (Modus `eins`) */
#define pclose _pclose
#endif

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static uint8_t          *s_raw = NULL; static size_t s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static uint8_t            s_blob27[600000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int bank_laden_27(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x27);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x27);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob27);
    if (ok) { memcpy(s_blob27, ems + off, len); if (!eb) eb = re15_enemy_alloc(0x27); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob27, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob27, len, &eb->skel_victim, &eb->anim_victim) == 0); }
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int bilder)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

static int32_t isqrt64(long long v) { long long r = 0; while ((r + 1) * (r + 1) <= v) r++; return (int32_t)r; }

static void zellen_nahe(const re15_actor_t *e, char *out, size_t n)
{
    out[0] = 0;
    int band = (int)e->floor;
    int32_t r = (int32_t)e->hit_radius_min + 60;
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        if (band != (c->floor >> 4)) continue;
        if ((unsigned)(e->x - ((int32_t)c->x - r)) < (unsigned)((int32_t)c->width + r * 2) &&
            (unsigned)(e->z - ((int32_t)c->z - r)) < (unsigned)((int32_t)c->density + r * 2)) {
            size_t len = strlen(out);
            if (len + 60 < n)
                snprintf(out + len, n - len, "%s#%d t%d u0=%02x u1=%02x (%d,%d %dx%d)", len ? " " : "", i, c->type,
                         c->u0, c->u1, (int)c->x, (int)c->z, (int)c->width, (int)c->density);
        }
    }
}

typedef struct { int f0, f1; int32_t x, z; int dist; int subs[32]; int los1; int klemme; int spruenge; int leon_sicht; char zellen[300]; } seg_t;

/* Raum + Baenke + beide Gorillas in der Original-Lage F195 (test_r35_affen takt): beide in der Jagd (Sub 3). */
static int aufbau(re15_actor_t **gout, re15_actor_t **plout)
{
    re15_game_state_init();
    re15_damage_seed_rng(0x2545f491u);   /* NB1: jeder Lauf mit demselben Engine-Zufall (FUN_8001af20-Zwilling) — Laeufe unabhaengig */
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) { printf("ROOM11C0 nicht ladbar\n"); return 0; }
    if (!bank_laden_27()) { printf("EM027-Bank fehlt\n"); return 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *g[2] = { aktor_vom_typ(0x27, 0), aktor_vom_typ(0x27, 1) };
    if (!g[0] || !g[1]) { printf("zwei Gorillas fehlen\n"); return 0; }
    g[0]->x = -5525; g[0]->z = -14883; g[0]->y = 0; g[0]->rot_y = 2731; g[0]->grid_id = 0x10; g[0]->floor = 0;
    g[0]->state = 1; g[0]->sub_state_1 = 3; g[0]->sub_state_2 = 1; g[0]->sub_state_3 = 0; g[0]->motion = 5; g[0]->anim_frame = 10;
    g[0]->anim_frac = 0; g[0]->dog_blocked_ctr = 9; g[0]->hit_react = 0; g[0]->dog_flags = 1; g[0]->mag_boost = 4;
    g[1]->x = -8915; g[1]->z = -12487; g[1]->y = 0; g[1]->rot_y = 93; g[1]->grid_id = 0x10; g[1]->floor = 0;
    g[1]->state = 1; g[1]->sub_state_1 = 3; g[1]->sub_state_2 = 1; g[1]->sub_state_3 = 0; g[1]->motion = 5; g[1]->anim_frame = 16;
    g[1]->anim_frac = 0; g[1]->dog_blocked_ctr = 15; g[1]->hit_react = 0; g[1]->dog_flags = 1; g[1]->mag_boost = 4;
    pl->hp = 100; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    gout[0] = g[0]; gout[1] = g[1]; *plout = pl;
    return 1;
}

/* (NB1-Szene mit geparktem Gorilla 2 entfernt — NB2 Mangel 4: szene2 unten fuehrt BEIDE Gorillas.) */

static int s_zg_anker = 400; /* NB2 (Abnahme 1, Mangel 4): Anker-Radius der Blockade (Manhattan); 40 zaehlte Spruenge/Pendeln
                              * auf der Stelle (ein Sprung versetzt um ~100) nicht als blockiert. `eins ... [anker]` */
/* NB2 (Abnahme 1, Maengel 3/4): Szene mit BEIDEN Gorillas (keiner geparkt). Leon fest auf (lx,lz) mit Blick auf Gorilla 1;
 * gx/gz != 0: Gorilla 1 dorthin in die BLINDE Jagd (+0x7 = 1, Latch 0); Gorilla 2 bleibt in der Original-Lage F195, ausser
 * Gorilla 1 steht dort (dann in dessen Lage). Je Gorilla: erster Angriff (Sub 5/6/8/15), kleinster Abstand, Spruenge, und
 * blockierte Bilder mit derselben Definition wie der Zufallsgang (lebt, kein Angriff, >= 90 Bilder im Anker 400). */
static int s_sz2_spur = 0;   /* Sonde szene2 ... 1: Spur je 60 Bilder */
typedef struct { int erst[2], dmin[2], spr[2], blind[2], pad36[2]; long blockiert[2], nach[2], nach_fern[2]; } szene2_t;
    /* NB3: nach = blockiert NACH dem ersten Angriff, nach_fern = davon die Bilder mit Leon >= 0x1771 (@0x80117bc8: der Bereich des Umwegs;
     * darunter Original-Nahjagd, Spur-J-Riegel griff/szene), pad36 = Umweg hat Pad #36 (Ost) gewaehlt */
static szene2_t szene2(int32_t lx, int32_t lz, int32_t gx, int32_t gz, int bilder)
{
    szene2_t r; memset(&r, 0, sizeof r); r.dmin[0] = r.dmin[1] = 1 << 30;
    re15_actor_t *g[2]; re15_actor_t *pl;
    if (!aufbau(g, &pl)) return r;
    pl->x = lx; pl->z = lz; pl->y = 0; pl->floor = 0;
    if (gx || gz) {
        if (labs(gx - g[1]->x) + labs(gz - g[1]->z) < 3200) { g[1]->x = g[0]->x; g[1]->z = g[0]->z; }
        g[0]->x = gx; g[0]->z = gz;
        g[0]->dog_flags = 0; g[0]->sub_state_3 = 1; g[0]->motion = 4;
    }
    pl->rot_y = (int16_t)((((int)re15_atan2_q12(g[0]->z - pl->z, g[0]->x - pl->x) - 0x400)) & 0xfff);
    int prev7[2] = { 0, 0 }, fa[2] = { 0, 0 }; long akk_fern[2] = { 0, 0 }; int32_t ax[2] = { g[0]->x, g[1]->x }, az[2] = { g[0]->z, g[1]->z };
    for (int f = 1; f <= bilder; f++) {
        frame(0, 0);
        if (pl->hp < 100) pl->hp = 100;
        for (int k = 0; k < 2; k++) {
            re15_actor_t *e = g[k]; int sub = e->sub_state_1 & 31;
            long long dx = e->x - pl->x, dz = e->z - pl->z; int d = isqrt64(dx * dx + dz * dz);
            if (d < r.dmin[k]) r.dmin[k] = d;
            int kampf = (sub == 5 || sub == 6 || sub == 8 || sub == 15);
            if (!r.erst[k] && kampf) r.erst[k] = f;
            if (sub == 7 && !prev7[k]) { r.spr[k]++; if (e->sub_state_3 == 3) r.blind[k]++; }
            prev7[k] = (sub == 7);
            if (s_sz2_spur && (f % (s_sz2_spur > 1 ? s_sz2_spur : 60)) == 0) { int32_t ux = 0, uz = 0; int up = -1; int ua = re15_r36_affen_umweg_lesen((int)(e - g_actors), &ux, &uz, &up);
                printf("  F%d G%d st=%d/%d/%d/%d @(%d,%d) rot=%d los=%d klemme=%d d=%d | Umweg %d (%d,%d) Pad %d | Leon (%d,%d)\n", f, k + 1, e->state, e->sub_state_1,
                       e->sub_state_2, e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y, e->dog_flags & 1, e->sca_wall_hit != 0, d, ua, (int)ux, (int)uz, up,
                       (int)pl->x, (int)pl->z); }
            { int32_t ux = 0, uz = 0; int up = -1; if (re15_r36_affen_umweg_lesen((int)(e - g_actors), &ux, &uz, &up) && up == 36) r.pad36[k] = 1; }
            int bewegt = (labs(e->x - ax[k]) + labs(e->z - az[k])) >= s_zg_anker;
            if (e->state == 1 && !kampf && !bewegt) { if (!fa[k]) { fa[k] = f; akk_fern[k] = 0; } if (d >= 0x1771) akk_fern[k]++; }
            else { if (fa[k] && f - fa[k] >= 90) { r.blockiert[k] += f - fa[k]; if (r.erst[k] && fa[k] > r.erst[k]) { r.nach[k] += f - fa[k]; r.nach_fern[k] += akk_fern[k]; } }
                   fa[k] = 0; ax[k] = e->x; az[k] = e->z; }
        }
    }
    for (int k = 0; k < 2; k++) if (fa[k] && bilder + 1 - fa[k] >= 90) { r.blockiert[k] += bilder + 1 - fa[k];
        if (r.erst[k] && fa[k] > r.erst[k]) { r.nach[k] += bilder + 1 - fa[k]; r.nach_fern[k] += akk_fern[k]; } }
    return r;
}

/* ---- Nachbesserung 1 (Abnahme 0, Maengel 5/6): ZUFALLSGANG-STATISTIK ueber viele Seeds -------------------------
 * Derselbe Zufallsgang wie unten (alle 90 Bilder neue Blickrichtung, Rennen = UP+CROSS, HP je Bild 100, im Griff
 * abwechselnd Quadrat/Kreuz), dieselbe Blockade-Definition (Gorilla lebt, beisst/greift nicht, bewegt sich >= 90
 * Bilder < 40 Einheiten ab Streckenbeginn), aber nur Summen: blockierte Bilder, Griffe, Sub-Histogramm der
 * blockierten Bilder. Der Riegel vergleicht damit Port-Wahl AN gegen AUS auf demselben Seed-Satz. */
typedef struct { long blockiert; int griffe; long sub[32]; long nah, fern, los, klemme; } zg_t;
static int s_zg_lage = 0;   /* 1: je blockierte Strecke eine LAGE-Zeile (Sonde stat ... 3) */
static zg_t zg_stat(int bilder, uint32_t seed)
{
    zg_t r; memset(&r, 0, sizeof r);
    re15_actor_t *g[2]; re15_actor_t *pl;
    if (!aufbau(g, &pl)) { r.blockiert = -1; return r; }
    int32_t ax[2], az[2]; int fa[2] = { 0, 0 }; long subs_akk[2][32]; long akk_nah[2] = { 0, 0 }, akk_los[2] = { 0, 0 }, akk_kl[2] = { 0, 0 };
    for (int k = 0; k < 2; k++) { ax[k] = g[k]->x; az[k] = g[k]->z; memset(subs_akk[k], 0, sizeof subs_akk[k]); }
    int ziel_rot = pl->rot_y, prev_gr = 0; uint32_t rng = seed;
    for (int f = 1; f <= bilder; f++) {
        if ((f % 90) == 1) { rng = rng * 1103515245u + 12345u; ziel_rot = (int)((rng >> 8) & 0xfff); }
        uint16_t held = 0;
        int err = ((ziel_rot - (int)(pl->rot_y & 0xfff) + 2048 + 4096) % 4096) - 2048;
        if (err > 48)       held = RE15_PAD_BIT_RIGHT;
        else if (err < -48) held = RE15_PAD_BIT_LEFT;
        else                held = RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS;
        int gr = re15_player_is_grabbed();
        if (gr) held = (uint16_t)((f & 1) ? RE15_PAD_BIT_SQUARE : RE15_PAD_BIT_CROSS);
        frame(held, (uint16_t)(gr ? held : 0));
        if (pl->hp < 100) pl->hp = 100;
        if (gr && !prev_gr) r.griffe++;
        prev_gr = gr;
        for (int k = 0; k < 2; k++) {
            re15_actor_t *e = g[k];
            int sub = e->sub_state_1 & 31;
            int lebt = (e->state == 1);
            int kampf = (sub == 5 || sub == 6 || sub == 8 || sub == 15);
            int bewegt = (labs(e->x - ax[k]) + labs(e->z - az[k])) >= s_zg_anker;
            if (lebt && !kampf && !bewegt) { subs_akk[k][sub]++; if (fa[k] == 0) fa[k] = f;
                long long ddx = e->x - pl->x, ddz = e->z - pl->z;
                akk_nah[k] += (ddx * ddx + ddz * ddz < 6001LL * 6001LL); akk_los[k] += (e->dog_flags & 1); akk_kl[k] += (e->sca_wall_hit != 0); }
            else {
                int dauer = fa[k] ? f - fa[k] : 0;
                if (dauer >= 90 && s_zg_lage) printf("LAGE %08x g%d %d %d leon %d %d dauer %d los %ld ab F%d sub3 %ld sub4 %ld sub7 %ld\n", seed, k, (int)ax[k], (int)az[k],
                                                     (int)pl->x, (int)pl->z, dauer, akk_los[k], fa[k], subs_akk[k][3], subs_akk[k][4], subs_akk[k][7]);
                if (dauer >= 90) { r.blockiert += dauer; for (int q = 0; q < 32; q++) r.sub[q] += subs_akk[k][q];
                                   r.nah += akk_nah[k]; r.fern += dauer - akk_nah[k]; r.los += akk_los[k]; r.klemme += akk_kl[k]; }
                fa[k] = 0; memset(subs_akk[k], 0, sizeof subs_akk[k]); akk_nah[k] = akk_los[k] = akk_kl[k] = 0;
                ax[k] = e->x; az[k] = e->z;
            }
        }
    }
    for (int k = 0; k < 2; k++) {                                   /* offene Strecke am Laufende mitzaehlen */
        int dauer = fa[k] ? bilder + 1 - fa[k] : 0;
        if (dauer >= 90) { r.blockiert += dauer; for (int q = 0; q < 32; q++) r.sub[q] += subs_akk[k][q];
                           r.nah += akk_nah[k]; r.fern += dauer - akk_nah[k]; r.los += akk_los[k]; r.klemme += akk_kl[k]; }
    }
    return r;
}

/* Seed-Satz des Riegels: 0x1234567, 0xbeef und 2..(n-1) aus einem festen LCG (reproduzierbar). */
static uint32_t zg_seed(int i)
{
    if (i == 0) return 0x1234567u;
    if (i == 1) return 0xbeefu;
    uint32_t s = 0x5eed0000u + (uint32_t)i * 2654435761u;
    return s;
}

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

int main(int argc, char **argv)
{
    /* "fest <x> <z> [bilder]": Leon steht still auf (x,z) und blickt auf Gorilla 1 — Spur je 30 Bilder + Summe
     * (Abstand min/Ende, erster Angriff, Anteile Sub-Zustaende, Klemme, Sicht, Zonenspruenge). */
    if (argc > 1 && strcmp(argv[1], "raster") == 0) {   /* raster [schritt] [radius]: Begehbarkeit fuer den Gorilla (echte Klemme, Band 0, Maske 4) */
        int st = (argc > 2) ? atoi(argv[2]) : 800, rad = (argc > 3) ? atoi(argv[3]) : 1600;
        int dlt = (argc > 4) ? atoi(argv[4]) : 0;   /* NB3: Vorlage = Knoten + (dlt,0) — 1 deckt den Zurueckschnapp-Artefakt auf */
        re15_actor_t *gg[2]; re15_actor_t *pp;
        if (!aufbau(gg, &pp)) return 1;
        for (int z = 26000; z >= -31000; z -= st) {
            printf("%6d ", z);
            for (int x = -33000; x <= 22000; x += st) {
                int32_t nx = x, nz = z; uint8_t ct = 0; uint16_t at = 0;
                re15_collision_constrain_contact_band(&g_room_rdt, x + dlt, z, &nx, &nz, rad, 0, 4u, &ct, &at);
                uint8_t zc = 0;
                char ch = (nx != x || nz != z) ? '#' : '.';
                if (re15_collision_zone_query(&g_room_rdt, x, z, 0, 0, 0x10u, &zc)) ch = (ch == '#') ? 'P' : 'p';
                else if (re15_collision_zone_query(&g_room_rdt, x, z, 0, 0, 0x20u, &zc)) ch = (ch == '#') ? 'Q' : 'q';
                putchar(ch);
            }
            putchar('\n');
        }
        printf("       x von -33000 bis 22000, Schritt %d, Radius %d; # = Klemme verschiebt, p/P = 0x10-Pad frei/geklemmt, q/Q = 0x20-Pad\n", st, rad);
        return 0;
    }
    if (argc > 3 && strcmp(argv[1], "szene2") == 0) {   /* NB2 Sonde: szene2 <lx> <lz> [gx gz] [bilder] [0 = Umweg aus] */
        int32_t lx = atoi(argv[2]), lz = atoi(argv[3]), gx = (argc > 5) ? atoi(argv[4]) : 0, gz = (argc > 5) ? atoi(argv[5]) : 0;
        int b = (argc > 6) ? atoi(argv[6]) : 2400;
        if (argc > 7 && atoi(argv[7]) == 0) re15_r36_affen_padsuche_enable(0);
        if (argc > 8) s_sz2_spur = atoi(argv[8]);
        szene2_t r = szene2(lx, lz, gx, gz, b);
        for (int k = 0; k < 2; k++)
            printf("SZENE2 Leon (%d,%d) G%d: erster Angriff %d, Abstand min %d, Spruenge %d (blind %d), blockiert %ld (Anker %d) nach %ld fern %ld pad36 %d\n",
                   (int)lx, (int)lz, k + 1, r.erst[k], r.dmin[k], r.spr[k], r.blind[k], r.blockiert[k], s_zg_anker, r.nach[k], r.nach_fern[k], r.pad36[k]);
        return 0;
    }
    if (argc > 3 && strcmp(argv[1], "klemme") == 0) {   /* NB3 Sonde: klemme <x> <z> [radius] [maske] [ox oz]: EIN Aufruf der Original-
                                                          * Klemme (FUN_8003b0a4-Zwilling) Band 0, mit COLL_TRACE-artiger Ausgabe */
        re15_actor_t *gg[2]; re15_actor_t *pp;
        if (!aufbau(gg, &pp)) return 1;
        int32_t x = atoi(argv[2]), z = atoi(argv[3]), r = (argc > 4) ? atoi(argv[4]) : 1600; uint32_t mk = (argc > 5) ? (uint32_t)atoi(argv[5]) : 4u;
        int32_t ox = (argc > 7) ? atoi(argv[6]) : x, oz = (argc > 7) ? atoi(argv[7]) : z;
        int32_t nx = x, nz = z; uint8_t ct = 0; uint16_t at = 0;
        int hit = re15_collision_constrain_contact_band(&g_room_rdt, ox, oz, &nx, &nz, r, 0, mk, &ct, &at);
        printf("KLEMME (%d,%d) alt (%d,%d) r %d maske %u -> (%d,%d) kontakt %d code %02x attr %04x\n", (int)x, (int)z, (int)ox, (int)oz, (int)r, (unsigned)mk,
               (int)nx, (int)nz, hit, ct, at);
        return 0;
    }
    if (argc > 5 && strcmp(argv[1], "leonlauf") == 0) {   /* NB3 Sonde: leonlauf <sx> <sz> <tx> <tz> [bilder]: Leon (Gorillas aus)
                                                            * rennt von (sx,sz) auf (tx,tz) zu (UP+CROSS, Drehen wie der Zufallsgang)
                                                            * — erreicht er die Stelle zu Fuss? (echte Spieler-Klemme) */
        re15_actor_t *gg[2]; re15_actor_t *pp;
        if (!aufbau(gg, &pp)) return 1;
        gg[0]->active = 0; gg[1]->active = 0;
        int b = (argc > 6) ? atoi(argv[6]) : 600;
        int32_t tx = atoi(argv[4]), tz = atoi(argv[5]);
        pp->x = atoi(argv[2]); pp->z = atoi(argv[3]); pp->y = 0; pp->floor = 0;
        long long best = -1; int32_t bx = pp->x, bz = pp->z;
        for (int f = 1; f <= b; f++) {
            int ziel = (int)((((int)re15_atan2_q12(tz - pp->z, tx - pp->x)) - 0x400) & 0xfff);
            int err = ((ziel - (int)(pp->rot_y & 0xfff) + 2048 + 4096) % 4096) - 2048;
            uint16_t held = (err > 48) ? RE15_PAD_BIT_RIGHT : (err < -48) ? RE15_PAD_BIT_LEFT : (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS);
            frame(held, 0);
            long long dx = pp->x - tx, dz = pp->z - tz, d = dx * dx + dz * dz;
            if (best < 0 || d < best) { best = d; bx = pp->x; bz = pp->z; }
            if ((f % 60) == 0) printf("  F%d Leon (%d,%d) rot=%d\n", f, (int)pp->x, (int)pp->z, (int)pp->rot_y);
        }
        printf("LEONLAUF (%s,%s) -> (%d,%d): naechster Punkt (%d,%d) Abstand %d, Ende (%d,%d)\n", argv[2], argv[3], (int)tx, (int)tz,
               (int)bx, (int)bz, (int)isqrt64(best), (int)pp->x, (int)pp->z);
        return 0;
    }
    if (argc > 5 && strcmp(argv[1], "weit") == 0) {   /* NB2 Sonde: weit <gx> <gz> <lx> <lz> [bilder]: Gorilla 1 auf (gx,gz) mit Blick
                                                        * auf Leon, Path-B-Weitsprung (+0x5 = 7, +0x7 = 0 @0x801180cc-f8) erzwungen */
        re15_actor_t *gg[2]; re15_actor_t *pp;
        if (!aufbau(gg, &pp)) return 1;
        int b = (argc > 6) ? atoi(argv[6]) : 300;
        pp->x = atoi(argv[4]); pp->z = atoi(argv[5]); pp->y = 0; pp->floor = 0;
        gg[1]->x = 20000; gg[1]->z = -20000;
        gg[0]->x = atoi(argv[2]); gg[0]->z = atoi(argv[3]); gg[0]->dog_flags = 1;
        gg[0]->rot_y = (int16_t)((((int)re15_atan2_q12(pp->z - gg[0]->z, pp->x - gg[0]->x)) - 0x400) & 0xfff);
        gg[0]->sub_state_1 = 7; gg[0]->sub_state_2 = 0; gg[0]->sub_state_3 = 0;
        for (int f = 1; f <= b; f++) {
            frame(0, 0); if (pp->hp < 100) pp->hp = 100;
            re15_actor_t *e = gg[0];
            if ((f % 10) == 0 || f < 4)
                printf("  F%d G1 st=%d/%d/%d/%d @(%d,%d) y=%d floor=%d rot=%d los=%d klemme=%d dist=%d\n", f, e->state, e->sub_state_1, e->sub_state_2,
                       e->sub_state_3, (int)e->x, (int)e->z, (int)e->y, (int)e->floor, (int)e->rot_y, e->dog_flags & 1, e->sca_wall_hit != 0,
                       (int)isqrt64((long long)(e->x - pp->x) * (e->x - pp->x) + (long long)(e->z - pp->z) * (e->z - pp->z)));
        }
        return 0;
    }
    if (argc > 4 && strcmp(argv[1], "eins") == 0) {   /* eins <seed-index> <0 AUS|1 AN> <bilder>: EIN Lauf je Prozess (Modulzustand
                                                         * bleibt sonst zwischen Laeufen haengen — gemessen: stat-AUS 73006 allein
                                                         * gegen 61502 im Wechsel mit AN) */
        int i = atoi(argv[2]), modus = atoi(argv[3]), b = atoi(argv[4]);
        if (argc > 5) s_zg_anker = atoi(argv[5]);
        if (argc > 6) s_zg_lage = atoi(argv[6]);   /* NB3: LAGE-Zeile je blockierter Strecke */
        re15_r36_affen_padsuche_enable(modus ? 1 : 0);
        zg_t r = zg_stat(b, zg_seed(i));
        printf("EINS seed 0x%08x modus %d blockiert %ld griffe %d nah %ld fern %ld los %ld klemme %ld sub3 %ld sub4 %ld sub7 %ld\n",
               zg_seed(i), modus, r.blockiert, r.griffe, r.nah, r.fern, r.los, r.klemme, r.sub[3], r.sub[4], r.sub[7]);
        return 0;
    }
    if (argc > 3 && strcmp(argv[1], "stat") == 0) {   /* stat <seeds> <bilder> [0|1|2 = nur AUS | nur AN | beide] */
        int n = atoi(argv[2]), b = atoi(argv[3]), modus = (argc > 4) ? atoi(argv[4]) : 2;
        if (argc > 5) s_zg_lage = atoi(argv[5]);
        long su_an = 0, su_aus = 0, sub_an[32], sub_aus[32]; int gr_an = 0, gr_aus = 0, besser = 0, schlechter = 0, gleich = 0;
        long q_an[4] = { 0, 0, 0, 0 }, q_aus[4] = { 0, 0, 0, 0 };   /* nah, fern, Sicht, Klemme (blockierte Bilder) */
        memset(sub_an, 0, sizeof sub_an); memset(sub_aus, 0, sizeof sub_aus);
        for (int i = 0; i < n; i++) {
            zg_t aus = { 0 }, an = { 0 };
            if (modus != 1) { re15_r36_affen_padsuche_enable(0); aus = zg_stat(b, zg_seed(i)); }
            if (modus != 0) { re15_r36_affen_padsuche_enable(1); an = zg_stat(b, zg_seed(i)); }
            re15_r36_affen_padsuche_enable(1);
            su_an += an.blockiert; su_aus += aus.blockiert; gr_an += an.griffe; gr_aus += aus.griffe;
            for (int q = 0; q < 32; q++) { sub_an[q] += an.sub[q]; sub_aus[q] += aus.sub[q]; }
            q_an[0] += an.nah; q_an[1] += an.fern; q_an[2] += an.los; q_an[3] += an.klemme;
            q_aus[0] += aus.nah; q_aus[1] += aus.fern; q_aus[2] += aus.los; q_aus[3] += aus.klemme;
            if (modus == 2) { if (an.blockiert < aus.blockiert) besser++; else if (an.blockiert > aus.blockiert) schlechter++; else gleich++; }
            printf("seed 0x%08x: AUS blockiert %5ld Griffe %2d | AN blockiert %5ld Griffe %2d\n", zg_seed(i), aus.blockiert, aus.griffe, an.blockiert, an.griffe);
        }
        printf("SUMME %d Seeds x %d Bilder x 2 Gorillas: AUS %ld blockiert, %d Griffe | AN %ld blockiert, %d Griffe | besser %d schlechter %d gleich %d\n",
               n, b, su_aus, gr_aus, su_an, gr_an, besser, schlechter, gleich);
        printf("Sub-Anteile der blockierten Bilder AUS:"); for (int q = 0; q < 32; q++) if (sub_aus[q]) printf(" %d:%ld", q, sub_aus[q]);
        printf("\nSub-Anteile der blockierten Bilder AN: "); for (int q = 0; q < 32; q++) if (sub_an[q]) printf(" %d:%ld", q, sub_an[q]);
        printf("\nblockiert nah(<6001)/fern/mit Sicht/an der Klemme: AUS %ld/%ld/%ld/%ld | AN %ld/%ld/%ld/%ld",
               q_aus[0], q_aus[1], q_aus[2], q_aus[3], q_an[0], q_an[1], q_an[2], q_an[3]);
        printf("\n");
        return 0;
    }
    const int fest = (argc > 3 && strcmp(argv[1], "fest") == 0);
    const int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    int bilder = fest ? ((argc > 4) ? atoi(argv[4]) : 1500) : ((argc > 1 && !riegel) ? atoi(argv[1]) : 6000);
    uint32_t seed = (argc > 2 && !fest) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0x1234567u;
    int32_t fest_x = fest ? atoi(argv[2]) : 0, fest_z = fest ? atoi(argv[3]) : 0;


    re15_actor_t *g[2]; re15_actor_t *pl;
    if (argc > 4 && !fest && strcmp(argv[4], "aus") == 0) re15_r36_affen_padsuche_enable(0);   /* Zufallsgang-Spur ohne Port-Wahl */
    if (!aufbau(g, &pl)) return 1;
    if (fest) {
        pl->x = fest_x; pl->z = fest_z; pl->y = 0; pl->floor = 0;
        if (argc > 6) { g[0]->x = atoi(argv[5]); g[0]->z = atoi(argv[6]); g[1]->x = 20000; g[1]->z = -20000; }   /* Gorilla 1 versetzt, Gorilla 2 geparkt */
        if (argc > 7) { g[0]->sub_state_3 = (uint8_t)atoi(argv[7]); g[0]->dog_flags = g[0]->sub_state_3 ? 0 : 1; g[0]->motion = g[0]->sub_state_3 ? 4 : 5; }   /* +0x7: 0 = Jagd mit Sicht, 1 = blind */
        pl->rot_y = (int16_t)((((int)re15_atan2_q12(g[0]->z - pl->z, g[0]->x - pl->x) - 0x400)) & 0xfff);
        int subs[2][32]; memset(subs, 0, sizeof subs); int klemme[2] = {0,0}, los[2] = {0,0}, spr[2] = {0,0}, sprb[2] = {0,0};
        int dmin[2] = { 1 << 30, 1 << 30 }, erst[2] = {0,0}; int spr_prev2[2] = {0,0};
        for (int f = 1; f <= bilder; f++) {
            frame(0, 0);
            if (pl->hp < 100) pl->hp = 100;
            for (int k = 0; k < 2; k++) {
                re15_actor_t *e = g[k]; int sub = e->sub_state_1 & 31;
                long long dx = e->x - pl->x, dz = e->z - pl->z; int d = isqrt64(dx * dx + dz * dz);
                if (d < dmin[k]) dmin[k] = d;
                subs[k][sub]++; klemme[k] += (e->sca_wall_hit != 0); los[k] += (e->dog_flags & 1);
                if (!erst[k] && (sub == 5 || sub == 6 || sub == 8 || sub == 15)) erst[k] = f;
                if (sub == 7 && !spr_prev2[k]) { spr[k]++; if (e->sub_state_3 == 3) sprb[k]++; }
                spr_prev2[k] = (sub == 7);
            }
            if ((f % 30) == 0) {
                re15_actor_t *e = g[0]; uint8_t zc = 0, zc2 = 0;
                int hit = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc);
                int hit2 = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x20u, &zc2);
                long long dx = e->x - pl->x, dz = e->z - pl->z;
                printf("  F%d G1 st=%d/%d/%d/%d @(%d,%d) rot=%d y=%d los=%d klemme=%d 1dc=%d dist=%d | pad10 %d/%02x pad20 %d/%02x | G2 st=%d/%d/%d/%d @(%d,%d) los=%d klemme=%d\n",
                       f, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y, (int)e->y, e->dog_flags & 1,
                       e->sca_wall_hit != 0, (int)e->dog_blocked_ctr, isqrt64(dx * dx + dz * dz), hit, zc, hit2, zc2,
                       g[1]->state, g[1]->sub_state_1, g[1]->sub_state_2, g[1]->sub_state_3, (int)g[1]->x, (int)g[1]->z, g[1]->dog_flags & 1, g[1]->sca_wall_hit != 0);
            }
        }
        for (int k = 0; k < 2; k++) {
            printf("Gorilla %d: Leon fest @(%d,%d), %d Bilder: Abstand min %d, Ende %d, erster Angriff Bild %d, Klemme %d, Sicht %d, Spruenge %d (blind %d) | Subs:",
                   k + 1, (int)fest_x, (int)fest_z, bilder, dmin[k], (int)isqrt64((long long)(g[k]->x - pl->x) * (g[k]->x - pl->x) + (long long)(g[k]->z - pl->z) * (g[k]->z - pl->z)),
                   erst[k], klemme[k], los[k], spr[k], sprb[k]);
            for (int q = 0; q < 32; q++) if (subs[k][q]) printf(" %d:%d", q, subs[k][q]);
            printf("\n");
        }
        return 0;
    }

    static seg_t segs[2][64]; int nseg[2] = { 0, 0 };
    int32_t ax[2], az[2]; int fa[2]; int subs_akk[2][32]; int los_akk[2], kl_akk[2], spr_akk[2]; int32_t lx[2], lz[2];
    for (int k = 0; k < 2; k++) { ax[k] = g[k]->x; az[k] = g[k]->z; fa[k] = 0; memset(subs_akk[k], 0, sizeof subs_akk[k]); los_akk[k] = kl_akk[k] = spr_akk[k] = 0; lx[k] = g[k]->x; lz[k] = g[k]->z; }
    int ziel_rot = pl->rot_y; int bisse = 0, griffe = 0; int prev_gr = 0; int leon_weg = 0;
    uint32_t rng = seed;
    int spr_prev[2] = { 0, 0 };
    for (int f = 1; f <= bilder; f++) {
        if ((f % 90) == 1) { rng = rng * 1103515245u + 12345u; ziel_rot = (int)((rng >> 8) & 0xfff); }
        uint16_t held = 0;
        int err = ((ziel_rot - (int)(pl->rot_y & 0xfff) + 2048 + 4096) % 4096) - 2048;
        if (err > 48)       held = RE15_PAD_BIT_RIGHT;
        else if (err < -48) held = RE15_PAD_BIT_LEFT;
        else                held = RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS;
        int gr = re15_player_is_grabbed();
        if (gr) held = (uint16_t)((f & 1) ? RE15_PAD_BIT_SQUARE : RE15_PAD_BIT_CROSS);
        int32_t plx0 = pl->x, plz0 = pl->z;
        frame(held, (uint16_t)(gr ? held : 0));
        leon_weg += (int)(labs(pl->x - plx0) + labs(pl->z - plz0));
        if (pl->hp < 100) { if (pl->hp < 40) bisse++; pl->hp = 100; }
        if (gr && !prev_gr) griffe++;
        prev_gr = gr;
        if (argc > 3 && (f % 60) == 0) {          /* trace: Gorilla argv[3] je 60 Bilder */
            int k = atoi(argv[3]); if (k < 0 || k > 1) k = 0;
            re15_actor_t *e = g[k]; uint8_t zc = 0, zc2 = 0;
            int hit = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc);
            int hit2 = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x20u, &zc2);
            int win = hit ? (((((int)(zc & 0xf0u)) << 4) - (int)e->rot_y + 512) & 0xfff) : -1;
            printf("  F%d G%d st=%d/%d/%d/%d @(%d,%d) rot=%d floor=%d los=%d klemme=%d 1dc=%d | pad10 hit=%d zc=%02x win=%d | pad20 hit=%d zc=%02x | Leon (%d,%d) rot=%d dist=%d\n",
                   f, k + 1, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y, (int)e->floor,
                   e->dog_flags & 1, e->sca_wall_hit != 0, (int)e->dog_blocked_ctr, hit, zc, win, hit2, zc2, (int)pl->x, (int)pl->z, (int)pl->rot_y,
                   (int)isqrt64((long long)(e->x - pl->x) * (e->x - pl->x) + (long long)(e->z - pl->z) * (e->z - pl->z)));
            { int32_t ux = 0, uz = 0; int up = -1; int ua = re15_r36_affen_umweg_lesen((int)(e - g_actors), &ux, &uz, &up);
              printf("        Umweg %d Ziel (%d,%d) Pad #%d\n", ua, (int)ux, (int)uz, up); }
        }
        for (int k = 0; k < 2; k++) {
            re15_actor_t *e = g[k];
            int sub = e->sub_state_1 & 31;
            int lebt = (e->state == 1);
            int kampf = (sub == 5 || sub == 6 || sub == 8 || sub == 15);
            int bewegt = (labs(e->x - ax[k]) + labs(e->z - az[k])) >= 40;
            if (e->sub_state_1 == 7 && e->sub_state_3 != 0 && e->sub_state_2 == 0 && !spr_prev[k]) spr_akk[k]++;
            spr_prev[k] = (e->sub_state_1 == 7);
            if (lebt && !kampf && !bewegt) {
                subs_akk[k][sub]++; los_akk[k] += (e->dog_flags & 1) ? 1 : 0; kl_akk[k] += (e->sca_wall_hit != 0) ? 1 : 0;
                if (fa[k] == 0) fa[k] = f;
            } else {
                int dauer = fa[k] ? f - fa[k] : 0;
                if (dauer >= 90 && nseg[k] < 64) {
                    seg_t *s = &segs[k][nseg[k]++];
                    s->f0 = fa[k]; s->f1 = f - 1; s->x = ax[k]; s->z = az[k];
                    long long dx = ax[k] - pl->x, dz = az[k] - pl->z; s->dist = isqrt64(dx * dx + dz * dz);
                    memcpy(s->subs, subs_akk[k], sizeof s->subs); s->los1 = los_akk[k]; s->klemme = kl_akk[k]; s->spruenge = spr_akk[k];
                    s->leon_sicht = !re15_re2_los_cells_blocked(&s_rdt, ax[k], az[k], pl->x, pl->z, (int)e->floor, 4u);
                    zellen_nahe(e, s->zellen, sizeof s->zellen);
                }
                fa[k] = 0; memset(subs_akk[k], 0, sizeof subs_akk[k]); los_akk[k] = kl_akk[k] = spr_akk[k] = 0;
                ax[k] = e->x; az[k] = e->z;
            }
            lx[k] = e->x; lz[k] = e->z;
        }
    }
    printf("ROOM11C0 %d Bilder, Seed 0x%x: Leon-Weg %d, Griffe %d, Bisse(hp<40) %d; Gorillas: (%d,%d) hp %d / (%d,%d) hp %d\n",
           bilder, seed, leon_weg, griffe, bisse, (int)g[0]->x, (int)g[0]->z, (int)g[0]->hp, (int)g[1]->x, (int)g[1]->z, (int)g[1]->hp);
    int gesamt = 0;
    for (int k = 0; k < 2; k++) {
        printf("== Gorilla %d: %d blockierte Strecken >= 90 Bilder\n", k + 1, nseg[k]);
        for (int i = 0; i < nseg[k]; i++) {
            seg_t *s = &segs[k][i]; gesamt += s->f1 - s->f0 + 1;
            printf("  F%d-F%d (%d Bilder) @(%d,%d) Leon-Abstand %d, Leon sichtbar(Zellstrahl) %d | LOS-Latch %d/%d, Klemme %d/%d, Zonenspruenge %d | Subs:",
                   s->f0, s->f1, s->f1 - s->f0 + 1, (int)s->x, (int)s->z, s->dist, s->leon_sicht, s->los1, s->f1 - s->f0 + 1, s->klemme, s->f1 - s->f0 + 1, s->spruenge);
            for (int q = 0; q < 32; q++) if (s->subs[q]) printf(" %d:%d", q, s->subs[q]);
            printf("\n     Zellen: %s\n", s->zellen[0] ? s->zellen : "-");
        }
    }
    printf("blockierte Bilder gesamt: %d von %d (%.1f%%)\n", gesamt, 2 * bilder, 100.0 * gesamt / (2.0 * bilder));
    if (riegel) {
    {
        /* NACHBESSERUNG 1 (Abnahme 0, Mangel 6): der Riegel misst die BLOCKADE, nicht nur "es gibt Griffe". 16 Seeds x 6000
         * Bilder Zufallsgang, AUS (Original-Regel) gegen AN (PORT-WAHL Umweg), jeder Lauf in einem EIGENEN Prozess
         * (`eins`): im selben Prozess bleibt Modulzustand haengen (gemessen: stat-AUS allein 73006 gegen 61502 im Wechsel).
         * Messung 2026-10-10 (Dossier NB1, Anker 40): AUS 28653 blockierte Bilder / 73 Griffe, AN 721 / 80, schlechtester +194.
         * NB2 (Abnahme 1, Mangel 4): Anker 400 (Spruenge/Pendeln auf der Stelle zaehlen jetzt als blockiert):
         * AUS 38439 / 73 Griffe, AN 4568 / 76 (11,9 %), kein Seed schlechter (schlechtester -100).
         * NB3 (Abnahme 2): AUS 38439 / 73, AN 5725 / 74 (14,9 %), schlechtester +59. */
        long su_aus = 0, su_an = 0; int gr_aus = 0, gr_an = 0, n_ok = 0; long schlimmer = -1000000; uint32_t s_schl = 0;
        for (int i = 0; i < 16; i++) {
            long bl[2] = { -1, -1 }; int gr[2] = { 0, 0 };
            for (int m = 0; m < 2; m++) {
                char cmd[1200]; snprintf(cmd, sizeof cmd, "\"%s\" eins %d %d 6000", argv[0], i, m);
                FILE *pp = popen(cmd, "r"); char zl[512];
                while (pp && fgets(zl, sizeof zl, pp)) {
                    unsigned sd; int mm; long b; int g2;
                    if (sscanf(zl, "EINS seed 0x%x modus %d blockiert %ld griffe %d", &sd, &mm, &b, &g2) == 4) { bl[m] = b; gr[m] = g2; }
                }
                if (pp) pclose(pp);
            }
            if (bl[0] < 0 || bl[1] < 0) continue;
            n_ok++; su_aus += bl[0]; su_an += bl[1]; gr_aus += gr[0]; gr_an += gr[1];
            if (bl[1] - bl[0] > schlimmer) { schlimmer = bl[1] - bl[0]; s_schl = zg_seed(i); }
            printf("  Seed 0x%08x: AUS %ld blockiert / %d Griffe | AN %ld / %d\n", zg_seed(i), bl[0], gr[0], bl[1], gr[1]);
        }
        CHECK(n_ok == 16, "16 Zufallsgaenge je AUS und AN gelaufen (%d)", n_ok);
        CHECK(su_aus >= 10000, "Gegenprobe Original-Regel: %ld blockierte Gorilla-Bilder (Befund 'noch oft blockiert')", su_aus);
        CHECK(su_an * 6 <= su_aus, "Umweg: blockierte Bilder (Anker %d) %ld <= 1/6 von %ld", s_zg_anker, su_an, su_aus);
        CHECK(schlimmer <= 600, "kein Seed mehr als 600 Bilder schlechter (schlechtester 0x%08x: %+ld)", s_schl, schlimmer);
        CHECK(gr_an * 10 >= gr_aus * 8, "Griffe AN %d >= 80%% von AUS %d (die Gorillas erreichen Leon)", gr_an, gr_aus);
    }
    {
        /* Punkt 2 (PORT-WAHL Umweg, re15_zombieki_r36.h) — NB3 (Abnahme 2, Maengel 1/2): sieben Szenen mit BEIDEN Gorillas, je
         * Szene und Modus ein EIGENER Prozess (Modulzustand). 1-3: Leon noerdlich/oestlich der Wagenbloecke, Gorilla 1 suedlich/
         * westlich in der blinden Jagd. 4 = Befundstelle (-24000,10000) (Leon steht danach auf (-24000,11938), s. 4b). 5 = Nordwest-
         * Gang (Abnahme 2 Mangel 1): Leon (-14000,2000), Gorilla 1 ab (-9013,-15461) (exe-Slot-2-Start) — die Original-Regel greift
         * hier an (Bild 430), der Umweg darf nicht spaeter sein und muss ueber das West-Pad #47 laufen (NB2: Pad #36 nach SCA-
         * Reihenfolge, nie). 6/7 = Taschenrand: die naechsten Punkte, die Leon zu Fuss erreicht (4b), vor den Abnahme-Plaetzen
         * (-26000,7000)/(-22000,7000). Pruefungen: Gegenprobe Original-Regel; mit Umweg erster Angriff auf Leon (beliebiger Gorilla)
         * vorhanden und nicht spaeter als die Original-Regel; Zahl der angreifenden Gorillas; KEINE FERN-Blockade nach dem ersten
         * Angriff (>= 0x1771 @0x80117bc8 = Bereich des Umwegs; darunter bleibt die Original-Nahjagd, die die Spur-J-Riegel griff/
         * szene bildgenau pinnen); keine Blind-Sprung-Schleife (Abnahme 1: 48 blind). Messung 2026-10-11 (Dossier NB3). */
        static const int32_t sz[7][6] = {   /* lx, lz, gx, gz, Bilder, angreifende Gorillas mindestens */
            { -7000, 12500, -14000, -3000, 2400, 2 }, { -7000, 12500, -5525, -14883, 2400, 1 },   /* 2: G1 Original-Nahjagd (Dossier) */
            { 7000, 4000, -8915, -12487, 2400, 2 }, { -24000, 10000, 0, 0, 2400, 2 }, { -14000, 2000, -9013, -15461, 2400, 2 },
            { -24452, 11938, 0, 0, 2400, 2 }, { -22288, 11938, 0, 0, 2400, 2 } };
        for (int i = 0; i < 7; i++) {
            szene2_t vn[2]; memset(vn, 0, sizeof vn);
            for (int m = 0; m < 2; m++) {
                char cmd[1200]; snprintf(cmd, sizeof cmd, "\"%s\" szene2 %d %d %d %d %d %d", argv[0], (int)sz[i][0], (int)sz[i][1],
                                         (int)sz[i][2], (int)sz[i][3], (int)sz[i][4], m);
                FILE *pp = popen(cmd, "r"); char zl[512];
                while (pp && fgets(zl, sizeof zl, pp)) {
                    int lx, lz, k, er, dm, sp, bl, p36; long bk, na, nf;
                    if (sscanf(zl, "SZENE2 Leon (%d,%d) G%d: erster Angriff %d, Abstand min %d, Spruenge %d (blind %d), blockiert %ld (Anker %*d) nach %ld fern %ld pad36 %d",
                               &lx, &lz, &k, &er, &dm, &sp, &bl, &bk, &na, &nf, &p36) == 11 && k >= 1 && k <= 2) {
                        vn[m].erst[k - 1] = er; vn[m].dmin[k - 1] = dm; vn[m].spr[k - 1] = sp; vn[m].blind[k - 1] = bl; vn[m].blockiert[k - 1] = bk;
                        vn[m].nach[k - 1] = na; vn[m].nach_fern[k - 1] = nf; vn[m].pad36[k - 1] = p36;
                    }
                }
                if (pp) pclose(pp);
            }
            szene2_t v = vn[0], n = vn[1];
            int ev = (v.erst[0] && v.erst[1]) ? (v.erst[0] < v.erst[1] ? v.erst[0] : v.erst[1]) : (v.erst[0] ? v.erst[0] : v.erst[1]);
            int en = (n.erst[0] && n.erst[1]) ? (n.erst[0] < n.erst[1] ? n.erst[0] : n.erst[1]) : (n.erst[0] ? n.erst[0] : n.erst[1]);
            int na_ = (n.erst[0] > 0) + (n.erst[1] > 0);
            printf("Szene %d Leon (%d,%d): ORIGINAL-REGEL G1 %d / G2 %d (blockiert %ld/%ld) | UMWEG G1 %d / G2 %d, blockiert %ld/%ld, nach dem ersten "
                   "Angriff %ld/%ld (fern %ld/%ld), Spruenge %d/%d (blind %d/%d), Pad #36 %d/%d\n", i + 1, (int)sz[i][0], (int)sz[i][1], v.erst[0], v.erst[1],
                   v.blockiert[0], v.blockiert[1], n.erst[0], n.erst[1], n.blockiert[0], n.blockiert[1], n.nach[0], n.nach[1], n.nach_fern[0],
                   n.nach_fern[1], n.spr[0], n.spr[1], n.blind[0], n.blind[1], n.pad36[0], n.pad36[1]);
            if (i == 4) CHECK(v.erst[0] > 0 && v.erst[1] == 0, "Szene 5 Gegenprobe (Original-Regel): Gorilla 1 greift an (Bild %d), Gorilla 2 nie (%d)", v.erst[0], v.erst[1]);
            else        CHECK(v.erst[0] == 0 && v.erst[1] == 0, "Szene %d Gegenprobe (Original-Regel): keiner der beiden Gorillas greift an (Befund)", i + 1);
            CHECK(en > 0 && (ev == 0 || en <= ev), "Szene %d Umweg: erster Angriff auf Leon Bild %d (Original-Regel %d)", i + 1, en, ev);
            CHECK(na_ >= sz[i][5], "Szene %d Umweg: %d Gorilla(s) greifen an (>= %d)", i + 1, na_, (int)sz[i][5]);
            CHECK(n.nach_fern[0] < 90 && n.nach_fern[1] < 90, "Szene %d Umweg: keine FERN-Blockade nach dem ersten Angriff (%ld/%ld Bilder; nah %ld/%ld = "
                  "Original-Nahjagd)", i + 1, n.nach_fern[0], n.nach_fern[1], n.nach[0] - n.nach_fern[0], n.nach[1] - n.nach_fern[1]);
            CHECK(n.blind[0] <= 2 && n.blind[1] <= 2, "Szene %d Umweg: keine Blind-Sprung-Schleife (blind %d / %d; Abnahme 1: 48)", i + 1, n.blind[0], n.blind[1]);
            if (i == 4) CHECK(!n.pad36[0] && !n.pad36[1], "Szene 5 Umweg nach Weglaenge: kein Gorilla laeuft ueber das Ost-Pad #36 (NB2: Gorilla 1 ueber #36, nie)");
        }
        {   /* 4b — Abnahme 2 Mangel 2: die Plaetze (-26000,7000)/(-27000,12000)/(-22000,7000) (und die Befundstelle) liegen IN den Zellen
             * #13/#20/#21 (ROOM11C0.RDT 0x014c4/0x01518/0x01524). Leon erreicht sie zu Fuss nicht (Spieler-Klemme, echte Spielschleife);
             * RE15_PLAYER_POS setzt ihn hinein. Und das Gorilla-Raster fuehrt dort keinen freien Knoten mehr (Frei-Test NB3); die
             * Gegenprobe mit der NB2-Vorlage (= Knoten) zeigt das Zurueckschnapp-Artefakt @0x8003be8c-a0. */
            static const int32_t ll[4][4] = { { -23000, 14400, -26000, 7000 }, { -23000, 14400, -27000, 12000 }, { -23000, 14400, -24000, 10000 },
                                              { -14600, 6000, -22000, 7000 } };
            for (int i = 0; i < 4; i++) {
                char cmd[1200]; snprintf(cmd, sizeof cmd, "\"%s\" leonlauf %d %d %d %d 600", argv[0], (int)ll[i][0], (int)ll[i][1], (int)ll[i][2], (int)ll[i][3]);
                FILE *pp = popen(cmd, "r"); char zl[512]; int bx = 0, bz = 0, ab = -1;
                while (pp && fgets(zl, sizeof zl, pp)) {
                    int sx, sz2, tx, tz;
                    if (sscanf(zl, "LEONLAUF (%d,%d) -> (%d,%d): naechster Punkt (%d,%d) Abstand %d", &sx, &sz2, &tx, &tz, &bx, &bz, &ab) == 7) break;
                }
                if (pp) pclose(pp);
                CHECK(ab >= 1900, "Leon rennt 600 Bilder auf (%d,%d) zu: naechster Punkt (%d,%d), %d entfernt (z >= 11938 / x >= -17322 = Zellkanten + 450 + 0x12)",
                      (int)ll[i][2], (int)ll[i][3], bx, bz, ab);
            }
            int frei1 = 0, frei0 = 0;
            for (int z = 5200; z <= 11400; z += 800) for (int x = -29800; x <= -20200; x += 800) {
                for (int dl = 0; dl < 2; dl++) {
                    int32_t nx = x, nz = z; uint8_t ct = 0; uint16_t at = 0;
                    re15_collision_constrain_contact_band(&g_room_rdt, x + dl, z, &nx, &nz, 1600, 0, 4u, &ct, &at);
                    if (nx == x && nz == z) { if (dl) frei1++; else frei0++; }
                }
            }
            CHECK(frei0 > 0 && frei1 == 0, "Nordwest-Tasche (x -29800..-20200, z 5200..11400, Schritt 800): frei mit Vorlage = Knoten %d (NB2-Artefakt), "
                  "mit Vorlage = Knoten + 1 %d", frei0, frei1);
        }
        re15_r36_affen_padsuche_enable(1);
    }
        printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
        return fails ? 1 : 0;
    }
    return 0;
}
