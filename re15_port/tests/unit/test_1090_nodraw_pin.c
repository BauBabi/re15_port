/* test_1090_nodraw_pin.c — PIN: die ROOM1090-Feuer-Emitter werden NICHT gezeichnet.
 *
 * NUTZER-REPORT (zweimal: 2026-08-22 "statt dessen schwirren komische Dreiecke rum" und
 * 2026-08-30 "die schwarzen Dreiecke sind noch immer ueber dem Feuer").
 *
 * BYTE-TRUE URSACHE: das Original entscheidet PRO KOERPERTEIL ueber Bit 0 der Part-Flags
 * (Part-Array entity+0x188, Stride 0xAC).
 *     Binder setzt es:   8001e74c: ori v0,zero,0x1 / 8001e758: sw v0,0(s2)
 *     Zeichner prueft:   8001ecc4: andi v0,v1,0x1
 *                        8001ecc8: beq  v0,zero,0x8001ee48   -> Epilog, KEINE Mesh-Ausgabe
 *     Typ 0x26 loescht:  801165d0: lw    a0,392(v0)   ; entity+0x188
 *                        801165d8: lw    v0,0(a0)
 *                        801165dc: addiu v1,zero,-2
 *                        801165e0: and   v0,v0,v1
 *                        801165e4: sw    v0,0(a0)
 * Die sieben Typ-0x26-Aktoren von ROOM1090 sind also UNSICHTBARE Traeger fuer ihre
 * Flammen-Effekte. Ihr Modell ist ein einziges Dreieck (EM26: 1 Bone, 3 Verts, 1 Face) —
 * genau das hat der Port gezeichnet.
 *
 * DIE WACHE IST NICHT VAKUANT: sie prueft zuerst, dass die sieben Emitter ueberhaupt
 * existieren und ihren INIT durchlaufen haben, und verlangt dann die Maske. Zusaetzlich die
 * Gegenprobe, dass ein normaler Gegner die Maske NICHT bekommt — sonst waere jede
 * Zeichen-Zusage im Spiel mit einem Schlag kaputt.
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

    printf("=== ROOM1090: die Feuer-Emitter werden nicht gezeichnet ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1090;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    for (int f = 0; f < 20; f++) frame_step();

    int n26 = 0, masked = 0, active_states = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x26) continue;
        n26++;
        if (e->no_draw) masked++;
        if (e->state != 0) active_states++;
    }
    printf("  %d Emitter vom Typ 0x26, %d davon durch den INIT gelaufen, %d mit Zeichenmaske\n",
           n26, active_states, masked);

    CHECK(n26 == 7,
          "ROOM1090 stellt die sieben Feuer-Emitter (%d) — Sce_em_set-Records @0x2214/0x2228/"
          "0x223C/0x2250/0x2264/0x2278/0x228C; ohne sie misst die Wache nichts", n26);
    CHECK(active_states == n26,
          "alle haben ihren INIT durchlaufen (%d von %d) — die Maske wird dort gesetzt "
          "(@0x801165d0-e4)", active_states, n26);
    CHECK(masked == n26,
          "und ALLE tragen die Zeichenmaske (%d von %d): das Original loescht Bit 0 der "
          "Part-Flags im INIT, der Zeichner steigt daraufhin vor der Mesh-Ausgabe aus "
          "(`andi v0,v1,0x1` @0x8001ecc4 / `beq v0,zero,0x8001ee48` @0x8001ecc8)",
          masked, n26);

    /* GEGENPROBE: ein normaler Gegner darf die Maske NICHT bekommen — sonst waere mit
     * derselben Zeile das halbe Spiel unsichtbar. */
    {
        re15_actor_t *z = &g_actors[RE15_ACTOR_MAX - 1];
        memset(z, 0, sizeof *z);
        z->active = 1; z->type = 0x10; z->state = 0; z->hp = 100;
        for (int f = 0; f < 4; f++) re15_enemy_ai_run_all(0);
        CHECK(!z->no_draw,
              "ein normaler Zombie (0x10) traegt die Maske NICHT — der Binder setzt Bit 0 "
              "unbedingt (`ori v0,zero,0x1` @0x8001e74c), nur der 0x26-INIT loescht es");
        z->active = 0;
    }

    free(buf);
    if (fails) { printf("\n1090 NODRAW: FAIL (%d)\n", fails); return 1; }
    printf("\n1090 NODRAW: die Emitter tragen ihre Flammen, ohne selbst sichtbar zu sein\n");
    return 0;
}
