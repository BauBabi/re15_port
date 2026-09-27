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
 *   (4) und ob das fuenfte Tor @0x8004716C-A4 je Typ SCHARF sein darf. Die Entscheidung
 *       faellt aus drei Laeufen je Typ: TOR AUS / TOR SCHARF / KONTROLLE (Box kuenstlich
 *       0/0). Die Kontrolle MUSS auf 0 Treffer fallen, sonst misst die Sonde das Tor gar
 *       nicht. Ergebnis: scharf fuer 0x10, 0x20 und 0x25 (Trefferzahl unveraendert),
 *       NICHT scharf fuer 0x21 und 0x26 (0 von 900 Treffern = dauerhaft untreffbar).
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

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
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

/* ===== TEIL 2c: DAS TOR SCHARF — ECHTE Treffer statt Tor-Rechnung =======================
 * ⛔ Teil 2b rechnet das Tor nur nach, waehrend die Treffer ungehindert landen. Das ist ein
 * KONTRAFAKTISCHES Mass: mit scharfem Tor faellt der Gegner seltener, also greift die
 * Kriecher-Box auch seltener. Die Entscheidung "darf 0x10 scharf?" braucht darum den echten
 * Lauf — re2_hit_box_set wird je Bild gesetzt (das ist GENAU der Code-Pfad, den ein Einbau
 * haette; kein Env-Schalter im Spielcode), und gezaehlt werden ECHTE HP-Abzuege.
 * Gegenprobe "ALT" = box_set je Bild auf 0 = der Stand von heute. */
typedef struct { int treffer, bilder, luecke_max, luecke, nieder; } scharf_t;
static scharf_t s_scharf[5][3];

/* scharf: 0 = box_set aus (Stand heute), 1 = scharf mit den echten Boxen,
 *         2 = KONTROLLE, scharf mit Box 0/0 (Fenster [-100,100)) — muss 0 Treffer geben,
 *             sonst misst die Sonde das Tor gar nicht. */
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
    int war_unten = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        int16_t vor = e->hp;
        if (e->hp < 60) { e->hp = 400; vor = 400; }
        if (!e->active) break;
        e->re2_hit_box_set = (uint8_t)(scharf ? 1 : 0);
        if (scharf == 2) { e->re2_hit_b98 = 0; e->re2_hit_h9e = 0; }
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        r->bilder++;
        if (e->state == 2 && !war_unten) { r->nieder++; war_unten = 1; }
        if (e->state != 2) war_unten = 0;
        if (e->hp < vor) { r->treffer++; r->luecke = 0; }
        else { r->luecke++; if (r->luecke > r->luecke_max) r->luecke_max = r->luecke; }
    }
    static const char *NAME[3] = { "TOR AUS   ", "TOR SCHARF", "KONTROLLE " };
    printf("  [%-11s %s] %4d Bilder | ECHTE Treffer %3d | Niederschlaege %d | "
           "laengste Strecke OHNE Treffer: %4d Bilder (%.1f s)\n",
           S->tag, NAME[scharf], r->bilder, r->treffer, r->nieder,
           r->luecke_max, r->luecke_max / 30.0);
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
static int s_nullbox_raeume = 0;    /* Raeume mit einer AUSGELIEFERTEN Spinne, deren
                                     * Unterkante 0 ist (Decken-/Wandspawn) */

/* Welche Box traegt eine AUSGELIEFERTE Spinne je Raum? Das entscheidet, ob das fuenfte Tor
 * fuer 0x25 gefahrlos scharf darf: der Boden-Spawn traegt -1400/1400 (Tabelle @0x801063A0,
 * Fenster [-100, 2900) -> Hgun 1668 kommt durch), Decke (@0x8010049C) und Wand
 * (Tabelle @0x801063E0 @0x801004EC-52C) tragen 0/1400 (Fenster [-1500, 1500) -> gesperrt). */
