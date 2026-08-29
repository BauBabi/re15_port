/* probe_1210_wandgriff.c — MESSUNG zum Nutzer-Report 2026-08-31:
 *   "Die Zombies in 1210 ziehen Leon immer noch durch die Wand wenn sie ihn grabben.
 *    Ausserdem clippen sie teilweise vollstaendig durch die Wand."
 *
 * Diese Datei BEHAUPTET nichts und PRUEFT nichts. Sie misst:
 *   (1) wo der Arm steht, wenn er zupackt,
 *   (2) wo Leon vorher stand und wohin er im Griff gesetzt wird,
 *   (3) ob dieser Punkt ueberhaupt begehbar ist (Constrain-Test statt on_floor —
 *       re15_collision_on_floor ist in ROOM1210 INVERTIERT, es meldet die WAND),
 *   (4) wie weit der Arm ueber die begehbare Kante hinaus in den Flur faehrt,
 *   (5) welche Kamera-Vierecke Arm und Spieler zur Griffzeit treffen.
 *
 * Erst danach darf eine Zeile Fix-Code entstehen.
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_math.h"
#include "re15_damage.h"
#include "re15_player.h"
#include "re2_ems.h"   /* RE2-Opferbank als Leihgeber */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

static uint8_t s_blob[0x80000];
static uint8_t s_blob_donor[0x80000];

/* Eine Bank aus CDEMD0.EMS in die Registry legen — wie pc_enemy_load, ohne Renderer.
 * OHNE die Leih-Opferbank rastet der Griff gar nicht ein (gemessen: der Arm erreicht
 * sub 4 und faellt im naechsten Bild auf sub 3 zurueck, re15_player_is_grabbed() bleibt 0). */
