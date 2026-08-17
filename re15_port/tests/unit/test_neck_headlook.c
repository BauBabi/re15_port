/*
 * test_neck_headlook.c — Batch A1 (NECK/HEAD-LOOK), 2026-08-17.
 *
 * Pinnt die vier byte-true Korrekturen des Kopf-Verhaltens. ALLE Sollwerte stammen aus
 * selbst nachdisassemblierten Instruktionen der PSX.EXE (Adressen je Assertion zitiert):
 *
 *  (a) Auto-Look-Scan vergleicht die WURZEL gegen den Radius (@0x800370f8 jal SquareRoot0,
 *      @0x8003715c sltu) — der alte Quadrat-Vergleich lief bei ~50 000 Units in den
 *      int32-Ueberlauf und trackte den ROOM1170-Rest-NPC.
 *  (b) Drei Klassen ueber +0x9a/+0x9 mit Prioritaet K1 > K2 > K3 (@0x80037118-70,
 *      @0x80037190-a4) und Ausschluss +0x9 & 0x60 (@0x8003710c).
 *  (c) Die Part-Klemme sitzt auf dem KOERPER-Residual OHNE Kopf-Keyframe
 *      (@0x80037598 subu a3,v1,s1 + Klemme @0x800375a4-d0); der Keyframe kommt erst im
 *      Slew-Delta (@0x800375dc lhu a1,98(s0)) -> gerenderter Kopf-Euler == +-Klemme.
 *  (d) Release-Bit 0x10 zielt Pitch 0 ABSOLUT (@0x80037554-60 andi 0x10 / sh zero,20(sp));
 *      der Akku laeuft damit auf -kf_vz (Savestate-Beleg accP=-186 bei kf vz=+186).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_skeleton.h"

/* Der Scan lebt in game_step_common.c (kein Header-Eintrag — nur der State-1-Prolog
 * ruft ihn). Signatur dort: int re15_autolook_scan(re15_actor_t*, uint32_t). */
extern int re15_autolook_scan(re15_actor_t *pl, uint32_t radius);

static int g_fail = 0;
#define CHECK(cond, ...) do { if (cond) { printf("  PASS: "); } \
                              else { printf("  FAIL: "); g_fail = 1; } \
                              printf(__VA_ARGS__); printf("\n"); } while (0)

/* ===== (a)/(b) Auto-Look-Scan ============================================== */

static void reset_actors(void)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->neck_target_slot = -1;
}

/* Setzt Actor `slot` als Kandidat: hp (+0x9a), grid (+0x9), Position relativ zum Spieler. */
static void put(int slot, uint8_t type, int16_t hp, uint8_t grid, int32_t dx, int32_t dz)
{
    re15_actor_t *n = &g_actors[slot];
    n->active = 1; n->type = type; n->hp = hp; n->grid_id = grid;
    n->x = g_actors[RE15_ACTOR_SLOT_PLAYER].x + dx;
    n->z = g_actors[RE15_ACTOR_SLOT_PLAYER].z + dz;
}

