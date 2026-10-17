/* ============================================================================================
 * probe_r31_boxen - Runde 30 / Welle 2: DIE TREFFERBOXEN +0x98 / +0x9E ALLER RE2-TYPEN
 *
 * Nutzer-Auftrag: "Und die Trefferboxen wenn die fehlen muessen natuerlich auch ermttelt und
 * uebernommen werden. Das soll alles sauber Resident Evil 2 entsprechen."
 *
 * Welle 2 hat die Boxen aus dem Vollscan aller `sh rt,152/158(rs)` in den fuenf
 * Gegner-Overlays (EMZ0 / EMD0G_MOD0 / EMOVL21_S0 / EMS25 / EMS26, roh @0x80100000) in den
 * Port uebernommen. Diese Sonde misst auf dem ECHTEN Weg (re15_game_step + Pad, echte RDTs,
 * RE2-KI-Geschmack, RE2-Bank shared_assets/RE2/CDEMD0.EMS geladen), ob
 *   (1) jeder Typ nach seinem INIT die Original-Werte traegt,
 *   (2) die KRIECHER-Box des Zombies KEINE Sackgasse ist (Gegenstelle = die Rampe
 *       @0x8010366C-94, gemessen in Bildern),
 *   (3) keine der Aenderungen einen Typ zugemacht hat,
 *   (4) welcher MECHANISMUS den Schuss sperrt. ⛔ RUNDE 34 UMGESTELLT: bis Runde 33 stand
 *       hier "darf das fuenfte Tor @0x8004716C-A4 je Typ scharf sein". Diese Frage ist
 *       beantwortet und die Antwort lautet NEIN — das Tor liegt nicht im Schuss-Pfad
 *       (FUN_800470C0 hat auf der Spielerseite nur die Aufrufstellen @0x80042F94 = Messer
 *       und @0x800467C0 = Bolzen; jede Schusswaffe geht ueber `jal 0x800410CC` @0x80043AFC).
 *       Teil 2c misst deshalb jetzt die HALTUNGSKLASSE (word0>>26)&7 (@0x800413C4-D8) gegen
 *       die Maskentabelle @0x800A6DB4, in drei Laeufen je Typ:
 *       KLASSE FEST 3 / WIE GEBAUT / KONTROLLE K=4. Zusaetzlich zaehlt die Sonde, wie oft
 *       das fuenfte Tor ueberhaupt noch befragt wird (muss beim Schuss 0 sein) und wie oft
 *       die Klasse gelesen wird (muss bei Applier-Typen > 0 sein).
 *       Die Boxen +0x98/+0x9E bleiben gemessen und gepinnt — sie sind weiterhin die Eingabe
 *       des MESSER-Tors.
 *   (5) RUNDE 38 (Spur R, Schachtspinnen ROOM2060/20A0): der Zensus sieht jetzt JEDE
 *       ausgelieferte Spinne; die vier Schachtspinnen warten mit leerem RE2-Kasten, der
 *       Original-Ausloeser (Raumskript sub01 -> Member_set grid 0x42) holt sie heraus, nach der
 *       Landung tragen sie -1400/1400 und werden getroffen (Teil 2e).
 *   (6) RUNDE 38 INTEGRATION (Argument `versteckt`, ctest r31_trefferboxen_versteckt): IM Schacht
 *       ist die Spinne wie im Original kein Ziel (Zielhilfe grid & 0x60 @0x8003710c-10, leeres
 *       Band @0x800120e4-ec, Latch @0x80012100) — RE2- und RE1.5-KI, Tuerweg Leon/Elza, plus
 *       der Zensus aller ausgelieferten Typen mit grid & 0x60.
 * ============================================================================================ */
#include "re15_emd.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re2_ems.h"
#include "re15_ems.h"      /* Runde 38 Integration: RE1.5-Bank (re15_ems_index_for_type / re15_ems_get_entry) */
#include "re15_math.h"     /* Runde 38 Integration: re15_squareroot0 = SquareRoot0 (Band-Soll der Stegspinne) */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int     re15_player_aim_ready(void);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void    re15_esp_fx_reset(void);
extern int     re15_player_gunbone_world(int32_t ox, int32_t oy, int32_t oz, int32_t out[3]);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static uint8_t           *s_keep = NULL;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Runde 38 Integration (Zensus grid & 0x60): ein Spielflag, das das Raumskript fuer den versteckten Zweig verlangt
 * (Ifel_ck vor dem Sce_em_set), NACH scd_vm_init setzen — re15_game_state_init loescht die Flags. -1 = keins. */
static int s_vorflag_bank = -1, s_vorflag_bit = 0;
static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    if (s_vorflag_bank >= 0) re15_game_flag_set((uint8_t)s_vorflag_bank, (uint8_t)s_vorflag_bit, 1);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int load_room(const char *sub, int room_id, int fire_sub)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return 0; }
    s_room_id = room_id; s_fire_sub = fire_sub;
    if (s_keep) free(s_keep);
    s_keep = buf;
    return 1;
}

static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    /* GLEICHE EBENE wie in probe_r29: der Spieler wird auf die Bodenhoehe des Gegners
     * gesetzt. OHNE diese Zeile bleibt pl->y auf 0, waehrend die RE2-Gegner je nach Raum
     * auf -530..-5400 stehen — dann misst die Sonde den Teleport-Versatz der Sonde selbst
     * und nicht die Spielgroesse (erster Lauf 2026-09-27 lief genau da hinein). */
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static uint16_t elev_pad_for(const re15_actor_t *e)
{
    if (e->type == 0x21) {
        if (e->aim_band == 4) return RE15_PAD_BIT_UP;
        if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
        return 0;
    }
    if (e->grid_id & 0x80) return RE15_PAD_BIT_DOWN;
    return 0;
}

/* RE2-Bank (CDEMD0.EMS) — OHNE sie haben alle Clips Laenge 0 und jede Animation ist nach
 * EINEM Bild "zu Ende" (Falle aus Runde 29). */
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != type) continue;
        if (type >= 0x10 && type <= 0x18 && (g_actors[s].grid_id & 0x80)) continue;
        return s;
    }
    return -1;
}

static int setup_target(uint8_t type, int weapon, int baby_force)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    load_re2_bank(type);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    if (baby_force) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x26)
                g_actors[s].re2s_baby_spawned = 1;
    }
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        if (baby_force)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x26)
                    g_actors[s].re2s_baby_spawned = 1;
        frame(0, 0);
    }
    int slot = find_type(type);
    if (slot < 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (type == 0x20 && g_actors[slot].state == 4 && g_actors[slot].grid_id == 0x40)
        g_actors[slot].grid_id = 0x43;
    return slot;
}

static void aim_up(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }
}

/* ===== Das Tor als reine Rechnung (kein Einbau) ========================================== */
/* DURCH <=> (uint32)(eY + b + 100 + h - zielY) < (uint32)(2*(h+100))   @0x8004717C-A4 */
static int tor(int32_t eY, int b, int h, int32_t zielY)
{
    uint32_t lhs = (uint32_t)(eY + b + 100 + h - zielY);
    uint32_t rhs = (uint32_t)(2 * (h + 100));
    return lhs < rhs;
}

/* ===== TEIL 1: DER BOX-ZENSUS IM PORT — traegt jeder Typ seine Original-Werte? ===========
 * Gemessen wird der Actor-Zustand nach dem INIT-Tick auf dem echten Weg. Die Soll-Werte
 * stehen jeweils mit der Adresse in BOXEN[] (Vollscan der fuenf Overlays, Welle 2). */
/* ⛔ Die Boxen sind ZUSTANDSABHAENGIG — je Typ steht darum die MENGE der Paare, die das
 * Original setzen kann, nicht ein einzelnes Paar. Jede Zeile traegt ihre Adresse. */
typedef struct {
    const char *tag; uint8_t type;
    int n; int b[4], h[4];
    const char *room; int room_id, fire_sub, weapon, baby;
    const char *quelle;
} boxsoll_t;

static const boxsoll_t SOLL[] = {
  { "ZOMBIE 0x10", 0x10, 2, { -1500, -350 }, { 1500, 350 },
    "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0,
    "EMZ0 INIT @0x8010095C/64 (stehend) | Kriecher @0x80100B18/20, @0x80103464/6C" },
  { "HUND   0x20", 0x20, 2, { -1000, -500 }, { 1000, 500 },
    "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0,
    "EMD0G INIT @0x80100294/9C | FUN_80104088 @0x8010409C/A8" },
  { "KRAEHE 0x21", 0x21, 2, { -350, 0 }, { 530, 530 },
    "STAGE1/ROOM10C0.RDT", 0x10C0, -1, 3, 0,
    "INIT @0x801003C4/DC | Neu-Berechner je Bild @0x801001EC-208 (+0x1F0 < 900 ? -350 : 0)" },
  { "SPINNE 0x25", 0x25, 2, { -1400, 0 }, { 1400, 1400 },
    "STAGE2/ROOM2050.RDT", 0x2050, -1, 3, 0,
    "Blockkopie Tab @0x801063A0 @0x801003BC-400 (Boden) | Decke @0x8010049C / "
    "Wand Tab @0x801063E0 @0x801004EC-52C" },
  { "BABY   0x26", 0x26, 1, { -10 }, { 10 },
    "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1,
    "EMS26 @0x80100168-78 (einziger Schreiber im Overlay)" },
};
#define SOLL_N ((int)(sizeof SOLL / sizeof SOLL[0]))

static int soll_ok(const boxsoll_t *S, int b, int h)
{
    for (int i = 0; i < S->n; i++) if (b == S->b[i] && h == S->h[i]) return 1;
    return 0;
}

typedef struct { int ok, b, h, box_set, durch, gueltig, hg_min, hg_max; } zensus_t;
static zensus_t s_zen[SOLL_N];

static void pass_zensus(int i, int budget)
{
    const boxsoll_t *S = &SOLL[i];
    zensus_t *z = &s_zen[i];
    memset(z, 0, sizeof *z);
    z->hg_min = 0x7fffffff; z->hg_max = -0x7fffffff;
    if (!load_room(S->room, S->room_id, S->fire_sub)) {
        printf("  [%-11s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", S->tag, S->room); return; }
    int slot = setup_target(S->type, S->weapon, S->baby);
    if (slot < 0) {
        printf("  [%-11s] FEHLLAUF: kein Gegner 0x%02X in %04X - sagt NICHTS\n",
               S->tag, S->type, S->room_id); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = 2000;
    aim_up(slot, back);
    z->b = e->re2_hit_b98; z->h = (int)e->re2_hit_h9e; z->box_set = e->re2_hit_box_set;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
        int32_t m[3];
        if (!re15_player_muzzle_world(m)) continue;
        z->gueltig++;
        int32_t hg = e->y - m[1];
        if (hg < z->hg_min) z->hg_min = hg;
        if (hg > z->hg_max) z->hg_max = hg;
        if (tor(e->y, e->re2_hit_b98, (int)e->re2_hit_h9e, m[1])) z->durch++;
    }
    z->ok = 1;
    printf("  [%-11s] Box %6d/%-5d %s | box_set=%d | Hgun %5d..%-5d | Tor DURCH %3d/%3d\n",
           S->tag, z->b, z->h, soll_ok(S, z->b, z->h) ? "OK " : "⛔ ",
           z->box_set, z->hg_min, z->hg_max, z->durch, z->gueltig);
    printf("                 Quelle: %s\n", S->quelle);
}

/* ===== TEIL 2: DIE ZOMBIE-KETTE — Setz-Stelle UND GEGENSTELLE in EINEM Lauf ==============
 * Der Nachweis, dass die Kriecher-Box KEINE Sackgasse ist: der Zombie wird
 * niedergeschlagen (Box -350/350), und die Rampe @0x8010366C-94 zieht sie je Bild um 10
 * auf, bis die Unterkante den Deckel -1500 erreicht. Gemessen wird die BILDERZAHL. */
typedef struct {
    int getroffen, klein_von, klein_bis, gross_wieder, min_b, max_h;
    int stand_durch, stand_bilder, klein_durch, klein_bilder, gross_durch, gross_bilder;
    int rampen_schritte;
} kette_t;
static kette_t s_kette2;

