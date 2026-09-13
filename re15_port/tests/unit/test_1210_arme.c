/* test_1210_arme.c — die Gitterhaende in ROOM1210: Ausloeser, Staffelung, Einmal-Riegel.
 *
 * BEFUND (Nutzer 2026-09-13): "Fuer ROOM 1210 habe ich mir jetzt die Arme im Korridor von
 * Resident Evil 2 noch einmal angeschaut wie die funktionieren. Diese greifen dann einmalig
 * durch die Wand, greifen Leon und gehen dann zurueck danach kommen sie waehrend man noch im
 * Raum ist nicht mehr wieder. ... Aber so muss das hier auch sein. Ausserdem sieht das etwas
 * komisch aus aktuell noch."
 *
 * GEMESSEN (probe_1210_trigger, begehbare Bahn, 287 Bilder): mit dem alten Tor — einem
 * 1300er-Radius um die AUSGEFAHRENE Hand — reagierten 0 von 10 Armen. Der Laufraum ist
 * x -20622..-18164, die Arm-Reihen stehen bei x -25000/-14000, die Haende landen bei
 * -20909/-18091; in der Flurmitte sind das 1409 Einheiten, 109 zu viel. Deshalb sitzt der
 * Radius jetzt am Arm-URSPRUNG (RE2s Form) und zaehlt den Koerperradius des Spielers mit.
 *
 * DREI DINGE SIND JETZT ZU, jedes mit eigener Herkunft:
 *  (1) SCHARF = byte-true. A[0] @0x8010c614 liest `lbu v0,9(a0)` / `andi v0,v0,0x1f` / `==1`
 *      -> `sb v0,5(a0)`. Gesetzt wird +0x9 von ROOM1210 sub02 @0x1EC8 (Member_set(12,1),
 *      Member 12 -> `sb a2,9(a0)` @0x800411f8), erreichbar nur ueber den Aot_set aot=6 sce=3
 *      @0x1EAE und nur solange Flag(3,44)==0 (`21 03 2c 00` in sub00 @0x1EA6). Der Port las
 *      dieses Byte NIE.
 *  (2) STAFFELUNG = ein Radius um den ARM-URSPRUNG, RE2s Form (`lw s0,496(s1)` /
 *      `sltiu s0,s0,0x514` @0x80102f3c), mit der Reichweite DIESES Armes:
 *      4091 (LUNGE_NET 2420 + MESH_REACH 1671) + 450 Spieler-Klemmradius @0x80073e9a = 4541.
 *      Der Koerperradius gehoert dazu, weil der begehbare Flur 4164 von der naechsten
 *      Armreihe entfernt ist - ohne ihn erreicht kein Arm je jemanden. Nachruestung, weil
 *      der Nutzer das "alle zehn in EINEM Bild" des Originals verworfen hat.
 *  (3) EINMAL = im Original kehrt +0x5 nie auf 0/1 zurueck (Voll-Scan aller Stores auf +0x5 im
 *      Arm-Baum 0x8010c1ec..0x8010d774: nur 1/2/(rng&1)+2/3/2/2/2/4). Der Port schickte den
 *      Arm nach dem Rueckzug auf sub 0, wo das Tor sofort wieder zog.
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

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
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

static int load_bank(uint8_t type)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob) {
        memcpy(s_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL; ok = 1;
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

/* Raum aufsetzen; gibt die Zahl der Arme zurueck und legt ihre Slots/z ab. */
static int aufsetzen(int *slots, int32_t *az)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { fprintf(stderr, "FAIL: ROOM1210.RDT fehlt\n"); return -1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { fprintf(stderr, "FAIL: RDT-Parse\n"); return -1; }
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

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
    load_bank(0x1A);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();

    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != 0x1A) continue;
        slots[n] = s; az[n] = g_actors[s].z; n++;
    }
    return n;
}

