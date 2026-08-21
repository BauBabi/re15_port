/* probe_1090_push.c — MESSSONDE "Kisten schieben" (ROOM1090).
 *
 * Teil 1 (Datenlage): PL00.EDD Clip 0x11 (17) / 0x12 (18) — Frame-Zahl, EDD-Eintrags-Flags
 *   (Bit 0x2000 = der Root-Motion-Tick @0x80035a5c) und die Root-Translation je Keyframe
 *   (EMR-Keyframe-Bytes 0..5, gelesen mit re15_emd_get_keyframe_speed).
 * Teil 2 (Kette): ROOM1090 laden, Spieler vor eine der beiden Typ-4-Kisten stellen, UP halten
 *   und Frame fuer Frame Zaehler / Pose / Objektposition protokollieren.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;

static uint8_t *read_file(const char *path, size_t *out_size)
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

static re15_emd_animation_t s_anim;
static re15_emd_skeleton_t  s_skel;
static int load_pl00(const char *base)
{
    char p[600];
    size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", base);
    uint8_t *edd = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", base);
    uint8_t *emr = read_file(p, &rsz);
    if (!edd || !emr) return 0;
    if (re15_emd_parse_animation(edd, esz, &s_anim) != 0) return 0;
    if (re15_emd_parse_skeleton (emr, rsz, &s_skel) != 0) return 0;
    return 1;
}

static void dump_clip(int clip)
{
    if (clip < 0 || clip >= s_anim.clip_count) { printf("  Clip %d: nicht vorhanden\n", clip); return; }
    const re15_emd_clip_t *c = &s_anim.clips[clip];
    printf("  Clip %2d (0x%02X): %d Frames\n", clip, clip, c->frame_count);
    for (int i = 0; i < c->frame_count; i++) {
        uint32_t e = re15_emd_get_frame_entry(&s_anim, clip, i);
        int kf = (int)(e & 0xFFFu);
        int16_t sx = 0, sy = 0, sz = 0;
        int ok = re15_emd_get_keyframe_speed(&s_skel, kf, &sx, &sy, &sz);
        printf("    f%02d entry=%08X kf=%3d flags=%04X%s  root=(%6d,%6d,%6d)%s\n",
               i, e, kf, (unsigned)((e >> 12) & 0xFFFFu),
               (e & 0x2000u) ? " [0x2000]" : "         ",
               ok ? sx : 0, ok ? sy : 0, ok ? sz : 0, ok ? "" : " (kein KF)");
    }
}

/* Ein Frame des Port-Zyklus in der byte-true Reihenfolge des Originals:
 *   Spieler-FSM (@0x8001ce0c) -> Wandklemme FUN_8003b0a4 -> Objekt-Pass FUN_8002bd44
 *   (@0x8001ce14: erst der SCHUB @0x8002bfa4, dann die Spieler-Ausschiebung @0x8002c0d8). */
static void one_frame(const re15_rdt_t *rdt, re15_actor_t *pl, uint16_t pad)
{
    int32_t ox = pl->x, oz = pl->z;
    re15_player_tick(NULL, pad);
    int32_t nx = pl->x, nz = pl->z;
    re15_collision_constrain(rdt, ox, oz, &nx, &nz);
    pl->x = nx; pl->z = nz;
    re15_prop_push_tick(rdt, pad);
    re15_collision_objects(&nx, &nz);
    pl->x = nx; pl->z = nz;
}

/* Setzt den Spieler `back` Einheiten hinter die buendige Schiebe-Position der Kiste und
 * laesst ihn UP halten. yaw 0/1024/2048/3072 = Blick +X/-Z/-X/+Z (vorwaerts = (cos,-sin)). */
static void place_before_box(re15_actor_t *pl, int box, int yaw, int32_t back)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = c, fz = -s;                                  /* Einheitsvektor * 4096 */
    int32_t d = 1350 + back;                                  /* 900 (Box) + 450 (Spieler) */
    pl->x = g_scd.props[box].x + (int32_t)g_scd.props[box].box_cx - (int32_t)((fx * d) >> 12);
    pl->z = g_scd.props[box].z + (int32_t)g_scd.props[box].box_cz - (int32_t)((fz * d) >> 12);
    pl->y = (int32_t)g_scd.props[box].y;
    pl->rot_y = (int16_t)yaw;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0; pl->anim_flags = 0;
    re15_collision_set_band((int)g_scd.props[box].band);
}

