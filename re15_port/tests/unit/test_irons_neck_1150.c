/* test_irons_neck_1150.c — REGRESSIONS-PIN (ctest) zum Nutzer-Report (WIEDERHOLT)
 * "Irons schaut im Original etwas diagonaler in Irons' Office."
 *
 * WAS HIER FESTGENAGELT WIRD
 * --------------------------
 * Irons (NPC-Typ 0x45, ROOM1150) muss Leon PERMANENT mit dem Kopf verfolgen — in BEIDEN
 * Achsen (Yaw + Pitch = "diagonal"), mit den Klemmen/Schritten seines NPC-INIT.
 *
 * ORIGINAL-BELEG (selbst disassembliert; die Herleitung steht vollstaendig in
 * game_step_common.c ueber re15_npc_neck_spawn_init()):
 *   Sce_em_set  @0x800420a0 : `sw zero,4(s0)` @0x800421e0 (Entity-State 0)
 *                             `sw s0,-14460(at)` @0x80042578 (DAT_800ac784 = neue Entity)
 *                             `lw v0,0(at)` @0x80042594 / `jalr v0` @0x8004259c
 *                             -> der Typ-Root laeuft INLINE beim Spawn mit state 0,
 *                                also LAEUFT der INIT (Tabelle 0x80121738[0] = 0x8011d2b8).
 *                             @0x800425a4 `lh v0,16(s2)` = 0 in ROOM1150 -> kein state-4-Override
 *                             @0x8004260c `andi v0,0x40` auf Entity+0x9 (= 0x00 hier)
 *                                -> KEIN +0x1b8 = 0x12  =>  +0x1b8 bleibt 0 = ENTITY-TRACKING
 *   INIT 0x8011d2b8         : +0x1a8 = &player @0x8011d314 · +0x9e = 0x78 @0x8011d334 ·
 *                             +0x1b8 = 0 @0x8011d344 · +0x1b9 = 8 @0x8011d354 ·
 *                             part8+0x98 = 0x40 @0x8011d36c · part8+0x9a = 0x30 @0x8011d374 ·
 *                             part8+0x9c = 0x2c8 @0x8011d37c · part8+0x94/96 = 0 @0x8011d384/88 ·
 *                             part8+0x9e = 0x138 @0x8011d38c
 *   Root 0x8011d140         : ruft die Neck-FSM jeden Tick, `jal 0x80037358` @0x8011d278
 *   Plc_motion  @0x80041b90 : schreibt NUR +0x94/+0x4/+0x5/+0x6/+0x7/+0x1c4/+0x1c8/+0x1ca
 *                             (@0x80041ba8-c8) — +0x1b8/+0x1b9 bleiben unberuehrt.
 *   ROOM1150 main00+0x012c  : `44 00 45 00 00 00 00 ff 62 ad 30 fd ed 99 00 00 00 00 00 00`
 *                             -> Typ 0x45, Behavior 0x00, Pos (-21150,-720,-26131)
 *   ROOM1150 sub02+0x0036   : `2e 02 00` Work_set(2,0) + `3f 00 03 00` Plc_motion(0,3)
 *
 * SAVESTATE-GROUND-TRUTH (Skill re15-savestate-ghidra, 7 NPC-Instanzen / 5 saubere Saves:
 * mzd_stage1_npc, mzd_stage1_dog, mzd_stage1_maggot, orig_1170_gp, lampwalk_base): jeder
 * dort lebende NPC steht auf state=4 UND traegt +0x1b9=8, +0x1a8=0x800aca54, +0x9e=120,
 * part8 step=(64,48), clamp=(0x2c8,0x138) — Werte, die AUSSCHLIESSLICH der INIT schreibt.
 * (Alle diese Saves haben Behavior 0x40 -> +0x1b8 = 0x12; ein Save mit Behavior 0x00 /
 *  Typ 0x45 existiert nicht, das Tracking selbst ist damit statisch belegt, nicht live.)
 *
 * PORT-DEFEKT, DEN DIESER PIN BEWACHT: der Port fuehrt den NPC-INIT als `case 0` des
 * Zustands-Dispatch. Der SCD stempelt state=4 (@0x80041bb0) noch im Raum-Lade-Frame, also
 * bevor die Entity-Schleife tickt -> case 0 lief nie -> neck_bone == 0 -> die Neck-FSM war
 * fuer Irons komplett aus (Kopf = roher Keyframe, yaw 0). Gefixt durch
 * re15_npc_neck_spawn_init() in re15_game_step (Port-Aequivalent des `jalr` @0x8004259c).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"
#include "re15_game_step.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static re15_emd_skeleton_t s_sk45; static re15_emd_animation_t s_an45;
static uint8_t s_em45[0x80000];

static int load_em045(void)
{
    size_t esz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x45);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, esz, idx, &off, &len) != 0 || len > sizeof s_em45) {
        free(ems); return 0;
    }
    memcpy(s_em45, ems + off, len);
    free(ems);
    return re15_emd_parse_own_bank(s_em45, len, &s_sk45, &s_an45) == 0;
}

/* GERENDERTE Kopf-Orientierung aus der Bone-8-Weltmatrix: fwd = rot8 * (1,0,0).
 * yaw   = atan2(dz,dx)                in 4096er-Einheiten (0 = +X, +ccw wie die Engine)
 * pitch = atan2(-dy, sqrt(dx^2+dz^2)) in 4096er-Einheiten (PSX: -Y = oben)
 * (Nur MESS-Mathematik des Tests — die Engine selbst rechnet mit ihren Q12-Tabellen.) */
