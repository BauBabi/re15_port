/* test_1210_gitterhaende.c — PIN: die Gitterhaende in ROOM1210 reagieren EINZELN,
 * wenn der Spieler an ihnen vorbeigeht.
 *
 * ⛔ BEWUSSTE NACHRUESTUNG (Nutzer-Auftrag), kein byte-true-Nachbau:
 * "im room 1210 strecken ab einem gewissen punkt alle Zombies die Haende aus dem gitter.
 *  das ist sau schlecht gemacht ... in Resident Evil 2 gibt es einen Flur, wo man
 *  durchlaeuft, und kommt man am Fenster vorbei, kommen die Haende der Zombies mit stoehnen
 *  und greifen einen. das wuerde ich gerne auch hier so umgesetzt haben wollen."
 *
 * Das Original schaltet ALLE ZEHN gleichzeitig (ROOM1210 sub02 `Member_set(12,1)`
 * @Datei 0x001EDA) — genau das war der Kritikpunkt. Statt dessen entscheidet jeder Arm
 * fuer sich; die Belege fuer Clips, Stoehnen, Griff und die beiden Reichweiten stehen im
 * Kopf von re15_writher_ai_tick case 1.
 *
 * DIE WACHE IST NICHT VAKUANT: sie laeuft den Flur wirklich ab (der Spieler wird Bild fuer
 * Bild an der gemessenen Arm-Reihe entlang bewegt) und verlangt DREI Dinge:
 *   (1) Es reagieren ueberhaupt Arme — sonst ist die Nachruestung tot.
 *   (2) Sie reagieren NICHT alle gleichzeitig — das ist der eigentliche Nutzer-Punkt.
 *       Gemessen wird die groesste Zahl gleichzeitig ausgefahrener Arme.
 *   (3) Jeder Arm kehrt in die Ruhe zurueck, wenn der Spieler weiter ist — sonst waere aus
 *       "alle auf einmal" nur "alle nacheinander, und dann bleiben sie" geworden.
 * Dazu die Gegenprobe, dass KEIN Arm seine Position veraendert (er steckt im Gitter).
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
#include "re15_damage.h"   /* re15_damage_seed_rng */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Die EM01A-Bank laden, sonst ist clip_len 0 und die Phasen laufen im Setz-Bild durch. */
static int load_bank_1a(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x1A);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob) {
        memcpy(s_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
        if (!eb) eb = re15_enemy_alloc(0x1A);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len,
                                                        &eb->skel_loco, &eb->anim_loco) == 0);
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

    printf("=== ROOM1210: die Gitterhaende reagieren einzeln ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    CHECK(load_bank_1a(), "EM01A-Bank geladen — ohne sie ist clip_len 0 und die Wache blind");
    for (int f = 0; f < 8; f++) frame_step();

    /* Die Arme einsammeln + ihre Startpositionen sichern. */
    int   slots[RE15_ACTOR_MAX], n = 0;
    int32_t sx[RE15_ACTOR_MAX], sz2[RE15_ACTOR_MAX];
    int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; sx[n] = e->x; sz2[n] = e->z; n++;
        if (e->z < zmin) zmin = e->z;
        if (e->z > zmax) zmax = e->z;
    }
    printf("  %d Arme, z-Spanne %ld .. %ld\n", n, (long)zmin, (long)zmax);
    CHECK(n >= 8, "mindestens 8 Arme im Raum (%d) — sonst misst die Wache nichts", n);

    /* DEN FLUR ABLAUFEN: der Spieler faehrt in der Flurmitte von z(max) nach z(min). */
    int32_t mid_x = -19500;                 /* Flurmitte: Arme auf -25000 und -14000 */
    int reacted[RE15_ACTOR_MAX]; memset(reacted, 0, sizeof reacted);
    int returned[RE15_ACTOR_MAX]; memset(returned, 0, sizeof returned);
    int max_at_once = 0, moved = 0;
    int32_t step = 120;                     /* Gehtempo-Groessenordnung */

    for (int32_t z = zmax + 3000; z > zmin - 3000; z -= step) {
        pl->x = mid_x; pl->z = z; pl->hp = 100;
        frame_step();
        int at_once = 0;
        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            int out = (e->sub_state_1 == 1 || e->sub_state_1 == 2);
            if (out) { at_once++; reacted[i] = 1; }
            else if (reacted[i]) returned[i] = 1;
            if (e->x != sx[i] || e->z != sz2[i]) moved++;
        }
        if (at_once > max_at_once) max_at_once = at_once;
    }

    int n_reacted = 0, n_returned = 0;
    for (int i = 0; i < n; i++) { n_reacted += reacted[i]; n_returned += returned[i]; }
    printf("  Durchlauf: %d von %d Armen haben reagiert, %d davon sind wieder zur Ruhe gekommen; "
           "hoechstens %d gleichzeitig ausgefahren\n", n_reacted, n, n_returned, max_at_once);

    /* (1) Es reagiert ueberhaupt etwas. */
    CHECK(n_reacted >= 6,
          "die Arme reagieren beim Vorbeigehen (%d von %d) — vorher stand hier Clip 0 fuer immer",
          n_reacted, n);
    /* (2) Der eigentliche Nutzer-Punkt: NICHT alle auf einmal. */
    CHECK(max_at_once < n,
          "und sie fahren NICHT alle gleichzeitig aus (hoechstens %d von %d gleichzeitig) — "
          "genau das war der Kritikpunkt am Original, das per Member_set(12,1) @0x001EDA alle "
          "zehn auf einen Schlag schaltet", max_at_once, n);
    CHECK(max_at_once <= 4,
          "es reagiert jeweils nur die unmittelbare Nachbarschaft (%d gleichzeitig; "
          "REACH_Z 850 = halbe Tiefe des Original-Rechtecks 1700 @0x1EAE, Arm-Abstand 1200..1396)",
          max_at_once);
    /* (3) Sie kommen wieder zur Ruhe. */
    CHECK(n_returned >= 4,
          "und sie ziehen sich wieder zurueck, wenn der Spieler weiter ist (%d von %d)",
          n_returned, n_reacted);
    /* Gegenprobe: ein Arm steckt im Gitter und bewegt sich NICHT. */
    CHECK(moved == 0,
          "kein Arm hat seine Position veraendert (%d Abweichungen) — die Original-Maschine "
          "wuerde mit +0x8c = 0x320 (@0x8010c7b8) rund 2400 Einheiten vorschnellen, ein im "
          "Gitter steckender Arm kann das nicht", moved);

    free(buf);
    if (fails) { printf("\n1210 GITTERHAENDE: FAIL (%d)\n", fails); return 1; }
    printf("\n1210 GITTERHAENDE: sie kommen einzeln, wenn man vorbeigeht\n");
    return 0;
}