static int load_bank(unsigned char type, unsigned char *blob, size_t blobcap)
{
    size_t n = 0;
    unsigned char *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim;
            memset(&tim, 0, sizeof tim);
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

static unsigned char *s_re2_ems;      /* muss resident bleiben: die Bank aliast hinein */

/* Den Leihgeber 0x10 aus der RE2-Bank laden statt aus der RE1.5-Bank.
 * ⛔ DAS IST DER UNTERSCHIED, den der Nutzer meldet: der Port leiht dem Arm (Typ 0x1A)
 * immer die Opfer-Bank des Zombies 0x10 (main.c:5021) — aber WELCHE Fassung, entscheidet
 * der KI-Geschmack. Unter "RE2 AI" kommt RE2s EM010 mit RE2s Ringkampf-Clips, und deren
 * Wurzel-Versaetze sind andere als die der RE1.5-Clips. */
static int load_donor_re2(void)
{
    size_t n = 0;
    s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &n);
    if (!s_re2_ems) { printf("  RE2-EMS fehlt\n"); return 0; }
    re15_enemy_bank_t *eb = re15_enemy_find(0x10);
    if (!eb) eb = re15_enemy_alloc(0x10);
    if (!eb) return 0;
    re15_tim_t tim;
    memset(&tim, 0, sizeof tim);
    if (re2_ems_load_bank(s_re2_ems, n, 0x10, eb, &tim) != 0) {
        printf("  RE2-Bank 0x10 nicht ladbar\n");
        return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    printf("  RE2-Leihgeber 0x10: victim_ok=%d, %d Opfer-Clips\n",
           eb->victim_ok, eb->victim_ok ? eb->anim_victim.clip_count : -1);
    return eb->victim_ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Wie frame_step, aber mit Pad-Bits — fuer den LAUFENDEN Spieler (Modus C). */
static void frame_pad(uint16_t held)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = held; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Begehbar = der Constrain-Lauf laesst einen Mini-Schritt von hier aus zu, ohne den
 * Punkt zu verschieben. Das ist derselbe Massstab, den die Spielfigur nimmt. */
static int begehbar(int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}

/* Wie weit muss man von (x,z) nach dx laufen, bis der Constrain sperrt? */
static int32_t kante(int32_t x, int32_t z, int32_t dx)
{
    int32_t px = x;
    for (int k = 0; k < 400; k++) {
        int32_t nx = px + dx, nz = z;
        re15_collision_constrain(&s_rdt, px, z, &nx, &nz);
        if (nx == px) break;
        px = nx;
    }
    return px;
}

static int g_frames, g_wand;
static int32_t g_tiefste = -99999;

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1210: wohin zieht der Griff den Spieler? ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    /* ⛔ DIE ENGINE-KLEMME ARBEITET AUF g_room_rdt, NICHT auf s_rdt. Ohne diese zwei
     * Zeilen ist g_room_rdt_ok == 0, die Klemme in re15_victim_place laeuft gar nicht,
     * und JEDE Messung an ihr ist ein Harness-Artefakt statt einer Aussage ueber das
     * Spiel. Genau daran sind hier drei Diagnosen gescheitert. */
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    {
        int arm = load_bank(0x1A, s_blob, sizeof s_blob);
        const char *m = getenv("RE15_PROBE_AI");
        int re2 = (m && *m == '2');
        int don;
        if (re2) {
            don = load_donor_re2();
            printf("  MODUS: RE2-AI (Leihgeber aus shared_assets/RE2/CDEMD0.EMS)\n");
        } else {
            don = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
            printf("  MODUS: RE1.5 (Leihgeber aus shared_assets/PSX/EMD/CDEMD0.EMS)\n");
        }
        re15_enemy_bank_t *db = re15_enemy_find(0x10);
        printf("  Baenke: Arm 0x1A=%d, Leihgeber 0x10=%d victim_ok=%d\n",
               arm, don, db ? db->victim_ok : -1);
        if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    }
    for (int f = 0; f < 8; f++) frame_step();

    /* Arme einsammeln */
    int slots[32], n = 0;
    int32_t hx[32], hz[32];
    for (int s = 1; s < RE15_ACTOR_MAX && n < 32; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1a) continue;
        slots[n] = s; hx[n] = e->x; hz[n] = e->z; n++;
    }
    printf("  %d Arme\n", n);
    if (n == 0) { free(buf); return 1; }

    /* (A) Fuer JEDEN Arm: Heimat, begehbare Kante auf seiner z-Zeile, Landepunkt. */
    printf("\n--- (A) Geometrie je Arm (Heimat, Flurkante auf seiner z-Zeile, Landepunkt) ---\n");
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        int ost = (e->rot_y & 0x0fff) != 0;          /* Ost-Reihe schaut nach -x */
        int32_t ostk = kante(-19500, hz[i], +25);    /* wie weit nach Osten laesst die Kollision? */
        int32_t wstk = kante(-19500, hz[i], -25);    /* und nach Westen? */
        int32_t land = ost ? (hx[i] - 2420) : (hx[i] + 2420);
        int32_t hand = ost ? (hx[i] - 4091) : (hx[i] + 4091);
        printf("  Arm %d slot %2d Heimat (%7ld,%7ld) yaw %4u | Flur x %7ld .. %7ld"
               " | Landepunkt %7ld (%s Kante um %ld) | Handpunkt %7ld begehbar=%d\n",
               i, slots[i], (long)hx[i], (long)hz[i], (unsigned)e->rot_y,
               (long)wstk, (long)ostk, (long)land,
               ost ? "ueber der Ost-" : "ueber der West-",
               (long)(ost ? (ostk - land) : (land - wstk)),
               (long)hand, begehbar(hand, hz[i]));
    }

    /* (B) Den Griff wirklich ausloesen: Spieler an die Kollisionsgrenze auf der Zeile
     *     des am besten erreichbaren Arms, dann laufen lassen und JEDES Bild messen. */
    /* RE15_WANDGRIFF_SIDE=west|ost: nur Arme der gewuenschten Seite kandidieren
     * (Nutzer-Report 2026-08-29 "linke Seite zieht immer noch in die Wand" —
     * die v0.3.35-Messung lief nur auf der Ost-Zeile). */
    const char *side_env = getenv("RE15_WANDGRIFF_SIDE");
    int want_ost = side_env ? (side_env[0] == 'o' || side_env[0] == 'O') : -1;
    int best = -1; int32_t bestgap = 0x7fffffff, bx = 0, bz = 0;
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        int ost = (e->rot_y & 0x0fff) != 0;
        if (want_ost >= 0 && ost != want_ost) continue;
        int32_t k = kante(-19500, hz[i], ost ? +25 : -25);
        int32_t hand = ost ? (hx[i] - 4091) : (hx[i] + 4091);
        int32_t gap = hand - k; if (gap < 0) gap = -gap;
        if (gap < bestgap) { bestgap = gap; best = i; bx = k; bz = hz[i]; }
    }
    if (best < 0) { printf("FAIL: kein Arm auf der gewuenschten Seite\n"); return 1; }
    printf("\n--- (B) Griff am erreichbarsten Arm: %d, Spieler auf (%ld,%ld) ---\n",
           best, (long)bx, (long)bz);
    printf("  Standpunkt begehbar = %d\n", begehbar(bx, bz));

    re15_actor_t *ga = &g_actors[slots[best]];
    int gemeldet = 0;
    int32_t worst_out = 0;
    for (int f = 0; f < 400; f++) {
        if (!re15_player_is_grabbed()) { pl->x = bx; pl->z = bz; pl->hp = 100; }
        frame_step();
        /* Wie weit ist der Arm ueber seine Flurkante hinaus? */
        {
            int ost = (ga->rot_y & 0x0fff) != 0;
            int32_t k = kante(-19500, bz, ost ? +25 : -25);
            int32_t over = ost ? (k - ga->x) : (ga->x - k);
            if (over > worst_out) worst_out = over;
        }
        if (re15_player_is_grabbed()) {
            g_frames++;
            if (!begehbar(pl->x, pl->z)) {
                g_wand++;
                if (g_wand <= 3)
                    printf("    WAND-BILD f%-3d Spieler (%7ld,%7ld)  Kante -18164\n",
                           f, (long)pl->x, (long)pl->z);
                if (pl->x > g_tiefste) g_tiefste = pl->x;   /* je groesser, desto tiefer in der Ostwand */
            }
        }
        if (0)
            printf("    f%-3d Arm sub=%d/%d clip=%u x=%7ld | Spieler (%7ld,%7ld) grabbed=%d\n",
                   f, ga->sub_state_1, ga->sub_state_2, (unsigned)ga->motion,
                   (long)ga->x, (long)pl->x, (long)pl->z, re15_player_is_grabbed());
        if (re15_player_is_grabbed() && gemeldet < 6) {
            printf("  Bild %3d GRIFF: Spieler (%7ld,%7ld) begehbar=%d | Arm (%7ld,%7ld)"
                   " | Anker Spieler (%7ld,%7ld) Arm (%7ld,%7ld)\n",
                   f, (long)pl->x, (long)pl->z, begehbar(pl->x, pl->z),
                   (long)ga->x, (long)ga->z,
                   (long)pl->anchor_x, (long)pl->anchor_z,
                   (long)ga->anchor_x, (long)ga->anchor_z);
            gemeldet++;
        }
    }
    printf("  groesster Ueberstand eines Arms ueber die Flurkante: %ld Einheiten\n",
           (long)worst_out);

    printf("\n\n=== ERGEBNIS: %d Griff-Bilder, davon %d IN DER WAND ===\n",
           g_frames, g_wand);
    if (g_wand) printf("    tiefster Punkt in der Ostwand: x = %ld (Kante -18164)\n", (long)g_tiefste);

    /* --- (C) LAUFENDER Spieler (Nutzer-Report 2026-08-29 "linke Seite zieht immer noch in
     *     die Wand"): statt Teleport-Stand laeuft der Spieler die Spur der gewaehlten Seite
     *     mit echten Pad-Bits ab (Tank-Steuerung wie probe_1090s drive_to). Griffe passieren
     *     MITTEN im Lauf — genau die Lage, die (B) nicht abdeckt (der Klemm-Anker ist dort
     *     der Standpunkt im Moment des Zupackens; kommt der schon grenzwertig, klemmt nichts
     *     mehr). Gemessen wird jedes Griff-Bild gegen begehbar(). --- */
    {
        /* Frischer Zustand: (B) hat Riegel/Arm-Latches verbraucht (Ein-Angreifer-Riegel,
         * Trefferbit, Arm-Substates). Banks bleiben in der Registry. */
        extern void re15_player_victim_reset(void);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_player_victim_reset();
        re15_player_cmd_reset();
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt);
        scd_room_reenter(&s_rdt, 0, 0, 0);
        {   re15_enemy_bank_t *db = re15_enemy_find(0x10);
            if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
        }
        for (int f = 0; f < 8; f++) frame_step();

        int wframes = 0, wwand = 0, wgrabs = 0, prev_grab = 0;
        int32_t wtief_w = 0, wtief_o = 0;
        /* Spur: x der Seitenmitte, z pendelt ueber die Arm-Reihen. */
        int32_t lane_x = (want_ost == 1) ? -18214 : -20572;   /* an der Kante entlang (Reichweiten-Tor oeffnet nur dort) */
        int32_t tzs[2] = { -21000, -6300 };
        int ti = 0, stuck = 0;
        re15_actor_t *plc = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        plc->x = lane_x; plc->z = tzs[1]; plc->hp = 100;
        for (int f = 0; f < 2600; f++) {
            plc->hp = 100;
            uint16_t bits = 0;
            if (!re15_player_is_grabbed()) {
                int32_t dx = lane_x - plc->x, dz = tzs[ti] - plc->z;
                if ((long long)dx*dx + (long long)dz*dz < 640000LL) { ti ^= 1; continue; }
                double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
                int want = ((int)(ang + 0.5)) & 0x0FFF;
                plc->rot_y = (int16_t)want;      /* Richtung direkt — nur der SCHRITT laeuft
                                                  * durch die echte Lokomotion+Kollision */
                bits = RE15_PAD_BIT_UP;
                int32_t bx0 = plc->x, bz0 = plc->z;
                frame_pad(bits);
                if (plc->x == bx0 && plc->z == bz0) { if (++stuck > 60) { ti ^= 1; stuck = 0; } }
                else stuck = 0;
            } else {
                frame_pad(0);
            }
            if (re15_player_is_grabbed()) {
                wframes++;
                if (!prev_grab) wgrabs++;
                if (!begehbar(plc->x, plc->z)) {
                    wwand++;
                    if (wwand <= 6)
                        printf("  (C) WAND-BILD f%-4d Spieler (%7ld,%7ld)\n",
                               f, (long)plc->x, (long)plc->z);
                    if (plc->x < -20622 && plc->x < wtief_w) wtief_w = plc->x;
                    if (plc->x > -18164 && plc->x > wtief_o) wtief_o = plc->x;
                }
            }
            prev_grab = re15_player_is_grabbed();
            if ((f % 400) == 0) {
                printf("  (C) f%-4d PL(%ld,%ld) grid/sub:", f, (long)plc->x, (long)plc->z);
                for (int s2 = 1; s2 < RE15_ACTOR_MAX; s2++)
                    if (g_actors[s2].active && g_actors[s2].type == 0x1a)
                        printf(" %d/%d", (int)g_actors[s2].grid_id, (int)g_actors[s2].sub_state_1);
                printf("\n");
            }
        }
        printf("\n=== (C) LAUF-ERGEBNIS: %d Griffe, %d Griff-Bilder, davon %d IN DER WAND ===\n",
               wgrabs, wframes, wwand);
        if (wtief_w) printf("    tiefster Punkt WESTWAND: x = %ld (Kante -20622)\n", (long)wtief_w);
        if (wtief_o) printf("    tiefster Punkt OSTWAND:  x = %ld (Kante -18164)\n", (long)wtief_o);
    }

    free(buf);
    printf("\nMESSUNG FERTIG\n");
    return 0;
}