int main(void)
{
    printf("=== ROOM1210 Gitterhaende: Ausloeser, Staffelung, Einmal-Riegel ===\n");
    int slots[RE15_ACTOR_MAX]; int32_t az[RE15_ACTOR_MAX];
    int n = aufsetzen(slots, az);
    if (n <= 0) { fprintf(stderr, "FAIL: keine Arme geladen\n"); return 1; }
    printf("  %d Arme geladen\n", n);

    re15_actor_t *pl  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *arm = &g_actors[slots[0]];

    /* (1) NICHT SCHARF: der Spieler steht dem Arm genau gegenueber (dz = 0), aber das
     *     Skript hat ihn nie scharfgeschaltet -> er bleibt drin (byte-true A[0]). */
    /* An die WAND der Armreihe, nicht in die Flurmitte: das Tor ist ein Radius um den
     * Arm-Ursprung (4091 Handreichweite + 450 Spieler-Klemmradius @0x80073e9a). Aus der
     * Flurmitte (-19500) sind es 5500 Einheiten - dort erreicht der Arm niemanden, und
     * der Test wuerde nur messen, dass ein zu weit entfernter Arm stillhaelt. */
    arm->grid_id = 0;
    pl->x = arm->x + (arm->x < -19500 ? 4100 : -4100);
    pl->z = arm->z;
    for (int f = 0; f < 60; f++) frame_step();
    CHECK("ohne grid_id faehrt der Arm NICHT aus (A[0] @0x8010c614)", arm->sub_state_1 == 0);

    /* (2) SCHARF + in Reichweite -> er faehrt aus. */
    arm->grid_id = 1;                      /* wie Member_set(12,1) @0x1EDA */
    int ausgefahren = 0;
    for (int f = 0; f < 60 && !ausgefahren; f++) {
        frame_step(); ausgefahren = (arm->sub_state_1 != 0);
    }
    CHECK("mit grid_id=1 und in Reichweite faehrt er aus", ausgefahren);

    /* (3) AUSSER REICHWEITE: ein Arm, der weiter weg steht, bleibt drin — das ist die
     *     Staffelung (Radius 4541 um den Arm-Ursprung). */
    {
        int fern = -1;
        for (int i = 1; i < n; i++) {
            int32_t d = az[i] - pl->z; if (d < 0) d = -d;
            if (d > 2000) { fern = slots[i]; break; }
        }
        if (fern >= 0) {
            g_actors[fern].grid_id = 1;
            for (int f = 0; f < 40; f++) frame_step();
            CHECK("ein Arm ausser Reichweite bleibt drin (Radius 4541 @0x80102f3c-Form)",
                  g_actors[fern].sub_state_1 == 0);
        } else {
            CHECK("ein Arm ausser Reichweite bleibt drin (Radius 4541 @0x80102f3c-Form)", 0);
        }
    }

    /* (4) EINMAL: der Spieler geht weiter (genau der Fall des Nutzers — man laeuft vorbei),
     *     der Arm zieht sich zurueck. Danach darf er im selben Raumbesuch nicht wieder
     *     ausfahren, auch wenn beide Tore erneut offen stehen.
     *     Nicht einfach stehen bleiben und warten: dann packt der Arm zu (Griff-Tor
     *     RE15_WRITHER_GRAB_DIST @0x801018f4) und der Ringkampf laeuft, statt des Rueckzugs. */
    {
        pl->z = arm->z + 4000;                            /* ausser z-Hoehe -> !reach */
        int wieder_ruhe = 0;
        for (int f = 0; f < 900 && !wieder_ruhe; f++) {
            frame_step();
            if (arm->sub_state_1 == 0) wieder_ruhe = 1;   /* Rueckzug fertig */
        }
        CHECK("der Arm zieht sich zurueck, wenn der Spieler weitergeht", wieder_ruhe);
        if (wieder_ruhe) {
            arm->grid_id = 1; pl->z = arm->z;             /* beide Tore wieder offen */
            int nochmal = 0;
            for (int f = 0; f < 200 && !nochmal; f++) {
                frame_step(); nochmal = (arm->sub_state_1 == 1);
            }
            CHECK("danach kommt er im selben Raumbesuch NICHT wieder", !nochmal);
        }
    }

    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