static void pass_zombie_kette(int budget)
{
    kette_t *r = &s_kette2;
    memset(r, 0, sizeof *r);
    r->getroffen = r->klein_von = r->klein_bis = r->gross_wieder = -1;
    r->min_b = 0x7fffffff; r->max_h = -0x7fffffff;
    if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        printf("  FEHLLAUF: ROOM1140 fehlt\n"); return; }
    int slot = setup_target(0x10, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Zombie 0x10 in 1140 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    int vor_b = e->re2_hit_b98;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        /* ⛔ NICHT auf 30000 festnageln: der Niederschlag ist im RE2-Schadenspfad an den
         * HP-Stand gekoppelt, ein unverwundbarer Zombie faellt nie. Stattdessen wird er nur
         * am Sterben gehindert. */
        if (e->hp < 60) e->hp = 400;
        if (!e->active) break;
        track(slot, 2000);
        int schuss = (f >= 10);
        frame((uint16_t)(RE15_PAD_BIT_R1 | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        int b = e->re2_hit_b98, h = (int)e->re2_hit_h9e;
        if (b < r->min_b) r->min_b = b;
        if (h > r->max_h) r->max_h = h;
        if (b != vor_b && b == vor_b - 10) r->rampen_schritte++;
        vor_b = b;
        int32_t m[3]; int mok = re15_player_muzzle_world(m);
        int durch = mok ? tor(e->y, b, h, m[1]) : -1;
        if (r->klein_von < 0) {
            if (b > -1500) { r->klein_von = f; }
            else { r->stand_bilder++; if (durch == 1) r->stand_durch++; continue; }
        }
        if (r->gross_wieder < 0) {
            if (b <= -1500) { r->gross_wieder = f; }
            else { r->klein_bis = f; r->klein_bilder++; if (durch == 1) r->klein_durch++;
                   continue; }
        }
        r->gross_bilder++; if (durch == 1) r->gross_durch++;
        if (r->gross_bilder >= 60) break;
    }
    printf("  STEHEND vor dem Treffer: Box -1500/1500, Tor DURCH %d/%d Bilder\n",
           r->stand_durch, r->stand_bilder);
    if (r->klein_von < 0) { printf("  ⛔ die Box wurde NIE klein — der Niederschlag kam nicht\n");
                            return; }
    printf("  KLEIN ab Bild %d | kleinste Box im Lauf %d/%d | %d Bilder klein | "
           "Tor DURCH %d/%d\n",
           r->klein_von, r->min_b, r->max_h, r->klein_bilder, r->klein_durch, r->klein_bilder);
    if (r->gross_wieder < 0) {
        printf("  ⛔ SACKGASSE: die Box wurde in %d Bildern NICHT wieder gross\n", budget);
        return; }
    printf("  WIEDER GROSS bei Bild %d = %d Bilder nach dem Niederschlag "
           "(Rampen-Schritte @0x8010366C-94 gezaehlt: %d)\n",
           r->gross_wieder, r->gross_wieder - r->klein_von, r->rampen_schritte);
    printf("  DANACH: Tor DURCH %d/%d Bilder\n", r->gross_durch, r->gross_bilder);
}

/* ===== TEIL 2b: DIE LAENGSTE SPERRE — waere das Tor fuer 0x10 gefahrlos scharf? =========
 * ⛔ Das ist die Runde-13/14-Frage in ihrer Hitbox-Form: eine Box, die klein wird und nie
 * wieder gross, macht den Gegner DAUERHAFT untreffbar. Gemessen wird deshalb nicht nur, DASS
 * die Gegenstelle existiert, sondern die LAENGSTE ununterbrochene Strecke, in der das Tor
 * @0x8004716C-A4 sperren wuerde — ueber einen langen Dauerbeschuss mit mehreren
 * Niederschlaegen. */
typedef struct { int bilder, durch, sperr_max, sperr_jetzt, nieder; } dauer_t;
static dauer_t s_dauer;

static void pass_zombie_dauer(int budget)
{
    dauer_t *r = &s_dauer; memset(r, 0, sizeof *r);
    if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        printf("  FEHLLAUF: ROOM1140 fehlt\n"); return; }
    int slot = setup_target(0x10, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Zombie 0x10 in 1140 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    int war_unten = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 60) e->hp = 400;
        if (!e->active) break;
        track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->state == 2 && !war_unten) { r->nieder++; war_unten = 1; }
        if (e->state != 2) war_unten = 0;
        int32_t m[3];
        if (!re15_player_muzzle_world(m)) continue;
        r->bilder++;
        if (tor(e->y, e->re2_hit_b98, (int)e->re2_hit_h9e, m[1])) {
            r->durch++; r->sperr_jetzt = 0;
        } else {
            r->sperr_jetzt++;
            if (r->sperr_jetzt > r->sperr_max) r->sperr_max = r->sperr_jetzt;
        }
    }
    printf("  %d Bilder Dauerbeschuss, %d Niederschlaege | Tor DURCH %d/%d | "
           "LAENGSTE ununterbrochene Sperre: %d Bilder (%.1f s @30 Hz)\n",
           budget, r->nieder, r->durch, r->bilder, r->sperr_max, r->sperr_max / 30.0);
}

/* ===== TEIL 2c: DER SPERR-MECHANISMUS — ECHTE Treffer ===================================
 * ⛔ RUNDE 34 UMGESTELLT. Bis Runde 33 mass dieser Teil das FUENFTE TOR (+0x98/+0x9E gegen
 * die Muendungshoehe) und schaltete es ueber re2_hit_box_set scharf/stumpf. Dieses Tor liegt
 * seit Runde 34 nicht mehr im Schuss-Pfad: FUN_800470C0 wird auf der Spielerseite nur von
 * Waffe 1 (Messer, @0x80042F94) und Waffe 12 (Bolzen, @0x800467C0) betreten, jede
 * Schusswaffe geht ueber `jal 0x800410CC` @0x80043AFC. Dort entscheidet die HALTUNGSKLASSE
 * (word0>>26)&7 (@0x800413C4-D8) gegen die Maskentabelle @0x800A6DB4.
 * Der Hebel dieses Teils greift deshalb jetzt an der KLASSE an:
 *   0 = MECHANISMUS AUS  : Klasse jedes Bild = 3 ("volle Box", `lui v1,0xc00` @0x801040CC)
 *                          -> die Stauchung FUN_80104088(0) ist zurueckgenommen
 *   1 = MECHANISMUS AN   : so, wie der Port laeuft
 *   2 = KONTROLLE        : Klasse jedes Bild = 4 (nur Bit 2). Die EBEN-Zeile 3 der
 *                          Maskentabelle @0x800A6DB4 lautet `2,0,0` und fragt AUSSCHLIESSLICH
 *                          Bit 1 ab -> die Trefferzahl MUSS auf 0 fallen. Faellt sie nicht,
 *                          laeuft der Applier an der Messung vorbei.
 *                          ⛔ WARUM NICHT Klasse 0: fuer die Zombie-Familie faengt die
 *                          Port-Ersatzmaske "parts==0 -> 3" (re15_damage.c, Fixture-Fall) den
 *                          Wert ab — gemessen 70 Treffer trotz Klasse 0. Klasse 4 umgeht die
 *                          Ersatzmaske und prueft dieselbe Sache.
 * ⛔ Die Kontrolle gilt NUR fuer Typen, die durch den Applier laufen (Zombie-Familie und
 * seit Runde 34 der Hund 0x20). Kraehe 0x21, Spinne 0x25 und Baby 0x26 gehen weiter ueber
 * den RE1.5-Streifen; fuer sie ist die Klassen-Kontrolle NICHT anwendbar und wird als Zahl
 * ausgewiesen statt als Schranke behauptet. */
typedef struct { int treffer, bilder, luecke_max, luecke, nieder;
                 uint32_t c_gate5, c_class; } scharf_t;
