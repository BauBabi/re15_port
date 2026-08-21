/**
 * @file test_re2_zombie_abc.c
 * @brief PIN ueber einen RNG-SEED-SWEEP — die drei Nutzer-Symptome aus v0.3.5 (RE2-Modus):
 *   (A) "Die Zombies fallen manchmal komisch um und stehen dann direkt wieder auf."
 *   (B) "Manchmal haben sie eine Laufanimation, aber laufen nicht."
 *   (C) "Es kommt vor, dass die Zombies unsterblich sind."
 *
 * ⛔ WARUM SWEEP UND NICHT EINZELLAUF: alle drei sind "manchmal". Ein Einzellauf faengt sie
 * nicht — genau daran ist in dieser Kampagne schon ein als fertig gemeldeter Fix gescheitert.
 * Gefahren wird deshalb der ECHTE Weg (re15_game_step + Pad, echte ROOM1140-Sce_em_set-Spawns,
 * GELADENE RE2-Bank EM010 — ohne Bank ist re15_actor_clip_len() == 0 und jede clip-getriebene
 * Phase endet im Setz-Tick, die Messung waere wertlos) ueber viele Seeds. "Seed" = Vorwuerfe auf
 * dem EINEN 16-Bit-PRNG-State des RE2-Overlays (@0x800CE318, re2z_rand/@0x80015FE8).
 *
 * GEMEINSAME WURZEL DER DREI SYMPTOME — EXEC[11] @0x8010439C:
 *   Der Port hatte dort `if (clip_done) set_state_word(0x101)` mit dem Vermerk
 *   "ENTRY OPEN (no port producer) / Exit MAPPED". Beides falsch:
 *     - 0xB01 wird an DREI Stellen committet (eigener Byte-Scan `addiu/ori rt,zero,0xB01`
 *       im Overlay: 0x80105ACC, 0x80106098, 0x80108224),
 *     - und das Original geht NIE nach 0x101, sondern in P2 nach
 *       **+0x4 = 0x00020501** (`lui v0,0x2` @0x801043EC im Delay-Slot + `ori v0,v0,0x501`
 *       @0x801045D4 + `sw v0,4(s0)` @0x801045DC) = EXEC[5] PHASE 2, die Bodenkette.
 *   P0 setzt +0x1D3 |= 0x80 (@0x8010444C-5C) und +0x10E |= 0x2000 (@0x80104460-70); beide werden
 *   NUR in der Bodenkette wieder geloescht (P2 @0x80103484-90, P8 @0x80103718-28 /
 *   @0x8010373C-4C). Der erfundene 0x101-Ausgang fuehrte daran vorbei -> der Zombie lief mit
 *   gesetztem +0x1D3 weiter und war hinter Gate (2) des Trefferfilters (@0x80047138-40) fuer
 *   immer untreffbar (C), er stand ohne Bodenphase sofort wieder (A), und der ~50-Frame-Clip 0x0A
 *   lief ohne eigene Fortbewegung (B).
 *
 * GEMESSEN (probe_re2z_abc, 64 Seeds x 900 Frames, Pistole/Messer/Waffe 4):
 *   VORHER : (C) 121-151 Vorfaelle in 64/64 Seeds; DOWN-Dauer immer 120..131 (drei Werte)
 *   NACHHER: (C) 0 in 0/64 Seeds; DOWN-Dauer 86..323 (die Liegezeit-Tabelle @0x8010004C)
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
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern int      re15_actor_clip_len(const re15_actor_t *a);
extern uint32_t re15_re2_rand(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

/* ---- Sweep-Ergebnis --------------------------------------------------------------------- */
typedef struct {
    int  blocked_incidents;   /* (C): hp >= 0 und Filter sperrt >= cwin Frames am Stueck */
    int  blocked_seeds;
    int  standup_incidents;   /* (A): DOWN endet aufrecht nach < amin Frames */
    int  down_n, down_min, down_max;
    int  down_saw_p2, down_saw_p3;   /* DOWN-Fenster, die den Aufschlag/die Zuckschleife sahen */
    long hits, deaths;
    long walk_frames, walk_path;
    int  banks_ok;
} sweep_t;

