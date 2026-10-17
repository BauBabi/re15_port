/* test_r38_spiderman.c — Runde 38 Spur T "spiderman" (Dossier analysis/befunde_runde38/T_spiderman.md).
 *
 * Misst die MECHANIK am echten Raumskript ROOM4001 (Elza, Spiderman = Typ 0x2B aus EMD/CDEMD1.EMS) mit dem echten
 * re15_game_step (Spieler-Zweig, re15_enemy_ai_run_all, Opfer-Tick):
 *
 *   austritt   Nutzer-Punkt 1. Raum mit Ck(4,20)=1 (Spiderman-Zweig von sub00 @0x01208) betreten: beide Spiderman
 *              stecken versteckt im Schacht (Zustand 4, +0x5 = 1, grid 0x41, nicht gezeichnet, word0 & 0x42,
 *              kein Ziel, kein Schatten) und BLEIBEN dort 600 Bilder lang (P1 wartet auf grid 0x42 @0x8011545C-60).
 *              Dann laeuft der Spieler in den Ausloeser (Aot 4 @0x01218): das Raumskript sub05 setzt grid 0x42
 *              (@0x015BA), der Austritt startet: sichtbar ab Bild 6 (@0x8011549C), greifbar + grid 0 ab Bild 0x21
 *              (@0x80115508-64), nach 35 Bildern (Clip 0x1D) ACTIVE mit +0x5 = 1 (0x101 @0x80115588-98).
 *              Gegenprobe: der zweite Spiderman (Ausloeser Aot 5) bleibt versteckt.
 *   griff1     Nutzer-Punkt 2. Spiderman schon draussen (Ck(3,0x91)=Ck(3,0xAF)=1 -> grid-0-Zweig), der Spieler-Anker
 *              ist VERGIFTET (Anker eines frueheren Griffs weit weg) — genau die Lage vor dem Fix. Der Spiderman
 *              greift (Spieler-hp 40 -> nach dem ersten Treffer < 50 -> Griff 1 @0x801128ac). Gefordert: im
 *              Latch-Bild ist der Spieler-Anker == Spiderman-Anker (FUN_8001ac38-Kopie @0x8001ad28-48), und in
 *              JEDEM Griff-/Kollaps-Bild bleibt der Spieler im Raum, auf seiner Seite jeder Wand und in
 *              Griff-Reichweite (Schranke aus den Bankdaten, s. u.). Vorher (e2cc7673): 14 600 Einheiten Sprung.
 *   griff2     Griff 2 (B[15], heute nur ueber +0x5 = 0xF erreichbar): EINMALIGE Spielerlage = Spiderman-Lage + 0x66C
 *              in Blickrichtung + 0x800 (@0x801141E0-E8 / @0x80114204-14), Spiderman bleibt stehen, danach KEINE
 *              Spielerbewegung mehr (Hook A [2]/[3] ohne ad68), Opfer-Clips 8/9/0xA, kein Yaw-Schreiben.
 *
 * NACHBESSERUNG 1 (Abnahme 0, M1/M2): GEZEICHNETES Bild. Jedes Bild rechnet der Test die Pose so nach, wie der
 * Renderer sie zeichnet (gezeichnet(): Bankwahl, re15_compute_actor_kf, re15_skel_compute_pose mit Ueberblendung).
 * Gefordert: im Bild, in dem ein Clip endet und die Phase wechselt, steht Bild fc-1 des Clips (FUN_8001f3bc rechnet
 * die Pose VOR dem Hochzaehlen: @0x8001f534/@0x8001f574 vor @0x8001f610-1c, Wrap `sb zero,149` @0x8001f63c) —
 * nie Bild 0 (vorher: Austritt Clip 0x1D Bild 0 = Startpose 1278 unter dem Boden, Griff-Clips ebenso). Das erste
 * ACTIVE-Bild blendet aus GENAU dieser Pose (+0x8F = 7 @0x80112184 / @0x80112320). Die Griff-P2 starten den
 * Halte-Clip beim Wrap-Wert 0 (kein +0x95-Schreiben @0x80112B9C / @0x80113094 / @0x801142C8). Losreissen (Tasten)
 * misst die Loese-Clips P5 -> P6 (P6 ohne anim_set @0x80112DA0 / @0x8011327C / @0x80114414).
 *
 * Rueckgabe 0 = alles gruen, 1 = Fehler, 77 = Assets fehlen (SKIP). */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_roy_r37.h"
#include "re15_spiderman_r38.h"
#include "re15_anim_select.h"
#include "re15_player.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);   /* re15_damage.c (FUN_8003703c) */

static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_rdt_buf;
static uint8_t           *s_ems;
static int                s_fails = 0;

#define CHECK(cond, ...) do { if (!(cond)) { s_fails++; printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); } \
                              else { printf("  ok:   "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* EM2B aus dem Elza-Archiv (Datei 0x27 @0x800223ac) in die Registry — wie pc_enemy_load ohne Renderer. */
static int bank_2b_laden(void)
{
    char p[600]; size_t n = 0; uint32_t sek = 0, len = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(RE15_EMS_ARCHIV_ELZA));
    if (!s_ems) s_ems = slurp(p, &n);
    if (!s_ems || !re15_ems_toc(RE15_EMS_ARCHIV_ELZA, RE15_R38_SM_TYP, &sek, &len)) return 0;
    const uint8_t *blob = s_ems + (size_t)sek * 2048u;
    re15_enemy_bank_t *eb = re15_enemy_find(RE15_R38_SM_TYP);
    if (!eb) eb = re15_enemy_alloc(RE15_R38_SM_TYP);
    if (!eb) return 0;
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(blob, len, &eb->skel_own, &eb->anim_own) == 0);
    return 1;
}