static scharf_t s_scharf[5][3];
static void pass_scharf(int si, int scharf, int budget)
{
    const boxsoll_t *S = &SOLL[si];
    scharf_t *r = &s_scharf[si][scharf]; memset(r, 0, sizeof *r);
    if (!load_room(S->room, S->room_id, S->fire_sub)) {
        printf("  FEHLLAUF: Raum %s fehlt\n", S->room); return; }
    int slot = setup_target(S->type, S->weapon, S->baby);
    if (slot < 0) {
        printf("  FEHLLAUF: kein Gegner 0x%02X in %04X - sagt NICHTS\n",
               S->type, S->room_id); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (S->weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    re15_dmg_mech_reset();
    int war_unten = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        int16_t vor = e->hp;
        if (e->hp < 60) { e->hp = 400; vor = 400; }
        if (!e->active) break;
        if (scharf == 0) e->re2z_parts = 3u;   /* nie gestaucht, s. Blockkopf */
        if (scharf == 2) e->re2z_parts = 4u;   /* nur Bit 2, s. Blockkopf */
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        r->bilder++;
        if (e->state == 2 && !war_unten) { r->nieder++; war_unten = 1; }
        if (e->state != 2) war_unten = 0;
        if (e->hp < vor) { r->treffer++; r->luecke = 0; }
        else { r->luecke++; if (r->luecke > r->luecke_max) r->luecke_max = r->luecke; }
    }
    re15_dmg_mech_counts(&r->c_gate5, &r->c_class, NULL);
    static const char *NAME[3] = { "KLASSE FEST 3", "WIE GEBAUT   ", "KONTROLLE K=4" };
    printf("  [%-11s %s] %4d Bilder | ECHTE Treffer %3d | Niederschlaege %d | "
           "laengste Strecke OHNE Treffer: %4d Bilder (%.1f s) | Tor5 %u / Klasse %u\n",
           S->tag, NAME[scharf], r->bilder, r->treffer, r->nieder,
           r->luecke_max, r->luecke_max / 30.0, r->c_gate5, r->c_class);
}

/* ===== TEIL 2d: DIE DECKENSPINNE — der Zweig, den ROOM2050 nicht zeigt ==================
 * re2s_init verzweigt ueber (+0x10E & 0xF) auf die Spawn-Sprungtabelle @0x80100004
 * (@0x80100430-58): 0 = BODEN (Box -1400/1400 aus Tabelle @0x801063A0), 2/3 = DECKE
 * (`sh zero,152(s2)` @0x8010049C -> Box 0/1400), >=4 = WAND (Tabelle @0x801063E0 -> 0/1400).
 * ROOM2050 liefert einen BODEN-Spawn; die Deckenvariante wird hier erzwungen, indem der
 * Spawn-Deskriptor gesetzt und der INIT-Zustand neu durchlaufen wird — derselbe Code-Pfad,
 * den ein Deckenraum nimmt. Gemessen wird wieder mit ECHTEN Treffern. */
static const char NL[2] = { 10, 0 };
static int s_decke_aus = -1, s_decke_scharf = -1, s_decke_b = 0, s_decke_h = 0;
static int s_nullbox_raeume = 0;    /* AUSGELIEFERTE Spinnen AUSSERHALB des Schacht-Wartens,
                                     * deren Unterkante 0 ist (Decken-/Wandspawn) */
static int s_spinnen_raeume = 0, s_spinnen_n = 0;
static int s_schacht_wartend = 0, s_schacht_box_falsch = 0;

extern void scd_register_current_rdt(const re15_rdt_t *rdt);

/* Spinnenraum aufbauen wie setup_target (RE2-KI, Bank, Waffe 3, 60 Bilder), aber OHNE die
 * uebrigen Aktoren abzuschalten: ROOM2060/20A0 tragen je ZWEI Schachtspinnen, und der Zensus
 * soll jede ausgelieferte Spinne sehen, nicht nur die erste. */
static void raum_aufbau_re2(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    load_re2_bank(0x25);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
}

/* SCHACHT-WARTEN (Runde 38 Spur R, Dossier analysis/befunde_runde38/R_spinnen.md 2.1): der
 * RE1.5-INIT @0x80110b6c setzt fuer grid 0x41 das Zustandswort 0x104 (`ori v0,zero,0x104` /
 * `sw v0,4(a0)` @0x80110d10-14 = Zustand 4, Unterzustand 1); Phase 0 @0x80114f08 wartet, solange
 * grid == 0x41 (`ori v0,zero,0x41` / `beq v1,v0,0x801153b4` @0x80114f18-1c). */
static int schacht_wartet(const re15_actor_t *e)
{
    return e->active && e->type == 0x25 && e->grid_id == 0x41 &&
           e->state == 4 && e->sub_state_1 == 1;
}

/* Welche Box traegt eine AUSGELIEFERTE Spinne je Raum? Das entscheidet, ob das fuenfte Tor
 * fuer 0x25 gefahrlos scharf darf: der Boden-Spawn traegt -1400/1400 (Tabelle @0x801063A0,
 * Fenster [-100, 2900) -> Hgun 1668 kommt durch).
 * ⛔ PRAEZISIERT (Runde 32): die 1668 ist die BINDPOSE von PL00 (1804+692-422-408 = 1666),
 * die re15_player_muzzle_world posiert - NICHT die Zielpose. Die Zielpose misst
 * HOCH 2751 / EBEN 2500 / TIEF 1988 (probe_r30b_muendung Teil 0) und kaeme bei
 * -1400/1400 ebenfalls durch (Fenster [-100,2900)), bei 0/1400 aber weiterhin nicht.
 * Der Satz oben bleibt also richtig, seine Zahl hat nur einen anderen Namen.
 * Decke (@0x8010049C) und Wand
 * (Tabelle @0x801063E0 @0x801004EC-52C) tragen 0/1400 (Fenster [-1500, 1500) -> gesperrt). */
static void pass_spinne_raeume(void)
{
    /* ROOM2090 ergaenzt (Runde 38): Zensus Spur R 1.1 (tools/scd_dump_room.py ueber alle
     * ROOM*.RDT) — Typ 0x25 steht in ROOM2030/2050/2060/2070/2090/20A0 (+ die bytegleichen
     * Elza-Zeilen 20x1); 2090 fehlte hier. */
    static const struct { const char *sub; int id; } RAUM[] = {
        { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE1/ROOM10D0.RDT", 0x10D0 },
        { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
        { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
        { "STAGE2/ROOM2030.RDT", 0x2030 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
        { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE2/ROOM2070.RDT", 0x2070 },
        { "STAGE2/ROOM2090.RDT", 0x2090 },
        { "STAGE2/ROOM20A0.RDT", 0x20A0 }, { "STAGE3/ROOM3010.RDT", 0x3010 },
        { "STAGE3/ROOM3020.RDT", 0x3020 }, { "STAGE4/ROOM4050.RDT", 0x4050 },
        { "STAGE4/ROOM4070.RDT", 0x4070 },
    };
    for (unsigned i = 0; i < sizeof RAUM / sizeof RAUM[0]; i++) {
        if (!load_room(RAUM[i].sub, RAUM[i].id, -1)) continue;
        raum_aufbau_re2();
        int n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x25) continue;
            n++; s_spinnen_n++;
            if (schacht_wartet(e)) {
                /* Der RE2-INIT (re2s_init, einziger Schreiber von +0x98/+0x9E/box_set vor dem
                 * ersten Zustands-Handler: Blockkopie @0x801003BC-400) laeuft fuer die
                 * Schachtspinne erst NACH der Landung (spinnen_r38.c re15_spinnen_r38_lauer_re2).
                 * Bis dahin bleibt der Kasten leer (0/0, box_set 0). */
                s_schacht_wartend++;
                int leer = (e->re2_hit_b98 == 0 && e->re2_hit_h9e == 0 && !e->re2_hit_box_set);
                if (!leer) s_schacht_box_falsch++;
                printf("    %04X Slot %2d: grid 0x%02X Zustand %d/%d Phase %d y %6d | Box %6d/%-5d set %d "
                       "-> WARTET IM SCHACHT (4/1 @0x80110d10-14)%s\n", RAUM[i].id, s, e->grid_id,
                       e->state, e->sub_state_1, e->sub_state_3, e->y, e->re2_hit_b98,
                       (int)e->re2_hit_h9e, e->re2_hit_box_set,
                       leer ? "" : "  <== Kasten NICHT leer");
                continue;
            }
            if (e->re2_hit_b98 == 0) s_nullbox_raeume++;
            printf("    %04X Slot %2d: grid 0x%02X Zustand %d/%d Modus %u y %6d | Box %6d/%-5d set %d %s\n",
                   RAUM[i].id, s, e->grid_id, e->state, e->sub_state_1,
                   (unsigned)e->re2s_mode222, e->y, e->re2_hit_b98, (int)e->re2_hit_h9e,
                   e->re2_hit_box_set,
                   (e->re2_hit_b98 == 0) ? "-> Fenster [-1500,1500) = GESPERRT" : "-> DURCH");
        }
        if (n) s_spinnen_raeume++;
    }
    printf("  %d Raeume mit lebender 0x25, %d Spinnen: %d warten im Schacht, %d ausserhalb mit "
           "Unterkante 0 (gesperrt)\n", s_spinnen_raeume, s_spinnen_n, s_schacht_wartend,
           s_nullbox_raeume);
}

/* ===== TEIL 2e: DIE SCHACHTSPINNEN NACH DEM ORIGINAL-AUSLOESER (Runde 38 Spur R) ===========
 * ROOM2060 und ROOM20A0 liefern je zwei Spinnen mit Sce_em_set grid 0x41 (im Schacht). Sie
 * warten in Zustand 4/1, bis das Raumskript ihren grid auf 0x42 setzt, kriechen heraus, steigen
 * ab und landen auf y 0 (Phasen @0x80114ec8, Tabelle @0x8010027c; Original per GDB gemessen,
 * R_belege/orig_2060_schacht_gift.txt / orig_20a0_schacht.txt). Der Ausloeser wird hier GENAU
 * wie im Original gefahren und wie Spur R ihn misst (test_r38_spinnen.c Teil A): der angezeigte
 * Cut work[0x0A] (DAT_800b0fe4) wird gesetzt, sub01 laeuft je Spielbild (Reseed FUN_8003f038
 * @0x8003f064-84) und ruft ueber Evt_exec das Unterskript mit `Member_set 0x0C 0x42`.
 *
 * ORIGINAL (RE1.5, selbst disassembliert): die wartende Spinne ist KEIN Ziel des Resolvers.
 *   INIT        word0 &= 0x1fffffff        `lui v1,0x1fff`/`and`/`sw v0,0(a0)` @0x80110d38-48
 *               (+0x93 = 0 @0x80110bc0, +0x78 = Kasten A *(0x80118e04) @0x80110d20-2c)
 *   Zustand 4/1 Phase 0 wartet ohne jeden Schreibzugriff (`beq` -> Epilog @0x80114f1c), die
 *               Phasen 0..8 @0x80114f08-0x801153b0 schreiben word0 nie
 *   ACTIVE      erst hier der Band-Stempel: word0 &= 0x1fffffff @0x80110f94-a4, grid < 2 oder
 *               grid == 5 -> `jal 0x80012aa4` (a0 = 0xfa0) @0x80110fd0
 *   Resolver    FUN_80011f50 nimmt nur Kandidaten mit word0 & Spieler & 0xe0000000 != 0
 *               (`lui v1,0xe000`/`and`/`beq v0,zero,0x80012128` @0x800120e4-ec) und ohne
 *               (+0x93 & 3) == 3 (`lui s4,0x300` @0x800120c0, `beq v0,s4` @0x80012100)
 *   Ausstieg    Phase 0 nach dem Ausloeser: +0x78 = Kasten C @0x80114f64-6c, +0x93 |= 3
 *               @0x80114f7c-88 (Latch -> kein Kandidat, solange sie kriecht)
 *   Landung     Phase 7: grid = 5 @0x80115250-54, Kasten A @0x8011522c-34; Phase 8: +0x93 = 0
 *               @0x80115344, Zustandswort 1 @0x8011539c-a0 -> naechstes ACTIVE stempelt das Band.
 *   Zielhilfe   FUN_8003703c (Waffe heben: `jal 0x8003703c` / `ori a0,zero,0x7530` @0x80032fa8-ac)
 *               ueberspringt JEDES Wesen mit grid & 0x60 (`lbu a0,-145(s0)` = +0x09,
 *               `andi v0,a0,0x60` / `bne v0,zero,0x80037174` @0x80037104-10) - grid 0x41/0x42.
 * Im RE2 gibt es keine Schachtspinne (INIT-Sprungtabelle @0x80100004 Eintrag 1 = Epilog
 * @0x8010058C); der Port faehrt den RE1.5-Ausstieg und uebergibt danach an den RE2-INIT
 * (Boden-Zweig @0x80100460), der den Kasten -1400/1400 aus Tabelle @0x801063A0 kopiert
 * (`lw a0,20(a1)`/`sw a0,152(s2)` @0x801003E4/F0, `lw v0,24(a1)`/`sw v0,156(s2)` @0x801003F4/FC).
 *
 * DER RIEGEL (Argument `riegel`) PRUEFT JE SCHACHTSPINNE (RE2-KI wie Teil 1-3):
 *   (a) nach dem Aufbau wartend (Zustand 4/1, grid 0x41) mit LEEREM RE2-Kasten (0/0, box_set 0):
 *       der RE2-INIT laeuft im Port erst nach der Landung (spinnen_r38.c
 *       re15_spinnen_r38_lauer_re2). Das ist ein PORT-Zustand und KEIN Treffer-Tor - das fuenfte
 *       Tor gilt nur mit box_set und nur fuer das Messer (Teil 2c, re15_damage.c);
 *   (b) der Original-Ausloeser setzt grid 0x41 -> 0x42;
 *   (c) Landung + RE2-INIT: Kasten -1400/1400, box_set 1 (Tabelle @0x801063A0+20/+26);
 *   (d) danach treffen echte Schuesse (> 0) - keine Schachtspinne bleibt dauerhaft untreffbar.
 * Bis zur Landung wird dabei nicht geschossen.
 * Die Spinne IM SCHACHT (wartend und beim Herauskriechen) — im Original kein Ziel (Zielhilfe und
 * Resolver, s.o.) — prueft der eigene Riegel `versteckt` (Block "RIEGEL `versteckt`" unten). */
typedef struct {
    const char *sub; int id; int em; int cut; int32_t sx, sy, sz;
    const char *quelle;
} schacht_t;
static const schacht_t SCHACHT[] = {
    { "STAGE2/ROOM2060.RDT", 0x2060, 0, 1,  5800, -3600,   6400,
      "Sce_em_set @Datei 0x0E26 | sub01 Cmp(work[0x0A]==1) @0x110A -> sub02 Member_set 0x0C 0x42 @0x1342" },
    { "STAGE2/ROOM2060.RDT", 0x2060, 1, 5, -6600, -3600,    800,
      "Sce_em_set @Datei 0x0E5C | sub01 Cmp(work[0x0A]==5) @0x1128 -> sub03 Member_set 0x0C 0x42 @0x13E8" },
    { "STAGE2/ROOM20A0.RDT", 0x20A0, 0, 4, 22600, -5400, -29000,
      "Sce_em_set @Datei 0x18CE | sub01 Cmp(work[0x0A]==4) @0x1A9C -> sub03 Member_set 0x0C 0x42 @0x1CB4" },
    { "STAGE2/ROOM20A0.RDT", 0x20A0, 1, 5, 19400, -3600, -13400,
      "Sce_em_set @Datei 0x1904 | sub01 Cmp(work[0x0A]==5) @0x1ABA -> sub04 Member_set 0x0C 0x42 @0x1D5A" },
};
#define SCHACHT_N ((int)(sizeof SCHACHT / sizeof SCHACHT[0]))

typedef struct {
    int ok, wartet, box_vor_b, box_vor_h, box_vor_set;
    int ausgeloest, gelandet, bilder_kriechen;
    int box_nach_b, box_nach_h, box_nach_set, zustand_nach;
    int32_t land_x, land_y, land_z;
    int bilder_nach, treffer_nach;
} schacht_mess_t;
static schacht_mess_t s_schacht[SCHACHT_N];

extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);

/* Spieler auf den BODEN unter dem Schacht stellen (y 0 = die Landehoehe: +0x1ba := 0
 * `sh zero,442` @0x80115044, y := +0x1ba @0x80115264-6c), `back` vor der Spinne, Blick auf sie. */
static void track_boden(int slot, int32_t back)
{
    track(slot, back);
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = 0;
}

/* Rastet die Zielhilfe des Ports (re15_player_aim_target = FUN_8003703c mit dem Radius des
 * Waffe-Hebens 0x7530 @0x80032fac) auf eine Spinne ein, die das Original ueberspringt
 * (grid & 0x60 != 0: `andi v0,a0,0x60` / `bne` @0x8003710c-10 - grid 0x41 wartend, 0x42
 * kriechend; erst Phase 7 setzt grid 5 @0x80115250-54)? */
static int ziel_auf_schacht(void)
{
    int32_t tx = 0, tz = 0;
    if (!re15_player_aim_target(0x7530, &tx, &tz)) return 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x25 && e->x == tx && e->z == tz && (e->grid_id & 0x60u)) return 1;
    }
    return 0;
}

static re15_actor_t *schacht_spinne(const schacht_t *S)
{
    /* Work_set kind 2 idx -> Aktor-Slot idx + 1 (scd_work_bind); die Sce_em_set-Lage prueft,
     * dass es wirklich die Spinne dieses Satzes ist. */
    re15_actor_t *e = &g_actors[S->em + 1];
    if (!e->active || e->type != 0x25 || e->x != S->sx || e->z != S->sz) return NULL;
    return e;
}

/* (a)-(d): Aufbau, Original-Ausloeser, Landung mit Kasten, Treffer danach. */
static void pass_schacht(int i, int budget_aus, int budget_nach)
{
    const schacht_t *S = &SCHACHT[i];
    schacht_mess_t *m = &s_schacht[i];
    memset(m, 0, sizeof *m);
    m->ausgeloest = m->gelandet = -1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!load_room(S->sub, S->id, -1)) { printf("  FEHLLAUF: %s fehlt\n", S->sub); return; }
    raum_aufbau_re2();
    re15_actor_t *e = schacht_spinne(S);
    if (!e) { printf("  FEHLLAUF: keine Spinne %d in %04X\n", S->em, S->id); return; }
    const int slot = S->em + 1;
    m->wartet = schacht_wartet(e);                                   /* (a) */
    m->box_vor_b = e->re2_hit_b98; m->box_vor_h = (int)e->re2_hit_h9e;
    m->box_vor_set = e->re2_hit_box_set;

    /* (b)/(c): sub01-Reseed je Bild und Evt_exec brauchen das registrierte RDT (scd_vm.c
     * s_current_rdt); danach wieder abmelden, damit die uebrigen Teile unveraendert laufen.
     * Bis zur Landung wird NICHT geschossen: im Original ist die Spinne bis dahin kein Ziel
     * (Blockkopf); das prueft der Riegel `versteckt`, dieser Lauf misst den Ausstieg. */
    scd_register_current_rdt(&s_rdt);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < budget_aus; f++) {
        pl->hp = 100;
        if (!e->active) break;
        if (m->ausgeloest < 0) g_scd.work_vars[0x0A] = (int16_t)S->cut;   /* angezeigter Cut */
        scd_vm_tick();
        track_boden(slot, 2000);
        frame(0, 0);
        if (m->ausgeloest < 0 && e->grid_id == 0x42) m->ausgeloest = f;   /* Member_set 0x0C 0x42 */
        if (m->ausgeloest >= 0 && m->gelandet < 0) {
            if (e->re2_hit_box_set) {                    /* RE2-INIT nach der Landung gelaufen */
                m->gelandet = f; m->land_x = e->x; m->land_y = e->y; m->land_z = e->z;
                break;
            }
            m->bilder_kriechen++;
        }
    }
    scd_register_current_rdt(NULL);
    m->box_nach_b = e->re2_hit_b98; m->box_nach_h = (int)e->re2_hit_h9e;   /* (c) */
    m->box_nach_set = e->re2_hit_box_set; m->zustand_nach = e->state;

    /* (d) NACH DER LANDUNG: echte Schuesse wie Teil 3 (gleiche Ebene). */
    if (m->gelandet >= 0) {
        aim_up(slot, 2000);
        for (int f = 0; f < budget_nach; f++) {
            pl->hp = 100; e->hp = 30000;
            if (!e->active) break;
            track(slot, 2000);
            frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
                  (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
            m->bilder_nach++;
            if (e->hp < 30000) m->treffer_nach++;
        }
    }
    m->ok = 1;
    printf("  [%04X Spinne %d, Cut %d] %s\n", S->id, S->em, S->cut, S->quelle);
    printf("    (a) wartend %d | RE2-Kasten %d/%d set %d\n", m->wartet, m->box_vor_b, m->box_vor_h,
           m->box_vor_set);
    printf("    (b) grid 0x41 -> 0x42 in Bild %d | Herauskriechen %d Bilder (ohne Beschuss)\n",
           m->ausgeloest, m->bilder_kriechen);
    printf("    (c) Landung + RE2-INIT %d Bilder nach dem Ausloeser auf (%d,%d,%d): Zustand %d, Kasten %d/%d "
           "set %d | (d) Treffer %d in %d Bildern\n",
           (m->gelandet >= 0 && m->ausgeloest >= 0) ? m->gelandet - m->ausgeloest : -1,
           m->land_x, m->land_y, m->land_z, m->zustand_nach, m->box_nach_b, m->box_nach_h,
           m->box_nach_set, m->treffer_nach, m->bilder_nach);
}

