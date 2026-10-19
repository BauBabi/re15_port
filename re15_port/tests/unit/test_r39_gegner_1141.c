/* test_r39_gegner_1141.c — RIEGEL Runde 39 Spur E "gegner", Nachbesserung 2: die Gorilla-Regel gilt fuer ALLE 0x27.
 *
 * Nutzer-Nachtrag 3 (AUFTRAG.md, 2026-10-18 14:25): "Zu 2. ja, wir machen das Gorilla Verhalten generell so." —
 * HP x2 (2 x 180 @0x8011f514 = 360) und Flinch erst nach 3 Handfeuerwaffen-Treffern (3 x 12 @0x8006ee44 = 36 Energie)
 * fuer jeden Gorilla Typ 0x27 in jedem Raum, beide Szenarien. Zensus Sce_em_set 0x27 (alle 240 RDTs): ROOM11C0 (Leon,
 * Riegel test_r39_gegner affen_*) und ROOM1141 (Elza, DIESER Riegel). Die kleinen Affen 0x29 bleiben unberuehrt.
 * Dossier analysis/befunde_runde39/E_gegner.md "Nachbesserung 2", Code engine/src/gegner_r39.c + include/re15_gegner_r39.h.
 *
 * Harness: die ECHTEN Engine-Wege wie test_r38_dining1141.c (scd_room_reenter mit allen Installern, SCD-VM samt
 * Runde-38-Programmen HAEMMERN/BEFREIUNG, Walker, Nachrichten-FSM, Wegwerf-Abfrage, re15_game_step); Elza = Figur 4,
 * Gorilla- und John-Bank aus CDEMD1.EMS (Elza-Archiv, wie die exe: "[elzaems] Typ 0x27 aus EMD/CDEMD1.EMS").
 * Jeder Teil misst zuerst "vorher" (re15_r39_gegner_enable(0) = Haken durchsichtig) und dann "nachher" im selben Lauf.
 *
 * Teile (Aufruf: test_r39_gegner_1141 <teil>):
 *   hp1141      ROOM1141 Kampf-Lage ((4,0xF2) = 1, sub00 Else-Zweig @0x00B54/@0x00B68, grid 0x10): beide 0x27 mit HP
 *               180 (vorher) / 360 (nachher); die Regel haengt am Typ, nicht am Raum (11C0/1141/fremder Raum = 360)
 *   zucken1141  dieselbe Kampf-Lage, Elza mit der SIG P228 (Waffe 4, 12 @0x8006ee48), drei ECHTE Schuesse (Pad R1 +
 *               Quadrat, re15_player_weapon_fire): vorher Clip 7 bei jedem Schuss, nachher erst beim dritten (36 Energie)
 *   ablauf1141  der ganze Raum-Ablauf: Phase H -> Gemaelde -> BEFREIUNG (Szene, die Gorillas brechen ein) -> Kampf mit
 *               P228-Treffern (Schreibsatz des Resolvers @0x80012420-2c, je 12) bis zum Tod beider Gorillas -> Kill-Flags
 *               (7,0xCB)/(7,0xCC) vom Todes-Commit -> sub01 @0x00C0C (4,0xF2) = 0 -> Johns Abgang (Cut 7, geparkt,
 *               (9,67), Riegel (4,0xC3)) -> Dachtuer -> ROOM1171. Vorher 16 Treffer je Gorilla (15 Flinches), nachher 31
 *               Treffer (10 Flinches, jeder 3.): der Ablauf bleibt mit der doppelten Energie lauffaehig.
 *   affe29      Gegenprobe kleiner Affe 0x29 (ROOM3040, Sce_em_set @0x01310/@0x01324): Regel 0, HP und die Antwort auf
 *               einen Pistolentreffer (14 = Zeile 0x29 Waffe 3) Bild fuer Bild gleich mit Haken aus/an
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
#include "re15_dining_r38.h"
#include "re15_gegner_r39.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
void re15_player_cmd_zero(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

#define P228_ID      4    /* Elzas SIG P228 (ROOM1051 Item 0x04, ablauf_elza.md E3) = RE1.5-Waffe 4 */
#define P228_SCHADEN 12   /* Zeile 0x27 @0x8006ee38 (Schritt 4 Byte je Waffe): Waffe 4 @0x8006ee48 = 12 (re15_damage.c s_wpn_dmg_maggot[4]) */