static void run_sweep(sweep_t *out, int seeds, int budget, int weapon, int cwin, int amin)
{
    memset(out, 0, sizeof *out);
    out->down_min = 1 << 30; out->down_max = -1;

    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
        re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
        g_current_room_id = 0x1140;
        if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
        if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
        g_scd.work_vars[10] = 0;
        for (int i = 0; i < 120; i++) scd_vm_tick();

        out->banks_ok = load_bank2(0x10) && load_bank2(0x11) && load_bank2(0x16);

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
        pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
        re15_collision_set_band(0);
        re15_player_set_aim_clip_len(12);
        re15_inv_load_briefing();
        re15_player_set_equipped_weapon(weapon);
        {   int es = re15_inv_equipped_slot();
            if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();   /* der SEED */

        {   int64_t sx = 0, szz = 0; int n = 0;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (is_zombie(&g_actors[s])) { sx += g_actors[s].x; szz += g_actors[s].z; n++; }
            if (n) { pl->x = (int32_t)(sx / n) + 2200 + (seed % 7) * 300;
                     pl->z = (int32_t)(szz / n) + 1500 - (seed % 5) * 400; }
            pl->rot_y = (int16_t)((seed * 337) & 0xfff);
        }
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

        int down_start[RE15_ACTOR_MAX], block_start[RE15_ACTOR_MAX];
        int saw_p2[RE15_ACTOR_MAX], saw_p3[RE15_ACTOR_MAX];
        int prev_st[RE15_ACTOR_MAX], prev_s1[RE15_ACTOR_MAX], hp_prev[RE15_ACTOR_MAX];
        int32_t px[RE15_ACTOR_MAX], pz[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) {
            down_start[s] = block_start[s] = -1; prev_st[s] = prev_s1[s] = -1;
            saw_p2[s] = saw_p3[s] = 0;
            hp_prev[s] = g_actors[s].hp; px[s] = g_actors[s].x; pz[s] = g_actors[s].z;
        }
        int seed_blocked = 0;
        int shot_period = 7 + (seed % 11), shot_first = 20 + (seed % 17);

        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (f >= shot_first && ((f - shot_first) % shot_period) == 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);

            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!is_zombie(e)) { down_start[s] = block_start[s] = -1; continue; }
                if (e->hp < hp_prev[s]) out->hits++;
                if (e->hp < 0 && hp_prev[s] >= 0) out->deaths++;
                hp_prev[s] = e->hp;

                /* (A) DOWN = der Sturz-Executor EXEC[5] */
                int down = (e->state == 1 && e->sub_state_1 == 5);
                if (down && down_start[s] < 0) { down_start[s] = f; saw_p2[s] = saw_p3[s] = 0; }
                if (down) { if (e->sub_state_2 == 2) saw_p2[s] = 1;
                            if (e->sub_state_2 == 3) saw_p3[s] = 1; }
                if (!down && down_start[s] >= 0) {
                    int dur = f - down_start[s];
                    int upright = (e->state == 1 && (e->sub_state_1 == 0 || e->sub_state_1 == 1));
                    if (e->hp >= 0) {
                        out->down_n++;
                        if (dur < out->down_min) out->down_min = dur;
                        if (dur > out->down_max) out->down_max = dur;
                        if (upright && dur < amin) out->standup_incidents++;
                        if (saw_p2[s]) out->down_saw_p2++;
                        if (saw_p3[s]) out->down_saw_p3++;
                    }
                    down_start[s] = -1;
                }

                /* (B) WEGLAENGE im WALK (Diagnose, kein Pin — siehe unten) */
                if (e->state == 1 && e->sub_state_1 == 1 && prev_st[s] == 1 && prev_s1[s] == 1) {
                    int32_t dx = e->x - px[s], dz = e->z - pz[s];
                    out->walk_frames++;
                    out->walk_path += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                }

                /* DIAGNOSE: der erste Tick, in dem ein KRIECHER (+0x10E&1) einen Substate
                 * ausserhalb seiner 3-Wort-Tabelle @0x8010C918 traegt. Genau dort wuerde das
                 * Original `jalr` auf ein Datenwort machen — der Zustand darf nicht entstehen. */
                if ((e->re2z_f10e & 1u) && e->sub_state_1 > 2 && e->state == 1) {
                    static int shown = 0;
                    if (shown < 8) {
                        shown++;
                        printf("     [CRAWL-OOB] seed=%d slot=%d f=%d s1=%u s2=%u 10E=0x%04x "
                               "vorher st=%d s1=%d\n", seed, s, f, e->sub_state_1,
                               e->sub_state_2, e->re2z_f10e, prev_st[s], prev_s1[s]);
                    }
                }

                /* (C) Kandidatenfilter sperrt, obwohl der Zombie lebt */
                int blocked = (e->hp >= 0) && (e->hit_react & 1u);
                if (!blocked) block_start[s] = -1;
                else {
                    if (block_start[s] < 0) block_start[s] = f;
                    if (f - block_start[s] == cwin) {
                        out->blocked_incidents++; seed_blocked = 1;
                        /* Diagnose beim Vorfall: OHNE den Zustand ist "unsterblich" nicht
                         * zuzuordnen (der Latch hat vier moegliche Quellen). */
                        printf("     [C-VORFALL] seed=%d slot=%d f=%d st=%u s1=%u s2=%u hp=%d "
                               "1D3=0x%02x 10E=0x%04x clip=%d\n", seed, s, f, e->state,
                               e->sub_state_1, e->sub_state_2, (int)e->hp, e->re2z_self1d3,
                               e->re2z_f10e, (int)e->motion);
                    }
                }
                prev_st[s] = e->state; prev_s1[s] = e->sub_state_1;
                px[s] = e->x; pz[s] = e->z;
            }
        }
        if (seed_blocked) out->blocked_seeds++;
    }
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    const int SEEDS = 64, FRAMES = 900, CWIN = 200, AMIN = 30;

    /* ================= (1) PISTOLE — der Fall aus dem Nutzer-Report ======================= */
    sweep_t p;
    run_sweep(&p, SEEDS, FRAMES, 3, CWIN, AMIN);

    /* Vorbedingung: ohne geladene Bank ist die ganze Messung wertlos (clip_len == 0). */
    CHECK(p.banks_ok, "VORBEDINGUNG: RE2-Baenke EM010/EM011/EM016 muessen geladen sein");
    /* Positiv-Kontrolle: der Sweep muss ueberhaupt Kampf gesehen haben, sonst waere jede
     * Null-Aussage darunter gruen, weil nichts passiert ist. */
    CHECK(p.hits   > 500, "POSITIV-KONTROLLE: der Sweep muss treffen, got %ld Treffer", p.hits);
    CHECK(p.deaths > 20,  "POSITIV-KONTROLLE: es muessen Zombies STERBEN, got %ld", p.deaths);
    CHECK(p.down_n > 50,  "POSITIV-KONTROLLE: die Sturzkette muss laufen, got %d DOWN-Fenster",
          p.down_n);

    /* ---- (C) DER BLOCKER: kein lebender Zombie darf dauerhaft untreffbar sein ------------ */
    CHECK(p.blocked_incidents == 0,
          "(C) UNSTERBLICH: %d Vorfaelle in %d/%d Seeds — ein Zombie mit hp >= 0 war >= %d Frames "
          "am Stueck vom Kandidatenfilter gesperrt (FUN_800470C0 @0x80047124-64). Wurzel war der "
          "erfundene EXEC[11]-Ausgang 0x101 statt 0x00020501 (@0x801043EC/@0x801045D4/@0x801045DC)",
          p.blocked_incidents, p.blocked_seeds, SEEDS, CWIN);

    /* ---- (A) kein Sturz darf nach wenigen Frames im aufrechten Gang enden ---------------- */
    CHECK(p.standup_incidents == 0,
          "(A) FALLEN UND SOFORT AUFSTEHEN: %d Vorfaelle — ein DOWN-Fenster endete nach < %d "
          "Frames aufrecht", p.standup_incidents, AMIN);
    /* Und die Bodenzeit muss ECHT und ZUFAELLIG sein: +0x15A = u16tbl@0x8010004C[…] + (r3&0xf)
     * (@0x80103428-88), Tabelle = {90,30,170,160,10,40,100,50,110,20,35,70,30,180,30,60}.
     * Vor dem Fix sprang der Port P1 -> P6 und lieferte konstant 120..131 (drei Werte). */
    CHECK(p.down_min >= 60,
          "(A) die Bodenzeit muss die Tabelle @0x8010004C tragen, kuerzestes DOWN = %d",
          p.down_min);
    CHECK(p.down_max >= 200,
          "(A) die Bodenzeit muss STREUEN (Tabelle bis 180 + (rand&0xf)), laengstes DOWN = %d",
          p.down_max);
    CHECK(p.down_max - p.down_min >= 100,
          "(A) Streuung der Bodenzeit zu klein (%d..%d) — das war die Signatur des alten "
          "P1 -> P6-Kurzschlusses (konstant 120..131)", p.down_min, p.down_max);
    /* ⛔ UND DIE STRUKTUR, nicht nur die Dauer: der Port sprang frueher von P1 DIREKT auf P6 und
     * uebersprang damit den AUFSCHLAG (P2 @0x80103404, `sb 3,6` @0x8010340C) und die
     * Zuck-Schleife P3/P4/P5 (@0x801034DC / @0x80103510 / @0x8010352C). Das Original zaehlt in P1
     * nur `+0x6 += Advance-Rueckgabe` (@0x80103384-98) und landet damit auf PHASE 2.
     * Eine reine Dauer-Zusicherung faengt das NICHT: der EXEC[11]-Weg steigt ohnehin bei Phase 2
     * ein (0x00020501) und liefert die lange Bodenzeit auch dann, wenn der Flinch-Weg noch
     * abkuerzt. Deshalb wird hier gepinnt, dass die Sturzfenster die Phasen wirklich SEHEN. */
    CHECK(p.down_saw_p2 * 4 >= p.down_n * 3,
          "(A) STRUKTUR: nur %d von %d Sturzfenstern haben PHASE 2 (den Aufschlag @0x80103404) "
          "gesehen — P1 muss nach +0x6 += ret auf Phase 2 gehen (@0x80103384-98), nicht auf 6",
          p.down_saw_p2, p.down_n);
    CHECK(p.down_saw_p3 * 4 >= p.down_n * 3,
          "(A) STRUKTUR: nur %d von %d Sturzfenstern haben PHASE 3 (die Zuck-Schleife "
          "@0x801034DC) gesehen", p.down_saw_p3, p.down_n);

    /* ---- (B) DIAGNOSE, bewusst KEIN Pin --------------------------------------------------
     * Der Rest der "Laufanimation ohne Weg"-Fenster hat eine WEGLAENGE von 107..851 Einheiten
     * pro 60 Frames bei kleinem NETTO — der Zombie laeuft also, kommt aber nicht vom Fleck
     * (Kollision/Kreisen im Navigator). Das liegt ausserhalb dieser Datei; gepinnt wird hier
     * nur, dass der WALK im Mittel ueberhaupt Strecke macht. */
    {   double per = p.walk_frames ? (double)p.walk_path / (double)p.walk_frames : 0.0;
        CHECK(per > 15.0,
              "(B) der WALK muss im Mittel Strecke machen, got %.2f Einheiten/Frame ueber "
              "%ld Frames", per, p.walk_frames);
    }

    /* ================= (2) MESSER — die dichte Trefferfolge =============================== */
    sweep_t k;
    run_sweep(&k, SEEDS, FRAMES, 1, CWIN, AMIN);
    CHECK(k.hits > 1000, "POSITIV-KONTROLLE Messer: got %ld Treffer", k.hits);
    CHECK(k.blocked_incidents == 0,
          "(C) UNSTERBLICH (Messer): %d Vorfaelle in %d/%d Seeds",
          k.blocked_incidents, k.blocked_seeds, SEEDS);
    CHECK(k.standup_incidents == 0,
          "(A) FALLEN UND SOFORT AUFSTEHEN (Messer): %d Vorfaelle", k.standup_incidents);

    /* ================= (3) NEGATIV-KONTROLLE: der Filter ist NICHT abgeschaltet ==========
     * Die Sperren muessen weiterhin greifen, sonst waere (C) trivial gruen. Geprueft an einem
     * Aktor ausserhalb der Spawn-Pose mit HP < 0 (Gate 3 @0x80047148-50) und mit +0x10E & 0x8000
     * (Gate 4 @0x80047158-64). */
    {   re15_actor_init(); re15_enemy_reset(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->active = 1; e->type = 0x10; e->state = 1; e->sub_state_1 = 1;
        e->hp = -1; e->hit_react = 0; e->re2z_self1d3 = 0; e->re2z_f10e = 0;
        re15_re2z_tick(s);
        CHECK((e->hit_react & 1u) != 0,
              "NEGATIV: eine LEICHE muss weiter gesperrt sein (Gate 3 `bltz` @0x80047150), "
              "+0x93 = 0x%02X", e->hit_react);
        e->hp = 50; e->re2z_f10e = 0x8000u; e->hit_react = 0;
        re15_re2z_tick(s);
        CHECK((e->hit_react & 1u) != 0,
              "NEGATIV: +0x10E & 0x8000 muss weiter sperren (Gate 4 `andi 0xc000` @0x80047160), "
              "+0x93 = 0x%02X", e->hit_react);
        e->hp = 50; e->re2z_f10e = 0; e->re2z_self1d3 = 0x80u; e->hit_react = 0;
        re15_re2z_tick(s);
        CHECK((e->hit_react & 1u) != 0,
              "NEGATIV: ein stehengebliebener Reaktions-Claim +0x1D3 = 0x80 OHNE 0x4000 muss "
              "weiter sperren (Gate 2 @0x80047140) — sonst faenge der Filter den EXEC[11]-Fall "
              "nicht mehr, +0x93 = 0x%02X", e->hit_react);
        re15_actor_free(s);
    }

    /* ================= (4) RE1.5-REGRESSIONSWACHE ========================================
     * Im RE1.5-Flavor darf sich NICHTS aus alledem zeigen: re15_re2z_tick laeuft dort nicht. */
    {   re15_actor_init(); re15_enemy_reset(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        int s = re15_actor_alloc(0x10);
        re15_actor_t *e = &g_actors[s];
        e->active = 1; e->type = 0x10; e->state = 1; e->sub_state_1 = 1; e->hp = 50;
        e->hit_react = 0x55u; e->re2z_self1d3 = 0x80u; e->re2z_f10e = 0x4000u;
        uint8_t before = e->hit_react;
        re15_enemy_ai_run_all(1);
        CHECK(e->hit_react == before || (e->hit_react & 1u) == (before & 1u),
              "RE1.5-WACHE: der RE2-Trefferfilter darf im RE1.5-Flavor +0x93 Bit 0 nicht "
              "umschreiben (vorher 0x%02X, nachher 0x%02X)", before, e->hit_react);
        CHECK(e->re2z_f10e == 0x4000u,
              "RE1.5-WACHE: die Latch-Aufraeumung ist im RE1.5-Flavor tot, got 10E = 0x%04X",
              e->re2z_f10e);
        re15_actor_free(s);
    }

    free(buf);
    if (fails) { printf("test_re2_zombie_abc: %d FAILURES\n", fails); return 1; }
    printf("test_re2_zombie_abc: OK  (Pistole: %ld Treffer / %ld Tode / DOWN %d..%d ueber %d "
           "Fenster; Messer: %ld Treffer)\n",
           p.hits, p.deaths, p.down_min, p.down_max, p.down_n, k.hits);
    return 0;
}
