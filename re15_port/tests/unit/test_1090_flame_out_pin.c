/* test_1090_flame_out_pin.c — PIN: nach dem Loeschen mit dem Feuerloescher ist der
 * Flammen-Effekt in ROOM1090 WEG — Emitter UND Partikel.
 *
 * ⛔ NUTZER-BEFUND (Uebergabe 2026-08-25, Punkt 1.2): "nachdem man mit dem [Wort fehlte im
 * Report] die flammen ausgemacht hat, muss der flammen effekt weg sein."
 *
 * Das fehlende Wort ist der FEUERLOESCHER, und das steht im Raum selbst: ROOM1090.RDT
 * MSG-Sektion (tbl[0x3C] = 0x2748) msg07 "I must hurry up and get something to put out this
 * fire to save that woman!", msg08 "Will you use the Fire Extinguisher?" (@0x2934, mit der
 * Yes/No-Terminierung 1b 03 02 01), msg09 "You've used the Fire Extinguisher." (@0x2961).
 *
 * DER MECHANISMUS ist eine SELBST-TUER, nicht ein Abschalt-Flag:
 *   ROOM1090 sub06 setzt flag(3,0x81) (@0x271E) und feuert `Aot_on 03` (@0x2726).
 *   Slot 3 traegt zu dem Zeitpunkt einen Door_aot_set (@0x2352, 32 B), dessen Ziel DERSELBE
 *   Raum ist: Payload STAGE 0x00, ROOM 0x09, CUT 0x06, Spawn (1252,-1800,-2529).
 *   `Aot_on` ist SOFORT-Feuern (LAB_800407bc: @0x800407ec Record holen, @0x8004082c
 *   `jalr v0` = sce-Handler ohne Geometrie-Test), der sce-2-Handler @0x800430bc bewaffnet
 *   die Transition, und der Warp laedt den Raum neu.
 *   Der Original-Warp kennt KEINEN "gleicher Raum"-Sonderfall (selbst nachdisassembliert,
 *   FUN_8001d600): @0x8001d94c Ziel-ROOM, @0x8001d930 Ziel-CUT, @0x8001d960 Ziel-STAGE,
 *   @0x8001d968 `beq v1,v0` vergleicht NUR die Stage, @0x8001d988 `jal 0x800396fc` laeuft
 *   unbedingt. Beim Neu-Laden greift dann `Ck(3,0x81)` und die brennenden Truemmer
 *   (Typ 0x26 — das sind KEINE Spinnen, siehe Memory reai-v2-spider-ai) spawnen nicht mehr.
 *
 * DER PORT-DEFEKT war NICHT die Kette — die lief. Gemessen verschwanden die sieben Emitter
 * und ihre Schadenszone korrekt (7 -> 0). Stehen blieben die ESP-PARTIKEL (Effekt-Id 0x10 x4
 * und 0x08 x3), weil der Selbst-Tuer-Wiedereintritt den 96-Slot-Effekt-Pool nicht wischte.
 * Im Original wischt ihn JEDES Raumladen:
 *     80019354: ori   a0,zero,0x60      ; 96 Slots
 *     80019364: addiu v0,v0,-132        ; Stride 132
 *     80019370: addiu at,at,29732       ; Basis 0x800A7424
 *     80019378: sb    zero,0(at)        ; Slot-Flags := 0
 * und eigener Voll-Scan der PSX.EXE nach `jal 0x80019354` findet GENAU EINEN Aufrufer:
 * 0x8003996C, im Raumlader FUN_800396fc. Der Fix sitzt deshalb in scd_room_reenter.
 *
 * DIE WACHE IST NICHT VAKUANT: sie verlangt, dass VORHER wirklich etwas brennt (Emitter UND
 * Partikel > 0) — sonst misst sie nichts — und dass NACHHER beides null ist.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_esp.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

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

static int count_emitters(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x26) n++;
    return n;
}

static int count_particles(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->active) n++;
    }
    return n;
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

    printf("=== ROOM1090: Flammen nach dem Feuerloescher (Selbst-Tuer @0x2352) ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1090;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
    re15_collision_set_band(0);

    /* Vorbedingung: der Feuerloescher ist AUFGENOMMEN. Nur im ELSE-Zweig von Ck(3,0x85,0)
     * @0x230A installiert sub00 die sce-3-Flaeche @0x2336 (-> sub06) UND die Selbst-Tuer
     * `Door_aot_set 03` @0x2352, die sub06 per `Aot_on 03` @0x2726 ausloest. */
    re15_game_flag_set(3, 0x85, 1);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 120; f++) frame_step();

    int em0 = count_emitters(), fx0 = count_particles();
    printf("  vor dem Loeschen: %d Emitter (Typ 0x26), %d Effekt-Partikel\n", em0, fx0);
    CHECK(em0 > 0, "es brennt ueberhaupt (%d Emitter) — sonst misst die Wache nichts", em0);
    CHECK(fx0 > 0, "es brennen SICHTBAR Partikel (%d) — genau die blieben vorher stehen "
                   "(gemessen: Effekt-Id 0x10 x4 und 0x08 x3)", fx0);

    /* Das Loesch-Event feuern — der Weg, den der Feuerloescher im Spiel nimmt. */
    scd_event_fire(6);
    for (int f = 0; f < 400; f++) frame_step();

    int em1 = count_emitters(), fx1 = count_particles();
    printf("  nach dem Loeschen: %d Emitter, %d Effekt-Partikel, flag(3,0x81) = %d\n",
           em1, fx1, re15_game_flag_get(3, 0x81));
    CHECK(re15_game_flag_get(3, 0x81) == 1,
          "sub06 setzt flag(3,0x81) (@0x271E) — sonst ist das Loesch-Event gar nicht gelaufen");
    CHECK(em1 == 0,
          "die brennenden Truemmer sind weg (%d Emitter) — der Selbst-Tuer-Wiedereintritt "
          "raeumt den Aktoren-Pool und `Ck(3,0x81)` haelt sie beim Neu-Aufbau draussen", em1);
    CHECK(fx1 == 0,
          "und die FLAMMEN-PARTIKEL sind weg (%d) — das ist der eigentliche Nutzer-Befund. "
          "Im Original wischt jedes Raumladen den 96-Slot-Effekt-Pool (FUN_80019354, einziger "
          "Aufrufer 0x8003996C im Raumlader FUN_800396fc)", fx1);

    free(buf);
    if (fails) { printf("\n1090 FLAME OUT: FAIL (%d)\n", fails); return 1; }
    printf("\n1090 FLAME OUT: Feuer und Flammen-Effekt verschwinden zusammen\n");
    return 0;
}