/* ===== RIEGEL `versteckt` (Runde 38 Integration): DIE SCHACHTSPINNE IM SCHACHT, RE2- UND RE1.5-KI ==========
 * Nutzer-Befund "Spinnen fliegen in der Luft" — gemessen in der echten exe (Tuerweg 2030 -> 2060 / 2031 -> 2061,
 * Leon und Elza): die Zielhilfe dreht den Spieler auf die versteckte Spinne (Blick 0x800 -> 0xA63), und unter der
 * RE2-KI trifft der Schuss die herauskriechende Spinne (+0x93 = 02 statt 03), die danach mit grid 0x42 auf y -3600
 * im RE2-Bodenmodus laeuft (Dossier R_spinnen.md "Integration: versteckte Schachtspinne").
 * ORIGINAL (selbst disassembliert und in DuckStation-GDB gemessen, R_belege/orig_2060_zielhilfe_messer.txt):
 *   Zielhilfe FUN_8003703c: `lbu a0,-145(s0)` @0x80037104 (+0x09), `andi v0,a0,0x60` @0x8003710c,
 *     `bne v0,zero,0x80037174` @0x80037110 -> jedes Wesen mit grid & 0x60 wird uebersprungen. Gemessen: Messer heben
 *     neben der kriechenden Spinne (grid 0x42, 1230 entfernt) -> 0x800acaf3 = 0, Blick bleibt; nach der Landung
 *     (grid 5) -> 1, Blick dreht 0xC0 je Bild.
 *   Resolver FUN_80011f50: Kandidat nur mit word0 & Spieler & 0xe0000000 (@0x800120d0-ec) und ohne (+0x93 & 3) == 3
 *     (`lui s4,0x300` @0x800120c0, `beq v0,s4` @0x80012100). Band der Spinne: INIT loescht (@0x80110d38-48),
 *     Zustand 4 schreibt word0 nie, ACTIVE stempelt (@0x80110f94-fd4). Gemessen: word0 0x00000801 im Warten, Kriechen,
 *     nach Phase 7 und Phase 8; 0x60000801 ab dem ersten ACTIVE-Bild. Latch: Phase 0 |= 3 (@0x80114f7c-88), 03 im
 *     ganzen Ausstieg, Phase 8 = 0 (@0x80115344).
 * Je Schachtspinne (4 Saetze grid 0x41) und KI misst der Riegel:
 *   W  wartend: Zielhilfe (re15_player_aim_target, Radius 0x7530 @0x80032fac) waehlt sie nicht; ein direkter
 *      Resolver-Aufruf (re15_player_weapon_fire, ohne Pad/FSM) trifft sie nicht; Salven eben/hoch vom Boden
 *      unter dem Schacht treffen nicht; danach wartet sie unveraendert (Zustand 4/1, grid 0x41, Spawn-Hoehe).
 *   K  Herauskriechen (Original-Ausloeser work[0x0A] -> sub01 -> Member_set 0x0C 0x42): Latch (+0x93&3)==3 zu
 *      Beginn JEDES Ausstiegsbildes, Zielhilfe 0, direkter Resolver 0, echte Schuesse treffen nicht, und die Spinne
 *      landet (verlaesst Zustand 4) — sie wird nicht aus dem Schacht geschossen.
 *   L  Landung (ohne Beschuss bis Phase 8): direkt nach dem Phase-8-Takt ist das Band noch leer -> Resolver 0
 *      (Zielhilfe darf sie schon waehlen: grid 5 bzw. 0); nach dem ersten ACTIVE-/RE2-INIT-Takt -> Resolver trifft,
 *      Zielhilfe waehlt sie; danach echte Treffer > 0 und kein Bild ueber dem Boden (y < -100, ausser RE2-Sprung).
 *   T  Tuerweg (ROOM2060 Leon / ROOM2061 Elza, Ziel (7600,0,3950) Blick 0x800, Door_aot_set ROOM2030 @Datei 0x1B4C):
 *      nur R1 + Feuer -> keine Zielhilfe, kein Treffer, Blick bleibt 0x800, Spinne wartet.
 * Dazu der ZENSUS grid & 0x60 (Block Z60 unten): jeder ausgelieferte Gegner-Typ mit einem solchen Spawn wird
 * versteckt nicht gewaehlt und nach seiner Original-Freigabe wieder gewaehlt. */
static const re15_ai_flavor_t VFL[2] = { RE15_AI_FLAVOR_RE2, RE15_AI_FLAVOR_RE15 };
static const char *const VFN[2] = { "RE2-KI", "RE1.5-KI" };

/* RE1.5-Bank (CDEMD0.EMS, Muster test_r38_spinnen.c bank_re15) — eigener Puffer je Typ (eb->buf, re15_enemy_reset
 * gibt ihn frei), damit mehrere Typen gleichzeitig geladen sein duerfen. */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static re15_enemy_bank_t *load_re15_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return NULL;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0 || len == 0) return NULL;
    uint8_t *blob = (uint8_t *)malloc(len);
    if (!blob) return NULL;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { free(blob); return NULL; }
    memcpy(blob, s_ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        free(blob); eb->type = 0; return NULL; }
    eb->ok = 1; eb->buf = blob;
    return eb;
}
/* Bank je KI wie im Spiel: die RE2-Gehirne (Zombie-Familie, Hund, Kraehe, Spinnen) auf der RE2-Bank, alle anderen
 * Typen (0x27/0x29/0x2B/0x30 haben kein RE2-Gehirn) auf ihrer RE1.5-Bank. */
static void bank_fuer(re15_ai_flavor_t fl, uint8_t type)
{
    int re2 = (fl == RE15_AI_FLAVOR_RE2) &&
              (type == 0x10 || type == 0x11 || type == 0x12 || type == 0x13 || type == 0x16 || type == 0x18 ||
               type == 0x20 || type == 0x21 || type == 0x25 || type == 0x26);
    if (re2) (void)load_re2_bank(type); else (void)load_re15_bank(type);
}
static void raum_aufbau_fl(re15_ai_flavor_t fl, uint8_t type)
{
    re15_ai_flavor_set(fl);
    bringup();
    bank_fuer(fl, type);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
}

/* Waehlt die Zielhilfe des Ports (re15_player_aim_target = FUN_8003703c, Radius 0x7530) GENAU diesen Aktor? */
static int ziel_ist(const re15_actor_t *e)
{
    int32_t tx = 0, tz = 0;
    if (!re15_player_aim_target(0x7530, &tx, &tz)) return 0;
    return e->active && e->x == tx && e->z == tz;
}

/* Direkter Resolver-Aufruf (FUN_80011f50-Zwilling re15_player_weapon_fire, Waffe 3) ohne Pad/FSM: trifft er GENAU
 * diesen Platz? Ziel und Spieler werden danach zurueckgesetzt — der Aufruf ist die Messung, nicht Spiel. */
static int direkter_treffer(int slot)
{
    re15_actor_t *e = &g_actors[slot], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t se = *e, sp = *pl;
    int r = re15_player_weapon_fire(3);
    *e = se; *pl = sp;
    return r == slot + 1;
}