static void run_scenario(const re15_rdt_t *rdt, int box, int yaw, const char *label,
                         int frames, int verbose)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t bx0 = g_scd.props[box].x, bz0 = g_scd.props[box].z;
    re15_player_push_reset();
    re15_prop_push_reset();
    place_before_box(pl, box, yaw, 0);
    printf("\n-- prop[%d], Schubrichtung %s (yaw %d): Start Kiste=(%ld,%ld) Spieler=(%ld,%ld) --\n",
           box, label, yaw, (long)bx0, (long)bz0, (long)pl->x, (long)pl->z);
    if (verbose)
        printf("   F |  px      pz   | Zaehler Kontakt sub8 phase Clip Frame | Kiste x      z\n");
    for (int f = 0; f < frames; f++) {
        int32_t px = g_scd.props[box].x, pz = g_scd.props[box].z;
        one_frame(rdt, pl, RE15_PAD_BIT_UP);
        if (verbose)
            printf("  %2d | %7ld %7ld |   %2d      %d     %d   %2d   %3d  %3d | %7ld %7ld%s\n",
                   f, (long)pl->x, (long)pl->z,
                   re15_prop_push_counter(box), re15_prop_push_contact(),
                   re15_player_push_substate(), re15_player_push_phase(),
                   (int)pl->motion, (int)pl->anim_frame,
                   (long)g_scd.props[box].x, (long)g_scd.props[box].z,
                   (g_scd.props[box].x != px || g_scd.props[box].z != pz) ? "  <== VERSCHOBEN" : "");
    }
    printf("   ENDE nach %d Bildern: Kiste=(%ld,%ld)  Weg=(%+ld,%+ld)  Zaehler=%d phase=%d\n",
           frames, (long)g_scd.props[box].x, (long)g_scd.props[box].z,
           (long)(g_scd.props[box].x - bx0), (long)(g_scd.props[box].z - bz0),
           re15_prop_push_counter(box), re15_player_push_phase());
    /* UP loslassen -> Ausstieg (@0x800357a8/c4 -> +0x06 = 6 -> 7 -> Idle). */
    int exit_f = -1;
    for (int f = 0; f < 40; f++) {
        one_frame(rdt, pl, 0);
        if (exit_f < 0 && re15_player_push_substate() == 0) exit_f = f;
    }
    printf("   UP los: Substate 8 verlassen nach %d Bildern, Zaehler=%d Clip=%d\n",
           exit_f, re15_prop_push_counter(box), (int)pl->motion);
    /* Kiste fuer den naechsten Lauf zuruecksetzen. */
    g_scd.props[box].x = bx0; g_scd.props[box].z = bz0;
}

