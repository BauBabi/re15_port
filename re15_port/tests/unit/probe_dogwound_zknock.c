/* probe_dogwound_zknock.c — MESSREIHE (Diagnose, kein add_test) zu ZWEI Nutzer-Reports
 * vom 2026-08-21, beide im RE1.5-KI-MODUS (dem Standardmodus):
 *
 *  (1) "Ich bekomme immer noch keinen sichtbaren Bluttreffer, wenn mich der Hund trifft."
 *  (2) "Der fette Zombie im Dining Room faellt immer noch alle 2 Schuesse um und braucht ewig."
 *
 * ORIGINAL-SEITE (selbst disassembliert, STAGE1.BIN roh @0x80100000):
 *  A) Hunde-Biss-Connect FUN_8010f15c Fenster 1, @0x8010f290-0x8010f364:
 *       8010f294 lbu +0x93(acae7)      ; Gate: nur wenn hit_react == 0
 *       8010f2a8 jal 0x8001a804        ; Kegel (0x7d0=2000, 0x180)
 *       8010f2c8 addiu a0,v0,-10       ; Spieler-HP -= 10
 *       8010f2dc bne (+0x1e4)          ; grab_armed != 0 -> GRAB
 *       8010f2e4 bltz hp               ; hp < 0         -> GRAB
 *       8010f2ec jal 0x800453d0 (a0=3) ; Raum-SE 3
 *       8010f30c sb 2 -> 0x800aca58    ; Spieler-cmd 2 (HIT)
 *       8010f31c/24 aca59 = a780()+2   ; Richtung
 *       8010f334 jal 0x8001af20        ; rng
 *       8010f33c-5c panel = byte[0x80121010 + aca59*2 + (rng&1)]
 *                   Bytes @0x80121014 = 05 07 04 06
 *       8010f360 jal 0x80037edc  a1=0x32 (50)   ; WUND-Akkumulator
 *     *** KEIN `jal 0x80019700` — der normale Hundebiss spawnt im ORIGINAL KEIN Blut. ***
 *     Voll-Zensus des Hunde-Codebereichs 0x8010d7f8-0x80112020:
 *       jal 0x80019700 nur @0x8010ebe0 / 0x80110aa0 / 0x80110c88 / 0x80110f9c (Hund selbst)
 *                      und @0x80111b58 / 0x80111ddc / 0x80111e30 (Grab-Release + Finisher)
 *       jal 0x80037edc nur @0x8010f360 (Biss) / 0x80111b64 (Release) / 0x80111e78/84/90 (Finisher)
 *     Wund-Schwelle FUN_80037edc: `sltiu v0,v0,0x78` @0x80037f14 = 120, Level-Clamp 2 @0x80037f40/50.
 *     => 50 pro Biss auf EINES von zwei Panels -> DREI Bisse auf DASSELBE Panel bis Stufe 1.
 *
 *  B) RE1.5-Zombie-POISE (das "alle 2 Schuesse umfallen"):
 *       80100824 jal 0x8001af20 / 8010082c andi v0,v0,0x3 / 80100838 addiu v0,v0,4
 *       8010083c sh v0,476(v1)             ; +0x1dc POISE := (rng&3)+4  = 4..7  (INIT)
 *       Schritt-Tabelle @0x8011fe30 (Zeiger auf Mini-Funktionen):
 *         [1]/[2] -> 0x80105a4c `addiu v0,v0,-2`
 *         [3]..[6]-> 0x80105a6c `addiu v0,v0,-3`     <- die PISTOLE ist Waffe 3
 *         [17]..[20] -> 0x80105a2c `addiu v0,v0,-1`, alle uebrigen 0x80105a24 `jr ra` (0)
 *       Aufruf sitzt IM Phase-0-Block des Stagger-Handlers (@0x80105d28) = EINMAL PRO TREFFER.
 *       Router-Gate @0x80105b18-68:
 *         80105b24 lh +0x1dc / 80105b2c bgez -> return
 *         80105b48 sb 1,+0x4 / 80105b58 sb 0x11,+0x5 / 80105b68 sb 0,+0x6   = KNOCKDOWN
 *     => Pistole: Poise 4/5 -> Sturz beim 2. Treffer, Poise 6/7 -> beim 3.  Aufstehen re-armt
 *        auf 4 (@0x801053xx, Wurf konstant 0) -> ab dann alle 2 Treffer.
 *     ORIGINAL-Zaehigkeit derselben Zombies: HP-Zeile @0x8011f034+typ*0x20
 *        0x10 {61..101} 0x11 {71..107} 0x16 {71..99}; Pistolenschaden @0x8006e0d0+typ*0x58+3*4 = 5
 *        => im RE1.5-ORIGINAL 13..22 Pistolentreffer PRO Zombie.
 *
 * Diese Sonde misst den PORT auf dem ECHTEN Weg (re15_game_step + Pad + echte RDT/SCD-Spawns)
 * gegen genau diese Zahlen, ueber einen RNG-Seed-Sweep (Flinch/Poise/Panelwahl sind zufaellig).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_collision.h"
#include "re15_room.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id  = 0x1140;
static int                s_fire_sub = -1;

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

static void bringup(uint32_t seed)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_esp_fx_reset(); re15_wound_reset();
    re15_damage_seed_rng(seed);
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

static int load_room(const char *base, const char *sub, int room_id, int fsub)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s", base, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse: %s\n", path); return 0; }
    s_room_id = room_id; s_fire_sub = fsub;
    return 1;
}

/* ===================== TEIL 1 — HUNDEBISS: Wunde statt Blutspritzer ======================= */

