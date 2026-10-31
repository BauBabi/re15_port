/* test_r41_birkin_wand.c — Riegel Abnahme 3 Spur R, N1/N2/N6: kein Schub, Griff oder Wurf laesst den Spieler in fester Kollision stehen.
 *
 * Modi (Argument):
 *   rein   Die Klemme allein an den gemessenen Faellen der Abnahme, ohne Gegner.
 *   g5     Echter re15_game_step in ROOM5090 mit G5 und Armen (RE15_ENDKAMPF_NUR_G5=1, s. CMake): Startlagen entlang aller Waende.
 *   griff  Echter re15_game_step mit EM30-Griff (Haken 0x8011afd8, cmd 5) und Wurf (cmd 6, derselbe Einstieg wie der Griff der Form bb_griff)
 *          aus allen Wandlagen in ROOM5090, ROOM5080, ROOM3070.
 *
 * Mass je Startlage: nach 300 Bildern steht der Spieler frei und kann laufen. "Frei" = Mittelpunkt in keiner festen Zelle, an JEDEM
 * Bildende, denn dort zeichnet der Port. "Kann laufen" = in mindestens einer von vier Richtungen 40 Bilder Vorwaerts bewegen ihn um
 * mindestens 300 Einheiten. Eingekeilt ist er nur, wenn alle vier Richtungen scheitern.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_math.h"
#include "re15_player.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_fade.h"
#include "re15_birkin_opfer.h"
#include "re15_nclip_r38.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x, uint16_t *kontakt);
extern int re15_g5_opfer_variante(void);
extern void re15_g5_opfer_start(re15_actor_t *pl, int variante);

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_war = 0;
static uint16_t           s_pad_alt = 0;
static uint32_t           s_zufall = 0x1badb002u;

static unsigned zufall(unsigned n)
{
    s_zufall = s_zufall * 1103515245u + 12345u;
    return ((s_zufall >> 16) & 0x7fffu) % n;
}

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

static re15_emd_animation_t s_pl00_anim; static re15_emd_skeleton_t s_pl00_skel;
static int pl00_laden(void)
{
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) return 0;
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 || re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) != 0) return 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    return 1;                                   /* edd/emr bleiben fuer die Bank-Zeiger am Leben */
}

static int re2_baenke(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    for (int k = 0; k < 2; k++) {
        uint8_t type = k ? 0x37u : 0x36u;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) return 0;
        if (re2_ems_load_bank(ems, n, type, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

static uint8_t s_blob[0x80000];
static int em30_bank(void)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) != 0 || len > sizeof s_blob) { free(ems); return 0; }
    memcpy(s_blob, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!eb || re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = (re15_emd_parse_victim_bank(s_blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    return eb->victim_ok;
}

/* Ein Spielbild wie probe_r20_birkin_push.c, mit Pad. */
static void bild(uint16_t pad)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_war) { g_scd.letterbox_countdown = 15; }
        s_cine_war = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = pad;
    s_ctx.pad_pressed = (uint16_t)(pad & (uint16_t)~s_pad_alt);
    s_pad_alt = pad;
    re15_game_step(&s_ctx);
}

/* Mittelpunkt in einer festen Zelle des Bandes: die Wahrheit, gegen die gemessen wird. Unabhaengig von beiden Klemmen. */
static int fest(int32_t x, int32_t z)
{
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        const int32_t x0 = e->x, z0 = e->z, w = e->width, d = e->density;
        if ((e->floor >> 4) != 0 || !(e->u0 & 1u)) continue;
        if (x < x0 || x >= x0 + w || z < z0 || z >= z0 + d) continue;
        if (e->type == 1) return i + 1;
        if (e->type == 6 && (int64_t)(z - z0) * w < (int64_t)(x - x0) * d) return i + 1;   /* unter der Linie (x0,z0)-(x0+w,z0+d) */
        if (e->type == 2) {                                                                /* Raute um die Zellmitte */
            const int64_t dx = 2 * (int64_t)x - (2 * (int64_t)x0 + w), dz = 2 * (int64_t)z - (2 * (int64_t)z0 + d);
            if ((dx < 0 ? -dx : dx) * d + (dz < 0 ? -dz : dz) * w < (int64_t)w * d) return i + 1;
        }
        if (e->type == 3) {
            const int64_t r = w / 2, dx = x - (x0 + r), dz = z - (z0 + r);
            if (dx * dx + dz * dz < r * r) return i + 1;
        }
    }
    return 0;
}