/* ---- Gezeichnetes Bild (Nachbesserung 1, Abnahme 0 M1/M2) ---------------------------------------------------
 * Wie der NPC-Zeichner in platform/pc/main.c NACH dem KI-Tick: Bank = eb->anim (re15_actor_anim_select:
 * clip_override = motion bei anim_flags & 4, sonst -1) bzw. die Loco-Bank, wenn re15_actor_uses_loco_bank;
 * Keyframe re15_compute_actor_kf(.., anim_frame); Pose re15_skel_compute_pose mit g_anim_pose_actor = Aktor
 * (Ueberblendung aus der zuletzt gezeichneten Pose, prev_root). Je Bild fuer jeden Spiderman nachgerechnet. */
typedef struct { int st, ss1, ss2, motion, frame, kf; int32_t root[3]; } gez_t;
static gez_t s_gez[RE15_ACTOR_MAX], s_gez_alt[RE15_ACTOR_MAX];
static int   s_gez_ok[RE15_ACTOR_MAX];
static int   s_rueck_modul = 0, s_rueck_alt = 0;     /* Ruecksprung auf Bild 0 im Wechselbild */
static int   s_br_ende = 0, s_br_p2 = 0, s_br_fehler = 0;   /* Bruellen B[2]: Clip-Ende, P2-Bild, Fehler */
static uint16_t s_taste = 0;                         /* Kante fuer das naechste Bild (Losreissen) */

static void gezeichnet(re15_actor_t *e, gez_t *g)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    memset(g, 0, sizeof *g);
    g->st = e->state; g->ss1 = e->sub_state_1; g->ss2 = e->sub_state_2;
    g->motion = e->motion; g->frame = (int)e->anim_frame; g->kf = -1;
    if (!b || !b->ok) return;
    const re15_emd_animation_t *an = &b->anim; const re15_emd_skeleton_t *sk = &b->skel;
    int ov = (e->anim_flags & 0x04) ? (int)e->motion : -1;
    if (re15_actor_uses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
        an = &b->anim_loco; sk = &b->skel_loco; ov = (int)e->motion;
    }
    g->kf = re15_compute_actor_kf(an, sk, e, ov, (uint32_t)e->anim_frame);
    static re15_skel_pose_t po[RE15_EMD_MAX_BONES];
    void *sav = g_anim_pose_actor; g_anim_pose_actor = e;
    (void)re15_skel_compute_pose(sk, g->kf, po);
    g_anim_pose_actor = sav;
    g->root[0] = e->prev_root[0]; g->root[1] = e->prev_root[1]; g->root[2] = e->prev_root[2];
}

/* Phasen, die spiderman_r38.c fuehrt (Austritt [0]/[1], Bruellen B[2], Griff 1/Kollaps/Griff 2). */
static int modul_phase(const gez_t *g)
{
    return (g->st == 4 && g->ss1 <= 1) ||
           (g->st == 1 && (g->ss1 == 2 || g->ss1 == 5 || g->ss1 == 9 || g->ss1 == 8 || g->ss1 == 10 ||
                           g->ss1 == 15));
}

static int clip_fc(int clip)
{
    re15_enemy_bank_t *b = re15_enemy_find(RE15_R38_SM_TYP);
    if (!b || !b->ok || clip < 0 || clip >= b->anim.clip_count) return -1;
    return b->anim.clips[clip].frame_count;
}

static void clip_wurzel(int clip, int frame, int32_t out[3])
{
    re15_enemy_bank_t *b = re15_enemy_find(RE15_R38_SM_TYP);
    out[0] = out[1] = out[2] = 0;
    if (!b || !b->ok || clip < 0 || clip >= b->anim.clip_count) return;
    int kf = (int)(b->anim.frames[b->anim.clips[clip].first_frame + frame] & 0xfff);
    int16_t x = 0, y = 0, z = 0;
    re15_emd_get_keyframe_position(&b->skel, kf, &x, &y, &z);
    out[0] = x; out[1] = y; out[2] = z;
}

static double abst3(const int32_t a[3], const int32_t b[3])
{
    double dx = (double)a[0] - b[0], dy = (double)a[1] - b[1], dz = (double)a[2] - b[2];
    return sqrt(dx * dx + dy * dy + dz * dz);
}

/* Sichtbarkeit eines Ruecksprungs: groesste Winkeldifferenz (12 bit, kuerzester Weg) ueber alle Knochen zwischen
 * zwei Keyframes der Hauptbank. */
