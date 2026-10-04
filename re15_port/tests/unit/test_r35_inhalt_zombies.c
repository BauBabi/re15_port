/*
 * Runde 35 Spur F, Punkt 3 — "Im ROOM 1010 sind die stehenden Zombies zu nah an der Tuer. Das ist
 * unfair, da man so keine Chance hat, den Zombies auszuweichen. Bitte setze sie ein Stueck weiter
 * zurueck. In ROOM 1220 teilweise genauso. Die Zombies in den Zellen muessen zumindest so weit weg
 * sein, das man eine Chance hat aus dem Raum wieder raus zu drehen." (AUFTRAG.md Z. 19)
 * Dossier: analysis/befunde_runde35/F_inhalt.md, Punkt 3.
 *
 * MISST mit der echten Spiel-Schleife (scd_room_reenter + scd_vm_tick + re15_game_step, RE2-KI =
 * Auslieferungs-Vorgabe, RE2-Baenke aus shared_assets/RE2/CDEMD0.EMS), je Eintritt (Raum, Cut):
 *   STEHEN  der Spieler bleibt am Eintrittspunkt stehen: Bild des ersten Griffs
 *           (re15_player_is_grabbed = PL+0x1D3 & 0x80) bzw. 0 = kein Griff in 600 Bildern.
 *   FLUCHT  der Spieler dreht sofort auf der Stelle zur Tuer (RECHTS gehalten, 96/Bild
 *           @0x80073ee4 = [0,96]) und drueckt dann VIERECK (Aktionstaste, virtuell 0x4000 <- RAW
 *           SQUARE @0x80073dbc): Ergebnis "raus" (g_room_change.pending, Tuer-Satz gefeuert) oder
 *           "gegriffen".
 * Ohne Argument: Pruefungen (Riegel). Mit Argument "mess": nur Tabelle, beide Stellungen (Original =
 * Positionen der RDT, Port = re15_zombie_abstand).
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
#include "re2_ems.h"
#include "re15_zombie_abstand.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

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

typedef struct {
    uint16_t raum;
    uint8_t  cut;                 /* Eintritts-Cut = work_vars[0x0A] (Tuer-Payload Byte 10)       */
    int32_t  x, z;                /* Eintrittspunkt (Ziel des Tuer-Satzes im Nachbarraum)          */
    int16_t  yaw;                 /* Blickrichtung beim Eintritt                                   */
    int16_t  yaw_tuer;            /* Blickrichtung zur Ausgangstuer                                */
    const char *beleg;
} eintritt_t;

/* Eintrittspunkte = Ziele der Tuer-Saetze im Nachbarraum (Door_aot_set +14 x, +18 z, +20 yaw,
 * +24 Cut; selbst gelesen):
 *   ROOM1020 main00 @0x01CA2 `3b 01 .. 42 0e 00 00 f4 1a 00 08 00 01 00` -> 1010 (3650,6900) yaw 2048 Cut 0
 *   ROOM1020 main00 @0x01C82 `3b 00 .. 42 0e 00 00 92 f0 00 08 00 01 04` -> 1010 (3650,-3950) yaw 2048 Cut 4
 *   ROOM1210 main00 @0x01CE6/@0x01D06/@0x01D26/@0x01D46/@0x01D66 -> 1220 Cut 0/2/4/6/8
 * Blick zur Tuer: die Tuer-Rechtecke des Raums (1010 main00 @0x008AE / @0x008CE x 4150..5150 =
 * +X des Eintrittspunkts; 1220 main00 @0x00DD2.. x -22000..-21500 = +X bzw. x -17300..-16800 = -X).
 * Port-Yaw: 0 = +X, 2048 = -X (probe_1010_kriecher.c Fall D). */