typedef struct {
    int ok, wartet;
    int w_ziel, w_res, w_eben, w_hoch, w_bilder, w_zustand, w_sub, w_grid; int32_t w_y, w_y0;
    int k_ok, k_bilder, k_bilder42, k_latch3, k_ziel, k_res, k_schuesse, k_treffer, k_gelandet;
    int k_zustand, k_sub, k_grid; int32_t k_y;
    int l_ok, l_bild, l_zustand_p8, l_ziel_p8, l_res_p8, l_ziel_a1, l_res_a1, l_treffer, l_bilder, l_schwebt;
    int l_band_p8, l_band_a1;     /* aim_band (Port-word0-Band) nach Phase 8 / nach dem ersten ACTIVE */
    int32_t l_y_min, l_y_max;
} versteckt_mess_t;
static versteckt_mess_t s_versteckt[2][SCHACHT_N];

static void pass_versteckt(int fi, int i, int budget)
{
    const re15_ai_flavor_t fl = VFL[fi];
    const schacht_t *S = &SCHACHT[i];
    versteckt_mess_t *v = &s_versteckt[fi][i];
    memset(v, 0, sizeof *v);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int slot = S->em + 1;

    /* ---- W: wartend ---- */
    if (!load_room(S->sub, S->id, -1)) return;
    raum_aufbau_fl(fl, 0x25);
    re15_actor_t *e = schacht_spinne(S);
    if (!e) return;
    v->wartet = schacht_wartet(e);
    v->w_y0 = e->y;
    track_boden(slot, 2000);
    v->w_ziel = ziel_ist(e);
    v->w_res = direkter_treffer(slot);
    for (int hoch = 0; hoch < 2; hoch++) {
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            if (!e->active) break;
            int16_t vor = e->hp;
            track_boden(slot, 2000);
            frame((uint16_t)(RE15_PAD_BIT_R1 | (hoch ? RE15_PAD_BIT_UP : 0u) | RE15_PAD_BIT_SQUARE),
                  (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
            v->w_bilder++;
            if (e->hp < vor || e->state == 2) { if (hoch) v->w_hoch++; else v->w_eben++; }
        }
    }
    v->w_zustand = e->state; v->w_sub = e->sub_state_1; v->w_grid = e->grid_id; v->w_y = e->y;
    v->ok = 1;

    /* ---- K: Herauskriechen (Original-Ausloeser), Feuer in jedem Ausstiegsbild ---- */
    if (!load_room(S->sub, S->id, -1)) return;
    raum_aufbau_fl(fl, 0x25);
    e = schacht_spinne(S);
    if (!e || !schacht_wartet(e)) return;
    scd_register_current_rdt(&s_rdt);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    int ausgeloest = 0;
    for (int f = 0; f < 600; f++) {
        pl->hp = 100;
        if (!e->active) break;
        if (!ausgeloest) g_scd.work_vars[0x0A] = (int16_t)S->cut;
        scd_vm_tick();
        if (e->grid_id == 0x42) ausgeloest = 1;
        const int kriecht = (e->state == 4 && e->sub_state_1 == 1 && e->sub_state_3 >= 1);
        track_boden(slot, 2000);
        if (kriecht) {
            v->k_bilder++;
            if ((e->hit_react & 3u) == 3u) v->k_latch3++;
            /* Zielhilfe nur, solange grid die Marke traegt (0x42): nach Phase 7 steht grid 5 (@0x80115250-54)
             * und die Spinne ist im Original fuer FUN_8003703c wieder waehlbar — das Band ist dann noch leer
             * (Phase 8 / erstes ACTIVE, Teil L). */
            if (e->grid_id & 0x60u) { v->k_bilder42++; if (ziel_ist(e)) v->k_ziel++; }
            if (direkter_treffer(slot)) v->k_res++;
        }
        int16_t vor = e->hp;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (kriecht && (f % 6) == 0) v->k_schuesse++;
        if (kriecht && (e->hp < vor || e->state == 2)) v->k_treffer++;
        if (ausgeloest && e->state != 4) {              /* Zustand 4 verlassen: Phase 8 (Landung) oder Treffer */
            v->k_gelandet = (e->state == 1 || e->state == 0) && e->y >= -100 && e->grid_id != 0x42;
            break;
        }
    }
    scd_register_current_rdt(NULL);
    v->k_zustand = e->state; v->k_sub = e->sub_state_1; v->k_grid = e->grid_id; v->k_y = e->y;
    v->k_ok = ausgeloest;

    /* ---- L: Landung ohne Beschuss, dann das Band-Tor an der Grenze Phase 8 / erstes ACTIVE ---- */
    if (!load_room(S->sub, S->id, -1)) return;
    raum_aufbau_fl(fl, 0x25);
    e = schacht_spinne(S);
    if (!e || !schacht_wartet(e)) return;
    scd_register_current_rdt(&s_rdt);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    ausgeloest = 0;
    int f8 = -1;
    for (int f = 0; f < 600; f++) {
        pl->hp = 100;
        if (!ausgeloest) g_scd.work_vars[0x0A] = (int16_t)S->cut;
        scd_vm_tick();
        if (e->grid_id == 0x42) ausgeloest = 1;
        track_boden(slot, 2000);
        frame(0, 0);
        if (ausgeloest && e->state != 4) { f8 = f; break; }   /* dieser Takt war Phase 8 (@0x80115338) */
    }
    scd_register_current_rdt(NULL);
    if (f8 < 0) return;
    v->l_bild = f8; v->l_zustand_p8 = e->state;
    track_boden(slot, 2000);
    v->l_ziel_p8 = ziel_ist(e);
    v->l_res_p8  = direkter_treffer(slot);                  /* Original: word0 0x00000801 -> kein Kandidat */
    v->l_band_p8 = e->aim_band;
    frame(0, 0);                                            /* erster ACTIVE- (RE1.5) bzw. RE2-INIT-Takt */
    track_boden(slot, 2000);
    v->l_ziel_a1 = ziel_ist(e);
    v->l_res_a1  = direkter_treffer(slot);                  /* Original: word0 0x60000801 -> Kandidat */
    v->l_band_a1 = e->aim_band;
    v->l_y_min = 0x7fffffff; v->l_y_max = -0x7fffffff;
    aim_up(slot, 2000);
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        v->l_bilder++;
        if (e->hp < 30000) v->l_treffer++;
        if (e->y < v->l_y_min) v->l_y_min = e->y;
        if (e->y > v->l_y_max) v->l_y_max = e->y;
        if (e->y < -100 && !(fl == RE15_AI_FLAVOR_RE2 && e->re2s_mode222 == 1)) v->l_schwebt++;
    }
    v->l_ok = 1;
}

/* T: der Spielerweg Tuer 2030 -> 2060 bzw. 2031 -> 2061 (Door_aot_set ROOM2030 Platz 3 @Datei 0x1B4C: Ziel
 * (7600,0,3950), Blick 0x800), dort nur Waffe heben + feuern — ohne Teleport, ohne Nachfuehren. */
typedef struct { int ok, ziel, treffer, wartet; int16_t blick_vor, blick_nach; } tuer_mess_t;
static tuer_mess_t s_tuer[2][2];
static void pass_versteckt_tuer(int fi, int elza, int budget)
{
    tuer_mess_t *t = &s_tuer[fi][elza];
    memset(t, 0, sizeof *t);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!load_room(elza ? "STAGE2/ROOM2061.RDT" : "STAGE2/ROOM2060.RDT", elza ? 0x2061 : 0x2060, -1)) return;
    raum_aufbau_fl(VFL[fi], 0x25);
    re15_actor_t *e = schacht_spinne(&SCHACHT[0]);
    if (!e || !schacht_wartet(e)) return;
    pl->x = 7600; pl->y = 0; pl->z = 3950; pl->rot_y = 0x800;
    t->blick_vor = pl->rot_y;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    t->ziel = ziel_ist(e);
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        int16_t vor = e->hp;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->hp < vor || e->state == 2) t->treffer++;
    }
    t->blick_nach = pl->rot_y;
    t->wartet = schacht_wartet(e);
    t->ok = 1;
}

/* ===== ZENSUS grid & 0x60 — jeder ausgelieferte Typ mit einem versteckten Spawn ==================================
 * Quelle: Sce_em_set (0x44, pc[3] = +0x09) und Member_set 0x0C ueber alle 240 RDTs (info/Re1.5/PSX; Werkzeug
 * R_belege/zensus_grid60.py, Ausgabe zensus_grid60.txt). Je Fall: versteckt nicht gewaehlt; nach der Original-
 * Freigabe (Member_set-Wert aus dem RDT, hier gesetzt wie actor_common.c case 12 `sb`) loescht das Gehirn die Bits an
 * der Original-Stelle bzw. traegt der Freigabewert sie schon nicht mehr -> wieder gewaehlt. Ohne Freigabe im RDT
 * bleibt das Wesen versteckt (Original ebenso). NPC-Typen (0x40-0x4d, grid 0x40) stehen nicht hier: sie tragen
 * HP -1 (@0x8011c744) und waren schon vorher kein Ziel. */
typedef struct {
    const char *tag, *sub; int id, fire_sub, em; uint8_t type, grid0; int freigabe, hirn, fbank, fbit; const char *quelle;
} zensus60_t;
static const zensus60_t Z60[] = {
  { "Hund Fenster", "STAGE1/ROOM11D0.RDT", 0x11D0, -1, 0, 0x20, 0x41, 0x42, 1, -1, 0,
    "Sce_em_set main00 @Datei 0x12BE | sub02 Member_set 0x42 @0x172C | grid := 0 `sb zero,9` @0x80111718" },
  { "Hund Zwinger", "STAGE1/ROOM1190.RDT", 0x1190, 13, 0, 0x20, 0x40, 0x43, 1, -1, 0,
    "Sce_em_set sub13 @Datei 0x2900 | sub10 Member_set 0x43 @0x27E6 | grid := 0 `sb zero,9` @0x8011161c" },
  { "Affe Ereignis", "STAGE4/ROOM4040.RDT", 0x4040, -1, 0, 0x29, 0x61, 0x41, 1, -1, 0,
    "Sce_em_set main00 @Datei 0x0EB0 | sub04 Member_set 0x41 @0x11D6 | grid &= 0xbf @0x80115978-7c" },
  { "Spiderman", "STAGE4/ROOM4001.RDT", 0x4001, -1, 2, 0x2B, 0x41, 0x42, 1, 4, 20,
    "Ck(4,20)=1 @0x120C, Sce_em_set sub00 @Datei 0x122C | sub05 Member_set 0x42 @0x15BA | grid := 0 `sb zero,9` @0x80115564" },
  { "Zombie 1200", "STAGE1/ROOM1200.RDT", 0x1200, -1, 2, 0x10, 0xA1, 0x81, 0, -1, 0,
    "Sce_em_set main00 @Datei 0x087E | sub02 Member_set 0x81 @0x09A4" },
  { "Gorilla 11C0", "STAGE1/ROOM11C0.RDT", 0x11C0, -1, 1, 0x27, 0x30, 0x10, 0, -1, 0,
    "Ck(4,64)=0 @0x176C, Sce_em_set sub00 @Datei 0x1784 | sub02 Member_set 0x10 @0x1A3A" },
  { "Birkin 3070", "STAGE3/ROOM3070.RDT", 0x3070, -1, 0, 0x30, 0x33, 0x13, 0, -1, 0,
    "Sce_em_set sub00 @Datei 0x33CE | sub02 Member_set 0x13 @0x358E" },
  { "Zombie 5020", "STAGE5/ROOM5020.RDT", 0x5020, -1, 0, 0x18, 0xAE, -1, 0, -1, 0,
    "Sce_em_set sub00 @Datei 0x12D6 | kein Member_set 0x0C im Raum" },
  { "Kraehe 1171", "STAGE1/ROOM1171.RDT", 0x1171, 7, 1, 0x21, 0x40, -1, 0, 3, 194,
    "Ck(3,194)=1 @0x16C4, Sce_em_set sub07 @Datei 0x16DC | kein Member_set 0x0C; Kraehen-Code STAGE1 0x80111a00-0x80116000 ohne grid-Store" },
};
#define Z60_N ((int)(sizeof Z60 / sizeof Z60[0]))
typedef struct { int ok, gespawnt, grid_spawn, ziel_versteckt, frei_bild, grid_frei, ziel_frei, nie_gewaehlt, bilder; } z60_mess_t;
static z60_mess_t s_z60[2][Z60_N];

