/**
 * @file test_re2_hit_reaction.c
 * @brief PIN — die RE2-Trefferreaktion: Oberkoerper-ZUCKEN + die beiden fehlenden Handler.
 *
 * Anlass (Nutzer, nach f91a4d7c): "Reaktion + Grunzer sind da, aber das feine Zucken beim
 * Treffer fehlt komplett." Dazu die dort als OPEN notierten Handler 0x801066FC (Ragdoll-Sturz)
 * und 0x8010703C (Rutsch-Rueckstoss).
 *
 * Gemessen wird, was der Nutzer wahrnimmt — die GERENDERTE Pose, die Tonfolge und die Position:
 *   (1) ZUCKEN  : waehrend der Reaktionsphasen P1/P2 des Haupt-Handlers 0x80105438 muss sich die
 *                 gerenderte Bone-0-Matrix (und damit die Welt-Position der oberen Bones) von
 *                 derselben Pose OHNE Injektion unterscheiden. Winkel-Belege:
 *                 P1 @0x801057A4-E8 (-((+0x158*(s8)+0x16B)<<3)*+0x15A), P2 @0x801058D4-960
 *                 (dasselbe ohne <<3), Gegenmatrix auf Bone 1 @0x801057EC-838 / @0x80105918-960.
 *                 Gegenprobe: bei +0x158 == 0 (Phase-1-Eintritt) ist der Winkel 0 -> KEINE
 *                 Abweichung; das trennt "Injektion wirkt" von "Test misst Rauschen".
 *   (2) RAGDOLL : Zeile 8 / Spalte 1 dispatcht 0x801066FC (Tabelle @0x8010C940). SE 12|13 und
 *                 SE 9 im ersten Frame (@0x80106960-90), Sturz-Clip 4 (@0x80106888), Bounce ab
 *                 +0x14D >= 20 mit SE 4 (@0x80106F0C), Ausgang als KRIECHER (+0x10E = 0x2001,
 *                 @0x80106B0C-10) im Zustand 1.
 *   (3) RUTSCH  : Zeile 12 / Spalte 1 dispatcht 0x8010703C. Schub 300 (@0x8010712C) mit Abbremsung
 *                 (+0x144 -= +0x15A, +0x15A += +0x16B @0x801071F0-228), Bone-0-Blend in P3/P5
 *                 (FUN_80028F48 @0x801072F8/@0x801073C0) und Ausgang 0x101/0x201 (@0x80107408-18).
 *   (4) ERREICHBARKEIT: der Schrotflinten-Schuss (Waffe 8) jenseits von 3000 landet wirklich in
 *                 Zeile 8 -> der Ragdoll ist im Spiel erreichbar, nicht nur synthetisch.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_anim_select.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_re2z_last_hit_handler(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}

/* ---- SE-Mitschnitt (derselbe Hook, den platform/pc/main.c registriert) ---- */
static int s_se[256], s_se_n;
static void probe_se(int id, int f2) { (void)f2; if (s_se_n < 256) s_se[s_se_n++] = id; }
static int  se_seen(int id) { for (int i = 0; i < s_se_n; i++) if (s_se[i] == id) return 1; return 0; }
static int  se_count(int id) { int n = 0; for (int i = 0; i < s_se_n; i++) if (s_se[i] == id) n++; return n; }

/* ---- die RE2-Bank laden (pc_enemy_load-Spiegel, RE2-Zweig) ---- */
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb) return eb;
    eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

/* Der Keyframe-Index, den der Renderer fuer den laufenden Clip zieht (dieselbe Rechnung wie
 * re2z_root_py in enemy_ai_re2_zombie.c). */
static int actor_kf(const re15_actor_t *e, const re15_enemy_bank_t *b)
{
    int clip = (int)e->motion;
    if (clip < 0 || clip >= b->anim.clip_count) return 0;
    int fc = b->anim.clips[clip].frame_count;
    if (fc <= 0) return 0;
    int fi = b->anim.clips[clip].first_frame + (int)(e->anim_frame % (uint32_t)fc);
    if (fi < 0 || fi >= b->anim.frame_count) return 0;
    return (int)(b->anim.frames[fi] & 0xfffu);
}