static const eintritt_t k_eintritte[] = {
    { 0x1010, 0,   3650,   6900, 2048,    0, "ROOM1020 @0x01CA2" },
    { 0x1010, 4,   3650,  -3950, 2048,    0, "ROOM1020 @0x01C82" },
    { 0x1220, 0, -22400,  -6500, 2048,    0, "ROOM1210 @0x01CE6" },
    { 0x1220, 2, -16600,  -9900,    0, 2048, "ROOM1210 @0x01D06" },
    { 0x1220, 4, -22400, -14000, 2048,    0, "ROOM1210 @0x01D26" },
    { 0x1220, 6, -16600, -17900,    0, 2048, "ROOM1210 @0x01D46" },
    { 0x1220, 8, -16600, -25050,    0, 2048, "ROOM1210 @0x01D66" },
    /* Elza-Variante (ROOM1211 traegt dieselben Tuer-Saetze an denselben Offsets). ROOM1011 fehlt:
     * dort spawnt sub00 die Zombies nur hinter einer Bedingung (Satz @0x00A96 im Block); der
     * Tabellen-Schluessel (Typ + Original-Lage) deckt ihn trotzdem. */
    { 0x1221, 2, -16600,  -9900,    0, 2048, "ROOM1211 @0x01D06" },
};
#define N_EINTRITTE ((int)(sizeof k_eintritte / sizeof k_eintritte[0]))

static uint8_t *s_raw1010, *s_raw1220, *s_raw1011, *s_raw1221;
static size_t   s_n1010, s_n1220, s_n1011, s_n1221;

static void eintreten(const eintritt_t *e)
{
    const uint8_t *raw = e->raum == 0x1010 ? s_raw1010 : e->raum == 0x1011 ? s_raw1011
                       : e->raum == 0x1221 ? s_raw1221 : s_raw1220;
    size_t n = e->raum == 0x1010 ? s_n1010 : e->raum == 0x1011 ? s_n1011
             : e->raum == 0x1221 ? s_n1221 : s_n1220;
    re15_rdt_parse(raw, n, &g_room_rdt);
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = e->cut;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    re15_inv_load_briefing();
    g_current_room_id = e->raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = e->x; pl->y = 0; pl->z = e->z; pl->rot_y = e->yaw;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    s_shown = e->cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, e->cut);
    g_scd.cut_auto_enabled = 1;
    load_bank_re2(0x10); load_bank_re2(0x11); load_bank_re2(0x16);
}

typedef struct { int griff_bild; int zombies; int32_t min_abstand0; } stehen_t;

static int32_t isqrt64(long long v) { long long r = 0; while ((r + 1) * (r + 1) <= v) r++; return (int32_t)r; }

static int32_t naechster(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t best = 1 << 30;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *z = &g_actors[s];
        if (!z->active || !re15_re2z_owns_type(z->type)) continue;
        long long dx = z->x - pl->x, dz = z->z - pl->z;
        int32_t d = isqrt64(dx * dx + dz * dz);
        if (d < best) best = d;
    }
    return best;
}

static int zaehle_zombies(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) n++;
    return n;
}

static void kandidaten_setzen(void);

static stehen_t stehen(const eintritt_t *e, int bilder)
{
    stehen_t r = { 0, 0, 0 };
    eintreten(e);
    kandidaten_setzen();
    r.zombies = zaehle_zombies();
    r.min_abstand0 = naechster();
    for (int f = 1; f <= bilder; f++) {
        frame(0, 0);
        if (re15_player_is_grabbed()) { r.griff_bild = f; break; }
    }
    return r;
}

/* Mindestabstand je Aktor-Slot waehrend des letzten FLUCHT-Laufs (bis "raus"/Griff). */
static int32_t s_flucht_min[RE15_ACTOR_MAX];

/* Abstand jedes Zombies zum Spieler in s_flucht_min einarbeiten. */
static void flucht_min_messen(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *z = &g_actors[s];
        if (!z->active || !re15_re2z_owns_type(z->type)) continue;
        long long dx = z->x - pl->x, dz = z->z - pl->z;
        int32_t d = isqrt64(dx * dx + dz * dz);
        if (d < s_flucht_min[s]) s_flucht_min[s] = d;
    }
}

