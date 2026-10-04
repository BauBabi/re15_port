/* ============================================================================================
 * Runde 35 Spur C — Zombie-Maedchen (Typ 0x13, EM013) in seinem EINZIGEN Raum ROOM4050/4051.
 * Dossier: analysis/befunde_runde35/C_zgirl.md
 *
 * Teile (Argument):
 *   zensus    T1  echtes ROOM4050/4051.RDT, main00 je Eintritts-Cut 0..14 ueber scd_room_reenter:
 *                 nur Cut 9 -> 0x13 @(-9900,0,1150) dir 512 Kill-Flag 0xa0 (RDT @0x01eb4) und
 *                 Cut 14 -> 0x13 @(1600,0,4700) dir 1024 Kill-Flag 0x7d (@0x01f5c); Cut 6/11 -> 0x18;
 *                 sonst kein Gegner. Switch(work_vars[0x0A]) @0x01E56 `13 0a d2 01`.
 *   killflag  T2  Kill-Flag gesetzt (Zone 8 = 0x800b1058, Stage-Index 3 >= 3) -> kein Spawn
 *                 (FUN_800420a0 @0x80042120-38).
 *   tuer      T3  Selbst-Tueren: Slot 6 -> Szenario 9, Slot 7 -> 14 (g_scd_pending_scenario), der
 *                 Verbraucher-Aufruf scd_room_reenter(..., sc) spawnt das Maedchen; Slot 0 (ROOM4040)
 *                 -> Raumwechsel, KEIN Szenario. Original: FUN_8001d600 @0x8001d968 vergleicht nur
 *                 die Stage, @0x8001d988 `jal 0x800396fc` laedt unbedingt.
 *                 Gegenprobe Stage 1: ROOM1090 Slot 3 (Selbst-Tuer, Cut 6) unveraendert.
 *   ki_re15   T4  RE1.5-KI in ROOM4050 Cut 9, Leon am Tuer-6-Ziel (-9350,-2600): INIT HP 50..81
 *                 (STAGE4 @0x8010abb8-d0), Annaeherung, Griff mit -10 (Aufprall) und -5 (Bisse),
 *                 Ansprung NIE scharf (@0x8010aca4 `andi 1`, ROOM4050 ohne Set(1,31)).
 *   ki_re2    T5  RE2-KI (Default des Spiels): Annaeherung + Griff + Spieler-HP sinkt.
 *   tod       T6  toedlicher Treffer -> Tod -> Leiche (Zustand 7) -> Kill-Flag 0xa0 in Zone 8 ->
 *                 Wiedereintritt Cut 9 spawnt NICHT mehr.
 *   messer    T7  Messerhieb re15_player_weapon_fire(1) -> +0x5 = Waffen-Id 1 (@0x800124bc); die
 *                 Original-Zeile 1 ist beim Maedchen 0 (jalr 0, unfertig) -> Port faehrt das
 *                 Standard-Zurueckzucken (RE2: ein Zombie wie alle), kein Haenger.
 *   selbsttueren T8 alle Raeume: jede Selbst-Tuer aus main00 (Cut-0-Eintritt) setzt das Szenario
 *                 = Ziel-Cut, der Wiedereintritt + 60 SCD-Bilder laufen durch.
 * ============================================================================================ */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re15_inventory.h"
#include "re15_enemy.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int re15_player_is_grabbed(void);

static int fehler = 0;
static void pruefe(const char *was, int ok_)
{
    printf("   [%s] %s\n", ok_ ? "OK " : "FEHL", was);
    if (!ok_) fehler++;
}

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

static re15_rdt_t s_rdt;
static uint8_t   *s_buf;

static int raum_laden(uint16_t rid)
{
    char rel[64]; size_t n = 0;
    snprintf(rel, sizeof rel, "STAGE%u/ROOM%04X.RDT", (unsigned)(rid >> 12), (unsigned)rid);
    free(s_buf);
    s_buf = datei(rel, &n);
    if (!s_buf) return 77;
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(s_buf, n, &s_rdt) != 0) return 1;
    g_current_room_id = rid;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    return 0;
}