/* Spieler 1500 vor das Wesen auf dessen Hoehe (wie track), dann die Zielhilfe fragen — nur dieses Wesen aktiv
 * (die uebrigen fuer die reine Abfrage kurz aus, danach zurueck), damit ein naeherer Nachbar das Urteil nicht stellt. */
static int ziel_nah(int slot)
{
    uint8_t aktiv[RE15_ACTOR_MAX];
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        aktiv[s] = g_actors[s].active;
        if (s != slot) g_actors[s].active = 0;
    }
    track(slot, 1500);
    int r = ziel_ist(&g_actors[slot]);
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = aktiv[s];
    return r;
}

static void pass_zensus60(int fi, int k)
{
    const zensus60_t *Z = &Z60[k];
    z60_mess_t *m = &s_z60[fi][k];
    memset(m, 0, sizeof *m);
    m->frei_bild = -1;
    if (!load_room(Z->sub, Z->id, Z->fire_sub)) return;
    re15_ai_flavor_set(VFL[fi]);
    s_vorflag_bank = Z->fbank; s_vorflag_bit = Z->fbit;   /* Zweig des versteckten Spawns (Ifel_ck) */
    bringup();
    s_vorflag_bank = -1;
    bank_fuer(VFL[fi], Z->type);
    for (int f = 0; f < 30; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    const int slot = Z->em + 1;
    re15_actor_t *e = &g_actors[slot];
    m->ok = 1;
    if (!e->active || e->type != Z->type) return;
    m->gespawnt = 1;
    m->grid_spawn = e->grid_id;
    m->ziel_versteckt = ziel_nah(slot);
    if (Z->freigabe < 0) {                               /* keine Freigabe im RDT: 300 Bilder versteckt? */
        m->nie_gewaehlt = 1;
        for (int f = 0; f < 300; f++) {
            g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
            frame(0, 0);
            if (!e->active) break;
            m->bilder++;
            if (!(e->grid_id & 0x60u) || ziel_nah(slot)) m->nie_gewaehlt = 0;
        }
        m->grid_frei = e->grid_id;
        return;
    }
    e->grid_id = (uint8_t)Z->freigabe;                  /* Member_set 0x0C = actor_common.c case 12 `sb` +0x09 */
    for (int f = 0; f < 600; f++) {
        if (!(e->grid_id & 0x60u)) { m->frei_bild = f; break; }
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        frame(0, 0);
        if (!e->active) break;
    }
    /* das Gehirn noch ein paar Bilder laufen lassen (INIT nach Takt-Sperre 0x20, ACTIVE), dann fragen */
    for (int f = 0; f < 10 && e->active; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    m->grid_frei = e->grid_id;
    m->ziel_frei = e->active && ziel_nah(slot);
}

/* ===== B: das Band der STEGSPINNE (ROOM2050, grid 0x00 floor 3, y -5400) — die Wirkung des Band-Tors ausserhalb
 * des Schachts. RE1.5-KI: das Band ist der ACTIVE-Stempel FUN_80012aa4(0xfa0) (@0x80110fd0) mit
 *   s0 = (SpielerY - SpinneY) / 1800 (Abschneiden, @0x80012abc-b1c), LEVEL bei (unsigned)s0 < 2 (@0x80012b20-44),
 *   UP (s0 > 0) bzw. DOWN im Ring 1000*|s0| < Abstand < 0xfa0 + 1000*|s0| (@0x80012b64-bb8);
 * leer -> kein Kandidat (@0x800120e4-ec). RE2-KI: kein Hoehenband (Port-Abbildung wie vorher) -> immer Kandidat.
 * Drei Spielerlagen: auf dem Steg 2000 entfernt, unten (y 0) 2000 bzw. 5000 waagerecht entfernt. */
typedef struct { int ok, band, soll, treffer; int32_t dist, px, py, pz; } stegband_t;
static stegband_t s_steg[2][3];
static const struct { int unten; int32_t dx, dz; } STEG_LAGE[3] = { { 0, -2000, 0 }, { 1, -2000, 0 }, { 1, 0, 5000 } };
static int aa4_soll(const re15_actor_t *pl, const re15_actor_t *e, int32_t *dist_out)
{
    int32_t s0 = (pl->y - e->y) / 1800;                                   /* @0x80012ad8 / @0x80012b0c-1c */
    int64_t dx = pl->x - e->x, dz = pl->z - e->z;
    int32_t dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));   /* SquareRoot0 @0x80012b58 */
    int32_t ring = (s0 < 0 ? -s0 : s0) * 1000;                            /* @0x80012b64-84 */
    int b = 0;
    if ((uint32_t)s0 < 2u) b |= 2;                                        /* LEVEL @0x80012b2c-44 */
    if ((uint32_t)dist < (uint32_t)(0xfa0 + ring) && ring < dist) b |= (s0 > 0) ? 4 : 1;   /* @0x80012b88-bb8 */
    if (dist_out) *dist_out = dist;
    return b;
}
static void pass_stegband(int fi)
{
    for (int k = 0; k < 3; k++) {
        stegband_t *m = &s_steg[fi][k];
        memset(m, 0, sizeof *m);
        if (!load_room("STAGE2/ROOM2050.RDT", 0x2050, -1)) return;
        raum_aufbau_fl(VFL[fi], 0x25);
        const int slot = 1;
        re15_actor_t *e = &g_actors[slot], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!e->active || e->type != 0x25) return;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        pl->x = e->x + STEG_LAGE[k].dx; pl->z = e->z + STEG_LAGE[k].dz; pl->y = STEG_LAGE[k].unten ? 0 : e->y;
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
        pl->hp = 100;
        frame(0, 0);                                    /* ACTIVE-Takt der Spinne stempelt mit dieser Spielerlage */
        m->band = e->aim_band;
        m->soll = aa4_soll(pl, e, &m->dist);
        m->px = pl->x; m->py = pl->y; m->pz = pl->z;
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
        m->treffer = direkter_treffer(slot);
        m->ok = 1;
    }
}

static void drucke_versteckt(void)
{
    for (int fi = 0; fi < 2; fi++) {
        printf("  --- %s ---\n", VFN[fi]);
        for (int i = 0; i < SCHACHT_N; i++) {
            const versteckt_mess_t *v = &s_versteckt[fi][i];
            printf("  [%04X Spinne %d] W: wartend %d | Zielhilfe %d | Resolver direkt %d | Salven %d Bilder: eben %d, "
                   "hoch %d | danach %d/%d grid 0x%02X y %d (Spawn %d)\n", SCHACHT[i].id, SCHACHT[i].em, v->wartet,
                   v->w_ziel, v->w_res, v->w_bilder, v->w_eben, v->w_hoch, v->w_zustand, v->w_sub, v->w_grid, v->w_y,
                   v->w_y0);
            printf("                   K: ausgeloest %d | %d Ausstiegsbilder (davon %d mit grid 0x42): Latch==3 in %d, "
                   "Zielhilfe %d, Resolver direkt %d | %d Schuesse, Treffer %d | danach %d/%d grid 0x%02X y %d, "
                   "gelandet %d\n", v->k_ok, v->k_bilder, v->k_bilder42, v->k_latch3, v->k_ziel, v->k_res,
                   v->k_schuesse, v->k_treffer, v->k_zustand, v->k_sub, v->k_grid, v->k_y, v->k_gelandet);
            printf("                   L: Phase 8 in Bild %d (Zustand %d): Zielhilfe %d, Resolver %d | erstes ACTIVE: "
                   "Zielhilfe %d, Resolver %d | Band %d -> %d | Salve %d Bilder: Treffer %d, y %d..%d, ueber dem Boden "
                   "%d\n", v->l_bild, v->l_zustand_p8, v->l_ziel_p8, v->l_res_p8, v->l_ziel_a1, v->l_res_a1,
                   v->l_band_p8, v->l_band_a1, v->l_bilder, v->l_treffer, v->l_y_min, v->l_y_max, v->l_schwebt);
        }
        for (int el = 0; el < 2; el++) {
            const tuer_mess_t *t = &s_tuer[fi][el];
            printf("  Tuerweg %s (7600,0,3950) Blick 0x%03X: Zielhilfe %d | Blick danach 0x%03X | Treffer %d | "
                   "Spinne 0 wartet %d\n", el ? "2031 -> 2061 (Elza)" : "2030 -> 2060 (Leon)",
                   (unsigned)(uint16_t)t->blick_vor, t->ziel, (unsigned)(uint16_t)t->blick_nach, t->treffer, t->wartet);
        }
        for (int k = 0; k < Z60_N; k++) {
            const z60_mess_t *m = &s_z60[fi][k];
            printf("  Zensus %-13s 0x%02X: gespawnt %d grid 0x%02X | versteckt gewaehlt %d | ", Z60[k].tag, Z60[k].type,
                   m->gespawnt, m->grid_spawn, m->ziel_versteckt);
            if (Z60[k].freigabe >= 0)
                printf("Freigabe 0x%02X -> Bits weg nach %d Bildern, grid 0x%02X, gewaehlt %d\n", Z60[k].freigabe,
                       m->frei_bild, m->grid_frei, m->ziel_frei);
            else
                printf("ohne Freigabe %d Bilder: nie gewaehlt %d, grid 0x%02X\n", m->bilder, m->nie_gewaehlt,
                       m->grid_frei);
        }
        for (int k = 0; k < 3; k++) {
            const stegband_t *m = &s_steg[fi][k];
            printf("  Stegspinne 2050 (y -5400), Spieler %s (%d,%d,%d), Abstand %d: Band %d (FUN_80012aa4(0xfa0)-Soll %d) | "
                   "Resolver direkt %d\n", STEG_LAGE[k].unten ? "unten" : "auf dem Steg", m->px, m->py, m->pz, m->dist,
                   m->band, m->soll, m->treffer);
        }
    }
}

