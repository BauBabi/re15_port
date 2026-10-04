/**
 * @file test_r35_raeume_arme.c
 * @brief Runde 35 Spur H, Punkt 4 — ROOM1210 Gitterarme: Griff und Leons Schuetteln (Clipping).
 *
 * Nutzer: "Die Zombie Arme in ROOM 1210, wenn sie einen greifen bewegen sich nicht synchron zu Leon
 * beim schuetteln, dadurch clipped er."
 *
 * Echter Weg wie unit_1210_arme_re2 (ROOM1210.RDT, zehn Arme, RE2-EM2D-Bank, game_step), Messarm 5.
 * Gemessen wird:
 *   (1) PIN: B4 P0 setzt Leon auf part[Hand]+0x5C/+0x64 (@0x80100C18-38) — die Parts traegt die Pose
 *       des LETZTEN 0x8002959C (B3 P1 @0x80100B24 im Vortakt, Bild VOR dem Weiterzaehlen
 *       @0x800295C8-F8 / +1 erst @0x80029B30), P0 selbst ruft keinen Advance (endet `j 0x80100D6C`
 *       @0x80100CAC). Erwartet: Leon steht auf der Hand von Clip 3 / Startbild des Vortakts — nicht auf
 *       Clip 5 Bild 0 (dem frueheren Port-Wert).
 *   (2) GLEICHLAUF: waehrend des Haltens zeigt Leon (Opfer-Clip 0, 19 Bilder) jedes Bild genau das
 *       Bild Arm + 1 (Clip 5, 19 Bilder) — Spieler-Routine 5 laeuft NACH allen Entities
 *       (@0x80026620 FUN_8003BFAC), der Hook-P0-Advance (@0x80101328) zaehlt im Griff-Takt schon, der
 *       Arm erst ab P1 (@0x80100D18).
 *   (3) CLIPPING-MASS (Leon zum Arm gewandt = Hook-P0 ohne Flip, FUN_80015910 @0x801012E0): waagerechter
 *       Abstand Arm-Hand <-> Leons Brustachse (PL00-Knochen 8, Renderpfad main.c Opfer-Override) je
 *       Halte-Bild; Hand innerhalb 120 = steckt im Oberkoerper. Erwartet 0 Bilder (vorher 9 von 19,
 *       Dossier 4.4).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <math.h>
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
static re15_emd_animation_t s_pa; static re15_emd_skeleton_t s_ps;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("ok:   %s\n", name); } while (0)

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
static void se_cap(int id, int f) { (void)id; (void)f; }
static void bank_cap(int b) { (void)b; }
static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
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
    if (!load_re2_bank_1A()) return 0;
    re15_re2arm_audio_hook(se_cap, bank_cap);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

static int32_t s_knochen[16][3];
/* Leons Brust-/Halsknochen 8 in Weltkoordinaten (Opfer-Override main.c: PL00-Knochen + Bindpose,
 * Keyframes + Clips der Greifer-Opferbank, clip_override = pl->motion). */
static int leon_brust(const re15_actor_t *pl, int32_t out[3])
{
    re15_enemy_bank_t *vb = re15_enemy_find(0x1A);
    if (!vb || !vb->victim_ok) return 0;
    re15_emd_skeleton_t vs = s_ps;
    vs.keyframe_data = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    re15_actor_t pr = *pl;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, &pr, (int)pl->motion, pl->anim_frame);
    if (kf < 0) return 0;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[8].trans, pl->rot_y, pl->x, 0, pl->z, out);
    if (getenv("R35_ARME_DUMP")) {               /* Messdump: alle Leon-Knochen dieses Bilds */
        for (int b = 0; b < s_ps.bone_count && b < 16; b++)
            re15_skel_bone_to_world(poses[b].trans, pl->rot_y, pl->x, 0, pl->z, s_knochen[b]);
    }
    return 1;
}