static void head_yaw_pitch(const re15_skel_pose_t *p8, int *yaw, int *pitch)
{
    double dx = (double)p8->rot[0], dy = (double)p8->rot[3], dz = (double)p8->rot[6];
    const double k = 4096.0 / (2.0 * 3.14159265358979);
    *yaw   = (int)(atan2(dz, dx) * k);
    *pitch = (int)(atan2(-dy, sqrt(dx * dx + dz * dz)) * k);
}

/* ROOM1150 hochfahren wie das Original: main00 spawnt Irons, danach laeuft sub02
 * (Work_set(2,0) + Plc_motion(0,3)) VOR dem ersten Entity-Tick. Anschliessend N Frames
 * in der echten Port-Reihenfolge ticken (scd_vm_tick -> re15_game_step -> AI -> Anim). */
static int run_scene(const re15_rdt_t *rdt, int32_t leon_x, int32_t leon_z, int frames,
                     re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    re15_actor_init();
    scd_vm_init();
    { extern void scd_register_current_rdt(const re15_rdt_t *); scd_register_current_rdt(rdt); }
    scd_register_room_events(rdt);
    g_current_room_id = 0x1150;
    if (rdt->main_scd)   scd_thread_start(0, rdt->main_scd);
    if (rdt->sub_scd[0]) scd_thread_start(1, rdt->sub_scd[0]);
    g_scd.work_vars[10] = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100;
    pl->x = leon_x; pl->y = 0; pl->z = leon_z; pl->rot_y = 0x05dc;
    /* Spieler-Neck-Defaults (State-0-INIT LAB_800318f8: Bone 8 @0x80031938, Steps 96/96
     * @0x800319a4-ac, Klemmen ±0x200/±0x138 @0x800319b0-c4) — re15_actor_init setzt sie,
     * hier nur explizit fuer die Lesbarkeit. */
    pl->neck_bone = 8; pl->neck_step_yaw = 96; pl->neck_step_pitch = 96;
    pl->neck_clamp_yaw = 0x200; pl->neck_clamp_pitch = 0x138; pl->neck_target_slot = -1;

    /* Der Raum-Lade-Frame: main00 (Spawn) + sub00 -> sub02 (Plc_motion -> state 4),
     * BEVOR die Entity-Schleife das erste Mal laeuft. Genau die Reihenfolge, in der der
     * Port-Defekt entstand. */
    scd_vm_tick();
    scd_event_fire(2);
    scd_vm_tick();

    int ir = -1;
    for (int f = 0; f < frames; f++) {
        scd_vm_tick();
        { re15_game_ctx_t g; memset(&g, 0, sizeof g); g.rdt_ok = 0; re15_game_step(&g); }
        re15_enemy_ai_run_all(1);
        re15_actors_anim_advance();
        if (ir < 0)
            for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && g_actors[i].type == 0x45) { ir = i; break; }
        if (ir >= 0) {
            re15_actor_t *a = &g_actors[ir];
            int kf = re15_compute_actor_kf(&s_an45, &s_sk45, a, a->motion, a->anim_frame);
            if (kf < 0) kf = 0;
            g_anim_pose_actor = a;
            re15_skel_compute_pose(&s_sk45, kf, poses);
            g_anim_pose_actor = NULL;
        }
    }
    return ir;
}

