/* test_r39_gegner_querung.c — RIEGEL Runde 39 Spur E "gegner", Nachbesserung 3 (Befund E, ROOM1141).
 *
 * Befund E (Nachbesserung 2, Versuche 5/6): die RE1.5-Wandklemme FUN_8003b0a4 -> FUN_8003bca8 nimmt die NAEHERE der
 * um den Radius aufgeblasenen Kanten (@0x8003bcec-cfc). Steht ein Aktor nach Stoss/Platzierung jenseits der Wandmitte,
 * landet er AUSSERHALB des Raums:
 *   (a) Gorilla gegen Gorilla (Koerper-Stoss FUN_8002aec4, R 3200, pen 2829) an der 2000 dicken Westwand (Zelle 0,
 *       x -10600..-8600): G3 x -6982 -> -9803 -> Klemme -12218 => Kampf endet nie (Softlock, Kampf ist Port-Pflicht).
 *   (b) Elza im Gorilla-Wurf (Opfer-Clip 1, Platzierung ad68 @0x8011c294) an der Suedwand (Zelle 1, z -25600..-23600):
 *       Platzierung z -24973 -> Klemme -26068 => Spielerin ausserhalb (Runde-38-T-Invariante verletzt).
 * Port (gegner_r39.c (5)): nach der RE1.5-Klemme die RE2-Retail-Regel FUN_8004c900 (Seite nach der Vorbild-Lage
 * +0x118/+0x11a, @0x8004c930-ca50) fuer eine DURCHQUERTE solide Rechteckzelle; Rest-Querung -> Vorbild-Lage.
 *
 * Jeder Teil misst zuerst "vorher" (re15_r39_gegner_enable(0) = Haken durchsichtig) und dann "nachher" im selben Lauf.
 * Harness = test_r39_gegner_1141.c (echter Raumaufbau ROOM1141 mit Elza, Kampf-Lage sub00 @0x00B54/@0x00B68, Banken aus
 * CDEMD1.EMS, re15_game_step).
 *
 * Teile (Aufruf: test_r39_gegner_querung <teil>):
 *   wand1141  (a) die Lage aus Versuch 5 an der Westwand (Stossender 371 oestlich, 50 noerdlich): ein echtes Spielbild
 *             (Stoss im Tick des Gestossenen + Klemme + Wache). Vorher x <= -12200 (ausserhalb, bleibt 90 Bilder draussen),
 *             nachher x = -9600 + 2601 = -6999 (RE2: Mitte + r + w/2 + 1) und 90 Bilder im Raum.
 *   frei1141  Gegenprobe: dieselbe Paar-Lage im freien Raum (kein Wandkontakt): 60 Bilder Bild fuer Bild gleich mit
 *             Haken aus/an (die Wache greift nur bei einer Querung).
 *   wurf1141  (b) die Wurf-Lage aus Versuch 6 (Gorilla (-2066,-21976) Blick 389 im Pin-Latch Sub 15 Phase 2, Elza
 *             (-362,-23142)): Runde-38-T-Invariante je Wurfbild (Raumhuelle, keine Strecke durch eine solide Zelle seit dem
 *             Griff, eigener Slab-Test). Vorher Elza hinter der Suedwand (z -26068), nachher in jedem Bild im Raum.
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
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_door_seq.h"
#include "re15_inventory.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_esp.h"
#include "re15_gegner_r39.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
extern void re15_player_aim_reset(void);
extern int  re15_player_victim_state(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (Muster test_r39_gegner_1141.c) ------------------------------------------------------------------ */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    return 0;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

static uint8_t s_bank[2][0x60000];
static int bank_laden(const char *archiv, uint8_t typ, int platz)
{
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/%s", RE15_ASSET_PSX_DIR, archiv);
    uint8_t *ems = slurp(p, &n);
    if (!ems) { printf("  FEHLER: %s fehlt\n", archiv); g_fail++; return 0; }
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0 || len > sizeof s_bank[0]) {
        printf("  FEHLER: Bank 0x%02X nicht in %s\n", typ, archiv); g_fail++; free(ems); return 0; }
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) { free(ems); return 0; }
    uint8_t *bank = s_bank[platz % 2];
    memcpy(bank, ems + off, len);
    free(ems);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(bank, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(bank, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(bank, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(bank, len, &eb->skel_own, &eb->anim_own) == 0);
    return 1;
}

/* Kampf-Lage ROOM1141 ohne Szene (wie test_r39_gegner_1141.c kampf_lage): (4,0xF2) = 1 -> sub00 Else-Zweig, Sce_em_set
 * @0x00B54 / @0x00B68 grid 0x10; Elza (Figur 4) an der Doppeltuer. */
static int kampf_lage(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0xF2, 1);
    if (rdt_laden(0x1141) != 0) { printf("  FEHLER: ROOM1141 nicht ladbar\n"); g_fail++; return 0; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    (void)bank_laden("CDEMD1.EMS", 0x27, 0);
    re15_discard_reset();
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    g_letterbox_level = 0;
    g_current_room_id = 0x1141; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = 4;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -7600; pl->z = -17600; pl->rot_y = 0;
    re15_collision_set_band(0);
    s_shown = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < 2; f++) frame();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x27) n++;
    return n == 2;
}