static int kf_winkel_max(int kf_a, int kf_b)
{
    re15_enemy_bank_t *b = re15_enemy_find(RE15_R38_SM_TYP);
    if (!b || !b->ok || kf_a < 0 || kf_b < 0) return -1;
    int m = 0;
    for (int k = 0; k < b->skel.bone_count; k++) {
        int16_t a[3], c[3];
        re15_emd_get_keyframe_angles(&b->skel, kf_a, k, &a[0], &a[1], &a[2]);
        re15_emd_get_keyframe_angles(&b->skel, kf_b, k, &c[0], &c[1], &c[2]);
        for (int i = 0; i < 3; i++) {
            int d = ((int)c[i] - (int)a[i]) & 0xfff;
            if (d > 0x800) d = 0x1000 - d;
            if (d > m) m = d;
        }
    }
    return m;
}

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = s_taste;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != RE15_R38_SM_TYP) { s_gez_ok[s] = 0; continue; }
        s_gez_alt[s] = s_gez[s];
        gezeichnet(e, &s_gez[s]);
        if (!s_gez_ok[s]) { s_gez_ok[s] = 1; continue; }
        const gez_t *p = &s_gez_alt[s], *c = &s_gez[s];
        int gewechselt = (p->st != c->st || p->ss1 != c->ss1 || p->ss2 != c->ss2);
        /* Bruellen B[2]: Fertig-Bild (P1 -> P2, +0x6 += fertig @0x80112374-84) und P2-Bild (@0x80112390, kein
         * anim_set) zeigen Clip 2 Bild fc-1; +0x5 = 3 erst im P2-Bild (@0x801123C4), nie schon im Fertig-Bild. */
        if (p->st == 1 && c->st == 1 && p->ss1 == 2) {
            const int soll = clip_fc(2) - 1;
            if (p->ss2 == 1 && c->ss1 == 2 && c->ss2 == 2) {
                s_br_ende++;
                if (!(c->motion == 2 && c->frame == soll)) s_br_fehler++;
            } else if (p->ss2 == 2 && c->ss1 == 3) {
                s_br_p2++;
                if (!(c->motion == 2 && c->frame == soll)) s_br_fehler++;
            } else if (p->ss2 == 1 && c->ss1 == 3) {
                s_br_fehler++;                               /* ohne P2-Bild direkt auf +0x5 = 3 (alter Port-Fall) */
            }
        }
        if (gewechselt && c->motion == p->motion && p->frame > 0 && c->frame == 0) {
            if (modul_phase(p)) {
                s_rueck_modul++;
                printf("  Ruecksprung (Modul): slot %d [%d/%d/%d] -> [%d/%d/%d] Clip 0x%X Bild %d -> 0 "
                       "(Wurzel %.0f, Winkel max %d/4096)\n", s,
                       p->st, p->ss1, p->ss2, c->st, c->ss1, c->ss2, c->motion, p->frame,
                       abst3(p->root, c->root), kf_winkel_max(p->kf, c->kf));
            } else {
                s_rueck_alt++;
                if (s_rueck_alt <= 6)
                    printf("  info: Ruecksprung im Alt-Pfad: slot %d [%d/%d/%d] -> [%d/%d/%d] Clip 0x%X Bild %d -> 0 "
                           "(Wurzel %.0f, Winkel max %d/4096)\n",
                           s, p->st, p->ss1, p->ss2, c->st, c->ss1, c->ss2, c->motion, p->frame,
                           abst3(p->root, c->root), kf_winkel_max(p->kf, c->kf));
            }
        }
    }
}

/* ROOM4001 frisch betreten: Flags setzen (nach scd_vm_init, das sie nullt), Raumskript hochfahren. */
static int raum_4001(const uint8_t (*flags)[2], int nflags, int32_t px, int32_t pz, int16_t prot)
{
    if (!s_rdt_buf) {
        size_t sz = 0;
        s_rdt_buf = slurp(RE15_ASSET_PSX_DIR "/STAGE4/ROOM4001.RDT", &sz);
        memset(&g_room_rdt, 0, sizeof g_room_rdt);
        if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, sz, &g_room_rdt) != 0) return 0;
    }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &g_room_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_room_rdt_ok = 1;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x5b1d3a7u);
    if (!bank_2b_laden()) return 0;
    g_current_room_id = 0x4001;
    for (int i = 0; i < nflags; i++) re15_game_flag_set(flags[i][0], flags[i][1], 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = prot;
    re15_collision_set_band(0);
    scd_register_room_events(&g_room_rdt);
    scd_room_reenter(&g_room_rdt, px, pz, 0);
    return 1;
}

static int spiderman_slots(int out[4])
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < 4; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38_SM_TYP) out[n++] = s;
    return n;
}

/* ---- Raum-Invariante (Riegel fuer Punkt 2) -------------------------------------------------------------------
 * "Spieler bleibt auf begehbarem Boden innerhalb des Raums": (a) innerhalb der Huelle aller SCA-Zellen, (b) keine
 * solide Wandzelle (Typ 1, u0 & 1 = die Spieler-Maske von re15_collision_constrain, Band 0) zwischen der Lage vor
 * dem Griff und der Lage jetzt — ein Griff darf ihn nicht durch eine Wand setzen. */
static int32_t s_hx0, s_hx1, s_hz0, s_hz1;
static void huelle(void)
{
    s_hx0 = s_hz0 = 0x7fffffff; s_hx1 = s_hz1 = -0x7fffffff;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        int32_t x0 = c->x, z0 = c->z, x1 = x0 + (int32_t)c->width, z1 = z0 + (int32_t)c->density;
        if (x0 < s_hx0) s_hx0 = x0;
        if (z0 < s_hz0) s_hz0 = z0;
        if (x1 > s_hx1) s_hx1 = x1;
        if (z1 > s_hz1) s_hz1 = z1;
    }
}
static int im_raum(int32_t x, int32_t z) { return x >= s_hx0 && x <= s_hx1 && z >= s_hz0 && z <= s_hz1; }
static int durch_wand(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    return re15_re2_los_cells_blocked(&g_room_rdt, x0, z0, x1, z1, 0, 1u);
}

/* Griff-Reichweite aus den Bankdaten: groesster Wurzelversatz der Opferbank + groesster der Spiderman-Griffclips
 * (0xE..0x15) + ATTACK1-Reichweite a804(2200) @0x8011284c-88. Liegt der Spieler weiter vom Spiderman weg, hat ihn
 * nicht der Griff dorthin gestellt. */