/* Die gerenderte Pose EINMAL mit und EINMAL ohne Injektion; liefert die groesste absolute
 * Abweichung in der Bone-0-Matrix und die Verschiebung des obersten Bones. */
typedef struct { int rot_delta; int top_shift; } twitch_t;
static twitch_t measure_twitch(re15_actor_t *e, re15_enemy_bank_t *b)
{
    twitch_t out = { 0, 0 };
    re15_skel_pose_t on[RE15_EMD_MAX_BONES], off[RE15_EMD_MAX_BONES];
    int kf = actor_kf(e, b);
    uint8_t save_on = e->re2_lean_on;
    int16_t save_w  = e->re2_bone0_wgt;

    /* Die Injektion haengt am AKTOR (g_anim_pose_actor), also MUSS er fuer beide Rechnungen
     * gesetzt sein. re15_skel_compute_pose SCHREIBT dabei den Crossfade-Zustand
     * (prev_angles/prev_root/anim_prev_valid) — vor der zweiten Rechnung wird er deshalb auf
     * den Stand vor der ersten zurueckgespult, sonst misst der Vergleich den Crossfade statt
     * der Injektion. */
    int16_t sv_ang[32][3]; int32_t sv_root[3]; uint8_t sv_valid;
    memcpy(sv_ang, e->prev_angles, sizeof sv_ang);
    memcpy(sv_root, e->prev_root, sizeof sv_root);
    sv_valid = e->anim_prev_valid;

    g_anim_pose_actor = e;
    if (re15_skel_compute_pose(&b->skel, kf, on) != 0) { g_anim_pose_actor = NULL; return out; }
    memcpy(e->prev_angles, sv_ang, sizeof sv_ang);
    memcpy(e->prev_root, sv_root, sizeof sv_root);
    e->anim_prev_valid = sv_valid;
    e->re2_lean_on = 0; e->re2_bone0_wgt = 0;
    int rc = re15_skel_compute_pose(&b->skel, kf, off);
    e->re2_lean_on = save_on; e->re2_bone0_wgt = save_w;
    g_anim_pose_actor = NULL;
    if (rc != 0) return out;

    for (int k = 0; k < 9; k++) {
        int d = on[0].rot[k] - off[0].rot[k];
        if (d < 0) d = -d;
        if (d > out.rot_delta) out.rot_delta = d;
    }
    int top = b->skel.bone_count - 1;
    for (int k = 0; k < 3; k++) {
        int d = on[top].trans[k] - off[top].trans[k];
        if (d < 0) d = -d;
        if (d > out.top_shift) out.top_shift = d;
    }
    return out;
}

/* ---- Raum-Bringup (ROOM1140, RE2-Flavor) ---- */
static re15_actor_t *bringup_zombie(uint8_t **buf_out, re15_enemy_bank_t **bank_out)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf) { CHECK(0, "ROOM1140.RDT fehlt"); return NULL; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { CHECK(0, "RDT-Parse"); free(buf); return NULL; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int first = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) { first = s; break; }
    if (first < 0) { CHECK(0, "kein Zombie 0x10 in ROOM1140"); free(buf); return NULL; }
    re15_enemy_bank_t *b = load_re2_bank(0x10);
    CHECK(b != NULL && b->skel.bone_count > 1,
          "RE2-Zombie-Bank fehlt (shared_assets/RE2/CDEMD0.EMS) — ohne sie ist die Pose nicht messbar");
    *buf_out = buf; *bank_out = b;
    return &g_actors[first];
}

/* Setzt den Gegner frisch als lebenden, gehenden Zombie neben den Spieler. */
static void reset_zombie(re15_actor_t *e, uint8_t row)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->state = 2; e->sub_state_1 = row; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = 60; e->hit_react = 0;
    e->re2z_res223 = 20; e->re2z_flag222 = 0; e->re2z_cd239 = 0;
    e->re2z_hits1d2 = 1;                       /* Basis-Zone 1 @0x80047294-98 */
    e->re2z_flags21a = 0; e->re2z_f10e = 0;
    e->re2z_t158 = 0; e->re2z_t15a = 0; e->re2z_dir16a = 0; e->re2z_gaitrow = 0;
    e->re2z_rag231 = 0; e->re2z_gy232 = 0;
    e->anim_frame = 0; e->anim_frac = 0;
    e->re2_lean[0] = e->re2_lean[1] = e->re2_lean[2] = 0;
    e->re2_lean_on = 0; e->re2_bone0_wgt = 0;
    pl->x = e->x - 800; pl->z = e->z; pl->rot_y = 0;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* ======================================================================================= */