static int gorilla_slot(int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x27) { if (k == nth) return s; k++; }
    return -1;
}

static void ruhig(re15_actor_t *g)
{
    /* Sub 0 Idle-Wander ohne Entscheid (+0x1dc != 0 -> kein Entscheid @0x80117490-98), wie test_r39_gegner.c */
    g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
    g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0; g->dog_blocked_ctr = 30000;
}

/* Raumhuelle (alle SCA-Zellen) + unabhaengiger Slab-Test gegen das Innere solider Rechteckzellen (Typ 1, Maske m) —
 * dieselbe Invariante wie test_r38_griff_alle.c (Runde 38 T), NICHT der Pruefling. */
static int32_t s_hx0, s_hx1, s_hz0, s_hz1;
static void huelle(void)
{
    s_hx0 = s_hz0 = 0x7fffffff; s_hx1 = s_hz1 = -0x7fffffff;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        int32_t x0 = c->x, z0 = c->z, x1 = x0 + (int32_t)c->width, z1 = z0 + (int32_t)c->density;
        if (x0 < s_hx0) s_hx0 = x0;
        if (z0 < s_hz0) s_hz0 = z0;
        if (x1 > s_hx1) s_hx1 = x1;
        if (z1 > s_hz1) s_hz1 = z1;
    }
}
static int im_raum(int32_t x, int32_t z) { return x >= s_hx0 && x <= s_hx1 && z >= s_hz0 && z <= s_hz1; }
static int durch_wand(int32_t x0, int32_t z0, int32_t x1, int32_t z1, unsigned maske)
{
    if (x0 == x1 && z0 == z1) return 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((c->floor >> 4) != 0 || !(c->u0 & maske) || c->type != 1) continue;
        double ax = c->x, az = c->z, bx = ax + c->width, bz = az + c->density;
        double lo = 0, hi = 1, px = x0, pz = z0, dx = (double)x1 - x0, dz = (double)z1 - z0;
        int ok = 1;
        if (dx == 0) { if (px <= ax || px >= bx) ok = 0; }
        else { double t0 = (ax - px) / dx, t1 = (bx - px) / dx; if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
               if (t0 > lo) lo = t0; if (t1 < hi) hi = t1; if (!(lo < hi)) ok = 0; }
        if (ok) {
            if (dz == 0) { if (pz <= az || pz >= bz) ok = 0; }
            else { double t0 = (az - pz) / dz, t1 = (bz - pz) / dz; if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
                   if (t0 > lo) lo = t0; if (t1 < hi) hi = t1; if (!(lo < hi)) ok = 0; }
        }
        if (ok) return 1;
    }
    return 0;
}

/* ================================================================== wand1141 ================================ */
/* Westwand Zelle 0: x -10600..-8600 (w 2000), Mitte -9600; Gorilla-Radius 1600 (Kasten @0x80121350). Aufgeblasene
 * Innenkante -8600 + 1600 = -7000, Aussenkante -10600 - 1600 = -12200 (RE1.5 +18 Haut: -6982 / -12218). */