static void test_scan(void)
{
    printf("== (a) Radius-Gate: WURZEL statt Quadrat (@0x800370f8 / @0x8003715c)\n");
    /* Gemessene ROOM1170-Post-Intro-Lage (lane17_NECK.md 7.1): Spieler (2664,-7336),
     * Rest-NPC Typ 0x47 @(-31000,29500) -> dx=-33664, dz=36836, dist ~49900.
     * dx*dx+dz*dz = 2 490 155 792 -> als int32 NEGATIV -> das alte `d2 < 4000*4000`
     * war WAHR und Leons Kopf hing dauerhaft an der Klemme. */
    reset_actors();
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 2664;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = -7336;
    put(1, 0x47, -1, 0x00, -33664, 36836);
    int rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 0, "ferner NPC (~49900 Units) wird NICHT getrackt (rv=%d, erwartet 0)", rv);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 1,
          "Fehlschlag schreibt trotzdem +0x1a8 = Entity-Slot 0 (@0x800371d8-dc)");

    /* Gegenprobe: derselbe NPC in Reichweite wird gefunden (rv=2, K3 weil hp<0). */
    reset_actors();
    put(1, 0x47, -1, 0x00, 1000, 1000);          /* dist = 1414 < 4000 */
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 2 && g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 1,
          "naher NPC (hp<0) wird als Klasse 3 getrackt (rv=%d)", rv);

    /* Randfall — und zugleich der Beleg, dass wirklich die BIOS-NAEHERUNG entscheidet und
     * nicht floor(sqrt): SquareRoot0 (@0x80065f60, Mantissen-Tabelle @0x8007d984)
     * UNTERSCHAETZT. Bei exakt 4000 Units Abstand liefert sie 3998 -> `sltu 3998,4000`
     * ist WAHR -> das Entity liegt noch INNEN. Erst ab ~4010 (Messung 4007) faellt es raus. */
    reset_actors();
    put(1, 0x47, -1, 0x00, 4000, 0);
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 2, "dist 4000 -> SquareRoot0 = 3998 -> noch INNEN (rv=%d)", rv);
    reset_actors();
    put(1, 0x47, -1, 0x00, 4010, 0);
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 0, "dist 4010 -> SquareRoot0 = 4007 -> AUSSEN (sltu, strikt <) (rv=%d)", rv);

    printf("== (b) Ausschluss +0x9 & 0x60 (@0x8003710c) + Klassen-Prioritaet\n");
    /* orig_1170_gp.sav: der Rest-NPC hat +0x9 = 0x40 (Cutscene-Spawn) und wird trotz
     * hp=-1 NIE Ziel. Der Sce_em_set-Tail @0x8004260c-18 setzt ihm dafuer +0x1b8=0x12. */
    reset_actors();
    put(1, 0x47, -1, 0x40, 100, 0);
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 0, "Cutscene-Spawn (+0x9 & 0x40) ist ausgeschlossen (rv=%d)", rv);

    reset_actors();
    put(1, 0x10, 105, 0x20, 100, 0);
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 0, "AI-Freeze (+0x9 & 0x20) ist ausgeschlossen (rv=%d)", rv);

    /* K1 (lebender Gegner) schlaegt K3 (Leiche), auch wenn die Leiche viel naeher ist.
     * Ground-Truth mzd_stage1_engage_live.sav: Player-neckflags 0x00, +0x1a8 = lebender
     * Zombie (hp=105). */
    reset_actors();
    put(1, 0x10, -1,  0x00,  500, 0);            /* Leiche, nah        -> K3 */
    put(2, 0x10, 105, 0x00, 3000, 0);            /* lebender Zombie    -> K1 */
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 1 && g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 2,
          "K1 (lebend) vor K3 (Leiche) (rv=%d slot=%d)",
          rv, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot);

    /* K1 schlaegt K2 (der 1140-"Lyer", +0x9 = 0x88 -> Bit 7). */
    reset_actors();
    put(1, 0x16, 60,  0x88,  500, 0);            /* Lyer, nah          -> K2 */
    put(2, 0x10, 105, 0x00, 3000, 0);            /* lebender Zombie    -> K1 */
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 1 && g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 2,
          "K1 vor K2 (@0x80037190 vor @0x8003719c) (rv=%d slot=%d)",
          rv, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot);

    /* K2 schlaegt K3, und K2 liefert ebenfalls rv=1 (@0x8003719c-a0 -> 0x800371f0). */
    reset_actors();
    put(1, 0x10, -1, 0x00,  500, 0);             /* Leiche             -> K3 */
    put(2, 0x16, 60, 0x88, 3000, 0);             /* Lyer               -> K2 */
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 1 && g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 2,
          "K2 vor K3, Rueckgabe 1 (rv=%d slot=%d)",
          rv, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot);

    /* Innerhalb einer Klasse gewinnt der NAECHSTE (`sltu v1,best`). */
    reset_actors();
    put(1, 0x10, 105, 0x00, 3000, 0);
    put(2, 0x10, 105, 0x00,  800, 0);
    rv = re15_autolook_scan(&g_actors[RE15_ACTOR_SLOT_PLAYER], 0xfa0);
    CHECK(rv == 1 && g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot == 2,
          "innerhalb K1 gewinnt der naechste (slot=%d)",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_target_slot);
}