/* ---- Harness (Muster test_r38_dining1141.c) ------------------------------------------------------------------ */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

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

static void spielbild(uint16_t held, uint16_t edge)
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
    s_bild++;
}

/* Wegwerf-Abfrage sichtbar -> KEIN Spielbild (wie platform/pc/main.c / test_r38_dining1141.c frame) */
static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}

/* Banken: Elza-Raeume laden aus CDEMD1.EMS (exe-Messlauf: "[elzaems] Typ 0x27 aus EMD/CDEMD1.EMS Sektor 893",
 * "Typ 0x49 ... Sektor 1709"); STAGE3 (Leon) 0x29 aus CDEMD0.EMS. */
static uint8_t s_bank[3][0x60000];
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
    uint8_t *bank = s_bank[platz % 3];
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

/* Raumaufbau wie der Tuer-Weg (Muster test_r38_dining1141.c room_boot); Flags bleiben. */
static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, int band, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    if (room == 0x1141u) { (void)bank_laden("CDEMD1.EMS", 0x49, 0); (void)bank_laden("CDEMD1.EMS", 0x27, 1); }
    if (room == 0x3040u) (void)bank_laden("CDEMD0.EMS", 0x29, 2);
    re15_discard_reset();
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;   /* Elza-RDT = Leon-RDT + 1 (@0x800397e4); 4 = Elza */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = (uint8_t)band; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(band);
    s_shown = cut; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

/* Schreibsatz des Resolvers (re15_player_weapon_fire / FUN_80011f50 @0x80012420-2c, Muster test_r39_gegner.c):
 * +0x5 = Waffen-Id (Spur ueber @0x801214a8: Waffe 4 -> 0x8011b018 = Boden-Flinch), +0x6 = 1, +0x7 = 0, hp -= dmg,
 * +0x93 |= 1, +0x4 = 2 (hp >= 0, `bgez` @0x80012528) bzw. 3. */
static void treffer(re15_actor_t *g, int waffe, int dmg)
{
    g->sub_state_1 = (uint8_t)waffe; g->sub_state_2 = 1; g->sub_state_3 = 0;
    g->hp = (int16_t)(g->hp - dmg); g->hit_react |= 1;
    g->state = (g->hp >= 0) ? 2 : 3;
}

/* Kampf-Lage ROOM1141 ohne Szene: (4,0xF2) = 1, (4,0xF0) = 0 -> sub00 Else-Zweig @0x00B46-@0x00B7C (Ck(4,0xF2) @0x00B4A,
 * Sce_em_set @0x00B54 (-6600,-9800) / @0x00B68 (-5550,-19750), grid 0x10). Elza an der Doppeltuer (main00 @0x0088E). */
static int kampf_lage(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0xF2, 1);
    if (room_boot(0x1141, -7600, 0, -17600, 0, 0, 0) != 0) return 0;
    for (int f = 0; f < 2; f++) frame(0, 0);
    return aktor_vom_typ(0x27, 0) && aktor_vom_typ(0x27, 1);
}

