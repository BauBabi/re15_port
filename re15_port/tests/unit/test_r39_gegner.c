/* test_r39_gegner.c — RIEGEL Runde 39 Spur E "gegner".
 *
 * Nutzer (AUFTRAG.md Runde 39):
 *  1. "Die Gorillas beim Parking Lot koennen ruhig doppelt so viel Energie haben. Ausserdem sollten sie erst die
 *     flinch Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden."
 *  2. "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen
 *     Hunden angegriffen/gebissen werden koennen" (Room "1090" = ROOM1190, Hunde-Luke).
 *  3. "Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original."
 *  4. "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim" (Schatten haengt beim Sprung mit).
 * Dossier analysis/befunde_runde39/E_gegner.md, Code engine/src/gegner_r39.c + include/re15_gegner_r39.h.
 *
 * Teile (Aufruf: test_r39_gegner <teil>); jeder Teil misst zuerst "vorher" (re15_r39_gegner_enable(0) = die
 * Haken dieser Spur sind durchsichtig) und dann "nachher" im selben Lauf:
 *   affen_hp      ROOM11C0 Kampf-Layout: beide 0x27 starten mit 360 (vorher 180 = @0x8011f514); seit Nachbesserung 2 jeder
 *                 0x27 in jedem Raum (ROOM1141 mit Elza: test_r39_gegner_1141.c), 0x29 nicht
 *   affen_zucken  Treffer wie der Resolver (@0x80012fd4-13020, 12 Energie @0x8006ee44): vorher Flinch (Clip 7) bei
 *                 jedem Treffer; nachher Treffer 1/2 ohne Clip 7 (Zustand 1, Unterzustaende bleiben, +0x93 = 0,
 *                 Blut = ESP-Effekt), Treffer 3 Flinch Clip 7 -> Exit Sub 7 (Runde 35: 3 Treffer -> Sprung);
 *                 Schrotflinte (38 @0x8006ee38+8*4) flincht sofort
 *   affen_schuss  dieselbe Kette ueber den ECHTEN Pistolenschuss (Pad R1 + Kreuz, re15_player_weapon_fire):
 *                 hp 360 -> 348 -> 336 -> 324, Flinch erst beim dritten Schuss
 *   hund_riegel   zwei RE2-Hunde im Flug am Spieler (Muster test_re2_dog_jaw_contact): vorher beissen beide im
 *                 selben Bild (-40); nachher beisst nur der erste (-20), PL+0x1D3 |= 0x80 (@0x801051f8-0x80105200),
 *                 der zweite kehrt am Tor @0x80104e5c-68 um; nach dem Ende der Spieler-Reaktion (+0x93 Bit 1 weg,
 *                 @0x80031964 / RE2 @0x8003f8b4) ist der Riegel frei und ein Hund beisst wieder
 *   kriecher_1010 ROOM1010 Cut 4 (Suedtuer), RE2-KI, beide 0x81-Kriecher: fern (3510) liegen sie (Sub 2, Clip 23);
 *                 Spieler in 2500 vor dem Kriecher: vorher liegt er weiter, nachher Wort 1 (Kriech-Lokomotion,
 *                 Clip 5) und er kommt naeher (RE1.5 Block B @0x80104718-48)
 *   kriecher_1220 dasselbe fuer die zwei 0x16-Kriecher in ROOM1220 (Elza-Raum 1221 = gleiche Spawns @0x00F2A/0x00F3E)
 *   schatten      Quad-Hoehe des NPC-Schattens: 0x27/0x29 mit Koerper-Y ueber dem Boden -> +0x1ba (dog_floor_y);
 *                 Hund/Zombie unveraendert; vorher Koerper-Y. Szenen-Gorillas (grid 0x30, geparkt y=-20000 / im Wagen
 *                 y=-2500) ebenso auf +0x1ba = -3600 (der Schwanz @0x80116f20-38 laeuft auf jedem Pfad; Nachbesserung 1).
 *                 Region-Cull wie das Original: Region-Viereck des RDT je Cut 0/1/3/4/5/12/13 == DAT_800ac790 der
 *                 Runde-35-Savestates, Punkt-im-Viereck (FUN_80014368) fuer beide Szenen-Positionen == Original-RAM
 *   hund_re15     RE1.5-Flavor: zwei RE1.5-Hunde im selben Biss-Bild — das Original gatet den Biss auf acae7 == 0
 *                 (@0x8010f294) vor dem Schaden (@0x8010f2c4-d0); nur der erste zieht -10 ab. Kein Haken beteiligt.
 *   kriecher_1010_re15 / kriecher_1220_re15  dieselbe Kriecher-Messung im RE1.5-Flavor: Block B ist dort nativ
 *                 (DECIDE[5] Sub 5 -> Wort 1, Kriech-Clip 0x1A), vorher wie nachher
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
#include "re2_ems.h"
#include "re15_esp.h"
#include "re15_affen.h"
#include "re15_ai_flavor.h"
#include "re15_gegner_r39.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void re15_actor_step_all_walkers(void);
void re15_player_cmd_zero(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

/* ---- Raum-Harness (Muster test_r38_affen3.c / test_r35_affen.c: Bildfolge wie platform/pc/main.c) ------------ */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    return 0;
}

static void frame(uint16_t held, uint16_t edge)
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
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, int band, uint8_t cut, int bilder)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(band);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
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

static double dist2d(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    double dx = (double)x0 - x1, dz = (double)z0 - z1; return sqrt(dx * dx + dz * dz);
}

/* RE2-Bank (EM010/EM016/EM020) aus shared_assets/RE2/CDEMD0.EMS (Muster probe_r16_kriecher1010.c) */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* EM027-Bank aus CDEMD0.EMS (wie test_r35_affen.c bank_laden_27) */
static int bank_re15(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return 0; }
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { free(ems); return 0; }
    uint8_t *blob = (uint8_t *)malloc(len);
    if (!blob) { free(ems); return 0; }
    memcpy(blob, ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    int ok = (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
    if (ok) { eb->ok = 1; eb->buf = blob;
              eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0); }
    else { eb->type = 0; free(blob); }
    free(ems);
    return ok;
}