/* Frischer Spielstand + Raumstart wie der Tuer-Weg (scd_room_reenter). keep_flags: Flags nicht
 * loeschen (Wiedereintritt nach Kill). */
static void raum_start(int32_t px, int32_t pz, uint8_t cut, int keep_flags)
{
    /* scd_vm_init ruft re15_game_state_init (scd_vm.c) und loescht damit die Flags — der
     * Wiedereintritt des Spiels laeuft nur ueber scd_room_reenter, die Flags bleiben. */
    if (!keep_flags) { re15_game_state_init(); scd_vm_init(); }
    re15_inv_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_ai_set_paused(0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 1;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0; pl->floor = 0;
    g_scd.player_mode = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, px, pz, cut);
    pl->x = px; pl->z = pz;
}

static int gegner(uint8_t typ, int *slot_out)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (slot_out && !n) *slot_out = s; n++; }
    return n;
}
static int alle_gegner(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) n++;
    return n;
}

/* ------------------------------------------------------------------------------ T1 zensus */
static void t_zensus(uint16_t rid)
{
    printf(" T1 zensus ROOM%04X\n", (unsigned)rid);
    for (int cut = 0; cut <= 14; cut++) {
        raum_start(-7000, -24800, (uint8_t)cut, 0);
        int gs = -1, n13 = gegner(0x13, &gs), n18 = gegner(0x18, NULL), na = alle_gegner();
        char m[200];
        if (cut == 9 || cut == 14) {
            const re15_actor_t *g = gs > 0 ? &g_actors[gs] : NULL;
            int okp = g && (cut == 9 ? (g->x == -9900 && g->z == 1150 && g->rot_y == 512 && g->em_flag_id == 0xa0)
                                     : (g->x == 1600 && g->z == 4700 && g->rot_y == 1024 && g->em_flag_id == 0x7d));
            snprintf(m, sizeof m, "Cut %2d: genau ein 0x13 an der RDT-Lage (n13=%d alle=%d @(%d,%d) r%d kf=0x%02x)",
                     cut, n13, na, g ? g->x : 0, g ? g->z : 0, g ? g->rot_y : 0, g ? g->em_flag_id : 0);
            pruefe(m, n13 == 1 && na == 1 && okp && g->grid_id == 0x00);
        } else if (cut == 6 || cut == 11) {
            snprintf(m, sizeof m, "Cut %2d: ein 0x18, kein 0x13 (n18=%d n13=%d)", cut, n18, n13);
            pruefe(m, n18 == 1 && n13 == 0 && na == 1);
        } else {
            snprintf(m, sizeof m, "Cut %2d: kein Gegner (alle=%d)", cut, na);
            pruefe(m, na == 0);
        }
    }
}

/* ---------------------------------------------------------------------------- T2 killflag */
static void t_killflag(void)
{
    printf(" T2 killflag ROOM4050\n");
    raum_start(-7000, -24800, 0, 0);
    pruefe("Stage 4 -> Kill-Zone 8 (0x800b1058)", re15_em_status_zone() == 8);
    re15_game_state_init();
    re15_game_flag_set(8, 0xa0, 1);
    raum_start(-9350, -2600, 9, 1);
    pruefe("Kill-Flag 0xa0 gesetzt -> Cut 9 spawnt kein 0x13", gegner(0x13, NULL) == 0);
    raum_start(1550, -1150, 14, 1);
    pruefe("Kill-Flag 0x7d frei -> Cut 14 spawnt weiter", gegner(0x13, NULL) == 1);
    re15_game_flag_set(8, 0x7d, 1);
    raum_start(1550, -1150, 14, 1);
    pruefe("Kill-Flag 0x7d gesetzt -> Cut 14 spawnt kein 0x13", gegner(0x13, NULL) == 0);
}

