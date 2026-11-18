/* test_r42_arme_re15_modell.c — RIEGEL (Runde 42, Strang "original", Stufe 2, 18.11.2026):
 * Die ROOM1210/1211-Gitterhaende zeichnen das RE1.5-Modell EM1A, das Verhalten bleibt das RE2-Zellenarm-Gehirn.
 *
 * NUTZERWUNSCH 18.11.2026, woertlich: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2
 * übernommen. Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das
 * Modell darf schon noch das aus Resident Evil 1.5 sein...."
 *
 * Belege im Modul engine/src/enemy_ai_re2_zellenarm.c, Block MODELL. Der Test laedt die Banken wie der Lader in
 * platform/pc/main.c: EM1A ueber den EXE-TOC (@0x80072f38 Leon / @0x80073178 Elza, Eintrag 0x1A) als Zeichen-Bank,
 * EM2D (RE2 CDEMD0.EMS kind 0x2D) als Logik-Bank, dann re15_re2arm_modell_einrichten.
 *
 * GEGENPROBE: auf aaea5f8c2 gibt es re15_re2arm_modell_einrichten / _modell_re15 / _em1a_clip / _takt nicht. Sie sind
 * hier SCHWACH deklariert: der Test baut dort trotzdem und faellt in den Pruefungen (Maske, Opferbank, Hoehe, Clips,
 * Rueckzug), statt am Linker zu scheitern.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"
#include "re15_roy_r37.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

/* schwach: fehlen auf dem Stand vor Stufe 2 (Gegenprobe) */
extern void re15_re2arm_modell_einrichten(re15_enemy_bank_t *anzeige, const re15_enemy_bank_t *em2d_logik) __attribute__((weak));
extern int  re15_re2arm_modell_re15(const re15_actor_t *e) __attribute__((weak));
extern int  re15_re2arm_em1a_clip(int re2_clip) __attribute__((weak));
extern int  re15_re2arm_takt(int slot, int *re2_clip, int *re2_bild) __attribute__((weak));

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems15e = NULL; static size_t s_ems15e_n = 0;
static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static uint8_t *s_em1a = NULL;  static size_t s_em1a_n = 0;
static re15_enemy_bank_t s_em2d;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static const uint8_t *toc_blob(const uint8_t *ems, size_t n, int archiv, size_t *len)
{
    uint32_t sek = 0, lg = 0;
    if (!ems || !re15_ems_toc(archiv, 0x1A, &sek, &lg)) return NULL;     /* Index type-0x10 @0x80022338 */
    if ((size_t)sek * 2048u + lg > n) return NULL;
    *len = lg; return ems + (size_t)sek * 2048u;
}
/* Banken wie platform/pc/main.c (pc_enemy_load_ex, RE1.5-Weg + Stufe-2-Anmeldung). */
static int banken_laden(void)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems2)  s_ems2  = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems15 || !s_ems2) return 0;
    if (!s_em1a) {
        size_t lg = 0; const uint8_t *b = toc_blob(s_ems15, s_ems15_n, RE15_EMS_ARCHIV_LEON, &lg);
        if (!b) return 0;
        s_em1a = (uint8_t *)malloc(lg); memcpy(s_em1a, b, lg); s_em1a_n = lg;
    }
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (re15_emd_parse_container(s_em1a, s_em1a_n, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_em1a, s_em1a_n, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em1a, s_em1a_n, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_em1a, s_em1a_n, &eb->skel_own, &eb->anim_own) == 0);
    eb->ok = 1; eb->buf = NULL;
    memset(&s_em2d, 0, sizeof s_em2d);
    re15_tim_t t2 = {0};
    if (re2_ems_load_bank(s_ems2, s_ems2_n, 0x2D, &s_em2d, &t2) != 0) return 0;
    if (re15_re2arm_modell_einrichten) re15_re2arm_modell_einrichten(eb, &s_em2d);
    return 1;
}
static int s_se[64], s_se_n = 0;
static void se_cap(int id, int f) { if (s_se_n < 64) s_se[s_se_n++] = id | (f << 8); }
static void bank_cap(int b) { (void)b; }

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static uint32_t word(const re15_actor_t *e)
{ return (uint32_t)e->state | ((uint32_t)e->sub_state_1 << 8) | ((uint32_t)e->sub_state_2 << 16); }
static int takt_clip(int slot) { int c = -1; if (re15_re2arm_takt) re15_re2arm_takt(slot, &c, NULL); return c; }