/* ===== (c)/(d) Neck-FSM auf einem synthetischen Skelett ==================== */

/* 9 Bones, Kette 0->1->...->8; Bone 8 = Kopf. Keyframe-Winkel 12 bit gepackt ab
 * Byte +12, 36 bit je Bone, LSB zuerst (re15_emd_get_keyframe_angles / bit_read). */
#define KF_BONES 9
#define KF_SIZE  60
static uint8_t s_kf[KF_SIZE];

static void kf_put12(int bit_offset, int value)
{
    uint32_t v = (uint32_t)(value & 0xFFF);
    for (int i = 0; i < 12; i++) {
        int bo = bit_offset + i;
        int byte_idx = 12 + (bo >> 3), inner = bo & 7;
        s_kf[byte_idx] = (uint8_t)((s_kf[byte_idx] & ~(1u << inner))
                                   | (((v >> i) & 1u) << inner));
    }
}

static void build_skel(re15_emd_skeleton_t *sk, int head_ay, int head_az)
{
    memset(s_kf, 0, sizeof s_kf);
    memset(sk, 0, sizeof *sk);
    sk->bone_count = KF_BONES;
    for (int b = 0; b < KF_BONES; b++) {
        sk->bone_parent[b]      = (int8_t)(b == 0 ? -1 : b - 1);
        sk->bone_mesh_index[b]  = (int8_t)b;
        sk->bone_relative_pos[b][0] = 0;
        sk->bone_relative_pos[b][1] = (int16_t)(b == 0 ? 0 : -100);
        sk->bone_relative_pos[b][2] = 0;
    }
    kf_put12(8 * 36 + 12, head_ay);      /* Bone 8, y-Winkel = Kopf-Keyframe-vy */
    kf_put12(8 * 36 + 24, head_az);      /* Bone 8, z-Winkel = Kopf-Keyframe-vz */
    sk->keyframe_size_bytes = KF_SIZE;
    sk->keyframe_count      = 1;
    sk->keyframe_data       = s_kf;
    sk->keyframe_data_size  = KF_SIZE;
}

/* Setzt den Spieler mit den byte-true State-0-INIT-Neck-Defaults auf:
 * Bone 8 @0x80031938, Steps 96/96 @0x800319a4-ac, Klemmen +-0x200/+-0x138
 * @0x800319b0-c4, Akkus 0 @0x800319bc-c0. */
static re15_actor_t *setup_player(void)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->neck_bone = 8;
    pl->neck_step_yaw = 96; pl->neck_step_pitch = 96;
    pl->neck_clamp_yaw = 0x200; pl->neck_clamp_pitch = 0x138;
    pl->neck_target_slot = -1;
    return pl;
}

static void run_pose(const re15_emd_skeleton_t *sk, re15_actor_t *pl, int ticks)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    extern void *g_anim_pose_actor;
    for (int t = 0; t < ticks; t++) {
        g_anim_pose_actor = pl;
        re15_skel_compute_pose(sk, 0, poses);
        g_anim_pose_actor = NULL;
    }
}