/* ================================================================== hp1141 ================================== */
static void teil_hp1141(void)
{
    printf("== hp1141 ==\n");
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        int ok = kampf_lage();
        PRUEF(ok, "[%s] ROOM1141 Kampf-Lage: zwei Gorillas 0x27 aus sub00 @0x00B54/@0x00B68", tag);
        if (!ok) continue;
        re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
        PRUEF(a->grid_id == 0x10 && b->grid_id == 0x10, "[%s] grid 0x%02x/0x%02x (Kampf-Variante `44 01 27 10` / `44 02 27 10`)",
              tag, a->grid_id, b->grid_id);
        PRUEF(g_gameflow.character == 4, "[%s] Figur 4 = Elza (Elza-RDT)", tag);
        const int soll = an ? 2 * 180 : 180;
        PRUEF(a->hp == soll && b->hp == soll,
              "[%s] ROOM1141 HP beider Gorillas = %d (gemessen %d / %d; Tabelle @0x8011f514 = 180, Saat @0x80117104%s)",
              tag, soll, (int)a->hp, (int)b->hp, an ? ", NUTZER-VORGABE x2" : "");
        PRUEF(re15_r39_affen_akku(a) == 0 && re15_r39_affen_akku(b) == 0, "[%s] Energie-Akku startet bei 0", tag);
    }
    /* Die Regel haengt am Typ, nicht am Raum (Nachbesserung 2): derselbe INIT-Haken in drei Raumnummern. */
    re15_r39_gegner_enable(1);
    re15_actor_t *d = &g_actors[5];
    const uint16_t raum_alt = g_current_room_id;
    static const uint16_t raeume[] = { 0x11C0, 0x1141, 0x2000 };
    for (unsigned i = 0; i < sizeof raeume / sizeof raeume[0]; i++) {
        memset(d, 0, sizeof *d); d->type = 0x27;
        g_current_room_id = raeume[i];
        PRUEF(re15_r39_affen_regel(d) == 1 && re15_r39_affen_hp_init(d, 180) == 360,
              "Raum %04X: Gorilla 0x27 -> Regel an, HP 2 x 180 = 360 (kein Raum-Tor mehr)", (unsigned)raeume[i]);
        d->type = 0x29;
        PRUEF(re15_r39_affen_regel(d) == 0 && re15_r39_affen_hp_init(d, 105) == 105,
              "Raum %04X: kleiner Affe 0x29 -> Regel aus, Tabellenwert bleibt", (unsigned)raeume[i]);
    }
    memset(d, 0, sizeof *d);
    g_current_room_id = raum_alt;
}

/* ================================================================== zucken1141 =============================== */
static int schuss(re15_actor_t *g, int *clip7, int *hp_vor, int *hp_nach)
{
    *clip7 = 0; *hp_vor = g->hp;
    for (int f = 0; f < 20; f++) frame(RE15_PAD_BIT_R1, 0);
    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE), RE15_PAD_BIT_SQUARE);
    int getroffen = (g->hp < *hp_vor);
    for (int f = 0; f < 45; f++) {
        frame(RE15_PAD_BIT_R1, 0);
        if (g->motion == 7 && g->state == 2) *clip7 = 1;
    }
    for (int f = 0; f < 10; f++) frame(0, 0);
    *hp_nach = g->hp;
    return getroffen;
}

static void ruhig(re15_actor_t *g)
{
    /* Sub 0 Idle-Wander ohne Entscheid (+0x1dc != 0 -> kein Entscheid @0x80117490-98), wie test_r39_gegner.c */
    g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
    g->state = 1; g->sub_state_1 = 0; g->sub_state_2 = 0; g->sub_state_3 = 0; g->dog_blocked_ctr = 30000;
}