/* -------------------------------------------------------------------------------- T3 tuer */
static void t_tuer(void)
{
    printf(" T3 tuer ROOM4050\n");
    static const struct { int slot; int cut; int32_t sx, sz; int32_t gx, gz; } t[2] = {
        { 6,  9, -9350, -2600, -9900, 1150 },      /* RDT @0x01c16 / Spawn @0x01eb4 */
        { 7, 14,  1550, -1150,  1600, 4700 },      /* RDT @0x01c36 / Spawn @0x01f5c */
    };
    for (int i = 0; i < 2; i++) {
        raum_start(-7000, -24800, 0, 0);
        g_scd_pending_scenario = -1; g_room_change.pending = 0;
        pruefe("vor der Tuer: kein Gegner (Eintritt Cut 0)", alle_gegner() == 0);
        re15_aot_fire_slot(t[i].slot);
        char m[160];
        snprintf(m, sizeof m, "Slot %d feuert -> Szenario %d (ist %d), kein Raumwechsel (pending=%d)",
                 t[i].slot, t[i].cut, g_scd_pending_scenario, g_room_change.pending);
        pruefe(m, g_scd_pending_scenario == t[i].cut && !g_room_change.pending);
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        snprintf(m, sizeof m, "Leon am Tuer-Ziel (%d,%d) ist (%d,%d)", t[i].sx, t[i].sz, pl->x, pl->z);
        pruefe(m, pl->x == t[i].sx && pl->z == t[i].sz);
        /* der Verbraucher (game_step_common.c: scd_room_reenter(c->rdt, pl->x, pl->z, sc)) */
        uint8_t sc = (uint8_t)g_scd_pending_scenario; g_scd_pending_scenario = -1;
        scd_room_reenter(&s_rdt, pl->x, pl->z, sc);
        int gs = -1;
        snprintf(m, sizeof m, "nach dem Wiedereintritt: Zombie-Maedchen an (%d,%d)", t[i].gx, t[i].gz);
        pruefe(m, gegner(0x13, &gs) == 1 && g_actors[gs].x == t[i].gx && g_actors[gs].z == t[i].gz);
    }
    /* Kreuz-Raum-Tuer: Slot 0 -> ROOM4040 (Payload `03 04 00`, RDT @0x01b56) */
    raum_start(-7000, -24800, 0, 0);
    g_scd_pending_scenario = -1; g_room_change.pending = 0;
    re15_aot_fire_slot(0);
    pruefe("Slot 0 -> Raumwechsel nach ROOM4040, KEIN Szenario",
           g_room_change.pending && g_room_change.room_id == 0x4040 && g_scd_pending_scenario == -1);
    g_room_change.pending = 0;

    /* Gegenprobe Stage 1 (Verhalten vor Runde 35, dort stiegen alle 21 Selbst-Tueren schon neu ein):
     * ROOM1110 main00 Slot 1 @0x00a6e -> Cut 7 (selbsttuer_zensus.py). */
    if (raum_laden(0x1110) == 0) {
        raum_start(-4550, -3000, 0, 0);
        g_scd_pending_scenario = -1; g_room_change.pending = 0;
        char m[140];
        int ist_tuer = (g_aot.slots[1].type == RE15_AOT_TYPE_DOOR);
        if (ist_tuer) re15_aot_fire_slot(1);
        snprintf(m, sizeof m, "Stage 1 unveraendert: ROOM1110 Slot 1 -> Szenario 7 (Tuer=%d, ist %d)",
                 ist_tuer, g_scd_pending_scenario);
        pruefe(m, ist_tuer && g_scd_pending_scenario == 7 && !g_room_change.pending);
        g_scd_pending_scenario = -1; g_room_change.pending = 0;
    }
    raum_laden(0x4050);
}

