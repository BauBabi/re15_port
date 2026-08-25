/* probe_1210_griff.c — MESSUNG: haelt der ROOM1210-Arm den Spieler jetzt wirklich fest?
 *
 * Nutzer-Auftrag: "Ja, stell das um, wenn dir da was fehlt, hole die Animation aus Resident
 * Evil 2." Der Arm (EM01A) hat kein Paar 3; er leiht sich die Opfer-Bank des Zombies (0x10)
 * ueber re15_victim_donor_set. Diese Sonde baut denselben Zustand wie die Plattform auf und
 * misst, WAS passiert — keine Behauptung, kein Pass/Fail.
 *
 * KEIN add_test — reine Messsonde. */
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
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"   /* re15_sin_q12/cos_q12, Posen-Helfer */
#include "re15_math.h"       /* re15_squareroot0 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob1a[0x80000];
static uint8_t            s_blob10[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Eine RE1.5-Bank aus CDEMD0.EMS in die Registry legen (wie pc_enemy_load ohne Renderer). */
static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len,
                                                          &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len,
                                                            &eb->skel_victim, &eb->anim_victim) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1210: haelt der Arm fest? ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);

    printf("  EM01A-Bank: %s\n", load_bank(0x1A, s_blob1a, sizeof s_blob1a) ? "ok" : "FEHLT");
    int d = load_bank(0x10, s_blob10, sizeof s_blob10);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    printf("  EM010-Spenderbank: %s, victim_ok=%d, victim-clips=%d\n",
           d ? "ok" : "FEHLT", db ? db->victim_ok : -1,
           (db && db->victim_ok) ? db->anim_victim.clip_count : -1);
    if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);

    for (int f = 0; f < 8; f++) frame_step();

    /* Einen Arm suchen und den Spieler direkt davor stellen (in seiner Blickrichtung). */
    int target = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) { target = s; break; }
    if (target < 0) { printf("FAIL: kein Arm im Raum\n"); return 1; }
    re15_actor_t *arm = &g_actors[target];
    printf("  Ziel: slot %d pos=(%ld,%ld) yaw=%d\n",
           target, (long)arm->x, (long)arm->z, (int)arm->rot_y);

    /* 400 Einheiten vor dem Arm, in seiner Blickrichtung (Mesh-Yaw-Konvention +0x400). */
    int32_t sn = re15_sin_q12(((int)arm->rot_y + 0x400) & 0xfff);
    int32_t cs = re15_cos_q12(((int)arm->rot_y + 0x400) & 0xfff);
    pl->x = arm->x + (int32_t)((sn * 400) >> 12);
    pl->z = arm->z + (int32_t)((cs * 400) >> 12);
    printf("  Spieler auf (%ld,%ld)\n", (long)pl->x, (long)pl->z);

    /* ---- MESSUNG A: der Wand-Durchlauf im Gehtempo (75/Bild, FUN_80041BE4 @0x80076cXX) ----
     * Fuer jeden Arm: wie nah kommt der Spieler waehrend welcher Phase? Das entscheidet, ob
     * ein GEHENDER Spieler ueberhaupt in RE2s Griff-Tor (Abstand < 1200 @0x801018f4) faellt. */
    {   int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            if (e->z < zmin) zmin = e->z;
            if (e->z > zmax) zmax = e->z;
        }
        int32_t best[RE15_ACTOR_MAX]; uint8_t bsub[RE15_ACTOR_MAX];
        for (int i = 0; i < RE15_ACTOR_MAX; i++) { best[i] = 0x7fffffff; bsub[i] = 255; }
        int gr = 0;
        for (int32_t z = zmax + 1500; z > zmin - 1500; ) {
            if (!re15_player_is_grabbed()) { pl->x = -25000 + 600; pl->z = z; z -= 75; }
            pl->hp = 100;
            frame_step();
            if (re15_player_victim_state()) gr++;
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active || e->type != 0x1A || e->x != -25000) continue;
                int32_t d = re15_enemy_player_dist(e, pl);
                if (d < best[s]) { best[s] = d; bsub[s] = e->sub_state_1; }
            }
        }
        printf("\n  -- Wand-Durchlauf im Gehtempo 75/Bild, x = -24400 --\n");
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (best[s] != 0x7fffffff)
                printf("     Arm slot %2d: dichteste Annaeherung %ld (Phase %u)\n",
                       s, (long)best[s], bsub[s]);
        printf("     Opfer-Bilder im Durchlauf: %d\n", gr);
        re15_player_victim_reset();
    }

    /* ---- MESSUNG B: WIE NAH KANN EIN ECHTER SPIELER UEBERHAUPT? -------------------------
     * Die Wand-Messung oben teleportiert den Spieler an der Kollision vorbei. Entscheidend
     * fuer das echte Spiel ist aber, wie weit re15_collision_constrain ihn an das Gitter
     * heranlaesst — RE2s Griff-Tor verlangt Abstand < 1200 (@0x801018f4). Also fuer jeden Arm
     * schrittweise aus der Flurmitte auf ihn zu gehen und messen, wo die Kollision stoppt. */
    {   printf("\n  -- Wie nah laesst die Kollision den Spieler an jeden Arm? --\n");
        int reachable = 0, total = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            total++;
            int32_t px = -19500, pz = e->z;              /* Flurmitte auf Hoehe des Arms */
            re15_actor_t probe = *pl; probe.x = px; probe.z = pz;
            for (int step = 0; step < 400; step++) {     /* 25er-Schritte auf den Arm zu */
                int32_t nx = px + ((e->x > px) ? 25 : -25), nz = pz;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                if (nx == px && nz == pz) break;         /* Kollision haelt */
                px = nx; pz = nz;
                if (px == e->x) break;
            }
            probe.x = px; probe.z = pz;
            int32_t d = re15_enemy_player_dist(e, &probe);
            int ok = (d < 0x4b0);
            reachable += ok;
            printf("     Arm slot %2d @(%6ld,%6ld): erreichbar bis x=%6ld -> Abstand %4ld  %s\n",
                   s, (long)e->x, (long)e->z, (long)px, (long)d,
                   ok ? "IM GRIFF-TOR" : "zu weit");
        }
        printf("     -> %d von %d Armen sind fuer einen laufenden Spieler erreichbar\n",
               reachable, total);
        /* Gegenprobe zur Constrain-Messung: den BEGEHBAREN Boden direkt abtasten. */
        for (int s = 1; s <= 4; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            int32_t lo = 0, hi = 0; int have = 0;
            for (int32_t x = -30000; x <= -8000; x += 50)
                if (re15_collision_on_floor(&s_rdt, x, e->z)) {
                    if (!have) { lo = x; have = 1; }
                    hi = x;
                }
            printf("     Boden-Abtastung auf z=%6ld: begehbar x %6ld .. %6ld  (Arm auf %6ld)\n",
                   (long)e->z, (long)lo, (long)hi, (long)e->x);
        }
        /* 2D-Abtastung: der NAECHSTE begehbare Punkt zu jedem Arm. Das ist das ehrliche Mass —
         * ROOM1210 ist kein gerader Flur, die Boden-Zeile oben haengt vom z ab. */
        printf("\n  -- Naechster begehbarer Punkt zu jedem Arm (2D-Abtastung, 50er-Raster) --\n");
        int in_gate = 0, cnt = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            cnt++;
            int32_t bd = 0x7fffffff, bx = 0, bz = 0;
            for (int32_t x = e->x - 6000; x <= e->x + 6000; x += 50)
                for (int32_t z = e->z - 6000; z <= e->z + 6000; z += 50) {
                    if (!re15_collision_on_floor(&s_rdt, x, z)) continue;
                    re15_actor_t probe = *pl; probe.x = x; probe.z = z;
                    int32_t d = re15_enemy_player_dist(e, &probe);
                    if (d < bd) { bd = d; bx = x; bz = z; }
                }
            if (bd < 0x4b0) in_gate++;
            printf("     Arm slot %2d @(%6ld,%6ld): naechster Boden (%6ld,%6ld) Abstand %5ld  %s\n",
                   s, (long)e->x, (long)e->z, (long)bx, (long)bz, (long)bd,
                   (bd < 0x4b0) ? "IM GRIFF-TOR" : "ausserhalb");
        }
        printf("     -> %d von %d Armen koennen einen Spieler ueberhaupt erreichen\n",
               in_gate, cnt);
    }

    /* ---- MESSUNG C: WIE WEIT REICHT DER ARM? --------------------------------------------
     * Wenn der Koerper hinter dem Gitter steht, ist der Abstand zum ENTITY-URSPRUNG das
     * falsche Mass — RE2 misst zwischen zwei Koerpern, die im selben Flur stehen. Hier
     * entscheidet, wie weit die HAND aus dem Gitter kommt. Das steht in den Daten: die
     * Bone-Positionen der EM01A-Pose. Also fuer jeden Clip und jeden Keyframe die groesste
     * Bone-Auslenkung entlang der Blickrichtung messen. */
    {   re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
        printf("\n  -- Wie weit reicht der Arm (EM01A-Posen, Bone-Auslenkung)? --\n");
        if (ab && ab->ok) {
            static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            for (int c = 0; c < ab->anim.clip_count && c < 8; c++) {
                int32_t maxf = 0, maxr = 0;
                for (int f = 0; f < ab->anim.clips[c].frame_count; f++) {
                    int kf = (int)(ab->anim.frames[ab->anim.clips[c].first_frame + f] & 0xfffu);
                    if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                    for (int bn = 0; bn < ab->skel.bone_count; bn++) {
                        int32_t w[3];
                        re15_skel_bone_to_world(poses[bn].trans, 0, 0, 0, 0, w);
                        int32_t fwd = w[0];                  /* Blickrichtung bei yaw 0 = +X */
                        int32_t rad = (int32_t)re15_squareroot0(
                            (uint32_t)((int64_t)w[0]*w[0] + (int64_t)w[2]*w[2]));
                        if (fwd > maxf) maxf = fwd;
                        if (rad > maxr) maxr = rad;
                    }
                }
                printf("     Clip %d (%3d Bilder): max. Auslenkung nach vorn %5ld, "
                       "max. Radius %5ld\n",
                       c, ab->anim.clips[c].frame_count, (long)maxf, (long)maxr);
            }
        } else {
            printf("     (EM01A-Bank fehlt)\n");
        }
    }

    int held = 0, bites = 0, min_hp = 100;
    int16_t prev_hp = pl->hp;
    for (int f = 0; f < 400; f++) {
        frame_step();
        int vs = re15_player_victim_state();
        if (vs) held++;
        if (pl->hp < prev_hp) { bites++; printf("    f%3d BISS: hp %d -> %d\n",
                                                f, (int)prev_hp, (int)pl->hp); }
        prev_hp = pl->hp;
        if (pl->hp < min_hp) min_hp = pl->hp;
        if (f < 90 || (f % 25) == 0)
            printf("    f%3d arm[sub=%u/%u clip=%u fr=%u] victim=%d var=%u grabbed=%d "
                   "PL[clip=%u fr=%u hp=%d] typ=0x%02X\n",
                   f, arm->sub_state_1, arm->sub_state_2, arm->motion, arm->anim_frame,
                   vs, (unsigned)0, re15_player_is_grabbed(),
                   pl->motion, pl->anim_frame, (int)pl->hp, re15_player_victim_type());
        if (pl->hp < 0) { printf("    f%3d Spieler tot\n", f); break; }
        pl->hp = 100; prev_hp = 100;   /* am Leben halten, damit mehrere Griffe messbar sind */
    }
    printf("\n  Bilder im Opfer-Zustand: %d von 400; Bisse: %d; tiefster HP-Stand: %d\n",
           held, bites, min_hp);

    /* ---- MESSUNG D: hat die Raum-Bank snd1 einen Record 3 (RE2s Biss-SE)? -------------- */
    {   const uint8_t *edt = s_rdt.snd_edt[1];
        int n = s_rdt.snd_edt_size[1];
        printf("\n  -- ROOM1210 snd1-EDT: %d Byte --\n", n);
        for (int id = 0; id < 8 && (id + 1) * 4 <= n; id++)
            printf("     SE %d: %02x %02x %02x %02x\n", id,
                   edt[id*4], edt[id*4+1], edt[id*4+2], edt[id*4+3]);
    }

    free(buf);
    printf("\nprobe_1210_griff: MESSUNG FERTIG\n");
    return 0;
}