/* Vier Richtungen je 40 Bilder Vorwaerts. Rueckgabe: groesster Weg. */
static int32_t laufprobe(int halte_hp)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t best = 0;
    for (int k = 0; k < 4; k++) {
        const int32_t sx = pl->x, sz = pl->z;
        pl->rot_y = (int16_t)(k * 1024);
        for (int f = 0; f < 40; f++) { if (halte_hp) pl->hp = 100; bild(RE15_PAD_BIT_UP); }
        {   const int64_t dx = pl->x - sx, dz = pl->z - sz;
            const int32_t w = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
            if (w > best) best = w;
        }
    }
    return best;
}

/* Wandnahe Startlagen: frei, kein Rechteck-Ueberstand, und ein Nachbar im Abstand 700 liegt in fester Zelle oder wird geklemmt. */
#define MAX_LAGEN 2000
static int32_t s_lage[MAX_LAGEN][2]; static int s_lagen;
static void lagen_bauen(int32_t x0, int32_t x1, int32_t z0, int32_t z1, int32_t raster)
{
    s_lagen = 0;
    re15_collision_set_band(0);
    for (int32_t z = z0; z <= z1; z += raster)
        for (int32_t x = x0; x <= x1; x += raster) {
            int32_t nx = x, nz = z, wand = 0;
            if (fest(x, z)) continue;
            /* Begehbar nach dem Klemmpfad des Spiels wie probe_p2_floor_dump: die RE1.5-Klemme laesst die Lage stehen. Die RE2-Klemme taugt
             * dafuer nicht, sie stellt in Taschen zwischen zwei Zellen auf die Vorlage zurueck und meldet so "unbewegt". */
            re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
            if (nx != x || nz != z) continue;
            for (int k = 0; k < 8 && !wand; k++) {
                static const int8_t dir[8][2] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1} };
                int32_t tx = x + 700 * dir[k][0], tz = z + 700 * dir[k][1], mx = tx, mz = tz;
                re15_collision_constrain_re2(&s_rdt, tx, tz, &mx, &mz);
                if (mx != tx || mz != tz || fest(tx, tz)) wand = 1;
            }
            if (wand && s_lagen < MAX_LAGEN) { s_lage[s_lagen][0] = x; s_lage[s_lagen][1] = z; s_lagen++; }
        }
}