/* Aufzeichnung je Halte-Bild f (Nachbesserung 1, M2): Hand, Brust, Leon-Wurzel/-Blick. */
typedef struct { int ok; int32_t h[3], b[3], lx, lz; int yaw; int pfr, afr; } bild_t;
static bild_t s_auf[2][60];
static int    s_auf_yaw[2];                    /* Leon-Blick unmittelbar nach dem Hook-P0 */

/* Ein Griff des Messarms mit Leon-Blick `yaw0` vor dem Zugriff. Liefert die Zahl der Halte-Bilder mit
 * Hand < 120 an der Brustachse (oder -1). `fall` (0 = Gesicht, 1 = Ruecken) waehlt die Aufzeichnung. */
static int griff(int yaw0, const char *name, int fall)
{
    memset(s_auf[fall], 0, sizeof s_auf[fall]);
    s_auf_yaw[fall] = -1;
    int slots[RE15_ACTOR_MAX]; int n = aufsetzen(slots);
    if (n < 10) { CHECK("zehn Arme + EM2D-Bank", 0); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int a5 = slots[4]; re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    pl->x = hx + 3500; pl->z = hz + 3500;
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;      /* Member_set(12,1) @0x1EDA */
    for (int f = 0; f < 20 && arm->sub_state_2 != 2; f++) frame_step();
    pl->x = hx + 2000; pl->z = hz;
    frame_step();                                                     /* A0 -> 0x101 REACH */
    int32_t hand[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), hand);
    pl->x = hand[0] + 300; pl->z = hand[2]; pl->rot_y = (int16_t)yaw0;
    /* Startbild-Historie: die Parts im Griff-Takt T tragen die Pose, die B3 P1 im Takt T-1 aus dem
     * damaligen Startbild baute (Clip 3, Bild = anim_frame zu Beginn von T-1). */
    int vor_mo = -1, vor_fr = -1, cur_mo = arm->motion, cur_fr = arm->anim_frame;
    int grab = 0;
    for (int f = 0; f < 60 && !grab; f++) {
        vor_mo = cur_mo; vor_fr = cur_fr;
        cur_mo = arm->motion; cur_fr = arm->anim_frame;
        frame_step();
        if (arm->sub_state_1 == 4) grab = 1;
    }
    char nm[200];
    snprintf(nm, sizeof nm, "[%s] Griff kommt zustande (A3 0x401 @0x80100A50)", name);
    CHECK(nm, grab);
    if (!grab) return -1;
    /* (1) Pin */
    int32_t erwartet[3], alt[3];
    {   const int16_t mo = arm->motion; const uint16_t fr = arm->anim_frame;
        arm->motion = (int16_t)vor_mo; arm->anim_frame = (uint16_t)vor_fr;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), erwartet);
        arm->motion = 5; arm->anim_frame = 0;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), alt);
        arm->motion = mo; arm->anim_frame = fr; }
    long dalt = (long)sqrt((double)(alt[0]-erwartet[0])*(alt[0]-erwartet[0]) +
                           (double)(alt[2]-erwartet[2])*(alt[2]-erwartet[2]));
    printf("  [%s] Pin Leon (%d,%d) | Parts-Pose Clip %d Bild %d -> (%d,%d) | Clip 5 Bild 0 -> (%d,%d), "
           "Abstand %ld | Leon-Blick %d\n", name, pl->x, pl->z, vor_mo, vor_fr, erwartet[0], erwartet[2],
           alt[0], alt[2], dalt, (int)pl->rot_y);
    snprintf(nm, sizeof nm, "[%s] (1) Pin = part[Hand] der Parts-Pose (Clip 3, Startbild Vortakt) "
                            "@0x80100C18-38, nicht Clip 5 Bild 0", name);
    long dneu = (long)sqrt((double)(pl->x-erwartet[0])*(pl->x-erwartet[0]) + (double)(pl->z-erwartet[2])*(pl->z-erwartet[2]));
    long dbis = (long)sqrt((double)(pl->x-alt[0])*(pl->x-alt[0]) + (double)(pl->z-alt[2])*(pl->z-alt[2]));
    printf("  [%s] Leon zur Parts-Pose-Hand %ld, zur Clip-5-Hand %ld\n", name, dneu, dbis);
    CHECK(nm, vor_mo == 3 && dneu <= 30 && dneu < dbis && dalt > 0);
    /* (2) Gleichlauf + (3) Clipping-Mass ueber einen vollen Zyklus nach der Ueberblendung */
    int gleich = 1, im_koerper = 0, bilder = 0; int mind = 1 << 30;
    for (int f = 0; f < 60; f++) {
        frame_step();
        if (f == 0) s_auf_yaw[fall] = (int)pl->rot_y & 0xfff;      /* nach Hook-P0 (@0x801012E0-18) */
        if (arm->sub_state_1 != 4 || re15_player_victim_state() != 4) break;
        if (pl->motion != 0 || arm->motion != 5) continue;
        if ((int)pl->anim_frame != (((int)arm->anim_frame + 1) % 19)) gleich = 0;
        if (f < 16) continue;                                       /* Ueberblendung Rate 15 */
        int32_t b8[3], h[3];
        if (!leon_brust(pl, b8)) continue;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), h);
        int d = (int)sqrt((double)(h[0]-b8[0])*(h[0]-b8[0]) + (double)(h[2]-b8[2])*(h[2]-b8[2]));
        {   bild_t *a = &s_auf[fall][f];
            a->ok = 1; memcpy(a->h, h, sizeof h); memcpy(a->b, b8, sizeof b8);
            a->lx = pl->x; a->lz = pl->z; a->yaw = (int)pl->rot_y & 0xfff;
            a->pfr = pl->anim_frame; a->afr = arm->anim_frame; }
        if (getenv("R35_ARME_DUMP")) {
            double c = cos(pl->rot_y * 3.14159265358979 / 2048.0), sn = sin(pl->rot_y * 3.14159265358979 / 2048.0);
            printf("  DUMP [%s] f%2d afr %2d hand=(%d,%d,%d)", name, f, (int)arm->anim_frame, h[0], h[1], h[2]);
            for (int b = 0; b < 16; b++) {
                int dx = h[0]-s_knochen[b][0], dz = h[2]-s_knochen[b][2], dy = h[1]-s_knochen[b][1];
                int vor = (int)(dx * c - dz * sn), seit = (int)(dx * sn + dz * c);
                if (b == 0 || b == 8 || b == 9 || b == 12) printf(" | b%d v%d s%d h%d", b, vor, seit, -dy);
            }
            printf("\n");
        }
        if (d < mind) mind = d;
        if (d < 120) im_koerper++;
        bilder++;
    }
    snprintf(nm, sizeof nm, "[%s] (2) Leon zeigt jedes Halte-Bild Arm-Bild + 1 (Clip 0/Clip 5, 19 Bilder)", name);
    CHECK(nm, gleich);
    printf("  [%s] Clipping-Mass: %d Halte-Bilder, Hand < 120 an der Brustachse in %d, Minimum %d\n",
           name, bilder, im_koerper, mind);
    return bilder ? im_koerper : -1;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: ROOM1210.RDT\n"); return 1; }
    {   size_t esz = 0, rsz = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        if (!edd || !emr || re15_emd_parse_animation(edd, esz, &s_pa) != 0 ||
            re15_emd_parse_skeleton(emr, rsz, &s_ps) != 0) { printf("FAIL: PL00\n"); return 1; } }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    /* Leon dem Fenster zugewandt (Blick -x = 2048): FUN_80015910 -> 0, kein Flip -> Gesicht zum Arm */
    int k_gesicht = griff(2048, "Gesicht", 0);
    CHECK("(3) Gesicht-Griff: Arm-Hand steckt in KEINEM Halte-Bild in Leons Oberkoerper (< 120)",
          k_gesicht == 0);
    /* Leon vom Fenster abgewandt (Blick +x = 0): Flip +2048 (@0x80101304-18) -> Ruecken zum Arm. */
    int k_ruecken = griff(0, "Ruecken", 1);
    printf("  Ruecken-Griff: %d Halte-Bilder mit Hand < 120 an der Brustachse\n", k_ruecken);

    /* (4) Nachbesserung 1, M2 — der Ruecken-Griff ist im RE2-Original KEIN eigener Fall, sondern der
     * Gesicht-Griff mit Leon um 180 Grad gedreht. Belegt (EM2D-Overlay CDEMD0_EM2D_ai1.BIN / RE2 PSX.EXE):
     *   - EINE Opferbank: B4 P0 kopiert +0x188/+0x18C des Arms nach PL+0x188/+0x18C (@0x80100C3C-5C);
     *   - EIN Opfer-Clip: Hook P0 `lui v0,0xf / sw v0,332(s1)` = Clip 0 (@0x801012A8-AC), P1 nur Advance;
     *   - der EINZIGE Unterschied: `lhu v0,118(s1) / addiu v0,v0,2048 / sh v0,118(s1)` (@0x8010130C-18),
     *     gegatet von FUN_80015910 (@0x801012E0); das Ergebnis s0 lebt nur im Hook;
     *   - der Arm liest PL+0x76 nirgends (einzige Zugriffe auf Offset 118 im Overlay: @0x8010130C/18) und
     *     PL.x/z nur VOR dem Griff (@0x80100028..0x80100B0C) — seine Hand-Bahn haengt nicht an Leons Blick.
     * Geprueft wird genau diese Konstruktion: (4a) Ruecken-Blick = Gesicht-Blick + 2048, (4b) Wurzel und
     * Hand-Bahn Bild fuer Bild gleich, (4c) Brust im Ruecken-Griff = an Leons Wurzel gespiegelte Brust des
     * Gesicht-Griffs. Das Clipping-Mass des Ruecken-Griffs ist damit die zwingende Folge der RE2-Daten. */
    {
        int vergl = 0, wurzel = 1, hand = 1, spiegel = 1, maxdh = 0, maxdb = 0;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[1][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if (g->lx != r->lx || g->lz != r->lz || g->pfr != r->pfr || g->afr != r->afr) wurzel = 0;
            int dh = abs(g->h[0] - r->h[0]) + abs(g->h[2] - r->h[2]);
            if (dh > maxdh) maxdh = dh;
            if (dh > 2) hand = 0;
            int db = abs((r->b[0] - r->lx) + (g->b[0] - g->lx)) + abs((r->b[2] - r->lz) + (g->b[2] - g->lz));
            if (db > maxdb) maxdb = db;
            if (db > 4) spiegel = 0;
        }
        printf("  (4) Ruecken vs. Gesicht: %d Bilder verglichen, Blick %d / %d, Hand-Abweichung max %d, "
               "Spiegel-Abweichung max %d\n", vergl, s_auf_yaw[1], s_auf_yaw[0], maxdh, maxdb);
        CHECK("(4a) Ruecken-Griff: Leon-Blick = Gesicht-Blick + 2048 (Flip @0x8010130C-18)",
              s_auf_yaw[0] >= 0 && s_auf_yaw[1] == ((s_auf_yaw[0] + 2048) & 0xfff));
        CHECK("(4b) Wurzel, Bildtakt und Hand-Bahn haengen nicht an Leons Blick (Arm liest PL+0x76 nie)",
              vergl >= 30 && wurzel && hand);
        CHECK("(4c) Brust im Ruecken-Griff = an Leons Wurzel gespiegelte Brust (EINE Bank @0x80100C3C-5C, "
              "EIN Clip @0x801012A8-AC)", vergl >= 30 && spiegel);
    }

    printf(g_fail ? "test_r35_raeume_arme: FAIL\n" : "test_r35_raeume_arme: OK\n");
    return g_fail ? 1 : 0;
}
