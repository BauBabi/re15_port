/**
 * @file test_re2z_death_getup.c
 * @brief REGRESSIONSWACHE fuer die zwei Nutzer-Befunde vom 2026-08-21 im RE2-KI-Modus:
 *
 *  (A) "Bei RE2 AI ist die Sterbeanimation nicht vollstaendig, friert ein vor dem kompletten Tod."
 *      URSACHE: der RE2-Advance hat KEIN "hold last frame". FUN_8002959C loest nur den
 *      Frame-Eintrag auf (`sw a2,376(a0)` @0x800295F8) und ruft FUN_80029614 @0x800295FC; DESSEN
 *      Schwanz ist der Frame-Zaehler (selbst disassembliert, info/re2leon/PSX.EXE):
 *        80029b28: lbu   v0,333(s2)      ; +0x14D
 *        80029b30: addiu v0,v0,1
 *        80029b34: sb    v0,333(s2)
 *        80029b3c: sltu  v0,v0,s3        ; s3 = Frame-Count (`lhu s3,0(v0)` @0x80029680)
 *        80029b40: bne   v0,zero,0x80029b50   ; noch drin -> return 0
 *        80029b48: sb    zero,333(s2)    ; **WRAP AUF 0**
 *        80029b4c: addiu v0,zero,1       ; return 1
 *      Phasen, die die Rueckgabe VERWERFEN, loopen ihren Clip also bis eine andere Bedingung
 *      greift. Eigener Scan aller 70 `jal 0x8002959c` in EMOVL10_S0.BIN: verworfen wird sie u.a.
 *      in death_MAGNUM P4 @0x801095D0, death_RIP P6 @0x80109288 und hit_MAIN P1/P2
 *      @0x80105790 / @0x801058C0. Der Port pinnte diese Clips auf fc-1 (play-once-Emulation in
 *      re15_actors_anim_advance) — GEMESSEN mit probe_re2z_deathgetup A (64 Seeds, Magnum):
 *        f806..f818 st=3 s1=5 s2=4 clip=2 af=53/54 kf=118 LOCO b8dy=-2735  (13 Frames UNBEWEGT)
 *      14 von 64 Laeufen, laengster Pin 22 Frames. Danach 0 von 64.
 *
 *  (B) "Nach dem Loesen des Raetsels im Generator-Raum steht der Zombie direkt neben Leon abrupt,
 *      quasi ohne Animation, direkt vom Liegen zum Stehen."  (Generatorraum = ROOM11F0)
 *      URSACHE: der D15.2-Skript-Wecker committete `0x901` = EXEC[9]. EXEC[9] @0x80103E48 ist
 *      BYTE-BELEGT KEIN Aufstehen vom Boden, sondern der STOSS-/TAUMEL-Executor:
 *        Clip-Tabelle @0x801000D8 = {3,3,4,0x0D}, Index `(+0x16B)*2 + (+0x16A)` @0x80103F14-20,
 *          +0x16A = frischer RNG-Wurf @0x80103EF0-F00 — alle drei Clips starten AUFRECHT.
 *        Schub `+0x144 = 400` @0x80103F60-64, Yaw-Offset `+0x158 = (+0x16B)<<11` @0x80103F78-80,
 *          Abbau `addiu v0,v0,-30` @0x80103FCC.
 *        Im Auslauf-Frame (+0x15A == 1 @0x80103FF0-F8) faellt er mit 7/8 HIN:
 *          `s = (r1 >> (r2&3)) & 7` @0x80104000-18, `s != 0 -> +0x4 = 0x501` @0x80104020-28.
 *      Der BODEN-Aufsteher ist EXEC[7]s eigene Kette: P1 haelt auf `+0x10E & 0x4000`
 *      (@0x8010381C-28), P2 setzt `re2z_param_clips[4+back]` = Clip 8/9 (@0x80103838-80),
 *      P3 spielt ihn aus (@0x801038D8-FC), P4 committet `0x101` (@0x80103900-0C).
 *      Der Bump darf also NUR den Limpet-Latch loesen (`andi v1,v1,0xbfff` / `sh v1,270(s0)`
 *      @0x80104F0C-10).
 *      GEMESSEN (probe_re2z_deathgetup B, ROOM11F0, die ECHTEN sub18-Bytes durch die ECHTE VM):
 *        vorher  f52 clip=23 Brust -150  ->  f53 s1=9 clip=3 Brust -2004   (EIN Frame)
 *        128/128 Aufsteh-Vorgaenge mit einem Brust-Sprung von 1400-1854 Einheiten je Frame;
 *        nachher 0/128, groesster Sprung 394, Aufsteh-Dauer 30 Frames.
 *
 * Die Pins hier brauchen die RE2-Bank EM010 (CDEMD0.EMS). Fehlt sie, endet der Test mit 77
 * (SKIP) — ohne Bank ist re15_actor_clip_len()==0 und JEDE clip-getriebene Aussage waere leer.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern void re15_actors_anim_advance(void);
extern void re15_re2z_rng_reset(void);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
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

/* Die Brusthoehe (Bone 8) relativ zum Aktor-Boden, aus DER Bank, aus der der Renderer posiert
 * (platform/pc/main.c -> re15_actor_uses_loco_bank -> re15_re2z_poses_loco_bank). */