/* Urteil gegen das Original; Rueckgabe = Zahl der Abweichungen. */
static int urteil_versteckt(void)
{
    int abw = 0;
    for (int fi = 0; fi < 2; fi++) {
        const char *K = VFN[fi];
        for (int i = 0; i < SCHACHT_N; i++) {
            const versteckt_mess_t *v = &s_versteckt[fi][i];
            const int id = SCHACHT[i].id, em = SCHACHT[i].em;
            if (!v->ok || !v->wartet) {
                printf("  FEHLER %s %04X/%d: nicht wartend gemessen - sagt NICHTS\n", K, id, em); abw++; continue; }
            if (v->w_ziel) { abw++;
                printf("  ABWEICHUNG %s %04X/%d W: Zielhilfe waehlt die WARTENDE Spinne (Original: `andi v0,a0,0x60` / "
                       "`bne` @0x8003710c-10)\n", K, id, em); }
            if (v->w_res) { abw++;
                printf("  ABWEICHUNG %s %04X/%d W: der Resolver nimmt die WARTENDE Spinne (Original: Band 0 "
                       "@0x80110d38-48 -> `beq v0,zero` @0x800120ec)\n", K, id, em); }
            if (v->w_eben + v->w_hoch > 0) { abw++;
                printf("  ABWEICHUNG %s %04X/%d W: Salven treffen die wartende Spinne (eben %d, hoch %d)\n", K, id, em,
                       v->w_eben, v->w_hoch); }
            if (v->w_zustand != 4 || v->w_sub != 1 || v->w_grid != 0x41 || v->w_y != v->w_y0) { abw++;
                printf("  ABWEICHUNG %s %04X/%d W: nach den Salven %d/%d grid 0x%02X y %d statt wartend 4/1 0x41 y %d\n",
                       K, id, em, v->w_zustand, v->w_sub, v->w_grid, v->w_y, v->w_y0); }
            if (!v->k_ok || v->k_bilder == 0) {
                printf("  FEHLER %s %04X/%d: Ausstieg nicht gemessen\n", K, id, em); abw++; continue; }
            if (v->k_latch3 != v->k_bilder) { abw++;
                printf("  ABWEICHUNG %s %04X/%d K: Latch (+0x93&3)==3 nur in %d von %d Ausstiegsbildern (Original: "
                       "Phase 0 `ori v0,v0,0x3` @0x80114f84 .. Phase 8 `sb zero,147` @0x80115344, gemessen 03)\n",
                       K, id, em, v->k_latch3, v->k_bilder); }
            if (v->k_ziel || v->k_bilder42 == 0) { abw++;
                printf("  ABWEICHUNG %s %04X/%d K: Zielhilfe waehlt die KRIECHENDE Spinne in %d von %d Bildern mit grid "
                       "0x42 (@0x8003710c-10)\n", K, id, em, v->k_ziel, v->k_bilder42); }
            if (v->k_res) { abw++;
                printf("  ABWEICHUNG %s %04X/%d K: der Resolver nimmt die KRIECHENDE Spinne in %d Bildern (Band 0 / "
                       "Latch @0x80012100)\n", K, id, em, v->k_res); }
            if (v->k_treffer) { abw++;
                printf("  ABWEICHUNG %s %04X/%d K: %d Treffer beim Herauskriechen, danach %d/%d grid 0x%02X y %d\n", K,
                       id, em, v->k_treffer, v->k_zustand, v->k_sub, v->k_grid, v->k_y); }
            if (!v->k_gelandet) { abw++;
                printf("  ABWEICHUNG %s %04X/%d K: die Spinne landet unter Beschuss nicht (danach %d/%d grid 0x%02X y %d)\n",
                       K, id, em, v->k_zustand, v->k_sub, v->k_grid, v->k_y); }
            if (!v->l_ok) {
                printf("  FEHLER %s %04X/%d: Landung nicht erreicht\n", K, id, em); abw++; continue; }
            if (v->l_res_p8) { abw++;
                printf("  ABWEICHUNG %s %04X/%d L: direkt nach Phase 8 nimmt der Resolver die Spinne (Original: word0 "
                       "0x00000801 bis zum ersten ACTIVE, Stempel @0x80110fd0)\n", K, id, em); }
            if (!v->l_res_a1 || !v->l_ziel_a1) { abw++;
                printf("  ABWEICHUNG %s %04X/%d L: nach dem ersten ACTIVE-Takt Resolver %d / Zielhilfe %d (Original: "
                       "0x60000801 + 0x800acaf3 = 1)\n", K, id, em, v->l_res_a1, v->l_ziel_a1); }
            if (VFL[fi] == RE15_AI_FLAVOR_RE15 && (v->l_band_p8 != 0 || v->l_band_a1 != 3)) { abw++;
                printf("  ABWEICHUNG %s %04X/%d L: Band nach Phase 8 %d / nach dem ersten ACTIVE %d, erwartet 0 / 3 "
                       "(INIT @0x80110d38-48, Stempel @0x80110fd0: FUN_80012aa4(0xfa0) mit s0 0 -> LEVEL @0x80012b3c-44, "
                       "0 < 2000 < 4000 -> DOWN @0x80012bb8; Original gemessen 0x00000801 -> 0x60000801)\n", K, id, em,
                       v->l_band_p8, v->l_band_a1); }
            if (v->l_treffer == 0) { abw++;
                printf("  ABWEICHUNG %s %04X/%d L: nach der Landung NIE getroffen (%d Bilder)\n", K, id, em, v->l_bilder); }
            if (v->l_schwebt) { abw++;
                printf("  ABWEICHUNG %s %04X/%d L: %d Bilder ueber dem Boden (y %d..%d)\n", K, id, em, v->l_schwebt,
                       v->l_y_min, v->l_y_max); }
        }
        for (int el = 0; el < 2; el++) {
            const tuer_mess_t *t = &s_tuer[fi][el];
            const char *W = el ? "2031 -> 2061" : "2030 -> 2060";
            if (!t->ok) { printf("  FEHLER %s Tuerweg %s nicht gemessen\n", K, W); abw++; continue; }
            if (t->ziel || t->treffer || !t->wartet || t->blick_nach != t->blick_vor) { abw++;
                printf("  ABWEICHUNG %s Tuerweg %s: Zielhilfe %d, Treffer %d, Blick 0x%03X -> 0x%03X, wartet %d "
                       "(Original: kein Ziel, kein Treffer, Blick bleibt, sie wartet bis Cut 1)\n", K, W, t->ziel,
                       t->treffer, (unsigned)(uint16_t)t->blick_vor, (unsigned)(uint16_t)t->blick_nach, t->wartet); }
        }
        for (int k = 0; k < Z60_N; k++) {
            const zensus60_t *Z = &Z60[k];
            const z60_mess_t *m = &s_z60[fi][k];
            if (!m->ok || !m->gespawnt) {
                printf("  (Zensus %s %s: im Testaufbau nicht gespawnt - nicht gemessen)\n", K, Z->tag); continue; }
            if (m->grid_spawn != Z->grid0) { abw++;
                printf("  FEHLER %s Zensus %s: Spawn-grid 0x%02X, erwartet 0x%02X (%s)\n", K, Z->tag, m->grid_spawn,
                       Z->grid0, Z->quelle); }
            if (m->ziel_versteckt) { abw++;
                printf("  ABWEICHUNG %s Zensus %s: versteckt (grid 0x%02X) gewaehlt (@0x8003710c-10)\n", K, Z->tag,
                       m->grid_spawn); }
            if (Z->freigabe >= 0) {
                if (m->frei_bild < 0 || !m->ziel_frei) { abw++;
                    printf("  UNERWUENSCHT %s Zensus %s: nach der Freigabe 0x%02X Bits weg nach %d Bildern, grid 0x%02X, "
                           "gewaehlt %d — der Port loescht die Original-Stelle nicht (%s)\n", K, Z->tag, Z->freigabe,
                           m->frei_bild, m->grid_frei, m->ziel_frei, Z->quelle); }
            } else if (!m->nie_gewaehlt) { abw++;
                printf("  ABWEICHUNG %s Zensus %s: ohne Freigabe doch gewaehlt bzw. Bits weg (grid 0x%02X)\n", K, Z->tag,
                       m->grid_frei); }
        }
        for (int k = 0; k < 3; k++) {
            const stegband_t *m = &s_steg[fi][k];
            if (!m->ok) { printf("  FEHLER %s Stegspinne 2050 Lage %d nicht gemessen\n", K, k); abw++; continue; }
            if (VFL[fi] == RE15_AI_FLAVOR_RE15) {
                /* RE1.5-Gehirn: Band = ACTIVE-Stempel, Kandidat genau bei nicht leerem Band (@0x800120e4-ec) */
                if (m->band != m->soll || m->treffer != (m->band != 0)) { abw++;
                    printf("  ABWEICHUNG %s Stegspinne Lage %d: Band %d, Soll %d (FUN_80012aa4(0xfa0) @0x80110fd0), Resolver %d "
                           "(Soll %d)\n", K, k, m->band, m->soll, m->treffer, m->soll != 0); }
            } else if (!m->treffer) { abw++;   /* RE2-Gehirn: kein Hoehenband, Kandidat wie vorher */
                printf("  UNERWUENSCHT %s Stegspinne Lage %d: der Resolver nimmt die RE2-Spinne nicht mehr\n", K, k); }
        }
    }
    return abw;
}

static void lauf_versteckt(int budget)
{
    for (int fi = 0; fi < 2; fi++) {
        for (int i = 0; i < SCHACHT_N; i++) pass_versteckt(fi, i, budget);
        pass_versteckt_tuer(fi, 0, budget);
        pass_versteckt_tuer(fi, 1, budget);
        for (int k = 0; k < Z60_N; k++) pass_zensus60(fi, k);
        pass_stegband(fi);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

static void pass_spinne_decke(int scharf, int budget)
{
    if (!load_room("STAGE2/ROOM2050.RDT", 0x2050, -1)) {
        printf("  FEHLLAUF: ROOM2050 fehlt\n"); return; }
    int slot = setup_target(0x25, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: keine Spinne 0x25 in 2050 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    /* Deckenvariante erzwingen: Deskriptor 2 + INIT-Zustand nochmal. */
    e->re2z_f10e = (uint16_t)((e->re2z_f10e & 0xfff0u) | 2u);
    e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    aim_up(slot, 2000);
    int treffer = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        int16_t vor = e->hp;
        if (e->hp < 60) { e->hp = 400; vor = 400; }
        if (!e->active) break;
        e->re2_hit_box_set = (uint8_t)(scharf ? 1 : 0);
        track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->hp < vor) treffer++;
    }
    s_decke_b = e->re2_hit_b98; s_decke_h = (int)e->re2_hit_h9e;
    if (scharf) s_decke_scharf = treffer; else s_decke_aus = treffer;
    printf("  [SPINNE DECKE %s] Box %d/%d | ECHTE Treffer %3d in %d Bildern%s",
           scharf ? "TOR SCHARF" : "TOR AUS   ", s_decke_b, s_decke_h, treffer, budget,
           (s_decke_b != 0)
             ? "   <== FEHLLAUF: der erzwungene Re-INIT nahm den Deckenzweig NICHT"
               " (Box unveraendert) - sagt NICHTS ueber die Deckenspinne"
             : "");
    printf("%s", NL);
}

/* ===== TEIL 3: GEGEN-UEBERKORREKTUR — jeder Typ wird weiter getroffen ====================
 * Das fuenfte Tor ist fuer die vier neu verdrahteten Typen bewusst UNSCHARF (box_set = 0).
 * Diese Messung belegt, dass die Box-Werte allein nichts zugemacht haben. */
static int pass_treffbar(const char *tag, uint8_t type, const char *room, int room_id,
                         int fire_sub, int weapon, int baby_force, int budget)
{
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%-11s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return -1; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) {
        printf("  [%-11s] FEHLLAUF: kein Gegner 0x%02X in %04X - sagt NICHTS\n",
               tag, type, room_id); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    int treffer = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->hp < 30000) treffer++;
    }
    printf("  [%-11s] Treffer %3d in %3d Bildern -> %s\n", tag, treffer, budget,
           treffer > 0 ? "treffbar" : "⛔ NIE GETROFFEN");
    return treffer;
}

static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;