/* ================================================================== 1a: HP ================================ */
static void teil_affen_hp(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);                 /* Kampf-Layout (grid 0x10) wie test_r35_affen.c */
    for (int an = 0; an <= 1; an++) {
        re15_r39_gegner_enable(an);
        if (room_boot(0x11C0, -9013, 0, -15461, 0, 5, 3, 3) != 0) return;
        re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
        PRUEF(a && b, "[%s] ROOM11C0: zwei Gorillas 0x27 aktiv", an ? "nachher" : "vorher");
        if (!a || !b) continue;
        int soll = an ? 360 : 180;
        PRUEF(a->hp == soll && b->hp == soll,
              "[%s] ROOM11C0 HP beider Gorillas = %d (gemessen %d / %d; Tabelle @0x8011f514 = 180, Saat @0x80117104)",
              an ? "nachher" : "vorher", soll, (int)a->hp, (int)b->hp);
        PRUEF(re15_r39_affen_akku(a) == 0, "[%s] Energie-Akku startet bei 0", an ? "nachher" : "vorher");
    }
    /* Nachbesserung 2 (Nutzer-Nachtrag 3 "generell"): ROOM1141 (dieselben 0x27, grid 0x30/0x10) verdoppelt ebenso —
     * die Regel haengt am Typ, nicht am Raum (Raum-Messung mit Elza: test_r39_gegner_1141.c) */
    re15_r39_gegner_enable(1);
    g_current_room_id = 0x1141;
    re15_actor_t dummy; memset(&dummy, 0, sizeof dummy); dummy.type = 0x27;
    PRUEF(re15_r39_affen_hp_init(&dummy, 180) == 360, "ROOM1141: 2 x 180 = 360 (NUTZER-VORGABE, alle Gorillas)");
    g_current_room_id = 0x11C0;
    PRUEF(re15_r39_affen_hp_init(&dummy, 180) == 360, "ROOM11C0: 2 x 180 = 360 (NUTZER-VORGABE)");
    dummy.type = 0x29;
    PRUEF(re15_r39_affen_hp_init(&dummy, 105) == 105 && !re15_r39_affen_regel(&dummy), "kleiner Affe 0x29: Regel aus, Tabellenwert bleibt");
}

/* ================================================================== 1b: Flinch-Schwelle (Resolver-Schreibsatz) == */
static void treffer_resolver(re15_actor_t *g, int zeile, int dmg)
{
    /* wie re15_player_weapon_fire / re15_enemy_take_damage (@0x80012fd4-13020): +0x5 = Waffen-Zeile, +0x6 = 1,
     * +0x7 = 0, hp -= dmg, +0x93 |= 1, +0x4 = 2 (hp >= 0) */
    g->sub_state_1 = (uint8_t)zeile; g->sub_state_2 = 1; g->sub_state_3 = 0;
    g->hp = (int16_t)(g->hp - dmg); g->hit_react |= 1;
    g->state = (g->hp >= 0) ? 2 : 3;
}

static re15_actor_t *gorilla_parken_einen(void)
{
    re15_actor_t *g = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    if (g) {
        /* ruhig halten: Sub 0 Idle-Wander ohne Entscheid (+0x1dc != 0 -> kein Entscheid @0x80117490-98) */
        g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0;
        g->dog_blocked_ctr = 30000; g->hit_react = 0; g->mag_hit_ctr = 0;
    }
    return g;
}

static void teil_affen_zucken(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        if (room_boot(0x11C0, -9013, 0, -15461, 0, 5, 3, 3) != 0) return;
        (void)bank_re15(0x27);
        re15_actor_t *g = gorilla_parken_einen();
        PRUEF(g != NULL, "[%s] Gorilla 1 geparkt (Sub 0, kein Entscheid)", tag);
        if (!g) continue;
        for (int f = 0; f < 2; f++) frame(0, 0);          /* ein Gehirn-Tick -> Merker gefuellt */
        int hp0 = g->hp;
        int flinch_bei[4] = {0, 0, 0, 0}, exit_sub[4] = {-1, -1, -1, -1};
        for (int hit = 1; hit <= 3; hit++) {
            int fx0 = re15_esp_fx_count();
            treffer_resolver(g, 3, 12);                    /* Pistole: Zeile 3 (Spur 0), 12 @0x8006ee44 */
            int clip7 = 0, zurueck = -1;
            for (int f = 0; f < 60; f++) {
                frame(0, 0);
                if (g->motion == 7) clip7 = 1;
                if (g->state == 1 && zurueck < 0) { zurueck = f; exit_sub[hit] = g->sub_state_1; }
                if (g->state == 1 && f > 0) break;
            }
            flinch_bei[hit] = clip7;
            printf("    [%s] Treffer %d: hp %d -> %d, Clip 7 = %d, Zustand 1 nach %d Bildern, Sub %d, akku=%d, fx %d -> %d\n",
                   tag, hit, hp0, (int)g->hp, clip7, zurueck, exit_sub[hit], re15_r39_affen_akku(g), fx0, re15_esp_fx_count());
            hp0 = g->hp;
            /* Sprung-/Jagd-Zustand nach einem Flinch neutralisieren, damit der naechste Treffer sauber beginnt */
            if (clip7) { g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
                         g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0; g->dog_blocked_ctr = 30000; }
            for (int f = 0; f < 2; f++) frame(0, 0);
        }
        if (!an) {
            PRUEF(flinch_bei[1] && flinch_bei[2] && flinch_bei[3],
                  "[vorher] jeder Pistolentreffer flincht (Clip 7 @0x8011b08c-90): %d %d %d", flinch_bei[1], flinch_bei[2], flinch_bei[3]);
        } else {
            PRUEF(!flinch_bei[1] && !flinch_bei[2],
                  "[nachher] Treffer 1 und 2 (12/24 Energie < 36) OHNE Flinch: Clip 7 = %d / %d", flinch_bei[1], flinch_bei[2]);
            PRUEF(exit_sub[1] == 0 && exit_sub[2] == 0,
                  "[nachher] Treffer 1/2: Gehirn laeuft in Zustand 1 / Sub 0 weiter (Unterzustaende aus dem Merker): %d / %d", exit_sub[1], exit_sub[2]);
            PRUEF(flinch_bei[3],
                  "[nachher] Treffer 3 (36 Energie = 3 x 12 @0x8006ee44) flincht (Clip 7)");
            PRUEF(exit_sub[3] == 7,
                  "[nachher] Flinch-Exit nach dem 3. Treffer -> Sub 7 Vergeltungs-Sprung (Runde 35: jeder Treffer zaehlt, @0x8011b194-98), Sub %d", exit_sub[3]);
            PRUEF(g->hp == 360 - 36, "[nachher] HP 360 -> 324 (jeder Treffer zieht 12 ab, auch ohne Flinch): %d", (int)g->hp);
        }
        /* +0x93-Riegel nach einem Treffer ohne Zucken: frei (sonst waere der Gorilla unverwundbar) */
        if (an) {
            g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0; g->hit_react = 0;
            for (int f = 0; f < 2; f++) frame(0, 0);
            treffer_resolver(g, 3, 12);
            frame(0, 0);
            PRUEF(g->state == 1 && (g->hit_react & 1) == 0,
                  "[nachher] Treffer ohne Zucken loescht +0x93 (wie der Flinch-Exit @0x8011b178): state=%d react=0x%02x", g->state, g->hit_react);
            /* Schrotflinte: Zeile 8 = 38 (@0x8006ee38 + 8*4) >= 36 -> Flinch sofort (Spur 1 = Luft-Treffer, Clip 8/9) */
            g->hit_react = 0; for (int f = 0; f < 2; f++) frame(0, 0);
            int hpv = g->hp;
            treffer_resolver(g, 8, 38);
            int clip89 = 0;
            for (int f = 0; f < 40; f++) { frame(0, 0); if (g->motion == 8 || g->motion == 9) clip89 = 1; if (g->state == 1 && f > 0) break; }
            PRUEF(clip89, "[nachher] Schrotflinte 38 Energie >= 36 -> Reaktion sofort (Spur 1 Clip 8/9 @0x8011b260-8c), hp %d -> %d", hpv, (int)g->hp);
        }
    }
}

