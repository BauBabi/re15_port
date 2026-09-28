/* probe_r30_hund_tod.c — Runde 30, Thema I (Dossier analysis/befunde_runde30/hund-tod.md)
 *   Nutzer: "I cannot die from a dog. He bites my neck, and i am standing again."
 *
 * Reiner MESSSTAND, kein Fix, keine Engine-Aenderung. Faehrt ROOM11D0 (freier Hunde-Satz,
 * Flag 3:152 = 1) mit dem echten Spielschritt und der Live-KI und schreibt JE BILD:
 *   hp, Spieler-Zustand, Opfer-FSM, Griff, tot?, Praesentation, Game-Over, Hunde-Zustand.
 *
 * Aufruf:
 *   probe_r30_hund_tod lauf <druck> <hp0> <bilder> [x z]
 *       druck 0 = KEINE Taste            (der Spieler fasst das Pad nicht an)
 *       druck 1 = Kreuz jedes 2. Bild    (der Nutzer drueckt, wie man es im Griff tut)
 *       druck 2 = EIN Druck im ersten Bild nach dem Latch
 *   probe_r30_hund_tod riegel            (exit 0/1: die Abnahme des Bau-Agenten)
 *
 * Umgebung R30_FIGUR=<n> (Bau-Agent, Nachtrag): Charakter-Byte der Spielfigur, der
 *   Port-Zwilling von DAT_800ACA5C — 0 = Leon (Default), 4 = Elza. Seit dem Bau haengt das
 *   Zaehler-Fenster von Sub 7 an re15_char_variant() (RE2: lbu v0,8(s3) / andi v0,v0,0x1
 *   @0x80102010-18 = Figuren-Nummer), also ist die Figur eine Messgroesse.
 *
 * Bauvorlage: probe_r27_hund_biss.c (Hochfahren wortgleich uebernommen). */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_esp.h"
#include "re2_ems.h"
#include "re15_math.h"
#include "re15_gameflow.h"   /* g_gameflow.character / re15_char_variant (R30_FIGUR) */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t           s_rdt;
static re15_camera_view_t   s_cam;
static re15_game_ctx_t      s_ctx;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_pl00(void)
{
    size_t a = 0, b = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
    return edd && emr &&
           re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
           re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0;
}

static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static re15_enemy_bank_t *load_bank_re2(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0;
    return NULL;
}

static uint16_t s_pad_now = 0, s_pad_edge = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = s_pad_now; s_ctx.pad_pressed = s_pad_edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    /* re15_enemy_reset() LOESCHT alle Baenke (memset, enemy_common.c:129-134). Die Bank muss
     * also NACH dem Reset und VOR dem Spawn geladen werden — sonst laufen alle Hunde-Clips
     * mit Laenge 1 und die Opfer-FSM startet nie (gemessen im ersten Sondenlauf: vs=0,
     * Clip 23/24/25 je EIN Bild). */
    if (!load_bank_re2(0x20)) printf("WARNUNG: EM020-RE2-Bank fehlt\n");
    { re15_enemy_bank_t *eb = re15_enemy_find(0x20);
      if (eb) fprintf(stderr, "[bank] 0x20 ok=%d victim_ok=%d clips=%d victim_clips=%d\n",
                      eb->ok, eb->victim_ok, eb->anim.clip_count,
                      eb->victim_ok ? eb->anim_victim.clip_count : -1); }
    re15_player_cmd_reset(); re15_player_victim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = 0x11D0; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_game_flag_set(3, 152, 1);          /* freier Hunde-Satz (Else-Zweig @Datei 0x13EE) */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -300; pl->z = -17400; pl->rot_y = 1024;
    re15_collision_set_band(0);
}

static int hunde(int *slots, int max)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20 && n < max) slots[n++] = s;
    return n;
}