int main(void)
{
    re15_re2z_audio_hook(probe_se, NULL);
    uint8_t *buf = NULL; re15_enemy_bank_t *bank = NULL;
    re15_actor_t *e = bringup_zombie(&buf, &bank);
    if (!e || !bank) { printf("test_re2_hit_reaction: %d FAIL\n", fails ? fails : 1); return 1; }

    printf("== Bank EM010: %d Bones, %d Clips; Bone-Eltern:", bank->skel.bone_count,
           bank->anim.clip_count);
    for (int i = 0; i < bank->skel.bone_count && i < 8; i++)
        printf(" %d<-%d", i, (int)bank->skel.bone_parent[i]);
    printf("\n   Clip 4: %d Frames, Clip 5: %d Frames\n",
           bank->anim.clip_count > 4 ? bank->anim.clips[4].frame_count : -1,
           bank->anim.clip_count > 5 ? bank->anim.clips[5].frame_count : -1);

    /* ---------------- (1) das OBERKOERPER-ZUCKEN ---------------- */
    {
        reset_zombie(e, 3);                    /* Zeile 3 -> Haupt-Handler 0x80105438 */
        s_se_n = 0;
        int max_rot = 0, max_shift = 0, twitch_frames = 0, phase1_zero_seen = 0;
        int lean_angles[40]; int na = 0;
        for (int f = 0; f < 40; f++) {
            frame();
            twitch_t t = measure_twitch(e, bank);
            if (na < 40) lean_angles[na++] = (int)e->re2_lean[2];
            if (e->re2_lean_on && e->re2_lean[2] == 0 && t.rot_delta == 0) phase1_zero_seen = 1;
            if (t.rot_delta > 0) twitch_frames++;
            if (t.rot_delta > max_rot)   max_rot   = t.rot_delta;
            if (t.top_shift > max_shift) max_shift = t.top_shift;
            if (f < 26)
                printf("  [zuck] f%-2d phase=%d +0x158=%d +0x16B=%d lean.z=%d rotDelta=%d topShift=%d\n",
                       f, e->sub_state_2, (int)e->re2z_t158, (int)(int8_t)e->re2z_gaitrow,
                       (int)e->re2_lean[2], t.rot_delta, t.top_shift);
        }
        CHECK(twitch_frames >= 15,
              "das Zucken laeuft nur in %d von 40 Frames — P1 (4 Ticks @0x80105850-54) + P2 "
              "(17 Ticks @0x80105970-7C) muessen es tragen", twitch_frames);
        CHECK(max_rot > 100,
              "die gerenderte Bone-0-Matrix weicht maximal um %d ab (Q12) — die Injektion "
              "@0x801057A4-E8 kommt im Renderpfad nicht an", max_rot);
        CHECK(max_shift > 20,
              "der oberste Bone verschiebt sich maximal um %d Einheiten — ohne die Gegenmatrix "
              "auf Bone 1 (@0x801057EC-838) waere das 0", max_shift);
        CHECK(phase1_zero_seen,
              "GEGENPROBE: beim P1-Eintritt ist +0x158 == 0, der Winkel also 0 und die Pose "
              "unveraendert — dieser Frame wurde nicht gesehen (der Test misst dann Rauschen)");
        /* Der belegte Rampen-Verlauf. f0 ist der P0-Frame (setzt +0x6 = 1, +0x158 = 0, +0x16B = 24
         * @0x801055B0-CC) und stellt noch keinen Winkel. Danach vier P1-Ticks mit +0x158 = 0..3
         * (`slti 3` @0x80105850) und dem <<3-Faktor: 0, -8*1*24*1, -8*2*24*1, -8*3*24*1
         * = 0/-192/-384/-576. Ab f5 P2 mit +0x158 = 16 abwaerts und OHNE <<3: -16*24 = -384. */
        CHECK(lean_angles[1] == 0 && lean_angles[2] == -192 && lean_angles[3] == -384 &&
              lean_angles[4] == -576,
              "P1-Rampe erwartet 0/-192/-384/-576 (=-8*+0x158*(s8)+0x16B*+0x15A @0x801057B0-C8), "
              "gemessen %d/%d/%d/%d", lean_angles[1], lean_angles[2], lean_angles[3],
              lean_angles[4]);
        CHECK(lean_angles[5] == -384 && lean_angles[6] == -360,
              "P2 startet mit +0x158 = 16 und OHNE den <<3 -> -384, -360, ... "
              "(@0x801058E0-F0 / @0x80105868), gemessen %d, %d", lean_angles[5], lean_angles[6]);
        printf("  [zuck] Frames mit Zucken=%d, max Bone0-Delta=%d (Q12), max Top-Shift=%d\n",
               twitch_frames, max_rot, max_shift);
    }

    /* ---------------- (2) der RAGDOLL-STURZ 0x801066FC ---------------- */
    {
        reset_zombie(e, 8);                    /* Zeile 8 / Spalte 1 -> 0x801066FC */
        s_se_n = 0;
        int32_t y0 = e->y;
        frame();
        CHECK(re15_re2z_last_hit_handler() == 3,
              "Zeile 8 / Spalte 1 muss 0x801066FC dispatchen (Tabelle @0x8010C940), Zelle %d",
              re15_re2z_last_hit_handler());
        CHECK(se_seen(12) || se_seen(13),
              "Ragdoll-Einstieg ohne Grunzer 12/13 (@0x80106960-78)");
        CHECK(se_seen(9), "Ragdoll-Einstieg ohne SE 9 (@0x80106988-90, unbedingt)");
        CHECK(e->motion == 4, "Sturz-Clip 4 erwartet (sw 0x30004,332 @0x80106888), motion=%d",
              (int)e->motion);
        CHECK(e->sub_state_2 == 1, "+0x6 muss nach P0 auf 1 stehen (@0x8010684C), ist %d",
              e->sub_state_2);
        CHECK(e->re2z_gy232 == (int16_t)y0,
              "+0x232 = Boden-Y (@0x80106994-A0) erwartet %d, ist %d", (int)y0, (int)e->re2z_gy232);
        CHECK((e->re2z_self1d3 & 0x80u) != 0,
              "+0x1D3 |= 0x80 fehlt (@0x801069A4-B0)");

        int bounce_se = 0, land_frames = 0, crawler = 0, y_moved = 0;
        int32_t ymin = e->y, ymax = e->y;
        for (int f = 0; f < 90; f++) {
            frame();
            if (e->y != y0) y_moved = 1;
            if (e->y < ymin) ymin = e->y;
            if (e->y > ymax) ymax = e->y;
            if (e->state == 1 && (e->re2z_f10e & 1u)) { crawler = 1; break; }
            land_frames++;
        }
        bounce_se = se_count(4);
        printf("  [ragdoll] Frames=%d, Y %d..%d (Start %d), SE4(Bounce)=%d, SE9=%d, Kriecher=%d, "
               "+0x10E=0x%04X state=%d sub=%d\n",
               land_frames, (int)ymin, (int)ymax, (int)y0, bounce_se, se_count(9), crawler,
               (unsigned)e->re2z_f10e, e->state, e->sub_state_1);
        CHECK(y_moved, "der Koerper faellt nicht — +0x3C += +0x15A (@0x80106E84-88) wirkt nicht");
        CHECK(bounce_se >= 1,
              "kein Aufprall-SE 4 (@0x80106F0C) — die Bounce-Physik laeuft nicht an");
        CHECK(crawler,
              "der Sturz endet nicht als KRIECHER: Phase 2 setzt +0x10E = 0x2001 und +0x4 = 1 "
              "(@0x80106B0C-10 / @0x80106B3C), gemessen +0x10E=0x%04X state=%d",
              (unsigned)e->re2z_f10e, e->state);
    }

    /* ---------------- (3) der RUTSCH-RUECKSTOSS 0x8010703C ---------------- */
    {
        reset_zombie(e, 12);                   /* Zeile 12 / Spalte 1 -> 0x8010703C */
        e->rot_y = 0;
        s_se_n = 0;
        int32_t x0 = e->x, z0 = e->z;
        frame();
        CHECK(re15_re2z_last_hit_handler() == 4,
              "Zeile 12 / Spalte 1 muss 0x8010703C dispatchen, Zelle %d",
              re15_re2z_last_hit_handler());
        CHECK(se_seen(12) || se_seen(13), "Rutsch-Einstieg ohne SE 12/13 (@0x801070F8-110)");
        CHECK(e->speed_h == 300 - 0,
              "+0x144 = 300 (@0x8010712C), danach in P1 -= +0x15A(=0) — gemessen %d",
              (int)e->speed_h);
        int moved = (e->x != x0 || e->z != z0);
        CHECK(moved, "der Rutsch bewegt den Koerper nicht (R_y(+0x158)*(+0x144,0,0) @0x801071B0-208)");

        int saw_blend = 0, phases_seen = 0, exit_ok = 0;
        uint8_t seen[8]; memset(seen, 0, sizeof seen);
        for (int f = 0; f < 200; f++) {
            if (e->state == 2 && e->sub_state_2 < 8) seen[e->sub_state_2] = 1;
            frame();
            if (e->re2_bone0_wgt > 0) saw_blend = 1;
            if (e->state == 1) { exit_ok = 1; break; }
        }
        for (int p = 0; p < 7; p++) if (seen[p]) phases_seen++;
        printf("  [rutsch] Phasen gesehen=%d, Bone0-Blend=%d, Ausgang state=%d sub=%d, "
               "dx=%d dz=%d\n", phases_seen, saw_blend, e->state, e->sub_state_1,
               (int)(e->x - x0), (int)(e->z - z0));
        CHECK(phases_seen >= 2,
              "die Rutsch-Zustandsmaschine bleibt in %d Phasen stecken (Tabelle @0x80100104 hat 7)",
              phases_seen);
        CHECK(exit_ok, "der Rutsch endet nie im Gang (P6 @0x80107408-18)");
        CHECK(e->sub_state_1 == 1 || e->sub_state_1 == 2,
              "Ausgang muss 0x101 oder 0x201 sein (@0x80107408/@0x80107418), sub=%d",
              e->sub_state_1);
    }

    /* ---------------- (4) ERREICHBARKEIT: Waffe 8 jenseits von 3000 ---------------- */
    {
        reset_zombie(e, 1);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* ALLE Zombies zaeh machen und ausser Reichweite schieben, damit der Schuss messbar den
         * hier beobachteten trifft; welcher Slot getroffen wurde, liefert der Rueckgabewert. */
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x10) {
                g_actors[s].hp = 400; g_actors[s].hit_react = 0;
                g_actors[s].state = 1; g_actors[s].sub_state_1 = 1;
            }
        pl->x = e->x - 4000; pl->z = e->z; pl->rot_y = 0;   /* > 3000 -> kein Instant-Kill */
        re15_player_set_equipped_weapon(8);
        int r = re15_player_weapon_fire(8);
        re15_actor_t *hit = (r > 0) ? &g_actors[r - 1] : NULL;
        printf("  [reach] Waffe 8 auf 4000: fire=%d slot=%d row(+0x5)=%d state=%d\n",
               r, r - 1, hit ? hit->sub_state_1 : -1, hit ? hit->state : -1);
        /* AKTUALISIERT 2026-08-18: die Zeile ist nicht mehr die rohe RE1.5-Waffen-Id, sondern die
         * uebersetzte RE2-ATTACKEN-ID (= Item-Id der Waffe, Beleg-Kette in enemy_ai_re2_zombie.c).
         * Die Remington M870 (RE1.5-Waffe 8) ist die STANDARD-Schrotflinte und landet damit auf
         * RE2-Id 7 (Shotgun, Schadensrecord 200/60/40 @0x800A412C+6*20); die staerkere SPAS-12
         * (RE1.5-Waffe 13) traegt RE2-Id 8 (Custom Shotgun, 300/80/60). Beide Zeilen dispatchen in
         * Spalte 1 dieselbe Ragdoll-Zelle 0x801066FC — der Sturz bleibt also erreichbar. */
        CHECK(hit && hit->sub_state_1 == 7 && hit->state == 2,
              "Waffe 8 jenseits 3000 muss in RE2-Zeile 7 (Schrotflinte) / HURT landen "
              "— fire=%d row=%d state=%d", r,
              hit ? hit->sub_state_1 : -1, hit ? hit->state : -1);
        if (hit && hit->state == 2) {
            hit->re2z_hits1d2 = 1; hit->sub_state_2 = 0; s_se_n = 0;
            frame();
            CHECK(re15_re2z_last_hit_handler() == 3,
                  "der ECHTE Schrotschuss erreicht 0x801066FC nicht (Zelle %d)",
                  re15_re2z_last_hit_handler());
        }
    }

    /* ---------------- (5) ZEILEN-ZENSUS DER TABELLE SELBST ---------------------------------
     * Zeigt, welche der 19 RE2-Attacken-Ids welche Zelle traegt (Spalte 1). Die drei NULL-Zeilen
     * 5/6/17 sind KEIN Loch: genau diese Ids tragen 900 Schaden (`0x384E1384` @0x800A412C+4*20 /
     * +5*20 / +16*20 = Magnum / Custom Magnum / Raketenwerfer), toeten den Zombie also immer, und
     * der Applier setzt dann `+0x4 = 3` (DEATH) statt 2 (@0x8004727C-90) — die HURT-Wurzel laeuft
     * dort nie. Welche RE1.5-WAFFE auf welche Zeile abgebildet wird und dass jede eine Reaktion
     * ausloest, pinnt seit 2026-08-18 der eigene Zensus test_re2_weapon_rows.c. */
    {
        static const char *hname[7] = { "NULL", "0x80105438", "0x80105BC0", "0x801066FC",
                                        "0x8010703C", "0x80107438", "0x80107EF0" };
        printf("  [zeilen] RE2-Attacken-Id -> Zelle(Spalte 1):\n");
        /* Nur EIN Zombie darf ticken — die Diagnose ist global, ein zweiter HURT-Zombie wuerde
         * sie im selben Frame ueberschreiben. */
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && &g_actors[s] != e) g_actors[s].active = 0;
        int shipped_ok = 1, null_rows = 0;
        for (int w = 0; w < 22; w++) {
            reset_zombie(e, 1);
            e->sub_state_1 = (uint8_t)w; e->re2z_hits1d2 = 1; e->sub_state_2 = 0;
            e->state = 2;
            frame();
            int h = re15_re2z_last_hit_handler();
            printf("      id=%-2d -> %s%s\n", w, hname[h < 7 ? h : 0],
                   h == 0 ? "   <== NULL" : "");
            if (h == 0) null_rows++;
            if (w >= 1 && w <= 4 && h != 1) shipped_ok = 0;
        }
        CHECK(shipped_ok,
              "die Messer-/Pistolen-Ids 1..4 muessen im Haupt-Handler 0x80105438 "
              "landen (Tabelle @0x8010C940 Zeilen 1-4)");
        CHECK(null_rows == 7,
              "erwartet werden GENAU 7 NULL-Zellen (Ids 0 = existiert nicht, 5/6/17 = 900-Schaden-"
              "Waffen und damit unerreichbar, 19/20/21 = ausserhalb der 19 RE2-Attacken-Ids), "
              "gemessen %d", null_rows);
        printf("  [zeilen] %d NULL-Zellen — der Port stempelt keine davon mehr "
               "(re15_re2z_row_for_weapon, Zensus in test_re2_weapon_rows)\n", null_rows);
    }

    free(buf);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(fails ? "test_re2_hit_reaction: %d FAIL\n" : "test_re2_hit_reaction: OK\n", fails);
    return fails ? 1 : 0;
}