static int raum_boot(const char *rel, unsigned id, int32_t px, int32_t pz, int cut)
{
    size_t rsz = 0; char p[512];
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *raw = slurp(p, &rsz);
    if (!raw || re15_rdt_parse(raw, rsz, &s_rdt) != 0) return 0;
    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 1024;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

/* --- rein: die gemessenen Lagen der Abnahme, nur Klemme --------------------------------------------------------------------- */
static void test_rein(void)
{
    if (!raum_boot("STAGE5/ROOM5090.RDT", 0x5090u, 8000, -24000, 14)) { CHECK(0, "ROOM5090"); return; }
    /* Ueberlauf Typ 6: f006 F1267. Die RE1.5-Klemme wirft nach (4287,-26207), Soll: Rueckstellung, Tiefe 744 ueber Grenze 400 @0x8004d864 */
    {   int32_t x = 4542, z = -25795, ax = x, az = z;
        re15_collision_constrain(&s_rdt, 3878, -25190, &ax, &az);
        CHECK(fest(ax, az) != 0, "Kontrolle: RE1.5-Klemme wirft nicht mehr in #30 (%d,%d)", (int)ax, (int)az);
        re15_collision_constrain_re2(&s_rdt, 3878, -25190, &x, &z);
        CHECK(x == 3878 && z == -25190, "f006: Vorlage @0x8004d8f0 erwartet, (%d,%d)", (int)x, (int)z);
    }
    /* Tiefe ohne Ueberlauf, aber ueber der Grenze 400: off_z = -475 (`slti v0,v0,401` @0x8004d878) -> Vorlage */
    {   int32_t x = 3000, z = -26213;
        re15_collision_constrain_re2(&s_rdt, 3000, -25500, &x, &z);
        CHECK(x == 3000 && z == -25500, "Grenze 400: Vorlage erwartet, (%d,%d)", (int)x, (int)z);
    }
    /* flache Tiefe: Schrift auf die Linie, kein Rueckfall */
    {   int32_t x = 3200, z = -25700;
        re15_collision_constrain_re2(&s_rdt, 3150, -25480, &x, &z);
        CHECK(!fest(x, z) && !(x == 3150 && z == -25480), "Diagonale flach: gleitet nicht (%d,%d)", (int)x, (int)z);
    }
    /* Box 29 von Norden, tief: RE1.5 nimmt die Suedkante (z -27668), RE2 die Herkunftsseite */
    {   int32_t x = 8000, z = -26800, ax = x, az = z;
        re15_collision_constrain(&s_rdt, 8000, -26800, &ax, &az);
        CHECK(az < -27000, "Kontrolle: RE1.5 ohne Herkunft (z %d)", (int)az);
        re15_collision_constrain_re2(&s_rdt, 8000, -24749, &x, &z);
        CHECK(z == -24749, "Box 29: z %d != -24749 (cell.z + h + t @0x8004ca4c)", (int)z);
    }
    /* Box 22 und 29 hintereinander, G5-Schub 5000: beide Zellen in Listenreihenfolge */
    {   int32_t x = 9000, z = -29700;
        re15_collision_constrain_re2(&s_rdt, 9000, -24749, &x, &z);
        CHECK(z == -24749, "Schub durch Box 29 in Box 22: z %d", (int)z);
    }
    /* Nordwand #23 und #28 ebenso */
    {   int32_t x = 9000, z = -17500;
        re15_collision_constrain_re2(&s_rdt, 9000, -22151, &x, &z);
        CHECK(z == -22151, "Schub durch Box 28 in Box 23: z %d", (int)z);
    }
    /* Vorlage in der Zelle (FUN_8004ca68): RE2 haelt nicht fest wie RE1.5 Code 0, sondern schiebt heraus, tief auf die Vorlage */
    {   int32_t x = 8000, z = -25000;
        re15_collision_constrain_re2(&s_rdt, 8000, -24990, &x, &z);
        CHECK(z == -24749, "Vorlage in der Zelle, flach: z %d", (int)z);
    }
    /* Laufen an der Wand: die freie Achse bleibt */
    {   int32_t x = 8100, z = -24760;
        re15_collision_constrain_re2(&s_rdt, 8000, -24749, &x, &z);
        CHECK(x == 8100 && z == -24749, "Laengs der Wand (%d,%d)", (int)x, (int)z);
    }
}

/* --- nclip: Abnahme 3 N3, Vorzeichen wie FUN_80027bec (MAC0 <= 0 weg) und FUN_80027dbc (MAC0 < 0 weg) ------------------------------------- */
static void test_nclip(void)
{
    CHECK(re15_nclip_r41_birkin(0x30u) && re15_nclip_r41_birkin(0x36u) && re15_nclip_r41_birkin(0x37u), "Birkin-Typen");
    CHECK(!re15_nclip_r41_birkin(0x10u) && !re15_nclip_r41_birkin(0x27u), "andere Gegner bleiben unveraendert");
    CHECK(re15_nclip_r38_mac0(0, 0, 10, 0, 0, 10) > 0, "Bezug: (0,0)(10,0)(0,10) ist MAC0 > 0");
    CHECK(!re15_nclip_r41_weg(0, 0, 0, 10, 0, 0, 10), "Vorderseite Dreieck bleibt");
    CHECK(re15_nclip_r41_weg(0, 0, 0, 0, 10, 10, 0), "Rueckseite Dreieck weg");
    CHECK(re15_nclip_r41_weg(0, 0, 0, 10, 0, 20, 0), "MAC0 = 0 Dreieck weg (`bgtz` @0x80027cc8)");
    CHECK(re15_nclip_r41_weg(1, 0, 0, 0, 10, 10, 0), "Rueckseite Viereck weg");
    CHECK(!re15_nclip_r41_weg(1, 0, 0, 10, 0, 20, 0), "MAC0 = 0 Viereck bleibt (FUN_80027dbc Z.40)");
    /* Die Lage, um die es geht: Cut 13 steht im G5. RID @0x200, Rand der Masse = Ursprung + 4494 (Blob-Bind 1800 @0x80100680 + Mesh-2-Bbox 2694). */
    if (raum_boot("STAGE5/ROOM5090.RDT", 0x5090u, 8000, -24000, 14)) {
        const re15_camera_cut_t *c = &s_rdt.cuts[13];
        CHECK(c->pos_x == 11050 && c->pos_y == -3110 && c->pos_z == -24812, "RID @0x200 Cut 13: (%d,%d,%d)", (int)c->pos_x, (int)c->pos_y, (int)c->pos_z);
        CHECK(c->target_x > c->pos_x, "Cut 13 blickt nach Osten");
        CHECK(12000 + 4494 > c->pos_x && 7000 + 4494 > c->pos_x, "Kappe X 12000 @0x80100fd0: die Masse reicht ueber die Kamera");
    }
}

/* --- g5: Startlagen entlang aller Waende, G5 und Arme im echten Spielschritt ---------------------------------------------------- */
static int s_boss = -1;
static int finde_boss(uint8_t typ)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) return s;
    return -1;
}

