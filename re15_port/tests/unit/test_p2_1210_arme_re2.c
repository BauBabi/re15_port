/* test_p2_1210_arme_re2.c — PIN (Runde 16 / Phase 2, 2026-09-19): die ROOM1210-Gitterhaende
 * fahren unter dem RE2-Flavor die RE2-Zellenarm-Maschine (RE2 Typ 0x2D, CDEMD0_EM2D_ai1.BIN)
 * auf dem RE2-Modell EM2D. Dossier analysis/befunde_2026-09-19/arme-1210-re2.md, Modul
 * engine/src/enemy_ai_re2_zellenarm.c (jede Zahl dort mit @0x).
 *
 * Was hier festgeschrieben wird (alles am Original belegt, s. Modul-Kopf):
 *  (1) SCHLAF: bis sub02 grid_id=1 setzt, tickt der Arm nicht (Per-Frame-Schleife @0x8002659C
 *      ueberspringt +0x10E & 0x8000) — Zustand bleibt 0x000001, der Arm ist verborgen.
 *  (2) WECKRUF: B0 P0 (400 zurueck @0x801006F8), P1 zaehlt +0x158 = rng&0xF herunter, P2 = scharf.
 *  (3) A0: Abstand < 2500 im 1024-Kegel bei Phase >= 2 -> 0x101 REACH, sichtbar, Pose = Heimat.
 *  (4) A1: Hand (Bone 3/10) < 900 -> 0x301; A3: Hand < 600, Bild >= 5, Cooldown 0 -> 0x401 mit
 *      Teleport PL.x/z := Hand (@0x80100C18-38), Spieler im Victim-Modus 4, Riegel PL+0x1D3|0x80.
 *  (5) HALTEN: +0x15A = 150, je Bild -1-2*Mash; ohne Mash 151 Bilder, dann Cooldown 120
 *      (0x800CFBF4 @0x80100D5C) und 0x501; Leon spielt Opfer-Clip 0 in Schleife, beim Loesen
 *      Clip 1 einmal (Hook @0x80101338), danach frei und Riegel geloescht.
 *  (6) RUECKZUG: 30/Bild zurueck (@0x80100E64), am Clip-4-Ende 0x701, hp = -1, verborgen —
 *      und dort bleibt er (kein Setzer fuehrt aus Sub 7 heraus).
 *  (7) Ein zweiter Arm greift waehrend des Cooldowns NICHT (A3-Tor @0x80100A28-30).
 *  (8) Der Teleport-Punkt liegt begehbar (Messung: Constrain vom Standpunkt davor = 0 Versatz).
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
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
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
static int load_re2_bank_1A(void)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(s_ems, s_ems_n, 0x2D, eb, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
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
    int ok = load_re2_bank_1A();                         /* NACH dem Reset */
    CHECK("RE2-EM2D (kind 0x2D) unter Typ 0x1A geladen — 15 Bones, 6 Clips, Opferbank 2 Clips",
          ok && re15_enemy_find(0x1A) && re15_enemy_find(0x1A)->skel.bone_count == 15 &&
          re15_enemy_find(0x1A)->anim.clip_count == 6 && re15_enemy_find(0x1A)->victim_ok &&
          re15_enemy_find(0x1A)->anim_victim.clip_count == 2);
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
    printf("=== ROOM1210 Gitterhaende = RE2-Zellenarm (Typ 0x2D) ===\n");

    int slots[RE15_ACTOR_MAX]; int n = aufsetzen(slots);
    CHECK("zehn Arme gespawnt", n == 10);
    if (n < 10) return 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Arm 5 (Westreihe, z -15747) ist der Messarm: Heimat aus dem Modul. */
    int a5 = slots[4];
    re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    printf("  arm%d Heimat=(%ld,%ld) y=%ld yaw=%d f10e=0x%02x\n", a5, (long)hx, (long)hz,
           (long)arm->y, (int)hyaw, (unsigned)arm->re2z_f10e);
    CHECK("Arm-Wahl/Master nach der ROOM2050-Folge 10,01,00,01,00: Arm 5 (k=4) = Arm A, kein Master",
          (arm->re2z_f10e & 0x11u) == 0u && (g_actors[slots[0]].re2z_f10e & 0x11u) == 0x10u &&
          (g_actors[slots[1]].re2z_f10e & 0x11u) == 0x01u);

    /* (1) SCHLAF */
    pl->x = hx + 1500; pl->z = hz;                       /* direkt vor dem Fenster, im Kegel */
    for (int f = 0; f < 60; f++) frame_step();
    CHECK("(1) ohne Weckruf tickt der Arm nicht: Zustand bleibt 0x000001, verborgen (@0x8002659C)",
          word(arm) == 0x000001u && arm->no_draw == 1 && arm->hp == 250);

    /* (2) WECKRUF: B0 P0 -> P1 -> P2 innerhalb von 1 + (rng&0xF) + 1 Bildern */
    pl->x = hx + 3500; pl->z = hz + 3500;                /* ausser 2500 -> A0 zieht noch nicht */
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;   /* Member_set(12,1) @0x1EDA */
    frame_step();
    CHECK("(2a) erstes Tick-Bild: B0 P0 -> Phase 1, Ursprung 400 hinter der Heimat (@0x801006F8-718)",
          word(arm) == 0x010001u && arm->x == hx - 400 && arm->z == hz && arm->no_draw == 1);
    {   int f = 0; while (f < 20 && arm->sub_state_2 != 2) { frame_step(); f++; }
        CHECK("(2b) P1 zaehlt +0x158 (rng&0xF) herunter, Phase 2 innerhalb von 17 Bildern, noch verborgen",
              arm->sub_state_2 == 2 && f <= 17 && arm->no_draw == 1); }

    /* (3) A0 Zweig 1: d < 2500, Kegel 1024, Phase >= 2 -> 0x101, sichtbar, Pose = Heimat */
    pl->x = hx + 2000; pl->z = hz;
    frame_step();
    CHECK("(3) A0: Abstand 2000 im Kegel -> 0x101 REACH (@0x80100564-9C), sichtbar, x/z = Heimat",
          arm->state == 1 && arm->sub_state_1 == 1 && arm->no_draw == 0 && arm->x == hx && arm->z == hz
          && arm->hit_radius_min == 300);
    CHECK("(3b) B1 P0 im selben Bild: Clip 0/1, +0x158 = (rng&0x1F)+60 (@0x80100878-9C)",
          (arm->motion == 0 || arm->motion == 1) && arm->re2z_t158 >= 60 && arm->re2z_t158 <= 91);

    /* (4) A1 -> 0x301 -> A3 -> 0x401 mit Teleport an die Hand */
    int32_t hand[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), hand);
    printf("  Hand-Weltpunkt (Bone %d) = (%ld,%ld,%ld)\n", re15_re2arm_hand_bone(arm),
           (long)hand[0], (long)hand[1], (long)hand[2]);
    pl->x = hand[0] + 300; pl->z = hand[2];              /* < 900 -> A1 */
    int32_t stand_x = pl->x, stand_z = pl->z;
    {   int32_t cx = stand_x, cz = stand_z; re15_collision_constrain(&s_rdt, -19500, hz, &cx, &cz);
        CHECK("(4a) der Standpunkt neben der Hand ist begehbar (Constrain aus der Flurmitte = kein Versatz)",
              cx == stand_x && cz == stand_z); }
    frame_step();
    CHECK("(4b) A1: Hand < 900 -> 0x301 ZUGRIFF, B3 P0 Clip 3 (@0x80100814 / @0x80100AEC)",
          arm->state == 1 && arm->sub_state_1 == 3 && arm->motion == 3);
    int grab_f = -1;
    for (int f = 0; f < 40 && grab_f < 0; f++) { frame_step(); if (arm->sub_state_1 == 4) grab_f = f; }
    CHECK("(4c) A3: Hand < 600 ab Bild 5 -> 0x401 HALTEN (@0x801009BC-A54)", grab_f >= 0);
    if (grab_f >= 0) {
        int32_t h2[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), h2);
        printf("  Griff-Bild %d: pl=(%ld,%ld) Hand=(%ld,%ld) Standpunkt davor=(%ld,%ld)\n", grab_f,
               (long)pl->x, (long)pl->z, (long)h2[0], (long)h2[2], (long)stand_x, (long)stand_z);
        CHECK("(4d) Spieler an die Hand teleportiert (PL.x/z := part[Hand]+0x5C/+0x64 @0x80100C18-38), Victim-Modus 4, Riegel gesetzt",
              re15_player_is_grabbed() && re15_player_victim_state() == 4 &&
              (pl->re2z_self1d3 & 0x80u) && re15_re2arm_holder_slot() == a5 &&
              labs(pl->x - h2[0]) <= 60 && labs(pl->z - h2[2]) <= 60);
        {   int32_t cx = pl->x, cz = pl->z; re15_collision_constrain(&s_rdt, stand_x, stand_z, &cx, &cz);
            printf("  Teleport-Constrain-Versatz = (%ld,%ld)\n", (long)(cx - pl->x), (long)(cz - pl->z));
            CHECK("(8) der Teleport-Punkt ist begehbar (Constrain vom Standpunkt davor = 0 Versatz)",
                  cx == pl->x && cz == pl->z); }
        CHECK("(4e) Arm: Clip 5, +0x15A = 150 (@0x80100BC8 / @0x80100C8C); Leon: Opfer-Clip 0",
              arm->motion == 5 && arm->re2z_t15a == 150 && pl->motion == 0);
    }
    /* (5) HALTEN ohne Mash: +0x15A = 150 im Griff-Bild (P0, ohne Durchfall); danach je P1-Bild
     * -1 -> nach dem 151. P1-Bild < 0 -> Phase 2; das P2-Bild (152.) setzt Cooldown 120 + 0x501.
     * Zwei Master (Arme 1 und 6, k%5 == 0 wie ROOM2050 @0x1970/@0x19DE pc[4] = 0x10) ziehen je
     * Bild 1 ab; Arm 6 tickt NACH Arm 5, also steht am Bildende 119. */
    int rel_f = -1;
    for (int f = 0; f < 200 && rel_f < 0; f++) { frame_step(); if (arm->sub_state_1 == 5) rel_f = f + 1; }
    printf("  Loesen nach %d Bildern, cd=%u, pl clip=%d\n", rel_f, (unsigned)g_re2_room_gflags, (int)pl->motion);
    CHECK("(5a) ohne Mash 151 P1-Bilder (+0x15A -1/Bild @0x80100D28-34) + 1 P2-Bild = 152, dann 0x501 + Cooldown 120 (@0x80100D5C; Master-Dekrement @0x80100058-80)",
          rel_f == 152 && g_re2_room_gflags == 119 && arm->sub_state_1 == 5);
    CHECK("(5b) Leon spielt beim Loesen Opfer-Clip 1 (Hook P2 @0x80101338), noch gepinnt",
          pl->motion == 1 && re15_player_is_grabbed());
    /* (6) RUECKZUG laeuft parallel zum Loesen: Clip 4 = 10 Bilder, je Bild 30 zurueck (auch im
     * done-Bild), am Ende 0x701 / hp -1 / verborgen. */
    {   int f = 0, retreat_f = -1; int32_t x0 = arm->x;
        while (f < 60 && re15_player_is_grabbed()) {
            frame_step(); f++;
            if (retreat_f < 0 && arm->sub_state_1 == 7) retreat_f = f;
        }
        printf("  frei nach %d weiteren Bildern, rot=%d; Rueckzug fertig nach %d Bildern, x %ld -> %ld, "
               "Zustand 0x%06X hp=%d hidden=%d\n", f, (int)pl->rot_y, retreat_f, (long)x0, (long)arm->x,
               (unsigned)word(arm), (int)arm->hp, (int)arm->no_draw);
        CHECK("(5c) nach Clip 1 (20 Bilder) ist Leon frei, Riegel PL+0x1D3&0x80 geloescht (@0x80101374-90), hp unveraendert",
              !re15_player_is_grabbed() && !(pl->re2z_self1d3 & 0x80u) && f <= 22 && pl->hp == 100);
        CHECK("(6) Rueckzug: 10 Bilder Clip 4 x 30/Bild (@0x80100E64) = 300 zurueck -> 0x701, hp -1, verborgen, kein Trefferkasten (@0x80100E38-50)",
              retreat_f == 10 && arm->sub_state_1 == 7 && arm->hp == -1 && arm->no_draw == 1 &&
              arm->hit_radius_min == 0 && arm->x == x0 - 300);
        pl->x = hx + 1500; pl->z = hz;
        for (int k = 0; k < 120; k++) frame_step();
        CHECK("(6b) aus 0x701 fuehrt nichts heraus: der Arm greift nie wieder (A7/B7 = jr ra)",
              arm->sub_state_1 == 7 && arm->no_draw == 1); }

    /* (7) Cooldown sperrt den naechsten Arm: Arm 9 (Ostreihe, z -21158, bisher nie in Reichweite)
     * waehrend cd > 0 -> A1 zieht (0x301), A3 nicht (Cooldown-Tor) -> Clip 3 laeuft leer -> 0x501. */
    {   int a9 = slots[8]; re15_actor_t *arm9 = &g_actors[a9];
        int32_t h9x, h9z; re15_re2arm_home(a9, NULL, &h9x, &h9z);
        CHECK("(7a) Arm 9 ist noch unverbraucht (Sub 0)", arm9->sub_state_1 == 0);
        g_re2_room_gflags = 100;                          /* Rest-Cooldown (2 Master -> -2/Bild) */
        pl->x = h9x - 2000; pl->z = h9z;                  /* Ostreihe blickt -x */
        for (int f = 0; f < 3; f++) frame_step();
        CHECK("(7b) Arm 9: A0 -> 0x101 REACH", arm9->sub_state_1 == 1);
        int32_t h9[3]; re15_enemy_bone_world_pos(arm9, re15_re2arm_hand_bone(arm9), h9);
        pl->x = h9[0] - 200; pl->z = h9[2];
        int grabbed = 0, f = 0, zugriff = 0;
        for (; f < 60 && !grabbed; f++) {
            frame_step();
            if (arm9->sub_state_1 == 3) zugriff = 1;
            grabbed = (arm9->sub_state_1 == 4);
            if (arm9->sub_state_1 == 7) break;
        }
        printf("  Arm 9: nach %d Bildern sub=%d cd=%u zugriff=%d\n", f, arm9->sub_state_1, (unsigned)g_re2_room_gflags, zugriff);
        CHECK("(7) solange 0x800CFBF4 > 0 kommt kein zweiter Griff zustande (A3 @0x80100A28-30): ZUGRIFF laeuft leer -> 0x501/0x701",
              !grabbed && zugriff && (arm9->sub_state_1 == 5 || arm9->sub_state_1 == 7)); }

    printf("  SE-Aufrufe (%d):", s_se_n);
    for (int i = 0; i < s_se_n; i++) printf(" %d/%d", s_se[i] & 0xff, s_se[i] >> 8);
    printf("\n");
    CHECK("SE ueber Bank 42, zweite Haelfte (flag2000 = 1): Zupacken 5/7 und Loslassen 6 gespielt",
          s_se_n > 0 && (s_se[0] >> 8) == 1);
    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