static uint8_t *s_dogblob = NULL;
static int load_dog_bank(const char *base)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x20);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return 0; }
    free(s_dogblob); s_dogblob = (uint8_t *)malloc(len);
    memcpy(s_dogblob, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_dogblob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

/* Ein Lauf: Leon steht neben dem Hund und wird gebissen; Leon wird am Leben gehalten, damit
 * die Bissfolge nicht durch den Tod abbricht. Ergebnis: Bisse bis Wund-Level >= 1, FX-Spawns
 * waehrend eines Biss-Ticks, Gesamtzahl der Bisse im Budget. */
static void dog_run(uint32_t seed, int *out_bites_to_level, int *out_bites, int *out_fx_on_bite,
                    int *out_level_reached)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    bringup(seed);
    int ds = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { ds = s; break; }
    if (ds < 0) { *out_bites = -1; *out_bites_to_level = -1; *out_fx_on_bite = 0;
                  *out_level_reached = 0; return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != ds) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *d  = &g_actors[ds];
    for (int f = 0; f < 30; f++) frame(0, 0);
    if (d->state == 4 && d->sub_state_1 == 0) d->grid_id = 0x43;   /* SCD-Marke @0x801113e4 */

    int bites = 0, bites_to_level = -1, fx_on_bite = 0, hp_last = pl->hp;
    int fx_last = re15_esp_fx_count();
    for (int f = 0; f < 2400; f++) {
        if (re15_player_victim_state() == 0 && !re15_player_is_grabbed()) {
            pl->x = d->x + 1200; pl->z = d->z; pl->y = d->y; pl->floor = d->floor;
        }
        pl->hp = 100;                       /* am Leben halten: wir messen die BISS-FOLGE */
        int fxb = re15_esp_fx_count();
        frame(0, 0);
        if (pl->hp < hp_last) {              /* ein Treffer ist gelandet */
            bites++;
            if (re15_esp_fx_count() > fxb) fx_on_bite++;
            int lv = 0;
            for (int p = 0; p < 8; p++) if (re15_wound_level(p) > 0) lv = 1;
            if (lv && bites_to_level < 0) bites_to_level = bites;
        }
        hp_last = pl->hp;
        fx_last = fx_last;                   /* (nur Lesbarkeit) */
        if (bites_to_level > 0 && bites >= bites_to_level + 1) break;
    }
    int maxlv = 0;
    for (int p = 0; p < 8; p++) if (re15_wound_level(p) > maxlv) maxlv = re15_wound_level(p);
    *out_bites = bites; *out_bites_to_level = bites_to_level;
    *out_fx_on_bite = fx_on_bite; *out_level_reached = maxlv;
}

/* ===================== TEIL 2 — ZOMBIE: Sturz-Kadenz + Trefferzahl ======================== */

static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) { pl->x = e->x - (int32_t)((double)dx / d * back);
                   pl->z = e->z - (int32_t)((double)dz / d * back); }
    else         { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static int pick_type(int want_type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type
            && !(g_actors[s].grid_id & 0x80)) return s;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type) return s;
    return -1;
}

/* Pistolenlauf gegen EINEN Zombie: Treffer bis Tod, Zahl der KNOCKDOWNS (+0x5 == 0x11),
 * Frames bis Tod, Start-HP, Schaden pro Treffer, Poise-Startwert. */