static void teil_wand1141(void)
{
    printf("== wand1141 ==\n");
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        int ok = kampf_lage();
        PRUEF(ok, "[%s] ROOM1141 Kampf-Lage: zwei Gorillas 0x27", tag);
        if (!ok) continue;
        const int sa = gorilla_slot(0), sb = gorilla_slot(1);
        re15_actor_t *a = &g_actors[sa], *b = &g_actors[sb];   /* a tickt zuerst = der Gestossene (b544 im eigenen Tick) */
        ruhig(a); ruhig(b);
        a->x = -6982; a->z = -9915; b->x = -6611; b->z = -9865;   /* Versuch 5: dx -371, Stossender sued-noerdlich versetzt */
        a->rot_y = 0; b->rot_y = 0;
        printf("  [%s] Gestossener slot %d (%d,%d) r%d, Stossender slot %d (%d,%d) r%d, Elza (%d,%d)\n", tag, sa, (int)a->x,
               (int)a->z, (int)a->hit_radius_min, sb, (int)b->x, (int)b->z, (int)b->hit_radius_min,
               (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        frame();
        printf("  [%s] nach 1 Bild: Gestossener (%d,%d) Kontakt +0x90 0x%02x, Stossender (%d,%d)\n", tag, (int)a->x, (int)a->z,
               (unsigned)a->ai_contact, (int)b->x, (int)b->z);
        int draussen = 0, drinnen = 0;
        int32_t xmin = a->x, xmax = a->x;
        for (int f = 0; f < 90; f++) {
            if (f) frame();
            if (a->x < xmin) xmin = a->x;
            if (a->x > xmax) xmax = a->x;
            if (a->x <= -10600) draussen++;        /* westlich der Westwand = ausserhalb des Raums */
            if (a->x >= -8600) drinnen++;
        }
        if (!an) {
            PRUEF(xmax <= -12200, "[vorher] RE1.5-Klemme FUN_8003bca8 (naehere Kante @0x8003bcec-cfc): Gestossener hinter der "
                  "Westwand, 90/90 Bilder x <= -12200 (gemessen %d Bilder draussen, x %d..%d) = Befund E (a)", draussen,
                  (int)xmin, (int)xmax);
        } else {
            PRUEF(drinnen == 90 && xmin >= -7000, "[nachher] RE2 FUN_8004c900 (Seite nach der Vorbild-Lage): Gestossener bleibt "
                  "im Raum, 90/90 Bilder x >= -7000 (gemessen %d drinnen, x %d..%d)", drinnen, (int)xmin, (int)xmax);
        }
    }
    re15_r39_gegner_enable(1);
    /* Kern direkt (Sonde): Versuch-5-Zahlen nach der RE1.5-Klemme -> RE2-Seite. */
    if (kampf_lage()) {
        int32_t x = -12218, z = -9848;
        int k = re15_r39_querung_sonde(1600, 0, 4u, -6982, -9649, &x, &z);
        PRUEF(k == 1 && x == -6999 && z == -9848, "Sonde Versuch 5: Vorbild (-6982,-9649), RE1.5 (-12218,-9848) -> (%d,%d), soll "
              "(-6999,-9848) = Mitte -9600 + (1600 + 1000 + 1) (@0x8004c9e0-ca04)", (int)x, (int)z);
        x = -6982; z = -9848;
        k = re15_r39_querung_sonde(1600, 0, 4u, -6982, -9649, &x, &z);
        PRUEF(k == 0 && x == -6982, "Sonde: RE1.5-Ergebnis auf der Herkunftsseite (Innenkante -6982) bleibt unberuehrt");
    }
}

/* ================================================================== frei1141 ================================ */
typedef struct { int32_t ax, az, bx, bz, px, pz; } paar_t;
static void teil_frei1141(void)
{
    printf("== frei1141 ==\n");
    enum { N = 60 };
    static paar_t v[2][N];
    for (int an = 0; an <= 1; an++) {
        re15_r39_gegner_enable(an);
        if (!kampf_lage()) { PRUEF(0, "Kampf-Lage"); return; }
        re15_actor_t *a = &g_actors[gorilla_slot(0)], *b = &g_actors[gorilla_slot(1)];
        ruhig(a); ruhig(b);
        a->x = 1129; a->z = -20550; b->x = 1500; b->z = -20500;   /* dieselbe Versetzung (-371,-50), keine Zelle in Reichweite */
        a->rot_y = 0; b->rot_y = 0;
        for (int f = 0; f < N; f++) {
            frame();
            v[an][f].ax = a->x; v[an][f].az = a->z; v[an][f].bx = b->x; v[an][f].bz = b->z;
            v[an][f].px = g_actors[RE15_ACTOR_SLOT_PLAYER].x; v[an][f].pz = g_actors[RE15_ACTOR_SLOT_PLAYER].z;
        }
        printf("  [%s] Bild 1: Gestossener (%d,%d), Stossender (%d,%d)\n", an ? "nachher" : "vorher", (int)v[an][0].ax,
               (int)v[an][0].az, (int)v[an][0].bx, (int)v[an][0].bz);
    }
    int erstes = -1;
    for (int f = 0; f < N && erstes < 0; f++) if (memcmp(&v[0][f], &v[1][f], sizeof v[0][f]) != 0) erstes = f;
    PRUEF(v[0][0].ax < 1129 - 2000, "der Stoss findet statt (Gestossener %d -> %d)", 1129, (int)v[0][0].ax);
    PRUEF(erstes < 0, "Stoss im freien Raum: %d Bilder Gorillas + Spielerin gleich mit Haken aus/an (erste Abweichung %d)", N, erstes);
    re15_r39_gegner_enable(1);
}

/* ================================================================== wurf1141 ================================ */
static void teil_wurf1141(void)
{
    printf("== wurf1141 ==\n");
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        if (!kampf_lage()) { PRUEF(0, "[%s] Kampf-Lage", tag); continue; }
        huelle();
        re15_actor_t *g = &g_actors[gorilla_slot(0)];
        g_actors[gorilla_slot(1)].active = 0;                    /* nur der Greifer (wie test_r38_griff_alle.c) */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* Versuch 6, Bild 3531: G3 (-2066,-21976) Blick 389, Sub 15 Phase 2 (Verbindung bei Clip-0x1c-Bild 15 @0x8011aafc),
         * Elza (-362,-23142) Blick 3735. */
        pl->x = -362; pl->z = -23142; pl->rot_y = 3735; pl->hit_react = 0; pl->hp = 100;
        g->x = -2066; g->z = -21976; g->rot_y = 389; g->mag_airborne = 0; g->floor = 0;
        g->y = (int32_t)g->dog_floor_y + 1600;                   /* Rear-up +0x1ba + 0x640 @0x8011a9b4-c0 */
        g->state = 1; g->sub_state_1 = 15; g->sub_state_2 = 2; g->sub_state_3 = 0;
        g->motion = 0x1c; g->anim_frame = 15; g->anim_frac = 0; g->hit_react = 1;
        const int32_t vor_x = pl->x, vor_z = pl->z;
        int latch = -1, wurfbilder = 0, aus = 0, durch = 0;
        int32_t zmin = pl->z, ax = 0, az = 0;
        for (int f = 0; f < 400; f++) {
            if (pl->hp < 100) pl->hp = 100;
            frame();
            int v = re15_player_victim_state();
            if (latch < 0 && v != 0) { latch = f; ax = pl->anchor_x; az = pl->anchor_z; }
            if (latch >= 0 && v != 0) {
                wurfbilder++;
                if (!im_raum(pl->x, pl->z)) aus++;
                if (durch_wand(vor_x, vor_z, pl->x, pl->z, 1u)) {
                    if (durch < 2) printf("  [%s] Bild %d: Elza (%d,%d) hinter einer Wand (vor dem Griff (%d,%d))\n", tag, f,
                                          (int)pl->x, (int)pl->z, (int)vor_x, (int)vor_z);
                    durch++;
                }
                if (pl->z < zmin) zmin = pl->z;
            }
            if (latch >= 0 && v == 0) break;
        }
        printf("  [%s] Latch Bild %d, Anker (%d,%d), %d Wurfbilder, kleinstes z %d, Ende (%d,%d)\n", tag, latch, (int)ax, (int)az,
               wurfbilder, (int)zmin, (int)pl->x, (int)pl->z);
        PRUEF(latch >= 0 && wurfbilder > 20, "[%s] Pin-Latch (Sub 15 Phase 2 @0x8011abe8) und Wurf (Opfer-Clip 1) laufen", tag);
        if (!an) {
            PRUEF(aus > 0 && zmin <= -26068, "[vorher] Befund E (b): Elza im Wurf ausserhalb des Raums (%d Bilder ausserhalb der Huelle, "
                  "kleinstes z %d; Aussenkante -25600 - 450 - 18 = -26068)", aus, (int)zmin);
        } else {
            PRUEF(aus == 0, "[nachher] Elza in jedem Wurfbild in der Raumhuelle (%d ausserhalb)", aus);
            PRUEF(durch == 0, "[nachher] kein Wurfbild hinter einer Wand (Runde-38-T-Invariante): %d", durch);
            PRUEF(zmin >= -23150 - 1, "[nachher] kleinstes z %d >= Innenkante (RE2 -24600 + 1451 = -23149)", (int)zmin);
        }
    }
    re15_r39_gegner_enable(1);
}

static int s_n_seq = 0;
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; s_n_seq++; }

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "wand1141")) teil_wand1141();
    if (alle || !strcmp(teil, "frei1141")) teil_frei1141();
    if (alle || !strcmp(teil, "wurf1141")) teil_wurf1141();
    re15_door_seq_setze_laeufer(NULL);
    re15_r39_gegner_enable(1);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