/* SUCHE: Kandidatenlage je Slot (nur fuer die Mess-Suche; x == INT32_MIN = Original). */
static int32_t s_kand_x[RE15_ACTOR_MAX], s_kand_z[RE15_ACTOR_MAX];
static uint8_t s_park[RE15_ACTOR_MAX];   /* SUCHE: noch nicht bearbeitete Verletzer ausblenden */
static void kandidaten_aus(void) { for (int s = 0; s < RE15_ACTOR_MAX; s++) s_kand_x[s] = INT32_MIN; }
static void kandidaten_setzen(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (s_kand_x[s] != INT32_MIN && g_actors[s].active) { g_actors[s].x = s_kand_x[s]; g_actors[s].z = s_kand_z[s]; }
        if (s_park[s]) g_actors[s].active = 0;
    }
}

/* 1 = raus (Tuer gefeuert), 0 = gegriffen, -1 = weder noch. *bild = Bild des Ergebnisses. */
static int flucht(const eintritt_t *e, int *bild, int *dreh_bilder)
{
    eintreten(e);
    kandidaten_setzen();
    for (int s = 0; s < RE15_ACTOR_MAX; s++) s_flucht_min[s] = 1 << 30;
    flucht_min_messen();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    *dreh_bilder = 0;
    int losgelassen = 1;
    for (int f = 1; f <= 200; f++) {
        int err = ((int)e->yaw_tuer - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        if (err > 48 || err < -48) {
            frame(RE15_PAD_BIT_RIGHT, (f == 1) ? RE15_PAD_BIT_RIGHT : 0);
            (*dreh_bilder)++;
        } else if (losgelassen) {
            frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* VIERECK-Flanke */
            losgelassen = 0;
        } else {
            frame(0, 0);                                       /* loslassen, naechste Flanke */
            losgelassen = 1;
        }
        /* nach dem Bild pruefen: der Griff-Riegel wird in re15_enemy_ai_run_all je Bild neu
         * abgeleitet (s_player_grabbed = 0 vor der Schleife), vorher steht der Wert des Vorlaufs */
        if (g_room_change.pending) { *bild = f; return 1; }
        flucht_min_messen();
        if (re15_player_is_grabbed()) { *bild = f; return 0; }
    }
    *bild = 200;
    return -1;
}

typedef struct { int griff, flucht, flucht_bild, dreh; int32_t naechster, flucht_min; int zombies;
                 int32_t sx[RE15_ACTOR_MAX], sz[RE15_ACTOR_MAX]; } ergebnis_t;

static void messen(const eintritt_t *e, ergebnis_t *r)
{
    memset(r, 0, sizeof *r);
    stehen_t st = stehen(e, 600);
    r->griff = st.griff_bild; r->zombies = st.zombies; r->naechster = st.min_abstand0;
    eintreten(e);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        int z = g_actors[s].active && re15_re2z_owns_type(g_actors[s].type);
        r->sx[s] = z ? g_actors[s].x : INT32_MIN; r->sz[s] = z ? g_actors[s].z : INT32_MIN;
    }
    r->flucht = flucht(e, &r->flucht_bild, &r->dreh);
    r->flucht_min = 1 << 30;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s_flucht_min[s] < r->flucht_min) r->flucht_min = s_flucht_min[s];
}

static void tabelle(const char *titel, ergebnis_t *res)
{
    printf("\n--- %s ---\n", titel);
    printf("Raum  Cut  Eintritt          naechster  Zombies  STEHEN Griff-Bild  FLUCHT (Drehbilder)  Flucht-min\n");
    for (int i = 0; i < N_EINTRITTE; i++) {
        const eintritt_t *e = &k_eintritte[i];
        ergebnis_t *r = &res[i];
        messen(e, r);
        printf("%04X  %3u  (%6ld,%6ld)  %8ld  %7d  %18d  %s Bild %d (%d)  %ld\n", e->raum, e->cut,
               (long)e->x, (long)e->z, (long)r->naechster, r->zombies, r->griff,
               r->flucht == 1 ? "raus" : r->flucht == 0 ? "GEGRIFFEN" : "offen", r->flucht_bild, r->dreh,
               (long)r->flucht_min);
    }
}