static int32_t griff_reichweite(void)
{
    re15_enemy_bank_t *b = re15_enemy_find(RE15_R38_SM_TYP);
    int32_t mv = 0, mg = 0;
    for (int c = 0; b && b->victim_ok && c < b->anim_victim.clip_count; c++)
        for (int f = 0; f < b->anim_victim.clips[c].frame_count; f++) {
            int kf = (int)(b->anim_victim.frames[b->anim_victim.clips[c].first_frame + f] & 0xfff);
            int16_t sx = 0, sy = 0, sz = 0;
            if (re15_emd_get_keyframe_speed(&b->skel_victim, kf, &sx, &sy, &sz)) {
                int32_t m = (int32_t)sqrt((double)sx * sx + (double)sz * sz);
                if (m > mv) mv = m;
            }
        }
    for (int c = 0x0e; b && b->ok && c <= 0x15 && c < b->anim.clip_count; c++)
        for (int f = 0; f < b->anim.clips[c].frame_count; f++) {
            int kf = (int)(b->anim.frames[b->anim.clips[c].first_frame + f] & 0xfff);
            int16_t sx = 0, sy = 0, sz = 0;
            if (re15_emd_get_keyframe_speed(&b->skel, kf, &sx, &sy, &sz)) {
                int32_t m = (int32_t)sqrt((double)sx * sx + (double)sz * sz);
                if (m > mg) mg = m;
            }
        }
    return mv + mg + 2200;
}

/* ============================================================================================================== */
static void teil_austritt(void)
{
    printf("== austritt (ROOM4001, Ck(4,20)=1, Ck(3,0x21)=1) ==\n");
    s_rueck_modul = 0; s_rueck_alt = 0; s_taste = 0; s_br_ende = 0; s_br_p2 = 0; s_br_fehler = 0;
    static const uint8_t fl[][2] = { {4, 20}, {3, 0x21} };
    if (!raum_4001(fl, 2, -8885, -6301, 1024)) { printf("  SKIP: Assets\n"); s_fails += 0; return; }
    for (int f = 0; f < 8; f++) bild();
    int sl[4]; int n = spiderman_slots(sl);
    CHECK(n == 2, "zwei Spiderman gespawnt (sub00 @0x0122C/@0x012BE), gefunden %d", n);
    if (n != 2) return;
    re15_actor_t *a = &g_actors[sl[0]], *b = &g_actors[sl[1]];
    int32_t ax = a->x, az = a->z;
    int alles_versteckt = 1, nie_ziel = 1, nie_schatten = 1;
    for (int f = 0; f < 600; f++) {
        bild();
        for (int k = 0; k < 2; k++) {
            re15_actor_t *e = &g_actors[sl[k]];
            if (!(e->state == 4 && e->sub_state_1 == 1 && e->sub_state_2 == 1 && e->grid_id == 0x41 &&
                  e->no_draw == 1 && (e->flags & 0x42) == 0x42)) alles_versteckt = 0;
            if (!re15_r38_sm_versteckt(e)) nie_ziel = 0;
            if (!re15_r38_sm_schatten_aus(e)) nie_schatten = 0;
        }
    }
    CHECK(alles_versteckt, "600 Bilder: beide in P1 (Zustand 4, +0x5 1, +0x6 1, grid 0x41, nicht gezeichnet, word0&0x42)");
    CHECK(a->x == ax && a->z == az, "Spiderman bewegt sich im Schacht nicht (%d,%d)", (int)a->x, (int)a->z);
    CHECK(nie_ziel, "im Schacht kein Ziel (grid & 0x60 @0x8003710c)");
    CHECK(nie_schatten, "im Schacht kein Schatten (+0xBC/+0xBE = 0 @0x80115370-74)");
    { int32_t tx, tz; CHECK(re15_player_aim_target(30000, &tx, &tz) == 0, "Aim-Latch findet kein Ziel"); }
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 100, "kein Angriff aus dem Schacht (hp %d)", g_actors[RE15_ACTOR_SLOT_PLAYER].hp);

    /* Spieler in den Ausloeser Aot 4 (-11727,-16440, 6000 x 5400) — von aussen hinein. */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int t42 = -1, t_sicht = -1, t_frei = -1, t_aktiv = -1;
    for (int f = 0; f < 400 && t_aktiv < 0; f++) {
        if (t42 < 0 && pl->z > -12000) { int32_t nz = pl->z - 75; pl->z = nz; }   /* hineinlaufen (75/Bild) */
        bild();
        if (t42 < 0 && a->grid_id == 0x42) t42 = f;
        if (t42 >= 0 && t_sicht < 0 && a->no_draw == 0) t_sicht = f;
        if (t42 >= 0 && t_frei < 0 && a->grid_id == 0 && (a->flags & 0x42) == 0) t_frei = f;
        if (t42 >= 0 && t_aktiv < 0 && a->state == 1) t_aktiv = f;
    }
    CHECK(t42 >= 0, "Raumskript setzt grid 0x42 beim Betreten von Aot 4 (sub05 @0x015BA), Bild %d", t42);
    if (t42 < 0) return;
    /* t42 = Bild, in dem P1 die 0x42 sieht (P2 laeuft im selben Bild mit Bild 0 an). */
    CHECK(t_sicht - t42 == 6, "sichtbar im 7. Austrittsbild (Bild 6 @0x8011549C): +%d", t_sicht - t42);
    CHECK(t_frei - t42 == 0x21, "greifbar + grid 0 bei Bild 0x21 (@0x80115508-64): +%d", t_frei - t42);
    CHECK(t_aktiv - t42 == 34, "ACTIVE nach Clip 0x1D (35 Bilder, 0x101 @0x80115588-98): +%d", t_aktiv - t42);
    CHECK(b->state == 4 && b->grid_id == 0x41 && b->no_draw == 1, "Gegenprobe: zweiter Spiderman (Aot 5) bleibt versteckt");
    if (t_aktiv < 0) return;

    /* NACHBESSERUNG 1 / M1: das Wechselbild (Zustand 4 -> 1) zeigt Clip 0x1D Bild fc-1, nicht Bild 0. */
    const int sa = (int)(a - g_actors);
    const int fc1d = clip_fc(0x1D);
    gez_t w = s_gez[sa];
    int32_t r_end[3], r_null[3];
    clip_wurzel(0x1D, fc1d - 1, r_end); clip_wurzel(0x1D, 0, r_null);
    printf("  Wechselbild: [%d/%d/%d] Clip 0x%X Bild %d, gezeichnete Wurzel (%d,%d,%d); Clip 0x1D Bild %d (%d,%d,%d), "
           "Bild 0 (%d,%d,%d)\n", w.st, w.ss1, w.ss2, w.motion, w.frame, (int)w.root[0], (int)w.root[1], (int)w.root[2],
           fc1d - 1, (int)r_end[0], (int)r_end[1], (int)r_end[2], (int)r_null[0], (int)r_null[1], (int)r_null[2]);
    CHECK(w.st == 1 && w.motion == 0x1D && w.frame == fc1d - 1,
          "M1: Wechselbild zeigt Clip 0x1D Bild %d (FUN_8001f3bc: Pose vor dem Hochzaehlen @0x8001f534, Wrap @0x8001f63c), "
          "gezeichnet Clip 0x%X Bild %d", fc1d - 1, w.motion, w.frame);
    CHECK(w.root[0] == r_end[0] && w.root[1] == r_end[1] && w.root[2] == r_end[2],
          "M1: gezeichnete Wurzel im Wechselbild = Clip 0x1D Bild %d (nicht die Startpose)", fc1d - 1);
    bild();
    gez_t na = s_gez[sa];
    int32_t r_neu[3]; clip_wurzel(na.motion, na.frame, r_neu);
    double d_ist = abst3(na.root, r_end), d_ziel = abst3(r_neu, r_end);
    printf("  erstes ACTIVE-Bild: [%d/%d/%d] Clip 0x%X Bild %d frac %d, gezeichnete Wurzel (%d,%d,%d), Abstand zur "
           "Austritts-Endpose %.0f (Clip-Ziel %.0f)\n", na.st, na.ss1, na.ss2, na.motion, na.frame, (int)a->anim_frac,
           (int)na.root[0], (int)na.root[1], (int)na.root[2], d_ist, d_ziel);
    CHECK(na.motion != 0x1D && a->anim_frac > 0,
          "erstes ACTIVE-Bild: neuer Clip mit Ueberblendung (+0x95 = 0 @0x80112164/@0x80112300, +0x8F = 7 @0x80112184/@0x80112320)");
    CHECK(d_ist <= d_ziel + 1.0,
          "M1: Ueberblendung startet an der Austritts-Endpose (Abstand %.0f <= %.0f), nicht an der Startpose", d_ist, d_ziel);
    /* Weiter bis das Bruellen (B[2], erste Handlung nach dem Austritt) vorbei ist und P2 auf +0x5 = 3 schaltet. */
    for (int f = 0; f < 200 && s_br_p2 == 0 && s_br_fehler == 0; f++) bild();
    printf("  Bruellen: Fertig-Bild %d, P2-Bild %d, Fehler %d (Clip 2 fc %d)\n", s_br_ende, s_br_p2, s_br_fehler,
           clip_fc(2));
    CHECK(s_br_ende == 1 && s_br_p2 == 1 && s_br_fehler == 0,
          "Bruellen B[2]: Fertig-Bild und P2-Bild zeigen Clip 2 Bild fc-1, +0x5 = 3 erst im P2-Bild (@0x80112374-84, "
          "@0x80112390-D4)");
    CHECK(s_rueck_modul == 0, "kein Ruecksprung auf Bild 0 in einem Wechselbild des Austritts: %d", s_rueck_modul);
}

