/* test_1090_npc_prop_push.c — PIN: eine NPC laeuft NICHT durch eine Kiste.
 *
 * NUTZER-REPORT 2026-08-30: "Ada kann noch durch die Kiste laufen."
 * (Der Vorgaenger-Fix v0.3.29 hatte nur die WAND geklemmt — Kisten sind ein anderes System.)
 *
 * BYTE-TRUE BELEG — FUN_8002bd44 schiebt JEDEN aktiven Aktor aus den Objekten, direkt hinter
 * dem Typ-Handler-`jalr` @0x8002be04:
 *     8002be0c: lui   v0,0x800b
 *     8002be10: addiu v0,v0,-13746        ; g_active_count
 *     8002be14: lbu   s1,0(v0)            ; Zahl der Aktoren
 *     8002be20: addiu s0,v0,478           ; erster Aktor
 *     8002be24: lw    v0,0(s0)
 *     8002be2c: andi  v0,v0,0x1
 *     8002be30: beq   v0,zero,0x8002be48  ; nur aktive
 *     8002be34: addu  a0,s0,zero          ; a0 = Aktor
 *     8002be3c: addu  a1,s2,zero          ; a1 = Objekt
 *     8002be40: jal   0x8002cabc
 *     8002be44: addu  a2,zero,zero        ; a2 = 0 = den AKTOR ausschieben
 *     8002be4c: addiu s0,s0,500           ; Stride 0x1F4
 * KEIN Typ-Filter, und — anders als der Spieler-Pfad @0x8002bef0 — keine Ausnahme fuer das
 * Objekt, auf dem gestanden wird (DAT_800ac788 ist ein reiner Spieler-Zustand).
 * Die einzige Ausnahme steht im Callee: `lw v0,0(a3)` / `andi v0,v0,0x40` /
 * `bne v0,zero,0x8002cbc4` @0x8002cad0-dc. Die NPC-Familie traegt dieses Bit NICHT — ihr INIT
 * ODERt 0x40000000 (`lui v1,0x4000` / `or v0,v0,v1` / `sw v0,0(a0)` @0x8011ccd4-dc), also das
 * HIGH-Bit.
 * RADIUS: das Original summiert Objekt-Box + Aktor-Box[+6] (`lhu v0,6(t2)` @0x8002cb3c-4c).
 * Die NPC-Box @0x80121658 = {0,-1530,0,450,1530,450} hat hx = 450 — exakt das PR, mit dem der
 * Port rechnet. Der Aufruf ist fuer 0x40..0x4d also zahlengleich.
 *
 * DIE WACHE IST NICHT VAKUANT:
 *   (1) Sie SUCHT die Kiste, statt sie zu behaupten, und weist ihre Halbmassen aus. Ohne ein
 *       kollidierendes Objekt bricht sie mit FAIL ab.
 *   (2) GEGENPROBE FERN: eine NPC weit weg von der Kiste darf sich NICHT bewegen — sonst
 *       waere (1) auch mit einem Aktor-Teleport gruen.
 *   (3) GEGENPROBE SPIELER: der Spieler wird aus derselben Kiste geschoben. Damit ist belegt,
 *       dass die Box wirklich fest ist und die Wache nicht an einem Datenartefakt haengt.
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
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1090: die Kiste haelt auch eine NPC (FUN_8002bd44 @0x8002be0c-4c) ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1090;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    /* Das Skript installiert die Objekte erst im Raum-Init — deshalb erst laufen lassen. */
    for (int f = 0; f < 20; f++) frame_step();

    /* --- (1) DIE KISTE SUCHEN -------------------------------------------------------- */
    int box = -1;
    printf("  Objekte im Raum: %u\n", (unsigned)g_scd.prop_count);
    for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++) {
        printf("    obj%d aktiv=%u Halbmasse=(%d,%d,%d) Mitte=(%d,%d,%d) Pos=(%ld,%ld)\n",
               i, (unsigned)g_scd.props[i].active,
               g_scd.props[i].box_hx, g_scd.props[i].box_hy, g_scd.props[i].box_hz,
               g_scd.props[i].box_cx, g_scd.props[i].box_cy, g_scd.props[i].box_cz,
               (long)g_scd.props[i].x, (long)g_scd.props[i].z);
        if (!g_scd.props[i].active) continue;
        if (g_scd.props[i].box_hx == 0 && g_scd.props[i].box_hz == 0) continue;
        if (box < 0) box = i;
    }
    CHECK(box >= 0,
          "ROOM1090 hat ein kollidierendes Objekt (Index %d) — ohne das misst die Wache nichts",
          box);
    if (box < 0) { free(buf); printf("\n1090 NPC-PROP: FAIL (%d)\n", fails); return 1; }

    const int32_t bx = (int32_t)g_scd.props[box].box_cx + g_scd.props[box].x;
    const int32_t bz = (int32_t)g_scd.props[box].box_cz + g_scd.props[box].z;
    const int32_t hx = (int32_t)(uint16_t)g_scd.props[box].box_hx;
    const int32_t hz = (int32_t)(uint16_t)g_scd.props[box].box_hz;

    /* --- (2) DIE NPC MITTEN IN DIE KISTE --------------------------------------------- */
    re15_actor_t *n = &g_actors[1];
    memset(n, 0, sizeof *n);
    n->active = 1; n->type = 0x42; n->hp = 100;
    n->x = bx; n->y = 0; n->z = bz;
    re15_enemy_apply_hitbox(n, 0x42);
    printf("  NPC Typ 0x42 auf die Kistenmitte (%ld,%ld) gesetzt, Halbmasse (%ld,%ld)\n",
           (long)bx, (long)bz, (long)hx, (long)hz);

    re15_actor_prop_pushout();

    int32_t dx = n->x - bx, dz = n->z - bz;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    printf("  nach EINEM Objekt-Tick: (%ld,%ld) — Versatz (%ld,%ld)\n",
           (long)n->x, (long)n->z, (long)dx, (long)dz);
    CHECK(dx > hx || dz > hz,
          "die NPC wird aus der Kiste HERAUSGESCHOBEN (Versatz %ld/%ld gegen Halbmasse "
          "%ld/%ld) — FUN_8002bd44 ruft `jal 0x8002cabc` @0x8002be40 mit a2 = 0 fuer JEDEN "
          "aktiven Aktor; der Port hatte nur den Spieler-Rumpf ab @0x8002bf14",
          (long)dx, (long)dz, (long)hx, (long)hz);

    /* --- (3) GEGENPROBE FERN: wer nicht in der Kiste steckt, wird nicht bewegt -------- */
    {
        const int32_t fx = bx + 20000, fz = bz + 20000;
        n->x = fx; n->z = fz;
        re15_actor_prop_pushout();
        printf("  fern: (%ld,%ld) -> (%ld,%ld)\n", (long)fx, (long)fz, (long)n->x, (long)n->z);
        CHECK(n->x == fx && n->z == fz,
              "eine NPC 20000 Einheiten neben der Kiste bleibt UNBERUEHRT — der Push-Out hat "
              "eine AABB-Vorpruefung (`adx > sumX` @0x8002cb58 / `adz > sumZ` @0x8002cb6c), "
              "er teleportiert nicht");
    }

    /* --- (4) GEGENPROBE SPIELER: dieselbe Kiste haelt auch ihn ----------------------- */
    {
        int32_t px = bx, pz = bz;
        re15_collision_objects_actor(&px, &pz);
        int32_t pdx = px - bx, pdz = pz - bz;
        if (pdx < 0) pdx = -pdx;
        if (pdz < 0) pdz = -pdz;
        printf("  Kontrolle Box: Mitte -> (%ld,%ld), Versatz (%ld,%ld)\n",
               (long)px, (long)pz, (long)pdx, (long)pdz);
        CHECK(pdx > hx || pdz > hz,
              "die Box ist wirklich fest (Versatz %ld/%ld) — die Wache haengt nicht an einem "
              "Datenartefakt", (long)pdx, (long)pdz);
    }

    free(buf);
    if (fails) { printf("\n1090 NPC-PROP: FAIL (%d)\n", fails); return 1; }
    printf("\n1090 NPC-PROP: die Kiste haelt die NPC\n");
    return 0;
}