int main(void)
{
    if (!load_em045()) { fprintf(stderr, "FAIL: EM045 (CDEMD0.EMS)\n"); return 1; }

    size_t sz = 0;
    uint8_t *data = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1150.RDT", &sz);
    if (!data) { fprintf(stderr, "FAIL: ROOM1150.RDT\n"); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, sz, &rdt) != 0) { fprintf(stderr, "FAIL: RDT parse\n"); return 1; }

    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];

    /* ================= (A) Szene wie im Skript: Leon auf dem Pos_set-Platz ============
     * sub03+0x0050 `32 00 c6 af 00 00 c5 9d` = Pos_set(0, 0xafc6, 0, 0x9dc5). */
    printf("== (A) Irons trackt Leon (Skript-Standpunkt)\n");
    int ir = run_scene(&rdt, (int16_t)0xafc6, (int16_t)0x9dc5, 120, poses);
    CHECK(ir > 0, "Irons (Typ 0x45) ist gespawnt");
    if (ir <= 0) { printf("IRONS NECK 1150: %d FAIL\n", fails ? fails : 1); return 1; }

    re15_actor_t *a  = &g_actors[ir];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* --- Spawn-Operanden (main00+0x012c) --- */
    CHECK(a->x == -21150 && a->y == -720 && a->z == -26131,
          "Spawn-Position (-21150,-720,-26131), ist (%ld,%ld,%ld)",
          (long)a->x, (long)a->y, (long)a->z);
    CHECK(a->grid_id == 0x00, "Behavior-Byte pc[3] = 0x00 -> grid_id 0 (kein 0x12-Override "
          "@0x8004260c-18), ist 0x%02x", a->grid_id);
    /* --- Plc_motion hat gestempelt (state 4 / clip 3, @0x80041ba8-b0) --- */
    CHECK(a->state == 4, "sub02 Plc_motion -> state 4 (@0x80041bb0), ist %d", (int)a->state);
    CHECK(a->motion == 3, "sub02 Plc_motion -> clip 3 (@0x80041ba8), ist %d", (int)a->motion);

    /* --- ... und der INIT-Neck-Block steht TROTZDEM (das ist der eigentliche Fix) --- */
    CHECK(a->neck_bone == 8,          "+0x1b9 = 8 (@0x8011d354), ist %d", (int)a->neck_bone);
    CHECK(a->neck_flags == 0x00,      "+0x1b8 = 0 = ENTITY-TRACKING (@0x8011d344), ist 0x%02x", a->neck_flags);
    CHECK(a->neck_target_slot == RE15_ACTOR_SLOT_PLAYER,
          "+0x1a8 = &player (@0x8011d314) -> Slot %d, ist %d",
          RE15_ACTOR_SLOT_PLAYER, (int)a->neck_target_slot);
    CHECK(a->neck_step_yaw == 0x40 && a->neck_step_pitch == 0x30,
          "part8 Steps 0x40/0x30 (@0x8011d36c/74), ist %d/%d",
          (int)a->neck_step_yaw, (int)a->neck_step_pitch);
    CHECK(a->neck_clamp_yaw == 0x2c8 && a->neck_clamp_pitch == 0x138,
          "part8 Klemmen 0x2c8/0x138 (@0x8011d37c/8c), ist 0x%x/0x%x",
          (unsigned)a->neck_clamp_yaw, (unsigned)a->neck_clamp_pitch);
    CHECK((int)a->neck_speed == 0x78, "+0x9e = 0x78 (@0x8011d334), ist 0x%x",
          (unsigned)(uint16_t)a->neck_speed);

    /* --- HARTER PIN auf die GERENDERTE Kopf-Orientierung (Yaw UND Pitch getrennt) ---
     * Das ist die Groesse, die der Nutzer sieht. Ohne den Fix (Neck-FSM aus) waeren es
     * yaw = 0 / pitch = -51 (roher Keyframe von Clip 3) — gemessen mit
     * probe_irons_neck_1150 (RE15_IRONS_LANE=B). */
    int hy = 0, hp = 0;
    head_yaw_pitch(&poses[8], &hy, &hp);
    printf("   Irons(%ld,%ld,%ld) roty=%d | Leon(%ld,%ld) | acc=(%d,%d) | HEAD yaw=%d pitch=%d\n",
           (long)a->x, (long)a->y, (long)a->z, a->rot_y, (long)pl->x, (long)pl->z,
           a->neck_yaw, a->neck_pitch, hy, hp);

    CHECK(a->neck_yaw   == -615, "Yaw-Akku konvergiert auf -615, ist %d",   a->neck_yaw);
    CHECK(a->neck_pitch ==  260, "Pitch-Akku konvergiert auf +260, ist %d", a->neck_pitch);
    CHECK(hy == 614, "GERENDERTER Kopf-Yaw = 614 (54.0 Grad zu Leon; ohne Fix 0), ist %d", hy);
    CHECK(hp == -312, "GERENDERTER Kopf-Pitch = -312 = die Klemme 0x138 (@0x8011d38c; "
          "ohne Fix -51 = roher Keyframe), ist %d", hp);
    /* DIAGONAL = beide Achsen gleichzeitig ausgelenkt. */
    CHECK(a->neck_yaw != 0 && a->neck_pitch != 0,
          "Kopf ist in BEIDEN Achsen ausgelenkt (= diagonal), ist (%d,%d)",
          a->neck_yaw, a->neck_pitch);
    /* Der Yaw zeigt geometrisch auf Leon (Kopf- statt Wurzel-Position -> ein paar Grad
     * Versatz sind byte-true, deshalb +-96 Einheiten = ~8.4 Grad Toleranz). */
    {
        double wa = atan2((double)(pl->z - a->z), (double)(pl->x - a->x)) * (4096.0 / 6.283185307);
        int d = (int)(hy - wa);
        CHECK(d > -96 && d < 96,
              "Kopf-Yaw zeigt auf Leon (Weltwinkel %.0f vs Kopf %d, Delta %d)", wa, hy, d);
    }

    /* ================= (B) TRACKING-BEWEIS: Leon woanders -> anderer Kopf-Yaw ========
     * Ein konstanter Keyframe-Kopf wuerde hier denselben Wert liefern. */
    printf("== (B) Tracking: Leon an anderer Position -> Kopf folgt\n");
    int ir2 = run_scene(&rdt, -22400, -25000, 120, poses);
    CHECK(ir2 > 0, "(B) Irons gespawnt");
    if (ir2 > 0) {
        re15_actor_t *b = &g_actors[ir2];
        int hy2 = 0, hp2 = 0;
        head_yaw_pitch(&poses[8], &hy2, &hp2);
        printf("   Leon(-22400,-25000) | acc=(%d,%d) | HEAD yaw=%d pitch=%d\n",
               b->neck_yaw, b->neck_pitch, hy2, hp2);
        CHECK(hy2 != hy, "(B) Kopf-Yaw folgt Leon (A: %d, B: %d)", hy, hy2);
        /* Leon steht hier so weit hinter Irons, dass das Weltziel die Yaw-Klemme
         * ueberschreitet -> der Akku laeuft exakt auf -0x2c8 (@0x8011d37c) und der
         * gerenderte Kopf steht auf 711. Damit ist auch die YAW-Klemme festgenagelt. */
        CHECK(b->neck_yaw == -712, "(B) Yaw-Akku = -0x2c8 = die Klemme (@0x8011d37c), ist %d",
              b->neck_yaw);
        CHECK(hy2 == 711, "(B) GERENDERTER Kopf-Yaw = 711 (Yaw-Klemme), ist %d", hy2);
        CHECK(hp2 == -312, "(B) Pitch weiter an der Klemme -312, ist %d", hp2);
        CHECK(b->neck_flags == 0x00 && b->neck_bone == 8,
              "(B) Neck-Konfiguration unveraendert (fl=0x%02x bone=%d)", b->neck_flags, (int)b->neck_bone);
    }

    free(data);
    if (fails) { printf("IRONS NECK 1150: %d FAIL\n", fails); return 1; }
    printf("IRONS NECK 1150: all checks passed\n");
    return 0;
}