/* Negativprobe: dieselbe Ausgangslage, aber ein Tor verletzt. Rueckgabe 1 = Kiste bewegt. */
static int neg_case(const re15_rdt_t *rdt, int box, int yaw,
                    int hold_up, int obj_type, int band, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t bx0 = g_scd.props[box].x, bz0 = g_scd.props[box].z;
    const uint8_t t0 = g_scd.props[box].obj_type, b0 = g_scd.props[box].band;
    re15_player_push_reset();
    re15_prop_push_reset();
    place_before_box(pl, box, yaw, back);
    g_scd.props[box].obj_type = (uint8_t)obj_type;
    g_scd.props[box].band     = (uint8_t)band;
    re15_collision_set_band((int)b0);      /* Spieler-Band bleibt das Original-Band */
    for (int f = 0; f < 60; f++) one_frame(rdt, pl, hold_up ? RE15_PAD_BIT_UP : 0);
    int moved = (g_scd.props[box].x != bx0 || g_scd.props[box].z != bz0);
    g_scd.props[box].x = bx0; g_scd.props[box].z = bz0;
    g_scd.props[box].obj_type = t0; g_scd.props[box].band = b0;
    return moved;
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    if (!load_pl00(base)) { printf("FEHLT: PLD/PL00.EDD bzw. PL00.EMR\n"); return 77; }
    printf("PL00: %d Bones, %d Clips, %d Keyframes\n",
           s_skel.bone_count, s_anim.clip_count, s_skel.keyframe_count);
    printf("SCHIEBE-CLIPS (Sub-Update 8, @0x8003593c Clip 0x11 / @0x800359e4 Clip 0x12):\n");
    dump_clip(0x11);
    dump_clip(0x12);

    /* ---------------- Teil 2: die Kette am echten Raum ---------------- */
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("FEHLT: %s\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("RDT-PARSE FEHLGESCHLAGEN\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_player_set_pl00_banks(&s_skel, &s_anim);
    re15_player_push_reset();
    re15_prop_push_reset();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    int box = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_type == 4 && g_scd.props[i].band == 1) box = i;
    if (box < 0) { printf("keine Typ-4-Kiste im Band 1\n"); return 1; }
    printf("\nKISTE prop[%d]: pos=(%ld,%ld,%ld) band=%u box c=(%d,%d,%d) h=(%d,%d,%d)\n",
           box, (long)g_scd.props[box].x, (long)g_scd.props[box].y, (long)g_scd.props[box].z,
           g_scd.props[box].band,
           g_scd.props[box].box_cx, g_scd.props[box].box_cy, g_scd.props[box].box_cz,
           g_scd.props[box].box_hx, g_scd.props[box].box_hy, g_scd.props[box].box_hz);

    /* SCA-Zellen, die die Validierung FUN_8003b558(obj,2) sehen kann (Band 1, (2 & u0) != 0). */
    printf("\n-- SCA-Zellen Band 1 mit (2 & u0) im Umkreis der Kiste --\n");
    for (int i = 0; i < rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &rdt.sca[i];
        if ((e->floor >> 4) != 1) continue;
        if ((2u & e->u0) == 0) continue;
        if (e->x > -4000 || e->x + e->width < -12000) continue;
        printf("   #%3d typ=%d u0=0x%02X rect x=[%d..%d] z=[%d..%d]\n",
               i, e->type, (unsigned)e->u0, (int)e->x, (int)(e->x + e->width),
               (int)e->z, (int)(e->z + e->density));
    }

    /* Steht der Spieler auf der buendigen Schiebeposition ueberhaupt frei? (Spieler-Klemme
     * FUN_8003b0a4: Radius 450, Solid-Maske 1, eigenes Band.) */
    printf("\n-- Freiheitsprobe der 4 Schiebe-Standorte (Radius 450, Maske 1) --\n");
    for (int b = 0; b < (int)g_scd.prop_count; b++) {
        if (!g_scd.props[b].active || g_scd.props[b].obj_type != 4) continue;
        static const int yaws[4] = { 0, 1024, 2048, 3072 };
        static const char *nm[4] = { "+X", "-Z", "-X", "+Z" };
        for (int k = 0; k < 4; k++) {
            int32_t c = re15_cos_q12(yaws[k]), s = re15_sin_q12(yaws[k]);
            int32_t px = g_scd.props[b].x + g_scd.props[b].box_cx - (int32_t)((c * 1350) >> 12);
            int32_t pz = g_scd.props[b].z + g_scd.props[b].box_cz - (int32_t)((-s * 1350) >> 12);
            int blocked = re15_collision_box_blocked(&rdt, px, pz, (int)g_scd.props[b].band, 450, 1u);
            printf("   prop[%d] %s: Standort (%ld,%ld) %s\n", b, nm[k], (long)px, (long)pz,
                   blocked ? "IN EINER WAND" : "frei");
        }
    }

    run_scenario(&rdt, box, 0,    "+X", 60, 1);
    run_scenario(&rdt, box, 1024, "-Z", 60, 0);
    run_scenario(&rdt, box, 2048, "-X", 60, 0);
    run_scenario(&rdt, box, 3072, "+Z", 60, 0);

    int box5 = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_type == 4 && g_scd.props[i].band == 5) box5 = i;
    if (box5 >= 0) {
        printf("\n================ KISTE prop[%d] (Band 5) ================\n", box5);
        run_scenario(&rdt, box5, 0,    "+X", 90, 0);
        run_scenario(&rdt, box5, 1024, "-Z", 90, 1);
        run_scenario(&rdt, box5, 2048, "-X", 90, 0);
        run_scenario(&rdt, box5, 3072, "+Z", 90, 1);
    }

    /* ---- NEGATIVPROBEN (dieselbe Ausgangslage, je EIN Tor verletzt) ---- */
    printf("\n================ NEGATIVPROBEN ================\n");
    printf("  ohne UP:      %s\n", neg_case(&rdt, box, 0, 0, 4, 1, 0) ? "VERSCHOBEN (FEHLER)" : "unbewegt");
    printf("  Typ 0:        %s\n", neg_case(&rdt, box, 0, 1, 0, 1, 0) ? "VERSCHOBEN (FEHLER)" : "unbewegt");
    printf("  falsches Band:%s\n", neg_case(&rdt, box, 0, 1, 4, 3, 0) ? "VERSCHOBEN (FEHLER)" : "unbewegt");
    printf("  ausser Reichweite (Start 4000 zurueck): %s\n",
           neg_case(&rdt, box, 0, 1, 4, 1, 4000) ? "VERSCHOBEN (FEHLER)" : "unbewegt");

    free(raw);
    return 0;
}