/* ============================================================================================================== */
/* NACHBESSERUNG 1 / M2: Wechselbilder der Griff-Phasen des Greifers (slot g) im Bild nach bild() pruefen.
 * Griff 1 P1 -> P2 (+0x6 += fertig @0x80112D8C-9C / @0x80113268-78): Zugriffs-Clip Bild fc-1;
 * erstes Halte-Bild (P2 ohne +0x95 @0x80112B9C / @0x80113094 -> P3): Halte-Clip Bild 1 (= Wrap 0 + ein anim_set);
 * Loese-Clip P5 -> P6 und P6 (ohne anim_set @0x80112DA0 / @0x8011327C): Bild fc-1; Kollaps P1 -> P2: Bild fc-1. */
typedef struct { int zugriff_ende, halte_start, loese_ende, p6_bild, kollaps_ende, fehler; } griffwechsel_t;
static void griff_wechsel(int g, griffwechsel_t *w)
{
    const gez_t *p = &s_gez_alt[g], *c = &s_gez[g];
    if (p->st != 1 || c->st != 1) return;
    if ((p->ss1 == 5 || p->ss1 == 9) && p->ss2 == 1 && c->ss2 == 2) {
        int soll = clip_fc(c->motion) - 1;
        w->zugriff_ende++;
        if (!((c->motion == 0x0E || c->motion == 0x12) && c->frame == soll)) {
            w->fehler++; printf("  M2 FEHLER: Zugriff-Ende Clip 0x%X Bild %d (soll %d)\n", c->motion, c->frame, soll);
        }
    }
    /* P2 faellt in P3; mit Tasten kann P3 im selben Bild schon +0x6 = 4 setzen (@0x80112CBC / @0x80113160). */
    if ((c->ss1 == 5 || c->ss1 == 9) && (c->ss2 == 3 || c->ss2 == 4) && p->ss2 == 2) {
        w->halte_start++;
        if (!((c->motion == 0x0F || c->motion == 0x13) && c->frame == 1)) {
            w->fehler++; printf("  M2 FEHLER: erstes Halte-Bild Clip 0x%X Bild %d (soll 1)\n", c->motion, c->frame);
        }
    }
    if ((p->ss1 == 5 || p->ss1 == 9) && p->ss2 == 5 && c->ss2 == 6) {
        int soll = clip_fc(c->motion) - 1;
        w->loese_ende++;
        if (!((c->motion == 0x10 || c->motion == 0x14) && c->frame == soll)) {
            w->fehler++; printf("  M2 FEHLER: Loese-Ende Clip 0x%X Bild %d (soll %d)\n", c->motion, c->frame, soll);
        }
    }
    if ((p->ss1 == 5 || p->ss1 == 9) && p->ss2 == 6 && c->ss1 == 3) {
        int soll = clip_fc(c->motion) - 1;
        w->p6_bild++;
        if (!((c->motion == 0x10 || c->motion == 0x14) && c->frame == soll)) {
            w->fehler++; printf("  M2 FEHLER: P6-Bild Clip 0x%X Bild %d (soll %d)\n", c->motion, c->frame, soll);
        }
    }
    if ((p->ss1 == 8 || p->ss1 == 10) && p->ss2 == 1 && c->ss2 == 2) {
        int soll = clip_fc(c->motion) - 1;
        w->kollaps_ende++;
        if (c->frame != soll) {
            w->fehler++; printf("  M2 FEHLER: Kollaps-Ende Clip 0x%X Bild %d (soll %d)\n", c->motion, c->frame, soll);
        }
    }
}