static void teil_zucken1141(void)
{
    printf("== zucken1141 ==\n");
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        if (!kampf_lage()) { PRUEF(0, "[%s] Kampf-Lage", tag); continue; }
        re15_actor_t *g = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
        g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000;     /* zweiter Gorilla geparkt (Root-Skip @0x80116df4-f8) */
        ruhig(g);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* Elza mit der SIG P228: Platz 0 = 0x04 x15, Munition 0x15 x60, ausgeruestet (Waffe 4) */
        re15_inv_init();
        g_inv.slots[0].id = P228_ID; g_inv.slots[0].qty = 15;
        g_inv.slots[1].id = 0x15;    g_inv.slots[1].qty = 60;
        re15_player_set_equipped_weapon(P228_ID);
        re15_player_set_aim_clip_len(12);
        pl->x = g->x + 2000; pl->z = g->z; pl->y = g->y;
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(g->z - pl->z, g->x - pl->x) - 0x400) & 0xfff);
        for (int f = 0; f < 2; f++) frame(0, 0);
        int treffer_n = 0, fl[4] = {0, 0, 0, 0}, hpv[4] = {0}, hpn[4] = {0}, akku[4] = {0};
        for (int k = 1; k <= 3; k++) {
            treffer_n += schuss(g, &fl[k], &hpv[k], &hpn[k]);
            akku[k] = re15_r39_affen_akku(g);
            printf("    [%s] Schuss %d: hp %d -> %d, Clip 7 = %d, Zustand %d/%d, akku=%d\n", tag, k, hpv[k], hpn[k], fl[k],
                   g->state, g->sub_state_1, akku[k]);
            if (fl[k]) ruhig(g);
            pl->x = g->x + 2000; pl->z = g->z;
            pl->rot_y = (int16_t)(((int)re15_atan2_q12(g->z - pl->z, g->x - pl->x) - 0x400) & 0xfff);
        }
        PRUEF(treffer_n == 3, "[%s] drei P228-Schuesse treffen (re15_player_weapon_fire, Waffe %d): %d/3", tag, P228_ID, treffer_n);
        if (treffer_n < 3) continue;
        PRUEF(hpv[1] - hpn[1] == P228_SCHADEN && hpv[2] - hpn[2] == P228_SCHADEN && hpv[3] - hpn[3] == P228_SCHADEN,
              "[%s] jeder Schuss zieht %d ab (@0x8006ee48): %d/%d/%d", tag, P228_SCHADEN,
              hpv[1] - hpn[1], hpv[2] - hpn[2], hpv[3] - hpn[3]);
        if (!an) PRUEF(fl[1] && fl[2] && fl[3], "[vorher] jeder Schuss flincht (Clip 7 @0x8011b08c-90): %d %d %d", fl[1], fl[2], fl[3]);
        else {
            PRUEF(!fl[1] && !fl[2] && fl[3], "[nachher] Flinch erst beim dritten Schuss (36 Energie): %d %d %d", fl[1], fl[2], fl[3]);
            PRUEF(hpv[1] == 360 && hpn[3] == 324, "[nachher] HP 360 -> 324 ueber drei Schuesse: %d -> %d", hpv[1], hpn[3]);
            PRUEF(akku[1] == 12 && akku[2] == 24, "[nachher] Akku 12 / 24 vor dem Flinch: %d / %d", akku[1], akku[2]);
        }
    }
}

/* ================================================================== ablauf1141 =============================== */
/* Phase H -> Gemaelde "ja" -> Szene BEFREIUNG bis zur Rueckgabe der Steuerung (Muster test_r38_dining1141.c
 * bis_zur_ecke): liefert 1, wenn der Kampf laeuft ((4,0xF2) = 1, beide Gorillas aktiv). */
