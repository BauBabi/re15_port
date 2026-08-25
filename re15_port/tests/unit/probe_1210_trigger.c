/* probe_1210_trigger.c — MESSUNG (kein Test, kein add_test):
 * WANN faehrt im Port der erste Arm aus, und wo steht der Spieler dann?
 *
 * Bezugsgroessen aus den Daten (nicht geschaetzt):
 *   Original-Ausloeser  = ROOM1210.RDT sub00, Aot_set @Datei 0x1EAE
 *                         2c 06 03 41 00 00 ac a9 68 c5 50 14 a4 06 ff 00 18 02 00 00
 *                         -> aot=6 sce=3 sat=0x41 x=-22100 z=-15000 w=5200 d=1700
 *   Rechteck-Test       = FUN_80042b64 @0x80042b64:
 *                         lh v0,0(a1) / lw v1,0(a2) / lhu a0,4(a1) / subu v1,v1,v0 /
 *                         sltu a0,a0,v1   =>  (unsigned)(px - x) <= w   (ECKE + Ausdehnung)
 *   Gehtempo            = 75/Bild (WALK 0x4B, FUN_80041BE4 / 0x80076cXX)
 *
 * VAKUANZ-HINWEIS: Bahn 1 setzt den Spieler pro Bild (wie test_1210_gitterhaende) —
 * das ist KEINE begehbare Bahn. Bahn 2 laeuft ueber re15_collision_constrain und wird
 * zusaetzlich mit re15_collision_on_floor gegengeprueft; nur ihre Zahlen zaehlen.
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
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_math.h"

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
static uint8_t            s_blob[0x80000];
static uint8_t            s_blob_donor[0x80000];

static int32_t pl_path_x[512], pl_path_z[512];
static int     path_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
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

/* Das ORIGINAL-Rechteck, byte-genau aus dem SCD (Ecke + Ausdehnung, FUN_80042b64). */
#define RECT_X (-22100)
#define RECT_Z (-15000)
#define RECT_W ( 5200)
#define RECT_D ( 1700)
static int in_rect(int32_t px, int32_t pz)
{   /* exakt der unsigned-Test: (unsigned)(p - corner) <= extent */
    return ((uint32_t)(px - RECT_X) <= (uint32_t)RECT_W) &&
           ((uint32_t)(pz - RECT_Z) <= (uint32_t)RECT_D);
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

    printf("=== ROOM1210: Ausloese-Zeitpunkt der Gitterhaende ===\n");
    printf("  Original-Rechteck (Ecke+Ausdehnung): x %d..%d, z %d..%d  (Mitte %d,%d)\n",
           RECT_X, RECT_X + RECT_W, RECT_Z, RECT_Z + RECT_D,
           RECT_X + RECT_W/2, RECT_Z + RECT_D/2);

    {   const uint8_t *p = buf + 0x1EAE;
        printf("  RDT@0x1EAE:");
        for (int i = 0; i < 20; i++) printf(" %02x", p[i]);
        printf("\n            -> op=%02x aot=%u sce=%u sat=0x%02x band=%u x=%d z=%d w=%u d=%u "
               "payload=%02x %02x %02x %02x %02x %02x\n",
               p[0], p[1], p[2], p[3], p[4],
               (int)(int16_t)(p[6] | (p[7] << 8)), (int)(int16_t)(p[8] | (p[9] << 8)),
               (unsigned)(p[10] | (p[11] << 8)), (unsigned)(p[12] | (p[13] << 8)),
               p[14], p[15], p[16], p[17], p[18], p[19]);
    }

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
    load_bank(0x1A, s_blob, sizeof s_blob);
    int donor = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    if (donor && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();

    int slots[RE15_ACTOR_MAX], n = 0;
    int32_t ax[RE15_ACTOR_MAX], az[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; ax[n] = e->x; az[n] = e->z; n++;
    }
    printf("\n  %d Arme:\n", n);
    for (int i = 0; i < n; i++) {
        int32_t dz = 0;
        if (az[i] > RECT_Z + RECT_D) dz = az[i] - (RECT_Z + RECT_D);
        else if (az[i] < RECT_Z)     dz = RECT_Z - az[i];
        int32_t dmid = az[i] - (RECT_Z + RECT_D/2);
        printf("     Arm slot %2d @(%6ld,%6ld) yaw=%4d  z-Abstand zum Rechteck: %5ld   "
               "zur Band-Mitte: %6ld %s\n",
               slots[i], (long)ax[i], (long)az[i], (int)g_actors[slots[i]].rot_y,
               (long)dz, (long)dmid, (dz == 0) ? "<< IM z-Band" : "");
    }

    /* ⛔ KORREKTUR EINER FRUEHEREN FEHLDEUTUNG: re15_collision_on_floor (FUN_8003b7f0) liefert 1,
     * wenn der Punkt IN einer SCA-Zelle liegt. Die SCA-Zellen von ROOM1210 sind SOLIDE
     * Hindernisse (der Push-out FUN_8003b0a4 schiebt aus ihnen heraus), also ist
     * on_floor == 1 gerade NICHT begehbar. probe_1210_griff hat diesen Test als
     * "Boden-Abtastung" gelesen und deshalb die WAENDE als begehbar ausgegeben.
     * Hier wird stattdessen mit dem Push-out gemessen: von der Flurmitte seitwaerts gehen,
     * bis re15_collision_constrain die Bewegung nicht mehr durchlaesst. */
    printf("\n  -- Wie weit laesst der Push-out den Spieler seitwaerts? (25er-Schritte) --\n");
    for (int32_t z = -4500; z >= -23000; z -= 1500) {
        int32_t wl = -19393, wr = -19393;
        for (int dir = 0; dir < 2; dir++) {
            int32_t cx = -19393, cz = z;
            for (int k = 0; k < 800; k++) {
                re15_collision_set_band(0);
                int32_t nx = cx + (dir ? 25 : -25), nz = cz;
                re15_collision_constrain(&s_rdt, cx, cz, &nx, &nz);
                if (nx == cx) break;
                cx = nx; cz = nz;
            }
            if (dir) wr = cx; else wl = cx;
        }
        printf("     z=%7ld : erreichbar x %7ld .. %7ld  (Breite %ld)%s\n",
               (long)z, (long)wl, (long)wr, (long)(wr - wl),
               (z <= RECT_Z + RECT_D && z >= RECT_Z) ? "   << z-Band des Rechtecks" : "");
    }

    printf("\n  -- Pro Arm: wie nah kommt der Spieler (Push-out) an Spawn bzw. Lunge-Ende? --\n");
    for (int i = 0; i < n; i++) {
        int32_t cx = -19393, cz = az[i];
        int dir = (ax[i] > cx) ? 1 : -1;
        for (int k = 0; k < 800; k++) {
            re15_collision_set_band(0);
            int32_t nx = cx + dir * 25, nz = cz;
            re15_collision_constrain(&s_rdt, cx, cz, &nx, &nz);
            if (nx == cx) break;
            cx = nx; cz = nz;
        }
        /* Lunge-Ende: netto 2420 in Blickrichtung (FUN_8010c714 / FUN_800245d8). */
        int32_t hs = re15_sin_q12((int)(((int)g_actors[slots[i]].rot_y + 0x400) & 0xfff));
        int32_t hc = re15_cos_q12((int)(((int)g_actors[slots[i]].rot_y + 0x400) & 0xfff));
        int32_t lx = ax[i] + (int32_t)((hs * 2420) >> 12);
        int32_t lz = az[i] + (int32_t)((hc * 2420) >> 12);
        int32_t d_spawn = ax[i] - cx; if (d_spawn < 0) d_spawn = -d_spawn;
        int32_t d_lunge = lx - cx;    if (d_lunge < 0) d_lunge = -d_lunge;
        printf("     Arm %2d @(%6ld,%6ld) Lunge-Ende (%6ld,%6ld): Spieler kommt bis x=%7ld"
               "  -> zum Spawn %5ld, zum Lunge-Ende %5ld %s\n",
               slots[i], (long)ax[i], (long)az[i], (long)lx, (long)lz, (long)cx,
               (long)d_spawn, (long)d_lunge, (d_lunge < 0x4b0) ? "<< IM GRIFF-TOR 1200" : "");
    }

    printf("\n  -- SOLIDE SCA-Zellen (Band 0) --\n");
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        printf("     [%2d] x %7ld..%7ld  z %7ld..%7ld  type %u u0=%02x u1=%02x floor=%02x\n",
               i, (long)e->x, (long)((int32_t)e->x + e->width),
               (long)e->z, (long)((int32_t)e->z + e->density),
               e->type, e->u0, e->u1, e->floor);
    }

    for (int pass = 0; pass < 2; pass++) {
        const char *name = pass ? "BAHN 2: echte Bewegung ueber re15_collision_constrain"
                                : "BAHN 1: pro Bild gesetzte Position (NICHT begehbar geprueft)";
        printf("\n  ===== %s =====\n", name);

        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt);
        scd_room_reenter(&s_rdt, 0, 0, 0);
        if (donor && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
        pl->x = -19500; pl->z = 5000;
        for (int f = 0; f < 8; f++) frame_step();
        n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x1A) continue;
            slots[n] = s; ax[n] = e->x; az[n] = e->z; n++;
        }

        int fired[RE15_ACTOR_MAX]; memset(fired, 0, sizeof fired);
        int32_t fire_pz[RE15_ACTOR_MAX], fire_px[RE15_ACTOR_MAX];
        int     fire_f[RE15_ACTOR_MAX], fire_inrect[RE15_ACTOR_MAX];
        int     level_f[RE15_ACTOR_MAX];
        for (int i = 0; i < RE15_ACTOR_MAX; i++) { fire_f[i] = -1; level_f[i] = -1; fire_inrect[i] = 0;
                                                   fire_pz[i] = 0; fire_px[i] = 0; }

        int32_t px = -19500, pz = -3500;
        int rect_entry_f = -1, rect_exit_f = -1;
        int max_at_once = 0, offfloor = 0, sub02_f = -1;
        path_n = 0;

        for (int f = 0; f < 400; f++) {
            /* Das Band folgt im Spiel dem Spieler-y; y == 0 -> Band 0. Pro Bild neu gesetzt,
             * weil re15_game_step es sonst mit-verstellt (dieselbe Falle wie in
             * test_1210_gitterhaende). */
            re15_collision_set_band(0);
            if (!re15_player_is_grabbed()) {
                int32_t nx = px, nz = pz - 75;
                if (pass) re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                px = nx; pz = nz;
            } else { px = pl->x; pz = pl->z; }
            pl->x = px; pl->z = pz; pl->hp = 100;
            /* Gegenprobe der Bahn: der Push-out darf sie NICHT verkuerzt haben (dann waere sie
             * nicht die Bahn, die im Spiel entsteht). Gezaehlt wird, wie oft der Schritt
             * geblockt wurde. */
            if (pass && pz != (int32_t)(-3500 - 75 * (f + 1))) offfloor++;
            if (pass && path_n < 512) { pl_path_x[path_n] = px; pl_path_z[path_n] = pz; path_n++; }

            frame_step();

            if (in_rect(px, pz)) { if (rect_entry_f < 0) rect_entry_f = f; rect_exit_f = f; }
            if (sub02_f < 0)
                for (int i = 0; i < n; i++)
                    if (g_actors[slots[i]].grid_id == 1) { sub02_f = f; break; }

            int at_once = 0;
            for (int i = 0; i < n; i++) {
                re15_actor_t *e = &g_actors[slots[i]];
                int out = (e->sub_state_1 != 0);
                if (out) at_once++;
                if (out && !fired[i]) {
                    fired[i] = 1; fire_f[i] = f; fire_pz[i] = pz; fire_px[i] = px;
                    fire_inrect[i] = in_rect(px, pz);
                }
                if (level_f[i] < 0 && pz <= az[i]) level_f[i] = f;
            }
            if (at_once > max_at_once) max_at_once = at_once;
            if (pz < -25000) break;
        }

        printf("     Bahn: von z=-3500 in 75er-Schritten; Endposition (%ld,%ld)%s\n",
               (long)px, (long)pz, pass ? "" : "  [gesetzt, nicht begehbar geprueft]");
        if (pass) printf("     Bilder, in denen der Push-out den Schritt verkuerzt hat: %d von %d\n",
                         offfloor, path_n);
        printf("     Rechteck betreten in Bild %d, zuletzt drin in Bild %d\n", rect_entry_f, rect_exit_f);
        printf("     Original-Kette (sub02 -> grid_id=1 auf allen Armen): erstes Bild %d\n", sub02_f);
        printf("     hoechstens %d Arme gleichzeitig ausgefahren\n", max_at_once);
        printf("     slot  Bild  Spieler-z  Arm-z     dz       im Rechteck  Vorlauf-Bilder bis auf Hoehe\n");
        for (int i = 0; i < n; i++) {
            if (fire_f[i] < 0) { printf("     %-4d  --- kein Ausfahren (Arm-z %ld)\n",
                                        slots[i], (long)az[i]); continue; }
            int32_t dz = fire_pz[i] - az[i];
            printf("     %-4d  %-4d  %-9ld  %-9ld %-8ld %-11s  %d\n",
                   slots[i], fire_f[i], (long)fire_pz[i], (long)az[i], (long)dz,
                   fire_inrect[i] ? "JA" : "nein",
                   (level_f[i] >= 0) ? (level_f[i] - fire_f[i]) : -1);
        }
    }

    /* ============ TOR-VERGLEICH auf der gemessenen Bahn (reine Arithmetik) ============
     * Kein AI-Lauf noetig: das Tor ist |pz - arm_z| < H && |px - arm_x| < 11000. Fuer jeden
     * Kandidaten H wird auf der GEMESSENEN Bahn 2 ausgewertet: hoechste Zahl gleichzeitig
     * offener Tore, Vorlauf in Bildern bis der Spieler auf Arm-Hoehe ist, und wie viele
     * Arme ueberhaupt reagieren. */
    {   struct { int32_t h; const char *src; } cand[] = {
            {  603, "halber kleinster Arm-Abstand (1207, Spawn-Tabelle main00)" },
            {  850, "halbe TIEFE des Original-Rechtecks (d=1700 @0x1EAE, Ecke+Ausdehnung)" },
            { 1200, "RE2-Griff-Tor 0x4b0 @0x801018f4" },
            { 1700, "PORT-STAND v0.3.28 (volle Tiefe als HALB-Breite = 3400 gesamt)" },
        };
        printf("\n  ===== TOR-VERGLEICH auf der gemessenen Bahn (%d Bilder) =====\n", path_n);
        for (unsigned c = 0; c < sizeof cand / sizeof cand[0]; c++) {
            int32_t H = cand[c].h;
            int maxsim = 0, opened = 0, lead_min = 9999, lead_max = -9999;
            for (int i = 0; i < n; i++) {
                int first = -1, level = -1;
                for (int f = 0; f < path_n; f++) {
                    int32_t dz = pl_path_z[f] - az[i]; if (dz < 0) dz = -dz;
                    int32_t dx = pl_path_x[f] - ax[i]; if (dx < 0) dx = -dx;
                    if (first < 0 && dz < H && dx < 11000) first = f;
                    if (level < 0 && pl_path_z[f] <= az[i]) level = f;
                }
                if (first >= 0) {
                    opened++;
                    if (level >= 0) {
                        int lead = level - first;
                        if (lead < lead_min) lead_min = lead;
                        if (lead > lead_max) lead_max = lead;
                    }
                }
            }
            for (int f = 0; f < path_n; f++) {
                int sim = 0;
                for (int i = 0; i < n; i++) {
                    int32_t dz = pl_path_z[f] - az[i]; if (dz < 0) dz = -dz;
                    int32_t dx = pl_path_x[f] - ax[i]; if (dx < 0) dx = -dx;
                    if (dz < H && dx < 11000) sim++;
                }
                if (sim > maxsim) maxsim = sim;
            }
            printf("     H=%4ld : %d/%d Arme reagieren, hoechstens %d gleichzeitig, "
                   "Vorlauf %d..%d Bilder\n               Herkunft: %s\n",
                   (long)H, opened, n, maxsim, lead_min, lead_max, cand[c].src);
        }
    }

    {   printf("\n  -- Arm-Abstaende in z je Spalte (Spawn-Tabelle main00) --\n");
        for (int col = 0; col < 2; col++) {
            int32_t colx = col ? -14000 : -25000;
            int32_t zs[16]; int m = 0;
            for (int i = 0; i < n; i++) if (ax[i] == colx) zs[m++] = az[i];
            for (int a = 0; a < m; a++) for (int b = a + 1; b < m; b++)
                if (zs[b] > zs[a]) { int32_t t = zs[a]; zs[a] = zs[b]; zs[b] = t; }
            printf("     x=%ld :", (long)colx);
            for (int a = 0; a < m; a++) printf(" %ld", (long)zs[a]);
            printf("   Abstaende:");
            for (int a = 1; a < m; a++) printf(" %ld", (long)(zs[a-1] - zs[a]));
            printf("\n");
        }
    }

    free(buf);
    printf("\nprobe_1210_trigger: MESSUNG FERTIG\n");
    return 0;
}