static void griff_lauf(const char *name, int32_t px, int32_t pz, int losreissen)
{
    printf("== %s (Spiderman draussen, Spieler-Anker vergiftet%s) ==\n", name, losreissen ? ", Tasten" : "");
    s_rueck_modul = 0; s_rueck_alt = 0; s_taste = 0; s_br_ende = 0; s_br_p2 = 0; s_br_fehler = 0;
    griffwechsel_t gw; memset(&gw, 0, sizeof gw);
    static const uint8_t fl[][2] = { {4, 20}, {3, 0x21}, {3, 0x91}, {3, 0xAF} };
    if (!raum_4001(fl, 4, px, pz, 1024)) { printf("  SKIP: Assets\n"); return; }
    huelle();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 40;                                       /* < 50 nach dem ersten Treffer -> Griff 1 @0x801128ac */
    pl->anchor_x = 30000; pl->anchor_z = 30000;        /* Anker eines frueheren Griffs (Lage vor dem Fix) */
    const int32_t reich = griff_reichweite();
    int latch = -1, anker_ok = 0, griffbilder = 0, aus_raum = 0, durch = 0, zu_weit = 0, varianten = 0;
    int32_t vor_x = pl->x, vor_z = pl->z, maxd = 0, maxsprung = 0, lx = pl->x, lz = pl->z;
    int greifer = -1;
    for (int f = 0; f < 3000; f++) {
        int war = re15_player_victim_state();
        int32_t ox = pl->x, oz = pl->z;
        /* Losreissen: Richtungs-Kanten jedes zweite Bild (Maske 0xF0F0 = FUN_80037024-Zwilling re15_mash_pressed) */
        s_taste = (losreissen && latch >= 0 && (f & 1)) ? (uint16_t)RE15_PAD_BIT_LEFT : 0;
        bild();
        if (greifer >= 0) griff_wechsel(greifer, &gw);
        int ist = re15_player_victim_state();
        if (!war && ist && latch < 0) {
            latch = f; vor_x = ox; vor_z = oz;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == RE15_R38_SM_TYP &&
                    (g_actors[s].sub_state_1 == 5 || g_actors[s].sub_state_1 == 9)) greifer = s;
            if (greifer >= 0)
                anker_ok = (pl->anchor_x == g_actors[greifer].anchor_x && pl->anchor_z == g_actors[greifer].anchor_z);
            varianten = re15_player_victim_variant();
            printf("  Latch Bild %d: Greifer slot %d +0x5=%d, Spieler-Anker (%d,%d), Greifer-Anker (%d,%d), aca59 %d\n",
                   f, greifer, greifer >= 0 ? g_actors[greifer].sub_state_1 : -1, (int)pl->anchor_x, (int)pl->anchor_z,
                   greifer >= 0 ? (int)g_actors[greifer].anchor_x : 0, greifer >= 0 ? (int)g_actors[greifer].anchor_z : 0,
                   varianten);
        }
        if (ist && latch >= 0) {
            griffbilder++;
            if (!im_raum(pl->x, pl->z)) aus_raum++;
            if (durch_wand(vor_x, vor_z, pl->x, pl->z)) durch++;
            if (greifer >= 0) {
                int32_t d = re15_enemy_player_dist(&g_actors[greifer], pl);
                if (d > maxd) maxd = d;
                if (d > reich) zu_weit++;
            }
            int32_t sp = (int32_t)sqrt((double)(pl->x - lx) * (pl->x - lx) + (double)(pl->z - lz) * (pl->z - lz));
            if (sp > maxsprung) maxsprung = sp;
        }
        lx = pl->x; lz = pl->z;
        if (latch >= 0 && f > latch + 400) break;
    }
    CHECK(latch >= 0, "Griff 1 kommt zustande (Bild %d)", latch);
    if (latch < 0) return;
    s_taste = 0;
    CHECK(anker_ok, "Latch: Spieler-Anker == Spiderman-Anker (FUN_8001ac38-Kopie @0x80112B08/@0x80113000)");
    if (!losreissen) CHECK(griffbilder > 50, "%d Griff-/Kollapsbilder gemessen", griffbilder);
    printf("  Wechselbilder: Zugriff-Ende %d, Halte-Start %d, Loese-Ende %d, P6 %d, Kollaps-Ende %d, Fehler %d; "
           "Ruecksprung Modul %d, Alt-Pfad %d\n", gw.zugriff_ende, gw.halte_start, gw.loese_ende, gw.p6_bild,
           gw.kollaps_ende, gw.fehler, s_rueck_modul, s_rueck_alt);
    CHECK(gw.zugriff_ende >= 1 && gw.halte_start >= 1,
          "M2: Zugriff-Ende (Clip 0xE/0x12 Bild fc-1) und erstes Halte-Bild (0xF/0x13 Bild 1, Wrap 0 @0x8001f63c) gemessen");
    if (losreissen)
        CHECK(gw.loese_ende >= 1 && gw.p6_bild >= 1,
              "M2: Losreissen gemessen: Loese-Ende P5 -> P6 und P6-Bild (0x10/0x14 Bild fc-1, P6 ohne anim_set)");
    else
        CHECK(gw.kollaps_ende >= 1, "Kollaps-Ende gemessen (0x11/0x15 Bild fc-1)");
    CHECK(gw.fehler == 0, "M2: jedes gemessene Wechselbild zeigt das letzte Bild des fertigen Clips: %d Fehler", gw.fehler);
    CHECK(s_br_fehler == 0, "Bruellen B[2] in diesem Lauf: %d Fertig-, %d P2-Bilder, %d Fehler", s_br_ende, s_br_p2,
          s_br_fehler);
    CHECK(s_rueck_modul == 0, "M2: kein Ruecksprung auf Bild 0 in einem Wechselbild der Griffe: %d", s_rueck_modul);
    CHECK(aus_raum == 0, "Spieler in jedem Griffbild innerhalb der Raumhuelle (%d,%d)..(%d,%d): %d ausserhalb",
          (int)s_hx0, (int)s_hz0, (int)s_hx1, (int)s_hz1, aus_raum);
    CHECK(durch == 0, "kein Griffbild hinter einer Wand (Strecke ab Lage vor dem Griff): %d", durch);
    CHECK(zu_weit == 0, "Abstand Spieler-Spiderman <= Griffreichweite %d (Bankdaten), max %d", (int)reich, (int)maxd);
    CHECK(maxsprung < reich, "groesster Bild-zu-Bild-Sprung im Griff %d (< %d)", (int)maxsprung, (int)reich);
}