/* --------------------------------------------------------------------------- T4/T5 ki */
/* Echter Spielschritt wie probe_1030_crawl_live: scd_vm_tick VOR re15_game_step, Gegner-Baenke
 * geladen (ohne Bank keine Wurzelbewegung -> keine Annaeherung). RE1.5-Bank aus EMD/CDEMD0.EMS
 * (re15_ems_index_for_type), RE2-Bank aus ../RE2/CDEMD0.EMS (re2_ems_load_bank) — dieselben
 * Quellen wie der PC-Lader. */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = datei("../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
static int bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = datei("EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}
static void schritt(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static void t_ki(int re2)
{
    printf(" %s ROOM4050 Cut 9, %s-KI (echter Spielschritt)\n", re2 ? "T5 ki_re2" : "T4 ki_re15", re2 ? "RE2" : "RE1.5");
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 9;
    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_enemy_reset(); re15_player_cmd_reset();
    re15_damage_seed_rng(0x4050u);
    re15_collision_set_band(0);
    raum_start(-9350, -2600, 9, 0);
    re15_inv_load_briefing();
    int ok_bank = re2 ? bank_re2(0x13) : bank_re15(0x13);
    char m[200];
    snprintf(m, sizeof m, "Gegner-Bank 0x13 geladen (%s)", re2 ? "RE2 CDEMD0.EMS" : "RE1.5 EMD/CDEMD0.EMS");
    pruefe(m, ok_bank);
    int gs = -1;
    pruefe("Spawn Cut 9", gegner(0x13, &gs) == 1);
    if (gs < 0) return;
    re15_actor_t *g = &g_actors[gs];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -9350; pl->z = -2600; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->rot_y = 3072;
    uint32_t d0 = 0, dmin = 0xffffffffu;
    int init_hp = -1, grab = 0, armed = 0, f_grab = -1;
    int16_t hp_vor = 100; int erster_abzug = 0, zweiter_abzug = 0;
    for (int f = 0; f < 900; f++) {
        schritt();
        if (f == 0) init_hp = g->hp;
        if (f == 1) d0 = g->ai_dist;
        if (f > 0 && g->ai_dist < dmin) dmin = g->ai_dist;
        if (g->ai_flags & 0x100) armed = 1;
        if (re15_player_is_grabbed() || (g->state == 1 && (g->sub_state_1 == 3 || g->sub_state_1 == 4))) {
            if (!grab) f_grab = f;
            grab = 1;
        }
        if (pl->hp < hp_vor) {
            int d = hp_vor - pl->hp;
            if (!erster_abzug) erster_abzug = d; else if (!zweiter_abzug) zweiter_abzug = d;
            hp_vor = pl->hp;
        }
        if (zweiter_abzug || pl->hp < 0) break;
    }
    if (!re2) {
        snprintf(m, sizeof m, "INIT: Zustand 1, HP %d in 50..81 (@0x8010abb8-d0)", init_hp);
        pruefe(m, g->state >= 1 && init_hp >= 50 && init_hp <= 81);
    } else {
        snprintf(m, sizeof m, "INIT: Zustand 1, HP %d > 0", init_hp);
        pruefe(m, g->state >= 1 && init_hp > 0);
    }
    snprintf(m, sizeof m, "Annaeherung: Abstand %u -> min %u", d0, dmin);
    pruefe(m, d0 > 3000 && dmin < 1300);
    snprintf(m, sizeof m, "Griff (ab Bild %d)", f_grab);
    pruefe(m, grab);
    if (!re2) {
        snprintf(m, sizeof m, "Schaden: erster Abzug %d (Aufprall -10 @0x8010277c), zweiter %d (Biss -5 @0x801027dc)",
                 erster_abzug, zweiter_abzug);
        pruefe(m, erster_abzug == 10 && zweiter_abzug == 5);
        pruefe("Ansprung nie scharf (+0x1d8 & 0x100 == 0; @0x8010aca4, kein Set(1,31) in ROOM4050)", !armed);
    } else {
        snprintf(m, sizeof m, "Schaden: erster Abzug %d, Spieler-HP 100 -> %d", erster_abzug, hp_vor);
        pruefe(m, erster_abzug > 0 && hp_vor < 100);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_enemy_reset();
}

/* -------------------------------------------------------------------------------- T6 tod */
static void t_tod(void)
{
    printf(" T6 tod ROOM4050 Cut 9, RE1.5-KI\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(0);
    re15_damage_seed_rng(0x7d7du);
    raum_start(-9350, -2600, 9, 0);
    int gs = -1;
    if (gegner(0x13, &gs) != 1) { pruefe("Spawn Cut 9", 0); return; }
    re15_actor_t *g = &g_actors[gs];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -9350; pl->z = -2600 - 20000;   /* weit weg: kein Griff */
    for (int f = 0; f < 3; f++) re15_enemy_ai_run_all(0);
    g->hp = 1;
    re15_enemy_take_damage(g, 0);
    pruefe("toedlicher Treffer -> Zustand 3 (@0x80013020)", g->state == 3);
    int leiche = 0;
    for (int f = 0; f < 400 && !leiche; f++) {
        re15_enemy_ai_run_all(0);
        if (g->state == (uint8_t)RE15_AI_STATE_CORPSE) leiche = 1;
    }
    pruefe("Tod -> Leiche (Zustand 7)", leiche);
    re15_enemy_ai_run_all(0);
    pruefe("Kill-Flag 0xa0 in Zone 8 gesetzt", re15_game_flag_get(8, 0xa0) == 1);
    raum_start(-9350, -2600, 9, 1);
    pruefe("Wiedereintritt Cut 9: kein Zombie-Maedchen mehr", gegner(0x13, NULL) == 0);
    re15_re15_re2z_import_set(1);
}

/* ------------------------------------------------------------------------------ T7 messer */
/* Der Messerhieb laeuft ueber re15_player_weapon_fire(1) (game_step_common.c, FUN_80011f50) und
 * stempelt +0x5 = Waffen-Id 1 (@0x800124bc). In der Treffer-Haupttabelle des Maedchens
 * @0x8012039c (STAGE1; STAGE4 0x801198c4) ist Zeile 1 Richtung 0/1 = 0 -> `jalr 0` im Original. */
static void t_messer(void)
{
    printf(" T7 messer ROOM4050 Cut 9, RE1.5-KI\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(0);
    re15_damage_seed_rng(0x1111u);
    raum_start(-9350, -2600, 9, 0);
    int gs = -1;
    if (gegner(0x13, &gs) != 1) { pruefe("Spawn Cut 9", 0); return; }
    re15_actor_t *g = &g_actors[gs];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 3; f++) re15_enemy_ai_run_all(0);   /* INIT -> Zustand 1, Trefferbox */
    g->hp = 60;
    pl->x = g->x; pl->z = g->z - 500; pl->rot_y = 3072;    /* Blick +z (Vorwaertspunkt-Formel aot_common.c) */
    int16_t hp0 = g->hp;
    int traf = re15_player_weapon_fire(1);
    char m[200];
    snprintf(m, sizeof m, "Messer trifft (ret %d): Zustand %d, +0x5 = %d (Waffen-Id 1), HP %d -> %d",
             traf, g->state, g->sub_state_1, hp0, g->hp);
    pruefe(m, g->state == 2 && g->sub_state_1 == 1 && g->hp < hp0);
    pl->z = g->z - 20000;                                  /* weg, damit kein Griff folgt */
    int zurueck = 0;
    for (int f = 0; f < 120 && !zurueck; f++) {
        re15_enemy_ai_run_all(0);
        if (g->state == 1) zurueck = 1;
    }
    snprintf(m, sizeof m, "Zurueckzucken endet in Zustand 1 (kein jalr-0-Haenger), Zustand %d", g->state);
    pruefe(m, zurueck);
    re15_re15_re2z_import_set(1);
}

/* -------------------------------------------------------------------------- T8 selbsttueren */
/* Die Verallgemeinerung trifft 46 Selbst-Tueren in Stage 2..6 (selbsttuer_zensus.py). Fuer jeden
 * Raum der Liste: Raumstart Cut 0, jede DOOR-AOT, deren Ziel (Stage/Raum/Variante, Formel aus
 * aot_fire_door) der Raum selbst ist, feuern -> Szenario == Ziel-Cut, kein Raumwechsel; dann den
 * Verbraucher (scd_room_reenter) fahren und 60 SCD-Bilder ticken — der Lauf muss durchkommen. */
#include "re15_room_list.h"
static void t_selbsttueren(void)
{
    printf(" T8 selbsttueren (alle Raeume, Eintritt Cut 0)\n");
    int n_tuer = 0, n_ok = 0, n_stage1 = 0, n_neu = 0;
    for (int i = 0; i < RE15_ROOM_COUNT; i++) {
        uint16_t rid = (uint16_t)re15_room_ids[i];
        if (raum_laden(rid) != 0) continue;
        raum_start(0, 0, 0, 0);
        int selbst[RE15_AOT_MAX], ns = 0;
        for (int sl = 0; sl < RE15_AOT_MAX; sl++) {
            if (g_aot.slots[sl].type != RE15_AOT_TYPE_DOOR) continue;
            const re15_aot_door_params_t *d = &g_aot.door_params[sl];
            unsigned ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | (rid & 0xFu);
            if (ziel != rid || (d->spawn_x == 0 && d->spawn_y == 0 && d->spawn_z == 0)) continue;
            selbst[ns++] = sl;
        }
        for (int k = 0; k < ns; k++) {
            int sl = selbst[k];
            raum_start(0, 0, 0, 0);
            const re15_aot_door_params_t *d = &g_aot.door_params[sl];
            n_tuer++;
            if ((rid >> 12) == 1) n_stage1++; else n_neu++;
            uint8_t cut = d->target_cut;
            g_scd_pending_scenario = -1; g_room_change.pending = 0;
            re15_aot_fire_slot(sl);
            int ok = (g_scd_pending_scenario == (int)cut && !g_room_change.pending);
            if (ok) {
                const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                uint8_t sc = (uint8_t)g_scd_pending_scenario; g_scd_pending_scenario = -1;
                scd_room_reenter(&s_rdt, pl->x, pl->z, sc);
                for (int f = 0; f < 60; f++) scd_vm_tick();
                n_ok++;
            } else {
                printf("   [FEHL] ROOM%04X Slot %d Cut %d: Szenario %d, Raumwechsel %d\n",
                       (unsigned)rid, sl, cut, g_scd_pending_scenario, g_room_change.pending);
            }
            g_scd_pending_scenario = -1; g_room_change.pending = 0;
        }
    }
    char m[200];
    snprintf(m, sizeof m, "%d Selbst-Tueren aus main00 bei Cut-0-Eintritt (Stage 1: %d, Stage 2..6: %d), "
                          "alle steigen neu ein: %d", n_tuer, n_stage1, n_neu, n_ok);
    pruefe(m, n_tuer > 0 && n_ok == n_tuer && n_neu >= 20);
    raum_laden(0x4050);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    int rc = raum_laden(0x4050);
    if (rc) return rc;
    printf("=== Runde 35 Spur C: Zombie-Maedchen ROOM4050 (%s) ===\n", teil);
    if (alle || !strcmp(teil, "zensus"))   { t_zensus(0x4050); if (raum_laden(0x4051) == 0) t_zensus(0x4051); raum_laden(0x4050); }
    if (alle || !strcmp(teil, "killflag")) t_killflag();
    if (alle || !strcmp(teil, "tuer"))     t_tuer();
    if (alle || !strcmp(teil, "ki_re15"))  t_ki(0);
    if (alle || !strcmp(teil, "ki_re2"))   t_ki(1);
    if (alle || !strcmp(teil, "tod"))      t_tod();
    if (alle || !strcmp(teil, "messer"))   t_messer();
    if (alle || !strcmp(teil, "selbsttueren")) t_selbsttueren();
    printf("=== %s: %d Fehler ===\n", teil, fehler);
    return fehler ? 1 : 0;
}