/* ================================================================== 1c: echter Pistolenschuss ============== */
static int schuss(re15_actor_t *g, int *clip7, int *hp_vor, int *hp_nach)
{
    /* Zielen (R1 halten), dann Kreuz-Flanke = Schuss (Muster probe_r16_kriecher1010.c Teil B) */
    *clip7 = 0; *hp_vor = g->hp;
    for (int f = 0; f < 20; f++) frame(RE15_PAD_BIT_R1, 0);
    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE), RE15_PAD_BIT_SQUARE);
    int getroffen = (g->hp < *hp_vor);
    for (int f = 0; f < 45; f++) {
        frame(RE15_PAD_BIT_R1, 0);
        if (g->motion == 7) *clip7 = 1;
    }
    for (int f = 0; f < 10; f++) frame(0, 0);
    *hp_nach = g->hp;
    return getroffen;
}

static void teil_affen_schuss(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        if (room_boot(0x11C0, -9013, 0, -15461, 0, 5, 3, 3) != 0) return;
        (void)bank_re15(0x27);
        re15_actor_t *g = gorilla_parken_einen();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!g) { PRUEF(0, "[%s] kein Gorilla", tag); continue; }
        /* Leon 2000 vor dem Gorilla, Blick auf ihn; Pistole = RE1.5-Waffe 3 */
        pl->x = g->x - 2000; pl->z = g->z; pl->y = g->y;
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(g->z - pl->z, g->x - pl->x) - 0x400) & 0xfff);
        re15_inv_load_briefing();
        re15_player_set_equipped_weapon(3);
        re15_player_set_aim_clip_len(12);
        for (int f = 0; f < 2; f++) frame(0, 0);
        int treffer = 0, fl[4] = {0, 0, 0, 0}, hpv[4], hpn[4];
        for (int k = 1; k <= 3; k++) {
            int ok = schuss(g, &fl[k], &hpv[k], &hpn[k]);
            treffer += ok;
            printf("    [%s] Schuss %d: hp %d -> %d, Clip 7 = %d, Zustand %d/%d, akku=%d\n", tag, k, hpv[k], hpn[k], fl[k],
                   g->state, g->sub_state_1, re15_r39_affen_akku(g));
            if (fl[k]) { g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
                         g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0; g->dog_blocked_ctr = 30000; }
            pl->x = g->x - 2000; pl->z = g->z;
            pl->rot_y = (int16_t)(((int)re15_atan2_q12(g->z - pl->z, g->x - pl->x) - 0x400) & 0xfff);
        }
        PRUEF(treffer == 3, "[%s] drei Pistolenschuesse treffen (re15_player_weapon_fire, Zeile 0x27 @0x8006ee38): %d/3", tag, treffer);
        if (treffer < 3) continue;
        PRUEF(hpv[1] - hpn[1] == 12 && hpv[2] - hpn[2] == 12 && hpv[3] - hpn[3] == 12,
              "[%s] jeder Schuss zieht 12 ab (@0x8006ee44): %d/%d/%d", tag, hpv[1] - hpn[1], hpv[2] - hpn[2], hpv[3] - hpn[3]);
        if (!an) PRUEF(fl[1] && fl[2] && fl[3], "[vorher] jeder Schuss flincht: %d %d %d", fl[1], fl[2], fl[3]);
        else {
            PRUEF(!fl[1] && !fl[2] && fl[3], "[nachher] Flinch erst beim dritten Schuss (36 Energie): %d %d %d", fl[1], fl[2], fl[3]);
            PRUEF(hpv[1] == 360 && hpn[3] == 324, "[nachher] HP 360 -> 324 ueber drei Schuesse: %d -> %d", hpv[1], hpn[3]);
        }
    }
}