typedef struct {
    int bisse, latch_bild, tot_bild, praes_bild, gameover_bild, cmd3_bild;
    int auferstanden_bild;       /* hp war < 0 und ist wieder >= 0 */
    int hp_min, hp_ende;
    int steht_wieder_bild;       /* nach hp<0: Opfer-FSM 0, nicht tot, kein Griff */
    int zielbilder_vor_tod;      /* Bilder mit aktiver Zielphase bei hp >= 0 (Abdeckung) */
    int zielpose_im_tod;         /* Bilder mit hp < 0, Zielphase aktiv UND Opfer-FSM 0 —
                                  * genau die Bedingung des Render-Overrides main.c:7616 */
    int stehbilder_im_tod;       /* Bilder mit hp < 0, in denen der Spieler die STEH-Pose
                                  * traegt (motion 200) — "he is standing again" */
    int clip_neustarts;          /* wie oft sprang der Opfer-Clip bei hp < 0 auf Bild 0 zurueck */
} ergebnis_t;
extern int re15_player_aim_active(void);

static int s_yaw = 1024;
static void lauf(int druck, int hp0, int nframes, int32_t sx, int32_t sz, FILE *out,
                 ergebnis_t *er)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->y = 0; pl->rot_y = (int16_t)s_yaw; pl->hp = (int16_t)hp0;
    pl->floor = 0; pl->state = 0; pl->motion = 0;
    re15_collision_set_band(0);
    int hs[8]; int nh = hunde(hs, 8);
    for (int i = 0; i < nh; i++)
        if (g_actors[hs[i]].grid_id == 0x41) g_actors[hs[i]].grid_id = 0x42;

    memset(er, 0, sizeof *er);
    er->latch_bild = er->tot_bild = er->praes_bild = er->gameover_bild = -1;
    er->cmd3_bild = er->auferstanden_bild = er->steht_wieder_bild = -1;
    er->hp_min = hp0;
    int war_negativ = 0, latch_gesehen = 0, einmal_gedrueckt = 0, v_afr = 0;
    int neustart_offen = 0;
    int v_hp = pl->hp, v_st = -1, v_vs = -1, v_gr = -1, v_tot = -1, v_pr = -1, v_go = -1;

    fprintf(out, "# druck=%d hp0=%d bilder=%d start=(%d,%d) hunde=%d figur=%d (ungerade=%d)\n",
            druck, hp0, nframes, (int)sx, (int)sz, nh, (int)g_gameflow.character,
            re15_char_variant());
    fprintf(out, "# f hp st s1 mot afr | vs vtyp gr tot praes cmd3 go flyin | pad |"
                 " hund: slot st s1 s2 s3 clip afr bite21e abort21c ctr16a rel220 hp\n");
    for (int f = 0; f < nframes; f++) {
        int gr_vor = re15_player_is_grabbed();
        s_pad_now = 0; s_pad_edge = 0;
        if (druck == 1 && (f & 1)) { s_pad_now = 0x4000; s_pad_edge = 0x4000; }
        if (druck == 2 && latch_gesehen && !einmal_gedrueckt) {
            s_pad_now = 0x4000; s_pad_edge = 0x4000; einmal_gedrueckt = 1;
        }
        if (druck == 3) {                      /* R1 GEHALTEN = der Spieler zielt (N2) */
            s_pad_now = RE15_PAD_BIT_R1;
            s_pad_edge = (f == 0) ? RE15_PAD_BIT_R1 : 0;
        }
        int hp_vor = pl->hp;
        frame();
        if (pl->hp < 0 && hp_vor < 0) {
            if (pl->motion == 200) er->stehbilder_im_tod++;
            /* Neustart = der Opfer-Clip stand schon weiter (v_afr = bisher groesstes Bild in
             * der Opfer-FSM bei hp < 0) und laeuft jetzt wieder von vorn */
            if (re15_player_victim_state() != 0) {
                if ((int)pl->anim_frame + 5 < v_afr && !neustart_offen) {
                    er->clip_neustarts++; neustart_offen = 1;
                }
                if ((int)pl->anim_frame > v_afr) { v_afr = (int)pl->anim_frame; neustart_offen = 0; }
            }
        }
        {   int ziel = re15_player_aim_active();
            if (ziel && pl->hp >= 0) er->zielbilder_vor_tod++;
            if (ziel && pl->hp < 0 && re15_player_victim_state() == 0) er->zielpose_im_tod++;
        }
        int gr  = re15_player_is_grabbed();
        int vs  = re15_player_victim_state();
        int vt  = (int)re15_player_victim_type();
        int tot = re15_player_is_dead();
        int pr  = re15_death_presentation_active();
        int c3  = re15_player_death_cmd3_active();
        if (pl->hp < hp_vor) er->bisse++;
        if (pl->hp < er->hp_min) er->hp_min = pl->hp;
        if (gr && !gr_vor && er->latch_bild < 0) { er->latch_bild = f; }
        if (gr) latch_gesehen = 1;
        if (tot && er->tot_bild < 0) er->tot_bild = f;
        if (pr  && er->praes_bild < 0) er->praes_bild = f;
        if (c3  && er->cmd3_bild < 0) er->cmd3_bild = f;
        if (g_gameover_active && er->gameover_bild < 0) er->gameover_bild = f;
        if (pl->hp < 0) war_negativ = 1;
        if (war_negativ && pl->hp >= 0 && er->auferstanden_bild < 0) er->auferstanden_bild = f;
        if (war_negativ && pl->hp >= 0 && vs == 0 && !gr && !tot && er->steht_wieder_bild < 0)
            er->steht_wieder_bild = f;

        /* der Hund, der den Spieler haelt oder beisst: Sub 7 zuerst, sonst der naechste */
        int hi = -1; int64_t best = -1;
        for (int i = 0; i < nh; i++) {
            re15_actor_t *d = &g_actors[hs[i]];
            if (!d->active) continue;
            if (d->state == 1 && d->sub_state_1 == 7) { hi = i; break; }
            int64_t dx = (int64_t)d->x - pl->x, dz = (int64_t)d->z - pl->z;
            int64_t q = dx * dx + dz * dz;
            if (best < 0 || q < best) { best = q; hi = i; }
        }
        int aend = (pl->hp != v_hp) || (pl->state != v_st) || (vs != v_vs) || (gr != v_gr) ||
                   (tot != v_tot) || (pr != v_pr) || (g_gameover_active != v_go);
        if (aend || (f % 15) == 0 || s_pad_edge) {
            fprintf(out, "%-5d %4d %2d %2d %3d %3d | %d 0x%02X %d %d %d %d %d %3d | %04X |",
                    f, pl->hp, pl->state, pl->sub_state_1, (int)pl->motion,
                    (int)pl->anim_frame, vs, vt, gr, tot, pr, c3, g_gameover_active,
                    g_death_flyin, s_pad_edge);
            if (hi >= 0) {
                re15_actor_t *d = &g_actors[hs[hi]];
                fprintf(out, " %d %d %2d %d %d %2d %3d %d %d %3d %d %d", hs[hi], d->state,
                        d->sub_state_1, d->sub_state_2, d->sub_state_3, (int)d->motion,
                        (int)d->anim_frame, d->re2d_bite21e, d->re2d_abort21c,
                        (int)(int8_t)d->re2z_dir16a, d->re2d_rel220, d->hp);
            }
            if (aend) fprintf(out, "   <-- AENDERUNG");
            fprintf(out, "\n");
        }
        v_hp = pl->hp; v_st = pl->state; v_vs = vs; v_gr = gr; v_tot = tot; v_pr = pr;
        v_go = g_gameover_active;
    }
    er->hp_ende = pl->hp;
}