static void pass_spinne_raeume(void)
{
    static const struct { const char *sub; int id; } RAUM[] = {
        { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE1/ROOM10D0.RDT", 0x10D0 },
        { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
        { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
        { "STAGE2/ROOM2030.RDT", 0x2030 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
        { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE2/ROOM2070.RDT", 0x2070 },
        { "STAGE2/ROOM20A0.RDT", 0x20A0 }, { "STAGE3/ROOM3010.RDT", 0x3010 },
        { "STAGE3/ROOM3020.RDT", 0x3020 }, { "STAGE4/ROOM4050.RDT", 0x4050 },
        { "STAGE4/ROOM4070.RDT", 0x4070 },
    };
    int gefunden = 0;
    for (unsigned i = 0; i < sizeof RAUM / sizeof RAUM[0]; i++) {
        if (!load_room(RAUM[i].sub, RAUM[i].id, -1)) continue;
        int slot = setup_target(0x25, 3, 0);
        if (slot < 0) continue;
        re15_actor_t *e = &g_actors[slot];
        gefunden++;
        if (e->re2_hit_b98 == 0) s_nullbox_raeume++;
        printf("    %04X: Modus %u | Box %6d/%-5d %s\n", RAUM[i].id,
               (unsigned)e->re2s_mode222, e->re2_hit_b98, (int)e->re2_hit_h9e,
               (e->re2_hit_b98 == 0) ? "-> Fenster [-1500,1500) = GESPERRT" : "-> DURCH");
    }
    printf("  %d Raeume mit lebender 0x25 geprueft, davon %d mit Unterkante 0 (gesperrt)\n",
           gefunden, s_nullbox_raeume);
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
    /* ===== DIE ENTSCHEIDENDEN RIEGEL DES SCHARFEN TORES ===================================
     * (a) Die KONTROLLE muss je Typ auf 0 Treffer fallen — sonst misst die Sonde das Tor
     *     gar nicht und alle anderen Zahlen sind wertlos (Falle "Sonde luegt").
     * (b) Fuer jeden SCHARFEN Typ muss TOR SCHARF genauso viele echte Treffer liefern wie
     *     TOR AUS. Der Hund ist ausgenommen: bei ihm IST die Absenkung der Befund
     *     (liegender Hund nicht treffbar, Runde 30).
     * (c) Fuer jeden NICHT scharfen Typ muss TOR SCHARF nachweislich auf 0 fallen — das ist
     *     die Zahl, mit der die Entscheidung begruendet ist. */
    for (int i = 0; i < SOLL_N; i++) {
        const scharf_t *aus = &s_scharf[i][0], *sch = &s_scharf[i][1], *ko = &s_scharf[i][2];
        if (aus->bilder == 0) { printf("  FEHLER: %s Teil 2c nicht gelaufen\n", SOLL[i].tag);
                               fail++; continue; }
        if (ko->treffer != 0) {
            printf("  FEHLER: %s KONTROLLE (Box 0/0) traf %d mal — die Sonde misst das Tor "
                   "NICHT\n", SOLL[i].tag, ko->treffer); fail++; }
        if (SOLL[i].type == 0x10 || SOLL[i].type == 0x25) {
            if (sch->treffer != aus->treffer) {
                printf("  FEHLER: %s scharfes Tor aendert die Trefferzahl (%d statt %d)\n",
                       SOLL[i].tag, sch->treffer, aus->treffer); fail++; }
        } else if (SOLL[i].type == 0x21 || SOLL[i].type == 0x26) {
            if (sch->treffer != 0) {
                printf("  FEHLER: %s waere mit scharfem Tor doch treffbar (%d) — dann muss es "
                       "gebaut werden\n", SOLL[i].tag, sch->treffer); fail++; }
            if (aus->treffer == 0) {
                printf("  FEHLER: %s wird schon OHNE Tor nie getroffen — die Messung sagt "
                       "nichts\n", SOLL[i].tag); fail++; }
        }
    }
    /* Alle ausgelieferten Spinnenraeume muessen eine tragende Box haben. */
    if (s_nullbox_raeume != 0) {
        printf("  FEHLER: %d ausgelieferte Spinnenraeume mit Unterkante 0 — das scharfe Tor "
               "sperrt sie dauerhaft\n", s_nullbox_raeume); fail++; }
    if (fail) { printf("FEHLGESCHLAGEN: %d Pruefung(en)\n", fail); return 1; }
    printf("OK\n");
    return 0;
}