static int bis_zum_kampf(void)
{
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);    /* (4,0xF0): ROOM1021 sub02 @0x01FE0 */
    re15_game_flag_set(9, RE15_R38F_BIT_TUER, 1);                 /* Tuer 1131 aufgeschlossen ((9,66)) */
    if (room_boot(0x1141, -7600, 0, -17600, 0, 0, 0) != 0) return 0;
    if (re15_dining_r38_phase_h() != 1) { printf("  FEHLER: keine Phase H\n"); return 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 60; f++) frame(0, 0);
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;               /* Gemaelde (Aot slot 3 @0x00B88), Blick -X */
    frame(0, 0);
    int ja = 0;
    for (int f = 0; f < 600 && !ja; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        ja = re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT);
    }
    if (!ja) { printf("  FEHLER: Gemaelde-Schalter nicht ausgeloest\n"); return 0; }
    /* Runde 39 Spur J (bei der Integration nachgezogen, Merge in r39/dining1141 Nachbesserung 2): seit J Nachbesserung 1
     * (Nutzer-Nachtrag 4) oeffnet das Gemaelde in Phase H nur den Rolladen (ROLLTOR39), danach laeuft Elza frei; die Szene mit
     * John und dem Gorilla-Einbruch startet erst am Riegel der Dachtuer (Ereignis 5, main00 @0x00A9A -> sub05 @0x00FC8, msg 12
     * mit Pausemaske `2b 0c ff ff`). Rolladen abwarten, Riegel druecken (Vorwaerts-Punkt (-7250,-250), Blick +Z), msg 12
     * schliessen - Muster test_r38_dining1141.c bis_zur_ecke. */
    {   int pm = 0, f1 = 0;
        for (int f = 0; f < 1200 && !f1; f++) { frame(0, 0); if (g_scd.player_mode == 2) pm = 1; if (pm && g_scd.player_mode == 0) f1 = 1; }
        if (!f1) { printf("  FEHLER: Rolladen-Szene gab die Steuerung nicht zurueck\n"); return 0; }
        pl->x = -7250; pl->z = -250 - 620; pl->rot_y = 3072;
        frame(0, 0);
        for (int f = 0; f < 300 && !re15_game_flag_get(4, 0xC3); f++)
            frame(f < 12 ? RE15_PAD_BIT_SQUARE : 0, f == 0 ? RE15_PAD_BIT_SQUARE : 0);
        if (!re15_game_flag_get(4, 0xC3)) { printf("  FEHLER: Riegel der Dachtuer nicht ausgeloest\n"); return 0; }
    }
    int pm2 = 0, frei = 0;
    for (int f = 0; f < 4000 && !frei; f++) {
        { uint16_t e = (g_scd.message_active && g_scd.message_id == 12 && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
        if (g_scd.player_mode == 2) pm2 = 1;
        if (pm2 && g_scd.player_mode == 0 && !g_scd.message_active && f > 200) frei = 1;
    }
    if (!frei) { printf("  FEHLER: Szene gab die Steuerung nicht zurueck\n"); return 0; }
    return re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT) && aktor_vom_typ(0x27, 0) && aktor_vom_typ(0x27, 1);
}

typedef struct { int treffer, flinch, hp_start, tot_bild; } kampf_t;