/* ============================================================================================================== */
static void teil_griff2(void)
{
    printf("== griff2 (B[15] 0x8011411C erzwungen) ==\n");
    static const uint8_t fl[][2] = { {4, 20}, {3, 0x21}, {3, 0x91}, {3, 0xAF} };
    if (!raum_4001(fl, 4, -12000, -9000, 1024)) { printf("  SKIP: Assets\n"); return; }
    for (int f = 0; f < 4; f++) bild();
    int sl[4]; int n = spiderman_slots(sl);
    CHECK(n >= 1, "Spiderman vorhanden (%d)", n);
    if (n < 1) return;
    re15_actor_t *e = &g_actors[sl[0]];
    for (int k = 1; k < n; k++) { g_actors[sl[k]].x = 30000; g_actors[sl[k]].z = 30000; g_actors[sl[k]].active = 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->anchor_x = 30000; pl->anchor_z = 30000;
    /* Spiderman 1600 vor den Spieler stellen, Blick zum Spieler; Griff 2 starten (+0x5 = 0xF, +0x6 = 0). */
    e->x = pl->x; e->z = pl->z - 1600; e->rot_y = 3072;
    pl->rot_y = 1024; pl->hp = 80;
    e->state = 1; e->sub_state_1 = 15; e->sub_state_2 = 0; e->sub_state_3 = 0; e->grid_id = 0;
    int32_t ex0 = e->x, ez0 = e->z;
    int w = ((int)e->rot_y + 0x800) & 0xfff;
    int32_t sx = ex0 + (int32_t)((int64_t)re15_cos_q12(w) * 0x66C >> 12);
    int32_t sz = ez0 + (int32_t)((-(int64_t)re15_sin_q12(w)) * 0x66C >> 12);
    int16_t blick0 = pl->rot_y;
    bild();
    int32_t gx = pl->x, gz = pl->z;
    printf("  Spiderman (%d,%d) r%d, Soll-Lage (%d,%d), Spieler (%d,%d), Opfer %d Var %d\n", (int)ex0, (int)ez0,
           (int)e->rot_y, (int)sx, (int)sz, (int)gx, (int)gz, re15_player_victim_state(), re15_player_victim_variant());
    int32_t nx = sx, nz = sz;
    re15_collision_constrain(&g_room_rdt, -12000, -9000, &nx, &nz);
    CHECK(gx == nx && gz == nz, "einmalige Spielerlage = Lage + 0x66C hinter dem Blick, wandgeklemmt (%d,%d)", (int)nx, (int)nz);
    CHECK(e->x == ex0 && e->z == ez0, "Spiderman bleibt stehen (Lage zurueck @0x80114218-38)");
    CHECK(re15_player_victim_state() == 1 && re15_player_victim_variant() >= 2, "Opfer-Maschine Griff 2 (aca59 2/3)");
    CHECK(pl->rot_y == blick0, "kein Yaw-Schreiben am Spieler (0x80115FF0 ohne a8f8)");
    int bewegt = 0, clip_ok = 1;
    /* NACHBESSERUNG 1 / M2 (Griff 2): Zugriff 0x18 P1 -> P2 (+0x6 += fertig @0x80114400-10) Bild fc-1; erstes
     * Halte-Bild 0x19 (P2 ohne +0x95 @0x801142C8-F4 -> P3) Bild 1; Loese-Clip 0x1A P5 -> P6 und P6 (kein `jal` bis
     * @0x80114484) Bild fc-1. */
    const int se = sl[0];
    int z_ende = 0, h_start = 0, l_ende = 0, p6 = 0, fehler2 = 0;
#define G2_WECHSEL() do { const gez_t *p_ = &s_gez_alt[se], *c_ = &s_gez[se];                                        \
        if (p_->ss1 == 15 && p_->ss2 == 1 && c_->ss2 == 2) { z_ende++;                                                  \
            if (!(c_->motion == 0x18 && c_->frame == clip_fc(0x18) - 1)) { fehler2++;                                  \
                printf("  M2 FEHLER (Griff 2): Zugriff-Ende Clip 0x%X Bild %d\n", c_->motion, c_->frame); } }         \
        if (c_->ss1 == 15 && p_->ss2 == 2 && c_->ss2 == 3) { h_start++;                                                 \
            if (!(c_->motion == 0x19 && c_->frame == 1)) { fehler2++;                                                   \
                printf("  M2 FEHLER (Griff 2): erstes Halte-Bild Clip 0x%X Bild %d\n", c_->motion, c_->frame); } }    \
        if (p_->ss1 == 15 && p_->ss2 == 5 && c_->ss2 == 6) { l_ende++;                                                  \
            if (!(c_->motion == 0x1A && c_->frame == clip_fc(0x1A) - 1)) { fehler2++;                                  \
                printf("  M2 FEHLER (Griff 2): Loese-Ende Clip 0x%X Bild %d\n", c_->motion, c_->frame); } }           \
        if (p_->ss1 == 15 && p_->ss2 == 6 && c_->ss1 == 0x0D) { p6++;                                                   \
            if (!(c_->motion == 0x1A && c_->frame == clip_fc(0x1A) - 1)) { fehler2++;                                  \
                printf("  M2 FEHLER (Griff 2): P6-Bild Clip 0x%X Bild %d\n", c_->motion, c_->frame); } } } while (0)
    s_rueck_modul = 0; s_rueck_alt = 0; s_taste = 0; s_br_ende = 0; s_br_p2 = 0; s_br_fehler = 0;
    for (int f = 0; f < 60 && re15_player_victim_state(); f++) {
        bild();
        G2_WECHSEL();
        if (pl->x != gx || pl->z != gz) bewegt++;
        if (re15_player_victim_state() == 1 && pl->motion != 8 && pl->motion != 9) clip_ok = 0;
    }
    CHECK(bewegt == 0, "danach keine Spielerbewegung mehr (Hook A [2]/[3] ohne ad68): %d Bilder bewegt", bewegt);
    CHECK(clip_ok, "Opfer-Clips 8 (Zugriff @0x80116044) / 9 (Halten @0x801160C8)");
    CHECK(e->sub_state_1 == 15 && e->x == ex0 && e->z == ez0, "Spiderman haelt (+0x5 0xF, keine Schleppbewegung)");
    /* Losreissen (Tasten *8 @0x80114358) bis P6 -> +0x5 = 0xD, danach noch zwei Bilder. */
    int nach = -1;
    for (int f = 0; f < 400 && (nach < 0 || f < nach + 2); f++) {
        s_taste = (f & 1) ? (uint16_t)RE15_PAD_BIT_LEFT : 0;
        bild();
        G2_WECHSEL();
        if (nach < 0 && e->sub_state_1 == 0x0D) nach = f;
    }
    s_taste = 0;
#undef G2_WECHSEL
    printf("  Griff-2-Wechselbilder: Zugriff-Ende %d, Halte-Start %d, Loese-Ende %d, P6 %d, Fehler %d; Ruecksprung %d\n",
           z_ende, h_start, l_ende, p6, fehler2, s_rueck_modul);
    CHECK(z_ende == 1 && h_start == 1 && l_ende == 1 && p6 == 1,
          "M2 Griff 2: alle vier Wechselbilder gemessen (Zugriff-Ende, Halte-Start, Loese-Ende, P6)");
    CHECK(fehler2 == 0, "M2 Griff 2: Wechselbilder zeigen das letzte Bild des fertigen Clips bzw. Halte-Bild 1: %d Fehler",
          fehler2);
    CHECK(s_rueck_modul == 0, "M2 Griff 2: kein Ruecksprung auf Bild 0: %d", s_rueck_modul);
}

int main(void)
{
    char p[600]; FILE *f;
    snprintf(p, sizeof p, "%s/STAGE4/ROOM4001.RDT", RE15_ASSET_PSX_DIR);
    if (!(f = fopen(p, "rb"))) { printf("SKIP: %s fehlt\n", p); return 77; }
    fclose(f);
    teil_austritt();
    griff_lauf("griff1 vorn/hinten Lauf A", -12000, -9000, 0);
    griff_lauf("griff1 Lauf B (Ecke)", -14000, -6000, 0);
    griff_lauf("griff1 Losreissen", -12000, -9000, 1);
    teil_griff2();
    printf("test_r38_spiderman: %d FAIL\n", s_fails);
    return s_fails ? 1 : 0;
}