/* ================================================================== 2: Hunde-Riegel ========================= */
static re15_actor_t *PL, *D1, *D2;
static void hund_flug(re15_actor_t *d, int32_t x, int32_t z)
{
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->hp = 90; d->floor = 0;
    d->state = 1; d->sub_state_1 = 3; d->sub_state_2 = 1; d->sub_state_3 = 0;   /* Sub 3 P1 = Flug, Kontakt-Pruefung @0x80101488 */
    d->x = x; d->y = 0; d->z = z; d->rot_y = 0; d->dog_floor_y = 0;
    d->speed_h = 0; d->re2d_vy146 = -400; d->re2d_air219 = 1;
    d->motion = 20; d->anim_frame = 4;
    d->re2d_bite21e = 0; d->re2d_nolatch22c = 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (eb && !eb->ok && re2_ems_load_bank(s_re2_ems, s_re2_n, 0x20, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}
static void hunde_aufbau(void)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; D1 = &g_actors[1]; D2 = &g_actors[2];
    memset(PL, 0, sizeof *PL);
    PL->active = 1; PL->hp = 100; PL->floor = 0; PL->x = 0; PL->z = 0;
    hund_flug(D1, 0, 0); hund_flug(D2, 0, 0);
    int32_t kiefer[3];
    re15_enemy_bone_world_pos(D1, 4, kiefer);               /* Part 4 = Kiefer (@0x80104E24), Radius 1000 (@0x80104E48) */
    PL->x = kiefer[0] + 600; PL->z = kiefer[2];
    PL->rot_y = (int16_t)(((int)D1->rot_y + 2048) & 0xfff);  /* frontal -> kein Latch-Pfad (0x80015910 @0x80104EEC) */
    D1->ai_dist = 0; D2->ai_dist = 0;
}

static void teil_hund_riegel(void)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) { printf("  SKIP: shared_assets/RE2/CDEMD0.EMS fehlt (RE2-Hundebank)\n"); return; }
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        hunde_aufbau();
        int32_t kiefer[3]; re15_enemy_bone_world_pos(D1, 4, kiefer);
        PRUEF(kiefer[0] - D1->x > 400, "[%s] RE2-EM020-Rig geladen (Kiefer %d vor der Wurzel)", tag, (int)(kiefer[0] - D1->x));
        int hp0 = PL->hp;
        re15_re2dog_tick(1);                                   /* Hund 1: Kontakt -> Boden-Biss 20 (@0x80104EB8) */
        int hp1 = PL->hp;
        re15_re2dog_tick(2);                                   /* Hund 2 im selben Bild */
        int hp2 = PL->hp;
        printf("    [%s] Bild 1: hp %d -> %d (Hund 1) -> %d (Hund 2); PL+0x1D3=0x%02x riegel_vom_hund=%d D1.21E=%d D2.21E=%d\n",
               tag, hp0, hp1, hp2, PL->re2z_self1d3, re15_r39_hund_riegel_vom_hund(), D1->re2d_bite21e, D2->re2d_bite21e);
        PRUEF(hp0 - hp1 == 20, "[%s] Hund 1 beisst: -20 (FUN_800401D4(20) @0x80104EB8-C0), gemessen %d", tag, hp0 - hp1);
        if (!an) {
            PRUEF(hp1 - hp2 == 20, "[vorher] Hund 2 beisst PARALLEL im selben Bild: -20 (gemessen %d) = der Nutzer-Befund", hp1 - hp2);
            PRUEF((PL->re2z_self1d3 & 0x80) == 0, "[vorher] kein Riegel PL+0x1D3 (0x%02x)", PL->re2z_self1d3);
            continue;
        }
        PRUEF((PL->re2z_self1d3 & 0x80) != 0, "[nachher] Boden-Biss setzt PL+0x1D3 |= 0x80 (@0x801051f8-0x80105200): 0x%02x", PL->re2z_self1d3);
        PRUEF(hp1 - hp2 == 0, "[nachher] Hund 2 kehrt am Tor @0x80104e5c-68 um: kein zweiter Biss (gemessen %d)", hp1 - hp2);
        PRUEF(D2->re2d_bite21e == 0, "[nachher] Hund 2 ohne Kontakt (+0x21E bleibt 0): %d", D2->re2d_bite21e);
        /* Spieler-Reaktion laeuft (+0x93 Bit 1 steht): Riegel bleibt */
        re15_r39_hund_riegel_tick();
        PRUEF((PL->re2z_self1d3 & 0x80) != 0 && (PL->hit_react & 1), "[nachher] waehrend der Reaktion (+0x93&1) bleibt der Riegel: 0x%02x react=0x%02x",
              PL->re2z_self1d3, PL->hit_react);
        hund_flug(D2, 0, 0); D2->ai_dist = 0;
        re15_re2dog_tick(2);
        PRUEF(PL->hp == hp2, "[nachher] auch im naechsten Bild kein Biss von Hund 2 (hp %d)", (int)PL->hp);
        /* Ende der Reaktion: NORMAL-Prolog loescht +0x93 (@0x80031964) = RE2 HURT-Exit @0x8003f8b4 -> Riegel frei */
        PL->hit_react = 0;
        re15_r39_hund_riegel_tick();
        PRUEF((PL->re2z_self1d3 & 0x80) == 0 && !re15_r39_hund_riegel_vom_hund(), "[nachher] nach der Reaktion ist der Riegel frei: 0x%02x", PL->re2z_self1d3);
        hund_flug(D2, 0, 0); D2->ai_dist = 0;
        int hp3 = PL->hp;
        re15_re2dog_tick(2);
        PRUEF(hp3 - PL->hp == 20, "[nachher] danach beisst ein Hund wieder: -20 (gemessen %d)", hp3 - (int)PL->hp);
        /* Riegel eines ANDEREN Besitzers (Zombie-Griff @0x80102754-60) wird vom Hunde-Tick nicht angefasst:
         * erst den Hunde-Riegel des letzten Bisses regulaer freigeben, dann das Byte fremd setzen */
        PL->hit_react = 0; re15_r39_hund_riegel_tick();
        PRUEF(!re15_r39_hund_riegel_vom_hund() && (PL->re2z_self1d3 & 0x80) == 0, "[nachher] Hunde-Riegel des zweiten Bisses regulaer frei");
        PL->re2z_self1d3 = 0x80;
        re15_r39_hund_riegel_tick();
        PRUEF((PL->re2z_self1d3 & 0x80) != 0, "[nachher] fremder Riegel (Zombie-Griff) bleibt stehen: 0x%02x", PL->re2z_self1d3);
        PL->re2z_self1d3 = 0;
    }
}

