/* probe_r28_messer_dauerschlag.c — MESSSONDE (Schritt 1: reproduzieren, KEIN Fix).
 *
 * Nutzer 2026-09-26: "wenn ich mit dem Messer die ganze Zeit nach unten schlage, kommen die
 * Zombies nicht nah genug an mich ran, um mich zu beissen. bzw. sie beissen mich dann nicht."
 *
 * Gefahren wird der ECHTE Weg (re15_game_step + Pad, echte ROOM1140-Spawns, RE2-Bank EM010,
 * RE2-KI = der Auslieferungs-Default seit 2026-08-22). Je Bild protokolliert:
 *   Abstand Spieler<->Zombie | Zombie +0x4/+0x5/+0x6 | Zombie +0x1D3 | Spieler-Zielphase |
 *   Waffenklasse (s_aim_melee) | Schlagzahl | Bissversuche | Spieler-HP
 *
 * Vier Laeufe, sonst identisch:
 *   A  Messer angelegt, R1 + RUNTEN gehalten, SQUARE im Dauerschlag
 *   B  Messer angelegt, R1 + RUNTEN gehalten, KEIN Schlag
 *   C  Messer angelegt, gar nicht gezielt (kein R1) — der Grundfall
 *   D  wie A, aber der Zombie ist UNVERWUNDBAR (flags|=4 -> aec4-Skip aus? nein:
 *      Schadens-Immunitaet ueber hit_react-Latch) — trennt "Schlag" von "Treffer"
 *
 * Original-Belege fuer die gemessenen Groessen:
 *   Koerper-Standabstand = 400 (Zombie-Box STAGE1.BIN @file 0x1f778) + 450 (Spieler-Box
 *   PSX.EXE @file 0x64694 = 0x1c2) = 850  (FUN_8002aec4 radSum @0x8002b164).
 *   Seiten-Griff G (das EINZIGE Angriffs-Tor ohne Bewegungs-Gate) @0x801018f4:
 *   sltiu 0x4b0 = dist < 1200.
 *   Lunge D/E @0x8010185c/@0x801018a4 verlangen 0x800CFBF6 & 0x15 bzw. & 0x17 —
 *   Spieler MUSS laufen/gehen.
 */
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_melee_dbg(void);
extern int  re15_player_aim_phase_debug(void);
extern int  re15_player_aim_clip(void);
extern int  re15_player_slash_window(void);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern void re15_re2z_onesave_reset(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

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

/* PL00W01 (Messer) EDD: 14 Clips. Nur die Laengen, die die Zielmaschine braucht. */
static const uint16_t FC_W01[14] = {22,16,52,1,50,30,10,22,1,23,1,23,1,32};

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    re15_re2z_onesave_reset();
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_lens(FC_W01, 14);
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

static int32_t dist2d(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t q = dx*dx + dz*dz; int32_t r = 0;
    while ((int64_t)(r+1)*(r+1) <= q) r++;
    return r;
}

typedef struct {
    const char *name;
    int bilder, schlaege, bisse, hp_verlust;
    int32_t dmin, dmax;
    int grab_versuche, lunge_versuche, snap_versuche;
    int treffer;                /* Treffer auf den Zombie (HP-Abfall)       */
    int frames_1d3_nz;          /* Bilder mit Zombie +0x1D3 != 0            */
    int frames_dlt1200;         /* Bilder mit Abstand < 1200 (Griff-Tor)    */
    int frames_hitreact_b0;     /* Bilder mit Zombie +0x93 Bit0 (Latch)     */
    int zhp_ende;
} lauf_t;

/* modus: 0 = A (Dauerschlag), 1 = B (nur zielen), 2 = C (gar nicht zielen),
 *        3 = D (Dauerschlag, Zombie unverwundbar). */
static void lauf(lauf_t *r, const char *name, int modus, int budget, int verbose)
{
    memset(r, 0, sizeof *r); r->name = name; r->dmin = 0x7fffffff; r->dmax = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(1);            /* MESSER (Id 1, Klasse < 3 @0x80074030) */

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    load_re2_bank(0x10);
    int slot = standing_zombie();
    if (slot < 0) { printf("FEHLLAUF %s: kein stehender Zombie\n", name); r->bilder = -1; return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    /* Startaufstellung: 2600 vor dem Zombie, Blick auf ihn. Er muss also selbst herankommen —
     * genau der Nutzerfall. */
    pl->x = e->x - 3400; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset();
    /* AUFWAERM-PHASE, in ALLEN Laeufen identisch: R1 halten bis zielbereit. Erst danach
     * beginnt die Messung — sonst unterscheiden sich A und B schon durch die Zielphase. */
    if (modus != 2) {
        int w = 0;
        for (; w < 120 && !re15_player_aim_ready(); w++) {
            pl->hp = 100; frame(RE15_PAD_BIT_R1, 0);
        }
        printf("Aufwaermen: zielbereit nach %d R1-Bildern, Abstand %d\n", w, dist2d(pl, e));
    }

    printf("\n===== LAUF %s =====\n", name);
    printf("Zombie Slot %d Typ 0x%02X hp=%d r_min=%u | Start-Abstand %d | Standabstand 400+450=850"
           " | Griff-Tor <1200 (@0x801018f4)\n",
           slot, e->type, e->hp, e->hit_radius_min, dist2d(pl, e), (int)0);

    int hp_last = pl->hp, zhp_last = e->hp;
    int s1_last = -1, ph_last = -1;
    int slash_lauf = 0;                 /* laeuft gerade ein Schlagfenster? */
    int zyklus = 0;

    for (int f = 0; f < budget; f++) {
        uint16_t cur = 0, edge = 0;
        if (modus != 2) {
            cur |= RE15_PAD_BIT_R1;
            cur |= RE15_PAD_BIT_DOWN;                     /* "nach unten" = Elevation tief */
        }
        if ((modus == 0 || modus == 3) && re15_player_aim_ready()) {
            /* Dauerschlag: SQUARE halten, mit einer Flanke je Zyklus (der Port verlangt die
             * Flanke wie das Original @0x80033344 / der Melee-Zwilling). */
            cur |= RE15_PAD_BIT_SQUARE;
            if ((zyklus % 4) == 0) edge |= RE15_PAD_BIT_SQUARE;
            zyklus++;
        }
        int ph_v = re15_player_aim_phase_debug();
        frame(cur, edge);
        r->bilder++;

        if (modus == 3) { e->hp = 60; e->hit_react = 0; }   /* unverwundbar halten */

        int32_t d = dist2d(pl, e);
        if (d < r->dmin) r->dmin = d;
        if (d > r->dmax) r->dmax = d;
        if (d < 1200) r->frames_dlt1200++;
        if (e->re2z_self1d3 != 0) r->frames_1d3_nz++;
        if (e->hit_react & 1u) r->frames_hitreact_b0++;

        /* Schlagzaehlung: jede STEIGENDE Flanke des Schlagfensters = ein Schlag */
        int sw = re15_player_slash_window();
        if (sw && !slash_lauf) r->schlaege++;
        slash_lauf = sw;

        if (e->hp < zhp_last) r->treffer++;
        zhp_last = e->hp;

        if (pl->hp < hp_last) { r->bisse++; r->hp_verlust += (hp_last - pl->hp); }
        hp_last = pl->hp;

        if (e->state == 1 && e->sub_state_1 != s1_last) {
            if (e->sub_state_1 == 3)  r->grab_versuche++;
            if (e->sub_state_1 == 12) r->lunge_versuche++;
            if (e->sub_state_1 == 14) r->snap_versuche++;
        }

        int ph = re15_player_aim_phase_debug();
        if (verbose && (e->sub_state_1 != s1_last || ph != ph_last || (f % 60) == 0)) {
            printf("f%-4d d=%5d | z st=%d s1=%2u s2=%2u 1D3=%3u 93=%02X hp=%3d | "
                   "pl ph=%02X melee=%d clip=%2d frame=%2u hp=%3d | slash=%d\n",
                   f, d, e->state, e->sub_state_1, e->sub_state_2, e->re2z_self1d3,
                   e->hit_react, e->hp, ph_v, re15_player_aim_melee_dbg(),
                   re15_player_aim_clip(), pl->anim_frame, pl->hp, sw);
        }
        s1_last = (e->state == 1) ? (int)e->sub_state_1 : -1;
        ph_last = ph;

        pl->hp = 100;                     /* HP konstant halten: wir zaehlen BISSE, nicht Tod */
        hp_last = 100;
        if (e->hp < 0) { printf("f%-4d ZOMBIE TOT — Lauf endet\n", f); break; }
    }
    r->zhp_ende = e->hp;
    printf("ABDECKUNG %s: %d Bilder | %d Schlaege | %d Treffer | %d Bisse | "
           "d_min=%d d_max=%d | <1200 in %d Bildern | +0x1D3!=0 in %d | +0x93&1 in %d | "
           "Griff=%d Lunge=%d Snap=%d | Zombie-HP Ende %d\n",
           name, r->bilder, r->schlaege, r->treffer, r->bisse, r->dmin, r->dmax,
           r->frames_dlt1200, r->frames_1d3_nz, r->frames_hitreact_b0,
           r->grab_versuche, r->lunge_versuche, r->snap_versuche, r->zhp_ende);
}


/* ---- REICHWEITEN-SWEEP: ab welchem Abstand landet ein Messerschlag? --------------------
 * KI angehalten, Zombie auf festen Abstand gesetzt, Spieler zielt (TIEF oder EBEN) und
 * schlaegt EINMAL. Gemessen wird, ob die HP fallen.
 * Vergleichswerte der Originale:
 *   RE1.5: Dispatch @0x8006E548[1] = 0x800127FC = Nahkampf-KEGEL, R = Reichweite
 *          (@0x8006E5A0[1] = 1100) + Gegner-Radius (400) = 1500 ab dem KLINGEN-Punkt.
 *   RE2  : Geometrie-Records des Messers @0x800A63A8 (EBEN) / @0x800A657C (TIEF),
 *          Stride 0x1C, Muster `ff/6 00/1 01/1 02/1 03/1 04/1 00/255` @0x800A6434.
 */
static int reichweite_sweep(int elev_down, int re2)
{
    printf("\n===== REICHWEITEN-SWEEP  (%s, %s) =====\n", elev_down ? "TIEF/RUNTER" : "EBEN", re2 ? "RE2-KI" : "RE1.5-KI");
    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(1);
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    load_re2_bank(0x10);
    int slot = standing_zombie();
    if (slot < 0) { printf("FEHLLAUF Sweep: kein Zombie\n"); return -1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    re15_enemy_ai_set_paused(1);                    /* KI eingefroren: nur die Geometrie zaehlt */
    pl->rot_y = 1024; pl->y = e->y; pl->z = e->z;
    uint16_t down = elev_down ? (uint16_t)RE15_PAD_BIT_DOWN : 0u;
    int letzter_treffer = -1, erster_treffer = -1;
    for (int d = 400; d <= 3600; d += 100) {
        pl->x = e->x - d; pl->hp = 100;
        re15_player_cmd_reset(); re15_player_aim_reset();
        e->hp = 200; e->hit_react = 0; e->re2z_self1d3 = 0;
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1;
        int w = 0;
        for (; w < 120 && !re15_player_aim_ready(); w++) frame((uint16_t)(RE15_PAD_BIT_R1|down), 0);
        int hp0 = e->hp, getroffen = 0;
        for (int k = 0; k < 40; k++) {
            e->hit_react = 0; e->re2z_self1d3 = 0;           /* jeden Tick treffbar halten */
            frame((uint16_t)(RE15_PAD_BIT_R1|down|RE15_PAD_BIT_SQUARE),
                  (uint16_t)((k == 0) ? RE15_PAD_BIT_SQUARE : 0));
            if (e->hp < hp0) { getroffen = 1; break; }
        }
        if (getroffen) { if (erster_treffer < 0) erster_treffer = d; letzter_treffer = d; }
        printf("  d=%4d  %s\n", d, getroffen ? "TREFFER" : "-");
    }
    printf("REICHWEITE (%s, %s): Treffer von %d bis %d\n",
           elev_down ? "TIEF" : "EBEN", re2 ? "RE2" : "RE1.5", erster_treffer, letzter_treffer);
    re15_enemy_ai_set_paused(0);
    return letzter_treffer;
}

int main(int argc, char **argv)
{
    int budget  = (argc > 1) ? atoi(argv[1]) : 900;
    int verbose = (argc > 2) ? atoi(argv[2]) : 0;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    if (getenv("RE15_R28_SWEEP")) { reichweite_sweep(1,1); reichweite_sweep(0,1);
                                    reichweite_sweep(1,0); reichweite_sweep(0,0); return 0; }
    lauf_t A, B, C, D;
    lauf(&A, "A  Messer, R1+RUNTER, DAUERSCHLAG", 0, budget, verbose);
    lauf(&B, "B  Messer, R1+RUNTER, KEIN Schlag", 1, budget, verbose);
    lauf(&C, "C  Messer, GAR NICHT gezielt",      2, budget, verbose);
    lauf(&D, "D  wie A, Zombie unverwundbar",     3, budget, verbose);

    printf("\n================ VERGLEICH ================\n");
    printf("%-36s | Bilder | Schlaege | Treffer | Bisse | d_min | <1200 | Griff | 1D3!=0\n", "Lauf");
    lauf_t *all[4] = { &A, &B, &C, &D };
    for (int i = 0; i < 4; i++) {
        lauf_t *r = all[i];
        printf("%-36s | %6d | %8d | %7d | %5d | %5d | %5d | %5d | %6d\n",
               r->name, r->bilder, r->schlaege, r->treffer, r->bisse, r->dmin,
               r->frames_dlt1200, r->grab_versuche, r->frames_1d3_nz);
    }

    /* ============================ RIEGEL (add_test) ==========================================
     * (1) REICHWEITE. Die RE1.5-Tester-Dispatch-Tabelle @0x8006E548 fuehrt GENAU die Ids 1/2
     *     auf den Nahkampf-KEGEL FUN_800127FC; dessen Treffer-Bedingung ist
     *     dist < Reichweite(@0x8006E5A0[1] = 1100) + Gegner-Radius (hbdata+6 = 400) = 1500.
     *     Am ALTEN Stand (Messer durch die RE2-Applier-Sub-Box) sind es 3000 (TIEF) bzw.
     *     3400 (EBEN) — der Riegel ist dort ROT.
     * (2) GEGEN-RIEGEL "nicht kaputtgemacht": das Messer muss weiterhin treffen (Sweep-Beginn
     *     bei 400) und die Reichweite darf nicht UNTER die RE1.5-Zahl fallen.
     * (3) GEGEN-RIEGEL "nicht ueberkorrigiert": der geschlagene Zombie darf NICHT haeufiger
     *     beissen als der unbehelligte (Bisse A <= Bisse B) — die Zombies duerfen nicht
     *     ploetzlich aggressiver werden als im Original.
     * (4) GEGEN-RIEGEL "nicht durch den Spieler": im Dauertreffer-Lauf D (kein Griff, also
     *     keine aec4-Ausnahme) muss der kleinste Abstand >= 850 bleiben — der byte-true
     *     Koerper-Standabstand Zombie 400 (STAGE1.BIN @file 0x1f778) + Spieler 450
     *     (PSX.EXE @file 0x64694 = 0x1c2), FUN_8002aec4 radSum @0x8002b164.
     * (5) Der Nicht-Schlag-Fall bleibt unveraendert: B muss weiterhin greifen und beissen. */
    int fehler = 0;
    int rw_t = reichweite_sweep(1, 1);        /* RE2-KI, TIEF  */
    int rw_e = reichweite_sweep(0, 1);        /* RE2-KI, EBEN  */
    int rw_5 = reichweite_sweep(1, 0);        /* RE1.5-KI, TIEF — die Sollzahl */
    printf("\nRIEGEL-ZAHLEN: Reichweite RE2/TIEF=%d RE2/EBEN=%d RE1.5/TIEF=%d"
           " | Bisse A=%d B=%d | Griffe A=%d B=%d | d_min D=%d\n",
           rw_t, rw_e, rw_5, A.bisse, B.bisse, A.grab_versuche, B.grab_versuche, D.dmin);
    if (rw_t > 1500 || rw_e > 1500) {
        printf("RIEGEL-ROT (1): Messer reicht %d/%d statt 1500 — die RE1.5-Kegel-Grenze"
               " (Reichweite 1100 @0x8006E5A0[1] + Radius 400) ist ausgehebelt\n", rw_t, rw_e);
        fehler = 1;
    }
    if (rw_t < 1400 || rw_e < 1400 || rw_5 < 1400) {
        printf("GEGEN-RIEGEL-ROT (2): Messer reicht nur noch %d/%d/%d — unter der"
               " RE1.5-Kegel-Grenze 1500\n", rw_t, rw_e, rw_5);
        fehler = 1;
    }
    if (A.bisse > B.bisse) {
        printf("GEGEN-RIEGEL-ROT (3): der geschlagene Zombie beisst OEFTER (A=%d) als der"
               " unbehelligte (B=%d) — Ueberkorrektur\n", A.bisse, B.bisse);
        fehler = 1;
    }
    if (D.dmin < 850) {
        printf("GEGEN-RIEGEL-ROT (4): d_min=%d < 850 — der Zombie laeuft in den Spieler"
               " hinein (Koerper-Standabstand 400+450, FUN_8002aec4 @0x8002b164)\n", D.dmin);
        fehler = 1;
    }
    if (B.grab_versuche < 1 || B.bisse < 1) {
        printf("FEHLLAUF (5): der GRUNDFALL B (kein Schlag) liefert weder Griff noch Biss"
               " (Griffe=%d Bisse=%d) — dann misst die Sonde nicht den Befund\n",
               B.grab_versuche, B.bisse);
        fehler = 1;
    }
    free(buf);
    if (fehler) return 1;
    printf("RIEGEL-GRUEN: Messer-Reichweite %d/%d (RE1.5-Soll 1500), Bisse A=%d <= B=%d,"
           " d_min(D)=%d >= 850\n", rw_t, rw_e, A.bisse, B.bisse, D.dmin);
    return 0;
}
