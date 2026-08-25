/* test_npc_wall_clamp.c — PIN: eine folgende NPC laeuft NICHT durch Waende.
 *
 * NUTZER-REPORT 2026-08-29: "Ada Lauf geht, aber sie kann durch alles durchlaufen — das
 * macht sie im original nicht."
 *
 * BYTE-TRUE BELEG: die NPC-Wurzel 0x8011cb70 klemmt JEDEN NPC nach dem Zustands-Dispatch
 * gegen die Raum-Kollision. Unmittelbar hinter dem `jalr` auf die Zustandstabelle
 * (@0x8011cc1c) steht:
 *     8011cc58: lw    v0,120(a0)   ; v0 = entity+0x78 = die Hitbox
 *     8011cc5c: ori   a2,zero,0x4  ; a2 = Solid-Maske 4
 *     8011cc60: lhu   a1,6(v0)     ; a1 = box+6 = Radius
 *     8011cc64: jal   0x8003b0a4   ; der Wand-Klemmer
 *     8011cc68: addiu a0,a0,52     ; a0 = entity+0x34 = Position
 * Der Port hatte diesen Aufruf fuer die NPC-Familie nirgends — die in v0.3.28 neu portierte
 * Eskorte schob die Figur deshalb frei durch Waende (re15_npc_pos_advance schreibt x/z
 * direkt). Jetzt laeuft am Ende jedes Eskorte-Ticks re15_npc_wall_clamp.
 *
 * DIE WACHE IST NICHT VAKUANT:
 *   (1) Sie prueft zuerst, dass die NPC sich ueberhaupt BEWEGT — sonst wuerde ein
 *       eingefrorener Aktor die Wand-Zusage trivial erfuellen.
 *   (2) Das Ziel liegt NACHWEISLICH hinter einer Wand: der Testpunkt wird vorher mit
 *       re15_collision_on_floor als NICHT begehbar bestaetigt.
 *   (3) Und die GEGENPROBE: mit einem ERREICHBAREN Ziel kommt sie auch an - sonst waere
 *       (2) auch dann gruen, wenn die Klemme sie einfach eingemauert haette.
 *
 * NICHT geprueft, mit Absicht: "steht immer auf begehbarem Boden". Das waere eine staerkere
 * Zusage als das Original gibt - FUN_8003b0a4 klemmt gegen SCA-WANDZELLEN (Maske 4), es ist
 * kein Boden-Test. Gemessen steht die NPC zwischendurch auf z-Zeilen ohne Boden-Zelle, ohne
 * durch eine Wand zu gehen.
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
#include "re15_damage.h"   /* re15_enemy_apply_hitbox */

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

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== NPC-Eskorte: die Wand haelt (Wurzel-Klemme @0x8011cc58-68) ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1090;
    re15_collision_set_band(0);

    /* Einen begehbaren Startpunkt und einen NICHT begehbaren Zielpunkt SUCHEN, statt sie
     * zu behaupten. Gesucht wird ein Paar auf derselben z-Zeile, 4000 Einheiten auseinander. */
    int32_t sx = 0, sz0 = 0, tx = 0; int found = 0;
    int32_t fminx = 0x7fffffff, fmaxx = -0x7fffffff, fminz = 0x7fffffff, fmaxz = -0x7fffffff;
    int floor_cells = 0;
    for (int32_t z = -40000; z <= 40000; z += 250)
        for (int32_t x = -40000; x <= 40000; x += 250) {
            if (!re15_collision_on_floor(&s_rdt, x, z)) continue;
            floor_cells++;
            if (x < fminx) fminx = x;  if (x > fmaxx) fmaxx = x;
            if (z < fminz) fminz = z;  if (z > fmaxz) fmaxz = z;
            if (!found && !re15_collision_on_floor(&s_rdt, x + 4000, z)) {
                sx = x; sz0 = z; tx = x + 4000; found = 1;   /* dahinter: Wand */
            }
        }
    printf("  begehbarer Boden: %d Zellen, x %ld..%ld, z %ld..%ld\n",
           floor_cells, (long)fminx, (long)fmaxx, (long)fminz, (long)fmaxz);
    printf("  Start (%ld,%ld) begehbar, Ziel (%ld,%ld) NICHT begehbar\n",
           (long)sx, (long)sz0, (long)tx, (long)sz0);
    CHECK(found, "ein Paar begehbar/nicht-begehbar gefunden — sonst misst die Wache nichts");
    if (!found) { free(buf); return 1; }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = tx; pl->y = 0; pl->z = sz0;          /* der Spieler steht hinter der Wand */

    re15_actor_t *n = &g_actors[1];
    n->active = 1; n->type = 0x42; n->state = 0; n->x = sx; n->y = 0; n->z = sz0;
    re15_enemy_apply_hitbox(n, 0x42);            /* +0x78-Box -> Radius fuer die Klemme */
    printf("  NPC Typ 0x42, Klemm-Radius %u\n", n->hit_radius_min);
    CHECK(n->hit_radius_min > 0,
          "die NPC hat einen Klemm-Radius (+0x78, `lhu a1,6(v0)` @0x8011cc60) — mit 0 waere "
          "die Klemme wirkungslos und die Wache blind");

    re15_enemy_ai_run_all(0);                    /* INIT */
    n->state = 1; n->sub_state_1 = 0; n->sub_state_2 = 0;   /* ESKORTE */

    /* --- (A) ZIEL HINTER DER WAND: sie darf NICHT hin. ---------------------------------
     * ⛔ Was hier NICHT geprueft wird und warum: "steht immer auf begehbarem Boden" waere
     * eine staerkere Zusage als das Original gibt. FUN_8003b0a4 ist ein WAND-Klemmer gegen
     * SCA-Zellen (Maske 4), kein Boden-Test — gemessen steht die NPC zwischendurch auf
     * z-Zeilen ohne Boden-Zelle, ohne durch eine Wand zu gehen. Geprueft wird deshalb, was
     * die Klemme wirklich leistet: das Ziel hinter der Wand bleibt unerreichbar und der
     * Fortschritt dorthin versiegt. */
    int32_t ox = n->x, oz = n->z;
    int moved = 0;
    int32_t maxx = n->x, x_at_180 = n->x;
    for (int f = 0; f < 240; f++) {
        re15_enemy_ai_run_all(0);
        if (n->x != ox || n->z != oz) moved = 1;
        if (n->x > maxx) maxx = n->x;
        if (f == 179) x_at_180 = maxx;
    }
    int32_t late = maxx - x_at_180;
    printf("  (A) Wand: weiteste x-Auslenkung %ld (Ziel %ld), Fortschritt in den letzten "
           "60 Bildern %ld\n", (long)maxx, (long)tx, (long)late);

    CHECK(moved,
          "die NPC bewegt sich ueberhaupt (Eskorte-Sub laeuft) - ohne Bewegung waere die "
          "Wand-Zusage trivial erfuellt");
    CHECK(maxx < tx,
          "sie kommt NICHT bis zum Ziel hinter der Wand (weiteste Auslenkung %ld < %ld) - "
          "vor dem Fix schrieb re15_npc_pos_advance x/z ungeklemmt und sie lief hindurch",
          (long)maxx, (long)tx);
    CHECK(late == 0,
          "und der Fortschritt zur Wand hin versiegt (%ld Einheiten in den letzten 60 "
          "Bildern) - die Klemme haelt, sie schiebt sich nicht langsam hinein", (long)late);

    /* --- (B) GEGENPROBE: mit einem ERREICHBAREN Ziel kommt sie an. ----------------------
     * Ohne diese Haelfte wuerde (A) auch dann gruen, wenn die NPC schlicht festhaengt. */
    /* Das Ziel muss WEIT genug weg sein, damit die Eskorte wirklich laufen MUSS: sie haelt
     * byte-true einen Abstand von 1500 zum Spieler (Folgepunkt `ori a0,zero,0x5dc`
     * @0x8011ce78) und laeuft erst ab +0x1d0 >= 1501 los (`sltiu v0,v0,0x5dd` @0x8004f118). */
    int32_t gx = sx + 4000, gz = sz0;
    if (!re15_collision_on_floor(&s_rdt, gx, gz)) { gx = sx - 4000; }
    CHECK(re15_collision_on_floor(&s_rdt, gx, gz),
          "es gibt ein ERREICHBARES Ziel fuer die Gegenprobe (%ld,%ld)", (long)gx, (long)gz);
    n->x = sx; n->z = sz0; n->state = 1; n->sub_state_1 = 0; n->sub_state_2 = 0;
    pl->x = gx; pl->z = gz;
    int32_t best = 0x7fffffff;
    for (int f = 0; f < 240; f++) {
        re15_enemy_ai_run_all(0);
        int32_t dx = n->x - gx, dz = n->z - gz;
        int32_t d = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (d < best) best = d;
    }
    printf("  (B) frei: kleinster Abstand zum erreichbaren Ziel %ld\n", (long)best);
    CHECK(best <= 1700,
          "mit einem erreichbaren Ziel kommt sie auch an (kleinster Abstand %ld; byte-true "
          "haelt die Eskorte 1500 Abstand - Folgepunkt @0x8011ce78, Losgeh-Schwelle 1501 "
          "@0x8004f118) - sonst haette die Klemme sie eingemauert und (A) waere wertlos",
          (long)best);

    free(buf);
    if (fails) { printf("\nNPC WALL CLAMP: FAIL (%d)\n", fails); return 1; }
    printf("\nNPC WALL CLAMP: die Wand haelt\n");
    return 0;
}