/* ================================================================== 2b: RE1.5-Flavor-Hund ================== */
static void teil_hund_re15(void)
{
    /* Das RE1.5-Original (STAGE1.BIN FUN_8010f15c, BITE Sub 8 Phase 1): im Sprungfenster (+0x95 > 0x0c @0x8010f254)
     * `lbu acae7 / bne -> kein Biss` @0x8010f294 VOR dem Schaden `acaee -= 10` @0x8010f2c4-d0; der Niederschlag setzt
     * acae7 |= 1 (@0x8010f338-44). Zwei Hunde im selben Bild: der zweite sieht acae7 != 0 -> kein zweiter Abzug.
     * Port: enemy_ai_common.c case 8 (`pl->hit_react == 0 && re15_dog_arc(...)`). Kein Haken dieser Spur beteiligt —
     * gemessen mit Haken aus UND an (gleiches Ergebnis). */
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        memset(pl, 0, sizeof *pl);
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 1000; pl->rot_y = 0;
        re15_actor_t *d[2] = { &g_actors[1], &g_actors[2] };
        for (int i = 0; i < 2; i++) {
            memset(d[i], 0, sizeof *d[i]);
            d[i]->active = 1; d[i]->type = 0x20; d[i]->state = 1; d[i]->sub_state_1 = 8; d[i]->sub_state_2 = 0;   /* BITE, Phase 0 */
            d[i]->x = i ? 300 : -300; d[i]->y = 0; d[i]->z = 0; d[i]->hp = 20; d[i]->dog_grab_armed = 0; d[i]->em_flag_id = 0xFF;
            d[i]->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - d[i]->z, pl->x - d[i]->x) - 0x400) & 0xfff);
            re15_enemy_apply_hitbox(d[i], 0x20);
        }
        int bild = -1, hp_bild = 0;
        for (int f = 0; f < 90 && bild < 0; f++) {
            re15_enemy_ai_run_all(0);
            if (pl->hp < 100) { bild = f; hp_bild = pl->hp; }
        }
        printf("    [%s] RE1.5: erster Abzug in Bild %d, hp=%d, acae7=0x%02x, Hund 1 Sub %d/%d, Hund 2 Sub %d/%d\n", tag, bild, hp_bild,
               pl->hit_react, d[0]->sub_state_1, d[0]->sub_state_2, d[1]->sub_state_1, d[1]->sub_state_2);
        PRUEF(bild >= 0 && hp_bild == 90, "[%s] RE1.5-Flavor: im Biss-Bild zieht nur EIN Hund ab (-10 @0x8010f2c4-d0, nicht -20): hp %d", tag, hp_bild);
        PRUEF((pl->hit_react & 1) != 0, "[%s] RE1.5-Flavor: Niederschlag setzt acae7 |= 1 (@0x8010f338-44): 0x%02x", tag, pl->hit_react);
        int hp_vor = pl->hp;
        for (int f = 0; f < 5; f++) re15_enemy_ai_run_all(0);
        PRUEF(pl->hp == hp_vor, "[%s] RE1.5-Flavor: solange acae7 steht, kein zweiter Biss (`lbu acae7 / bne` @0x8010f294): hp %d", tag, (int)pl->hp);
    }
}

/* ================================================================== 3: Kriecher ============================= */
/* room/typ/ziel = der gemessene 0x81-Kriecher (Spawn-Position), cut = Eintritts-Cut (work_vars[0x0A], Switch des
 * Spawn-Skripts), boot = Spieler-Startpunkt (begehbar), R = begehbarer Bezugspunkt (Tuer-Ziel bzw. der zweite
 * Liegeplatz): der Kriecher wird auf R gedreht (seine RDT-Gier ist nicht das Mass; das Mass sind Distanz < 0xbb8 und
 * der Sichtkegel 0x200 des Weckers), der Spieler steht fern auf R (>= 3000) und nah 2500 davor auf der Linie zu R. */
