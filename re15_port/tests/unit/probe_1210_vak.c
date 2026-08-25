/* probe_1210_vak.c — GEGENSONDE (Vakuanz-Linse) zum Befund "1210-ausloeser".
 * Kein add_test. Produktivcode unangetastet.
 *
 * Geprueft wird NICHT die Disasm-Kette, sondern ob die MESSUNG einen Zustand misst,
 * den das Spiel einnimmt:
 *   V1  Welche AOTs installiert der Raum wirklich (Tueren = die Enden der Bahn)?
 *   V2  Welches Kollisions-BAND setzt die Engine von selbst? (Die Sonde des Befundes
 *       erzwingt Band 0 in JEDEM Bild.)
 *   V3  Hat "0 blockierte Schritte" ueberhaupt Trennschaerfe? Gegenprobe: dieselbe
 *       Messung auf einer Bahn, die garantiert AUSSERHALB des Raums liegt.
 *   V4  Dieselbe Tor-Arithmetik wie im Befund, aber fuer BEIDE Tempi (WALK 75
 *       @0x80073ea4, RUN 200 @0x80073f24) und BEIDE Laufrichtungen.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"

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

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static void boot_room(void)
{
    re15_actor_t *pl;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    pl->x = -19500; pl->z = 5000;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { printf("FAIL: RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    boot_room();
    printf("=== V2: BAND, das die Engine SELBST fuehrt (kein set_band-Zwang) ===\n");
    printf("   nach Raum-Init: band=%d\n", re15_collision_debug_band());
    { int f; for (f = 0; f < 8; f++) frame_step(); }
    printf("   nach 8 Bildern (Spieler y=0, z=5000): band=%d\n", re15_collision_debug_band());
    {   re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int f;
        pl->x = -19500; pl->z = -14000;
        for (f = 0; f < 4; f++) frame_step();
        printf("   im Flur (-19500,-14000), 4 Bilder: band=%d (Spieler y=%ld)\n",
               re15_collision_debug_band(), (long)pl->y);
    }

    printf("\n=== V1: AOT-Belegung nach Raum-Init (Tueren = Enden der natuerlichen Bahn) ===\n");
    {   int i;
        for (i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active) continue;
            printf("  [%2d] type=%2u flags=0x%02x band=%u quad=%u  x %7ld..%7ld  z %7ld..%7ld",
                   i, a->type, a->sce_flags, a->band, a->has_quad,
                   (long)(a->x - a->half_w), (long)(a->x + a->half_w),
                   (long)(a->z - a->half_h), (long)(a->z + a->half_h));
            if (a->type == RE15_AOT_TYPE_DOOR) {
                const re15_aot_door_params_t *d = &g_aot.door_params[i];
                printf("  TUER -> stage %u room 0x%02x  spawn(%ld,%ld,%ld) yaw=%d",
                       d->dest_stage, d->dest_room,
                       (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z,
                       (int)d->spawn_yaw_4096);
            }
            printf("\n");
        }
    }

    printf("\n=== V3: Trennschaerfe von \"0 blockierte Schritte\" ===\n");
    {   const int32_t xs[2] = { -19500, -35000 };
        int b;
        for (b = 0; b < 2; b++) {
            int32_t px = xs[b], pz = -3500; int blocked = 0, steps = 0, f;
            for (f = 0; f < 287; f++) {
                int32_t nx = px, nz = pz - 75;
                re15_collision_set_band(0);
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                if (nz != pz - 75 || nx != px) blocked++;
                px = nx; pz = nz; steps++;
            }
            printf("  Bahn x=%7ld : %d von %d Schritten verkuerzt, Ende (%ld,%ld)\n",
                   (long)xs[b], blocked, steps, (long)px, (long)pz);
        }
        printf("  -> Ist der Zaehler auf BEIDEN Bahnen 0, belegt er keine Begehbarkeit.\n");
    }

    printf("\n=== V4: Tor-Arithmetik fuer beide Tempi und beide Richtungen ===\n");
    {   int32_t ax[32], az[32]; int n = 0, s, d, h, i, f;
        const int32_t sp[2] = { 75, 200 };
        const char  *spn[2] = { "WALK 75 @0x80073ea4", "RUN 200 @0x80073f24" };
        const int32_t H[3]  = { 603, 850, 1700 };
        boot_room();
        for (f = 0; f < 8; f++) frame_step();
        for (s = 1; s < RE15_ACTOR_MAX && n < 32; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            ax[n] = e->x; az[n] = e->z; n++;
        }
        for (d = 0; d < 2; d++) {
            for (s = 0; s < 2; s++) {
                for (h = 0; h < 3; h++) {
                    int fired[32], lead[32], maxsim = 0, react = 0;
                    int minlead = 9999, maxlead = -9999;
                    int32_t px = -19500, z0, step;
                    for (i = 0; i < n; i++) { fired[i] = -1; lead[i] = -1; }
                    z0   = d ? -25025 : -3500;
                    step = d ? +sp[s] : -sp[s];
                    for (f = 0; f < 4000; f++) {
                        int32_t pz = z0 + step * f;
                        int sim = 0;
                        if (d && pz > -3500) break;
                        if (!d && pz < -25025) break;
                        for (i = 0; i < n; i++) {
                            int32_t dz = pz - az[i], dx = px - ax[i];
                            int in, on;
                            if (dz < 0) dz = -dz;
                            if (dx < 0) dx = -dx;
                            in = (dz < H[h]) && (dx < 11000);
                            if (in) { sim++; if (fired[i] < 0) fired[i] = f; }
                            on = d ? (pz >= az[i]) : (pz <= az[i]);
                            if (lead[i] < 0 && on) lead[i] = f;
                        }
                        if (sim > maxsim) maxsim = sim;
                    }
                    for (i = 0; i < n; i++) {
                        if (fired[i] < 0) continue;
                        react++;
                        if (lead[i] >= 0) {
                            int L = lead[i] - fired[i];
                            if (L < minlead) minlead = L;
                            if (L > maxlead) maxlead = L;
                        }
                    }
                    printf("  %-4s %-20s H=%4ld : reagieren %2d/%d, max %d gleichzeitig, "
                           "Vorlauf %d..%d Bilder\n",
                           d ? "S->N" : "N->S", spn[s], (long)H[h], react, n, maxsim,
                           minlead, maxlead);
                }
            }
        }
        printf("  (Die Lunge dauert 3 Bilder: +0x9c=3 @0x8010c7a8, +0x8c=0x320 @0x8010c7b8.\n"
               "   Vorlauf minus 3 = Bilder, die der Arm fertig ausgefahren dasteht.)\n");
    }

    printf("\n=== V5: EINTRITT DURCH EINE FLUR-TUER (der Fall, den die Bahn-Messung auslaesst) ===\n");
    {   /* Tuer 3 (Rechteck z -14700..-12500) und Tuer 4 (z -18400..-16200) liegen MITTEN im
         * Flur. Wer dort hereinkommt, naehert sich keinem Arm — er steht sofort neben einem.
         * Gemessen mit der ECHTEN AI (kein Handsetzen eines States). */
        /* Die ANKUNFTS-Punkte stehen nicht in ROOM1210, sondern in den Tueren der
         * Nachbarraeume: ROOM1220.RDT / ROOM1221.RDT, Door_aot_set (0x3B) @Datei
         * 0x0DD2 / 0x0DF2 / 0x0E12 / 0x0E32 / 0x0E52, Ziel stage=0 room=0x21,
         * next_pos = pc[14..19]. */
        const int32_t xin[5] = { -20890, -18100, -20800, -18100, -18100 };
        const int32_t zin[5] = {  -6560,  -9900, -13700, -17300, -25600 };
        const char  *zn[5]   = { "ROOM1220 @0x0DD2 -> (-20890,-6560)",
                                 "ROOM1220 @0x0DF2 -> (-18100,-9900)",
                                 "ROOM1220 @0x0E12 -> (-20800,-13700)",
                                 "ROOM1220 @0x0E32 -> (-18100,-17300)",
                                 "ROOM1220 @0x0E52 -> (-18100,-25600)" };
        int k;
        for (k = 0; k < 5; k++) {
            re15_actor_t *pl;
            int f, s2, out = 0;
            boot_room();
            pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            for (f = 0; f < 8; f++) { re15_collision_set_band(0); frame_step(); }
            for (f = 0; f < 5; f++) {
                int32_t nx = xin[k], nz = zin[k];
                re15_collision_set_band(0);
                /* Ankunft + Push-out, wie beim Tuer-Eintritt. */
                re15_collision_constrain(&s_rdt, xin[k], zin[k], &nx, &nz);
                pl->x = nx; pl->z = nz;
                frame_step();
            }
            for (s2 = 1; s2 < RE15_ACTOR_MAX; s2++) {
                re15_actor_t *e = &g_actors[s2];
                if (!e->active || e->type != 0x1A) continue;
                if (e->sub_state_1 != 0) {
                    int32_t dz = pl->z - e->z; if (dz < 0) dz = -dz;
                    out++;
                    printf("        -> Arm slot %d @z %ld  |dz| = %ld\n",
                           s2, (long)e->z, (long)dz);
                }
            }
            printf("  %-36s Spieler (%ld,%ld): %d Arme sofort in Bewegung (Vorlauf 0)\n",
                   zn[k], (long)pl->x, (long)pl->z, out);
        }
    }
    return 0;
}