static void teil_ablauf1141(void)
{
    printf("== ablauf1141 ==\n");
    for (int an = 0; an <= 1; an++) {
        const char *tag = an ? "nachher" : "vorher";
        re15_r39_gegner_enable(an);
        int ok = bis_zum_kampf();
        PRUEF(ok, "[%s] Phase H -> Gemaelde -> BEFREIUNG: Kampf laeuft ((4,0xF2) = 1, zwei Gorillas)", tag);
        if (!ok) continue;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *john = &g_actors[1];
        re15_actor_t *g[2] = { aktor_vom_typ(0x27, 0), aktor_vom_typ(0x27, 1) };
        kampf_t k[2] = { { 0, 0, g[0]->hp, -1 }, { 0, 0, g[1]->hp, -1 } };
        const int soll_hp = an ? 360 : 180;
        PRUEF(k[0].hp_start == soll_hp && k[1].hp_start == soll_hp,
              "[%s] Gorillas aus BEFREIUNG (Saetze wie sub00 @0x00B1A/@0x00B2E) mit HP %d / %d (soll %d)", tag,
              k[0].hp_start, k[1].hp_start, soll_hp);
        PRUEF(g[0]->em_flag_id == 0xCB && g[1]->em_flag_id == 0xCC, "[%s] Kill-Flag-Indizes pc[7] 0x%02X / 0x%02X (0xCB/0xCC)",
              tag, g[0]->em_flag_id, g[1]->em_flag_id);
        pl->x = -3000; pl->z = -9000; pl->rot_y = 0;               /* Elza mitten im Raum, nicht in Johns Bahn */
        /* Kampf: jeder Gorilla, der treffbar ist (+0x93 Bit 0 frei = der Einmal-Riegel @0x8001300c, nicht tot),
         * bekommt einen P228-Treffer; Flinch = Wechsel auf Clip 7 in Zustand 2 (@0x8011b08c-90). */
        int min_spieler_hp = pl->hp;
        int mo_alt[2] = { 0, 0 };
        long kampf_ende = -1;
        for (int f = 0; f < 3000 && kampf_ende < 0; f++) {
            for (int i = 0; i < 2; i++) {
                re15_actor_t *e = g[i];
                if (!e->active || e->hp < 0 || e->state == 3 || e->state == 7) continue;
                if ((e->hit_react & 1) == 0 && (e->state == 1 || e->state == 2)) { treffer(e, P228_ID, P228_SCHADEN); k[i].treffer++; }
            }
            frame(0, 0);
            for (int i = 0; i < 2; i++) {
                re15_actor_t *e = g[i];
                /* Flinch-Beginn: Zustand 2 mit Clip 7, im Bild davor nicht (zwischen zwei Flinches liegt immer das
                 * Exit-Bild in Zustand 1 @0x8011b188 — der Clip kann dort noch 7 sein, darum Zustand UND Clip) */
                const int fl_jetzt = (e->state == 2 && e->motion == 7);
                if (fl_jetzt && !mo_alt[i]) k[i].flinch++;
                mo_alt[i] = fl_jetzt;
                if (k[i].tot_bild < 0 && (e->state == 3 || e->state == 7 || e->hp < 0)) k[i].tot_bild = (int)s_bild;
            }
            if (pl->hp < min_spieler_hp) min_spieler_hp = pl->hp;
            if (k[0].tot_bild >= 0 && k[1].tot_bild >= 0) kampf_ende = s_bild;
        }
        for (int i = 0; i < 2; i++)
            printf("    [%s] Gorilla %d: HP %d, %d Treffer bis zum Tod (Bild %d), %d Flinches (Clip 7)\n", tag, i + 1,
                   k[i].hp_start, k[i].treffer, k[i].tot_bild, k[i].flinch);
        PRUEF(kampf_ende >= 0, "[%s] beide Gorillas tot (Bild %ld)", tag, kampf_ende);
        const int soll_tr = an ? 31 : 16;                  /* hp >= 0 bleibt HURT (`bgez` @0x80012528): 360/12 + 1, 180/12 + 1 */
        const int soll_fl = an ? 10 : 15;                  /* nachher jeder 3. von 30 nicht-toedlichen, vorher jeder */
        PRUEF(k[0].treffer == soll_tr && k[1].treffer == soll_tr, "[%s] Treffer bis zum Tod: %d / %d (soll %d)", tag,
              k[0].treffer, k[1].treffer, soll_tr);
        PRUEF(k[0].flinch == soll_fl && k[1].flinch == soll_fl, "[%s] Flinches (Clip 7): %d / %d (soll %d)", tag,
              k[0].flinch, k[1].flinch, soll_fl);
        PRUEF(min_spieler_hp > 0, "[%s] Elza lebt durch den Kampf (kleinste HP %d)", tag, min_spieler_hp);
        /* Kill-Flags vom Todes-Commit (nicht vom Riegel gesetzt), dann sub01 @0x00BEE-@0x00C14 */
        long flags_bild = -1, f2_bild = -1, park_bild = -1;
        for (int f = 0; f < 3000; f++) {
            frame(0, 0);
            if (flags_bild < 0 && re15_game_flag_get(7, 0xCB) && re15_game_flag_get(7, 0xCC)) flags_bild = s_bild;
            if (f2_bild < 0 && !re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT)) f2_bild = s_bild;
            if (park_bild < 0 && john->x <= -29000) park_bild = s_bild;
            if (park_bild >= 0 && s_bild > park_bild + 40 && g_scd.player_mode == 0) break;
        }
        PRUEF(flags_bild >= 0, "[%s] Kill-Flags (7,0xCB)/(7,0xCC) vom Todes-Commit gesetzt (Bild %ld)", tag, flags_bild);
        PRUEF(f2_bild >= 0, "[%s] sub01 @0x00C0C: (4,0xF2) = 0 (Bild %ld)", tag, f2_bild);
        PRUEF(park_bild >= 0, "[%s] Johns Abgang: geparkt in Bild %ld", tag, park_bild);
        PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1 && re15_game_flag_get(4, 0xC3) == 1,
              "[%s] (9,%d) = 1, Riegel (4,0xC3) = 1", tag, RE15_R38F_BIT_JOHN_WEG);
        PRUEF(g_scd.player_mode == 0, "[%s] Steuerung frei", tag);
        PRUEF(g_aot.slots[1].active && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[1].dest_room == 0x17,
              "[%s] Slot 1 = Dachtuer -> Raum 0x%02X (soll 0x17 = ROOM1171)", tag, g_aot.door_params[1].dest_room);
    }
}