static void kriecher_messen(int flavor, uint16_t room, uint8_t typ, int32_t ziel_x, int32_t ziel_z, uint8_t cut,
                            int32_t boot_px, int32_t boot_pz, int32_t rx, int32_t rz)
{
    /* RE2-Flavor: WAIT = Wort 0x201 (Sub 2, Clip 23 @0x80103B5C), Lokomotion P0 = Clip 5 (@0x80103064).
     * RE1.5-Flavor: der 0x81-Spawn wartet in DECIDE[5] (FUN_8010466C, Sub 5) und kriecht nativ ueber Block B
     * (@0x80104718-48) in Wort 1 (Sub 0, Kriech-Clip 0x1A) — dort ist der Haken dieser Spur unbeteiligt. */
    const int re2 = (flavor == RE15_AI_FLAVOR_RE2);
    const int wait_sub = re2 ? 2 : 5, kriech_clip = re2 ? 5 : 0x1a;
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        re15_ai_flavor_set(flavor);
        if (room_boot(room, boot_px, 0, boot_pz, 0, 0, cut, 0) != 0) return;
        if (re2) (void)bank_re2(typ); else (void)bank_re15(typ);
        re15_actor_t *k = NULL;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == typ && g_actors[s].x == ziel_x && g_actors[s].z == ziel_z) { k = &g_actors[s]; break; }
        PRUEF(k != NULL, "[%s] ROOM%04X Cut %u: 0x81-Kriecher Typ 0x%02x bei (%d,%d) gespawnt", tag, room, cut, typ, (int)ziel_x, (int)ziel_z);
        if (!k) {
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type != 0)
                    printf("    aktiv: slot %d typ 0x%02x grid 0x%02x (%d,%d) st=%d/%d\n", s, g_actors[s].type, g_actors[s].grid_id,
                           (int)g_actors[s].x, (int)g_actors[s].z, g_actors[s].state, g_actors[s].sub_state_1);
            continue;
        }
        /* uebrige Gegner parken (nur der eine Kriecher wird gemessen) */
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && &g_actors[s] != k && g_actors[s].type != 0) { g_actors[s].grid_id |= 0x20; g_actors[s].x = 40000; g_actors[s].z = 40000; }
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* Kriecher auf R drehen (face_player-Formel der Hunde-Riegel: re15_atan2_q12(dz,dx) - 0x400) */
        k->rot_y = (int16_t)(((int)re15_atan2_q12(rz - k->z, rx - k->x) - 0x400) & 0xfff);
        double rl = dist2d(rx, rz, k->x, k->z);
        int32_t ux = (int32_t)((double)(rx - k->x) * 2500.0 / rl), uz = (int32_t)((double)(rz - k->z) * 2500.0 / rl);
        /* fern (R, >= 3000, ausserhalb Block B): liegt */
        pl->y = k->y; pl->floor = k->floor;
        for (int f = 0; f < 30; f++) { pl->x = rx; pl->z = rz; frame(0, 0); }
        PRUEF(k->state == 1 && k->sub_state_1 == wait_sub && (!re2 || k->motion == 0x17) && k->ai_dist >= 0xbb8u,
              "[%s] fern (dist %u >= 0xbb8, arc=%d): liegt (%s): st=%d/%d mo=0x%02x",
              tag, k->ai_dist, re15_ai_arc_test(k, pl->x, pl->z, 0x200),
              re2 ? "RE2 Wort 0x201 @0x80100B74, Clip 23 @0x80103B5C" : "RE1.5 DECIDE[5] FUN_8010466C, Sub 5",
              k->state, k->sub_state_1, (unsigned)k->motion);
        /* nah: 2500 vor dem Kriecher auf der Linie zu R */
        int32_t kx0 = k->x, kz0 = k->z;
        int geweckt = -1, kroch = 0, dist_weck = -1;
        for (int f = 0; f < 120; f++) {
            pl->x = kx0 + ux; pl->z = kz0 + uz;
            frame(0, 0);
            if (geweckt < 0 && k->state == 1 && k->sub_state_1 == 0) { geweckt = f; dist_weck = (int)k->ai_dist; }
            if (k->state == 1 && k->sub_state_1 == 0 && k->motion == kriech_clip) kroch = 1;   /* RE2: Lokomotion P0 Clip 5 @0x80103064; RE1.5: Kriech-Clip 0x1A */
        }
        double weg = dist2d(k->x, k->z, kx0, kz0);
        printf("    [%s] nah (dist beim Wecken %d, arc=%d): geweckt nach %d Bildern, Kriech-Clip gesehen=%d, Ende st=%d/%d mo=0x%02x dist=%u, Weg %.0f\n",
               tag, dist_weck, re15_ai_arc_test(k, pl->x, pl->z, 0x200), geweckt, kroch, k->state, k->sub_state_1, (unsigned)k->motion,
               k->ai_dist, weg);
        if (!re2) {
            PRUEF(geweckt >= 0 && dist_weck < 0xbb8 && kroch && weg > 200.0,
                  "[%s] RE1.5-Flavor: Block B nativ (@0x80104718-48) -> Wort 1 nach %d Bildern, Kriech-Clip 0x1A gesehen=%d, Weg %.0f (Haken dieser Spur unbeteiligt)",
                  tag, geweckt, kroch, weg);
        } else if (!an) {
            PRUEF(geweckt < 0 && weg < 1.0, "[vorher] RE2-WAIT ohne Distanz-Wecker: liegt weiter, Weg %.0f (der Nutzer-Befund)", weg);
        } else {
            PRUEF(geweckt >= 0 && dist_weck < 0xbb8, "[nachher] Spieler in Sicht < 3000 (dist %d) -> Wort 1 (RE1.5 Block B @0x80104718-48) nach %d Bildern", dist_weck, geweckt);
            PRUEF(kroch, "[nachher] Kriech-Lokomotion lief (Sub 0, Clip 5 @0x80103064)");
            PRUEF(weg > 200.0, "[nachher] er kriecht auf den Spieler zu: Weg %.0f Einheiten in 120 Bildern (danach ggf. RE2-Griff Sub 1)", weg);
        }
    }
}
/* ROOM1010 Cut 4 = Suedtuer (sub00 Case 4, Spawns @0x0A12/@0x0A26), Tuer-Ziel (3650,-3950) begehbar (probe_r16_kriecher1010) */
static void teil_kriecher_1010(void) { re15_game_state_init(); kriecher_messen(RE15_AI_FLAVOR_RE2, 0x1010, 0x10, 950, -1700, 4, 3650, -3950, 3650, -3950); }
static void teil_kriecher_1010_re15(void) { re15_game_state_init(); kriecher_messen(RE15_AI_FLAVOR_RE15, 0x1010, 0x10, 950, -1700, 4, 3650, -3950, 3650, -3950); }
/* ROOM1220 sub00 `Switch 0x0a` (Eintritts-Cut) Case 0 @0x00F24: zwei 0x16-Kriecher @0x00F2A (-25000,-6700) / @0x00F3E (-23500,-9700);
 * der zweite Liegeplatz ist der begehbare Bezugspunkt */
/* Gemessen wird der zweite (@0x00F3E, (-23500,-9700), liegt exakt dort); der erste wird vom Spawn-Schub auf
 * (-25398,-6739) gesetzt (Spawn in der Wand) — sein Liegeplatz ist der Bezugspunkt R (3517 entfernt). */
static void teil_kriecher_1220(void) { re15_game_state_init(); kriecher_messen(RE15_AI_FLAVOR_RE2, 0x1220, 0x16, -23500, -9700, 0, -25398, -6739, -25398, -6739); }
static void teil_kriecher_1220_re15(void) { re15_game_state_init(); kriecher_messen(RE15_AI_FLAVOR_RE15, 0x1220, 0x16, -23500, -9700, 0, -25398, -6739, -25398, -6739); }