extern int re15_g5_body_segment(int idx, int32_t *wx, int32_t *wy, int32_t *wz, int32_t *r);
static int g5_drueckt(const re15_actor_t *pl)
{
    for (int k = 0; k < 2; k++) {
        int32_t sx = 0, sz = 0, r = 0;
        if (!re15_g5_body_segment(k, &sx, NULL, &sz, &r)) continue;
        {   const int64_t dx = pl->x - sx, dz = pl->z - sz, rs = r + 450;
            if (dx * dx + dz * dz < rs * rs) return 1; }
    }
    return 0;
}

static void test_g5(const char *rel, unsigned id)
{
    if (!raum_boot(rel, id, 25600, -23350, 14)) { CHECK(0, "%s", rel); return; }
    if (!re2_baenke()) { CHECK(0, "RE2-Baenke"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    int trigger = -1;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        bild(0);
        if (trigger < 0 && s_shown == 12) trigger = f;
        if (trigger >= 0 && f > trigger + 3) break;
    }
    s_boss = finde_boss(0x36u);
    CHECK(s_boss > 0, "kein G5");
    if (s_boss <= 0) return;
    re15_actor_t *e = &g_actors[s_boss];

    /* N2: Leon an der Suedwand in der Reichweite der Intro-Arme, keine Eingabe. Abnahme: F1187-F1573 Lage (6865,-25290). */
    {
        pl->x = 7400; pl->z = -24732; pl->hp = 100; pl->rot_y = 0;
        int n_kontakt = 0, n_wand = 0, lauf_ok = 0, lauf_try = 0, geduld = 0;
        for (int f = 0; f < 1500 && e->motion != 5; f++) {
            int kontakt = 0;
            pl->hp = 100;
            for (int k = 0; k < 4; k++) { uint16_t ko = 0; if (re15_g5_tentakel_zustand(k, NULL, NULL, NULL, NULL, &ko) == 0 && (ko & 4u)) kontakt = 1; }
            bild(0);
            if (kontakt) n_kontakt++;
            if (fest(pl->x, pl->z)) n_wand++;
            if (kontakt && ++geduld >= 30) {
                /* der Arm liegt: laufen. Vorher warf die Klemme jede Eingabe auf die Lage in der Wand zurueck. */
                const int32_t sx = pl->x, sz = pl->z;
                pl->rot_y = 0;
                for (int g = 0; g < 30; g++) { pl->hp = 100; bild(RE15_PAD_BIT_UP); }
                if (abs((int)(pl->x - sx)) + abs((int)(pl->z - sz)) >= 150) lauf_ok++;
                lauf_try++; geduld = 0;
            }
        }
        fprintf(stderr, "N2: Arm-Kontakt %d Bilder, Bildende in der Wand %d, Laufproben %d/%d\n", n_kontakt, n_wand, lauf_ok, lauf_try);
        CHECK(n_kontakt > 0, "N2: kein Arm-Kontakt, der Riegel misst nichts");
        CHECK(n_wand == 0, "N2: %d Bildenden in der Wand", n_wand);
        CHECK(lauf_try > 0 && lauf_ok == lauf_try, "N2: Eingabe wirkt nicht (%d/%d)", lauf_ok, lauf_try);
    }

    /* N1: Leon in der Ecke #29/#30, der G5 (x 1960, Rand 4410 um x -40) drueckt mit gut 1000 je Bild. Die Rueckstellung @0x8004d8f0 nimmt den Schub
     * zurueck; ohne die Vor-Klemme nahm sie auch die Eingabe (Lauf z_w4 F1360-F2100). */
    {
        int32_t weg = 0;
        for (int f = 0; f < 240 && (re15_player_is_grabbed() || re15_player_hit_flinch_active() || re15_player_knockdown_active()); f++) { pl->hp = 100; bild(0); }
        pl->x = 4597; pl->z = -25030; pl->rot_y = 3072;        /* Blick nach Norden, weg von der Diagonale */
        for (int f = 0; f < 30; f++) {
            const int32_t ax = pl->x, az = pl->z;
            e->x = 1960; e->z = -23400; pl->hp = 100; pl->hit_react = 0;
            bild(RE15_PAD_BIT_UP);
            weg += abs((int)(pl->x - ax)) + abs((int)(pl->z - az));
        }
        fprintf(stderr, "N1: Ecke #29/#30 unter dem Koerper, 30 Bilder Vorwaerts: Weg %d, Ende (%d,%d)\n", (int)weg, (int)pl->x, (int)pl->z);
        CHECK(weg >= 200, "N1: Eingabe wirkt in der Ecke nicht (Weg %d)", (int)weg);
    }

    lagen_bauen(-900, 20400, -26500, -20340, 300);
    fprintf(stderr, "g5: %d wandnahe Startlagen\n", s_lagen);
    CHECK(s_lagen >= 150, "zu wenige Startlagen: %d", s_lagen);
    static const int32_t boss_x[] = { -1500, 1960, 4000, 6000, 8000, 10000, 12000 };
    int n_faelle = 0, n_wandbilder = 0, n_eingekeilt = 0, laengste = 0, n_griff = 0, n_gedrueckt = 0, max_warte = 0;
    int32_t kleinster_lauf = 0x7fffffff;
    for (int i = 0; i < s_lagen; i++) {
        /* aus dem Griff des Vorfalls heraus */
        for (int f = 0; f < 240 && (re15_player_is_grabbed() || re15_player_hit_flinch_active() || re15_player_knockdown_active()); f++) { pl->hp = 100; bild(0); }
        e->x = boss_x[zufall((unsigned)(sizeof boss_x / sizeof boss_x[0]))];
        e->hp = 600;
        pl->x = s_lage[i][0]; pl->z = s_lage[i][1]; pl->y = 0; pl->hp = 100; pl->rot_y = (int16_t)zufall(4096);
        uint16_t pad = 0; int folge = 0;
        for (int f = 0; f < 300; f++) {
            if ((i & 1) && f % 24 == 0) {            /* jede zweite Lage: Zufallseingaben, Laufen und Drehen */
                static const uint16_t tasten[6] = { 0, RE15_PAD_BIT_UP, RE15_PAD_BIT_DOWN, RE15_PAD_BIT_LEFT, RE15_PAD_BIT_RIGHT,
                                                    RE15_PAD_BIT_UP | RE15_PAD_BIT_LEFT };
                pad = tasten[zufall(6)];
            }
            pl->hp = 100;
            if ((i & 3) == 2 && f == 20 && re15_g5_opfer_variante() < 0 && !re15_player_is_grabbed()) re15_g5_opfer_start(pl, (i >> 2) & 3);   /* Griff erzwingen: Zug 800, Peitsche, Spiess 600 */
            bild(pad);
            if (re15_g5_opfer_variante() >= 0) n_griff++;
            if (fest(pl->x, pl->z)) { n_wandbilder++; if (++folge > laengste) laengste = folge; if (n_wandbilder <= 3)
                fprintf(stderr, "  Wandbild: Start (%d,%d) Bild %d Lage (%d,%d) Zelle %d Boss x %d\n", (int)s_lage[i][0], (int)s_lage[i][1], f, (int)pl->x, (int)pl->z, fest(pl->x, pl->z) - 1, (int)e->x); }
            else folge = 0;
        }
        for (int f = 0; f < 240 && (re15_player_is_grabbed() || re15_player_hit_flinch_active() || re15_player_knockdown_active()); f++) { pl->hp = 100; bild(0); }
        {   const int32_t kx = pl->x, kz = pl->z;
            int32_t weg = laufprobe(1);
            if (weg < 300) {
                /* Der Koerper ist ein Kreis von 6450: steht Leon in einer Nische hinter ihm, drueckt der Schub ihn in die Ecke, bis der G5
                 * weiterzieht. Das ist kein Fehler der Klemme, also warten; was danach noch steht, ist gefangen. */
                int warte = 0;
                while (warte < 2400 && g5_drueckt(pl)) { pl->hp = 100; bild(0); warte++; }
                if (warte > max_warte) max_warte = warte;
                n_gedrueckt++;
                weg = laufprobe(1);
            }
            if (weg < kleinster_lauf) kleinster_lauf = weg;
            if (weg < 300) { n_eingekeilt++; if (n_eingekeilt <= 3) fprintf(stderr, "  eingekeilt: Start (%d,%d) Lage (%d,%d) Weg %d Boss x %d\n", (int)s_lage[i][0], (int)s_lage[i][1], (int)kx, (int)kz, (int)weg, (int)e->x); }
        }
        n_faelle++;
    }
    fprintf(stderr, "g5: %d Faelle, Griff-Bilder %d, Bildenden in fester Zelle %d (laengste Folge %d), eingekeilt %d, kleinster Laufweg %d, vom Koerper gedrueckt %d (laengste Wartezeit %d Bilder)\n",
            n_faelle, n_griff, n_wandbilder, laengste, n_eingekeilt, (int)kleinster_lauf, n_gedrueckt, max_warte);
    CHECK(n_faelle >= 300, "zu wenige Faelle: %d", n_faelle);
    CHECK(n_griff > 0, "kein Griff der Arme im Fuzz, der Riegel misst ihn nicht");
    CHECK(n_wandbilder == 0, "%d Bildenden in fester Zelle", n_wandbilder);
    CHECK(n_eingekeilt == 0, "%d Faelle eingekeilt", n_eingekeilt);
}

/* --- griff: EM30-Griff und Wurf aus allen Wandlagen ---------------------------------------------------------------------------- */
static void griff_raum(const char *rel, unsigned id, int32_t kx, int32_t kz, int32_t bx, int32_t bz, int32_t x0, int32_t x1, int32_t z0, int32_t z1, int raster)
{
    if (!raum_boot(rel, id, kx, kz, 0)) { CHECK(0, "%s", rel); return; }
    if (!em30_bank()) { CHECK(0, "EM30-Bank"); return; }
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    int bs = re15_actor_alloc(0x30u);
    CHECK(bs > 0, "kein Aktor");
    if (bs <= 0) return;
    re15_actor_t *e = &g_actors[bs], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->active = 1; e->type = 0x30u; e->x = bx; e->y = 0; e->z = bz; e->hp = 400; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x30u);
    re15_enemy_ai_set_paused(1);                                  /* nur der Haken: die KI soll den Griff nicht ueberlagern */
    CHECK(re15_birkin_klemme_aktiv(), "Birkin-Klemme nicht aktiv");

    lagen_bauen(x0, x1, z0, z1, raster);
    int n_faelle = 0, n_wandbilder = 0, n_eingekeilt = 0, n_wandhalt = 0, laengste = 0, n_griff5 = 0, n_wurf = 0;
    int32_t weg_max = 0;
    for (int i = 0; i < s_lagen; i++) {
        for (int k = 0; k < ((id >> 12) == 5u ? 12 : 8); k++) {
            /* k 0..7: cmd 5 (Griff des 0x30), Haken [0] von vorn, [1] von hinten. k 8..11: cmd 6, Wurf: EM30 @0x8011defc und die Form (bb_griff).
             * STAGE3 hat fuer cmd 6 nur die Wurf-Tod-Maschine (re15_bk_opfer_start Variante 3), die endet nicht im freien Stand. */
            const int wurf = (k >= 8);
            const int v = wurf ? 0 : (k & 1);
            pl->x = s_lage[i][0]; pl->z = s_lage[i][1]; pl->y = 0; pl->hp = 100;
            pl->state = 1; pl->sub_state_1 = 0; pl->sub_state_2 = 0; pl->hit_react = 0; pl->anim_flags = 0;
            pl->rot_y = (int16_t)((((wurf ? k - 8 : k >> 1)) * 1024 + (int)zufall(200)) & 0xfff);
            if (!re15_bk_opfer_start(e, wurf ? 6 : 5, (uint8_t)v)) { CHECK(0, "Griff startet nicht"); return; }
            int folge = 0, phase5 = 0;
            for (int f = 0; f < 420 && re15_bk_opfer_cmd(); f++) {
                const int32_t ax = pl->x, az = pl->z;
                pl->hp = 100;
                bild((uint16_t)(f & 1 ? RE15_PAD_BIT_CROSS : 0));        /* Ruetteln: kuerzt den Ringkampf */
                {   const int32_t d = abs((int)(pl->x - ax)) + abs((int)(pl->z - az)); if (d > weg_max) weg_max = d; }
                if (re15_bk_opfer_phase() >= 5 && re15_bk_opfer_phase() <= 8 && v == 0 && !wurf) phase5 = 1;
                if (fest(pl->x, pl->z)) {
                    n_wandbilder++; if (++folge > laengste) laengste = folge;
                    if (n_wandbilder <= 3) fprintf(stderr, "  Wandbild %s: Start (%d,%d) Bild %d Lage (%d,%d) Zelle %d Gier %d\n", rel, (int)s_lage[i][0], (int)s_lage[i][1], f, (int)pl->x, (int)pl->z, fest(pl->x, pl->z) - 1, (int)pl->rot_y);
                } else folge = 0;
            }
            n_wandhalt += phase5;
            if (wurf) n_wurf++; else n_griff5++;
            CHECK(!re15_bk_opfer_cmd(), "%s: Griff endet nicht", rel);
            for (int f = 0; f < 60 && re15_player_hit_flinch_active(); f++) { pl->hp = 100; bild(0); }
            {   const int32_t kx2 = pl->x, kz2 = pl->z;
                const int32_t weg = laufprobe(1);
                if (weg < 300) { n_eingekeilt++; if (n_eingekeilt <= 3) fprintf(stderr, "  eingekeilt %s: Start (%d,%d) Lage (%d,%d) Weg %d\n", rel, (int)s_lage[i][0], (int)s_lage[i][1], (int)kx2, (int)kz2, (int)weg); }
            }
            n_faelle++;
        }
    }
    re15_enemy_ai_set_paused(0);
    fprintf(stderr, "griff %s: %d Lagen, %d Griffe, %d Wuerfe, Wand-Griffe %d, Bildenden in fester Zelle %d (Folge %d), eingekeilt %d, groesster Weg %d\n",
            rel, s_lagen, n_griff5, n_wurf, n_wandhalt, n_wandbilder, laengste, n_eingekeilt, (int)weg_max);
    CHECK(n_faelle >= 60, "%s: zu wenige Faelle %d", rel, n_faelle);
    /* Der Griff-Haken liest das Wand-Bit (@0x8011b140): von den Griffen [0] muss ein guter Teil an die Wand laufen. Ohne Bit aus der Vor-Klemme: 2 Prozent. */
    CHECK(n_wandhalt * 5 >= n_griff5, "%s: nur %d von %d Griffen liefen an die Wand (Wand-Bit)", rel, n_wandhalt, n_griff5);
    CHECK(n_wandbilder == 0, "%s: %d Bildenden in fester Zelle", rel, n_wandbilder);
    CHECK(n_eingekeilt == 0, "%s: %d Faelle eingekeilt", rel, n_eingekeilt);
}