/* ================================================================== affe29 =================================== */
typedef struct { uint8_t st, s1, s2, s3, mo; int16_t hp; int32_t x, z; } spur29_t;

static int affe29_lauf(int an, spur29_t *sp, int n, int16_t *hp0, int16_t *hp1)
{
    re15_r39_gegner_enable(an);
    re15_game_state_init();
    if (room_boot(0x3040, -8800, -1800, -4531, 0, 1, 0) != 0) return 0;
    for (int f = 0; f < 3; f++) frame(0, 0);
    re15_actor_t *a = aktor_vom_typ(0x29, 0), *b = aktor_vom_typ(0x29, 1);
    if (!a || !b) return 0;
    *hp0 = a->hp; *hp1 = b->hp;
    if (re15_r39_affen_regel(a) != 0) return -1;
    treffer(a, 3, 14);                                             /* Pistole: Zeile 0x29 Waffe 3 = 14 (re15_damage.c s_wpn_dmg_roach[3]) */
    for (int f = 0; f < n; f++) {
        frame(0, 0);
        sp[f].st = a->state; sp[f].s1 = a->sub_state_1; sp[f].s2 = a->sub_state_2; sp[f].s3 = a->sub_state_3;
        sp[f].mo = a->motion; sp[f].hp = a->hp; sp[f].x = a->x; sp[f].z = a->z;
    }
    return 1;
}

static void teil_affe29(void)
{
    printf("== affe29 ==\n");
    enum { N = 90 };
    static spur29_t v[N], w[N];
    int16_t va = 0, vb = 0, wa = 0, wb = 0;
    const int rv = affe29_lauf(0, v, N, &va, &vb);
    const int rw = affe29_lauf(1, w, N, &wa, &wb);
    PRUEF(rv == 1 && rw == 1, "ROOM3040: zwei kleine Affen 0x29 (Sce_em_set @0x01310/@0x01324), Regel fuer 0x29 aus (%d/%d)", rv, rw);
    if (rv != 1 || rw != 1) return;
    PRUEF(va == wa && vb == wb, "0x29-HP nach dem INIT gleich mit Haken aus/an: %d/%d vs %d/%d (keine Verdopplung)", va, vb, wa, wb);
    int erstes = -1, reagiert = 0;
    for (int f = 0; f < N; f++) {
        if (v[f].st != 1) reagiert = 1;
        if (erstes < 0 && memcmp(&v[f], &w[f], sizeof v[f]) != 0) erstes = f;
    }
    PRUEF(reagiert, "der Pistolentreffer loest die 0x29-Reaktion aus (Zustand != 1 in %d Bildern)", N);
    PRUEF(erstes < 0, "0x29-Antwort auf den Treffer Bild fuer Bild gleich mit Haken aus/an (%d Bilder, erste Abweichung %d)", N, erstes);
    re15_r39_gegner_enable(1);
}

static int s_n_seq = 0;
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; s_n_seq++; }

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "hp1141"))     teil_hp1141();
    if (alle || !strcmp(teil, "zucken1141")) teil_zucken1141();
    if (alle || !strcmp(teil, "ablauf1141")) teil_ablauf1141();
    if (alle || !strcmp(teil, "affe29"))     teil_affe29();
    re15_door_seq_setze_laeufer(NULL);
    re15_r39_gegner_enable(1);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