/* ================================================================== 4: Schatten ============================= */
/* Nachbesserung 1 (M1 der Abnahme 0): der Schatten-Schwanz der Gorilla-Wurzel laeuft auf JEDEM Pfad (Root-Skip
 * `andi 0x20 / bne` @0x80116df4-f8 springt nur ueber Gehirn + Push-out auf 0x80116e88; der Schwanz liegt @0x80116f20-38;
 * STAGE3 0x29: `bne` @0x80110b40 -> 0x80110bd0, Schwanz @0x80110c68-80). Sichtbar wird der Quad nur ueber den Region-
 * Cull in FUN_8001b064 (`lw a1,DAT_800ac790` @0x8001b180 / `jal 0x80014368` @0x8001b224 / `beq` @0x8001b22c). */
static void teil_schatten(void)
{
    re15_actor_t e; memset(&e, 0, sizeof e);
    e.active = 1; e.type = 0x27; e.y = -5200; e.dog_floor_y = -3600; e.state = 1; e.sub_state_1 = 7;
    for (int an = 0; an <= 1; an++) {
        re15_r39_gegner_enable(an);
        int32_t y = e.y;
        int r = re15_r39_schatten_y(&e, &y);
        if (!an) PRUEF(r == 0 && y == e.y, "[vorher] Quad-Y = Koerper-Y %d (haengt beim Sprung mit)", (int)y);
        else     PRUEF(r == 1 && y == -3600, "[nachher] Gorilla 0x27 im Sprung (Koerper %d): Quad-Y = +0x1ba %d (@0x80116f30)", (int)e.y, (int)y);
    }
    re15_r39_gegner_enable(1);
    e.type = 0x29; { int32_t y = e.y; PRUEF(re15_r39_schatten_y(&e, &y) == 1 && y == -3600, "kleiner Affe 0x29: Quad-Y = +0x1ba (STAGE3 @0x80110c78)"); }
    e.type = 0x20; { int32_t y = e.y; PRUEF(re15_r39_schatten_y(&e, &y) == 0 && y == e.y, "Hund 0x20: nicht angefasst (Runde 35 H eigener Weg)"); }
    e.type = 0x10; { int32_t y = e.y; PRUEF(re15_r39_schatten_y(&e, &y) == 0 && y == e.y, "Zombie 0x10: nicht angefasst"); }
    /* Original-Lage der Szenen-Gorillas (grid 0x30): der Schwanz laeuft, Quad auf +0x1ba = -3600 (Original-RAM s020
     * t=34.89: y=-2500, +0x1ba=-3600, Ecken 1700/1800) - kein Sonderweg (der Root-Skip-Haken war am Byte falsch) */
    e.type = 0x27; e.grid_id = 0x30; e.y = -2500; e.dog_floor_y = -3600;
    { int32_t y = e.y; PRUEF(re15_r39_schatten_y(&e, &y) == 1 && y == -3600,
                              "Szenen-Gorilla grid 0x30 im Wagen (y=-2500): Quad auf +0x1ba = %d wie im Original (Schwanz @0x80116f20-38 auf jedem Pfad)", (int)y); }
    e.type = 0x29; e.grid_id = 0x20; { int32_t y = e.y; PRUEF(re15_r39_schatten_y(&e, &y) == 1 && y == -3600, "kleiner Affe 0x29 mit grid&0x20: Quad auf +0x1ba (STAGE3 Schwanz @0x80110c68-80 auf jedem Pfad)"); }
    /* Szenen-Layout ROOM11C0 (ohne Flag 4/0x40): beide 0x27 kommen mit grid 0x30 und y=-20000 aus dem RDT (@0x01784/@0x01798) */
    re15_game_state_init();
    if (room_boot(0x11C0, -22604, 0, 14455, 0, 0, 0, 3) != 0) return;
    {
        re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
        PRUEF(a && b && (a->grid_id & 0x20) && (b->grid_id & 0x20) && a->y == -20000 && b->y == -20000,
              "ROOM11C0 Szene: beide Gorillas grid 0x%02x/0x%02x, y=%d/%d (RDT `e0 b1` = -20000, +0x1ba = -3600)",
              a ? a->grid_id : 0, b ? b->grid_id : 0, a ? (int)a->y : 0, b ? (int)b->y : 0);
        if (a && b) {
            int32_t qa = a->y, qb = b->y;
            PRUEF(re15_r39_schatten_y(a, &qa) == 1 && qa == -3600 && re15_r39_schatten_y(b, &qb) == 1 && qb == -3600,
                  "ROOM11C0 Szene: geparkte Gorillas Quad auf +0x1ba = %d/%d (Original-RAM s001 t=6.11: +0x1ba=-3600)", (int)qa, (int)qb);
            PRUEF(a->state == 1 && b->state == 1,
                  "ROOM11C0 Szene: INIT im Spawn-Bild trotz grid 0x30 (Sce_em_set loescht Bit 0x20 @0x8004256c-70 vor dem jalr @0x8004259c): Zustand %d/%d (Original-RAM s001: 1/1)", a->state, b->state);
        }
        /* Region-Cull = der Mechanismus, der im Original ueber den gezeichneten Quad entscheidet (FUN_8001b064
         * @0x8001b180/@0x8001b224/@0x8001b22c, Punkt-im-Viereck ueber (x,z) ohne Hoehe). Zonen aus dem RDT
         * (re15_rdt_get_region_quad = FUN_80014324 -> DAT_800ac790) gegen die Original-RAM-Werte der Runde-35-
         * Savestates (orig_scene/r3: DAT_800ac790 -> 20-Byte-Zone, Ecken @+4..+0x12) und das Ergebnis fuer die
         * beiden Szenen-Positionen (-1220,-21568) = Gorilla 1 geparkt und im Wagen, (-554,-25423) = Gorilla 2. */
        /* Cut 0: FUN_80014324 (@0x80014324-64: erster 20-Byte-Satz ab RDT+0x28 mit Byte+2 == Cut) liefert den Eintrag
         * @0x80121bd8 (RAM s000/s001: `8f ff 00 00`, DAT_800ac794 zeigt dorthin). DAT_800ac790 selbst schreibt nur der
         * Cut-Apply FUN_80021bbc (@0x80021c00 jal / @0x80021c0c sw) — beim Raumeintritt (s000/s001, vor dem ersten
         * Cut_chg) steht dort noch der STALE Zeiger 0x80121b80 (misaligned in die 11C0-Tabelle: [1530,0; 17586,0;
         * 10768,-32750; 0,-32590]); beide Vierecke schliessen die geparkten Gorillas aus (0/0). */
        static const struct { int cut; int16_t xs[4], zs[4]; int p1, p2; const char *sav; } zonen[] = {
            {  0, {-30500, -28600,-10999, -2997 }, {   4500,  28692,  28800,   4200 }, 0, 0, "s000/s001 @0x80121bd8 (DAT_800ac794)" },
            {  1, {-30301, -28200,  1800,  2603 }, {   4702,  27002,  29703,   4502 }, 0, 0, "s002 t=7.64"  },
            {  3, {-29901, -29300, -2098, -3693 }, { -20799,  19505,  19505, -20298 }, 0, 0, "s004 t=10.69" },
            {  4, {-30500, -29700, -2897, -2899 }, { -28901,  19803,  20704, -29900 }, 0, 0, "s005 t=12.19" },
            { 13, {-24200, -24502, 12901, 13004 }, { -18102,   5700,   5200, -17901 }, 0, 0, "s007-s019"    },
            { 12, {-23700, -24502,  5401,  5504 }, { -21102,  -1300,  -1300, -25401 }, 1, 0, "s020 t=34.89" },
            {  5, {-21767, -18463, 18004, 20301 }, { -31201,  -3298,  -3300, -32700 }, 1, 1, "s022 t=37.92" },
        };
        for (size_t i = 0; i < sizeof zonen / sizeof zonen[0]; i++) {
            int16_t xs[4] = {0, 0, 0, 0}, zs[4] = {0, 0, 0, 0};
            int ok = re15_rdt_get_region_quad(&s_rdt, zonen[i].cut, xs, zs);
            PRUEF(ok && memcmp(xs, zonen[i].xs, sizeof xs) == 0 && memcmp(zs, zonen[i].zs, sizeof zs) == 0,
                  "ROOM11C0 Cut %d: Region-Viereck des RDT (%d,%d)(%d,%d)(%d,%d)(%d,%d) == Original-RAM DAT_800ac790 (%s)",
                  zonen[i].cut, xs[0], zs[0], xs[1], zs[1], xs[2], zs[2], xs[3], zs[3], zonen[i].sav);
            int r1 = re15_aot_point_in_quad(-1220, -21568, xs, zs), r2 = re15_aot_point_in_quad(-554, -25423, xs, zs);
            int f1 = re15_aot_point_in_quad_fun80014368(-1220, -21568, xs, zs), f2 = re15_aot_point_in_quad_fun80014368(-554, -25423, xs, zs);
            PRUEF(r1 == zonen[i].p1 && r2 == zonen[i].p2 && f1 == r1 && f2 == r2,
                  "ROOM11C0 Cut %d: Punkt-im-Viereck (FUN_80014368) Gorilla 1 (-1220,-21568) = %d/%d (Original %d), Gorilla 2 (-554,-25423) = %d/%d (Original %d)",
                  zonen[i].cut, r1, f1, zonen[i].p1, r2, f2, zonen[i].p2);
        }
    }
    /* Kampf-Layout: `44 01 27 10 00 ..` @0x017E8 -> grid 0x10, pc[4] = 0 -> +0x1ba = 0 (steht bei y = 0); die
     * Szenen-Variante `44 01 27 30 02 ..` @0x01784 traegt 2 -> -3600 (Sce_em_set -(pc[4]*1800) @0x800421d4-0x80042210). */
    re15_game_state_init(); re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -9013, 0, -15461, 0, 5, 3, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0);
    if (g) {
        PRUEF((g->grid_id & 0x20) == 0, "ROOM11C0 Kampf: grid 0x%02x (Gehirn laeuft)", g->grid_id);
        int32_t boden = g->dog_floor_y;
        int32_t y = boden - 1500;   /* wie im Flug */
        g->y = y;
        int32_t q = y; re15_r39_schatten_y(g, &q);
        PRUEF(q == boden && boden == 0 && q != y, "ROOM11C0-Gorilla (Kampf-Variante pc[4]=0): Quad-Y = +0x1ba = %d, Koerper %d", (int)q, (int)y);
    }
}