static int s_flavor = RE15_AI_FLAVOR_RE15;
static int zombie_run(uint32_t seed, int want_type, int *hp0, int *dmg1, int *knocks,
                      int *frames, int *poise0, int *dead)
{
    re15_ai_flavor_set((re15_ai_flavor_t)s_flavor);
    bringup(seed);
    re15_inv_load_briefing();
    g_inv.slots[1].id = 3; g_inv.slots[1].qty = 250;
    re15_player_set_equipped_weapon(3);                 /* Browning HP = Waffe 3 */
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = pick_type(want_type);
    *dead = 0; *knocks = 0; *frames = 0; *hp0 = 0; *dmg1 = 0; *poise0 = 0;
    if (slot < 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, 2600);
    *hp0 = e->hp; *poise0 = e->hit_stun;
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, 2600); frame(RE15_PAD_BIT_R1, 0);
    }
    int hits = 0, hp_last = e->hp, was_knocked = 0;
    for (int f = 0; f < 4000; f++) {
        pl->hp = 100;
        track(slot, 2600);
        int on = ((f >> 1) & 1) == 0;
        frame((uint16_t)(RE15_PAD_BIT_R1 | (on ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(on && ((f & 3) == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) { if (!hits) *dmg1 = hp_last - e->hp; hits++; }
        hp_last = e->hp;
        /* KNOCKDOWN = ACTIVE(+0x4=1) mit +0x5 == 0x11 (Router-Gate @0x80105b48-68).
         * Im RE2-Modus gibt es diese Zeile nicht; dort zaehlt der DOWNED-Bit (+0x9 & 0x80),
         * den beide Gehirne fuehren — so ist die Zahl zwischen den Modi vergleichbar. */
        int knocked = (s_flavor == RE15_AI_FLAVOR_RE15)
                    ? (e->state == 1 && e->sub_state_1 == 0x11)
                    : ((e->grid_id & 0x80) != 0);
        if (knocked && !was_knocked) (*knocks)++;
        was_knocked = knocked;
        *frames = f + 1;
        if (e->hp < 0 || e->state == 3 || e->state == 7) { *dead = 1; break; }
    }
    return hits;
}

int main(int argc, char **argv)
{
    const char *envb = getenv("RE15_ASSET_DIR");
    const char *base = (envb && *envb) ? envb : RE15_ASSET_PSX_DIR;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    int nseeds = 64;
    if (argc > 1) nseeds = atoi(argv[1]);
    if (nseeds < 1) nseeds = 1;

    /* ---------------- TEIL 1: Hundebiss ---------------- */
    if (load_room(base, "STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        int bank = load_dog_bank(base);
        printf("\n===== TEIL 1 — HUNDEBISS (RE1.5-Modus, ROOM1190, echter game_step) =====\n");
        printf("EM020-Bank: %s\n", bank ? "geladen" : "FEHLT");
        printf("Original: KEIN Blut-Spawn (@0x8010f290-364 ohne jal 0x80019700),\n"
               "          nur FUN_80037edc(panel[0x80121014+..], 50) @0x8010f360; Schwelle 120 @0x80037f14\n");
        int sum = 0, n = 0, fxtotal = 0, minb = 999, maxb = 0, nolevel = 0;
        for (int s = 0; s < nseeds; s++) {
            int b2l = 0, bites = 0, fx = 0, lv = 0;
            dog_run(0x1000u + (uint32_t)s * 0x9e3779b9u, &b2l, &bites, &fx, &lv);
            fxtotal += fx;
            if (b2l > 0) { sum += b2l; n++; if (b2l < minb) minb = b2l; if (b2l > maxb) maxb = b2l; }
            else nolevel++;
            if (s < 8)
                printf("  seed %2d: Bisse=%2d  Bisse bis Wund-Level1=%2d  maxLevel=%d  FX-Spawns im Biss-Tick=%d\n",
                       s, bites, b2l, lv, fx);
        }
        printf("  -> %d/%d Laeufe erreichen Level 1; Bisse bis Level 1: min %d / max %d / Mittel %.2f\n",
               n, nseeds, n ? minb : -1, maxb, n ? (double)sum / n : 0.0);
        printf("  -> FX-Spawns waehrend eines Biss-Ticks (Soll 0, Original hat KEINEN): %d\n", fxtotal);
    }

    /* ---------------- TEIL 2: Zombie-Sturz-Kadenz ---------------- */
    if (load_room(base, "STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        printf("\n===== TEIL 2 — ZOMBIE, PISTOLE (RE1.5-Modus, ROOM1140 'Dining Room') =====\n");
        printf("Original-Erwartung: Poise (rng&3)+4 @0x8010082c-3c, -3/Treffer @0x8011fe3c->0x80105a6c,\n"
               "  Sturz-Gate @0x80105b18-68 => Sturz beim 2. (Poise 4/5) bzw. 3. (6/7) Treffer,\n"
               "  danach Poise-Neuladung 4 => ab dann alle 2 Treffer.\n");
        static const int types[3] = { 0x10, 0x11, 0x16 };
        for (int ti = 0; ti < 3; ti++) {
            int t = types[ti];
            int nsum = 0, ksum = 0, fsum = 0, cnt = 0, nmin = 9999, nmax = 0, kmin = 9999, kmax = 0;
            int hp_min = 9999, hp_max = 0, dmg = 0, p_min = 99, p_max = 0, miss = 0;
            for (int s = 0; s < nseeds; s++) {
                int hp0 = 0, d1 = 0, kn = 0, fr = 0, po = 0, dead = 0;
                int hits = zombie_run(0x2000u + (uint32_t)s * 0x9e3779b9u, t,
                                      &hp0, &d1, &kn, &fr, &po, &dead);
                if (hits <= 0 || !dead) { miss++; continue; }
                cnt++; nsum += hits; ksum += kn; fsum += fr; dmg = d1;
                if (hits < nmin) nmin = hits; if (hits > nmax) nmax = hits;
                if (kn < kmin) kmin = kn;     if (kn > kmax) kmax = kn;
                if (hp0 < hp_min) hp_min = hp0; if (hp0 > hp_max) hp_max = hp0;
                if (po < p_min) p_min = po;  if (po > p_max) p_max = po;
                if (s < 4)
                    printf("  Typ 0x%02X seed %2d: hp0=%3d poise0=%d dmg=%2d Treffer=%2d Stuerze=%d Frames=%d\n",
                           t, s, hp0, po, d1, hits, kn, fr);
            }
            if (!cnt) { printf("  Typ 0x%02X: n/a (kein Treffer/Tod im Messrahmen, %d Laeufe)\n", t, miss); continue; }
            printf("  Typ 0x%02X: hp0 %d..%d  poise0 %d..%d  dmg/Treffer %d | Treffer %d..%d (Mittel %.1f) "
                   "| Stuerze %d..%d (Mittel %.1f) | Frames Mittel %.0f | n/a %d\n",
                   t, hp_min, hp_max, p_min, p_max, dmg, nmin, nmax, (double)nsum / cnt,
                   kmin, kmax, (double)ksum / cnt, (double)fsum / cnt, miss);
            printf("           -> Treffer pro Sturz (Mittel) = %.2f\n",
                   ksum ? (double)nsum / ksum : 0.0);
        }

        /* GEGENPROBE: derselbe Zombie im RE2-KI-MODUS. Der Nutzer vergleicht genau das
         * ("verhaelt sich nicht wie in RE2 AI") — die Zahlen zeigen, WAS sich unterscheidet. */
        printf("\n  --- Gegenprobe RE2-KI-Modus, Typ 0x11 (derselbe Messrahmen) ---\n");
        s_flavor = RE15_AI_FLAVOR_RE2;
        {
            int nsum = 0, ksum = 0, fsum = 0, cnt = 0, hpv = 0, dmg = 0, miss = 0;
            for (int s = 0; s < nseeds; s++) {
                int hp0 = 0, d1 = 0, kn = 0, fr = 0, po = 0, dead = 0;
                int hits = zombie_run(0x2000u + (uint32_t)s * 0x9e3779b9u, 0x11,
                                      &hp0, &d1, &kn, &fr, &po, &dead);
                if (hits <= 0 || !dead) { miss++; continue; }
                cnt++; nsum += hits; ksum += kn; fsum += fr; hpv = hp0; dmg = d1;
            }
            if (cnt)
                printf("  Typ 0x11 RE2: hp0=%d dmg=%d | Treffer Mittel %.1f | DOWNED-Phasen Mittel %.1f "
                       "| Frames Mittel %.0f | n/a %d\n",
                       hpv, dmg, (double)nsum / cnt, (double)ksum / cnt, (double)fsum / cnt, miss);
            else
                printf("  Typ 0x11 RE2: n/a (%d Laeufe ohne Treffer/Tod)\n", miss);
        }
        s_flavor = RE15_AI_FLAVOR_RE15;
    }
    return 0;
}