static void test_clamp_basis(void)
{
    printf("== (c) Klemme VOR dem Keyframe-Abzug (@0x80037598 + @0x800375a4-d0)\n");
    re15_emd_skeleton_t sk;
    const int KF_AY = 200;
    build_skel(&sk, KF_AY, 0);
    re15_actor_t *pl = setup_player();
    /* Modus 2 (relativ, @0x80037480-b0): +0x162 = lokales Yaw-Ziel, weit jenseits der
     * Klemme. Bit 0x80 aus -> Schritte = Part-Defaults 96/96. */
    pl->neck_flags = 0x08;
    pl->neck_ty = 1500;      /* +0x162 */
    pl->neck_tz = 0;         /* +0x164 */
    run_pose(&sk, pl, 20);
    int rendered = KF_AY + pl->neck_yaw;
    CHECK(rendered == 0x200,
          "gerenderter Kopf-Euler == +Klemme 0x200 (ist %d; Akku %d = 0x200-%d)",
          rendered, (int)pl->neck_yaw, KF_AY);
    CHECK(pl->neck_yaw == 0x200 - KF_AY,
          "Akku-Ziel = geklemmtes Ziel - Keyframe (%d, erwartet %d)",
          (int)pl->neck_yaw, 0x200 - KF_AY);

    /* Negative Seite: Vorzeichen aus a3 & 0x800 (@0x800375b8-c8). */
    build_skel(&sk, KF_AY, 0);
    pl = setup_player();
    pl->neck_flags = 0x08;
    pl->neck_ty = -1500;
    run_pose(&sk, pl, 20);
    rendered = KF_AY + pl->neck_yaw;
    CHECK(rendered == -0x200,
          "gerenderter Kopf-Euler == -Klemme (ist %d)", rendered);

    /* Innerhalb der Klemme bleibt das Ziel unveraendert (kein Klemmen-Artefakt). */
    build_skel(&sk, KF_AY, 0);
    pl = setup_player();
    pl->neck_flags = 0x08;
    pl->neck_ty = 300;
    run_pose(&sk, pl, 20);
    CHECK(KF_AY + pl->neck_yaw == 300,
          "unterhalb der Klemme: gerenderter Euler == Ziel 300 (ist %d)",
          KF_AY + pl->neck_yaw);
}

static void test_release_absolute(void)
{
    printf("== (d) Release-Bit 0x10: Pitch-Ziel 0 ABSOLUT (@0x80037554-60)\n");
    re15_emd_skeleton_t sk;
    const int KF_AZ = 186;               /* Savestate: PL00-Keyframe-vz +186 */
    build_skel(&sk, 0, KF_AZ);
    re15_actor_t *pl = setup_player();
    pl->neck_flags = 0x12;               /* Idle-Release (Prolog @0x80031e04) */
    run_pose(&sk, pl, 20);
    CHECK(pl->neck_pitch == -KF_AZ,
          "Pitch-Akku laeuft auf -kf_vz (%d, erwartet %d; Savestate accP=-186 bei vz=+186)",
          (int)pl->neck_pitch, -KF_AZ);
    CHECK(KF_AZ + pl->neck_pitch == 0,
          "gerenderte Gesamt-Neigung == 0 (ist %d)", KF_AZ + pl->neck_pitch);
    /* Yaw-Release (Bit 0x02) zielt dagegen auf den Keyframe: Akku -> 0. */
    CHECK(pl->neck_yaw == 0,
          "Yaw-Release-Ziel = Kopf-Keyframe -> Akku 0 (@0x80037534-48) (ist %d)",
          (int)pl->neck_yaw);

    /* Grenzfall: ein Keyframe-Pitch JENSEITS der Klemme wird beim Release trotzdem
     * auf 0 gezogen (Ziel 0 liegt innerhalb der Klemme). */
    build_skel(&sk, 0, 700);             /* > 0x138 = 312 */
    pl = setup_player();
    pl->neck_flags = 0x12;
    run_pose(&sk, pl, 30);
    CHECK(700 + pl->neck_pitch == 0,
          "grosser Keyframe-Pitch wird ebenfalls auf 0 genullt (ist %d)",
          700 + pl->neck_pitch);
}

int main(void)
{
    printf("=== NECK/HEAD-LOOK — Batch A1 (Auto-Look-Scan + Klemmen-/Release-Basis) ===\n");
    test_scan();
    test_clamp_basis();
    test_release_absolute();
    printf(g_fail ? "NECK-HEADLOOK: FAIL\n" : "NECK-HEADLOOK: OK\n");
    return g_fail ? 1 : 0;
}