/* ================================================================================================== */
int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    printf("test_r39_gegner: %s\n", teil);
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "affen_hp"))      { printf("== affen_hp ==\n");      teil_affen_hp(); }
    if (alle || !strcmp(teil, "affen_zucken"))  { printf("== affen_zucken ==\n");  teil_affen_zucken(); }
    if (alle || !strcmp(teil, "affen_schuss"))  { printf("== affen_schuss ==\n");  teil_affen_schuss(); }
    if (alle || !strcmp(teil, "hund_riegel"))   { printf("== hund_riegel ==\n");   teil_hund_riegel(); }
    if (alle || !strcmp(teil, "hund_re15"))     { printf("== hund_re15 ==\n");     teil_hund_re15(); }
    if (alle || !strcmp(teil, "kriecher_1010")) { printf("== kriecher_1010 ==\n"); teil_kriecher_1010(); }
    if (alle || !strcmp(teil, "kriecher_1220")) { printf("== kriecher_1220 ==\n"); teil_kriecher_1220(); }
    if (alle || !strcmp(teil, "kriecher_1010_re15")) { printf("== kriecher_1010_re15 ==\n"); teil_kriecher_1010_re15(); }
    if (alle || !strcmp(teil, "kriecher_1220_re15")) { printf("== kriecher_1220_re15 ==\n"); teil_kriecher_1220_re15(); }
    if (alle || !strcmp(teil, "schatten"))      { printf("== schatten ==\n");      teil_schatten(); }
    printf("test_r39_gegner %s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