static int aufsetzen(int *slots)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    int ok = banken_laden();                             /* NACH dem Reset */
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    CHECK("(M0) Zeichen-Bank 0x1A = RE1.5-EM1A ueber den EXE-TOC: 4 Knochen, 4 Meshes, 4 Clips 30/39/30/54",
          ok && eb && eb->skel.bone_count == 4 && eb->md1.mesh_count == 4 && eb->anim.clip_count == 4 &&
          eb->anim.clips[0].frame_count == 30 && eb->anim.clips[1].frame_count == 39 &&
          eb->anim.clips[2].frame_count == 30 && eb->anim.clips[3].frame_count == 54);
    re15_re2arm_audio_hook(se_cap, bank_cap);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1210.RDT fehlt\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("=== ROOM1210 Gitterhaende: RE1.5-Modell EM1A, RE2-Verhalten (Runde 42 Stufe 2) ===\n");

    /* Elza: dasselbe Modell aus CDEMD1 (TOC @0x80073178) — Datenvoraussetzung, kein Port-Verhalten. */
    {   s_ems15e = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD1.EMS", &s_ems15e_n);
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        size_t l0 = 0, l1 = 0;
        const uint8_t *b0 = toc_blob(s_ems15, s_ems15_n, RE15_EMS_ARCHIV_LEON, &l0);
        const uint8_t *b1 = toc_blob(s_ems15e, s_ems15e_n, RE15_EMS_ARCHIV_ELZA, &l1);
        CHECK("(D) EM1A Leon (CDEMD0) == Elza (CDEMD1), 0x12EC0 Byte", b0 && b1 && l0 == 0x12EC0u && l1 == l0 &&
              memcmp(b0, b1, l0) == 0); }

    int slots[RE15_ACTOR_MAX]; int n = aufsetzen(slots);
    CHECK("zehn Arme gespawnt", n == 10);
    if (n < 10) return 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    int a5 = slots[4];
    re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    printf("  arm%d Heimat=(%ld,%ld) y=%ld yaw=%d f10e=0x%02x\n", a5, (long)hx, (long)hz, (long)arm->y, (int)hyaw,
           (unsigned)arm->re2z_f10e);

    CHECK("(M1) das Gehirn erkennt das RE1.5-Modell (re15_re2arm_modell_re15)",
          re15_re2arm_modell_re15 && re15_re2arm_modell_re15(arm) == 1);
    CHECK("(M2) alle vier EM1A-Teile werden gezeichnet (Rumpf, Kopf, beide Arme): Part-Maske 0",
          re15_re2arm_part_hide_mask(arm) == 0);
    CHECK("(M3) Leons Opfer-Clips sind die RE2-Funktion: Paar 3 aus EM2D (2 Clips, gleiche Laengen) auf der Zeichen-Bank",
          eb->victim_ok && s_em2d.victim_ok && eb->anim_victim.clip_count == 2 &&
          eb->anim_victim.clips[0].frame_count == s_em2d.anim_victim.clips[0].frame_count &&
          eb->anim_victim.clips[1].frame_count == s_em2d.anim_victim.clips[1].frame_count);
    {   int32_t r[3]; re15_enemy_bone_world_pos(arm, 0, r);
        printf("  Anker y=%ld, EM1A-Rumpf (Knochen 0) Welt-y=%ld\n", (long)arm->y, (long)r[1]);
        CHECK("(M4) Hoehe: Anker bleibt RE2 (y -2500), das bodenbezogene EM1A steht auf Record-y 0 (Rumpf-Wurzel -1855 +- Anim)",
              arm->y == -2500 && r[1] > -2000 && r[1] < -1700); }
    CHECK("(M5) Clip-Zuordnung RE2 -> EM1A = 0,1,0,2,3,1",
          re15_re2arm_em1a_clip && re15_re2arm_em1a_clip(0) == 0 && re15_re2arm_em1a_clip(1) == 1 &&
          re15_re2arm_em1a_clip(2) == 0 && re15_re2arm_em1a_clip(3) == 2 && re15_re2arm_em1a_clip(4) == 3 &&
          re15_re2arm_em1a_clip(5) == 1);

    /* (1) SCHLAF / (2) WECKRUF / (3) A0 — Verhalten wie RE2 (Werte wie unit_1210_arme_re2) */
    pl->x = hx + 1500; pl->z = hz;
    for (int f = 0; f < 60; f++) frame_step();
    CHECK("(1) ohne Weckruf tickt der Arm nicht (@0x8002659C): 0x000001, verborgen", word(arm) == 0x000001u && arm->no_draw == 1);
    pl->x = hx + 3500; pl->z = hz + 3500;
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;      /* Member_set(12,1) @0x1EDA */
    frame_step();
    CHECK("(2) B0 P0: 400 hinter die Heimat (@0x801006F8-718)", word(arm) == 0x010001u && arm->x == hx - 400);
    {   int f = 0; while (f < 20 && arm->sub_state_2 != 2) { frame_step(); f++; } }
    pl->x = hx + 2000; pl->z = hz;
    frame_step();
    CHECK("(3) A0 2500/Kegel 1024 -> 0x101 REACH, sichtbar, Heimat (@0x80100564-9C)",
          arm->state == 1 && arm->sub_state_1 == 1 && arm->no_draw == 0 && arm->x == hx && arm->z == hz);
    CHECK("(3b) REACH zeichnet EM1A-Clip 0/1 (RE2 rng&1 @0x80100878 -> dieselbe Nummer)",
          (arm->motion == 0 || arm->motion == 1) && takt_clip(a5) == (int)arm->motion);

    /* (4) Hand = EM1A-Handgelenk (Ring Quad-Vertices 4..7 von Mesh 2/3) */
    int32_t hand[3] = {0, 0, 0};
    re15_re2arm_hand_parts(a5, NULL, hand);
    {   double d = sqrt((double)(hand[0] - arm->x) * (hand[0] - arm->x) + (double)(hand[2] - arm->z) * (hand[2] - arm->z));
        printf("  Hand (EM1A-Knochen %d) = (%ld,%ld,%ld), %.0f vor dem Ursprung\n", re15_re2arm_hand_bone(arm),
               (long)hand[0], (long)hand[1], (long)hand[2], d);
        CHECK("(4a) Hand-Knochen Arm A = EM1A-Arm 2; Hand-Weltpunkt = Handgelenk 1200..1500 vor dem Ursprung, auf Armhoehe",
              re15_re2arm_hand_bone(arm) == 2 && d > 1200.0 && d < 1500.0 && hand[1] < -1900 && hand[1] > -2900); }
    pl->x = hand[0] + 300; pl->z = hand[2];
    {   int f1 = 0; do { frame_step(); f1++; } while (f1 < 30 && arm->sub_state_1 == 1); }
    CHECK("(4b) A1 Hand < 900 -> 0x301 ZUGRIFF; gezeichnet EM1A-Clip 2 (Ausfahren), Takt RE2-Clip 3",
          arm->state == 1 && arm->sub_state_1 == 3 && arm->motion == 2 && takt_clip(a5) == 3);
    int grab_f = -1;
    for (int f = 0; f < 40 && grab_f < 0; f++) { frame_step(); if (arm->sub_state_1 == 4) grab_f = f; }
    CHECK("(4c) A3 Hand < 600 ab RE2-Bild 5 -> 0x401 HALTEN (@0x801009BC-A54)", grab_f >= 0);
    if (grab_f >= 0) {
        int32_t h2[3]; re15_re2arm_hand_parts(a5, h2, NULL);
        double hxr = h2[0] - arm->x, hzr = h2[2] - arm->z, lx = pl->x - arm->x, lz = pl->z - arm->z;
        double ld = sqrt(lx * lx + lz * lz), kreuz = (hxr * lz - hzr * lx) / (sqrt(hxr * hxr + hzr * hzr) * ld + 1e-9);
        printf("  Griff-Bild %d: pl=(%ld,%ld) Hand=(%ld,%ld) Leon %.0f vom Ursprung, sin %.4f\n", grab_f, (long)pl->x,
               (long)pl->z, (long)h2[0], (long)h2[2], ld, kreuz);
        /* FUN_80034D0C schiebt nur, wenn Leon naeher als r 800 + 450 am Ursprung steht (@0x80100338-3C / @0x8003bdc0-c4);
         * die EM1A-Hand kann weiter draussen liegen — dann bleibt Leon auf der Hand. */
        const double dh = sqrt(hxr * hxr + hzr * hzr), soll = (dh >= 1250.0) ? dh : 1251.0;
        printf("  Hand %.0f vom Ursprung -> Soll-Abstand Leon %.0f\n", dh, soll);
        CHECK("(4d) Pin an die EM1A-Hand (@0x80100C18-38), danach der RE2-Push (FUN_80034D0C, r 1250), Victim-Modus 4",
              re15_player_is_grabbed() && re15_player_victim_state() == 4 && re15_re2arm_holder_slot() == a5 &&
              fabs(ld - soll) <= 1.5 && fabs(kreuz) < 0.01);
        CHECK("(4e) HALTEN zeichnet EM1A-Clip 1 (Takt RE2-Clip 5), +0x15A = 150; Leon Opfer-Clip 0",
              arm->motion == 1 && takt_clip(a5) == 5 && arm->re2z_t15a == 150 && pl->motion == 0);
    }
    int rel_f = -1;
    for (int f = 0; f < 200 && rel_f < 0; f++) { frame_step(); if (arm->sub_state_1 == 5) rel_f = f + 1; }
    printf("  Loesen nach %d Bildern, cd=%u\n", rel_f, (unsigned)g_re2_room_gflags);
    CHECK("(5) Halten ohne Mash 152 Bilder, dann 0x501 + Cooldown 120 (@0x80100D28-5C) — Takt wie RE2",
          rel_f == 152 && g_re2_room_gflags == 119 && arm->sub_state_1 == 5);
    {   int f = 0, retreat_f = -1, mo_rz = -1; int32_t x0 = arm->x;
        while (f < 60 && (re15_player_is_grabbed() || retreat_f < 0)) {
            if (arm->sub_state_1 == 5 && mo_rz < 0 && arm->sub_state_2 == 1) mo_rz = arm->motion;
            frame_step(); f++;
            if (retreat_f < 0 && arm->sub_state_1 == 7) retreat_f = f;
        }
        printf("  Rueckzug fertig nach %d Bildern (Clip %d), x %ld -> %ld, Zustand 0x%06X hp=%d hidden=%d\n",
               retreat_f, mo_rz, (long)x0, (long)arm->x, (unsigned)word(arm), (int)arm->hp, (int)arm->no_draw);
        CHECK("(6) Rueckzug zeichnet EM1A-Clip 3 (Zuruecksinken), dauert die 10 Bilder des RE2-Clips 4: 300 zurueck, "
              "0x701, hp -1, verborgen (@0x80100E38-64)",
              mo_rz == 3 && retreat_f == 10 && arm->sub_state_1 == 7 && arm->hp == -1 && arm->no_draw == 1 &&
              arm->x == x0 - 300); }

    /* (7) TREFFER: HURT @0x80100F50 -> 0x501 + SE 2/3; gezeichnet EM1A-Clip 3 */
    {   int a9 = slots[8]; re15_actor_t *arm9 = &g_actors[a9];
        int32_t h9x, h9z; re15_re2arm_home(a9, NULL, &h9x, &h9z);
        pl->x = h9x - 2000; pl->z = h9z;
        printf("  Arm 9 vorher: Zustand 0x%06X hidden=%d\n", (unsigned)word(arm9), (int)arm9->no_draw);
        for (int f = 0; f < 3; f++) frame_step();
        printf("  Arm 9 nachher: Zustand 0x%06X hidden=%d d=%u\n", (unsigned)word(arm9), (int)arm9->no_draw,
               (unsigned)arm9->ai_dist);
        /* Die EM1A-Hand liegt ~1250 vor dem Ursprung (EM2D ~1080): aus 2000 Abstand zieht A1 (< 900) schon im
         * zweiten Bild -> REACH oder bereits ZUGRIFF. */
        CHECK("(7a) Arm 9: A0 hat geweckt (0x101 REACH, ggf. schon 0x301 ZUGRIFF), sichtbar",
              (arm9->sub_state_1 == 1 || arm9->sub_state_1 == 3) && arm9->no_draw == 0);
        int se0 = s_se_n;
        arm9->state = 2;                                   /* Treffer: Routine 2 (HURT) */
        frame_step();
        CHECK("(7b) Treffer -> 0x501 Rueckzug (@0x80100FA0-A4), SE 2/3 (@0x80100FAC-C8)",
              arm9->state == 1 && arm9->sub_state_1 == 5 && s_se_n > se0 &&
              ((s_se[se0] & 0xff) == 2 || (s_se[se0] & 0xff) == 3));
        frame_step();
        CHECK("(7c) der getroffene Arm zeichnet EM1A-Clip 3 (Zuruecksinken), Takt RE2-Clip 4",
              arm9->motion == 3 && takt_clip(a9) == 4); }

    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