static void zeige(const char *name, const ergebnis_t *er)
{
    printf("%-22s bisse=%d hp_min=%d hp_ende=%d latch@%d tot@%d praes@%d cmd3@%d"
           " gameover@%d AUFERSTANDEN@%d steht_wieder@%d ziel_vor=%d ZIELPOSE_IM_TOD=%d"
           " STEHBILDER_IM_TOD=%d CLIP_NEUSTARTS=%d\n",
           name, er->bisse, er->hp_min,
           er->hp_ende, er->latch_bild, er->tot_bild, er->praes_bild, er->cmd3_bild,
           er->gameover_bild, er->auferstanden_bild, er->steht_wieder_bild,
           er->zielbilder_vor_tod, er->zielpose_im_tod, er->stehbilder_im_tod,
           er->clip_neustarts);
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "lauf";
    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11D0.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM11D0.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }
    if (!load_pl00()) { printf("FEHLT: PL00-Rig\n"); return 77; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    {   const char *fg = getenv("R30_FIGUR");      /* 0 = Leon (Default), 4 = Elza */
        g_gameflow.character = (fg && *fg) ? atoi(fg) : 0;
        printf("FIGUR: character=%d ungerade=%d\n", (int)g_gameflow.character,
               re15_char_variant()); }
    if (!load_bank_re2(0x20)) {
        printf("FEHLT: EM020-RE2-Bank (shared_assets/RE2/CDEMD0.EMS)\n"); return 77;
    }

    if (!strcmp(mode, "lauf")) {
        int druck  = (argc > 2) ? atoi(argv[2]) : 0;
        int hp0    = (argc > 3) ? atoi(argv[3]) : 20;
        int frames = (argc > 4) ? atoi(argv[4]) : 1500;
        int32_t sx = (argc > 5) ? atoi(argv[5]) : -7878;   /* Nutzer-Lage F1523 */
        int32_t sz = (argc > 6) ? atoi(argv[6]) : -17384;
        ergebnis_t er;
        if (argc > 7) s_yaw = atoi(argv[7]);
        bringup();
        lauf(druck, hp0, frames, sx, sz, stdout, &er);
        zeige("ERGEBNIS", &er);
        return 0;
    }
    if (!strcmp(mode, "suche")) {
        /* ABDECKUNG: Blickrichtung x Druck — welcher Lauf endet im LATCH (Kehlbiss),
         * welcher im Boden-Biss, und wer steht danach wieder? */
        int hp0    = (argc > 2) ? atoi(argv[2]) : 20;
        int frames = (argc > 3) ? atoi(argv[3]) : 1500;
        int32_t sx = (argc > 4) ? atoi(argv[4]) : -7878;
        int32_t sz = (argc > 5) ? atoi(argv[5]) : -17384;
        int laeufe = 0, toedlich = 0, mit_latch = 0, auferstanden = 0, gameover = 0;
        int druck_max = (argc > 6) ? atoi(argv[6]) : 1;
        for (int yaw = 0; yaw < 4096; yaw += 256) {
            for (int druck = 0; druck <= druck_max; druck++) {
                if (druck == 2) continue;
                ergebnis_t er; char nm[48];
                FILE *nul = fopen("NUL", "w"); if (!nul) nul = stdout;
                s_yaw = yaw;
                bringup();
                lauf(druck, hp0, frames, sx, sz, nul, &er);
                if (nul != stdout) fclose(nul);
                snprintf(nm, sizeof nm, "yaw=%-4d druck=%d", yaw, druck);
                zeige(nm, &er);
                laeufe++;
                if (er.hp_min < 0) toedlich++;
                if (er.latch_bild >= 0) mit_latch++;
                if (er.auferstanden_bild >= 0) auferstanden++;
                if (er.gameover_bild >= 0) gameover++;
            }
        }
        printf("ABDECKUNG: Laeufe %d | toedlich gebissen %d | mit Latch %d |"
               " AUFERSTANDEN %d | Game Over %d\n", laeufe, toedlich, mit_latch,
               auferstanden, gameover);
        return 0;
    }
    if (!strcmp(mode, "n2")) {
        /* N2 (android-r1-toggle.md): Tod WAEHREND des Zielens ueber den cmd-3-Pfad.
         * R1 gehalten, nach 90 Bildern faellt hp auf -1 (wie RE15_KILL_AT im Echtlauf);
         * danach 200 Bilder zaehlen, in denen die Zielphase bei hp < 0 noch aktiv ist UND
         * die Opfer-FSM 0 — das ist die Bedingung des Render-Overrides main.c:7616.
         * Die Hunde werden weit weg geparkt, damit NUR der cmd-3-Pfad gemessen wird. */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int hs[8]; int nh;
        int ziel_vor = 0, ziel_im_tod = 0, cmd3 = -1, tot_bilder = 0;
        bringup();
        nh = hunde(hs, 8);
        for (int i = 0; i < nh; i++) g_actors[hs[i]].active = 0;
        pl->x = -7878; pl->z = -17384; pl->y = 0; pl->rot_y = 1024; pl->hp = 100;
        for (int f = 0; f < 290; f++) {
            s_pad_now = RE15_PAD_BIT_R1; s_pad_edge = (f == 0) ? RE15_PAD_BIT_R1 : 0;
            if (f == 90) pl->hp = -1;
            frame();
            if (pl->hp >= 0) { if (re15_player_aim_active()) ziel_vor++; }
            else {
                tot_bilder++;
                if (re15_player_aim_active() && re15_player_victim_state() == 0) ziel_im_tod++;
                if (cmd3 < 0 && re15_player_death_cmd3_active()) cmd3 = f;
            }
        }
        printf("N2: Zielbilder vor dem Tod %d von 90 | cmd3 ab Bild %d | Bilder tot %d |"
               " davon mit aktiver Zielphase %d\n", ziel_vor, cmd3, tot_bilder, ziel_im_tod);
        if (ziel_vor == 0) { printf("FEHLLAUF: der Spieler hat nie gezielt\n"); return 1; }
        printf(ziel_im_tod ? "N2 ROT\n" : "N2 GRUEN\n");
        return ziel_im_tod ? 1 : 0;
    }
    if (!strcmp(mode, "riegel")) {
        /* ABNAHME (Soll nach dem Bau): hp 20, Hund beisst, der Spieler faellt unter 0 und
         * BLEIBT dort; kein Aufstehen; der Game-Over-Pfad wird erreicht — mit und ohne Taste. */
        static const int k_yaw[3] = { 0, 1024, 3072 };
        int fehler = 0, laeufe = 0, mit_latch = 0;
        for (int yi = 0; yi < 3; yi++)
        for (int druck = 0; druck <= 3; druck++) {
            ergebnis_t er; char nm[32];
            FILE *nul = fopen("NUL", "w"); if (!nul) nul = stdout;
            s_yaw = k_yaw[yi];
            bringup();
            lauf(druck, 20, 2400, -7878, -17384, nul, &er);
            if (nul != stdout) fclose(nul);
            snprintf(nm, sizeof nm, "riegel yaw=%d druck=%d", k_yaw[yi], druck);
            zeige(nm, &er);
            laeufe++;
            if (er.latch_bild >= 0) mit_latch++;
            if (er.hp_min >= 0) { printf("  FEHLLAUF: kein toedlicher Biss\n"); fehler++; continue; }
            if (er.auferstanden_bild >= 0) {
                printf("  ROT: hp wieder >= 0 in Bild %d\n", er.auferstanden_bild); fehler++;
            }
            if (er.hp_ende >= 0)   { printf("  ROT: hp am Ende %d\n", er.hp_ende); fehler++; }
            if (er.praes_bild < 0) { printf("  ROT: Todes-Praesentation nie gestartet\n"); fehler++; }
            if (er.gameover_bild < 0) { printf("  ROT: Game Over nie erreicht\n"); fehler++; }
            if (er.stehbilder_im_tod > 0) {
                printf("  ROT: %d Bilder Steh-Pose bei hp < 0\n", er.stehbilder_im_tod); fehler++;
            }
            if (er.clip_neustarts > 0) {
                printf("  ROT: Opfer-Clip %d-mal neu gestartet\n", er.clip_neustarts); fehler++;
            }
            if (er.zielpose_im_tod > 0) {
                printf("  ROT: %d Bilder Zielpose bei hp < 0\n", er.zielpose_im_tod); fehler++;
            }
        }
        /* ABDECKUNG: der Riegel ist wertlos, wenn er den Kehlbiss gar nicht erreicht */
        printf("ABDECKUNG: Laeufe %d, davon mit Latch (Kehlbiss) %d\n", laeufe, mit_latch);
        if (mit_latch < 6) { printf("  ROT: zu wenige Latch-Laeufe (< 6)\n"); fehler++; }
        printf(fehler ? "RIEGEL ROT (%d)\n" : "RIEGEL GRUEN (%d)\n", fehler);
        return fehler ? 1 : 0;
    }
    printf("unbekannter Modus\n");
    return 1;
}