static void test_griff(void)
{
    griff_raum("STAGE5/ROOM5090.RDT", 0x5090u, 8000, -23500, -14700, -23350, -900, 20400, -26500, -20340, 300);
    griff_raum("STAGE5/ROOM5091.RDT", 0x5091u, 8000, -23500, -14700, -23350, -900, 20400, -26500, -20340, 300);
    griff_raum("STAGE5/ROOM5080.RDT", 0x5080u, -18000, -18000, -8000, -8000, -26400, -9800, -26500, -9400, 400);
    griff_raum("STAGE5/ROOM5081.RDT", 0x5081u, -18000, -18000, -8000, -8000, -26400, -9800, -26500, -9400, 400);
    griff_raum("STAGE3/ROOM3070.RDT", 0x3070u, -9000, -10000, -3000, -3000, -26400, -5400, -18200, -2400, 400);
    griff_raum("STAGE3/ROOM3071.RDT", 0x3071u, -9000, -10000, -3000, -3000, -26400, -5400, -18200, -2400, 400);
}

int main(int argc, char **argv)
{
    const char *m = (argc > 1) ? argv[1] : "rein";
    if (!pl00_laden()) { printf("FEHLT: PL00\n"); return 77; }
    if (!strcmp(m, "rein"))  { test_rein(); test_nclip(); }
    if (!strcmp(m, "g5"))    { test_g5("STAGE5/ROOM5090.RDT", 0x5090u); test_g5("STAGE5/ROOM5091.RDT", 0x5091u); }
    if (!strcmp(m, "griff")) test_griff();
    if (fails) { fprintf(stderr, "test_r41_birkin_wand %s: %d FEHLER\n", m, fails); return 1; }
    fprintf(stderr, "test_r41_birkin_wand %s: OK\n", m);
    return 0;
}