int main(int argc, char **argv)
{
    int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    /* `versteckt` (Runde 38 Integration, ctest r31_trefferboxen_versteckt): NUR der Riegel der versteckten
     * Schachtspinne (RE2- und RE1.5-KI, Tuerweg Leon/Elza) und der Zensus grid & 0x60, Urteil gegen das Original
     * (Exit 1 bei jeder Abweichung). Block "RIEGEL `versteckt`" oben. */
    int versteckt = (argc > 1 && strcmp(argv[1], "versteckt") == 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    /* ⛔ OHNE die PL00-Bank liefert re15_player_muzzle_world 0 und das Tor-Urteil waere
     * ein leeres Feld (Falle aus Runde 29/30). */
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) { printf("SKIP: PL00.EDD/EMR fehlt"); return 77; }
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 ||
        re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) != 0) {
        printf("FAIL: PL00-Parse"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    if (versteckt) {
        printf("=== r31 versteckt: die Schachtspinne IM SCHACHT gegen das Original (RE2- und RE1.5-KI) ===\n");
        lauf_versteckt(240);
        drucke_versteckt();
        int abw = urteil_versteckt();
        if (abw) { printf("FEHLGESCHLAGEN: %d Abweichung(en) vom Original\n", abw); return 1; }
        printf("OK\n");
        return 0;
    }
    printf("=== r31: DIE TREFFERBOXEN ALLER RE2-TYPEN, gemessen ===\n");

    printf("\nTEIL 1 — Box-Zensus je Typ (Soll = Vollscan der Overlays, Welle 2)\n");
    for (int i = 0; i < SOLL_N; i++) pass_zensus(i, 240);

    printf("\nTEIL 2 — die ZOMBIE-Kette: Setz-Stelle und GEGENSTELLE in einem Lauf\n");
    pass_zombie_kette(900);

    printf("\nTEIL 2b — die laengste Sperre unter Dauerbeschuss (Zombie 0x10)\n");
    pass_zombie_dauer(1800);

    printf("\nTEIL 2c — das Tor SCHARF vs. AUS, ECHTE Treffer, JE TYP\n");
    for (int i = 0; i < SOLL_N; i++) {
        pass_scharf(i, 0, 900);
        pass_scharf(i, 1, 900);
        pass_scharf(i, 2, 900);    /* KONTROLLE: misst die Sonde das Tor ueberhaupt? */
    }

    printf("\nTEIL 2d — die DECKENspinne (Spawn-Deskriptor 2, @0x8010047C/@0x8010049C)\n");
    pass_spinne_decke(0, 900);
    pass_spinne_decke(1, 900);
    printf("  Box je AUSGELIEFERTEM Spinnenraum:\n");
    pass_spinne_raeume();

    printf("\nTEIL 2e — die SCHACHTSPINNEN (Runde 38 Spur R): Original-Ausloeser, Herauskriechen, Landung\n");
    /* Budgets sind Messfenster, keine Spielwerte. Das Original braucht vom Ausloeser (grid 0x41 -> 0x42;
     * bis dahin wartet Phase 0: `lbu v1,9(a0)` / `ori v0,zero,0x41` / `beq v1,v0` @0x80114f14-1c) bis zum
     * ACTIVE-Zustand (Zustandswort 1: `ori v0,zero,0x1` / `sh v0,4(v1)` @0x8011539c-a0 = 01 00 02 34
     * 04 00 62 a4, STAGE2.BIN) 73 Bilder (ROOM2060 Spinne 0 aus y -3600, R_belege/orig_2060_schacht_gift.txt
     * Zeile n62 VSync 5653 -> n135 VSync 5799) bzw. 90 Bilder (ROOM20A0 Spinne 0 aus y -5400,
     * R_belege/orig_20a0_schacht.txt grid 0x42 Zeile n41 VSync 5625 -> Zustand 1 Zeile n131 VSync 5805;
     * je Bild 2 VSyncs: (5805 - 5625) / 2 = 131 - 41 = 90). Die frueher hier zitierten 80 Bilder
     * (R_spinnen.md 4.1, Bild ~159 -> 239) waren die PORT-Messung auf dem echten Weg, nicht das Original.
     * 600 Bilder lassen jedem Ausstieg ein Vielfaches davon. 240 Bilder Beschuss danach wie Teil 3. */
    for (int i = 0; i < SCHACHT_N; i++) pass_schacht(i, 600, 240);

    /* Die Schachtspinne IM SCHACHT (Zielhilfe/Resolver/Latch, RE2- und RE1.5-KI) prueft seit der Integration
     * Runde 38 der eigene Riegel `probe_r31_boxen versteckt` (ctest r31_trefferboxen_versteckt). */

    printf("\nTEIL 3 — Gegen-Ueberkorrektur: jeder Typ wird weiter getroffen\n");
    int t[SOLL_N];
    for (int i = 0; i < SOLL_N; i++)
        t[i] = pass_treffbar(SOLL[i].tag, SOLL[i].type, SOLL[i].room, SOLL[i].room_id,
                             SOLL[i].fire_sub, SOLL[i].weapon, SOLL[i].baby, 240);

    if (!riegel) return 0;

    int fail = 0;
    printf("\n=== RIEGEL ===\n");
    for (int i = 0; i < SOLL_N; i++) {
        if (!s_zen[i].ok) { printf("  FEHLER: %s nicht gemessen\n", SOLL[i].tag); fail++; continue; }
        if (!soll_ok(&SOLL[i], s_zen[i].b, s_zen[i].h)) {
            printf("  FEHLER: %s Box %d/%d ist KEIN Original-Paar (%s)\n", SOLL[i].tag,
                   s_zen[i].b, s_zen[i].h, SOLL[i].quelle);
            fail++;
        }
        /* box_set = das fuenfte Tor @0x8004716C-A4 ist fuer diesen Typ SCHARF.
         * Scharf sind ZOMBIE 0x10, HUND 0x20 und SPINNE 0x25 — je gemessen mit
         * TOR AUS == TOR SCHARF bei gleicher Trefferzahl (Teil 2c) und einer Kontrolle,
         * die mit Box 0/0 auf 0 Treffer faellt. NICHT scharf sind KRAEHE 0x21 und
         * BABY 0x26: dort faellt die Trefferzahl mit scharfem Tor auf 0 von 900. */
        int soll_set = (SOLL[i].type == 0x10 || SOLL[i].type == 0x20 ||
                        SOLL[i].type == 0x25);
        if (s_zen[i].box_set != soll_set) {
            printf("  FEHLER: %s box_set=%d, erwartet %d\n", SOLL[i].tag,
                   s_zen[i].box_set, soll_set);
            fail++;
        }
        if (t[i] == 0) { printf("  FEHLER: %s wurde NIE getroffen\n", SOLL[i].tag); fail++; }
    }
    /* Die Kette darf keine Sackgasse sein. */
    if (s_kette2.klein_von < 0) {
        printf("  FEHLER: Zombie-Box wurde nie klein — die Kette ist nicht gemessen\n"); fail++;
    } else if (s_kette2.gross_wieder < 0) {
        printf("  FEHLER: SACKGASSE — die Zombie-Box wird nach dem Niederschlag nie "
               "wieder gross (Rampe @0x8010366C-94 nicht erreicht)\n"); fail++;
    } else if (s_kette2.min_b > -350) {
        printf("  FEHLER: kleinste Box %d, erwartet <= -350 (@0x80103464)\n", s_kette2.min_b);
        fail++;
    }
    /* Der stehende Zombie muss durch das Tor kommen — sonst waere die Zielpose-Luecke
     * nicht der Grund fuer das unscharfe Tor, sondern die Box selbst. */
    if (s_zen[0].gueltig > 0 && s_zen[0].durch != s_zen[0].gueltig) {
        printf("  FEHLER: stehender Zombie faellt durch das Tor (%d/%d)\n",
               s_zen[0].durch, s_zen[0].gueltig); fail++;
    }
    /* ===== DIE RIEGEL DES SPERR-MECHANISMUS (Runde 34 umgestellt) =========================
     * (a) ⛔ MECHANISMUS-PIN: beim SCHUSS darf das fuenfte Tor gar nicht mehr befragt werden.
     *     Der Zaehler c_gate5 MUSS 0 sein — er zaehlt genau die Urteile @0x8004716C-A4.
     * (b) Fuer die APPLIER-TYPEN (Zombie-Familie, Hund 0x20) muss die Klasse gelesen werden
     *     (c_class > 0) und die KONTROLLE mit Klasse 0 auf 0 Treffer fallen.
     * (c) Fuer die NICHT-Applier-Typen (Kraehe 0x21, Spinne 0x25, Baby 0x26) ist die
     *     Klassen-Kontrolle NICHT anwendbar — sie laufen ueber den RE1.5-Streifen. Ihre
     *     Zahlen werden ausgewiesen, nicht behauptet; geriegelt wird nur, dass sie ueberhaupt
     *     getroffen werden (kein Typ dauerhaft untreffbar).
     * (d) Beim HUND MUSS "KLASSE FEST 3" mehr Treffer liefern als "WIE GEBAUT" — das ist der
     *     Nutzer-Befund, jetzt am neuen Mechanismus gemessen. */
    for (int i = 0; i < SOLL_N; i++) {
        const scharf_t *aus = &s_scharf[i][0], *sch = &s_scharf[i][1], *ko = &s_scharf[i][2];
        int applier = (SOLL[i].type == 0x10 || SOLL[i].type == 0x20);
        if (aus->bilder == 0) { printf("  FEHLER: %s Teil 2c nicht gelaufen\n", SOLL[i].tag);
                               fail++; continue; }
        if (sch->c_gate5 != 0u) {
            printf("  FEHLER: %s das fuenfte Tor wurde beim SCHUSS %u mal befragt — es "
                   "gehoert nur noch zum Messer (@0x80042F94)\n",
                   SOLL[i].tag, sch->c_gate5); fail++; }
        if (sch->treffer == 0) {
            printf("  FEHLER: %s wurde in Teil 2c NIE getroffen\n", SOLL[i].tag); fail++; }
        if (applier) {
            if (sch->c_class == 0u) {
                printf("  FEHLER: %s die Haltungsklasse wurde NIE gelesen — der Applier "
                       "laeuft fuer diesen Typ nicht\n", SOLL[i].tag); fail++; }
            if (ko->treffer != 0) {
                printf("  FEHLER: %s KONTROLLE (Klasse 4) traf %d mal — der Applier liegt "
                       "nicht im gemessenen Pfad\n", SOLL[i].tag, ko->treffer); fail++; }
        } else {
            printf("  (Hinweis: %s laeuft NICHT ueber den Applier — Klassen-Kontrolle (K=4) traf "
                   "%d mal, Klassen-Lesungen %u; das ist erwartet, keine Schranke)\n",
                   SOLL[i].tag, ko->treffer, sch->c_class);
        }
        if (SOLL[i].type == 0x20 && aus->treffer <= sch->treffer) {
            printf("  FEHLER: HUND — mit fester Klasse 3 faellt nicht MEHR Schaden (%d) als "
                   "wie gebaut (%d); die Stauchung wirkt nicht\n",
                   aus->treffer, sch->treffer); fail++; }
    }
    /* Jede ausgelieferte Spinne ausserhalb des Schacht-Wartens muss eine tragende Box haben. */
    if (s_spinnen_n == 0) {
        printf("  FEHLER: keine ausgelieferte Spinne gefunden — der Zensus sagt NICHTS\n"); fail++; }
    if (s_nullbox_raeume != 0) {
        printf("  FEHLER: %d ausgelieferte Spinne(n) ausserhalb des Schachts mit Unterkante 0 — das scharfe "
               "Tor sperrt sie dauerhaft\n", s_nullbox_raeume); fail++; }
    /* Genau die vier Sce_em_set-Saetze mit grid 0x41 (ROOM2060 @Datei 0x0E26/0x0E5C, ROOM20A0
     * @0x18CE/0x1904) warten im Schacht — und zwar mit leerem RE2-Kasten (Teil 2e (a)). */
    if (s_schacht_wartend != SCHACHT_N) {
        printf("  FEHLER: %d Spinnen warten im Schacht, erwartet die %d Saetze mit grid 0x41\n",
               s_schacht_wartend, SCHACHT_N); fail++; }
    if (s_schacht_box_falsch != 0) {
        printf("  FEHLER: %d wartende Schachtspinne(n) mit gefuelltem RE2-Kasten — der RE2-INIT lief vor "
               "der Landung\n", s_schacht_box_falsch); fail++; }
    /* ===== TEIL 2e: je Schachtspinne nach dem Original-Ausloeser (Blockkopf Teil 2e (a)-(d)) ===== */
    for (int i = 0; i < SCHACHT_N; i++) {
        const schacht_mess_t *m = &s_schacht[i];
        const char *ort = SCHACHT[i].quelle;
        if (!m->ok) { printf("  FEHLER: Schachtspinne %04X/%d nicht gemessen (%s)\n", SCHACHT[i].id,
                             SCHACHT[i].em, ort); fail++; continue; }
        if (!m->wartet || m->box_vor_b != 0 || m->box_vor_h != 0 || m->box_vor_set != 0) {
            printf("  FEHLER: %04X/%d (a) vor dem Ausloeser wartend %d, RE2-Kasten %d/%d set %d (erwartet "
                   "wartend, 0/0, set 0)\n", SCHACHT[i].id, SCHACHT[i].em, m->wartet, m->box_vor_b,
                   m->box_vor_h, m->box_vor_set); fail++; }
        if (m->ausgeloest < 0) {
            printf("  FEHLER: %04X/%d (b) der Original-Ausloeser setzte grid nicht auf 0x42 (%s)\n",
                   SCHACHT[i].id, SCHACHT[i].em, ort); fail++; continue; }
        if (m->gelandet < 0) {
            printf("  FEHLER: %04X/%d (c) keine Landung mit RE2-INIT nach dem Ausloeser — die Spinne bleibt "
                   "ohne Kasten\n", SCHACHT[i].id, SCHACHT[i].em); fail++; continue; }
        if (m->box_nach_b != -1400 || m->box_nach_h != 1400 || m->box_nach_set != 1) {
            printf("  FEHLER: %04X/%d (c) Kasten nach der Landung %d/%d set %d, erwartet -1400/1400 set 1 "
                   "(Boden-INIT @0x80100460, Tabelle @0x801063A0+20/+26 via @0x801003E4-FC)\n",
                   SCHACHT[i].id, SCHACHT[i].em, m->box_nach_b, m->box_nach_h, m->box_nach_set); fail++; }
        if (m->treffer_nach == 0) {
            printf("  FEHLER: %04X/%d (d) nach der Landung NIE getroffen (%d Bilder)\n", SCHACHT[i].id,
                   SCHACHT[i].em, m->bilder_nach); fail++; }
    }
    if (fail) { printf("FEHLGESCHLAGEN: %d Pruefung(en)\n", fail); return 1; }
    printf("OK\n");
    return 0;
}