static int chest_dy(const re15_actor_t *e, int32_t *out)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b) return 0;
    const re15_emd_skeleton_t  *sk = &b->skel;
    const re15_emd_animation_t *an = &b->anim;
    int clip_override = -1;
    if (re15_re2z_poses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
        sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion;
    }
    if (sk->bone_count <= 8 || an->clip_count <= 0 || sk->keyframe_count <= 0) return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t w[3];
    re15_skel_bone_to_world(poses[8].trans, e->rot_y, e->x, e->y, e->z, w);
    *out = w[1] - e->y;
    return 1;
}

/* ============================================================================================
 * PIN A — die verwerfenden Advance-Phasen LOOPEN ihren Clip (statt auf fc-1 zu pinnen)
 * ========================================================================================== */
static void pin_a_death_loop(void)
{
    re15_actor_init(); re15_enemy_ai_set_paused(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;

    struct { const char *name; uint8_t row, col, phase; const char *addr; } cases[] = {
        { "death_MAGNUM P4", 5, 1, 3, "@0x801095D0" },   /* Zeile 5 Spalte 1 -> RE2ZD_92C4 */
        { "death_RIP    P6", 8, 1, 5, "@0x80109288" },   /* Zeile 8 Spalte 1 -> RE2ZD_8BEC */
    };

    for (unsigned c = 0; c < sizeof cases / sizeof cases[0]; c++) {
        int s = re15_actor_alloc(0x10);
        if (s < 0) { CHECK(0, "kein Aktor-Slot"); return; }
        re15_actor_t *e = &g_actors[s];
        e->x = 4000; e->z = 0; e->y = 0; e->hp = -1;
        e->state = 3; e->sub_state_1 = cases[c].row; e->re2z_hits1d2 = cases[c].col;
        e->sub_state_2 = cases[c].phase;
        e->re2z_flags21a = 0; e->re2z_f10e = 0; e->re2z_rag231 = 0;
        e->re2z_walkclip = 2;                       /* Pair-1-Clip 2 = der 54-Frame-Gang */
        e->grid_id = 0;

        re15_re2z_tick(s);                          /* P3/P5 setzt den Clip und faellt in P4/P6 */

        int fc = re15_actor_clip_len(e);
        CHECK(fc > 1, "%s: Clip-Laenge muss aus der geladenen Bank kommen (fc=%d)",
              cases[c].name, fc);
        int had_loop = (e->anim_flags & 0x04u) ? 1 : 0;
        CHECK(had_loop,
              "%s: der Clip MUSS das LOOP-Bit tragen — der Advance %s verwirft seine Rueckgabe, "
              "das Original wrappt also (`sb zero,333(s2)` @0x80029B48). anim_flags=0x%04X",
              cases[c].name, cases[c].addr, e->anim_flags);

        /* Zwei volle Clip-Laengen abfahren: der Frame-Zaehler MUSS wrappen und darf nie zwei
         * Ticks am Stueck auf fc-1 stehen. Die Phase selbst haelt der +0x158-Timer offen. */
        int wrapped = 0, pin_run = 0, pin_max = 0, prev = (int)e->anim_frame;
        e->re2z_t158 = 4000;                        /* Timer weit offen halten (Mess-Fenster) */
        for (int t = 0; t < 2 * fc + 8; t++) {
            re15_actors_anim_advance();
            re15_re2z_tick(s);
            e->re2z_t158 = 4000;
            int af = (int)e->anim_frame;
            if (af < prev) wrapped = 1;
            if (fc > 1 && af == fc - 1) { pin_run++; if (pin_run > pin_max) pin_max = pin_run; }
            else pin_run = 0;
            prev = af;
        }
        CHECK(wrapped, "%s: der Frame-Zaehler muss wrappen (@0x80029B48), tat er in %d Ticks nicht",
              cases[c].name, 2 * fc + 8);
        CHECK(pin_max <= 1,
              "%s: kein PIN auf dem letzten Frame — gemessen %d Ticks am Stueck auf fc-1 (%d)",
              cases[c].name, pin_max, fc - 1);

        /* ---- NEGATIV-KONTROLLE: ohne das LOOP-Bit muss der Pin wieder erscheinen ---------- */
        e->anim_flags &= (uint16_t)~0x04u;
        e->anim_frame = 0;
        pin_run = 0; pin_max = 0;
        for (int t = 0; t < 2 * fc + 8; t++) {
            re15_actors_anim_advance();
            if (fc > 1 && (int)e->anim_frame == fc - 1) { pin_run++; if (pin_run > pin_max) pin_max = pin_run; }
            else pin_run = 0;
        }
        CHECK(pin_max > 1,
              "%s: NEGATIV-KONTROLLE unwirksam — ohne LOOP-Bit muesste der Advancer pinnen "
              "(gemessen %d)", cases[c].name, pin_max);
        printf("  [A] %s: clip=%d fc=%d LOOP=%d wrap=%d | NEGATIV-KONTROLLE (LOOP aus): "
               "Pin %d Ticks\n", cases[c].name, (int)e->motion, fc, had_loop, wrapped, pin_max);
        re15_actor_free(s);
    }
}

/* ============================================================================================
 * PIN B — der Skript-Wecker fuehrt ueber den BODEN-Aufsteher, nicht ueber EXEC[9]
 * ========================================================================================== */
static void pin_b_script_wake(void)
{
    re15_actor_init(); re15_enemy_ai_set_paused(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;

    /* ---- B1 DATEN-DISKRIMINATOR: welche Clips starten am Boden? ---------------------------
     * Genau dieser Vergleich hat den Befund entschieden. Er ist zugleich die Wache dagegen,
     * dass jemand den Wecker wieder auf EXEC[9] legt. */
    {
        int s = re15_actor_alloc(0x10);
        re15_actor_t *q = &g_actors[s];
        q->x = q->y = q->z = 0; q->rot_y = 0; q->anim_frame = 0; q->anim_freeze = 0;
        q->state = 1; q->sub_state_1 = 7; q->sub_state_2 = 0;      /* -> Aktions-Bank */
        const int getup9[3] = { 3, 4, 13 };                        /* @0x801000D8 {3,3,4,0x0D} */
        for (int i = 0; i < 3; i++) {
            int32_t dy = 0; q->motion = (int16_t)getup9[i];
            CHECK(chest_dy(q, &dy), "Bank-Pose fuer Clip %d nicht berechenbar", getup9[i]);
            printf("  [B1] EXEC[9]-Clip %2d: Brust in Frame 0 = %d\n", getup9[i], (int)dy);
            CHECK(dy < -1500,
                  "EXEC[9]-Clip %d muesste AUFRECHT starten (Tabelle @0x801000D8) — Brust %d",
                  getup9[i], (int)dy);
        }
        const int ground[2] = { 8, 9 };                            /* re2z_param_clips[4+back] */
        for (int i = 0; i < 2; i++) {
            int32_t dy = 0; q->motion = (int16_t)ground[i];
            CHECK(chest_dy(q, &dy), "Bank-Pose fuer Clip %d nicht berechenbar", ground[i]);
            printf("  [B1] EXEC[7]-P2-Clip %d: Brust in Frame 0 = %d\n", ground[i], (int)dy);
            CHECK(dy > -700,
                  "der Boden-Aufsteher %d (@0x80103840-80) muss AM BODEN starten — Brust %d",
                  ground[i], (int)dy);
        }
        re15_actor_free(s);
    }

    /* ---- B2 ABLAUF: Liegender + Nibble-Bump -> EXEC[7] P2/P3/P4, KEINE EXEC[9], und die
     * Brust darf nie um mehr als 500 Einheiten je Frame springen. ---------------------------- */
    {
        int s = re15_actor_alloc(0x16);
        re15_actor_t *e = &g_actors[s];
        e->x = 4000; e->z = 0; e->y = 0; e->rot_y = 0; e->hp = 60;
        e->state = 1; e->sub_state_1 = 7; e->sub_state_2 = 0;
        e->grid_id = 0x88; e->re2z_f10e = 0x4002; e->ai_dist = 60000;
        re15_re2z_tick(s); re15_actors_anim_advance();             /* P0 */
        re15_re2z_tick(s); re15_actors_anim_advance();             /* P1 haelt */
        CHECK(e->sub_state_1 == 7 && e->sub_state_2 == 1,
              "Liegender muss in EXEC[7] P1 haengen (Limpet @0x8010381C-28)");

        int32_t dy_prev = 0, dy = 0, maxjump = 0;
        CHECK(chest_dy(e, &dy_prev), "Pose des Liegenden nicht berechenbar");
        CHECK(dy_prev > -700, "der Liegende muss AM BODEN sein — Brust %d", (int)dy_prev);

        e->grid_id = 0x8A;                        /* Member_set(12,0x8A) `sb a2,9(a0)` @0x800411f8 */
        int saw9 = 0, saw_ground = 0, stood = 0, rise_frames = 0;
        for (int t = 0; t < 400; t++) {
            re15_actors_anim_advance();
            re15_re2z_tick(s);
            if (e->state == 1 && e->sub_state_1 == 9) saw9 = 1;
            if (e->state == 1 && e->sub_state_1 == 7 && e->sub_state_2 >= 2 &&
                (e->motion == 8 || e->motion == 9)) { saw_ground = 1; rise_frames++; }
            if (e->state == 1 && e->sub_state_1 == 1) stood = 1;
            if (chest_dy(e, &dy)) {
                int32_t d = dy - dy_prev; if (d < 0) d = -d;
                if (d > maxjump) maxjump = d;
                dy_prev = dy;
            }
            if (stood) break;
        }
        printf("  [B2] Bodenclip=%d (%d Frames), EXEC[9]=%d, steht=%d, groesster Brust-Sprung "
               "je Frame = %d\n", saw_ground, rise_frames, saw9, stood, (int)maxjump);
        CHECK(!saw9,
              "der Skript-Wecker darf NICHT nach EXEC[9] committen — das ist der Stoss-/Taumel-"
              "Executor @0x80103E48 (Clips {3,3,4,0x0D} @0x801000D8 starten aufrecht)");
        CHECK(saw_ground,
              "EXEC[7] P2 muss den Boden-Aufsteher 8/9 spielen (@0x80103838-80)");
        CHECK(rise_frames >= 20,
              "der Aufsteh-Clip muss SICHTBAR ablaufen (Clip 8/9 = 80 Frames) — gemessen %d",
              rise_frames);
        CHECK(stood, "und P4 muss `0x101` committen (@0x80103900-0C)");
        CHECK(maxjump < 500,
              "die Brust darf beim Aufstehen nicht springen — gemessen %d Einheiten in EINEM "
              "Frame (der gemeldete Fehler mass 1854)", (int)maxjump);
        re15_actor_free(s);
    }
}

int main(void)
{
    printf("== RE2-Zombie: Sterbeanimation + Skript-Wecker ==\n");
    if (!load_bank2(0x10) || !load_bank2(0x16)) {
        printf("SKIP: RE2-Bank CDEMD0.EMS nicht ladbar — ohne Bank ist jede Clip-Aussage leer\n");
        return 77;
    }
    pin_a_death_loop();
    pin_b_script_wake();
    if (fails) { printf("test_re2z_death_getup: %d FAILURES\n", fails); return 1; }
    printf("test_re2z_death_getup: OK\n");
    return 0;
}