int main(int argc, char **argv)
{
    kandidaten_aus();
    s_raw1010 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &s_n1010);
    s_raw1220 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1220.RDT", &s_n1220);
    s_raw1011 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1011.RDT", &s_n1011);
    s_raw1221 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1221.RDT", &s_n1221);
    if (!s_raw1010 || !s_raw1220 || !s_raw1011 || !s_raw1221) { printf("FAIL: RDT nicht lesbar\n"); return 1; }

    const int mess = (argc > 1 && strcmp(argv[1], "mess") == 0);
    if (argc > 2 && strcmp(argv[1], "tempo") == 0) {     /* Zombie-Bahn je Bild (Eintritt argv[2]) */
        re15_zombie_abstand_set_aktiv(argc > 3 ? atoi(argv[3]) : 0);
        const eintritt_t *e = &k_eintritte[atoi(argv[2])];
        eintreten(e);
        int32_t lx[RE15_ACTOR_MAX], lz[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) { lx[s] = g_actors[s].x; lz[s] = g_actors[s].z; }
        for (int f = 1; f <= 400; f++) {
            frame(0, 0);
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                const re15_actor_t *z = &g_actors[s];
                if (!z->active || !re15_re2z_owns_type(z->type)) continue;
                long long dx = z->x - lx[s], dz = z->z - lz[s];
                printf("f%3d slot%d st=%u s1=%2u s2=%2u clip=0x%02X pos=(%6ld,%6ld) d=%5u schritt=%4ld grab=%d\n",
                       f, s, z->state, z->sub_state_1, z->sub_state_2, z->motion, (long)z->x, (long)z->z,
                       (unsigned)z->ai_dist, (long)isqrt64(dx * dx + dz * dz), re15_player_is_grabbed());
                lx[s] = z->x; lz[s] = z->z;
            }
            if (re15_player_is_grabbed()) break;
        }
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "suche") == 0) {
        /* REGEL (Dossier Punkt 3): je Eintritt wird jeder Zombie, der bei der schnellstmoeglichen
         * Flucht (sofort drehen 96/Bild @0x80073ee4, VIERECK) naeher als die Sprungschwelle
         * 0xBB8 = 3000 (DECISION[0] `sltiu 0xbb8` @0x80101374 / DECISION[2] @0x801020A8) an den
         * Spieler kommt, um die KLEINSTE Strecke versetzt (Ringe 100, 200, ... um die Original-Lage,
         * 64 Richtungen), bei der sein Flucht-Mindestabstand >= 3000 ist und die Flucht "raus" endet;
         * auf dem kleinsten passenden Ring gewinnt der Ort mit dem GROESSTEN Abstand zum Eintritt
         * ("weiter zurueck"). Kandidat nur mit freiem Weg von der Original-Lage (Zellen-Strahl,
         * Band 0, Maske 4 = Gegner-Wandklemme) und freier Grundflaeche (re15_collision_box_blocked,
         * Radius hit_radius_min). Noch nicht bearbeitete Verletzer sind waehrend der Suche
         * ausgeblendet; das Ergebnis prueft der Riegel danach GEMEINSAM ueber den echten Haken. */
        re15_zombie_abstand_set_aktiv(0);
        for (int i = 0; i < N_EINTRITTE; i++) {
            const eintritt_t *e = &k_eintritte[i];
            kandidaten_aus();
            int bild = 0, dreh = 0;
            int32_t x0[RE15_ACTOR_MAX], z0[RE15_ACTOR_MAX], min0[RE15_ACTOR_MAX];
            eintreten(e);
            for (int s = 0; s < RE15_ACTOR_MAX; s++) { x0[s] = g_actors[s].x; z0[s] = g_actors[s].z; }
            int fl = flucht(e, &bild, &dreh);
            for (int s = 0; s < RE15_ACTOR_MAX; s++) min0[s] = s_flucht_min[s];
            printf("Eintritt %04X Cut %u: Flucht %s Bild %d\n", e->raum, e->cut, fl == 1 ? "raus" : "GEGRIFFEN", bild);
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (min0[s] != (1 << 30) && min0[s] < 3000) s_park[s] = 1;
            /* Durchgang 0 mit ausgeblendeten Verletzern, danach GEMEINSAM: solange ein Zombie bei der
             * Flucht mit allen neuen Lagen unter 3000 kommt, wird er erneut gesucht (Kandidat muss
             * dann die Flucht fuer ALLE Zombies >= 3000 halten). */
            for (int pass = 0; pass < 4; pass++) {
            if (pass > 0) {
                int f3 = flucht(e, &bild, &dreh);
                int verletzt = 0;
                for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                    min0[s] = s_flucht_min[s];
                    if (min0[s] != (1 << 30) && min0[s] < 3000) verletzt = 1;
                }
                printf("  Durchgang %d gemeinsam: Flucht %s, %s\n", pass, f3 == 1 ? "raus" : "GEGRIFFEN",
                       verletzt ? "Verletzer uebrig" : "alle >= 3000");
                if (!verletzt && f3 == 1) break;
            }
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                if (min0[s] == (1 << 30)) continue;
                if (min0[s] >= 3000) { if (pass == 0) printf("   slot %d (%ld,%ld) Flucht-min %ld >= 3000 -> bleibt\n",
                                              s, (long)x0[s], (long)z0[s], (long)min0[s]); continue; }
                s_park[s] = 0;
                s_kand_x[s] = INT32_MIN;
                int best = 0; int32_t bx = 0, bz = 0, bmin = 0, bstart = -1; int bbild = 0;
                for (int r = 100; r <= 8000 && !best; r += 100) {
                    for (int a = 0; a < 64; a++) {
                        double w = a * 6.283185307179586 / 64.0;
                        int32_t cx = x0[s] + (int32_t)(r * cos(w) + (r * cos(w) >= 0 ? 0.5 : -0.5));
                        int32_t cz = z0[s] + (int32_t)(r * sin(w) + (r * sin(w) >= 0 ? 0.5 : -0.5));
                        eintreten(e);
                        const re15_actor_t *z = &g_actors[s];
                        if (re15_re2_los_cells_blocked(&s_rdt, x0[s], z0[s], cx, cz, 0, 4u)) continue;
                        if (re15_collision_box_blocked(&s_rdt, cx, cz, 0, (int32_t)z->hit_radius_min,
                                                       z->sca_mask ? z->sca_mask : 4u)) continue;
                        s_kand_x[s] = cx; s_kand_z[s] = cz;
                        int f2 = flucht(e, &bild, &dreh);
                        int32_t start = isqrt64((long long)(cx - e->x) * (cx - e->x) + (long long)(cz - e->z) * (cz - e->z));
                        int32_t alle = 1 << 30;
                        for (int q = 1; q < RE15_ACTOR_MAX; q++) if (s_flucht_min[q] < alle) alle = s_flucht_min[q];
                        if (f2 == 1 && s_flucht_min[s] >= 3000 && (pass == 0 || alle >= 3000) && start > bstart) {
                            best = r; bx = cx; bz = cz; bmin = s_flucht_min[s]; bstart = start; bbild = bild;
                        }
                        s_kand_x[s] = INT32_MIN;
                    }
                }
                if (best) {
                    s_kand_x[s] = bx; s_kand_z[s] = bz;
                    printf("   slot %d (%ld,%ld) Flucht-min %ld -> (%ld,%ld) Versatz %d, Flucht-min %ld, raus Bild %d, "
                           "Startabstand %ld\n", s, (long)x0[s], (long)z0[s], (long)min0[s], (long)bx, (long)bz, best,
                           (long)bmin, bbild, (long)bstart);
                } else printf("   slot %d (%ld,%ld): KEINE Lage gefunden\n", s, (long)x0[s], (long)z0[s]);
            }
            for (int s = 0; s < RE15_ACTOR_MAX; s++) s_park[s] = 0;
            }
        }
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "karte") == 0) {     /* Grundriss-Rohdaten fuers Dossier */
        for (int r = 0; r < 2; r++) {
            const uint8_t *raw = r ? s_raw1220 : s_raw1010; size_t n = r ? s_n1220 : s_n1010;
            re15_rdt_t rd; re15_rdt_parse(raw, n, &rd);
            for (int i = 0; i < rd.sca_count; i++)
                printf("SCA %04X %d %d %d %d %d %d 0x%02X 0x%02X %d\n", r ? 0x1220 : 0x1010, i,
                       rd.sca[i].x, rd.sca[i].z, rd.sca[i].width, rd.sca[i].density,
                       rd.sca[i].type, rd.sca[i].u0, rd.sca[i].u1, rd.sca[i].floor);
        }
        return 0;
    }
    static ergebnis_t orig[N_EINTRITTE], port[N_EINTRITTE];
    re15_zombie_abstand_set_aktiv(0);
    tabelle("ORIGINAL (RDT-Positionen)", orig);
    re15_zombie_abstand_set_aktiv(1);
    tabelle("PORT (re15_zombie_abstand)", port);
    if (mess) return 0;
    printf("\n");

    /* A: der Befund des Nutzers ist im Original messbar (Gegenprobe ohne Tabelle) */
    CHECK(orig[3].flucht == 0, "A ORIGINAL 1220 Cut 2: Flucht unmoeglich, gegriffen in Bild %d "
          "(Zombie @0x00F5A Startabstand %ld)", orig[3].flucht_bild, (long)orig[3].naechster);
    CHECK(orig[0].flucht_min < 3000, "A ORIGINAL 1010 Cut 0: Zombie kommt waehrend der Flucht auf %ld < 0xBB8",
          (long)orig[0].flucht_min);

    /* B: Port — von jedem Eintritt kommt man raus, kein Zombie kommt in die Sprungschwelle */
    for (int i = 0; i < N_EINTRITTE; i++) {
        const eintritt_t *e = &k_eintritte[i];
        CHECK(port[i].flucht == 1, "B %04X Cut %u (%s): Flucht raus in Bild %d (%d Drehbilder)",
              e->raum, e->cut, e->beleg, port[i].flucht_bild, port[i].dreh);
        CHECK(port[i].flucht_min >= 3000, "B %04X Cut %u: Flucht-Mindestabstand %ld >= 0xBB8 = 3000 "
              "(@0x80101374 / @0x801020A8)", e->raum, e->cut, (long)port[i].flucht_min);
    }

    /* C: Stehen — der Griff kommt spaeter (oder gar nicht) als im Original, wo der Nutzer klagte */
    static const int k_klage[] = { 0, 3, 7 };         /* 1010 Cut 0, 1220 Cut 2, 1221 Cut 2 */
    for (int k = 0; k < 3; k++) {
        int i = k_klage[k];
        int ok = (port[i].griff == 0) || (orig[i].griff != 0 && port[i].griff > orig[i].griff);
        CHECK(ok, "C %04X Cut %u: Stehen -> Griff Original Bild %d, Port Bild %d (0 = keiner in 600)",
              k_eintritte[i].raum, k_eintritte[i].cut, orig[i].griff, port[i].griff);
    }

    /* D: jeder Tabellen-Satz steht nach dem Spawn genau auf seiner neuen Lage, alle anderen
     *    Zombies genau auf ihrer Original-Lage (der Haken aendert nichts sonst) */
    int verschoben = 0, gleich = 0, falsch = 0;
    for (int i = 0; i < N_EINTRITTE; i++)
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (orig[i].sx[s] == INT32_MIN) continue;
            int in_tab = 0; int16_t nx = 0, nz = 0;
            for (int t = 0; t < re15_zombie_abstand_anzahl(); t++) {
                uint16_t ra; uint8_t sl, ty; int16_t x0, z0, x1, z1;
                re15_zombie_abstand_eintrag(t, &ra, &sl, &ty, &x0, &z0, &x1, &z1);
                if (ra == (k_eintritte[i].raum & 0xFFFEu) && orig[i].sx[s] == x0 && orig[i].sz[s] == z0)
                    { in_tab = 1; nx = x1; nz = z1; }
            }
            if (in_tab) { if (port[i].sx[s] == nx && port[i].sz[s] == nz) verschoben++; else falsch++; }
            else        { if (port[i].sx[s] == orig[i].sx[s] && port[i].sz[s] == orig[i].sz[s]) gleich++; else falsch++; }
        }
    CHECK(falsch == 0 && verschoben == 9, "D Spawn: %d Saetze auf neuer Lage (7 Tabelle + 2 in ROOM1221), "
          "%d unveraendert, %d falsch", verschoben, gleich, falsch);

    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
